/** ============================================================================
 *  @file   mem.h
 *
 *  @path   $(DSPLINK)/gpp/src/osal/
 *
 *  @desc   Defines the interfaces and data structures for the
 *          sub-component MEM.
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


#if !defined (MEM_H)
#define MEM_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  MEM_DEFAULT
 *
 *  @desc   Default attributes for OS independent operations for memory
 *          allocation & de-allocation.
 *          OS dependent attributes shall be defined in file 'mem_os.h'.
 *  ============================================================================
 */
#define MEM_DEFAULT    NULL

/** ============================================================================
 *  @macro  FREE_PTR
 *
 *  @desc   Frees memory pointed to by ptr and sets it to NULL. Also returns
 *          status of MEM_Free function call
 *  ============================================================================
 */
#define FREE_PTR(ptr)  MEM_Free ((Pvoid *) ((Pvoid) &ptr), MEM_DEFAULT)


/** ============================================================================
 *  @name   MemMapInfo
 *
 *  @desc   Forward declaration for the OS specific structure containing memory
 *          mapping information.
 *  ============================================================================
 */
typedef struct MemMapInfo_tag MemMapInfo ;

/** ============================================================================
 *  @name   MemUnmapInfo
 *
 *  @desc   Forward declaration for the OS specific structure containing memory
 *          unmapping information.
 *  ============================================================================
 */
typedef struct MemUnmapInfo_tag MemUnmapInfo ;


/** ============================================================================
 *  @func   MEM_Initialize
 *
 *  @desc   Initializes the MEM sub-component.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Memory error occurred.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MEM_Initialize (Void) ;


/** ============================================================================
 *  @func   MEM_Finalize
 *
 *  @desc   Releases all resources used by this sub-component.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Memory error occurred.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MEM_Finalize (Void) ;


/** ============================================================================
 *  @func   MEM_Alloc
 *
 *  @desc   Allocates the specified number of bytes.
 *
 *  @arg    ptr
 *              Location where pointer to allocated memory will be kept .
 *  @arg    cBytes
 *              Number of bytes to allocate.
 *  @arg    arg
 *              Type of memory to allocate. MEM_DEFAULT should be used if there
 *              is no need of special memory.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *
 *  @enter  MEM must be initialized.
 *          ptr must be a valid pointer.
 *          cBytes must be greater than 0.
 *
 *  @leave  *ptr must be a valid pointer upon successful completion otherwise
 *          it must be NULL.
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MEM_Alloc (OUT Void ** ptr, IN Uint32 cBytes, IN OUT Pvoid arg) ;


/** ============================================================================
 *  @func   MEM_Calloc
 *
 *  @desc   Allocates the specified number of bytes and clears them by filling
 *          it with 0s.
 *
 *  @arg    ptr
 *              Location where pointer to allocated memory will be kept
 *  @arg    cBytes
 *              Number of bytes to allocate.
 *  @arg    arg
 *              Type of memory to allocate. MEM_DEFAULT should be used if there
 *              is no need of special memory.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *
 *  @enter  MEM must be initialized.
 *          ptr must be a valid pointer.
 *          cBytes must be greater than 0.
 *
 *  @leave  *ptr must be a valid pointer upon successful completion otherwise
 *          it must be NULL.
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MEM_Calloc (OUT Void ** ptr, IN Uint32 cBytes, IN OUT Pvoid arg) ;


/** ============================================================================
 *  @func   MEM_Free
 *
 *  @desc   Frees up the allocated chunk of memory.
 *
 *  @arg    ptr
 *              Pointer to pointer to start of memory to be freed.
 *  @arg    arg
 *              Type of memory allocated.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Memory error.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *
 *  @enter  MEM must be initialized.
 *          memBuf must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MEM_Free (IN Pvoid * ptr, IN Pvoid arg) ;


/** ============================================================================
 *  @func   MEM_Map
 *
 *  @desc   Maps a specified memory area into the GPP virtual space.
 *
 *  @arg    mapInfo
 *              Data required for creating the mapping.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EMEMORY
 *              Could not map the given memory address.
 *
 *  @enter  mapInfo pointer must be valid.
 *
 *  @leave  None
 *
 *  @see    MEM_Unmap
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MEM_Map (IN OUT MemMapInfo * mapInfo) ;


/** ============================================================================
 *  @func   MEM_Unmap
 *
 *  @desc   Unmaps the specified memory area.
 *
 *  @arg    unmapInfo
 *              Information required for unmapping a memory area.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *
 *  @enter  unmapInfo pointer must be valid.
 *
 *  @leave  None.
 *
 *  @see    MEM_Map
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MEM_Unmap (IN MemUnmapInfo * unmapInfo) ;

/** ============================================================================
 *  @func   MEM_Copy
 *
 *  @desc   Copies data between the specified memory areas.
 *
 *  @arg    dst
 *              Destination address
 *  @arg    src
 *              Source address
 *  @arg    len
 *              length of data to be coiped.
 *  @arg    endian
 *              Endianism
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MEM_Copy (IN Uint8 * dst, OUT Uint8 * src, IN Uint32 len, IN Endianism endian) ;


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   MEM_Debug
 *
 *  @desc   Prints debug information for MEM.
 *
 *  @arg    None
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
EXPORT_API
Void
MEM_Debug (Void) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MEM_H) */
