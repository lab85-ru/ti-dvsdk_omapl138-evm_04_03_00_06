/** ============================================================================
 *  @file   ldrv_mpcs.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/MPCS/
 *
 *  @desc   Implementation of MPCS - (Multi Processor Critical Section) for two
 *          processors only. This implementation is based on Peterson's
 *          Algorithm.
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
#include <mpcsdefs.h>
#include <linkcfgdefs.h>

/*  ----------------------------------- Profiling                   */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* if defined (DDSP_PROFILE) */

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <mem.h>
#include <sync.h>

/*  ----------------------------------- Generic Functions           */
#include <list.h>

/*  ----------------------------------- User API                    */
#include <mpcs.h>

/*  ----------------------------------- Link Driver                 */
#include <ldrv_mpcs.h>
#include <ldrv_mpcs_os.h>
#include <ldrv_smm.h>
#include <ldrv.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   COMPONENT_ID
 *
 *  @desc   Component ID of this module. That will be used by TRC macros.
 *  ============================================================================
 */
#define COMPONENT_ID         ID_LDRV_MPCS

/** ============================================================================
 *  @name   SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_LDRV_MPCS, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/*  ----------------------------------------------------------------------------
 *  @name   MPCS_FREE
 *
 *  @desc   Value to indicate that the critical section is free.
 *  ----------------------------------------------------------------------------
 */
#define MPCS_FREE           0xBADEu

/*  ----------------------------------------------------------------------------
 *  @name   MPCS_BUSY
 *
 *  @desc   Value to indicate that the critical section is busy.
 *  ----------------------------------------------------------------------------
 */
#define MPCS_BUSY           0xDABAu

/*  ----------------------------------------------------------------------------
 *  @name   SELF
 *
 *  @desc   Value to indicate index number of the current processor. For owner
 *          of MPCS Object, it will be set to 0, otherwise set to 1 for peer.
 *  ----------------------------------------------------------------------------
 */
#define SELF                0u

/*  ----------------------------------------------------------------------------
 *  @name   OTHER
 *
 *  @desc   Value to indicate index number of the other processor.For owner of
 *          MPCS Object, it will be set to 1, otherwise set to 0 for peer.
 *  ----------------------------------------------------------------------------
 */
#define OTHER               1u


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_MPCS_Object
 *
 *  @desc   This structure defines the LDRV MPCS object, which contains all
 *          state information required by the LDRV MPCS component.
 *
 *  @field  ctrlPtr
 *              Pointer to the MPCS shared memory control region.
 *  @field  dspCtrlAddr
 *              Address of the MPCS shared memory control region in DSP address
 *              space.
 *  @field  physCtrlAddr
 *              Address of the MPCS shared memory control region in physical
 *              address space.
 *  @field  ctrlSize
 *              Size of the MPCS shared memory control region.
 *  @field  memEntry
 *              Entry ID of the MPCS shared memory control region.
 *  @field  maxEntries
 *              Maximum instances of MPCS that can be created.
 *  ----------------------------------------------------------------------------
 */
typedef struct LDRV_MPCS_Object_tag {
    MPCS_Ctrl *     ctrlPtr ;
    Uint32          dspCtrlAddr ;
    Uint32          physCtrlAddr ;
    Uint32          ctrlSize ;
    Uint32          memEntry ;
    Uint32          maxEntries ;
} LDRV_MPCS_Object ;


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_MPCS_State
 *
 *  @desc   State object for the LDRV_MPCS component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC LDRV_MPCS_Object LDRV_MPCS_State [MAX_DSPS] ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_MPCS_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool LDRV_MPCS_IsInitialized [MAX_DSPS] ;


