/** ============================================================================
 *  @file   buf_pool.c
 *
 *  @path   $(DSPLINK)/dsp/src/pools/
 *
 *  @desc   Implementation of the buffer pool. This module implements the
 *          pool interface as required by the POOL module of DSP/BIOS(TM).
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


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <sys.h>
#include <buf.h>
#include <mem.h>
#include <pool.h>
#include <platform.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <buf_pool.h>
#include <_buf_pool.h>
#include <dbc.h>


#ifdef __cplusplus
extern "C" {
#endif

/*  ============================================================================
 *  @const  DEFAULT_ALIGN
 *
 *  @desc   Default alignment for MEM_alloc
 *  ============================================================================
 */
#define DEFAULT_ALIGN   0


/** ----------------------------------------------------------------------------
 *  @func   BUFPOOL_open
 *
 *  @desc   Opens the buffer pool and configures it according to the user
 *          attributes.
 *          This function is not reentrant.
 *
 *  @arg    object
 *              Placeholder to receive the BUFPOOL object.
 *  @arg    params
 *              Parameters for configuring the BUFPOOL.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EALLOC
 *              Failure during memory operation.
 *
 *  @enter  object must be a valid pointer.
 *          params must be valid.
 *
 *  @leave  On success, the value returned through the object pointer must be
 *          non-NULL.
 *          On failure, the value returned through the object pointer must be
 *          NULL.
 *
 *  @see    BUFPOOL_Params, BUFPOOL_State, BUFPOOL_close ()
 *  ----------------------------------------------------------------------------
 */
static
Int
BUFPOOL_open (Ptr * object, Ptr params) ;


/** ----------------------------------------------------------------------------
 *  @func   BUFPOOL_close
 *
 *  @desc   Closes the pool, and cleans up its state object.
 *          This function is not reentrant.
 *
 *  @arg    object
 *              Handle to the BUFPOOL state object.
 *
 *  @ret    None.
 *
 *  @enter  object must be valid.
 *
 *  @leave  None.
 *
 *  @see    BUFPOOL_State, BUFPOOL_open ()
 *  ----------------------------------------------------------------------------
 */
static
Void
BUFPOOL_close (Ptr object) ;


/** ----------------------------------------------------------------------------
 *  @func   BUFPOOL_alloc
 *
 *  @desc   Allocates a buffer of the specified size.
 *          This function is reentrant.
 *
 *  @arg    object
 *              Handle to the BUFPOOL state object.
 *  @arg    buf
 *              Placeholder to receive the handle to the allocated buffer.
 *  @arg    size
 *              Size (in MADUs) of the buffer to be allocated.
 *
 *  @ret    SYS_OK
 *              The buffer was successfully allocated
 *          SYS_EALLOC
 *              Failure in buffer allocation.
 *          SYS_EINVAL
 *              Invalid size specified.
 *
 *  @enter  The pool must be opened.
 *          object must be valid.
 *          buf must be a valid pointer.
 *
 *  @leave  On success, *buf contains a non-NULL value.
 *          On failure, *buf is NULL.
 *
 *  @see    BUFPOOL_State, BUFPOOL_free ()
 *  ----------------------------------------------------------------------------
 */
static
Int
BUFPOOL_alloc (Ptr object, Ptr * buf, size_t size) ;


/** ----------------------------------------------------------------------------
 *  @func   BUFPOOL_free
 *
 *  @desc   Frees a buffer of the specified size.
 *          This function is reentrant.
 *
 *  @arg    object
 *              Handle to the BUFPOOL state object.
 *  @arg    buf
 *              Pointer to the buffer to be freed.
 *  @arg    size
 *              Size (in MADUs) of the buffer to be freed.
 *
 *  @ret    None
 *
 *  @enter  The pool must be opened.
 *          object must be valid.
 *          buf must be valid.
 *
 *  @leave  None
 *
 *  @see    BUFPOOL_State, BUFPOOL_alloc ()
 *  ----------------------------------------------------------------------------
 */
static
Void
BUFPOOL_free (Ptr object, Ptr buf, size_t size) ;



/** ============================================================================
 *  @name   BUFPOOL_FXNS
 *
 *  @desc   Buffer pool function pointer table.
 *  ============================================================================
 */
const POOL_Fxns BUFPOOL_FXNS = {
    &BUFPOOL_open,
    &BUFPOOL_close,
    &BUFPOOL_alloc,
    &BUFPOOL_free
} ;


