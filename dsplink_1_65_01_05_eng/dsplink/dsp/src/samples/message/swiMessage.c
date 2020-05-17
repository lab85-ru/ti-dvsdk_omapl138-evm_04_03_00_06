/** ============================================================================
 *  @file   swiMessage.c
 *
 *  @path   $(DSPLINK)/dsp/src/samples/message/
 *
 *  @desc   This is simple SWI based application that uses MSGQ interface to
 *          implement messaging from GPP. It receives a messages from the GPP,
 *          verifies its content and sends it back to the GPP.
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
#include <log.h>
#include <tsk.h>
#include <msgq.h>
#include <pool.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <failure.h>
#include <platform.h>
#if defined (DSP_BOOTMODE_NOBOOT)
#include <zcpy_mqt.h>
#include <sma_pool.h>
#include <dsplinkmsgq.h>
#endif

/*  ----------------------------------- Sample Headers              */
#include <message_config.h>
#include <swiMessage.h>


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   FILEID needed for SET_FAILURE_REASON macro.
 *  ============================================================================
 */
#define FILEID  FID_APP_C

/** ============================================================================
 *  @const  dspMsgQName
 *
 *  @desc   location to hold the DSP message queue.
 *  ============================================================================
 */
extern Uint8   dspMsgQName[DSP_MAX_STRLEN] ;

/** ============================================================================
 *  @name   trace
 *
 *  @desc   trace LOG_Obj used to do LOG_printf
 *  ============================================================================
 */
extern LOG_Obj trace ;

#if  defined (DSP_BOOTMODE_NOBOOT)
/** ============================================================================
 *  @name   DSPLINK_initFlag
 *
 *  @desc   Flag to indicate to the DSPLINK_init.
 *  ============================================================================
 */
extern volatile Uint32 DSPLINK_initFlag ;

/** ============================================================================
 *  @name   smaPoolObj
 *
 *  @desc   Global object for pool parameters for the dynamic POOL_open call
 *  ============================================================================
 */
extern SMAPOOL_Params            smaPoolObj ;
/** ============================================================================
 *  @name   MESSAGE_MqtParams
 *
 *  @desc   Extern declaration for msgq parameters for the array of pools
 *  ============================================================================
 */
extern ZCPYMQT_Params  mqtParams ;
#endif


/** ----------------------------------------------------------------------------
 *  @func   messageSWI
 *
 *  @desc   Entry point for SWI that does message handling.
 *
 *  @arg    arg0
 *              Pointer to information structure
 *  @arg    arg1
 *              Currently not used
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
static Void messageSWI (Arg arg0, Arg arg1) ;


/** ============================================================================
 *  @func   SWIMESSAGE_create
 *
 *  @desc   Create phase of SWIMESSAGE application. It allocates
 *          SWIMESSAGE_TransferInfo structure and intializes it with configured
 *          values.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int SWIMESSAGE_create (SWIMESSAGE_TransferInfo ** infoPtr)
{
    Int                       status     = SYS_OK ;
    MSGQ_Attrs                msgqAttrs  = MSGQ_ATTRS ;
    SWIMESSAGE_TransferInfo * info       = NULL ;
    SWI_Attrs                 swiAttrs   = SWI_ATTRS ;
#if defined (DSP_BOOTMODE_NOBOOT)
    MSGQ_TransportObj         transport ;
    POOL_Obj                  poolObj ;

    smaPoolObj.poolId        = 0;
    smaPoolObj.exactMatchReq = TRUE ;
    poolObj.initFxn = SMAPOOL_init ;
    poolObj.fxns    = (POOL_Fxns *) &SMAPOOL_FXNS ;
    poolObj.params  = &(smaPoolObj) ;
    poolObj.object  = NULL ;

    status = POOL_open (0, &poolObj) ;

    if (status == SYS_OK) {
        mqtParams.poolId  = 0 ;
        transport.initFxn = ZCPYMQT_init ;         /* Init Function                 */
        transport.fxns    = (MSGQ_TransportFxns *) &ZCPYMQT_FXNS ;     /* Transport interface functions */
        transport.params  = &mqtParams ;   /* Transport params              */
        transport.object  = NULL ;                 /* Filled in by transport        */
        transport.procId  = ID_GPP  ;              /* Processor Id                  */

        status = MSGQ_transportOpen (ID_GPP, &transport) ;
     }
#endif

    /* Allocate SWIMESSAGE_TransferInfo structure */
    *infoPtr = MEM_calloc (DSPLINK_SEGID,
                           sizeof (SWIMESSAGE_TransferInfo),
                           DSPLINK_BUF_ALIGN) ;
    if (*infoPtr == NULL) {
        status = SYS_EALLOC ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        info = *infoPtr ;
        info->localMsgq    = MSGQ_INVALIDMSGQ ;
        info->locatedMsgq  = MSGQ_INVALIDMSGQ ;
        /* Initialize the sequenceNumber */
        info->sequenceNumber = 0;
    }

    /* Create SWI for receiving and sending messages */
    if (status == SYS_OK) {
        swiAttrs.fxn   = messageSWI ;
        swiAttrs.arg0  = (Arg) info ;
        info->swi = SWI_create (&swiAttrs) ;
        if (info->swi == NULL) {
            status = SYS_EALLOC;
            SET_FAILURE_REASON (status) ;
        }
    }

    /* Setup message queues */
    if (status == SYS_OK) {
        /* Fill in the attributes for this message queue. */
        msgqAttrs.notifyHandle = info->swi;
        msgqAttrs.post         = (MSGQ_Post) SWI_post ;
        msgqAttrs.pend         = NULL ;

        SYS_sprintf ((Char *)dspMsgQName, "%s%d",
                              DSP_MSGQNAME,
                              GBL_getProcId ()) ;

        /* Opening message queue */
        status = MSGQ_open ((String )dspMsgQName, &info->localMsgq, &msgqAttrs) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {
            /* Set the message queue that will receive any async. errors. */
            MSGQ_setErrorHandler (info->localMsgq, SAMPLE_POOL_ID) ;

            /* Now post the message SWI to start the ball rolling */
            SWI_post (info->swi) ;
        }
    }

    return status;
}

