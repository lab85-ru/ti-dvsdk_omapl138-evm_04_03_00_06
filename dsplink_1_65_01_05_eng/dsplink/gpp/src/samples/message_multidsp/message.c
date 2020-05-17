/** ============================================================================
 *  @file   message.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/message_multidsp/
 *
 *  @desc   This is an application which sends messages to the DSP
 *          processors and receives them back using DSP/BIOS LINK.
 *          The message contents received are verified against the data
 *          sent to DSP. It demonstrates communication  between host processor
 *          and the two DSP processors.
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
 *  @const  DSP_MSGQ_BASE_NAME_LENGTH
 *
 *  @desc   length of the DSP msgq base name
 *  ============================================================================
 */
#define DSP_MSGQ_BASE_NAME_LENGTH  30u

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
STATIC Char8 SampleDspMsgqBaseName [DSP_MSGQ_BASE_NAME_LENGTH] = "DSPMSGQ" ;

/** ============================================================================
 *  @name   SampleGppMsgq
 *
 *  @desc   Local GPP's MSGQ Object.
 *  ============================================================================
 */
STATIC MSGQ_Queue SampleGppMsgq = (Uint32) MSGQ_INVALIDMSGQ ;

/** ============================================================================
 *  @name   SampleDspMsgq_0
 *
 *  @desc   Handle to the  first DSP's MSGQ Object.
 *  ============================================================================
 */
STATIC MSGQ_Queue SampleDspMsgq_0 = (Uint32) MSGQ_INVALIDMSGQ ;

/** ============================================================================
 *  @name   SampleDspMsgq_1
 *
 *  @desc    Handle to the  second DSP's MSGQ Object.
 *  ============================================================================
 */
STATIC MSGQ_Queue SampleDspMsgq_1 = (Uint32) MSGQ_INVALIDMSGQ ;

/** ============================================================================
 *  @name   dspMsgqName_0
 *
 *  @desc   Place holder for the MSGQ name created on DSP ID 0
 *  ============================================================================
 */
Char8   dspMsgqName_0 [DSP_MAX_STRLEN] ;

/** ============================================================================
 *  @name   dspMsgqName_1
 *
 *  @desc   Place holder for the MSGQ name created on DSP ID 1.
 *  ============================================================================
 */
Char8   dspMsgqName_1 [DSP_MAX_STRLEN] ;

#if defined (VERIFY_DATA)
/** ----------------------------------------------------------------------------
 *  @func   MESSAGEMULTIDSP_VerifyData
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
MESSAGEMULTIDSP_VerifyData (IN MSGQ_Msg msg, IN Uint16 sequenceNumber) ;
#endif /* if defined (VERIFY_DATA) */


#if  (DM6437_PHYINTERFACE == PCI_INTERFACE )
/** ============================================================================
 *  @name   DM6437_PCI_Config_0
 *
 *  @desc   DSP/BIOS LINK configuration structure for LC Processsor id 0 .
 *  ============================================================================
 */
extern LINKCFG_DspConfig  DM6437_PCI_Config_0 ;

/** ============================================================================
 *  @name   DM6437_PCI_Config_1
 *
 *  @desc   DSP/BIOS LINK configuration structure for LC Processsor id 1 .
 *  ============================================================================
 */
extern LINKCFG_DspConfig  DM6437_PCI_Config_1 ;
#endif


