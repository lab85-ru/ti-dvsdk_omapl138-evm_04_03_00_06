/** ============================================================================
 *  @file   dm648_hal_pci_dma.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM648/pci/
 *
 *  @desc   DMA control module.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation version 2.
 *  
 *  This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 *  whether express or implied; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *  ============================================================================
 */

#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include <asm/page.h>
#include <asm/io.h>

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- OSAL Headers                */
#include <sync.h>
#include <mem.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm648_hal.h>
#include <dm648_hal_pci.h>
#include <dm648_hal_pci_dma.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_HAL_DMA


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_ARCH_HAL_DMA, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/* ============================================================================
 *  @func   DM648_halPciReadDMA
 *
 *  @desc   DMAs contents from DSP memory to GPP Memory. Here read means DSP
 *          write.
 *          Porting to use HOST DMA requires:
 *          Use the srcAddr and dstAddr argument to program the src and dst of
 *          the DMA engine, Specific the size and then start the DMA, Here
 *          DMA must be called in spinlock context, so that transfer is seemed
 *          as atomic.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM648_halPciReadDMA (IN Pvoid           halObj,
                     IN Uint32          srcAddr,
                     IN Uint32          dstAddr,
                     IN Uint32          size)
{
    DSP_STATUS        status    = DSP_SOK ;
    DM648_pciRegs  *  pciRegs   = NULL    ;
    DM648_HalObj  *   halObject = (DM648_HalObj *) halObj ;
    Uint32            oldBar0TrlVal ;
    Uint32            irqFlags      ;
#if defined(LINUXPC_DSP_DMA)
    DM648_edmaRegs  * edmaRegs  = NULL  ;
    Uint32            readSize    = 0   ;
    Uint32            i           = 0   ;
    Uint32            chnlId      = 0   ;
    Uint32            pageBase      ;
    Uint32            numUSec       ;
    Uint32            tmp           ;
    Uint32            count         ;
    Uint32            tSize         ;
#else
    Uint32            rBytes      = 0   ;
    Uint32            availBytes  = 0   ;
    Char8 *           dspPtr      = NULL;
    Char8 *           curHostPtr        ;
    Uint32            cBytes            ;
#endif

    TRC_4ENTER ("DM648_halPciReadDMA", halObj, srcAddr, dstAddr, size) ;

    DBC_Require (halObject != NULL) ;
    DBC_Require (size > 0) ;

    if (halObject == NULL) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
         pciRegs  = (DM648_pciRegs *) (  halObject->pci.barVirt [CFG_REGS_BAR_NO]
                                       + DM648_PCIREG_BASE - DM648_BAR2_BASE) ;
#if defined(LINUXPC_DSP_DMA)
        if (halObject->pci.edmaChnlId == 0xFFFFFFFFu) {
            PRINT_Printf (
                    "EDMA Channel ID not provided, please provide a valid"
                    "EDMA Channel for DSPLink in the CFG file."
                    "This EDMA Channel must be used by DSPLink "
                    "exculsively\n") ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else {
            chnlId = halObject->pci.edmaChnlId ;
        }

        if (DSP_SUCCEEDED (status)) {
            pciRegs  = (DM648_pciRegs *)
                                 (  halObject->pci.barVirt [CFG_REGS_BAR_NO]
                                  + DM648_PCIREG_BASE - DM648_BAR2_BASE) ;

            irqFlags = SYNC_SpinLockStartEx (halObject->lock) ;

            /* Save the old translation register value */
            oldBar0TrlVal = pciRegs->PCIBAR0TRL ;
            /* program the new address */
            pciRegs->PCIBAR0TRL = DM648_EDMAREG_BASE & pciRegs->PCIBAR0MSK ;

            edmaRegs = (DM648_edmaRegs *)
                              (  halObject->pci.barVirt [SCRATCH_BAR_NUMBER]
                               + DM648_EDMAREG_BASE
                               - pciRegs->PCIBAR0TRL) ;

            count = (  (size + (dstAddr & ~DM648_PCIADWRBITMASK))
                    / (DM648_PCIADLEN)) + 1 ;
            for (i = 0 ; i < count ; i++) {
                pageBase = (  (dstAddr + (DM648_PCIADLEN * i))
                            & DM648_PCIADWRBITMASK) ;
                pciRegs->PCIADDSUB [i] = pageBase ;
            }

            do {
                /* Set the interrupt enable for the EDMA Channel
                 * identified by chnlId.
                 */
                if (chnlId < 32u) {
                    edmaRegs->IESR |= 0x1u << chnlId ;
                }
                else {
                    edmaRegs->IESRH |= 0x1u << (chnlId - 32u) ;
                }

                /* Clear any pending interrupt. */
                if (chnlId < 32u) {
                    edmaRegs->ICR |= 0x1u << chnlId ;
                }
                else {
                    edmaRegs->ICRH |= 0x1u << (chnlId - 32u) ;
                }

                /* Populate the Param entry. */
                /* Set the  TCC field in the oprion paramter such
                 * that corrsponding
                 * bit in the IPR will be set after transfer completion.
                 */
                edmaRegs->PARAMENTRY [chnlId].OPTION  =
                                          ( 0x00100004u | (chnlId << 12)) ;
                edmaRegs->PARAMENTRY [chnlId].SRC          = srcAddr ;
                /* Calculate the DSP PCI address for the PC address */
                tmp = DM648_PCIMEM_BASE + (dstAddr & ~DM648_PCIADWRBITMASK);
                edmaRegs->PARAMENTRY [chnlId].DST          = tmp ;

                /* Calculate the A & B count */
                if (size > 0x7000)  {
                    tmp   = size / 0x7000 ;
                    size -= (tmp * 0x7000) ;
                    tmp <<= 16 ;
                    tmp  |= 0x7000 ;
                    tSize = 0x7000 ;
                }
                else {
                    tmp = 0x10000 | size ;
                    tSize = size ;
                    size = 0 ;
                }

                edmaRegs->PARAMENTRY [chnlId].A_B_CNT      = tmp ;
                edmaRegs->PARAMENTRY [chnlId].LINK_BCNTRLD = 0xFFFF ;
                edmaRegs->PARAMENTRY [chnlId].SRC_DST_CIDX = 0 ;
                /* C Count is set to 1 since mostly size will not
                 * be more than 1GB
                 */
                edmaRegs->PARAMENTRY [chnlId].CCNT         = 0x1 ;
                /* no offset difference required */
                edmaRegs->PARAMENTRY [chnlId].SRC_DST_BIDX = 0x70007000 ;

                /* Clear any pending interrupt. */
                if (chnlId < 32u) {
                    edmaRegs->ESR  |= 0x1u << chnlId ;
                }
                else {
                    edmaRegs->ESRH |= 0x1u << (chnlId - 32u) ;
                }

                /* wait for current DMA to finish. */
                numUSec = tSize / PCI33_DMA_MAXTHROUGHPUT ;
                if (chnlId < 32u) {
                    do {
                        SYNC_UDelay (numUSec) ;
                        /* now check in steps of 10 usec. */
                        numUSec = 10 ;
                    } while ((edmaRegs->IPR & (0x1u << chnlId)) == 0) ;
                }
                else {
                    do {
                        SYNC_UDelay (numUSec) ;
                        /* now check in steps of 10 usec. */
                        numUSec = 10 ;
                    } while (( edmaRegs->IPRH &
                               (0x1u << (chnlId - 32u))) == 0) ;
                }

                if (size != 0u) {
                    srcAddr += tSize ;
                    dstAddr += tSize ;
                }
            } while (size != 0) ;

            /* restore the old value */
            pciRegs->PCIBAR0TRL = oldBar0TrlVal ;

            SYNC_SpinLockEndEx (halObject->lock, irqFlags) ;
             if (readSize != size) {
                status = DSP_EFAIL ;
                SET_FAILURE_REASON ;
            }

        }
