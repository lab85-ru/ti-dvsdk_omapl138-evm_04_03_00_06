/** ============================================================================
 *  @file   dsplinkpool.h
 *
 *  @path   $(DSPLINK)/dsp/src/pools/
 *
 *  @desc   Defines the interface of the DSPLINK POOL Manager component.
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


#if !defined (DSPLINKPOOL_H)
#define DSPLINKPOOL_H


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   DSPLINKPOOL_Config
 *
 *  @desc   This structure defines the configuration information shared between
 *          the POOL instances on the GPP and DSP-sides.
 *
 *  @field  poolSize
 *              Size of the buffer pool.
 *  @field  ipsId
 *              ID of the IPS used (if any). A value of -1 indicates that no IPS
 *              is required by the pool.
 *  @field  ipsEventNo
 *              IPS Event number associated with POOL (if any).  A value of -1
 *              indicates that no IPS is required by the pool.
 *  @field  arg1
 *              First optional pool-specific argument.
 *  @field  arg2
 *              Second optional pool-specific argument.
 *  ============================================================================
 */
typedef struct DSPLINKPOOL_Config_tag {
    Uint32            poolSize ;
    Uint32            ipsId ;
    Uint32            ipsEventNo ;
    Uint32            arg1 ;
    Uint32            arg2 ;
} DSPLINKPOOL_Config ;

/** ============================================================================
 *  @name   DSPLINKPOOL_Ctrl
 *
 *  @desc   This structure defines the control structure required by the
 *          LDRV_POOL component for each POOL instance
 *
 *  @field  dspAddr
 *              DSP address of the shared control structure for the POOL
 *              instance.
 *  @field  config
 *              Configuration structure for the POOL instance.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct DSPLINKPOOL_Ctrl_tag {
    Uint32             dspAddr ;
    DSPLINKPOOL_Config config ;
    Uint16             padding [DSPLINKPOOL_CTRL_PADDING] ;
} DSPLINKPOOL_Ctrl ;


/** ============================================================================
 *  @func   DSPLINKPOOL_init
 *
 *  @desc   This function initializes the DSPLINKPOOL component.
 *
 *  @arg    procId
 *              Processor Identifier with which the POOLs are shared.
 *  @arg    numPools
 *              Number of POOL instances configured.
 *  @arg    shAddr
 *              Address in shared memory for the POOL control structure.
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
DSPLINKPOOL_init (IN Uint16 procId, IN Uint32 numPools, IN Uint32 shAddr) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* if !defined (DSPLINKPOOL_H) */