/** ============================================================================
 *  @func   MESSAGEMULTIDSP_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @modif  MESSAGE_InpBufs , MESSAGE_OutBufs
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGEMULTIDSP_Create (IN Char8 * dspExecutable_0,
                        IN Char8 * dspExecutable_1,
                        IN Char8 * strNumIterations )
{
    DSP_STATUS       status  = DSP_SOK  ;
    Uint32           numArgs = NUM_ARGS ;
    MSGQ_LocateAttrs syncLocateAttrs ;
    Char8 *          args [NUM_ARGS] ;
#if  (DM6437_PHYINTERFACE == PCI_INTERFACE )
    PROC_Attrs       procAttrs ;
#endif

    MESSAGEMULTIDSP_0Print ("Entered MESSAGEMULTIDSP_Create ()\n") ;

    /*
     *  Create and initialize the proc object.
     */
    status = PROC_setup (NULL) ;

    /*
     *  Attach the first Dsp with which the transfers have to be done.
     */
    if (DSP_SUCCEEDED (status)) {
#if  (DM6437_PHYINTERFACE == PCI_INTERFACE )
        procAttrs.dspCfgPtr = (LINKCFG_DspConfig *) &DM6437_PCI_Config_0 ;
        status = PROC_attach (ID_DSP_PROCESSOR_0, &procAttrs) ;
#else
        status = PROC_attach (ID_DSP_PROCESSOR_0, NULL) ;
#endif
        if (DSP_FAILED (status)) {
            MESSAGEMULTIDSP_2Print ("PROC_attach () failed for DSP processsor"
                                    "%d. Status = [0x%x]\n",
                                    ID_DSP_PROCESSOR_0, status) ;
        }
    }
     else {
        MESSAGEMULTIDSP_1Print ("PROC_setup () failed. Status = [0x%x]\n",
                                status) ;
    }

    /*
     *  Attach the second Dsp with which the transfers have to be done.
     */
    if (DSP_SUCCEEDED (status)) {
#if  (DM6437_PHYINTERFACE == PCI_INTERFACE )
         procAttrs.dspCfgPtr = (LINKCFG_DspConfig *) &DM6437_PCI_Config_1 ;
        status = PROC_attach (ID_DSP_PROCESSOR_1, &procAttrs) ;
#else
        status = PROC_attach (ID_DSP_PROCESSOR_1, NULL) ;
#endif

        if (DSP_FAILED (status)) {
            MESSAGEMULTIDSP_2Print ("PROC_attach () failed for DSP processsor"
                                    "%d.Status = [0x%x]\n",
                                    ID_DSP_PROCESSOR_1, status) ;
        }
    }

    /*
     *  Open the pool to be used for  first processor.
     */
    if (DSP_SUCCEEDED (status)) {
        status = POOL_open (POOL_makePoolId(ID_DSP_PROCESSOR_0, SAMPLE_POOL_ID),
                            &SamplePoolAttrs) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTIDSP_1Print ("POOL_open () failed. Status = [0x%x]\n",
                            status) ;
        }
    }


    /*
     *  Open the pool to be used for  first processor.
     */
    if (DSP_SUCCEEDED (status)) {
        status = POOL_open (POOL_makePoolId(ID_DSP_PROCESSOR_1, SAMPLE_POOL_ID),
                            &SamplePoolAttrs) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTIDSP_1Print ("POOL_open () failed. Status = [0x%x]\n",
                            status) ;
        }
    }


    /*
     *  Open the GPP's message queue
     */
    if (DSP_SUCCEEDED (status)) {
        status = MSGQ_open (SampleGppMsgqName, &SampleGppMsgq, NULL) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTIDSP_1Print ("MSGQ_open () failed. Status = [0x%x]\n",
                            status) ;
        }
    }

    /*
     *  Set the gpp message queue that will receive any async. errors
     *  can use any pool irrespective of DSP processor.
     */
    if (DSP_SUCCEEDED (status)) {
        status = MSGQ_setErrorHandler (SampleGppMsgq,
                                       POOL_makePoolId(ID_DSP_PROCESSOR_0,
                                                       SAMPLE_POOL_ID)) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTIDSP_1Print ("MSGQ_setErrorHandler () failed. "
                                    "Status = [0x%x]\n", status) ;
        }
    }

    /*
     *  Load the executable on to the first  DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        args [0] = strNumIterations ;
        status = PROC_load (ID_DSP_PROCESSOR_0,
                            dspExecutable_0,
                            numArgs,
                            args) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTIDSP_2Print ("PROC_load () failed for DSP processor ID"
                                    "%d .Status = [0x%x]\n",
                                    ID_DSP_PROCESSOR_0, status) ;
        }
    }


    /*
     *  Load the executable on to the second  DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        args [0] = strNumIterations ;
        status = PROC_load (ID_DSP_PROCESSOR_1,
                            dspExecutable_1,
                            numArgs,
                            args) ;
        if (DSP_FAILED (status)) {
             MESSAGEMULTIDSP_2Print ("PROC_load () failed for DSP processor ID"
                                     "%d .Status = [0x%x]\n",
                                     ID_DSP_PROCESSOR_1, status) ;
        }
    }

    /*
     *  Start execution on first DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        status = PROC_start (ID_DSP_PROCESSOR_0) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTIDSP_2Print ("PROC_start () failed for DSP processor ID"
                                    "%d .Status = [0x%x]\n",
                                    ID_DSP_PROCESSOR_0, status) ;
        }
    }

    /*
     *  Start execution on second DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        status = PROC_start (ID_DSP_PROCESSOR_1) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTIDSP_2Print ("PROC_start () failed for DSP processor ID"
                                    "%d .Status = [0x%x]\n",
                                    ID_DSP_PROCESSOR_1, status) ;
        }
    }

    /*
     *  Open the remote transport for first DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        mqtAttrs.poolId = POOL_makePoolId(ID_DSP_PROCESSOR_0, SAMPLE_POOL_ID)  ;
        status = MSGQ_transportOpen (ID_DSP_PROCESSOR_0, &mqtAttrs) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTIDSP_2Print ("MSGQ_transportOpen () failed for DSP "
                                    "processor ID %d .Status = [0x%x]\n",
                                    ID_DSP_PROCESSOR_0, status) ;
        }
     }

    /*
     *  Open the remote transport for second DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        mqtAttrs.poolId = POOL_makePoolId(ID_DSP_PROCESSOR_1, SAMPLE_POOL_ID)  ;
        status = MSGQ_transportOpen (ID_DSP_PROCESSOR_1, &mqtAttrs) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTIDSP_2Print ("MSGQ_transportOpen () failed for DSP "
                                    "processor ID %d . Status = [0x%x]\n",
                                    ID_DSP_PROCESSOR_1, status) ;
        }
     }


    /*
     * Locate the  First DSP's message queue
     */
    if (DSP_SUCCEEDED (status)) {
        /* Get the First DSP MSGQ name by concatinating the Base DSP name with
         * processor Id
         */
        sprintf(dspMsgqName_0 ,
                "%s%d",
                SampleDspMsgqBaseName,
                ID_DSP_PROCESSOR_0 ) ;
        MESSAGEMULTIDSP_1Print (" MSGQ Name of DSP Processor Id  0 :%s  \n",
                                        (Uint32)dspMsgqName_0) ;
        syncLocateAttrs.timeout = WAIT_FOREVER;
        status = DSP_ENOTFOUND ;
        while ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
            status = MSGQ_locate (dspMsgqName_0,
                                  &SampleDspMsgq_0,
                                  &syncLocateAttrs) ;
            if ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
                MESSAGEMULTIDSP_Sleep (100000) ;
            }
            else if (DSP_FAILED (status)) {
                MESSAGEMULTIDSP_1Print ("0MSGQ_locate () failed for MSGQ %s ",
                                        (Uint32)dspMsgqName_0) ;
                MESSAGEMULTIDSP_1Print ("0Status = [0x%x]\n",
                                        (Uint32)status) ;
            }
        }
    }

    /*
     *  Locate the  second DSP's message queue
     */
    if (DSP_SUCCEEDED (status)) {
        /* Get the First DSP MSGQ name by concatinating the Base DSP name with
         * processor Id
         */
        sprintf(dspMsgqName_1 ,
                "%s%d",
                SampleDspMsgqBaseName,
                ID_DSP_PROCESSOR_1 ) ;
        MESSAGEMULTIDSP_1Print ("MSGQ Name of DSP Processor Id  1 : %s \n ",
                                (Uint32)dspMsgqName_1) ;
        syncLocateAttrs.timeout = WAIT_FOREVER;
        status = DSP_ENOTFOUND ;
        while ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
            status = MSGQ_locate (dspMsgqName_1,
                                  &SampleDspMsgq_1,
                                  &syncLocateAttrs) ;
            if ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
                MESSAGEMULTIDSP_Sleep (100000) ;
            }
            else if (DSP_FAILED (status)) {
                   MESSAGEMULTIDSP_2Print ("1MSGQ_locate () failed for MSGQ %s "
                                           "1Status = [0x%x]\n",
                                           (Uint32)dspMsgqName_1, status) ;
            }
        }
    }

    MESSAGEMULTIDSP_0Print ("Leaving MESSAGEMULTIDSP_Create ()\n") ;

    return status ;
}


