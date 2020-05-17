/** ============================================================================
 *  @file   readwrite.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/readwrite/
 *
 *  @desc   This sample application demonstrates the usage of PROC_read ()
 *          and PROC_write () APIs.
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
#if defined (DA8XXGEM)
#include <loaderdefs.h>
#endif


/*  ----------------------------------- APP HEADER                       */
#include <readwrite.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @name   NUM_ARGS
 *
 *  @desc   Number of arguments specified to the DSP application.
 *  ============================================================================
 */
#define NUM_ARGS                1

/** ============================================================================
 *  @name   SAMPLE_POOL_ID
 *
 *  @desc   ID of the POOL used for the sample.
 *  ============================================================================
 */
#define SAMPLE_POOL_ID          0

/** ============================================================================
 *  @name   NUM_BUFOBJS_IN_POOL
 *
 *  @desc   Number of BUF objects in a pool to be configured for the allocator.
 *  ============================================================================
 */
#define NUM_BUFOBJS_IN_POOL     2

/** ============================================================================
 *  @const  NUM_BUFS_IN_BUF0
 *
 *  @desc   Number of buffers in first buffer pool.
 *  ============================================================================
 */
#define NUM_BUFS_IN_BUF0        4

/** ============================================================================
 *  @const  NUM_BUFS_IN_BUF1
 *
 *  @desc   Number of buffers in second buffer pool.
 *  ============================================================================
 */
#define NUM_BUFS_IN_BUF1        4

/** ============================================================================
 *  @const  APP_MSG_SIZE
 *
 *  @desc   Messaging buffer used by the application.
 *          Note: This buffer is aligned according to the alignment expected
 *          by the platform.
 *  ============================================================================
 */
#define APP_MSG_SIZE  DSPLINK_ALIGN (sizeof (SampleMessage), DSPLINK_BUF_ALIGN)

/** ============================================================================
 *  @name   SampleMessage
 *
 *  @desc   Structure used to pass the scaling factor from the GPP to the DSP.
 *
 *  @field  msgHeader
 *              Required first field of a message.
 *  @field  gppWriteAddr
 *              DSP Memory address where GPP writes.
 *  @field  dspWriteAddr
 *              DSP Memory address where GPP writes.
 *  @field  size
 *              Size of data written
 *  @field  scalingFactor
 *              Used to scale the output buffer values.
 *  ============================================================================
 */
typedef struct SampleMessage_tag {
    MSGQ_MsgHeader msgHeader     ;
    Uint32         gppWriteAddr  ;
    Uint32         dspWriteAddr  ;
    Uint32         size          ;
    Uint32         scalingFactor ;
} SampleMessage ;


/** ============================================================================
 *  @const  SampleGppMsgqName
 *
 *  @desc   Name of the first MSGQ on the GPP.
 *  ============================================================================
 */
STATIC Char8 SampleGppMsgqName [DSP_MAX_STRLEN] = "GPPMSGQ1" ;

/** ============================================================================
 *  @const  SampleDspMsgqName
 *
 *  @desc   Name of the first MSGQ on the DSP.
 *  ============================================================================
 */
STATIC Char8 SampleDspMsgqName [DSP_MAX_STRLEN] = "DSPMSGQ1" ;

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

#if defined (DA8XXGEM)
/** ============================================================================
 *  @name   dspAddr
 *
 *  @desc   Address of c_int00 in the DSP executable.
 *  ============================================================================
 */
Uint32 RDWR_dspAddr ;

/** ============================================================================
 *  @name   shmAddr
 *
 *  @desc   Address of symbol DSPLINK_shmBaseAddres in the DSP executable.
 *  ============================================================================
 */
Uint32 RDWR_shmAddr ;

/** ============================================================================
 *  @name   argsAddr
 *
 *  @desc   Address of .args section in the DSP executable.
 *  ============================================================================
 */
Uint32 RDWR_argsAddr ;

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
 *  @macro  CONVERT_ENDIANISM
 *
 *  @desc   Converts the endianism of the 16-bit word if required.
 *  ============================================================================
 */
#if defined (DIFF_ENDIANISM)
#define CONVERT_ENDIANISM(x)  BYTESWAP_WORD(x)
#else /* if defined (CONVERT_ENDIANISM) */
#define CONVERT_ENDIANISM(x)  x
#endif /* if defined (CONVERT_ENDIANISM) */


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
STATIC Uint32 SampleBufSizes [NUM_BUFOBJS_IN_POOL] =
{
    APP_MSG_SIZE,
    SAMPLEMQT_CTRLMSG_SIZE
} ;

