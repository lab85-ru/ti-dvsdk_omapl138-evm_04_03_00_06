/** ============================================================================
 *  @file   CFG_Linux.c
 *
 *  @path   $(DSPLINK)/config/all/
 *
 *  @desc   Defines the GPP-OS specific configuration information for
 *          DSP/BIOS LINK for Linux.
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


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <procdefs.h>


#if defined (__cplusplus)
EXTERN "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   LINKCFG_sigNumArray
 *
 *  @desc   Array of signal numbers
 *          NOTE: The number of signals entered in this array must match the
 *                value of NUMSIGNALS in LINKCFG_gppOsObject object.
 *  ============================================================================
 */
STATIC Uint32  LINKCFG_sigNumArray [] = {
    1u,                         /* SIGHUP   */
    2u,                         /* SIGINT   */
    4u,                         /* SIGILL   */
    6u,                         /* SIGABRT  */
    8u,                         /* SIGFPE   */
    11u,                        /* SIGSEGV  */
    14u,                        /* SIGALRM  */
    15u                         /* SIGTERM  */
} ;

/** ============================================================================
 *  @name   LINKCFG_gppOsObject
 *
 *  @desc   Object for the GPP OS specific configuration.
 *  ============================================================================
 */
LINKCFG_GppOs LINKCFG_gppOsObject = {
    TRUE,                           /* HANDLESIGNALS  : Should signals be handled for cleanup */
    8u,                             /* NUMSIGNALS     : Number of signals to be handled */
    (Uint32 *) &LINKCFG_sigNumArray /* SIGNUMARRAY    : Pointer to the array of signals to be handled */
} ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


