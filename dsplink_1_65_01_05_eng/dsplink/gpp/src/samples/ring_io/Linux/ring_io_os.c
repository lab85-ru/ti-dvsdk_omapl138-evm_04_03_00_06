/** ============================================================================
 *  @file   ring_io_os.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/ring_io/Linux/
 *
 *  @desc   OS specific implementation of functions used by the ring_io
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
#include <pthread.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <proc.h>
/*  ----------------------------------- Application Header            */
#include <ring_io.h>
#include <ring_io_os.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   RING_IO_SemObject
 *
 *  @desc   This object is used by various SEM API's.
 *
 *  @field  sem
 *              Linux semaphore.
 *
 *  @see    None
 *  ============================================================================
 */
typedef struct RING_IO_SemObject_tag {
    sem_t  sem ;
} RING_IO_SemObject ;

/** ============================================================================
 *  @func   RING_IO_0Print
 *
 *  @desc   Print a message without any arguments.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
RING_IO_0Print (Char8 * str)
{
    printf (str) ;
}

/** ============================================================================
 *  @func   RING_IO_1Print
 *
 *  @desc   Print a message with one arguments.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
RING_IO_1Print (Char8 * str, Uint32 arg)
{
    printf (str, arg) ;
}
/** ============================================================================
 *  @func   RING_IO_YieldClient
 *
 *  @desc   Yields tasks execution.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
RING_IO_YieldClient ()
{
    sched_yield() ;
}
/** ============================================================================
 *  @func   RING_IO_Sleep
 *
 *  @desc   Sleeps for the specified number of microseconds.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
RING_IO_Sleep (Uint32 uSec)
{
    usleep (uSec) ;
}

/** ============================================================================
 *  @func   RING_IO_IntToString
 *
 *  @desc   This function converts an integer value into a string.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
RING_IO_IntToString (IN Int num, OUT Char8 * str)
{
    /*  ------------------------------------------------------------------------
     *  There can be a max of 7 char str for signed int including terminator
     *  so we use a temp string of 7 char. The index is to the rightmost space
     *  ie. 5. We divide the number by 10, and successively store the remainder
     *  in the rightmost space while decrementing the counter. Later we add the
     *  sign if required.
     *  ------------------------------------------------------------------------
     */
    Int16      sign      = 0 ;
    Char8      remainder = 0 ;
    char       temp [11] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'} ;
    Int16      index     = 9 ;

    sign = (num < 0) ? -1 : 1 ;
    num = num * sign ;

    do {
        remainder = num % 10 ;
        temp [index] = '0' + remainder ;
        num = num / 10 ;
        index-- ;
    } while ((num > 0)&& (index >= 0)) ;


    if (sign == -1) {
        temp [index] = '-' ;
    }
    else {
        index++ ;
    }

    strncpy (str, &(temp [index]), 11) ;
}

/** ============================================================================
 *  @func   RING_IO_CreateSem
 *
 *  @desc   This function creates a semaphore.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
RING_IO_CreateSem (OUT Pvoid * semPtr)
{
    DSP_STATUS          status = DSP_SOK ;
    RING_IO_SemObject * semObj ;
    int                 osStatus ;

    semObj = malloc (sizeof (RING_IO_SemObject)) ;
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

    return (status) ;
}

/** ============================================================================
 *  @func   RING_IO_DeleteSem
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
RING_IO_DeleteSem (IN Pvoid semHandle)
{
    DSP_STATUS          status = DSP_SOK ;
    RING_IO_SemObject * semObj = semHandle ;
    int                 osStatus ;

    osStatus = sem_destroy (&(semObj->sem)) ;
    if (osStatus < 0) {
        status = DSP_EFAIL ;
    }

    free (semObj) ;

    return (status) ;
}

/** ============================================================================
 *  @func   RING_IO_WaitSem
 *
 *  @desc   This function waits on a semaphore.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
RING_IO_WaitSem (IN Pvoid semHandle)
{
    DSP_STATUS          status = DSP_SOK ;
    RING_IO_SemObject * semObj = semHandle ;
    int                 osStatus ;

    osStatus = sem_wait (&(semObj->sem)) ;
    if (osStatus < 0) {
        status = DSP_EFAIL ;
    }

    return (status) ;
}

/** ============================================================================
 *  @func   RING_IO_PostSem
 *
 *  @desc   This function posts a semaphore.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
RING_IO_PostSem (IN Pvoid semHandle)
{
    DSP_STATUS          status = DSP_SOK ;
    RING_IO_SemObject * semObj = semHandle ;
    int                 osStatus ;

    osStatus = sem_post (&(semObj->sem)) ;
    if (osStatus < 0) {
        status = DSP_EFAIL ;
    }

    return (status) ;
}

#ifdef RING_IO_MULTIPROCESS
 /** ============================================================================
 *  @func   RING_IO_getLinkAccess
 *
 *  @desc   Function  that allows the child  process to  use the link
 *          components.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Int
RING_IO_getLinkAccess(Uint8 processorId)
{
    DSP_STATUS  status   = DSP_SOK ;

    /* Call the Link APIs that allows child process to be able to use the
     * Link components
     */

    status = PROC_Attach (processorId, NULL) ;

    if (DSP_FAILED (status)) {
        RING_IO_1Print ("Attach Failed. Status = [0x%x]", status) ;
    }

    return (status) ;
}
#endif

