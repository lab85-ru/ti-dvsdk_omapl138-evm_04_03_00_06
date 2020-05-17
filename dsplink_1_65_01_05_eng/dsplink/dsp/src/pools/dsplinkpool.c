/** ============================================================================
 *  @file   dsplinkpool.c
 *
 *  @path   $(DSPLINK)/dsp/src/pools/
 *
 *  @desc   Implementation of the DSPLINK POOL Manager component.
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
#include <dsplinkpool.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define FILEID               FID_DSPLINKPOOL_C

/** ============================================================================
 *  @const  DSPLINKPOOL_ctrlPtr
 *
 *  @desc   Array of pointers to shared control regions for all pool instances.
 *  ============================================================================
 */
DSPLINKPOOL_Ctrl * DSPLINKPOOL_ctrlPtr [MAX_POOLENTRIES] ;

/** ============================================================================
 *  Create named sections for all DSPLINK functions to allow specific memory
 *  placement.
 *  ============================================================================
 */
#pragma CODE_SECTION (DSPLINKPOOL_init,             ".text:DSPLINK_init")


/** ============================================================================
 *  @func   DSPLINKPOOL_init
 *
 *  @desc   This function initializes the DSPLINKPOOL component.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
DSPLINKPOOL_init (IN Uint16 procId, IN Uint32 numPools, IN Uint32 shAddr)
{
    Int                status  = SYS_OK ;
    DSPLINKPOOL_Ctrl * ctrlPtr = (DSPLINKPOOL_Ctrl *) shAddr ;
    Uint32             i ;

    (Void) procId ; /* To avoid compiler warning. */

    /* Initialize all POOL objects for this DSP. */
    for (i = 0 ; i < numPools ; i++) {
        DSPLINKPOOL_ctrlPtr [i] = &(ctrlPtr [i]) ;
        HAL_cacheInv ((Void *) DSPLINKPOOL_ctrlPtr [i],
                      sizeof (DSPLINKPOOL_Ctrl)) ;
    }

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
