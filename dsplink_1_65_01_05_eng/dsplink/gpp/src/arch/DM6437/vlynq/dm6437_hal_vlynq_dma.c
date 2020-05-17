/** ============================================================================
 *  @file   dm6437_hal_vlynq_dma.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM6437/vlynq/
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


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <sync.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm6437_hal.h>
#include <dm6437_hal_vlynq.h>
#include <dm6437_hal_vlynq_dma.h>


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

/** ============================================================================
 *  @macro  DELAY_COUNT
 *
 *  @desc   Since PrOS doesnot provides usec delay we need to use for loop.
 *  ============================================================================
 */
#define  DELAY_COUNT       0x3000u


/* ============================================================================
 *  @func   DM6437_halVlynqReadDMA
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
DM6437_halVlynqReadDMA (IN Pvoid           halObj,
                        IN Uint32          srcAddr,
                        IN Uint32          dstAddr,
                        IN Uint32          size)
{
    DSP_STATUS           status      = DSP_SOK ;
    volatile DRA44XGEM_edmaRegs              * edmaRegs          = NULL ;
    DM6437_HalObj *      halObject   = (DM6437_HalObj *) halObj ;
    DM6437_vlynqRegs *   vlynqRegs   = NULL    ;
    Uint32               readSize    = 0       ;
    Uint32               chnlId      = -1u     ;
    Uint32               tmp                   ;
    Uint32               tSize                 ;
    Uint32               orgPageBase           ;
    Uint32               irqFlags              ;


    TRC_4ENTER ("DM6437_halVlynqReadDMA", halObj, srcAddr, dstAddr, size) ;

    DBC_Require (halObject != NULL) ;
    DBC_Require (size > 0) ;

    edmaRegs   = (volatile DRA44XGEM_edmaRegs *) (DRA44XGEM_EDMA_REGBASE) ;
    vlynqRegs  = (DM6437_vlynqRegs *) halObject->vlynq.ctrlBaseAddr ;

    if (halObject == NULL) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        if (halObject->vlynq.edmaChnlId == 0xFFFFFFFFu) {
            PRINT_Printf (
                    "EDMA Channel ID not provided, please provide a valid"
                    "EDMA Channel for DSPLink in the CFG file."
                    "This EDMA Channel must be used by DSPLink exculsively\n") ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else {
            chnlId = halObject->vlynq.edmaChnlId ;
        }
    }

    irqFlags = SYNC_SpinLockStartEx (halObject->lock) ;

    if (DSP_SUCCEEDED (status)) {
        /* Save the original page base address */
        orgPageBase = vlynqRegs->PEER_RAMO4 ;
        /* Enable the shadow region 0 for the channel */
        if (chnlId < 32u) {
            edmaRegs->DRA[0].DRAE  |= (0x1u << chnlId);
        }
         else {
            edmaRegs->DRA[0].DRAEH |= (0x1u << (chnlId - 32u));
        }

        do {
            vlynqRegs->PEER_RAMO4 = srcAddr ;
            /* Set the interrupt enable for the required channel. */
            if (chnlId < 32u) {
                edmaRegs->SHADOW[0].IESR |= 0x1u << chnlId ;
            }
            else {
                edmaRegs->SHADOW[0].IESRH |= 0x1u << (chnlId - 32u) ;
            }

            /* Clear any pending interrupt. */
            if (chnlId < 32u) {
                edmaRegs->SHADOW[0].ICR  |= 0x1u << chnlId ;
            }
            else {
                edmaRegs->SHADOW[0].ICRH |= 0x1u << (chnlId - 32u) ;
            }

            /* Populate the Param entry. */
            /* Set the  TCC field in the oprion paramter such that corrsponding
             * bit in the IPR will be set after transfer completion.
             */
            edmaRegs->PARAMENTRY [chnlId].OPTION = ( 0x00100004u | (chnlId << 12)  );
            edmaRegs->PARAMENTRY [chnlId].SRC    =
                                                 halObject->vlynq.region4Addr ;
            edmaRegs->PARAMENTRY [chnlId].DST    = dstAddr ;

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
            /* C Count is set to 1 since mostly size will not be more than 1GB*/
            edmaRegs->PARAMENTRY [chnlId].CCNT         = 0x1u ;
            /* no offset difference required */
            edmaRegs->PARAMENTRY [chnlId].SRC_DST_BIDX = 0x70007000u ;

            /* Set the interrupt enable for 1st Channel. */
            if (chnlId < 32u) {
                edmaRegs->SHADOW[0].EESR  |= 0x1u << chnlId ;
            }
            else {
                edmaRegs->SHADOW[0].EESRH |= 0x1u << (chnlId - 32u) ;
            }

            /* Enale the event corresponding to the chnlId
             * in the event set register
             */
            if (chnlId < 32u) {
                edmaRegs->SHADOW[0].ESR  |= 0x1u << chnlId ;
            }
            else {
                edmaRegs->SHADOW[0].ESRH |= 0x1u << (chnlId - 32u) ;
            }

            /* wait for current DMA to finish. */
            if (chnlId < 32u) {
                while ((edmaRegs->SHADOW[0].IPR & (0x1u << chnlId)) == 0u) ;
            }
            else {
                 while ((edmaRegs->SHADOW[0].IPRH & (0x1u << (chnlId - 32u))) == 0u) ;
            }

            if (size != 0u) {
                srcAddr += tSize ;
                dstAddr += tSize ;
            }

        } while (size != 0u) ;

        /* Restore the original page base address */
        vlynqRegs->PEER_RAMO4 = orgPageBase ;

        if (readSize != size) {
            status = DSP_EFAIL ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    SYNC_SpinLockEndEx (halObject->lock, irqFlags) ;

    TRC_1LEAVE ("DM6437_halVlynqReadDMA", status) ;

    return status ;
}