/** ============================================================================
 *  @func   RING_IO_Create_client
 *
 *  @desc   Function to create a new thread or a Process.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Uint32
RING_IO_Create_client (RING_IO_ClientInfo * pInfo, Pvoid funcPtr, Pvoid args)
{
    DSP_STATUS  status      = DSP_SOK ;

#ifdef RING_IO_MULTIPROCESS
    pid_t       processId ;
    int         result ;
    DSP_STATUS  retStatus   = DSP_SOK ;
    int         (*lptrToFun)(void*) = NULL ;

#endif

#ifdef RING_IO_MULTIPROCESS
    /* Create a child process */
    processId = fork() ;
    if (processId ==  0)  {
        /* In Child Process */

        /* Get the access privileges for the child process */
        retStatus = RING_IO_getLinkAccess(pInfo->processorId);
        if (DSP_FAILED(retStatus)) {
            RING_IO_0Print ("ERR: Unable to initialize DspLink in  Client \n"
                            /*,&pInfo->processName */ );
        }else {
            lptrToFun = funcPtr;
            /* Call the user function */
            result =(lptrToFun) (args) ;
        }

        /* Exit from the child process */
        exit (0) ;
    }
    else if (processId < 0) {
        status = -1 ;
        RING_IO_1Print ("Call to fork failed. Status [0x%x]\n",
                         status) ;
    }
    else {
        /* In parent Process */
        pInfo->pid = processId ;
        status = 0 ;
    }

#else
    status = pthread_create(&pInfo->tid,
                            NULL,/* Attributes of the thread.*/
                            (void*) funcPtr, /* Pointer to Function.*/
                            args);
#endif

    switch(status) {
        case 0:
            return (DSP_SOK);
        case -1:
            return (DSP_EFAIL);
    }

    return (status) ;
}

/** ============================================================================
 *  @func   RING_IO_Join_client
 *
 *  @desc   Wait for thread/process to terminate.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RING_IO_Join_client (RING_IO_ClientInfo *pInfo)
{
    DSP_STATUS  status   = DSP_EFAIL ;
#ifdef RING_IO_MULTIPROCESS
    int         statLoc ;
#endif

#ifdef RING_IO_MULTIPROCESS
    do {
        status = waitpid (pInfo->pid,
                          &statLoc,
                          (   WSTOPPED
                            | WUNTRACED
                           ) );
        if (status < 0) {
            if (EINTR == errno){
                RING_IO_0Print ("Signal received in Main process !!! \n") ;
                RING_IO_0Print ("Terminate the child process \n") ;
                status = API_RESTART ;
            }
            else {
                RING_IO_1Print ("Error exit from wait. Status [0x%x]\n",
                                status) ;
                RING_IO_1Print ( "errno is %d \n", errno );
                status = DSP_EFAIL ;
            }
        }
        else {
            if (WIFEXITED(statLoc) != 0) {
                /* Child process exited normally */
                status = DSP_SOK ;
            }
            else if (WIFSIGNALED(statLoc)) {

                /* Child exited due to uncaught signal */
                RING_IO_1Print (" Child process exited due to signal %d \n",
                                  WTERMSIG (statLoc)) ;
                /* Terminate the child processes and Main process also */
                kill(0, SIGABRT) ;
                status = DSP_EFAIL ;

            } else {
                 RING_IO_1Print (" Child process exited due to status 0%x \n",
                                   status) ;
            }
        }

    } while (status == API_RESTART);

#else
    status = pthread_join(pInfo->tid, NULL);
#endif

    if (status != 0) {
        status = DSP_EFAIL ;
    }
    else {
        status = DSP_SOK;
    }
    return (status) ;
}

/** ============================================================================
 *  @func   RING_IO_Exit_client
 *
 *  @desc   Function to exit from a caller task(thread/process).
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RING_IO_Exit_client (RING_IO_ClientInfo *pInfo)
{
    DSP_STATUS status   = DSP_SOK;

#ifdef RING_IO_MULTIPROCESS
    if(pInfo != NULL) {
        PROC_detach(pInfo->processorId) ;
    }
    exit (0) ;
#else
    pthread_exit(NULL);
#endif

    return (status) ;
}


/** ============================================================================
 *  @func   RING_IO_OS_init
 *
 *  @desc   This function creates a mem pool.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
RING_IO_OS_init(Void)
{
    DSP_STATUS  status = DSP_SOK ;

    return status ;
}


/** ============================================================================
 *  @func   RING_IO_OS_exit
 *
 *  @desc   This function deletes a mem pool.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
RING_IO_OS_exit(Void)
{
    DSP_STATUS status = DSP_SOK ;

    return status ;
}

/** ============================================================================
 *  @func   RING_IO_Atoll
 *
 *  @desc   Converts ascii to long int
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Uint32
RING_IO_Atoll (Char8 * str)
{
     Uint32 val = 0 ;
     val = strtoll (str, NULL, 16) ;
     return val ;
}

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
