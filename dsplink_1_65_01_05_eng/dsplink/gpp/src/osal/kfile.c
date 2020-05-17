/** ============================================================================
 *  @file   kfile.c
 *
 *  @path   $(DSPLINK)/gpp/src/osal/
 *
 *  @desc   Implementation of sub-component KFILE.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation version 2.
 *  
 *  This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 *  whether express or implied; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *  ============================================================================
 */


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <osal.h>

/*  ----------------------------------- Generic Function            */
#include<gen_utils.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Identifier for sub-component.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_OSAL_KFILE


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_OSAL_KFILE, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   KFILE_IsInitialized
 *
 *  @desc   Flag to keep track of initialization of this subcomponent.
 *  ============================================================================
 */
Bool KFILE_IsInitialized = FALSE ;


/** ============================================================================
 *  @func   KFILE_Initialize
 *
 *  @desc   Initializes KFILE suc-component by allocating all resources.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
KFILE_Initialize ()
{
    DSP_STATUS status = DSP_SOK ;

    TRC_0ENTER ("KFILE_Initialize") ;

    DBC_Require (KFILE_IsInitialized == FALSE) ;

    KFILE_IsInitialized = TRUE ;

    TRC_1LEAVE ("KFILE_Initialize", status) ;

    return status ;
}


/** ============================================================================
 *  @func   KFILE_Finalize
 *
 *  @desc   Releases resources used by this sub-component.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
KFILE_Finalize ()
{
    DSP_STATUS status = DSP_SOK ;

    TRC_0ENTER ("KFILE_Finalize") ;

    DBC_Require (KFILE_IsInitialized == TRUE) ;

    KFILE_IsInitialized = FALSE ;

    TRC_1LEAVE ("KFILE_Finalize", status) ;

    return status ;
}


/** ============================================================================
 *  @func   KFILE_Open
 *
 *  @desc   Opens a file specified by name of file.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
KFILE_Open (IN CONST FileName          fileName,
            IN CONST Char8 *           mode,
            IN CONST KFILE_Interface * fnTable,
            OUT      KFileObject **    fileHandlePtr)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_3ENTER ("KFILE_Open", fileName, mode, fileHandlePtr) ;

    DBC_Require (KFILE_IsInitialized == TRUE) ;
    DBC_Require (fileName != NULL) ;
    DBC_Require (mode != NULL) ;
    DBC_Require (fileHandlePtr != NULL) ;

    if (   (fileName == NULL)
        || (fileHandlePtr == NULL)
        || (mode == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        *fileHandlePtr = NULL ;
        status = MEM_Alloc ((Void **) fileHandlePtr,
                            sizeof (KFileObject),
                            MEM_DEFAULT) ;

        if (DSP_SUCCEEDED (status)) {
            (*fileHandlePtr)->signature = SIGN_KFILE ;
            (*fileHandlePtr)->fnTable = (KFILE_Interface *) fnTable ;
            (*fileHandlePtr)->fileObj = NULL ;
            status = fnTable->kfileOpen (fileName,
                                         mode,
                                         &((*fileHandlePtr)->fileObj)) ;
            if (DSP_SUCCEEDED (status)) {
                (*fileHandlePtr)->isOpen = TRUE ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }

        /*  --------------------------------------------------------------------
         *  If the function call failed then free the object allocated before.
         *  --------------------------------------------------------------------
         */
        if (DSP_FAILED (status)) {
            (*fileHandlePtr)->signature = SIGN_NULL ;
            FREE_PTR (*fileHandlePtr) ;
        }
    }

    DBC_Ensure (   (   DSP_SUCCEEDED (status)
                    && IS_OBJECT_VALID (*fileHandlePtr, SIGN_KFILE))
                || DSP_FAILED (status)) ;

    DBC_Ensure (   DSP_SUCCEEDED (status)
                || (   DSP_FAILED (status)
                    && (fileHandlePtr != NULL)
                    && (*fileHandlePtr == NULL))) ;

    TRC_1LEAVE ("KFILE_Open", status) ;

    return status ;
}


