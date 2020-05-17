/** ============================================================================
 *  @file   _mpcs.h
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Defines the internal interfaces and data structures for the API
 *          sub-component MPCS.
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


#if !defined (_MPCS_H)
#define _MPCS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <mpcsdefs.h>
#include <_mpcs_os.h>
#include <_sync_usr.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   MPCS_Obj
 *
 *  @desc   Multiprocessor Critical Section object, which is used for protecting
 *          a specific critical section between multiple processors. This object
 *          is not shared between the processors, and the object instance is
 *          specific to the process creating the MPCS object.
 *
 *  @field  mpcsObj
 *              Handle to the MPCS object in user space of the process.
 *  @field  syncCsObj
 *              Handle to the user-side SYNC CS object.
 *  @field  mpcsId
 *              MPCS Id i.e. entry slot in MPCS entry table.
 *  @field  procId
 *              Processor Identifier.
 *  ============================================================================
 */
typedef struct MPCS_Obj_tag {
    MPCS_ShObj *        mpcsObj ;
    SYNC_USR_CsObject * syncCsObj ;
    Uint32              mpcsId ;
    ProcessorId         procId ;
} MPCS_Obj ;


/** ============================================================================
 *  @name   MPCS_Object_tag
 *
 *  @desc   This structure defines the MPCS object, which contains all
 *          state information required by the MPCS component.
 *
 *  @field  ctrlMemInfo
 *              Memory segment information for where the MPCS control region
 *              data structure is located.
 *  @field  ctrlPtr
 *              Pointer to MPCS Control region.
 *  @field  entryPtr
 *              Pointer to MPCS Entry table
 *  @field  csObj
 *              Handle to global MPCS protecting the MPCS region
 *  ============================================================================
 */
struct MPCS_Object_tag {
    MPCS_MemInfo  ctrlMemInfo ;
    MPCS_Ctrl *   ctrlPtr     ;
    MPCS_Entry *  entryPtr    ;
    MPCS_Handle   csObj       ;
} ;


/** ============================================================================
 *  @func   _MPCS_init
 *
 *  @desc   This function initialize the MPCS component.
 *
 *  @arg    procId
 *              Identifier of the processor with which the MPCS region is to be
 *              shared.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *
 *  @leave  None.
 *
 *  @see    _MPCS_exit ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_MPCS_init (IN ProcessorId procId) ;


/** ============================================================================
 *  @func   _MPCS_exit
 *
 *  @desc   This function finalizes the MPCS component.
 *
 *  @arg    procId
 *              Identifier of the processor with which the MPCS region is shared
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EACCESSDENIED
 *              The MPCS component has not been initialized.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *
 *  @leave  None.
 *
 *  @see    _MPCS_init ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_MPCS_exit (IN ProcessorId procId) ;


/** ============================================================================
 *  @func   _MPCS_open
 *
 *  @desc   This function opens an MPCS object specified by its name and gets a
 *          handle to the object. This function allows the user to open an MPCS
 *          object by a special reserved name MPCS_RESV_LOCKNAME indicating that
 *          the object is not registered within the MPCS entries table.
 *          For such objects, the user already has the pointer to the MPCS
 *          shared object in its process space.
 *          Every process that needs to use the MPCS object must get a handle to
 *          the object by calling this API.
 *
 *  @arg    procId
 *              ID of the processor with which the MPCS object is to be shared.
 *  @arg    name
 *              System-wide unique name for the MPCS object.
 *              Specifying the name as MPCS_RESV_LOCKNAME expects the user to
 *              pass the pointer to the MPCS shared object through the mpcsShObj
 *              parameter.
 *  @arg    mpcsHandle
 *              Location to receive the MPCS object handle, which is valid in
 *              the process space of the calling process.
 *  @arg    mpcsShObj
 *              Pointer to the MPCS shared object in the caller's process space.
 *              This is an optional argument that is provided if the user
 *              already has the pointer to the MPCS shared object, and wishes
 *              to open the specific MPCS object. This parameter must be
 *              specified by the user if the name used is MPCS_RESV_LOCKNAME.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_ENOTFOUND
 *              Specified MPCS object name does not exist.
 *          DSP_EACCESSDENIED
 *              The MPCS component has not been initialized.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          name must be valid.
 *          mpcsHandle must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MPCS_close ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_MPCS_open (IN     ProcessorId    procId,
            IN     Pstr           name,
            OUT    MPCS_Handle *  mpcsHandle,
            IN     MPCS_ShObj *   mpcsShObj) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (_MPCS_H) */
