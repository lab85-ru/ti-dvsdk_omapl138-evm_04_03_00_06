/** ============================================================================
 *  @file   ldrv_pool.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/POOLS/
 *
 *  @desc   Defines the POOL Interface structure and associated function
 *          signature.
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


#if !defined (LDRV_POOL_H)
#define LDRV_POOL_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <_dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @name   LDRV_POOL_Config
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
typedef struct LDRV_POOL_Config_tag {
    Uint32            poolSize ;
    Uint32            ipsId ;
    Uint32            ipsEventNo ;
    Uint32            arg1 ;
    Uint32            arg2 ;
} LDRV_POOL_Config ;

/** ============================================================================
 *  @name   LDRV_POOL_Ctrl
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
typedef struct LDRV_POOL_Ctrl_tag {
    Uint32            dspAddr ;
    LDRV_POOL_Config  config ;
    Uint16            padding [LDRV_POOL_CTRL_PADDING] ;
} LDRV_POOL_Ctrl ;


/** ============================================================================
 *  @func   LDRV_POOL_moduleInit
 *
 *  @desc   This function initializes the LDRV_POOL module.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    LDRV_POOL_moduleExit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_POOL_moduleInit (Void) ;


/** ============================================================================
 *  @func   LDRV_POOL_moduleExit
 *
 *  @desc   This function finalizes the LDRV_POOL module.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    LDRV_POOL_moduleInit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_POOL_moduleExit (Void) ;


/** ============================================================================
 *  @name   LDRV_POOL_init
 *
 *  @desc   This function initializes the POOL component.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    shDspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the LDRV IPS component.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EINVALIDARG
 *              Invalid argument
 *          DSP_EMEMORY
 *              Out of memory.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          shDspAddr must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    LDRV_POOL_exit ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_init (IN ProcessorId procId, OUT Uint32 * shDspAddr) ;


/** ============================================================================
 *  @name   LDRV_POOL_exit
 *
 *  @desc   This function finalizes the POOL component.
 *
 *  @arg    procId
 *              Processor Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_POOL_init ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_exit (IN ProcessorId procId) ;


/** ============================================================================
 *  @name   LDRV_POOL_open
 *
 *  @desc   This function creates the allocator for given pool id.
 *
 *  @arg    poolId
 *              Pool ID.
 *  @arg    poolOpenParams
 *              Pointer to parameters for opening the pool.
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
 *  @enter  poolId must be a valid ID.
 *          poolOpenParams must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_POOL_close ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_open (IN PoolId poolId, IN POOL_OpenParams * poolOpenParams) ;


/** ============================================================================
 *  @name   LDRV_POOL_close
 *
 *  @desc   This function destroys the allocator for given poolId.
 *
 *  @arg    poolId
 *              Pool ID.
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
 *  @enter  poolId must be a valid ID.
 *
 *  @leave  None
 *
 *  @see    LDRV_POOL_open ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_close (IN PoolId poolId) ;


/** ============================================================================
 *  @name   LDRV_POOL_alloc
 *
 *  @desc   This function allocates a buffer from pool with given pool id, and
 *          returns the pointer to the user.
 *
 *  @arg    poolId
 *              Pool ID.
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
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  poolId must be a valid ID.
 *          bufPtr must be valid.
 *          size must be greater than zero.
 *
 *  @leave  None
 *
 *  @see    LDRV_POOL_close ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_alloc (IN  PoolId      poolId,
                 OUT Pvoid *     bufPtr,
                 IN  Uint32      size) ;


/** ============================================================================
 *  @name   LDRV_POOL_free
 *
 *  @desc   This function frees a buffer.
 *
 *  @arg    poolId
 *              Pool ID.
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
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  poolId must be a valid ID.
 *          buf must be valid.
 *          size must be greater than zero.
 *
 *  @leave  None
 *
 *  @see    LDRV_POOL_alloc ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_free (IN  PoolId   poolId,
                IN  Pvoid    buf,
                IN  Uint32   size) ;


/** ============================================================================
 *  @func   LDRV_POOL_writeback
 *
 *  @desc   This function read the content of GPP buffer into DSP buffer.
 *
 *  @arg    poolId
 *              Pool ID.
 *  @arg    buf
 *              Pointer to the buffer.
 *  @arg    size
 *              Size of the buffer to be allocated.
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
 *  @enter  size must be more than zero
 *          bufPtr must be valid.
 *          object should be valid.
 *
 *  @leave  buf must not be NULL, if NULL, then returned status should be
 *          DSP_EMEMORY
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_writeback (IN PoolId poolId, IN  Pvoid  buf, IN  Uint32 size) ;


/** ============================================================================
 *  @func   LDRV_POOL_invalidate
 *
 *  @desc   This function read the content of DSP buffer into GPP buffer.
 *
 *  @arg    poolId
 *              Pool ID.
 *  @arg    buf
 *              Pointer to the buffer.
 *  @arg    size
 *              Size of the buffer to be allocated.
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
 *  @enter  size must be more than zero
 *          bufPtr must be valid.
 *          object should be valid.
 *
 *  @leave  buf must not be NULL, if NULL, then returned status should be
 *          DSP_EMEMORY
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_invalidate (IN PoolId poolId, IN  Pvoid  buf, IN  Uint32 size) ;


/** ============================================================================
 *  @name   LDRV_POOL_xltBuf
 *
 *  @desc   This function translate buffers from different address spaces.
 *
 *  @arg    poolId
 *              Pool ID.
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
 *  @enter  poolId must be a valid ID.
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
LDRV_POOL_xltBuf (IN  PoolId           poolId,
                  IN  Pvoid            buf,
                  OUT Pvoid *          cBuf,
                  IN  POOL_AddrXltFlag xltFlag) ;


/** ============================================================================
 *  @func   LDRV_POOL_getPoolId
 *
 *  @desc   This function gets the pool id of the buffer that was allocated from
 *          the pool.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    addr
 *              Buffer address.
 *  @arg    addrType
 *              Address type i.e AddrType_Knl or AddrType_Dsp.
 *  @arg    poolId
 *              Pointer to the variable that will be updated with poolId if
 *              success.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ENOTFOUND
 *              PoolId corresponding to the buffer is not available.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_getPoolId (IN  ProcessorId procId,
                     IN  Pvoid       addr,
                     IN  AddrType    addrType,
                     OUT PoolId *    poolId) ;


/** ============================================================================
 *  @name   LDRV_POOL_reconfigure
 *
 *  @desc   This function reconfigures the plugged memory allocator.
 *
 *  @arg    poolId
 *              Pool ID.
 *  @arg    args
 *              Argument for reconfiguring the plugged memory allocator.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_ENOTIMPL
 *              Function not implemented by the pool.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  poolId must be a valid ID.
 *          args must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_reconfigure (IN PoolId poolId,
                       IN Pvoid  args) ;


#if defined (__cplusplus)
}
#endif


#endif /* !defined (LDRV_POOL_H) */
