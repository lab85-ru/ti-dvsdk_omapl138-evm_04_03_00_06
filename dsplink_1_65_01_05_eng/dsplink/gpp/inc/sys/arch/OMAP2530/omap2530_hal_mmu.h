/** ============================================================================
 *  @file   omap2530_hal_mmu.h
 *
 *  @path   $(DSPLINK)/gpp/inc/sys/arch/OMAP2530/
 *
 *  @desc   Hardware Abstraction Layer for Memory management
 *          module in OMAP.
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


#if !defined (ID_ARCH_HAL_MMU_H)
#define ID_ARCH_HAL_MMU_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- Generic Functions           */
#include <list.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <omap2530_hal.h>


#if defined (__cplusplus)
extern "C" {
#endif

/** ============================================================================
 *  @name   listOfEntries
 *
 *  @desc   Information for MMU Entry
 *
 *  @field  element
 *              ListElement.
 *  @field  dspVirtAddr
 *              DSP virtual address.
 *  @field  actualSize
 *              Actual Size of Entry.
 *  @field  processAddr
 *              Processed address.
 *  @field  processSize
 *              Processed size.
 *  @field  tlbEntry
 *              TLB entry of the MMU module.
 *  @field  type
 *              Type of entry.
 *  ============================================================================
 */
typedef struct listOfEntries {
    ListElement    element ;
    Uint32 dspVirtAddr ;
    Uint32 actualSize ;
    Uint32 processAddr ;
    Uint32 processSize ;
    Uint32 tlbEntry ;
    Bool type ;
} MMU_EntryInfo ;

/** ============================================================================
 *  @func   OMAP2530_halMmuCtrl
 *
 *  @desc   MMU conrtoller.
 *
 *  @arg    halObj.
 *              Pointer to HAL object.
 *  @arg    cmd.
 *              Command.
 *  @arg    memTable
 *              Memory Table.
 *  @arg    memEntries.
 *              Number of entries in memTable.
 *  @arg    mmuInfo
 *              Control information for mapping.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ECONFIG
 *              Configuration error.
 *          DSP_EINVALIDARG
 *              An invalid argument was specified.
 *          DSP_ERANGE
 *              Invalid range for dynamic entry.
 *          DSP_SEXISTS
 *              Enrty exists.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP2530_halMmuCtrl (IN         Pvoid               halObj,
                     IN         DSP_MmuCtrlCmd cmd,
                     IN         LINKCFG_MemEntry *  memTable,
                     IN         Uint32 memEntries,
                     IN         ProcMemMapInfo *    mmuInfo) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (OMAP2530_HAL_PWR_H) */
