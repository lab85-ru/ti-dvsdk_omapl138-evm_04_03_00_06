/** ============================================================================
 *  @file   message_multi.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/message_multi/
 *
 *  @desc   This application sends messages to the DSP processor and receives
 *          them back using DSP/BIOS LINK.
 *          The message contents received are verified against the data
 *          sent to DSP.
 *          This is a multi-application message sample. It takes the application
 *          number as a second parameter, and accordingly opens and
 *          locates the corresponding message queues, performing message
 *          transfers with the DSP.
 *          It also demonstrates usage of dynamic configuration.
 *          This application also opens one separate pool for each application
 *          instance.
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
#include <msgq.h>
#include <pool.h>
#include <notify.h>
#if defined (DA8XXGEM)
#include <loaderdefs.h>
#endif


/*  ----------------------------------- Application Header              */
#include <message_multi.h>
#include <message_multi_os.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @const   NUM_ARGS
 *
 *  @desc   Number of arguments specified to the DSP application.
 *  ============================================================================
 */
#define NUM_ARGS           1

/*  ============================================================================
 *  @const   NUM_POOLS
 *
 *  @desc   Number of pools to be configured.
 *  ============================================================================
 */
#define NUM_POOLS          (MAX_APPS + 1)

/** ============================================================================
 *  @const  MQT_POOL_ID
 *
 *  @desc   Pool ID of the common pool to be used for MSGQ transport.
 *  ============================================================================
 */
#define MQT_POOL_ID        0u

/** ============================================================================
 *  @const  MQT_POOL_NUM_SIZES
 *
 *  @desc   Number of buffer sizes in the common pool used for MSGQ transport.
 *  ============================================================================
 */
#define MQT_POOL_NUM_SIZES  1

/** ============================================================================
 *  @const  NUM_CTRL_MSG
 *
 *  @desc   Number of buffers to be reserved for MQT control messages.
 *  ============================================================================
 */
#define NUM_CTRL_MSG        (MAX_APPS * 2)

/** ============================================================================
 *  @const  NUMMSGPOOLS
 *
 *  @desc   Number of BUF pools in the pool used by each application instance.
 *  ============================================================================
 */
#define NUMMSGPOOLS         1

/** ============================================================================
 *  @const  NUMMSGINPOOL0
 *
 *  @desc   Number in first message pool.
 *  ============================================================================
 */
#define NUMMSGINPOOL0       1

/** ============================================================================
 *  @name   MESSAGE_IPS_ID
 *
 *  @desc   IPS ID to be used with NOTIFY module.
 *  ============================================================================
 */
#define MESSAGE_IPS_ID      0

/** ============================================================================
 *  @name   MESSAGE_IPS_EVENTNO_BASE
 *
 *  @desc   Base IPS event number to be used witin the MESSAGE_IPS_ID.
 *  ============================================================================
 */
#define MESSAGE_IPS_EVENTNO_BASE 5

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
 *  Definitions required for the sample that vary based on physical link
 *  being used for the MQT.
 *  ============================================================================
 */
STATIC ZCPYMQT_Attrs  mqtAttrs ;

/** ============================================================================
 *  @name   SampleBufSizes
 *
 *  @desc   Message sizes managed by the pool.
 *  ============================================================================
 */
STATIC Uint32 SampleBufSizes [NUMMSGPOOLS] =
{
    APP_BUFFER_SIZE
} ;

/** ============================================================================
 *  @name   SampleNumBuffers
 *
 *  @desc   Number of messages in each pool managed by the pool.
 *  ============================================================================
 */
STATIC Uint32 SampleNumBuffers [NUMMSGPOOLS] =
{
    NUMMSGINPOOL0
} ;


/** ============================================================================
 *  @name   SamplePoolAttrs
 *
 *  @desc   Definition of attributes for the pool based on physical link used
 *          by the MQT.
 *  ============================================================================
 */
#if (defined (PCIINTERFACE) || defined (VLYNQINTERFACE))
STATIC DMAPOOL_Attrs SamplePoolAttrs =
{
    NUMMSGPOOLS,
    SampleBufSizes,
    SampleNumBuffers,
    TRUE
} ;
#else
STATIC SMAPOOL_Attrs SamplePoolAttrs =
{
    NUMMSGPOOLS,
    SampleBufSizes,
    SampleNumBuffers,
    TRUE
} ;
#endif


