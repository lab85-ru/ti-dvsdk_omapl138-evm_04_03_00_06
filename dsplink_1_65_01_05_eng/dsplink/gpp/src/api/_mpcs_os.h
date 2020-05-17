/** ============================================================================
 *  @file   _mpcs_os.h
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Defines the internal OS-specific interfaces and data structures for
 *          the API sub-component MPCS.
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


#if !defined (_MPCS_OS_H)
#define _MPCS_OS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <mpcsdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   _MPCS_OS_open
 *
 *  @desc   This function performs the OS-specific activities for opening an
 *          MPCS object specified by its name.
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
 *          DSP_SEXISTS
 *              MPCS corresponding to the specified name already exists,
 *              and a handle to it is returned.
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
 *  @see    _MPCS_OS_close ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MPCS_OS_open (IN     ProcessorId    procId,
               IN     Pstr           name,
               OUT    MPCS_Handle *  mpcsHandle,
               IN     MPCS_ShObj *   mpcsShObj) ;


/** ============================================================================
 *  @func   _MPCS_OS_close
 *
 *  @desc   This function performs the OS-specific activities for closing an
 *          MPCS object specified by its handle.
 *
 *  @arg    procId
 *              ID of the processor with which the MPCS is shared.
 *  @arg    mpcsHandle
 *              Handle to the MPCS object to be closed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SFREE
 *              The last close for specified MPCS resulted in it getting closed.
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
 *          mpcsHandle must be valid.
 *
 *  @leave  None
 *
 *  @see    _MPCS_OS_open ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MPCS_OS_close (IN     ProcessorId    procId,
                IN     MPCS_Handle    mpcsHandle) ;


/** ============================================================================
 *  @func   _MPCS_OS_enter
 *
 *  @desc   This function performs the OS-specific activities for local
 *          protection
 *
 @arg    procId
 *              ID of the processor with which the MPCS is shared.
 *  @arg    mpcsHandle
 *              Handle to the MPCS object to be closed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EACCESSDENIED
 *              The MPCS component has not been initialized.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          mpcsHandle must be valid.
 *
 *  @leave  None
 *
 *  @modif  None
 *
 *  @see    _MPCS_OS_leave ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MPCS_OS_enter (IN     ProcessorId    procId,
                IN     MPCS_Handle    mpcsHandle) ;


/** ============================================================================
 *  @func   _MPCS_OS_leave
 *
 *  @desc   This function performs the OS-specific activities for local
 *          protection
 *
 @arg    procId
 *              ID of the processor with which the MPCS is shared.
 *  @arg    mpcsHandle
 *              Handle to the MPCS object to be closed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EACCESSDENIED
 *              The MPCS component has not been initialized.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          mpcsHandle must be valid.
 *
 *  @leave  None
 *
 *  @modif  None
 *
 *  @see    _MPCS_OS_enter ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MPCS_OS_leave (IN     ProcessorId    procId,
                IN     MPCS_Handle    mpcsHandle) ;



#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (_MPCS_OS_H) */
