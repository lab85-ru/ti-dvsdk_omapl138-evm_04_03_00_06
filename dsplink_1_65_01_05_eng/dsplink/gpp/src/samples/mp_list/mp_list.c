/** ============================================================================
 *  @file   mp_list.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/mp_list/
 *
 *  @desc   This sample application demonstrates the usage of MPLIST api's
 *          for directly accessing DSP memory.
 *          This is a TSK based application that demonstrates the usage of the
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


/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>

/*  ----------------------------------- DSP/BIOS LINK API               */
#include <proc.h>
#include <mplist.h>
#include <pool.h>
#include <notify.h>
#if defined (DA8XXGEM)
#include <loaderdefs.h>
#endif


/*  ----------------------------------- Application Header              */
#include <mp_list.h>
#include <mp_list_os.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @const   NUM_ARGS
 *
 *  @desc   Number of arguments specified to the DSP application.
 *  ============================================================================
 */
#define NUM_ARGS              2

/** ============================================================================
 *  @name   SAMPLE_POOL_ID
 *
 *  @desc   ID of the POOL used for the sample.
 *  ============================================================================
 */
#define SAMPLE_POOL_ID        0

/** ============================================================================
 *  @name   NUM_BUF_SIZES
 *
 *  @desc   Number of buffer pools to be configured for the allocator.
 *  ============================================================================
 */
#define NUM_BUF_SIZES         2

/** ============================================================================
 *  @const  NUM_MPLIST
 *
 *  @desc   Number of MPLISTS
 *  ============================================================================
 */
#define NUM_MPLIST             1

/** ============================================================================
 *  @const  MAX_NUM_ELEMENTS
 *
 *  @desc   Maximum number of elements that can be added to a list.
 *  ============================================================================
 */
#define MAX_NUM_ELEMENTS       1024

/** ============================================================================
 *  @const  NUM_BUF_POOL0
 *
 *  @desc   Number of buffers in first buffer pool.
 *  ============================================================================
 */
#define NUM_BUF_POOL0         NUM_MPLIST

#if defined (DA8XXGEM)
/** ============================================================================
 *  @name   dspAddr
 *
 *  @desc   Address of c_int00 in the DSP executable.
 *  ============================================================================
 */
Uint32 MP_LIST_dspAddr ;

/** ============================================================================
 *  @name   shmAddr
 *
 *  @desc   Address of symbol DSPLINK_shmBaseAddres in the DSP executable.
 *  ============================================================================
 */
Uint32 MP_LIST_shmAddr ;

/** ============================================================================
 *  @name   argsAddr
 *
 *  @desc   Address of .args section in the DSP executable.
 *  ============================================================================
 */
Uint32 MP_LIST_argsAddr ;
#endif

/** ============================================================================
 *  @const  SampleGppMpListName
 *
 *  @desc   Name of the MPLIST on the GPP.
 *  ============================================================================
 */
STATIC Char8 SampleGppMpListName [DSP_MAX_STRLEN] = "GPPMPLIST" ;

/** ============================================================================
 *  @name   MP_LIST_MplistHandle
 *
 *  @desc   Handle to the MpList used by the application in writer mode.
 *  ============================================================================
 */
STATIC MPLIST_Handle MP_LIST_MplistHandle = NULL      ;

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
 *  ============================================================================
 */
typedef struct MP_LIST_ListData_tag{
    MPLIST_Header head ;
    Uint32        iter ;
    Uint32        pos ;
    Uint32        value ;
} MP_LIST_ListData ;

/** ============================================================================
 *  @name   MP_LIST_Data
 *
 *  @desc   Pointer to list element data.
 *  ============================================================================
 */
MP_LIST_ListData * MP_LIST_Data [MAX_NUM_ELEMENTS] ;

/** ============================================================================
 *  @const  MP_LIST_IPS_ID
 *
 *  @desc   The IPS ID to be used for sending notification events to the DSP.
 *  ============================================================================
 */
