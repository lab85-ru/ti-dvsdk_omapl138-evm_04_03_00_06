/** ============================================================================
 *  @file   ringio.c
 *
 *  @path   $(DSPLINK)/dsp/src/ringio/
 *
 *  @desc   Implementation of API sub-component RING IO.
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
#include <mem.h>
#include <sem.h>
#include <sys.h>
#include <string.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <platform.h>
#include <failure.h>
#include <dbc.h>
#include <hal_cache.h>
#include <mpcs.h>
#include <_mpcs.h>
#include <notify.h>
#include <ringio.h>
#include <_ringio.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define FILEID                FID_RINGIO_C


/** ============================================================================
 *  @name   RingIO_ctrlPtr
 *
 *  @desc   Pointer to RingIO Control region.
 *  ============================================================================
 */
RingIO_Ctrl * RingIO_ctrlPtr = NULL ;

/** ============================================================================
 *  @name   RingIO_mpcsObj
 *
 *  @desc   Handle to global MPCS object for protecting the RingIO region.
 *  ============================================================================
 */
MPCS_Handle RingIO_mpcsObj    = NULL ;


/** ----------------------------------------------------------------------------
 *  @func   RingIO_getControlHandle
 *
 *  @desc   This function gets the control structure handle corresponding to an
 *          RingIO ID.
 *
 *  @arg    id
 *              RingIO ID.
 *
 *  @ret    Handle to the control stucture
 *              Operation successfully completed.
 *          NULL
 *              Entry not found.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ----------------------------------------------------------------------------
 */
static
RingIO_ControlStruct *
RingIO_getControlHandle (IN  Char * name) ;