/* ============================================================================
 *  @func   DM6437_halVlynqWriteDMA
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
DM6437_halVlynqWriteDMA (IN Pvoid           halObj,
                         IN Uint32          srcAddr,
                         IN Uint32          dstAddr,
                         IN Uint32          size)
{
    DSP_STATUS              status    = DSP_SOK ;
    volatile DRA44XGEM_edmaRegs * edmaRegs = NULL ;
    DM6437_vlynqRegs *      vlynqRegs   = NULL    ;
    DM6437_HalObj *         halObject   = (DM6437_HalObj *) halObj ;
    Uint32                  writeSize   = 0   ;
    Uint32                  chnlId      = -1u ;
    Uint32                  tmp               ;
    Uint32                  tSize             ;
    Uint32                  orgPageBase       ;
    Uint32                  irqFlags          ;

    TRC_4ENTER ("DM6437_halVlynqWriteDMA", halObj, srcAddr, dstAddr, size) ;

    DBC_Require (halObject != NULL) ;
    DBC_Require (size > 0) ;

    edmaRegs   = (volatile DRA44XGEM_edmaRegs *) (DRA44XGEM_EDMA_REGBASE) ;
    vlynqRegs  = (DM6437_vlynqRegs *) halObject->vlynq.ctrlBaseAddr ;

    if (halObject == NULL) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        if (halObject->vlynq.edmaChnlId == 0xFFFFFFFFu) {
            PRINT_Printf (
                    "EDMA Channel ID not provided, please provide a valid"
                    "EDMA Channel for DSPLink in the CFG file."
                    "This EDMA Channel must be used by DSPLink exculsively\n") ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else {
            chnlId = halObject->vlynq.edmaChnlId ;
        }
    }

    irqFlags = SYNC_SpinLockStartEx (halObject->lock) ;

    if (DSP_SUCCEEDED (status)) {
        /* Save the original page base address */
        orgPageBase = vlynqRegs->PEER_RAMO4 ;

        /* Enable the shadow region 0 for the channel */
        if (chnlId < 32u) {
            edmaRegs->DRA[0].DRAE  |= (0x1u << chnlId);
        }
         else {
            edmaRegs->DRA[0].DRAEH |= (0x1u << (chnlId - 32u));
        }

        do {
            vlynqRegs->PEER_RAMO4 = dstAddr ;
            /* Set the interrupt enable for 1st Channel. */
            if (chnlId < 32u) {
                edmaRegs->SHADOW[0].IESR |= 0x1u << chnlId ;

            }
            else {
                edmaRegs->SHADOW[0].IESRH |= 0x1u << (chnlId - 32u) ;
            }

            /* Clear any pending interrupt. */
            if (chnlId < 32u) {
                edmaRegs->SHADOW[0].ICR |= 0x1u << chnlId ;
            }
            else {
                edmaRegs->SHADOW[0].ICRH |= 0x1u << (chnlId - 32u) ;
            }

            /* Populate the Param entry. */
            /* Set the  TCC field in the oprion paramter such that corrsponding
             * bit in the IPR will be set after transfer completion.
             */
            edmaRegs->PARAMENTRY [chnlId].OPTION = ( 0x00100004u | (chnlId << 12)  );
            edmaRegs->PARAMENTRY [chnlId].SRC   = srcAddr ;
            edmaRegs->PARAMENTRY [chnlId].DST   = halObject->vlynq.region4Addr ;

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
            /* C Count is set to 1 since mostly size will not be more than 1GB*/
            edmaRegs->PARAMENTRY [chnlId].CCNT         = 0x1u ;

            /* Set the interrupt enable for 1st Channel. */
            if (chnlId < 32u) {
                edmaRegs->SHADOW[0].EESR  |= 0x1u << chnlId ;
            }
            else {
                edmaRegs->SHADOW[0].EESRH |= 0x1u << (chnlId - 32u) ;
            }

            /* Clear any pending interrupt. */
            if (chnlId < 32u) {
                edmaRegs->SHADOW[0].ESR  |= 0x1u << chnlId ;
            }
            else {
                edmaRegs->SHADOW[0].ESRH |= 0x1u << (chnlId - 32u) ;
            }

            /* wait for current DMA to finish. */
            if (chnlId < 32u) {
                while ((edmaRegs->SHADOW[0].IPR & (0x1u << chnlId)) == 0u) ;
            }
            else {
                while ((edmaRegs->SHADOW[0].IPRH & (0x1u << (chnlId - 32u))) == 0u) ;
            }

            if (size != 0u) {
                srcAddr += tSize ;
                dstAddr += tSize ;
            }

        } while (size != 0u) ;

        /* Restore the original page base address */
        vlynqRegs->PEER_RAMO4 = orgPageBase ;

        if (writeSize != size) {
            status = DSP_EFAIL ;
       }
    }
    else {
        SET_FAILURE_REASON ;
    }

    SYNC_SpinLockEndEx (halObject->lock,irqFlags) ;

    TRC_1LEAVE ("DM6437_halVlynqWriteDMA", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
