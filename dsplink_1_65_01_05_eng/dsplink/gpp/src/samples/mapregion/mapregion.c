/** ============================================================================
 *  @file   mapregion.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/mapregion/
 *
 *  @desc   This sample application demonstrates the usage of PROC_control ()
 *          for directly accessing DSP memory.
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


/*  ----------------------------------- C Libraries                 */
#include <string.h>

/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>

/*  ----------------------------------- DSP/BIOS LINK API               */
#include <proc.h>
#include <msgq.h>
#include <pool.h>

/*  ----------------------------------- APP HEADER                       */
#include <mapregion.h>


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
 *  @const  dsp_addr1
 *
 *  @desc   Address of the first memory region on the DSP.
 *  ============================================================================
 */
Uint32  dsp_addr1 ;

/** ============================================================================
 *  @const  dsp_addr2
 *
 *  @desc   Address of the second memory region on the DSP.
 *  ============================================================================
 */
Uint32  dsp_addr2 ;

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

/** ============================================================================
 *  @name   LINKCFG_config
 *
 *  @desc   Extern declaration to the default DSP/BIOS LINK configuration
 *          structure.
 *  ============================================================================
 */
extern  LINKCFG_Object LINKCFG_config ;

/** ============================================================================
 *  @name   SampleBufSizes
 *
 *  @desc   Message sizes managed by the pool.
 *  ============================================================================
 */
Uint32 SampleBufSizes [NUM_BUFOBJS_IN_POOL] =
{
    APP_MSG_SIZE,
    ZCPYMQT_CTRLMSG_SIZE
} ;

/** ============================================================================
 *  @name   SampleNumBuffers
 *
 *  @desc   Number of messages in each pool managed by the pool.
 *  ============================================================================
 */
Uint32 SampleNumBuffers [NUM_BUFOBJS_IN_POOL] =
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
STATIC SMAPOOL_Attrs SamplePoolAttrs =
{
    NUM_BUFOBJS_IN_POOL,
    SampleBufSizes,
    SampleNumBuffers
} ;


/** ============================================================================
 *  @func   MAPREGION_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @modif  MAPREGION_Buffers
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MAPREGION_Create (IN Char8 * dspExecutable,
                  IN Char8 * strBufferSize,
                  IN Char8 * strNumIterations,
                  IN Uint8   processorId)
{
    DSP_STATUS       status   = DSP_SOK   ;
    Uint32           i        = 0         ;
    Char8 *          args [NUM_ARGS]      ;
    MSGQ_LocateAttrs syncLocateAttrs      ;
    PCPYMQT_Attrs    mqtAttrs             ;

    MAPREGION_0Print ("Entered MAPREGION_Create ()\n") ;

    /* Get the dsp addresses to map */
   /* Find out the REMAINING section entry id in the mem table */
   while (  (strcmp ("REMAINING" ,LINKCFG_config.dspConfigs[processorId]->memTables[0][i].name) != 0)
          && (i < LINKCFG_config.dspConfigs[processorId]->dspObject->memEntries )) {
       i++ ;
   }
    if (i == LINKCFG_config.dspConfigs[processorId]->dspObject->memEntries){
        /* Failed to find out the REMAINING section */
        status = DSP_ECONFIG ;
    }
    if (DSP_SUCCEEDED (status)) {
        /* Assign the base address of REMAINING section to dsp_add1 */
        dsp_addr1 = LINKCFG_config.dspConfigs[processorId]->memTables[0][i].physAddr ;
        MAPREGION_1Print ("Adrress of first mapped DSP memory region 0x%x \n",
                          dsp_addr1) ;

        /* Calculate the address for dsp_addr2
         * Use base address + (section length/2 )
         */
         dsp_addr2 = dsp_addr1 + (LINKCFG_config.dspConfigs[processorId]->memTables[0][i].size/2) ;
         MAPREGION_1Print ("Adrress of second mapped DSP memory region 0x%x \n",
                           dsp_addr2) ;
    }

    if (DSP_SUCCEEDED (status)) {
    /*
     *  Create and initialize the proc object.
     */
    if (DSP_SUCCEEDED (status)) {
        status = PROC_setup (NULL) ;
    }
    }
    /*
     *  Attach the Dsp with which the transfers have to be done.
     */
    if (DSP_SUCCEEDED (status)) {
        status = PROC_attach (processorId, NULL) ;
        if (DSP_FAILED (status)) {
            MAPREGION_1Print ("PROC_attach failed. Status: [0x%x]\n", status) ;
        }
    }
    else {
        MAPREGION_1Print ("PROC_setup failed. Status: [0x%x]\n", status) ;
    }

    /*
     *  Open the pool.
     */
    if (DSP_SUCCEEDED (status)) {
        status = POOL_open (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                            &SamplePoolAttrs) ;
        if (DSP_FAILED (status)) {
            MAPREGION_1Print ("POOL_open () failed. Status: [0x%x]\n", status) ;
        }
    }

    /*
     *  Open the GPP's message queue
     */
    if (DSP_SUCCEEDED (status)) {
        status = MSGQ_open (SampleGppMsgqName, &SampleGppMsgq, NULL) ;
        if (DSP_FAILED (status)) {
            MAPREGION_1Print ("MSGQ_open () failed. Status: [0x%x]\n",
                            status) ;
        }
    }

    /*
     *  Load the executable on the DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        args [0] = strNumIterations ;

        status = PROC_load (processorId, dspExecutable, NUM_ARGS, args) ;
        if (DSP_FAILED (status)) {
            MAPREGION_1Print ("PROC_load failed. Status: [0x%x]\n", status) ;
        }
    }

    /*
     *  Start execution on DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        status = PROC_start (processorId) ;
        if (DSP_FAILED (status)) {
            MAPREGION_1Print ("PROC_start failed. Status: [0x%x]\n", status) ;
        }
    }

    /*
     *  Open the remote transport.
     */
    if (DSP_SUCCEEDED (status)) {
        mqtAttrs.poolId = POOL_makePoolId(processorId, SAMPLE_POOL_ID);
        status = MSGQ_transportOpen (processorId, &mqtAttrs) ;
        if (DSP_FAILED (status)) {
            MAPREGION_1Print ("MSGQ_transportOpen () failed. Status: [0x%x]\n",
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
            status = MSGQ_locate (SampleDspMsgqName,
                                  &SampleDspMsgq,
                                  &syncLocateAttrs) ;
            if ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY)) {
                MAPREGION_Sleep (100000) ;
            }
            else if (DSP_FAILED (status)) {
                MAPREGION_1Print ("MSGQ_locate () failed. Status = [0x%x]\n",
                                status) ;
            }
        }
    }

    MAPREGION_0Print ("Leaving MAPREGION_Create ()\n") ;

    return status ;
}


