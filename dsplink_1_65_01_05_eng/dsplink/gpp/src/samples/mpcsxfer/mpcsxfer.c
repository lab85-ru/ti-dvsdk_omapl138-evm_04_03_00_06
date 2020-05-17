/** ============================================================================
 *  @file   mpcsxfer.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/mpcsxfer/
 *
 *  @desc   This is an application which transfers data between the GPP and DSP
 *          through a basic mechanism of shared buffers with mutually exclusive
 *          access protection. The shared buffers are allocated using POOL
 *          component. Mutually exclusive access to the shared buffers is
 *          provided through MPCS component. Notification of control and data
 *          buffer pointers is sent to the DSP using the NOTIFY component.
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
#include <pool.h>
#include <mpcs.h>
#include <notify.h>
#if defined (DA8XXGEM)
#include <loaderdefs.h>
#endif


/*  ----------------------------------- Application Header              */
#include <mpcsxfer.h>
#include <mpcsxfer_os.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @const   NUM_ARGS
 *
 *  @desc   Number of arguments specified to the DSP application.
 *  ============================================================================
 */
#define NUM_ARGS                       2

/** ============================================================================
 *  @name   SAMPLE_POOL_ID
 *
 *  @desc   ID of the POOL used for the sample.
 *  ============================================================================
 */
#define SAMPLE_POOL_ID                 0

/** ============================================================================
 *  @name   NUM_BUF_SIZES
 *
 *  @desc   Number of buffer pools to be configured for the allocator.
 *  ============================================================================
 */
#define NUM_BUF_SIZES                  3

/** ============================================================================
 *  @const  NUM_BUF_POOL0
 *
 *  @desc   Number of buffers in first buffer pool.
 *  ============================================================================
 */
#define NUM_BUF_POOL0                  1

/** ============================================================================
 *  @const  NUM_BUF_POOL1
 *
 *  @desc   Number of buffers in second buffer pool.
 *  ============================================================================
 */
#define NUM_BUF_POOL1                  1

/** ============================================================================
 *  @const  NUM_BUF_POOL2
 *
 *  @desc   Number of buffers in third buffer pool.
 *  ============================================================================
 */
#define NUM_BUF_POOL2                  1

/*  ============================================================================
 *  @const   MPCSXFER_INVALID_ID
 *
 *  @desc   Indicates invalid processor ID within the MPCSXFER_Ctrl structure.
 *  ============================================================================
 */
#define MPCSXFER_INVALID_ID            (Uint32) -1

/** ============================================================================
 *  @const  MPCSXFER_IPS_ID
 *
 *  @desc   The IPS ID to be used for sending notification events to the DSP.
 *  ============================================================================
 */
#define MPCSXFER_IPS_ID                0

/** ============================================================================
 *  @const  MPCSXFER_IPS_EVENTNO
 *
 *  @desc   The IPS event number to be used for sending notification events to
 *          the DSP.
 *  ============================================================================
 */
#define MPCSXFER_IPS_EVENTNO           5

/** ============================================================================
 *  @const  MPCSXFER_GPP_STAMP
 *
 *  @desc   Value indicating GPP stamp on the contents of the data buffer.
 *  ============================================================================
 */
#define MPCSXFER_GPP_STAMP             0xABAB

/** ============================================================================
 *  @const  MPCSXFER_DSP_SUCCESS_STAMP
 *
 *  @desc   Value indicating DSP stamp on the contents of the data buffer for
 *          successful operation.
 *  ============================================================================
 */
#define MPCSXFER_DSP_STAMP             0xCDCD

#if defined (DA8XXGEM)
/** ============================================================================
 *  @name   dspAddr
 *
 *  @desc   Address of c_int00 in the DSP executable.
 *  ============================================================================
 */
 Uint32 MPCSXFER_dspAddr ;

/** ============================================================================
 *  @name   shmAddr
 *
 *  @desc   Address of symbol DSPLINK_shmBaseAddres in the DSP executable.
 *  ============================================================================
 */
Uint32 MPCSXFER_shmAddr ;

/** ============================================================================
 *  @name   argsAddr
 *
 *  @desc   Address of .args section in the DSP executable.
 *  ============================================================================
 */
Uint32 MPCSXFER_argsAddr ;
#endif


