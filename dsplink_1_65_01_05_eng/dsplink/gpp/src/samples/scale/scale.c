/** ============================================================================
 *  @file   scale.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/scale/
 *
 *  @desc   This is a scale application which sends a buffer across the DSP
 *          processor and recieves a buffer back using DSP/BIOS LINK. The DSP
 *          scales the returned buffer based on the control message sent by the
 *          GPP. The data buffer received is verified aginst the data sent to
 *          DSP. This application transfers only one buffer and no buffer(s) are
 *          queued.
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


/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>

/*  ----------------------------------- DSP/BIOS LINK API               */
#include <proc.h>
#include <chnl.h>
#include <msgq.h>
#include <pool.h>
#if defined (DA8XXGEM)
#include <loaderdefs.h>
#endif


/*  ----------------------------------- Application Header              */
#include <scale.h>
#include <scale_os.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @const  NUM_ARGS
 *
 *  @desc   Number of arguments specified to the DSP application.
 *  ============================================================================
 */
#define NUM_ARGS             2

/** ============================================================================
 *  @name   SAMPLE_POOL_ID
 *
 *  @desc   ID of the POOL used for the sample.
 *  ============================================================================
 */
#define SAMPLE_POOL_ID       0

/** ============================================================================
 *  @name   NUM_BUF_SIZES
 *
 *  @desc   Number of buffer pools to be configured for the allocator.
 *  ============================================================================
 */
#define NUM_BUF_SIZES        4

/** ============================================================================
 *  @const  NUM_BUF_POOL0
 *
 *  @desc   Number of buffers in first buffer pool.
 *  ============================================================================
 */
#define NUM_BUF_POOL0        1

/** ============================================================================
 *  @const  NUM_BUF_POOL1
 *
 *  @desc   Number of buffers in second buffer pool.
 *  ============================================================================
 */
#define NUM_BUF_POOL1        2

/** ============================================================================
 *  @const  NUM_BUF_POOL2
 *
 *  @desc   Number of buffers in third buffer pool.
 *  ============================================================================
 */
#define NUM_BUF_POOL2        1

/** ============================================================================
 *  @name   NUM_BUF_POOL3
 *
 *  @desc   Number of buffers in fourth buffer pool.
 *  ============================================================================
 */
#define NUM_BUF_POOL3        3

/** ============================================================================
 *  @name   SAMPLE_CTRLMSG_SIZE
 *
 *  @desc   Control message size.
 *  ============================================================================
 */
#if defined (ZCPY_LINK)
#define SAMPLE_CTRLMSG_SIZE ZCPYMQT_CTRLMSG_SIZE
#endif /* if defined (ZCPY_LINK) */


/*  ============================================================================
 *  @name   XFER_CHAR
 *
 *  @desc   The value used to initialize the output buffer and used for
 *          validation against the input buffer received.
 *  ============================================================================
 */
#define XFER_CHAR            0x1

/** ============================================================================
 *  @const  SCALE_REQUEST_MSGID
 *
 *  @desc   Message id to denote a scaling change.
 *  ============================================================================
 */
#define SCALE_REQUEST_MSGID  1

/** ============================================================================
 *  @const  APP_MSG_SIZE
 *
 *  @desc   Messaging buffer used by the application.
 *          Note: This buffer is aligned according to the alignment expected
 *          by the platform.
 *  ============================================================================
 */
#define APP_MSG_SIZE  DSPLINK_ALIGN (sizeof (SCALE_ScaleMsg), DSPLINK_BUF_ALIGN)

#if defined (DA8XXGEM)
/** ============================================================================
 *  @name   dspAddr
 *
 *  @desc   Address of c_int00 in the DSP executable.
 *  ============================================================================
 */
Uint32 SCALE_dspAddr ;

/** ============================================================================
 *  @name   shmAddr
 *
 *  @desc   Address of symbol DSPLINK_shmBaseAddres in the DSP executable.
 *  ============================================================================
 */
Uint32 SCALE_shmAddr ;

/** ============================================================================
 *  @name   argsAddr
 *
 *  @desc   Address of .args section in the DSP executable.
 *  ============================================================================
 */
Uint32 SCALE_argsAddr ;

/** ============================================================================
 *  @name   LINKCFG_config
 *
 *  @desc   Extern declaration to the default DSP/BIOS LINK configuration
 *          structure.
 *  ============================================================================
 */