/** ============================================================================
 *  @func   MESSAGEMULTIDSP_Execute
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGEMULTIDSP_Execute (IN Uint32 numIterations)
{
    DSP_STATUS  status           = DSP_SOK ;
    Uint16      sequenceNumber_0 = 0 ;
    Uint16      sequenceNumber_1 = 0 ;
    Uint16      refMsgId         = 0 ;
    Uint16      msgId            = 0 ;
    MSGQ_Queue  dspMsgq          = 0;
    Uint32      i ;
    MSGQ_Msg    msg ;


    MESSAGEMULTIDSP_0Print ("Entered MESSAGEMULTIDSP_Execute ()\n") ;

    for (i = 1 ;
         (   (numIterations == 0) || (i <= (numIterations + 1)))
          && (DSP_SUCCEEDED (status)) ;
         i++) {
        /*
         *  Receive the message.
         */
        status = MSGQ_get (SampleGppMsgq, WAIT_FOREVER, &msg) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTIDSP_1Print ("MSGQ_get () failed. Status = [0x%x]\n",
                            status) ;
        }

#if defined (VERIFY_DATA)
        if (msg->srcProcId == ID_DSP_PROCESSOR_0 ) {
            /*  Message is from first DSP processor */
            refMsgId = sequenceNumber_0 ;
        }
        else if (msg->srcProcId == ID_DSP_PROCESSOR_1 ) {
            /*  Message is from second DSP processor */
            refMsgId = sequenceNumber_1 ;
        }
        else {
            MESSAGEMULTIDSP_1Print ("== ERROR: MESSAGEMULTIDSP from unknown DSP"
                                    "processor %d == \n",
                                    msg->srcProcId) ;
        }
        /*
         *  Verify correctness of data received.
         */
        if (DSP_SUCCEEDED (status)) {
            status = MESSAGEMULTIDSP_VerifyData (msg, refMsgId) ;
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

            if (msg->srcProcId == ID_DSP_PROCESSOR_0 ) {
                /*  Message is from first DSP processor */
                sequenceNumber_0++ ;

                dspMsgq = SampleDspMsgq_0 ;
                /* Make sure that the sequenceNumber stays within the permitted
                 * range for applications.
                 */
                MESSAGEMULTIDSP_1Print ("Message from  "
                                        "processor %d  \n",
                                        msg->srcProcId) ;
                if (sequenceNumber_0 == MSGQ_INTERNALIDSSTART) {
                    sequenceNumber_0 = 0 ;
                }
            }
            else if (msg->srcProcId == ID_DSP_PROCESSOR_1 ) {
                /*  Message is from second DSP processor */
                sequenceNumber_1++ ;
                MESSAGEMULTIDSP_1Print ("Message from  "
                                        "processor %d  \n",
                                        msg->srcProcId) ;
                dspMsgq = SampleDspMsgq_1 ;
                /* Make sure that the sequenceNumber stays within the permitted
                 * range for applications.
                 */
                if (sequenceNumber_1 == MSGQ_INTERNALIDSSTART) {
                    sequenceNumber_1 = 0 ;
                }
            }
            else {
                MESSAGEMULTIDSP_1Print ("== ERROR: Message from unknown DSP "
                                        "processor %d == \n",
                                        msg->srcProcId) ;
            }

            /*
             *  Send the same message received in earlier MSGQ_get () call.
             */
            if (DSP_SUCCEEDED (status)) {
                msgId = MSGQ_getMsgId (msg) ;
                MSGQ_setMsgId (msg, msgId) ;
                status = MSGQ_put (dspMsgq, msg) ;
                if (DSP_FAILED (status)) {
                    MSGQ_free (msg) ;
                    MESSAGEMULTIDSP_1Print ("MSGQ_put () failed. Status "
                                            "= [0x%x]\n", status) ;
                }
            }

            if (DSP_SUCCEEDED (status) && ((i % 100) == 0)) {
                MESSAGEMULTIDSP_1Print ("Transferred %ld messages\n", i) ;
                MESSAGEMULTIDSP_1Print ("    %ld messages to DSP 0\n",
                                        sequenceNumber_0) ;
                MESSAGEMULTIDSP_1Print ("    %ld messages to DSP 1\n",
                                        sequenceNumber_1) ;
            }
        }
    }

    MESSAGEMULTIDSP_0Print ("Leaving MESSAGEMULTIDSP_Execute ()\n") ;

    return status ;
}


