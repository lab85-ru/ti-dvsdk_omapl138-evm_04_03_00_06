/** ============================================================================
 *  @file   coff.c
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Implements the DSP COFF loader.
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
#include <_bitops.h>
#include <_safe.h>
#include <loaderdefs.h>
#include <procdefs.h>
#include <dsp.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <kfile.h>
#include <mem.h>

/*  ----------------------------------- Generic Functions           */
#include <gen_utils.h>

#include <coff_int.h>
#include <coff.h>
#include <coff_55x.h>
#include <coff_64x.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID         ID_GEN_COFF

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define  SET_FAILURE_REASON   TRC_SetReason (status, FID_C_GEN_COFF, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */

/** ============================================================================
 *  @macro  IS_LOADABLE_SECTION
 *
 *  @desc   Macro to find if the section is lodable.
 *  ============================================================================
 */
#define IS_LOADABLE_SECTION(x) (!(   (((x).flags & (  SECT_NOLOAD     \
                                                  | SECT_DSECT        \
                                                  | SECT_BSS          \
                                                  | SECT_COPY)) > 0)  \
                                  || ((x).size == 0)                  \
                                  || ((x).fpRawData == 0)))

/** ============================================================================
 *  @macro  BIOS5_SECTION_IDENTIFIER
 *
 *  @desc   Section name which indicates DSP/BIOS version is 5.xx
 *  ============================================================================
 */
#define BIOS5_SECTION_IDENTIFIER  ".bios"

#if defined (DDSP_DEBUG)
/*  ============================================================================
 *  @func   COFF_debug
 *
 *  @desc   Prints the debug information of COFF sub=module.
 *
 *  @arg    obj
 *              The context object obtained through COFF_init.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Void
COFF_debug (IN COFF_Context *   obj) ;
#endif /* defined (DDSP_DEBUG) */