/** ============================================================================
 *  @func   SWIMESSAGE_delete
 *
 *  @desc   Delete phase of SWIMESSAGE application. Deallocates all the
 *          resources that were allocated during create phase.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int SWIMESSAGE_delete(SWIMESSAGE_TransferInfo * info)
{
    Int     status     = SYS_OK ;
    Int     tmpStatus  = SYS_OK ;
    Bool    freeStatus = FALSE ;

    /* Release the located message queue */
    if (info->locatedMsgq != MSGQ_INVALIDMSGQ) {
        status = MSGQ_release (info->locatedMsgq) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    /*
     *  Reset the error handler before deleting the MSGQ that receives the error
     *  messages.
     */
    MSGQ_setErrorHandler (MSGQ_INVALIDMSGQ, POOL_INVALIDID) ;

    /* Delete the message queue */
    if (info->localMsgq != MSGQ_INVALIDMSGQ) {
        tmpStatus = MSGQ_close (info->localMsgq) ;
        if ((status == SYS_OK) && (tmpStatus != SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }
    }

    /* Free the info structure */
    freeStatus = MEM_free (DSPLINK_SEGID,
                           info,
                           sizeof (SWIMESSAGE_TransferInfo)) ;
    if ((status == SYS_OK) && (freeStatus != TRUE)) {
        status = SYS_EFREE ;
        SET_FAILURE_REASON (status) ;
    }

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   messageSWI
 *
 *  @desc   SWI entry point. This SWI is posted when a new message comes in.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static Void messageSWI (Arg arg0, Arg arg1)
{
    Int                         status  = SYS_OK;
    SWIMESSAGE_TransferInfo *   info    = (SWIMESSAGE_TransferInfo *) arg0 ;
    static Bool                 firstTime = TRUE ;
    MSGQ_Msg                    msg ;
    MSGQ_LocateAsyncAttrs       asyncLocateAttrs ;

    (Void) arg1 ; /* To avoid compiler warning */

    if (firstTime == TRUE) {
        firstTime = FALSE ;
        /* Start off by sending an async locate request to the GPP to get the
         * handle to the remote GPP message queue.
         * The sample assumes that the GPP message queue has been opened by the
         * time execution reaches here on the DSP-side.
         */
        asyncLocateAttrs.poolId = SAMPLE_POOL_ID ;
        asyncLocateAttrs.arg    = (Arg) NULL ;
        status = MSGQ_locateAsync (GPP_MSGQNAME,
                                   info->localMsgq,
                                   &asyncLocateAttrs) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }
    else {
        /* This is a SWI so timeout must be zero */
        status = MSGQ_get (info->localMsgq, &msg, 0) ;
        if (status == SYS_OK) {
            /* Handle the received message */
            switch (MSGQ_getMsgId (msg)) {
            case MSGQ_ASYNCLOCATEMSGID:
                info->locatedMsgq = ((MSGQ_AsyncLocateMsg *) msg)->msgqQueue ;

                /* Must free the message */
                MSGQ_free (msg) ;

                /* Allocate and send the first message */
                status = MSGQ_alloc (SAMPLE_POOL_ID,
                                     &msg,
                                     APP_BUFFER_SIZE) ;
                if (status == SYS_OK) {
                    MSGQ_setMsgId (msg, info->sequenceNumber) ;
                    status = MSGQ_put (info->locatedMsgq, msg) ;
                    if (status != SYS_OK) {
                        /* Must free the message */
                        MSGQ_free (msg) ;
                        SET_FAILURE_REASON (status) ;
                    }
                }
                else {
                    SET_FAILURE_REASON (status) ;
                }
                break ;

            case MSGQ_ASYNCERRORMSGID:
                LOG_printf(&trace, "Transport error Type = %d",
                    ((MSGQ_AsyncErrorMsg *) msg)->errorType) ;

                /* Must free the message */
                MSGQ_free (msg) ;
                status = SYS_EBADIO ;
                SET_FAILURE_REASON (status) ;
                break ;

            default:
                /* Check to make sure correct message is received */
                if (MSGQ_getMsgId (msg) != info->sequenceNumber) {
                    LOG_printf(&trace, "Out of sequence message!");
                    status = SYS_EBADIO ;
                    SET_FAILURE_REASON (status) ;
                }
                else {
                    /* Increment the sequenceNumber for next received message */
                    info->sequenceNumber++ ;
                    if (info->sequenceNumber == MSGQ_INTERNALIDSSTART) {
                        info->sequenceNumber = 0 ;
                    }
                    MSGQ_setMsgId (msg, info->sequenceNumber) ;
                    /* Send the message back */
                    status = MSGQ_put (info->locatedMsgq, msg) ;
                    if (status != SYS_OK) {
                        SET_FAILURE_REASON (status) ;
                    }
                }
                break ;
            }
        }
        else {
            SET_FAILURE_REASON(status);
        }
    }
}
