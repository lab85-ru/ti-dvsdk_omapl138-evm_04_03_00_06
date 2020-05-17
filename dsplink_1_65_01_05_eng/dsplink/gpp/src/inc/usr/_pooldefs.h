/** ============================================================================
 *  @file   _pooldefs.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Definitions of constants and structures for pools.
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


#if !defined (_POOLDEFS_H)
#define _POOLDEFS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <pooldefs.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @const  POOL_INVALIDID
 *
 *  @desc   Reserved for denoting an invalid pool Id.
 *  ============================================================================
 */
#define POOL_INVALIDID         ((PoolId) MAX_POOLENTRIES)


/** ============================================================================
 *  @name   POOL_AddrXltFlag
 *
 *  @desc   This enum defines direction of buffer translation.
 *
 *  @field  USR_TO_KNL
 *              User to kernel address translation.
 *  @field  USR_TO_PHY
 *              User to physical address translation.
 *  @field  USR_TO_DSP
 *              User to DSP address translation.
 *  @field  PHY_TO_USR
 *              Physical to user address translation.
 *  @field  PHY_TO_KNL
 *              Physical to kernel address translation.
 *  @field  PHY_TO_DSP
 *              Physical to DSP address translation.
 *  @field  KNL_TO_USR
 *              Kernel to user address translation.
 *  @field  KNL_TO_PHY
 *              Kernel to physical address translation.
 *  @field  KNL_TO_DSP
 *              Kernel to DSP address translation.
 *  @field  DSP_TO_USR
 *              DSP to user address translation.
 *  @field  DSP_TO_PHY
 *              DSP to physical address translation.
 *  @field  DSP_TO_KNL
 *              DSP to kernel address translation.
 *  ============================================================================
 */
typedef enum  {
    USR_TO_KNL = (AddrType_Usr | (AddrType_Knl << 8u)),
    USR_TO_PHY = (AddrType_Usr | (AddrType_Phy << 8u)),
    USR_TO_DSP = (AddrType_Usr | (AddrType_Dsp << 8u)),
    PHY_TO_USR = (AddrType_Phy | (AddrType_Usr << 8u)),
    PHY_TO_KNL = (AddrType_Phy | (AddrType_Knl << 8u)),
    PHY_TO_DSP = (AddrType_Phy | (AddrType_Dsp << 8u)),
    KNL_TO_USR = (AddrType_Knl | (AddrType_Usr << 8u)),
    KNL_TO_PHY = (AddrType_Knl | (AddrType_Phy << 8u)),
    KNL_TO_DSP = (AddrType_Knl | (AddrType_Dsp << 8u)),
    DSP_TO_USR = (AddrType_Dsp | (AddrType_Usr << 8u)),
    DSP_TO_PHY = (AddrType_Dsp | (AddrType_Phy << 8u)),
    DSP_TO_KNL = (AddrType_Dsp | (AddrType_Knl << 8u))
} POOL_AddrXltFlag ;


/** ============================================================================
 *  @name   POOL_OpenParams
 *
 *  @desc   Forward declaration of POOL_OpenParams_tag
 *  ============================================================================
 */
typedef struct POOL_OpenParams_tag POOL_OpenParams ;


/** ============================================================================
 *  @name   FnPoolInit
 *
 *  @desc   Signature of function that initialize the plugged memory allocator.
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
 *  ============================================================================
 */
typedef DSP_STATUS (*FnPoolInit) (IN  ProcessorId dspId,
                                  IN  Uint32      poolId,
                                  OUT Uint32 *    shDspAddr,
                                  OUT Pvoid *     object) ;

/** ============================================================================
 *  @name   FnPoolExit
 *
 *  @desc   Signature of function that finalize the plugged memory allocator.
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
 *  ============================================================================
 */
typedef DSP_STATUS (*FnPoolExit) (IN  ProcessorId dspId,
                                  IN  Uint32      poolId,
                                  IN  Void *      object) ;

/** ============================================================================
 *  @name   FnPoolOpen
 *
 *  @desc   Signature of function that creates the plugged memory allocator.
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
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnPoolOpen) (IN  ProcessorId       dspId,
                                  IN  Uint32            poolId,
                                  IN  Void *            object,
                                  IN  POOL_OpenParams * poolOpenParams) ;

/** ============================================================================
 *  @name   FnPoolClose
 *
 *  @desc   Signature of function that destroys plugged memory allocator.
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
 *  ============================================================================
 */
typedef DSP_STATUS (*FnPoolClose) (IN  ProcessorId       dspId,
                                   IN  Uint32            poolId,
                                   IN  Void *            object) ;