/** ============================================================================
 *  @name   SampleNumBuffers
 *
 *  @desc   Number of messages in each pool managed by the pool.
 *  ============================================================================
 */
STATIC Uint32 SampleNumBuffers [NUM_BUFOBJS_IN_POOL] =
{
    NUM_BUFS_IN_BUF0,
    NUM_BUFS_IN_BUF1
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
    NUM_BUFOBJS_IN_POOL,
    SampleBufSizes,
    SampleNumBuffers,
    TRUE
} ;
#endif /* if defined ZCPY_LINK */


/** ============================================================================
 *  @func   RDWR_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @modif  RDWR_Buffers
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
RDWR_Create (IN Char8 * dspExecutable,
             IN Char8 * strBufferSize,
             IN Char8 * strNumIterations,
             IN Uint8   processorId)
{
    DSP_STATUS       status   = DSP_SOK   ;
    Char8 *          args [NUM_ARGS]      ;
    MSGQ_LocateAttrs syncLocateAttrs      ;
#if defined (DA8XXGEM)
    NOLOADER_ImageInfo  imageInfo ;
#endif

    RDWR_0Print ("Entered RDWR_Create ()\n") ;


    /*
     *  OS initialization
     */
    status = RDWR_OS_init () ;

    /*
     *  Create and initialize the proc object.
     */

    if (DSP_SUCCEEDED (status)) {
        status = PROC_setup (NULL) ;
    }

    /*
     *  Attach the Dsp with which the transfers have to be done.
     */
    if (DSP_SUCCEEDED (status)) {
        status = PROC_attach (processorId, NULL) ;
        if (DSP_FAILED (status)) {
            RDWR_1Print ("PROC_attach failed. Status: [0x%x]\n", status) ;
        }
    }
    else {
        RDWR_1Print ("PROC_setup failed. Status: [0x%x]\n", status) ;
    }

    /*
     *  Open the pool.
     */
    if (DSP_SUCCEEDED (status)) {
        status = POOL_open (
                            POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                            &SamplePoolAttrs) ;
        if (DSP_FAILED (status)) {
            RDWR_1Print ("POOL_open () failed. Status: [0x%x]\n", status) ;
        }
    }

    /*
     *  Open the GPP's message queue
     */
    if (DSP_SUCCEEDED (status)) {
        status = MSGQ_open (SampleGppMsgqName, &SampleGppMsgq, NULL) ;
        if (DSP_FAILED (status)) {
            RDWR_1Print ("MSGQ_open () failed. Status: [0x%x]\n",
                            status) ;
        }
    }

    /*
     *  Load the executable on the DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        args [0] = strNumIterations ;

#if defined (DA8XXGEM)
        if (LINKCFG_config.dspConfigs [processorId]->dspObject->doDspCtrl
                      ==  DSP_BootMode_NoBoot) {
            imageInfo.dspRunAddr  = RDWR_dspAddr ;
            imageInfo.shmBaseAddr = RDWR_shmAddr ;
            imageInfo.argsAddr    = RDWR_argsAddr ;
            imageInfo.argsSize    = 50 ;
            status = PROC_load (processorId,
                                (Char8 *) &imageInfo,
                                NUM_ARGS,
                                args) ;
        }
        else
#endif
        {
            status = PROC_load (processorId, dspExecutable, NUM_ARGS, args) ;
        }

        if (DSP_FAILED (status)) {
            RDWR_1Print ("PROC_load failed. Status: [0x%x]\n", status) ;
        }
    }

    /*
     *  Start execution on DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        status = PROC_start (processorId) ;
        if (DSP_FAILED (status)) {
            RDWR_1Print ("PROC_start failed. Status: [0x%x]\n", status) ;
        }
    }

    /*
     *  Open the remote transport.
     */
    if (DSP_SUCCEEDED (status)) {
        mqtAttrs.poolId = POOL_makePoolId(processorId, SAMPLE_POOL_ID) ;
        status = MSGQ_transportOpen (processorId, &mqtAttrs) ;
        if (DSP_FAILED (status)) {
            RDWR_1Print ("MSGQ_transportOpen () failed. Status: [0x%x]\n",
                            status) ;
        }
    }

    /*
     *  Locate the DSP's message queue
     */
    if (DSP_SUCCEEDED (status)) {
        syncLocateAttrs.timeout = WAIT_FOREVER ;
        status = DSP_ENOTFOUND ;
        while ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
            status = MSGQ_locate (SampleDspMsgqName,
                                  &SampleDspMsgq,
                                  &syncLocateAttrs) ;
            if ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
                RDWR_Sleep (100000) ;
            }
            else if (DSP_FAILED (status)) {
                RDWR_1Print ("MSGQ_locate () failed. Status = [0x%x]\n",
                                status) ;
            }
        }
    }

    RDWR_0Print ("Leaving RDWR_Create ()\n") ;

    return status ;
}


