/** ============================================================================
 *  @file   sma_pool.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/POOLS/
 *
 *  @desc   Defines the interfaces and data structures for the SMA POOL
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


#if !defined (SMAPOOL_H)
#define SMAPOOL_H


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
 *  @const  SMAPOOL_CTRL_PADLENGTH
 *
 *  @desc   Padding required for DSP L2 cache line alignment for SMAPOOL control
 *          structure.
 *          This value is calculated based on the sum of the sizes of the fields
 *          within the control structure.
 *  ============================================================================
 */
#define SMAPOOL_CTRL_PADLENGTH    ((  CACHE_L2_LINESIZE                        \
                                    - (  sizeof (Uint16)                       \
                                       + sizeof (Uint32))) / 2)

/** ============================================================================
 *  @const  SMAPOOL_OBJ_PADLENGTH
 *
 *  @desc   Padding required for L2 cache line alignment within the SMAPOOL
 *          shared memory object.
 *          This value is calculated based on the sum of the sizes of the fields
 *          within the structure.
 *  ============================================================================
 */
#define SMAPOOL_OBJ_PADLENGTH     ((CACHE_L2_LINESIZE - sizeof (Uint16)) / 2)


/** ============================================================================
 *  @const  MIN_SMABUF_SIZE
 *
 *  @desc   This constant defines the minimum size of the buffer to be
 *          allocated.
 *  ============================================================================
 */
#define MIN_SMABUF_SIZE (sizeof (SmaBufHeader))

/** ============================================================================
 *  @const  SMABUF_BUF_PADLENGTH
 *
 *  @desc   Padding required for DSP L2 cache line alignment for SMABUF object.
 *          This value is calculated based on the sum of the sizes of the fields
 *          within the structure.
 *  ============================================================================
 */
#define SMABUF_BUF_PADLENGTH     ((  CACHE_L2_LINESIZE                         \
                                    - (  (6 * sizeof (Uint16))                 \
                                       + (4 * sizeof (Uint32)))) / 2)

#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @name   SMABUF_FREESTAMP
 *
 *  @desc   Value to indicate that the buffer if free.
 *  ============================================================================
 */
#define SMABUF_FREESTAMP        0xBEEF

/** ============================================================================
 *  @name   SMABUF_ALLOCSTAMP
 *
 *  @desc   Value to indicate that the buffer is allocated.
 *  ============================================================================
 */
#define SMABUF_ALLOCSTAMP        0xCAFE
#endif /* if defined (DDSP_PROFILE) */


/** ============================================================================
 *  @name   SmaBufObj
 *
 *  @desc   This structure defines the buffer pool object. It maintains the pool
 *          of buffers of a particular fixed size.
 *
 *  @field  nextFree
 *              Pointer to next free buffer.
 *  @field  startAddress
 *              The start address of buffer pool in GPP address space.
 *  @field  bufDspAddress
 *              The start address of buffer pool in DSP address space.
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
 *              component, and should be freed during SMABUF_Delete ()
 *  @field  freeHandle
 *              Indicates whether the SmaBufhandle was allocated within the
 *              SMABUF component, and should be freed during  SMABUF_Delete ()
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct SmaBufObj_tag {
    volatile Uint32        nextFree      ;
    volatile Uint32        startAddress  ;
    volatile Uint32        bufDspAddress ;
    volatile Uint32        size          ;
    volatile Uint16        dspMaduSize   ;
    volatile Uint16        wordSwap      ;
    volatile Uint16        totalBuffers  ;
    volatile Uint16        freeBuffers   ;
    volatile Uint16        freePool      ;
    volatile Uint16        freeHandle    ;
             Uint16        padding [SMABUF_BUF_PADLENGTH] ;
} SmaBufObj ;

/** ============================================================================
 *  @name   SmaBufHandle
 *
 *  @desc   This is the Handle to buffer pool objects.
 *  ============================================================================
 */
typedef  SmaBufObj * SmaBufHandle  ;


/** ============================================================================
 *  @name   SmaBufHeader
 *
 *  @desc   Forward declaration for SmaBufHeader_tag.
 *  ============================================================================
 */
typedef struct SmaBufHeader_tag SmaBufHeader ;