extern  LINKCFG_Object LINKCFG_config ;
#endif


/** ============================================================================
 *  @name   SCALE_ScaleMsg
 *
 *  @desc   Structure used to pass the scaling factor from the GPP to the DSP.
 *
 *  @field  msgHeader
 *              Required first field of a message.
 *  @field  scalingFactor
 *              Used to scale the output buffer values.
 *  ============================================================================
 */
typedef struct SCALE_ScaleMsg_tag {
    MSGQ_MsgHeader  msgHeader     ;
    Uint16          scalingFactor ;
} SCALE_ScaleMsg ;


/*  ============================================================================
 *  @name   SCALE_XferValue
 *
 *  @desc   The scaling factor for the returned buffer values.
 *  ============================================================================
 */
STATIC Uint8  SCALE_XferValue = 1 ;

/*  ============================================================================
 *  @name   SCALE_BufferSize
 *
 *  @desc   Size of buffer to be used for data transfer.
 *  ============================================================================
 */
STATIC Uint32  SCALE_BufferSize ;

/*  ============================================================================
 *  @name   SCALE_NumIterations
 *
 *  @desc   Size of buffer to be used for data transfer.
 *  ============================================================================
 */
STATIC Uint32  SCALE_NumIterations ;

/** ============================================================================
 *  @name   SCALE_Buffers
 *
 *  @desc   Array of buffers used by input channel.
 *          Length of array in this application is 1.
 *  ============================================================================
 */
STATIC Char8 * SCALE_Buffers [1] ;

/** ============================================================================
 *  @name   SCALE_IOReq
 *
 *  @desc   It gives information for adding or reclaiming an input request.
 *  ============================================================================
 */
STATIC ChannelIOInfo SCALE_IOReq  ;

/** ============================================================================
 *  @const  MsgqDsp1
 *
 *  @desc   Name of the first MSGQ on the DSP.
 *  ============================================================================
 */
STATIC Char8 SampleDspMsgqName [DSP_MAX_STRLEN] = "DSPMSGQ1" ;

/** ============================================================================
 *  @name   SampleDspMsgq
 *
 *  @desc   DSP's MSGQ Object.
 *  ============================================================================
 */
STATIC MSGQ_Queue SampleDspMsgq = (Uint32) MSGQ_INVALIDMSGQ ;


#if defined (VERIFY_DATA)
/** ----------------------------------------------------------------------------
 *  @func   SCALE_VerifyData
 *
 *  @desc   This function verifies the data-integrity of given buffer.
 *
 *  @arg    buf
 *              This is the pointer of the buffer whose contents are to be
 *              validated.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Contents of the input buffer and the output buffer are
 *              different.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    SCALE_XferValue
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
SCALE_VerifyData (IN Char8 * buf) ;
#endif /*  defined (VERIFY_DATA) */


/** ============================================================================
 *  @func   SCALE_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @modif  SCALE_Buffers , SCALE_Buffers
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
SCALE_Create (IN Char8 * dspExecutable,
              IN Char8 * strBufferSize,
              IN Char8 * strNumIterations,
              IN Uint8   processorId)
{
    DSP_STATUS    status                  = DSP_SOK ;
    Uint32        numArgs                 = 0 ;
    Uint32        size    [NUM_BUF_SIZES] = {APP_MSG_SIZE,
                                             SAMPLE_CTRLMSG_SIZE,
                                             DSPLINK_ALIGN (
                                                   sizeof (MSGQ_AsyncLocateMsg),
                                                   DSPLINK_BUF_ALIGN)} ;
    Uint32        numBufs [NUM_BUF_SIZES] = {NUM_BUF_POOL0,
                                             NUM_BUF_POOL1,
                                             NUM_BUF_POOL2,
                                             NUM_BUF_POOL3} ;
    ChannelAttrs  chnlAttrInput ;
    ChannelAttrs  chnlAttrOutput ;
    Char8       * args [NUM_ARGS] ;
#if defined (ZCPY_LINK)
    SMAPOOL_Attrs poolAttrs ;
    ZCPYMQT_Attrs mqtAttrs ;
#endif /* if defined (ZCPY_LINK) */
#if defined (PCPY_LINK)
    BUFPOOL_Attrs poolAttrs ;
    PCPYMQT_Attrs mqtAttrs ;