/** ============================================================================
 *  @func   KFILE_Close
 *
 *  @desc   Closes a file handle.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
KFILE_Close (IN KFileObject * fileHandle)
{
    DSP_STATUS   status = DSP_SOK ;

    TRC_1ENTER ("KFILE_Close", fileHandle) ;

    DBC_Require (KFILE_IsInitialized == TRUE) ;
    DBC_Require (fileHandle != NULL) ;
    DBC_Require (IS_OBJECT_VALID (fileHandle, SIGN_KFILE)) ;
    DBC_Require ((fileHandle != NULL) && (fileHandle->isOpen == TRUE)) ;

    if (IS_OBJECT_VALID (fileHandle, SIGN_KFILE) == FALSE) {
        status = DSP_EPOINTER ;
        SET_FAILURE_REASON ;
    }
    else if (fileHandle->isOpen == FALSE) {
        status = DSP_EFILE ;
        SET_FAILURE_REASON ;
    }
    else {
        DBC_Assert (fileHandle->fnTable != NULL) ;
        status = fileHandle->fnTable->kfileClose (fileHandle->fileObj) ;
            if (DSP_SUCCEEDED (status)) {
                fileHandle->isOpen = FALSE ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        fileHandle->fileObj   = NULL ;
        fileHandle->signature = SIGN_NULL  ;
        FREE_PTR (fileHandle) ;
    }

    TRC_1LEAVE ("KFILE_Close", status) ;

    return status ;
}


/** ============================================================================
 *  @func   KFILE_Read
 *
 *  @desc   Reads a specified number of items of specified size
 *          bytes from file to a buffer.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
KFILE_Read (IN OUT  Char8 *       buffer,
            IN      Uint32        size,
            IN      Uint32        count,
            IN      KFileObject * fileHandle)
{
    DSP_STATUS status  = DSP_SOK ;

    TRC_4ENTER ("KFILE_Read", buffer, size, count, fileHandle) ;

    DBC_Require (KFILE_IsInitialized == TRUE) ;
    DBC_Require (fileHandle != NULL) ;
    DBC_Require (IS_OBJECT_VALID (fileHandle, SIGN_KFILE)) ;
    DBC_Require (buffer != NULL) ;
    DBC_Require ((fileHandle != NULL) && (fileHandle->isOpen == TRUE)) ;

    if (IS_OBJECT_VALID (fileHandle, SIGN_KFILE) == FALSE) {
        status = DSP_EPOINTER ;
        SET_FAILURE_REASON ;
    }
    else if (fileHandle->isOpen == FALSE) {
        status = DSP_EFILE ;
        SET_FAILURE_REASON ;
    }
    else if (buffer == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        DBC_Assert (fileHandle->fnTable != NULL) ;
        status = fileHandle->fnTable->kfileRead (buffer,
                                                 size,
                                                 count,
                                                 fileHandle->fileObj) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("KFILE_Read", status) ;

    return status ;
}


/** ============================================================================
 *  @func   KFILE_Seek
 *
 *  @desc   Repositions the file pointer according to specified arguments.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
KFILE_Seek (IN  KFileObject *   fileHandle,
            IN  Int32           offset,
            IN  KFILE_FileSeek  origin)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_3ENTER ("KFILE_Seek", fileHandle, offset, origin) ;

    DBC_Require (KFILE_IsInitialized == TRUE) ;
    DBC_Require (fileHandle != NULL) ;
    DBC_Require (IS_OBJECT_VALID (fileHandle, SIGN_KFILE)) ;
    DBC_Require ((fileHandle != NULL) && (fileHandle->isOpen == TRUE)) ;

    if (IS_OBJECT_VALID (fileHandle, SIGN_KFILE) == FALSE) {
        status = DSP_EPOINTER ;
        SET_FAILURE_REASON ;
    }
    else if (fileHandle->isOpen == FALSE) {
        status = DSP_EFILE ;
        SET_FAILURE_REASON ;
    }
    else {
        DBC_Assert (fileHandle->fnTable != NULL) ;
        status = fileHandle->fnTable->kfileSeek (fileHandle->fileObj,
                                                 offset,
                                                 origin) ;

        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("KFILE_Seek", status) ;

    return status ;
}


/** ============================================================================
 *  @func   KFILE_Tell
 *
 *  @desc   Returns the current file pointer position for the specified
 *          file handle.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
KFILE_Tell (IN  KFileObject * fileHandle,
            OUT Int32 *       pos)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("KFILE_Tell", fileHandle, pos) ;

    DBC_Require (KFILE_IsInitialized == TRUE) ;
    DBC_Require (fileHandle != NULL) ;
    DBC_Require (IS_OBJECT_VALID (fileHandle, SIGN_KFILE)) ;
    DBC_Require ((fileHandle != NULL) && (fileHandle->isOpen == TRUE)) ;
    DBC_Require (pos != NULL) ;

    if (IS_OBJECT_VALID (fileHandle, SIGN_KFILE) == FALSE) {
        status = DSP_EPOINTER ;
        SET_FAILURE_REASON ;
    }
    else if (fileHandle->isOpen == FALSE) {
        status = DSP_EFILE ;
        SET_FAILURE_REASON ;
    }
    else if (pos == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        DBC_Assert (fileHandle->fnTable != NULL) ;
        status = fileHandle->fnTable->kfileTell (fileHandle->fileObj,
                                                 pos) ;

        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    DBC_Ensure (   (DSP_SUCCEEDED (status) && (pos != NULL) && (*pos >= 0))
                || (DSP_FAILED (status))) ;

    TRC_1LEAVE ("KFILE_Tell", status) ;

    return status ;
}


/** ============================================================================
 *  @func   KFILE_GetSize
 *
 *  @desc   Return the size of file in Bytes.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
KFILE_GetSize (IN KFileObject * fileHandle, OUT Uint32 * size)
{
    TRC_2ENTER ("KFILE_GetSize", fileHandle, size) ;

    DBC_Require (KFILE_IsInitialized == TRUE) ;
    DBC_Require (fileHandle != NULL) ;
    DBC_Require (IS_OBJECT_VALID (fileHandle, SIGN_KFILE)) ;
    DBC_Require ((fileHandle != NULL) && (fileHandle->isOpen == TRUE)) ;

    DBC_Assert (fileHandle->fnTable != NULL) ;
    fileHandle->fnTable->kfileGetSize (fileHandle->fileObj, size) ;

    TRC_0LEAVE ("KFILE_GetSize") ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