/** ============================================================================
 *  @const  MsgqGpp1
 *
 *  @desc   Name of the first MSGQ on the GPP.
 *  ============================================================================
 */
STATIC Char8 SampleGppMsgqName [DSP_MAX_STRLEN] = "GPPMSGQ" ;

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
 *  @name   SampleAppNum
 *
 *  @desc   Application instance number for this application.
 *  ============================================================================
 */
STATIC Uint32 SampleAppNum = 0 ;

/** ============================================================================
 *  @name   MESSAGE_SemPtr
 *
 *  @desc   Pointer to the semaphore used for synchronizing with the DSP-side
 *          application.
 *  ============================================================================
 */
STATIC Void * MESSAGE_SemPtr = NULL ;

/** ============================================================================
 *  @name   LINKCFG_config
 *
 *  @desc   Extern declaration to the default DSP/BIOS LINK configuration
 *          structure.
 *  ============================================================================
 */
extern  LINKCFG_Object LINKCFG_config ;

/** ============================================================================
 *  @name   LINKCFG_messageMultiPoolTables
 *
 *  @desc   Extern declaration of array of Pool tables in the system.
 *  ============================================================================
 */
extern  LINKCFG_Pool * LINKCFG_messageMultiPoolTables [] ;

/** ----------------------------------------------------------------------------
 *  @func   MESSAGE_Notify
 *
 *  @desc   This function implements the event callback registered with the
 *          NOTIFY component to receive notification indicating that the DSP-
 *          side application has opened the pool.
 *
 *  @arg    eventNo
 *              Event number associated with the callback being invoked.
 *  @arg    arg
 *              Fixed argument registered with the IPS component along with
 *              the callback function.
 *  @arg    info
 *              Run-time information provided to the upper layer by the NOTIFY
 *              component. This information is specific to the IPS being
 *              implemented.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ----------------------------------------------------------------------------
 */
STATIC
Void
MESSAGE_Notify (Uint32 eventNo, Pvoid arg, Pvoid info) ;


#if defined (VERIFY_DATA)
/** ----------------------------------------------------------------------------
 *  @func   MESSAGEMULTI_VerifyData
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
MESSAGEMULTI_VerifyData (IN MSGQ_Msg msg, IN Uint16 sequenceNumber) ;
#endif /* if defined (VERIFY_DATA) */