/** ============================================================================
 *  @name   MPCSXFER_Ctrl
 *
 *  @desc   Control structure for the MPCS sample application.
 *
 *  @field  procId
 *              ID of the processor currently owning the buffer.
 *  @field  iterNo
 *              Current iteration number corresponding to the contents of the
 *              buffer.
 *  @field  dataVerify
 *              Indicates whether the data verification was successful.
 *  ============================================================================
 */
typedef struct MPCSXFER_Ctrl_tag {
    Uint32   procId ;
    Uint32   iterNo ;
    Uint32   dataVerify ;
} MPCSXFER_Ctrl ;


/*  ============================================================================
 *  @name   MPCSXFER_BufferSize
 *
 *  @desc   Size of buffer to be used for data transfer.
 *  ============================================================================
 */
STATIC Uint32  MPCSXFER_BufferSize ;

/*  ============================================================================
 *  @name   MPCSXFER_NumIterations
 *
 *  @desc   Number of iterations of data transfer.
 *  ============================================================================
 */
STATIC Uint32  MPCSXFER_NumIterations ;

/** ============================================================================
 *  @const  MPCSXFER_MpcsObjName
 *
 *  @desc   Name of the MPCS created on the GPP to provide mutually exclusive
 *          access to the control and data buffers.
 *  ============================================================================
 */
STATIC Char8 MPCSXFER_MpcsObjName [DSP_MAX_STRLEN] = "MPCSXFER_MPCSOBJ" ;

/** ============================================================================
 *  @name   MPCSXFER_MpcsHandle
 *
 *  @desc   Handle to the MPCS object created on the GPP to provide mutually
 *          exclusive access to the control and data buffers.
 *  ============================================================================
 */
STATIC MPCS_Handle MPCSXFER_MpcsHandle = NULL ;

/** ============================================================================
 *  @name   MPCSXFER_CtrlBuf
 *
 *  @desc   Pointer to the shared control structure used by the MPCSXFER sample
 *          application.
 *  ============================================================================
 */
STATIC MPCSXFER_Ctrl * MPCSXFER_CtrlBuf = NULL ;

/** ============================================================================
 *  @name   MPCSXFER_DataBuf
 *
 *  @desc   Pointer to the shared data buffer used by the MPCSXFER sample
 *          application.
 *  ============================================================================
 */
STATIC Uint16 * MPCSXFER_DataBuf = NULL ;

/** ============================================================================
 *  @name   MPCSXFER_SemPtr
 *
 *  @desc   Pointer to the semaphore used for synchronizing with the DSP-side
 *          application.
 *  ============================================================================
 */
STATIC Void * MPCSXFER_SemPtr = NULL ;

#if defined (DA8XXGEM)
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
 *  @func   MPCSXFER_Notify
 *
 *  @desc   This function implements the event callback registered with the
 *          NOTIFY component to receive notification indicating that the DSP-
 *          side application has completed its setup phase.
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
 *  ============================================================================
 */
STATIC
Void
MPCSXFER_Notify (Uint32 eventNo, Pvoid arg, Pvoid info) ;