/** ----------------------------------------------------------------------------
 *  @func   RingIO_notifyCallback
 *
 *  @desc   Callback function registered for event notification from remote
 *          processor.
 *
 *  @arg    eventNo
 *              Event number associated with the callback being invoked.
 *  @arg    arg
 *              Callback argument received. The notification component invokes
 *              this callback with the argument as specified during
 *              registration
 *  @arg    info
 *              Optional payload sent by the remote processor.
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
Void
RingIO_notifyCallback (IN  Uint32 eventNo, IN  Void * arg, IN  Void * info) ;


/** ============================================================================
 *  @func   RingIO_create
 *
 *  @desc   This function creates a RingIO Channel.
 *          This function creates a RingIO Channel in Shared memory using the
 *          creation params specified. The control parameters need not specify
 *          the data buffer and attribute buffer pointers during creation.
 *          But note that before opening the channel for reading or writing both
 *          the buffer pointers should be set.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
#if defined (DSPLINK_LEGACY_SUPPORT)
RingIO_create (IN  Char * name, IN  RingIO_Attrs * attrs)
#else
RingIO_create (IN Uint32 procId, IN  Char * name, IN  RingIO_Attrs * attrs)
#endif /* if defined (DSPLINK_LEGACY_SUPPORT) */
{
    Int                  status           = RINGIO_SUCCESS ;
    Int                  tmpStatus        = RINGIO_SUCCESS ;
    RingIO_ControlHandle ringIoCtrl       = NULL ;
    RingIO_BufPtr        ringIoDataBufPtr = NULL ;
    RingIO_BufPtr        ringIoAttrBufPtr = NULL ;
    MPCS_ShObj *         ringIoMpcsObj    = NULL ;
    Int32                slot             = -1   ;
    Uint32               i ;
    MPCS_Attrs           mpcsAttrs ;

    DBC_require (name  != NULL) ;
    DBC_require (attrs != NULL) ;

#if (!defined(DSPLINK_LEGACY_SUPPORT))
    (Void) procId ;
#endif /* if defined (DSPLINK_LEGACY_SUPPORT) */

    if ((name == NULL) || (attrs == NULL)) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else if (   (    (attrs->transportType != RINGIO_TRANSPORT_DSP_DSP)
                 &&  (attrs->transportType != RINGIO_TRANSPORT_GPP_DSP))
             || (attrs->dataBufSize == 0)) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        /* Check if the name already exists in the array and get a free slot
         * in the array if the existing matching name is not found.
         */
        status = MPCS_enter (RingIO_mpcsObj) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {
            for (i = 0 ;
                 (i < RingIO_ctrlPtr->maxEntries) && (status == SYS_OK) ;
                 i++) {
                HAL_cacheInv ((Ptr) &(RingIO_ctrlPtr->dspAddrEntry [i]),
                              sizeof (RingIO_Entry)) ;
                if (strncmp (name,
                             RingIO_ctrlPtr->dspAddrEntry [i].name,
                             RINGIO_NAME_MAX_LEN) == 0) {
                    status = RINGIO_EALREADYEXISTS ;
                    SET_FAILURE_REASON (status) ;
                }

                if (   (slot == -1)
                    && (RingIO_ctrlPtr->dspAddrEntry [i].phyControl ==  NULL)) {
                    /* Reserve the found slot. */
                    RingIO_ctrlPtr->dspAddrEntry [i].phyControl = (Void *) 1 ;
                    HAL_cacheWbInv ((Ptr) &(RingIO_ctrlPtr->dspAddrEntry [i]),
                                    sizeof (RingIO_Entry)) ;
                    slot = i ;
                }
            }

            /* Existing matching name was not found, but all slots are full. */
            if ((status == SYS_OK) && (slot == -1)) {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON (status) ;
            }
            tmpStatus = MPCS_leave (RingIO_mpcsObj) ;
            if ((status == SYS_OK) && (tmpStatus != SYS_OK)) {
                status = tmpStatus ;
                SET_FAILURE_REASON (status) ;
            }

            if (status == SYS_OK) {
                /* Allocate memory for control structure */
                status = POOL_alloc (attrs->ctrlPoolId,
                                     (Ptr *) &ringIoCtrl,
                                     sizeof (RingIO_ControlStruct)) ;

                /* Allocate memory for Data buffer. */
                if (status == SYS_OK) {
                    status = POOL_alloc (attrs->dataPoolId,
                                         (Ptr *) &ringIoDataBufPtr,
                                         (attrs->dataBufSize + attrs->footBufSize)) ;
                    /* Allocate memory for Attribute buffer, if required. */
                    if ((status == SYS_OK) && (attrs->attrBufSize != 0)) {
                        status = POOL_alloc (attrs->attrPoolId,
                                             (Ptr *) &ringIoAttrBufPtr,
                                             attrs->attrBufSize) ;
                    }

                    /* Allocate memory for lock structure. */
                    if (status == SYS_OK) {
                        /* The size for the MPCS object is same for both GPP-DSP
                         * as well as DSP-DSP (semaphore object) to ensure that
                         * the padding size is unchanged.
                         */
                        status = POOL_alloc (attrs->lockPoolId,
                                             (Ptr *) &ringIoMpcsObj,
                                             sizeof (MPCS_ShObj)) ;
                        if (status == SYS_OK) {
                            if (attrs->transportType == RINGIO_TRANSPORT_GPP_DSP) {
                                mpcsAttrs.poolId = attrs->lockPoolId ;
                                /* Use the RingIO name as the MPCS name. */
                                status = MPCS_create (RingIO_ctrlPtr->dspId,
                                                      name,
                                                      ringIoMpcsObj,
                                                      &mpcsAttrs) ;
                            }
                            else if (   attrs->transportType
                                     == RINGIO_TRANSPORT_DSP_DSP) {
                                SEM_new ((SEM_Handle) ringIoMpcsObj, 1) ;
                            }
                        }
                    }
                }

                if (status == SYS_OK) {
                    status = MPCS_enter (RingIO_mpcsObj) ;
                    if (status != SYS_OK) {
                        SET_FAILURE_REASON (status) ;
                    }
                    else {
                        /* The virtControl field will be set by the GPP-side. */
                        RingIO_ctrlPtr->dspAddrEntry [slot].phyControl  =
                                                                    ringIoCtrl ;
                        RingIO_ctrlPtr->dspAddrEntry [slot].virtControl = NULL ;

                        ringIoCtrl->procId          = GBL_getProcId () ;
                        /* Initialize the control structure */
                        ringIoCtrl->entryId         = slot ;
                        ringIoCtrl->transportType   = (Uint32) attrs->transportType ;

                        ringIoCtrl->phyLockHandle   = ringIoMpcsObj ;

                        ringIoCtrl->phyBufStart     = ringIoDataBufPtr ;
                        ringIoCtrl->phyBufEnd       =  attrs->dataBufSize
                            + attrs->footBufSize
                            - 1 ;

                        ringIoCtrl->curBufEnd       = attrs->dataBufSize - 1 ;
                        ringIoCtrl->dataBufEnd      = attrs->dataBufSize - 1 ;
                        ringIoCtrl->dataBufSize     = attrs->dataBufSize ;
                        ringIoCtrl->footBufSize     = attrs->footBufSize ;

                        ringIoCtrl->validSize       = 0 ;
                        ringIoCtrl->emptySize       = attrs->dataBufSize ;

                        ringIoCtrl->phyAttrStart    = ringIoAttrBufPtr ;
                        if (attrs->attrBufSize != 0) {
                            ringIoCtrl->phyAttrBufEnd = attrs->attrBufSize - 1 ;
                        }
                        else {
                            ringIoCtrl->phyAttrBufEnd = (Uint32) -1l ;
                        }
                        ringIoCtrl->curAttrBufEnd   =
                                               ringIoCtrl->phyAttrBufEnd ;

                        ringIoCtrl->validAttrSize   = 0 ;
                        ringIoCtrl->emptyAttrSize   = attrs->attrBufSize ;

                        ringIoCtrl->prevAttrOffset  = RINGIO_INVALID_OFFSET ;

                        /* Make sure both reader and writer are uninitialized */
                        ringIoCtrl->reader.isValid  = FALSE ;
                        ringIoCtrl->reader.refCount = 0 ;
                        ringIoCtrl->writer.isValid  = FALSE ;
                        ringIoCtrl->writer.refCount = 0 ;
                        ringIoCtrl->reader.openMode = (Uint32) RINGIO_MODE_READ ;
                        ringIoCtrl->writer.openMode = (Uint32) RINGIO_MODE_WRITE ;

                        RingIO_ctrlPtr->dspAddrEntry [slot].ctrlPoolId  =
                            attrs->ctrlPoolId ;
                        RingIO_ctrlPtr->dspAddrEntry [slot].dataPoolId  =
                            attrs->dataPoolId ;
                        RingIO_ctrlPtr->dspAddrEntry [slot].attrPoolId  =
                            attrs->attrPoolId ;
                        RingIO_ctrlPtr->dspAddrEntry [slot].lockPoolId  =
                            attrs->lockPoolId ;
                        RingIO_ctrlPtr->dspAddrEntry [slot].ownerProcId =
                            GBL_getProcId () ;

                        strncpy (RingIO_ctrlPtr->dspAddrEntry [slot].name,
                                name,
                                RINGIO_NAME_MAX_LEN) ;

                        HAL_cacheWbInv
                              ((Ptr)
                               (RingIO_ctrlPtr->dspAddrEntry [slot].phyControl),
                               sizeof (RingIO_ControlStruct)) ;
                        HAL_cacheWbInv
                              ((Ptr) &(RingIO_ctrlPtr->dspAddrEntry [slot]),
                               sizeof (RingIO_Entry)) ;

                        tmpStatus = MPCS_leave (RingIO_mpcsObj) ;
                        if ((status == SYS_OK) && (tmpStatus != SYS_OK)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON (status) ;
                        }
                    }
                    if (status != SYS_OK) {
                        /* Reset the slot to empty so that it
                         * can be used later.
                         */
                        RingIO_ctrlPtr->dspAddrEntry [slot].phyControl = NULL ;

                        if (ringIoCtrl != NULL) {
                            POOL_free (attrs->ctrlPoolId,
                                       (Ptr) ringIoCtrl,
                                       sizeof (RingIO_ControlStruct)) ;
                        }

                        if (ringIoDataBufPtr != NULL) {
                            POOL_free (attrs->dataPoolId,
                                       (Ptr) ringIoDataBufPtr,
                                       (  attrs->dataBufSize
                                        + attrs->footBufSize)) ;
                        }

                        if (ringIoAttrBufPtr != NULL) {
                            POOL_free (attrs->attrPoolId,
                                       (Ptr) ringIoAttrBufPtr,
                                       attrs->attrBufSize) ;
                        }

                        if (ringIoMpcsObj != NULL) {
                            POOL_free (attrs->lockPoolId,
                                      (Ptr) ringIoMpcsObj,
                                      sizeof (MPCS_ShObj)) ;
                        }

                        HAL_cacheWbInv
                                ((Ptr) &(RingIO_ctrlPtr->dspAddrEntry [slot]),
                                 sizeof (RingIO_Entry)) ;
                    }
                }
            }
        }
    }

    if (status != SYS_OK) {
        if (slot != -1) {
            /* Reset the slot to empty so that it can be used later. */
            RingIO_ctrlPtr->dspAddrEntry [slot].phyControl = NULL ;

            if (ringIoCtrl != NULL) {
                POOL_free (attrs->ctrlPoolId,
                           (Ptr) ringIoCtrl,
                           sizeof (RingIO_ControlStruct)) ;
            }

            if (ringIoDataBufPtr != NULL) {
                POOL_free (attrs->dataPoolId,
                           (Ptr) ringIoDataBufPtr,
                           (  attrs->dataBufSize
                              + attrs->footBufSize)) ;
            }

            if (ringIoAttrBufPtr != NULL) {
                POOL_free (attrs->attrPoolId,
                           (Ptr) ringIoAttrBufPtr,
                           attrs->attrBufSize) ;
            }

            if (ringIoMpcsObj != NULL) {
                POOL_free (attrs->lockPoolId,
                           (Ptr) ringIoMpcsObj,
                           sizeof (MPCS_ShObj)) ;
            }

            HAL_cacheWbInv
                   ((Ptr) &(RingIO_ctrlPtr->dspAddrEntry [slot]),
                    sizeof (RingIO_Entry)) ;
        }
    }

    return MAP_STATUS (status) ;
}


