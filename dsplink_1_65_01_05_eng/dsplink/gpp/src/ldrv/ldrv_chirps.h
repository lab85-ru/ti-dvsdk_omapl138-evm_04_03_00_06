/** ============================================================================
 *  @file   ldrv_chirps.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Functions to manage list of LDRV_CHNL_Irp objects.
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


#if !defined (LDRV_CHIRPS_H)
#define LDRV_CHIRPS_H


/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>

/*  ----------------------------------- Generic Function              */
#include <list.h>

/*  ----------------------------------- Link Driver                   */
#include <ldrv_chnl.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   LDRV_CHIRPS_CreateList
 *
 *  @desc   Allocates a chirp list.
 *
 *  @arg    numIRPs
 *              Number of chirps to be allocated in the list.
 *
 *  @ret    NULL
 *              Error allocating list.
 *          non=NULL
 *              Pointer to allocated list.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
List *
LDRV_CHIRPS_CreateList (IN Uint32 numIRPs) ;


/** ============================================================================
 *  @func   LDRV_CHIRPS_FreeList
 *
 *  @desc   Deallocates a chirp list.
 *
 *  @arg    chirpList
 *              List to be freed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Error occurred while freeing the memory.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHIRPS_FreeList (IN List * chirpList) ;


/** ============================================================================
 *  @func   LDRV_CHIRPS_Alloc
 *
 *  @desc   Allocates a single chirp.
 *
 *  @arg    None
 *
 *  @ret    NULL
 *              Error allocating chirp.
 *          Non=NULL
 *              Pointer to allocated chirp.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
LDRV_CHNL_Irp *
LDRV_CHIRPS_Alloc (Void) ;


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_CHIRPS_Print
 *
 *  @desc   Prints information about a single CHIRP (Channel IO Request Packet).
 *
 *  @arg    chirp
 *              CHIRP to be printed.
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
LDRV_CHIRPS_Print (IN LDRV_CHNL_Irp * chirp) ;


/** ============================================================================
 *  @func   LDRV_CHIRPS_Debug
 *
 *  @desc   Prints debug information about a CHIRP (Channel IO Request Packet)
 *          List.
 *
 *  @arg    chirpList
 *              CHIRP List to be printed.
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
LDRV_CHIRPS_Debug (IN List * chirpList) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_CHIRPS_H) */
