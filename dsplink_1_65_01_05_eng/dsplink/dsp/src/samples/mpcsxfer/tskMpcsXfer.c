/** ============================================================================
 *  @file   tskMpcsXfer.c
 *
 *  @path   $(DSPLINK)/dsp/src/samples/mpcsxfer/
 *
 *  @desc   This is a TSK based application that demonstrates the usage of the
 *          MPCS component.
 *          A shared data buffer and control structure are allocated by the GPP-
 *          side application. Pointers to these memory regions are received on
 *          the DSP-side through an event registered with the NOTIFY component.
 *          These shared data and control buffers are provided with mutually
 *          exclusive access from the GPP and DSP-side application using an MPCS
 *          object created on the GPP.
 *          A handle to this MPCS object is obtained on the DSP-side by opening
 *          the MPCS object by an application-specific name.
 *          For the number of iterations of data transfer specified by the user,
 *          the DSP-side application attempts to gain access to the shared data
 *          and control buffers by entering the MPCS. It then checks if the
 *          buffer is available for use (procId in the control structure is
 *          MPCSXFER_INVALID_ID), or filled by the GPP.
 *          - If it has been filled by the DSP, nothing further is done for this
 *            iteration, and the MPCS control is relinquished.
 *          - If available, the buffer is claimed and its contents filled with
 *            the DSP stamp.
 *          - If filled by the GPP, the buffer contents are verified against
 *            expected value. The results are written back to the control
 *            structure. The buffer is now claimed by the DSP, and the DSP stamp
 *            is filled into the buffer.
 *          Control of the data and control buffers is now relinquished by
 *          leaving the MPCS region.
 *          The data transfer are carried out till the number of iterations
 *          specified is completed for both GPP->DSP and DSP->GPP transfers.
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
#include <gbl.h>
#include <log.h>
#include <swi.h>
#include <sys.h>
#include <tsk.h>
#include <pool.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <failure.h>
#include <dsplink.h>
#include <platform.h>
#include <notify.h>
#include <mpcs.h>
#include <hal_cache.h>
#if defined (DSP_BOOTMODE_NOBOOT)
#include <sma_pool.h>
#endif
/*  ----------------------------------- Sample Headers              */
#include <mpcsxfer_config.h>
#include <tskMpcsXfer.h>


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   FILEID is used by SET_FAILURE_REASON macro.
 *  ============================================================================
 */
#define FILEID  FID_APP_C

/** ============================================================================
 *  @name   MPCSXFER_BufferSize
 *
 *  @desc   Declaration of the size of buffer to be used for data transfer.
 *  ============================================================================
 */
extern Uint16 MPCSXFER_BufferSize ;

/** ============================================================================
 *  @name   MPCSXFER_NumIterations
 *
 *  @desc   Declaration of the iterations of data transfer to be done by the
 *          application.
 *  ============================================================================
 */
extern Uint32 MPCSXFER_NumIterations ;


