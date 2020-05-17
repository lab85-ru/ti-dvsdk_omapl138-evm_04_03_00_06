/** ============================================================================
 *  @file   message.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/message/
 *
 *  @desc   This is an application which sends messages to the DSP
 *          processor and receives them back using DSP/BIOS LINK.
 *          The message contents received are verified against the data
 *          sent to DSP.
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


#include <stdio.h>

/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>

/*  ----------------------------------- DSP/BIOS LINK API               */
#include <proc.h>
#include <msgq.h>
#include <pool.h>
#if defined (DA8XXGEM)
#include <loaderdefs.h>
#endif


/*  ----------------------------------- Application Header              */
#include <message.h>
#include <message_os.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @const   NUM_ARGS
 *
 *  @desc   Number of arguments specified to the DSP application.
 *  ============================================================================
 */
#define NUM_ARGS 1

/** ============================================================================
 *  @name   SAMPLE_POOL_ID
 *
 *  @desc   ID of the POOL used for the sample.
 *  ============================================================================
 */
#define SAMPLE_POOL_ID       0

/** ============================================================================
 *  @const  NUMMSGPOOLS
 *
 *  @desc   Number of BUF pools in the pool
 *  ============================================================================
 */
#define NUMMSGPOOLS     4

/** ============================================================================
 *  @const  NUMMSGINPOOL0
 *
 *  @desc   Number in first message pool.
 *  ============================================================================
 */
#define NUMMSGINPOOL0   1

/** ============================================================================
 *  @const  NUMMSGINPOOL1
 *
 *  @desc   Number in second message pool.
 *  ============================================================================
 */
#define NUMMSGINPOOL1   2

/** ============================================================================
 *  @const  NUMMSGINPOOL2
 *
 *  @desc   Number in third message pool.
 *  ============================================================================
 */
#define NUMMSGINPOOL2   2

/** ============================================================================
 *  @const  NUMMSGINPOOL3
 *
 *  @desc   Number in fourth message pool.
 *  ============================================================================
 */
#define NUMMSGINPOOL3   4

/** ============================================================================
 *  @const  APP_BUFFER_SIZE
 *
 *  @desc   Messaging buffer used by the application.
 *          Note: This buffer is aligned according to the alignment expected
 *          by the platform.
 *  ============================================================================
 */
#define APP_BUFFER_SIZE  DSPLINK_ALIGN (sizeof (MSGQ_MsgHeader),       \
                                        DSPLINK_BUF_ALIGN)


/** ============================================================================
 *  Definitions required for the sample that vary based on physical link
 *  being used for the MQT.
 *  ============================================================================
 */
#if defined ZCPY_LINK
#define SAMPLEMQT_CTRLMSG_SIZE  ZCPYMQT_CTRLMSG_SIZE
STATIC ZCPYMQT_Attrs  mqtAttrs ;
#endif /* if defined ZCPY_LINK */

/** ============================================================================
 *  @name   SampleBufSizes
 *
 *  @desc   Message sizes managed by the pool.
 *  ============================================================================
 */
STATIC Uint32 SampleBufSizes [NUMMSGPOOLS] =
{
    APP_BUFFER_SIZE,
    SAMPLEMQT_CTRLMSG_SIZE,
    DSPLINK_ALIGN (sizeof (MSGQ_AsyncLocateMsg), DSPLINK_BUF_ALIGN),
    DSPLINK_ALIGN (sizeof (MSGQ_AsyncErrorMsg), DSPLINK_BUF_ALIGN)
} ;

/** ============================================================================
 *  @name   SampleNumBuffers
 *
 *  @desc   Number of messages in each pool managed by the pool.
 *  ============================================================================
 */
STATIC Uint32 SampleNumBuffers [NUMMSGPOOLS] =
{
    NUMMSGINPOOL0,
    NUMMSGINPOOL1,
    NUMMSGINPOOL2,
    NUMMSGINPOOL3
} ;


/** ============================================================================
 *  @name   SamplePoolAttrs
 *
 *  @desc   Definition of attributes for the pool based on physical link used
 *          by the MQT.
 *  ============================================================================
 */
#if defined ZCPY_LINK
STATIC SMAPOOL_Attrs SamplePoolAttrs =
{
    NUMMSGPOOLS,
    SampleBufSizes,
    SampleNumBuffers,
    TRUE
} ;
#endif /* if defined ZCPY_LINK */

/** ============================================================================
 *  @const  MsgqGpp1
 *
 *  @desc   Name of the first MSGQ on the GPP.
 *  ============================================================================
 */
STATIC Char8 SampleGppMsgqName [DSP_MAX_STRLEN] = "GPPMSGQ1" ;

/** ============================================================================
 *  @const  MsgqDsp1
 *
 *  @desc   Name of the first MSGQ on the DSP.
 *  ============================================================================
 */
