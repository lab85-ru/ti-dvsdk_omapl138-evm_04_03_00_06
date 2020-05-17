/** ============================================================================
 *  @file   ldrv_ringio.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/RINGIO/
 *
 *  @desc   Low level functions for RingIO module.
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


/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <_dsplink.h>
#include <procdefs.h>
#include <linkcfgdefs.h>
#include <_ringiodefs.h>
#include <ringiodefs.h>

/*  ----------------------------------- Trace & Debug                 */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Header                   */
#include <mem.h>

/*  ----------------------------------- Link Driver                   */
#include <ldrv_ringio.h>
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
#define  COMPONENT_ID       ID_LDRV_RINGIO

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_LDRV_RINGIO, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_RINGIO_Object
 *
 *  @desc   This structure defines the LDRV RINGIO object, which contains all
 *          state information required by the LDRV RINGIO component.
 *
 *  @field  ctrlPtr
 *              Pointer to the RINGIO shared memory control region.
 *  @field  dspCtrlAddr
 *              Address of the RINGIO shared memory control region in DSP
 *              address space.
 *  @field  physCtrlAddr
 *              Address of the RINGIO shared memory control region in physical
 *              address space.
 *  @field  ctrlSize
 *              Size of the RINGIO shared memory control region.
 *  @field  memEntry
 *              Entry ID of the RINGIO shared memory control region.
 *  ----------------------------------------------------------------------------
 */
typedef struct LDRV_RINGIO_Object_tag {
    RingIO_Ctrl *   ctrlPtr ;
    Uint32          dspCtrlAddr ;
    Uint32          physCtrlAddr ;
    Uint32          ctrlSize ;
    Uint32          memEntry ;
} LDRV_RINGIO_Object ;


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_RINGIO_State
 *
 *  @desc   State object for the LDRV_RINGIO component containing all
 *          information required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC LDRV_RINGIO_Object LDRV_RINGIO_State [MAX_DSPS] ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_RINGIO_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool LDRV_RINGIO_IsInitialized [MAX_DSPS] ;


