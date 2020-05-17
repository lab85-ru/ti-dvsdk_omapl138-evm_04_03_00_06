/** ============================================================================
 *  @file   sma_pool.c
 *
 *  @path   $(DSPLINK)/dsp/src/pools/DspBios/
 *
 *  @desc   Implementation of SMAPOOL-Shared Memory Allocator POOL sub component
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <sys.h>
#include <gbl.h>
#include <pool.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <platform.h>
#include <dbc.h>
#include <failure.h>
#include <hal_cache.h>
#include <_dsplink.h>
#include <mpcs.h>
#include <_mpcs.h>
#include <_sma_pool.h>
#include <sma_pool.h>
#include <dsplinkpool.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define     FILEID  FID_SMAPOOL_C

/** ============================================================================
 *  @name   INIT_VALUE
 *
 *  @desc   Value to indicate the initialization of SMA module.
 *  ============================================================================
 */
#define     INIT_VALUE           0XDEAF

/** ============================================================================
 *  @name   EXIT_VALUE
 *
 *  @desc   Value to indicate the finalization of SMA module.
 *  ============================================================================
 */
#define     EXIT_VALUE           0xFACE

#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @name   SMAPOOL_ALLOCSTAMP
 *
 *  @desc   Value to indicate that the buffer is allocated.
 *  ============================================================================
*/
#define     SMAPOOL_ALLOCSTAMP   0xCAFE
#endif /* if defined (DDSP_PROFILE) */

/** ============================================================================
 *  @name   BUS_WIDTH
 *
 *  @desc   Width of bus in MADUs.
 *  ============================================================================
 */
#define     BUS_WIDTH            2


/** ============================================================================
 *  @const  DSPLINKPOOL_ctrlPtr
 *
 *  @desc   Declaration of array of pointers to shared control regions for all
 *          pool instances.
 *  ============================================================================
 */
extern DSPLINKPOOL_Ctrl * DSPLINKPOOL_ctrlPtr [MAX_POOLENTRIES] ;

/** ============================================================================
 *  @name   DSPLINK_shmBaseAddress
 *
 *  @desc   Declaration of base address for the DSPLINK shared memory.
 *  ============================================================================
 */
extern Uint32 DSPLINK_shmBaseAddress ;


/** ============================================================================
 *  @func   SMAPOOL_FXNS
 *
 *  @desc   Allocator interface functions. There is no action for the close
 *          function, so just using FXN_F_nop to save footprint.
 *
 *  @modif  None.
 *  ============================================================================
 */
const POOL_Fxns SMAPOOL_FXNS = {
    SMAPOOL_open,
    FXN_F_nop,
    SMAPOOL_alloc,
    SMAPOOL_free,
} ;


/** ============================================================================
 *  @func   SMAPOOL_init
 *
 *  @desc   This function initializes the SMA component.
 *
 *  @modif  None.
 *  ============================================================================
 */
Void
SMAPOOL_init ()
{
    _DSPLINK_init () ;
}


/** ============================================================================
 *  @func   SMAPOOL_open
 *
 *  @desc   This function opens the SMA POOL module. Base address of SMA
 *          POOL is configured through configuration database. it waits until
 *          GPP makes its init flag set, i.e. initializes the SMA POOL module.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
SMAPOOL_open (Ptr * object, Ptr params)
{
    Int              status    = SYS_OK ;
    SMAPOOL_Params * smaParams = (SMAPOOL_Params *) params ;
    DRV_Ctrl *       drvCtrl   = (DRV_Ctrl *) DSPLINK_shmBaseAddress ;
    MPCS_Handle      temp      = NULL ;
    SMAPOOL_Obj *    smaCtrl ;

    *object = (Ptr) DSPLINKPOOL_ctrlPtr [smaParams->poolId] ;
    smaCtrl = (SMAPOOL_Obj *) DSPLINKPOOL_ctrlPtr [smaParams->poolId]->dspAddr ;

    /* Wait for GPP to open the Pool. */
    do {
        HAL_cacheInv ((Ptr) &(smaCtrl->ctrlPtr), sizeof (smaCtrl->ctrlPtr)) ;
    }
    while (smaCtrl->ctrlPtr.isGppInit != INIT_VALUE) ;

    /* Pool shared structures have been created by the GPP at this point. */

    HAL_cacheInv ((Ptr) &(smaCtrl->exactMatchReq),
                  sizeof (smaCtrl->exactMatchReq)) ;

    /* The MPCS object is created by the GPP-side. */
    status = _MPCS_open (ID_GPP,
                         MPCS_RESV_LOCKNAME,
                         &temp,
                         &(smaCtrl->mpcsObj)) ;

    drvCtrl->poolDspInitDone = (Uint32) status ;
    HAL_cacheWbInv ((Void *) drvCtrl, sizeof (DRV_Ctrl)) ;

    /* Handshake with the GPP */
    DSPLINK_handshake (ID_GPP, DRV_HANDSHAKE_POOL) ;

    return status ;
}


