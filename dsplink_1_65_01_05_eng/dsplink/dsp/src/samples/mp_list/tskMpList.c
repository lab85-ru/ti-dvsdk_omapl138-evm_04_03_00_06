/** ============================================================================
 *  @file   tskMpList.c
 *
 *  @path   $(DSPLINK)/dsp/src/samples/mp_list/
 *
 *  @desc   This is a TSK based application that demonstrates the usage of the
 *          MPLIST component.
 *          A shared list is created by the GPP-side application. The
 *          application  allocates memory for the number of list elements as
 *          required by the user. The GPP-side appication initializes the list
 *          element 'iter' with the iteration number. It also initializes the
 *          list element 'pos' with the current position in the list.  It adds
 *          the element into the list using the putTail api. After the GPP-side
 *          has finished adding all elements to the list, it sends a
 *          notification to the DSP-side through an event registered with the
 *          NOTIFY component.
 *          A handle to this MPLIST object is obtained on the DSP-side by
 *          opening the MPLIST object by an application-specific name. Using the
 *          handle the DSP-side application pops the elements from the list
 *          using getHead api. It modifies the list element 'value' as a
 *          function of the elements 'iter' and 'pos'. After modification, the
 *          list element is added to the list using putTail api. This is done
 *          for all elements in the list. After this is done, the DSP-side sends
 *          a notification to the GPP side through an event registered with the
 *          NOTIFY component. This event notifies the GPP that the DSP has
 *          finished modifying the list.
 *          The GPP-side verifies that the list element values are consistent
 *          with what is expected. This sequence is done for the number of
 *          iterations specified by the user.
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
#include <sys.h>
#include <tsk.h>
#include <pool.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <failure.h>
#include <dsplink.h>
#include <platform.h>
#include <notify.h>
#include <hal_cache.h>
#if defined (DSP_BOOTMODE_NOBOOT)
#include <sma_pool.h>
#endif

/*  ----------------------------------- MpList Headers              */
#include <mplist.h>

/*  ----------------------------------- Sample Headers              */
#include <mp_list_config.h>
#include <tskMpList.h>


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   FILEID is used by SET_FAILURE_REASON macro.
 *  ============================================================================
 */
#define FILEID  FID_APP_C

/** ============================================================================
 *  @name   MP_LIST_NumIterations
 *
 *  @desc   Iterations of data transfer to be done by the application.
 *  ============================================================================
 */
extern Uint16 MP_LIST_NumIterations ;

/** ============================================================================
 *  @name   MP_LIST_NumElem
 *
 *  @desc   Number of elements to put at tail of list.
 *  ============================================================================
 */
extern Uint16 MP_LIST_NumElem ;

/** ============================================================================
 *  @name   MP_LIST_ListData
 *
 *  @desc   List element data structure.
 *
 *  @field  head
 *             An element of type MPLIST_Header. This is used to indicate that
 *             it is a list element.
 * @field   iter
 *             An element of type Uint32. The GPP-side application sets it to
 *             the value of the current iteration in progress.
 * @field   pos
 *             An element of type Uint32. The GPP-side application sets it to
 *             the value of the current position of list element in list.
 * @field   value
 *             An element of type Uint32. The DSP-side application sets it to
 *             a function of the iter and pos element, namely a multiplication
 *             of the two. This value is used by the GPP-side application to
 *             check if the SP-side has correctly manipulated the list element.
 *
 *  ============================================================================
 */
typedef struct MP_LIST_ListData_tag{
    MPLIST_Header head ;
    Uint32        iter ;
    Uint32        pos ;
    Uint32        value ;
} MP_LIST_ListData ;


/** ----------------------------------------------------------------------------
 *  @func   TSKMPLIST_notify
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
TSKMPLIST_notify (Uint32 eventNo, Ptr arg, Ptr info) ;

#if defined (DSP_BOOTMODE_NOBOOT)
SMAPOOL_Params            smaPoolObj ;
#endif
/** ============================================================================
 *  @func   TSKMPLIST_create
 *
 *  @desc   Create phase function for the MP_LIST application. Initializes the
 *          TSKMPLIST_TransferInfo structure with the information that will be
 *          used by the other phases of the application.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int TSKMPLIST_create(TSKMPLIST_TransferInfo ** infoPtr)
{
    Int                      status = SYS_OK ;
    TSKMPLIST_TransferInfo * info   = NULL ;
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

    /* Allocate TSKMPLIST_TransferInfo structure that will be initialized
     * and passed to other phases of the application
     */
    *infoPtr = MEM_calloc (DSPLINK_SEGID,
                           sizeof (TSKMPLIST_TransferInfo),
                           DSPLINK_BUF_ALIGN) ;
    if (*infoPtr == NULL) {
        status = SYS_EALLOC ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        info = *infoPtr ;
    }

    /*
     *  Register notification for the event callback to understand when the
     *  GPP-side finished adding all elements to the list.
     */
    if (status == SYS_OK) {
        status = NOTIFY_register (ID_GPP,
                                  MP_LIST_IPS_ID,
                                  MP_LIST_IPS_EVENTNO,
                                  (FnNotifyCbck) TSKMPLIST_notify,
                                  info) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }


    /* Get a handle to the MpList created by GPP-side application */
    if (status == SYS_OK) {
        status = MPLIST_open(ID_GPP,
                             SAMPLE_MP_LIST_GPP_NAME,
                             &(info->gppMpListHandle)) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    if (status == SYS_OK) {
        /* Filling up the transfer info structure */
        info->numIterations = MP_LIST_NumIterations ;
        info->numElem       = MP_LIST_NumElem ;
        SEM_new (&(info->notifySemObj), 0) ;
    }

    /*
     *  Send notification to the GPP-side that the application has completed its
     *  setup and is ready for further execution.
     */
    if (status == SYS_OK) {
        status = NOTIFY_notify (ID_GPP,
                                MP_LIST_IPS_ID,
                                MP_LIST_IPS_EVENTNO,
                                (Uint32) 0) ; /* No payload to be sent. */
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }


    return status ;
}