#else
        /* Do memcpy instead of DMA*/
         /* Default memcopy */
        curHostPtr        = __va(dstAddr)    ;
        irqFlags = SYNC_SpinLockStartEx (halObject->lock) ;
        /* Save the old translation register value */
        oldBar0TrlVal = pciRegs->PCIBAR0TRL ;
        cBytes = size ;
        /* if the address is within the already mapped region (within 32MB) */
        while (cBytes > 0) {
            if (   ((srcAddr + rBytes) >= pciRegs->PCIBAR0TRL)
                && (  (srcAddr + rBytes)
                    <=
                      ( pciRegs->PCIBAR0TRL +
                        halObject->pci.barLen [SCRATCH_BAR_NUMBER]))) {
                dspPtr = (Char8 *)
                            (  halObject->pci.barVirt [SCRATCH_BAR_NUMBER]
                             + (  (srcAddr + rBytes)
                                -  pciRegs->PCIBAR0TRL)) ;
                availBytes = (  (halObject->pci.barLen [SCRATCH_BAR_NUMBER])
                              - (((Uint32) srcAddr + rBytes) -
                                  pciRegs->PCIBAR0TRL)) ;
                if (availBytes >= cBytes) {
                    MEM_Copy ((unsigned char *) curHostPtr,
                              (unsigned char *) dspPtr,
                              cBytes,
                              Endianism_Default) ;
                    cBytes = 0 ;
                }
                else {
                    rBytes += availBytes ;
                    MEM_Copy ((unsigned char *)curHostPtr,
                              (unsigned char *) dspPtr,
                              availBytes,
                              Endianism_Default) ;
                    cBytes -= availBytes ;
                    /* Increment the  current  host pointer */
                    curHostPtr += availBytes ;
                }
            }
            else {
                pciRegs->PCIBAR0TRL = (((Uint32) srcAddr + rBytes) &
                                       (0xFE000000)) ;
            }
        }
        /* restore the old value */
         pciRegs->PCIBAR0TRL = oldBar0TrlVal ;

        SYNC_SpinLockEndEx (halObject->lock,irqFlags) ;
