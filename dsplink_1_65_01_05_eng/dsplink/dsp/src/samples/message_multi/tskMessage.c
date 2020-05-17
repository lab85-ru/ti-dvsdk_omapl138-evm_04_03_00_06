/** ============================================================================
 *  @file   tskMessage.c
 *
 *  @path   $(DSPLINK)/dsp/src/samples/message_multi/
 *
 *  @desc   This is a multi-application message sample. It interacts with
 *          multiple application instances on the GPP-side.
 *          It creates MAX_APPS tasks. The tasks wait on notification from the
 *          GPP about the application instance that is created on the GPP-side.
 *          When this notification is received, it opens and locates the
 *          corresponding message queues, performing message transfers with the
 *          GPP.
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
#include <sem.h>
#include <log.h>
#include <tsk.h>
#include <msgq.h>
#include <pool.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <platform.h>
#include <failure.h>
#include <notify.h>

#if defined (MSGQ_ZCPY_LINK)
#if ( PHYINTERFACE == PCI_INTERFACE) ||( PHYINTERFACE == VLYNQ_INTERFACE)
#include <dma_pool.h>
#else
#include <sma_pool.h>
#endif
#endif /* if defined (MSGQ_ZCPY_LINK) */


/*  ----------------------------------- Sample Headers              */
#include <message_config.h>
#include <tskMessage.h>


#ifdef __cplusplus
extern "C" {
#endif


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   FILEID is used by SET_FAILURE_REASON macro.
 *  ============================================================================
 */
#define FILEID  FID_APP_C

/** ============================================================================
 *  @name   trace
 *
 *  @desc   trace LOG_Obj used to do LOG_printf
 *  ============================================================================
 */
extern LOG_Obj trace ;

/** ============================================================================
 *  @name   numTransfers
 *
 *  @desc   Iterations of message transfer to be done by the application.
 *          A value of 0 in numTransfers implies infinite iterations.
 *  ============================================================================
 */
extern Uint16 numTransfers ;

/** ============================================================================
 *  @name   MESSAGE_Pools
 *
 *  @desc   Extern declaration for configuration array of MESSAGE_Pools.
 *  ============================================================================
 */
extern POOL_Obj MESSAGE_Pools [] ;

#if ( PHYINTERFACE == PCI_INTERFACE) ||( PHYINTERFACE == VLYNQ_INTERFACE)
/** ============================================================================
 *  @name   MESSAGE_PoolParams
 *
 *  @desc   Extern declaration for pool parameters for the array of pools
 *  ============================================================================
 */
extern DMAPOOL_Params MESSAGE_PoolParams [NUM_POOLS] ;

/** ============================================================================
 *  @name   smaPoolObj
 *
 *  @desc   Global object for pool parameters for the dynamic POOL_open call
 *  ============================================================================
 */
DMAPOOL_Params            dmaPoolObj ;
#else
/** ============================================================================
 *  @name   MESSAGE_PoolParams
 *
 *  @desc   Extern declaration for pool parameters for the array of pools
 *  ============================================================================
 */
extern SMAPOOL_Params MESSAGE_PoolParams [NUM_POOLS] ;

/** ============================================================================
 *  @name   smaPoolObj
 *
 *  @desc   Global object for pool parameters for the dynamic POOL_open call
 *  ============================================================================
 */
SMAPOOL_Params            smaPoolObj ;
#endif



/** ============================================================================
 *  @func   TSKMESSAGE_create
 *
 *  @desc   Create phase function for the TSKMESSAGE application. Initializes
 *          the TSKMESSAGE_TransferInfo structure with the information that will
 *          be used by the other phases of the application.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int TSKMESSAGE_create (TSKMESSAGE_TransferInfo * info)
{
    Int                       status     = SYS_OK ;
    Uint32                    poolId     = info->appNum ;
    MSGQ_Attrs                msgqAttrs  = MSGQ_ATTRS ;
    MSGQ_LocateAttrs          syncLocateAttrs ;
    POOL_Obj                  poolObj ;

    info->numTransfers = numTransfers ;
    info->localMsgq    = MSGQ_INVALIDMSGQ ;
    info->locatedMsgq  = MSGQ_INVALIDMSGQ ;



     /* Open the pool to be used for this application. */
    if (status == SYS_OK) {
#if ( PHYINTERFACE == PCI_INTERFACE) ||( PHYINTERFACE == VLYNQ_INTERFACE)
        dmaPoolObj.poolId        = poolId ;
        dmaPoolObj.exactMatchReq = TRUE ;
        poolObj.initFxn = DMAPOOL_init ;
        poolObj.fxns    = (POOL_Fxns *) &DMAPOOL_FXNS ;
        poolObj.params  = &(dmaPoolObj) ;
#else
        smaPoolObj.poolId        = poolId ;
        smaPoolObj.exactMatchReq = TRUE ;
        poolObj.initFxn = SMAPOOL_init ;
        poolObj.fxns    = (POOL_Fxns *) &SMAPOOL_FXNS ;
        poolObj.params  = &(smaPoolObj) ;
#endif

        poolObj.object  = NULL ;

        status = POOL_open (poolId, &poolObj) ;

        if (status == SYS_OK) {
            status = NOTIFY_notify (ID_GPP,
                                    MESSAGE_IPS_ID,
                                    MESSAGE_IPS_EVENTNO_BASE,
                                    info->appNum) ;
        }
    }


    if (status == SYS_OK) {
        /* Set the semaphore to a known state. */
        SEM_new (&(info->msgqSemObj), 0) ;

        /* Fill in the attributes for this message queue. */
        msgqAttrs.notifyHandle = &(info->msgqSemObj) ;
        msgqAttrs.pend         = (MSGQ_Pend) SEM_pendBinary ;
        msgqAttrs.post         = (MSGQ_Post) SEM_postBinary ;

        /* Creating message queue */
        status = MSGQ_open (info->dspMsgqName, &info->localMsgq, &msgqAttrs) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {
            /* Set the message queue that will receive any async. errors. */
            MSGQ_setErrorHandler (info->localMsgq, poolId) ;

            /* Synchronous locate. */
            status = SYS_ENOTFOUND ;
            while ((status == SYS_ENOTFOUND) || (status == SYS_ENODEV)) {
                syncLocateAttrs.timeout = SYS_FOREVER ;
                status = MSGQ_locate (info->gppMsgqName,
                                      &info->locatedMsgq,
                                      &syncLocateAttrs) ;
                if ((status == SYS_ENOTFOUND) || (status == SYS_ENODEV)) {
                    TSK_sleep (1000) ;
                }
                else if (status != SYS_OK) {
#if !defined (LOG_COMPONENT)
                    LOG_printf (&trace, "MSGQ_locate (msgqOut) failed. "
                                "Status = 0x%x\n", status) ;
#endif
                }
            }
        }

        /* Initialize the sequenceNumber */
        info->sequenceNumber = 0 ;
    }

    return status ;
}


