/** ============================================================================
 *  @file   dsplink.h
 *
 *  @path   $(DSPLINK)/dsp/inc/
 *
 *  @desc   Defines constants, data types and structures used by
 *          DSP/BIOS(TM) LINK.
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


#if !defined (DSPLINK_)
#define DSPLINK_


/*  ----------------------------------- DSP/BIOS Headers            */
#include <que.h>
#include <pool.h>
#if defined (MSGQ_COMPONENT)
#include <msgq.h>
#endif /* #if defined (MSGQ_COMPONENT) */

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <platform.h>
#include <version.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  IN/OUT/CONST
 *
 *  @desc   Argument specification syntax
 *  ============================================================================
 */
#define IN                              /* The argument is INPUT  only */
#define OUT                             /* The argument is OUTPUT only */
#define CONST   const

/** ============================================================================
 *  @const  DSP_MAX_STRLEN
 *
 *  @desc   Maximum length of string.
 *  ============================================================================
 */
#define DSP_MAX_STRLEN        32


/*  ============================================================================
 *  @const  MAX_IPS
 *
 *  @desc   Maximum number of IPS objects supported for each DSP.
 *  ============================================================================
 */
#define  MAX_IPS              16

#if defined (POOL_COMPONENT)
/** ============================================================================
 *  @name   MAX_POOLENTRIES
 *
 *  @desc   The maximum number of pools that can be configured.
 *  ============================================================================
 */
#define MAX_POOLENTRIES       32
#endif /* if defined (POOL_COMPONENT) */

/** ============================================================================
 *  @const  DSPLINK_DRIVER_NAME
 *
 *  @desc   DSPLINK driver name
 *  ============================================================================
 */
#define DSPLINK_DRIVER_NAME   "/dsplink"

/** ============================================================================
 *  @const  MAX_DRIVER_NAME_LEN
 *
 *  @desc   Maximum length of the driver name.
 *  ============================================================================
 */
#define MAX_DRIVER_NAME_LEN    16


#if defined (POOL_COMPONENT)
#if !defined (POOL_open) /* To ensure no clash with future BIOS definition */
/** ============================================================================
 *  @name   POOL_open
 *
 *  @desc   Function to set allocator pool id in POOL as well as call init and
 *          open function.
 *
 *  @arg    poolId
 *              Pool id.
 *  @arg    poolObj
 *              Allocator object of type POOL_Obj.
 *  @ret    SYS_OK
 *              Operation completed successfully.
 *          SYS_EINVAL
 *              POOL is not defined.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    POOL_close
 *  ============================================================================
 */
Int
POOL_open(Uint16     poolId,
          POOL_Obj * poolObj) ;
#endif /* if !defined (POOL_open) */


#if !defined (POOL_close) /* To ensure no clash with future BIOS definition */
/** ============================================================================
 *  @name   POOL_close
 *
 *  @desc   Macro over the allocator for closing a pool.
 *
 *  @arg    poolId
 *              Pool id.
 *
 *  @see    POOL_open
 *  ============================================================================
 */
#define POOL_close(poolId)                           \
        (((POOL->allocators [poolId]).fxns->close)       \
                (POOL->allocators [poolId].object))
#endif /* if !defined (POOL_close) */


#if !defined (POOL_alloc) /* To ensure no clash with future BIOS definition */
/** ============================================================================
 *  @name   POOL_alloc
 *
 *  @desc   Macro over the allocator for allocating a buffer.
 *
 *  @arg    poolId
 *              Pool id.
 *  @arg    bufPtr
 *              Placeholder to return the allocator buffer.
 *  @arg    size
 *              Size of buffer to be allocated.
 *
 *  @see    POOL_free
 *  ============================================================================
 */
#define POOL_alloc(poolId, bufPtr, size)                        \
        (((POOL->allocators [poolId]).fxns->alloc)              \
                (POOL->allocators [poolId].object,              \
                 bufPtr,                                        \
                 size))
#endif /* if !defined (POOL_alloc) */


#if !defined (POOL_free) /* To ensure no clash with future BIOS definition */
/** ============================================================================
 *  @name   POOL_free
 *
 *  @desc   Macro over the allocator for freeing a buffer.
 *
 *  @arg    poolId
 *              Pool id.
 *  @arg    buf
 *              Pointer to the buffer to be freed.
 *  @arg    size
 *              Size of buffer to be freed.
 *
 *  @see    POOL_alloc
 *  ============================================================================
 */
#define POOL_free(poolId, buf, size)                            \
        (((POOL->allocators [poolId]).fxns->free)               \
                (POOL->allocators [poolId].object,              \
                 buf,                                           \
                 size))
#endif /* if !defined (POOL_free) */
#endif /* if defined (POOL_COMPONENT) */


#if defined (CHNL_COMPONENT)
/** ============================================================================
 *  @name   CHNL_Irp
 *
 *  @desc   CHIRP (Channel Input/output Request Packet) data structure.
 *          This structure matches its counterpart on the GPP side.
 *
 *  @field  link
 *              List element header needed for this structure.
 *  @field  buffer
 *              Buffer to be filled/emptied.
 *  @field  arg
 *              Issue reclaim argument.
 *  @field  size
 *              Buffer length.
 *  @field  iocStatus
 *              Status of IO Completion.
 *  @field  chnlId
 *              Channel ID
 *  ============================================================================
 */
typedef struct CHNL_Irp_tag {
    QUE_Elem  link      ;
    Ptr       buffer    ;
    LgUns     arg       ;
    LgUns     size      ;
    LgUns     iocStatus ;
    LgUns     chnlId ;
} CHNL_Irp ;
#endif /* if defined (CHNL_COMPONENT) */



/** ============================================================================
 *  @name   POOL
 *
 *  @desc   POOL variable defined by MSGQ module. This is temporary until
 *          this definition is exported by DSP/BIOS configuration tool.
 *  ============================================================================
 */
extern POOL_Config  * POOL ;


/** ============================================================================
 *  @func   DSPLINK_init
 *
 *  @desc   This function initializes DSP/BIOS LINK.
 *
 *  @arg    None.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
Void
DSPLINK_init (Void) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* if !defined (DSPLINK_) */