#endif /* if defined (PCPY_LINK) */
#if defined (DA8XXGEM)
    NOLOADER_ImageInfo  imageInfo ;
#endif


    SCALE_0Print ("Entered SCALE_Create ()\n") ;

    /*
     *  Create and initialize the proc object.
     */
    status = PROC_setup (NULL) ;

    /*
     *  Attach the Dsp with which the transfers have to be done.
     */
    if (DSP_SUCCEEDED (status)) {
        status = PROC_attach (processorId, NULL) ;
        if (DSP_FAILED (status)) {
            SCALE_1Print ("PROC_attach () failed. Status = [0x%x]\n",
                            status) ;
        }
    }
    else {
        SCALE_1Print ("PROC_setup () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Open the pool.
     */
    if (DSP_SUCCEEDED (status)) {
        size [3] = SCALE_BufferSize ;
        poolAttrs.bufSizes      = (Uint32 *) &size ;
        poolAttrs.numBuffers    = (Uint32 *) &numBufs ;
        poolAttrs.numBufPools   = NUM_BUF_SIZES ;
#if defined (ZCPY_LINK)
        poolAttrs.exactMatchReq = TRUE ;
#endif /* if defined (ZCPY_LINK) */
        status = POOL_open (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                            &poolAttrs) ;
        if (DSP_FAILED (status)) {
            SCALE_1Print ("POOL_open () failed. Status = [0x%x]\n", status) ;
        }
    }

    /*
     *  Load the executable on the DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        numArgs  = NUM_ARGS         ;
        args [0] = strBufferSize    ;
        args [1] = strNumIterations ;

#if defined (DA8XXGEM)
        if  (    (LINKCFG_config.
                  dspConfigs [processorId]->dspObject->doDspCtrl)
             ==  DSP_BootMode_NoBoot) {
            imageInfo.dspRunAddr  = SCALE_dspAddr ;
            imageInfo.shmBaseAddr = SCALE_shmAddr ;
            imageInfo.argsAddr    = SCALE_argsAddr ;
            imageInfo.argsSize    = 50 ;
            status = PROC_load (processorId, (Char8 *) &imageInfo, numArgs, args) ;
        }
        else
#endif
        {
            status = PROC_load (processorId, dspExecutable, numArgs, args) ;
        }
        if (DSP_FAILED (status)) {
            SCALE_1Print ("PROC_load () failed. Status = [0x%x]\n", status) ;
        }
    }

    /*
     *  Create a channel to DSP
     */
    if (DSP_SUCCEEDED (status)) {
        chnlAttrOutput.mode      = ChannelMode_Output     ;
        chnlAttrOutput.endianism = Endianism_Default      ;
        chnlAttrOutput.size      = ChannelDataSize_16bits ;

        status = CHNL_create (processorId, CHNL_ID_OUTPUT, &chnlAttrOutput) ;
        if (DSP_FAILED (status)) {
            SCALE_1Print ("CHNL_create () failed. Status = [0x%x]\n",
                            status) ;
        }
    }

    /*
     *  Create a channel from DSP
     */
    if (DSP_SUCCEEDED (status)) {
        chnlAttrInput.mode      = ChannelMode_Input      ;
        chnlAttrInput.endianism = Endianism_Default      ;
        chnlAttrInput.size      = ChannelDataSize_16bits ;

        status = CHNL_create (processorId, CHNL_ID_INPUT, &chnlAttrInput) ;
        if (DSP_FAILED (status)) {
            SCALE_1Print ("CHNL_create () failed. Status = [0x%x]\n",
                            status) ;
        }
    }

    /*
     *  Allocate buffer(s) for data transfer to DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        status = CHNL_allocateBuffer (processorId,
                                      CHNL_ID_OUTPUT,
                                      SCALE_Buffers,
                                      SCALE_BufferSize ,
                                      1) ;
        if (DSP_FAILED (status)) {
            SCALE_1Print ("CHNL_allocateBuffer () failed. Status = [0x%x]\n",
                            status) ;
        }
    }

    /*
     *  Start execution on DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        status = PROC_start (processorId) ;
        if (DSP_FAILED (status)) {
            SCALE_1Print ("PROC_start () failed. Status = [0x%x]\n",
                            status) ;
        }
    }

    /*
     *  Open the remote transport.
     */
    if (DSP_SUCCEEDED (status)) {
        mqtAttrs.poolId = POOL_makePoolId(processorId, SAMPLE_POOL_ID) ;
        status = MSGQ_transportOpen (processorId, &mqtAttrs) ;
        if (DSP_FAILED (status)) {
            SCALE_1Print ("MSGQ_transportOpen () failed. Status = [0x%x]\n",
                            status) ;
        }
    }

    SCALE_0Print ("Leaving SCALE_Create ()\n") ;

    return status ;
}


/** ============================================================================
 *  @func   SCALE_Execute
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
SCALE_Execute (IN Uint32 numIterations, Uint8 processorId)
{
    DSP_STATUS       status = DSP_ENOTFOUND ;
    Uint8 *          ptr8   = NULL          ;
    Uint16 *         ptr16  = NULL          ;
    MSGQ_LocateAttrs syncLocateAttrs        ;
    Uint32           i                      ;
    Uint32           j                      ;
    SCALE_ScaleMsg * msg                    ;

    SCALE_0Print ("Entered SCALE_Execute ()\n") ;

    /*
     *  Locate the DSP's message queue
     */
    syncLocateAttrs.timeout = WAIT_FOREVER;
    while ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
        status = MSGQ_locate (SampleDspMsgqName,
                              &SampleDspMsgq,
                              &syncLocateAttrs) ;
        if ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
            SCALE_Sleep (100000) ;
        }
        else if (DSP_FAILED (status)) {
            SCALE_1Print ("MSGQ_locate () failed. Status = [0x%x]\n", status) ;
        }
    }

    /*
     *  Fill the IO Request structure
     *  It gives Information for adding or reclaiming an input request.
     */
    if (DSP_SUCCEEDED (status)) {
        SCALE_IOReq.buffer = SCALE_Buffers [0] ;
        SCALE_IOReq.size   = SCALE_BufferSize   ;
    }

    for (i = 1 ;
        (  (SCALE_NumIterations == 0) || (i <= SCALE_NumIterations))
         &&(DSP_SUCCEEDED (status)) ;
            i++) {
        /*
         *  Initialize the buffer with valid data.
         */
        if (DSP_SUCCEEDED (status)) {
            ptr8  = (Uint8 *)  (SCALE_IOReq.buffer) ;
            ptr16 = (Uint16 *) (SCALE_IOReq.buffer) ;
            for (   (j = 0) ;
                    (DSP_SUCCEEDED (status))
                 && (j < SCALE_BufferSize / DSP_MAUSIZE) ;
                     j++ ) {
                if (DSP_MAUSIZE == 1) {
                    *ptr8 = XFER_CHAR ;
                    ptr8++ ;
                }
                else if (DSP_MAUSIZE == 2) {
                    *ptr16 = XFER_CHAR ;
                    ptr16++ ;
                }
            }
        }

        /*
         *  Send data to DSP.
         *  Issue 'filled' buffer to the channel.
         */
        status = CHNL_issue (processorId, CHNL_ID_OUTPUT, &SCALE_IOReq) ;

        /*
         *  Reclaim 'empty' buffer from the channel
         */
        if (DSP_SUCCEEDED (status)) {
            status = CHNL_reclaim (processorId,
                                   CHNL_ID_OUTPUT,
                                   WAIT_FOREVER,
                                   &SCALE_IOReq) ;
            if (DSP_FAILED (status)) {
                SCALE_1Print ("CHNL_reclaim () failed. Status = [0x%x]\n",
                                status) ;
            }
        }
        else {
            SCALE_1Print ("CHNL_issue () failed. Status = [0x%x]\n", status) ;
        }

        /*
         *  Receive data from DSP
         *  Issue 'empty' buffer to the channel.
         */
        if (DSP_SUCCEEDED (status)) {
            status = CHNL_issue (processorId, CHNL_ID_INPUT, &SCALE_IOReq) ;
            if (DSP_FAILED (status)) {
                SCALE_1Print ("CHNL_issue () failed. Status = [0x%x]\n",
                                status) ;
            }
        }

        /*
         *  Reclaim 'filled' buffer from the channel
         */
        if (DSP_SUCCEEDED (status)) {
            status = CHNL_reclaim (processorId,
                                   CHNL_ID_INPUT,
                                   WAIT_FOREVER,
                                   &SCALE_IOReq) ;
            if (DSP_FAILED (status)) {
                SCALE_1Print ("CHNL_reclaim () failed. Status = [0x%x]\n",
                                status) ;
            }
        }

#if defined (VERIFY_DATA)
        /*
         *  Verify correctness of data received.
         */
        if (DSP_SUCCEEDED (status)) {
            status = SCALE_VerifyData (SCALE_IOReq.buffer) ;
        }
#endif
        if (DSP_SUCCEEDED (status) && (i % 100) == 0) {
            SCALE_1Print ("Transferred %d buffers\n", i) ;
        }

        if (    ((i != SCALE_NumIterations) || (SCALE_NumIterations == 0))
            &&  ((i % 10) == 0)) {
            /* Change the scaling factor */
            SCALE_XferValue++ ;
            status = MSGQ_alloc (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                                 APP_MSG_SIZE,
                                 (MSGQ_Msg *) &msg) ;
            if (DSP_SUCCEEDED (status)) {
                msg->scalingFactor = SCALE_XferValue ;
                /* Send the allocated message */
                MSGQ_setMsgId ((MSGQ_Msg) msg, SCALE_REQUEST_MSGID) ;
                status = MSGQ_put (SampleDspMsgq, (MSGQ_Msg) msg) ;
                if (DSP_SUCCEEDED (status)) {
                    SCALE_1Print ("Changed the scale factor to: %d\n",
                                  SCALE_XferValue) ;
                }
                else {
                    SCALE_1Print ("MSGQ_put () failed. Status = [0x%x]\n",
                                    status) ;
                }
            }
            else {
                SCALE_1Print ("MSGQ_alloc () failed. Status = [0x%x]\n",
                                status) ;
            }
        }
    }

    /*
     *  Release the remote message queue
     */
    status = MSGQ_release (SampleDspMsgq) ;
    if (DSP_FAILED (status)) {
        SCALE_1Print ("MSGQ_release () failed. Status = [0x%x]\n", status) ;
    }

    SCALE_0Print ("Leaving SCALE_Execute ()\n") ;

    return status ;
}


