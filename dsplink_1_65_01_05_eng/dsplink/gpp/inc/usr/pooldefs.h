/** ============================================================================
 *  @file   pooldefs.h
 *
 *  @path   $(DSPLINK)/gpp/inc/usr/
 *
 *  @desc   Definitions of constants and structures for pools.
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


#if !defined (POOLDEFS_H)
#define POOLDEFS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @name   POOL_makePoolId
 *
 *  @desc   This macro returns a Pool Id by combining PROCID and POOL No in the
 *          Following format:
 *          |<--- 8 bit --->|<--- 8 bit --->|
 *          |     ProcId    |     PoolNo    |
 *  ============================================================================
 */
#define POOL_makePoolId(procId, poolNo) (  ((poolNo) & 0xFFu)                  \
                                       | ((((procId) & 0xFFu) << 8u) & 0xFFFFu))

/** ============================================================================
 *  @name   POOL_getProcId
 *
 *  @desc   Returns the processor Identifier from the PoolId.
 *  ============================================================================
 */
#define POOL_getProcId(poolId)      ((((poolId) & 0xFF00u) >> 8u) & 0x00FFu)

/** ============================================================================
 *  @name   POOL_getPoolNo
 *
 *  @desc   Returns the pool Number from the PoolId.
 *  ============================================================================
 */
#define POOL_getPoolNo(poolId)      ((poolId) & 0x00FFu)

/** ============================================================================
 *  @name   MAX_POOLENTRIES
 *
 *  @desc   The maximum number of pools that can be configured per processors.
 *  ============================================================================
 */
#define MAX_POOLENTRIES     32u

/** ============================================================================
 *  @macro  IS_VALID_POOLID
 *
 *  @desc   Is the pool ID valid for a processor.
 *  ============================================================================
 */
#define IS_VALID_POOLID(id)   (POOL_getPoolNo((id)) < MAX_POOLENTRIES)

/** ============================================================================
 *  @name   MAX_SMABUFENTRIES
 *
 *  @desc   The maximum number of buffer pools that can be configured for SMA.
 *  ============================================================================
 */
#define MAX_SMABUFENTRIES   16u

/** ============================================================================
 *  @name   MAX_DMABUFENTRIES
 *
 *  @desc   The maximum number of buffer pools that can be configured for DMA.
 *  ============================================================================
 */
#define MAX_DMABUFENTRIES   16u


/** ============================================================================
 *  @name   AddrType
 *
 *  @desc   Enumerates the various types of addresses that can be translated
 *          by the POOL.
 *
 *  @field  AddrType_Usr
 *              User virtual address
 *  @field  AddrType_Phy
 *              Physical address
 *  @field  AddrType_Knl
 *              Kernel address
 *  @field  AddrType_Dsp
 *              DSP address (if the buffer is in shared memory).
 *  ============================================================================
 */
typedef enum {
    AddrType_Usr  = 0u,
    AddrType_Phy  = 1u,
    AddrType_Knl  = 2u,
    AddrType_Dsp  = 3u
} AddrType ;


/** ============================================================================
 *  @name   BUFPOOL_Attrs
 *
 *  @desc   This structure defines the attributes required for initialization of
 *          the buffer pool.
 *
 *  @field  numBufPools
 *              Number of entries in the arrays of buffer sizes and number of
 *              buffers in the params structure.
 *  @field  bufSizes
 *              Array of sizes of the buffers in the buffer pools.
 *              This array is of size numBufObjs.
 *  @field  numBuffers
 *              Array of number of buffers in all buffer pools.
 *              This array is of size numBufObjs.
 *  ============================================================================
 */
typedef struct BUFPOOL_Attrs_tag {
    Uint32     numBufPools ;
    Uint32 *   bufSizes ;
    Uint32 *   numBuffers ;
} BUFPOOL_Attrs ;

/** ============================================================================
 *  @name   SMAPOOL_Attrs
 *
 *  @desc   This structure defines the attributes of the SMA POOL.
 *
 *  @field  numBufPools
 *              Number of buffer pools.
 *  @field  bufSizes
 *              Array of sizes of the buffers in each buffer pools.
 *  @field  numBuffers
 *              Array of number of buffers in each buffer pools.
 *  @field  exactMatchReq
 *              Flag indicating whether requested size is to be rounded to
 *              nearest available size in Pools or exact match has to be
 *              performed.
 *  ============================================================================
 */
typedef struct SMAPOOL_Attrs_tag {
    Uint32        numBufPools ;
    Uint32  *     bufSizes    ;
    Uint32  *     numBuffers  ;
    Bool          exactMatchReq ;
} SMAPOOL_Attrs ;


/** ============================================================================
 *  @name   DMAPOOL_Attrs
 *
 *  @desc   This structure defines the attributes of the DMA POOL.
 *
 *  @field  numBufPools
 *              Number of buffer pools.
 *  @field  bufSizes
 *              Array of sizes of the buffers in each buffer pools.
 *  @field  numBuffers
 *              Array of number of buffers in each buffer pools.
 *  @field  exactMatchReq
 *              Flag indicating whether requested size is to be rounded to
 *              nearest available size in Pools or exact match has to be
 *              performed.
 *  ============================================================================
 */
typedef struct DMAPOOL_Attrs_tag {
    Uint32        numBufPools ;
    Uint32  *     bufSizes    ;
    Uint32  *     numBuffers  ;
    Bool          exactMatchReq ;
} DMAPOOL_Attrs ;


/** ============================================================================
 *  @deprecated The deprecated data structure BUFPOOL_Attrs has been replaced
 *              with BUFPOOL_Attrs.
 *
 *  ============================================================================
 */
#define BufPoolAttrs               BUFPOOL_Attrs

/** ============================================================================
 *  @deprecated The deprecated data structure SmaPoolAttrs has been replaced
 *              with SMAPOOL_Attrs.
 *
 *  ============================================================================
 */
#define SmaPoolAttrs               SMAPOOL_Attrs


#if defined (__cplusplus)
}
#endif


#endif /* !defined (POOLDEFS_H) */