#endif
    }

    TRC_1LEAVE ("DM648_halPciReadDMA", status) ;

    return status ;
}


/* ============================================================================
 *  @func   DM648_halPciWriteDMA
 *
 *  @desc   DMAs contents from GPP memory to DSP Memory. Here write means DSP
 *          read.
 *          Porting to use HOST DMA requires:
 *          Use the srcAddr and dstAddr argument to program the src and dst of
 *          the DMA engine, Specific the size and then start the DMA, Here
 *          DMA must be called in spinlock context, so that transfer is seemed
 *          as atomic.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM648_halPciWriteDMA (IN Pvoid           halObj,
                      IN Uint32          srcAddr,
                      IN Uint32          dstAddr,
                      IN Uint32          size)
{
    DSP_STATUS           status    = DSP_SOK ;
    DM648_pciRegs *      pciRegs   = NULL    ;
    DM648_HalObj *       halObject = (DM648_HalObj *) halObj ;
    Uint32               irqFlags            ;
    Uint32               oldBar0TrlVal       ;

#if defined(LINUXPC_DSP_DMA)
    DM648_edmaRegs *     edmaRegs  = NULL    ;
    Uint32               chnlId    = 0       ;
    Uint32               writeSize = 0       ;
    Uint32               pageBase            ;
    Uint32               numUSec             ;
    Uint32               tmp                 ;
    Uint32               i                   ;
    Uint32               count               ;
    Uint32               tSize               ;
#else

    Uint32               wBytes    = 0       ;
    Char8 *              dspPtr     = NULL   ;
    Uint32               availBytes = 0      ;
    Char8 *              curHostPtr          ;
    Uint32               cBytes              ;
#endif
    TRC_4ENTER ("DM648_halPciWriteDMA", halObj, srcAddr, dstAddr, size) ;

    DBC_Require (halObject != NULL) ;
    DBC_Require (size > 0) ;

    if (halObject == NULL) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
         pciRegs  = (DM648_pciRegs *)
                                    (  halObject->pci.barVirt [CFG_REGS_BAR_NO]
                                     + DM648_PCIREG_BASE - DM648_BAR2_BASE) ;

#if defined(LINUXPC_DSP_DMA)
        if (halObject->pci.edmaChnlId == 0xFFFFFFFFu) {
            PRINT_Printf (
                    "EDMA Channel ID not provided, please provide a valid"
                    "EDMA Channel for DSPLink in the CFG file."
                    "This EDMA Channel must be used by DSPLink "
                    "exculsively\n") ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else {
            chnlId = halObject->pci.edmaChnlId ;
        }

        if (DSP_SUCCEEDED (status)) {
            irqFlags = SYNC_SpinLockStartEx (halObject->lock) ;

            /* Save the old translation register value */
            oldBar0TrlVal = pciRegs->PCIBAR0TRL ;

            /* program the new address */
            pciRegs->PCIBAR0TRL = DM648_EDMAREG_BASE & pciRegs->PCIBAR0MSK ;
            edmaRegs = (DM648_edmaRegs *)
                              (  halObject->pci.barVirt [SCRATCH_BAR_NUMBER]
                               + DM648_EDMAREG_BASE
                               - pciRegs->PCIBAR0TRL) ;

            count = (  (size + (srcAddr & ~DM648_PCIADWRBITMASK))
                     / (DM648_PCIADLEN)) + 1 ;
            for (i = 0 ; i < count ; i++) {
                pageBase = (  (srcAddr + (DM648_PCIADLEN * i))
                            & DM648_PCIADWRBITMASK) ;
                pciRegs->PCIADDSUB [i] = pageBase ;
            }

            do {
                /* Set the interrupt enable for the EDMA Channel
                 * identified by chnlId.
                 */
                if (chnlId < 32u) {
                    edmaRegs->IESR |= 0x1u << chnlId ;
                }
                else {
                    edmaRegs->IESRH |= 0x1u << (chnlId - 32u) ;
                }

                /* Clear any pending interrupt. */
                if (chnlId < 32u) {
                    edmaRegs->ICR |= 0x1u << chnlId ;
                }
                else {
                    edmaRegs->ICRH |= 0x1u << (chnlId - 32u) ;
                }

                /* Populate the Param entry. */
                /* Set the  TCC field in the oprion paramter such that
                 * corrsponding bit in the IPR will be set after transfer
                 * completion.
                 */
                edmaRegs->PARAMENTRY [chnlId].OPTION  =
                                           ( 0x00100004u | (chnlId << 12)) ;

                /* Calculate the DSP PCI address for the PC address */
                tmp = DM648_PCIMEM_BASE + (srcAddr & ~DM648_PCIADWRBITMASK);
                edmaRegs->PARAMENTRY [chnlId].SRC          = tmp ;
                edmaRegs->PARAMENTRY [chnlId].DST          = dstAddr ;

                /* Calculate the A & B count */
                if (size > 0x7000)  {
                    tmp   = size / 0x7000 ;
                    size -= (tmp * 0x7000) ;
                    tmp <<= 16 ;
                    tmp  |= 0x7000 ;
                    tSize = 0x7000 ;
                }
                else {
                    tmp = 0x10000 | size ;
                    tSize = size ;
                    size = 0 ;

                }

                edmaRegs->PARAMENTRY [chnlId].A_B_CNT      = tmp ;
                edmaRegs->PARAMENTRY [chnlId].LINK_BCNTRLD = 0xFFFF ;
                edmaRegs->PARAMENTRY [chnlId].SRC_DST_CIDX = 0 ;
                /* no offset difference required */
                edmaRegs->PARAMENTRY [chnlId].SRC_DST_BIDX = 0x70007000 ;
                /* C Count is set to 1 since mostly size will not be more
                 * than 1GB
                 */
                edmaRegs->PARAMENTRY [chnlId].CCNT         = 0x1 ;

                /* Clear any pending interrupt. */
                if (chnlId < 32u) {
                    edmaRegs->ESR  |= 0x1u << chnlId ;
                }
                else {
                    edmaRegs->ESRH |= 0x1u << (chnlId - 32u) ;
                }

                /* wait for current DMA to finish. */
                numUSec = tSize / PCI33_DMA_MAXTHROUGHPUT ;
                if (chnlId < 32u) {
                    do {
                        SYNC_UDelay (numUSec) ;
                        /* now check in steps of 10 usec. */
                        numUSec = 10 ;
                    } while ((edmaRegs->IPR & (0x1u << chnlId)) == 0) ;
                }
                else {
                    do {
                        SYNC_UDelay (numUSec) ;
                        /* now check in steps of 10 usec. */
                        numUSec = 10 ;
                    } while (( edmaRegs->IPRH &
                               (0x1u << (chnlId - 32u))) == 0) ;
                }

                if (size != 0u) {
                    srcAddr += tSize ;
                    dstAddr += tSize ;
                }

            } while (size != 0) ;

            /* restore the old value */
            pciRegs->PCIBAR0TRL = oldBar0TrlVal ;

            SYNC_SpinLockEndEx (halObject->lock, irqFlags) ;
            if (writeSize != size) {
                status = DSP_EFAIL ;
            }
        }