/** ============================================================================
 *  @func   MESSAGEMULTIDSP_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          MESSAGEMULTIDSP_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGEMULTIDSP_Delete ()
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;

    MESSAGEMULTIDSP_0Print ("Entered MESSAGEMULTIDSP_Delete ()\n") ;

    /*
     *  Release the remote message queue of first DSP processor
     */
    status = MSGQ_release (SampleDspMsgq_0) ;
    if (DSP_FAILED (status)) {
        MESSAGEMULTIDSP_1Print ("MSGQ_release () failed. Status = [0x%x]\n",
                                status) ;
    }
    /*
     *  Release the remote message queue of second DSP processor
     */
    status = MSGQ_release (SampleDspMsgq_1) ;
    if (DSP_FAILED (status)) {
        MESSAGEMULTIDSP_1Print ("MSGQ_release () failed. Status = [0x%x]\n",
                                status) ;
    }

    /*
     *  Close the remote transport  of first DSP Processor
     */
    tmpStatus = MSGQ_transportClose (ID_DSP_PROCESSOR_0) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTIDSP_2Print ("MSGQ_transportClose () failed for DSP %d."
                                "Status = [0x%x]\n",
                                ID_DSP_PROCESSOR_0, status) ;
    }

    /*
     *  Close the remote transport  of first DSP Processor
     */
    tmpStatus = MSGQ_transportClose (ID_DSP_PROCESSOR_1) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTIDSP_2Print ("MSGQ_transportClose () failed for DSP %d."
                                "Status = [0x%x]\n",
                                ID_DSP_PROCESSOR_1, status) ;
    }

    /*
     *  Stop execution on  first DSP.
     */
    tmpStatus = PROC_stop (ID_DSP_PROCESSOR_0) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTIDSP_2Print ("PROC_stop () failed for DSP %d."
                                "Status = [0x%x]\n",
                                ID_DSP_PROCESSOR_0, status) ;
    }

    /*
     *  Stop execution on second  DSP.
     */
    tmpStatus = PROC_stop (ID_DSP_PROCESSOR_1) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
         MESSAGEMULTIDSP_2Print ("PROC_stop () failed for DSP %d."
                                 "Status = [0x%x]\n",
                                 ID_DSP_PROCESSOR_1, status) ;
    }

    /*
     * Reset the error handler before deleting the MSGQ that receives
     * the error messages.
     */
    tmpStatus = MSGQ_setErrorHandler (MSGQ_INVALIDMSGQ, MSGQ_INVALIDMSGQ) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTIDSP_1Print ("MSGQ_setErrorHandler () failed."
                                "Status = [0x%x]\n", status) ;
    }

    /*
     *  Close the GPP's message queue
     */
    tmpStatus = MSGQ_close (SampleGppMsgq) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTIDSP_1Print ("MSGQ_close () failed. Status = [0x%x]\n",
                                status) ;
    }

    /*
     *  Close the pool of first DSP
     */
    tmpStatus = POOL_close (
                        POOL_makePoolId(ID_DSP_PROCESSOR_0, SAMPLE_POOL_ID)) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTIDSP_2Print ("POOL_close () failed for DSP %d."
                                "Status = [0x%x]\n",
                                ID_DSP_PROCESSOR_0, status) ;
    }

    /*
     *  Close the pool of second DSP
     */
    tmpStatus = POOL_close (
                         POOL_makePoolId(ID_DSP_PROCESSOR_1, SAMPLE_POOL_ID)) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTIDSP_2Print ("POOL_close () failed for DSP %d."
                                "Status = [0x%x]\n",
                                ID_DSP_PROCESSOR_1, status) ;
    }

    /*
     *  Detach from the  first DSP processor
     */
    tmpStatus = PROC_detach  (ID_DSP_PROCESSOR_0) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTIDSP_2Print ("PROC_detach () failed for DSP %d."
                                "Status = [0x%x]\n",
                                ID_DSP_PROCESSOR_0, status) ;
    }

    /*
     *  Detach from the  second DSP processor
     */
    tmpStatus = PROC_detach  (ID_DSP_PROCESSOR_1) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTIDSP_2Print ("PROC_detach () failed for DSP %d."
                                "Status = [0x%x]\n",
                                ID_DSP_PROCESSOR_1, status) ;
    }

    /*
     *  Destroy the PROC object.
     */
    tmpStatus = PROC_destroy () ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTIDSP_1Print ("PROC_destroy () failed. Status = [0x%x]\n",
                                status) ;
    }

    MESSAGEMULTIDSP_0Print ("Leaving MESSAGEMULTIDSP_Delete ()\n") ;
}