#define MP_LIST_IPS_ID                0

/** ============================================================================
 *  @const  MP_LIST_IPS_EVENTNO
 *
 *  @desc   The IPS event number to be used for sending notification events to
 *          the DSP.
 *  ============================================================================
 */
#define MP_LIST_IPS_EVENTNO           5

/** ============================================================================
 *  @const  MP_LIST_SemPtr
 *
 *  @desc   The semaphore used by the GPP to wait for notification from
 *          the DSP.
 *  ============================================================================
 */
Pvoid        MP_LIST_SemPtr     = NULL ;

/** ============================================================================
 *  @name   LINKCFG_config
 *
 *  @desc   Extern declaration to the default DSP/BIOS LINK configuration
 *          structure.
 *  ============================================================================
 */
extern  LINKCFG_Object LINKCFG_config ;


/** ----------------------------------------------------------------------------
 *  @func   MP_LIST_VerifyData
 *
 *  @desc   This function verifies the data-integrity of given mplist.
 *
 *  @arg    iter
 *             The value of the current iteration in progress.
 *  @arg    pos
 *             The value of the current position of list element in list.
 *  @arg    elem
 *              This is the pointer of the mplist element whose contents are to
 *              be validated.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Contents of the mplist is unexpected.
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
MP_LIST_VerifyData (IN Uint32 iter, IN Uint32 pos, IN MP_LIST_ListData * elem) ;


/** ----------------------------------------------------------------------------
 *  @func   MP_LIST_Notify
 *
 *  @desc   This function implements the notification callback for the MPLIST
 *          opened by the GPP.
 *
 *  @arg    eventNo
 *              Event number used for notification.
 *  @arg    arg
 *              Parameter used while registering the notification.
 *  @arg    info
 *              Parameter used while registering the notification.
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
STATIC
NORMAL_API
Void
MP_LIST_Notify (IN     Uint32 eventNo,
                IN     Pvoid  arg,
                IN     Pvoid  info) ;


/** ============================================================================
 *  @func   MP_LIST_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @modif
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MP_LIST_Create (IN Char8 * dspExecutable,
                IN Char8 * strNumIterations,
                IN Char8 * strNumElem,
                IN Uint8   processorId)
{
    DSP_STATUS    status  = DSP_SOK  ;
    Uint32        numArgs = NUM_ARGS ;
    Uint32        numElem = 0 ;
    Uint32        listElemSize = DSPLINK_ALIGN (sizeof (MP_LIST_ListData),
                                                DSPLINK_BUF_ALIGN) ;
    Char8 *       args [NUM_ARGS] ;
    Uint32        numBufs [NUM_BUF_SIZES] ;
    Uint32        size [NUM_BUF_SIZES] ;
    Uint32        i ;
    SMAPOOL_Attrs poolAttrs ;
    MPLIST_Attrs  mplistAttrs ;
#if defined (DA8XXGEM)
    NOLOADER_ImageInfo  imageInfo ;
#endif


    MP_LIST_0Print ("Entered MP_LIST_Create ()\n") ;

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
            MP_LIST_1Print ("PROC_attach () failed. Status = [0x%x]\n",
                            status) ;
        }
    }
    else {
        MP_LIST_1Print ("PROC_setup () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Open the pool.
     */
    if (DSP_SUCCEEDED (status)) {
         size [0] = sizeof (MPLIST_List) ;

         numElem  = MP_LIST_Atoi (strNumElem) ;
         size [1] = listElemSize ;

         numBufs [0] = NUM_BUF_POOL0 ;
         numBufs [1] = numElem ;

         poolAttrs.bufSizes      = (Uint32 *) &size ;
         poolAttrs.numBuffers    = (Uint32 *) &numBufs ;
         poolAttrs.numBufPools   = NUM_BUF_SIZES ;
         poolAttrs.exactMatchReq = TRUE ;
         status = POOL_open (POOL_makePoolId(processorId, SAMPLE_POOL_ID), &poolAttrs) ;
         if (DSP_FAILED (status)) {
             MP_LIST_1Print ("POOL_open () failed. Status = [0x%x]\n",
                             status) ;
         }
    }

    /*
     *  Create the list and open the handle in current process context.
     */
    if (DSP_SUCCEEDED (status)) {
        mplistAttrs.poolId = POOL_makePoolId(processorId, SAMPLE_POOL_ID) ;
        status = MPLIST_create (processorId,
                                SampleGppMpListName,
                                NULL,
                                &mplistAttrs ) ;
        if (DSP_FAILED (status)) {
            MP_LIST_1Print ("MPLIST_create() failed. Status = [0x%x]\n",
                            status) ;
        }
        else {
            status = MPLIST_open (processorId,
                                  SampleGppMpListName,
                                  &MP_LIST_MplistHandle) ;
            if (DSP_FAILED (status)) {
                MP_LIST_1Print ("MPLIST_open() failed. Status = [0x%x]\n",
                                status) ;
            }
        }
    }

    /*
     *  Set the notification for GPP. When the DSP has
     *  modified the list elements, notification will be
     *  sent. Create the semaphore to be used for
     *  notification.
     */
    if (DSP_SUCCEEDED (status)) {
        status = MP_LIST_CreateSem (&MP_LIST_SemPtr) ;

        if (DSP_SUCCEEDED(status)) {
            /* Set the notifier for list created by the GPP. */
            status = NOTIFY_register (processorId,
                                      MP_LIST_IPS_ID,
                                      MP_LIST_IPS_EVENTNO,
                                      (FnNotifyCbck) MP_LIST_Notify,
                                      MP_LIST_SemPtr) ;
            if (DSP_FAILED (status)) {
                MP_LIST_1Print ("NOTIFY_register() failed. Status = [0x%x]\n",
                                status) ;
            }
        }
        else {
            MP_LIST_1Print ("MPLIST_CreateSem() failed. Status = [0x%x]\n",
                            status) ;
        }
    }

    if (DSP_SUCCEEDED(status)) {
        for (i = 0; (i < numElem) && (DSP_SUCCEEDED (status)); i++)  {
            status = POOL_alloc (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                                 (Pvoid *) &MP_LIST_Data [i],
                                 (listElemSize)) ;
            if (DSP_FAILED (status)) {
                MP_LIST_1Print ("POOL_alloc() failed. \
                                 Status = [0x%x]\n", status) ;
            }
        }
    }

    /*
     *  Load the executable on the DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        args [0] = strNumIterations ;
        args [1] = strNumElem ;
#if defined (DA8XXGEM)
    if (LINKCFG_config.dspConfigs [processorId]->dspObject->doDspCtrl ==
                     DSP_BootMode_NoBoot) {
        imageInfo.dspRunAddr  = MP_LIST_dspAddr ;
        imageInfo.shmBaseAddr = MP_LIST_shmAddr ;
        imageInfo.argsAddr    = MP_LIST_argsAddr ;
        imageInfo.argsSize    = 0x50         ;

        status = PROC_load (processorId, (Char8 *) &imageInfo, numArgs, args) ;
    }
    else
#endif
        {
            status = PROC_load (processorId, dspExecutable, numArgs, args) ;
        }

        if (DSP_FAILED (status)) {
            MP_LIST_1Print ("PROC_load () failed. Status = [0x%x]\n", status) ;
        }
    }

    /*
     *  Start execution on DSP.
     */
    if (DSP_SUCCEEDED (status)) {
        status = PROC_start (processorId) ;
        if (DSP_FAILED (status)) {
            MP_LIST_1Print ("PROC_start () failed. Status = [0x%x]\n",
                            status) ;
        }
    }

    /*
     *  Wait for the DSP-side application to indicate that it has
     *  completed its setup. The DSP-side application sends notification
     *  of the IPS event when it is ready to proceed with further
     *  execution of the application.
     */
    if (DSP_SUCCEEDED (status)) {
        status = MP_LIST_WaitSem (MP_LIST_SemPtr) ;
        if (DSP_FAILED (status)) {
            MP_LIST_1Print ("MP_LIST_WaitSem () failed. Status = [0x%x]\n",
                             status) ;
        }
    }

    MP_LIST_0Print ("Leaving MP_LIST_Create ()\n") ;

    return status ;
}