/** ============================================================================
 *  @func   MESSAGEMULTI_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @modif  MESSAGE_InpBufs , MESSAGE_OutBufs
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGEMULTI_Create (IN Char8 * dspExecutable,
                     IN Char8 * strNumIterations,
                     IN Char8 * strAppNum,
                     IN Uint8   processorId)
{
    DSP_STATUS       status                           = DSP_SOK  ;
    Uint32           numArgs                          = NUM_ARGS ;
    Uint32           poolId                           = (POOL_makePoolId (processorId, SampleAppNum)) ;
    Uint32           mqtNumBufs  [MQT_POOL_NUM_SIZES] = {NUM_CTRL_MSG} ;
    Uint32           mqtBufSizes [MQT_POOL_NUM_SIZES] = {ZCPYMQT_CTRLMSG_SIZE} ;
    Uint8            i                                = 0 ;
    MSGQ_LocateAttrs syncLocateAttrs ;
    Char8 *          args [NUM_ARGS] ;
    Char8 *          gppMsgqNamePtr ;
    Char8 *          dspMsgqNamePtr ;
    Char8            gppMsgqName [DSP_MAX_STRLEN] ;
    Char8            dspMsgqName [DSP_MAX_STRLEN] ;
    DMAPOOL_Attrs    mqtPoolParams ;
    PROC_Attrs       procAttrs ;
#if defined (DA8XXGEM)
    NOLOADER_ImageInfo  imageInfo ;
#endif


    MESSAGEMULTI_1Print ("App [%2d]: Entered MESSAGEMULTI_Create ()\n",
                          SampleAppNum) ;

    gppMsgqNamePtr = MESSAGEMULTI_Strcpy (gppMsgqName, SampleGppMsgqName) ;
    gppMsgqNamePtr = MESSAGEMULTI_Strcat (gppMsgqName, strAppNum) ;
    dspMsgqNamePtr = MESSAGEMULTI_Strcpy (dspMsgqName, SampleDspMsgqName) ;
    dspMsgqNamePtr = MESSAGEMULTI_Strcat (dspMsgqName, strAppNum) ;

#if defined(OS_WINCE)
    MESSAGEMULTI_2PrintStr ("App [%2d]: gppMsgqName [%s]\n",
                    SampleAppNum,
                    gppMsgqNamePtr) ;
    MESSAGEMULTI_2PrintStr ("App [%2d]: dspMsgqName [%s]\n",
                    SampleAppNum,
                    dspMsgqNamePtr) ;

#else
    
    MESSAGEMULTI_2Print ("App [%2d]: gppMsgqName [%s]\n",
                    SampleAppNum,
                    (Uint32) gppMsgqNamePtr) ;
    MESSAGEMULTI_2Print ("App [%2d]: dspMsgqName [%s]\n",
                    SampleAppNum,
                    (Uint32) dspMsgqNamePtr) ;

#endif    

    /*
     *  OS initialization
     */
    status = MESSAGEMULTI_OS_init () ;

    /*
     *  Create and initialize the proc object.
     */

    if (DSP_SUCCEEDED (status)) {
        /* Update the dynamic configuration to be able to open multiple POOLs.*/
        LINKCFG_config.dspConfigs [processorId]->linkDrvObjects [0].numPools =
                                                                     NUM_POOLS ;

        /* Copy the default pool configuration  */
        for ( i = 0;  i < NUM_POOLS;  i++) {
            memcpy (&LINKCFG_messageMultiPoolTables[0][i],
                     LINKCFG_config.dspConfigs [processorId]->poolTables[0],
                     sizeof (LINKCFG_Pool) );
             LINKCFG_messageMultiPoolTables[0][i].poolSize = 0x5000u ;
        }

        LINKCFG_config.dspConfigs [processorId]->poolTables = (LINKCFG_Pool **)
                                                LINKCFG_messageMultiPoolTables ;

        /* Use the dynamic configuration for PROC_setup */
        status = PROC_setup ((LINKCFG_Object *)&LINKCFG_config) ;
        MESSAGEMULTI_2Print ("App [%2d]: PROC_setup status [0x%x]\n",
                SampleAppNum,
                status) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTI_2Print ("App [%2d]: PROC_setup () failed. status [0x%x]\n",
                    SampleAppNum,
                    status) ;
        }
    }

    /*
     *  Attach the Dsp with which the transfers have to be done.
     */
    if (DSP_SUCCEEDED (status)) {
        procAttrs.dspCfgPtr = (LINKCFG_DspConfig *)
                                      LINKCFG_config.dspConfigs [processorId] ;
        status = PROC_attach (processorId, &procAttrs) ;
        MESSAGEMULTI_2Print ("App [%2d]: PROC_attach status [0x%x]\n",
                             SampleAppNum,
                             status) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTI_2Print ("App [%2d]: PROC_attach () failed."
                                 " status [0x%x]\n",
                                 SampleAppNum,
                                 status) ;
        }
    }

    /*
     *  Open the common pool to be used for MSGQ transport.
     */
    if (DSP_SUCCEEDED (status)) {
        mqtPoolParams.numBufPools   = MQT_POOL_NUM_SIZES ;
        mqtPoolParams.bufSizes      = mqtBufSizes ;
        mqtPoolParams.numBuffers    = mqtNumBufs ;
        mqtPoolParams.exactMatchReq = TRUE ;

        status = POOL_open (
                            POOL_makePoolId(processorId,MQT_POOL_ID),
                            &mqtPoolParams) ;
        MESSAGEMULTI_2Print ("App [%2d]: POOL_open MQT_POOL_ID status [0x%x]\n",
                             SampleAppNum,
                             status) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTI_2Print ("App [%2d]: POOL_open () failed.  Pool ID [%d]"
                                 " status [0x%x]\n",
                                 SampleAppNum,
                                 status) ;
        }
    }

    /*
     *  Open the GPP's message queue
     */
    if (DSP_SUCCEEDED (status)) {
        status = MSGQ_open (gppMsgqNamePtr, &SampleGppMsgq, NULL) ;
        MESSAGEMULTI_2Print ("App [%2d]: MSGQ_open status [0x%x]\n",
                             SampleAppNum,
                             status) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTI_2Print ("App [%2d]: MSGQ_open () failed."
                                 " status [0x%x]\n",
                                 SampleAppNum,
                                 status) ;
        }
    }

    /*
     *  Load the executable on the DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        args [0] = strNumIterations ;
#if defined (DA8XXGEM)
        if (LINKCFG_config.dspConfigs [processorId]->dspObject->doDspCtrl ==
                     DSP_BootMode_NoBoot) {
            imageInfo.dspRunAddr  = MESSAGE_dspAddr ;
            imageInfo.shmBaseAddr = MESSAGE_shmAddr ;
            imageInfo.argsAddr    = MESSAGE_argsAddr ;
            imageInfo.argsSize    = 50         ;
            status = PROC_load (processorId, (Char8 *) &imageInfo, numArgs, args) ;
        }
        else
#endif

        {
            status = PROC_load (processorId, dspExecutable, numArgs, args) ;
        }
        MESSAGEMULTI_2Print ("App [%2d]: PROC_load status [0x%x]\n",
                             SampleAppNum,
                             status) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTI_2Print ("App [%2d]: PROC_load () failed."
                                 " status [0x%x]\n",
                                 SampleAppNum,
                                 status) ;
        }
    }

     /*
      *  Start execution on DSP.
      */
    if (DSP_SUCCEEDED (status)) {
        status = PROC_start (processorId) ;
        MESSAGEMULTI_2Print ("App [%2d]: PROC_start status [0x%x]\n",
                             SampleAppNum,
                             status) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTI_2Print ("App [%2d]: PROC_start () failed."
                                 " status [0x%x]\n",
                                 SampleAppNum,
                                 status) ;
        }
    }

    /*
     *  Open the pool.
     */
    if (DSP_SUCCEEDED (status)) {
        status = POOL_open (poolId, &SamplePoolAttrs) ;
        MESSAGEMULTI_2Print ("App [%2d]: POOL_open App Pool status [0x%x]\n",
                             poolId,
                             status) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTI_2Print ("App [%2d]: POOL_open () failed. App Pool"
                                 " status [0x%x]\n",
                                 poolId,
                                 status) ;
        }
    }

    /*
     *  Open the remote transport.
     */
    if (DSP_SUCCEEDED (status)) {
        mqtAttrs.poolId = POOL_makePoolId(processorId, MQT_POOL_ID );
        status = MSGQ_transportOpen (processorId, &mqtAttrs) ;
        MESSAGEMULTI_2Print ("App [%2d]: MSGQ_transportOpen status [0x%x]\n",
                             SampleAppNum,
                             status) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTI_2Print ("App [%2d]: MSGQ_transportOpen () failed. "
                                 " status [0x%x]\n",
                                 SampleAppNum,
                                 status) ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        /* Create the semaphore to be used for notification */
        status = MESSAGEMULTI_CreateSem (&MESSAGE_SemPtr) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTI_2Print ("App [%2d]: MESSAGEMULTI_CreateSem () failed. "
                                 " status [0x%x]\n",
                                 SampleAppNum,
                                 status) ;
        }
    }

    /*
     *  Register for notification from DSP-side.
     */
    if (DSP_SUCCEEDED (status)) {
        status = NOTIFY_register (processorId,
                                  MESSAGE_IPS_ID,
                                  MESSAGE_IPS_EVENTNO_BASE,
                                  MESSAGE_Notify,
                                  MESSAGE_SemPtr) ;
        MESSAGEMULTI_2Print ("App [%2d]: NOTIFY_register status [0x%x]\n",
                             SampleAppNum,
                             status) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTI_2Print ("App [%2d]: NOTIFY_register () failed."
                                 " status [0x%x]\n",
                                 SampleAppNum,
                                 status) ;
        }
    }

    /* Send notification to DSP task about its application number. */
    if (DSP_SUCCEEDED (status)) {
        /* Try to send notification till the DSP-side task registers its
         * notifier.
         */
        do {
            status = NOTIFY_notify (processorId,
                                    MESSAGE_IPS_ID,
                                    (MESSAGE_IPS_EVENTNO_BASE + SampleAppNum),
                                    SampleAppNum) ;
        }
        while (status == DSP_ENOTREADY) ;
        MESSAGEMULTI_2Print ("App [%2d]: NOTIFY_notify status [0x%x]\n",
                             SampleAppNum,
                             status) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTI_2Print ("App [%2d]: NOTIFY_notify () failed."
                                 " status [0x%x]\n",
                                 SampleAppNum,
                                 status) ;
        }
    }

    /*
     *  Wait for the DSP-side application to indicate that it has opened its
     *  pool.
     */
    if (DSP_SUCCEEDED (status)) {
        status = MESSAGEMULTI_WaitSem (MESSAGE_SemPtr) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTI_2Print ("App [%2d]: MESSAGE_WaitSem () failed. "
                                 " status [0x%x]\n",
                                 SampleAppNum,
                                 status) ;
        }
    }

    /*
     *  Locate the DSP's message queue
     */
    if (DSP_SUCCEEDED (status)) {
        syncLocateAttrs.timeout = WAIT_FOREVER;
        status = DSP_ENOTFOUND ;
        while ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
            status = MSGQ_locate (dspMsgqNamePtr,
                                  &SampleDspMsgq,
                                  &syncLocateAttrs) ;
            if ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
                MESSAGEMULTI_Sleep (100000) ;
            }
            else if (DSP_FAILED (status)) {
                MESSAGEMULTI_2Print ("App [%2d]: MSGQ_locate () failed. "
                                     " status [0x%x]\n",
                                     SampleAppNum,
                                     status) ;
            }
        }
    }

    MESSAGEMULTI_1Print ("App [%2d]: Leaving MESSAGEMULTI_Create ()\n",
                         SampleAppNum) ;

    return status ;
}


