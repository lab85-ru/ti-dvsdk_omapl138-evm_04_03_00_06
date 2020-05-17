/** ============================================================================
 *  @file   kfile_pseudo.c
 *
 *  @path   $(DSPLINK)/gpp/src/osal/
 *
 *  @desc   Implementation of sub-component KFILE based on the pseudo file
 *          system generated using file2array.pl.
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


/*  ----------------------------------- OS Specific Headers         */

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
#define  COMPONENT_ID  ID_OSAL_KFILE_PSEUDO


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_OSAL_KFILE_PSEUDO, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */

/** ============================================================================
 *  @name   FileAttrs
 *
 *  @desc   The FileAttrs gives information about the file for memory-buffer
 *          based implementation of the psuedo-file system.
 *
 *  @field  content
 *              Pointer to file contents.
 *  @field  size
 *              Size of file.
 *  ============================================================================
 */
typedef struct FileAttrs_tag {
    CONST Char8 * content ;
    Uint32        size    ;
} FileAttrs ;

/** ============================================================================
 *  @name   FileDetails
 *
 *  @desc   The FileDetails type definition defined for files to be included
 *          in Link image. This structure is defined by the file2array.pl
 *          script for Pseudo file system implementation.
 *
 *  @field  name
 *              Name of file.
 *  @field  content
 *              Pointer to file contents.
 *  @field  size
 *              Size of file.
 *  @field  modTime
 *              Time of last modification.
 *  ============================================================================
 */
typedef struct FileDetails_tag {
    CONST Char8 * name    ;
    CONST Char8 * content ;
    Uint32        size    ;
    Uint32        modTime ;
} FileDetails ;

#if defined (KFILE_PSEUDO)
/** ============================================================================
 *  @name   FileDirectory, FilesInDirectory
 *
 *  @desc   Externs defined for accessing the file system defined
 *          by file2array.pl.
 *  ============================================================================
 */
extern FileDetails FileDirectory [] ;
extern Int32       FilesInDirectory ;
#endif /* if defined (KFILE_PSEUDO) */

/** ============================================================================
 *  @name   KFILEPSEUDO_FileInfo
 *
 *  @desc   File information structure
 *  ============================================================================
 */
STATIC FileDetails KFILEPSEUDO_FileInfo ;

/** ============================================================================
 *  @name   KFILEPSEUDO_Object
 *
 *  @desc   Defines the KFILE file object structure for the KFILE-pseudo module.
 *
 *  @field  fileInfo
 *              Pointer to the FileDetail structure.
 *  @field  curPos
 *              Current file position indicator.
 *
 *  @see    FileDetails
 *  ============================================================================
 */
typedef struct KFILEPSEUDO_Object_tag {
    FileDetails * fileInfo  ;
    Int32         curPos    ;
} KFILEPSEUDO_Object ;


/** ----------------------------------------------------------------------------
 *  @func   KFILEPSEUDO_open
 *
 *  @desc   Opens a file.
 *
 *  @arg    fileName
 *              Name of the file to be opened.
 *  @arg    mode
 *              Mode for opening the file. This argument is case-sensitive.
 *              Expected modes are: "r" for read, "w" for write, "a" for append
 *              and "m" for memory-based usage.
 *  @arg    fileHandlePtr
 *              Pointer to location where the file object pointer is to be set.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_EFILE
 *              File not found.
 *          DSP_EMEMORY
 *              Out of memory error.
 *
 *  @enter  fileName must be valid.
 *          mode must be valid.
 *          fileHandlePtr must be valid.
 *
 *  @leave  fileHandlePtr contains the file Object on success.
 *
 *  @see    KFILEPSEUDO_close
 *  ----------------------------------------------------------------------------
 */
STATIC
DSP_STATUS
KFILEPSEUDO_open (IN CONST FileName       fileName,
                  IN CONST Char8 *        mode,
                  IN       Void **        fileHandlePtr) ;


