/** ============================================================================
 *  @file   dsplink.c
 *
 *  @path   $(DSPLINK)/dsp/src/base/drv/
 *
 *  @desc   Implementation of the functionality to initialize DSP/BIOS LINK.
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
#include <gbl.h>
#include <clk.h>
#include <pool.h>
#include <string.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <failure.h>
#include <dbc.h>
#include <hal.h>
#include <hal_cache.h>
#include <_dsplink.h>
#include <shm_drv.h>
#include <dsplinkips.h>

#if defined (POOL_COMPONENT)
#include <dsplinkpool.h>
#endif /* if defined (POOL_COMPONENT) */

#if defined (MPCS_COMPONENT)
#include <_mpcs.h>
#endif /* if defined (MPCS_COMPONENT) */

#if defined (MPLIST_COMPONENT)
#include <_mplist.h>
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (MSGQ_COMPONENT)
#include <msgq.h>
#include <dsplinkmqt.h>
#endif /* if defined (MSGQ_COMPONENT) */

#if defined (CHNL_COMPONENT)
#include <dsplinkdata.h>
#endif /* if defined (CHNL_COMPONENT) */

#if defined (NOTIFY_COMPONENT)
#include <_notify.h>
#endif /* if defined (NOTIFY_COMPONENT) */

#if defined (RINGIO_COMPONENT)
#include <_ringio.h>
#endif /* if defined (RINGIO_COMPONENT) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define FILEID               FID_DRV_C

/** ============================================================================
 *  @const  COMP_UNINITIALIZED
 *
 *  @desc   Indicates that the component is uninitialized.
 *  ============================================================================
 */
#define COMP_UNINITIALIZED        (Uint32) -1


/** ============================================================================
 *  @const  DSPLINK_shmBaseAddress
 *
 *  @desc   Location where the starting address of the shared memory used by
 *          the DSPLINK components is stored.
 *  ============================================================================
 */
#pragma DATA_ALIGN(DSPLINK_shmBaseAddress, CACHE_L2_LINESIZE)
#pragma DATA_SECTION (DSPLINK_shmBaseAddress, ".data:DSPLINK_shmBaseAddress") ;
Uint32 DSPLINK_shmBaseAddress ;
#pragma DATA_SECTION (DSPLINK_shmBaseAddress_pad, ".data:DSPLINK_shmBaseAddress") ;
Uint8  DSPLINK_shmBaseAddress_pad[CACHE_L2_LINESIZE- sizeof(Uint32)] ;

/** ----------------------------------------------------------------------------
 *  @name   DSPLINK_isInitialized
 *
 *  @desc   Indicates if DSP/BIOS LINK has been initialized
 *  ----------------------------------------------------------------------------
 */
static Bool DSPLINK_isInitialized = FALSE ;

/** ============================================================================
 *  Create named sections for all DSPLINK functions to allow specific memory
 *  placement.
 *  ============================================================================
 */
#pragma CODE_SECTION (DSPLINK_init,                 ".text:DSPLINK_init")

/** ============================================================================
 *  @const  NUM_CHARS_VERSION
 *
 *  @desc   Indicates number of characters to be checked for version matching
 *          This will check only 1.6\0
 *  ============================================================================
 */
#define NUM_CHARS_VERSION       4


/** ============================================================================
 *  @func   DSPLINK_init
 *
 *  @desc   This function initializes DSP/BIOS LINK. It is called by the
 *          applications.
 *
 *  @modif  None.
 *  ============================================================================
 */