/** ============================================================================
 *  @func   MESSAGEMULTI_Execute
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGEMULTI_Execute (IN Uint32 numIterations, Uint8 processodId)
{
    DSP_STATUS  status         = DSP_SOK ;
    Uint16      sequenceNumber = 0 ;
    Uint16      msgId          = 0 ;
    Uint32      i ;
    MSGQ_Msg    msg ;

    MESSAGEMULTI_1Print ("App [%2d]: Entered MESSAGEMULTI_Execute ()\n",
                         SampleAppNum) ;

    for (i = 1 ;
         (   (numIterations == 0) || (i <= (numIterations + 1)))
          && (DSP_SUCCEEDED (status)) ;
         i++) {
        /*
         *  Receive the message.
         */
        status = MSGQ_get (SampleGppMsgq, WAIT_FOREVER, &msg) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTI_2Print ("App [%2d]: MSGQ_get () failed."
                                 " status [0x%x]\n",
                                 SampleAppNum,
                                 status) ;
        }
#if defined (VERIFY_DATA)
        /*
         *  Verify correctness of data received.
         */
        if (DSP_SUCCEEDED (status)) {
            status = MESSAGEMULTI_VerifyData (msg, sequenceNumber) ;
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
                    MESSAGEMULTI_2Print ("MSGQ_put () failed.  App [%2d]"
                                         " status [0x%x]\n",
                                         SampleAppNum,
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

            if (DSP_SUCCEEDED (status) && ((i % 100) == 0)) {
                MESSAGEMULTI_2Print ("App [%2d]: Transferred %d messages\n",
                                     SampleAppNum,
                                     i) ;
            }
        }
    }

    MESSAGEMULTI_1Print ("App [%2d]: Leaving MESSAGEMULTI_Execute ()\n",
                         SampleAppNum) ;

    return status ;
}