/** ----------------------------------------------------------------------------
 *  @func   KFILEPSEUDO_close
 *
 *  @desc   Closes a file handle.
 *
 *  @arg    fileHandle
 *             Handle of file to be closed, returned from the open call.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFILE
 *              File is not open.
 *          DSP_EPOINTER
 *              Invalid file object.
 *
 *  @enter  fileHandle must be a valid handle to a file opened earlier.
 *
 *  @leave  Memory allocated for fileHandle is freed.
 *
 *  @see    KFILEPSEUDO_open
 *  ----------------------------------------------------------------------------
 */
STATIC
DSP_STATUS
KFILEPSEUDO_close (IN Void * fileHandle) ;


/** ----------------------------------------------------------------------------
 *  @func   KFILEPSEUDO_read
 *
 *  @desc   Reads a specified number of items of specified size
 *          bytes from file to a buffer.
 *
 *  @arg    buffer
 *             Buffer in which the contents of file are read.
 *  @arg    size
 *             Size of each object to read from file.
 *  @arg    count
 *             Number of objects to read.
 *  @arg    fileHandle
 *             Pointer to file object to read from.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_EPOINTER
 *              Invalid file object.
 *          DSP_EFILE
 *              File is not open or error reading file.
 *          DSP_ERANGE
 *              The requested number of bytes is beyond EOF.
 *
 *  @enter  fileHandle must be a valid handle to a file opened earlier.
 *          buffer must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    KFILEPSEUDO_open
 *  ----------------------------------------------------------------------------
 */
STATIC
DSP_STATUS
KFILEPSEUDO_read (IN OUT  Char8 *       buffer,
                  IN      Uint32        size,
                  IN      Uint32        count,
                  IN      Void *        fileHandle) ;


/** ----------------------------------------------------------------------------
 *  @func   KFILEPSEUDO_seek
 *
 *  @desc   Repositions the file pointer according to specified arguments.
 *
 *  @arg    fileHandle
 *              Pointer to the file object to seek into.
 *  @arg    offset
 *              Offset for positioning the file pointer.
 *  @arg    origin
 *              Origin for calculating absolute position where file pointer
 *              is to be positioned. This can take the following values:
 *                  KFILE_SeekSet
 *                  KFILE_SeekCur
 *                  KFILE_SeekEnd
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_EPOINTER
 *              Invalid file object.
 *          DSP_EFILE
 *              File is not opened.
 *          DSP_ERANGE
 *              Offset and origin combination is beyond file size range.
 *
 *  @enter  fileHandle must be a valid handle to a file opened earlier.
 *          origin must be a valid supported value.
 *
 *  @leave  None
 *
 *  @see    KFILEPSEUDO_tell
 *  ----------------------------------------------------------------------------
 */
STATIC
DSP_STATUS
KFILEPSEUDO_seek (IN  Void *          fileHandle,
                  IN  Int32           offset,
                  IN  KFILE_FileSeek  origin) ;


/** ----------------------------------------------------------------------------
 *  @func   KFILEPSEUDO_tell
 *
 *  @desc   Returns the current file pointer position for the specified
 *          file handle.
 *
 *  @arg    fileHandle
 *              Pointer to the file object.
 *  @arg    pos
 *              Out argument for holding the current file position
 *              indicator value.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_EPOINTER
 *              Invalid file object.
 *          DSP_EFILE
 *              file is not opened.
 *
 *  @enter  fileHandle must be a valid handle to a file opened earlier.
 *          pos must be a valid pointer
 *
 *  @leave  None
 *
 *  @see    KFILEPSEUDO_seek
 *  ----------------------------------------------------------------------------
 */
STATIC
DSP_STATUS
KFILEPSEUDO_tell (IN  Void *        fileHandle,
                  OUT Int32 *       pos) ;


