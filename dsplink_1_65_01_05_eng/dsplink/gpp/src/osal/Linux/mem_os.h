/** ============================================================================
 *  @file   mem_os.h
 *
 *  @path   $(DSPLINK)/gpp/src/osal/Linux/
 *
 *  @desc   Defines the OS dependent attributes & structures for the
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


#if !defined (MEM_OS_H)
#define MEM_OS_H

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  MEM_KERNEL
 *
 *  @desc   Example memory type. One has to handle it in MEM_Alloc if one wants
 *          to implement it.
 *  ============================================================================
 */
#define MEM_KERNEL      GFP_KERNEL


/** ============================================================================
 *  @type   MemAttrs
 *
 *  @desc   OS dependent attributes for allocating memory.
 *
 *  @field  MEM_UNCACHED: Uncached memory
 *          MEM_CACHED: Cached memory
 *  ============================================================================
 */
typedef enum MemAttrs_tag {
    MEM_UNCACHED = 0u,
    MEM_CACHED
} MemAttrs ;

/** ============================================================================
 *  @type   MemAllocAttrs
 *
 *  @desc   OS dependent attributes for allocating memory.
 *
 *  @field  physicalAddress
 *              Physical address of the allocated memory.
 *  @field  bigArea
 *              Allocate a special area (e.g BIG PHYSICAL AREA).
 *  ============================================================================
 */
typedef struct MemAllocAttrs_tag {
    Uint32 *    physicalAddress ;
    Bool        bigArea         ;
} MemAllocAttrs ;

/** ============================================================================
 *  @type   MemFreeAttrs
 *
 *  @desc   OS dependent attributes for freeing memory.
 *
 *  @field  physicalAddress
 *              Physical address of the memory to be freed.
 *  @field  size
 *              Size of the memory to be freed.
 *  @field  bigArea
 *              Free a special area (e.g BIG PHYSICAL AREA).
 *  ============================================================================
 */
typedef struct MemFreeAttrs_tag {
    Uint32 *    physicalAddress ;
    Uint32      size ;
    Bool        bigArea         ;
} MemFreeAttrs ;


/** ============================================================================
 *  @type   MemMapInfo_tag
 *
 *  @desc   OS dependent definition of the information required for mapping a
 *          memory region.
 *
 *  @field  src
 *              Address to be mapped.
 *  @field  size
 *              Size of memory region to be mapped.
 *  @field  dst
 *              Mapped address.
 *  @field  memAttrs
 *              Mapped address attributes.
 *  ============================================================================
 */
struct MemMapInfo_tag {
    Uint32   src  ;
    Uint32   size ;
    Uint32   dst  ;
    MemAttrs memAttrs ;
} ;


/** ============================================================================
 *  @type   MemUnmapInfo_tag
 *
 *  @desc   OS dependent definition of the information required for unmapping
 *          a previously mapped memory region.
 *
 *  @field  addr
 *              Address to be unmapped. This is the address returned as 'dst'
 *              address from a previous call to MEM_Map () in the MemMapInfo
 *              structure.
 *  @field  size
 *              Size of memory region to be unmapped.
 *  @field  memAttrs
 *              Mapped address attributes.
 *  ============================================================================
 */
struct MemUnmapInfo_tag {
    Uint32  addr ;
    Uint32  size ;
    MemAttrs memAttrs ;
} ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MEM_OS_H) */