#else
        curHostPtr       = __va(srcAddr)    ;
        irqFlags = SYNC_SpinLockStartEx (halObject->lock) ;
        cBytes = size ;
        /* Save the old translation register value */
        oldBar0TrlVal = pciRegs->PCIBAR0TRL ;
         /* if the address is within the already mapped region (within 32MB) */
        while (cBytes > 0) {
            if (   ((dstAddr + wBytes) >= pciRegs->PCIBAR0TRL)
                && (   (dstAddr + wBytes)
                    <= (  pciRegs->PCIBAR0TRL
                        + halObject->pci.barLen [SCRATCH_BAR_NUMBER]))) {
                dspPtr = (Char8 *)
                           (  halObject->pci.barVirt [SCRATCH_BAR_NUMBER]
                            + ((dstAddr + wBytes) - pciRegs->PCIBAR0TRL)) ;
                availBytes = (  (halObject->pci.barLen [SCRATCH_BAR_NUMBER])
                              - (((Uint32) dstAddr + wBytes) -
                                   pciRegs->PCIBAR0TRL)) ;
                if (availBytes >= cBytes) {
                    MEM_Copy ((unsigned char *)dspPtr,
                              (unsigned char *)curHostPtr,
                              cBytes,
                              Endianism_Default) ;
                    cBytes = 0 ;
                }
                else {
                    wBytes += availBytes ;
                    MEM_Copy ((unsigned char *) dspPtr,
                              (unsigned char *) curHostPtr,
                              availBytes,
                              Endianism_Default) ;
                    cBytes -= availBytes ;
                    curHostPtr += availBytes ;
                }
            }
            else {
                pciRegs->PCIBAR0TRL = (((Uint32) dstAddr + wBytes) &
                                        (0xFE000000)) ;
            }
        }
         /* restore the old value */
         pciRegs->PCIBAR0TRL = oldBar0TrlVal ;
        SYNC_SpinLockEndEx (halObject->lock, irqFlags) ;
#endif
    }

    TRC_1LEAVE ("DM648_halPciWriteDMA", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