/** ============================================================================
 *  @func   MESSAGEMULTI_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          MESSAGEMULTI_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGEMULTI_Delete (Uint8 processorId)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    Uint32     poolId    = POOL_makePoolId(processorId, SampleAppNum) ;

    MESSAGEMULTI_1Print ("App [%2d]: Entered MESSAGEMULTI_Delete ()\n",
                         SampleAppNum) ;

    /*
     *  Release the remote message queue
     */
    status = MSGQ_release (SampleDspMsgq) ;
    MESSAGEMULTI_2Print ("App [%2d]: MSGQ_release status [0x%x]\n",
                         SampleAppNum,
                         status) ;
    if (DSP_FAILED (status)) {
        MESSAGEMULTI_2Print ("App [%2d]: MSGQ_release () failed."
                             " status [0x%x]\n",
                             SampleAppNum,
                             status) ;
    }

    /*
     *  Close the remote transport
     */
    tmpStatus = MSGQ_transportClose (processorId) ;
    MESSAGEMULTI_2Print ("App [%2d]: MSGQ_transportClose status [0x%x]\n",
                         SampleAppNum,
                         tmpStatus) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTI_2Print ("App [%2d]: MSGQ_transportClose () failed. "
                             " status [0x%x]\n",
                             SampleAppNum,
                             status) ;
    }

    /*
     *  Unregister for notification of event registered earlier.
     */
    tmpStatus = NOTIFY_unregister (processorId,
                                   MESSAGE_IPS_ID,
                                   MESSAGE_IPS_EVENTNO_BASE,
                                   MESSAGE_Notify,
                                   MESSAGE_SemPtr) ;
    MESSAGEMULTI_2Print ("App [%2d]: NOTIFY_unregister status [0x%x]\n",
                         SampleAppNum,
                         tmpStatus) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTI_2Print ("App [%2d]: NOTIFY_unregister () failed."
                             " status [0x%x]\n",
                             SampleAppNum,
                             status) ;
    }

    /*
     *  Delete the semaphore used for notification.
     */
    if (MESSAGE_SemPtr != NULL) {
        tmpStatus = MESSAGEMULTI_DeleteSem (MESSAGE_SemPtr) ;
        MESSAGE_SemPtr = NULL ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            MESSAGEMULTI_2Print ("App [%2d]: PROC_setup () failed."
                                 " status [0x%x]\n",
                                 SampleAppNum,
                                 status) ;
        }
    }

    /*
     *  Stop execution on DSP.
     */
    tmpStatus = PROC_stop (processorId) ;
    MESSAGEMULTI_2Print ("App [%2d]: PROC_stop status [0x%x]\n",
                         SampleAppNum,
                         status) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTI_2Print ("App [%2d]: PROC_stop () failed. status [0x%x]\n",
                             SampleAppNum,
                             status) ;
    }

    /*
     *  Close the GPP's message queue
     */
    tmpStatus = MSGQ_close (SampleGppMsgq) ;
    MESSAGEMULTI_2Print ("App [%2d]: MSGQ_close status [0x%x]\n",
                         SampleAppNum,
                         tmpStatus) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTI_2Print ("App [%2d]: MSGQ_close () failed. status [0x%x]\n",
                             SampleAppNum,
                             status) ;
    }

    /*
     *  Close the pool
     */
    tmpStatus = POOL_close (poolId) ;
    MESSAGEMULTI_2Print ("App [%2d]: POOL_close App Pool status [0x%x]\n",
                         SampleAppNum,
                         tmpStatus) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTI_2Print ("App [%2d]: POOL_close () failed."
                             "  App Pool status [0x%x]\n",
                             SampleAppNum,
                             status) ;
    }

    /*
     *  Close the common pool used by MSGQ transport.
     */
    tmpStatus = POOL_close (POOL_makePoolId(processorId, MQT_POOL_ID)) ;
    MESSAGEMULTI_2Print ("App [%2d]: POOL_close MQT_POOL_ID status [0x%x]\n",
                         SampleAppNum,
                         tmpStatus) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTI_2Print ("App [%2d]: POOL_close () failed."
                             " MQT_POOL_ID status [0x%x]\n",
                             SampleAppNum,
                             status) ;
    }

    /*
     *  Detach from the processor
     */
    tmpStatus = PROC_detach  (processorId) ;
    MESSAGEMULTI_2Print ("App [%2d]: PROC_detach status [0x%x]\n",
                         SampleAppNum,
                         tmpStatus) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTI_2Print ("App [%2d]: PROC_detach () failed.status [0x%x]\n",
                             SampleAppNum,
                             status) ;
    }

    /*
     *  Destroy the PROC object.
     */
    tmpStatus = PROC_destroy () ;
    MESSAGEMULTI_2Print ("App [%2d]: PROC_destroy status [0x%x]\n",
                         SampleAppNum,
                         tmpStatus) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTI_2Print ("App [%2d]: PROC_destroy () failed."
                             " status [0x%x]\n",
                             SampleAppNum,
                             status) ;
    }

    /*
     *  OS Finalization
     */
    tmpStatus = MESSAGEMULTI_OS_exit () ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MESSAGEMULTI_1Print ("MESSAGEMULTI_OS_exit () failed. Status"
                             "= [0x%x]\n", status) ;
    }

    MESSAGEMULTI_1Print ("App [%2d]: Leaving MESSAGEMULTI_Delete ()\n",
                         SampleAppNum) ;
}


