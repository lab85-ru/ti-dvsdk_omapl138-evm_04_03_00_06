/** ============================================================================
 *  @file   _mpcs_os.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/Linux/
 *
 *  @desc   Implementation of internal OS-specific functions for API
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


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- User API                    */
#include <_mpcs.h>
#include <mpcs.h>
#include <_mem_usr.h>
#include <_sync_usr.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent ID.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_API_MPCS_OS

/*  ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason
 *  ============================================================================
 */
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_MPCS_OS, __LINE__)


/** ============================================================================
 *  @func   _MPCS_OS_open
 *
 *  @desc   This function performs the OS-specific activities for opening an
 *          MPCS object specified by its name.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MPCS_OS_open (IN     ProcessorId    procId,
               IN     Pstr           name,
               OUT    MPCS_Handle *  mpcsHandle,
               IN     MPCS_ShObj *   mpcsShObj)
{
    DSP_STATUS  status  = DSP_SOK ;
    MPCS_Obj ** mpcsObj = (MPCS_Obj **) mpcsHandle ;

    TRC_4ENTER ("_MPCS_OS_open", procId, name, mpcsHandle, mpcsShObj) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (name       != NULL) ;
    DBC_Require (mpcsHandle != NULL) ;

    /* A different MPCS object handle is returned for each process/thread. */
    status = _MEM_USR_calloc ((Void **) mpcsHandle, sizeof (MPCS_Obj)) ;
    if (DSP_SUCCEEDED (status)) {
        (*mpcsObj)->mpcsObj = mpcsShObj ;
        /* Create/get a handle to a SYNC_USR CS object for protection between
         * multiple user processes and threads.
         */
        status = _SYNC_USR_createCS (name, &((*mpcsObj)->syncCsObj)) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
            _MEM_USR_free ((Void **) mpcsHandle) ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("_MPCS_OS_open", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _MPCS_OS_close
 *
 *  @desc   This function performs the OS-specific activities for closing an
 *          MPCS object specified by its handle.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MPCS_OS_close (IN     ProcessorId    procId,
                IN     MPCS_Handle    mpcsHandle)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    MPCS_Obj * mpcsObj   = (MPCS_Obj *) mpcsHandle ;

    TRC_2ENTER ("_MPCS_OS_close", procId, mpcsHandle) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (mpcsHandle != NULL) ;

    /* Delete/release the handle to the SYNC_USR CS object created for
     * protection between multiple user processes and threads.
     */
    status = _SYNC_USR_deleteCS (&(mpcsObj->syncCsObj)) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    /* Free the user MPCS handle. */
    tmpStatus = _MEM_USR_free ((Void **) &mpcsHandle) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("_MPCS_OS_close", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _MPCS_OS_enter
 *
 *  @desc   This function performs the OS-specific activities for local
 *          protection
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MPCS_OS_enter (IN     ProcessorId    procId,
                IN     MPCS_Handle    mpcsHandle)
{
    DSP_STATUS status    = DSP_SOK ;
    MPCS_Obj *    mpcsObj = (MPCS_Obj *) mpcsHandle ;

    TRC_2ENTER ("_MPCS_OS_enter", procId, mpcsHandle) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (mpcsHandle != NULL) ;

    status = _SYNC_USR_enterCS (mpcsObj->syncCsObj) ;

    TRC_1LEAVE ("_MPCS_OS_enter", status) ;

    return status ;
}

/** ============================================================================
 *  @func   _MPCS_OS_leave
 *
 *  @desc   This function performs the OS-specific activities for local
 *          protection
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MPCS_OS_leave (IN     ProcessorId    procId,
                IN     MPCS_Handle    mpcsHandle)
{
    DSP_STATUS status     = DSP_SOK ;
    MPCS_Obj *    mpcsObj = (MPCS_Obj *) mpcsHandle ;

    TRC_2ENTER ("_MPCS_OS_leave", procId, mpcsHandle) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (mpcsHandle != NULL) ;

    status = _SYNC_USR_leaveCS (mpcsObj->syncCsObj) ;

    TRC_1LEAVE ("_MPCS_OS_leave", status) ;

    return status ;
}
#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