/** ============================================================================
 *  @func   RDWR_Execute
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
RDWR_Execute (IN Uint32  dspAddress,
              IN Uint32  bufferSize,
              IN Uint32  numIterations,
              IN Uint8   processorId)
{
    DSP_STATUS      status   = DSP_SOK ;
    Uint32 *        bufIn    = NULL ;
    Uint32 *        bufOut   = NULL ;
    Uint8 *         ptr8     = NULL ;
    Uint8 *         ptr8_1   = NULL ;
    Uint16 *        ptr16    = NULL ;
    Uint16 *        ptr16_1  = NULL ;
    Uint32          dspAddr1 = dspAddress ;
    Uint32          dspAddr2 = (dspAddress + bufferSize) ;
    SampleMessage * msg ;
    Uint32          i, j ;

    RDWR_0Print ("Entered RDWR_Execute ()\n") ;

    status = RDWR_AllocateBuffer (bufferSize, (Pvoid *) &bufIn) ;
    if (DSP_SUCCEEDED (status)) {
        status = RDWR_AllocateBuffer (bufferSize, (Pvoid *) &bufOut) ;
        if (DSP_FAILED (status)) {
            RDWR_1Print ("Buffer Allocation Failed. Status: [0x%x]\n",
                              status) ;
        }
    }
    else {
       RDWR_1Print ("Buffer Allocation Failed. Status: [0x%x]\n", status) ;
    }

    for (i = 1 ;
        (  ((numIterations == 0) || (i <= numIterations))
         && DSP_SUCCEEDED (status)) ;
         i++) {
        /*
         *  Prime the DSP memory regions to allow for data integrity check
         */
        ptr8  = (Uint8 *) (bufIn) ;
        ptr16 = (Uint16 *) (bufIn) ;
        for (j = 0 ;
             DSP_SUCCEEDED (status) && (j < bufferSize / DSP_MAUSIZE) ;
             j++) {
            if (DSP_MAUSIZE == 1) {
                *ptr8 = 0 ;
                ptr8++ ;
            }
            else if (DSP_MAUSIZE == 2) {
                *ptr16 = 0 ;
                ptr16++ ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            status = PROC_write (processorId, dspAddr2, bufferSize, bufIn) ;
            if (DSP_FAILED (status)) {
                RDWR_1Print ("PROC_write Failed. Status: [0x%x]\n",
                                  status) ;
            }
        }

        /*  Prime the data buffer for the sample application
         *  - Initialize the buffer to '1's. This value is multiplied
         *    by the DSP with the iteration number
         */
        ptr8  = (Uint8 *)  (bufOut) ;
        ptr16 = (Uint16 *) (bufOut) ;
        for (j = 0 ;
             DSP_SUCCEEDED (status) && (j < (bufferSize / DSP_MAUSIZE)) ;
             j++) {
            if (DSP_MAUSIZE == 1) {
                *ptr8  = 0x1 ;
                ptr8++ ;
            }
            else if (DSP_MAUSIZE == 2) {
                *ptr16 = CONVERT_ENDIANISM (0x1) ;
                ptr16++ ;
            }
        }

        /*  Write the data buffer to the DSP side */
        if (DSP_SUCCEEDED (status)) {
            status = PROC_write (processorId, dspAddr1, bufferSize, bufOut) ;
            if (DSP_FAILED (status)) {
                RDWR_1Print ("PROC_write Failed. Status: [0x%x]\n",
                                  status) ;
            }
        }

        /*  Inform the DSP side that the data buffer has been written */
        if (DSP_SUCCEEDED (status)) {
            status = MSGQ_alloc (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                                 APP_MSG_SIZE,
                                 (MSGQ_Msg *) &msg) ;
            if (DSP_SUCCEEDED (status)) {
                /* Set the message id as the scaling factor */
#if (defined (OMAP) && defined (PCPY_LINK)) || defined (WORD_SWAP)
                msg->gppWriteAddr  = WORDSWAP_LONG ((dspAddr1  / DSP_MAUSIZE)) ;
                msg->dspWriteAddr  = WORDSWAP_LONG ((dspAddr2  / DSP_MAUSIZE)) ;
                msg->size          = WORDSWAP_LONG ((bufferSize/ DSP_MAUSIZE)) ;
                msg->scalingFactor = WORDSWAP_LONG (i) ;
#else /* if (defined (OMAP) && defined (PCPY_LINK)) || defined (WORD_SWAP) */
                msg->gppWriteAddr  = (dspAddr1   / DSP_MAUSIZE) ;
                msg->dspWriteAddr  = (dspAddr2   / DSP_MAUSIZE) ;
                msg->size          = (bufferSize / DSP_MAUSIZE) ;
                msg->scalingFactor = i ;
#endif /* if (defined (OMAP) && defined (PCPY_LINK)) || defined (WORD_SWAP) */

                /* Send the message */
                status = MSGQ_put (SampleDspMsgq, (MSGQ_Msg) msg) ;
                if (DSP_FAILED (status)) {
                    RDWR_1Print ("MSGQ_put failed. Status: [0x%x]\n",
                                      status) ;
                }
            }
            else {
                RDWR_1Print ("MSGQ_alloc failed. Status: [0x%x]\n",
                                  status) ;
            }
        }

        /*  Wait for a message from the DSP confirming it has written data  */
        if (DSP_SUCCEEDED (status)) {
            status = MSGQ_get (SampleGppMsgq, WAIT_FOREVER, (MSGQ_Msg *) &msg) ;
            if (DSP_SUCCEEDED (status)) {
                status = MSGQ_free ((MSGQ_Msg) msg) ;
                if (DSP_FAILED (status)) {
                    RDWR_1Print ("MSGQ_free failed. Status: [0x%x]\n",
                                      status) ;
                }
            }
            else {
                RDWR_1Print ("MSGQ_get failed. Status: [0x%x]\n", status) ;
            }
        }

        /*  Read from the DSP memory region */
        if (DSP_SUCCEEDED (status)) {
            status = PROC_read (processorId, dspAddr2, bufferSize, bufIn) ;
            if (DSP_FAILED (status)) {
                RDWR_1Print ("PROC_read Failed. Status: [0x%x]\n",
                                  status) ;
            }
        }
        else {
            RDWR_1Print ("PROC_write Failed. Status: [0x%x]\n", status) ;
        }

        /* Verify the data read back */
        if (DSP_SUCCEEDED (status)) {
            ptr8    = (Uint8 *)  bufIn ;
            ptr8_1  = (Uint8 *)  bufOut ;
            ptr16   = (Uint16 *) bufIn ;
            ptr16_1 = (Uint16 *) bufOut ;
            for (j = 0 ;
                 (j < (bufferSize / DSP_MAUSIZE)) && DSP_SUCCEEDED (status);
                 j++) {
                if (DSP_MAUSIZE == 1) {
                    if (ptr8 [j] != (Uint8) (ptr8_1 [j] * i)) {
                        RDWR_1Print ("Data mismatch at [0x%x]\n", j) ;
                        RDWR_1Print ("  Expected [0x%x]\n", (ptr8_1 [j] * i)) ;
                        RDWR_1Print ("  Received [0x%x]\n", ptr8 [j]) ;
                        status = DSP_EFAIL ;
                    }
                }
                else if (DSP_MAUSIZE == 2) {
                    if (   CONVERT_ENDIANISM (ptr16 [j])
                        != (Uint16) (CONVERT_ENDIANISM (ptr16_1 [j]) * i)) {
                        RDWR_1Print ("Data mismatch at [0x%x]\n", j) ;
                        RDWR_1Print ("  Expected [0x%x]\n",
                                     (CONVERT_ENDIANISM (ptr16_1 [j]) * i)) ;
                        RDWR_1Print ("  Received [0x%x]\n",
                                     CONVERT_ENDIANISM (ptr16 [j])) ;
                        status = DSP_EFAIL ;
                    }
                }
            }

            if ((i % 100) == 0) {
                RDWR_1Print ("Verified %5d Iterations of "
                             "Correct Data Read/ Write\n", i) ;
            }
        }
    }

    if (bufIn != NULL) {
        RDWR_FreeBuffer ((Pvoid *) &bufIn) ;
    }

    if (bufOut != NULL) {
        RDWR_FreeBuffer ((Pvoid *) &bufOut) ;
    }

    RDWR_0Print ("Leaving RDWR_Execute ()\n") ;

    return status ;
}


