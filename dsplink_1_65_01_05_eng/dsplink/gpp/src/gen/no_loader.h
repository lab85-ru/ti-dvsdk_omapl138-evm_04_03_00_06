/** ============================================================================
 *  @file   no_loader.h
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Implements the DSP Dummy Loader.
 *          This loader is used when the DSPLINK_NOLOAD_MODE and
 *          DSPLINK_NOBOOT_MODE is selected. In this mode am external entity
 *          GPP applocation or GPP Boot Loader or DSP Loader will pre load the
 *          DSP. Using COFF_load API the application will pass the entry point
 *          i.e. c_int00 to PROC_start API.
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


#if !defined (NO_LOADER_H)
#define NO_LOADER_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_loaderdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   NOLOADER_init
 *
 *  @desc   Initializes the context object for loading a base image file or
 *          a section. This function is required to be called before any other
 *          function is called from this sub component.
 *
 *  @arg    procId
 *              Processor Identifier for the target DSP.
 *  @arg    baseImage
 *              Identifier for the file.
 *  @arg    args
 *              Arguments for initialization of the loader.
 *  @arg    objCtx
 *              Location to receive the context object for the loader.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *
 *  @enter  procId must be valid.
 *          baseImage must not be NULL.
 *          contextObj must not be NULL.
 *
 *  @leave  None
 *
 *  @see    NOLOADER_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
NOLOADER_init (IN  ProcessorId      procId,
               IN  Pstr             baseImage,
               IN  LoaderInitArgs * args,
               OUT Pvoid *          objCtx) ;


/** ============================================================================
 *  @func   NOLOADER_exit
 *
 *  @desc   Deallocates the object(s) created by NOLOADER_init function
 *          and releases the context.
 *
 *  @arg    objCtx
 *              The context object obtained through NOLOADER_init.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFILE
 *              File is not open.
 *          DSP_EPOINTER
 *              Invalid file object.
 *
 *  @enter  objCtx must be valid.
 *
 *  @leave  None
 *
 *  @see    NOLOADER_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
NOLOADER_exit (IN  Pvoid objCtx) ;


/*  ============================================================================
 *  @func   NOLOADER_load
 *
 *  @desc   Gets the entry point of the loaded DSP executable.
 *
 *  @arg    procId
 *              Target DSP identifier where the base image is to be loaded.
 *  @arg    loaderObj
 *              This object is used to receive arguments from PMGR.
 *  @arg    argc
 *              Number of arguments to be passed to the base image upon start.
 *  @arg    argv
 *              Arguments to be passed to DSP main application.
 *  @arg    entryPt
 *              OUT argument for returning entry address for the executable.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *
 *  @enter  procId must be valid.
 *          baseImage must be a valid file identifier.
 *          entryAddress must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    PMGR_PROC_load
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
NOLOADER_load (IN  ProcessorId     procId,
               IN  LoaderObject *  loaderObj,
               IN  Uint32          argc,
               IN  Char8 **        argv,
               OUT Uint32 *        entryPt) ;


/*  ============================================================================
 *  @func   NOLOADER_loadSection
 *
 *  @desc   Loads a section from the DSP executable onto the DSP. PMGR_PROC_load
 *          calls this through the function pointer table.
 *
 *  @arg    procId
 *              DSP identifier.
 *  @arg    loaderObj
 *              This object is used to receive arguments from PMGR.
 *  @arg    sectId
 *              Identifier for section to load.
 *
 *  @ret    DSP_ENOTIMPL
 *              Function not implemented.
 *
 *  @enter  procId must be valid.
 *          baseImage must be a valid file identifier.
 *          sectId must be a valid section identifier.
 *
 *  @leave  None
 *
 *  @see    PMGR_PROC_load
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
NOLOADER_loadSection (IN  ProcessorId     procId,
                      IN  LoaderObject *  loaderObj,
                      IN  Uint32          sectId) ;


/** ============================================================================
 *  @func   NOLOADER_getSymbolAddress
 *
 *  @desc   Gets the DSP address corresponding to a symbol within a DSP
 *          executable currently loaded on the DSP.
 *
 *  @arg    procId
 *              DSP identifier.
 *  @arg    objCtx
 *              Context of the loader object.
 *  @arg    symbolName
 *              Name of the symbol to be looked up.
 *  @arg    dspAddr
 *              Location to receive the DSP address for the specified symbol.
 *
 *
 *  @ret    DSP_ENOTIMPL
 *              Function not implemented.
 *
 *  @enter  procId must be valid.
 *          objCtx must be valid pointer.
 *          symbolName must be valid pointer.
 *          dspAddr must be valid pointer.
 *
 *  @leave  On success, dspAddr contains a valid DSP address.
 *          On failure, dspAddr is NULL.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
NOLOADER_getSymbolAddress (IN   ProcessorId     procId,
                           IN   Void *          objCtx,
                           IN   Char8 *         symbolName,
                           OUT  Uint32 *        dspAddr) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (NO_LOADER_H) */
