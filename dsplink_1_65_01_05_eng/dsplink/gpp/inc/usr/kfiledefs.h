/** ============================================================================
 *  @file   kfiledefs.h
 *
 *  @path   $(DSPLINK)/gpp/inc/usr/
 *
 *  @desc   Defines data types and structures used by KFILE module.
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


#if !defined (KFILEDEFS_H)
#define KFILEDEFS_H

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   KFILE_Seek
 *
 *  @desc   Enumerates the values used for repositioning the
 *          file position indicator.
 *
 *  @field  KFILE_SeekSet
 *              Seek from beginning of file.
 *  @field  KFILE_SeekCur
 *              Seek from current position.
 *  @field  KFILE_SeekEnd
 *              Seek from end of file.
 *  ============================================================================
 */
typedef enum {
    KFILE_SeekSet = 0x00,
    KFILE_SeekCur = 0x01,
    KFILE_SeekEnd = 0x02
} KFILE_FileSeek ;


/** ============================================================================
 *  @name   FileName
 *
 *  @desc   Definition for identifying files.
 *  ============================================================================
 */
typedef Pstr FileName ;


/** ============================================================================
 *  @name   FnKfileOpen
 *
 *  @desc   Signature of the function for opening a handle to a KFILE file.
 *
 *  @arg    fileName
 *              Name of the file to be opened.
 *  @arg    mode
 *              Mode for opening the file. This argument is case-sensitive.
 *              Expected modes are: "r" for read, "w" for write and
 *              "a" for append.
 *  @arg    fileHandle
 *              Pointer to the file object.
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
 *  @enter  Subcomponent must be initialized.
 *          fileName must be valid.
 *          mode must be valid.
 *          fileHandle must be valid.
 *
 *  @leave  fileHandle contains the fileObject on success.
 *
 *  @see    FnKfileClose
 *  ============================================================================
 */
typedef DSP_STATUS (*FnKfileOpen) (IN CONST FileName       fileName,
                                   IN CONST Char8 *        mode,
                                   IN       Void **        fileHandlePtr) ;


/** ============================================================================
 *  @name   FnKfileClose
 *
 *  @desc   Signature of the function for closing a handle to a KFILE file.
 *
 *  @arg    fileHandle
 *             Handle of file to be closed, returned from KFILE_Open.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFILE
 *              File is not open.
 *          DSP_EPOINTER
 *              Invalid file object.
 *
 *  @enter  Subcomponent must be initialized.
 *          fileHandle must be a valid handle to a file opened earlier.
 *
 *  @leave  Memory allocated for fileHandle is freed.
 *
 *  @see    FnKfileOpen
 *  ============================================================================
 */
typedef DSP_STATUS (*FnKfileClose) (IN  Void * fileHandle) ;


/** ============================================================================
 *  @name   FnKfileRead
 *
 *  @desc   Signature of the function for reading from a KFILE file.
 *
 *  @arg    buffer
 *             Buffer in which the contents of file are read.
 *  @arg    size
 *             Size of each object to read from file.
 *  @arg    count
 *             Number of objects to read.
 *  @arg    fileHandle
 *             KFileObject to read from.
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
 *  @enter  Subcomponent must be initialized.
 *          fileHandle must be a valid file pointer opened earlier.
 *
 *  @leave  None
 *
 *  @see    FnKfileOpen
 *  ============================================================================
 */
typedef DSP_STATUS (*FnKfileRead) (OUT Char8 *       buffer,
                                   IN  Uint32        size,
                                   IN  Uint32        count,
                                   IN  Void *        fileHandle) ;


/** ============================================================================
 *  @name   FnKfileSeek
 *
 *  @desc   Signature of the function for repositioning the file pointer within
 *          a KFILE file.
 *
 *  @arg    fileHandle
 *              The fileObject to seek into.
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
 *  @enter  Subcomponent must be initialized.
 *          fileHandle must be a valid handle to a file opened earlier.
 *
 *  @leave  None
 *
 *  @see    FnKfileTell
 *  ============================================================================
 */
typedef DSP_STATUS (*FnKfileSeek) (IN  Void *          fileHandle,
                                   IN  Int32           offset,
                                   IN  KFILE_FileSeek  origin) ;


/** ============================================================================
 *  @name   FnKfileTell
 *
 *  @desc   Signature of the function for returning the current file pointer
 *          position within a KFILE file.
 *
 *  @arg    fileHandle
 *              The fileObject pointer.
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
 *  @enter  Subcomponent must be initialized.
 *          fileHandle must be a valid handle to a file opened earlier.
 *
 *  @leave  None
 *
 *  @see    FnKfileSeek
 *  ============================================================================
 */
typedef DSP_STATUS (*FnKfileTell) (IN  Void *        fileHandle,
                                   OUT Int32 *       pos) ;


/** ============================================================================
 *  @name   FnKfileGetSize
 *
 *  @desc   Signature of the function for returning the file size of the KFILE
 *          file.
 *
 *  @arg    fileHandle
 *              Handle to the file object.
 *  @arg    size
 *              Out argument for holding the file size.
 *
 *  @ret    None.
 *
 *  @enter  Subcomponent must be initialized.
 *          fileHandle must be a valid handle to a file opened earlier.
 *
 *  @leave  None
 *
 *  @see    FnKfileSeek
 *  ============================================================================
 */
typedef Void (*FnKfileGetSize) (IN  Void *        fileHandle,
                                OUT Uint32 *      size) ;


/** ============================================================================
 *  @name   KFILE_Interface
 *
 *  @desc   Structure containing interface functions exported by the KFILE
 *          OSAL subcomponent.
 *
 *  @field  kfileOpen
 *              Function pointer providing the abstraction to the KFILE module's
 *              open function
 *  @field  kfileClose
 *              Function pointer providing the abstraction to the KFILE module's
 *              close function.
 *  @field  kfileRead
 *              Function pointer providing the abstraction to the KFILE module's
 *              read function.
 *  @field  kfileSeek
 *              Function pointer providing the abstraction to the KFILE module's
 *              seek function.
 *  @field  kfileTell
 *              Function pointer providing the abstraction to the KFILE module's
 *              tell function.
 *  @field  kfileGetSize
 *              Function pointer providing the abstraction to the KFILE module's
 *              getSize function.
 *  ============================================================================
 */
typedef struct KFILE_Interface_tag {
    FnKfileOpen        kfileOpen ;
    FnKfileClose       kfileClose ;
    FnKfileRead        kfileRead ;
    FnKfileSeek        kfileSeek ;
    FnKfileTell        kfileTell ;
    FnKfileGetSize     kfileGetSize ;
} KFILE_Interface ;

/** ============================================================================
 *  @name   KFileObject
 *
 *  @desc   Definition of the KFILE object used by all KFILE functions.
 *
 *  @field  signature
 *              Signature of the KFILE object.
 *  @field  fnTable
 *              Pointer to the KFILE function table.
 *  @field  fileObj
 *              Pointer to the KFILE module-specific KFILE object.
 *  @field  isOpen
 *              Flag to track whether the file is isOpen.
 *
 *  @see    KFILE_Open ()
 *  ============================================================================
 */
typedef struct KFileObject_tag {
    Uint32             signature ;
    KFILE_Interface *  fnTable ;
    Void *             fileObj ;
    Bool               isOpen ;
} KFileObject ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif   /* !defined (KFILEDEFS_H) */