/** ============================================================================
 *  @func   SCALE_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          SCALE_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *
 *  @modif  SCALE_Buffers , SCALE_Buffers
 *  ============================================================================
 */
NORMAL_API
Void
SCALE_Delete (Uint8 processorId)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;

    SCALE_0Print ("Entered SCALE_Delete ()\n") ;

    /*
     *  Close the remote transport
     */
    status = MSGQ_transportClose (processorId) ;
    if (DSP_FAILED (status)) {
        SCALE_1Print ("MSGQ_transportClose () failed. Status = [0x%x]\n",
                        status) ;
    }

    /*
     *  Stop execution on DSP.
     */
    tmpStatus = PROC_stop (processorId) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        SCALE_1Print ("PROC_stop () failed. Status = [0x%x]\n",
                        tmpStatus) ;
    }

    /*
     *  Free the buffer(s) allocated for channel to DSP
     */
    tmpStatus = CHNL_freeBuffer (processorId,
                                 CHNL_ID_OUTPUT,
                                 SCALE_Buffers,
                                 1) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        SCALE_1Print ("CHNL_freeBuffer () failed. Status = [0x%x]\n",
                        tmpStatus) ;
    }

    /*
     *  Delete the input channel
     */
    tmpStatus = CHNL_delete  (processorId, CHNL_ID_INPUT) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        SCALE_1Print ("CHNL_delete () failed. Status = [0x%x]\n",
                        tmpStatus) ;
    }

    /*
     *  Delete the output channel
     */
    tmpStatus = CHNL_delete  (processorId, CHNL_ID_OUTPUT) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        SCALE_1Print ("CHNL_delete () failed. Status = [0x%x]\n",
                        tmpStatus) ;
    }

    /*
     *  Close the pool
     */
    tmpStatus = POOL_close (POOL_makePoolId(processorId, SAMPLE_POOL_ID)) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        SCALE_1Print ("POOL_close () failed. Status = [0x%x]\n",
                        tmpStatus) ;
    }

    /*
     *  Detach from the processor
     */
    tmpStatus = PROC_detach (processorId) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        SCALE_1Print ("PROC_detach () failed. Status = [0x%x]\n",
                        tmpStatus) ;
    }

    /*
     *  Destroy the PROC object.
     */
    tmpStatus = PROC_destroy () ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        SCALE_1Print ("PROC_destroy () failed. Status = [0x%x]\n",
                        tmpStatus) ;
    }

    SCALE_0Print ("Leaving SCALE_Delete ()\n") ;
}