/** ============================================================================
 *  @name   SmaBufHeader_tag
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
struct SmaBufHeader_tag {
    volatile SmaBufHeader *   next ;
#if defined (DDSP_PROFILE)
             Uint16          usedOnce ;
#endif /* if defined (DDSP_PROFILE) */
} ;


/** ============================================================================
 *  @name   SMAPOOL_Ctrl
 *
 *  @desc   This structure defines the control attributes required for both
 *          processors to work with buffer pools of buffers of a particular
 *          fixed size. This structure is shared between GPP and DSP.
 *
 *  @field  numBufs
 *              Number of buffer pools to be configured in the SMA.
 *  @field  isGppInit
 *              Flag by which GPP will indicate the success of its
 *              initialization.
 *  @field  padding
 *              Padding for alignment.
 *  @field  smaBufObjs
 *              Array for SmaBuf Objects.
 *  ============================================================================
 */
typedef struct SMAPOOL_Ctrl_tag {
    volatile Uint32       numBufs   ;
    volatile Uint16       isGppInit ;
    volatile Uint16       padding [SMAPOOL_CTRL_PADLENGTH] ;
    volatile SmaBufObj    smaBufObjs [MAX_SMABUFENTRIES]  ;
} SMAPOOL_Ctrl ;


/** ============================================================================
 *  @name   SMAPOOL_ShmObj
 *
 *  @desc   This structure defines the object for SMAPOOL in shared memory.
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
typedef struct SMAPOOL_ShmObj_tag {
             MPCS_ShObj      mpcsObj       ;
    volatile Uint16          exactMatchReq ;
             Uint16          padding [SMAPOOL_OBJ_PADLENGTH] ;
             SMAPOOL_Ctrl    ctrlPtr  ;
} SMAPOOL_ShmObj ;


/** ============================================================================
 *  @func   SMAPOOL_init
 *
 *  @desc   This function initializes the SMA POOL component.
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
 *  @see    SMAPOOL_exit ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SMAPOOL_init (IN  ProcessorId dspId,
              IN  Uint32      poolId,
              OUT Uint32 *    shDspAddr,
              OUT Pvoid *     object) ;


/** ============================================================================
 *  @func   SMAPOOL_exit
 *
 *  @desc   This function finalizes the SMA POOL component.
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
 *  @see    SMAPOOL_init ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SMAPOOL_exit (IN  ProcessorId dspId,
              IN  Uint32      poolId,
              IN  Void *      object) ;


/** ============================================================================
 *  @func   SMAPOOL_open
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
 *  @see    SMAPOOL_Attrs, SMAPOOL_close ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SMAPOOL_open (IN  ProcessorId       dspId,
              IN  Uint32            poolId,
              IN  Void *            object,
              IN  POOL_OpenParams * poolOpenParams) ;


/** ============================================================================
 *  @func   SMAPOOL_close
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
 *  @see    SMAPOOL_open ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SMAPOOL_close (IN  ProcessorId       dspId,
               IN  Uint32            poolId,
               IN  Void *            object) ;


/** ============================================================================
 *  @func   SMAPOOL_alloc
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
 *  @see    SMAPOOL_free ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SMAPOOL_alloc (IN  ProcessorId       dspId,
               IN  Uint32            poolId,
               IN  Void *            object,
               OUT Pvoid *           bufPtr,
               IN  Uint32            size) ;


/** ============================================================================
 *  @func   SMAPOOL_free
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
 *  @see    SMAPOOL_alloc ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SMAPOOL_free (IN  ProcessorId       dspId,
              IN  Uint32            poolId,
              IN  Void *            object,
              IN  Void *            buf,
              IN  Uint32            size) ;


/** ============================================================================
 *  @func   SMAPOOL_xltBuf
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
SMAPOOL_xltBuf (IN  ProcessorId      dspId,
                IN  Uint32           poolId,
                IN  Void *           object,
                IN  Pvoid            buf,
                OUT Pvoid *          cBuf,
                IN  POOL_AddrXltFlag xltFlag) ;


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   SMAPOOL_instrument
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
SMAPOOL_instrument (IN  ProcessorId     dspId,
                    IN  Uint32          poolId,
                    IN  Void *          object,
                    IN  Pvoid           params) ;
#endif /* defined (DDSP_PROFILE) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (SMAPOOL_H) */