/** ----------------------------------------------------------------------------
 *  @func   MPCSXFER_VerifyData
 *
 *  @desc   This function verifies the data-integrity of given data buffer.
 *
 *  @arg    ctrlBuf
 *              Pointer to the control buffer whose contents are to be verified.
 *  @arg    dataBuf
 *              Pointer to the data whose contents are to be verified.
 *  @arg    iterNo
 *              Expected iteration number for DSP.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Data verification failed.
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
MPCSXFER_VerifyData (IN MPCSXFER_Ctrl * ctrlBuf,
                     IN Uint16 *        dataBuf,
                     IN Uint32          iterNo) ;


/** ============================================================================
 *  @func   MPCSXFER_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MPCSXFER_Create (IN Char8 * dspExecutable,
                 IN Char8 * strBufferSize,
                 IN Char8 * strNumIterations,
                 IN Uint8   processorId)
{
    DSP_STATUS      status     = DSP_SOK  ;
    Uint32          numArgs    = NUM_ARGS ;
    Void *          dspCtrlBuf = NULL ;
    Void *          dspDataBuf = NULL ;
    Uint32          numBufs [NUM_BUF_SIZES] = {NUM_BUF_POOL0,
                                               NUM_BUF_POOL1,
                                               NUM_BUF_POOL2} ;
    Uint32          size    [NUM_BUF_SIZES] ;
    SMAPOOL_Attrs   poolAttrs ;
    MPCS_Attrs      mpcsAttrs ;
    Char8 *         args [NUM_ARGS] ;
#if defined (DA8XXGEM)
    NOLOADER_ImageInfo  imageInfo ;
#endif


    MPCSXFER_0Print ("Entered MPCSXFER_Create ()\n") ;

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
            MPCSXFER_1Print ("PROC_attach () failed. Status = [0x%x]\n",
                            status) ;
        }
    }
    else {
        MPCSXFER_1Print ("PROC_setup () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Open the pool.
     */
    if (DSP_SUCCEEDED (status)) {
        size [0] = DSPLINK_ALIGN (sizeof (MPCSXFER_Ctrl), DSPLINK_BUF_ALIGN) ;
        size [1] = MPCSXFER_BufferSize ;
        size [2] = sizeof (MPCS_ShObj) ;
        poolAttrs.bufSizes      = (Uint32 *) &size ;
        poolAttrs.numBuffers    = (Uint32 *) &numBufs ;
        poolAttrs.numBufPools   = NUM_BUF_SIZES ;
        poolAttrs.exactMatchReq = TRUE ;
        status = POOL_open (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                            &poolAttrs) ;
        if (DSP_FAILED (status)) {
            MPCSXFER_1Print ("POOL_open () failed. Status = [0x%x]\n",
                             status) ;
        }
    }

    /*
     *  Allocate the control structure to be used for the application.
     */
    if (DSP_SUCCEEDED (status)) {
        status = POOL_alloc (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                             (Void **) &MPCSXFER_CtrlBuf,
                             DSPLINK_ALIGN (sizeof (MPCSXFER_Ctrl),
                                            DSPLINK_BUF_ALIGN)) ;
        /* Get the translated DSP address to be sent to the DSP. */
        if (DSP_SUCCEEDED (status)) {
            status = POOL_translateAddr (
                                  POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                                         &dspCtrlBuf,
                                         AddrType_Dsp,
                                         (Void *) MPCSXFER_CtrlBuf,
                                         AddrType_Usr) ;
            if (DSP_FAILED (status)) {
                MPCSXFER_1Print ("POOL_translateAddr () CtrlBuf failed."
                                 " Status = [0x%x]\n",
                                 status) ;
            }
        }
        else {
            MPCSXFER_1Print ("POOL_alloc () CtrlBuf failed. Status = [0x%x]\n",
                             status) ;
        }
    }

    /*
     *  Allocate the data buffer to be used for the application.
     */
    if (DSP_SUCCEEDED (status)) {
        status = POOL_alloc (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                             (Void **) &MPCSXFER_DataBuf,
                             MPCSXFER_BufferSize) ;
        /* Get the translated DSP address to be sent to the DSP. */
        if (DSP_SUCCEEDED (status)) {
            status = POOL_translateAddr (
                                   POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                                         &dspDataBuf,
                                         AddrType_Dsp,
                                         (Void *) MPCSXFER_DataBuf,
                                         AddrType_Usr) ;
            if (DSP_FAILED (status)) {
                MPCSXFER_1Print ("POOL_translateAddr () DataBuf failed."
                                 " Status = [0x%x]\n",
                                 status) ;
            }
        }
        else {
            MPCSXFER_1Print ("POOL_alloc () DataBuf failed. Status = [0x%x]\n",
                             status) ;
        }
    }

    /*
     *  Create the MPCS object used for protecting the control and data buffers.
     */
    if (DSP_SUCCEEDED (status)) {
        mpcsAttrs.poolId = POOL_makePoolId(processorId, SAMPLE_POOL_ID) ;
        status = MPCS_create (processorId,
                              MPCSXFER_MpcsObjName,
                              NULL,
                              &mpcsAttrs) ;
        if (DSP_FAILED (status)) {
            MPCSXFER_1Print ("MPCS_create () failed. Status = [0x%x]\n",
                            status) ;
        }
    }

    /*
     *  Open the GPP's MPCS object.
     */
    if (DSP_SUCCEEDED (status)) {
        status = MPCS_open (processorId,
                            MPCSXFER_MpcsObjName,
                            &MPCSXFER_MpcsHandle) ;
        if (DSP_FAILED (status)) {
            MPCSXFER_1Print ("MPCS_open () failed. Status = [0x%x]\n", status) ;
        }
    }

    /*
     *  Initialize the shared control structure
     */
    if (DSP_SUCCEEDED (status)) {
        MPCSXFER_CtrlBuf->procId     = MPCSXFER_INVALID_ID ;
        MPCSXFER_CtrlBuf->iterNo     = 0 ;
        MPCSXFER_CtrlBuf->dataVerify = (Uint32) TRUE ;
        POOL_writeback (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                        MPCSXFER_CtrlBuf,
                        DSPLINK_ALIGN (sizeof (MPCSXFER_Ctrl),
                                       DSPLINK_BUF_ALIGN)) ;
    }

    if (DSP_SUCCEEDED (status)) {
        /* Create the semaphore to be used for notification */
        status = MPCSXFER_CreateSem (&MPCSXFER_SemPtr) ;
        if (DSP_FAILED (status)) {
            MPCSXFER_1Print ("MPCSXFER_CreateSem () failed Status = [0x%x]\n",
                             status) ;
        }
    }

    /*
     *  Register for notification that the DSP-side application setup is
     *  complete.
     */
    if (DSP_SUCCEEDED (status)) {
        status = NOTIFY_register (processorId,
                                  MPCSXFER_IPS_ID,
                                  MPCSXFER_IPS_EVENTNO,
                                  (FnNotifyCbck) MPCSXFER_Notify,
                                  MPCSXFER_SemPtr) ;
        if (DSP_FAILED (status)) {
            MPCSXFER_1Print ("NOTIFY_register () failed Status = [0x%x]\n",
                             status) ;
        }
    }

    /*
     *  Load the executable on the DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        args [0] = strBufferSize ;
        args [1] = strNumIterations ;
#if defined (DA8XXGEM)
        if (LINKCFG_config.dspConfigs [processorId]->dspObject->doDspCtrl
                      ==  DSP_BootMode_NoBoot) {
            imageInfo.dspRunAddr  = MPCSXFER_dspAddr ;
            imageInfo.shmBaseAddr = MPCSXFER_shmAddr ;
            imageInfo.argsAddr    = MPCSXFER_argsAddr ;
            imageInfo.argsSize    = 0x50         ;

            status = PROC_load (processorId, (Char8 *) &imageInfo, numArgs, args) ;
        }
        else
#endif
        {
            status = PROC_load (processorId, dspExecutable, numArgs, args) ;
        }

        if (DSP_FAILED (status)) {
            MPCSXFER_1Print ("PROC_load () failed. Status = [0x%x]\n", status) ;
        }
    }

    /*
     *  Start execution on DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        status = PROC_start (processorId) ;
        if (DSP_FAILED (status)) {
            MPCSXFER_1Print ("PROC_start () failed. Status = [0x%x]\n",
                             status) ;
        }
    }

    /*
     *  Wait for the DSP-side application to indicate that it has completed its
     *  setup. The DSP-side application sends notification of the IPS event
     *  when it is ready to proceed with further execution of the application.
     */
    if (DSP_SUCCEEDED (status)) {
        status = MPCSXFER_WaitSem (MPCSXFER_SemPtr) ;
        if (DSP_FAILED (status)) {
            MPCSXFER_1Print ("MPCSXFER_WaitSem () failed. Status = [0x%x]\n",
                             status) ;
        }
    }

    /*
     *  Send notifications to the DSP with information about the address of the
     *  control structure and data buffer to be used by the application.
     *
     */
    if (DSP_SUCCEEDED (status)) {
        status = NOTIFY_notify (processorId,
                                MPCSXFER_IPS_ID,
                                MPCSXFER_IPS_EVENTNO,
                                (Uint32) dspCtrlBuf) ;
        if (DSP_SUCCEEDED (status)) {
            status = NOTIFY_notify (processorId,
                                    MPCSXFER_IPS_ID,
                                    MPCSXFER_IPS_EVENTNO,
                                    (Uint32) dspDataBuf) ;
            if (DSP_FAILED (status)) {
                MPCSXFER_1Print ("NOTIFY_notify () DataBuf failed."
                                 " Status = [0x%x]\n",
                                 status) ;
            }
        }
        else {
            MPCSXFER_1Print ("NOTIFY_notify () CtrlBuf failed."
                             " Status = [0x%x]\n",
                             status) ;
        }
    }

    MPCSXFER_0Print ("Leaving MPCSXFER_Create ()\n") ;

    return status ;
}


