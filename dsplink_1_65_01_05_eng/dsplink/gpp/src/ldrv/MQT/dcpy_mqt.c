/** ============================================================================
 *  @file   dcpy_mqt.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/MQT/
 *
 *  @desc   Implementation of the DCPY MQT for GPP-DSP communication.
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

/*  ----------------------------------- Trace & Debug                 */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Header                   */
#include <dpc.h>
#include <mem.h>
#include <sync.h>
#include <print.h>
#include <isr.h>

/*  ----------------------------------- Generic Function              */
#include <gen_utils.h>
#include <list.h>

/*  ----------------------------------- User API                      */
#include <msgq.h>
#include <mpcs.h>

/*  ----------------------------------- Link Driver                   */
#include <ldrv_pool.h>
#include <dma_pool.h>
#include <ldrv_mqt.h>
#include <ldrv_msgq.h>
#include <ldrv_mpcs.h>
#include <shm_drv.h>
#include <ldrv_mplist.h>
#include <ldrv.h>

#if defined (CHNL_COMPONENT)
#include <ldrv_chnl.h>
#include <dcpy_data.h>
#endif /* if defined (CHNL_COMPONENT) */
#include <ips.h>
#include <dcpy_mqt.h>
#include <dsp.h>

#include <ldrv_mqt.h>
#include <ldrv_smm.h>
#include <ldrv.h>

/*  ----------------------------------- LOG  Headers                 */
#if defined (LOG_COMPONENT)
#include <linklog.h>
#endif /* if defined (LOG_COMPONENT) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent ID.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_MQT_ZCPY

/** ============================================================================
 *  @macro  MQT_EXIT_VALUE
 *
 *  @desc   Indicates MQT is uninitialized.
 *  ============================================================================
 */
#define  MQT_EXIT_VALUE     0xB0CA

/** ============================================================================
 *  @macro  MQT_INIT_VALUE
 *
 *  @desc   Indicates MQT is initialized.
 *  ============================================================================
 */