/** ----------------------------------------------------------------------------
 *  @func   COFF_seekToSectionHeader
 *
 *  @desc   Repositions the file position indicator to the section header.
 *
 *  @arg    fileObj
 *              Handle to the COFF file.
 *  @arg    sectIndex
 *              Section  Index.
 *  @arg    swap
 *              Flag to indicate that headers in this file are swapped.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_RANGE
 *              Seek error in file.
 *
 *  @enter  fileObj must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_seekToSectionHeader (IN KFileObject * fileObj,
                          IN Uint32        sectIndex,
                          IN Bool          swap) ;


/** ----------------------------------------------------------------------------
 *  @func   COFF_isSwapped
 *
 *  @desc   Checks if the fields of headers are stored as byte
 *          swapped values.
 *
 *  @arg    fileObj
 *              Handle to the COFF file.
 *  @arg    dspArch
 *              Architecture of the dsp.
 *  @arg    isSwapped
 *              OUT argument to contain if the COFF headers in file are swapped.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_RANGE
 *              Seek error in file.
 *
 *  @enter  fileObj must be a valid pointer.
 *          isSwapped must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_isSwapped (IN  KFileObject * fileObj,
                IN  DspArch       dspArch,
                OUT Bool *        isSwapped) ;


/** ----------------------------------------------------------------------------
 *  @func   COFF_isValidFile
 *
 *  @desc   Checks to indicate if the file data format is valid for the given
 *          architecture.
 *
 *  @arg    fileObj
 *              Handle to the COFF file.
 *  @arg    dspArch
 *              Architecture of the dsp.
 *  @arg    isValid
 *              OUT argument to contain if the file data format is valid
 *              for the given architecture.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_RANGE
 *              Seek error in file.
 *
 *  @enter  fileObj must be a valid pointer.
 *          isValid must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_isValidFile (IN  KFileObject * fileObj,
                  IN  DspArch       dspArch,
                  OUT Bool *        isValid) ;



/** ----------------------------------------------------------------------------
 *  @func   COFF_fillArgsBuffer
 *
 *  @desc   Fills up the specified buffer with arguments to be sent to DSP's
 *          "main" function for the given architecture.
 *
 *  @arg    dspArch
 *              Architecture of the dsp.
 *  @arg    argc
 *              Number of arguments to be passed.
 *  @arg    argv
 *              Argument strings to be passed.
 *  @arg    size
 *              Size of the '.args' section obtained from the COFF file.
 *  @arg    loadAddr
 *              Load address for the '.args' section.
 *  @arg    wordSize
 *              Wordsize on the target DSP.
 *  @arg    endian
 *              Endianism to be used while writing data.
 *  @arg    argsBuf
 *              Buffer to be filled with formatted argc and argv.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ESIZE
 *              Insufficient space in .args buffer to hold all the arguments.
 *          DSP_EMEMORY
 *              Out of memory error.
 *
 *  @enter  argc must be greater than 0.
 *          argv must be valid pointer.
 *          argsBuf must be a valid pointer.
 *          sizeBuf must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_fillArgsBuffer (IN  DspArch   dspArch,
                     IN  Uint32    argc,
                     IN  Char8 **  argv,
                     IN  Uint32    sectSize,
                     IN  Uint32    loadAddr,
                     IN  Uint32    wordSize,
                     IN  Endianism endian,
                     IN  Void *    argsBuf) ;


/** ----------------------------------------------------------------------------
 *  @func   COFF_getOptHeaderSize
 *
 *  @desc   Get the size of optional header in file. This function is used at
 *          many places to quickly seek to the desired field in file.
 *
 *  @arg    fileObj
 *              Handle to the COFF file.
 *  @arg    swap
 *              specifies whether the bytes need to be swapped.
 *  @arg    size
 *              OUT argument to contain the optional header size.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_RANGE
 *              Seek error in file.
 *
 *  @enter  fileObj must be a valid pointer.
 *          size must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getOptHeaderSize (IN  KFileObject * fileObj,
                       IN  Bool          swap,
                       OUT Int32 *       size) ;


/** ----------------------------------------------------------------------------
 *  @func   COFF_getSymTabDetails
 *
 *  @desc   Gets the details associated to the symbol table -
 *          i.e. number of symbols in the file and the offset of symbol table
 *          in file.
 *
 *  @arg    fileObj
 *              Handle to the COFF file.
 *  @arg    swap
 *              Specifies whether the bytes need to be swapped.
 *  @arg    offsetSymTab
 *              OUT argument to contain the offset of symbol table.
 *  @arg    numSymbols
 *              OUT argument to contain the number of symbols.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_RANGE
 *              Seek error in file.
 *
 *  @enter  fileObj must be a valid pointer.
 *          offsetSymTab must be a valid pointer.
 *          numSymbols must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getSymTabDetails (IN  KFileObject * fileObj,
                       IN  Bool          swap,
                       OUT Uint32 *      offsetSymTab,
                       OUT Uint32 *      numSymbols) ;


/** ----------------------------------------------------------------------------
 *  @func   COFF_getNumSections
 *
 *  @desc   Get the total number of sections in file.
 *
 *  @arg    fileObj
 *              Handle to the COFF file.
 *  @arg    swap
 *              specifies whether the bytes need to be swapped.
 *  @arg    numSections
 *              OUT argument to contain the number of sections.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_RANGE
 *              Seek error in file.
 *
 *  @enter  fileObj must be valid pointer.
 *          numSections must be valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getNumSections (IN  KFileObject * fileObj,
                     IN  Bool          swap,
                     OUT Uint32 *      numSections) ;


/** ----------------------------------------------------------------------------
 *  @func   COFF_getFileHeader
 *
 *  @desc   Gets the File Header information.
 *          The caller should allocate memory for file header.
 *
 *  @arg    obj
 *              The context object obtained through COFF_init.
 *  @arg    fileHeader
 *              OUT argument for containing file header information.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *          DSP_ERANGE
 *              File seek operation failed.
 *          DSP_EFILE
 *              File format not supported.
 *
 *  @enter  obj must be a valid pointer.
 *          fileHeader must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getFileHeader (IN  COFF_Context  *    obj,
                    OUT COFF_FileHeader *  fileHeader) ;

/** ----------------------------------------------------------------------------
 *  @func   COFF_getOptionalHeader
 *
 *  @desc   Gets the COFF file's optional header.
 *          The caller should allocate memory for optional header.
 *
 *  @arg    obj
 *              The context object obtained through COFF_init.
 *  @arg    optHeader
 *              OUT argument for containing optional header information.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *          DSP_ERANGE
 *              File seek operation failed.
 *          DSP_EFILE
 *              No optional header in file.
 *
 *  @enter  obj must be valid.
 *          optHeader must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getOptionalHeader (IN  COFF_Context   *  obj,
                        OUT COFF_OptHeader *  optHeader) ;


/** ----------------------------------------------------------------------------
 *  @func   COFF_getSectionHeader
 *
 *  @desc   Gets the header information for a section.
 *          The caller should allocate memory for section header.
 *
 *  @arg    sectIndex
 *              Section index.
 *  @arg    obj
 *              The context object obtained through COFF_init.
 *  @arg    sectHeader
 *              OUT argument containing section header.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *          DSP_ERANGE
 *              File seek operation failed.
 *          DSP_EFILE
 *              File format not supported.
 *
 *  @enter  obj must be valid
 *          sectHeader must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getSectionHeader (IN  Uint32                sectId,
                       IN  COFF_Context  *       obj,
                       OUT COFF_SectionHeader *  sectHeader) ;


/** ----------------------------------------------------------------------------
 *  @func   COFF_getDspBiosVersion
 *
 *  @desc   Gets the DspBios version from the executable.
 *
 *  @arg    obj
 *              The context object obtained through COFF_init.
 *  @arg    numSections
 *              The number of sections in the COFF file.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *          DSP_ERANGE
 *              File seek operation failed.
 *          DSP_EFILE
 *              File format not supported.
 *
 *  @enter  obj must be valid
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getDspBiosVersion (IN      COFF_Context  * obj,
                        IN      Uint32        numSections) ;


/** ----------------------------------------------------------------------------
 *  @func   COFF_getSectionData
 *
 *  @desc   Gets the data associated with a section.
 *          Memory for buffer should be allocated prior to invoking this
 *          function.
 *
 *  @arg    sectIndex
 *              Section index.
 *  @arg    obj
 *              The context object obtained through COFF_init.
 *  @arg    data
 *              IN OUT argument containing location into which data buffer
 *              associated with the section is present.
 *              In case of file-based loader, this is an IN argument, and in
 *              case of memory-based loader, it is an OUT argument.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *          DSP_ERANGE
 *              File seek operation failed.
 *          DSP_EFILE
 *              File format not supported.
 *
 *  @enter  obj must be a valid pointer.
 *          data must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getSectionData (IN     Uint32          sectId,
                     IN     COFF_Context  * obj,
                     IN OUT Char8 **        data) ;


/** ----------------------------------------------------------------------------
 *  @func   COFF_getString
 *
 *  @desc   Gets the string from string table if required. This function
 *          checks if the 'str' argument is a valid string, if not, it looks
 *          up the string in string-table.
 *          If the str argument is a 8 character symbol name padded with
 *          nulls, COFF_getString returns a reference to the symbol name
 *          being passed to it in the outStr variable (i.e. in this scenario
 *          outStr = str). Else it returns a pointer to the offset where
 *          the string is present in the string table.
 *
 *  @arg    str
 *              Contains the string or the string offset.
 *  @arg    obj
 *              The context object obtained through COFF_init.
 *  @arg    outStr
 *              OUT argument containing the string.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *          DSP_EFILE
 *              File format not supported.
 *          DSP_ERANGE
 *              File seek operation failed.
 *          DSP_EMEMORY
 *              Out of memory.
 *
 *  @enter  obj must be a valid pointer.
 *          str must be a valid pointer.
 *          outStr must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getString (IN  Char8 *         str,
                IN  COFF_Context *  obj,
                OUT Char8 **        outStr) ;


/** ----------------------------------------------------------------------------
 *  @func   COFF_getSymbolTable
 *
 *  @desc   Get the primary SymbolEntry for all the symbols in the coff file.
 *          Memory for the symbol table is allocated in this function.
 *
 *  @arg    obj
 *              The context object obtained through COFF_init.
 *  @arg    offsetSymTab
 *              IN argument which contains offset to symbol table.
 *  @arg    symTable
 *              OUT argument for holding the symbol table.
 *  @arg    numSymbols
 *              OUT argument for holding the actual number of distinct symbols
 *              present in the file.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *          DSP_EFILE
 *              File format not supported.
 *          DSP_ERANGE
 *              File seek operation failed.
 *          DSP_EMEMORY
 *              Out of memory.
 *
 *  @enter  obj must be a valid pointer.
 *          symTable must be a valid poitner.
 *          numSymbols must be a valid poitner.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getSymbolTable (IN  COFF_Context      * obj,
                     IN  Uint32              offsetSymTab,
                     OUT COFF_SymbolEntry ** symTable,
                     OUT Uint32           *  numSymbols) ;


/** ----------------------------------------------------------------------------
 *  @func   COFF_getStringTable
 *
 *  @desc   Get the string table for the coff file.
 *          Memory for the string table is allocated in this function.
 *
 *  @arg    obj
 *              The context object obtained through COFF_init.
 *  @arg    numSymbols
 *              IN argument which contains number of symbols.
 *  @arg    offsetSymTab
 *              IN argument for storing offset to symbol table.
 *  @arg    strTable
 *              OUT argument for holding the string table.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EINVALIDARG
 *              Failure due to invalid argument.
 *          DSP_EFILE
 *              File format not supported.
 *          DSP_ERANGE
 *              File seek operation failed.
 *          DSP_EMEMORY
 *              Out of memory.
 *
 *  @enter  obj must be a valid pointer.
 *          strTable must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getStringTable (IN  COFF_Context  *  obj,
                     IN  Uint32           numSymbols,
                     IN  Uint32           offsetSymTab,
                     OUT Char8        **  strTable) ;


/** ============================================================================
 *  @func   COFF_init
 *
 *  @desc   Initializes the context object for loading a base image file or
 *          a section. This function is required to be called before any other
 *          function is called from this sub component.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFF_init (IN  ProcessorId      procId,
           IN  Pstr             baseImage,
           IN  LoaderInitArgs * args,
           IN  Void *           objCtx)
{
    DSP_STATUS      status       = DSP_SOK ;
    DSP_STATUS      tmpStatus    = DSP_SOK ;
    Bool            isValidArch  = FALSE ;
    Bool            isSwapped    = FALSE ;
    COFF_Context *  obj          = (COFF_Context *) objCtx ;
    Uint32          offsetSymTab = 0 ;

    TRC_4ENTER ("COFF_init", procId, baseImage, args, objCtx) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (baseImage != NULL) ;
    DBC_Require (objCtx != NULL) ;

    obj->isSwapped  = FALSE ;
    obj->dspArch    = args->dspArch ;

    status = COFF_isValidFile (obj->fileObj, obj->dspArch, &isValidArch) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }
    else if (isValidArch == FALSE) {
        status = DSP_ECORRUPTFILE ;
        SET_FAILURE_REASON ;
    }
    else {
        status = COFF_isSwapped (obj->fileObj, obj->dspArch, &isSwapped) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else if (isSwapped == TRUE) {
            obj->isSwapped = TRUE ;
        }
    }

    /*  ------------------------------------------------------------------------
     *  Populate the string table.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status)) {
        status = COFF_getSymTabDetails (obj->fileObj,
                                        obj->isSwapped,
                                        &offsetSymTab,
                                        &(obj->numSymbols)) ;

        if (DSP_SUCCEEDED (status)) {
            if (obj->numSymbols != 0) {
                status = COFF_getStringTable (obj,
                                              obj->numSymbols,
                                              offsetSymTab,
                                              &(obj->strTab)) ;
            }
        }
    }

    /*  ------------------------------------------------------------------------
     *  Get the symbol table.
     *  ------------------------------------------------------------------------
     */
    if (DSP_SUCCEEDED (status))  {
        if (obj->numSymbols != 0) {
            status = COFF_getSymbolTable (obj,
                                          offsetSymTab,
                                          &(obj->symTab),
                                          &(obj->numSymbols)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    if (DSP_FAILED (status)) {
         if (obj != NULL) {
            /*  ----------------------------------------------------------------
             *  Free the symbol table.
             *  ----------------------------------------------------------------
             */
            if (obj->symTab != NULL) {
                tmpStatus = FREE_PTR (obj->symTab) ;
                if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }

            /*  ----------------------------------------------------------------
             *  Free the string table.
             *  ----------------------------------------------------------------
             */
            if (obj->strTab != NULL) {
                if (obj->isFileBased != COFF_MEM_LOADER) {
                    tmpStatus = FREE_PTR (obj->strTab) ;
                    if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    obj->strTab = NULL ;
                }
            }
         }
    }

    TRC_1LEAVE ("COFF_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   COFF_exit
 *
 *  @desc   Deallocates the object(s) created by COFF_init function and
 *          releases the context.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFF_exit (IN  Pvoid objCtx)
{
    DSP_STATUS     status    = DSP_SOK ;
    DSP_STATUS     tmpStatus = DSP_SOK ;
    COFF_Context * obj       = (COFF_Context *) objCtx ;

    DBC_Require (objCtx != NULL) ;

    TRC_1ENTER ("COFF_exit", objCtx) ;

    if (obj == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        obj->startAddr  = 0     ;
        obj->isSwapped  = FALSE ;

        /*  --------------------------------------------------------------------
         *  Free the symbol table.
         *  --------------------------------------------------------------------
         */
        if (obj->symTab != NULL) {
            tmpStatus = FREE_PTR (obj->symTab) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        /*  --------------------------------------------------------------------
         *  Free the string table.
         *  --------------------------------------------------------------------
         */
        if (obj->strTab != NULL) {
            if (obj->isFileBased != COFF_MEM_LOADER) {
                tmpStatus = FREE_PTR (obj->strTab) ;
                if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
            else {
                obj->strTab = NULL ;
            }
        }
    }

    TRC_1LEAVE ("COFF_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   COFF_load
 *
 *  @desc   Loads the Coff format file on the DSP. This is called by
 *          PMGR_PROC_load through the function pointer table.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFF_load (IN  ProcessorId     procId,
           IN  LoaderObject *  loaderObj,
           IN  Uint32          argc,
           IN  Char8 **        argv,
           OUT Uint32 *        entryPt)
{
    DSP_STATUS         status         = DSP_SOK ;
    DSP_STATUS         tempStatus     = DSP_SOK ;
    Int32              cmpResult      = -1      ;
    Bool               loadedArgs     = FALSE   ;
    Bool               argsSection    = FALSE   ;
    Char8 *            sectName       = NULL    ;
    Uint32             virtAddr       = ADDRMAP_INVALID ;
    Uint32             symbolFromStrTab [3]     ;
    COFF_Context *     obj                      ;
    COFF_FileHeader    fileHeader     = {0}     ;
    COFF_OptHeader     optHeader      = {0}     ;
    COFF_SectionHeader sectHeader               ;
    Uint32             i                        ;
    Uint32             virtualAddress           ;
    Uint32             sectHeaderSize           ;

    TRC_5ENTER ("COFF_load",
                procId,
                loaderObj,
                argc,
                argv,
                entryPt) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (loaderObj != NULL) ;
    DBC_Require (entryPt != NULL) ;
    DBC_Require (   ((argc == 0) && (argv == NULL))
                 || ((argc != 0) && (argv != NULL))) ;

    DBC_Assert (loaderObj->objCtx != NULL) ;

    obj = loaderObj->objCtx ;
    DBC_Assert (obj != NULL) ;

    status = COFF_getFileHeader (obj, &fileHeader) ;

    if (DSP_SUCCEEDED (status)) {
        status = COFF_getOptionalHeader (obj, &optHeader) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    status = COFF_getDspBiosVersion (obj, fileHeader.numSections) ;
    if (DSP_SUCCEEDED (status)) {
        for (i = 0 ;   (DSP_SUCCEEDED (status))
                    && (i < fileHeader.numSections) ; i++) {

            /*  --------------------------------------------------------------------
             *  A DSP executable image can contains some sections that
             *  are not-loadable.
             *  So check if the section is a loadable section and contains
             *  data to be written.
             *  --------------------------------------------------------------------
             */
            status = COFF_getSectionHeader (i, obj, &sectHeader) ;
            if (DSP_SUCCEEDED (status)) {
                if ((argc > 0) && (loadedArgs == FALSE)) {
                    MEM_Copy ((Uint8 *)&(symbolFromStrTab [0]),
                              (Uint8 *)&(sectHeader.name),
                              COFF_NAME_LEN,
                              Endianism_Default) ;
                    symbolFromStrTab [2] = '\0' ;

                    status = COFF_getString ((Char8 *) symbolFromStrTab,
                                             obj,
                                             &sectName) ;
                }
                if (DSP_SUCCEEDED (status)) {
                    if (IS_LOADABLE_SECTION (sectHeader)) {
                        sectHeader.isLoadSection = TRUE ;
                    }
                    else {
                        sectHeader.isLoadSection = FALSE ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }

            if ((DSP_SUCCEEDED (status)) && (TRUE == sectHeader.isLoadSection)) {
                /* -----------------------------------------------------------
                 * Memory allocation for all section data is done for COFF
                 * i.e. file based loader.
                 * -----------------------------------------------------------
                 */
                if (obj->isFileBased == COFF_LOADER) {
                    status = MEM_Alloc ((Void **) &(sectHeader.data),
                                        sectHeader.size,
                                        MEM_DEFAULT) ;
                }

                if (DSP_SUCCEEDED (status)) {
                    if ((argc > 0) && (loadedArgs == FALSE)) {
                    /* -----------------------------------------------------------
                     * Args processing
                     * -----------------------------------------------------------
                     */
                        status = GEN_Strcmp (sectName, ".args", &cmpResult) ;

                        if ((DSP_SUCCEEDED (status)) && (cmpResult == 0)) {
                            loadedArgs = TRUE ;
                            argsSection = TRUE ;
                            /* If the section name is .args then load
                             * the user arguments.
                             * For COFF SHM and COFF MEM
                             * allocate a section for the .args data.
                             */
                            if (   (obj->isFileBased == COFF_SHM_LOADER)
                                || (obj->isFileBased == COFF_MEM_LOADER)) {
                                status = MEM_Alloc ((Void **)&(sectHeader.data),
                                                    sectHeader.size,
                                                    MEM_DEFAULT) ;
                            }

                            /* Load args into an internally allocated buffer */
                            if (DSP_SUCCEEDED (status)) {
                                status = COFF_fillArgsBuffer (obj->dspArch,
                                             (Uint32) argc,
                                             argv,
                                             (Uint32) sectHeader.size,
                                             (Uint32) sectHeader.virtualAddress,
                                             (Uint32) loaderObj->maduSize,
                                             loaderObj->endian,
                                             sectHeader.data) ;
                                if (DSP_FAILED (status)) {
                                    SET_FAILURE_REASON ;
                                }
                            }

                            /* Write the args to memory for COFFSHM & COFFMEM */
                            if (    (DSP_SUCCEEDED (status))
                                 && (   (obj->isFileBased == COFF_SHM_LOADER)
                                     || (obj->isFileBased == COFF_MEM_LOADER))){
                                virtualAddress =
                                            (Uint32) sectHeader.virtualAddress ;
                                sectHeaderSize = (Uint32) sectHeader.size ;
                                status = (*(loaderObj->fnWriteDspMem))
                                        (procId,
                                         virtualAddress,
                                         loaderObj->endian,
                                         sectHeaderSize,
                                         (Uint8 *) ((Pvoid)
                                         sectHeader.data)) ;
                                if (DSP_FAILED (status)) {
                                    SET_FAILURE_REASON ;
                                }

                                tempStatus = FREE_PTR (sectHeader.data) ;
                                if (    DSP_FAILED (tempStatus)
                                    &&  DSP_SUCCEEDED (status)) {
                                    status = tempStatus ;
                                    SET_FAILURE_REASON ;
                                }
                            }
                        }
                        else if ((DSP_SUCCEEDED (status)) && (cmpResult != 0)) {
                            /* Write section data for sections other than args*/
                            if (obj->isFileBased == COFF_SHM_LOADER) {
                                virtualAddress = (Uint32)
                                                    sectHeader.virtualAddress ;
                                virtAddr = (*(loaderObj->fnAddrConvert))
                                                                (procId,
                                                                 virtualAddress,
                                                                 DspToGpp)  ;

                                /* Directly write section data in
                                 * DSP address space
                                 */
                                if (virtAddr != ADDRMAP_INVALID) {
                                    status    =
                                        COFF_getSectionData (i,
                                                             obj,
                                                          (Char8 **)&virtAddr) ;
                                    if (DSP_FAILED (status)) {
                                        SET_FAILURE_REASON ;
                                    }
                                }
                            }
                            else {
                                /* Write section data in buffer */
                                status = COFF_getSectionData
                                                          ( i,
                                                            obj,
                                                            &sectHeader.data) ;
                                if (DSP_FAILED (status)) {
                                    SET_FAILURE_REASON ;
                                }
                            }
                        }
                        else {
                            SET_FAILURE_REASON ;
                        }
                    }
                    else {
                        /* Optimization once .args processing is done */
                        if (obj->isFileBased == COFF_SHM_LOADER) {
                            virtualAddress = (Uint32)
                                                    sectHeader.virtualAddress ;
                            virtAddr = (*(loaderObj->fnAddrConvert))
                                                                (procId,
                                                                 virtualAddress,
                                                                 DspToGpp)  ;


                            /* Directly write section data in
                             * DSP address space
                             */
                            if (virtAddr != ADDRMAP_INVALID) {
                                status    =
                                    COFF_getSectionData (i,
                                                         obj,
                                                      (Char8 **)&virtAddr) ;
                                if (DSP_FAILED (status)) {
                                    SET_FAILURE_REASON ;
                                }
                            }
                        }
                        else {
                            /* Write section data in buffer */
                            status = COFF_getSectionData (i,
                                                          obj,
                                                          &sectHeader.data) ;

                            if (DSP_FAILED (status)) {
                                SET_FAILURE_REASON ;
                            }
                        }
                    }

                    /* Write section data from buffer to DSP memory space.
                     * For COFFSHM loader, data is written separately directly
                     * to memory earlier.
                     * Here, for COFF loader, write the data into DSP memory
                     * space.
                     * For COFFMEM loader, all sections except .args are written
                     * here. .args section is written earlier in a separate
                     * handling.
                     */
                    if (   (DSP_SUCCEEDED (status))
                        && (    (obj->isFileBased == COFF_LOADER)
                            ||  (   (obj->isFileBased == COFF_MEM_LOADER)
                                 && (argsSection == FALSE)))) {
                        virtualAddress = (Uint32) sectHeader.virtualAddress ;
                        sectHeaderSize = (Uint32) sectHeader.size ;
                        status = (*(loaderObj->fnWriteDspMem))
                                                 (procId,
                                                  virtualAddress,
                                                  loaderObj->endian,
                                                  sectHeaderSize,
                                                  (Uint8 *) ((Pvoid)
                                                  sectHeader.data)) ;

                        if (DSP_SUCCEEDED (status)) {
                            status = KFILE_Seek (obj->fileObj,
                                                sectHeaderSize,
                                                KFILE_SeekSet) ;
                            if (DSP_FAILED (status)) {
                                SET_FAILURE_REASON ;
                            }
                        }

                        if (DSP_FAILED (status)) {
                            SET_FAILURE_REASON ;
                        }
                    }
                    else if (argsSection == TRUE) {
                        argsSection = FALSE ;
                    }

                    if (obj->isFileBased == COFF_LOADER) {
                        tempStatus = FREE_PTR (sectHeader.data) ;
                        if (DSP_FAILED (tempStatus) && DSP_SUCCEEDED (status)) {
                            status = tempStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
        }
    }
    /* Close the file handle since it is not needed anymore. */
    if (obj->fileObj != NULL) {
        tempStatus = KFILE_Close (obj->fileObj) ;
        if (DSP_FAILED (tempStatus) && DSP_SUCCEEDED (status)) {
            status = tempStatus ;
            SET_FAILURE_REASON ;
        }
        obj->fileObj = NULL  ;
    }

    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }
    else {
        *entryPt = (Uint32) optHeader.entry ;
    }

    TRC_1LEAVE ("COFF_load", status) ;

    return status ;
}


/** ============================================================================
 *  @func   COFF_getSymbolAddress
 *
 *  @desc   Gets the DSP address corresponding to a symbol within a DSP
 *          executable currently loaded on the DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFF_getSymbolAddress (IN   ProcessorId     procId,
                       IN   Void *          objCtx,
                       IN   Char8 *         symbolName,
                       OUT  Uint32 *        dspAddr)
{
    DSP_STATUS         status     = DSP_SOK                 ;
    Int32              cmpResult  = -1                      ;
    COFF_SymbolEntry * symTab     = NULL                    ;
    Char8 *            symName    = NULL                    ;
    Uint32             symbolFromStrTab [3]                 ;
    COFF_Context *     obj                                  ;
    Uint32             numSymbols                           ;
    Uint32             i                                    ;

    TRC_4ENTER ("COFF_getSymbolAddress", procId, objCtx, symbolName, dspAddr) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (objCtx != NULL) ;
    DBC_Require (symbolName != NULL) ;
    DBC_Require (dspAddr    != NULL) ;

    if (    (!(IS_VALID_PROCID (procId)))
        ||  (objCtx     == NULL)
        ||  (symbolName == NULL)
        ||  (dspAddr    == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        *dspAddr = 0 ;

        obj = (COFF_Context *) objCtx ;
        symTab     = obj->symTab ;

        numSymbols = obj->numSymbols ;

        /* Check whether the configuration supports loading symbols. */
        if (symTab == NULL) {
            status = DSP_ENOTSUPPORTED ;
            SET_FAILURE_REASON ;
        }
        else {
            for (i = 0 ; (i < numSymbols) && (cmpResult != 0) ; i++) {
                /*  ------------------------------------------------------------
                 *  If the symbol name is a 8 character symbol name padded with
                 *  nulls, COFF_getString returns a reference to the symbol name
                 *  being passed to it. To account for null terminated string a
                 *  a copy is made to a local string and then passed to the
                 *  COFF_getString function.
                 *  ------------------------------------------------------------
                 */
                MEM_Copy ((Uint8 *)&(symbolFromStrTab [0]),
                          (Uint8 *)&(symTab[i].name),
                          COFF_NAME_LEN,
                          Endianism_Default) ;
                symbolFromStrTab [2] = '\0' ;

                /*  ------------------------------------------------------------
                 *  Get_String functions  symbol name from either symbol table
                 *  if symbol name is less than 8 characters or from the string
                 *  table. Compare with the given string to check match.
                 *  ------------------------------------------------------------
                 */
                status = COFF_getString ((Char8 *) symbolFromStrTab,
                                         obj,
                                         &symName) ;
                if (DSP_SUCCEEDED (status)) {
                    status = GEN_Strcmp (symName, symbolName, &cmpResult) ;
                    if (DSP_SUCCEEDED (status)) {
                        if (cmpResult == 0) {
                            *dspAddr = symTab [i].value ;
                        }
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
                }
            }

            if (DSP_SUCCEEDED (status) && ((*dspAddr) == ((Uint32) NULL))) {
                status = DSP_ENOTFOUND ;
            }
        }
    }

    TRC_1LEAVE ("COFF_getSymbolAddress", status) ;

    return status ;
}


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   COFF_debug
 *
 *  @desc   Prints the debug information of COFF sub=module.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
COFF_debug (IN COFF_Context *   obj)
{
    TRC_0ENTER ("COFF_debug") ;

    DBC_Require (obj != NULL) ;

    if (obj != NULL) {
        TRC_1PRINT (TRC_LEVEL1, "   fileObj  :    [0x%x]\n", obj->fileObj) ;
        TRC_1PRINT (TRC_LEVEL1, "   startAddr:    [0x%x]\n", obj->startAddr) ;
        TRC_1PRINT (TRC_LEVEL1, "   isSwapped:    [0x%x]\n", obj->isSwapped) ;
    }

    TRC_0LEAVE ("COFF_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


/** ============================================================================
 *  @func   COFF_loadSection
 *
 *  @desc   Loads a section from the DSP executable onto the DSP. This is called
 *          by PMGR_PROC_loadSection through the function pointer table.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFF_loadSection (IN  ProcessorId     procId,
                  IN  LoaderObject *  loaderObj,
                  IN  Uint32          sectId)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_3ENTER ("COFF_loadSection", procId, loaderObj, sectId) ;

    status = DSP_ENOTIMPL ;
    SET_FAILURE_REASON ;

    TRC_1LEAVE ("COFF_loadSection", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   COFF_getFileHeader
 *
 *  @desc   Gets the file header from the coff file.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getFileHeader (IN  COFF_Context *     obj,
                    OUT COFF_FileHeader *  fileHeader)
{
    DSP_STATUS    status  = DSP_SOK ;
    Bool          swap    = FALSE   ;
    KFileObject * fileObj = NULL    ;

    TRC_2ENTER ("COFF_getFileHeader", obj, fileHeader) ;

    DBC_Require (obj != NULL) ;
    DBC_Require (fileHeader != NULL) ;

    if ((obj == NULL) || (fileHeader == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        swap = obj->isSwapped ;

        DBC_Assert (obj->fileObj != NULL) ;
        fileObj = obj->fileObj ;
        /* Read the file header from the start of the file.*/
        status = KFILE_Seek (fileObj, 0, KFILE_SeekSet) ;
        if (DSP_SUCCEEDED (status)) {
            fileHeader->version           = (Uint16) COFF_read16 (fileObj, swap) ;
            fileHeader->numSections       = (Uint16) COFF_read16 (fileObj, swap) ;
            fileHeader->dateTime          = COFF_read32 (fileObj, swap) ;
            fileHeader->fpSymTab          = COFF_read32 (fileObj, swap) ;
            fileHeader->numSymTabEntries  = COFF_read32 (fileObj, swap) ;
            fileHeader->numBytesOptHeader = (Uint16) COFF_read16 (fileObj, swap) ;
            fileHeader->flags             = (Uint16) COFF_read16 (fileObj, swap) ;
            fileHeader->targetId          = (Uint16) COFF_read16 (fileObj, swap) ;
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("COFF_getFileHeader", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   COFF_getOptionalHeader
 *
 *  @desc   Gets the optional header information from the COFF file.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getOptionalHeader (IN  COFF_Context  *   obj,
                        OUT COFF_OptHeader *  optHeader)
{
    DSP_STATUS    status            = DSP_SOK ;
    Bool          swap              = FALSE   ;
    Int32         numBytesOptHeader = 0       ;
    KFileObject * fileObj           = NULL    ;

    TRC_2ENTER ("COFF_getOptionalHeader", obj, optHeader) ;

    DBC_Require (obj != NULL) ;
    DBC_Require (optHeader != NULL) ;

    if ((obj == NULL) || (optHeader == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        swap    = obj->isSwapped ;

        DBC_Assert (obj->fileObj != NULL) ;
        fileObj = obj->fileObj   ;
        status = COFF_getOptHeaderSize (fileObj, swap, &numBytesOptHeader) ;

        if (DSP_SUCCEEDED (status)) {
            if (numBytesOptHeader > 0) {
                /* Seek to optional header offset */
                status = KFILE_Seek (fileObj,
                                     SIZE_COFF_FILE_HEADER,
                                     KFILE_SeekSet) ;

                if (DSP_SUCCEEDED (status)) {
                    optHeader->magic          = COFF_read16 (fileObj, swap) ;
                    optHeader->version        = COFF_read16 (fileObj, swap) ;
                    optHeader->sizeExeCode    = COFF_read32 (fileObj, swap) ;
                    optHeader->sizeInitData   = COFF_read32 (fileObj, swap) ;
                    optHeader->sizeUninitData = COFF_read32 (fileObj, swap) ;
                    optHeader->entry          = COFF_read32 (fileObj, swap) ;
                    optHeader->addrExe        = COFF_read32 (fileObj, swap) ;
                    optHeader->addrInitData   = COFF_read32 (fileObj, swap) ;
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                status = DSP_EFILE ;
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("COFF_getOptionalHeader", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   COFF_getSectionHeader
 *
 *  @desc   Gets the header information for the specified section
 *          from the given coff file.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getSectionHeader (IN  Uint32                sectId,
                       IN  COFF_Context  *       obj,
                       OUT COFF_SectionHeader *  sectHeader)
{
    DSP_STATUS    status    = DSP_SOK ;
    Bool          swap      = FALSE   ;
    KFileObject * fileObj   = NULL    ;
    Int32         i                   ;

    TRC_3ENTER ("COFF_getSectionHeader", sectId, obj, sectHeader) ;

    DBC_Require (obj != NULL) ;
    DBC_Require (sectHeader != NULL) ;

    if ((obj == NULL) || (sectHeader == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        swap    = obj->isSwapped ;

        DBC_Assert (obj->fileObj != NULL) ;
        fileObj = obj->fileObj   ;
        status = COFF_seekToSectionHeader (fileObj, sectId, swap) ;
        if (DSP_SUCCEEDED (status)) {
            for (i = 0 ; i < COFF_NAME_LEN ; i++) {
                sectHeader->name [i] = COFF_read8 (fileObj) ;
            }
            sectHeader->physicalAddress = COFF_read32 (fileObj, swap) ;
            sectHeader->virtualAddress  = COFF_read32 (fileObj, swap) ;
            sectHeader->size            = COFF_read32 (fileObj, swap) ;
            sectHeader->fpRawData       = COFF_read32 (fileObj, swap) ;
            sectHeader->fpReloc         = COFF_read32 (fileObj, swap) ;
            sectHeader->fpLineNum       = COFF_read32 (fileObj, swap) ;
            sectHeader->numReloc        = (Uint32) COFF_read32 (fileObj, swap) ;
            sectHeader->numLine         = (Uint32) COFF_read32 (fileObj, swap) ;
            sectHeader->flags           = (Uint32) COFF_read32 (fileObj, swap) ;
            sectHeader->reserved        = (Uint16) COFF_read16 (fileObj, swap) ;
            sectHeader->memPageNum      = (Uint16) COFF_read16 (fileObj, swap) ;
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("COFF_getSectionHeader", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   COFF_getDspBiosVersion
 *
 *  @desc   Gets the DSP/BIOS version from the given coff file.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getDspBiosVersion (IN      COFF_Context  *   obj,
                        IN      Uint32            numSections)
{
    DSP_STATUS    status         = DSP_SOK ;
    Int32              cmpResult = -1      ;
    Int32              i                   ;
    Uint32             symbolFromStrTab [3] ;
    COFF_SectionHeader sectHeader               ;

    TRC_2ENTER ("COFF_getDspBiosVersion", obj, numSections) ;

    DBC_Require (obj != NULL) ;

    if (obj == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        /* Do a search only for DspArch_C64x */
        if (obj->dspArch == DspArch_C64x) {

            /* Set to DSP/BIOS 6.xx */
            obj->dspArch = DspArch_C64x_Bios6 ;

            for (i = 0 ;   (DSP_SUCCEEDED (status))
                        && (i < numSections) ; i++) {
                status = COFF_getSectionHeader (i, obj, &sectHeader) ;
                if (DSP_SUCCEEDED (status)) {
                    MEM_Copy ((Uint8 *)&(symbolFromStrTab [0]),
                              (Uint8 *)&(sectHeader.name),
                              COFF_NAME_LEN,
                              Endianism_Default) ;
                    symbolFromStrTab [2] = '\0' ;
                    /* If .bios is found the DSP/BIOS version is 5.xx.
                     * The call to COFF_getString has been removed as
                     * .bios will always fit in COFF_NAME_LEN and
                     * never go into the string table.
                     */
                    status = GEN_Strcmp ((Char8 *) symbolFromStrTab,
                                         BIOS5_SECTION_IDENTIFIER,
                                         &cmpResult) ;
                    if ((DSP_SUCCEEDED (status)) && (cmpResult == 0)) {
                        obj->dspArch = DspArch_C64x_Bios5 ;
                        break ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
        }
    }

    TRC_1LEAVE ("COFF_getDspBiosVersion", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   COFF_getSectionData
 *
 *  @desc   Gets the section data of the specified section from the COFF file.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getSectionData (IN     Uint32          sectId,
                     IN     COFF_Context  * obj,
                     IN OUT Char8 **        data)
{
    DSP_STATUS    status         = DSP_SOK ;
    Int32         sizeOffset     = 0       ;
    Bool          swap           = FALSE   ;
    KFileObject * fileObj        = NULL    ;
    Uint32        sectSize                 ;
    Uint32        sectDataOffset           ;

    TRC_3ENTER ("COFF_getSectionData", sectId, obj, data) ;

    DBC_Require (obj != NULL) ;
    DBC_Require (data != NULL) ;

    if ((obj == NULL) || (data == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else if ((obj->isFileBased != COFF_MEM_LOADER) && (*data == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        swap = obj->isSwapped ;

        DBC_Assert (obj->fileObj != NULL) ;
        fileObj = obj->fileObj   ;
        status = COFF_seekToSectionHeader (fileObj, sectId, swap) ;
        if (DSP_SUCCEEDED (status)) {
            /*  ------------------------------------------------------------
             *  sizeOffset is calculated by taking the length of
             *  section name, the section's physical address
             *  and its virtual address.
             *  ------------------------------------------------------------
             */
            sizeOffset =  (sizeof (Char8) * COFF_NAME_LEN)
                         + sizeof (Int32)
                         + sizeof (Int32) ;
            status = KFILE_Seek (fileObj, sizeOffset, KFILE_SeekCur) ;

            if (DSP_SUCCEEDED (status)) {
                sectSize       = (Uint32) COFF_read32 (fileObj, swap) ;
                sectDataOffset = (Uint32) COFF_read32 (fileObj, swap) ;

                status = KFILE_Seek (fileObj,
                                     sectDataOffset,
                                     KFILE_SeekSet) ;

                if (DSP_SUCCEEDED (status)) {
                    if (obj->isFileBased != COFF_MEM_LOADER) {
                        /* Read from file if the loader is file-based. */
                        status = KFILE_Read (*data,
                                             sizeof (Uint8),
                                             sectSize,
                                             fileObj) ;

                        if (DSP_FAILED (status)) {
                            SET_FAILURE_REASON ;
                        }
                    }
                    else {
                        *data = (Char8 *)
                                   (obj->imageInfo.fileAddr + sectDataOffset) ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("COFF_getSectionData", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   COFF_getString
 *
 *  @desc   Gets the null terminated string from string table if required.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getString (IN  Char8 *         str,
                IN  COFF_Context  * obj,
                OUT Char8 **        outStr)
{
    DSP_STATUS    status   = DSP_SOK ;
    Bool          swap     = FALSE   ;
    Uint32        strOffset          ;
    Uint32        offsetinStrTab     ;

    TRC_3ENTER ("COFF_getString", str, obj, outStr) ;

    DBC_Require (obj != NULL) ;
    DBC_Require (str != NULL) ;
    DBC_Require (outStr != NULL) ;

    if ((obj == NULL) || (str == NULL) || (outStr == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        *outStr = NULL ;

        /*  --------------------------------------------------------------------
         *  If the first four bytes of the 'string' are NULL - it indicates that
         *  string is present in the symbol table.
         *  --------------------------------------------------------------------
         */
        if (*((Uint32 *) ((Pvoid) str)) == 0) {
            swap = obj->isSwapped ;

            if (swap == TRUE) {
                strOffset = BYTESWAP_LONG (*((Uint32 *) (Pvoid) (str + 4))) ;
            }
            else {
                strOffset = *(Uint32 *) ((Pvoid) (str + 4)) ;
            }

            /*  ----------------------------------------------------------------
             *  Decrement the offset by 4 to account for the 4 bytes needed
             *  to store the size
             *  ----------------------------------------------------------------
             */
            strOffset -= 4;

            offsetinStrTab = ((Uint32)((Pvoid) obj->strTab) + strOffset) ;

            /* Return the offset in the string table */
            (*outStr)  = (Char8 *) (offsetinStrTab) ;
        }
        else {
            /* Return the 'str' argument itself */
            (*outStr)  = str ;
        }
    }

    TRC_1LEAVE ("COFF_getString", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   COFF_getSymbolTable
 *
 *  @desc   Gets the symbol table from the COFF file.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getSymbolTable (IN  COFF_Context *      obj,
                     IN  Uint32              offsetSymTab,
                     OUT COFF_SymbolEntry ** symTable,
                     OUT Uint32 *            numSymbols)
{
    DSP_STATUS         status             = DSP_SOK ;
    DSP_STATUS         tmpStatus          = DSP_SOK ;
    Bool               swap               = FALSE   ;
    KFileObject     *  fileObj            = NULL    ;
    Char8           *  symData            = NULL    ;
    Char8           *  symDataAddress     = NULL    ;
    COFF_SymbolEntry * coffSymbolEntry    = NULL    ;
    Uint32             stringTableStart             ;
    Uint32             i                            ;
    Uint32             j                            ;
    Uint8              temp8_1 ;
    Uint8              temp8_2 ;
    Uint8              temp8_3 ;
    Uint8              temp8_4 ;
    Uint32             temp ;

    TRC_4ENTER ("COFF_getSymbolTable", obj, offsetSymTab, symTable,
                                       numSymbols) ;

    DBC_Require (obj        != NULL) ;
    DBC_Require (numSymbols != NULL) ;
    DBC_Require (symTable   != NULL) ;

    if ((obj == NULL) || (numSymbols == NULL) || (symTable == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        *symTable = NULL ;
        swap      = obj->isSwapped ;

        DBC_Assert (obj->fileObj != NULL) ;
        fileObj   = obj->fileObj   ;

        /* Allocate memory for the symbol table. */
        status = MEM_Alloc ((Void **) symTable,
                            ((obj->numSymbols) * sizeof (COFF_SymbolEntry)),
                            MEM_DEFAULT) ;

        /* Read symbol table into memory */
        if (DSP_SUCCEEDED (status)) {
            status = KFILE_Seek (fileObj,
                                 offsetSymTab,
                                 KFILE_SeekSet) ;
            if (DSP_SUCCEEDED (status)) {
                /* Allocate memory for the symbol table. */
                status = MEM_Alloc ((Void **) &symData,
                                    ((*numSymbols) * SIZE_COFF_SYMBOL_ENTRY),
                                    MEM_DEFAULT) ;
               if (DSP_FAILED (status)) {
                   SET_FAILURE_REASON ;
               }
            }

            if (DSP_SUCCEEDED (status)) {
                /* Store start address of symbol table */
                symDataAddress   = symData ;
                stringTableStart =
                                 ((*numSymbols) * SIZE_COFF_SYMBOL_ENTRY)
                                 + offsetSymTab ;
                status = KFILE_Read (symData,
                                    ((*numSymbols) *
                                      SIZE_COFF_SYMBOL_ENTRY),
                                    READ_REC_SIZE,
                                    fileObj) ;
                if (DSP_SUCCEEDED (status)) {
                    i = 0 ;
                    while ((offsetSymTab < stringTableStart)
                            && DSP_SUCCEEDED (status)) {
                        coffSymbolEntry = (COFF_SymbolEntry *)
                                                         ((Pvoid) symData) ;

                        /*  ------------------------------------------------
                         *  File up entries of symbol table and account for
                         *  swapping
                         *  ------------------------------------------------
                         */
                        for (j = 0 ; j < COFF_NAME_LEN ; j++) {
                            (*symTable) [i].name [j] =
                                                  coffSymbolEntry->name[j] ;
                        }

                        temp8_1 = *((Uint8 *)
                                          ((Uint32) coffSymbolEntry + 8)) ;
                        temp8_2 = *((Uint8 *)
                                          ((Uint32) coffSymbolEntry + 9)) ;
                        temp8_3 = *((Uint8 *)
                                          ((Uint32) coffSymbolEntry + 10)) ;
                        temp8_4 = *((Uint8 *)
                                          ((Uint32) coffSymbolEntry + 11)) ;
                        temp = (Uint32) (    ((Uint32) temp8_4 << 24)
                                         |   ((Uint32) temp8_3 << 16)
                                         |   ((Uint32) temp8_2 << 8)
                                         |   ((Uint32) temp8_1)) ;
                        (*symTable) [i].value     = temp ;
                        (*symTable) [i].sectNum   =
                                                  coffSymbolEntry->sectNum ;
                        (*symTable) [i].type      =  coffSymbolEntry->type ;
                        (*symTable) [i].storage   =
                                                  coffSymbolEntry->storage ;
                        (*symTable) [i].numAuxEnt =
                                                coffSymbolEntry->numAuxEnt ;
                        if (swap == TRUE) {
                            (*symTable) [i].value =
                               BYTESWAP_LONG ((*symTable) [i].value) ;
                            (*symTable) [i].sectNum =
                               BYTESWAP_WORD ((*symTable) [i].sectNum) ;
                            (*symTable) [i].type =
                               BYTESWAP_WORD ((*symTable) [i].type) ;
                        }
                        symData      += SIZE_COFF_SYMBOL_ENTRY;
                        offsetSymTab += SIZE_COFF_SYMBOL_ENTRY ;

                        if ((*symTable) [i].numAuxEnt == 1) {
                            symData      += SIZE_COFF_SYMBOL_ENTRY ;
                            offsetSymTab += SIZE_COFF_SYMBOL_ENTRY ;
                        }
                        i++ ;
                    }
                    if (DSP_SUCCEEDED (status)) {
                        *numSymbols = i ;
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }

                /* Free temporary memory reserved for symbol table */
                if (symDataAddress != NULL) {
                    tmpStatus = FREE_PTR (symDataAddress) ;
                    if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
            }
            else {
                tmpStatus = FREE_PTR (*symTable) ;
                if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("COFF_getSymbolTable", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   COFF_getStringTable
 *
 *  @desc   Gets the String table from the COFF file.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getStringTable (IN  COFF_Context  *  obj,
                     IN  Uint32           numSymbols,
                     IN  Uint32           offsetSymTab,
                     OUT Char8        **  strTable)
{
    DSP_STATUS    status             = DSP_SOK ;
    DSP_STATUS    tmpStatus          = DSP_SOK ;
    Bool          swap               = FALSE   ;
    KFileObject * fileObj            = NULL    ;
    Uint32        sizeStringTable              ;
    Uint32        offsetStrTable               ;

    TRC_4ENTER ("COFF_getStringTable", obj, numSymbols, offsetSymTab,
                                       strTable) ;

    DBC_Require (obj        != NULL) ;
    DBC_Require (strTable   != NULL) ;

    if ((obj == NULL) || (strTable == NULL))  {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        swap    = obj->isSwapped ;

        DBC_Assert (obj->fileObj != NULL) ;
        fileObj = obj->fileObj   ;

        /* Get the offset of the string table. */
        offsetStrTable = ((numSymbols) * SIZE_COFF_SYMBOL_ENTRY)
                         + offsetSymTab ;

        /* Validate position into file and set seek pointer. */
        status = KFILE_Seek (fileObj, offsetStrTable, KFILE_SeekSet) ;
        if (DSP_SUCCEEDED (status)) {
            if (obj->isFileBased == COFF_MEM_LOADER) {
                sizeStringTable  = (Uint32) COFF_read32 (fileObj, swap) ;
                sizeStringTable -= 4 ;

                *strTable = (Char8 *)
                                 (obj->imageInfo.fileAddr + offsetStrTable + 4)  ;
                status = KFILE_Seek (obj->fileObj,
                                    sizeStringTable * READ_REC_SIZE,
                                    KFILE_SeekSet) ;
            }
            else {
                /*  ------------------------------------------------------------
                 *  Get the string table into memory. This will enable faster
                 *  access to the string name if the name is in the string table
                 *  ------------------------------------------------------------
                 */

                /*  ------------------------------------------------------------
                 *  Get the size of the string table. The first 4 bytes of the
                 *  string table indicate its size. Decrement the size of the
                 *  string table by 4 to account for the 4 bytes needed to store
                 *  the size
                 *  ------------------------------------------------------------
                 */
                sizeStringTable  = (Uint32) COFF_read32 (fileObj, swap) ;
                sizeStringTable -= 4 ;

                /* Allocate memory for string table */
                status = MEM_Alloc ((Void **) strTable, sizeStringTable,
                                     MEM_DEFAULT) ;
                if (DSP_SUCCEEDED (status)) {
                    /* Read string table into memory */
                    status = KFILE_Read (*strTable ,
                                          sizeStringTable,
                                          READ_REC_SIZE,
                                          fileObj) ;
                    if (DSP_FAILED (status)) {
                        tmpStatus = FREE_PTR (*strTable) ;
                        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
                }
                else {
                   SET_FAILURE_REASON ;
                }
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("COFF_getStringTable", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   COFF_seekToSectionHeader
 *
 *  @desc   Positions the file pointer to the start of section header.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_seekToSectionHeader (IN KFileObject * fileObj,
                          IN Uint32        sectIndex,
                          IN Bool          swap)
{
    DSP_STATUS status        = DSP_SOK ;
    Uint32     numSections   = 0       ;
    Int32      optHdrSize              ;
    Int32      sectHdrOffset           ;

    TRC_3ENTER ("COFF_seekToSectionHeader", fileObj, sectIndex, swap) ;

    DBC_Require (fileObj != NULL) ;

    status = COFF_getOptHeaderSize (fileObj, swap, &optHdrSize) ;
    if (DSP_SUCCEEDED (status)) {
        status = COFF_getNumSections (fileObj, swap, &numSections) ;

        DBC_Assert (sectIndex < numSections) ;

        if (sectIndex < numSections) {
            /* Get the offset where the section begins */
            sectHdrOffset = (Int32)  (  SIZE_COFF_FILE_HEADER
                                      + optHdrSize
                                      + (sectIndex * SIZE_COFF_SECTION_HEADER)) ;

            status = KFILE_Seek (fileObj, sectHdrOffset, KFILE_SeekSet) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            status = DSP_ERANGE ;
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("COFF_seekToSectionHeader", status) ;

    return status ;

}


/** ----------------------------------------------------------------------------
 *  @func   COFF_getOptHeaderSize
 *
 *  @desc   Gets the size (in bytes) of optional header.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getOptHeaderSize (IN KFileObject * fileObj, IN Bool swap, OUT Int32 * size)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_3ENTER ("COFF_getOptHeaderSize", fileObj, swap, size) ;

    DBC_Require (fileObj != NULL) ;
    DBC_Require (size != NULL) ;

    status = KFILE_Seek (fileObj,
                         SIZE_OPT_HDR_LOC,
                         KFILE_SeekSet) ;
    if (DSP_SUCCEEDED (status)) {
        *size = COFF_read16 (fileObj, swap) ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        TRC_1PRINT (TRC_LEVEL3, "Optional Header Size: %d\n", *size) ;
    }

    TRC_1LEAVE ("COFF_getOptHeaderSize", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   COFF_getSymTabDetails
 *
 *  @desc   Gets the information about symbol table in file.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getSymTabDetails (IN  KFileObject * fileObj,
                       IN  Bool          swap,
                       OUT Uint32 *      offsetSymTab,
                       OUT Uint32 *      numSymbols)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_4ENTER ("COFF_getSymTabDetails",
                fileObj,
                swap,
                offsetSymTab,
                numSymbols) ;

    DBC_Require (fileObj != NULL) ;
    DBC_Require (offsetSymTab != NULL) ;
    DBC_Require (numSymbols != NULL) ;

    status = KFILE_Seek (fileObj, SYMTAB_OFFSET, KFILE_SeekSet) ;
    if (DSP_SUCCEEDED (status)) {
        *offsetSymTab = (Uint32) COFF_read32 (fileObj, swap) ;
        *numSymbols   = (Uint32) COFF_read32 (fileObj, swap) ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        TRC_1PRINT (TRC_LEVEL3, "Number of Symbols: %d\n", *numSymbols) ;
    }

    TRC_1LEAVE ("COFF_getSymTabDetails", status) ;

    return status;
}


/** ----------------------------------------------------------------------------
 *  @func   COFF_getNumSections
 *
 *  @desc   Gets the number of section in the COFF file.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_getNumSections (IN  KFileObject * fileObj,
                     IN  Bool          swap,
                     OUT Uint32 *      numSections)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_3ENTER ("COFF_getNumSections", fileObj, swap, numSections) ;

    DBC_Require (fileObj != NULL) ;
    DBC_Require (numSections != NULL) ;

    status = KFILE_Seek (fileObj, NUM_SECT_OFFSET, KFILE_SeekSet) ;
    if (DSP_SUCCEEDED (status)) {
        *numSections = (Uint32) COFF_read16 (fileObj, swap) ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        TRC_1PRINT (TRC_LEVEL3, "Number of Sections: %d\n", *numSections) ;
    }

    TRC_1LEAVE ("COFF_getNumSections", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   COFF_isValidFile
 *
 *  @desc   Checks to indicate if the file data format is valid for the given
 *          architecture.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_isValidFile (IN  KFileObject * fileObj,
                  IN  DspArch       dspArch,
                  OUT Bool *        isValid)
{
    DSP_STATUS status  = DSP_SOK ;

    TRC_3ENTER ("COFF_isValidFile", fileObj, dspArch, isValid) ;

    DBC_Require (fileObj != NULL) ;
    DBC_Require (isValid != NULL) ;

    *isValid = FALSE ;
    switch (dspArch) {
    case DspArch_C55x:
        status = COFF_isValidFile_55x (fileObj, isValid) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        break ;

    case DspArch_C64x:
    case DspArch_C64x_Bios5:
    case DspArch_C64x_Bios6:
        status = COFF_isValidFile_64x (fileObj, isValid) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        break ;

    default:
        TRC_0PRINT (TRC_LEVEL7, "Invalid architecture specified.\n") ;
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
        break ;
    }

    TRC_1LEAVE ("COFF_isValidFile", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   COFF_isSwapped
 *
 *  @desc   Check to indicate if the file data is swapped.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_isSwapped (IN  KFileObject * fileObj,
                IN  DspArch       dspArch,
                OUT Bool *        isSwapped)
{
    DSP_STATUS status  = DSP_SOK ;

    TRC_3ENTER ("COFF_isSwapped", fileObj, dspArch, isSwapped) ;

    DBC_Require (fileObj != NULL)   ;
    DBC_Require (isSwapped != NULL) ;

    *isSwapped = FALSE ;
    switch (dspArch) {
    case DspArch_C55x:
        status = COFF_isSwapped_55x (fileObj, isSwapped) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        break ;

    case DspArch_C64x:
    case DspArch_C64x_Bios5:
    case DspArch_C64x_Bios6:
        status = COFF_isSwapped_64x (fileObj, isSwapped) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        break ;

    default:
        TRC_0PRINT (TRC_LEVEL7, "Invalid architecture specified.\n") ;
        status = DSP_ECORRUPTFILE ;
        SET_FAILURE_REASON ;
        break ;
    }

    TRC_1LEAVE ("COFF_isSwapped", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   COFF_fillArgsBuffer
 *
 *  @desc   Fills up the specified buffer with arguments to be sent to DSP's
 *          "main" function for the given architecture.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
COFF_fillArgsBuffer (IN  DspArch   dspArch,
                     IN  Uint32    argc,
                     IN  Char8 **  argv,
                     IN  Uint32    sectSize,
                     IN  Uint32    loadAddr,
                     IN  Uint32    wordSize,
                     IN  Endianism endian,
                     IN  Void *    argsBuf)
{
    DSP_STATUS status  = DSP_SOK ;

    TRC_6ENTER ("COFF_fillArgsBuffer", argc, argv, sectSize, loadAddr, wordSize,
                                       dspArch) ;

    DBC_Require (argc > 0) ;
    DBC_Require (argv != NULL) ;
    DBC_Require (argsBuf != NULL) ;

    switch (dspArch) {
    case DspArch_C55x:
        status = COFF_fillArgsBuffer_55x (argc,
                                          argv,
                                          sectSize,
                                          loadAddr,
                                          wordSize,
                                          endian,
                                          argsBuf) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        break ;

    case DspArch_C64x:
    case DspArch_C64x_Bios5:
    case DspArch_C64x_Bios6:
        status = COFF_fillArgsBuffer_64x (dspArch,
                                          argc,
                                          argv,
                                          sectSize,
                                          loadAddr,
                                          wordSize,
                                          endian,
                                          argsBuf) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        break ;

    default:
        TRC_0PRINT (TRC_LEVEL7, "Invalid architecture specified.\n") ;
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
        break ;
    }

    TRC_1LEAVE ("COFF_fillArgsBuffer", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