/** ============================================================================
 *  @func   MPCSXFER_Execute
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MPCSXFER_Execute (IN Uint32 numIterations, Uint8 processorId)
{
    DSP_STATUS  status    = DSP_SOK ;
    DSP_STATUS  tmpStatus = DSP_SOK ;
    Uint32      gppIter ;
    Uint32      dspIter ;
    Uint32      i ;

    MPCSXFER_0Print ("Entered MPCSXFER_Execute ()\n") ;

    for (gppIter = 1, dspIter = 1 ;
         (   (   (numIterations == 0)
              || ((gppIter <= numIterations) && (dspIter <= numIterations)))
          && (DSP_SUCCEEDED (status))) ;
         ) {
        /* Begin protection of the shared control and data buffers. */
        status = MPCS_enter (MPCSXFER_MpcsHandle) ;
        if (DSP_FAILED (status)) {
            MPCSXFER_1Print ("MPCS_enter () failed. Status = [0x%x]\n",
                            status) ;
        }
        else {
            POOL_invalidate (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                             MPCSXFER_CtrlBuf,
                             DSPLINK_ALIGN (sizeof (MPCSXFER_Ctrl),
                                            DSPLINK_BUF_ALIGN)) ;
            POOL_invalidate (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                             MPCSXFER_DataBuf,
                             MPCSXFER_BufferSize) ;
            if (    (MPCSXFER_CtrlBuf->procId == processorId)
                ||  (MPCSXFER_CtrlBuf->procId == MPCSXFER_INVALID_ID)) {
                if (MPCSXFER_CtrlBuf->procId == processorId) {
                    /* Verify correctness of DSP data received. */
                    status = MPCSXFER_VerifyData (MPCSXFER_CtrlBuf,
                                                  MPCSXFER_DataBuf,
                                                  dspIter) ;
                    if (DSP_SUCCEEDED (status)) {
                        dspIter++ ;
                    }
                }

                /* Now claim the buffer and fill it with the GPP data. */
                if (DSP_SUCCEEDED (status)) {
                    /* Take control of the buffer. */
                    MPCSXFER_CtrlBuf->procId = ID_GPP ;
                    MPCSXFER_CtrlBuf->iterNo = gppIter ;

                    /* Fill the data buffer with the GPP stamp. */
                    for (i = 0 ; i < (MPCSXFER_BufferSize / 2) ; i++) {
                        MPCSXFER_DataBuf [i] = MPCSXFER_GPP_STAMP ;
                    }

                    /* Increment the GPP iteration number for next time. */
                    gppIter++ ;
                }
            }
            else if (MPCSXFER_CtrlBuf->procId == ID_GPP) {
                /* Do nothing if the buffer is already filled by the GPP. Some
                 * other processing can be done here if required.
                 * Sleep for some time to simulate this.
                 */
                MPCSXFER_Sleep (10) ;
            }
            POOL_writeback (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                            MPCSXFER_CtrlBuf,
                            DSPLINK_ALIGN (sizeof (MPCSXFER_Ctrl),
                                           DSPLINK_BUF_ALIGN)) ;
            POOL_writeback (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                            MPCSXFER_DataBuf,
                            MPCSXFER_BufferSize) ;
            /* End protection of the shared control and data buffers. */
            tmpStatus = MPCS_leave (MPCSXFER_MpcsHandle) ;
            if (DSP_FAILED (tmpStatus)) {
                MPCSXFER_1Print ("MPCS_leave () failed. Status = [0x%x]\n",
                                tmpStatus) ;
                if (DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                }
            }
        }

        if (DSP_SUCCEEDED (status) && ((gppIter % 100) == 0)) {
            MPCSXFER_1Print ("GPP->DSP: Transferred %ld buffers\n", gppIter) ;
        }

        if (DSP_SUCCEEDED (status) && ((dspIter % 100) == 0)) {
            MPCSXFER_1Print ("DSP->GPP: Transferred %ld buffers\n", dspIter) ;
        }
    }

    MPCSXFER_0Print ("Leaving MPCSXFER_Execute ()\n") ;

    return status ;
}