STATIC Char8 SampleDspMsgqName [DSP_MAX_STRLEN] = "DSPMSGQ" ;

/** ============================================================================
 *  @name   SampleGppMsgq
 *
 *  @desc   Local GPP's MSGQ Object.
 *  ============================================================================
 */
STATIC MSGQ_Queue SampleGppMsgq = (Uint32) MSGQ_INVALIDMSGQ ;

/** ============================================================================
 *  @name   SampleDspMsgq
 *
 *  @desc   DSP's MSGQ Object.
 *  ============================================================================
 */
STATIC MSGQ_Queue SampleDspMsgq = (Uint32) MSGQ_INVALIDMSGQ ;

/** ============================================================================
 *  @name   dspMsgqName
 *
 *  @desc   Place holder for the MSGQ name created on DSP
 *  ============================================================================
 */
Char8   dspMsgqName [DSP_MAX_STRLEN] ;

#if defined (DA8XXGEM)
/** ============================================================================
 *  @name   dspAddr
 *
 *  @desc   Address of c_int00 in the DSP executable.
 *  ============================================================================
 */
Uint32 MESSAGE_dspAddr ;

/** ============================================================================
 *  @name   shmAddr
 *
 *  @desc   Address of symbol DSPLINK_shmBaseAddres in the DSP executable.
 *  ============================================================================
 */
Uint32 MESSAGE_shmAddr ;

/** ============================================================================
 *  @name   argsAddr
 *
 *  @desc   Address of .args section in the DSP executable.
 *  ============================================================================
 */
Uint32 MESSAGE_argsAddr ;
#endif

/** ============================================================================
 *  @name   LINKCFG_config
 *
 *  @desc   Extern declaration to the default DSP/BIOS LINK configuration
 *          structure.
 *  ============================================================================
 */
extern  LINKCFG_Object LINKCFG_config ;


#if defined (VERIFY_DATA)
/** ----------------------------------------------------------------------------
 *  @func   MESSAGE_VerifyData
 *
 *  @desc   This function verifies the data-integrity of given message.
 *
 *  @arg    msg
 *              This is the pointer of the message whose contents are to be
 *              validated.
 *  @arg    sequenceNumber
 *              Expected value of the msgqId.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Contents of the message is unexpected.
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
DSP_STATUS
MESSAGE_VerifyData (IN MSGQ_Msg msg, IN Uint16 sequenceNumber) ;
#endif /* if defined (VERIFY_DATA) */


/** ============================================================================
 *  @func   MESSAGE_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @modif  MESSAGE_InpBufs , MESSAGE_OutBufs
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGE_Create (IN Char8 * dspExecutable,
                IN Char8 * strNumIterations,
                IN Uint8   processorId)
{
    DSP_STATUS       status  = DSP_SOK  ;
    Uint32           numArgs = NUM_ARGS ;
    MSGQ_LocateAttrs syncLocateAttrs ;
    Char8 *          args [NUM_ARGS] ;
#if defined (DA8XXGEM)
    NOLOADER_ImageInfo  imageInfo ;
#endif

    MESSAGE_0Print ("Entered MESSAGE_Create ()\n") ;

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
            MESSAGE_1Print ("PROC_attach () failed. Status = [0x%x]\n",
                            status) ;
        }
    }

    /*
     *  Open the pool.
     */
    if (DSP_SUCCEEDED (status)) {
        status = POOL_open (
                            POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                            &SamplePoolAttrs) ;
        if (DSP_FAILED (status)) {
            MESSAGE_1Print ("POOL_open () failed. Status = [0x%x]\n",
                            status) ;
        }
    }
    else {
        MESSAGE_1Print ("PROC_setup () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Open the GPP's message queue
     */
    if (DSP_SUCCEEDED (status)) {
        status = MSGQ_open (SampleGppMsgqName, &SampleGppMsgq, NULL) ;
        if (DSP_FAILED (status)) {
            MESSAGE_1Print ("MSGQ_open () failed. Status = [0x%x]\n",
                            status) ;
        }
    }

    /*
     *  Set the message queue that will receive any async. errors
     */
    if (DSP_SUCCEEDED (status)) {
        status = MSGQ_setErrorHandler (SampleGppMsgq,
                                       POOL_makePoolId(processorId,
                                                       SAMPLE_POOL_ID)) ;
        if (DSP_FAILED (status)) {
            MESSAGE_1Print ("MSGQ_setErrorHandler () failed. Status = [0x%x]\n",
                            status) ;
        }
    }

    /*
     *  Load the executable on the DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        args [0] = strNumIterations ;
#if defined (DA8XXGEM)
        if  ( LINKCFG_config.dspConfigs [processorId]->dspObject->doDspCtrl
                  ==  DSP_BootMode_NoBoot) {
            imageInfo.dspRunAddr  = MESSAGE_dspAddr ;
            imageInfo.shmBaseAddr = MESSAGE_shmAddr ;
            imageInfo.argsAddr    = MESSAGE_argsAddr ;
            imageInfo.argsSize    = 50 ;        ;
            status = PROC_load (processorId, (Char8 *) &imageInfo, numArgs, args) ;
        }
        else
#endif
        {
            status = PROC_load (processorId, dspExecutable, numArgs, args) ;
        }
        if (DSP_FAILED (status)) {
            MESSAGE_1Print ("PROC_load () failed. Status = [0x%x]\n", status) ;
        }
    }

     /*
      *  Start execution on DSP.
      */
    if (DSP_SUCCEEDED (status)) {
        status = PROC_start (processorId) ;
        if (DSP_FAILED (status)) {
            MESSAGE_1Print ("PROC_start () failed. Status = [0x%x]\n",
                            status) ;
        }
    }

    /*
     *  Open the remote transport.
     */
    if (DSP_SUCCEEDED (status)) {
        mqtAttrs.poolId = POOL_makePoolId(processorId, SAMPLE_POOL_ID)  ;
        status = MSGQ_transportOpen (processorId, &mqtAttrs) ;
        if (DSP_FAILED (status)) {
            MESSAGE_1Print ("MSGQ_transportOpen () failed. Status = [0x%x]\n",
                            status) ;
        }
     }

    /*
     *  Locate the DSP's message queue
     */
    if (DSP_SUCCEEDED (status)) {
        syncLocateAttrs.timeout = WAIT_FOREVER;
        status = DSP_ENOTFOUND ;
        sprintf(dspMsgqName ,
                "%s%d",
                SampleDspMsgqName,
                processorId ) ;
        while ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
            status = MSGQ_locate (dspMsgqName,
                                  &SampleDspMsgq,
                                  &syncLocateAttrs) ;
            if ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
                MESSAGE_Sleep (100000) ;
            }
            else if (DSP_FAILED (status)) {
                MESSAGE_1Print ("MSGQ_locate () failed. Status = [0x%x]\n",
                                status) ;
            }
        }
    }

    MESSAGE_0Print ("Leaving MESSAGE_Create ()\n") ;

    return status ;
}


