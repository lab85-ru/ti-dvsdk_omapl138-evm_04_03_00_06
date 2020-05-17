/** ============================================================================
 *  @file   tskScale.c
 *
 *  @path   $(DSPLINK)/dsp/src/samples/scale/
 *
 *  @desc   This is simple TSK based application that uses SIO interface to
 *          implement scale for GPP. It receives data from the a input
 *          channel and send the same data back from output channel. Input and
 *          output channel numbers are configurable through header file of this
 *          application. It also uses a message queue to receive a
 *          scaling factor for the output data.
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
#include <log.h>
#include <swi.h>
#include <sys.h>
#include <sio.h>
#include <tsk.h>
#include <msgq.h>
#include <pool.h>
#if defined (DSP_BOOTMODE_NOBOOT)
#include <dio.h>
#include <iom.h>
#endif


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <failure.h>
#include <dsplink.h>
#include <platform.h>
#if defined (DSP_BOOTMODE_NOBOOT)
#include <sma_pool.h>
#include <zcpy_mqt.h>
#include <dsplinkmsgq.h>
#endif
/*  ----------------------------------- Sample Headers              */
#include <scale_config.h>
#include <tskScale.h>
#include <scale.h>


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   FILEID is used by SET_FAILURE_REASON macro.
 *  ============================================================================
 */
#define FILEID  FID_APP_C

/** ============================================================================
 *  @name   xferBufSize
 *
 *  @desc   Size of the buffer size for TSK based scale.
 *  ============================================================================
 */
extern Uint16 xferBufSize ;

/** ============================================================================
 *  @name   numTransfers
 *
 *  @desc   Iterations of data transfer to be done by the application.
 *  ============================================================================
 */
extern Uint16 numTransfers ;

#if defined (DSP_BOOTMODE_NOBOOT)
#if defined (CHNL_COMPONENT)
/** ============================================================================
 *  @name   ZCPYDATA_FXNS
 *
 *  @desc   Global object for pool parameters for the dynamic POOL_open call
 *  ============================================================================
 */
extern IOM_Fxns ZCPYDATA_FXNS ;

/** ============================================================================
 *  @name   ZCPYDATA_init
 *
 *  @desc   external declaration for ZCPYDATA_init function
 *  ============================================================================
 */
extern Void ZCPYDATA_init (Void) ;


/** ============================================================================
 *  @name   dioAttrs
 *
 *  @desc   dioAttrs structure
 *  ============================================================================
 */
DIO_Params dioAttrs = {
     "/dsplink",
      NULL
} ;

/** ============================================================================
 *  @name   devAttrs
 *
 *  @desc   devAttrs structure
 *  ============================================================================
 */
DEV_Attrs devAttrs = {
        0,            /* devId */
        0,            /* dsplink deviceParams */
        DEV_IOMTYPE,  /* dsplink driver type */
        0             /* dsplink devp */
} ;

/** ============================================================================
 *  @name   dioDevAttrs
 *
 *  @desc   dioDevAttrs structure
 *  ============================================================================
 */
DEV_Attrs dioDevAttrs = {
        0,            /* devId */
        &dioAttrs,    /* DIO deviceParams */
        DEV_SIOTYPE,  /* DIO type */
        0             /* devp */
} ;
#endif
/** ============================================================================
 *  @name   smaPoolObj
 *
 *  @desc   Global object for pool parameters for the dynamic POOL_open call
 *  ============================================================================
 */
SMAPOOL_Params            smaPoolObj ;

#endif

