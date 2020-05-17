/** ============================================================================
 *  @file   shm_drv.c
 *
 *  @path   $(DSPLINK)/dsp/src/base/drv/DspBios/
 *
 *  @desc   Implementation of Shared Memory Driver functionality.
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
#include <platform.h>
#include <dbc.h>
#include <hal_cache.h>
#include <hal_interrupt.h>
#include <shm_drv.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define FILEID               FID_SHMDRV_C

/** ============================================================================
 *  @const  GPP_HANDSHAKE
 *
 *  @desc   Handshake value written by GPP.
 *  ============================================================================
 */
#define GPP_HANDSHAKE       (Uint32) 0xC0C00000

/** ============================================================================
 *  @const  DSP_HANDSHAKE
 *
 *  @desc   Handshake value written by DSP.
 *  ============================================================================
 */
#define DSP_HANDSHAKE       (Uint32) 0xBABA0000


/** ----------------------------------------------------------------------------
 *  @name   SHMDRV_ctrlPtr
 *
 *  @desc   Pointer to the shared memory control structure for the SHMDRV
 *          component.
 *  ----------------------------------------------------------------------------
 */
static SHMDRV_Ctrl * SHMDRV_ctrlPtr = NULL ;

/** ============================================================================
 *  Create named sections for all SHMDRV functions to allow specific memory
 *  placement.
 *  ============================================================================
 */
#pragma CODE_SECTION (SHMDRV_init,                 ".text:DSPLINK_init")
#pragma CODE_SECTION (SHMDRV_handshake,            ".text:DSPLINK_init")


/** ============================================================================
 *  @func   SHMDRV_init
 *
 *  @desc   Performs global initialization of the SHM driver.
 *
 *  @modif  SHMDRV_obj
 *  ============================================================================
 */
Int
SHMDRV_init (IN Uint16 procId, IN Uint32 shAddr)
{
    SHMDRV_ctrlPtr = (SHMDRV_Ctrl *) shAddr ;

    (Void) procId ; /* To avoid compiler warning. */

    DBC_ensure (SHMDRV_ctrlPtr != NULL) ;

    return SYS_OK ;
}


/** ----------------------------------------------------------------------------
 *  @func   SHMDRV_handshake
 *
 *  @desc   Performs handshaking with the GPP to synchronize the drivers on both
 *          processors.
 *
 *  @modif  SHMDRV_obj.ptrCtrl
 *  ----------------------------------------------------------------------------
 */
Void
SHMDRV_handshake  (IN Uint32 procId, IN Uint16 dspHandshake)
{
    (Void) procId ; /* To avoid compiler warning. */

    /* Read the value of token that GPP writes as part of synchronization */
    do {
        HAL_cacheInv ((Void *) SHMDRV_ctrlPtr, sizeof (SHMDRV_Ctrl)) ;
    }
    while (SHMDRV_ctrlPtr->handshakeGpp != GPP_HANDSHAKE) ;

    /* Write the value on which GPP is waiting as part of synchronization
     * procedure.
     */
    SHMDRV_ctrlPtr->handshakeDsp |= (DSP_HANDSHAKE | dspHandshake) ;
    HAL_cacheWbInv ((Void *) SHMDRV_ctrlPtr, sizeof (SHMDRV_Ctrl)) ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