/** ============================================================================
 *  @func   MP_LIST_Execute
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MP_LIST_Execute (IN Uint32 numIterations,
                 IN Uint32 numElem,
                 IN Uint8  processorId)
{
    DSP_STATUS         status         = DSP_SOK ;
    MP_LIST_ListData * currentElement = NULL ;
    Uint32             i ;
    Uint32             j ;

    MP_LIST_0Print ("Entered MP_LIST_Execute ()\n") ;

    if (DSP_SUCCEEDED (status)) {
        for (i = 1 ; ((i <= numIterations)) && (DSP_SUCCEEDED (status)) ; i++) {

            /* Add elements to list. The number of elements to be added
             * is decided by the user
             */
            for (j = 0 ; ((j < numElem)) && (DSP_SUCCEEDED (status)) ; j++) {
                /* Initialize the list elements with the iteration
                 * number and the position. This will be passed to the
                 * DSP. The DSP will modify the calue as a function of
                 * the iteration number and the position.
                 */
                MP_LIST_Data[j]->iter   = i ;
                MP_LIST_Data[j]->pos    = j ;
                MP_LIST_Data[j]->value  = 0 ;
                POOL_writeback (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                                MP_LIST_Data [j],
                                sizeof (MP_LIST_ListData)) ;

                 /* Add element to list */
                 status = MPLIST_putTail (MP_LIST_MplistHandle,
                                          (MPLIST_Elem) MP_LIST_Data [j]);
                 if (DSP_FAILED (status)) {
                     MP_LIST_1Print ("MPLIST_putTail () failed. \
                                      Status = [0x%x]\n",
                                      status) ;
                 }
            }


            if (DSP_SUCCEEDED (status)) {
                /* The GPP side sends a notification to the DSP side telling
                 * that the elements have been added to the list.
                 */
                status = NOTIFY_notify (processorId,
                                        MP_LIST_IPS_ID,
                                        MP_LIST_IPS_EVENTNO,
                                        (Uint32 ) 0) ;

                if (DSP_FAILED (status)) {
                    MP_LIST_1Print ("NOTIFY_notify () failed."
                                     " Status = [0x%x]\n",
                                     status) ;
                }

                /* Wait for notification that the DSP has modified
                 *  the list. After the DSP modifies the list , it sends
                 *  a notification back to the GPP.
                 */
                if (DSP_SUCCEEDED (status)) {
                    status = MP_LIST_WaitSem (MP_LIST_SemPtr) ;
                    if (DSP_FAILED (status)) {
                        MP_LIST_1Print ("MP_LIST_WaitSem () SEM failed "
                                        "Status = [0x%x]\n",
                                        status) ;
                    }
                }

                for (j = 0 ;
                     ((j < numElem)) && (DSP_SUCCEEDED (status)) ;
                     j++) {
                    /*
                     *  Verify correctness of data received.
                     */
                    status = MPLIST_getHead (MP_LIST_MplistHandle,
                                             (MPLIST_Elem *) &currentElement) ;
                    if (DSP_SUCCEEDED(status)) {
                        POOL_invalidate (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                                         currentElement,
                                         sizeof (MP_LIST_ListData)) ;
                        status = MP_LIST_VerifyData (i, j, currentElement) ;
                        if (DSP_FAILED (status)) {
                            MP_LIST_1Print ("Data verification failed."
                                            " Status = [0x%x]\n",
                                            status) ;
                        }
                    }
                    else {
                        MP_LIST_1Print ("MPLIST_getHead failed."
                                        " Status = [0x%x]\n",
                                        status) ;
                    }
                }

                if (DSP_SUCCEEDED (status) && ((i % 100) == 0)) {
                    MP_LIST_1Print ("Verified %ld list based transfer\n",
                                    i) ;
                }
            }
        }
    }

    MP_LIST_0Print ("Leaving MP_LIST_Execute ()\n") ;

    return status ;
}


