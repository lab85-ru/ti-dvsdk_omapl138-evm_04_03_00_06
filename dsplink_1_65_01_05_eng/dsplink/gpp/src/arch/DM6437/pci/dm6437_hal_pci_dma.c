/** ============================================================================
 *  @file   dm6437_hal_pci_dma.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM6437/pci/
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
#include <dm6437_hal.h>
#include <dm6437_hal_pci.h>
#include <dm6437_hal_pci_dma.h>


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
 *  @func   DM6437_halPciReadDMA
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
DM6437_halPciReadDMA (IN Pvoid           halObj,
                      IN Uint32          srcAddr,
                      IN Uint32          dstAddr,
                      IN Uint32          size)
{
    DSP_STATUS        status    = DSP_SOK ;

    DM6437_pciRegs *  pciRegs   = NULL    ;
    DM6437_HalObj *   halObject = (DM6437_HalObj *) halObj ;
    Uint32            irqFlags         ;
    Uint32            pageBase         ;

#if defined(LINUXPC_DSP_DMA)
    DM6437_edmaRegs * edmaRegs  = NULL ;
    Uint32            readSize  = 0    ;
    Uint32            i         = 0    ;
    Uint32            chnlId    = 0    ;
    Uint32            numUSec          ;
    Uint32            tmp              ;
    Uint32            count            ;
    Uint32            tSize            ;
#else
    Uint32            rBytes      = 0    ;
    Uint32            availBytes  = 0    ;
    Char8 *           dspPtr      = NULL ;
    Char8 *           curHostPtr         ;
#endif


    TRC_4ENTER ("DM6437_halPciReadDMA", halObj, srcAddr, dstAddr, size) ;

    DBC_Require (halObject != NULL) ;
    DBC_Require (size > 0) ;

    if (halObject == NULL) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        pciRegs  = (DM6437_pciRegs *) ( halObject->pci.barVirt [CFG_REGS_BAR_NO]
                                       + DM6437_PCIREG_BASE) ;

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
            edmaRegs = (DM6437_edmaRegs *)(
                                    halObject->pci.barVirt [CFG_REGS_BAR_NO]
                                    + DM6437_EDMAREG_BASE) ;

            count = (  (size + (dstAddr & ~DM6437_PCIADWRBITMASK))
                     / (DM6437_PCIADLEN)) + 1 ;

            irqFlags = SYNC_SpinLockStartEx (halObject->lock) ;

            for (i = 0 ; i < count ; i++) {
                pageBase = (  (dstAddr + (DM6437_PCIADLEN * i))
                            & DM6437_PCIADWRBITMASK) ;
                pciRegs->PCIADDSUB [i] = pageBase ;
            }

            do {
                /* Set the interrupt enable for 1st Channel. */
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
                 * corrsponding  bit in the IPR will be set
                 * after transfer completion.
                 */
                edmaRegs->PARAMENTRY [chnlId].OPTION       =
                                         ( 0x00100004u | (chnlId << 12)  );
                edmaRegs->PARAMENTRY [chnlId].SRC          = srcAddr ;
                /* Calculate the DSP PCI address for the PC address */
                tmp = DM6437_PCIMEM_BASE +
                      (dstAddr & ~DM6437_PCIADWRBITMASK);
                edmaRegs->PARAMENTRY [chnlId].DST          = tmp ;

                /* Calculate the A & B count */
                if (size > 0x7000u)  {
                    tmp   = size / 0x7000u ;
                    tSize = (tmp * 0x7000u) ;
                    size -= (tmp * 0x7000u) ;
                    tmp <<= 16u ;
                    tmp  |= 0x7000u ;
                }
                else {
                    tmp = 0x10000u | size ;
                    tSize = size ;
                    size = 0u ;
                }

                edmaRegs->PARAMENTRY [chnlId].A_B_CNT      = tmp ;
                edmaRegs->PARAMENTRY [chnlId].LINK_BCNTRLD = 0xFFFFu ;
                edmaRegs->PARAMENTRY [chnlId].SRC_DST_CIDX = 0u ;
                /* no offset difference required */
                edmaRegs->PARAMENTRY [chnlId].SRC_DST_BIDX = 0x70007000u ;
                /* C Count is set to 1 since mostly size will not
                 * be more than 1GB
                 */
                edmaRegs->PARAMENTRY [chnlId].CCNT         = 0x1u ;

                /* Set the interrupt enable for 1st Channel. */
                if (chnlId < 32u) {
                    edmaRegs->EESR  |= 0x1u << chnlId ;
                }
                else {
                    edmaRegs->EESRH |= 0x1u << (chnlId - 32u) ;
                }

                /* Clear any pending interrupt. */
                if (chnlId < 32u) {
                    edmaRegs->ESR  |= 0x1u << chnlId ;
                }
                else {
                    edmaRegs->ESRH |= 0x1u << (chnlId - 32u) ;
                }

                /* wait for current DMA to finish. */
                if (chnlId < 32u) {
                    numUSec = tSize / PCI33_DMA_MAXTHROUGHPUT ;
                    do {
                        SYNC_UDelay (numUSec) ;
                        /* now check in steps of 10 usec. */
                        numUSec = 10 ;
                    } while ((edmaRegs->IPR & (0x1u << chnlId)) == 0) ;
                }
                else {
                    numUSec = tSize / PCI33_DMA_MAXTHROUGHPUT ;
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

            } while (size != 0u) ;

            SYNC_SpinLockEndEx (halObject->lock,irqFlags) ;

            if (readSize != size) {
               status = DSP_EFAIL ;
            }

        }

 #else
        /* Default memcopy */
        curHostPtr        =  (Char8 *) __va(dstAddr)    ;
        irqFlags = SYNC_SpinLockStartEx (halObject->lock) ;
        pageBase = pciRegs->PCIBAR0TRL ;
        /* if the address is within the already mapped region (within 32MB) */
        while (size > 0) {
            if (   ((srcAddr + rBytes) >= pciRegs->PCIBAR0TRL)
                && ((srcAddr + rBytes) <=
                     pciRegs->PCIBAR0TRL + 0x2000000u)) {
                dspPtr = (Char8 *)(  halObject->pci.barVirt [0u]
                                   + ((srcAddr + rBytes) -
                                       pciRegs->PCIBAR0TRL)) ;
                availBytes = (  (halObject->pci.barLen [0u])
                              - (((Uint32) srcAddr + rBytes) -
                                  pciRegs->PCIBAR0TRL)) ;
                if (availBytes >= size) {
                    MEM_Copy ((unsigned char *) curHostPtr,
                              (unsigned char *) dspPtr,
                              size,
                              Endianism_Default) ;
                    size = 0 ;
                }
                else {
                    rBytes += availBytes ;
                    MEM_Copy ((unsigned char *)curHostPtr,
                              (unsigned char *) dspPtr,
                              availBytes,
                              Endianism_Default) ;
                    size -= availBytes ;
                    /* Increment the  current  host pointer */
                    curHostPtr += availBytes ;
                }
            }
            else {
                pciRegs->PCIBAR0TRL = (((Uint32) srcAddr + rBytes) &
                                       (0xFE000000)) ;
            }
        }
        pciRegs->PCIBAR0TRL = pageBase ;

        SYNC_SpinLockEndEx (halObject->lock,irqFlags) ;