/** ============================================================================
 *  @func   MAPREGION_Execute
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MAPREGION_Execute (IN Uint32 bufferSize,
                   IN Uint32 numIterations,
                   Uint8 processorId)
{
    DSP_STATUS      status    = DSP_SOK ;
    DSP_STATUS      tmpStatus = DSP_SOK ;
    Uint8 *         temp   = NULL ;
    Uint32          verify = 0 ;
    ProcMemMapInfo  mapInfo ;
    SampleMessage * msg ;
    Uint32          i, j ;

    MAPREGION_0Print ("Entered MAPREGION_Execute ()\n") ;

    for (i = 1 ;
         (i <= numIterations) && DSP_SUCCEEDED (status) ;
         i++, verify++) {
        /*
         *  Prime the DSP memory regions to allow for data integrity check
         */
        mapInfo.dspAddr = dsp_addr2 ;
        mapInfo.size    = bufferSize ;
        status = PROC_control (processorId,
                               PROC_CTRL_CMD_MAP_DSPMEM,
                               &mapInfo) ;
        if (DSP_SUCCEEDED (status)) {
            /* Prime the memory region */
            temp = (Uint8 *) (mapInfo.mappedAddr) ;
            for (j = 0 ; j < (mapInfo.mappedSize / DSP_MAUSIZE) ; j++) {
                *temp++ = 0 ;
            }

            /* Relinquish control of the memory region */
            status = PROC_control (processorId,
                                   PROC_CTRL_CMD_UNMAP_DSPMEM,
                                   &mapInfo) ;
            if (DSP_FAILED (status)) {
                MAPREGION_1Print ("Unmap output region failed."
                                  " Status: [0x%x]\n", status) ;
            }
        }
        else {
            MAPREGION_1Print ("Map failed for initializing input region."
                              " Status: [0x%x]\n", status) ;
        }

        /*  Prime the data buffer for the sample application
         *  - Initialize the buffer to '1's. This value is multiplied
         *    by the DSP with the iteration number
         */
        if (DSP_SUCCEEDED (status)) {
            mapInfo.dspAddr = dsp_addr1 ;
            mapInfo.size    = bufferSize ;
            status = PROC_control (processorId,
                                   PROC_CTRL_CMD_MAP_DSPMEM,
                                   &mapInfo) ;
            if (DSP_SUCCEEDED (status)) {
                temp = (Uint8 *) (mapInfo.mappedAddr) ;
                for (j = 0 ; j < (mapInfo.mappedSize / DSP_MAUSIZE) ; j++) {
                    *temp = 1 ;
                    temp++ ;
                }

                /* Relinquish control of the memory region */
                status = PROC_control (processorId,
                                       PROC_CTRL_CMD_UNMAP_DSPMEM,
                                       &mapInfo) ;
                if (DSP_FAILED (status)) {
                    MAPREGION_1Print ("Unmap output region failed."
                                      " Status: [0x%x]\n", status) ;
                }
            }
            else {
                MAPREGION_1Print ("Map failed for initializing output region."
                                  " Status: [0x%x]\n", status) ;
            }
        }
        else {
            MAPREGION_1Print ("Map input region failed. Status: [0x%x]\n",
                              status) ;
        }

        /*  Inform the DSP side that the data buffer has been written */
        if (DSP_SUCCEEDED (status)) {
            status = MSGQ_alloc (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                                 APP_MSG_SIZE,
                                 (MSGQ_Msg *) &msg) ;
            if (DSP_SUCCEEDED (status)) {
                /* Wrap-around verification scalingfactor to fit within
                 * the 8-bit value.
                 */
                if (verify == 0x100) {
                    verify = 0 ;
                }
                /* Set the message id as the scaling factor */
                msg->gppWriteAddr  = dsp_addr1 ;
                msg->dspWriteAddr  = dsp_addr2 ;
                msg->size          = mapInfo.mappedSize ;
                msg->scalingFactor = verify ;

                /* Send the message */
                status = MSGQ_put (SampleDspMsgq, (MSGQ_Msg) msg) ;
                if (DSP_FAILED (status)) {
                    MAPREGION_1Print ("MSGQ_put failed. Status: [0x%x]\n",
                                      status) ;
                }
            }
            else {
                MAPREGION_1Print ("MSGQ_alloc failed. Status: [0x%x]\n",
                                  status) ;
            }
        }

        /*  Wait for a message from the DSP confirming it has written data  */
        if (DSP_SUCCEEDED (status)) {
            status = MSGQ_get (SampleGppMsgq, WAIT_FOREVER, (MSGQ_Msg *) &msg) ;
            if (DSP_SUCCEEDED (status)) {
                status = MSGQ_free ((MSGQ_Msg) msg) ;
                if (DSP_FAILED (status)) {
                    MAPREGION_1Print ("MSGQ_free failed. Status: [0x%x]\n",
                                      status) ;
                }
            }
            else {
                MAPREGION_1Print ("MSGQ_get failed. Status: [0x%x]\n", status) ;
            }
        }

        /*  Read from the DSP memory region */
        if (DSP_SUCCEEDED (status)) {
            mapInfo.dspAddr = dsp_addr2 ;
            mapInfo.size    = bufferSize ;
            status = PROC_control (processorId,
                                   PROC_CTRL_CMD_MAP_DSPMEM,
                                   &mapInfo) ;
            if (DSP_FAILED (status)) {
                MAPREGION_1Print ("Map input region failed. Status: [0x%x]\n",
                                  status) ;
            }
        }
        else {
            MAPREGION_1Print ("PROC_write Failed. Status: [0x%x]\n", status) ;
        }

        /* Verify the data read back */
        if (DSP_SUCCEEDED (status)) {
            temp = (Uint8 *) (mapInfo.mappedAddr) ;
            for (j = 0 ;
                     (j < mapInfo.mappedSize / DSP_MAUSIZE)
                 &&  DSP_SUCCEEDED (tmpStatus) ;
                 j++) {
                if (*temp++ != verify) {
                    MAPREGION_1Print ("Data mismatch at [0x%x]\n", j) ;
                    tmpStatus = DSP_EFAIL ;
                }
            }
        }

        if (DSP_SUCCEEDED (status) && DSP_SUCCEEDED (tmpStatus)) {
            if ((i % 100) == 0) {
                MAPREGION_1Print ("Verified %5d Iterations of "
                                  "Correct Data Read/ Write\n", i) ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            /* Relinquish control of the memory region */
            status = PROC_control (processorId,
                                   PROC_CTRL_CMD_UNMAP_DSPMEM,
                                   &mapInfo) ;
            if (DSP_FAILED (status)) {
                MAPREGION_1Print ("Unmap output region failed."
                                  " Status: [0x%x]\n", status) ;
            }
            else {
                status = tmpStatus ;
            }
        }
    }

    MAPREGION_0Print ("Leaving MAPREGION_Execute ()\n") ;

    return status ;
}