/** ----------------------------------------------------------------------------
 *  @func   TSKMPCSXFER_notify
 *
 *  @desc   This function implements the event callback registered with the
 *          NOTIFY component to receive the pointers to the control and data
 *          buffers allocated by the GPP-side application.
 *
 *  @arg    eventNo
 *              Event number associated with the callback being invoked.
 *  @arg    arg
 *              Fixed argument registered with the NOTIFY component along with
 *              the callback function.
 *  @arg    info
 *              Run-time information provided to the upper layer by the NOTIFY
 *              component. This information is specific to the NOTIFY being
 *              implemented.
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
static
Void
TSKMPCSXFER_notify (Uint32 eventNo, Ptr arg, Ptr info) ;


/** ----------------------------------------------------------------------------
 *  @func   MPCSXFER_verifyData
 *
 *  @desc   This function verifies the data-integrity of given data buffer.
 *
 *  @arg    ctrlBuf
 *              Pointer to the control buffer whose contents are to be verified.
 *  @arg    dataBuf
 *              Pointer to the data whose contents are to be verified.
 *  @arg    iterNo
 *              Expected iteration number for GPP.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Data verification failed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static
Int
MPCSXFER_verifyData (MPCSXFER_Ctrl * ctrlBuf,
                     Uint16 *        dataBuf,
                     Uint32          iterNo) ;

#if defined (DSP_BOOTMODE_NOBOOT)
SMAPOOL_Params            smaPoolObj ;
#endif

/** ============================================================================
 *  @func   TSKMPCSXFER_create
 *
 *  @desc   Create phase function for the MPCSXFER application. Initializes the
 *          TSKMPCSXFER_TransferInfo structure with the information that will be
 *          used by the other phases of the application.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
TSKMPCSXFER_create (TSKMPCSXFER_TransferInfo ** infoPtr)
{
    Int                        status    = SYS_OK ;
    Bool                       semStatus = TRUE ;
    TSKMPCSXFER_TransferInfo * info      = NULL ;
#if defined (DSP_BOOTMODE_NOBOOT)
    POOL_Obj                  poolObj ;

    smaPoolObj.poolId        = 0;
    smaPoolObj.exactMatchReq = TRUE ;
    poolObj.initFxn = SMAPOOL_init ;
    poolObj.fxns    = (POOL_Fxns *) &SMAPOOL_FXNS ;
    poolObj.params  = &(smaPoolObj) ;
    poolObj.object  = NULL ;

    status = POOL_open (0, &poolObj) ;
#endif

    /* Allocate TSKMPCSXFER_TransferInfo structure that will be initialized
     * and passed to other phases of the application */
    if (status == SYS_OK) {
        *infoPtr = MEM_calloc (DSPLINK_SEGID,
                               sizeof (TSKMPCSXFER_TransferInfo),
                               0) ; /* No alignment restriction */
        if (*infoPtr == NULL) {
            status = SYS_EALLOC ;
            SET_FAILURE_REASON (status) ;
        }
        else {
            info = *infoPtr ;
        }
    }

    /* Fill up the transfer info structure */
    if (status == SYS_OK) {
        info->mpcsHandle    = NULL ; /* Initialized in MPCS_open (). */
        info->dataBuf       = NULL ; /* Set through notification callback. */
        info->ctrlBuf       = NULL ; /* Set through notification callback. */
        info->numIterations = MPCSXFER_NumIterations ;
        info->bufferSize    = MPCSXFER_BufferSize ;
        SEM_new (&(info->notifySemObj), 0) ;
    }

    /*
     *  Open the MPCS object created by the GPP and get a handle to it.
     */
    if (status == SYS_OK) {
        status = MPCS_open (ID_GPP,
                            MPCSXFER_MPCSOBJ_NAME,
                            &(info->mpcsHandle)) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    /*
     *  Register notification for the event callback to get control and data
     *  buffer pointers from the GPP-side.
     */
    if (status == SYS_OK) {
        status = NOTIFY_register (ID_GPP,
                                  MPCSXFER_IPS_ID,
                                  MPCSXFER_IPS_EVENTNO,
                                  (FnNotifyCbck) TSKMPCSXFER_notify,
                                  info) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    /*
     *  Send notification to the GPP-side that the application has completed its
     *  setup and is ready for further execution.
     */
    if (status == SYS_OK) {
        status = NOTIFY_notify (ID_GPP,
                                MPCSXFER_IPS_ID,
                                MPCSXFER_IPS_EVENTNO,
                                (Uint32) 0) ; /* No payload to be sent. */
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    /*
     *  Wait for the event callback from the GPP-side to post the semaphore
     *  indicating receipt of the control buffer pointer.
     */
    if (status == SYS_OK) {
        semStatus = SEM_pend (&(info->notifySemObj), SYS_FOREVER) ;
        if ((semStatus == FALSE) || (info->ctrlBuf == NULL)) {
            status = SYS_EINVAL ;
            SET_FAILURE_REASON (status) ;
        }
    }

    /*
     *  Wait for the event callback from the GPP-side to post the semaphore
     *  indicating receipt of the data buffer pointer.
     */
    if (status == SYS_OK) {
        semStatus = SEM_pend (&(info->notifySemObj), SYS_FOREVER) ;
        if ((semStatus == FALSE) || (info->dataBuf == NULL)) {
            status = SYS_EINVAL ;
            SET_FAILURE_REASON (status) ;
        }
    }

    return status ;
}


/** ============================================================================
 *  @func   TSKMPCSXFER_execute
 *
 *  @desc   Execute phase function for the TSKMPCSXFER application.
 *          The application exchanges data with the GPP-side using the shared
 *          control and data buffers protected using the MPCS object.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
TSKMPCSXFER_execute (TSKMPCSXFER_TransferInfo * info)
{
    Int     status    = SYS_OK ;
    Int     tmpStatus = SYS_OK ;
    Uint16  dspId     = GBL_getProcId () ;
    Uint32  gppIter ;
    Uint32  dspIter ;
    Uint32  i ;

    for (gppIter = 1, dspIter = 1 ;
         (   (   (info->numIterations == 0)
              || (    (gppIter <= info->numIterations)
                   && (dspIter <= info->numIterations)))
          && (status == SYS_OK)) ;
         ) {
        /* Begin protection of the shared control and data buffers. */
        status = MPCS_enter (info->mpcsHandle) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {
            /* Invalidate cache for the control and data buffers. */
            HAL_cacheInv ((Ptr) info->ctrlBuf,
                          DSPLINK_ALIGN (sizeof (MPCSXFER_Ctrl),
                                         DSPLINK_BUF_ALIGN)) ;
            HAL_cacheInv ((Ptr) info->dataBuf, info->bufferSize) ;
            if (    (info->ctrlBuf->procId == ID_GPP)
                ||  (info->ctrlBuf->procId == MPCSXFER_INVALID_ID)) {
                if (info->ctrlBuf->procId == ID_GPP) {
                    /* Verify correctness of GPP data received. */
                    status = MPCSXFER_verifyData (info->ctrlBuf,
                                                  info->dataBuf,
                                                  gppIter) ;
                    if (status == SYS_OK) {
                        info->ctrlBuf->dataVerify = (Uint32) TRUE ;
                        gppIter++ ;
                    }
                    else {
                        info->ctrlBuf->dataVerify = (Uint32) FALSE ;
                    }
                }

                /* Now claim the buffer and fill it with the DSP data. */

                /* Take control of the buffer. */
                info->ctrlBuf->procId = dspId ;
                info->ctrlBuf->iterNo = dspIter ;

                /* Fill the data buffer with the DSP stamp. */
                for (i = 0 ; i < (info->bufferSize / 2) ; i++) {
                    info->dataBuf [i] = MPCSXFER_DSP_STAMP ;
                }

                /* Increment the DSP iteration number for next time. */
                dspIter++ ;
            }
            else if (info->ctrlBuf->procId == dspId) {
                /* Do nothing if the buffer is already filled by the DSP. */
            }

            /* Write back cache contents for the control and data buffers. */
            HAL_cacheWbInv ((Ptr) info->ctrlBuf,
                            DSPLINK_ALIGN (sizeof (MPCSXFER_Ctrl),
                                           DSPLINK_BUF_ALIGN)) ;
            HAL_cacheWbInv ((Ptr) info->dataBuf, info->bufferSize) ;
            /* End protection of the shared control and data buffers. */
            tmpStatus = MPCS_leave (info->mpcsHandle) ;
            if (tmpStatus != SYS_OK) {
                SET_FAILURE_REASON (tmpStatus) ;
                if (status == SYS_OK) {
                    status = tmpStatus ;
                }
            }
        }
    }

    return status ;
}


