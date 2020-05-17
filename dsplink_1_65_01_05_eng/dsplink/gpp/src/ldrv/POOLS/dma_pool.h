/** ============================================================================
 *  @file   dma_pool.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/POOLS/
 *
 *  @desc   Defines the interfaces and data structures for the DMA POOL
 *          sub-component.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation version 2.
 *  
 *  This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 *  whether express or implied; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *  ============================================================================
 */


#if !defined (DMAPOOL_H)
#define DMAPOOL_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <mpcs.h>
#include <_pooldefs.h>

/*  ----------------------------------- Profiling                   */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */



#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  DMAPOOL_CTRL_PADLENGTH
 *
 *  @desc   Padding required for DSP L2 cache line alignment for DMAPOOL control
 *          structure.
 *          This value is calculated based on the sum of the sizes of the fields
 *          within the control structure.
 *  ============================================================================
 */
#define DMAPOOL_CTRL_PADLENGTH    ((  CACHE_L2_LINESIZE                        \
                                    - (  sizeof (Uint16)                       \
                                       + sizeof (Uint32))) / 2)

/** ============================================================================
 *  @const  DMAPOOL_OBJ_PADLENGTH
 *
 *  @desc   Padding required for L2 cache line alignment within the DMAPOOL
 *          shared memory object.
 *          This value is calculated based on the sum of the sizes of the fields
 *          within the structure.
 *  ============================================================================
 */
#define DMAPOOL_OBJ_PADLENGTH     ((CACHE_L2_LINESIZE - sizeof (Uint16)) / 2)


/** ============================================================================
 *  @const  MIN_DMABUF_SIZE
 *
 *  @desc   This constant defines the minimum size of the buffer to be
 *          allocated.
 *  ============================================================================
 */
#define MIN_DMABUF_SIZE (sizeof (DmaBufHeader))

/** ============================================================================
 *  @const  DMABUF_BUF_PADLENGTH
 *
 *  @desc   Padding required for DSP L2 cache line alignment for DMABUF object.
 *          This value is calculated based on the sum of the sizes of the fields
 *          within the structure.
 *  ============================================================================
 */
#define DMABUF_BUF_PADLENGTH     ((  CACHE_L2_LINESIZE                         \
                                    - (  (6 * sizeof (Uint16))                 \
                                       + (6 * sizeof (Uint32)))) / 2)

#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @name   DMABUF_FREESTAMP
 *
 *  @desc   Value to indicate that the buffer if free.
 *  ============================================================================
 */
#define DMABUF_FREESTAMP        0xBEEF

/** ============================================================================
 *  @name   DMABUF_ALLOCSTAMP
 *
 *  @desc   Value to indicate that the buffer is allocated.
 *  ============================================================================
 */
#define DMABUF_ALLOCSTAMP        0xCAFE
#endif /* if defined (DDSP_PROFILE) */


/** ============================================================================
 *  @name   DmaBufObj
 *
 *  @desc   This structure defines the buffer pool object. It maintains the pool
 *          of buffers of a particular fixed size.
 *
 *  @field  nextFree
 *              Pointer to next free buffer.
 *  @field  startAddress
 *              The start address of buffer pool in GPP address space.
 *  @field  bufDspAddress
 *              The virtual address of buffer pool in DSP address space.
 *  @field  bufDspPhysAddress
 *              The physical address of buffer pool in DSP address space.
 *  @field  bufGppPhysAddress
 *              The physical address of buffer pool in GPP address space.
 *  @field  size
 *              Size of the buffers in this pool.
 *  @field  dspMaduSize
 *              Size of DSP's elementary addressable unit.
 *  @field  wordSwap
 *              Indicates whether word-swap is enabled for the DSP MMU.
 *  @field  totalBuffers
 *              Total number of buffers in pool.
 *  @field  freeBuffers
 *              Number of free buffers in pool.
 *  @field  freePool
 *              Indicates whether the buffer pool was allocated within the MpBUF
 *              component, and should be freed during DMABUF_Delete ()
 *  @field  freeHandle
 *              Indicates whether the DmaBufhandle was allocated within the
 *              DMABUF component, and should be freed during  DMABUF_Delete ()
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct DmaBufObj_tag {
    volatile Uint32        nextFree      ;
    volatile Uint32        startAddress  ;
    volatile Uint32        bufDspAddress ;
    volatile Uint32        bufDspPhysAddress ;
    volatile Uint32        bufGppPhysAddress ;
    volatile Uint32        size          ;
    volatile Uint16        dspMaduSize   ;
    volatile Uint16        wordSwap      ;
    volatile Uint16        totalBuffers  ;
    volatile Uint16        freeBuffers   ;
    volatile Uint16        freePool      ;
    volatile Uint16        freeHandle    ;
             Uint16        padding [DMABUF_BUF_PADLENGTH] ;
} DmaBufObj ;

/** ============================================================================
 *  @name   DmaBufHandle
 *
 *  @desc   This is the Handle to buffer pool objects.
 *  ============================================================================
 */