Void
DSPLINK_init  (Void)
{
    Uint16     dspHandshake = DRV_HANDSHAKE_BASE ;
    DRV_Ctrl * ctrlPtr      = NULL ;

    /* Call the internal function to initialize DSPLINK. By this time, data
     * drivers (if any) and pools (if any) are already initialized.
     */
    _DSPLINK_init () ;

    ctrlPtr = (DRV_Ctrl *) DSPLINK_shmBaseAddress ;

    /* If data drivers or pools were not included by the application, complete
     * the handshake for them to ensure that the GPP-side can proceed further.
     */
#if defined (POOL_COMPONENT)
    if (ctrlPtr->poolDspInitDone == COMP_UNINITIALIZED) {
        ctrlPtr->poolDspInitDone = (Uint32) SYS_OK ;
        dspHandshake |= DRV_HANDSHAKE_POOL ;
    }
#endif /* if defined (POOL_COMPONENT) */

#if defined (CHNL_COMPONENT)
    if (ctrlPtr->dataDspInitDone == COMP_UNINITIALIZED) {
        ctrlPtr->dataDspInitDone = (Uint32) SYS_OK ;
        dspHandshake |= DRV_HANDSHAKE_DATA ;
    }
#endif /* if defined (CHNL_COMPONENT) */

    HAL_cacheWbInv ((Void *) ctrlPtr, sizeof (DRV_Ctrl)) ;

    /* Complete the handshake for configured components as expected by the
     * GPP, even if they are not included by the application.
     */
    SHMDRV_handshake (ID_GPP, dspHandshake) ;
}


/** ============================================================================
 *  @func   _DSPLINK_init
 *
 *  @desc   This function initializes DSP/BIOS LINK. Tihs is the internal
 *          implementation for the function to initialize DSPLINK.
 *
 *  @modif  None.
 *  ============================================================================
 */