/** ============================================================================
 *  @func   MESSAGEMULTIDSP_Main
 *
 *  @desc   Entry point for the application
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGEMULTIDSP_Main (IN Char8 * dspExecutable_0,
                      IN Char8 * dspExecutable_1,
                      IN Char8 * strNumIterations)
{
    DSP_STATUS status        = DSP_SOK ;
    Uint32     numIterations = 0       ;


    MESSAGEMULTIDSP_0Print ("========== Sample Application : MESSAGEMULTIDSP "
                            "==========\n") ;

    if (   (dspExecutable_0 != NULL)
        && (dspExecutable_1 != NULL)
        && (strNumIterations != NULL)) {
        numIterations = MESSAGEMULTIDSP_Atoi (strNumIterations) ;

        if (numIterations >  0xFFFF) {
            status = DSP_EINVALIDARG ;
            MESSAGEMULTIDSP_1Print ("ERROR! Invalid arguments specified for  "
                                    "MESSAGEMULTIDSP application.\n"
                                    "     Max iterations = %d\n",
                                    0xFFFF) ;
        }
        else {

            if (DSP_SUCCEEDED (status)) {
                /*
                 *  Specify the dsp executable file name for message creation
                 *  phase.
                 */
                status = MESSAGEMULTIDSP_Create (dspExecutable_0,
                                                 dspExecutable_1,
                                                 strNumIterations) ;

                /*
                 *  Execute the MESSAGEMULTIDSP execute phase.
                 */
                if (DSP_SUCCEEDED (status)) {
                    status = MESSAGEMULTIDSP_Execute (numIterations * MAX_DSPS) ;
                }

                /*
                 *  Perform cleanup operation.
                 */
                MESSAGEMULTIDSP_Delete () ;
           }
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        MESSAGEMULTIDSP_0Print ("ERROR! Invalid arguments specified for  "
                                "MESSAGEMULTIDSP application\n") ;
    }

    MESSAGEMULTIDSP_0Print ("======================================"
                            "==============\n") ;
}


#if defined (VERIFY_DATA)
/** ----------------------------------------------------------------------------
 *  @func   MESSAGEMULTIDSP_VerifyData
 *
 *  @desc   This function verifies the data-integrity of given buffer.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
MESSAGEMULTIDSP_VerifyData (IN MSGQ_Msg msg, IN Uint16 sequenceNumber)
{
    DSP_STATUS status = DSP_SOK ;
    Uint16     msgId ;

    /*
     *  Verify the MESSAGEMULTIDSP
     */
    msgId = MSGQ_getMsgId(msg);
    if (msgId != sequenceNumber) {
        status = DSP_EFAIL;
        MESSAGEMULTIDSP_0Print ("ERROR! Data integrity check failed\n") ;
    }

    return status ;
}
#endif /* if defined (VERIFY_DATA) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
