/** ============================================================================
 *  @file   mp_list_os.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/mp_list/Linux/
 *
 *  @desc   OS specific implementation of functions used by the mp_list
 *          sample application.
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


/*  ----------------------------------- OS Specific Headers           */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <stdlib.h>

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>

/*  ----------------------------------- Application Header            */
#include <mp_list.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

/** ============================================================================
 *  @name   MP_LIST_SemObject
 *
 *  @desc   This object is used by various SEM API's.
 *
 *  @field  sem
 *              Linux semaphore.
 *
 *  @see    None
 *  ============================================================================
 */
typedef struct MP_LIST_SemObject_tag {
    sem_t  sem ;
} MP_LIST_SemObject ;


/** ============================================================================
 *  @func   MP_LIST_0Print
 *
 *  @desc   Print a mp_list without any arguments.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MP_LIST_0Print (Char8 * str)
{
    printf (str) ;
}


/** ============================================================================
 *  @func   MP_LIST_1Print
 *
 *  @desc   Print a mp_list with one arguments.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MP_LIST_1Print (Char8 * str, Uint32 arg)
{
    printf (str, arg) ;
}


/** ============================================================================
 *  @func   MP_LIST_Sleep
 *
 *  @desc   Sleeps for the specified number of microseconds.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MP_LIST_Sleep (Uint32 uSec)
{
    usleep (uSec) ;
}


/** ============================================================================
 *  @func   MP_LIST_CreateSem
 *
 *  @desc   This function creates a semaphore.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MP_LIST_CreateSem (OUT Pvoid * semPtr)
{
    DSP_STATUS          status = DSP_SOK ;
    MP_LIST_SemObject * semObj ;
    int                 osStatus ;

    semObj = malloc (sizeof (MP_LIST_SemObject)) ;
    if (semObj != NULL) {
        osStatus = sem_init (&(semObj->sem), 0, 0) ;
        if (osStatus < 0) {
            status = DSP_EFAIL ;
        }
        else {
            *semPtr = (Pvoid) semObj ;
        }
    }
    else {
        *semPtr = NULL ;
        status = DSP_EFAIL ;
    }

    return status ;
}


/** ============================================================================
 *  @func   MP_LIST_DeleteSem
 *
 *  @desc   This function deletes a semaphore.
 *
 *  @arg    semHandle
 *              Pointer to the semaphore object to be deleted.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MP_LIST_DeleteSem (IN Pvoid semHandle)
{
    DSP_STATUS          status = DSP_SOK ;
    MP_LIST_SemObject * semObj = semHandle ;
    int                 osStatus ;

    osStatus = sem_destroy (&(semObj->sem)) ;
    if (osStatus < 0) {
        status = DSP_EFAIL ;
    }

    free (semObj) ;

    return status ;
}


/** ============================================================================
 *  @func   MP_LIST_WaitSem
 *
 *  @desc   This function waits on a semaphore.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MP_LIST_WaitSem (IN Pvoid semHandle)
{
    DSP_STATUS          status = DSP_SOK ;
    MP_LIST_SemObject * semObj = semHandle ;
    int                 osStatus ;

    osStatus = sem_wait (&(semObj->sem)) ;
    if (osStatus < 0) {
        status = DSP_EFAIL ;
    }

    return status ;
}


/** ============================================================================
 *  @func   MP_LIST_PostSem
 *
 *  @desc   This function posts a semaphore.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MP_LIST_PostSem (IN Pvoid semHandle)
{
    DSP_STATUS          status = DSP_SOK ;
    MP_LIST_SemObject * semObj = semHandle ;
    int                 osStatus ;

    osStatus = sem_post (&(semObj->sem)) ;
    if (osStatus < 0) {
        status = DSP_EFAIL ;
    }

    return status ;
}


/** ============================================================================
 *  @func   MP_LIST_Atoll
 *
 *  @desc   Converts ascii to long int
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Uint32
MP_LIST_Atoll (Char8 * str)
{
     Uint32 val = 0 ;
     val = strtoll (str, NULL, 16) ;
     return val ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