#endif
    }

    TRC_1LEAVE ("DM6437_halPciReadDMA", status) ;

    return status ;
}


/* ============================================================================
 *  @func   DM6437_halPciWriteDMA
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
DM6437_halPciWriteDMA (IN Pvoid           halObj,
                       IN Uint32          srcAddr,
                       IN Uint32          dstAddr,
                       IN Uint32          size)
{
    DSP_STATUS           status    = DSP_SOK ;
    DM6437_pciRegs *     pciRegs   = NULL    ;
    DM6437_HalObj *      halObject = (DM6437_HalObj *) halObj ;
    Uint32               irqFlags            ;
    Uint32               pageBase            ;

#if defined(LINUXPC_DSP_DMA)
    DM6437_edmaRegs *   edmaRegs  = NULL     ;
    Uint32               numUSec             ;
    Uint32               tmp                 ;
    Uint32               i                   ;
    Uint32               count               ;
    Uint32               tSize               ;
    Uint32               writeSize = 0       ;
    Uint32               chnlId    = 0       ;
#else
    Uint32               wBytes     = 0      ;
    Uint32               availBytes = 0      ;
    Char8 *              dspPtr     = NULL   ;
    Char8 *              curHostPtr          ;
#endif



    TRC_4ENTER ("DM6437_halPciWriteDMA", halObj, srcAddr, dstAddr, size) ;

    DBC_Require (halObject != NULL) ;
    DBC_Require (size > 0) ;

    if (halObject == NULL) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        pciRegs  = (DM6437_pciRegs *) ( halObject->pci.barVirt [CFG_REGS_BAR_NO]
                                       + DM6437_PCIREG_BASE) ;

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
            edmaRegs = (DM6437_edmaRegs *)(
                                   halObject->pci.barVirt [CFG_REGS_BAR_NO]
                                   + DM6437_EDMAREG_BASE) ;
            count = (  (size + (srcAddr & ~DM6437_PCIADWRBITMASK))
                     / (DM6437_PCIADLEN)) + 1 ;

            irqFlags = SYNC_SpinLockStartEx (halObject->lock) ;

            for (i = 0 ; i < count ; i++) {
                pageBase = (  (srcAddr + (DM6437_PCIADLEN * i))
                            & DM6437_PCIADWRBITMASK) ;
                pciRegs->PCIADDSUB [i] = pageBase ;
            }

            do {
                /* Set the interrupt enable for 1st Channel. */
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
                edmaRegs->PARAMENTRY [chnlId].OPTION       =
                                         ( 0x00100004u | (chnlId << 12)  );
                tmp = DM6437_PCIMEM_BASE +
                      (srcAddr & ~DM6437_PCIADWRBITMASK);
                edmaRegs->PARAMENTRY [chnlId].SRC          = tmp ;
                edmaRegs->PARAMENTRY [chnlId].DST          = dstAddr ;

                /* Calculate the A & B count */
                if (size > 0x7000u)  {
                    tmp   = size / 0x7000u ;
                    tSize = (tmp * 0x7000u) ;
                    size -= (tmp * 0x7000u) ;
                    tmp <<= 16u ;
                    tmp  |= 0x7000u ;
                }
                else {
                    tmp = 0x10000u | size ;
                    tSize = size ;
                    size = 0u ;
                }

                edmaRegs->PARAMENTRY [chnlId].A_B_CNT      = tmp ;
                edmaRegs->PARAMENTRY [chnlId].LINK_BCNTRLD = 0xFFFFu ;
                edmaRegs->PARAMENTRY [chnlId].SRC_DST_CIDX = 0u ;
                /* no offset difference required */
                edmaRegs->PARAMENTRY [chnlId].SRC_DST_BIDX = 0x70007000u ;
                /* C Count is set to 1 since mostly size will not be more
                 * than 1GB
                 */
                edmaRegs->PARAMENTRY [chnlId].CCNT         = 0x1u ;

                /* Set the interrupt enable for 1st Channel. */
                if (chnlId < 32u) {
                    edmaRegs->EESR  |= 0x1u << chnlId ;
                }
                else {
                    edmaRegs->EESRH |= 0x1u << (chnlId - 32u) ;
                }

                /* Clear any pending interrupt. */
                if (chnlId < 32u) {
                    edmaRegs->ESR  |= 0x1u << chnlId ;
                }
                else {
                    edmaRegs->ESRH |= 0x1u << (chnlId - 32u) ;
                }

                /* wait for current DMA to finish. */
                if (chnlId < 32u) {
                    numUSec = tSize / PCI33_DMA_MAXTHROUGHPUT ;
                    do {
                        SYNC_UDelay (numUSec) ;
                        /* now check in steps of 10 usec. */
                        numUSec = 10 ;
                    } while ((edmaRegs->IPR & (0x1u << chnlId)) == 0) ;
                }
                else {
                    numUSec = tSize / PCI33_DMA_MAXTHROUGHPUT ;
                    do {
                        SYNC_UDelay (numUSec) ;
                        /* now check in steps of 10 usec. */
                        numUSec = 10 ;
                    } while ((edmaRegs->IPRH &
                              (0x1u << (chnlId - 32u))) == 0) ;
                }

                if (size != 0u) {
                    srcAddr += tSize ;
                    dstAddr += tSize ;
                }

            } while (size != 0u) ;

            SYNC_SpinLockEndEx (halObject->lock, irqFlags) ;
             if (writeSize != size) {
                status = DSP_EFAIL ;
            }
        }

