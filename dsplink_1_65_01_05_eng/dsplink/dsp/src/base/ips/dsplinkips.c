/** ============================================================================
 *  @file   dsplinkips.c
 *
 *  @path   $(DSPLINK)/dsp/src/base/ips/
 *
 *  @desc   Implementation of the DSPLINK Inter Processor Signaling Manager
 *          component.
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

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <dbc.h>
#include <failure.h>
#include <hal_cache.h>
#include <dsplinkips.h>
#include <ips.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define FILEID               FID_DSPLINKIPS_C

/** ============================================================================
 *  Create named sections for all DSPLINK functions to allow specific memory
 *  placement.
 *  ============================================================================
 */
#pragma CODE_SECTION (DSPLINKIPS_init,              ".text:DSPLINK_init")


/** ============================================================================
 *  @func   DSPLINKIPS_init
 *
 *  @desc   This function initializes the DSPLINKIPS component.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
DSPLINKIPS_init (IN Uint16 procId, IN Uint32 numIps, IN Uint32 shAddr)
{
    Int               status  = SYS_OK ;
    DSPLINKIPS_Ctrl * ctrlPtr = (DSPLINKIPS_Ctrl *) shAddr ;
    Uint32            i ;

    DBC_assert (ctrlPtr != NULL) ;

    /* Initialize all IPS objects for this DSP. */
    for (i = 0 ; (i < numIps) && (status == SYS_OK) ; i++) {
        HAL_cacheInv ((Void *) &(ctrlPtr [i]), sizeof (DSPLINKIPS_Ctrl)) ;

        status = IPS_init (procId, i, (Uint32) &(ctrlPtr [i])) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