/** ============================================================================
 *  @func   LDRV_RINGIO_moduleInit
 *
 *  @desc   This function initializes the LDRV_RINGIO module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_RINGIO_moduleInit (Void)
{
    DSP_STATUS           status = DSP_SOK ;
    Uint32               dspId ;

    TRC_0ENTER ("LDRV_RINGIO_moduleInit") ;

    for (dspId = 0 ; (dspId < MAX_DSPS) && DSP_SUCCEEDED (status) ; dspId++) {
        /* Initialize the IsInitialized flag for the DSP. */
        LDRV_RINGIO_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_RINGIO_moduleInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_RINGIO_moduleExit
 *
 *  @desc   This function finalizes the LDRV_RINGIO module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_RINGIO_moduleExit (Void)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    Uint32     dspId ;

    TRC_0ENTER ("LDRV_RINGIO_moduleExit") ;

    /* Finalize the IsInitialized flags for all DSPs. */
    for (dspId = 0 ; dspId < MAX_DSPS ; dspId++) {
        if (LDRV_RINGIO_IsInitialized [dspId] == TRUE) {
            tmpStatus = LDRV_RINGIO_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("LDRV_RINGIO_moduleExit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_RINGIO_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_RINGIO_init (IN ProcessorId dspId, OUT Uint32 * shDspAddr)
{
    DSP_STATUS           status = DSP_SOK ;
    LINKCFG_RingIo *     ringIoObject ;
    LINKCFG_Dsp *        dspObj ;
    LINKCFG_LinkDrv *    linkDrv ;
    LDRV_RINGIO_Object * ringIoState ;
    LINKCFG_DspConfig *  dspCfg ;

    TRC_2ENTER ("LDRV_RINGIO_init", dspId, shDspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (shDspAddr != NULL) ;

    ringIoState  = &(LDRV_RINGIO_State [dspId]) ;

    dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj    = dspCfg->dspObject ;
    linkDrv   = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
    ringIoObject = &(dspCfg->ringIoObjects [linkDrv->ringIoTableId]) ;

    *shDspAddr = (Uint32) NULL ;
    if (LDRV_RINGIO_IsInitialized [dspId] == FALSE) {
        /*  --------------------------------------------------------------------
         * Validate the setup configuration parameters for the link driver.
         *  --------------------------------------------------------------------
         */
        ringIoState = &(LDRV_RINGIO_State [dspId]) ;

        /* Initialize the state object. */
        ringIoState->ctrlPtr      = NULL ;
        ringIoState->dspCtrlAddr  = (Uint32) NULL ;
        ringIoState->physCtrlAddr = (Uint32) NULL ;
        ringIoState->ctrlSize     = 0 ;
        ringIoState->memEntry     = ringIoObject->memEntry ;

        /* Check if the memEntry is in valid range. */
        if (    (ringIoObject->memEntry != (Uint32) -1)
            &&  (ringIoObject->memEntry >= dspObj->memEntries)) {
            PRINT_Printf ("<1>Configuration error:"
                          " Incorrect RINGIO memEntry specified [0x%x]\n",
                          ringIoObject->memEntry) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else if (    (ringIoObject->ipsId != (Uint32) -1)
                 &&  (ringIoObject->ipsId >= linkDrv->numIpsEntries)) {
            /* Check if the ipsId is in valid range. */
            PRINT_Printf ("<1>Configuration error:"
                          " Incorrect RINGIO ipsId specified [0x%x]\n",
                          ringIoObject->ipsId) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else if (    (ringIoObject->ipsId      != (Uint32) -1)
                 &&  (ringIoObject->ipsEventNo != (Uint32) -1)
                 &&  (   ringIoObject->ipsEventNo
                      >= dspCfg->ipsTables [linkDrv->ipsTableId]
                                          [ringIoObject->ipsId].numIpsEvents)) {
            /* Check if the ipsEventNo is in valid range. */
            PRINT_Printf ("<1>Configuration error:"
                          " Incorrect RINGIO ipsEventNo specified [0x%x]\n",
                          ringIoObject->ipsEventNo) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else {
            /* Added for MISRAC compliance */
        }
    }

    if (   (LDRV_RINGIO_IsInitialized [dspId] == FALSE)
        && (DSP_SUCCEEDED (status))) {
        /*  --------------------------------------------------------------------
         *  Allocate shared memory required for the RINGIO component.
         *  --------------------------------------------------------------------
         */
        ringIoState->ctrlSize = sizeof (RingIO_Ctrl)
                         +  (sizeof (RingIO_Entry) * ringIoObject->maxEntries) ;
        status = LDRV_SMM_alloc (dspId,
                                 ringIoState->memEntry,
                                 &(ringIoState->physCtrlAddr),
                                 &(ringIoState->dspCtrlAddr),
                                 (Uint32 *) ((Pvoid) &(ringIoState->ctrlPtr)),
                                 ringIoState->ctrlSize) ;
        if (DSP_SUCCEEDED (status)) {
            *shDspAddr = ringIoState->dspCtrlAddr ;
            TRC_1PRINT (TRC_LEVEL4,
                        "RINGIO Ctrl GPP address      [0x%x]\n",
                        ringIoState->ctrlPtr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "RINGIO Ctrl DSP address      [0x%x]\n",
                        ringIoState->dspCtrlAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "RINGIO Ctrl physical address [0x%x]\n",
                        ringIoState->physCtrlAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "RINGIO Ctrl size             [0x%x]\n",
                        ringIoState->ctrlSize) ;

            /* ringIoState->ctrlPtr->entry is set on user-side with user
             * address.
             */
            ringIoState->ctrlPtr->dspAddrEntry = (RingIO_Entry *)
                                         (   (Uint32) (ringIoState->dspCtrlAddr)
                                          +  sizeof (RingIO_Ctrl)) ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        LDRV_RINGIO_IsInitialized [dspId] = TRUE ;
        if (DSP_FAILED (status)) {
            /* LDRV_RINGIO_exit executes cleanup only if the initialized flag is
             * TRUE.
             */
            LDRV_RINGIO_exit (dspId) ;
            LDRV_RINGIO_IsInitialized [dspId] = FALSE ;
        }
    }

    TRC_1LEAVE ("LDRV_RINGIO_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_RINGIO_exit
 *
 *  @desc   This function finalizes the LDRV RINGIO module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_RINGIO_exit (IN ProcessorId dspId)
{
    DSP_STATUS           status    = DSP_SOK ;
    LDRV_RINGIO_Object * ringIoState ;

    TRC_1ENTER ("LDRV_RINGIO_exit", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (LDRV_RINGIO_IsInitialized [dspId] == TRUE) {
        ringIoState = &(LDRV_RINGIO_State [dspId]) ;

        /*  --------------------------------------------------------------------
         *  Free shared memory allocated for the RINGIO component.
         *  --------------------------------------------------------------------
         */
        if (ringIoState->ctrlPtr != NULL) {
            status = LDRV_SMM_free (dspId,
                                    ringIoState->memEntry,
                                    ringIoState->ctrlSize) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        LDRV_RINGIO_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_RINGIO_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_RINGIO_getMemInfo
 *
 *  @desc   This function returns the memory information for the RINGIO region.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_RINGIO_getMemInfo (IN OUT RingIO_MemInfo * ringIoMemInfo)
{
    DSP_STATUS           status = DSP_SOK ;
    LDRV_RINGIO_Object * ringIoState ;

    TRC_1ENTER ("LDRV_RINGIO_getMemInfo", ringIoMemInfo) ;

    DBC_Require (ringIoMemInfo != NULL) ;
    DBC_Require (IS_VALID_PROCID (ringIoMemInfo->procId)) ;
    DBC_Assert  (LDRV_RINGIO_IsInitialized [ringIoMemInfo->procId] == TRUE) ;

    ringIoState  = &(LDRV_RINGIO_State [ringIoMemInfo->procId]) ;

    ringIoMemInfo->kernAddr = (Uint32) ringIoState->ctrlPtr ;
    ringIoMemInfo->physAddr = ringIoState->physCtrlAddr ;
    ringIoMemInfo->size     = ringIoState->ctrlSize ;

    TRC_1LEAVE ("LDRV_RINGIO_getMemInfo", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