/** ============================================================================
 *  @func   SCALE_Main
 *
 *  @desc   Entry point for the application
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
SCALE_Main (IN Char8 * dspExecutable,
            IN Char8 * strBufferSize,
            IN Char8 * strNumIterations,
            IN Char8 * strProcessorId)
{
    DSP_STATUS status = DSP_SOK ;
    Uint8 processorId = 0 ;

    SCALE_0Print ("=============== Sample Application : SCALE ==========\n") ;

    if (   (dspExecutable != NULL)
        && (strBufferSize != NULL)
        && (strNumIterations != NULL)) {

        /*
         *  Validate the buffer size and number of iterations specified.
         */
        SCALE_BufferSize = DSPLINK_ALIGN (SCALE_Atoi (strBufferSize),
                                          DSPLINK_BUF_ALIGN) ;

        SCALE_NumIterations = SCALE_Atoi (strNumIterations) ;
        processorId         = SCALE_Atoi (strProcessorId) ;

        if (   (SCALE_NumIterations >  0xFFFF)
            || (SCALE_BufferSize == 0)
            || (processorId >= MAX_DSPS)) {
            status = DSP_EINVALIDARG ;
            SCALE_1Print ("ERROR! Invalid arguments specified for  "
                          "scale application.\n"
                          "     Max iterations = %d\n",
                          0xFFFF) ;
            SCALE_1Print ("     Buffer size    = %d\n",
                          SCALE_BufferSize) ;
            SCALE_1Print ("     DSP processor Id    = %d\n",
                          processorId) ;
        }
        else {
            /*
             *  Specify the dsp executable file name and the buffer size for
             *  loop creation phase.
             */
            if (DSP_SUCCEEDED (status)) {
                status = SCALE_Create (dspExecutable,
                                       strBufferSize,
                                       strNumIterations,
                                       processorId) ;
            }

            /*
             *  Execute the data transfer loop.
             */
            if (DSP_SUCCEEDED (status)) {
                status = SCALE_Execute (SCALE_NumIterations, processorId) ;
            }

            /*
             *  Perform cleanup operation.
             */
            SCALE_Delete (processorId) ;
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        SCALE_0Print ("ERROR! Invalid arguments specified for "
                     "scale application\n") ;
    }

    SCALE_0Print ("====================================================\n") ;
}