#define  MQT_INIT_VALUE     0xC0BA

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON TRC_SetReason (status, FID_C_LDRV_MQT_ZCPY, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   DCPYMQT_Object
 *
 *  @desc   This structure defines the DCPYMQT object, which contains all state
 *          information required by the DCPYMQT component.
 *
 *  @field  poolId
 *              The pool to be used by the DCPY MQT for control messages.
 *  @field  mqtId
 *              ID of the MQT.
 *  @field  ackMsgList
 *              List of locateAck messages received from the DSP.
 *  @arg    dpcObj
 *              DPC object used by the DCPY MQT.
 *  @field  wordSwap
 *              Indicates whether word-swap is enabled for the DSP MMU.
 *  @field  dspId
 *              Indicates which dsp it is attached with.
 *  @field  ipsId
 *              Indicates which IPS is used for communicating with the DSP.
 *  @field  ipsEventNo
 *              IPS Event no associated with MQT.
 *  @field  dspMaduSize
 *              Indicates MADU size for the DSP.
 *  @field  memEntry
 *              ID of the memory entry for the DCPY MQT shared control region.
 *  @field  ctrlPtr
 *              Pointer to the DCPYMQT control structure in shared memory.
 *  ============================================================================
 */
typedef struct DCPYMQT_Object_tag {
    Uint32              dspId ;
    Uint32              mqtId ;
    PoolId              poolId ;
    List *              ackMsgList ;
    DpcObject *         dpcObj ;
    Bool                wordSwap ;
    Uint32              ipsId ;
    Uint32              ipsEventNo ;
    Uint32              dspMaduSize ;
    Uint32              memEntry ;
    DCPYMQT_Ctrl *      ctrlPtr ;
} DCPYMQT_Object ;


/** ============================================================================
 *  @name   DCPYMQT_Interface
 *
 *  @desc   The MQT APIs are exposed to MSGQ through the function table.
 *  ============================================================================
 */
MQT_Interface DCPYMQT_Interface = {
    &DCPYMQT_init,
    &DCPYMQT_exit,
    &DCPYMQT_open,
    &DCPYMQT_close,
    &DCPYMQT_locate,
    &DCPYMQT_release,
    &DCPYMQT_put
#if defined (DDSP_DEBUG)
    ,&DCPYMQT_debug
#endif /* defined (DDSP_DEBUG) */
} ;


/** ----------------------------------------------------------------------------
 *  @name   DCPYMQT_State
 *
 *  @desc   State object for the DCPYMQT component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC DCPYMQT_Object DCPYMQT_State [MAX_DSPS] ;

/*  ----------------------------------------------------------------------------
 *  @name   DCPYMQT_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool DCPYMQT_IsInitialized [MAX_DSPS] ;


/** ----------------------------------------------------------------------------
 *  @func   DCPYMQT_callback
 *
 *  @desc   Implements the callback invoked by the SHMIPS component on receiving
 *          a message from the remote processor.
 *
 *  @arg    ipsEventNo
 *              Event number associated with the callback.
 *  @arg    arg
 *              Argument registered with the SHMIPS component along with the
 *              callback function.
 *  @arg    info
 *              Pointer to message received in the event.
 *
 *  @ret    None.
 *
 *  @enter  info must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Void
DCPYMQT_callback (IN Uint32 ipsEventNo, IN Pvoid arg, IN Pvoid info) ;


/** ----------------------------------------------------------------------------
 *  @func   DCPYMQT_dpc
 *
 *  @desc   Implements the Deferred Procedure Call for the DCPYMQT.
 *
 *  @arg    refData
 *              Parameter specified while creating the DPC.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Void
DCPYMQT_dpc (IN Pvoid refData) ;


/** ----------------------------------------------------------------------------
 *  @func   DCPYMQT_send
 *
 *  @desc   Sends the specified message to the DSP.
 *
 *  @arg    dcpyMqtState
 *              Pointer to the state object for the MQT.
 *  @arg    msg
 *              Pointer to the message to be sent to the DSP.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DCPYMQT_send (IN DCPYMQT_Object * dcpyMqtState,
              IN Pvoid            msg) ;


/** ============================================================================
 *  @func   DCPYMQT_init
 *
 *  @desc   This function initializes the DCPY MQT.
 *
 *  @modif  DCPYMQT_State [dspId]
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYMQT_init (IN  ProcessorId dspId,
              IN  Uint32      mqtId,
              OUT Uint32 *    shDspAddr)
{
    DSP_STATUS          status = DSP_SOK ;
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_Mqt *       mqtObject ;
    DCPYMQT_Object *    dcpyMqtState ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_3ENTER ("DCPYMQT_init", dspId, mqtId, shDspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (shDspAddr != NULL) ;

    if (DCPYMQT_IsInitialized [dspId] == FALSE) {
        dcpyMqtState = &(DCPYMQT_State [dspId]) ;
        dspCfg       = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj       = dspCfg->dspObject ;
        mqtObject    = &(dspCfg->mqtObjects [mqtId]) ;

        /*  --------------------------------------------------------------------
         *  Initialize the state object fields.
         *  --------------------------------------------------------------------
         */
        dcpyMqtState->dspId       = dspId ;
        dcpyMqtState->mqtId       = mqtId ;
        dcpyMqtState->poolId      = POOL_INVALIDID ;
        dcpyMqtState->ackMsgList  = NULL ;
        dcpyMqtState->dpcObj      = NULL ;
        dcpyMqtState->wordSwap    = dspObj->wordSwap ;
        dcpyMqtState->ipsId       = mqtObject->ipsId ;
        dcpyMqtState->ipsEventNo  = mqtObject->ipsEventNo ;
        dcpyMqtState->dspMaduSize = dspObj->maduSize ;
        dcpyMqtState->memEntry    = mqtObject->memEntry ;

        /*  --------------------------------------------------------------------
         *  Allocate shared memory required for the DCPYMQT component.
         *  --------------------------------------------------------------------
         */
        status = LDRV_SMM_alloc (dspId,
                                 mqtObject->memEntry,
                                 NULL, /* Physical address not required. */
                                 shDspAddr,
                                 (Uint32 *) &(dcpyMqtState->ctrlPtr),
                                 sizeof (DCPYMQT_Ctrl)) ;
        if (DSP_SUCCEEDED (status)) {
            TRC_1PRINT (TRC_LEVEL4,
                        "DCPYMQT Ctrl GPP address [0x%x]\n",
                        dcpyMqtState->ctrlPtr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "DCPYMQT Ctrl DSP address [0x%x]\n",
                        *shDspAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "DCPYMQT Ctrl size [0x%x]\n",
                        sizeof (DCPYMQT_Ctrl)) ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        DCPYMQT_IsInitialized [dspId] = TRUE ;
        if (DSP_FAILED (status)) {
            /* DCPYMQT_exit executes cleanup only if the initialized flag is
             * TRUE.
             */
            DCPYMQT_exit (dspId, mqtId) ;
            DCPYMQT_IsInitialized [dspId] = FALSE ;
        }
        else {
            /* Indicates GPP is yet to call trasnport open. */
            dcpyMqtState->ctrlPtr->isOpen = MQT_EXIT_VALUE ;
        }
    }

    TRC_1LEAVE ("DCPYMQT_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DCPYMQT_exit
 *
 *  @desc   This function finalizes the DCPY MQT.
 *
 *  @modif  DCPYMQT_State [dspId]
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYMQT_exit (IN ProcessorId dspId,
              IN Uint32      mqtId)
{
    DSP_STATUS          status     = DSP_SOK ;
    LINKCFG_Mqt *       mqtObject ;
    DCPYMQT_Object *    dcpyMqtState ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_2ENTER ("DCPYMQT_exit", dspId, mqtId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (DCPYMQT_IsInitialized [dspId] == TRUE) {
        dcpyMqtState = &(DCPYMQT_State [dspId]) ;
        dspCfg       = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        mqtObject    = &(dspCfg->mqtObjects [mqtId]) ;

        /*  --------------------------------------------------------------------
         *  Free shared memory required for the DCPY MQT component.
         *  --------------------------------------------------------------------
         */
        if (dcpyMqtState->ctrlPtr != NULL) {
            status = LDRV_SMM_free (dspId,
                                    mqtObject->memEntry,
                                    sizeof (DCPYMQT_Ctrl)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        /*  --------------------------------------------------------------------
         *  Finalize the state object fields.
         *  --------------------------------------------------------------------
         */
        dcpyMqtState->dspId       = dspId ;
        dcpyMqtState->mqtId       = mqtId ;
        dcpyMqtState->poolId      = POOL_INVALIDID ;
        dcpyMqtState->ackMsgList  = NULL ;
        dcpyMqtState->dpcObj      = NULL ;
        dcpyMqtState->wordSwap    = FALSE ;
        dcpyMqtState->ipsId       = 0 ;
        dcpyMqtState->ipsEventNo  = 0 ;
        dcpyMqtState->dspMaduSize = 0 ;
        dcpyMqtState->ctrlPtr = NULL ;

        DCPYMQT_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("DCPYMQT_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DCPYMQT_open
 *
 *  @desc   This function opens the DCPY MQT and configures it according to
 *          the user attributes.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYMQT_open (IN  LDRV_MSGQ_TransportHandle mqtHandle, IN  Pvoid mqtAttrs)
{
    DSP_STATUS       status  = DSP_SOK ;
    DCPYMQT_Ctrl *   ctrlPtr = NULL ;
    DCPYMQT_Attrs *  attrs   = (DCPYMQT_Attrs *) mqtAttrs ;
    DCPYMQT_Object * dcpyMqtState ;
    ProcessorId      dspId ;

    TRC_2ENTER ("DCPYMQT_open", mqtHandle, mqtAttrs) ;

    DBC_Require (mqtHandle != NULL) ;
    DBC_Require (mqtAttrs  != NULL) ;

    dspId = mqtHandle->dspId ;
    DBC_Assert  (IS_VALID_PROCID (dspId)) ;

    dcpyMqtState = &(DCPYMQT_State [dspId]) ;
    mqtHandle->object = dcpyMqtState ;

    dcpyMqtState->poolId  = attrs->poolId ;

    ctrlPtr = dcpyMqtState->ctrlPtr ;

    status = LIST_Create (&(dcpyMqtState->ackMsgList)) ;
    if (DSP_SUCCEEDED (status)) {
        /* Create the DPC for message transfer */
        status = DPC_Create (DCPYMQT_dpc,
                             dcpyMqtState,
                             &(dcpyMqtState->dpcObj)) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        LDRV_MPLIST_clear (dspId,
                           (List *) &(ctrlPtr->toDspList)) ;
        LDRV_MPLIST_clear (dspId,
                           (List *) &(ctrlPtr->fmDspList)) ;

        /*  Initialize the shared critical section objects */
        status = LDRV_MPCS_create (dspId,
                                   (MPCS_ShObj *) &(ctrlPtr->csToDspList)) ;
        if (DSP_SUCCEEDED (status)) {
            status = LDRV_MPCS_create (dspId,
                                       (MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            status = IPS_register (dspId,
                                   dcpyMqtState->ipsId,
                                   dcpyMqtState->ipsEventNo,
                                   &DCPYMQT_callback,
                                   mqtHandle->object) ;
            if (DSP_SUCCEEDED (status)) {
                status = IPS_setReserved (dspId,
                                          dcpyMqtState->ipsId,
                                          dcpyMqtState->ipsEventNo) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_FAILED (status)) {
        /*  Cleanup allocated objects in case of failure */
        if (mqtHandle->object != NULL) {
            IPS_unregister (dspId,
                            dcpyMqtState->ipsId,
                            dcpyMqtState->ipsEventNo,
                            &DCPYMQT_callback,
                            mqtHandle->object) ;

            IPS_clearReserved (dspId,
                               dcpyMqtState->ipsId,
                               dcpyMqtState->ipsEventNo) ;

            mqtHandle->object = NULL ;

            if (ctrlPtr != NULL) {
                /*  Finalize the shared lists */
                LDRV_MPLIST_clear (dspId,
                                   (List *) &(ctrlPtr->toDspList)) ;
                LDRV_MPLIST_clear (dspId,
                                   (List *) &(ctrlPtr->fmDspList)) ;

                /*  Finalize the shared critical section objects */
                LDRV_MPCS_delete (dspId,
                                  (MPCS_ShObj *) &(ctrlPtr->csToDspList)) ;
                LDRV_MPCS_delete (dspId,
                                  (MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;
            }

            if (dcpyMqtState->dpcObj != NULL) {
                DPC_Delete (dcpyMqtState->dpcObj) ;
            }

            if (dcpyMqtState->ackMsgList != NULL) {
                LIST_Delete (dcpyMqtState->ackMsgList) ;
            }
        }
    }

    if (DSP_SUCCEEDED (status)) {
        ctrlPtr->isOpen = MQT_INIT_VALUE ;
    }

    TRC_1LEAVE ("DCPYMQT_open", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DCPYMQT_close
 *
 *  @desc   This function closes the DCPY MQT, and cleans up its state object.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYMQT_close (IN  LDRV_MSGQ_TransportHandle mqtHandle)
{
    DSP_STATUS         status    = DSP_SOK ;
    DSP_STATUS         tmpStatus = DSP_SOK ;
    MSGQ_Msg           msg       = NULL ;
    DCPYMQT_Ctrl *     ctrlPtr ;
    DCPYMQT_Object *   dcpyMqtState ;
    ProcessorId        dspId ;

    TRC_1ENTER ("DCPYMQT_close", mqtHandle) ;

    DBC_Require (mqtHandle != NULL) ;

    dspId = mqtHandle->dspId ;
    dcpyMqtState = (DCPYMQT_Object *) (mqtHandle->object) ;

    if (dcpyMqtState != NULL) {
        ctrlPtr = dcpyMqtState->ctrlPtr ;
        status = IPS_unregister (dspId,
                                 dcpyMqtState->ipsId,
                                 dcpyMqtState->ipsEventNo,
                                 &DCPYMQT_callback,
                                 mqtHandle->object) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }

        tmpStatus = IPS_clearReserved (dspId,
                                   dcpyMqtState->ipsId,
                                   dcpyMqtState->ipsEventNo) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        if (dcpyMqtState->dpcObj != NULL) {
            /* Cancel the DPC and delete the DPC object */
            tmpStatus = DPC_Cancel (dcpyMqtState->dpcObj) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            tmpStatus = DPC_Delete (dcpyMqtState->dpcObj) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        SYNC_ProtectionStart () ;
        if (dcpyMqtState->ackMsgList != NULL) {
            /* Clear out any existing locateAck messages */
            while (   (LIST_IsEmpty (dcpyMqtState->ackMsgList) != TRUE)
                   && (DSP_SUCCEEDED (tmpStatus))) {
                tmpStatus = LIST_GetHead (dcpyMqtState->ackMsgList,
                                          (ListElement **) &msg) ;
                if ((DSP_SUCCEEDED (tmpStatus)) && (msg != NULL)) {
                    tmpStatus = LDRV_MSGQ_free (msg) ;
                    if (   (DSP_SUCCEEDED (status))
                        && (DSP_FAILED (tmpStatus))) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
            }

            SYNC_ProtectionEnd () ;

            tmpStatus = LIST_Delete (dcpyMqtState->ackMsgList) ;
            if (   (DSP_SUCCEEDED (status))
                && (DSP_FAILED (tmpStatus))) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
        else {
            SYNC_ProtectionEnd () ;
        }

        if (ctrlPtr != NULL) {
            /* Clear out any messages not yet processed */
            while (   !LDRV_MPLIST_isEmpty (dspId,
                                            (List *) &(ctrlPtr->toDspList))
                   && (DSP_SUCCEEDED (status))) {
                LDRV_MPCS_enter ((MPCS_ShObj *) &(ctrlPtr->csToDspList)) ;
                LDRV_MPLIST_getHead  (dspId,
                                        (List *) &(ctrlPtr->toDspList),
                                      (ListElement **) &msg ) ;
                LDRV_MPCS_leave ((MPCS_ShObj *) &(ctrlPtr->csToDspList)) ;
                if (msg != NULL) {
                    status = LDRV_MSGQ_free (msg) ;
                }
            }

            LDRV_MPCS_enter ((MPCS_ShObj *) &(ctrlPtr->csToDspList)) ;
            /*  Finalize the shared lists */
            LDRV_MPLIST_clear (dspId,
                               (List *) &(ctrlPtr->toDspList)) ;
            LDRV_MPCS_leave ((MPCS_ShObj *) &(ctrlPtr->csToDspList)) ;

            while (   !LDRV_MPLIST_isEmpty (dspId,
                                            (List *) &(ctrlPtr->fmDspList))
                   && (DSP_SUCCEEDED (status))) {
                LDRV_MPCS_enter ((MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;
                LDRV_MPLIST_getHead  (dspId,
                                        (List *) &(ctrlPtr->fmDspList),
                                      (ListElement **) &msg ) ;
                LDRV_MPCS_leave ((MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;
                if (msg != NULL) {
                    status = LDRV_MSGQ_free (msg) ;
                }
            }

            LDRV_MPCS_enter ((MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;
            /*  Finalize the shared lists */
            LDRV_MPLIST_clear (dspId,
                               (List *) &(ctrlPtr->fmDspList)) ;
            LDRV_MPCS_leave ((MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;


            /*  Finalize the shared critical section objects */
            tmpStatus = LDRV_MPCS_delete (
                                     dspId,
                                     (MPCS_ShObj *) &(ctrlPtr->csToDspList)) ;
            tmpStatus = LDRV_MPCS_delete (
                                     dspId,
                                     (MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;
            ctrlPtr->isOpen = MQT_EXIT_VALUE ;
        }

        mqtHandle->object = NULL ;
    }

    DBC_Ensure (mqtHandle->object == NULL) ;

    TRC_1LEAVE ("DCPYMQT_close", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DCPYMQT_locate
 *
 *  @desc   This function locates the message queue identified by the specified
 *          MSGQ name
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYMQT_locate (IN     LDRV_MSGQ_TransportHandle mqtHandle,
                IN     Pstr                    queueName,
                IN     Bool                    sync,
                IN OUT MSGQ_Queue *             msgqQueue,
                IN     Pvoid                   locateAttrs)
{
    DSP_STATUS        status       = DSP_SOK ;
    DSP_STATUS        tmpStatus    = DSP_SOK ;
    DCPYMQT_CtrlMsg * msg          = NULL ;
    DCPYMQT_CtrlMsg * ackMsg       = NULL ;
    DCPYMQT_CtrlMsg * nextMsg      = NULL ;
    SyncSemObject *   locateSem    = NULL ;
    DCPYMQT_Object *  dcpyMqtState ;
    SyncAttrs         syncAttrs ;

    TRC_5ENTER ("DCPYMQT_locate",
                mqtHandle,
                queueName,
                sync,
                msgqQueue,
                locateAttrs) ;

    DBC_Require (mqtHandle   != NULL) ;
    DBC_Require (queueName   != NULL) ;
    DBC_Require (locateAttrs != NULL) ;

    dcpyMqtState = (DCPYMQT_Object *) (mqtHandle->object) ;
    DBC_Assert (dcpyMqtState != NULL) ;

    /* Allocate the locate message buffer */
    status = LDRV_MSGQ_alloc (dcpyMqtState->poolId,
                              DCPYMQT_CTRLMSG_SIZE,
                              (MSGQ_Msg *) &msg) ;
    if (DSP_SUCCEEDED (status)) {
        GEN_Strcpyn ((Char8 *) msg->ctrlMsg.locateMsg.msgqName,
                     queueName,
                     DSP_MAX_STRLEN) ;
        msg->msgHeader.dstId = MSGQ_INVALIDMSGQ ;
        msg->msgHeader.msgId = DCPYMQT_CTRLCMD_LOCATE ;
        if (sync == TRUE) {
            /* Create the locate semaphore */
            syncAttrs.flag = (Uint32) SyncSemType_Counting ;
            status = SYNC_CreateSEM (&locateSem, &syncAttrs) ;
            if (DSP_SUCCEEDED (status)) {
                msg->ctrlMsg.locateMsg.semHandle  = (Uint32) locateSem ;
                msg->ctrlMsg.locateMsg.replyQueue = (Uint32) MSGQ_INVALIDMSGQ ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
        else {
            msg->ctrlMsg.locateMsg.semHandle  = (Uint32) NULL ;
            msg->ctrlMsg.locateMsg.replyQueue = (Uint32) *msgqQueue ;

            msg->ctrlMsg.locateMsg.poolId     = ((MSGQ_LocateAsyncAttrs *)
                                                          locateAttrs)->poolId ;

            msg->ctrlMsg.locateMsg.arg        = (Uint32)
                                  ((MSGQ_LocateAsyncAttrs *) locateAttrs)->arg ;
        }

        if (DSP_SUCCEEDED (status)) {
            status = DCPYMQT_send (dcpyMqtState, (Pvoid) msg) ;
            if (status == DSP_ENOTREADY) {
                *msgqQueue = (Uint32) MSGQ_INVALIDMSGQ ;
            }
        }

        if (DSP_SUCCEEDED (status) && (sync == TRUE)) {
            /*  Synchronous location */
            status = SYNC_WaitSEM (locateSem,
                                  ((MSGQ_LocateAttrs *) locateAttrs)->timeout) ;
            if (DSP_SUCCEEDED (status)) {
                SYNC_ProtectionStart () ;
                status = LIST_First (dcpyMqtState->ackMsgList,
                                     (ListElement **) &ackMsg) ;
                /* No check needed since parameters are assured non-NULL */
                DBC_Assert (DSP_SUCCEEDED (status)) ;

                /* If the semaphore was posted without queueing up the
                 * received locateAck message, initialize status to
                 * failure.
                 */
                status = DSP_EFAIL ;
                while ((ackMsg != NULL) && (status == DSP_EFAIL)) {
                    if (    (ackMsg->ctrlMsg.locateAckMsg.semHandle)
                        ==  (Uint32) (locateSem)) {
                        /* Found the locate acknowledgement */
                        *msgqQueue = SWAP_LONG (
                                        ackMsg->ctrlMsg.locateAckMsg.msgqQueue,
                                        dcpyMqtState->wordSwap) ;
                        if (*msgqQueue == (Uint32) MSGQ_INVALIDMSGQ) {
                            /* MSGQ was not found on the DSP. */
                            status = DSP_ENOTFOUND ;
                        }
                        else {
                            /* MSGQ was found on the DSP. */
                            status = DSP_SOK ;
                        }

                        tmpStatus = LIST_RemoveElement (
                                                      dcpyMqtState->ackMsgList,
                                                      (ListElement *) ackMsg) ;
                        if (DSP_FAILED (tmpStatus)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                        LDRV_MSGQ_free ((MSGQ_Msg) ackMsg) ;
                        break ;
                    }
                    else {
                        tmpStatus = LIST_Next (dcpyMqtState->ackMsgList,
                                               (ListElement *)  ackMsg,
                                               (ListElement **) &nextMsg) ;
                        if (DSP_SUCCEEDED (tmpStatus)) {
                            ackMsg = nextMsg ;
                        }
                        else {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
                }

                SYNC_ProtectionEnd () ;
            }
            else {
                /* Assume that the locate msg is reused for ack by the
                 * DCPY MQT on the DSP.
                 * Indicate that the locate request is no longer pending.
                 */
                msg->ctrlMsg.locateMsg.semHandle = (Uint32) NULL ;

                if (   (   ((MSGQ_LocateAttrs *) locateAttrs)->timeout
                             == SYNC_NOWAIT)
                         && (status == SYNC_E_FAIL)) {
                    status = DSP_ENOTCOMPLETE ;
                    SET_FAILURE_REASON ;
                }
            }
        }
        else if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
            /* Free the locate message on failure to send it. */
            LDRV_MSGQ_free ((MSGQ_Msg) msg) ;
        }

        if (locateSem != NULL) {
            tmpStatus = SYNC_DeleteSEM (locateSem) ;
            if (    DSP_FAILED (tmpStatus)
                &&  (status != DSP_SOK)
                &&  (status != DSP_ENOTFOUND)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    /*  Cleanup on failure to locate the MSGQ */
    if (DSP_FAILED (status)) {
        if (sync == TRUE) {
            if (*msgqQueue != (Uint32) MSGQ_INVALIDMSGQ) {
                DCPYMQT_release (mqtHandle, *msgqQueue) ;
            }
            *msgqQueue = MSGQ_INVALIDMSGQ ;
        }
    }

    TRC_1LEAVE ("DCPYMQT_locate", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DCPYMQT_release
 *
 *  @desc   This function releases the MSGQ identified by the MSGQ handle that
 *          was located earlier.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYMQT_release (IN LDRV_MSGQ_TransportHandle mqtHandle, IN MSGQ_Queue msgqQueue)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("DCPYMQT_release", mqtHandle, msgqQueue) ;

    DBC_Require (mqtHandle != NULL) ;
    DBC_Require (msgqQueue != MSGQ_INVALIDMSGQ) ;
    DBC_Assert  (mqtHandle->object != NULL) ;

    /* Nothing to be done for releasing the MSGQ. */

    TRC_1LEAVE ("DCPYMQT_release", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DCPYMQT_put
 *
 *  @desc   This function sends a message to the specified MSGQ.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYMQT_put (IN LDRV_MSGQ_TransportHandle mqtHandle, IN MSGQ_Msg msg)
{
    DSP_STATUS      status = DSP_SOK ;

    TRC_2ENTER ("DCPYMQT_put", mqtHandle, msg) ;

    DBC_Require (mqtHandle  != NULL) ;
    DBC_Require (msg        != NULL) ;

    status = DCPYMQT_send ((DCPYMQT_Object *) mqtHandle->object, msg) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("DCPYMQT_put", status) ;

    return status ;
}


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   DCPYMQT_debug
 *
 *  @desc   This function prints debug information about the MQT and the\
 *          specified message queue.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
DCPYMQT_debug (IN LDRV_MSGQ_TransportHandle mqtHandle)
{
    DCPYMQT_Object *  dcpyMqtState ;

    TRC_1ENTER ("DCPYMQT_debug", mqtHandle) ;

    DBC_Require (mqtHandle != NULL) ;

    if (mqtHandle == NULL) {
        TRC_0PRINT (TRC_LEVEL4,
                    "Invalid argument(s) passed to DCPYMQT_debug ()\n") ;
    }
    else {
        if (DCPYMQT_IsInitialized [mqtHandle->dspId] == TRUE) {
            dcpyMqtState = (DCPYMQT_Object *) (mqtHandle->object) ;
            if (dcpyMqtState == NULL) {
                TRC_0PRINT (TRC_LEVEL4,
                            "The DCPY MQT component is not opened.\n") ;
            }
            else {
                TRC_0PRINT (TRC_LEVEL4, "The DCPY MQT component is opened.\n") ;
                TRC_1PRINT (TRC_LEVEL4, "    dspId          = [%d]\n",
                            dcpyMqtState->dspId) ;
                TRC_1PRINT (TRC_LEVEL4, "    mqtId          = [%d]\n",
                            dcpyMqtState->mqtId) ;
                TRC_1PRINT (TRC_LEVEL4, "    poolId         = [%d]\n",
                            dcpyMqtState->poolId) ;
                TRC_1PRINT (TRC_LEVEL4, "    ackMsgList     = [0x%x]\n",
                            dcpyMqtState->ackMsgList) ;
                TRC_1PRINT (TRC_LEVEL4, "    dpcObj         = [0x%x]\n",
                            dcpyMqtState->wordSwap) ;
                TRC_1PRINT (TRC_LEVEL4, "    wordSwap       = [0x%x]\n",
                            dcpyMqtState->wordSwap) ;
                TRC_1PRINT (TRC_LEVEL4, "    ipsId          = [%d]\n",
                            dcpyMqtState->ipsId) ;
                TRC_1PRINT (TRC_LEVEL4, "    ipsEventNo     = [%d]\n",
                            dcpyMqtState->ipsEventNo) ;
                TRC_1PRINT (TRC_LEVEL4, "    dspMaduSize    = [%d]\n",
                            dcpyMqtState->dspMaduSize) ;
                TRC_1PRINT (TRC_LEVEL4, "    memEntry       = [%d]\n",
                            dcpyMqtState->memEntry) ;
                TRC_1PRINT (TRC_LEVEL4, "    ctrlPtr        = [0x%x]\n",
                            dcpyMqtState->ctrlPtr) ;
            }
        }
        else {
            TRC_1PRINT (TRC_LEVEL4,
                        "DCPYMQT component for DSP ID %d is not initialized.\n",
                        mqtHandle->dspId) ;
        }
    }

    TRC_0LEAVE ("DCPYMQT_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


/** ----------------------------------------------------------------------------
 *  @func   DCPYMQT_callback
 *
 *  @desc   Implements the callback invoked by the SHMIPS component on receiving
 *          a message from the remote processor.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Void
DCPYMQT_callback (IN Uint32 ipsEventNo, IN Pvoid arg, IN Pvoid info)
{
    DSP_STATUS        status  = DSP_SOK ;
    DCPYMQT_Object *  dcpyMqtState ;

    TRC_3ENTER ("DCPYMQT_callback", ipsEventNo, arg, info) ;

    dcpyMqtState = (DCPYMQT_Object *) arg ;
    DBC_Assert (dcpyMqtState != NULL) ;

    status = DPC_Schedule (dcpyMqtState->dpcObj) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_0LEAVE ("DCPYMQT_callback") ;
}


/** ----------------------------------------------------------------------------
 *  @func   DCPYMQT_dpc
 *
 *  @desc   Implements the Deferred Procedure Call for the DCPYMQT.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Void
DCPYMQT_dpc (IN Pvoid refData)
{
    DSP_STATUS            status      = DSP_SOK ;
    DSP_STATUS            tmpStatus   = DSP_SOK ;
    DCPYMQT_Ctrl *        ctrlPtr     = NULL ;
    Uint16                poolId      = POOL_INVALIDID ;
    PoolId                tPoolId ;
    MSGQ_Msg              recvMsg ;
    MSGQ_Queue            msgqQueue ;
    ProcessorId           dspId ;
    Char8                 msgqName [DSP_MAX_STRLEN] ;
    DCPYMQT_Object *      dcpyMqtState ;
    DCPYMQT_CtrlMsg *     msg ;
    MSGQ_Queue            replyQueue ;
    MSGQ_AsyncLocateMsg * newMsg ;

    TRC_1ENTER ("DCPYMQT_dpc", refData) ;

    DBC_Require (refData != NULL) ;

    dcpyMqtState   = (DCPYMQT_Object *) refData ;

    if ((dcpyMqtState == NULL) || (dcpyMqtState->ctrlPtr == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        dspId          = dcpyMqtState->dspId ;
        ctrlPtr        = dcpyMqtState->ctrlPtr ;

        while (   !LDRV_MPLIST_isEmpty (dspId,
                                        (List *) &(ctrlPtr->fmDspList))
               && (DSP_SUCCEEDED (status))) {
            LDRV_MPCS_enter ((MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;
            LDRV_MPLIST_getHead  (dspId,
                                  (List *) &(ctrlPtr->fmDspList),
                                  (ListElement **) &recvMsg ) ;
            LDRV_MPCS_leave ((MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;

            /* Here recvMsg address type is always DSP address */
            status = LDRV_POOL_getPoolId (dspId,
                                          recvMsg,
                                          AddrType_Knl,
                                          &poolId );
            if (DSP_SUCCEEDED (status) && IS_VALID_POOLID (poolId)) {
                LDRV_POOL_invalidate (poolId,
                                      recvMsg,
                                      sizeof (MSGQ_MsgHeader)) ;
                LDRV_POOL_invalidate (poolId, recvMsg, recvMsg->size) ;
                recvMsg->poolId = poolId ;
            }
            else {
                SET_FAILURE_REASON ;
            }

            if (recvMsg != NULL) {
                /* Adjust the message size using the DSP MADU size */
                recvMsg->size = recvMsg->size * dcpyMqtState->dspMaduSize ;
			 /* Adjust the pool Id */
                recvMsg->poolId = POOL_makePoolId (dcpyMqtState->dspId,
                                                   recvMsg->poolId) ;

                /*  ----------------------------------------------------------------
                 *  Control message
                 *  ----------------------------------------------------------------
                 */
                if (recvMsg->dstId == MSGQ_INVALIDMSGQ) {
                    TRC_0PRINT (TRC_LEVEL4, "Received control message\n") ;
                    msg = (DCPYMQT_CtrlMsg *) recvMsg ;
                    /*  ------------------------------------------------------------
                     *  Locate acknowledgement
                     *  ------------------------------------------------------------
                     */
                    if (msg->msgHeader.msgId == DCPYMQT_CTRLCMD_LOCATEACK) {
                        TRC_0PRINT (TRC_LEVEL4, "Locate Ack control message\n") ;
                        if (    msg->ctrlMsg.locateAckMsg.replyQueue
                            ==  (Uint32) MSGQ_INVALIDMSGQ) {
                            /*  ----------------------------------------------------
                             *  Synchronous location
                             *  ----------------------------------------------------
                             */
                            TRC_0PRINT (TRC_LEVEL4, "Synchronous location\n") ;
                            if (   msg->ctrlMsg.locateAckMsg.semHandle
                                != (Uint32) NULL) {
                                status = LIST_PutTail (dcpyMqtState->ackMsgList,
                                                       (ListElement *) msg) ;
                                if (DSP_SUCCEEDED (status)) {
                                    status = SYNC_SignalSEM ((SyncSemObject *)
                                              msg->ctrlMsg.locateAckMsg.semHandle) ;
                                    if (DSP_FAILED (status)) {
                                        SET_FAILURE_REASON ;
                                    }
                                }
                                else {
                                    SET_FAILURE_REASON ;
                                }
                            }
                            else {
                                /* Locate request is no longer pending.
                                 * Free the message.
                                 */
                                LDRV_MSGQ_free (recvMsg) ;
                            }
                        }
                        else {
                            /*  ----------------------------------------------------
                             *  Asynchronous location
                             *  ----------------------------------------------------
                             */
                            TRC_0PRINT (TRC_LEVEL4, "Asynchronous location\n") ;
                            msgqQueue = SWAP_LONG (
                                                msg->ctrlMsg.locateAckMsg.msgqQueue,
                                                dcpyMqtState->wordSwap) ;
                            if (msgqQueue != (Uint32) MSGQ_INVALIDMSGQ) {
                                TRC_0PRINT (TRC_LEVEL4, "MSGQ found on DSP\n") ;
                                /* MSGQ was found on the DSP */
                                replyQueue = (MSGQ_Queue)
                                             msg->ctrlMsg.locateAckMsg.replyQueue ;
                                /* Allocate a message for the locate ack */
                                tPoolId = POOL_makePoolId (
                                                 dcpyMqtState->dspId,
                                                 msg->ctrlMsg.locateAckMsg.poolId) ;
                                status = LDRV_MSGQ_alloc (
                                         tPoolId,
                                         DSPLINK_ALIGN (sizeof(MSGQ_AsyncLocateMsg),
                                                        DSPLINK_BUF_ALIGN),
                                         (MSGQ_Msg *) &newMsg) ;

                                if (DSP_SUCCEEDED (status)) {
                                    newMsg->arg = (Pvoid)
                                                  msg->ctrlMsg.locateAckMsg.arg ;
                                    newMsg->msgqQueue = msgqQueue ;

                                    MSGQ_setMsgId ((MSGQ_Msg) newMsg,
                                                   MSGQ_ASYNCLOCATEMSGID) ;
                                    status = LDRV_MSGQ_put (replyQueue,
                                                            (MSGQ_Msg) newMsg) ;
                                    if (DSP_FAILED (status)) {
                                        SET_FAILURE_REASON ;
                                        LDRV_MSGQ_free ((MSGQ_Msg) newMsg) ;
                                    }
                                }
                            }

                            /* Free the received message */
                            tmpStatus = LDRV_MSGQ_free (recvMsg) ;
                            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                                status = tmpStatus ;
                                SET_FAILURE_REASON ;
                            }
                        }
                    }
                    else if (msg->msgHeader.msgId == DCPYMQT_CTRLCMD_LOCATE) {
                        TRC_0PRINT (TRC_LEVEL4, "Locate control message\n") ;
                        /*  --------------------------------------------------------
                         *  Locate
                         *  --------------------------------------------------------
                         */
                        status = GEN_Strcpyn (msgqName,
                                          (Char8 *) msg->ctrlMsg.locateMsg.msgqName,
                                          DSP_MAX_STRLEN) ;
                        TRC_1PRINT (TRC_LEVEL4, "MSGQ to be located: [%s]\n",
                                    (Char8 *) msgqName) ;
                        /* No check needed since parameters are assured non-NULL */
                        DBC_Assert (DSP_SUCCEEDED (status)) ;

                        status = LDRV_MSGQ_locateLocal ((Pstr) msgqName,
                                                        &msgqQueue) ;
                        if ((DSP_FAILED (status)) && (status != DSP_ENOTFOUND)) {
                            SET_FAILURE_REASON ;
                        }
                        /*  --------------------------------------------------------
                         *  Reuse the received message.
                         *  Since the message is reused, there is no need to copy
                         *  the common fields between the locate and locateAck
                         *  messages in the union.
                         *  Note: The locate protocol assumes that the received
                         *        locate request message will be reused for locate
                         *        ack.
                         *  --------------------------------------------------------
                         */
                        msg->msgHeader.msgId = DCPYMQT_CTRLCMD_LOCATEACK ;
                        msg->ctrlMsg.locateAckMsg.msgqQueue = SWAP_LONG (
                                                           (Uint32) msgqQueue,
                                                           dcpyMqtState->wordSwap) ;

                        /* Send the acknowledgement message to the DSP */
                        status = DCPYMQT_send (dcpyMqtState,
                                               (Pvoid) msg) ;
                        if (DSP_FAILED (status)) {
                            SET_FAILURE_REASON ;
                            /* Free the message on failure to send it to the DSP. */
                            LDRV_MSGQ_free (recvMsg) ;
                        }
                    }
                }
                else {
                    TRC_0PRINT (TRC_LEVEL4, "Received regular message\n") ;
                    /*  ------------------------------------------------------------
                     *  Regular message
                     *  ------------------------------------------------------------
                     */
                    msgqQueue = MSGQ_getDstQueue (recvMsg) ;

#if defined (LOG_DG_MSGQ_QUE)
                    DSPLINKLOG_LogEvent (DG_MSGQ_QUE,
                                         recvMsg->msgId,
                                         (Uint32) msgqQueue,
                                         (Uint32) recvMsg,
                                         recvMsg->size,
                                         0,
                                         0) ;
#endif /* if defined (LOG_DG_MSGQ_QUE) */

                    status = LDRV_MSGQ_put (msgqQueue, recvMsg) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                        LDRV_MSGQ_free (recvMsg) ;
                    }
                }
            }
        }
    }

    TRC_0LEAVE ("DCPYMQT_dpc") ;
}


/** ----------------------------------------------------------------------------
 *  @func   DCPYMQT_send
 *
 *  @desc   Used for sending events.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DCPYMQT_send (IN DCPYMQT_Object * dcpyMqtState,
              IN Pvoid            msg)
{
    DSP_STATUS      status  = DSP_SOK ;
    DCPYMQT_Ctrl *  ctrlPtr = NULL ;
    Uint16          poolId  = POOL_INVALIDID ;
    ProcessorId     dspId ;

    TRC_2ENTER ("DCPYMQT_send", dcpyMqtState, msg) ;

    DBC_Require (dcpyMqtState != NULL) ;
    DBC_Require (msg != NULL) ;

    ctrlPtr = dcpyMqtState->ctrlPtr ;
    dspId   = dcpyMqtState->dspId ;

    /* Adjust the message size using the DSP MADU size */
    ((MSGQ_Msg) msg)->size = ((MSGQ_Msg)msg)->size / dcpyMqtState->dspMaduSize ;

    /*  Send the message to the DSP */
    LDRV_MPCS_enter ((MPCS_ShObj *) &(ctrlPtr->csToDspList)) ;
    LDRV_MPLIST_putTail  (dspId,
                          (List *) &(ctrlPtr->toDspList),
                          (ListElement *) msg) ;

    /* Adjust the pool Id */
    ((MSGQ_Msg) msg)->poolId = POOL_getPoolNo (((MSGQ_Msg) msg)->poolId) ;

    /* The message must belong to some POOL if not  generate assertion */
    status = LDRV_POOL_getPoolId (dspId,
                                  msg,
                                  AddrType_Knl,
                                  &poolId );
    if (IS_VALID_POOLID (poolId)) {
       LDRV_POOL_writeback (poolId, msg, ((MSGQ_Msg) msg)->size) ;
    }

    status = IPS_notify (dspId,
                         dcpyMqtState->ipsId,
                         dcpyMqtState->ipsEventNo,
                         (Uint16) 0,
                         FALSE) ;
    if (DSP_FAILED (status)) {
        /* Remove the queued message */
        LDRV_MPLIST_removeElement  (dspId,
                                    (List *) &(ctrlPtr->toDspList),
                                    (ListElement *) msg) ;
        SET_FAILURE_REASON ;
    }

    LDRV_MPCS_leave ((MPCS_ShObj *) &(ctrlPtr->csToDspList)) ;

    TRC_1LEAVE ("DCPYMQT_send", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