/** ============================================================================
 *  @func   MP_LIST_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          MP_LIST_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MP_LIST_Delete (IN Uint32 numElem, Uint8 processorId)
{
    DSP_STATUS status       = DSP_SOK ;
    DSP_STATUS tmpStatus    = DSP_SOK ;
    Uint32     listElemSize = DSPLINK_ALIGN (sizeof (MP_LIST_ListData),
                                             DSPLINK_BUF_ALIGN) ;
    Uint32     i ;

    MP_LIST_0Print ("Entered MP_LIST_Delete ()\n") ;

    /*
     *  Stop execution on DSP.
     */
    tmpStatus = PROC_stop (processorId) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MP_LIST_1Print ("PROC_stop () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Unregister notification for the event callback
     */
    tmpStatus = NOTIFY_unregister (processorId,
                                   MP_LIST_IPS_ID,
                                   MP_LIST_IPS_EVENTNO,
                                   (FnNotifyCbck) MP_LIST_Notify,
                                   MP_LIST_SemPtr) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MP_LIST_1Print ("NOTIFY_unregister () failed. Status = [0x%x]\n",
                        status) ;
    }

    /* Delete the semaphores used for notification */
    if (MP_LIST_SemPtr != NULL) {
        tmpStatus = MP_LIST_DeleteSem (MP_LIST_SemPtr) ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            MP_LIST_1Print ("MP_LIST_DeleteSem () failed "
                            "Status = [0x%x]\n",
                            status) ;
        }
    }

    /* Free the memory allocated for the list elements */
    for (i = 0; i < numElem; i++) {
        if (MP_LIST_Data [i] != NULL) {
            tmpStatus = POOL_free (POOL_makePoolId(processorId, SAMPLE_POOL_ID),
                                   (Pvoid ) MP_LIST_Data [i],
                                   listElemSize) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                MP_LIST_1Print ("POOL_Free () failed. \
                                Status = [0x%x]\n", status) ;
            }
        }
    }

    /*
     *  Close the mplist handle
     */
    if (MP_LIST_MplistHandle != NULL ) {
        tmpStatus = MPLIST_close (MP_LIST_MplistHandle) ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            MP_LIST_1Print ("MPLIST_close() failed. \
                            Status = [0x%x]\n", status) ;
        }
    }

    /*
     *  Delete the mplist
     */
    tmpStatus = MPLIST_delete (processorId, SampleGppMpListName) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MP_LIST_1Print ("MPLIST_Delete() failed. Status = [0x%x]\n",
                        status) ;
    }

    /*
     *  Close the pool
     */
    tmpStatus = POOL_close (POOL_makePoolId(processorId, SAMPLE_POOL_ID)) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MP_LIST_1Print ("POOL_close () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Detach from the processor
     */
    tmpStatus = PROC_detach (processorId) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MP_LIST_1Print ("PROC_detach () failed. Status = [0x%x]\n", status) ;
    }

    /*
     *  Destroy the PROC object.
     */
    tmpStatus = PROC_destroy () ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        MP_LIST_1Print ("PROC_destroy () failed. Status = [0x%x]\n", status) ;
    }

    MP_LIST_0Print ("Leaving MP_LIST_Delete ()\n") ;
}