/** ============================================================================
 *  @func   LDRV_MPCS_moduleInit
 *
 *  @desc   This function initializes the LDRV_MPCS module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPCS_moduleInit (Void)
{
    DSP_STATUS         status = DSP_SOK ;
    Uint32             dspId ;

    TRC_0ENTER ("LDRV_MPCS_moduleInit") ;

    for (dspId = 0 ; (dspId < MAX_DSPS) && DSP_SUCCEEDED (status) ; dspId++) {
        /* Initialize the IsInitialized flag for the DSP. */
        LDRV_MPCS_IsInitialized [dspId] = FALSE ;
    }

    status = LDRV_MPCS_OS_moduleInit ( ) ;

    TRC_1LEAVE ("LDRV_MPCS_moduleInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPCS_moduleExit
 *
 *  @desc   This function finalizes the LDRV_MPCS module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPCS_moduleExit (Void)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    Uint32     dspId ;

    TRC_0ENTER ("LDRV_MPCS_moduleExit") ;

    status = LDRV_MPCS_OS_moduleExit ( ) ;

    /* Finalize the IsInitialized flags for all DSPs. */
    for (dspId = 0 ; dspId < MAX_DSPS ; dspId++) {
        if (LDRV_MPCS_IsInitialized [dspId] == TRUE) {
            tmpStatus = LDRV_MPCS_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("LDRV_MPCS_moduleExit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPCS_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPCS_init (IN ProcessorId dspId, OUT Uint32 * shDspAddr)
{
    DSP_STATUS         status = DSP_SOK ;
    LINKCFG_Mpcs *     mpcsObject ;
    LINKCFG_Dsp *      dspObj ;
    LINKCFG_DspConfig * dspCfg ;
    LINKCFG_LinkDrv *  linkDrv ;
    LDRV_MPCS_Object * mpcsState ;
    MPCS_Ctrl *        mpcsCtrl ;
    Uint32             i ;
    MPCS_Entry *       entry ;

    TRC_2ENTER ("LDRV_MPCS_init", dspId, shDspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (shDspAddr != NULL) ;

    mpcsState  = &(LDRV_MPCS_State [dspId]) ;

    dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj    = dspCfg->dspObject ;
    linkDrv   = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
    mpcsObject = &(dspCfg->mpcsObjects [linkDrv->mpcsTableId]) ;

    *shDspAddr = (Uint32) NULL ;
    if (LDRV_MPCS_IsInitialized [dspId] == FALSE) {
        /*  --------------------------------------------------------------------
         * Validate the setup configuration parameters for the link driver.
         *  --------------------------------------------------------------------
         */
        mpcsState = &(LDRV_MPCS_State [dspId]) ;

        /* Initialize the state object. */
        mpcsState->ctrlPtr      = NULL ;
        mpcsState->dspCtrlAddr  = (Uint32) NULL ;
        mpcsState->physCtrlAddr = (Uint32) NULL ;
        mpcsState->ctrlSize     = 0 ;
        mpcsState->memEntry     = mpcsObject->memEntry ;
        mpcsState->maxEntries   = mpcsObject->maxEntries ;

        /* Check if the memEntry is in valid range. */
        if (    (mpcsObject->memEntry != (Uint32) -1)
            &&  (mpcsObject->memEntry >= dspObj->memEntries)) {
            PRINT_Printf ("<1>Configuration error:"
                          " Incorrect MPCS memEntry specified [0x%x]\n",
                          mpcsObject->memEntry) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else if (    (mpcsObject->ipsId != (Uint32) -1)
                 &&  (mpcsObject->ipsId >= linkDrv->numIpsEntries)) {
            /* Check if the ipsId is in valid range. */
            PRINT_Printf ("<1>Configuration error:"
                          " Incorrect MPCS ipsId specified [0x%x]\n",
                          mpcsObject->ipsId) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else if (    (mpcsObject->ipsId      != (Uint32) -1)
                 &&  (mpcsObject->ipsEventNo != (Uint32) -1)
                 &&  (   mpcsObject->ipsEventNo
                      >= dspCfg->ipsTables [linkDrv->ipsTableId]
                                            [mpcsObject->ipsId].numIpsEvents)) {
            /* Check if the ipsEventNo is in valid range. */
            PRINT_Printf ("<1>Configuration error:"
                          " Incorrect MPCS ipsEventNo specified [0x%x]\n",
                          mpcsObject->ipsEventNo) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else {
            /*Added for MISRAC compliance */
        }
    }

    if (   (LDRV_MPCS_IsInitialized [dspId] == FALSE)
        && (DSP_SUCCEEDED (status))) {
        /*  --------------------------------------------------------------------
         *  Allocate shared memory required for the MPCS component.
         *  --------------------------------------------------------------------
         */
        mpcsState->ctrlSize =   sizeof (MPCS_Ctrl)
                             +  (sizeof (MPCS_Entry) * mpcsObject->maxEntries) ;
        status = LDRV_SMM_alloc (dspId,
                                 mpcsState->memEntry,
                                 &(mpcsState->physCtrlAddr),
                                 &(mpcsState->dspCtrlAddr),
                                 (Uint32 *) ((Pvoid) &(mpcsState->ctrlPtr)),
                                 mpcsState->ctrlSize) ;
        if (DSP_SUCCEEDED (status)) {
            *shDspAddr = mpcsState->dspCtrlAddr ;
            TRC_1PRINT (TRC_LEVEL4,
                        "MPCS Ctrl GPP address      [0x%x]\n",
                        mpcsState->ctrlPtr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "MPCS Ctrl DSP address      [0x%x]\n",
                        mpcsState->dspCtrlAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "MPCS Ctrl physical address [0x%x]\n",
                        mpcsState->physCtrlAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "MPCS Ctrl size             [0x%x]\n",
                        mpcsState->ctrlSize) ;

            mpcsCtrl = mpcsState->ctrlPtr ;
            DBC_Assert (mpcsCtrl != NULL) ;

            /* Initialize the control region. */
            mpcsCtrl->dspId      = dspId ;
            mpcsCtrl->maxEntries = mpcsState->maxEntries ;
            mpcsCtrl->ipsId      = mpcsObject->ipsId ;
            mpcsCtrl->ipsEventNo = mpcsObject->ipsEventNo ;
            entry = (MPCS_Entry *) (   (Uint32) (mpcsState->ctrlPtr)
                                              +  sizeof (MPCS_Ctrl)) ;
            mpcsCtrl->dspAddrEntry = (MPCS_Entry *)
                                           (   (Uint32) (mpcsState->dspCtrlAddr)
                                            +  sizeof (MPCS_Ctrl)) ;

            /* Initialize the entries table. */
            for (i = 0 ; i < mpcsState->maxEntries ; i++) {
                entry [i].physAddress = NULL ;
                entry [i].name [0]    = '\0' ;
            }

            /* Create the MPCS to be used for protecting the shared region. */
            status = LDRV_MPCS_create (dspId, &(mpcsCtrl->lockObj)) ;
            if (DSP_SUCCEEDED (status)) {
                mpcsCtrl->isInitialized = TRUE ;
            }
            else {
                SET_FAILURE_REASON ;
            }

            /* mpcsCtrl->entry is reset on user-side with user address */
        }
        else {
            SET_FAILURE_REASON ;
        }

        LDRV_MPCS_IsInitialized [dspId] = TRUE ;
        if (DSP_FAILED (status)) {
            /* LDRV_MPCS_exit executes cleanup only if the initialized flag is
             * TRUE.
             */
            LDRV_MPCS_exit (dspId) ;
            LDRV_MPCS_IsInitialized [dspId] = FALSE ;
        }
    }

    TRC_1LEAVE ("LDRV_MPCS_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPCS_exit
 *
 *  @desc   This function finalizes the LDRV MPCS module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPCS_exit (IN ProcessorId dspId)
{
    DSP_STATUS         status    = DSP_SOK ;
    DSP_STATUS         tmpStatus = DSP_SOK ;
    LDRV_MPCS_Object * mpcsState ;
    MPCS_Ctrl *        mpcsCtrl ;
    Uint32             i ;
    MPCS_Entry *       entry ;

    TRC_1ENTER ("LDRV_MPCS_exit", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (LDRV_MPCS_IsInitialized [dspId] == TRUE) {
        mpcsState = &(LDRV_MPCS_State [dspId]) ;
        mpcsCtrl  = mpcsState->ctrlPtr ;

        if (mpcsCtrl != NULL) {
            /* mpcsCtrl->entry is reset on user-side with user address, so it
             * is again set to kernel address.
             */
            entry = (MPCS_Entry *) (   (Uint32) (mpcsState->ctrlPtr)
                                              +  sizeof (MPCS_Ctrl)) ;

            status = LDRV_MPCS_delete (dspId, &(mpcsCtrl->lockObj)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            /* Finalize the entries table. */
            for (i = 0 ; i < mpcsState->maxEntries ; i++) {
                entry [i].physAddress = NULL ;
                entry [i].name [0]    = '\0' ;
            }

            /*  ----------------------------------------------------------------
             *  Free shared memory allocated for the MPCS component.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_SMM_free (dspId,
                                       mpcsState->memEntry,
                                       mpcsState->ctrlSize) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            mpcsCtrl->isInitialized = FALSE ;
        }

        LDRV_MPCS_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_MPCS_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPCS_create
 *
 *  @desc   This function creates and initializes an instance of the MPCS
 *          object.
 *          The memory for the object may or may not be provided by the user.
 *          If provided by the user, the memory for the object must be shared
 *          across the processors using the MPCS. It must also already be mapped
 *          into user space for OSes supporting user/kernel separation.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_create (IN     ProcessorId    dspId,
                  IN     MPCS_ShObj *   mpcsObj)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("LDRV_MPCS_create", dspId, mpcsObj) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (mpcsObj != NULL) ;

    mpcsObj->turn                     = SELF  ;
#if defined (DDSP_PROFILE)
    mpcsObj->gppMpcsObj.conflicts     = 0 ;
    mpcsObj->gppMpcsObj.numCalls      = 0 ;
    mpcsObj->dspMpcsObj.conflicts     = 0 ;
    mpcsObj->dspMpcsObj.numCalls      = 0 ;
#endif

    status = LDRV_MPCS_OS_create (dspId, mpcsObj) ;

        mpcsObj->dspMpcsObj.localLock     = 0 ;
        mpcsObj->gppMpcsObj.flag          = (Uint16) MPCS_FREE  ;
        mpcsObj->dspMpcsObj.flag          = (Uint16) MPCS_FREE  ;
        mpcsObj->gppMpcsObj.freeObject    = (Uint16) FALSE  ;
        mpcsObj->dspMpcsObj.freeObject    = (Uint16) FALSE  ;
    mpcsObj->dspMpcsObj.mpcsEntryAddr = 0 ;
    mpcsObj->gppMpcsObj.mpcsEntryAddr = 0 ;

    TRC_1LEAVE ("LDRV_MPCS_create", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPCS_delete
 *
 *  @desc   This function deletes the instance of the MPCS object.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_delete (IN     ProcessorId  dspId,
                  IN     MPCS_ShObj * mpcsObj)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_2ENTER ("LDRV_MPCS_delete", dspId, mpcsObj) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (mpcsObj != NULL) ;

    DBC_Assert (mpcsObj->dspMpcsObj.flag != MPCS_BUSY) ;
    mpcsObj->dspMpcsObj.flag = (Uint16) MPCS_FREE ;
    mpcsObj->gppMpcsObj.flag = (Uint16) MPCS_FREE ;

    status = LDRV_MPCS_OS_delete (dspId, mpcsObj) ;

    TRC_1LEAVE ("LDRV_MPCS_delete", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPCS_enter
 *
 *  @desc   This function enters the critical section specified by the MPCS
 *          object.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_enter (IN     MPCS_ShObj * mpcsObj)
{
    DSP_STATUS status = DSP_SOK ;
#if defined (DDSP_PROFILE)
    Bool       conflictFlag = FALSE ;
#endif

    TRC_1ENTER ("LDRV_MPCS_enter", mpcsObj) ;

    DBC_Require (mpcsObj != NULL) ;

    LDRV_MPCS_OS_enter (mpcsObj) ;

    /* indicate, needs to use the resource. */
    mpcsObj->gppMpcsObj.flag = (Uint16) MPCS_BUSY;

    /* Give away the turn. */
    mpcsObj->turn = OTHER ;

    /* Wait while the other process is using the resource and has the turn. */
    while (     (mpcsObj->dspMpcsObj.flag == (Uint16) MPCS_BUSY)
           &&   (mpcsObj->turn != (Uint16) SELF)) {
#if defined (DDSP_PROFILE)
        conflictFlag = TRUE ;
#endif
    }

#if defined (DDSP_PROFILE)
    mpcsObj->gppMpcsObj.numCalls++ ;
    if (conflictFlag == TRUE) {
        mpcsObj->gppMpcsObj.conflicts++ ;
    }
#endif

    TRC_1LEAVE ("LDRV_MPCS_enter", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPCS_leave
 *
 *  @desc   This function leaves the critical section specified by the MPCS
 *          object.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_leave (IN     MPCS_ShObj * mpcsObj)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("LDRV_MPCS_leave", mpcsObj) ;

    DBC_Require (mpcsObj != NULL) ;

    /* Release the resource. */
    mpcsObj->gppMpcsObj.flag = (Uint16) MPCS_FREE;

    status = LDRV_MPCS_OS_leave (mpcsObj) ;

    TRC_1LEAVE ("LDRV_MPCS_leave", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPCS_getMemInfo
 *
 *  @desc   This function returns the memory information for the MPCS region.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPCS_getMemInfo (IN OUT MPCS_MemInfo * mpcsMemInfo)
{
    DSP_STATUS         status = DSP_SOK ;
    LDRV_MPCS_Object * mpcsState ;

    TRC_1ENTER ("LDRV_MPCS_GetMemInfo", mpcsMemInfo) ;

    DBC_Require (mpcsMemInfo != NULL) ;
    DBC_Require (IS_VALID_PROCID (mpcsMemInfo->procId)) ;
    DBC_Assert  (LDRV_MPCS_IsInitialized [mpcsMemInfo->procId] == TRUE) ;

    mpcsState  = &(LDRV_MPCS_State [mpcsMemInfo->procId]) ;

    mpcsMemInfo->kernAddr = (Uint32) mpcsState->ctrlPtr ;
    mpcsMemInfo->physAddr = mpcsState->physCtrlAddr ;
    mpcsMemInfo->size     = mpcsState->ctrlSize ;

    TRC_1LEAVE ("LDRV_MPCS_GetMemInfo", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