#if defined (VERIFY_DATA)
/** ----------------------------------------------------------------------------
 *  @func   SCALE_VerifyData
 *
 *  @desc   This function verifies the data-integrity of given buffer.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
SCALE_VerifyData (IN Char8 * buf)
{
    DSP_STATUS status = DSP_SOK ;
    Uint16 *   ptr16  = NULL    ;
    Int16      i                ;

    /*
     *  Verify the data
     */
    ptr16 = (Uint16 *) (buf) ;
    for (i = 0 ;
         DSP_SUCCEEDED (status) && (i < SCALE_BufferSize / DSP_MAUSIZE) ;
         i++) {
        if (DSP_MAUSIZE == 1) {
            if (*buf != (Char8) (XFER_CHAR * SCALE_XferValue)) {
                SCALE_0Print ("ERROR! Data integrity check failed\n") ;
                SCALE_1Print ("    Expected [0x%x]\n",
                              (XFER_CHAR * SCALE_XferValue)) ;
                SCALE_1Print ("    Received [0x%x]\n", *buf) ;
                status = DSP_EFAIL ;
            }
            buf++ ;
        }
        else if (DSP_MAUSIZE == 2) {
            if (*ptr16 != (Uint16) (XFER_CHAR * SCALE_XferValue)) {
                SCALE_0Print ("ERROR! Data integrity check failed\n") ;
                SCALE_1Print ("    Expected [0x%x]\n",
                              (XFER_CHAR * SCALE_XferValue)) ;
                SCALE_1Print ("    Received [0x%x]\n", *buf) ;
                status = DSP_EFAIL ;
            }
            ptr16++ ;
        }
    }

    return status ;
}
#endif /* if defined (VERIFY_DATA) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