/** ============================================================================
 *  @func   MAPREGION_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          MAPREGION_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *
 *  @modif  MAPREGION_Buffers
 *  ============================================================================
 */
NORMAL_API
Void
MAPREGION_Delete (Uint8 processorId)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;

    MAPREGION_0Print ("Entered MAPREGION_Delete ()\n") ;

    /*
     *  Release the remote message queue
     */
    status = MSGQ_release (SampleDspMsgq) ;
    if (DSP_FAILED (status)) {
        MAPREGION_1Print ("MSGQ_release () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Close the remote transport
     */
    status = MSGQ_transportClose (processorId) ;
    if (DSP_FAILED (status)) {
        MAPREGION_1Print ("MSGQ_transportClose () failed. Status = [0x%x]\n",
                        status) ;
    }

    /*
     *  Stop execution on DSP.
     */
    tmpStatus = PROC_stop (processorId) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        MAPREGION_1Print ("PROC_stop () failed (output). Status: [0x%x]\n",
                     tmpStatus) ;
    }

    /*
     *  Close the GPP's message queue
     */
    tmpStatus = MSGQ_close (SampleGppMsgq) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MAPREGION_1Print ("MSGQ_close () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Close the pool
     */
    tmpStatus = POOL_close (POOL_makePoolId(processorId, SAMPLE_POOL_ID)) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        MAPREGION_1Print ("POOL_close () failed. Status = [0x%x]\n",
                        tmpStatus) ;
    }

    /*
     *  Detach from the processor
     */
    tmpStatus = PROC_detach  (processorId) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        MAPREGION_1Print ("PROC_detach () failed. Status: [0x%x]\n",
                          tmpStatus) ;
    }

    /*
     *  Destroy the PROC object.
     */
    tmpStatus = PROC_destroy () ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        MAPREGION_1Print ("PROC_destroy () failed. Status: [0x%x]\n",
                          tmpStatus) ;
    }

    MAPREGION_0Print ("Leaving MAPREGION_Delete ()\n") ;
}