/** ============================================================================
 *  @func   RingIO_delete
 *
 *  @desc   This function deletes a RingIO channel.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
#if defined (DSPLINK_LEGACY_SUPPORT)
RingIO_delete (IN  Char * name)
#else
RingIO_delete (IN Uint32 procId, IN  Char * name)
#endif /* if defined (DSPLINK_LEGACY_SUPPORT) */
{
    Int                  status    = RINGIO_SUCCESS ;
    Int                  tmpStatus = RINGIO_SUCCESS ;
    RingIO_ControlHandle control ;
    Uint32  i ;

    DBC_require (name != NULL) ;

#if (!defined (DSPLINK_LEGACY_SUPPORT))
    (Void) procId ;
#endif /* if defined (DSPLINK_LEGACY_SUPPORT) */

    if (name == NULL) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        tmpStatus = MPCS_enter (RingIO_mpcsObj) ;
        DBC_assert (tmpStatus == SYS_OK) ;
        if (tmpStatus != SYS_OK) {
            SET_FAILURE_REASON (tmpStatus) ;
        }

        for (i = 0 ; i < RingIO_ctrlPtr->maxEntries ; i++) {
            HAL_cacheInv ((Ptr) &(RingIO_ctrlPtr->dspAddrEntry [i]),
                          sizeof (RingIO_Entry)) ;

            /* Check if the entry is valid. */
            if (    (RingIO_ctrlPtr->dspAddrEntry [i].phyControl != NULL)
                &&  (   RingIO_ctrlPtr->dspAddrEntry [i].phyControl
                     != (Void *) 1)) {
                if (strncmp (name, RingIO_ctrlPtr->dspAddrEntry [i].name,
                            RINGIO_NAME_MAX_LEN) == 0) {
                    break ;
                }
            }
        }

        if (i != RingIO_ctrlPtr->maxEntries) {
            control = RingIO_ctrlPtr->dspAddrEntry [i].phyControl ;
            DBC_assert (control != NULL) ;

            HAL_cacheInv ((Ptr) (control),sizeof (RingIO_ControlStruct)) ;

            if (!control->reader.isValid && !control->writer.isValid) {
                RingIO_ctrlPtr->dspAddrEntry [i].phyControl = (Void *) 1 ;
                /* Delete the created MPCS object. */
                if (control->transportType == (Uint32) RINGIO_TRANSPORT_GPP_DSP) {
                    status = MPCS_delete (RingIO_ctrlPtr->dspId, name) ;
                    DBC_assert (status == SYS_OK) ;
                    if (status != SYS_OK) {
                        SET_FAILURE_REASON (status) ;
                    }
                }

                if (control->phyLockHandle != NULL) {
                    POOL_free (RingIO_ctrlPtr->dspAddrEntry [i].lockPoolId,
                               (Ptr) control->phyLockHandle,
                               sizeof (MPCS_ShObj)) ;
                }

                if (control->phyAttrStart != NULL) {
                    POOL_free (RingIO_ctrlPtr->dspAddrEntry [i].attrPoolId,
                               (Ptr) control->phyAttrStart,
                               control->phyAttrBufEnd + 1) ;
                }

                if (control->phyBufStart != NULL) {
                    POOL_free (RingIO_ctrlPtr->dspAddrEntry [i].dataPoolId,
                               (Ptr) control->phyBufStart,
                               control->phyBufEnd + 1) ;
                }

                /* Free the control structure. */
                POOL_free (RingIO_ctrlPtr->dspAddrEntry [i].ctrlPoolId,
                           (Ptr) control,
                           sizeof (RingIO_ControlStruct)) ;

                memset (&RingIO_ctrlPtr->dspAddrEntry [i],
                        0,
                        sizeof (RingIO_Entry)) ;

                HAL_cacheWbInv ((Ptr) (control),
                                sizeof (RingIO_ControlStruct)) ;
                HAL_cacheWbInv ((Ptr) &(RingIO_ctrlPtr->dspAddrEntry [i]),
                                sizeof (RingIO_Entry)) ;
            }
            else {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON (status) ;
            }
        }
        else {
            status = RINGIO_EFAILURE ;
            SET_FAILURE_REASON (status) ;
        }

        tmpStatus = MPCS_leave (RingIO_mpcsObj) ;
        DBC_assert (tmpStatus == SYS_OK) ;
        if ((status == SYS_OK) && (tmpStatus != SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }

    }

    return MAP_STATUS (status) ;
}


/** ============================================================================
 *  @func   RingIO_open
 *
 *  @desc   This function opens a RingIO Channel.
 *          This function is used to open an RingIO Channel either for reading
 *          or writing. Only one reader and one writer can be opened on a RingIO
 *          Channel.
 *
 *  @modif  None.
 *  ============================================================================
 */
