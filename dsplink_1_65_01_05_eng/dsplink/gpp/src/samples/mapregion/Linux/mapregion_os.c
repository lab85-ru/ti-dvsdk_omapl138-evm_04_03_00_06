/** ============================================================================
 *  @file   mapregion_os.c
 *
 *  @path   $(DSPLINK)/gpp/src/samples/mapregion/Linux/
 *
 *  @desc   OS specific implementation of functions used by the mapregion
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
#include <malloc.h>

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>

/*  ----------------------------------- Application Header            */
#include <mapregion.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */



/** ============================================================================
 *  @func   MAPREGION_0Print
 *
 *  @desc   Print a message without any arguments.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MAPREGION_0Print (Char8 * str)
{
    printf (str) ;
    fflush (stdout) ;
}


/** ============================================================================
 *  @func   MAPREGION_1Print
 *
 *  @desc   Print a message with one arguments.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MAPREGION_1Print (Char8 * str, Uint32 arg)
{
    printf (str, arg) ;
    fflush (stdout) ;
}

/** ============================================================================
 *  @func   MAPREGION_Sleep
 *
 *  @desc   Sleeps for the specified number of microseconds.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\MAPREGION_os.c
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
MAPREGION_Sleep (IN Uint32 uSec)
{
    usleep (uSec) ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
