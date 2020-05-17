/** ============================================================================
 *  @file   mplistdefs.h
 *
 *  @path   $(DSPLINK)/gpp/inc/usr/
 *
 *  @desc   Global definitions for MPLIST module
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


#if !defined (MPLISTDEFS_H)
#define MPLISTDEFS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <mpcsdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   MPLIST_Attrs
 *
 *  @desc   This structure defines the MPLIST Creation Parameters.
 *
 *  @field  poolId
 *              Pool to be used to allocate memory for MPLIST data structures.
 *  ============================================================================
 */
typedef struct MPLIST_Attrs_tag {
    PoolId       poolId  ;
} MPLIST_Attrs ;

/** ============================================================================
 *  @name   MPLIST_Handle
 *
 *  @desc   Handle to the MPLIST object.
 *  ============================================================================
 */
typedef void * MPLIST_Handle ;

/** ============================================================================
 *  @name   MPLIST_Header
 *
 *  @desc   An element of a list.
 *
 *  @field  next
 *              Next node pointer.
 *  @field  prev
 *              Previous node pointer.
 *  ============================================================================
 */
typedef struct MPLIST_Header_tag {
    struct MPLIST_Header_tag * next ;
    struct MPLIST_Header_tag * prev ;
} MPLIST_Header ;

/** ============================================================================
 *  @name   MPLIST_Elem
 *
 *  @desc   This type is used for the handle to the shared list entry structure.
 *  ============================================================================
 */
typedef MPLIST_Header * MPLIST_Elem ;

/** ============================================================================
 *  @name   MPLIST_List
 *
 *  @desc   Definition of a MPLIST.
 *
 *  @field  head
 *              Head of the list.
 *  @field  padding
 *              Padding for alignment.
 *  @field  mpcsObj
 *              MPCS object used for protection of the list.
 *  ============================================================================
 */
typedef struct MPLIST_List_tag {
    MPLIST_Header head ;
    ADD_PADDING  (padding, MPLIST_LIST_PADDING)
    MPCS_ShObj    mpcsObj ;
} MPLIST_List ;

/** ============================================================================
 *  @name   MPLIST_Entry
 *
 *  @desc   MPLIST descriptor entry for each MPLIST in the system.
 *
 *  @field  ownerProcId
 *              Processor ID of the creator of the MpList.
 *  @field  poolId
 *              Pool id to be used to allocate memory for all MpList data
 *              structures.
 *  @field  phyListHandle
 *              Handle of the MpList in Physical (DSP) address space.
 *  @field  name
 *              System wide unique identifier for the MpList.
 *  @field  padding
 *              Padding to make stucture cache aligned.
 *  ============================================================================
 */
typedef struct MPLIST_Entry_tag {
    Uint16       ownerProcId ;
    Uint16       poolId ;
    Pvoid        phyListHandle  ;
    Char8        name [DSP_MAX_STRLEN] ;
    ADD_PADDING  (padding, MPLIST_ENTRY_PADDING)
} MPLIST_Entry ;

/** ============================================================================
 *  @name   MPLIST_Ctrl
 *
 *  @desc   This structure defines the control structure required by the
 *          MPLIST component. It contains information about all MPLIST objects
 *          shared between the GPP and a specific DSP.
 *
 *  @field  isInitialized
 *              Flag to indicate if this region was initialized.
 *  @field  dspId
 *              ID of the DSP with which the MPLIST region is shared.
 *  @field  maxEntries
 *              Maximum number of MPLIST instances supported by the MPLIST.
 *  @field  ipsId
 *              ID of the IPS to be used (if any). A value of -1 indicates that
 *              no IPS is required by the MPLIST.
 *  @field  ipsEventNo
 *              IPS Event number associated with MPLIST (if any). A value of -1
 *              indicates that no IPS is required by the MPLIST.
 *  @field  dspAddrEntry
 *              Pointer to array in DSP address space of MPLIST objects that can
 *              be created.
 *  @field  padding
 *              Padding.
 *  @field  lockObj
 *              Lock used to protect the shared MPLIST control region from
 *              multiple simultaneous accesses.
 *  ============================================================================
 */
typedef struct MPLIST_Ctrl_tag {
    Uint32         isInitialized ;
    Uint32         dspId ;
    Uint32         maxEntries ;
    Uint32         ipsId ;
    Uint32         ipsEventNo ;
    MPLIST_Entry * dspAddrEntry ;
    ADD_PADDING    (padding, MPLIST_CTRL_PADDING)
    MPCS_ShObj     lockObj ;
} MPLIST_Ctrl ;

/** ============================================================================
 *  @name   MPLIST_MemInfo
 *
 *  @desc   Entry structure for the MpList data transport
 *
 *  @field  physAddr
 *              Physical address of the memory region for MpList
 *  @field  kernAddr
 *              Kernel address of the memory region for MpList
 *  @field  userAddr
 *              User address of the memory region for MpList
 *  @field  size
 *              Size of the memory region for MpList
 *  ============================================================================
 */
typedef struct MPLIST_MemInfo_tag {
    ProcessorId procId ;
    Uint32      physAddr ;
    Uint32      kernAddr ;
    Uint32      userAddr ;
    Uint32      size ;
} MPLIST_MemInfo ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* if !defined (MPLISTDEFS_H) */