/** ============================================================================
 *  @func   MPCSXFER_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          MPCSXFER_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MPCSXFER_Delete (Uint8 processorId)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;

    MPCSXFER_0Print ("Entered MPCSXFER_Delete ()\n") ;

    /*
     *  Stop execution on DSP.
     */
    status = PROC_stop (processorId) ;
    if (DSP_FAILED (status)) {
        MPCSXFER_1Print ("PROC_stop () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Unregister for notification of event registered earlier.
     */
    tmpStatus = NOTIFY_unregister (processorId,
                                   MPCSXFER_IPS_ID,
                                   MPCSXFER_IPS_EVENTNO,
                                   (FnNotifyCbck) MPCSXFER_Notify,
                                   MPCSXFER_SemPtr) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MPCSXFER_1Print ("NOTIFY_unregister () failed Status = [0x%x]\n",
                         status) ;
    }

    /*
     *  Delete the semaphore used for notification.
     */
    if (MPCSXFER_SemPtr != NULL) {
        tmpStatus = MPCSXFER_DeleteSem (MPCSXFER_SemPtr) ;
        MPCSXFER_SemPtr = NULL ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            MPCSXFER_1Print ("MPCSXFER_DeleteSem () failed Status = [0x%x]\n",
                             status) ;
        }
    }

    /*
     *  Close the handle to the MPCS object.
     */
    if (MPCSXFER_MpcsHandle != NULL) {
        tmpStatus = MPCS_close (processorId, MPCSXFER_MpcsHandle) ;
        MPCSXFER_MpcsHandle = NULL ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            MPCSXFER_1Print ("MPCS_close () failed. Status = [0x%x]\n",
                             status) ;
        }
    }

    /*
     *  Delete the MPCS object created on the GPP.
     */
    tmpStatus = MPCS_delete (processorId, MPCSXFER_MpcsObjName) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MPCSXFER_1Print ("MPCS_delete () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Free the memory allocated for the data buffer.
     */
    tmpStatus = POOL_free (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                           (Void *) MPCSXFER_DataBuf,
                           MPCSXFER_BufferSize) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MPCSXFER_1Print ("POOL_free () DataBuf failed. Status = [0x%x]\n",
                         status) ;
    }

    /*
     *  Free the memory allocated for the control buffer.
     */
    tmpStatus = POOL_free (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                           (Void *) MPCSXFER_CtrlBuf,
                           DSPLINK_ALIGN (sizeof (MPCSXFER_Ctrl),
                                          DSPLINK_BUF_ALIGN)) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MPCSXFER_1Print ("POOL_free () CtrlBuf failed. Status = [0x%x]\n",
                         status) ;
    }

    /*
     *  Close the pool
     */
    tmpStatus = POOL_close (POOL_makePoolId(processorId, SAMPLE_POOL_ID)) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MPCSXFER_1Print ("POOL_close () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Detach from the processor
     */
    tmpStatus = PROC_detach  (processorId) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MPCSXFER_1Print ("PROC_detach () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Destroy the PROC object.
     */
    tmpStatus = PROC_destroy () ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MPCSXFER_1Print ("PROC_destroy () failed. Status = [0x%x]\n", status) ;
    }

    MPCSXFER_0Print ("Leaving MPCSXFER_Delete ()\n") ;
}


