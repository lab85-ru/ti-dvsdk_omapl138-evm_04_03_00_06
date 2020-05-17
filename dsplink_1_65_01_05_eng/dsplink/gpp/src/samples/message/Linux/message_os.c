/** ============================================================================
 *  @file   message_os.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/message/Linux/
 *
 *  @desc   OS specific implementation of functions used by the message
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
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>

/*  ----------------------------------- Application Header            */
#include <message.h>
#include <message_os.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

#if defined (MESSAGE_PROFILE)
/** ============================================================================
 *  @name   startUsecTime
 *
 *  @desc   This variable stores the value of start time in microseconds.
 *  ============================================================================
 */
STATIC Uint32 startUsecTime = 0 ;

/** ============================================================================
 *  @name   endUsecTime
 *
 *  @desc   This variable stores the value of end time in microseconds.
 *  ============================================================================
 */
STATIC Uint32 endUsecTime = 0 ;

/** ============================================================================
 *  @name   initialUsecTime
 *
 *  @desc   This variable stores the value of reset time in microsecond.
 *  ============================================================================
 */
STATIC struct timeval initialUsecTime ;

#endif /* if defined (MESSAGE_PROFILE) */


/** ============================================================================
 *  @func   MESSAGE_0Print
 *
 *  @desc   Print a message without any arguments.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGE_0Print (Char8 * str)
{
    printf (str) ;
}


/** ============================================================================
 *  @func   MESSAGE_1Print
 *
 *  @desc   Print a message with one arguments.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGE_1Print (Char8 * str, Uint32 arg)
{
    printf (str, arg) ;
}


/** ============================================================================
 *  @func   MESSAGE_Sleep
 *
 *  @desc   Sleeps for the specified number of microseconds.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGE_Sleep (Uint32 uSec)
{
    usleep (uSec) ;
}


/** ============================================================================
 *  @func   MESSAGE_Atoll
 *
 *  @desc   Converts ascii to long int
 *
 *  @ret    <valid integer>
 *              If the 'initial' part of the string represents a valid integer
 *          0
 *              If the string passed does not represent an integer or is NULL.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
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


#if defined (MESSAGE_PROFILE)
/** ============================================================================
 *  @func   MESSAGE_ResetUsecTime
 *
 *  @desc   This function records the reset time in microseconds.
 *
 *  @modif  initialUsecTime
 *  ============================================================================
 */
STATIC
Void
MESSAGE_ResetUsecTime (Void)
{
    DSP_STATUS     status   = DSP_SOK ;
    Int32          osStatus = 0       ;

    osStatus = gettimeofday (&initialUsecTime, NULL) ;
    if (osStatus != 0) {
        status = DSP_EFAIL ;
        MESSAGE_1Print ("System call gettimeofday returned error."
                        " Status = 0x%x", status) ;
    }
}


/** ============================================================================
 *  @func   MESSAGE_GetStartTime ()
 *
 *  @desc   This function records the reset time in microseconds.
 *
 *  @modif  startUsecTime
 *  ============================================================================
 */
EXPORT_API
Void
MESSAGE_GetStartTime (Void)
{
    DSP_STATUS     status   = DSP_SOK ;
    Int32          osStatus = 0       ;
    struct timeval currentTime        ;

    MESSAGE_ResetUsecTime () ;

    osStatus = gettimeofday (&currentTime, NULL) ;
    if (osStatus != 0) {
        status = DSP_EFAIL ;
        MESSAGE_1Print ("System call gettimeofday returned error."
                        " Status = 0x%x", status) ;
    }
    else {
        startUsecTime = (  (     (currentTime.tv_sec - initialUsecTime.tv_sec)
                               * 1000000)
                            + (currentTime.tv_usec - initialUsecTime.tv_usec)) ;
    }
}


/** ============================================================================
 *  @func   MESSAGE_GetEndTime ()
 *
 *  @desc   This function records the reset time in microseconds.
 *
 *  @modif  endUsecTime
 *  ============================================================================
 */
EXPORT_API
Void
MESSAGE_GetEndTime (Void)
{
    DSP_STATUS     status   = DSP_SOK ;
    Int32          osStatus = 0       ;
    struct timeval currentTime        ;

    osStatus = gettimeofday (&currentTime, NULL) ;
    if (osStatus != 0) {
        status = DSP_EFAIL ;
        MESSAGE_1Print ("System call gettimeofday returned error."
                        " Status = 0x%x", status) ;
    }
    else {
        endUsecTime = (  (  (currentTime.tv_sec - initialUsecTime.tv_sec)
                            * 1000000)
                          + (currentTime.tv_usec - initialUsecTime.tv_usec)) ;
    }
}

/** ============================================================================
 *  @func   MESSAGE_GetProfileInfo
 *
 *  @desc   It converts a time variable to elapsed time structure.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
MESSAGE_GetProfileInfo (Uint32 numIterations)
{
    Real32     roundTripTime = 0    ;
    Uint32     usecTimeTaken = 0    ;
    Real32     numSeconds    = 0    ;

    usecTimeTaken = (endUsecTime - startUsecTime) ;
    if (usecTimeTaken != 0) {
        roundTripTime = (Real32)(  (Real32)(usecTimeTaken)
                                 / (Real32)(numIterations)) ;
    }

    /* To calculate number of seconds in usecTimeTaken */
    numSeconds      = (Real32)((Real32) usecTimeTaken / 1000000.0) ;

    MESSAGE_1Print ("Transferring %d iterations took ", numIterations) ;
    MESSAGE_1Print ("%d seconds ", numSeconds) ;
    MESSAGE_1Print ("%d microseconds.\n", (Real32)(usecTimeTaken % 1000000)) ;

    MESSAGE_1Print ("RoundTrip Time for 1 message is %d microseconds.\n",
                    roundTripTime) ;

    return ;
}

#endif /* if defined (MESSAGE_PROFILE) */

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
