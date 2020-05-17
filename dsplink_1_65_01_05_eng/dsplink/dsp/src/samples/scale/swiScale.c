/** ============================================================================
 *  @file   swiScale.c
 *
 *  @path   $(DSPLINK)/dsp/src/samples/scale/
 *
 *  @desc   This is simple DSP/BIOS LINK SWI application that does data transfer
 *          in loopback mode using GIO interface.  It opens two channels one
 *          for input and one for output. It also uses a message queue to
 *          receive a scaling factor. Channel numbers configurable. Maximum
 *          buffer size of input and output channels is also configurable
 *          though #defines in header file. It is important whenever one makes
 *          changes in the configuration of this application. He should also
 *          make changes on the corresponding GPP side application sources.
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
#include <iom.h>
#include <gio.h>
#include <msgq.h>
#include <pool.h>
#if defined (DSP_BOOTMODE_NOBOOT)
#include <dio.h>
#include <iom.h>
#include <sma_pool.h>
#include <zcpy_mqt.h>
#include <dsplinkmsgq.h>
#endif
/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <failure.h>
#include <dsplink.h>
#include <platform.h>

/*  ----------------------------------- Sample Headers              */
#include <scale_config.h>
#include <swiScale.h>
#include <scale.h>


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   FILEID needed for SET_FAILURE_REASON macro.
 *  ============================================================================
 */
#define FILEID  FID_APP_C

/** ============================================================================
 *  @const  READ_MAILBOX_MASK
 *
 *  @desc   Read mailbox mask.
 *  ============================================================================
 */
#define READ_MAILBOX_MASK   0x1

/** ============================================================================
 *  @const  WRITE_MAILBOX_MASK
 *
 *  @desc   Write mailbox mask.
 *  ============================================================================
 */
#define WRITE_MAILBOX_MASK  0x2

/** ============================================================================
 *  @const  INITIAL_DATA_MAILBOX_VAL
 *
 *  @desc   Initial mailbox value.
 *  ============================================================================
 */
#define INITIAL_DATA_MAILBOX_VAL (READ_MAILBOX_MASK | WRITE_MAILBOX_MASK)

/** ============================================================================
 *  @const  INITIAL_MSG_MAILBOX_VAL
 *
 *  @desc   Initial mailbox value for msgSWI.
 *  ============================================================================
 */
#define INITIAL_MSG_MAILBOX_VAL 0x0

/** ============================================================================
 *  @name   xferBufSize
 *
 *  @desc   Size of the buffer size for SWI based loopback.
 *  ============================================================================
 */
extern Uint16 xferBufSize ;

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
extern DIO_Params dioAttrs ;

/** ============================================================================
 *  @name   devAttrs
 *
 *  @desc   devAttrs structure
 *  ============================================================================
 */
extern DEV_Attrs devAttrs ;

/** ============================================================================
 *  @name   dioDevAttrs
 *
 *  @desc   dioDevAttrs structure
 *  ============================================================================
 */
extern DEV_Attrs dioDevAttrs ;
#endif

/** ============================================================================
 *  @name   DSPLINK_initFlag
 *
 *  @desc   Flag to indicate to the DSPLINK_init.
 *  ============================================================================
 */
extern volatile Uint32 DSPLINK_initFlag  ;
/** ============================================================================
 *  @name   smaPoolObj
 *
 *  @desc   Global object for pool parameters for the dynamic POOL_open call
 *  ============================================================================
 */
extern SMAPOOL_Params            smaPoolObj ;
#endif