/** ============================================================================
 *  @func   MESSAGE_Execute
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGE_Execute (IN Uint32 numIterations, Uint8 processorId)
{
    DSP_STATUS  status         = DSP_SOK ;
    Uint16      sequenceNumber = 0 ;
    Uint16      msgId          = 0 ;
    Uint32      i ;
    MSGQ_Msg    msg ;

    MESSAGE_0Print ("Entered MESSAGE_Execute ()\n") ;

#if defined (MESSAGE_PROFILE)
    MESSAGE_GetStartTime () ;
#endif

    for (i = 1 ;
         (   (numIterations == 0) || (i <= (numIterations + 1)))
          && (DSP_SUCCEEDED (status)) ;
         i++) {
        /*
         *  Receive the message.
         */
        status = MSGQ_get (SampleGppMsgq, WAIT_FOREVER, &msg) ;
        if (DSP_FAILED (status)) {
            MESSAGE_1Print ("MSGQ_get () failed. Status = [0x%x]\n",
                            status) ;
        }
#if defined (VERIFY_DATA)
        /*
         *  Verify correctness of data received.
         */
        if (DSP_SUCCEEDED (status)) {
            status = MESSAGE_VerifyData (msg, sequenceNumber) ;
            if (DSP_FAILED (status)) {
                MSGQ_free (msg) ;
            }
        }
#endif

        /* If the message received is the final one, free it. */
        if ((numIterations != 0) && (i == (numIterations + 1))) {
            MSGQ_free (msg) ;
        }
        else {
            /*
             *  Send the same message received in earlier MSGQ_get () call.
             */
            if (DSP_SUCCEEDED (status)) {
                msgId = MSGQ_getMsgId (msg) ;
                MSGQ_setMsgId (msg, msgId) ;
                status = MSGQ_put (SampleDspMsgq, msg) ;
                if (DSP_FAILED (status)) {
                    MSGQ_free (msg) ;
                    MESSAGE_1Print ("MSGQ_put () failed. Status = [0x%x]\n",
                                    status) ;
                }
            }

            sequenceNumber++ ;
            /* Make sure that the sequenceNumber stays within the permitted
             * range for applications.
             */
            if (sequenceNumber == MSGQ_INTERNALIDSSTART) {
                sequenceNumber = 0 ;
            }

#if !defined (MESSAGE_PROFILE)
            if (DSP_SUCCEEDED (status) && ((i % 100) == 0)) {
                MESSAGE_1Print ("Transferred %ld messages\n", i) ;
            }
#endif
        }
    }