/** ============================================================================
 *  Create named sections for all BUFPOOL functions to allow specific memory
 *  placement.
 *  ============================================================================
 */
#pragma CODE_SECTION (BUFPOOL_open,  ".text:BUFPOOL_open")
#pragma CODE_SECTION (BUFPOOL_close, ".text:BUFPOOL_close")
#pragma CODE_SECTION (BUFPOOL_alloc, ".text:BUFPOOL_alloc")
#pragma CODE_SECTION (BUFPOOL_free,  ".text:BUFPOOL_free")


/** ----------------------------------------------------------------------------
 *  @func   BUFPOOL_open
 *
 *  @desc   Opens the buffer pool and configures it according to the user
 *          attributes.
 *          This function is not reentrant.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Int
BUFPOOL_open (Ptr * object, Ptr params)
{
    Int              status     = SYS_OK ;
    BUFPOOL_Params * poolParams = (BUFPOOL_Params *) params ;
    BUF_Attrs        bufAttrs   = BUF_ATTRS ;
    Uint16           count      = 0 ;
    BUFPOOL_State *  poolState ;
    BUFPOOL_Handle   bufObjs ;
    Uint32           numBufObjs ;
    Uint32           i ;
    Uint32           j ;
    Uint32 *         bufSize ;
    Uint32 *         numBufs ;

    DBC_require (object != NULL) ;
    DBC_require (params != NULL) ;

    /*  ------------------------------------------------------------------------
     *  Default parameters are not supported for the BUFPOOL. User must always
     *  specify valid parameters.
     *  ------------------------------------------------------------------------
     */
    if ((object == NULL) || (params == NULL)) {
        status = SYS_EINVAL ;
        if (object != NULL) {
            *object = NULL ;
        }
    }
    else {
        poolState = MEM_alloc (DSPLINK_SEGID,
                               sizeof (BUFPOOL_Obj),
                               DEFAULT_ALIGN) ;
        if (poolState != MEM_ILLEGAL) {
            numBufObjs = poolParams->numBufObjs ;
            /*  Check if user has specified more than one buffer pool of same
             *  sized buffers. If so, combine such pools into a single one with
             *  the total number of buffers
             */
            bufSize  = MEM_alloc  (DSPLINK_SEGID,
                                   (numBufObjs * sizeof (Uint32)),
                                   DEFAULT_ALIGN) ;
            numBufs  = MEM_calloc (DSPLINK_SEGID,
                                   (numBufObjs * sizeof (Uint32)),
                                   DEFAULT_ALIGN) ;
            if ((bufSize != MEM_ILLEGAL) && (numBufs != MEM_ILLEGAL)) {
                for (i = 0 ; i < numBufObjs ; i++) {
                    if (numBufs [i] != (Uint32) -1) {
                        poolParams->bufSizes [i] =
                                   DSPLINK_ALIGN (poolParams->bufSizes [i],
                                                      DSPLINK_BUF_ALIGN) ;
                        bufSize [count] = poolParams->bufSizes [i] ;
                        numBufs [count] = poolParams->numBuffers [i] ;
                        for (j = i + 1 ; j < numBufObjs ; j++) {
                            if (bufSize [count] == poolParams->bufSizes [j]) {
                                numBufs [count] += poolParams->numBuffers [j] ;
                                numBufs [j] = (Uint32) -1 ;
                            }
                        }
                        count++ ;
                    }
                }

                bufObjs = MEM_alloc (DSPLINK_SEGID,
                                     (count * sizeof (BUFPOOL_Obj)),
                                     DEFAULT_ALIGN) ;
                if (bufObjs != MEM_ILLEGAL) {
                    poolState->numBufObjs = count ;
                    poolState->bufObjs    = bufObjs ;
                    bufAttrs.segid = poolParams->segId ;

                    for (i = 0 ; (i < count) ; i++) {
                        bufObjs [i].bufSize = bufSize [i] ;
                        bufObjs [i].bufList = BUF_create (
                                  numBufs [i],       /* Number of buffers    */
                                  bufSize [i],       /* Size of each buffer  */
                                  DSPLINK_BUF_ALIGN, /* Alignment of buffers */
                                  &bufAttrs) ;       /* Attributes           */
                        if (bufObjs [i].bufList == NULL) {
                            status = SYS_EALLOC ;
                            break ;
                        }
                    }
                }
                else {
                    status = SYS_EALLOC ;
                }
            }
            else {
                status = SYS_EALLOC ;
            }

            /* Free temporary memory */
            if (bufSize != MEM_ILLEGAL) {
                MEM_free (DSPLINK_SEGID,
                          bufSize,
                          (numBufObjs * sizeof (Uint32))) ;
            }
            if (numBufs != MEM_ILLEGAL) {
                MEM_free (DSPLINK_SEGID,
                          numBufs,
                          (numBufObjs * sizeof (Uint32))) ;
            }
        }
        else {
            status = SYS_EALLOC ;
        }

        *object = poolState ;

        if (status != SYS_OK) {
            /*  Cleanup any allocated resources on failure */
            if (poolState != MEM_ILLEGAL) {
                BUFPOOL_close (poolState) ;
            }
        }
    }

    DBC_ensure (   ((status == SYS_OK) && (*object != NULL))
                || (status != SYS_OK)) ;
    DBC_ensure (   ((status != SYS_OK) && (object == NULL))
                || ((status != SYS_OK) && (object != NULL) && (*object == NULL))
                || (status == SYS_OK)) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   BUFPOOL_close
 *
 *  @desc   Closes the pool, and cleans up its state object.
 *          This function is not reentrant.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Void
BUFPOOL_close (Ptr object)
{
    BUFPOOL_State *  poolState ;
    BUFPOOL_Handle   bufObjs ;
    Uint32           numBufObjs ;
    Uint32           i ;

    DBC_require (object != NULL) ;

    poolState = (BUFPOOL_State *) object ;
    DBC_assert (poolState != NULL) ;

    bufObjs    = poolState->bufObjs ;
    numBufObjs = poolState->numBufObjs ;
    if (bufObjs != MEM_ILLEGAL) {
        for (i = 0 ;
             ((i < numBufObjs) && (bufObjs [i].bufList != NULL)) ;
             i++) {
            BUF_delete (bufObjs [i].bufList) ;
        }
        MEM_free (DSPLINK_SEGID,
                  bufObjs,
                  (numBufObjs * sizeof (BUFPOOL_Obj))) ;
    }
    MEM_free (DSPLINK_SEGID, poolState, sizeof (BUFPOOL_State)) ;
}


/** ----------------------------------------------------------------------------
 *  @func   BUFPOOL_alloc
 *
 *  @desc   Allocates a buffer of the specified size.
 *          This function is reentrant.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Int
BUFPOOL_alloc (Ptr object, Ptr * buf, size_t size)
{
    Int              status = SYS_EINVAL ;
    BUFPOOL_State *  poolState ;
    BUFPOOL_Handle   bufObjs ;
    Uint32           i ;

    DBC_require (object != NULL) ;
    DBC_require (buf    != NULL) ;

    poolState = (BUFPOOL_State *) object ;

    /* Align the buffer size to cache line for correct cache operation */
    size = DSPLINK_ALIGN (size, DSPLINK_BUF_ALIGN) ;

    /* Parameters are not validated for this function for run-time efficiency */
    bufObjs = poolState->bufObjs ;
    *buf = NULL ; /* Initialize the return parameter to NULL */
    for (i = 0 ; i < poolState->numBufObjs ; i++) {
        if (bufObjs [i].bufSize == size) {
            *buf = BUF_alloc (bufObjs [i].bufList) ;
            if (*buf != MEM_ILLEGAL) {
                DBC_assert (((Uint32) *buf % DSPLINK_BUF_ALIGN) == 0) ;
                status = SYS_OK ;
            }
            else {
                status = SYS_EALLOC ;
            }
            break ;
        }
    }

    DBC_ensure (    ((status == SYS_OK) && (*buf != NULL))
                ||  ((status != SYS_OK) && (*buf == NULL))) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   BUFPOOL_free
 *
 *  @desc   Frees a buffer of the specified size.
 *          This function is reentrant.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Void
BUFPOOL_free (Ptr object, Ptr buf, size_t size)
{
    BUFPOOL_State * poolState ;
    BUFPOOL_Handle  bufObjs ;
    Uint32         i ;

    DBC_require (object != NULL) ;
    DBC_require (buf    != NULL) ;

    poolState = (BUFPOOL_State *) object ;

    /* Align the buffer size to cache line for correct cache operation */
    size = DSPLINK_ALIGN (size, DSPLINK_BUF_ALIGN) ;

    bufObjs = poolState->bufObjs ;
    for (i = 0 ; i < poolState->numBufObjs ; i++) {
        if (bufObjs [i].bufSize == size) {
            BUF_free (bufObjs [i].bufList, buf) ;
            break ;
        }
    }
}


#ifdef __cplusplus
}
#endif /* extern "C" */