Void
_DSPLINK_init  (Void)
{
    Int        status       = SYS_OK ;
    Uint16     dspHandshake = DRV_HANDSHAKE_BASE ;
    DRV_Ctrl * ctrlPtr      = NULL ;

    /* Ensure that the component is initialized only once. */
    if (DSPLINK_isInitialized == FALSE) {
        DSPLINK_isInitialized = TRUE ;
        do {
            /* Wait till DSPLink is initialized i.e. PROC_start is called */
            HAL_cacheInv ((Void *) &DSPLINK_shmBaseAddress, sizeof (Uint32)) ;
        } while (DSPLINK_shmBaseAddress == 0x0) ;

        ctrlPtr      = (DRV_Ctrl *) DSPLINK_shmBaseAddress ;
        DBC_assert (ctrlPtr != NULL) ;

        HAL_init () ;

        HAL_cacheInv ((Void *) ctrlPtr, sizeof (DRV_Ctrl)) ;

        /* Check if GPP version matches the expected DSP version. */
        if (strncmp (ctrlPtr->version, DSPLINK_VERSION, (NUM_CHARS_VERSION -1)) != 0) {
            status = SYS_ENODEV ; /* Version mismatch. */
            ctrlPtr->drvDspInitDone = (Uint32) status ;
            SET_FAILURE_REASON (status) ;
        }
        else {
            /* If cpuFreq is configured as -1, default DSP/BIOS setting is
             * to be used.
             */
            if (ctrlPtr->config.cpuFreq != (Uint32) -1) {
                /* Configure the DSP CPU frequency. */
                GBL_setFrequency (ctrlPtr->config.cpuFreq) ;

                /* Reconfigure the timer */
                if (CLK_reconfig () == FALSE) {
                    status = SYS_EINVAL ;
                }
            }
        }

        if (status == SYS_OK) {
            ctrlPtr->procId = GBL_getProcId () ;
        }

        if (status == SYS_OK) {
            status = SHMDRV_init (ID_GPP, ctrlPtr->linkAddr) ;
            dspHandshake |= DRV_HANDSHAKE_DRV ;
            ctrlPtr->drvDspInitDone = (Uint32) status ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }

        if (status == SYS_OK) {
            status = DSPLINKIPS_init (ID_GPP,
                                      ctrlPtr->config.numIpsEntries,
                                      ctrlPtr->ipsAddr) ;
            dspHandshake |= DRV_HANDSHAKE_IPS ;
            ctrlPtr->ipsDspInitDone = (Uint32) status ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }

#if defined (POOL_COMPONENT)
        if ((status == SYS_OK) && (ctrlPtr->poolConfigured == TRUE)) {
            status = DSPLINKPOOL_init (ID_GPP,
                                       ctrlPtr->config.numPools,
                                       ctrlPtr->poolAddr) ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }
#endif /* if defined (POOL_COMPONENT) */

#if defined (MPCS_COMPONENT)
        if ((status == SYS_OK) && (ctrlPtr->mpcsConfigured == TRUE)) {
            status = MPCS_init (ID_GPP, ctrlPtr->mpcsAddr) ;
            dspHandshake |= DRV_HANDSHAKE_MPCS ;
            ctrlPtr->mpcsDspInitDone = (Uint32) status ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }
#endif /* if defined (MPCS_COMPONENT) */

#if defined (MPLIST_COMPONENT)
        if ((status == SYS_OK) && (ctrlPtr->mplistConfigured == TRUE)) {
            status = MPLIST_init (ID_GPP, ctrlPtr->mplistAddr) ;
            dspHandshake |= DRV_HANDSHAKE_MPLIST ;
            ctrlPtr->mplistDspInitDone = (Uint32) status ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (MSGQ_COMPONENT)
        if ((status == SYS_OK) && (ctrlPtr->mqtConfigured == TRUE)) {
            status = DSPLINKMQT_init (ID_GPP, ctrlPtr->mqtAddr) ;
            /* For MQT, handshake cannot be done after MQT is opened, since
             * mqtOpen is called after main (). If MSGQ were not enabled by
             * application, having handshake done by the MQT would result in
             * GPP-side wait for handshake not getting completed successfully.
             */
            dspHandshake |= DRV_HANDSHAKE_MQT ;
            ctrlPtr->mqtDspInitDone = (Uint32) status ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }
#endif /* if defined (MSGQ_COMPONENT) */

#if defined (CHNL_COMPONENT)
        if ((status == SYS_OK) && (ctrlPtr->dataConfigured == TRUE)) {
            status = DSPLINKDATA_init (ID_GPP,
                                       ctrlPtr->config.numDataDrivers,
                                       ctrlPtr->dataAddr) ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }
#endif /* if defined (CHNL_COMPONENT) */

#if defined (NOTIFY_COMPONENT)
        if (status == SYS_OK) {
            status = NOTIFY_init (ID_GPP,
                                  ctrlPtr->config.numIpsEntries,
                                  ctrlPtr->ipsAddr) ;
            dspHandshake |= DRV_HANDSHAKE_NOTIFY ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
                if (ctrlPtr->ipsDspInitDone == SYS_OK) {
                    ctrlPtr->ipsDspInitDone = (Uint32) status ;
                }
            }
        }
#endif /* if defined (NOTIFY_COMPONENT) */

#if defined (RINGIO_COMPONENT)
        if ((status == SYS_OK) && (ctrlPtr->ringIoConfigured == TRUE)) {
            status = RingIO_init (ID_GPP, ctrlPtr->ringIoAddr) ;
            dspHandshake |= DRV_HANDSHAKE_RINGIO ;
            ctrlPtr->ringIoDspInitDone = (Uint32) status ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }
#endif /* if defined (RINGIO_COMPONENT) */

        /* Write the DSP-side version information into the control structure. */
        strcpy (ctrlPtr->version, DSPLINK_VERSION) ;

        HAL_cacheWbInv ((Void *) ctrlPtr, sizeof (DRV_Ctrl)) ;

        /* Handshake with the GPP */
        DSPLINK_handshake (ID_GPP, dspHandshake) ;
    }

    DBC_ensure (DSPLINK_isInitialized == TRUE) ;
}


#if defined (POOL_COMPONENT)
#if !defined (POOL_open) /* To ensure no clash with future BIOS definition */
/** ============================================================================
 *  @func   POOL_open
 *
 *  @desc   Function to set allocator pool id in POOL as well as call init and
 *          open function.
 *
 *  @modif  POOL.
 *  ============================================================================
 */
Int
POOL_open(Uint16     poolId,
          POOL_Obj * poolObj)
{
    Int          status    = SYS_OK ;
    POOL_Handle  poolHandle ;

    /* Make sure everything is hooked up */
    if (POOL == NULL) {
       status = SYS_EINVAL ;
    }
    else {
       POOL->allocators[poolId] = *poolObj ;

       poolHandle = &(POOL->allocators[poolId]) ;

       if (poolHandle->initFxn != NULL) {
           /* Call the allocator's init and open function.  */
           poolHandle->initFxn()  ;
       }

       status = poolHandle->fxns->open (&(poolHandle->object),
                                            poolHandle->params) ;
    }
    return status ;
}
#endif /* if !defined (POOL_open) */
#endif /* if defined (POOL_COMPONENT) */



#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