/** ============================================================================
 *  @func   MESSAGEMULTI_Main
 *
 *  @desc   Entry point for the application
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGEMULTI_Main (IN Char8 * dspExecutable,
                   IN Char8 * strNumIterations,
                   IN Char8 * strAppNum,
                   IN Char8 * strProcessorId)
{
    DSP_STATUS status        = DSP_SOK ;
    Uint32     numIterations = 0       ;
    Uint8      processorId   = 0       ;

    if (    (dspExecutable      != NULL)
        &&  (strNumIterations   != NULL)
        &&  (strAppNum          != NULL)) {
        numIterations = MESSAGEMULTI_Atoi (strNumIterations) ;
        SampleAppNum  = MESSAGEMULTI_Atoi (strAppNum) ;
        processorId   = MESSAGEMULTI_Atoi (strProcessorId) ;

		MESSAGEMULTI_1Print ("======= Sample Application : MESSAGEMULTI App [%d]"
							" =======\n",
							SampleAppNum) ;

        if (    (SampleAppNum  == 0)
            ||  (SampleAppNum  >  MAX_APPS)
            ||  (numIterations >  0xFFFF)) {
            status = DSP_EINVALIDARG ;
            MESSAGEMULTI_1Print ("ERROR! Invalid arguments specified for  "
                                 "message_multi application.\n"
                                 "     MAX_APPS       = %d\n",
                                 MAX_APPS) ;
            MESSAGEMULTI_1Print ("     Max iterations = %d\n",
                                 0xFFFF) ;
        }
        else {
            if (processorId >= MAX_DSPS) {
                 MESSAGEMULTI_1Print ("==Error: Invalid processor id %d "
                                      "specified ==\n", processorId) ;
                 status = DSP_EINVALIDARG ;
            }

            if (DSP_SUCCEEDED (status)) {
                /*
                 *  Specify the dsp executable file name for message creation
                 *  phase.
                 */
                status = MESSAGEMULTI_Create (dspExecutable,
                                              strNumIterations,
                                              strAppNum,
                                              processorId) ;

                /*
                 *  Execute the message execute phase.
                 */
                if (DSP_SUCCEEDED (status)) {
                    status = MESSAGEMULTI_Execute (numIterations,
                                                   processorId) ;
                }

                /*
                 *  Perform cleanup operation.
                 */
                MESSAGEMULTI_Delete (processorId) ;
            }
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        MESSAGEMULTI_0Print ("ERROR! Invalid arguments specified for  "
                        "message_multi application\n") ;
    }

    MESSAGEMULTI_0Print ("================================================\n") ;
}

