/** ============================================================================
 *  @file   ldrv_mqt.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/MQT/
 *
 *  @desc   Implements the interface exposed by the LDRV_MQT (Message Queue
 *          Transport) sub-component.
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
#include <procdefs.h>
#include <linkcfgdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <mem.h>

/*  ----------------------------------- Generic Functions           */
#include <gen_utils.h>

/*  ----------------------------------- Link Driver                 */
#include <cfgmap.h>
#include <ldrv_mqt.h>
#include <ldrv_smm.h>
#include <ldrv.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_MQT

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  TRC_SetReason (status, FID_C_LDRV_MQT, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ----------------------------------------------------------------------------
 *  @name   LDRV_MQT_Object
 *
 *  @desc   Defines the LDRV MQT object, which contains all the component
 *          specific information.
 *
 *  @field  mqtId
 *              Identifier of the MQT for this DSP.
 *  @field  ctrlPtr
 *              Pointer to the LDRV MQT shared memory control region.
 *  @field  dspCtrlAddr
 *              Address of the LDRV MQT shared memoyr control region in DSP
 *              address space.
 *  @field  memEntry
 *              Entry ID of the MQT shared memory control region.
 *  @field  interface
 *              Pointer to the MQT interface table for this DSP.
 *  ----------------------------------------------------------------------------
 */
typedef struct LDRV_MQT_Object_tag {
    Uint32           mqtId ;
    LDRV_MQT_Ctrl *  ctrlPtr ;
    Uint32           dspCtrlAddr ;
    Uint32           memEntry ;
    MQT_Interface *  interface ;
} LDRV_MQT_Object ;


/** ----------------------------------------------------------------------------
 *  @name   LDRV_MQT_State
 *
 *  @desc   State object for the LDRV_MQT component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC LDRV_MQT_Object LDRV_MQT_State [MAX_DSPS] ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_MQT_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool LDRV_MQT_IsInitialized [MAX_DSPS] ;

/** ============================================================================
 *  @func   LDRV_MQT_moduleInit
 *
 *  @desc   This function initializes the LDRV_MQT module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MQT_moduleInit (Void)
{
    DSP_STATUS        status = DSP_SOK ;
    Uint32            dspId ;

    TRC_0ENTER ("LDRV_MQT_moduleInit") ;

    for (dspId = 0 ; (dspId < MAX_DSPS) && DSP_SUCCEEDED (status) ; dspId++) {
        /* Initialize the IsInitialized flag for the DSP. */
        LDRV_MQT_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_MQT_moduleInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MQT_moduleExit
 *
 *  @desc   This function finalizes the LDRV_MQT module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MQT_moduleExit (Void)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    Uint32     dspId ;

    TRC_0ENTER ("LDRV_MQT_moduleExit") ;

    /* Finalize the IsInitialized flags for all DSPs. */
    for (dspId = 0 ; dspId < MAX_DSPS ; dspId++) {
        if (LDRV_MQT_IsInitialized [dspId] == TRUE) {
            tmpStatus = LDRV_MQT_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("LDRV_MQT_moduleExit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MQT_init
 *
 *  @desc   This function initializes the LDRV_MQT component.
 *
 *  @modif  LDRV_MQT_State
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MQT_init (IN ProcessorId dspId, OUT Uint32 * shDspAddr)
{
    DSP_STATUS          status = DSP_SOK ;
    LDRV_MQT_Object *   mqtState ;
    LINKCFG_DspConfig * dspCfg ;
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_LinkDrv *   linkDrv ;
    LINKCFG_Mqt *       mqtObject ;
    Int32               strCmpResult ;
    Uint32              mapId ;

    TRC_2ENTER("LDRV_MQT_init", dspId, shDspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (shDspAddr != NULL) ;

    if (LDRV_MQT_IsInitialized [dspId] == FALSE) {
        /*  --------------------------------------------------------------------
         * Validate the setup configuration parameters for the link driver.
         *  --------------------------------------------------------------------
         */
        mqtState = &(LDRV_MQT_State [dspId]) ;

        dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj    = dspCfg->dspObject ;
        linkDrv   = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
        mqtObject = &(dspCfg->mqtObjects [linkDrv->mqtId]) ;

        /* Initialize the state object. */
        mqtState->mqtId       = linkDrv->mqtId ;
        mqtState->ctrlPtr     = NULL ;
        mqtState->dspCtrlAddr = (Uint32) NULL ;
        mqtState->memEntry    = mqtObject->memEntry ;
        mqtState->interface   = NULL ;

        /* Check if the configured MQT is available and set the interface table
         * if the configuration is valid.
         */
        for (mapId = 0 ; mapId < CFGMAP_Config [dspId]->numMqts ; mapId++) {
            status = GEN_Strcmp (mqtObject->name,
                                 CFGMAP_Config [dspId]->mqtObjects [mapId].name,
                                 &strCmpResult) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            if (strCmpResult == 0) {
                mqtState->interface =
                           CFGMAP_Config [dspId]->mqtObjects [mapId].interface ;
                break ;
            }
        }

        if (mapId == CFGMAP_Config [dspId]->numMqts) {
            /* Configured MQT is not available. */
            PRINT_Printf ("<1>Configuration error:"
                          " Incorrect MQT name specified [%s]\n",
                          mqtObject->name) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else {
            /* Check if the memEntry is in valid range. */
            if (    (mqtObject->memEntry != (Uint32) -1)
                &&  (mqtObject->memEntry >= dspObj->memEntries)) {
                PRINT_Printf ("<1>Configuration error:"
                              " Incorrect MQT memEntry specified [0x%x]\n",
                              mqtObject->memEntry) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
            else if (    (mqtObject->ipsId != (Uint32) -1)
                     &&  (mqtObject->ipsId >= linkDrv->numIpsEntries)) {
                /* Check if the ipsId is in valid range. */
                PRINT_Printf ("<1>Configuration error:"
                              " Incorrect MQT ipsId specified [0x%x]\n",
                              mqtObject->ipsId) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
            else if (    (mqtObject->ipsId      != (Uint32) -1)
                     &&  (mqtObject->ipsEventNo != (Uint32) -1)
                     &&  (   mqtObject->ipsEventNo
                          >= dspCfg->ipsTables [linkDrv->ipsTableId]
                                             [mqtObject->ipsId].numIpsEvents)) {
                /* Check if the ipsEventNo is in valid range. */
                PRINT_Printf ("<1>Configuration error:"
                              " Incorrect MQT ipsEventNo specified [0x%x]\n",
                              mqtObject->ipsEventNo) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            /*  ----------------------------------------------------------------
             *  Allocate shared memory required for the LDRV_MQT component.
             *  ----------------------------------------------------------------
             */
            status = LDRV_SMM_alloc (dspId,
                                     mqtState->memEntry,
                                     NULL, /* Physical address not required.  */
                                     &(mqtState->dspCtrlAddr),
                                     (Uint32 *) ((Pvoid)&(mqtState->ctrlPtr)),
                                     DSPLINK_ALIGN (sizeof (LDRV_MQT_Ctrl),
                                                    DSPLINK_BUF_ALIGN)) ;
            if (DSP_SUCCEEDED (status)) {
                *shDspAddr = mqtState->dspCtrlAddr ;
                TRC_1PRINT (TRC_LEVEL4,
                            "LDRV_MQT Ctrl GPP address [0x%x]\n",
                            mqtState->ctrlPtr) ;
                TRC_1PRINT (TRC_LEVEL4,
                            "LDRV_MQT Ctrl DSP address [0x%x]\n",
                            mqtState->dspCtrlAddr) ;
                TRC_1PRINT (TRC_LEVEL4,
                            "LDRV_MQT Ctrl size        [0x%x]\n",
                            DSPLINK_ALIGN (sizeof (LDRV_MQT_Ctrl),
                                           DSPLINK_BUF_ALIGN)) ;

                /*  ------------------------------------------------------------
                 *  Write shared configuration information into the ctrl
                 *  structure.
                 *  ------------------------------------------------------------
                 */
                mqtState->ctrlPtr->config.maxMsgSize = mqtObject->maxMsgSize ;
                mqtState->ctrlPtr->config.ipsId      = mqtObject->ipsId ;
                mqtState->ctrlPtr->config.ipsEventNo = mqtObject->ipsEventNo ;
                mqtState->ctrlPtr->config.arg1       = mqtObject->arg1 ;
                mqtState->ctrlPtr->config.arg2       = mqtObject->arg2 ;

            }
            else {
                SET_FAILURE_REASON ;
            }

            if (DSP_SUCCEEDED (status)) {
                /*  ------------------------------------------------------------
                 *  Initialize the MQT towards the DSP and get the address of
                 *  its shared memory control region in DSP address space.
                 *  There can be only a single MQT towards the DSP at a time,
                 *  so the LDRV_MQT component does not require its own shared
                 *  memory area.
                 *  ------------------------------------------------------------
                 */
                status = (mqtState->interface->init) (
                                                dspId,
                                                mqtState->mqtId,
                                                &(mqtState->ctrlPtr->dspAddr)) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }

            LDRV_MQT_IsInitialized [dspId] = TRUE ;
            if (DSP_FAILED (status)) {
                /* LDRV_MQT_exit executes cleanup only if the initialized flag
                 * is TRUE.
                 */
                LDRV_MQT_exit (dspId) ;
                LDRV_MQT_IsInitialized [dspId] = FALSE ;
            }
        }
    }

    TRC_1LEAVE ("LDRV_MQT_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MQT_exit
 *
 *  @desc   This function finalizes the LDRV_MQT component.
 *
 *  @modif  LDRV_MQT_State
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MQT_exit (IN ProcessorId dspId)
{
    DSP_STATUS        status    = DSP_SOK ;
    DSP_STATUS        tmpStatus = DSP_SOK ;
    LDRV_MQT_Object * mqtState ;

    TRC_1ENTER ("LDRV_MQT_exit", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (LDRV_MQT_IsInitialized [dspId] == TRUE) {
        mqtState  = &(LDRV_MQT_State [dspId]) ;

        if (mqtState->interface != NULL) {
            /*  ----------------------------------------------------------------
             *  Finalize the MQT toward the DSP.
             *  ----------------------------------------------------------------
             */
            status = (mqtState->interface->exit) (dspId, mqtState->mqtId) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        if (mqtState->ctrlPtr != NULL) {
            /*  ----------------------------------------------------------------
             *  Free shared memory allocated for the LDRV_MQT component.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_SMM_free (dspId,
                                       mqtState->memEntry,
                                       DSPLINK_ALIGN (sizeof (LDRV_MQT_Ctrl),
                                                      DSPLINK_BUF_ALIGN)) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        LDRV_MQT_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_MQT_exit", status) ;

    return status ;
}


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_MQT_debug
 *
 *  @desc   Prints the current status of this subcomponent.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_MQT_debug (IN  ProcessorId dspId)
{
#if defined (TRACE_ENABLE)
    LDRV_MQT_Object * mqtState ;
#endif /* if defined (TRACE_ENABLE) */

    TRC_1ENTER ("LDRV_SMM_debug", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (LDRV_MQT_IsInitialized [dspId] == TRUE) {
#if defined (TRACE_ENABLE)
        mqtState = &(LDRV_MQT_State [dspId]) ;
#endif /* if defined (TRACE_ENABLE) */
        /*  --------------------------------------------------------------------
         *  Print the LDRV MQT component state information.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_1PRINT (TRC_LEVEL4,
                    "LDRV MQT component state information for DSP ID %d:\n",
                    dspId) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    mqtId            = [0x%x].\n",
                    mqtState->mqtId) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ctrlPtr          = [0x%x].\n",
                    mqtState->ctrlPtr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dspCtrlAddr      = [0x%x].\n",
                    mqtState->dspCtrlAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    memEntry         = [0x%x].\n",
                    mqtState->memEntry) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    interface        = [0x%x].\n",
                    mqtState->interface) ;

        TRC_0PRINT (TRC_LEVEL4, "Shared memory region information:\n") ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dspAddr           = [0x%x]\n",
                    mqtState->ctrlPtr->dspAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    config.maxMsgSize = [0x%x]\n",
                    mqtState->ctrlPtr->config.maxMsgSize) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    config.ipsId      = [0x%x]\n",
                    mqtState->ctrlPtr->config.ipsId) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    config.ipsEventNo = [0x%x]\n",
                    mqtState->ctrlPtr->config.ipsEventNo) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    config.arg1       = [0x%x]\n",
                    mqtState->ctrlPtr->config.arg1) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    config.arg2       = [0x%x]\n",
                    mqtState->ctrlPtr->config.arg2) ;
    }
    else {
        TRC_1PRINT (TRC_LEVEL4,
                    "LDRV_MQT component for DSP ID %d is not initialized.\n",
                    dspId) ;
    }

    TRC_0LEAVE ("LDRV_MQT_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif
