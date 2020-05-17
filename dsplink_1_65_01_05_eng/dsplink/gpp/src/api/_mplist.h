/** ============================================================================
 *  @file   _mplist.h
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Defines the internal interfaces and data structures for the API
 *          sub-component MPLIST.
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


#if !defined (_MPLIST_H)
#define _MPLIST_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   MPLIST_Obj
 *
 *  @desc   Multiprocessor List object, which is used for getting a handle to
 *          the list section between multiple processors. This object is not
 *          shared between the processors, and the object instance is specific
 *          to the process opening the handle.
 *
 *  @field  procId
 *              Processor Identifier.
 *  @field  virtListHandle
 *              Handle of the MpList in user space.
 *  @field  virtLockHandle
 *              Handle of the MpList specific MPCS object in user space.
 *  @field  virtEntryHandle
 *              Handle of the MpList entry in the shared region of user space.
 *  ============================================================================
 */
typedef struct MPLIST_Obj_tag {
    ProcessorId  procId         ;
    Pvoid        virtListHandle ;
    Pvoid        virtLockHandle ;
    Pvoid        virtEntryHandle ;
} MPLIST_Obj ;

/** ============================================================================
 *  @name   MPLIST_Object
 *
 *  @desc   This structure defines the MPLIST object, which contains all
 *          state information required by the MPLIST component.
 *
 *  @field  ctrlMemInfo
 *              Memory segment information for where the MPLIST control region
 *              data structure is located.
 *  @field  ctrlPtr
 *              Pointer to MPLIST Control region.
 *  @field  entryPtr
 *              Pointer to MPLIST Entry table
 *  @field  mpcsObj
 *              Handle to global MPCS protecting the MPLIST region
 *  ============================================================================
 */
typedef struct MPLIST_Object_tag {
    MPLIST_MemInfo  ctrlMemInfo ;
    MPLIST_Ctrl *   ctrlPtr     ;
    MPLIST_Entry *  entryPtr    ;
    MPCS_Handle     mpcsObj     ;
} MPLIST_Object ;


/** ============================================================================
 *  @func   _MPLIST_moduleInit
 *
 *  @desc   This function initializes the MPLIST module.
 *          This function does the system level initialization for the MPLIST
 *          module. It initializes and allocates all resources common to all
 *          applications using the MPLIST module.
 *
 *  @arg    procId
 *              Processor Id
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EINVALIDARG
 *              Invalid Parameter passed.
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
_MPLIST_moduleInit (IN  ProcessorId procId) ;


/** ============================================================================
 *  @func   _MPLIST_moduleExit
 *
 *  @desc   This function finalizes the MPLIST module.
 *          This function does the system level finalization for the MPLIST
 *          module. It finalizes and allocates all resources common to all
 *          applications using the MPLIST module.
 *
 *  @arg    procId
 *              Processor Id
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EINVALIDARG
 *              Invalid Parameter passed.
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
_MPLIST_moduleExit (IN ProcessorId procId) ;


/** ============================================================================
 *  @func   _MPLIST_init
 *
 *  @desc   This function initializes the MPLIST component for the calling
 *          process.
 *
 *  @arg    procId
 *              Processor Id
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EINVALIDARG
 *              Invalid Parameter passed.
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
_MPLIST_init (IN  ProcessorId procId) ;


/** ============================================================================
 *  @func   _MPLIST_exit
 *
 *  @desc   This function finalizes the MPLIST component for the calling
 *          process.
 *
 *  @arg    procId
 *              Processor Id
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EINVALIDARG
 *              Invalid Parameter passed.
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
_MPLIST_exit (IN ProcessorId procId) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (_MPLIST_H) */