RingIO_Handle
RingIO_open (IN  Char * name, IN  RingIO_OpenMode openMode, IN  Uint32 flags)
{
    Int                  status      = RINGIO_SUCCESS ;
    Int                  tmpStatus   = RINGIO_SUCCESS ;
    RingIO_ControlHandle control     = NULL ;
    RingIO_Client *      client      = NULL ;
    RingIO_Client *      otherClient = NULL ;
    RingIO_Handle        handle      = NULL ;
    Uint32               procId      = GBL_getProcId () ;

    DBC_require (name != NULL) ;
    DBC_require (   (openMode == RINGIO_MODE_READ)
                 || (openMode == RINGIO_MODE_WRITE)) ;

    if (    (name != NULL)
        &&  (   (openMode == RINGIO_MODE_READ)
             || (openMode == RINGIO_MODE_WRITE))) {
        status = MPCS_enter (RingIO_mpcsObj) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {
            /* Make sure the id specified has an associated control struct */
            control = RingIO_getControlHandle (name) ;

            if ((control != NULL) && (status == SYS_OK)) {
                if (flags & RINGIO_CONTROL_CACHEUSE) {
                    HAL_cacheInv ((Ptr) (control),
                                  sizeof (RingIO_ControlStruct)) ;
                }

                /* Get the client handle */
                if (openMode == (Uint32) RINGIO_MODE_WRITE) {
                    client      = &control->writer ;
                    otherClient = &control->reader ;
                }
                else {
                    client      = &control->reader ;
                    otherClient = &control->writer ;
                }

                /* Check if the RingIO has already been opened in this
                 * mode.
                 */
                if (client->isValid) {
                    status = RINGIO_EFAILURE ;
                    SET_FAILURE_REASON (status) ;
                }
                /* Check if the open call is supported by the transport
                 * mode used for creating this RingIO.
                 */
                else if (  (control->transportType
                            == (Uint32) RINGIO_TRANSPORT_GPP_DSP)
                        && (otherClient->isValid)
                        && (otherClient->procId    == procId)) {
                    status = RINGIO_EFAILURE ;
                    SET_FAILURE_REASON (status) ;
                }
                else {
                    /* Open the MPCS object to get the virtual
                     * address of lock.
                     */
                    if (   control->transportType
                        == (Uint32) RINGIO_TRANSPORT_GPP_DSP) {
                        status = MPCS_open
                              (RingIO_ctrlPtr->dspId,
                              name,
                              (MPCS_Handle *) (&client->virtLockHandle)) ;
                    }
                    else {
                        client->virtLockHandle = control->phyLockHandle ;
                    }

                    if (status == SYS_OK) {
                        client->virtControlHandle = control ;
                        status = _RingIO_acquireLock (client) ;
                        if (status != SYS_OK) {
                            MPCS_close (RingIO_ctrlPtr->dspId,
                                    (MPCS_Handle) client->virtLockHandle) ;
                            SET_FAILURE_REASON (status) ;
                        }
                        else {
                            /* Now initialize the client */
                            client->isValid          = TRUE ;
                            client->procId           = procId ;
                            client->pDataStart       =
                                control->phyBufStart ;
                            client->pAttrStart       =
                                control->phyAttrStart ;

                            if (client->refCount == 0) {
                                client->acqStart         = 0 ;
                                client->acqSize          = 0 ;

                                client->acqAttrStart     = 0 ;
                                client->acqAttrSize      = 0 ;
                            }

                            client->notifyFunc       = NULL ;
                            client->notifyParam      = NULL ;
                            client->notifyType       =
                                RINGIO_NOTIFICATION_NONE ;
                            client->notifyWaterMark  = 0 ;
                            client->notifyFlag       = FALSE ;

                            client->flags            = flags ;
                            client->refCount         = 1 ;

                            handle = client ;

                            if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                                HAL_cacheWbInv ((Ptr) (client),
                                                sizeof (RingIO_Client)) ;
                            }

                            tmpStatus = _RingIO_releaseLock (client) ;
                            if (   (tmpStatus != SYS_OK)
                                && (status == SYS_OK)) {
                                status = tmpStatus ;
                                SET_FAILURE_REASON (status) ;
                            }
                        }
                    }
                    else {
                        SET_FAILURE_REASON (status) ;
                    }
                }
            }

            tmpStatus = MPCS_leave (RingIO_mpcsObj) ;
            if (   (tmpStatus != SYS_OK)
                && (status == SYS_OK)) {
                status = tmpStatus ;
                SET_FAILURE_REASON (status) ;
            }
        }
    }

    return handle ;
}


/** ============================================================================
 *  @func   RingIO_close
 *
 *  @desc   This function closes a RingIO instance.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
RingIO_close (IN  RingIO_Handle handle)
{
    Int                  status      = RINGIO_SUCCESS ;
    Int                  tmpStatus   = RINGIO_SUCCESS ;
    RingIO_Client *      client      = (RingIO_Client*) handle ;
    RingIO_NotifyFunc    notifyFunc  = NULL ;
    RingIO_Client *      otherClient = NULL ;
    RingIO_ControlHandle control ;

    DBC_require (handle != NULL) ;

    if (handle == NULL) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
            HAL_cacheInv ((Ptr) (client), sizeof (RingIO_Client)) ;
        }
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;

        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
            HAL_cacheInv ((Ptr) (control), sizeof (RingIO_ControlStruct)) ;
        }

        /*  Make sure that no data buffers or attribute buffers are acquired
         *  before allowing the client to close
         */
        if ((!client->isValid) || (client->acqSize) || (client->acqAttrSize)) {
            status = RINGIO_EFAILURE ;
        }
        else {
            /* Acquire Global RingIO lock  */
            status = MPCS_enter (RingIO_mpcsObj) ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
            else {
                status = _RingIO_acquireLock (client) ;
                if (status != SYS_OK) {
                    SET_FAILURE_REASON (status) ;
                }
                else {
                    client->isValid = FALSE ;

                    if (control->transportType == (Uint32) RINGIO_TRANSPORT_GPP_DSP) {
                        if (IS_READER(client)) {
                            otherClient = &(control->writer) ;
                        }
                        else {
                            otherClient = &(control->reader) ;
                        }

                        notifyFunc                = client->notifyFunc ;

                        client->notifyFunc        = NULL ;
                        client->notifyParam       = NULL ;
                        client->notifyType        = RINGIO_NOTIFICATION_NONE ;
                        client->notifyWaterMark   = 0 ;
                        client->notifyFlag        = FALSE ;

                        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                            HAL_cacheWbInv ((Ptr) (client), sizeof (RingIO_Client)) ;
                        }

                        tmpStatus = _RingIO_releaseLock (client) ;
                        if ((tmpStatus != SYS_OK) && (status == SYS_OK)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON (status) ;
                        }

                        /* Check if a notification was registered. */
                        if (notifyFunc != NULL) {
                            status = NOTIFY_unregister
                                   (otherClient->procId,
                                    RingIO_ctrlPtr->ipsId,
                                    RingIO_ctrlPtr->ipsEventNo,
                                    (FnNotifyCbck) &RingIO_notifyCallback,
                                    client) ;
                            DBC_assert (status == SYS_OK) ;
                        }
                    }
                    else {
                        client->notifyFunc        = NULL ;
                        client->notifyParam       = NULL ;
                        client->notifyType        = RINGIO_NOTIFICATION_NONE ;
                        client->notifyWaterMark   = 0 ;
                        client->notifyFlag        = FALSE ;

                        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                            HAL_cacheWbInv ((Ptr) (client),
                                            sizeof (RingIO_Client)) ;
                        }

                        tmpStatus = _RingIO_releaseLock (client) ;
                        if ((tmpStatus != SYS_OK) && (status == SYS_OK)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON (status) ;
                        }
                    }

                    /* Close the MPCS object. */
                    if (control->transportType == (Uint32) RINGIO_TRANSPORT_GPP_DSP) {
                        tmpStatus = MPCS_close
                                         (RingIO_ctrlPtr->dspId,
                                          (MPCS_Handle) client->virtLockHandle) ;
                        if ((tmpStatus != SYS_OK) && (status == SYS_OK)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON (status) ;
                        }
                    }
                }

                /* Release Global RingIO lock  */
                tmpStatus = MPCS_leave (RingIO_mpcsObj) ;
                if (   (tmpStatus != SYS_OK)
                    && (status == SYS_OK)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON (status) ;
                }
            }
        }
    }

    return MAP_STATUS (status) ;
}