/** ============================================================================
 *  @func   TSKMPCSXFER_delete
 *
 *  @desc   Delete phase function for the TSKMPCSXFER application. It frees
 *          all the resources of allocated during create phase of the
 *          application.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
TSKMPCSXFER_delete (TSKMPCSXFER_TransferInfo * info)
{
    Int    status     = SYS_OK ;
    Int    tmpStatus  = SYS_OK ;
    Bool   freeStatus = FALSE ;
    /*
     *  Unregister notification for the event callback used to get control and
     *  data buffer pointers from the GPP-side.
     */
    status = NOTIFY_unregister (ID_GPP,
                                MPCSXFER_IPS_ID,
                                MPCSXFER_IPS_EVENTNO,
                                (FnNotifyCbck) TSKMPCSXFER_notify,
                                info) ;
    if (status != SYS_OK) {
        SET_FAILURE_REASON (status) ;
    }

    /*
     *  Close the handle to MPCS object created by the GPP.
     */
    if (info->mpcsHandle != NULL) {
        tmpStatus = MPCS_close (ID_GPP, info->mpcsHandle) ;
        info->mpcsHandle = NULL ;
        if ((status == SYS_OK) && (tmpStatus != SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }
    }

    /* Free the info structure */
    freeStatus = MEM_free (DSPLINK_SEGID,
                           info,
                           sizeof (TSKMPCSXFER_TransferInfo)) ;
    info = NULL ;
    if ((status == SYS_OK) && (freeStatus != TRUE)) {
        status = SYS_EFREE ;
        SET_FAILURE_REASON (status) ;
    }

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   TSKMPCSXFER_notify
 *
 *  @desc   This function implements the event callback registered with the
 *          NOTIFY component to receive the pointers to the control and data
 *          buffers allocated by the GPP-side application.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Void
TSKMPCSXFER_notify (Uint32 eventNo, Ptr arg, Ptr info)
{
    TSKMPCSXFER_TransferInfo * mpcsInfo = (TSKMPCSXFER_TransferInfo *) arg ;

    (Void) eventNo ; /* To avoid compiler warning. */

    /* First notification is for the control buffer pointer. */
    if (mpcsInfo->ctrlBuf == NULL) {
        mpcsInfo->ctrlBuf = (MPCSXFER_Ctrl *) info ;
    }
    else {
        /* Second notification is for the data buffer pointer. */
        mpcsInfo->dataBuf = (Uint16 *) info ;
    }

    /* Post the semaphore. */
    SEM_post (&(mpcsInfo->notifySemObj)) ;
}


/** ----------------------------------------------------------------------------
 *  @func   MPCSXFER_verifyData
 *
 *  @desc   This function verifies the data-integrity of given buffer.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Int
MPCSXFER_verifyData (MPCSXFER_Ctrl * ctrlBuf,
                     Uint16 *        dataBuf,
                     Uint32          iterNo)
{
    Int     status = SYS_OK ;
    Uint32  i ;

    /*
     *  Verify the iteration number against the expected value.
     */
    if (ctrlBuf->iterNo != iterNo) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else if ((Bool) ctrlBuf->dataVerify == FALSE) {
        status = SYS_EINVAL;
        SET_FAILURE_REASON (status) ;
    }
    else {
        /*
         *  Verify the contents of the data buffer.
         */
        for (i = 0 ;
             i < (MPCSXFER_BufferSize / 2) && (status == SYS_OK) ;
             i++) {
            if (dataBuf [i] != MPCSXFER_GPP_STAMP) {
                status = SYS_EINVAL;
                SET_FAILURE_REASON (status) ;
            }
        }
    }

    return status ;
}
