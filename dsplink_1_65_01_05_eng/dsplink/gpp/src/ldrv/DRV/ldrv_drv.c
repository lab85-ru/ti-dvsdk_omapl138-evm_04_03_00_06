/** ============================================================================
 *  @file   ldrv_drv.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/DRV/
 *
 *  @desc   Implements the interface exposed by LDRV_DRV subcomponent.
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
#include <version.h>
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Generic Functions           */
#include <gen_utils.h>

/*  ----------------------------------- Processor Manager           */
#include <pmgr_proc.h>

/*  ----------------------------------- Link Driver                 */
#include <cfgmap.h>
#include <ldrv.h>
#include <ldrv_drv.h>
#include <ldrv_proc.h>
#include <ldrv_smm.h>

#include <ldrv_ips.h>

#if defined (POOL_COMPONENT)
#include <ldrv_pool.h>
#endif /* if defined (POOL_COMPONENT) */
#if defined (MPCS_COMPONENT)
#include <ldrv_mpcs.h>
#endif /* if defined (MPCS_COMPONENT) */
#if defined (MPLIST_COMPONENT)
#include <ldrv_mplist.h>
#endif /* if defined (MPLIST_COMPONENT) */
#if defined (RINGIO_COMPONENT)
#include <ldrv_ringio.h>
#endif /* if defined (RINGIO_COMPONENT) */
#if defined (CHNL_COMPONENT)
#include <ldrv_data.h>
#endif /* if defined (CHNL_COMPONENT) */
#if defined (MSGQ_COMPONENT)
#include <ldrv_mqt.h>
#endif /* if defined (MSGQ_COMPONENT) */

