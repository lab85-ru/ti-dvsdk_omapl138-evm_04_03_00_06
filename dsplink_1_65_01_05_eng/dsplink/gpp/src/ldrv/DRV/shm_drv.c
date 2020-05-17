/** ============================================================================
 *  @file   shm_drv.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/DRV/
 *
 *  @desc   Implementation of the Shared Memory driver component for
 *          initialization and synchronization of the GPP and DSP-side drivers.
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
#include <linkcfgdefs.h>
#include <procdefs.h>

#if defined (CHNL_COMPONENT)
#include <_datadefs.h>
#endif /* if defined (CHNL_COMPONENT) */


/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Link Driver                 */
#include <shm_drv.h>
#include <ldrv_drv.h>
#include <ldrv_smm.h>
#include <ldrv.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_DRV_SHM

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  TRC_SetReason (status, FID_C_LDRV_DRV_SHM, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @const  GPP_HANDSHAKE
 *
 *  @desc   Value written by GPP for synchronizing the link driver.
 *  ============================================================================
 */
#define GPP_HANDSHAKE               (Uint32) 0xC0C00000u

/** ============================================================================
 *  @const  DSP_HANDSHAKE
 *
 *  @desc   Basic value written by DSP for synchronizing the link driver.
 *  ============================================================================
 */
#define DSP_HANDSHAKE               (Uint32) 0xBABA0000u


/** ----------------------------------------------------------------------------
 *  @name   SHMDRV_Object
 *
 *  @desc   Defines the SHM link driver object, which contains all the
 *          component-specific information.
 *
 *  @field  ctrlPtr
 *              Pointer to the SHM Driver control structure in shared memory.
 *  ----------------------------------------------------------------------------
 */
typedef struct SHMDRV_Object_tag {
    SHMDRV_Ctrl *  ctrlPtr ;
} SHMDRV_Object ;


/** ----------------------------------------------------------------------------
 *  @name   SHMDRV_State
 *
 *  @desc   State object for the SHMDRV component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC SHMDRV_Object SHMDRV_State [MAX_DSPS] ;

/*  ----------------------------------------------------------------------------
 *  @name   SHMDRV_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool SHMDRV_IsInitialized [MAX_DSPS] ;


/** ============================================================================
 *  @name   SHMDRV_Interface
 *
 *  @desc   SHM driver interface table
 *  ============================================================================
 */
DRV_Interface SHMDRV_Interface = {
    &SHMDRV_init,
    &SHMDRV_exit,
    &SHMDRV_handshake
#if defined (DDSP_DEBUG)
    ,&SHMDRV_debug
#endif /* if defined (DDSP_DEBUG) */
} ;


/** ============================================================================
 *  @func   SHMDRV_init
 *
 *  @desc   This function initializes the SHMDRV component.
 *
 *  @modif  SHMDRV_State [dspId]
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
SHMDRV_init (IN  ProcessorId dspId,
             IN  Uint32      linkDrvId,
             OUT Uint32 *    shDspAddr)
{
    DSP_STATUS        status = DSP_SOK ;
    LINKCFG_LinkDrv * linkDrv ;
    SHMDRV_Object *   shmDrvState ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_3ENTER ("SHMDRV_init", dspId, linkDrvId, shDspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (shDspAddr != NULL) ;

    if (SHMDRV_IsInitialized [dspId] == FALSE) {
        shmDrvState = &(SHMDRV_State [dspId]) ;
        dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        linkDrv  = &(dspCfg->linkDrvObjects [linkDrvId]) ;

        /*  --------------------------------------------------------------------
         *  Allocate shared memory required for the SHM_DRV component.
         *  --------------------------------------------------------------------
         */
        status = LDRV_SMM_alloc (dspId,
                                 linkDrv->memEntry,
                                 NULL, /* Physical address not required. */
                                 shDspAddr,
                                 (Uint32 *) ((Pvoid) &(shmDrvState->ctrlPtr)),
                                 sizeof (SHMDRV_Ctrl)) ;
        if (DSP_SUCCEEDED (status)) {
            TRC_1PRINT (TRC_LEVEL4,
                        "SHMDRV Ctrl GPP address [0x%x]\n",
                        shmDrvState->ctrlPtr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "SHMDRV Ctrl DSP address [0x%x]\n",
                        *shDspAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "SHMDRV Ctrl size [0x%x]\n",
                        sizeof (SHMDRV_Ctrl)) ;

            /*  ----------------------------------------------------------------
             *  Initialize the SHM driver control fields
             *  ----------------------------------------------------------------
             */
            shmDrvState->ctrlPtr->handshakeGpp   = 0x0 ;
            shmDrvState->ctrlPtr->handshakeDsp   = 0x0 ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        SHMDRV_IsInitialized [dspId] = TRUE ;
        if (DSP_FAILED (status)) {
            /* SHMDRV_exit executes cleanup only if the initialized flag is
             * TRUE.
             */
            SHMDRV_exit (dspId, linkDrvId) ;
            SHMDRV_IsInitialized [dspId] = FALSE ;
        }
    }

    TRC_1LEAVE ("SHMDRV_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SHMDRV_exit
 *
 *  @desc   This function finalizes the SHMDRV component.
 *
 *  @modif  SHMDRV_State [dspId]
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
SHMDRV_exit (IN ProcessorId dspId, IN Uint32 linkDrvId)
{
    DSP_STATUS        status     = DSP_SOK ;
    LINKCFG_LinkDrv * linkDrv ;
    SHMDRV_Object *   shmDrvState ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_2ENTER ("SHMDRV_exit", dspId, linkDrvId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (SHMDRV_IsInitialized [dspId] == TRUE) {
        shmDrvState = &(SHMDRV_State [dspId]) ;
        dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        linkDrv  = &(dspCfg->linkDrvObjects [linkDrvId]) ;

        if (shmDrvState->ctrlPtr != NULL) {
            /* -----------------------------------------------------------------
             *  Clear the link-driver control structure in DSP memory space
             * -----------------------------------------------------------------
             */
            shmDrvState->ctrlPtr->handshakeGpp   = 0x0 ;
            shmDrvState->ctrlPtr->handshakeDsp   = 0x0 ;

            /*  ----------------------------------------------------------------
             *  Free shared memory required for the SHM_DRV component.
             *  ----------------------------------------------------------------
             */
            status = LDRV_SMM_free (dspId,
                                    linkDrv->memEntry,
                                    sizeof (SHMDRV_Ctrl)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        SHMDRV_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("SHMDRV_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SHMDRV_handshake
 *
 *  @desc   This function performs the necessary handshake between the drivers
 *          on the GPP & DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
SHMDRV_handshake (IN  ProcessorId   dspId,
                  IN  Uint32        linkDrvId,
                  IN  DRV_Handshake hshkCtrl)
{
    DSP_STATUS        status       = DSP_SOK ;
    Uint32            dspHandshake = DSP_HANDSHAKE ;
    Uint32            i            = 0 ;
    LINKCFG_LinkDrv * linkDrv ;
    SHMDRV_Object *   shmDrvState ;
    SHMDRV_Ctrl *     ctrlPtr ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_3ENTER ("SHMDRV_handshake", dspId, linkDrvId, hshkCtrl) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Assert (SHMDRV_IsInitialized [dspId] == TRUE) ;

    shmDrvState = &(SHMDRV_State [dspId]) ;
    dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    linkDrv  = &(dspCfg->linkDrvObjects [linkDrvId]) ;
    DBC_Assert (shmDrvState->ctrlPtr != NULL) ;
    ctrlPtr     = shmDrvState->ctrlPtr ;
    DBC_Assert (ctrlPtr != NULL) ;

    switch (hshkCtrl) {
        case DRV_HandshakeSetup:
            /* Clear the handshake control fields */
            ctrlPtr->handshakeDsp = 0x0 ;
            ctrlPtr->handshakeGpp = 0x0 ;
            break ;

        case DRV_HandshakeStart:
            /* Set the GPP handshake value. */
            ctrlPtr->handshakeGpp = GPP_HANDSHAKE ;
            break ;

        case DRV_HandshakeCompl:
            /* Generate the DSP handshake value to be checked against. */
            dspHandshake |= DRV_HANDSHAKE_DRV ;
            dspHandshake |= DRV_HANDSHAKE_IPS ;
#if defined (POOL_COMPONENT)
            dspHandshake |= DRV_HANDSHAKE_POOL ;
#endif /* if defined (POOL_COMPONENT) */
#if defined (MPCS_COMPONENT)
            dspHandshake |= DRV_HANDSHAKE_MPCS ;
#endif /* if defined (MPCS_COMPONENT) */
#if defined (MPLIST_COMPONENT)
            dspHandshake |= DRV_HANDSHAKE_MPLIST ;
#endif /* if defined (MPLIST_COMPONENT) */
#if defined (MSGQ_COMPONENT)
            dspHandshake |= DRV_HANDSHAKE_MQT ;
#endif /* if defined (MSGQ_COMPONENT) */
#if defined (CHNL_COMPONENT)
            dspHandshake |= DRV_HANDSHAKE_DATA ;
#endif /* if defined (CHNL_COMPONENT) */
#if defined (NOTIFY_COMPONENT)
            dspHandshake |= DRV_HANDSHAKE_NOTIFY ;
#endif /* if defined (NOTIFY_COMPONENT) */
#if defined (RINGIO_COMPONENT)
            dspHandshake |= DRV_HANDSHAKE_RINGIO ;
#endif /* if defined (RINGIO_COMPONENT) */
            TRC_1PRINT (TRC_LEVEL4,
                        "Expected DSP handshake value: [0x%x]",
                        dspHandshake) ;

            /* Wait for DSP to write its handshake value. */
            while (   (ctrlPtr->handshakeDsp != dspHandshake)
                   &&  DSP_SUCCEEDED (status)) {
                i++ ;
                if (    (linkDrv->hshkPollCount != (Uint32) -1)
                    &&  (i == linkDrv->hshkPollCount)) {
                    status = DSP_ETIMEOUT ;
                    SET_FAILURE_REASON ;
                }
            }

            break ;
        default:
            /* Added for MISRAC compliance */
            break ;
    }

    TRC_1LEAVE ("SHMDRV_handshake", status) ;

    return status ;
}


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   SHMDRV_debug
 *
 *  @desc   This function prints the current status of the SHMDRV subcomponent.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
SHMDRV_debug (IN ProcessorId dspId, IN  Uint32 linkDrvId)
{
#if defined (TRACE_ENABLE)
    SHMDRV_Object * shmDrvState ;
#endif /* if defined (TRACE_ENABLE) */

    TRC_2ENTER ("SHMDRV_debug", dspId, linkDrvId) ;

    if (SHMDRV_IsInitialized [dspId] == TRUE) {
#if defined (TRACE_ENABLE)
        shmDrvState = &(SHMDRV_State [dspId]) ;
#endif /* if defined (TRACE_ENABLE) */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_0PRINT (TRC_LEVEL4, "SHMDRV component information:\n") ;
        TRC_1PRINT (TRC_LEVEL4, "    dspId         = [0x%x]\n", dspId) ;
        TRC_1PRINT (TRC_LEVEL4, "    ctrlPtr       = [0x%x]\n",
                                shmDrvState->ctrlPtr) ;
        TRC_0PRINT (TRC_LEVEL4, "SHMDRV Shared Memory Control Area\n") ;
        TRC_1PRINT (TRC_LEVEL4, "    handshakeGpp  = [0x%x]\n",
                                shmDrvState->ctrlPtr->handshakeGpp) ;
        TRC_1PRINT (TRC_LEVEL4, "    handshakeDsp  = [0x%x]\n",
                                shmDrvState->ctrlPtr->handshakeDsp) ;
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
    }
    else {
        TRC_2PRINT (TRC_LEVEL4,
          "SHMDRV component for DSP ID %d Link driver %d is not initialized.\n",
          dspId,
          linkDrvId) ;
    }

    TRC_0LEAVE ("SHMDRV_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* if defined (__cplusplus) */
