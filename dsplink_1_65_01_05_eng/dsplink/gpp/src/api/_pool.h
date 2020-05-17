/** ============================================================================
 *  @file   _pool.h
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Internal declarations for POOL component.
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


#if !defined (_POOL_H)
#define _POOL_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <_pooldefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  MAX_ADDR_TYPES
 *
 *  @desc   Maximum number of address types.
 *  ============================================================================
 */
#define MAX_ADDR_TYPES      4u


/** ============================================================================
 *  @name   POOL_AddrInfo
 *
 *  @desc   This structure defines the config attributes required for
 *          mapping/unmapping the buffers.
 *
 *  @field  isInit
 *              Pool is initialized or not.
 *  @field  addr
 *              Array of addresses containing the same address in different
 *              address spaces.
 *  @field  size
 *              Size of memory block in bytes.
 *  ============================================================================
 */
typedef struct POOL_AddrInfo_tag {
    Bool    isInit ;
    Uint32  addr [MAX_ADDR_TYPES] ;
    Uint32  size ;
} POOL_AddrInfo ;


/** ============================================================================
 *  @func   POOL_addrConfig
 *
 *  @desc   Pool driver specific configuration data.
 *
 *  @modif  object.
 *  ============================================================================
 */
extern POOL_AddrInfo POOL_addrConfig [MAX_DSPS][MAX_POOLENTRIES] ;


/** ============================================================================
 *  @func   _POOL_init
 *
 *  @desc   This function initializes the DRV POOL component.
 *
 *  @arg    None
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
EXPORT_API
Void
_POOL_init (Void) ;


/** ============================================================================
 *  @func   _POOL_exit
 *
 *  @desc   This function finalizes the DRV POOL component.
 *
 *  @arg    None
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
EXPORT_API
Void
_POOL_exit (Void) ;


/** ============================================================================
 *  @func   _POOL_xltBuf
 *
 *  @desc   This function translates the buffer address between two address
 *          spaces.
 *
 *  @arg    poolId
 *              Pool Identification number.
 *  @arg    bufPtr
 *              Pointer to the buffer. It contains the translated address on
 *              successful completion.
 *  @arg    xltFlag
 *              Flag to indicate whether user to kernel or reverse direction
 *              translation required.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_ERANGE
 *              Invalid address range.
 *
 *  @enter  poolId should be valid.
 *          bufPtr should be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_POOL_xltBuf (IN     PoolId           poolId,
              IN OUT Pvoid *          bufPtr,
              IN     POOL_AddrXltFlag xltFlag) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (_POOL_H) */