/** ----------------------------------------------------------------------------
 *  @func   readFinishCb/writeFinishCb
 *
 *  @desc   Declaration of callback functions to be called by GIO when
 *          IOM_READ/IOM_WRITE request is over.
 *
 *  @arg    arg
 *              Argument given while doing GIO_submit.
 *          status
 *              Status of IO.
 *          bufp
 *              Buffer on which IO has completed.
 *          size
 *              Size of IO done.
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
static Void readFinishCb(Ptr arg, Int status, Ptr bufp, Uns size);
static Void writeFinishCb(Ptr arg, Int status, Ptr bufp, Uns size);

/** ----------------------------------------------------------------------------
 *  @func   dataSWI
 *
 *  @desc   Entry point for SWI that does data processing.
 *
 *  @arg    arg
 *              Argument given while doing GIO_submit.
 *          status
 *              Status of IO.
 *          bufp
 *              Buffer on which IO has completed.
 *          size
 *              Size of IO done.
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
static Void dataSWI(Arg arg0, Arg arg1);

/** ----------------------------------------------------------------------------
 *  @func   msgSWI
 *
 *  @desc   Entry point for SWI that does message processing.
 *
 *  @arg    arg0
 *              Info structure
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
static Void msgSWI(Arg arg0, Arg arg1);


/** ============================================================================
 *  @func   SWISCALE_create
 *
 *  @desc   Create phase of SWISCALE application. It allocates
 *          SWISCALE_TransferInfo structure and intializes it with configured
 *          values.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int SWISCALE_create(SWISCALE_TransferInfo ** infoPtr)
{
    Int                     status    = SYS_OK ;
    SWI_Attrs               swiAttrs  = SWI_ATTRS ;
    MSGQ_Attrs              msgqAttrs = MSGQ_ATTRS ;
    SWISCALE_TransferInfo * info ;
 #if defined (DSP_BOOTMODE_NOBOOT)
    POOL_Obj                poolObj          ;
    ZCPYMQT_Params          mqtParams        ;
    MSGQ_TransportObj       transport ;

    {
        while (DSPLINK_initFlag != 0xC0C0BABA) ;
    }
    /* Initialize DSP/BIOS LINK. */
    DSPLINK_init () ;

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

    /* Allocate SWISCALE_TransferInfo structure */
    *infoPtr = MEM_calloc (DSPLINK_SEGID,
                           sizeof (SWISCALE_TransferInfo),
                           DSPLINK_BUF_ALIGN) ;
    if (*infoPtr == NULL) {
        status = SYS_EALLOC ;
        SET_FAILURE_REASON(status) ;
    }
    else {
        info = *infoPtr ;
    }

    /* Initialize SWISCALE_TransferInfo structure */
    if (status == SYS_OK) {
        info->bufferSize        = xferBufSize ;
        (info->appReadCb).fxn   = readFinishCb ;
        (info->appReadCb).arg   =(Ptr) info ;
        (info->appWriteCb).fxn  = writeFinishCb ;
        (info->appWriteCb).arg  = (Ptr) info ;
        info->scalingFactor     = 1 ;
    }

    /* Create channel handles */
    if (status == SYS_OK) {
        GIO_Attrs gioAttrs = GIO_ATTRS ;
        info->gioInputChan = GIO_create (INPUT_CHANNEL,
                                         IOM_INPUT,
                                         NULL,
                                         NULL,
                                         &gioAttrs) ;
        info->gioOutputChan = GIO_create (OUTPUT_CHANNEL,
                                          IOM_OUTPUT,
                                          NULL,
                                          NULL,
                                          &gioAttrs) ;
        if ((info->gioInputChan == NULL) || (info->gioOutputChan == NULL)) {
            status = SYS_EALLOC ;
            SET_FAILURE_REASON (status) ;
        }
    }

    /* Create SWI for sending and receiving data */
    if (status == SYS_OK) {
        swiAttrs.fxn        = dataSWI ;
        swiAttrs.arg0       = (Arg) info ;
        swiAttrs.mailbox    = INITIAL_DATA_MAILBOX_VAL ;
        info->swiData = SWI_create (&swiAttrs) ;
        if (info->swiData == NULL) {
            status = SYS_EALLOC ;
            SET_FAILURE_REASON (status) ;
        }
    }

    /* Create SWI for receiving messages */
    if (status == SYS_OK) {
        swiAttrs.fxn        = msgSWI ;
        swiAttrs.arg0       = (Arg) info ;
        swiAttrs.mailbox    = INITIAL_MSG_MAILBOX_VAL ;
        info->swiMsg = SWI_create (&swiAttrs) ;
        if (info->swiMsg == NULL) {
            status = SYS_EALLOC ;
            SET_FAILURE_REASON (status) ;
        }
    }

    /* Allocate input and output buffers */
    if (status == SYS_OK) {
        status = POOL_alloc (SAMPLE_POOL_ID,
                             (Ptr *) &(info->inputBuffer),
                             info->bufferSize) ;
        if (status == SYS_OK) {
            status = POOL_alloc (SAMPLE_POOL_ID,
                                 (Ptr *) &(info->outputBuffer),
                                 info->bufferSize) ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }
        else {
            SET_FAILURE_REASON (status) ;
        }
    }

    /* Set-up the message queue */
    if (status == SYS_OK) {
        /* Fill in the attributes for this message queue. */
        msgqAttrs.notifyHandle = info->swiMsg;
        msgqAttrs.post         = (MSGQ_Post) SWI_post ;
        msgqAttrs.pend         = NULL ;

        /* Creating message queue */
        status = MSGQ_open (DSP_MSGQNAME, &info->msgqQueue, &msgqAttrs) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    return status;
}