typedef  DmaBufObj * DmaBufHandle  ;


/** ============================================================================
 *  @name   DmaBufHeader
 *
 *  @desc   Forward declaration for DmaBufHeader_tag.
 *  ============================================================================
 */
typedef struct DmaBufHeader_tag DmaBufHeader ;

/** ============================================================================
 *  @name   DmaBufHeader_tag
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
struct DmaBufHeader_tag {
    volatile DmaBufHeader *   next ;
#if defined (DDSP_PROFILE)
             Uint16          usedOnce ;
#endif /* if defined (DDSP_PROFILE) */
} ;


/** ============================================================================
 *  @name   DMAPOOL_Ctrl
 *
 *  @desc   This structure defines the control attributes required for both
 *          processors to work with buffer pools of buffers of a particular
 *          fixed size. This structure is shared between GPP and DSP.
 *
 *  @field  numBufs
 *              Number of buffer pools to be configured in the DMA.
 *  @field  isGppInit
 *              Flag by which GPP will indicate the success of its
 *              initialization.
 *  @field  padding
 *              Padding for alignment.
 *  @field  dmaBufObjs
 *              Array for DmaBuf Objects.
 *  ============================================================================
 */
typedef struct DMAPOOL_Ctrl_tag {
    volatile Uint32       numBufs   ;
    volatile Uint16       isGppInit ;
    volatile Uint16       padding [DMAPOOL_CTRL_PADLENGTH] ;
    volatile DmaBufObj    dmaBufObjs [MAX_DMABUFENTRIES]  ;
} DMAPOOL_Ctrl ;


/** ============================================================================
 *  @name   DMAPOOL_ShmObj
 *
 *  @desc   This structure defines the object for DMAPOOL in shared memory.
 *
 *  @field  mpcsObj
 *              MPCS object.
 *  @field  exactMatchReq
 *              Flag indicating whether requested size is to be rounded to
 *              nearest available size in Pools.
 *  @field  padding
 *              Padding for alignment.
 *  @field  ctrlPtr
 *              DMAPOOL control structure.
 *  ============================================================================
 */
typedef struct DMAPOOL_ShmObj_tag {
             MPCS_ShObj      mpcsObj       ;
    volatile Uint16          exactMatchReq ;
             Uint16          padding [DMAPOOL_OBJ_PADLENGTH] ;
             DMAPOOL_Ctrl    ctrlPtr  ;
} DMAPOOL_ShmObj ;