/** ============================================================================
 *  @func   MP_LIST_Main
 *
 *  @desc   Entry point for the application
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MP_LIST_Main (IN Char8 * dspExecutable,
              IN Char8 * strNumIterations,
              IN Char8 * strNumElem,
              IN Char8 * strProcessorId)
{
    DSP_STATUS status        = DSP_SOK ;
    Uint32     numIterations = 0       ;
    Uint32     numElem       = 0       ;
    Uint8      processorId   = 0       ;

    MP_LIST_0Print ("========== Sample Application : MP_LIST ==========\n") ;

    if (   (dspExecutable    != NULL)
        && (strNumIterations != NULL)
        && (strNumElem       != NULL)) {
        numIterations = MP_LIST_Atoi (strNumIterations) ;
        numElem       = MP_LIST_Atoi (strNumElem) ;
        processorId   = MP_LIST_Atoi (strProcessorId) ;

        if (    (numIterations <= 0xFFFF)
            &&  (numElem       >  0)
            &&  (numElem       <= MAX_NUM_ELEMENTS)) {

            if (processorId >= MAX_DSPS) {
                MP_LIST_1Print ("==Error: Invalid processor id %d  specified"
                                " ==\n", processorId) ;
                status = DSP_EINVALIDARG ;
            }

            if (DSP_SUCCEEDED (status)) {
                /*
                 *  Specify the dsp executable file name for mplist creation phase.
                 */
                status = MP_LIST_Create (dspExecutable,
                                         strNumIterations,
                                         strNumElem,
                                         processorId) ;

                /*
                 *  Execute the mplist execute phase.
                 */
                if (DSP_SUCCEEDED (status)) {
                    status = MP_LIST_Execute (numIterations,
                                              numElem,
                                              processorId) ;
                }

                /*
                 *  Perform cleanup operation.
                 */
                MP_LIST_Delete (numElem, processorId) ;
            }
        }
        else {
            status = DSP_EINVALIDARG ;
            MP_LIST_1Print ("ERROR! Invalid arguments specified for  "
                            "mplist application.\n"
                            "     Max iterations = %d\n",
                            0xFFFF) ;
            MP_LIST_1Print ("     Max number of elements that can be added into"
                            " the list   = %d\n",
                            MAX_NUM_ELEMENTS) ;
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        MP_LIST_0Print ("ERROR! Invalid arguments specified for  "
                        "mplist application\n") ;
    }

    MP_LIST_0Print ("====================================================\n") ;
}

