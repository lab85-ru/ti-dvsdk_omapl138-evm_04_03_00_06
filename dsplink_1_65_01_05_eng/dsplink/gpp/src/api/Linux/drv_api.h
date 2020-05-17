/** ============================================================================
 *  @file   drv_api.h
 *
 *  @path   $(DSPLINK)/gpp/src/api/Linux/
 *
 *  @desc   User side driver wrapper interface.
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


#if !defined (DRV_API_H)
#define DRV_API_H


/*  ----------------------------------- OS headers                  */
#include <sys/types.h>
#include <unistd.h>

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  DRV_PROTECT_INIT
 *
 *  @desc   OS dependent definition of protection function.
 *          This function initailizes driver protection mechanism between
 *          multiple processes.
 *  ============================================================================
 */
#define DRV_PROTECT_INIT(handle)          DRV_ProtectInit (handle)

/** ============================================================================
 *  @macro  DRV_PROTECT_EXIT
 *
 *  @desc   OS dependent definition of protection function.
 *          This function finalizes driver protection mechanism between
 *          multiple processes.
 *  ============================================================================
 */
#define DRV_PROTECT_EXIT(handle)          DRV_ProtectExit (handle)

/** ============================================================================
 *  @macro  DRV_PROTECT_ENTER
 *
 *  @desc   OS dependent definition of protection function.
 *          This function enters protection between multiple processes.
 *  ============================================================================
 */
#define DRV_PROTECT_ENTER(handle)         DRV_ProtectEnter (handle)

/** ============================================================================
 *  @macro  DRV_PROTECT_LEAVE
 *
 *  @desc   OS dependent definition of protection function.
 *          This function leaves protection between multiple processes.
 *  ============================================================================
 */
#define DRV_PROTECT_LEAVE(handle)         DRV_ProtectLeave (handle)

/** ============================================================================
 *  @macro  DRV_SET_CURSTATUS
 *
 *  @desc   OS dependent definition of protection function.
 *          This function sets the status of DSPLink components.
 *  ============================================================================
 */
#define DRV_SET_CURSTATUS(curStatus)      (curStatus = getpid ())

/** ============================================================================
 *  @macro  DRV_RESET_CURSTATUS
 *
 *  @desc   OS dependent definition of protection function.
 *          This function resets the status of DSPLink components.
 *  ============================================================================
 */
#define DRV_RESET_CURSTATUS(curStatus)    (curStatus = FALSE)

/** ============================================================================
 *  @macro  DRV_CHECK_CURSTATUS
 *
 *  @desc   OS dependent definition of protection function.
 *          This function returns information about whether the specified
 *          curstatus value is set.
 *  ============================================================================
 */
#define DRV_CHECK_CURSTATUS(curStatus)    (curStatus == getpid ())

/** ============================================================================
 *  @macro  DRV_STARTUP_INIT
 *
 *  @desc   OS dependent definition of startup initialization functionality.
 *          This function performs all initialization required for startup.
 *  ============================================================================
 */
#define DRV_STARTUP_INIT(handle)          (DSPLINK_isSignal = FALSE)

/** ============================================================================
 *  @name   DRV_Object_tag
 *
 *  @desc   OS specific definition of the driver object.
 *
 *  @field  signature
 *              Signature of the object.
 *  @field  driverHandle
 *              OS driver handle.
 *  @field  refCount
 *              Reference count for the driver object.
 *  @field  pageSize
 *              current physical memory page size in bytes
 *  ============================================================================
 */
typedef struct DRV_Object_tag {
    Uint32   signature    ;
    Int32    driverHandle ;
    Uint32   refCount     ;
    Int32    pageSize;
} DRV_Object ;


/** ============================================================================
 *  @name   DRV_handle
 *
 *  @desc   Handle to global driver object.
 *  ============================================================================
 */
extern DRV_Object * DRV_handle ;