/** ============================================================================
 *  @func   RingIO_acquire
 *
 *  @desc   This function acquire a data buffer from RingIO.
 *          This function is used to open an RingIO Channel either for reading
 *          or writing. Only one reader and one writer can be opened on a RingIO
 *          Channel.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
RingIO_acquire (IN     RingIO_Handle   handle,
                OUT    RingIO_BufPtr * pData,
                IN OUT Uint32 *        pSize)
{
    Int                  status    = RINGIO_SUCCESS ;
    Int                  tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client*       client    = (RingIO_Client*) handle ;
    RingIO_ControlHandle control ;

    DBC_require (handle  != NULL) ;
    DBC_require (pData != NULL) ;
    DBC_require (pSize    != NULL) ;

    if ((handle == NULL) || (pData == NULL) || (pSize == NULL)) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
            HAL_cacheInv ((Ptr) (client), sizeof (RingIO_Client)) ;
        }
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;

        status = _RingIO_acquireLock (client) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {
            if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                HAL_cacheInv ((Ptr) (control), sizeof (RingIO_ControlStruct)) ;
            }

            if (!client->isValid) {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON (status) ;
            }
            else {
                /* Nothing to be done if zero size buffer is being acquired. */
                if (*pSize != 0) {
                    if (client->openMode ==  (Uint32) RINGIO_MODE_WRITE){
                        status = _RingIO_writerAcquire (client, pData, pSize) ;
                    }
                    else{
                        status = _RingIO_readerAcquire (client, pData, pSize) ;
                    }

                    if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                        HAL_cacheWbInv ((Ptr) (control),
                                        sizeof (RingIO_ControlStruct));
                    }
                }
            }

            tmpStatus = _RingIO_releaseLock (client) ;
            if ((tmpStatus != SYS_OK) && (status == SYS_OK)) {
                status = tmpStatus ;
                SET_FAILURE_REASON (status) ;
            }
        }
    }

    return MAP_STATUS (status) ;
}



/** ============================================================================
 *  @func   RingIO_release
 *
 *  @desc   This function releases a data buffer to RingIO.
 *          This function releases an acquired buffer or part of it.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
RingIO_release (IN  RingIO_Handle handle, IN  Uint32 size)
{
    Int                  status     = RINGIO_SUCCESS ;
    Int                  lockStatus = RINGIO_SUCCESS ;
    Int                  tmpStatus  = RINGIO_SUCCESS ;
    RingIO_Client*       client     = (RingIO_Client*) handle ;
    RingIO_ControlHandle control ;

    DBC_require (handle != NULL) ;

    if (handle == NULL) {
        status = RINGIO_EFAILURE ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
            HAL_cacheInv ((Ptr) (client), sizeof (RingIO_Client)) ;
        }
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;

        status = _RingIO_acquireLock (client) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {
            if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                HAL_cacheInv ((Ptr) (control), sizeof (RingIO_ControlStruct)) ;
            }

            if (!client->isValid){
                lockStatus = _RingIO_releaseLock (client) ;
                DBC_assert (lockStatus == SYS_OK) ;
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON (status) ;
            }
            else {
                /* Nothing to be done if zero size buffer is being released. */
                if (size != 0) {
                    if (IS_WRITER(client)) {
                        status = _RingIO_writerRelease (client, size) ;
                    }
                    else {
                        status = _RingIO_readerRelease (client, size) ;
                    }

                    if (status == RINGIO_SUCCESS) {
                        tmpStatus = _RingIO_checkNotifier (client) ;
                    }

                    if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                        HAL_cacheWbInv ((Ptr) (control),
                                         sizeof (RingIO_ControlStruct)) ;
                    }
                    lockStatus = _RingIO_releaseLock (client) ;
                    if ((status == SYS_OK) && (lockStatus != SYS_OK)) {
                        status = lockStatus ;
                        SET_FAILURE_REASON (status) ;
                    }

                    if ((status == SYS_OK) && (tmpStatus != RINGIO_EFAILURE)) {
                        /* 0 = WaterMark_Notification_Message */
                        tmpStatus = _RingIO_callNotifier (client, 0) ;
                        if (tmpStatus != SYS_OK) {
                            status = tmpStatus ;
                        }
                    }
                }
                else {
                    lockStatus =  _RingIO_releaseLock (client) ;
                    if ((status == SYS_OK) && (lockStatus != SYS_OK)) {
                        status = lockStatus ;
                        SET_FAILURE_REASON (status) ;
                    }
                }
            }
        }
    }

    return MAP_STATUS (status) ;
}


/** ============================================================================
 *  @func   RingIO_cancel
 *
 *  @desc   cancle previous acquires.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
RingIO_cancel (IN  RingIO_Handle handle)
{
    Int                  status    = RINGIO_SUCCESS ;
    Int                  tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client*       client    = (RingIO_Client*) handle ;
    RingIO_ControlHandle control ;

    DBC_require (handle  != NULL) ;

    if (handle == NULL) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
            HAL_cacheInv ((Ptr) (client), sizeof (RingIO_Client)) ;
        }
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;

        status = _RingIO_acquireLock (client) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {
            if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                HAL_cacheInv ((Ptr) (control), sizeof (RingIO_ControlStruct)) ;
            }

            if (!client->isValid) {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON (status) ;
            }
            else {
                if (client->openMode == (Uint32) RINGIO_MODE_WRITE){
                    status = _RingIO_writerCancel (client) ;
                }
                else{
                    status = _RingIO_readerCancel (client) ;
                }

                if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                    HAL_cacheWbInv ((Ptr) (control),
                                    sizeof (RingIO_ControlStruct)) ;
                }
            }

            tmpStatus = _RingIO_releaseLock (client) ;
            if ((tmpStatus != SYS_OK) && (status == SYS_OK)) {
                status = tmpStatus ;
                SET_FAILURE_REASON (status) ;
            }
        }
    }

    return MAP_STATUS (status) ;
}


/** ============================================================================
 *  @func   RingIO_getvAttribute
 *
 *  @desc   This function gets an attribute with a variable-sized payload from
 *          the attribute buffer.
 *          If an attribute is present, the attribute type, the optional
 *          parameter, a pointer to the optional payload and the payload size
 *          are returned.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
RingIO_getvAttribute (IN     RingIO_Handle handle,
                      OUT    Uint16 *      type,
                      OUT    Uint32 *      param,
                      IN     RingIO_BufPtr vptr,
                      IN OUT Uint32 *      pSize)
{
    Int                  status    = RINGIO_SUCCESS ;
    Int                  tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client *      client    = (RingIO_Client*)handle ;
    RingIO_ControlHandle control   = NULL ;

    DBC_require (handle  != NULL) ;
    DBC_require (type    != NULL) ;
    DBC_require (param   != NULL) ;

    /* vptr and size may be NULL if called from RingIO_getAttribute (). */
    if ((handle == NULL) || (type == NULL) || (param == NULL)) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
            HAL_cacheInv ((Ptr) (client), sizeof (RingIO_Client)) ;
        }
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;

        status = _RingIO_acquireLock (client) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {

            if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                HAL_cacheInv ((Ptr) (control), sizeof (RingIO_ControlStruct)) ;
            }

            /* Make sure that it's not a writer trying to read an attribute */
            if (IS_WRITER(client)){
                status = SYS_EINVAL ;
                SET_FAILURE_REASON (status) ;
            }
            else {
                status = _RingIO_readerGetvAttribute (client,
                                                      type,
                                                      param,
                                                      vptr,
                                                      pSize) ;
            }

            if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                HAL_cacheWbInv ((Ptr) (control),
                                 sizeof (RingIO_ControlStruct)) ;
            }
            tmpStatus = _RingIO_releaseLock (client) ;
            if ((tmpStatus != SYS_OK) && (status == SYS_OK)) {
                status = tmpStatus ;
                SET_FAILURE_REASON (status) ;
            }
        }
    }

    return MAP_STATUS (status) ;
}


