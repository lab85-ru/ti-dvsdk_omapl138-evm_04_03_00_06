/** ============================================================================
 *  @file   coff_mem.h
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Defines memory-based COFF loader interface.
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


#if !defined (COFF_MEM_H)
#define COFF_MEM_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <_loaderdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   COFFMEM_init
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
 *          DSP_EFILE
 *              File not found.
 *          DSP_EMEMORY
 *              Out of memory error.
 *
 *  @enter  procId must be valid.
 *          baseImage must not be NULL.
 *          contextObj must not be NULL.
 *
 *  @leave  None
 *
 *  @see    COFFMEM_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFFMEM_init (IN  ProcessorId      procId,
              IN  Pstr             baseImage,
              IN  LoaderInitArgs * args,
              OUT Pvoid *          objCtx) ;


/** ============================================================================
 *  @func   COFFMEM_exit
 *
 *  @desc   Deallocates the object(s) created by COFFMEM_init function and
 *          releases the context.
 *
 *  @arg    objCtx
 *              The context object obtained through COFFMEM_init.
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
 *  @see    COFFMEM_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
COFFMEM_exit (IN  Pvoid objCtx) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (COFF_MEM_H) */