/** ============================================================================
 *  @func   MAPREGION_Main
 *
 *  @desc   Entry point for the application
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MAPREGION_Main (IN Char8 * dspExecutable,
                IN Char8 * strBufferSize,
                IN Uint32  bufferSize,
                IN Char8 * strNumIterations,
                IN Uint32  numIterations,
                IN Uint8   processorId)
{
    DSP_STATUS status = DSP_SOK ;

    MAPREGION_0Print (
                  "============= Sample Application : MAPREGION ==========\n") ;

    if (   (dspExecutable != NULL)
        && (strBufferSize != NULL)
        && (strNumIterations != NULL)) {
        if ((numIterations >  0xFFFF) ||  (bufferSize == 0)) {
            status = DSP_EINVALIDARG ;
            MAPREGION_1Print ("ERROR! Invalid arguments specified for  "
                              "mapregion application.\n"
                              "     Max iterations = %d\n",
                              0xFFFF) ;
            MAPREGION_1Print ("     Buffer size    = %d\n",
                              bufferSize) ;
        }
        else {

            if (processorId >= MAX_DSPS) {
                MAPREGION_1Print ("== Error: Invalid processor id %d specified  ==\n",
                             processorId) ;
                status = DSP_EFAIL ;

            }

            if (DSP_SUCCEEDED (status)) {
                /*
                 *  Specify the dsp executable file name and the buffer size for
                 *  loop creation phase.
                 */
                status = MAPREGION_Create (dspExecutable,
                                           strBufferSize,
                                           strNumIterations,
                                           processorId ) ;

                /*
                 *  Execute the data transfer loop.
                 */
                if (DSP_SUCCEEDED (status)) {
                    status = MAPREGION_Execute (bufferSize,
                                                numIterations,
                                                processorId) ;
                    if (DSP_FAILED (status)) {
                        MAPREGION_1Print ("Execute phase failed. Status:"
                                          "[0x%x]\n", status) ;
                    }
                }
                else {
                    MAPREGION_1Print ("Create phase failed. Status: [0x%x]\n",
                                      status) ;
                }

                /*
                 *  Perform cleanup operation.
                 */
                MAPREGION_Delete (processorId) ;
            }
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        MAPREGION_0Print ("ERROR! Invalid arguments specified for while "
                     "executing loop application\n") ;
    }

    MAPREGION_0Print (
                  "=======================================================\n") ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