/** ============================================================================
 *  @func   MPCSXFER_Main
 *
 *  @desc   Entry point for the application
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MPCSXFER_Main (IN Char8 * dspExecutable,
               IN Char8 * strBufferSize,
               IN Char8 * strNumIterations,
               IN Char8 * strProcessorId)
{
    DSP_STATUS status       = DSP_SOK ;
    Uint8      processorId  = 0 ;

    MPCSXFER_0Print ("========== Sample Application : MPCSXFER ==========\n") ;

    if (   (dspExecutable != NULL)
        && (strBufferSize != NULL)
        && (strNumIterations != NULL)) {
        /*
         *  Validate the buffer size and number of iterations specified.
         */
        MPCSXFER_BufferSize = DSPLINK_ALIGN (MPCSXFER_Atoi (strBufferSize),
                                             DSPLINK_BUF_ALIGN) ;

        MPCSXFER_NumIterations = MPCSXFER_Atoi (strNumIterations) ;
        processorId            = MPCSXFER_Atoi (strProcessorId) ;
        if ((MPCSXFER_NumIterations >  0xFFFF) || (MPCSXFER_BufferSize == 0)) {
            status = DSP_EINVALIDARG ;
            MPCSXFER_1Print ("ERROR! Invalid arguments specified for  "
                             "mpcsxfer application.\n"
                             "     Max iterations = %d\n",
                             0xFFFF) ;
            MPCSXFER_1Print ("     Buffer size    = %d\n",
                             MPCSXFER_BufferSize) ;
        }
        else {

             if (processorId >= MAX_DSPS) {
                 MPCSXFER_1Print ("== Error: Invalid processor id %d specified"
                                  " ==\n", processorId) ;
                 status = DSP_EINVALIDARG ;
            }

            if (DSP_SUCCEEDED (status)) {
                /*
                 *  Specify the dsp executable file name and the buffer size for
                 *  MPCSXFER creation phase.
                 */
                status = MPCSXFER_Create (dspExecutable,
                                          strBufferSize,
                                          strNumIterations,
                                          processorId) ;

                 /*
                  *  Execute the RING_IO data transfer for specified iterations.
                  */
                 if (DSP_SUCCEEDED (status)) {
                     status = MPCSXFER_Execute (MPCSXFER_NumIterations,
                                                processorId) ;
                 }

                /*
                 *  Perform cleanup operation.
                 */
                MPCSXFER_Delete (processorId) ;
            }
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        MPCSXFER_0Print ("ERROR! Invalid arguments specified for  "
                         "MPCSXFER application\n") ;
    }

    MPCSXFER_0Print ("====================================================\n") ;
}