/** ============================================================================
 *  @func   TSKMESSAGE_execute
 *
 *  @desc   Execute phase function for the TSKMESSAGE application. Application
 *          receives a message, verifies the id and sends it back.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int TSKMESSAGE_execute (TSKMESSAGE_TransferInfo * info)
{
    Int         status = SYS_OK ;
    Uint32      poolId = info->appNum ;
    MSGQ_Msg    msg ;
    Uint32      i ;

    /* Allocate and send the message */
    status = MSGQ_alloc (poolId, &msg, APP_BUFFER_SIZE) ;

    if (status == SYS_OK) {
        MSGQ_setMsgId (msg, info->sequenceNumber) ;
        MSGQ_setSrcQueue (msg, info->localMsgq) ;

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

    /* Execute the loop for configured number of transfers
     * A value of 0 in numTransfers implies infinite iterations
     */
    for (i = 0 ;
         (   ((info->numTransfers == 0) || (i < info->numTransfers))
          && (status == SYS_OK)) ;
         i++) {
        /* Receive a message */
        status = MSGQ_get (info->localMsgq, &msg, SYS_FOREVER) ;
        if (status == SYS_OK) {
            /* Check if an asynchronous error message is received */
            if (MSGQ_getMsgId (msg) == MSGQ_ASYNCERRORMSGID) {
#if !defined (LOG_COMPONENT)
                LOG_printf(&trace, "Transport error Type = %d",
                    ((MSGQ_AsyncErrorMsg *) msg)->errorType) ;
#endif

                /* Must free the message */
                MSGQ_free (msg) ;
                status = SYS_EBADIO ;
                SET_FAILURE_REASON (status) ;
            }
            else if (MSGQ_getMsgId (msg) != info->sequenceNumber) {
                /* Check to make sure correct message is received */
#if !defined (LOG_COMPONENT)
                LOG_printf (&trace, "Out of sequence message!") ;
#endif
                MSGQ_free (msg) ;
                status = SYS_EBADIO ;
                SET_FAILURE_REASON (status) ;
            }
            else {
                /* Increment the sequenceNumber for next received message */
                info->sequenceNumber++ ;
                /* Make sure that the sequenceNumber stays within the permitted
                 * range for applications.
                 */
                if (info->sequenceNumber == MSGQ_INTERNALIDSSTART) {
                    info->sequenceNumber = 0 ;
                }

                MSGQ_setMsgId (msg, info->sequenceNumber) ;
                MSGQ_setSrcQueue (msg, info->localMsgq) ;

                /* Send the message back */
                status = MSGQ_put (info->locatedMsgq, msg) ;
                if (status != SYS_OK) {
                    SET_FAILURE_REASON(status);
                }
            }
        }
        else {
            SET_FAILURE_REASON (status) ;
        }
    }

    return status ;
}


/** ============================================================================
 *  @func   TSKMESSAGE_delete
 *
 *  @desc   Delete phase function for the TSKMESSAGE application. It deallocates
 *          all the resources of allocated during create phase of the
 *          application.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int TSKMESSAGE_delete (TSKMESSAGE_TransferInfo * info)
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
                           sizeof (TSKMESSAGE_TransferInfo)) ;
    if ((status == SYS_OK) && (freeStatus != TRUE)) {
        status = SYS_EFREE ;
        SET_FAILURE_REASON (status) ;
    }

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