/** ============================================================================
 *  @func   SWISCALE_execute
 *
 *  @desc   Execute phase of SWISCALE application. It gets the application
 *          started after that SWIs get posted automatically.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int SWISCALE_execute(SWISCALE_TransferInfo * info)
{
    Int     status  = SYS_OK ;
    size_t  bufSize = info->bufferSize ;
    Int     iomStatus ;

    /* Submit a read request */
    iomStatus = GIO_submit (info->gioInputChan,
                            IOM_READ,
                            info->inputBuffer,
                            &bufSize,
                            &(info->appReadCb)) ;

    if (iomStatus != IOM_PENDING) {
        status = SYS_EBADIO ;
        SET_FAILURE_REASON (status) ;
    }

    /* We don't write for the first time so clearing write bit
     * from SWI's mailbox */
    SWI_andn (info->swiData, WRITE_MAILBOX_MASK) ;

    return status ;
}


/** ============================================================================
 *  @func   SWISCALE_delete
 *
 *  @desc   Delete phase of SWISCALE application. Deallocates all the
 *          resources that were allocated during create phase.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int SWISCALE_delete(SWISCALE_TransferInfo * info)
{
    Int status = SYS_OK ;
    Int iomStatus ;

    if (info != NULL) {
        /* Delete data SWI */
        if (info->swiData != NULL) {
            SWI_delete (info->swiData) ;
        }

        /* Delete data SWI */
        if (info->swiMsg != NULL) {
            SWI_delete (info->swiMsg) ;
        }

        /* Delete the message queue */
        if (info->msgqQueue != NULL) {
            status = MSGQ_close (info->msgqQueue) ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }


        /* Delete GIO input channel */
        if (info->gioInputChan != NULL) {
            iomStatus = GIO_delete (info->gioInputChan) ;
            if ((status == SYS_OK) && (iomStatus != IOM_COMPLETED)) {
                status = SYS_EBADIO ;
                SET_FAILURE_REASON (status) ;
            }
        }

        /* Delete GIO output channel */
        if (info->gioOutputChan != NULL) {
            iomStatus = GIO_delete (info->gioOutputChan) ;
            if ((status == SYS_OK) && (iomStatus != IOM_COMPLETED)) {
                status = SYS_EBADIO ;
                SET_FAILURE_REASON (status) ;
            }
        }

        /* Deallocate inputBuffer */
        if (info->inputBuffer != NULL) {
            POOL_free (SAMPLE_POOL_ID, info->inputBuffer, info->bufferSize) ;
        }

        /* Deallocate outputBuffer */
        if (info->outputBuffer != NULL) {
            POOL_free (SAMPLE_POOL_ID, info->outputBuffer, info->bufferSize) ;
        }

        /* Deallocate info structure */
        if (!MEM_free (DSPLINK_SEGID, info, sizeof (SWISCALE_TransferInfo))) {
            status = SYS_EALLOC ;
            SET_FAILURE_REASON (status) ;
        }
    }
    else {
        status = SYS_EALLOC ;
    }

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   readFinishCb
 *
 *  @desc   Function to be registered as callback funtion for IOM_READ command
 *          of GIO_submit.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static Void readFinishCb(Ptr arg, Int status, Ptr bufp, Uns size)
{
    SWISCALE_TransferInfo *info = (SWISCALE_TransferInfo *) arg ;

    /* Received full buffer from the GPP */
    info->inputBuffer = bufp ;

    info->readMAUs = size ;
    if (status != IOM_COMPLETED) {
        SET_FAILURE_REASON (status) ;
    }
    /* Give indication to SWI that READ is over */
    SWI_andn (info->swiData, READ_MAILBOX_MASK) ;
}


