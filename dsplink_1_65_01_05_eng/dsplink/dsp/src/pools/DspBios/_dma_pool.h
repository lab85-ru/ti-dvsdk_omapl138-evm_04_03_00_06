/** ============================================================================
 *  @file   _dma_pool.h
 *
 *  @path   $(DSPLINK)/dsp/src/pools/DspBios/
 *
 *  @desc   Internal declarations for the DMAPOOL - DMA Memory Allocator POOL
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


#if !defined (_DMA_POOL_H)
#define _DMA_POOL_H


/*  ----------------------------------- DSP/BIOS Headers            */
#include <pool.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <platform.h>
#include <shm_drv.h>
#include <ips.h>
#if defined (CHNL_COMPONENT)
#include <_zcpy_data.h>
#endif /* if defined (CHNL_COMPONENT) */
#include <mpcs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   MAX_DMABUFENTRIES
 *
 *  @desc   The maximum number of buffer pools that can be configured for DMA.
 *  ============================================================================
 */
#define MAX_DMABUFENTRIES   16u


/** ============================================================================
 *  @const  DMAPOOL_CTRL_PADDING
 *
 *  @desc   Padding required for DSP L2 cache line alignment for SMAPOOL control
 *          structure.
 *          This value is calculated based on the sum of the sizes of the fields
 *          within the control structure.
 *  ============================================================================
 */
#define DMAPOOL_CTRL_PADDING      ((  CACHE_L2_LINESIZE                        \
                                    - (  sizeof (Uint16)                       \
                                       + sizeof (Uint32))) / 2)

/** ============================================================================
 *  @const  DMAPOOL_BUF_PADDING
 *
 *  @desc   Padding required for DSP L2 cache line alignment for SMABUF object.
 *          This value is calculated based on the sum of the sizes of the fields
 *          within the structure.
 *  ============================================================================
 */
#define DMAPOOL_BUF_PADDING       ((  CACHE_L2_LINESIZE                         \
                                    - (  (6 * sizeof (Uint16))                 \
                                       + (6 * sizeof (Uint32)))) / 2)

/** ============================================================================
 *  @const  DMAPOOL_OBJ_PADDING
 *
 *  @desc   Padding required for L2 cache line alignment within the SMAPOOL
 *          object.
 *          This value is calculated based on the sum of the sizes of the fields
 *          within the structure.
 *  ============================================================================
 */
#define DMAPOOL_OBJ_PADDING     ((CACHE_L2_LINESIZE - sizeof (Uint16)) / 2)


/** ============================================================================
 *  @name   DMAPOOL_BufObj
 *
 *  @desc   This structure defines the buffer pool object. It maintains the pool
 *          of buffers of a particular fixed size.
 *
 *  @field  nextFree
 *              Pointer to next free buffer.
 *  @field  reserved1
 *              Reserved.
 *  @field  bufDspAddress
 *              The start address of buffer pool in DSP address space.
 *  @field  bufDspPhysAddress
 *              The physical address of buffer pool in DSP address space.
 *  @field  bufGppPhysAddress
 *              The physical address of buffer pool in GPP address space.
 *  @field  size
 *              Size of the buffers in this pool.
 *  @field  dspMaduSize
 *              Size of DSP's elementary addressable unit.
 *  @field  reserved2
 *              Reserved.
 *  @field  totalBuffers
 *              Total number of buffers in pool.
 *  @field  freeBuffers
 *              Number of free buffers in pool.
 *  @field  reserved3
 *              Reserved.
 *  @field  reserved4
 *              Reserved.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct DMAPOOL_BufObj_tag {
    volatile Uint32      nextFree      ;
    volatile Uint32      reserved1     ;
    volatile Uint32      bufDspAddress ;
    volatile Uint32      bufDspPhysAddress ;
    volatile Uint32      bufGppPhysAddress ;
    volatile Uint32      size          ;
    volatile Uint16      dspMaduSize   ;
    volatile Uint16      reserved2     ;
    volatile Uint16      totalBuffers  ;
    volatile Uint16      freeBuffers   ;
    volatile Uint16      reserved3     ;
    volatile Uint16      reserved4     ;
             Uint16      padding [DMAPOOL_BUF_PADDING] ;
} DMAPOOL_BufObj ;

/** ============================================================================
 *  @name   DMAPOOL_Ctrl
 *
 *  @desc   This structure defines the control attributes required for both
 *          processors to work with buffer pools of buffers of a particular
 *          fixed size.
 *
 *  @field  numBufs
 *              Number of buffer pools to be configured in the SMAPOOL.
 *  @field  mpBufObj
 *              Array of buffer pools for various buffer sizes.
 *  @field  isGppInit
 *              Flag by which GPP will indicate the success of its
 *              initialization.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct DMAPOOL_Ctrl_tag {
    volatile Uint32  numBufs   ;
    volatile Uint16  isGppInit ;
             Uint16  padding [DMAPOOL_CTRL_PADDING] ;
    volatile DMAPOOL_BufObj  dmaBufObjs [MAX_DMABUFENTRIES] ;
} DMAPOOL_Ctrl ;

/** ============================================================================
 *  @name   DMAPOOL_Obj
 *
 *  @desc   This structure defines the object for SMAPOOL.
 *
 *  @field  mpcsObj
 *              MPCS object.
 *  @field  exactMatchReq
 *              Flag indicating whether requested size is to be rounded to
 *              nearest available size in Pools.
 *  @field  padding
 *              Padding for alignment.
 *  @field  ctrlPtr
 *              SMAPOOL control structure.
 *  ============================================================================
 */