/** ============================================================================
 *  @func   SMAPOOL_alloc
 *
 *  @desc   This function allocates a free buffer from the specified buffer pool
 *          and returns it to the user.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
SMAPOOL_alloc (Ptr object, Ptr * buffer, size_t size)
{
    Int                status      = SYS_OK ;
    DSPLINKPOOL_Ctrl * poolCtrlPtr = (DSPLINKPOOL_Ctrl *) object ;
    Uint32             i           = 0 ;
    SMAPOOL_Obj *      smaCtrl ;
    SMAPOOL_BufObj *   bufHandle ; /* to hold the buffer handles */

    DBC_require (object != 0) ;
    DBC_require (size   >  0) ;
    DBC_require (buffer != 0) ;

    smaCtrl   = (SMAPOOL_Obj *) poolCtrlPtr->dspAddr ;
    bufHandle = (SMAPOOL_BufObj *) smaCtrl->ctrlPtr.smaBufObjs ;
    HAL_cacheInv ((Ptr) bufHandle, sizeof (SMAPOOL_BufObj)) ;

    /* Align the size to BUS WIDTH. */
    size = DSPLINK_ALIGN (size, BUS_WIDTH) ;

    for (i = 0 ; i < smaCtrl->ctrlPtr.numBufs ; i++ ) {
        if (smaCtrl->exactMatchReq == FALSE) {
            /* Look for the nearest match for the requested size */
            if (bufHandle[i].size >= size) {
                /* Found a match for required size. */
                break ;
            }
        }
        else {
            /* Look for the exact match for the requested size */
            if (bufHandle[i].size == size) {
                    /* Found a match for required size. */
                break ;
            }
        }

        HAL_cacheInv ((Ptr) &bufHandle [i], sizeof (SMAPOOL_BufObj)) ;
    }

    /* Buffer pool found with specified size */
    if (i != smaCtrl->ctrlPtr.numBufs) {
        /* Enter the critical section */
        MPCS_enter ((MPCS_Handle) &(smaCtrl->mpcsObj)) ;
        HAL_cacheInv ((Ptr) &bufHandle [i], sizeof (SMAPOOL_BufObj)) ;

        if (bufHandle[i].freeBuffers == 0) {
            status = SYS_EALLOC ; /* All buffers are allocated. */
            SET_FAILURE_REASON(status);
        }
        else {
            *buffer             = (Uint32 *) (  bufHandle[i].bufDspAddress
                                              + bufHandle[i].nextFree) ;
            HAL_cacheInv ((Ptr) *buffer, size) ;
            bufHandle[i].nextFree = (Uint32)
                                  ((SMAPOOL_BufHeader *) *buffer)->next ;
            bufHandle[i].freeBuffers-- ;
            HAL_cacheWbInv ((Ptr) &bufHandle [i], sizeof (SMAPOOL_BufObj)) ;
        }

        /* Leave the critical section */
        MPCS_leave ((MPCS_Handle) &(smaCtrl->mpcsObj)) ;
    }
    else {
          status = SYS_EINVAL ; /* Required size not found. */
          SET_FAILURE_REASON (status) ;
    }

    return status ;
}


/** ============================================================================
 *  @func   SMAPOOL_free
 *
 *  @desc   This function frees the buffer specified by the user, and returns it
 *          to the buffer pool.
 *
 *  @modif  None.
 *  ============================================================================
 */
Void
SMAPOOL_free (Ptr object, Ptr buffer, size_t size)
{
    DSPLINKPOOL_Ctrl *  poolCtrlPtr = (DSPLINKPOOL_Ctrl *) object ;
    Uint32              i           = 0 ;
    SMAPOOL_Obj *       smaCtrl ;
    SMAPOOL_BufObj  *   bufHandle ; /* to hold the buffer handles */

    DBC_require (object != 0) ;
    DBC_require (size   >  0) ;
    DBC_require (buffer != NULL) ;

    smaCtrl   = (SMAPOOL_Obj *) poolCtrlPtr->dspAddr ;
    bufHandle = (SMAPOOL_BufObj *) smaCtrl->ctrlPtr.smaBufObjs ;
    HAL_cacheInv ((Ptr) bufHandle, sizeof (SMAPOOL_BufObj)) ;

    /* Align the size to BUS WIDTH. */
    size = DSPLINK_ALIGN (size, BUS_WIDTH) ;

    for (i = 0 ; i < smaCtrl->ctrlPtr.numBufs ; i++ ) {
        if (smaCtrl->exactMatchReq == FALSE) {
            /* Look for the nearest match for the requested size */
            if (bufHandle[i].size >= size) {
                /* Found a match for required size. */
                break ;
            }
        }
        else {
            /* Look for the exact match for the requested size */
            if (bufHandle[i].size == size) {
                /* Found a match for required size. */
                break ;
            }
        }
        HAL_cacheInv ((Ptr) &bufHandle [i], sizeof (SMAPOOL_BufObj)) ;
    }

    /* Buffer pool found with specified size */
    if (i != smaCtrl->ctrlPtr.numBufs) {
        /* Enter the critical section */
        MPCS_enter ((MPCS_Handle) &(smaCtrl->mpcsObj)) ;

        HAL_cacheInv ((Ptr) &bufHandle [i], sizeof (SMAPOOL_BufObj)) ;
        HAL_cacheInv ((Ptr) buffer, size) ;
#if defined (DDSP_PROFILE)
        /*  usedOnce field set to indicate that buffer was
         *  allocated
         */
        ((SMAPOOL_BufHeader *) buffer)->usedOnce = SMAPOOL_ALLOCSTAMP ;
#endif /* if defined (DDSP_PROFILE) */

        ((SMAPOOL_BufHeader *) buffer)->next =
                       (SMAPOOL_BufHeader *) ((Uint32) bufHandle[i].nextFree) ;
        HAL_cacheWbInv ((Ptr) buffer, size) ;
        bufHandle[i].nextFree = (Uint32) (  (Uint32) buffer
                                          - bufHandle [i].bufDspAddress) ;

        bufHandle[i].freeBuffers++ ;
        HAL_cacheWbInv ((Ptr) &bufHandle [i], sizeof (SMAPOOL_BufObj)) ;

        /* Leave the critical section */
        MPCS_leave ((MPCS_Handle) &(smaCtrl->mpcsObj)) ;
    }
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