/** ----------------------------------------------------------------------------
 *  @func   writeFinishCb
 *
 *  @desc   Function to registered as callback funtion for IOM_WRITE
 *          command of GIO_submit.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static Void writeFinishCb(Ptr arg, Int status, Ptr bufp, Uns size)
{
    SWISCALE_TransferInfo *info =(SWISCALE_TransferInfo *)arg;

    (Void) size ; /* To remove compiler warning */

    /* Received empty buffer from the GPP */
    info->outputBuffer = bufp ;

    if (status != IOM_COMPLETED) {
        SET_FAILURE_REASON (status) ;
    }
    /* Give indication to SWI that WRITE is over */
    SWI_andn (info->swiData, WRITE_MAILBOX_MASK) ;
}


/** ----------------------------------------------------------------------------
 *  @func   dataSWI
 *
 *  @desc   SWI entry point. This SWI runs for the following reasons:
 *             - IOM_READ and IOM_WRITE requests both get completed.
 *               It puts does buffer processing and starts the next IOM_READ
 *               and IOM_WRITE requests.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static Void dataSWI(Arg arg0, Arg arg1)
{
    SWISCALE_TransferInfo * info      = (SWISCALE_TransferInfo *) arg0 ;
    Int                     status    = SYS_OK ;
    Uns                     numMAUs   = info->readMAUs ;
    size_t                  bufSize   = info->bufferSize ;
    Int                     iomStatus ;
    Uns                     i ;

    (Void) arg1 ; /* To remove compiler warning */

    /* Do processing of data here */
    for (i = 0 ; i < numMAUs; i++) {
        info->outputBuffer [i] = info->inputBuffer [i] * info->scalingFactor ;
    }

    /* Submit a Read data request */
    iomStatus = GIO_submit (info->gioInputChan,
                            IOM_READ,
                            info->inputBuffer,
                            &bufSize,
                            &(info->appReadCb)) ;
    if (iomStatus != IOM_PENDING) {
        status = SYS_EBADIO ;
        SET_FAILURE_REASON (status) ;
    }

    /* Submit a Write data request */
    if (status == SYS_OK) {
        iomStatus = GIO_submit (info->gioOutputChan,
                                IOM_WRITE,
                                info->outputBuffer,
                                &numMAUs,
                                &(info->appWriteCb)) ;
    }
    if (iomStatus != IOM_PENDING) {
        status = SYS_EBADIO ;
        SET_FAILURE_REASON (status) ;
    }
}


/** ----------------------------------------------------------------------------
 *  @func   msgSWI
 *
 *  @desc   SWI entry point. This SWI runs for the following reasons:
 *             - When a GPP message arrives.
 *               The scaling factor is updated accordingly.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static Void msgSWI(Arg arg0, Arg arg1)
{
    Int                     status    = SYS_OK ;
    SWISCALE_TransferInfo * info      = (SWISCALE_TransferInfo *) arg0 ;
    MSGQ_Msg                msg ;

    (Void) arg1 ; /* To remove compiler warning */

    status = MSGQ_get (info->msgqQueue, &msg, 0) ;
    if (status == SYS_OK) {
        if (MSGQ_getMsgId (msg) == SCALE_REQUEST_MSGID) {
            info->scalingFactor = ((SCALE_ScaleMsg *) msg)->scalingFactor ;
        }
        else {
            status = SYS_EBADIO ;
            SET_FAILURE_REASON (status) ;
        }

        MSGQ_free (msg) ;
    }
}

