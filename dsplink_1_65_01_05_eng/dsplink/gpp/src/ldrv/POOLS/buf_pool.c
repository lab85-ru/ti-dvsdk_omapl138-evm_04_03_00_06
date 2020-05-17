/** ============================================================================
 *  @file   buf_pool.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/POOLS/
 *
 *  @desc   Implements the BUF pool interface.
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


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>
#include <procdefs.h>
#include <linkcfgdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Header                   */
#include <mem.h>
#include <mem_os.h>
#include <sync.h>

/*  ----------------------------------- Link Driver                   */
#include <cfgmap.h>
#include <ldrv_pool.h>
#include <ldrv_smm.h>
#include <mpbuf.h>
#include <buf_pool.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent ID.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_POOL_BUF


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON TRC_SetReason (status, FID_C_LDRV_POOL_BUF, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   BUS_WIDTH
 *
 *  @desc   Width of bus in bytes.
 *  ============================================================================
 */
#define BUS_WIDTH  4u


/** ============================================================================
 *  @name   BUFPOOL_Interface
 *
 *  @desc   BUF Pool interface table.
 *  ============================================================================
 */
POOL_Interface BUFPOOL_Interface = {
    &BUFPOOL_open,
    &BUFPOOL_close,
    &BUFPOOL_alloc,
    &BUFPOOL_free,
    NULL,
    NULL,
    NULL,
    NULL
} ;


/** ============================================================================
 *  @func   BUFPOOL_init
 *
 *  @desc   Performs global initialization of the buffer pool.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
BUFPOOL_init (IN  ProcessorId dspId,
              IN  Uint32      poolId,
              OUT Uint32 *    shDspAddr,
              OUT Pvoid *     object)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_4ENTER ("BUFPOOL_init", dspId, poolId, shDspAddr, object) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId    != POOL_INVALIDID) ;
    DBC_Require (shDspAddr != NULL) ;
    DBC_Assert  (object    != NULL) ;

    /* The BUF POOL does not require any shared memory. */
    *shDspAddr = (Uint32) NULL ;

    /* Initialize the MPBUF component. */
    status = MPBUF_Initialize () ;
    if (DSP_SUCCEEDED (status)) {
        /* Allocate memory for the state object */
        status = MEM_Calloc ((Void **) &object,
                             (sizeof (BUFPOOL_State)),
                             MEM_DEFAULT) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("BUFPOOL_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   BUFPOOL_exit
 *
 *  @desc   Performs global finalization of the buffer pool.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
BUFPOOL_exit (IN  ProcessorId dspId,
              IN  Uint32      poolId,
              IN  Void *      object)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus ;

    TRC_3ENTER ("BUFPOOL_exit", dspId, poolId, object) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId    != POOL_INVALIDID) ;

    if (object != NULL) {
        /* Free memory for the state object  */
        status = FREE_PTR (object) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    /* Finalize the MPBUF component. */
    tmpStatus = MPBUF_Finalize () ;
    if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
        status = tmpStatus ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("BUFPOOL_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   BUFPOOL_open
 *
 *  @desc   Opens the buffer pool and configures it according to the user
 *          attributes. This function is called once during
 *          POOL_open (). It creates an instance of the
 *          BUFPOOL_State object, initializes it, and returns the handle to the
 *          caller. This handle to the pool state object is passed to the
 *          allocator during its functions for allocating and freeing the
 *          messages. The steps involved in initialization of the pool are:
 *
 *              1.Calculate the total size of buffer required for the pool.
 *                This is equivalent to:
 *                The sum of (For each pool: (numBufs * bufSize))
 *
 *              2.Allocate a contiguous buffer of calculated size.
 *
 *              3.Create and initialize each buffer pool through calls to
 *                MPBUF_Create (), passing the kernel address of the contiguous
 *                buffer reserved for each pool through step (2).
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
BUFPOOL_open (IN  ProcessorId       dspId,
              IN  Uint32            poolId,
              IN  Void *            object,
              IN  POOL_OpenParams * poolOpenParams)
{
    DSP_STATUS       status     = DSP_SOK ;
    DSP_STATUS       tmpStatus  = DSP_SOK ;
    Uint16           count      = 0 ;
    Uint32           size       = 0 ;
    Bool             first      = FALSE ;
    BUFPOOL_State *  poolState ;
    BUFPOOL_Attrs *  bufAttrs ;
    Uint32           numBufPools ;
    BUFPOOL_Obj *    bufObjs ;
    Uint32           i ;
    Uint32           j ;
    Uint32           startAddr ;
    Uint32 *         bufSize ;
    Uint32 *         numBufs ;
    MemAllocAttrs    allocArg ;
    Uint32           irqFlags ;

    TRC_2ENTER ("BUFPOOL_open", object, poolOpenParams) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId         != POOL_INVALIDID) ;
    DBC_Require (object         != NULL) ;
    DBC_Require (poolOpenParams != NULL) ;

    poolState = (BUFPOOL_State *) object ;

    irqFlags = SYNC_SpinLockStart () ;
    if (poolState->refCount == 0) {
        poolState->refCount = 1 ;
        first = TRUE ;
    }
    SYNC_SpinLockEnd (irqFlags) ;

    if (first == TRUE) {
        bufAttrs  = (BUFPOOL_Attrs *) poolOpenParams->params ;
        DBC_Require (bufAttrs != NULL) ;
        /* Attributes are required for the first call to BUFPOOL_open () */
        if (bufAttrs == NULL) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        else {
            /*  ----------------------------------------------------------------
             *  Validate attributes from the parameters structure
             *  ----------------------------------------------------------------
             */
            if (    (bufAttrs->bufSizes == NULL)
                ||  (bufAttrs->numBuffers == NULL)) {
                status = DSP_EINVALIDARG ;
                SET_FAILURE_REASON ;
            }
            else {
                /*  ------------------------------------------------------------
                 *  Check if user has specified more than one buffer pool of
                 *  same sized buffers. If so, combine such pools into a single
                 *  one with the total number of buffers.
                 *  ------------------------------------------------------------
                 */
                numBufPools = bufAttrs->numBufPools ;
                status = MEM_Alloc ((Void **) &bufSize,
                                    (numBufPools * sizeof (Uint32)),
                                    MEM_DEFAULT) ;
                if (DSP_SUCCEEDED (status)) {
                    status = MEM_Calloc ((Void **) &numBufs,
                                        (numBufPools * sizeof (Uint32)),
                                        MEM_DEFAULT) ;
                    if (DSP_SUCCEEDED (status)) {
                        for (i = 0, count = 0 ; i < numBufPools ; i++) {
                            if (numBufs [i] != (Uint32) -1) {
                                bufSize [count] = DSPLINK_ALIGN (
                                                         bufAttrs->bufSizes [i],
                                                         BUS_WIDTH) ;
                                DBC_Assert (bufSize [count] % BUS_WIDTH == 0) ;
                                numBufs [count] = bufAttrs->numBuffers [i] ;
                                for (j = i + 1 ; j < numBufPools ; j++) {
                                    if (    bufSize [count]
                                        ==  DSPLINK_ALIGN (
                                                        bufAttrs->bufSizes [j],
                                                        BUS_WIDTH)) {
                                        numBufs [count] +=
                                                      bufAttrs->numBuffers [j] ;
                                        numBufs [j] = (Uint32) -1 ;
                                    }
                                }
                                count++ ;
                            }
                        }
                    }
                    else {
                        SET_FAILURE_REASON ;
                        FREE_PTR (bufSize) ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }

            if (DSP_SUCCEEDED (status)) {
                /* Allocate memory for the buffer pool objects */
                numBufPools = count ;
                poolState->numBufPools = numBufPools ;
                status = MEM_Calloc ((Void **) &bufObjs,
                                     (numBufPools * sizeof (BUFPOOL_Obj)),
                                     MEM_DEFAULT) ;
                if (DSP_SUCCEEDED (status)) {
                    poolState->bufObjs = bufObjs ;
                    /*  --------------------------------------------------------
                     *  Calculate the size of the continuous memory chunk which
                     *  has to be allocated
                     *  --------------------------------------------------------
                     */
                    for (i = 0 ; i < numBufPools ; i++) {
                        size += (numBufs [i] * bufSize [i]) ;
                    }
                    poolState->size = size ;

                    status = MEM_Alloc ((Void **) &(poolState->virtAddr),
                                        size,
                                        &allocArg) ;
                    if (DSP_SUCCEEDED (status)) {
                        startAddr = poolState->virtAddr ;
                        /* Save the physical address in the state object */
                        poolState->phyAddr = (Uint32) allocArg.physicalAddress ;
                        for (i = 0 ;
                             ((i < numBufPools) && (DSP_SUCCEEDED (status))) ;
                             i++) {
                            /*  ------------------------------------------------
                             *  Create MPBUF pools for the arguments specified
                             *  by the user
                             *  ------------------------------------------------
                             */
                            bufObjs [i].bufSize = bufSize [i] ;
                            status = MPBUF_Create (numBufs   [i],
                                                   bufSize   [i],
                                                   &(bufObjs [i].bufList),
                                                   startAddr,
                                                   (Uint32) NULL) ;
                            startAddr += (numBufs  [i]) * (bufSize [i]) ;
                        }
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }

                if (DSP_FAILED (status)) {
                    BUFPOOL_close (dspId, poolId, (Pvoid) poolState) ;
                }

                tmpStatus = FREE_PTR (bufSize) ;
                if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }

                tmpStatus = FREE_PTR (numBufs) ;
                if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
        }
    }
    else {
        poolState->refCount++ ;
        status = DSP_SALREADYOPENED ;
    }

    if (DSP_SUCCEEDED (status)) {
        /* ---------------------------------------------------------------------
         * Update the pool parameters for translation. later to be used with
         * alloc and free, when called from user process.
         * ---------------------------------------------------------------------
         */
         poolOpenParams->physAddr = poolState->phyAddr ;
         poolOpenParams->virtAddr = poolState->virtAddr ;
         poolOpenParams->dspAddr  = (Uint32) NULL ; /* Unshared pool */
         poolOpenParams->size     = poolState->size ;
    }

    TRC_1LEAVE ("BUFPOOL_open", status) ;

    return status ;
}


/** ============================================================================
 *  @func   BUFPOOL_close
 *
 *  @desc   This function closes the pool, and cleans up its state object.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
BUFPOOL_close (IN  ProcessorId       dspId,
               IN  Uint32            poolId,
               IN  Void *            object)
{
    DSP_STATUS       status      = DSP_SOK ;
    DSP_STATUS       tmpStatus   = DSP_SOK ;
    Bool             last        = FALSE ;
    BUFPOOL_State *  poolState ;
    BUFPOOL_Obj *    bufObjs ;
    Uint32           numBufPools ;
    Uint32           i ;
    MemFreeAttrs     freeArg ;
    Uint32           irqFlags ;

    TRC_3ENTER ("BUFPOOL_close", dspId, poolId, object) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId         != POOL_INVALIDID) ;
    DBC_Require (object         != NULL) ;

    poolState = (BUFPOOL_State *) object ;

    if (poolState != NULL) {
        if (poolState->refCount == 0) {
            status = DSP_EFAIL ;
            SET_FAILURE_REASON ;
        }
        else {
            irqFlags = SYNC_SpinLockStart () ;
            poolState->refCount-- ;
            if (poolState->refCount == 0) {
                last = TRUE ;
            }
            SYNC_SpinLockEnd (irqFlags) ;

            /* Close the pool if its reference count has reached 0. */
            if (last == TRUE) {
                bufObjs     = poolState->bufObjs ;
                numBufPools = poolState->numBufPools ;

                if (bufObjs != NULL) {
                    /*  --------------------------------------------------------
                     *  Delete the BUF pools
                     *  --------------------------------------------------------
                     */
                    for (i = 0 ;
                         ((i < numBufPools) && (bufObjs [i].bufList != NULL)) ;
                         i++) {
                        tmpStatus = MPBUF_Delete (bufObjs [i].bufList) ;
                        if (    (DSP_SUCCEEDED (status))
                            &&  (DSP_FAILED (tmpStatus))) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }

                    /*  --------------------------------------------------------
                     *  Free memory for the buffer pool objects
                     *  --------------------------------------------------------
                     */
                    tmpStatus = FREE_PTR (bufObjs) ;
                    if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }

                if ((Pvoid) (poolState->virtAddr) != NULL) {
                    /*  --------------------------------------------------------
                     *  Free memory for the continuous memory chunk which was
                     *  allocated for creating BUF pools
                     *  --------------------------------------------------------
                     */
                    freeArg.physicalAddress = (Uint32 *) (poolState->phyAddr) ;
                    freeArg.size            = poolState->size ;
                    tmpStatus = MEM_Free ((Pvoid *) &(poolState->virtAddr),
                                          &freeArg) ;
                    if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
            }
        }
    }

    TRC_1LEAVE ("BUFPOOL_close", status) ;

    return status ;
}


/** ============================================================================
 *  @func   BUFPOOL_alloc
 *
 *  @desc   This function allocates a message buffer of the specified size.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
BUFPOOL_alloc (IN  ProcessorId       dspId,
               IN  Uint32            poolId,
               IN  Void *            object,
               OUT Pvoid *           bufPtr,
               IN  Uint32            size)
{
    DSP_STATUS        status = DSP_SOK ;
    Bool              found  = FALSE ;
    BUFPOOL_State *   poolState ;
    BUFPOOL_Obj *     bufObjs ;
    Uint32            i ;
    Uint32            irqFlags ;

    TRC_5ENTER ("BUFPOOL_alloc", dspId, poolId, object, bufPtr, size) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId != POOL_INVALIDID) ;
    DBC_Require (object != NULL) ;
    DBC_Require (bufPtr != NULL) ;
    DBC_Require (size   != 0) ;

    *bufPtr = NULL ;
    poolState = (BUFPOOL_State *) object ;

    /* Check if pool is opened. */
    if (poolState->refCount == 0) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }
    else {
        bufObjs = poolState->bufObjs ;

        /* ---------------------------------------------------------------------
         * Align the size to bus width.
         * ---------------------------------------------------------------------
         */
        size  = DSPLINK_ALIGN (size, BUS_WIDTH) ;

        /*  --------------------------------------------------------------------
         *  Look for buffer object for specified size
         *  --------------------------------------------------------------------
         */
        for (i = 0 ; ((i < poolState->numBufPools) && (found == FALSE)) ; i++) {
            if (bufObjs [i].bufSize == size) {
                found = TRUE ;
                irqFlags = SYNC_SpinLockStart () ;
                status = MPBUF_Alloc (bufObjs [i].bufList, bufPtr) ;
                SYNC_SpinLockEnd (irqFlags) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
        }

        /*  --------------------------------------------------------------------
         * Invalid size requested by user
         *  --------------------------------------------------------------------
         */
        if (found == FALSE) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
    }

    DBC_Ensure (   ((*bufPtr == NULL) && DSP_FAILED (status))
                || (DSP_SUCCEEDED (status))) ;

    DBC_Ensure (   ((*bufPtr != NULL) && DSP_SUCCEEDED (status))
                 || (DSP_FAILED (status))) ;

    TRC_1LEAVE ("BUFPOOL_alloc", status) ;

    return status ;
}


/** ============================================================================
 *  @func   BUFPOOL_free
 *
 *  @desc   This function frees a buffer of the specified size.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
BUFPOOL_free (IN  ProcessorId       dspId,
              IN  Uint32            poolId,
              IN  Void *            object,
              IN  Void *            buf,
              IN  Uint32            size)
{
    DSP_STATUS        status = DSP_SOK ;
    Bool              found  = FALSE ;
    BUFPOOL_State *   poolState ;
    BUFPOOL_Obj *     bufObjs ;
    Uint32            i ;
    Uint32            irqFlags ;

    TRC_5ENTER ("BUFPOOL_free", dspId, poolId, object, buf, size) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId != POOL_INVALIDID) ;
    DBC_Require (object != NULL) ;
    DBC_Require (buf    != NULL) ;
    DBC_Require (size   != 0) ;

    poolState = (BUFPOOL_State *) object ;

    /* Check if pool is opened. */
    if (poolState->refCount == 0) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }
    else {
        bufObjs = poolState->bufObjs ;

        /* ---------------------------------------------------------------------
         * Align the size to bus width.
         * ---------------------------------------------------------------------
         */
        size  = DSPLINK_ALIGN (size, BUS_WIDTH) ;

        /*  --------------------------------------------------------------------
         *  Look for buffer object for specified size
         *  --------------------------------------------------------------------
         */
        for (i = 0 ; ((i < poolState->numBufPools) && (found == FALSE)) ; i++) {
            if (bufObjs [i].bufSize == size) {
                found = TRUE ;
                irqFlags = SYNC_SpinLockStart () ;
                status = MPBUF_Free (bufObjs [i].bufList, buf) ;
                SYNC_SpinLockEnd (irqFlags) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
        }

        /*  --------------------------------------------------------------------
         *  Invalid size requested by user
         *  --------------------------------------------------------------------
         */
        if (found == FALSE) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("BUFPOOL_free", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
