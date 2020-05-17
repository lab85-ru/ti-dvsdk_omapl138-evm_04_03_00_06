/** ============================================================================
 *  @file   tskMpList.h
 *
 *  @path   $(DSPLINK)/dsp/src/samples/mp_list/
 *
 *  @desc   Header file for tskMpList application.
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


#if !defined (TSKMPLIST_)
#define TSKMPLIST_


/*  ----------------------------------- DSP/BIOS Headers            */
#include <sem.h>

/*  ----------------------------------- MpList Headers              */
#include <mplistdefs.h>
#include <mplist.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   TSKMPLIST_TransferInfo
 *
 *  @desc   Structure used to keep various information needed by various phases
 *          of the application.
 *
 *  @field  numIterations
 *              Number of iterations to be done.
 *  @field  numElem
 *              Number of elements present in the list.
 *  @field notifySemObj
 *              Semaphore object used for notification.
 *  @field gppMpListHandle
 *               Handle to the MpList created by the GPP-side.
 *  ============================================================================
 */
typedef struct TSKMPLIST_TransferInfo_tag {
    Uint16          numIterations   ;
    Uint16          numElem         ;
    SEM_Obj         notifySemObj    ;
    MPLIST_Handle   gppMpListHandle ;
} TSKMPLIST_TransferInfo ;


/** ============================================================================
 *  @func   TSKMPLIST_create
 *
 *  @desc   Create phase function of MP_LIST application.
 *
 *  @arg    transferInfo
 *              Information for transfer.
 *
 *  @ret    SYS_OK
 *              Successful operation.
 *          SYS_EBADIO
 *              Failure occured while doing IO.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
Int TSKMPLIST_create (TSKMPLIST_TransferInfo ** transferInfo) ;

/** ============================================================================
 *  @func   TSKMPLIST_execute
 *
 *  @desc   Excecute phase function of MP_LIST application.
 *
 *  @arg    transferInfo
 *              Information for transfer.
 *
 *  @ret    SYS_OK
 *              Successful operation.
 *          SYS_EBADIO
 *              Failure occured while doing IO.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
Int TSKMPLIST_execute (TSKMPLIST_TransferInfo * transferInfo) ;

/** ============================================================================
 *  @func   TSKMPLIST_delete
 *
 *  @desc   Delete phase function of MP_LIST application.
 *
 *  @arg    transferInfo
 *              Information for transfer.
 *
 *  @ret    SYS_OK
 *              Successful operation.
 *          SYS_EBADIO
 *              Failure occured while doing IO.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
Int TSKMPLIST_delete (TSKMPLIST_TransferInfo * transferInfo) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (TSKMPLIST_) */