/** ============================================================================
 *  @func   RDWR_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          RDWR_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *
 *  @modif  RDWR_Buffers
 *  ============================================================================
 */
NORMAL_API
Void
RDWR_Delete (IN Uint8 processorId)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;

    RDWR_0Print ("Entered RDWR_Delete ()\n") ;

    /*
     *  Release the remote message queue
     */
    status = MSGQ_release (SampleDspMsgq) ;
    if (DSP_FAILED (status)) {
        RDWR_1Print ("MSGQ_release () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Close the remote transport
     */
    status = MSGQ_transportClose (processorId) ;
    if (DSP_FAILED (status)) {
        RDWR_1Print ("MSGQ_transportClose () failed. Status = [0x%x]\n",
                        status) ;
    }

    /*
     *  Stop execution on DSP.
     */
    tmpStatus = PROC_stop (processorId) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        RDWR_1Print ("PROC_stop () failed (output). Status: [0x%x]\n",
                     tmpStatus) ;
    }

    /*
     *  Close the GPP's message queue
     */
    tmpStatus = MSGQ_close (SampleGppMsgq) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        RDWR_1Print ("MSGQ_close () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Close the pool
     */
    tmpStatus = POOL_close (POOL_makePoolId(processorId, SAMPLE_POOL_ID)) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        RDWR_1Print ("POOL_close () failed. Status = [0x%x]\n",
                        tmpStatus) ;
    }

    /*
     *  Detach from the processor
     */
    tmpStatus = PROC_detach  (processorId) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        RDWR_1Print ("PROC_detach () failed. Status: [0x%x]\n", tmpStatus) ;
    }

    /*
     *  Destroy the PROC object.
     */
    tmpStatus = PROC_destroy () ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        RDWR_1Print ("PROC_destroy () failed. Status: [0x%x]\n", tmpStatus) ;
    }

    /*
     *  OS Finalization
     */
    tmpStatus = RDWR_OS_exit () ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        RDWR_1Print ("RDWR_OS_exit () failed. Status = [0x%x]\n", status) ;
    }

    RDWR_0Print ("Leaving RDWR_Delete ()\n") ;
}