/** ============================================================================
 *  @name   FnPoolAlloc
 *
 *  @desc   Signature of function that allocates a buffer, and returns the
 *          pointer to the user.
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
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnPoolAlloc) (IN  ProcessorId       dspId,
                                   IN  Uint32            poolId,
                                   IN  Void *            object,
                                   OUT Pvoid *           bufPtr,
                                   IN  Uint32            size) ;

/** ============================================================================
 *  @name   FnPoolFree
 *
 *  @desc   Signature of function that frees a buffer.
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
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnPoolFree) (IN  ProcessorId       dspId,
                                  IN  Uint32            poolId,
                                  IN  Void *            object,
                                  IN  Void *            buf,
                                  IN  Uint32            size) ;

/** ============================================================================
 *  @name   FnPoolWriteback
 *
 *  @desc   Signature of function that flushes a buffer's content.
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
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnPoolWriteback) (IN  ProcessorId       dspId,
                                       IN  Uint32            poolId,
                                       IN  Void *            object,
                                       IN  Void *            buf,
                                       IN  Uint32            size) ;

/** ============================================================================
 *  @name   FnPoolInvalidate
 *
 *  @desc   Signature of function that invalidates a buffer's content.
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
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnPoolInvalidate) (IN  ProcessorId       dspId,
                                        IN  Uint32            poolId,
                                        IN  Void *            object,
                                        IN  Void *            buf,
                                        IN  Uint32            size) ;

/** ============================================================================
 *  @name   FnPoolXltBuf
 *
 *  @desc   Signature of function that translates buffer.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    poolId
 *              Pool Identifier.
 *  @arg    object
 *              Pointer to the pool-specific object.
 *  @arg    buf
 *              Pointer to the buffer to be translated.
 *  @arg    cBuf
 *              Converted address.
 *  @arg    type
 *              Direction of translation.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnPoolXltBuf) (IN  ProcessorId       dspId,
                                    IN  Uint32            poolId,
                                    IN  Void *            object,
                                    IN  Void *            buf,
                                    OUT Pvoid *           cBuf,
                                    IN  POOL_AddrXltFlag  xltFlag) ;

/** ============================================================================
 *  @name   FnPoolReconfigure
 *
 *  @desc   Signature of function that reconfigure the plugged memory allocator.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    poolId
 *              Pool Identifier.
 *  @arg    object
 *              Pointer to the pool-specific object.
 *  @arg    args
 *              Argument for reconfiguring the plugged memory allocator.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnPoolReconfigure) (IN  ProcessorId       dspId,
                                         IN  Uint32            poolId,
                                         IN  Void *            object,
                                         IN  Pvoid             args) ;


/** ============================================================================
 *  @name   POOL_Interface
 *
 *  @desc   Interface functions exported by the POOL subcomponent.
 *
 *  @field  open
 *              Function pointer to the plugged pool's open function.
 *  @field  close
 *              Function pointer to the plugged pool's close function.
 *  @field  alloc
 *              Function pointer to the plugged pool's alloc function.
 *  @field  free
 *              Function pointer to the plugged pool's free function.
 *  @field  reconfigure
 *              Function pointer to the plugged pool's reconfigure function.
 *  @field  writeback
 *              Function pointer to the plugged pool's writeback function.
 *  @field  invalidate
 *              Function pointer to the plugged pool's invalidate function.
 *  @field  xltBuf
 *              Function pointer to the plugged pool's xltBuf function.
 *  ============================================================================
 */
typedef struct POOL_Interface_tag {
    FnPoolOpen        open        ;
    FnPoolClose       close       ;
    FnPoolAlloc       alloc       ;
    FnPoolFree        free        ;
    FnPoolReconfigure reconfigure ;
    FnPoolWriteback   writeback   ;
    FnPoolInvalidate  invalidate  ;
    FnPoolXltBuf      xltBuf      ;
} POOL_Interface ;


/** ============================================================================
 *  @name   POOL_OpenParams_tag
 *
 *  @desc   This structure defines the pool open specific parameters.
 *
 *  @field  params
 *              Pointer to the user provided parameters.
 *  @field  physAddr
 *              Physical address of memory block.
 *  @field  virtAddr
 *              Address of memory block in kernel virtual address space.
 *  @field  dspAddr
 *              Address of memory block in DSP address space (If the pool is in
 *              shared memory).
 *  @field  size
 *              Size of memory block in bytes.
 *  ============================================================================
 */
struct POOL_OpenParams_tag {
    Pvoid   params ;
    Uint32  physAddr ;
    Uint32  virtAddr ;
    Uint32  dspAddr ;
    Uint32  size ;
} ;


#if defined (__cplusplus)
}
#endif


#endif /* !defined (_POOLDEFS_H) */