/** ----------------------------------------------------------------------------
 *  @func   KFILEPSEUDO_getSize
 *
 *  @desc   Return the size of file in Bytes.
 *
 *  @arg    fileHandle
 *              Pointer to the file object.
 *  @arg    size
 *              Location where the size is to be returned.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_EFILE
 *              File not found.
 *          DSP_EMEMORY
 *              Out of memory error.
 *
 *  @enter  fileHandle must be valid.
 *
 *  @leave  None.
 *
 *  @see    KFILEPSEUDO_seek
 *  ----------------------------------------------------------------------------
 */
STATIC
Void
KFILEPSEUDO_getSize (IN Void * fileHandle, OUT Uint32 * size) ;


/** ============================================================================
 *  @name   KFILEPSEUDO_Interface
 *
 *  @desc   Definition of the interface table for KFILEPSEUDO module.
 *  ============================================================================
 */
KFILE_Interface KFILEPSEUDO_Interface =
{
    &KFILEPSEUDO_open,
    &KFILEPSEUDO_close,
    &KFILEPSEUDO_read,
    &KFILEPSEUDO_seek,
    &KFILEPSEUDO_tell,
    &KFILEPSEUDO_getSize
} ;


/** ----------------------------------------------------------------------------
 *  @func   KFILEPSEUDO_open
 *
 *  @desc   Opens a file.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
DSP_STATUS
KFILEPSEUDO_open (IN CONST FileName       fileName,
                  IN CONST Char8 *        mode,
                  IN       Void **        fileHandlePtr)
{
    DSP_STATUS           status = DSP_SOK ;
    KFILEPSEUDO_Object * fileObj = NULL ;
#if defined (KFILE_PSEUDO)
    Uint32               length = 0 ;
    Int32                index  = 0 ;
    Bool                 found  = FALSE ;
    Int32                match  = 0 ;
#endif /* defined (KFILE_PSEUDO) */

    TRC_3ENTER ("KFILEPSEUDO_Open", fileName, mode, fileHandlePtr) ;

    DBC_Require (fileName != NULL)            ;
    DBC_Require (mode != NULL)                ;
    DBC_Require (fileHandlePtr != NULL)          ;

#if defined (KFILE_PSEUDO)
    DBC_Require (FileDirectory != NULL) ;
    DBC_Require (FilesInDirectory > 0) ;

    if ((FileDirectory == NULL) || (FilesInDirectory == 0)) {
        status = DSP_EFILE ;
        SET_FAILURE_REASON ;
    }