/** ----------------------------------------------------------------------------
 *  @func   MP_LIST_VerifyData
 *
 *  @desc   This function verifies the data-integrity of given buffer.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
MP_LIST_VerifyData (IN Uint32 iter, IN Uint32 pos, IN MP_LIST_ListData * elem)
{
    DSP_STATUS status = DSP_SOK ;

    if (elem != NULL) {
        /*
         *  Verify the mplist
         */
        if ((elem->iter != iter) || (elem->pos != pos)) {
            /* Verify that the element is received in the expected order. */
            status = DSP_EFAIL ;
        }
        else if ((elem->value) != ((elem->iter) *  (elem->pos))) {
            /* Verify that the DSP has processed the element correctly. */
            status = DSP_EFAIL ;
        }
    }
    else {
        MP_LIST_0Print ("Null element received in MP_LIST_VerifyData ()\n") ;
    }

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   MP_LIST_Notify
 *
 *  @desc   This function implements the notification callback for the MPLIST
 *          opened by the GPP.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Void
MP_LIST_Notify (IN     Uint32 eventNo,
                IN     Pvoid  arg,
                IN     Pvoid  info)
{
    DSP_STATUS status = DSP_SOK ;

    /* Post the semaphore. */
    status = MP_LIST_PostSem ((Pvoid) arg) ;
    if (DSP_FAILED (status)) {
        MP_LIST_1Print ("MP_LIST_PostSem () failed. Status = [0x%x]\n",
                        status) ;
    }
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

