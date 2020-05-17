/** ============================================================================
 *  @file   ringio.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/
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


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

#include <_pooldefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- OSAL Headers                */
#include <drvdefs.h>
#include <drv_api.h>

/*  ----------------------------------- User API                    */
#include <mpcs.h>
#include <pool.h>
#include <_pooldefs.h>
#include <ringiodefs.h>
#include <_ringiodefs.h>
#include <ringio.h>
#include <_ringio.h>
#include <notify.h>

/*  ----------------------------------- C Libraries                 */
#include <string.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent ID.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_API_RINGIO

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason
 *  ============================================================================
 */
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_RINGIO, __LINE__)

/** ============================================================================
 *  @macro  MAP_STATUS
 *
 *  @desc   Map the status code to ensure that RINGIO_SUCCESS is returned from
 *          all APIs for success.
 *  ============================================================================
 */
#define MAP_STATUS(status)    (((status) == DSP_SOK) ? (RINGIO_SUCCESS): (status))


/** ============================================================================
 *  @name   RingIO_State
 *
 *  @desc   State object for the RINGIO component containing all information
 *          required by it.
 *  ============================================================================
 */
extern RingIO_Object RingIO_State [MAX_DSPS] ;


/** ============================================================================
 *  @func   RingIO_create
 *
 *  @desc   This function creates a RingIO instance in Shared memory using the
 *          creation params specified.
 *
 *          This function creates a RingIO Channel in Shared memory using the
 *          creation params specified. The control parameters need not specify
 *          the data buffer and attribute buffer pointers during creation.
 *          But note that before opening the channel for reading or writing both
 *          the buffer pointers should be set.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
#if defined (DSPLINK_LEGACY_SUPPORT)
RingIO_create (IN Char8 *         name,
               IN RingIO_Attrs *  attrs)
#else
RingIO_create (IN ProcessorId     procId,
               IN Char8 *         name,
               IN RingIO_Attrs *  attrs)
#endif /* if defined (DSPLINK_LEGACY_SUPPORT) */
{
    DSP_STATUS           status           = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus        = RINGIO_SUCCESS ;
    RingIO_ControlHandle ringIoCtrl       = NULL  ;
    RingIO_ControlHandle phyControl       = NULL  ;
    RingIO_BufPtr        ringIoDataBufPtr = NULL ;
    RingIO_BufPtr        phyBufStart      = NULL ;
    RingIO_BufPtr        ringIoAttrBufPtr = NULL ;
    RingIO_BufPtr        phyAttrStart     = NULL ;
    MPCS_ShObj *         ringIoMpcsObj    = NULL ;
    Void *               phyLockHandle    = NULL ;
    RingIO_Object *      ringIoState      = NULL ;
    Int32                slot             = -1   ;
    Uint32               i ;
    MPCS_Attrs           mpcsAttrs ;
#if defined (DSPLINK_LEGACY_SUPPORT)
    ProcessorId          procId ;
#endif /* if defined (DSPLINK_LEGACY_SUPPORT) */

    TRC_2ENTER ("RingIO_create", name, attrs) ;

    DBC_Require (name  != NULL) ;
    DBC_Require (attrs != NULL) ;

#if defined (DSPLINK_LEGACY_SUPPORT)
    procId = 0u ;
#endif /* if defined (DSPLINK_LEGACY_SUPPORT) */

    if ((name == NULL) || (attrs == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else if (   (attrs->transportType != (Uint32) RINGIO_TRANSPORT_GPP_DSP)
             || (attrs->dataBufSize == 0)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        ringIoState = &RingIO_State [procId] ;
        DBC_Assert (ringIoState != NULL) ;
        if (ringIoState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = MPCS_enter (ringIoState->mpcsObj) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            /* Check if the name already exists in the array and get a free slot
             * in the array if the existing matching name is not found.
             */
            for (i = 0 ;
                 (   (i < ringIoState->ctrlPtr->maxEntries)
                  && DSP_SUCCEEDED (status)) ;
                   i++) {
                if (   strncmp (name,
                                ringIoState->entryPtr [i].name,
                                RINGIO_NAME_MAX_LEN)
                    == 0) {
                    status = RINGIO_EALREADYEXISTS ;
                    SET_FAILURE_REASON ;
                }

                if (       (slot == -1)
                        && (ringIoState->entryPtr [i].phyControl ==  NULL)) {
                    /* Reserve the found slot. */
                    ringIoState->entryPtr [i].phyControl = (Pvoid) 1 ;
                    slot = i ;
                }
            }

            /* Existing matching name was not found, but all slots are full. */
            if (DSP_SUCCEEDED (status) && (slot == -1)) {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON ;
            }

            tmpStatus = MPCS_leave (ringIoState->mpcsObj) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            /* Allocate memory for control structure */
            status = POOL_alloc (attrs->ctrlPoolId,
                                 (Pvoid *) &ringIoCtrl,
                                 sizeof (RingIO_ControlStruct)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                status = POOL_translateAddr (attrs->ctrlPoolId,
                                             (Pvoid *)&(phyControl),
                                             AddrType_Dsp,
                                             ringIoCtrl,
                                             AddrType_Usr) ;
            }

            /* Allocate memory for Data buffer. */
            if (DSP_SUCCEEDED (status)) {
                status = POOL_alloc (attrs->dataPoolId,
                                    (Pvoid *) &ringIoDataBufPtr,
                                    (attrs->dataBufSize + attrs->footBufSize)) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                else {
                    status = POOL_translateAddr (attrs->dataPoolId,
                                                 (Pvoid *)&(phyBufStart),
                                                 AddrType_Dsp,
                                                 ringIoDataBufPtr,
                                                 AddrType_Usr) ;

                }
            }

            /* Allocate memory for Attribute buffer, if required. */
            if (DSP_SUCCEEDED (status) && (attrs->attrBufSize != 0)) {
                status = POOL_alloc (attrs->attrPoolId,
                                     (Pvoid *) &ringIoAttrBufPtr,
                                     attrs->attrBufSize) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                else {
                    status = POOL_translateAddr (attrs->attrPoolId,
                                                 (Pvoid *) &(phyAttrStart),
                                                 AddrType_Dsp,
                                                 ringIoAttrBufPtr,
                                                 AddrType_Usr) ;
                }
            }

            /* Allocate memory for lock structure. */
            if (DSP_SUCCEEDED (status)) {
                status = POOL_alloc (attrs->lockPoolId,
                                     (Pvoid *) &ringIoMpcsObj,
                                     sizeof (MPCS_ShObj)) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                else {
                    status = POOL_translateAddr (attrs->lockPoolId,
                                                 (Pvoid *) &(phyLockHandle),
                                                 AddrType_Dsp,
                                                 ringIoMpcsObj,
                                                 AddrType_Usr) ;
                    if (DSP_SUCCEEDED (status)) {
                        mpcsAttrs.poolId = attrs->lockPoolId ;
                        /* Use the RingIO name as the MPCS name. */
                        status = MPCS_create (ringIoState->ctrlPtr->dspId,
                                              name,
                                              ringIoMpcsObj,
                                              &mpcsAttrs) ;
                    }
                }
            }

            if (DSP_SUCCEEDED (status)) {
                status = MPCS_enter (ringIoState->mpcsObj) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                else {
                    ringIoState->entryPtr [slot].phyControl  = phyControl ;
                    ringIoState->entryPtr [slot].virtControl = ringIoCtrl ;

                    /* Initialize the control structure */
                    ringIoCtrl->procId      = procId ;
                    ringIoCtrl->entryId         = slot   ;
                    ringIoCtrl->transportType   =
                                            (Uint32) attrs->transportType ;

                    ringIoCtrl->phyLockHandle   = phyLockHandle ;

                    ringIoCtrl->phyBufStart     = phyBufStart ;
                    ringIoCtrl->phyBufEnd       =    attrs->dataBufSize
                                                   + attrs->footBufSize
                                                   - 1 ;

                    ringIoCtrl->curBufEnd       = attrs->dataBufSize - 1 ;
                    ringIoCtrl->dataBufEnd      = attrs->dataBufSize - 1 ;
                    ringIoCtrl->dataBufSize     = attrs->dataBufSize ;
                    ringIoCtrl->footBufSize     = attrs->footBufSize ;

                    ringIoCtrl->validSize         = 0 ;
                    ringIoCtrl->emptySize         = attrs->dataBufSize ;

                    ringIoCtrl->phyAttrStart      = phyAttrStart ;
                    if (attrs->attrBufSize != 0) {
                        ringIoCtrl->phyAttrBufEnd = attrs->attrBufSize - 1 ;
                    }
                    else {
                        ringIoCtrl->phyAttrBufEnd = (Uint32) -1l ;
                    }
                    ringIoCtrl->curAttrBufEnd     = ringIoCtrl->phyAttrBufEnd ;

                    ringIoCtrl->validAttrSize     = 0 ;
                    ringIoCtrl->emptyAttrSize     = attrs->attrBufSize ;

                    ringIoCtrl->prevAttrOffset    = RINGIO_INVALID_OFFSET ;

                    /* Make sure both reader and writer are uninitialized */
                    ringIoCtrl->reader.isValid    = FALSE ;
                    ringIoCtrl->reader.refCount   = 0 ;

                    ringIoCtrl->writer.isValid    = FALSE ;
                    ringIoCtrl->writer.refCount   = 0 ;

                    ringIoCtrl->reader.openMode   = (Uint32) RINGIO_MODE_READ ;
                    ringIoCtrl->writer.openMode   = (Uint32) RINGIO_MODE_WRITE ;

                    ringIoState->entryPtr[slot].ctrlPoolId = attrs->ctrlPoolId ;
                    ringIoState->entryPtr[slot].dataPoolId = attrs->dataPoolId ;
                    ringIoState->entryPtr[slot].attrPoolId = attrs->attrPoolId ;
                    ringIoState->entryPtr[slot].lockPoolId = attrs->lockPoolId ;
                    ringIoState->entryPtr[slot].ownerProcId = ID_GPP ;

                    strncpy (ringIoState->entryPtr [slot].name,
                            name,
                            RINGIO_NAME_MAX_LEN) ;

                    if (attrs->ctrlPoolId != POOL_INVALIDID) {
                        POOL_writeback (attrs->ctrlPoolId,
                                ringIoCtrl,
                                sizeof (RingIO_ControlStruct)) ;
                    }

                    tmpStatus = MPCS_leave (ringIoState->mpcsObj) ;
                    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
            }

            if (DSP_FAILED (status)) {
                ringIoState->entryPtr [slot].phyControl = (Pvoid) 1 ;

                MPCS_delete (ringIoState->ctrlPtr->dspId, name) ;

                /* Free any memory that was successfully allocated. */
                if (ringIoCtrl != NULL) {
                    POOL_free (attrs->ctrlPoolId,
                               (Pvoid) ringIoCtrl,
                               sizeof (RingIO_ControlStruct)) ;
                }

                if (ringIoDataBufPtr != NULL) {
                    POOL_free (attrs->dataPoolId,
                               (Pvoid) ringIoDataBufPtr,
                               (attrs->dataBufSize + attrs->footBufSize)) ;
                }

                if (ringIoAttrBufPtr != NULL) {
                    POOL_free (attrs->attrPoolId,
                               (Pvoid) ringIoAttrBufPtr,
                               attrs->attrBufSize) ;
                }

                if (ringIoMpcsObj != NULL) {
                    POOL_free (attrs->lockPoolId,
                               (Pvoid) ringIoMpcsObj,
                               sizeof (MPCS_ShObj)) ;
                }

                if (attrs->ctrlPoolId != POOL_INVALIDID) {
                    POOL_writeback (attrs->ctrlPoolId,
                                    ringIoCtrl,
                                    sizeof (RingIO_ControlStruct)) ;
                }

                memset (&ringIoState->entryPtr [slot], 0, sizeof (RingIO_Entry));

                /* Reset the slot to empty so that it can be used later. */
                ringIoState->entryPtr [slot].phyControl = NULL ;
            }
        }
        else {
            if (slot != -1) {
                /* Reset the slot to empty so that it can be used later. */
                ringIoState->entryPtr [slot].phyControl = NULL ;
            }
        }
    }

    status = MAP_STATUS (status) ;

    TRC_1LEAVE ("RingIO_create", status) ;

    return status ;
}


/** ============================================================================
 *  @func   RingIO_delete
 *
 *  @desc   This function deletes a RingIO channel.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
#if defined (DSPLINK_LEGACY_SUPPORT)
RingIO_delete (IN Char8 * name)
#else
RingIO_delete (IN ProcessorId procId, IN Char8 * name)
#endif /* if defined (DSPLINK_LEGACY_SUPPORT) */
{
    DSP_STATUS           status           = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus        = RINGIO_SUCCESS ;
    RingIO_BufPtr        ringIoDataBufPtr = NULL ;
    RingIO_BufPtr        ringIoAttrBufPtr = NULL ;
    RingIO_Object *      ringIoState      = NULL ;
    MPCS_ShObj *         ringIoMpcsObj    = NULL ;
    RingIO_ControlHandle control ;
    Uint32               i ;
#if defined (DSPLINK_LEGACY_SUPPORT)
    ProcessorId          procId ;
#endif /* if defined (DSPLINK_LEGACY_SUPPORT) */

    TRC_1ENTER ("RingIO_delete", name) ;

    DBC_Require (name != NULL) ;

#if defined (DSPLINK_LEGACY_SUPPORT)
    procId = 0u ;
#endif /* if defined (DSPLINK_LEGACY_SUPPORT) */

    if (name == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        ringIoState = &RingIO_State [procId] ;
        DBC_Assert (ringIoState != NULL) ;
        if (ringIoState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = MPCS_enter (ringIoState->mpcsObj) ;
        DBC_Assert (DSP_SUCCEEDED (status)) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }

        /* Find the RingIO entry corresponding to the specified name. */
        for (i = 0 ; i < ringIoState->ctrlPtr->maxEntries ; i++) {
            /* Check if the entry is valid. */
            if (    (ringIoState->entryPtr [i].phyControl != NULL)
                 && (ringIoState->entryPtr [i].phyControl != (Pvoid) 1)) {
                if (strncmp (name,
                            ringIoState->entryPtr [i].name,
                            RINGIO_NAME_MAX_LEN) == 0) {
                    break ;
                }
            }
        }

        if (i != ringIoState->ctrlPtr->maxEntries) {
            control = ringIoState->entryPtr [i].virtControl ;
            /* If the base  actual pool number is not equal to POOL_INVALIDID
             * invalidate it.
             */
            if (POOL_getPoolNo(GET_CTRL_POOL_ID(i, procId )) != POOL_INVALIDID){

                POOL_invalidate (GET_CTRL_POOL_ID(i, procId),
                                 ringIoState->entryPtr [i].virtControl,
                                 sizeof (RingIO_ControlStruct)) ;
            }
            DBC_Assert (control != NULL) ;

            if (!control->reader.isValid && !control->writer.isValid) {
                ringIoState->entryPtr [i].phyControl = (Pvoid) 1 ;
                /* Delete the created MPCS object. Name of the MPCS object
                 * is the same as the RingIO name.
                 */
                status = MPCS_delete (ringIoState->ctrlPtr->dspId, name) ;
                DBC_Assert (DSP_SUCCEEDED (status)) ;

                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }

                if (control->phyLockHandle != NULL) {
                    /* Get the virtual address of lock object. */
                    tmpStatus = POOL_translateAddr (GET_LOCK_POOL_ID(i, procId),
                                                    (Pvoid *) &(ringIoMpcsObj),
                                                    AddrType_Usr,
                                                    control->phyLockHandle,
                                                    AddrType_Dsp) ;
                    /* Free the buffer only if a valid translated address was
                     * received.
                     */
                    if (DSP_SUCCEEDED (tmpStatus)) {
                        tmpStatus = POOL_free (GET_LOCK_POOL_ID(i, procId),
                                               (Pvoid) ringIoMpcsObj,
                                               sizeof (MPCS_ShObj)) ;
                        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
                    else {
                        if (DSP_SUCCEEDED (status)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
                }

                if (control->phyAttrStart != NULL) {
                    /* Get the virtual address of attribute buffer. */
                    tmpStatus = POOL_translateAddr (
                                             GET_ATTR_POOL_ID(i, procId),
                                             (Pvoid *) &(ringIoAttrBufPtr),
                                             AddrType_Usr,
                                             control->phyAttrStart,
                                             AddrType_Dsp) ;
                    /* Free the buffer only if a valid translated address
                     * was received.
                     */
                    if (DSP_SUCCEEDED (tmpStatus)) {
                        tmpStatus = POOL_free (GET_ATTR_POOL_ID(i, procId),
                                               (Pvoid) ringIoAttrBufPtr,
                                               control->phyAttrBufEnd + 1) ;
                        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
                    else {
                        if (DSP_SUCCEEDED (status)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
                }

                if (control->phyBufStart != NULL) {
                    /* Get the virtual address of data buffer. */
                    tmpStatus = POOL_translateAddr( GET_DATA_POOL_ID(i, procId),
                                                   (Pvoid *)&(ringIoDataBufPtr),
                                                    AddrType_Usr,
                                                    control->phyBufStart,
                                                    AddrType_Dsp) ;
                    /* Free the buffer only if a valid translated address was
                     * received.
                     */
                    if (DSP_SUCCEEDED (tmpStatus)) {
                        tmpStatus = POOL_free (GET_DATA_POOL_ID(i, procId),
                                               (Pvoid) ringIoDataBufPtr,
                                               control->phyBufEnd + 1) ;
                        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
                    else {
                        if (DSP_SUCCEEDED (status)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
                }

                /* Free the control structure. */
                tmpStatus = POOL_free (GET_CTRL_POOL_ID(i, procId),
                                       (Pvoid) control,
                                       sizeof (RingIO_ControlStruct)) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }

                memset (&ringIoState->entryPtr [i], 0, sizeof (RingIO_Entry));
            }
            else {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON ;
            }
        }
        else {
            status = RINGIO_EFAILURE ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = MPCS_leave (ringIoState->mpcsObj) ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

    }

    status = MAP_STATUS (status) ;

    TRC_1LEAVE ("RingIO_delete", status) ;

    return status ;
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
EXPORT_API
RingIO_Handle
RingIO_open (IN Char8 * pName, IN RingIO_OpenMode openMode, IN Uint32 flags)
{
    DSP_STATUS           status           = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus        = RINGIO_SUCCESS ;
    RingIO_BufPtr        ringIoDataBufPtr = NULL ;
    RingIO_BufPtr        ringIoAttrBufPtr = NULL ;
    RingIO_ControlHandle control          = NULL ;
    RingIO_Handle        handle           = NULL ;
    Uint32               i                = 0 ;
    ProcessorId          procId           = 0 ;
    RingIO_Client *      client           = NULL ;
    RingIO_Client *      otherClient      = NULL ;
    RingIO_Object *      ringIoState      = NULL ;

    TRC_3ENTER ("RingIO_open", pName, openMode, flags) ;

    DBC_Require (pName != NULL) ;
    DBC_Require (   (openMode == RINGIO_MODE_READ)
                 || (openMode == RINGIO_MODE_WRITE)) ;

    if (    (pName == NULL)
        ||  (   (openMode != RINGIO_MODE_READ)
             && (openMode != RINGIO_MODE_WRITE))) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        for (procId = 0 ;
             (procId < MAX_PROCESSORS) && (control == NULL) ;
             procId++) {
            ringIoState = &RingIO_State [procId] ;
            DBC_Assert (ringIoState != NULL) ;
            if (ringIoState->ctrlPtr != NULL) {
                status = MPCS_enter (ringIoState->mpcsObj) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                else {
                    /* Make sure the name specified has an associated control
                     * struct
                     */
                    for (i = 0 ; i < ringIoState->ctrlPtr->maxEntries ; i++) {
                        /* Check if the entry is valid. */
                        if (   (ringIoState->entryPtr [i].phyControl != NULL)
                            && (   ringIoState->entryPtr [i].phyControl
                                != (Pvoid) 1)){
                            if (strncmp (pName,
                                         ringIoState->entryPtr [i].name,
                                         RINGIO_NAME_MAX_LEN) == 0) {
                                break ;
                            }
                        }
                    }

                    if (i != ringIoState->ctrlPtr->maxEntries) {
                        /* Get the virtual address of control structure. */
                        status = POOL_translateAddr (
                                   GET_CTRL_POOL_ID(i, procId),
                                   (Pvoid *) &control,
                                   AddrType_Usr,
                                   (Pvoid) ringIoState->entryPtr [i].phyControl,
                                   AddrType_Dsp) ;

                        if (   (control != NULL)
                            && (   POOL_getPoolNo(GET_CTRL_POOL_ID(i, procId))
                                != POOL_INVALIDID)) {
                            POOL_invalidate (GET_CTRL_POOL_ID(i, procId),
                                             control,
                                             sizeof (RingIO_ControlStruct)) ;
                        }
                        /* No need to check for success, since the buffers have
                         *  been allocated on the other processor using POOL.
                         */
                        DBC_Assert (DSP_SUCCEEDED (status)) ;
                        DBC_Assert (control != NULL) ;
                    }
                    else {
                        control = NULL ;
                    }

                    tmpStatus = MPCS_leave (ringIoState->mpcsObj) ;
                    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
            }
        }
    }

    if (control == NULL) {
        status = RINGIO_EFAILURE ;
        SET_FAILURE_REASON ;
    }
    else if (DSP_FAILED(status)) {
        status = RINGIO_EFAILURE ;
        SET_FAILURE_REASON ;
    }
    else {
        /* Get the client handle */
        if (openMode == (Uint32) RINGIO_MODE_WRITE) {
            client      = &control->writer ;
            otherClient = &control->reader ;
        }
        else {
            client      = &control->reader ;
            otherClient = &control->writer ;
        }

        /* Check if the RingIO has already been opened in this mode. */
        if (client->isValid) {
            status = RINGIO_EFAILURE ;
            SET_FAILURE_REASON ;
        }
        /* Check if the open call is supported by the transport mode used for
         * creating this RingIO.
         */
        else if (    (    control->transportType
                       == (Uint32) RINGIO_TRANSPORT_DSP_DSP)
                 ||  (   (   control->transportType
                          == (Uint32) RINGIO_TRANSPORT_GPP_DSP)
                      && (otherClient->isValid)
                      && (otherClient->procId == ID_GPP))) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        else {
            /* Acquire the  RingIO global lock */
            status = MPCS_enter (ringIoState->mpcsObj) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                /* Open the MPCS object to get the virtual address of lock. */
                status = MPCS_open (ringIoState->ctrlPtr->dspId,
                                    pName,
                                    (MPCS_Handle *) (&client->virtLockHandle)) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                else {
                    client->virtControlHandle = control ;
                    status = _RingIO_acquireLock (client) ;
                    if (DSP_FAILED (status)) {
                        /* Close the MPCS object. */
                        tmpStatus = MPCS_close (ringIoState->ctrlPtr->dspId,
                                (MPCS_Handle) client->virtLockHandle) ;
                        DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
                        SET_FAILURE_REASON ;
                    }
                    else {
                        /* If owner is other processor. */
                        if (ringIoState->entryPtr [i].ownerProcId != ID_GPP) {
                            ringIoState->entryPtr [i].virtControl = control ;
                        }

                        /* Now initialize the client */
                        client->isValid           = TRUE ;
                        client->procId            = ID_GPP ;

                        /* Get the virtual address of attribute buffer,
                         *  if required.
                         */
                        if (control->phyAttrStart != NULL) {
                            status = POOL_translateAddr (
                                               GET_ATTR_POOL_ID(i, control->procId),
                                               (Pvoid *) &(ringIoAttrBufPtr),
                                               AddrType_Usr,
                                               control->phyAttrStart,
                                               AddrType_Dsp) ;
                            DBC_Assert (DSP_SUCCEEDED (status)) ;
                        }
                        client->pAttrStart = ringIoAttrBufPtr ;

                        /* Get the virtual address of data buffer. */
                        status = POOL_translateAddr (
                                               GET_DATA_POOL_ID(i, control->procId),
                                               (Pvoid *) &(ringIoDataBufPtr),
                                               AddrType_Usr,
                                               control->phyBufStart,
                                               AddrType_Dsp) ;
                        DBC_Assert (DSP_SUCCEEDED (status)) ;
                        client->pDataStart = ringIoDataBufPtr ;

                        if (client->refCount == 0) {
                            client->acqStart          = 0 ;
                            client->acqSize           = 0 ;

                            client->acqAttrStart      = 0 ;
                            client->acqAttrSize       = 0 ;
                        }

                        client->notifyFunc        = NULL ;
                        client->notifyParam       = NULL ;
                        client->notifyType        = RINGIO_NOTIFICATION_NONE ;
                        client->notifyWaterMark   = 0 ;
                        client->notifyFlag        = FALSE ;

                        client->flags             = flags ;
                        client->refCount          = 1 ;


                        handle = client ;

                        if (   POOL_getPoolNo(GET_CTRL_POOL_ID(i, control->procId))
                            != POOL_INVALIDID){
                            POOL_writeback (GET_CTRL_POOL_ID(i, control->procId),
                                            client,
                                            sizeof (RingIO_Client)) ;
                        }
                        tmpStatus = _RingIO_releaseLock (client) ;
                        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
                }
                /* Release the RingIO global lock */
                tmpStatus = MPCS_leave (ringIoState->mpcsObj) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
        }
    }

    TRC_1LEAVE ("RingIO_open", handle) ;

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
EXPORT_API
DSP_STATUS
RingIO_close (IN RingIO_Handle handle)
{
    DSP_STATUS           status         = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus      = RINGIO_SUCCESS ;
    RingIO_Client *      client         = (RingIO_Client*) handle ;
    RingIO_NotifyFunc    notifyFunc     = NULL ;
    Void *               virtLockHandle = NULL ;
    RingIO_Object *      ringIoState    = NULL ;
    RingIO_Client*       otherClient ;
    RingIO_ControlHandle control = NULL;
    Uint32               entryId ;

    TRC_1ENTER ("RingIO_close", handle) ;

    DBC_Require (handle != NULL) ;

    if (handle == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        control = client->virtControlHandle ;
        DBC_Assert (control != NULL) ;
        ringIoState = &RingIO_State [control->procId] ;
        DBC_Assert (ringIoState != NULL) ;
        if (ringIoState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        entryId = GET_ENTRY_ID(control) ;

        if (   POOL_getPoolNo(GET_CTRL_POOL_ID(entryId,control->procId))
            != POOL_INVALIDID){
            /* Get the pool id from the entry id and dsp processor Id
             * and do invalidate
             */
            POOL_invalidate (GET_CTRL_POOL_ID(entryId, control->procId),
                             control,
                             sizeof (RingIO_ControlStruct)) ;
        }

        /* Make sure that no data buffers or attribute buffers are acquired
         * before allowing the client to close
         */
        if ((!client->isValid) || (client->acqSize) || (client->acqAttrSize)) {
            status = RINGIO_EFAILURE ;
            SET_FAILURE_REASON ;
        }
        else {
            /*Enter the Global ringio mpcs lock */
            status = MPCS_enter (ringIoState->mpcsObj) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                status = _RingIO_acquireLock (client) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                else {
                    client->isValid = FALSE;

                    /* Store local copy of virtLockHandle to
                     * ensure correct close of current MPCS handle
                     * in simultaneous open/close scenario.
                     */
                    virtLockHandle = client->virtLockHandle ;

                    if (   control->transportType
                        == (Uint32) RINGIO_TRANSPORT_GPP_DSP) {
                        if (IS_READER(client)) {
                            otherClient = &(control->writer) ;
                        }
                        else {
                            otherClient = &(control->reader) ;
                        }

                        /* Store value of notifyFunc to ensure NOTIFY_unregister
                         * outside lock.
                         */
                        notifyFunc                = client->notifyFunc ;

                        client->notifyFunc        = NULL ;
                        client->notifyParam       = NULL ;
                        client->notifyType        = RINGIO_NOTIFICATION_NONE ;
                        client->notifyWaterMark   = 0 ;
                        client->notifyFlag        = FALSE ;

                        if (   (POOL_getPoolNo( GET_CTRL_POOL_ID(entryId,
                                               control->procId))
                                )
                            != POOL_INVALIDID){
                            POOL_writeback (
                                GET_CTRL_POOL_ID(entryId, control->procId),
                                client,
                                sizeof (RingIO_Client)) ;
                        }

                        tmpStatus = _RingIO_releaseLock (client) ;
                        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }

                        /* Check if a notification was registered. */
                        if (notifyFunc != NULL) {
                            /* Unregister any registered notification. */
                            status = NOTIFY_unregister (otherClient->procId,
                                    ringIoState->ctrlPtr->ipsId,
                                    ringIoState->ctrlPtr->ipsEventNo,
                                    &RingIO_notifyCallback,
                                    client) ;
                            DBC_Assert (DSP_SUCCEEDED (status)) ;
                        }
                    }
                    else {
                        client->notifyFunc        = NULL ;
                        client->notifyParam       = NULL ;
                        client->notifyType        = RINGIO_NOTIFICATION_NONE ;
                        client->notifyWaterMark   = 0 ;
                        client->notifyFlag        = FALSE ;

                        if (   (POOL_getPoolNo(
                                    GET_CTRL_POOL_ID(entryId, control->procId))
                               )
                            != POOL_INVALIDID){
                            POOL_writeback (
                                GET_CTRL_POOL_ID(entryId, control->procId),
                                client,
                                sizeof (RingIO_Client)) ;
                        }

                        tmpStatus = _RingIO_releaseLock (client) ;
                        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }

                    /* Close the MPCS object. */
                    if (   control->transportType
                        == (Uint32) RINGIO_TRANSPORT_GPP_DSP) {
                        /* Close the MPCS object. */
                        tmpStatus = MPCS_close (ringIoState->ctrlPtr->dspId,
                                                (MPCS_Handle) virtLockHandle) ;
                        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
                }

                /* Leave the Global ringio mpcs lock */
                tmpStatus = MPCS_leave (ringIoState->mpcsObj) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }

            }
       }
    }

    status = MAP_STATUS (status) ;

    TRC_1LEAVE ("RingIO_close", status) ;

    return status ;
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
EXPORT_API
DSP_STATUS
RingIO_acquire (IN     RingIO_Handle   handle,
                OUT    RingIO_BufPtr * dataBuf,
                IN OUT Uint32 *        size)
{
    DSP_STATUS           status    = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client*       client    = (RingIO_Client*) handle ;
    RingIO_Object *      ringIoState = NULL ;
    RingIO_ControlHandle control = NULL;
    Uint32               entryId ;

    TRC_3ENTER ("RingIO_acquire", handle, dataBuf, size) ;

    DBC_Require (handle  != NULL) ;
    DBC_Require (dataBuf != NULL) ;
    DBC_Require (size    != NULL) ;

    if ((handle == NULL) || (dataBuf == NULL) || (size == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        control = client->virtControlHandle ;
        DBC_Assert (control != NULL) ;
        ringIoState = &RingIO_State [control->procId] ;
        DBC_Assert (ringIoState != NULL) ;
        if (ringIoState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        /* Nothing to be done if zero size buffer is being acquired. */
        if (*size != 0) {
            status = _RingIO_acquireLock (client) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                entryId = GET_ENTRY_ID (control) ;
                if (  POOL_getPoolNo(GET_CTRL_POOL_ID(entryId, control->procId))
                    != POOL_INVALIDID){
                    POOL_invalidate (GET_CTRL_POOL_ID(entryId, control->procId),
                                     control,
                                     sizeof (RingIO_ControlStruct)) ;
                }

                if (!client->isValid) {
                    status = RINGIO_EFAILURE ;
                    SET_FAILURE_REASON ;
                }
                else {
                    if (client->openMode ==  (Uint32) RINGIO_MODE_WRITE) {
                        status = _RingIO_writerAcquire (client, dataBuf, size) ;
                    }
                    else {
                        status = _RingIO_readerAcquire (client, dataBuf, size) ;
                    }
                }

                if (  POOL_getPoolNo(GET_CTRL_POOL_ID(entryId, control->procId))
                    != POOL_INVALIDID){
                    POOL_writeback (GET_CTRL_POOL_ID(entryId, control->procId),
                                    control,
                                    sizeof (RingIO_ControlStruct)) ;
                }
                tmpStatus = _RingIO_releaseLock (client) ;
                if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
        }
    }

    status = MAP_STATUS (status) ;

    TRC_1LEAVE ("RingIO_acquire", status) ;

    return status ;
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
EXPORT_API
DSP_STATUS
RingIO_release (IN RingIO_Handle handle, IN Uint32 size)
{
    DSP_STATUS           status     = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus  = RINGIO_SUCCESS ;
    DSP_STATUS           lockStatus = RINGIO_SUCCESS ;
    RingIO_Client *      client     = (RingIO_Client*) handle ;
    RingIO_Object *      ringIoState      = NULL ;
    RingIO_ControlHandle control = NULL;
    Uint32               entryId ;

    TRC_2ENTER ("RingIO_release", handle, size) ;

    DBC_Require (handle != NULL) ;

    if (handle == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        control = client->virtControlHandle ;
        DBC_Assert (control != NULL) ;
        ringIoState = &RingIO_State [control->procId] ;
        DBC_Assert (ringIoState != NULL) ;
        if (ringIoState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        /* Nothing to be done if zero size buffer is being released. */
        if (size != 0) {
            status = _RingIO_acquireLock (client) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                entryId = GET_ENTRY_ID (control) ;
                if (  POOL_getPoolNo(GET_CTRL_POOL_ID(entryId, control->procId))
                    != POOL_INVALIDID){
                    POOL_invalidate (GET_CTRL_POOL_ID(entryId, control->procId),
                                     control,
                                     sizeof (RingIO_ControlStruct)) ;
                }

                if (!client->isValid) {
                    lockStatus = _RingIO_releaseLock (client) ;
                    DBC_Assert (DSP_SUCCEEDED (lockStatus)) ;
                    status = RINGIO_EFAILURE ;
                    SET_FAILURE_REASON ;
                }
                else {
                    if (IS_WRITER(client)) {
                        status = _RingIO_writerRelease (client, size) ;
                    }
                    else {
                        status = _RingIO_readerRelease (client, size) ;
                    }

                    if (DSP_SUCCEEDED (status)) {
                        tmpStatus = _RingIO_checkNotifier (client) ;
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }

                    if (   (POOL_getPoolNo(GET_CTRL_POOL_ID(entryId,
                                           control->procId))
                            )
                         != POOL_INVALIDID){
                        POOL_writeback (
                                     GET_CTRL_POOL_ID(entryId, control->procId),
                                     control,
                                     sizeof (RingIO_ControlStruct)) ;
                    }
                    lockStatus = _RingIO_releaseLock (client) ;
                    if (DSP_FAILED (lockStatus) && DSP_SUCCEEDED (status)) {
                        status = lockStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
                if (DSP_SUCCEEDED (status) && (tmpStatus != RINGIO_EFAILURE)) {
                    /* 0 = WaterMark_Notification_Message */
                    tmpStatus = _RingIO_callNotifier (client, 0) ;
                    if (DSP_FAILED (tmpStatus)) {
                        status = tmpStatus ;
                    }
                }
            }
        }
    }

    status = MAP_STATUS (status) ;

    TRC_1LEAVE ("RingIO_release", status) ;

    return status ;
}


/** ============================================================================
 *  @func   RingIO_cancel
 *
 *  @desc   Cancel previous acquires.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RingIO_cancel (RingIO_Handle handle)
{
    DSP_STATUS           status    = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client*       client    = (RingIO_Client*) handle ;
    RingIO_Object *      ringIoState      = NULL ;
    RingIO_ControlHandle control = NULL;
    Uint32               entryId ;

    TRC_0ENTER ("RingIO_cancel") ;
    DBC_Assert (handle  != NULL) ;

    if (handle == NULL) {
        status =  DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        control = client->virtControlHandle ;
        DBC_Assert (control != NULL) ;
        ringIoState = &RingIO_State [control->procId] ;
        DBC_Assert (ringIoState != NULL) ;
        if (ringIoState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = _RingIO_acquireLock (client) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            entryId = GET_ENTRY_ID(control) ;
            if (   POOL_getPoolNo(GET_CTRL_POOL_ID(entryId, control->procId))
                != POOL_INVALIDID){
                POOL_invalidate (GET_CTRL_POOL_ID(entryId, control->procId),
                                 control,
                                 sizeof (RingIO_ControlStruct)) ;
            }
            if (!client->isValid) {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON ;
            }
            else {
                if (client->openMode ==  (Uint32) RINGIO_MODE_WRITE){
                    status = _RingIO_writerCancel (client) ;
                }
                else{
                    status = _RingIO_readerCancel (client) ;
                }
            }

            if (   POOL_getPoolNo(GET_CTRL_POOL_ID(entryId, control->procId))
                != POOL_INVALIDID){
                POOL_writeback (GET_CTRL_POOL_ID(entryId, control->procId),
                                control,
                                sizeof (RingIO_ControlStruct)) ;
            }
            tmpStatus = _RingIO_releaseLock (client) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    status = MAP_STATUS (status);

    TRC_1LEAVE ("RingIO_cancel", status) ;

    return status ;
}


/** ============================================================================
 *  @func   RingIO_getAttribute
 *
 *  @desc   This function gets a fixed-size attribute from the attribute buffer.
 *          If an attribute is present, the attribute type and a related
 *          parameter are returned.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RingIO_getAttribute (IN  RingIO_Handle handle,
                     OUT Uint16 *      type,
                     OUT Uint32 *      param)
{
    DSP_STATUS status = RINGIO_SUCCESS ;

    TRC_3ENTER ("RingIO_getAttribute", handle, type, param) ;

    DBC_Require (handle  != NULL) ;
    DBC_Require (type    != NULL) ;
    DBC_Require (param   != NULL) ;

    /* No need to validate parameters since it's done in RingIO_getvAttribute */
    status = RingIO_getvAttribute (handle, type, param, NULL, NULL) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    status = MAP_STATUS (status) ;

    TRC_1LEAVE ("RingIO_getAttribute", status) ;

    return status ;
}


/** ============================================================================
 *  @func   RingIO_setAttribute
 *
 *  @desc   This function sets a fixed-size attribute at the offset provided in
 *          the acquired data buffer.
 *          If the offset is not in the range of the acquired data buffer, the
 *          attribute is not set, and an error is returned. One exception to
 *          this rule is when no data buffer has been acquired. In this case an
 *          attribute is set at the next data buffer offset that can be acquired
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RingIO_setAttribute (IN     RingIO_Handle handle,
                     IN     Uint32        offset,
                     IN     Uint16        type,
                     IN     Uint32        param)
{
    DSP_STATUS status = RINGIO_SUCCESS ;

    TRC_4ENTER ("RingIO_setAttribute", handle, offset, type, param) ;

    DBC_Require (handle != NULL) ;

    /* No need to validate parameters since it's done in RingIO_setvAttribute */
    status = RingIO_setvAttribute (handle, offset, type, param, NULL, 0) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    status = MAP_STATUS (status) ;

    TRC_1LEAVE ("RingIO_getAttribute", status) ;

    return status ;
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
EXPORT_API
DSP_STATUS
RingIO_getvAttribute (IN     RingIO_Handle handle,
                      OUT    Uint16 *      type,
                      OUT    Uint32 *      param,
                      IN     RingIO_BufPtr vptr,
                      IN OUT Uint32 *      size)
{
    RingIO_Client      * client    = (RingIO_Client *) handle ;
    DSP_STATUS           status    = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus = RINGIO_SUCCESS ;
    RingIO_Object *      ringIoState      = NULL ;
    RingIO_ControlHandle control = NULL;
    Uint32               entryId ;

    TRC_5ENTER ("RingIO_getvAttribute", handle, type, param, vptr, size) ;

    DBC_Require (handle  != NULL) ;
    DBC_Require (type    != NULL) ;
    DBC_Require (param   != NULL) ;

    /* vptr and size may be NULL if called from RingIO_getAttribute (). */
    if ((handle == NULL) || (type == NULL) || (param == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        control = client->virtControlHandle ;
        DBC_Assert (control != NULL) ;
        ringIoState = &RingIO_State [control->procId] ;
        DBC_Assert (ringIoState != NULL) ;
        if (ringIoState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = _RingIO_acquireLock (client) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            entryId = GET_ENTRY_ID(control) ;
            if (   POOL_getPoolNo(GET_CTRL_POOL_ID(entryId, control->procId))
                != POOL_INVALIDID){
                POOL_invalidate (GET_CTRL_POOL_ID(entryId, control->procId),
                                 control,
                                 sizeof (RingIO_ControlStruct)) ;
            }

            /* Make sure that it's not a writer
             * trying to read an attribute
             */
            if (IS_WRITER(client)) {
                status = DSP_EINVALIDARG ;
                SET_FAILURE_REASON ;
            }
            else{
                status = _RingIO_readerGetvAttribute (client,
                                                      type,
                                                      param,
                                                      vptr,
                                                      size) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }

            if (   POOL_getPoolNo(GET_CTRL_POOL_ID(entryId, control->procId))
                != POOL_INVALIDID){
                POOL_writeback (GET_CTRL_POOL_ID(entryId, control->procId),
                                control,
                                sizeof (RingIO_ControlStruct)) ;
            }
            tmpStatus = _RingIO_releaseLock (client) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    status = MAP_STATUS (status) ;

    TRC_1LEAVE ("RingIO_getvAttribute", status) ;

    return status ;
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
EXPORT_API
DSP_STATUS
RingIO_setvAttribute (IN     RingIO_Handle handle,
                      IN     Uint32        offset,
                      IN     Uint16        type,
                      IN     Uint32        param,
                      IN     RingIO_BufPtr pdata,
                      IN     Uint32        size)
{
    DSP_STATUS           status    = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client      * client    = (RingIO_Client *) handle ;
    RingIO_Object *      ringIoState      = NULL ;
    RingIO_ControlHandle control = NULL;
    Uint32               entryId ;

    TRC_6ENTER ("RingIO_setvAttribute",
                handle, offset, type, param, pdata, size) ;

    DBC_Require (handle  != NULL) ;

    if (handle == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        control = client->virtControlHandle ;
        DBC_Assert (control != NULL) ;
        ringIoState = &RingIO_State [control->procId] ;
        DBC_Assert (ringIoState != NULL) ;
        if (ringIoState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = _RingIO_acquireLock (client) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            entryId = GET_ENTRY_ID(control) ;
            if (   POOL_getPoolNo(GET_CTRL_POOL_ID(entryId, control->procId))
                != POOL_INVALIDID){
                POOL_invalidate (GET_CTRL_POOL_ID(entryId, control->procId),
                                 control,
                                 sizeof (RingIO_ControlStruct)) ;
            }

            /* Make sure that it's not a reader trying to set an attribute */
            if (IS_READER(client)) {
                status = DSP_EINVALIDARG ;
                SET_FAILURE_REASON ;
            }
            else {
                status = _RingIO_writerSetvAttribute (client,
                                                      offset,
                                                      type,
                                                      param,
                                                      pdata,
                                                      size) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }

            if (   POOL_getPoolNo(GET_CTRL_POOL_ID(entryId, control->procId))
                != POOL_INVALIDID){
                POOL_writeback (GET_CTRL_POOL_ID(entryId, control->procId),
                                control,
                                sizeof (RingIO_ControlStruct)) ;
            }
            tmpStatus = _RingIO_releaseLock (client) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    status = MAP_STATUS (status) ;

    TRC_1LEAVE ("RingIO_setvAttribute", status) ;

    return status ;
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
EXPORT_API
DSP_STATUS
RingIO_flush (IN  RingIO_Handle handle,
              IN  Bool          hardFlush,
              OUT Uint16 *      type,
              OUT Uint32 *      param,
              OUT Uint32 *      bytesFlushed)
{
    RingIO_Client      * client    = (RingIO_Client *) handle ;
    DSP_STATUS           status    = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus = RINGIO_SUCCESS ;
    DSP_STATUS           lockStatus = RINGIO_SUCCESS ;
    RingIO_Object *      ringIoState      = NULL ;
    RingIO_ControlHandle control = NULL;
    Uint32               entryId ;

    TRC_3ENTER ("RingIO_flush", handle, type, param) ;

    DBC_Require (handle  != NULL) ;
    DBC_Require (type    != NULL) ;
    DBC_Require (param   != NULL) ;
    DBC_Require (bytesFlushed != NULL) ;

    if ((handle == NULL)
        || (type == NULL)
        || (param == NULL)
        || (bytesFlushed == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        control = client->virtControlHandle ;
        DBC_Assert (control != NULL) ;
        ringIoState = &RingIO_State [control->procId] ;
        DBC_Assert (ringIoState != NULL) ;
        if (ringIoState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = _RingIO_acquireLock (client) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            entryId = GET_ENTRY_ID(control) ;
            if (   POOL_getPoolNo(GET_CTRL_POOL_ID(entryId, control->procId))
                != POOL_INVALIDID){
                POOL_invalidate (GET_CTRL_POOL_ID(entryId, control->procId),
                                 control,
                                 sizeof (RingIO_ControlStruct)) ;
            }

            /*  Before flush is called any acquired buffer should be released.
             *  This is TRUE for both the reader and the writer.
             */
            if (client->acqSize) {
                *type = RINGIO_INVALID_ATTR ;
                status =  RINGIO_EFAILURE ;
                SET_FAILURE_REASON ;
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

                    if (DSP_SUCCEEDED (status)) {
                        tmpStatus = _RingIO_checkNotifier (client) ;
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
                }

                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }

            if (   POOL_getPoolNo(GET_CTRL_POOL_ID(entryId, control->procId))
                != POOL_INVALIDID){
                POOL_writeback (GET_CTRL_POOL_ID(entryId, control->procId),
                                control,
                                sizeof (RingIO_ControlStruct)) ;
            }

            lockStatus = _RingIO_releaseLock (client) ;
            if (DSP_FAILED (lockStatus) && DSP_SUCCEEDED (status)) {
                status = lockStatus ;
                SET_FAILURE_REASON ;
            }

            if (    (DSP_SUCCEEDED (status))
                 && (tmpStatus != RINGIO_EFAILURE)
                 && (IS_READER(client) == TRUE)) {
                /* 0 = WaterMark_Notification_Message */
                tmpStatus = _RingIO_callNotifier (client, 0) ;
                if (DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                }
            }
        }
    }

    status = MAP_STATUS (status) ;

    TRC_1LEAVE ("RingIO_flush", status) ;

    return status ;
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
EXPORT_API
DSP_STATUS
RingIO_setNotifier (IN  RingIO_Handle      handle,
                    IN  RingIO_NotifyType  notifyType,
                    IN  Uint32             notifyWatermark,
                    IN  RingIO_NotifyFunc  notifyFunc,
                    IN  RingIO_NotifyParam notifyParam)
{
    DSP_STATUS           status    = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client *      client    = (RingIO_Client *) handle ;
    RingIO_Object *      ringIoState      = NULL ;
    RingIO_ControlHandle control   = NULL ;
    Bool                 isRegistered = FALSE ;
    RingIO_Client *      otherClient ;
    Uint32               entryId ;

    TRC_5ENTER ("RingIO_setNotifier",
                handle,
                notifyType,
                notifyWatermark,
                notifyFunc,
                notifyParam) ;

    DBC_Require (handle     != NULL) ;
    DBC_Require (notifyFunc != NULL) ;
    DBC_Require (   (notifyType == RINGIO_NOTIFICATION_NONE)
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
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        control = client->virtControlHandle ;
        DBC_Assert (control != NULL) ;
        ringIoState = &RingIO_State [control->procId] ;
        DBC_Assert (ringIoState != NULL) ;
        if (ringIoState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = _RingIO_acquireLock (client) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            if (notifyWatermark > control->dataBufSize) {
                tmpStatus = _RingIO_releaseLock (client) ;
                DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
                status = DSP_EINVALIDARG ;
                SET_FAILURE_REASON ;
            }
            else {
                entryId = GET_ENTRY_ID(control) ;

                if (   (POOL_getPoolNo(
                            GET_CTRL_POOL_ID(entryId, control->procId))
                        )
                     != POOL_INVALIDID){
                    POOL_invalidate (GET_CTRL_POOL_ID(entryId, control->procId),
                                     control,
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

                        client->notifyType      = notifyType ;
                        client->notifyWaterMark = notifyWatermark ;
                        client->notifyFunc      = NULL ;
                        client->notifyParam     = notifyParam ;

                        /* Check if a notification was registered. */
                        if (isRegistered == TRUE) {
                            /* Unregister any registered notification. */
                            status = NOTIFY_unregister (
                                               otherClient->procId,
                                               ringIoState->ctrlPtr->ipsId,
                                               ringIoState->ctrlPtr->ipsEventNo,
                                               &RingIO_notifyCallback,
                                               client) ;
                            DBC_Assert (DSP_SUCCEEDED (status)) ;
                        }

                        if (   (POOL_getPoolNo(
                                    GET_CTRL_POOL_ID(entryId, control->procId))
                                )
                            != POOL_INVALIDID){
                            POOL_writeback (
                                    GET_CTRL_POOL_ID(entryId, control->procId),
                                    control,
                                    sizeof (RingIO_ControlStruct)) ;
                        }
                    }
                    else {
                        status = RINGIO_EFAILURE ;
                        SET_FAILURE_REASON ;
                    }
                    tmpStatus = _RingIO_releaseLock (client) ;
                    if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }

                    if (DSP_SUCCEEDED (status)) {
                        if (otherClient->isValid) {
                            status = NOTIFY_register (
                                          otherClient->procId,
                                          ringIoState->ctrlPtr->ipsId,
                                          ringIoState->ctrlPtr->ipsEventNo,
                                          &RingIO_notifyCallback,
                                          client) ;
                            if (DSP_FAILED (status)) {
                                SET_FAILURE_REASON ;
                            }
                            if (DSP_SUCCEEDED (status)) {
                                /* Update the client structure with the
                                 * user supplied call back function
                                 */
                                client->notifyFunc = notifyFunc ;
                                if (   (POOL_getPoolNo(
                                                    GET_CTRL_POOL_ID(entryId,
                                                             control->procId))
                                        )
                                    != POOL_INVALIDID){
                                    POOL_writeback (
                                     GET_CTRL_POOL_ID(entryId, control->procId),
                                                client,
                                                sizeof (RingIO_Client)) ;
                                }
                            }
                        }
                        else {
                            status = RINGIO_EFAILURE ;
                            SET_FAILURE_REASON ;
                        }
                    }
                }
                else {
                    client->notifyType      = notifyType ;
                    client->notifyWaterMark = notifyWatermark ;
                    client->notifyFunc      = notifyFunc ;
                    client->notifyParam     = notifyParam ;

                    tmpStatus = _RingIO_releaseLock (client) ;
                    if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
            }
        }
    }

    status = MAP_STATUS (status) ;

    TRC_1LEAVE ("RingIO_setNotifier", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   RingIO_sendNotify
 *
 *  @desc   Send a notification to the other client manually.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
EXPORT_API
DSP_STATUS
RingIO_sendNotify (IN RingIO_Handle handle, IN RingIO_NotifyMsg msg)
{
    DSP_STATUS           status      = RINGIO_SUCCESS ;
    RingIO_Client *      client      = (RingIO_Client *) handle ;

    TRC_0ENTER ("RingIO_sendNotify") ;
    DBC_Assert (handle  != NULL) ;

    if (handle == NULL) {
        status =  DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else{
        status = _RingIO_callNotifier (client, msg) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    status = MAP_STATUS (status) ;

    TRC_1LEAVE ("RingIO_sendNotify", status) ;

    return status ;

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
EXPORT_API
Uint32
RingIO_getValidSize (IN     RingIO_Handle   handle )
{
    DSP_STATUS           status    = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client*       client    = NULL ;
    RingIO_Object *      ringIoState      = NULL ;
    Uint32               size      = 0;
    RingIO_ControlHandle control = NULL;
    Uint32               entryId ;

    DBC_Require (handle  != NULL) ;

    if (handle != NULL ) {
        client  = (RingIO_Client*) handle ;
        control = client->virtControlHandle ;
        DBC_Assert (control != NULL) ;
        ringIoState = &RingIO_State [control->procId] ;
        DBC_Assert (ringIoState != NULL) ;
        if (ringIoState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = _RingIO_acquireLock (client) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {

            if (!client->isValid) {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON ;
            }
            else {
                entryId = GET_ENTRY_ID (control) ;
                if (   (POOL_getPoolNo(
                                    GET_CTRL_POOL_ID(entryId, control->procId))
                        )
                    != POOL_INVALIDID){
                    POOL_invalidate (GET_CTRL_POOL_ID(entryId, control->procId),
                                     control,
                                     sizeof (RingIO_ControlStruct)) ;
                }
                /* Return the valid data size */
                size = control->validSize;
            }

            tmpStatus = _RingIO_releaseLock (client) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
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
EXPORT_API
Uint32
RingIO_getEmptySize (IN     RingIO_Handle   handle )
{
    DSP_STATUS           status    = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client*       client    = NULL ;
    RingIO_Object *      ringIoState      = NULL ;
    Uint32               size      = 0;
    RingIO_ControlHandle control = NULL;
    Uint32               entryId ;

    DBC_Require (handle  != NULL) ;

    if (handle != NULL ) {
        client  = (RingIO_Client*) handle ;
        control = client->virtControlHandle ;
        DBC_Assert (control != NULL) ;
        ringIoState = &RingIO_State [control->procId] ;
        DBC_Assert (ringIoState != NULL) ;
        if (ringIoState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = _RingIO_acquireLock (client) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {

            if (!client->isValid) {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON ;
            }
            else {
                entryId = GET_ENTRY_ID (control) ;
                if (   (POOL_getPoolNo(
                                    GET_CTRL_POOL_ID(entryId, control->procId))
                       )
                    != POOL_INVALIDID){
                    POOL_invalidate (GET_CTRL_POOL_ID(entryId, control->procId),
                                     control,
                                     sizeof (RingIO_ControlStruct)) ;
                }
                /* Return the empty size */
                size = control->emptySize;
            }

            tmpStatus = _RingIO_releaseLock (client) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
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
EXPORT_API
Uint32
RingIO_getValidAttrSize (IN     RingIO_Handle   handle )
{
    DSP_STATUS           status    = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client*       client    = NULL ;
    Uint32               size      = 0;
    RingIO_Object *      ringIoState      = NULL ;
    RingIO_ControlHandle control = NULL;
    Uint32               entryId ;

    DBC_Require (handle  != NULL) ;

    if (handle != NULL ) {
        client    = (RingIO_Client*) handle ;
        control = client->virtControlHandle ;
        DBC_Assert (control != NULL) ;
        ringIoState = &RingIO_State [control->procId] ;
        DBC_Assert (ringIoState != NULL) ;
        if (ringIoState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = _RingIO_acquireLock (client) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {

            if (!client->isValid) {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON ;
            }
            else {
                entryId = GET_ENTRY_ID (control) ;
                if (    (POOL_getPoolNo(
                                    GET_CTRL_POOL_ID(entryId, control->procId))
                        )
                     != POOL_INVALIDID){
                    POOL_invalidate (GET_CTRL_POOL_ID(entryId, control->procId),
                                     control,
                                     sizeof (RingIO_ControlStruct)) ;
                }
                /* Return the valid attrbute size */
                size = control->validAttrSize;
            }

            tmpStatus = _RingIO_releaseLock (client) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
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
EXPORT_API
Uint32
RingIO_getEmptyAttrSize (IN     RingIO_Handle   handle )
{
    DSP_STATUS           status    = RINGIO_SUCCESS ;
    DSP_STATUS           tmpStatus = RINGIO_SUCCESS ;
    RingIO_Client*       client    = NULL ;
    Uint32               size      = 0;
    RingIO_Object *      ringIoState      = NULL ;
    RingIO_ControlHandle control = NULL;
    Uint32               entryId ;

    DBC_Require (handle  != NULL) ;

    if (handle != NULL ) {
        client  = (RingIO_Client*) handle ;
        control = client->virtControlHandle ;
        DBC_Assert (control != NULL) ;
        ringIoState = &RingIO_State [control->procId] ;
        DBC_Assert (ringIoState != NULL) ;
        if (ringIoState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = _RingIO_acquireLock (client) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {

            if (!client->isValid) {
                status = RINGIO_EFAILURE ;
                SET_FAILURE_REASON ;
            }
            else {
                entryId = GET_ENTRY_ID (control) ;
                if (  POOL_getPoolNo(GET_CTRL_POOL_ID(entryId, control->procId))
                    != POOL_INVALIDID){
                    POOL_invalidate (GET_CTRL_POOL_ID(entryId,control->procId),
                                     control,
                                     sizeof (RingIO_ControlStruct)) ;
                }
                /* Return the empty attrbute size */
                size = control->emptyAttrSize;
            }

            tmpStatus = _RingIO_releaseLock (client) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

    }

    return (size) ;
}

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