#endif /* defined (KFILE_PSEUDO) */
    if (DSP_SUCCEEDED (status)) {
        if ((mode [0] != 'r') && (mode [0] != 'm')) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
    }


    if (DSP_SUCCEEDED (status)) {
        *fileHandlePtr = NULL ;

        /* Check if memory buffer based pseudo-file system is to be used. */
        if (mode [0] == 'm') {
            KFILEPSEUDO_FileInfo.content = ((FileAttrs *) fileName)->content ;
            KFILEPSEUDO_FileInfo.size    = ((FileAttrs *) fileName)->size ;
            KFILEPSEUDO_FileInfo.name    = NULL ;
            KFILEPSEUDO_FileInfo.modTime = 0 ;
        }
        else {
#if defined (KFILE_PSEUDO)
            status = GEN_Strlen (fileName, &length) ;
            if (length == 0) {
                status = DSP_EINVALIDARG ;
                SET_FAILURE_REASON ;
            }
            else {
                index = 0 ;
                while (     (found == FALSE)
                       &&   (index < FilesInDirectory)
                       &&   DSP_SUCCEEDED (status)) {
                    status = GEN_Strcmp (fileName,
                                         FileDirectory [index].name,
                                         &match) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                    else {
                        if (match == 0) {
                            found = TRUE ;
                        }
                        else {
                            index++ ;
                        }
                    }
                }
                if (DSP_SUCCEEDED (status) && (found == FALSE)) {
                    status = DSP_EFILE ;
                    SET_FAILURE_REASON ;
                }
            }
#else /* if defined (KFILE_PSEUDO) */
            /* CFG_MAP ensures that for 'r' mode, execution never comes at this
             * point
             */
#endif /* defined (KFILE_PSEUDO) */
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = MEM_Alloc ((Void **) &fileObj,
                            sizeof (KFILEPSEUDO_Object),
                            MEM_DEFAULT) ;

        if (DSP_SUCCEEDED (status)) {
            *fileHandlePtr = (Void *) fileObj ;
            if (mode [0] == 'm') {
                fileObj->fileInfo  = &KFILEPSEUDO_FileInfo ;
            }
            else {
#if defined (KFILE_PSEUDO)
                fileObj->fileInfo  = &FileDirectory [index] ;
#endif /* defined (KFILE_PSEUDO) */
            }
            fileObj->curPos    = 0 ;
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    DBC_Ensure (   DSP_SUCCEEDED (status)
                || (   DSP_FAILED (status)
                    && (fileHandlePtr != NULL)
                    && (*fileHandlePtr == NULL))) ;

    TRC_1LEAVE ("KFILEPSEUDO_Open", status) ;

    return status ;
}


/** ============================================================================
 *  @func   KFILEPSEUDO_close
 *
 *  @desc   Closes a file handle.
 *
 *  @modif  None
 *  ============================================================================
 */
STATIC
DSP_STATUS
KFILEPSEUDO_close (IN Void * fileHandle)
{
    DSP_STATUS           status  = DSP_SOK ;
    KFILEPSEUDO_Object * fileObj = NULL ;

    TRC_1ENTER ("KFILEPSEUDO_Close", fileHandle) ;

    DBC_Require (fileHandle != NULL) ;

    fileObj = (KFILEPSEUDO_Object *) fileHandle ;

    FREE_PTR (fileObj) ;

    DBC_Ensure (   (DSP_SUCCEEDED (status) && (fileObj == NULL))
                || (DSP_FAILED (status))) ;

    TRC_1LEAVE ("KFILEPSEUDO_Close", status) ;

    return status ;
}


/** ============================================================================
 *  @func   KFILEPSEUDO_read
 *
 *  @desc   Reads a specified number of items of specified size
 *          bytes from file to a buffer.
 *
 *  @modif  None
 *  ============================================================================
 */
STATIC
DSP_STATUS
KFILEPSEUDO_read (IN OUT  Char8 *       buffer,
                  IN      Uint32        size,
                  IN      Uint32        count,
                  IN      Void *        fileHandle)
{
    DSP_STATUS           status  = DSP_SOK ;
    KFILEPSEUDO_Object * fileObj = NULL ;
    Uint32               reqSize = 0 ;
    Uint32               i       = 0 ;

    TRC_4ENTER ("KFILEPSEUDO_Read", buffer, size, count, fileHandle) ;

    DBC_Require (fileHandle != NULL) ;
    DBC_Require (buffer != NULL) ;

    reqSize = size * count ;
    fileObj = (KFILEPSEUDO_Object *) fileHandle ;

    if ((fileObj->curPos + reqSize) > fileObj->fileInfo->size) {
        status = DSP_ERANGE ;
        SET_FAILURE_REASON  ;
    }
    else {
        for (i = 0; i < (size * count); i++) {
            buffer [i] = fileObj->fileInfo->content [fileObj->curPos + i] ;
        }

        fileObj->curPos += reqSize ; /* update fileObj pointer */
    }

    TRC_1LEAVE ("KFILEPSEUDO_Read", status) ;

    return status ;
}


/** ============================================================================
 *  @func   KFILEPSEUDO_seek
 *
 *  @desc   Repositions the file pointer according to specified arguments.
 *
 *  @modif  None
 *  ============================================================================
 */
STATIC
DSP_STATUS
KFILEPSEUDO_seek (IN  Void *          fileHandle,
                  IN  Int32           offset,
                  IN  KFILE_FileSeek  origin)
{
    DSP_STATUS           status  = DSP_SOK ;
    KFILEPSEUDO_Object * fileObj = NULL ;

    TRC_3ENTER ("KFILEPSEUDO_Seek", fileHandle, offset, origin) ;

    DBC_Require (fileHandle != NULL) ;
    DBC_Require (   (origin == KFILE_SeekSet)
                 || (origin == KFILE_SeekCur)
                 || (origin == KFILE_SeekEnd)) ;

    fileObj = (KFILEPSEUDO_Object *) fileHandle ;

    switch (origin) {
        case KFILE_SeekSet:
            if (   (offset < 0)
                || (((Uint32) (offset)) > fileObj->fileInfo->size)) {
                status = DSP_ERANGE ;
                SET_FAILURE_REASON ;
            }
            else {
                fileObj->curPos = offset ;
            }
            break ;

        case KFILE_SeekCur:
            if (   (  ((Uint32)(fileObj->curPos + offset))
                > fileObj->fileInfo->size)
                || ((fileObj->curPos + offset) < 0)) {
                status = DSP_ERANGE ;
                SET_FAILURE_REASON ;
            }
            else {
                fileObj->curPos += offset ;
            }
            break ;

        case KFILE_SeekEnd:
            /*  ----------------------------------------------------------------
             *  A negative offset indicates offset from the end of file.
             *  Check that the specified offset is not beyond
             *  the bounds of the file.
             *  ----------------------------------------------------------------
             */

            if (   (offset > 0)
                || (((Uint32)(-offset)) > fileObj->fileInfo->size)) {
                status = DSP_ERANGE ;
                SET_FAILURE_REASON ;
            }
            else {
                fileObj->curPos = (fileObj->fileInfo->size + offset) ;
            }
            break ;

        default:
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
            break ;
    }

    TRC_1LEAVE ("KFILEPSEUDO_Seek", status) ;

    return status ;
}


/** ============================================================================
 *  @func   KFILEPSEUDO_tell
 *
 *  @desc   Returns the current file pointer position for the specified
 *          file handle.
 *
 *  @modif  None
 *  ============================================================================
 */
STATIC
DSP_STATUS
KFILEPSEUDO_tell (IN  Void *        fileHandle,
                  OUT Int32 *       pos)
{
    DSP_STATUS           status  = DSP_SOK ;
    KFILEPSEUDO_Object * fileObj = NULL ;

    TRC_2ENTER ("KFILEPSEUDO_Tell", fileHandle, pos) ;

    DBC_Require (fileHandle != NULL) ;
    DBC_Require (pos != NULL) ;

    fileObj = (KFILEPSEUDO_Object *) fileHandle ;

    *pos = fileObj->curPos ;

    DBC_Ensure (   (DSP_SUCCEEDED (status) && (pos != NULL) && (*pos >= 0))
                || (DSP_FAILED (status))) ;

    TRC_1LEAVE ("KFILEPSEUDO_Tell", status) ;

    return status ;
}


/** ============================================================================
 *  @func   KFILEPSEUDO_getSize
 *
 *  @desc   Return the size of file in Bytes.
 *
 *  @modif  None
 *  ============================================================================
 */
STATIC
Void
KFILEPSEUDO_getSize (IN Void * fileHandle, OUT Uint32 * size)
{
    KFILEPSEUDO_Object * fileObj = NULL ;

    TRC_2ENTER ("KFILEPSEUDO_GetSize", fileHandle, size) ;

    DBC_Require (fileHandle != NULL) ;

    fileObj = (KFILEPSEUDO_Object *) fileHandle ;

    *size = fileObj->fileInfo->size ;

    TRC_0LEAVE ("KFILEPSEUDO_GetSize") ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