/** ============================================================================
 *  @func   RingIO_setvAttribute
 *
 *  @desc   This function sets an attribute with a variable sized payload at the
 *          offset provided in the acquired data buffer.
 *          If the offset is not in the range of the acquired data buffer, the
 *          attribute is not set, and an error is returned. One exception to
 *          this rule is when no data buffer has been acquired. In this case an
 *          attribute is set at the next data buffer offset that can be acquired
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
RingIO_setvAttribute (IN     RingIO_Handle handle,
                      IN     Uint32        offset,
                      IN     Uint16        type,
                      IN     Uint32        param,
                      IN     RingIO_BufPtr pdata,
                      IN     Uint32        size)
{
    Int                  status    = RINGIO_SUCCESS ;
    Int                  tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client *      client    = (RingIO_Client*)handle ;
    RingIO_ControlHandle control   = client->virtControlHandle ;

    DBC_require (handle  != NULL) ;

    if (handle == NULL) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
            HAL_cacheInv ((Ptr) (client), sizeof (RingIO_Client)) ;
        }
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;

        status = _RingIO_acquireLock (client) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {
            if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                HAL_cacheInv ((Ptr) (control), sizeof (RingIO_ControlStruct)) ;
            }

            /* Make sure that it's not a reader trying to set an attribute */
            if (IS_READER(client)){
                status = SYS_EINVAL ;
                SET_FAILURE_REASON (status) ;
            }
            else {
                status = _RingIO_writerSetvAttribute (client,
                                                      offset,
                                                      type,
                                                      param,
                                                      pdata,
                                                      size) ;
            }

            if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                HAL_cacheWbInv ((Ptr) (control), sizeof (RingIO_ControlStruct)) ;
            }

            tmpStatus = _RingIO_releaseLock (client) ;
            if ((tmpStatus != SYS_OK) && (status == SYS_OK)) {
                status = tmpStatus ;
                SET_FAILURE_REASON (status) ;
            }
        }
    }

    return MAP_STATUS (status) ;
}


/** ============================================================================
 *  @func   RingIO_flush
 *
 *  @desc   This function flushes the data buffer.
 *          This function is used to flush the data from the RingIO.
 *          Behavior of this function depends on the value of hardFlush argument
 *          When hardFlush is false:
 *          If function is called for the writer, all the valid data in buffer
 *          after the first attribute location will be discarded. In case there
 *          are no attributes, no data will be cleared from the buffer. Note
 *          that this does not include the data that has been already acquired
 *          by the reader. Note that the attribute will also be cleared from the
 *          attribute buffer.
 *          For the reader, all the data till the next attribute location will
 *          be discarded. And if there is no attribute in the buffer, all valid
 *          data will get discarded. Note that the attribute will remain the
 *          attribute buffer. This is different from the behavior mentioned for
 *          the writer.
 *          When hardFlush is true:
 *          If function is called from the writer, all committed data and
 *          attributes that is not acquired by reader are removed from
 *          the RingIO instance. The writer pointer is moved to point to
 *          reader's head pointer
 *          If function is called from the reader, all data and attributes
 *          that can be subsequently acquired from the reader are removed.
 *
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
RingIO_flush (RingIO_Handle handle,
              Bool          hardFlush,
              Uint16 *      type,
              Uint32 *      param,
              Uint32 *      bytesFlushed)
{
    Int                  status    = RINGIO_SUCCESS ;
    Int                  tmpStatus = RINGIO_SUCCESS ;
    Int                  lockStatus = RINGIO_SUCCESS ;
    RingIO_Client *      client    = (RingIO_Client*) handle ;
    RingIO_ControlHandle control ;

    DBC_require (handle  != NULL) ;
    DBC_require (type    != NULL) ;
    DBC_require (param   != NULL) ;
    DBC_require (bytesFlushed != NULL) ;

    if ((handle == NULL) || (type == NULL) || (param == NULL)
         || (bytesFlushed == NULL)) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
            HAL_cacheInv ((Ptr) (client), sizeof (RingIO_Client)) ;
        }
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;

        status = _RingIO_acquireLock (client) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {
            if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                HAL_cacheInv ((Ptr) (control), sizeof (RingIO_ControlStruct)) ;
            }

            /*  Before Flush is called any acquired buffer should be released.
             *  This is TRUE for both the reader and the writer.
             */
            if (client->acqSize) {
                *type = RINGIO_INVALID_ATTR ;
                status = RINGIO_EFAILURE ;
            }
            else {
                if (IS_WRITER(client)) {
                    status = _RingIO_writerFlush (client,
                                                  hardFlush,
                                                  type,
                                                  param,
                                                  bytesFlushed) ;
                }
                else {
                    status = _RingIO_readerFlush (client,
                                                  hardFlush,
                                                  type,
                                                  param,
                                                  bytesFlushed) ;
                    if (status == SYS_OK) {
                        tmpStatus = _RingIO_checkNotifier (client) ;
                    }
                    else {
                        SET_FAILURE_REASON (status) ;
                    }
                }

                if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                    HAL_cacheWbInv ((Ptr) (control),
                                    sizeof (RingIO_ControlStruct)) ;
                }
            }

            lockStatus = _RingIO_releaseLock (client) ;
            if ((status == SYS_OK) && (lockStatus != SYS_OK)) {
                status = lockStatus ;
                SET_FAILURE_REASON (status) ;
            }

            if (    (status == SYS_OK)
                 && (tmpStatus != RINGIO_EFAILURE)
                 && (IS_READER(client) == TRUE)) {
                /* 0 = WaterMark_Notification_Message */
                tmpStatus = _RingIO_callNotifier (client, 0) ;
                if (tmpStatus != SYS_OK) {
                    status = tmpStatus ;
                }
            }
        }
    }

    return MAP_STATUS (status) ;
}