typedef struct DMAPOOL_Obj_tag {
             MPCS_ShObj    mpcsObj  ;
    volatile Uint16        exactMatchReq ;
             Uint16        padding [DMAPOOL_OBJ_PADDING] ;
             DMAPOOL_Ctrl  ctrlPtr  ;
} DMAPOOL_Obj ;

/** ============================================================================
 *  @name   DMAPOOL_BufHeader
 *
 *  @desc   This structure defines the buffer header. This structure maintains
 *          information required to link the buffers within each buffer pool.
 *
 *  @field  next
 *              Pointer to next buffer header.
 *  @field  usedOnce
 *              Indicates if the buffer has been used at least once. When
 *              profiling is enabled, this field is used for getting statistics
 *              regarding the usage of the buffers within the buffer pools.
 *  ============================================================================
 */
typedef struct DMAPOOL_BufHeader_tag {
    volatile struct  DMAPOOL_BufHeader_tag * next ;
#if defined (DDSP_PROFILE)
             Uint16  usedOnce ;
#endif /* if defined (DDSP_PROFILE) */
} DMAPOOL_BufHeader ;


/** ============================================================================
 *  @func   DMAPOOL_open
 *
 *  @desc   This function opens creates the SMAPOOL object.
 *
 *  @arg    object
 *              Pointer to the SMAPOOL object to be created.
 *  @arg    params
 *              Allocator-specific setup parameters.
 *
 *  @ret    SYS_OK
 *              Object was created successfully.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    DMAPOOL_exit
 *  ============================================================================
 */
Int
DMAPOOL_open (Ptr * object, Ptr params) ;


/** ============================================================================
 *  @func   DMAPOOL_alloc
 *
 *  @desc   This function allocates a free buffer from the specified buffer pool
 *          and returns it to the user.
 *
 *  @arg    object
 *              Pointer to the SMAPOOL object.
 *  @arg    buffer
 *              Pointer to the buffer to be created.
 *  @arg    size
 *              Size of the buffer to be allocated.
 *
 *  @ret    SYS_OK
 *              Buffer created successfully.
 *
 *  @enter  size must be greater than zero.
 *          smaObj should be valid.
 *          buffer should be valid.
 *
 *  @leave  None
 *
 *  @see    DMAPOOL_free
 *  ============================================================================
 */
Int
DMAPOOL_alloc (Ptr object, Ptr * buffer, size_t size) ;


/** ============================================================================
 *  @func   DMAPOOL_free
 *
 *  @desc   This function frees the buffer specified by the user, and returns it
 *          to the buffer pool.
 *
 *  @arg    object
 *              Pointer to the SMAPOOL object.
 *  @arg    buffer
 *              Pointer to the buffer to be freed.
 *  @arg    size
 *              Size of the buffer to be freed.
 *
 *  @ret    None.
 *
 *  @enter  buffer must be valid.
 *          size must be greater than zero.
 *          smaObj should be valid.
 *
 *  @leave  None
 *
 *  @see    DMAPOOL_alloc
 *  ============================================================================
 */
Void
DMAPOOL_free (Ptr object, Ptr  buffer, size_t size) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (_DMA_POOL_H) */
