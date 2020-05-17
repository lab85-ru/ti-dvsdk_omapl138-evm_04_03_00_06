/** ============================================================================
 *  @file   _notify.c
 *
 *  @path   $(DSPLINK)/dsp/src/notify/
 *
 *  @desc   Implementation of internal functions for API sub-component NOTIFY.
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


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <sys.h>
#include <que.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <failure.h>
#include <dbc.h>
#include <hal_cache.h>
#include <_notify.h>
#include <notify.h>
#include <dsplinkips.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define FILEID    FID__NOTIFY_C

/** ============================================================================
 *  @name   NOTIFY_ctrlPtr
 *
 *  @desc   Pointer to the shared memory control structure for the NOTIFY
 *          component.
 *  ============================================================================
 */
DSPLINKIPS_Ctrl *  NOTIFY_ctrlPtr = NULL ;

/** ============================================================================
 *  @name   NOTIFY_numIps
 *
 *  @desc   Number of IPS instances configured for this DSP.
 *  ============================================================================
 */
Uint32             NOTIFY_numIps  = 0 ;


/** ============================================================================
 *  @func   NOTIFY_init
 *
 *  @desc   This function initializes the NOTIFY module.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
NOTIFY_init (IN Uint16 procId, IN Uint32 numIps, IN Uint32 shAddr)
{
    Int status = SYS_OK ;
    Uint32   i ;

    (void) procId ;
    DBC_require (shAddr != (Uint32) NULL) ;

    NOTIFY_ctrlPtr = (DSPLINKIPS_Ctrl *) shAddr ;
    NOTIFY_numIps  = numIps ;

    for (i = 0 ; (i < NOTIFY_numIps) ; i++) {
        HAL_cacheInv ((Void *) &(NOTIFY_ctrlPtr [i]), sizeof (DSPLINKIPS_Ctrl));
    }
    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