/** ============================================================================
 *  @func   RingIO_setNotifier
 *
 *  @desc   This function sets Notification parameters for the RingIO Client.
 *          Both the reader and writer can set their notification mechanism
 *          using this function
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
RingIO_setNotifier (IN  RingIO_Handle      handle,
                    IN  RingIO_NotifyType  notifyType,
                    IN  Uint32             notifyWatermark,
                    IN  RingIO_NotifyFunc  notifyFunc,
                    IN  RingIO_NotifyParam pNotifyParam)
{
    Int                  status      = RINGIO_SUCCESS ;
    Int                  tmpStatus   = RINGIO_SUCCESS ;
    RingIO_Client *      client      = (RingIO_Client*) handle ;
    RingIO_Client *      otherClient = NULL ;
    Bool                 isRegistered = FALSE ;
    RingIO_ControlHandle control ;

    DBC_require (handle     != NULL) ;
    DBC_require (notifyFunc != NULL) ;
    DBC_require (   (notifyType == RINGIO_NOTIFICATION_NONE)
                 || (notifyType == RINGIO_NOTIFICATION_ALWAYS)
                 || (notifyType == RINGIO_NOTIFICATION_ONCE)
                 || (notifyType == RINGIO_NOTIFICATION_HDWRFIFO_ALWAYS)
                 || (notifyType == RINGIO_NOTIFICATION_HDWRFIFO_ONCE)) ;

    if (    (handle     == NULL)
        ||  (notifyFunc == NULL)
        ||  (   (notifyType != RINGIO_NOTIFICATION_NONE)
             && (notifyType != RINGIO_NOTIFICATION_ALWAYS)
             && (notifyType != RINGIO_NOTIFICATION_ONCE)
             && (notifyType != RINGIO_NOTIFICATION_HDWRFIFO_ALWAYS)
             && (notifyType != RINGIO_NOTIFICATION_HDWRFIFO_ONCE))) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
            HAL_cacheInv ((Ptr) (client), sizeof (RingIO_Client)) ;
        }
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;

        status = _RingIO_acquireLock (client) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {
            if (notifyWatermark > control->dataBufSize) {
                tmpStatus = _RingIO_releaseLock (client) ;
                DBC_assert (tmpStatus == SYS_OK) ;
                status = SYS_EINVAL ;
                SET_FAILURE_REASON (status) ;
            }
            else {
                if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                        HAL_cacheInv ((Ptr) (control),
                                       sizeof (RingIO_ControlStruct)) ;
                }

                if (   control->transportType
                    == (Uint32) RINGIO_TRANSPORT_GPP_DSP) {
                    if (IS_READER(client)) {
                        otherClient = &(control->writer) ;
                    }
                    else {
                        otherClient = &(control->reader) ;
                    }

                    if (otherClient->isValid) {
                        if (client->notifyFunc != NULL) {
                            isRegistered = TRUE ;
                        }

                        client->notifyType         =   notifyType ;
                        client->notifyWaterMark    =   notifyWatermark ;
                        client->notifyFunc         =   NULL ;
                        client->notifyParam        =   pNotifyParam ;

                        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                            HAL_cacheWbInv ((Ptr) (client),
                                            sizeof (RingIO_Client)) ;
                        }

                        tmpStatus = _RingIO_releaseLock (client) ;
                        if ((tmpStatus != SYS_OK) && (status == SYS_OK)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON (status) ;
                        }

                        /* Check if a notification was registered. */
                        if (isRegistered == TRUE) {
                            /* Unregister any registered notification. */
                            status = NOTIFY_unregister
                                            (otherClient->procId,
                                             RingIO_ctrlPtr->ipsId,
                                             RingIO_ctrlPtr->ipsEventNo,
                                             &RingIO_notifyCallback,
                                             client) ;
                            DBC_assert (status == SYS_OK) ;
                        }

                        if (status == SYS_OK)  {
                            if (otherClient->isValid) {
                                status = NOTIFY_register
                                         (otherClient->procId,
                                          RingIO_ctrlPtr->ipsId,
                                          RingIO_ctrlPtr->ipsEventNo,
                                          (FnNotifyCbck) &RingIO_notifyCallback,
                                          client) ;
                                DBC_assert (status == SYS_OK) ;
                                if (status == SYS_OK) {
                                    /* Update the client structure with the
                                     * user supplied call back function
                                     */
                                    client->notifyFunc = notifyFunc ;
                                    if (client->flags
                                        & RINGIO_CONTROL_CACHEUSE) {
                                        HAL_cacheWbInv ((Ptr) (client),
                                                       sizeof ( RingIO_Client));
                                    }
                                }
                           }
                           else {
                                status = RINGIO_EFAILURE ;
                                SET_FAILURE_REASON (status) ;
                            }
                        }
                    }
                    else {
                       _RingIO_releaseLock (client) ;
                        status = RINGIO_EFAILURE ;
                        SET_FAILURE_REASON (status) ;
                    }
                }
                else {
                    client->notifyType         =   notifyType ;
                    client->notifyWaterMark    =   notifyWatermark ;
                    client->notifyFunc         =   notifyFunc ;
                    client->notifyParam        =   pNotifyParam ;

                    if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                        HAL_cacheWbInv ((Ptr) (client), sizeof (RingIO_Client)) ;
                    }

                    tmpStatus = _RingIO_releaseLock (client) ;
                    if ((tmpStatus != SYS_OK) && (status == SYS_OK)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON (status) ;
                    }
                }
            }
        }
    }

    return MAP_STATUS (status) ;
}


/** ----------------------------------------------------------------------------
 *  @func   RingIO_getControlHandle
 *
 *  @desc   This function gets the control structure handle corresponding to a
 *          RingIO name.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
static
RingIO_ControlHandle
RingIO_getControlHandle (IN  Char * name)
{
    Uint32 i ;

    for (i = 0 ; i < RingIO_ctrlPtr->maxEntries ; i++) {
        HAL_cacheInv ((Ptr) &(RingIO_ctrlPtr->dspAddrEntry [i]),
                      sizeof (RingIO_Entry)) ;

        /* Check if the entry is valid. */
        if (    (RingIO_ctrlPtr->dspAddrEntry [i].phyControl != NULL)
            &&  (RingIO_ctrlPtr->dspAddrEntry [i].phyControl != (Void *) 1)) {
            if (strncmp (name, RingIO_ctrlPtr->dspAddrEntry[i].name,
                        RINGIO_NAME_MAX_LEN) == 0) {
                return RingIO_ctrlPtr->dspAddrEntry[i].phyControl ;
            }
        }
    }

    return NULL ;
}