/** ============================================================================
 *  @name   DRV_Initialize
 *
 *  @desc   Initialization function for the driver.
 *
 *  @arg    drvObj
 *              pointer to driver object to be initialized.
 *  @arg    arg
 *              optional argument to be used for initialization.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EPOINTER
 *              Invalid pointer.
 *          DSP_EFAIL
 *              Operation failed.
 *
 *  @enter  drvObj must be a valid pointer.
 *
 *  @leave  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DRV_Initialize (IN OUT DRV_Object ** drvObj,     IN OUT Pvoid arg) ;


/** ============================================================================
 *  @name   DRV_Finalize
 *
 *  @desc   Finalization function for the driver.
 *
 *  @arg    drvObj
 *              Pointer to driver object to be finalized.
 *  @arg    arg
 *              Optional argument to be used for finalization.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SFINALIZED
 *              Operation successfully completed and the object is freed.
 *          DSP_EPOINTER
 *              Invalid pointer.
 *          DSP_EFAIL
 *              Operation failed.
 *
 *  @enter  drvObj must be a valid pointer.
 *
 *  @leave  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DRV_Finalize (IN DRV_Object * drvObj,     IN OUT Pvoid arg) ;


/** ============================================================================
 *  @name   DRV_Invoke
 *
 *  @desc   Trap to underlying kernel side Linux LINK driver.
 *
 *  @arg    drvObj
 *              Pointer to driver.
 *  @arg    cmdId
 *              Command identifier.
 *  @arg    arg1
 *              Optional argument 1.
 *  @arg    arg2
 *              Optional argument 2.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EATTACHED
 *              Incorrect state.
 *          DSP_EFAIL
 *              Operation failed.
 *
 *  @enter  drvObj must be a valid pointer.
 *
 *  @leave  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DRV_Invoke (IN         DRV_Object * drvObj,
            IN         Uint32       cmdId,
                IN OUT Pvoid        arg1,
                IN OUT Pvoid        arg2) ;


/** ============================================================================
 *  @name   DRV_ProtectInit
 *
 *  @desc   This function initailizes driver protection mechanism between
 *          multiple processes. This API can be called by multiple processes,
 *          and will create the semaphore if it does not exist.
 *
 *  @arg    drvObj
 *              Pointer to driver object
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EPOINTER
 *              Invalid pointer.
 *          DSP_EFAIL
 *              Operation failed.
 *
 *  @enter  drvObj must be a valid pointer.
 *
 *  @leave  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DRV_ProtectInit (IN DRV_Object * drvObj) ;


/** ============================================================================
 *  @name   DRV_ProtectExit
 *
 *  @desc   This function finalizes driver protection mechanism between
 *          multiple processes. This function must be called only by the last
 *          exiting process.
 *
 *  @arg    drvObj
 *              Pointer to driver object
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EPOINTER
 *              Invalid pointer.
 *          DSP_EFAIL
 *              Operation failed.
 *
 *  @enter  drvObj must be a valid pointer.
 *
 *  @leave  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DRV_ProtectExit (IN DRV_Object * drvObj) ;


/** ============================================================================
 *  @name   DRV_ProtectEnter
 *
 *  @desc   This function enters driver protection between multiple processes.
 *
 *  @arg    drvObj
 *              Pointer to driver object
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EPOINTER
 *              Invalid pointer.
 *          DSP_EFAIL
 *              Operation failed.
 *
 *  @enter  drvObj must be a valid pointer.
 *
 *  @leave  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DRV_ProtectEnter (IN DRV_Object * drvObj) ;


/** ============================================================================
 *  @name   DRV_ProtectLeave
 *
 *  @desc   This function leaves driver protection between multiple processes.
 *
 *  @arg    drvObj
 *              Pointer to driver object
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EPOINTER
 *              Invalid pointer.
 *          DSP_EFAIL
 *              Operation failed.
 *
 *  @enter  drvObj must be a valid pointer.
 *
 *  @leave  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DRV_ProtectLeave (IN DRV_Object * drvObj) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !define (DRV_API_H) */