/** ============================================================================
 *  @func   RDWR_Main
 *
 *  @desc   Entry point for the application
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
RDWR_Main (IN Char8 * dspExecutable,
           IN Char8 * strDspAddress,
           IN Uint32  dspAddress,
           IN Char8 * strBufferSize,
           IN Uint32  bufferSize,
           IN Char8 * strNumIterations,
           IN Uint32  numIterations,
           IN Uint8   processorId)
{
    DSP_STATUS status = DSP_SOK ;

    RDWR_0Print ("============= Sample Application : READWRITE ==========\n") ;

    if (   (dspExecutable != NULL)
        && (strBufferSize != NULL)
        && (strNumIterations != NULL)) {
        if (   (numIterations >  0xFFFF)
            || (bufferSize == 0)
            || (processorId >= MAX_DSPS)) {
            status = DSP_EINVALIDARG ;
            RDWR_1Print ("ERROR! Invalid arguments specified for  "
                         "readwrite application.\n"
                         "     Max iterations = %d\n",
                         0xFFFF) ;
            RDWR_1Print ("     Buffer size    = %d\n",
                         bufferSize) ;
            RDWR_1Print ("     DSP processorId    = %d\n",
                         processorId) ;
        }
        else {
            /*
             *  Specify the dsp executable file name and the buffer size for
             *  loop creation phase.
             */
            status = RDWR_Create (dspExecutable,
                                  strBufferSize,
                                  strNumIterations,
                                  processorId) ;

            /*
             *  Execute the data transfer loop.
             */
            if (DSP_SUCCEEDED (status)) {
                status = RDWR_Execute (dspAddress,
                                       bufferSize,
                                       numIterations,
                                       processorId) ;
                if (DSP_FAILED (status)) {
                    RDWR_1Print ("Execute phase failed. Status: [0x%x]\n",
                                 status) ;
                }
            }
            else {
                RDWR_1Print ("Create phase failed. Status: [0x%x]\n", status) ;
            }

            /*
             *  Perform cleanup operation.
             */
            RDWR_Delete (processorId) ;
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        RDWR_0Print ("ERROR! Invalid arguments specified for while executing "
                     "readwrite application\n") ;
    }

    RDWR_0Print ("=======================================================\n") ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

