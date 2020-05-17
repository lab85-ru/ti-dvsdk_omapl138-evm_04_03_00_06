/** ============================================================================
 *  @file   ldrv_ips.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/IPS/
 *
 *  @desc   Implements the interface exposed by LDRV_IPS subcomponent.
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
#include <ldrv.h>
#include <ldrv_smm.h>
#include <ldrv_ips.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_IPS

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  TRC_SetReason (status, FID_C_LDRV_IPS, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   LDRV_IPS_Config
 *
 *  @desc   This structure defines the configuration information shared between
 *          the IPS instances on the GPP and DSP-sides.
 *
 *  @field  numIpsEvents
 *              Number of events supported by the IPS.
 *  @field  gppIntId
 *              Interrupt Number to used by the IPS on GPP-side.
 *  @field  dspIntId
 *              Interrupt Number to used by the IPS on DSP-side.
 *  @field  dspIntVectorId
 *              Interrupt Vector Number to used by the IPS on DSP-side.
 *  @field  arg1
 *              First IPS-specific optional argument.
 *  @field  arg2
 *              Second IPS-specific optional argument.
 *  ============================================================================
 */
typedef struct LDRV_IPS_Config_tag {
    Uint32       numIpsEvents ;
    Uint32       gppIntId ;
    Uint32       dspIntId ;
    Uint32       dspIntVectorId ;
    Uint32       arg1 ;
    Uint32       arg2 ;
} LDRV_IPS_Config ;

/** ============================================================================
 *  @name   LDRV_IPS_Ctrl
 *
 *  @desc   This structure defines the control structure required by the
 *          LDRV_IPS component for each IPS instance
 *
 *  @field  dspAddr
 *              DSP address of the shared control structure for the IPS
 *              instance.
 *  @field  config
 *              Configuration structure for the IPS instance.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct LDRV_IPS_Ctrl_tag {
    Uint32           dspAddr ;
    LDRV_IPS_Config  config ;
    Uint16           padding [LDRV_IPS_CTRL_PADDING] ;
} LDRV_IPS_Ctrl ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_IPS_Info
 *
 *  @desc   This structure contains information about an IPS associated with a
 *          specific IPS ID.
 *
 *  @field  init
 *              Initialization function for the IPS.
 *  @field  exit
 *              Finalization function for the IPS.
 *  @field  debug
 *              Debug function for the IPS.
 *  ----------------------------------------------------------------------------
 */
typedef struct LDRV_IPS_Info_tag {
    FnIpsInit               init ;
    FnIpsExit               exit ;
    FnIpsDebug              debug ;
} LDRV_IPS_Info ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_IPS_Object
 *
 *  @desc   This structure defines the LDRV IPS object, which contains all state
 *          information required by the LDRV IPS component.
 *
 *  @field  ctrlPtr
 *              Pointer to the LDRV IPS shared memory control region.
 *  @field  dspCtrlAddr
 *              Address of the LDRV IPS shared memoyr control region in DSP
 *              address space.
 *  @field  numIps
 *              Number of IPS towards the specific DSP.
 *  @field  ipsInfo
 *              Pointer to the array of IPS information objects.
 *  ----------------------------------------------------------------------------
 */
typedef struct LDRV_IPS_Object_tag {
    LDRV_IPS_Ctrl * ctrlPtr ;
    Uint32          dspCtrlAddr ;
    Uint16          numIps ;
    LDRV_IPS_Info * ipsInfo ;
} LDRV_IPS_Object ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_IPS_State
 *
 *  @desc   State object for the LDRV_IPS component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC LDRV_IPS_Object LDRV_IPS_State [MAX_DSPS] ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_IPS_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool LDRV_IPS_IsInitialized [MAX_DSPS] ;