/** ============================================================================
 *  @func   TSKSCALE_create
 *
 *  @desc   Create phase function for the TSKSCALE application. Initializes the
 *          TSKSCALE_TransferInfo structure with the information that will be
 *          used by the other phases of the application.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int TSKSCALE_create(TSKSCALE_TransferInfo ** infoPtr)
{
    Int                     status    = SYS_OK ;
    MSGQ_Attrs              msgqAttrs = MSGQ_ATTRS ;
    SIO_Attrs               attrs     = SIO_ATTRS ;
    TSKSCALE_TransferInfo * info      = NULL ;
#if defined (DSP_BOOTMODE_NOBOOT)
    POOL_Obj                poolObj          ;
    ZCPYMQT_Params          mqtParams        ;
    MSGQ_TransportObj       transport ;

    smaPoolObj.poolId        = 0;
    smaPoolObj.exactMatchReq = TRUE ;
    poolObj.initFxn = SMAPOOL_init ;
    poolObj.fxns    = (POOL_Fxns *) &SMAPOOL_FXNS ;
    poolObj.params  = &(smaPoolObj) ;
    poolObj.object  = NULL ;

    status = POOL_open (0, &poolObj) ;
#if defined (CHNL_COMPONENT)
    if (status == SYS_OK) {
        /* Create IOM driver dynamically */
        status = DEV_createDevice("/dsplink",
                                  &ZCPYDATA_FXNS,
                                  (Fxn) &ZCPYDATA_init,
                                  &devAttrs) ;
        if (status == SYS_OK) {
            /* Create DIO adapter dynamically */
            status = DEV_createDevice("/dio_dsplink",
                                      &DIO_tskDynamicFxns,
                                      NULL,
                                      &dioDevAttrs);
        }
    }
#endif
    if (status == SYS_OK) {
        mqtParams.poolId  = 0 ;
        transport.initFxn = ZCPYMQT_init ;         /* Init Function                 */
        transport.fxns    = (MSGQ_TransportFxns *) &ZCPYMQT_FXNS ;     /* Transport interface functions */
        transport.params  = &mqtParams ;   /* Transport params              */
        transport.object  = NULL ;                 /* Filled in by transport        */
        transport.procId  = ID_GPP  ;              /* Processor Id                  */


        status = MSGQ_transportOpen (ID_GPP,
                                     &transport) ;
     }
#endif

    /* Allocate TSKSCALE_TransferInfo structure that will be initialized
     * and passed to other phases of the application */
    *infoPtr = MEM_calloc (DSPLINK_SEGID,
                           sizeof (TSKSCALE_TransferInfo),
                           DSPLINK_BUF_ALIGN) ;
    if (*infoPtr == NULL) {
        status = SYS_EALLOC ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        info = *infoPtr ;
    }

    if (status == SYS_OK) {
        /* Filling up the transfer info structure */
        info->numTransfers  = numTransfers ;
        info->bufferSize    = xferBufSize ;
        info->numBuffers    = 1 ;
        info->scalingFactor = 1 ;

        /* Attributes for the stream creation */
        attrs.nbufs   = info->numBuffers ;
        attrs.segid   = DSPLINK_SEGID ;
        attrs.align   = DSPLINK_BUF_ALIGN ;
        attrs.flush   = TRUE ;
        attrs.model   = SIO_ISSUERECLAIM ;
        attrs.timeout = SYS_FOREVER ;

        /* Creating input and output streams */
        info->inputStream = SIO_create (INPUT_CHANNEL,
                                        SIO_INPUT,
                                        info->bufferSize,
                                        &attrs);
        info->outputStream = SIO_create (OUTPUT_CHANNEL,
                                         SIO_OUTPUT,
                                         info->bufferSize,
                                         &attrs) ;

        if ((info->inputStream == NULL) || (info->outputStream == NULL)) {
            status = SYS_EALLOC ;
            SET_FAILURE_REASON (status) ;
        }
    }

    /* Allocating the one buffer that will be used in the transfer */
    if (status == SYS_OK) {
        status = POOL_alloc (SAMPLE_POOL_ID,
                             (Ptr *) &(info->buffer),
                             info->bufferSize) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    /* Set-up the message queue */
    if (status == SYS_OK) {
        /* Set the semaphore to a known state. */
        SEM_new (&(info->notifySemObj), 0) ;

        /* Fill in the attributes for this message queue. */
        msgqAttrs.notifyHandle = &(info->notifySemObj) ;
        msgqAttrs.pend         = (MSGQ_Pend) SEM_pendBinary ;
        msgqAttrs.post         = (MSGQ_Post) SEM_postBinary ;

        /* Creating message queue */
        status = MSGQ_open (DSP_MSGQNAME, &info->msgqQueue, &msgqAttrs) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }
    return status ;
}


