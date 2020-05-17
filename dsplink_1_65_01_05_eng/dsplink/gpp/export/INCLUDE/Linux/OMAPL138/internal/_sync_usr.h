/** ============================================================================
 *  @file   _sync_usr.h
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Defines the interfaces and data structures for the sub-component
 *          SYNC for user-side.
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


#if !defined (_SYNC_USR_H)
#define _SYNC_USR_H

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @name   SYNC_USR_CsObject
 *
 *  @desc   Forward declaration. See corresponding C file for actual definition.
 *  ============================================================================
 */
typedef struct SYNC_USR_CsObject_tag SYNC_USR_CsObject ;


/** ============================================================================
 *  @func   _SYNC_USR_init
 *
 *  @desc   Initializes the SYNC USR subcomponent.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
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
_SYNC_USR_init (Void) ;


/** ============================================================================
 *  @func   _SYNC_USR_exit
 *
 *  @desc   This function frees up all resources used by the SYNC USR
 *          subcomponent.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
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
_SYNC_USR_exit (Void) ;


/** ============================================================================
 *  @func   _SYNC_USR_createCS
 *
 *  @desc   Creates the Critical section structure.
 *
 *  @arg    idKey
 *              String key to identify the CS being created. If a CS with the
 *              specified key has been created, a handle to the same CS is
 *              returned to provide protection between multiple processes.
 *              If a CS corresponding to specified key does not exist, a new
 *              object is created and returned to the user.
 *  @arg    csObj
 *              Location to receive the pointer to created critical section
 *              object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SEXISTS
 *              Semaphore corresponding to the specified idKey already exists,
 *              and is returned.
 *          DSP_EMEMORY
 *              Operation failed due to insufficient memory.
 *          DSP_EPOINTER
 *              Invalid pointer passed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  idKey must not be NULL.
 *          csObj must not be NULL.
 *
 *  @leave  In case of success csObj is valid.
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_SYNC_USR_createCS (IN Pstr idKey, IN OUT SYNC_USR_CsObject ** csObj) ;


/** ============================================================================
 *  @func   _SYNC_USR_deleteCS
 *
 *  @desc   Deletes the critical section object.
 *
 *  @arg    csObj
 *              Pointer to location where address of critical section to be
 *              deleted is stored.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SFREE
 *              The last delete for specified semaphore resulted in it getting
 *              deleted.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  idKey must not be NULL.
 *          csObj must be a valid object.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_SYNC_USR_deleteCS (IN OUT SYNC_USR_CsObject ** csObj) ;


/** ============================================================================
 *  @func   _SYNC_USR_enterCS
 *
 *  @desc   This function enters the critical section that is passed as an
 *          argument to it. After successful return of this function no other
 *          process can enter until this process exits the CS.
 *
 *  @arg    csObj
 *              Critical section to enter.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  csObj must be a valid object.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_SYNC_USR_enterCS (IN SYNC_USR_CsObject * csObj) ;


/** ============================================================================
 *  @func   _SYNC_USR_leaveCS
 *
 *  @desc   This function makes the critical section available for other
 *          processes to enter.
 *
 *  @arg    csObj
 *              Critical section to leave.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  csObj should be a valid object.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_SYNC_USR_leaveCS (IN SYNC_USR_CsObject * csObj) ;


/** ============================================================================
 *  @func   _SYNC_USR_sleep
 *
 *  @desc   This function yields control to other tasks for the defined time
 *          interval.
 *
 *  @arg    delay.
 *              Time (in milliseconds) for which yield is requested. If 0 is
 *              specified, it just yields control without any specific delay.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_SYNC_USR_sleep (IN Uint32 delay) ;


/** ============================================================================
 *  @func   _SYNC_USR_stateObjInit
 *
 *  @desc   Initialize SYNC USR sub-component by allocating all resources.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_SYNC_USR_stateObjInit (Void) ;


/** ============================================================================
 *  @func   SYNC_USR_setPriority
 *
 *  @desc   Change the priority of the current  task.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_SYNC_USR_setPriority(IN Uint32 priority);


/** ============================================================================
 *  @func   SYNC_USR_GetPriority
 *
 *  @desc   Get the priority of the current  task.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_SYNC_USR_getPriority (OUT Uint32 * priority);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !define (_SYNC_USR_H) */
