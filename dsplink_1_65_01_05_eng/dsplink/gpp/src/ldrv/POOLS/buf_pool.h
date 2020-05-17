/** ============================================================================
 *  @file   buf_pool.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/POOLS/
 *
 *  @desc   Defines the BUF pool interface.
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


#if !defined (BUF_POOL_H)
#define BUF_POOL_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <pooldefs.h>
#include <mpbuf.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   BUFPOOL_Obj
 *
 *  @desc   Buffer object for the pool, representing a BUF for a specific size.
 *
 *  @field  bufSize
 *              Size of the buffers in the buffer pool.
 *  @field  bufList
 *              List of buffers in the buffer pool.
 *  ============================================================================
 */
typedef struct BUFPOOL_Obj_tag {
    Uint32        bufSize ;
    MpBufHandle   bufList ;
} BUFPOOL_Obj ;


/** ============================================================================
 *  @name   BUFPOOL_State
 *
 *  @desc   BUFPOOL state object. One instance of this object is created each
 *          time BUFPOOL_open () is called.
 *
 *  @field  numBufPools
 *              Number of buffer objects configured in the pool.
 *  @field  bufObjs
 *              Array of buffer objects for various buffer sizes. The array is
 *              dynamically allocated of size equal to the one specified by the
 *              user.
 *  @field  phyAddr
 *              Physical address of the buffer pool allocated.
 *  @field  virtAddr
 *              Virtual address of the buffer pool allocated.
 *  @field  size
 *              Total size of memory allocated for all buffer pools.
 *  @field  refCount
 *              Reference count for the pool.
 *  ============================================================================
 */
typedef struct BUFPOOL_State_tag {
    Uint32          numBufPools ;
    BUFPOOL_Obj *   bufObjs     ;
    Uint32          phyAddr     ;
    Uint32          virtAddr    ;
    Uint32          size        ;
    Uint32          refCount    ;
} BUFPOOL_State ;


/** ============================================================================
 *  @func   BUFPOOL_init
 *
 *  @desc   Performs global initialization of the buffer pool.
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
 *  @see    BUFPOOL_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
BUFPOOL_init (IN  ProcessorId dspId,
              IN  Uint32      poolId,
              OUT Uint32 *    shDspAddr,
              OUT Pvoid *     object) ;


/** ============================================================================
 *  @func   BUFPOOL_exit
 *
 *  @desc   Performs global finalization of the buffer pool.
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
 *  @see    BUFPOOL_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
BUFPOOL_exit (IN  ProcessorId dspId,
              IN  Uint32      poolId,
              IN  Void *      object) ;


/** ============================================================================
 *  @func   BUFPOOL_open
 *
 *  @desc   Opens the buffer pool and configures it according to the user
 *          attributes.
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
 *              This component has been successfully opened.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          poolId must be valid.
 *          object must be valid.
 *          poolOpenParams must be valid.
 *
 *  @leave  None
 *
 *  @see    BUFPOOL_Attrs, BUFPOOL_State, BUFPOOL_close
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
BUFPOOL_open (IN  ProcessorId       dspId,
              IN  Uint32            poolId,
              IN  Void *            object,
              IN  POOL_OpenParams * poolOpenParams) ;


/** ============================================================================
 *  @func   BUFPOOL_close
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
 *  @see    BUFPOOL_State, BUFPOOL_open, BUF_Delete
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
BUFPOOL_close (IN  ProcessorId       dspId,
               IN  Uint32            poolId,
               IN  Void *            object) ;


/** ============================================================================
 *  @func   BUFPOOL_alloc
 *
 *  @desc   This function allocates a buffer of the specified size.
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
 *  @see    BUFPOOL_State, BUFPOOL_free, BUF_Alloc
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
BUFPOOL_alloc (IN  ProcessorId       dspId,
               IN  Uint32            poolId,
               IN  Void *            object,
               OUT Pvoid *           bufPtr,
               IN  Uint32            size) ;


/** ============================================================================
 *  @func   BUFPOOL_free
 *
 *  @desc   This function frees the buffer given by the user of the specified
 *          size.
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
 *  @see    BUFPOOL_State, BUFPOOL_alloc , BUF_Free
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
BUFPOOL_free (IN  ProcessorId       dspId,
              IN  Uint32            poolId,
              IN  Void *            object,
              IN  Void *            buf,
              IN  Uint32            size) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (BUF_POOL_H) */