/** ----------------------------------------------------------------------------
 *  @func   MPCSXFER_Notify
 *
 *  @desc   This function implements the event callback registered with the
 *          NOTIFY component to receive notification indicating that the DSP-
 *          side application has completed its setup phase.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
Void
MPCSXFER_Notify (Uint32 eventNo, Pvoid arg, Pvoid info)
{
    DSP_STATUS status = DSP_SOK ;

    /* Post the semaphore. */
    status = MPCSXFER_PostSem ((Pvoid) arg) ;
    if (DSP_FAILED (status)) {
        MPCSXFER_1Print ("MPCSXFER_PostSem () failed. Status = [0x%x]\n",
                         status) ;
    }
}


/** ----------------------------------------------------------------------------
 *  @func   MPCSXFER_VerifyData
 *
 *  @desc   This function verifies the data-integrity of given buffer.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
MPCSXFER_VerifyData (IN MPCSXFER_Ctrl * ctrlBuf,
                     IN Uint16 *        dataBuf,
                     IN Uint32          iterNo)
{
    DSP_STATUS status = DSP_SOK ;
    Uint32     i ;

    /*
     *  Verify the iteration number against the expected value.
     */
    if (ctrlBuf->iterNo != iterNo) {
        status = DSP_EFAIL ;
        MPCSXFER_0Print ("ERROR! Data integrity check failed\n") ;
        MPCSXFER_1Print ("Expected iteration Number [%d]\n", iterNo) ;
        MPCSXFER_1Print ("Received iteration Number [%d]\n", ctrlBuf->iterNo) ;
    }
    else if ((Bool) ctrlBuf->dataVerify == FALSE) {
        status = DSP_EFAIL;
        MPCSXFER_0Print ("ERROR! DSP-side data integrity check failed\n") ;
    }
    else {
        /*
         *  Verify the contents of the data buffer.
         */
        for (i = 0 ;
             i < (MPCSXFER_BufferSize / 2) && DSP_SUCCEEDED (status) ;
             i++) {
            if (dataBuf [i] != MPCSXFER_DSP_STAMP) {
                status = DSP_EFAIL;
                MPCSXFER_0Print ("ERROR! Data integrity check failed\n") ;
                MPCSXFER_1Print ("Expected [0x%x]\n",
                                 MPCSXFER_DSP_STAMP) ;
                MPCSXFER_1Print ("Received [0x%x]\n", dataBuf [i]) ;
            }
        }
    }

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