/** ============================================================================
 *  @func   TSKSCALE_execute
 *
 *  @desc   Execute phase function for the TSKSCALE application. Application
 *          receives the data from the input channel and sends the same data
 *          back on output channel. Channel numbers can be configured through
 *          header file.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int TSKSCALE_execute(TSKSCALE_TransferInfo * info)
{
    Int      status    = SYS_OK;
    Char *   buffer    = (Char *) info->buffer;
    Arg      arg       = 0;
    Uns      i, j;
    Int      nmadus;
    MSGQ_Msg msg;

    /* Execute the loop for configured number of transfers
     * A value of 0 in numTransfers implies infinite iterations
     */
    for (i = 0 ;
         (   ((info->numTransfers == 0) || (i < info->numTransfers))
          && (status == SYS_OK)) ;
         i++) {
        /* Receive a data buffer from GPP */
        status = SIO_issue (info->inputStream,
                            buffer,
                            info->bufferSize,
                            arg) ;
        if (status == SYS_OK) {
            nmadus = SIO_reclaim (info->inputStream,
                                  (Ptr *) &buffer,
                                  &arg) ;
            if (nmadus < 0) {
                status = -nmadus;
                SET_FAILURE_REASON (status) ;
            }
            else {
                info->receivedSize = nmadus ;
            }
        }
        else {
            SET_FAILURE_REASON (status) ;
        }

        /* Check to see if there is a scaling message */
        status = MSGQ_get (info->msgqQueue, &msg, 0) ;
        if (status == SYS_OK) {
           if (MSGQ_getMsgId (msg) == SCALE_REQUEST_MSGID) {
               info->scalingFactor = ((SCALE_ScaleMsg *)msg)->scalingFactor ;
           }
           else {
               status = SYS_EBADIO ;
               SET_FAILURE_REASON (status) ;
           }

           MSGQ_free (msg) ;
        }
        else {
            status = SYS_OK ; /* Not an error. */
        }

        /* Do processing on this buffer */
        if (status == SYS_OK) {
            for (j = 0 ; (j < info->receivedSize) && (status == SYS_OK); j++) {
                buffer [j] *= info->scalingFactor ;
            }
        }

        /* Send the processed buffer back to GPP */
        if (status == SYS_OK) {
            status = SIO_issue (info->outputStream,
                                buffer,
                                info->receivedSize,
                                arg) ;

            if (status == SYS_OK) {
                nmadus = SIO_reclaim (info->outputStream,
                                      (Ptr *)&(buffer),
                                      &arg) ;
                if (nmadus < 0) {
                    status = -nmadus ;
                    SET_FAILURE_REASON (status) ;
                }
            }
            else {
                SET_FAILURE_REASON (status) ;
            }
        }
    }
    return status ;
}


/** ============================================================================
 *  @func   TSKSCALE_delete
 *
 *  @desc   Delete phase function for the TSKSCALE application. It deallocates
 *          all the resources of allocated during create phase of the
 *          application.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int TSKSCALE_delete (TSKSCALE_TransferInfo * info)
{
    Int         status     = SYS_OK ;
    Int         funcStatus = SYS_OK ;
    Bool        freeStatus = FALSE ;

    /* Delete the message queue */
    if (info->msgqQueue != NULL) {
        status = MSGQ_close (info->msgqQueue) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    /* Delete input stream */
    if (info->inputStream != NULL) {
        funcStatus = SIO_delete(info->inputStream);
        if ((status == SYS_OK) && (funcStatus != SYS_OK)) {
            status = funcStatus;
            SET_FAILURE_REASON(status);
        }
    }

    /* Delete output stream */
    if (info->outputStream != NULL) {
        funcStatus = SIO_delete(info->outputStream);
        if ((status == SYS_OK) && (funcStatus != SYS_OK)) {
            status = funcStatus;
            SET_FAILURE_REASON(status);
        }
    }

    /* Delete the buffer */
    if (info->numBuffers > 0) {
        if (info->buffer != NULL) {
            POOL_free (SAMPLE_POOL_ID, info->buffer, info->bufferSize) ;
        }
    }

    /* Free the info structure */
    freeStatus = MEM_free(DSPLINK_SEGID, info, sizeof (TSKSCALE_TransferInfo));
    if (freeStatus != TRUE) {
        status = SYS_EFREE;
        SET_FAILURE_REASON(status);
    }

    return status;
}