#if defined (MESSAGE_PROFILE)
    if (DSP_SUCCEEDED (status)) {
        MESSAGE_GetEndTime () ;
        MESSAGE_GetProfileInfo (numIterations) ;
    }
#endif

    MESSAGE_0Print ("Leaving MESSAGE_Execute ()\n") ;

    return status ;
}


/** ============================================================================
 *  @func   MESSAGE_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          MESSAGE_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGE_Delete (Uint8 processorId)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;

    MESSAGE_0Print ("Entered MESSAGE_Delete ()\n") ;

    /*
     *  Release the remote message queue
     */
    status = MSGQ_release (SampleDspMsgq) ;
    if (DSP_FAILED (status)) {
        MESSAGE_1Print ("MSGQ_release () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Close the remote transport
     */
    tmpStatus = MSGQ_transportClose (processorId) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGE_1Print ("MSGQ_transportClose () failed. Status = [0x%x]\n",
                        status) ;
    }

    /*
     *  Stop execution on DSP.
     */
    tmpStatus = PROC_stop (processorId) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGE_1Print ("PROC_stop () failed. Status = [0x%x]\n", status) ;
    }

    /*
     * Reset the error handler before deleting the MSGQ that receives
     * the error messages.
     */
    tmpStatus = MSGQ_setErrorHandler (MSGQ_INVALIDMSGQ, MSGQ_INVALIDMSGQ) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGE_1Print ("MSGQ_setErrorHandler () failed. Status = [0x%x]\n",
                        status) ;
    }

    /*
     *  Close the GPP's message queue
     */
    tmpStatus = MSGQ_close (SampleGppMsgq) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGE_1Print ("MSGQ_close () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Close the pool
     */
    tmpStatus = POOL_close (POOL_makePoolId(processorId, SAMPLE_POOL_ID)) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGE_1Print ("POOL_close () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Detach from the processor
     */
    tmpStatus = PROC_detach  (processorId) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGE_1Print ("PROC_detach () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Destroy the PROC object.
     */
    tmpStatus = PROC_destroy () ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGE_1Print ("PROC_destroy () failed. Status = [0x%x]\n", status) ;
    }

    MESSAGE_0Print ("Leaving MESSAGE_Delete ()\n") ;
}


/** ============================================================================
 *  @func   MESSAGE_Main
 *
 *  @desc   Entry point for the application
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGE_Main (IN Char8 * dspExecutable,
              IN Char8 * strNumIterations,
              IN Char8 * strProcessorId)
{
    DSP_STATUS status        = DSP_SOK ;
    Uint32     numIterations = 0       ;
    Uint8      processorId   = 0 ;

    MESSAGE_0Print ("========== Sample Application : MESSAGE ==========\n") ;

    if ((dspExecutable != NULL) && (strNumIterations != NULL)) {
        numIterations = MESSAGE_Atoi (strNumIterations) ;

        if (numIterations >  0xFFFF) {
            status = DSP_EINVALIDARG ;
            MESSAGE_1Print ("ERROR! Invalid arguments specified for  "
                            "message application.\n"
                            "     Max iterations = %d\n",
                            0xFFFF) ;
        }
        else {
            processorId = MESSAGE_Atoi (strProcessorId) ;

            if (processorId >= MAX_DSPS) {
                MESSAGE_1Print (
                            "== Error: Invalid processor id %d specified ==\n",
                             processorId) ;
                status = DSP_EFAIL ;

            }

            if (DSP_SUCCEEDED (status)) {
                /*
                 *  Specify the dsp executable file name for message creation
                 *  phase.
                 */
                status = MESSAGE_Create (dspExecutable,
                                         strNumIterations,
                                         processorId) ;

                /*
                 *  Execute the message execute phase.
                 */
                if (DSP_SUCCEEDED (status)) {
                    status = MESSAGE_Execute (numIterations,
                                              processorId) ;
                }

                /*
                 *  Perform cleanup operation.
                 */
                MESSAGE_Delete (processorId) ;
            }
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        MESSAGE_0Print ("ERROR! Invalid arguments specified for  "
                        "message application\n") ;
    }

    MESSAGE_0Print ("====================================================\n") ;
}

#if defined (VERIFY_DATA)
/** ----------------------------------------------------------------------------
 *  @func   MESSAGE_VerifyData
 *
 *  @desc   This function verifies the data-integrity of given buffer.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
MESSAGE_VerifyData (IN MSGQ_Msg msg, IN Uint16 sequenceNumber)
{
    DSP_STATUS status = DSP_SOK ;
    Uint16     msgId ;

    /*
     *  Verify the message
     */
    msgId = MSGQ_getMsgId(msg);
    if (msgId != sequenceNumber) {
        status = DSP_EFAIL;
        MESSAGE_0Print ("ERROR! Data integrity check failed\n") ;
    }

    return status ;
}
#endif /* if defined (VERIFY_DATA) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