/** ----------------------------------------------------------------------------
 *  @func   RingIO_notifyCallback
 *
 *  @desc   Callback function registered for event notification from remote
 *          processor.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
Void
RingIO_notifyCallback (IN  Uint32 eventNo, IN  Void * arg, IN  Void * info)
{
    RingIO_Client * client  = (RingIO_Client *) arg ;
    Uint32          entryId = (Uint32) info & 0xFFFF ;
    Uint16          msg     = (Uint16)((Uint32)info >> 16);

    (Void) eventNo ; /* To remove compiler warning */

    /*
     *  Determine that the notification is for the current client and invoke
     *  the registered callback
     */
    if (    (GET_ENTRY_ID(client) == entryId)
         && ( client->notifyFunc != NULL)) {
        client->notifyFunc (client, client->notifyParam, msg) ;
    }
}


/** ----------------------------------------------------------------------------
 *  @func   RingIO_sendNotify
 *
 *  @desc   Send a notification to the other client manually.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
Int
RingIO_sendNotify (RingIO_Handle handle,RingIO_NotifyMsg msg)
{
    Int            status = RINGIO_SUCCESS ;
    RingIO_Client* client = (RingIO_Client*) handle ;

    DBC_require (handle  != NULL) ;

    if (handle == NULL) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else{
         status = _RingIO_callNotifier (client, msg) ;
    }

    return MAP_STATUS (status) ;

}

/** ============================================================================
 *  @func   RingIO_getValidSize
 *
 *  @desc   returns the valid size of the RingIO if the Client is valid.
 *          incase of any failure returns zero.
 *
 *  @modif  None.
 *  ============================================================================
 */
Uint32
RingIO_getValidSize (IN  RingIO_Handle   handle )
{
    Int                  status    = RINGIO_SUCCESS ;
    Int                  tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client*       client    = (RingIO_Client*) handle;
    Uint32               size      = 0;
    RingIO_ControlHandle control ;

    DBC_require (handle  != NULL) ;

    if (handle != NULL ) {
        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
            HAL_cacheInv ((Ptr) (client), sizeof (RingIO_Client)) ;
        }
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;

        status = _RingIO_acquireLock (client) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {

            if (!client->isValid) {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON (status) ;
            }
            else {
                if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                    HAL_cacheInv ((Ptr) (control),
                                   sizeof (RingIO_ControlStruct)) ;
                }
                /* Return the valid data size */
                size = control->validSize ;
            }

       }

        tmpStatus = _RingIO_releaseLock (client) ;
        if ((tmpStatus != SYS_OK) && (status == SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }
    }

    return (size) ;
}

/** ============================================================================
 *  @func   RingIO_getEmptySize
 *
 *  @desc   returns the Empty size of the RingIO if the Client is valid.
 *          incase of any failure returns zero.
 *
 *  @modif  None.
 *  ============================================================================
 */
Uint32
RingIO_getEmptySize (IN     RingIO_Handle   handle )
{
    Int                  status    = RINGIO_SUCCESS ;
    Int                  tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client*       client    = ( RingIO_Client*) handle ;
    Uint32               size      = 0;
    RingIO_ControlHandle control ;

    DBC_require (handle  != NULL) ;

    if (handle != NULL ) {
        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
            HAL_cacheInv ((Ptr) (client), sizeof (RingIO_Client)) ;
        }
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;

        status = _RingIO_acquireLock (client) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {

            if (!client->isValid) {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON (status) ;
            }
            else {
                if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                    HAL_cacheInv ((Ptr) (control),
                                   sizeof (RingIO_ControlStruct)) ;
                }
                /* Return the empty size */
                size = control->emptySize ;
             }

       }

        tmpStatus = _RingIO_releaseLock (client) ;
        if ((tmpStatus != SYS_OK) && (status == SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }
    }

    return (size) ;
}

/** ============================================================================
 *  @func   RingIO_getValidAttrSize
 *
 *  @desc   returns the Valid attr size of the RingIO if the Client is valid.
 *          incase of any failure it returns zero.
 *
 *  @modif  None.
 *  ============================================================================
 */
Uint32
RingIO_getValidAttrSize (IN     RingIO_Handle   handle )
{
    Int                  status    = RINGIO_SUCCESS ;
    Int                  tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client*       client    = (RingIO_Client*) handle ;
    Uint32               size      = 0;
    RingIO_ControlHandle control ;

    DBC_require (handle  != NULL) ;

    if (handle == NULL  ) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
            HAL_cacheInv ((Ptr) (client), sizeof (RingIO_Client)) ;
        }
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;

        status = _RingIO_acquireLock (client) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {

            if (!client->isValid) {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON (status) ;
            }
            else {
                if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                    HAL_cacheInv ((Ptr) (control),
                                  sizeof (RingIO_ControlStruct)) ;
                }
                /* Return the valid attrbute size */
                size = control->validAttrSize ;
             }

       }

        tmpStatus = _RingIO_releaseLock (client) ;
        if ((tmpStatus != SYS_OK) && (status == SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }
    }

    return (size) ;
}

/** ============================================================================
 *  @func   RingIO_getEmptyAttrSize
 *
 *  @desc   returns the Empty attributesize of the RingIO if  Client is valid.
 *          incase of any failure it returns zero.
 *
 *  @modif  None.
 *  ============================================================================
 */
Uint32
RingIO_getEmptyAttrSize (IN     RingIO_Handle   handle )
{
    Int                  status    = RINGIO_SUCCESS ;
    Int                  tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client*       client    = (RingIO_Client*) handle ;
    Uint32               size      = 0;
    RingIO_ControlHandle control ;

    DBC_require (handle  != NULL) ;

    if (handle != NULL ) {
        if (client->flags & RINGIO_CONTROL_CACHEUSE) {
            HAL_cacheInv ((Ptr) (client), sizeof (RingIO_Client)) ;
        }
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;

        status = _RingIO_acquireLock (client) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {

            if (!client->isValid) {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON (status) ;
            }
            else {
                if (client->flags & RINGIO_CONTROL_CACHEUSE) {
                    HAL_cacheInv ((Ptr) (control),
                                   sizeof (RingIO_ControlStruct)) ;
                }
                /* Return the empty size */
                size = control->emptyAttrSize ;
             }

       }

        tmpStatus = _RingIO_releaseLock (client) ;
        if ((tmpStatus != SYS_OK) && (status == SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }
    }

    return (size) ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