/** ============================================================================
 *  @func   LDRV_IPS_moduleInit
 *
 *  @desc   This function initializes the LDRV_IPS module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_IPS_moduleInit (Void)
{
    DSP_STATUS        status = DSP_SOK ;
    Uint32     dspId ;

    TRC_0ENTER ("LDRV_IPS_moduleInit") ;

    for (dspId = 0 ; (dspId < MAX_DSPS) && DSP_SUCCEEDED (status) ; dspId++) {
        /* Initialize the IsInitialized flags for the DSP. */
        LDRV_IPS_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_IPS_moduleInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_IPS_moduleExit
 *
 *  @desc   This function finalizes the LDRV_IPS module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_IPS_moduleExit (Void)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    Uint32     dspId ;

    TRC_0ENTER ("LDRV_IPS_moduleExit") ;

    /* Finalize the IsInitialized flags for all DSPs. */
    for (dspId = 0 ; dspId < MAX_DSPS ; dspId++) {
        if (LDRV_IPS_IsInitialized [dspId] == TRUE) {
            tmpStatus = LDRV_IPS_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("LDRV_IPS_moduleExit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_IPS_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_IPS_init (IN ProcessorId dspId, OUT Uint32 * shDspAddr)
{
    DSP_STATUS          status  = DSP_SOK ;
    LDRV_IPS_Ctrl *     ipsCtrl = NULL ;
    LDRV_IPS_Object *   ipsState ;
    LINKCFG_Ips *       ipsTable ;
    LINKCFG_Dsp *     dspObj ;
    LINKCFG_LinkDrv * linkDrv ;
    Int32             strCmpResult ;
    Uint32            ipsId ;
    Uint32            mapId ;
    LINKCFG_DspConfig * dspCfg ;
    Uint16              i ;

    TRC_2ENTER ("LDRV_IPS_init", dspId, shDspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (shDspAddr != NULL) ;

    ipsState = &(LDRV_IPS_State [dspId]) ;
    dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj   = dspCfg->dspObject ;
    linkDrv  = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
    ipsTable = &(dspCfg->ipsTables [linkDrv->ipsTableId][0]) ;

    *shDspAddr = (Uint32) NULL ;
    if (LDRV_IPS_IsInitialized [dspId] == FALSE) {
        /*  --------------------------------------------------------------------
         * Validate the setup configuration parameters for the IPS.
         *  --------------------------------------------------------------------
         */
        /* Initialize the state object. */
        ipsState->ctrlPtr     = NULL ;
        ipsState->dspCtrlAddr = (Uint32) NULL ;
        ipsState->numIps      = linkDrv->numIpsEntries ;

        if (ipsState->numIps > MAX_IPS) {
            /* Number of IPS in table is more than max supported. */
            PRINT_Printf ("<1>Configuration error:"
               " Number of IPS in table [%d] is more than max supported [%d]\n",
               ipsState->numIps,
               MAX_IPS) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else {
            status = MEM_Calloc ((Void **) &(ipsState->ipsInfo),
                                 (sizeof (LDRV_IPS_Info) * ipsState->numIps),
                                 MEM_DEFAULT) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            /* For each IPS, check if the configured IPS is available and set
             * the interface table if the configuration is valid.
             */
            for (ipsId = 0 ;
                 (ipsId < ipsState->numIps) && DSP_SUCCEEDED (status) ;
                 ipsId++) {
                for (mapId = 0 ; mapId < CFGMAP_Config [dspId]->numIps ; mapId++) {
                    status = GEN_Strcmp (ipsTable [ipsId].name,
                                         CFGMAP_Config [dspId]->ipsObjects [mapId].name,
                                         &strCmpResult) ;
                    DBC_Assert (DSP_SUCCEEDED (status)) ;
                    if (strCmpResult == 0) {
                        ipsState->ipsInfo [ipsId].init  =
                                        CFGMAP_Config [dspId]->ipsObjects [mapId].init ;
                        ipsState->ipsInfo [ipsId].exit  =
                                        CFGMAP_Config [dspId]->ipsObjects [mapId].exit ;
#if defined (DDSP_DEBUG)
                        ipsState->ipsInfo [ipsId].debug =
                                        CFGMAP_Config [dspId]->ipsObjects [mapId].debug ;
#endif /* if defined (DDSP_DEBUG) */
                        break ;
                    }
                }

                if (mapId == CFGMAP_Config [dspId]->numIps) {
                    /* Configured IPS is not available. */
                    PRINT_Printf ("<1>Configuration error:"
                                  " Incorrect IPS name specified [%s]\n",
                                  ipsTable [ipsId].name) ;
                    status = DSP_ECONFIG ;
                    SET_FAILURE_REASON ;
                }
                else if (   (ipsTable [ipsId].memEntry != (Uint32) -1)
                         && (ipsTable [ipsId].memEntry >= dspObj->memEntries)) {
                    /* Check if the memEntry is in valid range. */
                    PRINT_Printf ("<1>Configuration error:"
                                  " Incorrect IPS memEntry specified [0x%x]\n",
                                  ipsTable [ipsId].memEntry) ;
                    status = DSP_ECONFIG ;
                    SET_FAILURE_REASON ;
                }
                else {
                     /*Added for MISRAC compliance */
                }
            }

            if (DSP_FAILED (status)) {
                FREE_PTR (ipsState->ipsInfo) ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
        /*  ----------------------------------------------------------------
         *  Allocate shared memory required for the LDRV_IPS component.
         *  Use the memory entry of the Link driver for the LDRV_IPS
         *  component.
         *  ----------------------------------------------------------------
         */
        status = LDRV_SMM_alloc (dspId,
                                 linkDrv->memEntry,
                                 NULL, /* Physical address not required.  */
                                 &(ipsState->dspCtrlAddr),
                                 (Uint32 *) ((Pvoid) &(ipsState->ctrlPtr)),
                                 DSPLINK_ALIGN ((    (sizeof (LDRV_IPS_Ctrl)
                                                 *   ipsState->numIps)),
                                                DSPLINK_BUF_ALIGN)) ;
        if (DSP_SUCCEEDED (status)) {
            *shDspAddr = ipsState->dspCtrlAddr ;
            TRC_1PRINT (TRC_LEVEL4,
                        "LDRV_IPS Ctrl GPP address [0x%x]\n",
                        ipsState->ctrlPtr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "LDRV_IPS Ctrl DSP address [0x%x]\n",
                        ipsState->dspCtrlAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "LDRV_IPS Ctrl size        [0x%x]\n",
                        DSPLINK_ALIGN ((    (sizeof (LDRV_IPS_Ctrl)
                                        *   ipsState->numIps)),
                                       DSPLINK_BUF_ALIGN)) ;

            ipsCtrl = ipsState->ctrlPtr ;
            DBC_Assert (ipsCtrl != NULL) ;
        }
        else {
            SET_FAILURE_REASON ;
        }

            /* -----------------------------------------------------------------
         * Initialize the IPS component(s)
             * -----------------------------------------------------------------
         */
        DBC_Assert (ipsState->ipsInfo != NULL) ;
            for (i = 0 ;
                 (i < ipsState->numIps) && DSP_SUCCEEDED (status) ;
                 i++) {
                /*  ------------------------------------------------------------
                 *  Write shared configuration information into the ctrl
                 *  structure.
                 *  ------------------------------------------------------------
             */
            ipsCtrl [i].config.numIpsEvents   = ipsTable [i].numIpsEvents ;
            ipsCtrl [i].config.gppIntId       = ipsTable [i].gppIntId ;
            ipsCtrl [i].config.dspIntId       = ipsTable [i].dspIntId ;
                ipsCtrl [i].config.dspIntVectorId = ipsTable [i].dspIntVectorId;
            ipsCtrl [i].config.arg1           = ipsTable [i].arg1 ;
            ipsCtrl [i].config.arg2           = ipsTable [i].arg2 ;

                /*  ------------------------------------------------------------
                 *  Initialize the IPS towards the DSP and get the address of
                 *  its shared memory control region in DSP address space.
                 *  ------------------------------------------------------------
             */
            DBC_Assert (ipsState->ipsInfo [i].init != NULL) ;
            status = (ipsState->ipsInfo [i].init) (dspId,
                                                   i,
                                                   &(ipsCtrl [i].dspAddr)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        LDRV_IPS_IsInitialized [dspId] = TRUE ;
        if (DSP_FAILED (status)) {
                /* LDRV_IPS_exit executes cleanup only if the initialized flag
                 * is TRUE.
             */
            LDRV_IPS_exit (dspId) ;
            LDRV_IPS_IsInitialized [dspId] = FALSE ;
        }
    }
    }

    TRC_1LEAVE ("LDRV_IPS_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_IPS_exit
 *
 *  @desc   This function finalizes the LDRV IPS module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_IPS_exit (IN ProcessorId dspId)
{
    DSP_STATUS        status      = DSP_SOK ;
    DSP_STATUS        tmpStatus   = DSP_SOK ;
    LDRV_IPS_Object * ipsState ;
    LINKCFG_Dsp *     dspObj ;
    LINKCFG_LinkDrv * linkDrv ;
    LINKCFG_DspConfig * dspCfg ;
    Uint16            i ;

    TRC_1ENTER ("LDRV_IPS_exit", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    ipsState = &(LDRV_IPS_State [dspId]) ;
    dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj   = dspCfg->dspObject ;
    linkDrv  = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;

    if (LDRV_IPS_IsInitialized [dspId] == TRUE) {
        if (ipsState->ipsInfo != NULL) {
            /* -----------------------------------------------------------------
             * Finalize the IPS component(s)
             * -----------------------------------------------------------------
             */
            for (i = 0 ; (i < ipsState->numIps) ; i++) {
                if (ipsState->ipsInfo [i].exit != NULL) {
                    tmpStatus  = (ipsState->ipsInfo [i].exit) (dspId, i) ;
                    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
            }
        }

        /*  --------------------------------------------------------------------
         *  Free shared memory allocated for the LDRV_IPS component.
         *  Use the memory entry of the Link driver for the LDRV_IPS component.
         *  --------------------------------------------------------------------
         */
        if (ipsState->ctrlPtr != NULL) {
            tmpStatus = LDRV_SMM_free (dspId,
                                       linkDrv->memEntry,
                                       DSPLINK_ALIGN ((  (sizeof (LDRV_IPS_Ctrl)
                                                       * ipsState->numIps)),
                                                      DSPLINK_BUF_ALIGN)) ;
            ipsState->ctrlPtr = NULL ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        if (LDRV_IPS_State [dspId].ipsInfo != NULL) {
            tmpStatus = FREE_PTR (LDRV_IPS_State [dspId].ipsInfo) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        LDRV_IPS_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_IPS_exit", status) ;

    return status ;
}


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_IPS_debug
 *
 *  @desc   Prints the current status of this subcomponent.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_IPS_debug (IN  ProcessorId dspId)
{
    LDRV_IPS_Object * ipsState ;
    Uint16            i ;

    TRC_1ENTER ("LDRV_IPS_debug", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    ipsState = &(LDRV_IPS_State [dspId]) ;

    if (LDRV_IPS_IsInitialized [dspId] == TRUE) {
        /*  --------------------------------------------------------------------
         *  Print the LDRV IPS component state information.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_1PRINT (TRC_LEVEL4,
                    "LDRV IPS component state information for DSP ID %d:\n",
                    dspId) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ctrlPtr          = [0x%x].\n",
                    ipsState->ctrlPtr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dspCtrlAddr      = [0x%x].\n",
                    ipsState->dspCtrlAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    numIps           = [0x%x].\n",
                    ipsState->numIps) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ipsInfo          = [0x%x].\n",
                    ipsState->ipsInfo) ;
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;

        /*  --------------------------------------------------------------------
         *  Print the information for the shared memory regions.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "Shared memory region information:\n") ;
        for (i = 0 ; (i < ipsState->numIps) ; i++) {
            TRC_2PRINT (TRC_LEVEL4,
                        "    IPS ID %d: dspAddr               = [0x%x]\n",
                        i,
                        ipsState->ctrlPtr [i].dspAddr) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    IPS ID %d: config.numIpsEvents   = [0x%x]\n",
                        i,
                        ipsState->ctrlPtr [i].config.numIpsEvents) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    IPS ID %d: config.gppIntId       = [0x%x]\n",
                        i,
                        ipsState->ctrlPtr [i].config.gppIntId) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    IPS ID %d: config.dspIntId       = [0x%x]\n",
                        i,
                        ipsState->ctrlPtr [i].config.dspIntId) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    IPS ID %d: config.dspIntVectorId = [0x%x]\n",
                        i,
                        ipsState->ctrlPtr [i].config.dspIntVectorId) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    IPS ID %d: config.arg1           = [0x%x]\n",
                        i,
                        ipsState->ctrlPtr [i].config.arg1) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    IPS ID %d: config.arg2           = [0x%x]\n",
                        i,
                        ipsState->ctrlPtr [i].config.arg2) ;

            /*  ----------------------------------------------------------------
             *  Call the debug function for the specific IPS.
             *  ----------------------------------------------------------------
             */
            TRC_1PRINT (TRC_LEVEL4,
                        "Platform-specifc IPS information for IPS ID %d:\n",
                        i) ;
            (ipsState->ipsInfo [i].debug) (dspId, i) ;
        }
    }
    else {
        TRC_1PRINT (TRC_LEVEL4,
                    "LDRV_IPS component for DSP ID %d is not initialized.\n",
                    dspId) ;
    }

    TRC_0LEAVE ("LDRV_IPS_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif
