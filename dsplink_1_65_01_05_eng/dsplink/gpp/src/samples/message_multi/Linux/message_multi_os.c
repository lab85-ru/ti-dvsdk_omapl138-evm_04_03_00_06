/** ============================================================================
 *  @file   message_multi_os.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/message_multi/Linux/
 *
 *  @desc   OS specific implementation of functions used by the message_multi
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <stdlib.h>

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>

/*  ----------------------------------- Application Header            */
#include <message_multi.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   MESSAGE_SemObject
 *
 *  @desc   This object is used by various SEM API's.
 *
 *  @field  sem
 *              Linux semaphore.
 *
 *  @see    None
 *  ============================================================================
 */
typedef struct MESSAGE_SemObject_tag {
    sem_t  sem ;
} MESSAGE_SemObject ;


/** ============================================================================
 *  @func   MESSAGEMULTI_OS_init
 *
 *  @desc   This function does OS specific initialization.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGEMULTI_OS_init (Void)
{
    DSP_STATUS          status = DSP_SOK ;

    return status ;
}


/** ============================================================================
 *  @func   MESSAGEMULTI_OS_exit
 *
 *  @desc   This function OS specific finalization.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGEMULTI_OS_exit(Void)
{
    DSP_STATUS status = DSP_SOK ;

    return status ;
}


/** ============================================================================
 *  @func   MESSAGEMULTI_0Print
 *
 *  @desc   Print a message without any arguments.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGEMULTI_0Print (Char8 * str)
{
    printf (str) ;
}


/** ============================================================================
 *  @func   MESSAGEMULTI_1Print
 *
 *  @desc   Print a message with one arguments.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGEMULTI_1Print (Char8 * str, Uint32 arg)
{
    printf (str, arg) ;
}


/** ============================================================================
 *  @func   MESSAGEMULTI_2Print
 *
 *  @desc   Print a message with two arguments.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGEMULTI_2Print (Char8 * str, Uint32 arg1, Uint32 arg2)
{
    printf (str, arg1, arg2) ;
}


/** ============================================================================
 *  @func   MESSAGEMULTI_Sleep
 *
 *  @desc   Sleeps for the specified number of microseconds.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGEMULTI_Sleep (Uint32 uSec)
{
    usleep (uSec) ;
}


/** ============================================================================
 *  @func   MESSAGEMULTI_CreateSem
 *
 *  @desc   This function creates a semaphore.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGEMULTI_CreateSem (OUT Pvoid * semPtr)
{
    DSP_STATUS          status = DSP_SOK ;
    MESSAGE_SemObject * semObj ;
    int                 osStatus ;

    semObj = malloc (sizeof (MESSAGE_SemObject)) ;
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
 *  @func   MESSAGEMULTI_DeleteSem
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
MESSAGEMULTI_DeleteSem (IN Pvoid semHandle)
{
    DSP_STATUS          status = DSP_SOK ;
    MESSAGE_SemObject * semObj = semHandle ;
    int                 osStatus ;

    osStatus = sem_destroy (&(semObj->sem)) ;
    if (osStatus < 0) {
        status = DSP_EFAIL ;
    }

    free (semObj) ;

    return status ;
}


/** ============================================================================
 *  @func   MESSAGEMULTI_WaitSem
 *
 *  @desc   This function waits on a semaphore.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGEMULTI_WaitSem (IN Pvoid semHandle)
{
    DSP_STATUS          status = DSP_SOK ;
    MESSAGE_SemObject * semObj = semHandle ;
    int                 osStatus ;

    osStatus = sem_wait (&(semObj->sem)) ;
    if (osStatus < 0) {
        status = DSP_EFAIL ;
    }

    return status ;
}


/** ============================================================================
 *  @func   MESSAGEMULTI_PostSem
 *
 *  @desc   This function posts a semaphore.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGEMULTI_PostSem (IN Pvoid semHandle)
{
    DSP_STATUS          status = DSP_SOK ;
    MESSAGE_SemObject * semObj = semHandle ;
    int                 osStatus ;

    osStatus = sem_post (&(semObj->sem)) ;
    if (osStatus < 0) {
        status = DSP_EFAIL ;
    }

    return status ;
}


/** ============================================================================
 *  @func   MESSAGE_Atoll
 *
 *  @desc   Converts ascii to long int
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Uint32
MESSAGE_Atoll (Char8 * str)
{
     Uint32 val = 0 ;
     val = strtoll (str, NULL, 16) ;
     return val ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