#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_DRV

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  TRC_SetReason (status, FID_C_LDRV_DRV, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_DRV_Object
 *
 *  @desc   This structure defines the LDRV DRV object, which contains all state
 *          information required by the LDRV DRV component.
 *
 *  @field  linkDrvId
 *              Link driver identifier.
 *  @field  ctrlPtr
 *              Pointer to the LDRV DRV shared memory control region.
 *  @field  dspCtrlAddr
 *              Address of the LDRV DRV shared memory control region in DSP
 *              address space.
 *  @field  interface
 *              Pointer to the link driver interface table for this DSP.
 *  ----------------------------------------------------------------------------
 */
typedef struct LDRV_DRV_Object_tag {
    Uint32           linkDrvId ;
    LDRV_DRV_Ctrl *  ctrlPtr ;
    Uint32           dspCtrlAddr ;
    DRV_Interface *  interface ;
} LDRV_DRV_Object ;


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_DRV_State
 *
 *  @desc   State object for the LDRV_DRV component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC LDRV_DRV_Object LDRV_DRV_State [MAX_DSPS] ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_DRV_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool LDRV_DRV_IsInitialized [MAX_DSPS] ;


/** ============================================================================
 *  @func   LDRV_DRV_moduleInit
 *
 *  @desc   This function initializes the LDRV_DRV module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DRV_moduleInit (Void)
{
    DSP_STATUS        status = DSP_SOK ;
    Uint32                  dspId ;

    TRC_0ENTER ("LDRV_DRV_moduleInit") ;

    for (dspId = 0 ; (dspId < MAX_DSPS) && DSP_SUCCEEDED (status) ; dspId++) {
        /* Initialize the IsInitialized flag for the DSP. */
        LDRV_DRV_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_DRV_moduleInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_DRV_moduleExit
 *
 *  @desc   This function finalizes the LDRV_DRV module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DRV_moduleExit (Void)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    Uint32     dspId ;

    TRC_0ENTER ("LDRV_DRV_moduleExit") ;

    /* Finalize the IsInitialized flags for all DSPs. */
    for (dspId = 0 ; dspId < MAX_DSPS ; dspId++) {
        if (LDRV_DRV_IsInitialized [dspId] == TRUE) {
            tmpStatus = LDRV_DRV_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("LDRV_DRV_moduleExit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_DRV_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DRV_init (IN ProcessorId dspId)
{
    DSP_STATUS              status  = DSP_SOK ;
    LDRV_DRV_Ctrl *         drvCtrl = NULL ;
    LINKCFG_DspConfig *     dspCfg ;
    LINKCFG_Dsp *           dspObj ;
    LINKCFG_LinkDrv *       linkDrv ;
    LDRV_DRV_Object *       drvState ;
    Int32                   strCmpResult ;
    Uint32                  mapId ;

    TRC_1ENTER ("LDRV_DRV_init", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    drvState = &(LDRV_DRV_State [dspId]) ;
    dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj   = dspCfg->dspObject ;
    linkDrv  = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;

    if (LDRV_DRV_IsInitialized [dspId] == FALSE) {
        /*  --------------------------------------------------------------------
         * Validate the setup configuration parameters for the link driver.
         *  --------------------------------------------------------------------
         */
        drvState = &(LDRV_DRV_State [dspId]) ;

        /* Initialize the state object. */
        drvState->linkDrvId   = dspObj->linkDrvId ;
        drvState->ctrlPtr     = NULL ;
        drvState->dspCtrlAddr = (Uint32) NULL ;
        drvState->interface   = NULL ;

        /* Check if the configured link driver is available and set the
         * interface table if the configuration is valid.
         */
        for (mapId = 0 ; mapId < CFGMAP_Config [dspId]->numLinkDrvs ; mapId++) {
            status = GEN_Strcmp (
                     dspCfg->linkDrvObjects [drvState->linkDrvId].name,
                     CFGMAP_Config [dspId]->linkDrvObjects [mapId].name,
                     &strCmpResult) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            if (strCmpResult == 0) {
                drvState->interface =
                       CFGMAP_Config [dspId]->linkDrvObjects [mapId].interface ;
                break ;
            }
        }

        if (mapId == CFGMAP_Config [dspId]->numLinkDrvs) {
            /* Configured link driver is not available. */
            PRINT_Printf ("<1>Configuration error:"
                   " Incorrect link driver name specified [%s]\n",
                   dspCfg->linkDrvObjects [drvState->linkDrvId].name) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else {
            /* Check if the memEntry is in valid range. */
            if (    (linkDrv->memEntry != (Uint32) -1)
                &&  (linkDrv->memEntry >= dspObj->memEntries)) {
                PRINT_Printf ("<1>Configuration error:"
                           " Incorrect link driver memEntry specified [0x%x]\n",
                           linkDrv->memEntry) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
            else if (linkDrv->ipsTableId >= dspCfg->numIpsTables) {
                /* Check if the ipsTableId is in valid range. */
                PRINT_Printf ("<1>Configuration error:"
                         " Incorrect link driver ipsTableId specified [0x%x]\n",
                         linkDrv->ipsTableId) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
#if defined (POOL_COMPONENT)
            else if (linkDrv->poolTableId >= dspCfg->numPoolTables) {
                /* Check if the poolTableId is in valid range. */
                PRINT_Printf ("<1>Configuration error:"
                        " Incorrect link driver poolTableId specified [0x%x]\n",
                        linkDrv->poolTableId) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
#endif /* if defined (POOL_COMPONENT) */
#if defined (CHNL_COMPONENT)
            else if (linkDrv->dataTableId >= dspCfg->numDataTables) {
                /* Check if the dataTableId is in valid range. */
                PRINT_Printf ("<1>Configuration error:"
                        " Incorrect link driver dataTableId specified [0x%x]\n",
                        linkDrv->dataTableId) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
#endif /* if defined (CHNL_COMPONENT) */
#if defined (MSGQ_COMPONENT)
            else if (linkDrv->mqtId >= dspCfg->numMqts) {
                /* Check if the mqtId is in valid range. */
                PRINT_Printf ("<1>Configuration error:"
                              " Incorrect link driver mqtId specified [0x%x]\n",
                              linkDrv->mqtId) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
#endif /* if defined (MSGQ_COMPONENT) */
#if defined (RINGIO_COMPONENT)
            else if (linkDrv->ringIoTableId >= dspCfg->numRingIo) {
                /* Check if the ringIoTableId is in valid range. */
                PRINT_Printf ("<1>Configuration error:"
                      " Incorrect link driver ringIoTableId specified [0x%x]\n",
                      linkDrv->ringIoTableId) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
#endif /* if defined (RINGIO_COMPONENT) */
#if defined (MPLIST_COMPONENT)
            else if (linkDrv->mplistTableId >= dspCfg->numMpList) {
                /* Check if the mplistTableId is in valid range. */
                PRINT_Printf ("<1>Configuration error:"
                      " Incorrect link driver mplistTableId specified [0x%x]\n",
                      linkDrv->mplistTableId) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
#endif /* if defined (MPLIST_COMPONENT) */
#if defined (MPCS_COMPONENT)
            else if (linkDrv->mpcsTableId >= dspCfg->numMpcs) {
                /* Check if the mpcsTableId is in valid range. */
                PRINT_Printf ("<1>Configuration error:"
                        " Incorrect link driver mpcsTableId specified [0x%x]\n",
                        linkDrv->mpcsTableId) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
#endif /* if defined (MPCS_COMPONENT) */
        }

        /*  --------------------------------------------------------------------
         *  Initialize the Shared Memory Manager component.
         *  --------------------------------------------------------------------
         */
        if (DSP_SUCCEEDED (status)) {
            status = LDRV_SMM_init (dspId) ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            /*  ----------------------------------------------------------------
             *  Allocate shared memory required for the LDRV_DRV component.
             *  ----------------------------------------------------------------
             */
            status = LDRV_SMM_alloc (dspId,
                                     linkDrv->memEntry,
                                     NULL, /* Physical address not required. */
                                     &(drvState->dspCtrlAddr),
                                     (Uint32 *) ((Pvoid) &(drvState->ctrlPtr)),
                                     sizeof (LDRV_DRV_Ctrl)) ;
            if (DSP_SUCCEEDED (status)) {
                TRC_1PRINT (TRC_LEVEL4,
                            "LDRV_DRV Ctrl GPP address [0x%x]\n",
                            drvState->ctrlPtr) ;
                TRC_1PRINT (TRC_LEVEL4,
                            "LDRV_DRV Ctrl DSP address [0x%x]\n",
                            drvState->dspCtrlAddr) ;
                TRC_1PRINT (TRC_LEVEL4,
                            "LDRV_DRV Ctrl size        [0x%x]\n",
                            sizeof (LDRV_DRV_Ctrl)) ;

                drvCtrl = drvState->ctrlPtr ;
                DBC_Assert (drvCtrl != NULL) ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            /*  ----------------------------------------------------------------
             *  Initialize all fields of the shared memory control structure.
             *  ----------------------------------------------------------------
             */
            status = GEN_Strcpyn (drvCtrl->version,
                                  DSPLINK_VERSION,
                                  DSP_MAX_STRLEN) ;
            if (DSP_SUCCEEDED (status)) {
                drvCtrl->linkAddr          = 0 ;
                drvCtrl->drvDspInitDone    = (Uint32) -1 ;
                drvCtrl->ipsAddr           = 0 ;
                drvCtrl->ipsDspInitDone    = (Uint32) -1 ;
                drvCtrl->poolAddr          = 0 ;
                drvCtrl->poolConfigured    = 0 ;
                drvCtrl->poolDspInitDone   = (Uint32) -1 ;
                drvCtrl->mpcsAddr          = 0 ;
                drvCtrl->mpcsConfigured    = 0 ;
                drvCtrl->mpcsDspInitDone   = (Uint32) -1 ;
                drvCtrl->mplistAddr        = 0 ;
                drvCtrl->mplistConfigured  = 0 ;
                drvCtrl->mplistDspInitDone = (Uint32) -1 ;
                drvCtrl->mqtAddr           = 0 ;
                drvCtrl->mqtConfigured     = 0 ;
                drvCtrl->mqtDspInitDone    = (Uint32) -1 ;
                drvCtrl->dataAddr          = 0 ;
                drvCtrl->dataConfigured    = 0 ;
                drvCtrl->dataDspInitDone   = (Uint32) -1 ;
                drvCtrl->ringIoAddr        = 0 ;
                drvCtrl->ringIoConfigured  = 0 ;
                drvCtrl->ringIoDspInitDone = (Uint32) -1 ;

                drvCtrl->config.numIpsEntries  = linkDrv->numIpsEntries ;
                drvCtrl->config.numPools       = linkDrv->numPools ;
                drvCtrl->config.numDataDrivers = linkDrv->numDataDrivers ;
                drvCtrl->config.cpuFreq        = dspObj->cpuFreq ;

            }
            else {
                SET_FAILURE_REASON ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            /*  ----------------------------------------------------------------
             *  Initialize the link towards the DSP and get the address of its
             *  shared memory control region in DSP address space.
             *  ----------------------------------------------------------------
             */
            status = (drvState->interface->init) (dspId,
                                                  drvState->linkDrvId,
                                                  &(drvCtrl->linkAddr)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            /*  ----------------------------------------------------------------
             *  Initialize the IPS component and get the address of its
             *  shared memory control region in DSP address space.
             *  ----------------------------------------------------------------
             */
            status = LDRV_IPS_init (dspId, &(drvCtrl->ipsAddr)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

#if defined (POOL_COMPONENT)
        if (DSP_SUCCEEDED (status)) {
            /*  ----------------------------------------------------------------
             *  Initialize the POOL component and get the address of its shared
             *  memory control region in DSP address space.
             *  ----------------------------------------------------------------
             */
            status = LDRV_POOL_init (dspId, &(drvCtrl->poolAddr)) ;
            if (DSP_SUCCEEDED (status)) {
                drvCtrl->poolConfigured = (Uint32) TRUE ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
#endif /* if defined (POOL_COMPONENT) */

#if defined (MPCS_COMPONENT)
        if (DSP_SUCCEEDED (status)) {
            /*  ----------------------------------------------------------------
             *  Initialize the MPCS component and get the address of its shared
             *  memory control region in DSP address space.
             *  ----------------------------------------------------------------
             */
            status = LDRV_MPCS_init (dspId, &(drvCtrl->mpcsAddr)) ;
            if (DSP_SUCCEEDED (status)) {
                drvCtrl->mpcsConfigured = (Uint32) TRUE ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
#endif /* if defined (MPCS_COMPONENT) */

#if defined (MPLIST_COMPONENT)
        if (DSP_SUCCEEDED (status)) {
            /*  ----------------------------------------------------------------
             *  Initialize the MPLIST component and get the address of its
             *  shared memory control region in DSP address space.
             *  ----------------------------------------------------------------
             */
            status = LDRV_MPLIST_init (dspId, &(drvCtrl->mplistAddr)) ;
            if (DSP_SUCCEEDED (status)) {
                drvCtrl->mplistConfigured = (Uint32) TRUE ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (MSGQ_COMPONENT)
        if (DSP_SUCCEEDED (status)) {
            /*  ----------------------------------------------------------------
             *  Initialize the MQT component and get the address of its shared
             *  memory control region in DSP address space.
             *  ----------------------------------------------------------------
             */
            status = LDRV_MQT_init (dspId, &(drvCtrl->mqtAddr)) ;
            if (DSP_SUCCEEDED (status)) {
                drvCtrl->mqtConfigured = (Uint32) TRUE ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
#endif /* if defined (MSGQ_COMPONENT) */

#if defined (CHNL_COMPONENT)
        if (DSP_SUCCEEDED (status)) {
            /*  ----------------------------------------------------------------
             *  Initialize the DATA component and get the address of its shared
             *  memory control region in DSP address space.
             *  ----------------------------------------------------------------
             */
            status = LDRV_DATA_init (dspId, &(drvCtrl->dataAddr)) ;
            if (DSP_SUCCEEDED (status)) {
                drvCtrl->dataConfigured = (Uint32) TRUE ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
#endif /* if defined (CHNL_COMPONENT) */

#if defined (RINGIO_COMPONENT)
        if (DSP_SUCCEEDED (status)) {
            /*  ----------------------------------------------------------------
             *  Initialize the RINGIO component and get the address of its
             *  shared memory control region in DSP address space.
             *  ----------------------------------------------------------------
             */
            status = LDRV_RINGIO_init (dspId, &(drvCtrl->ringIoAddr)) ;
            if (DSP_SUCCEEDED (status)) {
                drvCtrl->ringIoConfigured = (Uint32) TRUE ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
#endif /* if defined (RINGIO_COMPONENT) */

        LDRV_DRV_IsInitialized [dspId] = TRUE ;
        if (DSP_FAILED (status)) {
            /* LDRV_DRV_exit executes cleanup only if the initialized flag is
             * TRUE.
             */
            LDRV_DRV_exit (dspId) ;
            LDRV_DRV_IsInitialized [dspId] = FALSE ;
        }
    }

    TRC_1LEAVE ("LDRV_DRV_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_DRV_exit
 *
 *  @desc   This function finalizes the LDRV DRV module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DRV_exit (IN ProcessorId dspId)
{
    DSP_STATUS        status    = DSP_SOK ;
    DSP_STATUS        tmpStatus = DSP_SOK ;
    LDRV_DRV_Object * drvState ;
    LINKCFG_Dsp *     dspObj ;
    LINKCFG_LinkDrv * linkDrv ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_1ENTER ("LDRV_DRV_exit", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (LDRV_DRV_IsInitialized [dspId] == TRUE) {
        if (LDRV_LinkCfgPtr != NULL) {
            dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
            drvState = &(LDRV_DRV_State [dspId]) ;
            dspObj   = dspCfg->dspObject ;
            linkDrv  = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;

#if defined (RINGIO_COMPONENT)
            /*  ----------------------------------------------------------------
             *  Finalize the RINGIO component.
             *  ----------------------------------------------------------------
             */
            status = LDRV_RINGIO_exit (dspId) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
#endif /* if defined (RINGIO_COMPONENT) */

#if defined (CHNL_COMPONENT)
            /*  ----------------------------------------------------------------
             *  Finalize the DATA component.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_DATA_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
#endif /* if defined (CHNL_COMPONENT) */

#if defined (MSGQ_COMPONENT)
            /*  ----------------------------------------------------------------
             *  Finalize the MQT component.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_MQT_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
#endif /* if defined (MSGQ_COMPONENT) */

#if defined (MPLIST_COMPONENT)
            /*  ----------------------------------------------------------------
             *  Finalize the MPLIST component.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_MPLIST_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (MPCS_COMPONENT)
            /*  ----------------------------------------------------------------
             *  Finalize the MPCS component.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_MPCS_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
#endif /* if defined (MPCS_COMPONENT) */

#if defined (POOL_COMPONENT)
            /*  ----------------------------------------------------------------
             *  Finalize the POOL component.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_POOL_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
#endif /* if defined (POOL_COMPONENT) */

            /*  ----------------------------------------------------------------
             *  Finalize the IPS component.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_IPS_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            /*  ----------------------------------------------------------------
             *  Finalize the link toward the DSP.
             *  ----------------------------------------------------------------
             */
            if (drvState->interface != NULL) {
                tmpStatus = (drvState->interface->exit) (dspId,
                                                         drvState->linkDrvId) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }

            /*  ----------------------------------------------------------------
             *  Free shared memory allocated for the LDRV_DRV component.
             *  ----------------------------------------------------------------
             */
            if (drvState->ctrlPtr != NULL) {
                tmpStatus = LDRV_SMM_free (dspId,
                                           linkDrv->memEntry,
                                           sizeof (LDRV_DRV_Ctrl)) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }

            /*  ----------------------------------------------------------------
             *  Finalize the Shared Memory Manager component.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_SMM_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        LDRV_DRV_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_DRV_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_DRV_handshake
 *
 *  @desc   Does the necessary handshake (if required) between the drivers
 *          on the GPP & DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DRV_handshake (IN ProcessorId dspId, IN DRV_Handshake hshkCtrl)
{
    DSP_STATUS        status    = DSP_SOK ;
    LDRV_DRV_Object * drvState ;
    Uint32            dspShmBase ;
    LDRV_DRV_Ctrl *   drvCtrl ;
    Int32             strCmpResult ;

    TRC_2ENTER ("LDRV_DRV_handshake", dspId, hshkCtrl) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Assert (LDRV_DRV_IsInitialized [dspId] == TRUE) ;

    drvState = &(LDRV_DRV_State [dspId]) ;
    drvCtrl  = drvState->ctrlPtr ;

    if (hshkCtrl == DRV_HandshakeSetup) {
        /* Set all DSP-side modules to uninitialized state. */
        drvCtrl->drvDspInitDone    = (Uint32) -1 ;
        drvCtrl->ipsDspInitDone    = (Uint32) -1 ;
#if defined (POOL_COMPONENT)
        drvCtrl->poolDspInitDone   = (Uint32) -1 ;
#endif /* if defined (POOL_COMPONENT) */
#if defined (MPCS_COMPONENT)
        drvCtrl->mpcsDspInitDone   = (Uint32) -1 ;
#endif /* if defined (MPCS_COMPONENT) */
#if defined (MPLIST_COMPONENT)
        drvCtrl->mplistDspInitDone = (Uint32) -1 ;
#endif /* if defined (MPLIST_COMPONENT) */
#if defined (MSGQ_COMPONENT)
        drvCtrl->mqtDspInitDone    = (Uint32) -1 ;
#endif /* if defined (MSGQ_COMPONENT) */
#if defined (CHNL_COMPONENT)
        drvCtrl->dataDspInitDone   = (Uint32) -1 ;
#endif /* if defined (CHNL_COMPONENT) */
#if defined (RINGIO_COMPONENT)
        drvCtrl->ringIoDspInitDone = (Uint32) -1 ;
#endif /* if defined (RINGIO_COMPONENT) */

        /*  --------------------------------------------------------------------
         *  Get the DSP address of location where the value of shared memory
         *  start location is to be written and write the value at this location
         *  --------------------------------------------------------------------
         */
        status = PMGR_PROC_getSymbolAddress (dspId,
                                             DRV_SHMBASESYMBOL,
                                             &dspShmBase) ;
        if (DSP_SUCCEEDED (status)) {
            TRC_1PRINT (TRC_LEVEL4,
                        "DRV shared memory base symbol DSP address [0x%x]n",
                        dspShmBase) ;
            status = LDRV_PROC_write (
                                 dspId,
                                 dspShmBase,
                                 Endianism_Default,
                                 sizeof (Uint32),
                                 (Uint8 *) ((Pvoid) &(drvState->dspCtrlAddr))) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
                PRINT_Printf (
                      "Error: Failed to write shared memory base address into"
                      " DSP-side symbol: .data:DSPLINK_shmBaseAddress\n"
                      "Ensure DSP-side memory map (TCF/MAP) matches with "
                      "/dsplink/config/all/CFG_<PLATFORM>.c\n") ;
            }
        }
        else {
            /* If symbol table is stripped, allow starting DSP executable. This
             * assumes that DSP-side application has used SECTIONS directive to
             * set the value of the shm base symbol to shared memory start
             * address in its linker command file.
             * For example:
             * SECTIONS {
             *     .data:DSPLINK_shmBaseAddress: fill=0x8FE05000 {} > DDR
             * }
             */
            if ((status == DSP_ENOTFOUND) || (status == DSP_ENOTSUPPORTED)) {
                status = DSP_SOK ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
    }

    if (DSP_SUCCEEDED (status)) {
        /*  --------------------------------------------------------------------
         *  Handshake with the DSP
         *  --------------------------------------------------------------------
         */
        status = (drvState->interface->handshake) (dspId,
                                                   drvState->linkDrvId,
                                                   hshkCtrl) ;
        if (DSP_FAILED (status)) {
            TRC_1PRINT (TRC_LEVEL4,
                        "    Driver handshake with DSP failed [0x%x]:\n",
                        status) ;
            SET_FAILURE_REASON ;
        }
    }

    if (hshkCtrl == DRV_HandshakeCompl) {
        /* Check whether the DSPLINK version on the DSP-side matches the
         * expected version on the GPP-side.
         */
        DBC_Assert (drvCtrl != NULL) ;


        GEN_Strcmpn (drvCtrl->version,
                     DSPLINK_VERSION,
                     NUM_CHARS_VERSION - 1,
                     &strCmpResult) ;
        if (strCmpResult != 0) {
            status = DSP_EVERSION ;
            SET_FAILURE_REASON ;
            PRINT_Printf ("Version mismatch between GPP and DSP-side\n") ;
            PRINT_Printf ("    GPP-side version    [%s]:\n",
                          DSPLINK_VERSION) ;
            PRINT_Printf ("    DSP-side version    [%s]:\n",
                          drvCtrl->version) ;
        }
        /* Check whether the DSP configuration matches the GPP expectations
         * and was successful.
         */
        else if (    (drvCtrl->drvDspInitDone    != 0)
                 ||  (drvCtrl->procId            != dspId)
                 ||  (drvCtrl->ipsDspInitDone    != 0)
#if defined (POOL_COMPONENT)
                 ||  (drvCtrl->poolDspInitDone   != 0)
#endif /* if defined (POOL_COMPONENT) */
#if defined (MPCS_COMPONENT)
                 ||  (drvCtrl->mpcsDspInitDone   != 0)
#endif /* if defined (MPCS_COMPONENT) */
#if defined (MPLIST_COMPONENT)
                 ||  (drvCtrl->mplistDspInitDone != 0)
#endif /* if defined (MPLIST_COMPONENT) */
#if defined (MSGQ_COMPONENT)
                 ||  (drvCtrl->mqtDspInitDone    != 0)
#endif /* if defined (MSGQ_COMPONENT) */
#if defined (CHNL_COMPONENT)
                 ||  (drvCtrl->dataDspInitDone   != 0)
#endif /* if defined (CHNL_COMPONENT) */
#if defined (RINGIO_COMPONENT)
                 ||  (drvCtrl->ringIoDspInitDone != 0)
#endif /* if defined (RINGIO_COMPONENT) */
            ) {
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;

            PRINT_Printf ("DSP-side configuration mismatch/failure\n") ;
            PRINT_Printf ("0              -> success\n") ;
            PRINT_Printf ("Positive value -> DSP-side failure code.\n") ;
            PRINT_Printf ("(Uint32) -1    -> DSP-side component was not"
                          " initialized.\n\n") ;
            PRINT_Printf ("    ProcId received : %d, Expected : %d\n",
                          drvCtrl->procId,
                          dspId) ;
            PRINT_Printf ("    DRV configuration status    [0x%x]\n",
                          drvCtrl->drvDspInitDone) ;
            PRINT_Printf ("    IPS configuration status    [0x%x]\n",
                          drvCtrl->ipsDspInitDone) ;
#if defined (POOL_COMPONENT)
            PRINT_Printf ("    POOL configuration status   [0x%x]\n",
                          drvCtrl->poolDspInitDone) ;
#endif /* if defined (POOL_COMPONENT) */
#if defined (MPCS_COMPONENT)
            PRINT_Printf ("    MPCS configuration status   [0x%x]\n",
                          drvCtrl->mpcsDspInitDone) ;
#endif /* if defined (MPCS_COMPONENT) */
#if defined (MPLIST_COMPONENT)
            PRINT_Printf ("    MPLIST configuration status [0x%x]\n",
                          drvCtrl->mplistDspInitDone) ;
#endif /* if defined (MPLIST_COMPONENT) */
#if defined (MSGQ_COMPONENT)
            PRINT_Printf ("    MQT configuration status    [0x%x]\n",
                          drvCtrl->mqtDspInitDone) ;
#endif /* if defined (MSGQ_COMPONENT) */
#if defined (CHNL_COMPONENT)
            PRINT_Printf ("    DATA configuration status   [0x%x]\n",
                          drvCtrl->dataDspInitDone) ;
#endif /* if defined (CHNL_COMPONENT) */
#if defined (RINGIO_COMPONENT)
            PRINT_Printf ("    RINGIO configuration status [0x%x]\n",
                          drvCtrl->ringIoDspInitDone) ;
#endif /* if defined (RINGIO_COMPONENT) */
        }
        else {
        }
    }

    TRC_1LEAVE ("LDRV_DRV_handshake", status) ;

    return status ;
}


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_DRV_debug
 *
 *  @desc   Prints the current status of this subcomponent.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_DRV_debug (IN  ProcessorId dspId)
{
    LDRV_DRV_Object * drvState ;

    TRC_1ENTER ("LDRV_DRV_debug", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    drvState = &(LDRV_DRV_State [dspId]) ;

    if (LDRV_DRV_IsInitialized [dspId] == TRUE) {
        /*  --------------------------------------------------------------------
         *  Print the LDRV DRV component state information.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_1PRINT (TRC_LEVEL4,
                    "LDRV DRV component state information for DSP ID %d:\n",
                    dspId) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ctrlPtr          = [0x%x].\n",
                    drvState->ctrlPtr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dspCtrlAddr      = [0x%x].\n",
                    drvState->dspCtrlAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    interface        = [0x%x].\n",
                    drvState->interface) ;
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;

        /*  --------------------------------------------------------------------
         *  Print the information for the shared memory region.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "Shared memory region information:\n") ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    version              = [%s]\n",
                    drvState->ctrlPtr->version) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ProcId               = [%d]\n",
                    drvState->ctrlPtr->procId) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    linkAddr             = [%d]\n",
                    drvState->ctrlPtr->linkAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    drvDspInitDone       = [0x%x]\n",
                    drvState->ctrlPtr->drvDspInitDone) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ipsAddr              = [0x%x]\n",
                    drvState->ctrlPtr->ipsAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ipsDspInitDone       = [0x%x]\n",
                    drvState->ctrlPtr->ipsDspInitDone) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    poolAddr             = [0x%x]\n",
                    drvState->ctrlPtr->poolAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    poolConfigured       = [0x%x]\n",
                    drvState->ctrlPtr->poolConfigured) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    poolDspInitDone      = [0x%x]\n",
                    drvState->ctrlPtr->poolDspInitDone) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    mpcsAddr             = [0x%x]\n",
                    drvState->ctrlPtr->mpcsAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    mpcsConfigured       = [0x%x]\n",
                    drvState->ctrlPtr->mpcsConfigured) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    mpcsDspInitDone      = [0x%x]\n",
                    drvState->ctrlPtr->mpcsDspInitDone) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    mplistAddr           = [0x%x]\n",
                    drvState->ctrlPtr->mplistAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    mplistConfigured     = [0x%x]\n",
                    drvState->ctrlPtr->mplistConfigured) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    mplistDspInitDone    = [0x%x]\n",
                    drvState->ctrlPtr->mplistDspInitDone) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    mqtAddr              = [0x%x]\n",
                    drvState->ctrlPtr->mqtAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    mqtConfigured        = [0x%x]\n",
                    drvState->ctrlPtr->mqtConfigured) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    mqtDspInitDone       = [0x%x]\n",
                    drvState->ctrlPtr->mqtDspInitDone) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dataAddr             = [0x%x]\n",
                    drvState->ctrlPtr->dataAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dataConfigured       = [0x%x]\n",
                    drvState->ctrlPtr->dataConfigured) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dataDspInitDone      = [0x%x]\n",
                    drvState->ctrlPtr->dataDspInitDone) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ringIoAddr           = [0x%x]\n",
                    drvState->ctrlPtr->ringIoAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ringIoConfigured     = [0x%x]\n",
                    drvState->ctrlPtr->ringIoConfigured) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ringIoDspInitDone    = [0x%x]\n",
                    drvState->ctrlPtr->ringIoDspInitDone) ;

        TRC_1PRINT (TRC_LEVEL4,
                    "    config.numIpsEntries  = [0x%x]\n",
                    drvState->ctrlPtr->config.numIpsEntries) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    config.numPools       = [0x%x]\n",
                    drvState->ctrlPtr->config.numPools) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    config.numDataDrivers = [0x%x]\n",
                    drvState->ctrlPtr->config.numDataDrivers) ;

        /*  --------------------------------------------------------------------
         *  Call the debug function for the specific link driver.
         *  --------------------------------------------------------------------
         */
        TRC_2PRINT (TRC_LEVEL4,
                    "Platform-specifc Link driver information for DSP ID %d"
                    " Link driver ID %d:\n",
                    dspId,
                    drvState->linkDrvId) ;
        drvState->interface->debug (dspId, drvState->linkDrvId) ;
    }
    else {
        TRC_1PRINT (TRC_LEVEL4,
                    "LDRV_DRV component for DSP ID %d is not initialized.\n",
                    dspId) ;
    }

    TRC_0LEAVE ("LDRV_DRV_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif
