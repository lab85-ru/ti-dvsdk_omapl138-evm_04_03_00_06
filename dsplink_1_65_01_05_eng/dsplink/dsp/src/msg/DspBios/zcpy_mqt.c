/** ============================================================================
 *  @file   zcpy_mqt.c
 *
 *  @path   $(DSPLINK)/dsp/src/msg/DspBios/
 *
 *  @desc   Implementation of the ZCPY MQT for GPP-DSP communication.
 *          This module implements the transport interface as required by the
 *          MSGQ model of DSP/BIOS(TM).
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
#include <hwi.h>
#include <swi.h>
#if defined (DSP_TSK_MODE)
#include <tsk.h>
#endif /* if defined (DSP_TSK_MODE) */
#include <sem.h>
#include <mem.h>
#include <que.h>
#include <string.h>
#include <msgq.h>
#include <msgq_mqt.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <dbc.h>
#include <failure.h>
#include <hal_cache.h>
#include <_dsplink.h>
#include <mpcs.h>
#include <_mpcs.h>
#include <ips.h>
#include <dsplinkmqt.h>
#include <zcpy_mqt.h>
#include <_zcpy_mqt.h>

/*  ----------------------------------- LOG  Headers                 */
#if defined (LOG_COMPONENT)
#include <linklog.h>
#endif /* if defined (LOG_COMPONENT) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define FILEID               FID_ZCPYMQT_C

/** ============================================================================
 *  @const  ZCPYMQT_MEM_ALIGN
 *
 *  @desc   Default alignment to be used for internal memory allocations.
 *  ============================================================================
 */
#define ZCPYMQT_MEM_ALIGN  0

/** ============================================================================
 *  @const  ISR_DISABLE
 *
 *  @desc   To disable interrupts.
 *  ============================================================================
 */
#define ISR_DISABLE  HWI_disable()

/** ============================================================================
 *  @const  ISR_RESTORE
 *
 *  @desc   To restore interrupts.
 *  ============================================================================
 */
#define ISR_RESTORE(key)  HWI_restore(key)

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
 *  @name   ZCPYMQT_State
 *
 *  @desc   Defines the ZCPYMQT state object, which contains all the
 *          component-specific information.
 *
 *  @field  poolId
 *              Pool ID used for allocating control messages.
 *  @field  ackMsgQueue
 *              Queue of locateAck messages received from the GPP.
 *  @field  ipsId
 *              IPS ID associated with MQT.
 *  @field  ipsEventNo
 *              IPS Event no associated with MQT.
 *  @field  ctrlPtr
 *              Pointer to the ZCPYMQT control structure in shared memory.
 *  @field  tskHandle
 *              TSK handler.
 *              Only defined if DSP_TSK_MODE is enabled.
 *  @field  zcpyMqtSem
 *              Semaphore object for ZCPYMQT.
 *  @field  swiHandle
 *              SWI for processing of locate functionality in non-ISR context.
 *              Only defined if DSP_SWI_MODE is enabled.
 *  ============================================================================
 */