/** ============================================================================
 *  @func   DMAPOOL_init
 *
 *  @desc   This function initializes the DMA POOL component.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    poolId
 *              Pool Identifier.
 *  @arg    shDspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the POOL component.
 *  @arg    object
 *              Location to receive the pool-specific object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          poolId must be valid.
 *          shDspAddr must be a valid pointer.
 *          object must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    DMAPOOL_exit ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_init (IN  ProcessorId dspId,
              IN  Uint32      poolId,
              OUT Uint32 *    shDspAddr,
              OUT Pvoid *     object) ;


/** ============================================================================
 *  @func   DMAPOOL_exit
 *
 *  @desc   This function finalizes the DMA POOL component.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    poolId
 *              Pool Identifier.
 *  @arg    object
 *              Pointer to the pool-specific object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          poolId must be valid.
 *
 *  @leave  None
 *
 *  @see    DMAPOOL_init ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_exit (IN  ProcessorId dspId,
              IN  Uint32      poolId,
              IN  Void *      object) ;


/** ============================================================================
 *  @func   DMAPOOL_open
 *
 *  @desc   This function opens the buffer pool and configures it according to
 *          the user attributes.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    poolId
 *              Pool Identifier.
 *  @arg    object
 *              Pointer to the pool-specific object.
 *  @arg    poolOpenParams
 *              An argument for creating POOL and plugged memory allocator.
 *
 *  @ret    DSP_SOK
 *              The buffer pool has been successfully created.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_ERANGE
 *              The size of memory required is outside configured range.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          poolId must be valid.
 *          object must be valid.
 *          poolOpenParams must be valid.
 *
 *  @see    DMAPOOL_Attrs, DMAPOOL_close ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_open (IN  ProcessorId       dspId,
              IN  Uint32            poolId,
              IN  Void *            object,
              IN  POOL_OpenParams * poolOpenParams) ;


/** ============================================================================
 *  @func   DMAPOOL_close
 *
 *  @desc   This function closes the pool and cleans up its state object.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    poolId
 *              Pool Identifier.
 *  @arg    object
 *              Pointer to the pool-specific object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          poolId must be valid.
 *          object must be valid.
 *
 *  @leave  None
 *
 *  @see    DMAPOOL_open ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_close (IN  ProcessorId       dspId,
               IN  Uint32            poolId,
               IN  Void *            object) ;


/** ============================================================================
 *  @func   DMAPOOL_alloc
 *
 *  @desc   This function allocates a free buffer from the specified buffer pool
 *          and returns it to the user.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    poolId
 *              Pool Identifier.
 *  @arg    object
 *              Pointer to the pool-specific object.
 *  @arg    bufPtr
 *              Location to receive the allocated buffer.
 *  @arg    size
 *              Size of the buffer to be allocated.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EINVALIDARG
 *              Invalid buffer size requested by user.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          poolId must be valid.
 *          object must be valid.
 *          bufPtr must be a valid pointer.
 *          size must not be zero.
 *
 *  @leave  On success, the bufPtr return parameter contents must be non-NULL.
 *          On failure, the bufPtr return parameter contents must be NULL.
 *
 *  @see    DMAPOOL_free ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_alloc (IN  ProcessorId       dspId,
               IN  Uint32            poolId,
               IN  Void *            object,
               OUT Pvoid *           bufPtr,
               IN  Uint32            size) ;


/** ============================================================================
 *  @func   DMAPOOL_free
 *
 *  @desc   This function frees the buffer specified by the user, and returns it
 *          to the buffer pool.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    poolId
 *              Pool Identifier.
 *  @arg    object
 *              Pointer to the pool-specific object.
 *  @arg    buf
 *              Pointer to the buffer to be freed.
 *  @arg    size
 *              Size of the buffer to be freed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EINVALIDARG
 *              Invalid buffer size requested by user.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          poolId must be valid.
 *          object must be valid.
 *          buf must be valid.
 *          size must not be zero.
 *
 *  @leave  None
 *
 *  @see    DMAPOOL_alloc ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_free (IN  ProcessorId       dspId,
              IN  Uint32            poolId,
              IN  Void *            object,
              IN  Void *            buf,
              IN  Uint32            size) ;


/** ============================================================================
 *  @func   DMAPOOL_writeback
 *
 *  @desc   This function writes the content of GPP buffer into DSP buffer (with
 *          offset in sync).
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    poolId
 *              Pool Identifier.
 *  @arg    object
 *              Pointer to the pool-specific object.
 *  @arg    buf
 *              Pointer to the buffer.
 *  @arg    size
 *              Size of the buffer to be written back.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          poolId must be valid.
 *          object must be valid.
 *          buf must be valid.
 *          size must not be zero.
 *
 *  @leave  None
 *
 *  @see    DMAPOOL_invalidate ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_writeback (IN  ProcessorId       dspId,
                   IN  Uint32            poolId,
                   IN  Pvoid             object,
                   IN  Pvoid             buf,
                   IN  Uint32            size) ;


/** ============================================================================
 *  @func   DMAPOOL_invalidate
 *
 *  @desc   This function reads the content of DSP buffer into GPP buffer (with
 *          offset in sync).
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    poolId
 *              Pool Identifier.
 *  @arg    object
 *              Pointer to the pool-specific object.
 *  @arg    buf
 *              Pointer to the buffer.
 *  @arg    size
 *              Size of the buffer to be invalidated.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          poolId must be valid.
 *          object must be valid.
 *          buf must be valid.
 *          size must not be zero.
 *
 *  @leave  None
 *
 *  @see    DMAPOOL_writeback ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_invalidate (IN  ProcessorId       dspId,
                    IN  Uint32            poolId,
                    IN  Pvoid             object,
                    IN  Pvoid             buf,
                    IN  Uint32            size) ;


/** ============================================================================
 *  @func   DMAPOOL_xltBuf
 *
 *  @desc   This function translates buffers across different address spaces.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    poolId
 *              Pool Identifier.
 *  @arg    object
 *              Pointer to the pool-specific object.
 *  @arg    buf
 *              Pointer to the buffer.
 *  @arg    cBuf
 *              Pointer to the converted buffer.
 *  @arg    xltFlag
 *              Direction of translation.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          poolId must be valid.
 *          object must be valid.
 *          buf must be valid.
 *          cBuf must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_xltBuf (IN  ProcessorId      dspId,
                IN  Uint32           poolId,
                IN  Void *           object,
                IN  Pvoid            buf,
                OUT Pvoid *          cBuf,
                IN  POOL_AddrXltFlag xltFlag) ;


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   DMAPOOL_instrument
 *
 *  @desc   This function gets the instrumentation data for a buffer pool.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    poolId
 *              Pool Identifier.
 *  @arg    object
 *              Pointer to the pool-specific object.
 *  @arg    params
 *              Container to fetch the instrument value.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *
 *  @enter  dspId must be valid.
 *          poolId must be valid.
 *          object must be valid.
 *          params must be valid.
 *
 *  @leave  None
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_instrument (IN  ProcessorId     dspId,
                    IN  Uint32          poolId,
                    IN  Void *          object,
                    IN  Pvoid           params) ;
#endif /* defined (DDSP_PROFILE) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (DMAPOOL_H) */
