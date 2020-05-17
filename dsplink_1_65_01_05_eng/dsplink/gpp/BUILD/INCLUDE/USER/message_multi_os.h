/** ============================================================================
 *  @file   message_multi_os.h
 *
 *  @path   $(DSPLINK)/gpp/src/samples/message_multi/Linux/
 *
 *  @desc   OS specific definitions for the message_multi sample.
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


#if !defined (MESSAGEMULTI_OS_H)
#define MESSAGEMULTI_OS_H


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

#include <string.h>

/** ============================================================================
 *  @func   atoi
 *
 *  @desc   Extern declaration for function that converts a string into an
 *          integer value.
 *
 *  @arg    str
 *              String representation of the number.
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
extern int atoi (const char * str) ;


/** ============================================================================
 *  @func   MESSAGEMULTI_CreateSem
 *
 *  @desc   This function creates a semaphore.
 *
 *  @arg    semPtr
 *              Location to receive the semaphore object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
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
MESSAGEMULTI_CreateSem (OUT Pvoid * semPtr) ;


/** ============================================================================
 *  @func   MESSAGEMULTI_DeleteSem
 *
 *  @desc   This function deletes a semaphore.
 *
 *  @arg    semHandle
 *              Pointer to the semaphore object to be deleted.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
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
MESSAGEMULTI_DeleteSem (IN Pvoid semHandle) ;


/** ============================================================================
 *  @func   MESSAGEMULTI_WaitSem
 *
 *  @desc   This function waits on a semaphore.
 *
 *  @arg    semHandle
 *              Pointer to the semaphore object to wait on.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
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
MESSAGEMULTI_WaitSem (IN Pvoid semHandle) ;


/** ============================================================================
 *  @func   MESSAGEMULTI_PostSem
 *
 *  @desc   This function posts a semaphore.
 *
 *  @arg    semHandle
 *              Pointer to the semaphore object to be posted.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
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
MESSAGEMULTI_PostSem (IN Pvoid semHandle) ;


/** ============================================================================
 *  @func   strcat
 *
 *  @desc   Extern declaration for function that concatenates two strings.
 *
 *  @arg    dest
 *              Destination string.
 *  @arg    src
 *              Source string to be appended to the destination string.
 *
 *  @ret    Pointer to the resulting string.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
extern char * strcat (char * dest, const char * src) ;


/** ============================================================================
 *  @func   strcpy
 *
 *  @desc   Extern declaration for function that copies strings
 *
 *  @arg    dest
 *              Destination string.
 *  @arg    src
 *              Source string
 *
 *  @ret    None.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
extern char * strcpy (char * dest, const char * src) ;


/** ============================================================================
 *  @func   MESSAGE_Atoi
 *
 *  @desc   This function converts a string into an integer value.
 *
 *  @arg    str
 *              String representation of the number.
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
#define MESSAGEMULTI_Atoi atoi


/** ============================================================================
 *  @func   MESSAGEMULTI_Strcat
 *
 *  @desc   This function concatenates two strings.
 *
 *  @arg    dest
 *              Destination string.
 *  @arg    src
 *              Source string to be appended to the destination string.
 *
 *  @ret    Pointer to the resulting string.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
#define MESSAGEMULTI_Strcat strcat


/** ============================================================================
 *  @func   MESSAGEMULTI_Strcpy
 *
 *  @desc   This function copies source string to destination string
 *
 *  @arg    dest
 *              Destination string.
 *  @arg    src
 *              Source string.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
#define MESSAGEMULTI_Strcpy strcpy


/** ============================================================================
 *  @func   MESSAGEMULTI_OS_init
 *
 *  @desc   This function initializes the OS specific component.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
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
MESSAGEMULTI_OS_init (Void) ;


/** ============================================================================
 *  @func   MESSAGEMULTI_OS_exit
 *
 *  @desc   This function finalizes the OS specific component.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
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
MESSAGEMULTI_OS_exit (Void) ;


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
MESSAGE_Atoll (Char8 * str) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MESSAGEMULTI_OS_H) */