typedef struct ZCPYMQT_State_tag {
    Uint16              poolId ;
    QUE_Obj             ackMsgQueue ;
    Uint32              ipsId ;
    Uint32              ipsEventNo ;
    ZCPYMQT_Ctrl *      ctrlPtr ;
#if defined (DSP_TSK_MODE)
    TSK_Handle          tskHandle ;
    SEM_Obj             zcpyMqtSem ;
#else /* if defined (DSP_TSK_MODE) */
    SWI_Handle          swiHandle ;
#endif /* if defined (DSP_TSK_MODE) */
} ZCPYMQT_State ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_open
 *
 *  @desc   Opens the ZCPY MQT and configures it according to the user
 *          attributes.
 *
 *  @arg    mqtHandle
 *              Handle to the MSGQ transport object.
 *
 *  @ret    SYS_OK
 *              Component successfully opened.
 *          SYS_EALLOC
 *              Failure during memory operation.
 *
 *  @enter  The handle to the MSGQ transport object must be valid.
 *
 *  @leave  None.
 *
 *  @see    ZCPYMQT_Params, ZCPYMQT_State, ZCPYMQT_close ()
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYMQT_open (MSGQ_TransportHandle mqtHandle) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_close
 *
 *  @desc   Closes the ZCPY MQT, and cleans up its state object.
 *
 *  @arg    mqtHandle
 *              Handle to the MSGQ transport object.
 *
 *  @ret    SYS_OK
 *              Component successfully opened.
 *          SYS_EFREE
 *              Failure during memory operation.
 *
 *  @enter  The handle to the MSGQ transport object must be valid.
 *
 *  @leave  The object field in the MSGQ transport object is set to NULL.
 *
 *  @see    ZCPYMQT_State, ZCPYMQT_open ()
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYMQT_close (MSGQ_TransportHandle mqtHandle) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_put
 *
 *  @desc   Sends a message to the specified remote MSGQ.
 *
 *  @arg    mqtHandle
 *              Handle to the MSGQ transport object.
 *  @arg    msg
 *              Pointer to the message to be sent.
 *
 *  @ret    SYS_OK
 *              The message has been successfully sent.
 *
 *  @enter  The handle to the MSGQ transport object must be valid.
 *          The pointer to the message must be valid.
 *
 *  @leave  None
 *
 *  @see    ZCPYMQT_callback ()
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYMQT_put (MSGQ_TransportHandle mqtHandle, MSGQ_Msg msg) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_locate
 *
 *  @desc   Attempts to locate a message queue present on the remote processor.
 *          The message queue to be located is identified by its system-wide
 *          unique name.
 *
 *  @arg    mqtHandle
 *              Handle to the MSGQ transport object.
 *  @arg    queueName
 *              Name of the MSGQ to be located.
 *  @arg    sync
 *              Indicates whether the locate is synchronous or asynchronous.
 *  @arg    msgqQueue
 *              Location to receive the handle to the located message queue.
 *  @arg    locateAttrs
 *              Attributes for location of the MSGQ.
 *
 *  @ret    SYS_OK
 *              The message queue has been successfully located.
 *          SYS_ENOTFOUND
 *              The message queue does not exist on the remote processor.
 *          SYS_ETIMEOUT
 *              Timeout during location of the MSGQ.
 *          SYS_EALLOC
 *              Failure during memory operation.
 *
 *  @enter  The handle to the MSGQ transport object must be valid.
 *          The queueName must be valid.
 *          The locateAttrs must be valid.
 *          The msgqQueue must be valid depending on whether the locate is
 *          synchronous or asynchronous
 *
 *  @leave  On success, for synchronous locate, the *msgqQueue is set to a
 *          valid value.
 *          On failure, for synchronous locate, the *msgqQueue is set to an
 *          invalid value.
 *
 *  @see    ZCPYMQT_release ()
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYMQT_locate (MSGQ_TransportHandle mqtHandle,
                String               queueName,
                Bool                 sync,
                MSGQ_Queue *         msgqQueue,
                Ptr                  locateAttrs) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_release
 *
 *  @desc   This function releases the MSGQ located earlier.
 *
 *  @arg    mqtHandle
 *              Handle to the MSGQ transport object.
 *  @arg    msgqQueue
 *              Handle to the MSGQ to be released.
 *
 *  @ret    SYS_OK
 *              The message queue has been successfully released.
 *
 *  @enter  The handle to the MSGQ transport object must be valid.
 *          The handle to the message queue must be valid.
 *
 *  @leave  None.
 *
 *  @see    ZCPYMQT_locate ()
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYMQT_release (MSGQ_TransportHandle mqtHandle, MSGQ_Queue msgqQueue) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_callback
 *
 *  @desc   Implements the callback invoked by the SHMIPS component on receiving
 *          a message from the remote processor.
 *
 *  @arg    eventNo
 *              Event number associated with the callback being invoked.
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
static
Void
ZCPYMQT_callback (Uint32 eventNo, Ptr arg, Ptr info) ;

#if defined (DSP_SWI_MODE)
/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_swiFxn
 *
 *  @desc   Implements the SWI function for the ZCPYMQT.
 *
 *  @arg    arg0
 *              First argument specified while creating the SWI.
 *  @arg    arg1
 *              Second argument specified while creating the SWI.
 *
 *  @ret    None.
 *
 *  @enter  arg0 must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static
Void
ZCPYMQT_swiFxn (Arg arg0, Arg arg1) ;
#endif /* if defined (DSP_SWI_MODE) */


#if defined (DSP_TSK_MODE)
/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_tskFxn
 *
 *  @desc   Implements the TSK function for the ZCPYMQT.
 *
 *  @arg    arg0
 *              First argument specified while creating the TSK.
 *  @ret    None.
 *
 *  @enter  arg0 must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static
Void
ZCPYMQT_tskFxn(Arg arg0) ;
#endif /* if defined (DSP_TSK_MODE) */


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_msgCtrl
 *
 *  @desc   Implements the message control function for the SWI and TSK.
 *
 *  @arg    mqtState
 *              Pointer to ZCPYMQT state object.
 *
 *  @ret    None.
 *
 *  @enter  mqtState must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static
Void
ZCPYMQT_msgCtrl (ZCPYMQT_State * mqtState) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_send
 *
 *  @desc   Sends a message to the specified remote MSGQ.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYMQT_send (ZCPYMQT_State * mqtState, Void * bufPtr) ;