/** ----------------------------------------------------------------------------
 *  @func   MESSAGE_Notify
 *
 *  @desc   This function implements the event callback registered with the
 *          NOTIFY component to receive notification indicating that the DSP-
 *          side application has opened the pool.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
Void
MESSAGE_Notify (Uint32 eventNo, Pvoid arg, Pvoid info)
{
    DSP_STATUS status = DSP_SOK ;

    if ((Uint32) info == SampleAppNum) {
        /* Post the semaphore. */
        status = MESSAGEMULTI_PostSem ((Pvoid) arg) ;
        if (DSP_FAILED (status)) {
            MESSAGEMULTI_1Print ("MESSAGEMULTI_PostSem () failed."
                                 " Status = [0x%x]\n",
                                 status) ;
        }
    }
}


#if defined (VERIFY_DATA)
/** ----------------------------------------------------------------------------
 *  @func   MESSAGEMULTI_VerifyData
 *
 *  @desc   This function verifies the data-integrity of given buffer.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
MESSAGEMULTI_VerifyData (IN MSGQ_Msg msg, IN Uint16 sequenceNumber)
{
    DSP_STATUS status = DSP_SOK ;
    Uint16     msgId ;

    /*
     *  Verify the message
     */
    msgId = MSGQ_getMsgId(msg);
    if (msgId != sequenceNumber) {
        status = DSP_EFAIL;
        MESSAGEMULTI_0Print ("ERROR! Data integrity check failed\n") ;
    }

    return status ;
}
#endif /* if defined (VERIFY_DATA) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