/** ============================================================================
 *  @func   TSKMPLIST_execute
 *
 *  @desc   Execute phase function for the TSKSCALE application. Application
 *          receives the data from the input channel and sends the same data
 *          back on output channel. Channel numbers can be configured through
 *          header file.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int TSKMPLIST_execute(TSKMPLIST_TransferInfo * info)
{
    Int                status         = SYS_OK ;
    Int                semStatus      = SYS_OK ;
    Int                tmpStatus      = SYS_OK ;
    Uint32             listElemSize   = DSPLINK_ALIGN (sizeof(MP_LIST_ListData),
                                                       DSPLINK_BUF_ALIGN) ;
    MPLIST_Handle      mplistHandle   = info->gppMpListHandle ;
    MP_LIST_ListData * currentElement = NULL ;
    Uint32   i ;
    Uint32   j ;

    for (i = 1 ; ((i <= info->numIterations) && (status == SYS_OK)) ; i++) {

        /* Waiting for notification from GPP side indicating that elements
         * have been added to list.
         */
        semStatus = SEM_pend (&(info->notifySemObj), SYS_FOREVER) ;
        if (semStatus == FALSE) {
            status = SYS_EINVAL ;
            SET_FAILURE_REASON (status) ;
        }
        else {
            for (j = 0 ;
                 ((j < info->numElem)) &&  ((status == SYS_OK)) ;
                 j++) {
                /* Get list head */
                status = MPLIST_getHead(mplistHandle,
                                        (MPLIST_Elem *) &currentElement) ;
                if (status == SYS_OK) {
                    /* Get list element contents */
                    HAL_cacheInv((Ptr)currentElement, listElemSize) ;

                    /* Update list element contents */
                    currentElement->value =   currentElement->iter
                                            * currentElement->pos ;
                    HAL_cacheWbInv((Ptr)currentElement, listElemSize) ;
                    /* Add element to end of list */
                    status = MPLIST_putTail (mplistHandle,
                                             (MPLIST_Elem ) currentElement) ;
                    if (status != SYS_OK) {
                        SET_FAILURE_REASON (status) ;
                    }
                }
                else {
                    SET_FAILURE_REASON (status) ;
                }
            }
            if (status == SYS_OK) {
                tmpStatus = NOTIFY_notify (ID_GPP,
                                           MP_LIST_IPS_ID,
                                           MP_LIST_IPS_EVENTNO,
                                           (Uint32 ) 0) ;
                if ((tmpStatus != SYS_OK) && (status == SYS_OK)) {
                    status = tmpStatus ;
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
 *  @func   TSKMPLIST_delete
 *
 *  @desc   Delete phase function for the TSKSCALE application. It deallocates
 *          all the resources of allocated during create phase of the
 *          application.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int TSKMPLIST_delete (TSKMPLIST_TransferInfo * info)
{
    Int           status       = SYS_OK ;
    Bool          freeStatus   = FALSE ;
    MPLIST_Handle mplistHandle = info->gppMpListHandle ;

    /*
     *  Close the mplist handle
     */
    status = MPLIST_close(mplistHandle) ;
    if (status != SYS_OK) {
        SET_FAILURE_REASON (status) ;
    }

    /*
     *  Unregister notification for the event callback.
     */
    status = NOTIFY_unregister (ID_GPP,
                                MP_LIST_IPS_ID,
                                MP_LIST_IPS_EVENTNO,
                                (FnNotifyCbck) TSKMPLIST_notify,
                                info) ;
    if (status != SYS_OK) {
        SET_FAILURE_REASON (status) ;
    }

    /* Free the info structure */
    freeStatus = MEM_free (DSPLINK_SEGID,
                           info,
                           sizeof (TSKMPLIST_TransferInfo)) ;
    info = NULL ;
    if ((status == SYS_OK) && (freeStatus != TRUE)) {
        status = SYS_EFREE ;
        SET_FAILURE_REASON (status) ;
    }

    return status ;
}

/** ----------------------------------------------------------------------------
 *  @func   TSKMPLIST_notify
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
TSKMPLIST_notify (Uint32 eventNo, Ptr arg, Ptr info)
{
    TSKMPLIST_TransferInfo * mplistInfo = (TSKMPLIST_TransferInfo *) arg ;

    (Void) eventNo ; /* To avoid compiler warning. */
    (Void) info ; /* To avoid compiler warning. */

    /* Post the semaphore. */
    SEM_post (&(mplistInfo->notifySemObj)) ;
}