/** ============================================================================
 *  @name   ZCPYMQT_FXNS
 *
 *  @desc   ZCPY MQT function pointer table.
 *  ============================================================================
 */
MSGQ_TransportFxns ZCPYMQT_FXNS = {
    &ZCPYMQT_open,
    &ZCPYMQT_close,
    &ZCPYMQT_locate,
    &ZCPYMQT_release,
    &ZCPYMQT_put
} ;


/** ============================================================================
 *  @name   ZCPYMQT_PARAMS
 *
 *  @desc   Default parameters structure for the ZCPY MQT.
 *  ============================================================================
 */
static ZCPYMQT_Params ZCPYMQT_PARAMS = {0} ;  /* Default Pool ID   */

/** ============================================================================
 *  @name   DSPLINKMQT_ctrlPtr
 *
 *  @desc   Declaration of pointer to the shared memory control structure for
 *          the DSPLINKMQT component.
 *  ============================================================================
 */
extern DSPLINKMQT_Ctrl * DSPLINKMQT_ctrlPtr ;


/** ============================================================================
 *  Create named sections for all ZCPYMQT functions to allow specific memory
 *  placement.
 *  ============================================================================
 */
#pragma CODE_SECTION (ZCPYMQT_init,        ".text:DSPLINK_init")
#pragma CODE_SECTION (ZCPYMQT_open,        ".text:ZCPYMQT_open")
#pragma CODE_SECTION (ZCPYMQT_close,       ".text:ZCPYMQT_close")

#pragma CODE_SECTION (ZCPYMQT_locate,      ".text:ZCPYMQT_locate")
#pragma CODE_SECTION (ZCPYMQT_release,     ".text:ZCPYMQT_release")

#pragma CODE_SECTION (ZCPYMQT_put,         ".text:ZCPYMQT_msgXfer")
#pragma CODE_SECTION (ZCPYMQT_callback,    ".text:ZCPYMQT_msgXfer")


/** ============================================================================
 *  @func   ZCPYMQT_init
 *
 *  @desc   Performs global initialization of the ZCPY MQT.
 *
 *  @modif  None
 *  ============================================================================
 */