#else
        curHostPtr       = (Char8 *) __va(srcAddr)    ;
        irqFlags = SYNC_SpinLockStartEx (halObject->lock) ;
        pageBase = pciRegs->PCIBAR0TRL ;
         /* if the address is within the already mapped region (within 32MB) */
        while (size > 0) {
            if (   ((dstAddr + wBytes) >= pciRegs->PCIBAR0TRL)
                && ((dstAddr + wBytes) <=
                     pciRegs->PCIBAR0TRL + 0x2000000u)) {
                dspPtr = (Char8 *)(  halObject->pci.barVirt [0u]
                                   + ((dstAddr + wBytes) -
                                       pciRegs->PCIBAR0TRL)) ;
                availBytes = (  (halObject->pci.barLen [0u])
                              - (((Uint32) dstAddr + wBytes) -
                                   pciRegs->PCIBAR0TRL)) ;
                if (availBytes >= size) {
                    MEM_Copy ((unsigned char *)dspPtr,
                              (unsigned char *)curHostPtr,
                              size,
                              Endianism_Default) ;
                    size = 0 ;
                }
                else {
                    wBytes += availBytes ;
                    MEM_Copy ((unsigned char *) dspPtr,
                              (unsigned char *) curHostPtr,
                              availBytes,
                              Endianism_Default) ;
                    size -= availBytes ;
                    curHostPtr += availBytes ;
                }
            }
            else {
                pciRegs->PCIBAR0TRL = (((Uint32) dstAddr + wBytes) &
                                        (0xFE000000)) ;
            }
        }
        pciRegs->PCIBAR0TRL = pageBase ;
        SYNC_SpinLockEndEx (halObject->lock, irqFlags) ;
#endif
    }

    TRC_1LEAVE ("DM6437_halPciWriteDMA", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
