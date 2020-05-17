/** ============================================================================
 *  @file   shm_drv.h
 *
 *  @path   $(DSPLINK)/dsp/src/base/drv/DspBios/
 *
 *  @desc   Declarations for the Shared Memory Driver.
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


#if !defined (SHM_DRV_H)
#define SHM_DRV_H


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <platform.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  SHMDRV_CTRL_PADDING
 *
 *  @desc   Padding required for L2 cache line alignment within SHMDRV control
 *          structure.
 *  ============================================================================
 */
#define SHMDRV_CTRL_PADDING  ((CACHE_L2_LINESIZE - (sizeof (Uint32) * 2)) / 2)


/** ============================================================================
 *  @name   SHMDRV_Ctrl
 *
 *  @desc   Defines the control structure used for handshaking over the
 *          SHM driver.
 *
 *  @field  handshakeGpp
 *              Handshake field to be updated by GPP.
 *  @field  handshakeGpp
 *              Handshake field to be updated by DSP.
 *  @field  padding
 *              Padding for alignment
 *  ============================================================================
 */
typedef struct SHMDRV_Ctrl_tag {
    volatile Uint32  handshakeGpp ;
    volatile Uint32  handshakeDsp ;
             Uint16  padding [SHMDRV_CTRL_PADDING] ;
} SHMDRV_Ctrl ;


/** ============================================================================
 *  @func   SHMDRV_init
 *
 *  @desc   Performs global initialization of the SHM driver.
 *
 *  @arg    procId
 *              Processor Identifier with which the SHM driver is shared.
 *  @arg    shAddr
 *              Address in shared memory for the SHMDRV control structure.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid argument.
 *          SYS_EALLOC
 *              Memory allocation failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
Int
SHMDRV_init (IN Uint16 procId, IN Uint32 shAddr) ;


/** ============================================================================
 *  @func   SHMDRV_handshake
 *
 *  @desc   Performs handshaking with the GPP to synchronize the drivers on both
 *          processors.
 *
 *  @arg    procId
 *              Processor Identifier with which the SHM driver is shared.
 *  @arg    dspHandshake
 *              DSP handshake value to be sent to the GPP.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  The SHM drivers on the two processors must be synchronized.
 *
 *  @see    None.
 *  ============================================================================
 */
Void SHMDRV_handshake (IN Uint32 procId, IN Uint16 dspHandshake) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (SHM_DRV_H) */