Void
ZCPYMQT_init  ()
{
    _DSPLINK_init () ;
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_open
 *
 *  @desc   Opens the ZCPY MQT and configures it according to the user
 *          attributes.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYMQT_open (MSGQ_TransportHandle mqtHandle)
{
    Int               status    = SYS_OK ;
    ZCPYMQT_Params *  params    = (ZCPYMQT_Params *) mqtHandle->params ;
    ZCPYMQT_State *   mqtState ;
#if defined (DSP_TSK_MODE)
    TSK_Attrs         tskAttrs     = TSK_ATTRS ;
#else /* if defined (DSP_TSK_MODE) */
    SWI_Attrs         swiAttrs  = SWI_ATTRS;
#endif /* if defined (DSP_TSK_MODE) */
    MPCS_Handle       temp ;

    DBC_require (mqtHandle != NULL) ;

    mqtHandle->object = NULL ;

    /*  If no attributes are specified, use the default ones. */
    if (params == NULL) {
        params = (ZCPYMQT_Params *) &ZCPYMQT_PARAMS ;
    }

    /*  Allocate and initialize the MQT state object */
    mqtState = MEM_calloc (DSPLINK_SEGID,
                           sizeof (ZCPYMQT_State),
                           ZCPYMQT_MEM_ALIGN) ;
    if (mqtState != MEM_ILLEGAL) {
        mqtHandle->object     = (Ptr) mqtState ;
        mqtState->poolId      = params->poolId ;
        mqtState->ipsId       = DSPLINKMQT_ctrlPtr->config.ipsId ;
        mqtState->ipsEventNo  = DSPLINKMQT_ctrlPtr->config.ipsEventNo ;
        mqtState->ctrlPtr     = (ZCPYMQT_Ctrl *) DSPLINKMQT_ctrlPtr->dspAddr ;

        /* wait for GPP to open its trasnport. */
        do {
            HAL_cacheInv ((Ptr) &mqtState->ctrlPtr->isOpen, sizeof (Uint16)) ;
        } while (mqtState->ctrlPtr->isOpen != MQT_INIT_VALUE) ;

        QUE_new (&(mqtState->ackMsgQueue)) ;


#if defined (DSP_TSK_MODE)
        SEM_new (&(mqtState->zcpyMqtSem), 0) ;

        if (DSPLINKMQT_ctrlPtr->config.arg1 != 0) {

            if (   (DSPLINKMQT_ctrlPtr->config.arg1 > TSK_MINPRI)
                && (DSPLINKMQT_ctrlPtr->config.arg1 <= TSK_MAXPRI)) {
                tskAttrs.priority  = DSPLINKMQT_ctrlPtr->config.arg1 ;
            }
            else {
                status = SYS_EINVAL ;
                SET_FAILURE_REASON (status) ;
            }
        }
        else {
            tskAttrs.priority  = 15 ;
        }

        if (status == SYS_OK) {
            if (DSPLINKMQT_ctrlPtr->config.arg2 != 0) {
                tskAttrs.stacksize = DSPLINKMQT_ctrlPtr->config.arg2 ;
            }

            tskAttrs.name       = "DSPLINK_ZCPYMQT" ;
            mqtState->tskHandle = TSK_create((Fxn)ZCPYMQT_tskFxn,
                                             &tskAttrs,
                                             mqtHandle->object) ;
            if (mqtState->tskHandle == NULL) {
                status = SYS_EALLOC ;
                SET_FAILURE_REASON (status) ;
            }
        }
#else /* if defined (DSP_TSK_MODE) */
        swiAttrs.fxn  = ZCPYMQT_swiFxn ;
        swiAttrs.arg0 = (Arg) mqtHandle->object ;
        mqtState->swiHandle = SWI_create (&swiAttrs) ;
        if (mqtState->swiHandle == NULL) {
            status = SYS_EALLOC ;
            SET_FAILURE_REASON (status) ;
        }
#endif /* if defined (DSP_TSK_MODE) */
    }
    else {
        status = SYS_EALLOC ;
        SET_FAILURE_REASON (status) ;
    }

    if (status == SYS_OK) {
        IPS_register (ID_GPP,
                      mqtState->ipsId,
                      mqtState->ipsEventNo,
                      &ZCPYMQT_callback,
                      mqtHandle->object) ;
        DBC_assert (status == SYS_OK) ;

        if (status == SYS_OK) {
            status  = IPS_setReserved (ID_GPP,
                                       mqtState->ipsId,
                                       mqtState->ipsEventNo) ;
            DBC_assert (status == SYS_OK) ;

            /* The MPCS object is created by the GPP-side. */
            status = _MPCS_open (ID_GPP,
                                 MPCS_RESV_LOCKNAME,
                                 &temp,
                                 (MPCS_ShObj *) &(mqtState->ctrlPtr->csToDspList)) ;
            /* The call will always be successful since parameters are non-NULL. */
            DBC_assert (status == SYS_OK) ;

            status = _MPCS_open (ID_GPP,
                                 MPCS_RESV_LOCKNAME,
                                 &temp,
                                 (MPCS_ShObj *) &(mqtState->ctrlPtr->csFmDspList)) ;
            /* The call will always be successful since parameters are non-NULL. */
            DBC_assert (status == SYS_OK) ;
        }
    }

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_close
 *
 *  @desc   Closes the ZCPY MQT, and cleans up its state object.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYMQT_close (MSGQ_TransportHandle mqtHandle)
{
    Int               status  = SYS_OK ;
    QUE_Handle        queHandle ;
    ZCPYMQT_State *   mqtState ;
    MSGQ_Msg          msg ;

    DBC_require (mqtHandle != NULL) ;

    mqtState = (ZCPYMQT_State *) mqtHandle->object ;
    DBC_assert (mqtState != NULL) ;

    IPS_unregister (ID_GPP,
                    mqtState->ipsId,
                    mqtState->ipsEventNo,
                    &ZCPYMQT_callback,
                    mqtHandle->object) ;

    IPS_clearReserved (ID_GPP,
                       mqtState->ipsId,
                       mqtState->ipsEventNo) ;

    queHandle = &(mqtState->ackMsgQueue) ;
    /* Clear out any existing locateAck messages */
    while ((msg = QUE_get (queHandle)) != (MSGQ_Msg) queHandle) {
        MSGQ_free (msg) ;
    }

#if defined (DSP_TSK_MODE)
    if (mqtState->tskHandle != NULL) {
        TSK_delete (mqtState->tskHandle) ;
    }
#else /* if defined (DSP_TSK_MODE) */
    if (mqtState->swiHandle != NULL) {
        SWI_delete (mqtState->swiHandle) ;
    }
#endif /* if defined (DSP_TSK_MODE) */

    if (!MEM_free (DSPLINK_SEGID, mqtState, sizeof (ZCPYMQT_State))) {
        status = SYS_EFREE ;
        SET_FAILURE_REASON (status) ;
    }

    mqtHandle->object = NULL ;

    DBC_ensure (mqtHandle->object == NULL) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_put
 *
 *  @desc   Sends a message to the specified remote MSGQ.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYMQT_put (MSGQ_TransportHandle mqtHandle, MSGQ_Msg msg)
{
    ZCPYMQT_State *    mqtState ;

#if defined (LOG_DG_MSGQ_PUT)
    Uint32 tmp ;
#endif /* if defined (LOG_DG_MSGQ_PUT) */

    DBC_require (mqtHandle != NULL) ;
    DBC_require (msg       != NULL) ;

    mqtState    = (ZCPYMQT_State *) mqtHandle->object ;

#if defined (LOG_DG_MSGQ_PUT)
    tmp = ((Uint32) ID_GPP << 16) | msg->dstId ;
    DSPLINKLOG_logEvent (DG_MSGQ_PUT,
                         msg->msgId,
                         tmp,
                         (Uint32) msg,
                         msg->size,
                         0,
                         0) ;
#endif /* if defined (LOG_DG_MSGQ_PUT) */

    HAL_cacheWbInv ((Ptr) msg, msg->size) ;

    ZCPYMQT_send (mqtState, (Ptr) msg) ;

    return SYS_OK ;
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_locate
 *
 *  @desc   Attempts to locate a message queue present on the remote processor.
 *          The message queue to be located is identified by its system-wide
 *          unique name.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYMQT_locate (MSGQ_TransportHandle mqtHandle,
                String               queueName,
                Bool                 sync,
                MSGQ_Queue *         msgqQueue,
                Ptr                  locateAttrs)
{
    ZCPYMQT_CtrlMsg * msg    = NULL ;
    Int               status = SYS_OK ;
    ZCPYMQT_State *   mqtState ;
    SEM_Obj           locateSem ;
    Uns               key ;
    Uint32            length ;

    DBC_require (mqtHandle   != NULL) ;
    DBC_require (queueName   != NULL) ;
    DBC_require (locateAttrs != NULL) ;

    mqtState = (ZCPYMQT_State *) mqtHandle->object ;
    DBC_assert (mqtState != NULL) ;

    length = strlen (queueName) ;
    if (length >= DSP_MAX_STRLEN) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        /* Allocate the locate message buffer */
        status = MSGQ_alloc (mqtState->poolId,
                             (MSGQ_Msg *) &msg,
                             ZCPYMQT_CTRLMSG_SIZE) ;
        if (status == SYS_OK) {
            strncpy ((Char *) msg->ctrlMsg.locateMsg.msgqName,
                     queueName,
                     DSP_MAX_STRLEN) ;
            msg->msgHeader.dstId = MSGQ_INVALIDMSGQ ;
            msg->msgHeader.msgId = ZCPYMQT_CTRLCMD_LOCATE ;
            if (sync == TRUE) {
                SEM_new (&locateSem, 0) ;
                msg->ctrlMsg.locateMsg.semHandle  = (Uint32) &locateSem ;
                msg->ctrlMsg.locateMsg.replyQueue = (Uint32) MSGQ_INVALIDMSGQ ;
            }
            else {
                msg->ctrlMsg.locateMsg.semHandle  = NULL ;
                msg->ctrlMsg.locateMsg.replyQueue = (Uint32) *msgqQueue ;
                msg->ctrlMsg.locateMsg.poolId     = ((MSGQ_LocateAsyncAttrs *)
                                                              locateAttrs)->poolId ;
                msg->ctrlMsg.locateMsg.arg        = (Uint32)
                                      ((MSGQ_LocateAsyncAttrs *) locateAttrs)->arg ;
            }

            HAL_cacheWbInv ((Ptr) msg, msg->msgHeader.size) ;

            status = ZCPYMQT_send (mqtState, (Ptr) msg) ;
            if (status == SYS_ENODEV) {
                *msgqQueue = (Uint32) MSGQ_INVALIDMSGQ ;
                MSGQ_free ((MSGQ_Msg) msg) ;
            }

            if ((sync == TRUE) && (status == SYS_OK)) {
                /*  Synchronous location */
                if (    SEM_pend (&locateSem,
                                  ((MSGQ_LocateAttrs *) locateAttrs)->timeout)
                    ==  TRUE) {
                    msg = QUE_head (&(mqtState->ackMsgQueue)) ;

                    /* If the semaphore was posted without queueing up the
                     * received locateAck message, initialize status to failure.
                     */
                    status = SYS_EBADIO ;
                    while ((QUE_Handle) msg != &(mqtState->ackMsgQueue)) {
                        if (    (msg->ctrlMsg.locateAckMsg.semHandle)
                            ==  (Uint32) (&locateSem)) {
                            /* Found the MSGQ */
                            *msgqQueue = msg->ctrlMsg.locateAckMsg.msgqQueue ;
                            if (*msgqQueue == (Uint32) MSGQ_INVALIDMSGQ) {
                                /* MSGQ was not found on the GPP. */
                                status = SYS_ENOTFOUND ;
                            }
                            else {
                                /* MSGQ was found on the GPP. */
                                status = SYS_OK ;
                            }
                            key = ISR_DISABLE ;
                            QUE_remove (msg) ;
                            ISR_RESTORE (key) ;
                            MSGQ_free ((MSGQ_Msg) msg) ;
                            break ;
                        }
                        else {
                            msg = QUE_next (msg) ;
                        }
                    }
                }
                else {
                    status = SYS_ETIMEOUT ;
                    SET_FAILURE_REASON (status) ;
                    /* Assume that the locate msg is reused for ack by the
                     * ZCPY MQT on the GPP.
                     * Indicate that the locate request is no longer pending.
                     */
                    HAL_cacheInv ((Ptr) msg, msg->msgHeader.size) ;
                    msg->ctrlMsg.locateMsg.semHandle = (Uint32) NULL ;
                    HAL_cacheWbInv ((Ptr) msg, msg->msgHeader.size) ;
                }
            }
        }
    }

    /*  Cleanup on failure to locate the MSGQ */
    if (status != SYS_OK) {
        if (sync == TRUE) {
            if (*msgqQueue != (Uint32) MSGQ_INVALIDMSGQ) {
                ZCPYMQT_release (mqtHandle, *msgqQueue) ;
            }
            *msgqQueue = (Uint32) MSGQ_INVALIDMSGQ ;
        }
    }

    DBC_ensure (    (sync == FALSE)
                ||  (   (status     == SYS_OK)
                     && (sync       == TRUE)
                     && (*msgqQueue != (Uint32) MSGQ_INVALIDMSGQ))
                ||  (status != SYS_OK)) ;

    DBC_ensure (    (sync == FALSE)
                ||  (   (status      != SYS_OK)
                     && (sync        == TRUE)
                     && (*msgqQueue  == (Uint32) MSGQ_INVALIDMSGQ))
                ||  (status == SYS_OK)) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_release
 *
 *  @desc   This function releases the MSGQ located earlier.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYMQT_release (MSGQ_TransportHandle mqtHandle, MSGQ_Queue msgqQueue)
{
    DBC_require (mqtHandle != NULL) ;
    DBC_require (msgqQueue != (Uint32) MSGQ_INVALIDMSGQ) ;

    (Void) mqtHandle ; /* To avoid compiler warning */
    (Void) msgqQueue ; /* To avoid compiler warning */

    /* Nothing to be done for releasing the MSGQ. */

    return SYS_OK ;
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_callback
 *
 *  @desc   Implements the callback invoked by the SHMIPS component on receiving
 *          a message from the remote processor.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
static
Void
ZCPYMQT_callback (Uint32 eventNo, Ptr arg, Ptr info)
{
    ZCPYMQT_State * mqtState = (ZCPYMQT_State *) arg ;

    (void) eventNo ;
    (void) info ;

    DBC_assert (mqtState != NULL) ;

#if defined (DSP_TSK_MODE)
    SEM_post (&(mqtState->zcpyMqtSem)) ;
#else /* if defined (DSP_TSK_MODE) */
    SWI_post (mqtState->swiHandle) ;
#endif /* if defined (DSP_TSK_MODE) */
}


#if defined (DSP_SWI_MODE)
/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_swiFxn
 *
 *  @desc   Implements the SWI function for the ZCPYMQT.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
static
Void
ZCPYMQT_swiFxn (Arg arg0, Arg arg1)
{
    ZCPYMQT_State *         mqtState ;

    DBC_require (arg0 != NULL) ;

    (Void) arg1 ;
    mqtState = (ZCPYMQT_State *) arg0 ;

    DBC_require (mqtState != NULL) ;

    ZCPYMQT_msgCtrl (mqtState) ;

}
#endif /* if defined (DSP_SWI_MODE) */


#if defined (DSP_TSK_MODE)
/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_tskFxn
 *
 *  @desc   Implements the TSK function for the ZCPYMQT.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
static
Void
ZCPYMQT_tskFxn(Arg arg0)
{
    ZCPYMQT_State *         mqtState ;
    Bool i = 1 ;

    DBC_require (arg0 != NULL) ;

    mqtState = (ZCPYMQT_State *) arg0 ;

    DBC_require (mqtState != NULL) ;

    while (i) {
        SEM_pend(&(mqtState->zcpyMqtSem), SYS_FOREVER) ;
        ZCPYMQT_msgCtrl (mqtState) ;
    }
}
#endif /* if defined (DSP_TSK_MODE) */


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_msgCtrl
 *
 *  @desc   Implements the message control for TSK and SWI function.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
static
Void
ZCPYMQT_msgCtrl (ZCPYMQT_State * mqtState)
{
    Int                     status = SYS_OK ;
    MSGQ_Queue              msgqQueue ;
    MSGQ_Msg                recvMsg ;
    QUE_Elem *              ptr ;
    QUE_Elem *              tmpPtr ;
    ZCPYMQT_Ctrl *          ctrlPtr ;
    ZCPYMQT_CtrlMsg *       msg ;
    MSGQ_Queue              replyQueue ;
    MSGQ_AsyncLocateMsg *   newMsg ;

    ctrlPtr = mqtState->ctrlPtr ;

    HAL_cacheInv ((Ptr) &(ctrlPtr->toDspList), sizeof (ctrlPtr->toDspList)) ;
    while (!QUE_empty (&(ctrlPtr->toDspList))) {
        /* No special handling is needed for the case where no message is
         * received from the SHMIPS.
         */
        MPCS_enter ((MPCS_Handle) &(ctrlPtr->csToDspList)) ;

        /* Ensure that all three queue elements involved in QUE_get () are
         * invalidated.
         */
        HAL_cacheInv ((Ptr) &(ctrlPtr->toDspList),sizeof (ctrlPtr->toDspList)) ;
        tmpPtr = QUE_next (&(ctrlPtr->toDspList)) ;
        HAL_cacheInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;
        tmpPtr = QUE_next (tmpPtr) ;
        HAL_cacheInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;

        ptr = QUE_get ((QUE_Handle) &(ctrlPtr->toDspList)) ;

        /* Ensure that both queue elements involved in QUE_get () are
         * written back.
         */
        HAL_cacheWbInv ((Ptr) &(ctrlPtr->toDspList),
                        sizeof (ctrlPtr->toDspList)) ;
        HAL_cacheWbInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;

        MPCS_leave ((MPCS_Handle) &(ctrlPtr->csToDspList)) ;

        if (ptr != (QUE_Handle) &(ctrlPtr->toDspList)) {
            recvMsg = (MSGQ_Msg) ptr ;

            HAL_cacheInv ((Ptr) recvMsg, recvMsg->size) ;
            /*  ----------------------------------------------------------------
             *  Control message
             *  ----------------------------------------------------------------
             */
            if (recvMsg->dstId == MSGQ_INVALIDMSGQ) {
                /*  ------------------------------------------------------------
                 *  Locate acknowledgement
                 *  ------------------------------------------------------------
                 */
                msg = (ZCPYMQT_CtrlMsg *) recvMsg ;
                if (msg->msgHeader.msgId == ZCPYMQT_CTRLCMD_LOCATEACK) {
                    if (    msg->ctrlMsg.locateAckMsg.replyQueue
                        ==  (Uint32) MSGQ_INVALIDMSGQ) {
                        /*  ----------------------------------------------------
                         *  Synchronous location
                         *  ----------------------------------------------------
                         */
                        if (msg->ctrlMsg.locateAckMsg.semHandle != NULL) {
                            QUE_put (&(mqtState->ackMsgQueue), (Ptr) msg) ;
                            SEM_post ((SEM_Handle)
                                       msg->ctrlMsg.locateAckMsg.semHandle) ;
                        }
                        else {
                            /* Locate request is no longer pending. Free the
                             * message.
                             */
                            MSGQ_free (recvMsg) ;
                        }
                    }
                    else {
                        /*  ----------------------------------------------------
                         *  Asynchronous location
                         *  ----------------------------------------------------
                         */
                        if (    msg->ctrlMsg.locateAckMsg.msgqQueue
                            !=  MSGQ_INVALIDMSGQ) {
                            /* MSGQ was found on the GPP */
                            replyQueue = (MSGQ_Queue)
                                         msg->ctrlMsg.locateAckMsg.replyQueue ;

                            /* Check if the local MSGQ exists */
                            if (MSGQ_exist (replyQueue) == TRUE) {
                                /* Allocate a message for the locate ack */
                                status = MSGQ_alloc (
                                    msg->ctrlMsg.locateAckMsg.poolId,
                                    (MSGQ_Msg *) &newMsg,
                                    DSPLINK_ALIGN (sizeof (MSGQ_AsyncLocateMsg),
                                    DSPLINK_BUF_ALIGN)) ;

                                if (status == SYS_OK) {
                                    newMsg->arg = (Arg)
                                                 msg->ctrlMsg.locateAckMsg.arg ;
                                    newMsg->msgqQueue =
                                           msg->ctrlMsg.locateAckMsg.msgqQueue ;

                                    MSGQ_setMsgId ((MSGQ_Msg) newMsg,
                                                   MSGQ_ASYNCLOCATEMSGID) ;
                                    status = MSGQ_put (replyQueue,
                                                       (MSGQ_Msg) newMsg) ;
                                    DBC_assert (status == SYS_OK) ;
                                    if (status != SYS_OK) {
                                        MSGQ_free ((MSGQ_Msg) newMsg) ;
                                    }
                                }
                            }
                        }

                        /* Free the received message */
                        MSGQ_free (recvMsg) ;
                    }
                }
                else if (msg->msgHeader.msgId == ZCPYMQT_CTRLCMD_LOCATE) {
                    /*  --------------------------------------------------------
                     *  Locate
                     *  --------------------------------------------------------
                     */
                    status = MSGQ_locateLocal ((String)
                                                msg->ctrlMsg.locateMsg.msgqName,
                                               &msgqQueue) ;

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
                    msg->msgHeader.msgId = ZCPYMQT_CTRLCMD_LOCATEACK ;
                    if (status == SYS_OK) {
                        /* MSGQ exists */
                        msg->ctrlMsg.locateAckMsg.msgqQueue = msgqQueue ;
                    }
                    else if (status == SYS_ENOTFOUND) {
                        /* MSGQ does not exist */
                        msg->ctrlMsg.locateAckMsg.msgqQueue = (Uint32)
                                                              MSGQ_INVALIDMSGQ ;
                        status = SYS_OK ;
                    }

                    /* Send the acknowldgement message to the GPP */
                    HAL_cacheWbInv ((Ptr) msg, msg->msgHeader.size) ;
                    ZCPYMQT_send (mqtState, (Ptr) msg) ;
                }
            }
            else {
                /*  ------------------------------------------------------------
                 *  Regular message
                 *  ------------------------------------------------------------
                 */
                MSGQ_getDstQueue (recvMsg, &msgqQueue) ;

#if defined (LOG_GD_MSGQ_QUE)
                DSPLINKLOG_logEvent (GD_MSGQ_QUE,
                                     recvMsg->msgId,
                                     (Uint32) msgqQueue,
                                     (Uint32) recvMsg,
                                     recvMsg->size,
                                     0,
                                     0) ;
#endif /* if defined (LOG_GD_MSGQ_QUE) */
                /* Check if the local MSGQ exists */
                if (MSGQ_exist (msgqQueue) == TRUE) {
                    status = MSGQ_put (msgqQueue, recvMsg) ;
                    DBC_assert (status == SYS_OK) ;
                    if (status != SYS_OK) {
                        MSGQ_free (recvMsg) ;
                    }
                }
                else {
                    MSGQ_free (recvMsg) ;
                }
            }
        }
    }
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_send
 *
 *  @desc   Sends a message to the specified remote MSGQ.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYMQT_send (ZCPYMQT_State * mqtState, Void * bufPtr)
{
    Uint16            status = SYS_OK ;
    ZCPYMQT_Ctrl *    ctrlPtr ;
    QUE_Elem *        tmpPtr ;

#if defined (LOG_DG_MSGQ_PUT)
    Uint32 tmp ;
#endif /* if defined (LOG_DG_MSGQ_PUT) */

    DBC_require (mqtState != NULL) ;
    DBC_require (bufPtr    != NULL) ;

    ctrlPtr = mqtState->ctrlPtr ;

    MPCS_enter ((MPCS_Handle) &(ctrlPtr->csFmDspList)) ;

    HAL_cacheInv ((Ptr) &(ctrlPtr->fmDspList), sizeof (ctrlPtr->fmDspList)) ;

    tmpPtr = QUE_prev (&(ctrlPtr->fmDspList)) ;
    HAL_cacheInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;

    QUE_put ((QUE_Handle) &(ctrlPtr->fmDspList), bufPtr) ;
    HAL_cacheWbInv ((Ptr) bufPtr, sizeof (QUE_Elem)) ;
    HAL_cacheWbInv ((Ptr) &(ctrlPtr->fmDspList), sizeof (ctrlPtr->fmDspList)) ;
    HAL_cacheWbInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;

    status = IPS_notify (ID_GPP,
                         mqtState->ipsId,
                         mqtState->ipsEventNo,
                         (Uint16) 0,
                         FALSE) ;
    if (status != SYS_OK) {
        /* Remove the queued msg on failure */
        HAL_cacheInv ((Ptr) bufPtr, sizeof (bufPtr)) ;
        tmpPtr = QUE_next (bufPtr) ;
        HAL_cacheInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;
        tmpPtr = QUE_prev (bufPtr) ;
        HAL_cacheInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;

        QUE_remove (bufPtr) ;

        HAL_cacheWbInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;
        tmpPtr = QUE_next (bufPtr) ;
        HAL_cacheWbInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;
    }

    MPCS_leave ((MPCS_Handle) &(ctrlPtr->csFmDspList)) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
