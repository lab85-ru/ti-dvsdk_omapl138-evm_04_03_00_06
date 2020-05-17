/** ============================================================================
 *  @file   _mpcs.h
 *
 *  @path   $(DSPLINK)/dsp/src/mpcs/
 *
 *  @desc   Defines the internal interfaces and data structures for the
 *          MPCS-Multi Processor Critical Section, sub-component.
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


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

/** ============================================================================
 *  @name   MPCS_SEMCREATED_OPEN
 *
 *  @desc   Value to indicate that the SEM object created in _MPCS_open.
 *  ============================================================================
 */
#define MPCS_SEMCREATED_OPEN            0xABABABAB

/** ============================================================================
 *  @name   MPCS_SEMCREATED_CREATE
 *
 *  @desc   Value to indicate that the SEM object created in MPCS_create.
 *  ============================================================================
 */
#define MPCS_SEMCREATED_CREATE          0xCDCDCDCD

/** ============================================================================
 *  @func   MPCS_init
 *
 *  @desc   This function initializes the MPCS module.
 *
 *  @arg    procId
 *              ID of the processor with which the MPCS component is shared.
 *  @arg    shAddr
 *              Address in shared memory for the MPCS control structure.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid argument.
 *          SYS_EALLOC
 *              Memory allocation failure.
 *
 *  @enter  None.
 *
 *  @leave  None
 *
 *  @see    MPCS_exit ()
 *  ============================================================================
 */
Int
MPCS_init (IN Uint16 procId, IN Uint32 shAddr) ;


/** ============================================================================
 *  @func   MPCS_exit
 *
 *  @desc   This function finalizes the MPCS module.
 *
 *  @arg    procId
 *              ID of the processor with which the MPCS component is shared.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid argument.
 *          SYS_EALLOC
 *              Memory allocation failure.
 *
 *  @enter  None.
 *
 *  @leave  None
 *
 *  @see    MPCS_init ()
 *  ============================================================================
 */
Int
MPCS_exit (IN Uint16 procId) ;


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
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid argument.
 *          SYS_EALLOC
 *              Operation failed due to a memory error.
 *          SYS_ENOTFOUND
 *              Specified MPCS object name does not exist.
 *          SYS_EDEAD
 *              The MPCS component has not been initialized.
 *
 *  @enter  name must be valid.
 *          mpcsHandle must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MPCS_close ()
 *  ============================================================================
 */
Int
_MPCS_open (IN     Uint16         procId,
            IN     Char *         name,
            OUT    MPCS_Handle *  mpcsHandle,
            IN     MPCS_ShObj *   mpcsShObj) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (_MPCS_H) */
