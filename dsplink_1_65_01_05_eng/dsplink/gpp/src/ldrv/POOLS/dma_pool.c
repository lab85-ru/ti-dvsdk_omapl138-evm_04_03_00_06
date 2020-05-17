/** ============================================================================
 *  @file   dma_pool.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/POOLS/
 *
 *  @desc   Implementation of DMA - DMA Copy Memory Allocator.
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
#include <linkcfgdefs.h>
#include <pooldefs.h>
#include <_pooldefs.h>
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Profile                     */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* if defined (DDSP_PROFILE) */

/*  ----------------------------------- OSAL Headers                */
#include <mem_os.h>
#include <mem.h>
#include <sync.h>

/*  ----------------------------------- Generic Functions           */
#include <list.h>

/*  ----------------------------------- User API                    */
#include <mpcs.h>

/*  ----------------------------------- ARCH Headers                */
#include <dsp.h>

/*  ----------------------------------- LDRV Headers                */
#include <ldrv_pool.h>
#include <dma_pool.h>
#include <ldrv_mpcs.h>
#include <ldrv_utils.h>
#include <ldrv_smm.h>
#include <ldrv.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   COMPONENT_ID
 *
 *  @desc   Component ID of this module. That will be used by TRC macros.
 *  ============================================================================
 */
#define COMPONENT_ID    ID_LDRV_POOL_DMA

/** ============================================================================
 *  @name   SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON TRC_SetReason (status, FID_C_LDRV_POOL_DMA, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   INIT_VALUE
 *
 *  @desc   Value to indicate the initialization of DMA module.
 *  ============================================================================
 */
#define INIT_VALUE  0xDEAF

/** ============================================================================
 *  @name   EXIT_VALUE
 *
 *  @desc   Value to indicate the finalization of DMA module.
 *  ============================================================================
 */
#define EXIT_VALUE  0xFACE

/** ============================================================================
 *  @name   BUS_WIDTH
 *
 *  @desc   Width of bus in bytes.
 *  ============================================================================
 */
#define BUS_WIDTH  4


/** ============================================================================
 *  @name   DMAPOOL_Object
 *
 *  @desc   This structure defines the state of the DMA.
 *
 *  @field  dspId
 *              ID of the processor with which the memory manager is shared.
 *  @field  size
 *              Length of the shared memory region.
 *  @field  gppAddr
 *              Virtual address of the shared memory to be used by DMA,
 *              in GPP address space.
 *  @field  dspAddr
 *              Address of the shared memory to be used by DMA, in DSP address
 *              space.
 *  @field  physAddr
 *              physical Address of the shared memory to be used by DMA.
 *  @field  dspMaduSize
 *              DSP elementary addressable unit size.
 *  @field  dmaShmObj
 *              Pointer to the DMA shared memory object.
 *  @field  ctrlPtr
 *              Pointer to the DMA shared memory control structure.
 *  @field  wordSwap
 *              Indicates whether word-swap is enabled for the DSP MMU.
 *  @field  memEntry
 *              ID of the memory entry for the DMAPOOL shared memory region.
 *  @field  refCount
 *              Reference count for the pool.
 *  @field  lock
 *              Lock for using POOL resources.
 *  ============================================================================
 */
typedef struct DMAPOOL_Object_tag {
    ProcessorId      dspId         ;
    Uint32           size          ;
    Uint32           gppAddr       ;
    Uint32           dspAddr       ;
    Uint32           physAddr      ;
    Uint16           dspMaduSize   ;
    DMAPOOL_ShmObj * dmaShmObj     ;
    DMAPOOL_Ctrl *   ctrlPtr       ;
    Bool             wordSwap      ;
    Uint32           memEntry      ;
    Uint32           refCount      ;
    Uint32           bufMemSecId   ;
    Uint32           bufPhysMemAddr ;
    Uint32           bufGppMemAddr ;
    Uint32           bufDspMemAddr ;
    Uint32           bufPoolSize   ;
    Pvoid            lock          ;
} DMAPOOL_Object ;



/** ============================================================================
 *  @name   DMAPOOL_Interface
 *
 *  @desc   DMA Pool interface table.
 *  ============================================================================
 */
POOL_Interface DMAPOOL_Interface = {
    &DMAPOOL_open,
    &DMAPOOL_close,
    &DMAPOOL_alloc,
    &DMAPOOL_free,
    NULL,
    &DMAPOOL_writeback,
    &DMAPOOL_invalidate,
    &DMAPOOL_xltBuf
} ;


/** ============================================================================
 *  @func   DMAPOOL_init
 *
 *  @desc   This function initializes the DMA POOL component.
 *
 *  @modif  object.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_init (IN  ProcessorId dspId,
              IN  Uint32      poolId,
              OUT Uint32 *    shDspAddr,
              OUT Pvoid *     object)
{
    DSP_STATUS          status   = DSP_SOK ;
    DMAPOOL_Object *    dmaState = NULL    ;
    LINKCFG_Dsp *       dspObj             ;
    LINKCFG_LinkDrv *   linkDrv            ;
    LINKCFG_Pool *      poolObject         ;
    LINKCFG_DspConfig * dspCfg             ;

    TRC_4ENTER ("DMAPOOL_init", dspId, poolId, shDspAddr, object) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId    != POOL_INVALIDID) ;
    DBC_Require (shDspAddr != NULL) ;
    DBC_Assert  (object    != NULL) ;

    dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj   = dspCfg->dspObject ;
    linkDrv    = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
    poolObject = &(dspCfg->poolTables [linkDrv->poolTableId][poolId]) ;

    /*  ------------------------------------------------------------------------
     *  Allocate memory for the DMAPOOL state object.s
     *  ------------------------------------------------------------------------
     */
    status = MEM_Calloc (object, sizeof (DMAPOOL_Object), MEM_DEFAULT) ;
    if (DSP_SUCCEEDED (status)) {
        dmaState = (DMAPOOL_Object *) *object ;

        /* Initialize the state object fields. */
        dmaState->dspId       = dspId ;
        dmaState->size        = sizeof (DMAPOOL_ShmObj) ;
        dmaState->dspMaduSize = dspObj->maduSize ;
        dmaState->wordSwap    = dspObj->wordSwap ;
        dmaState->memEntry    = poolObject->memEntry ;
        dmaState->bufMemSecId = poolObject->poolMemEntry ;
        /* Create the lock */
        dmaState->lock = SYNC_SpinLockCreate () ;

        /*  --------------------------------------------------------------------
         *  Allocate shared memory required for the DMAPOOL.
         *  --------------------------------------------------------------------
         */
        status = LDRV_SMM_alloc (dspId,
                                 dmaState->memEntry,
                                 &(dmaState->physAddr),
                                 &(dmaState->dspAddr),
                                 &(dmaState->gppAddr),
                                 dmaState->size) ;
        if (DSP_SUCCEEDED (status)) {
            *shDspAddr = dmaState->dspAddr ;
            TRC_1PRINT (TRC_LEVEL4,
                        "DMAPOOL shared memory GPP address      [0x%x]\n",
                        dmaState->gppAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "DMAPOOL shared memory DSP address      [0x%x]\n",
                        dmaState->dspAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "DMAPOOL shared memory physical address [0x%x]\n",
                        dmaState->physAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "DMAPOOL shared memory size             [0x%x]\n",
                        dmaState->size) ;
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        dmaState->bufPoolSize = poolObject->poolSize ;
        status = LDRV_SMM_alloc (dspId,
                                 dmaState->bufMemSecId,
                                 &(dmaState->bufPhysMemAddr),
                                 &(dmaState->bufDspMemAddr),
                                 &(dmaState->bufGppMemAddr),
                                 dmaState->bufPoolSize) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        /* ---------------------------------------------------------------------
         * check whether shared memory is mapped correctly.
         * ---------------------------------------------------------------------
         */
        if (dmaState->gppAddr == 0) {
            status = DSP_EMEMORY ;
            SET_FAILURE_REASON ;
        }
        else {
            /* -----------------------------------------------------------------
             * Create the DMAPOOL_Ctrl structure in the shared memory block.
             * -----------------------------------------------------------------
             */
            dmaState->dmaShmObj = (DMAPOOL_ShmObj *) (dmaState->gppAddr) ;
            dmaState->ctrlPtr   = &(dmaState->dmaShmObj->ctrlPtr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "DMAPOOL Ctrl GPP address               [0x%x]\n",
                        dmaState->ctrlPtr) ;

            /* -----------------------------------------------------------------
             * Create the MPCS Component.
             * -----------------------------------------------------------------
             */
            status = LDRV_MPCS_create (dspId, &(dmaState->dmaShmObj->mpcsObj)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    if (DSP_FAILED (status)) {
        DMAPOOL_exit (dspId, poolId, *object) ;
    }

    TRC_1LEAVE ("DMAPOOL_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DMAPOOL_exit
 *
 *  @desc   This function finalizes the DMA POOL component.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_exit (IN  ProcessorId dspId,
              IN  Uint32      poolId,
              IN  Void *      object)
{
    DSP_STATUS        status    = DSP_SOK ;
    DSP_STATUS        tmpStatus = DSP_SOK ;
    DMAPOOL_Object *  dmaState            ;

    TRC_3ENTER ("DMAPOOL_exit", dspId, poolId, object) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId    != POOL_INVALIDID) ;

    dmaState  = (DMAPOOL_Object *) object ;
    if (dmaState != NULL) {
        if (dmaState->dmaShmObj != NULL) {
            /* -----------------------------------------------------------------
             * Finalize the MPCS object.
             * -----------------------------------------------------------------
             */
            status = LDRV_MPCS_delete (dspId, &(dmaState->dmaShmObj->mpcsObj)) ;
        }

        if (dmaState->bufGppMemAddr != (Uint32) NULL) {
            /*  ----------------------------------------------------------------
             *  Free shared memory allocated for the SMAPOOL component.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_SMM_free (dspId,
                                       dmaState->bufMemSecId,
                                       dmaState->bufPoolSize) ;
            if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        if (dmaState->gppAddr != (Uint32) NULL) {
            /*  ----------------------------------------------------------------
             *  Free shared memory allocated for the DMAPOOL component.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_SMM_free (dspId,
                                       dmaState->memEntry,
                                       dmaState->size) ;
            if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        /* Delete  lock */
        SYNC_SpinLockDelete (dmaState->lock) ;

        tmpStatus = FREE_PTR (dmaState) ;
        if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("DMAPOOL_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DMAPOOL_open
 *
 *  @desc   This function will create buffer handles and buffer pools.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_open (IN  ProcessorId       dspId,
              IN  Uint32            poolId,
              IN  Void *            object,
              IN  POOL_OpenParams * poolOpenParams)
{
    DSP_STATUS       status   = DSP_SOK ;
    Uint32           i        = 0       ;
    Uint32           j        = 0       ;
    Uint16           count    = 0       ;
    Bool             first    = FALSE   ;
    DMAPOOL_Ctrl *   ctrlPtr  = NULL    ;
    DMAPOOL_Attrs *  dmaAttrs           ;
    DMAPOOL_Object * dmaState           ;
    Uint32           bufDspHeader       ; /* Holds the start address of buffers in DSP
                                           * address space.
                                           */
    DmaBufObj *      dmaBufObj          ; /* Holds the buffer handles */
    Uint8 *          bufHeader          ; /* Holds the buffer handles */
    Uint32 *         bufSize            ;
    Uint32 *         numBufs            ;
    Uint32           temp               ;
    Uint32           irqFlags           ;
    DspDmaArgs       dmaArgs            ;

    TRC_4ENTER ("DMAPOOL_open", dspId, poolId, object, poolOpenParams) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId         != POOL_INVALIDID) ;
    DBC_Require (object         != NULL) ;
    DBC_Require (poolOpenParams != NULL) ;

    dmaState = (DMAPOOL_Object *) object ;

    irqFlags = SYNC_SpinLockStartEx (dmaState->lock) ;
    if (dmaState->refCount == 0) {
        first = TRUE ;
    }
    else {
        status = DSP_SALREADYOPENED ;
    }
    dmaState->refCount++ ;

    SYNC_SpinLockEndEx (dmaState->lock, irqFlags) ;

    if (first == TRUE) {
        dmaAttrs = (DMAPOOL_Attrs *) poolOpenParams->params ;
        DBC_Require (dmaAttrs != NULL) ;
        /* Attributes are required for the first call to DMAPOOL_open () */
        if (dmaAttrs == NULL) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        else {
            DBC_Require (dmaAttrs->numBufPools > 0) ;
            DBC_Require (dmaAttrs->numBufPools <= MAX_DMABUFENTRIES) ;

            if (dmaAttrs->numBufPools == 0) {
                status = DSP_EINVALIDARG ;
                SET_FAILURE_REASON ;
            }
            else {
            /* Check for pool sizes being cache line aligned */
                for (i = 0 ; i < dmaAttrs->numBufPools ; i++) {
                    if  (dmaAttrs->numBuffers [i] != 0u) {
                        for (j = 0 ; j < dmaAttrs->numBuffers [i]; j++) {
                            if (   (dmaAttrs->bufSizes [i] % CACHE_L2_LINESIZE)
                                != 0u) {
                                status = DSP_EINVALIDARG ;
                                SET_FAILURE_REASON ;
                                break ;
                            }
                        }
                    }
                    else {
                        status = DSP_EINVALIDARG ;
                        SET_FAILURE_REASON ;
                        break ;
                    }
                }
            }

            if (DSP_SUCCEEDED (status)) {
                ctrlPtr = dmaState->ctrlPtr ;

                dmaState->dmaShmObj->exactMatchReq = (Uint16)
                                                     dmaAttrs->exactMatchReq ;

                /* -------------------------------------------------------------
                 * Convert the buffer handle from DSP to GPP address space.
                 * -------------------------------------------------------------
                 */
                dmaBufObj = (DmaBufObj *) dmaState->ctrlPtr->dmaBufObjs ;

                /* -------------------------------------------------------------
                 * Buffer header are created at a distance
                 * "sizeof(DmaBufObj) * MAX_DMABUFENTRIES", to provide a provision
                 * for adding new buffer pools later on.
                 * -------------------------------------------------------------
                 */
                bufHeader    =  (Uint8 *) dmaState->bufGppMemAddr ;
                bufDspHeader =  dmaState->bufDspMemAddr ;
                TRC_1PRINT (TRC_LEVEL4,
                            "DMAPOOL buffers start GPP address      [0x%x]\n",
                            bufHeader) ;
                TRC_1PRINT (TRC_LEVEL4,
                            "DMAPOOL buffers start DSP address      [0x%x]\n",
                            bufDspHeader) ;

                /* -------------------------------------------------------------
                 * Check for duplicate size entries in DMA attributes.
                 * -------------------------------------------------------------
                 */
                status = MEM_Alloc ((Void **) &bufSize,
                                    (dmaAttrs->numBufPools * sizeof (Uint32)),
                                    MEM_DEFAULT) ;
                if (DSP_SUCCEEDED (status)) {
                    status = MEM_Calloc ((Void **) &numBufs,
                                        (dmaAttrs->numBufPools * sizeof (Uint32)),
                                        MEM_DEFAULT) ;
                    if (DSP_SUCCEEDED (status)) {
                        for (i = 0 ; i < dmaAttrs->numBufPools ; i++) {
                            if (numBufs [i] != (Uint32) -1) {
                                bufSize [count] = DSPLINK_ALIGN (
                                                         dmaAttrs->bufSizes [i],
                                                         BUS_WIDTH) ;
                                DBC_Assert (bufSize [count] % BUS_WIDTH == 0) ;
                                numBufs [count] = dmaAttrs->numBuffers [i] ;
                                for (j = i + 1 ; j < dmaAttrs->numBufPools ; j++) {
                                    if (    bufSize [count]
                                        ==  DSPLINK_ALIGN (
                                                        dmaAttrs->bufSizes [j],
                                                        BUS_WIDTH)) {
                                        numBufs [count] +=
                                                      dmaAttrs->numBuffers [j] ;
                                        numBufs [j] = (Uint32) -1 ;
                                    }
                                }
                                count++ ;
                            }
                        }
                    }
                    else {
                        irqFlags = SYNC_SpinLockStartEx (dmaState->lock) ;
                        dmaState->refCount -- ;
                        SYNC_SpinLockEndEx (dmaState->lock, irqFlags) ;
                        SET_FAILURE_REASON ;
                        FREE_PTR (bufSize) ;
                    }
                }
                else {
                    irqFlags = SYNC_SpinLockStartEx (dmaState->lock) ;
                    dmaState->refCount -- ;
                    SYNC_SpinLockEndEx (dmaState->lock, irqFlags) ;
                    SET_FAILURE_REASON ;
                }

                if (DSP_SUCCEEDED (status)) {
                    /* sort the buf size first */
                    for (i = 0 ; i < count ; i++) {
                        for (j = i + 1 ; j < count ; j++) {
                            if (bufSize [i] > bufSize [j]) {
                                temp = bufSize [i] ;
                                bufSize [i] = bufSize [j] ;
                                bufSize [j] = temp ;

                                /* Also arrange the numbufs in accordance to sizes. */
                                temp = numBufs [i] ;
                                numBufs [i] = numBufs [j] ;
                                numBufs [j] = temp ;
                            }
                        }
                    }

                    /* ---------------------------------------------------------
                     * Update the control structure for number of buffer pools
                     * ---------------------------------------------------------
                     */
                    ctrlPtr->numBufs = count ;
                    temp = dmaState->bufPhysMemAddr ;
                    for (i = 0 ; (i < count) && DSP_SUCCEEDED (status) ; i++) {
                         if (   ((numBufs [i] * bufSize [i]) + (Uint32) bufHeader)
                             <= (dmaState->bufGppMemAddr + dmaState->bufPoolSize)) {
                            dmaBufObj[i].dspMaduSize = dmaState->dspMaduSize ;
                            dmaBufObj[i].wordSwap    = dmaState->wordSwap ;
                            dmaBufObj [i].size = BYTE_TO_MADU (bufSize [i],
                                                            dmaBufObj [i].dspMaduSize) ;
                            dmaBufObj [i].size = SWAP_LONG (dmaBufObj [i].size,
                                                            dmaBufObj [i].wordSwap) ;
                            dmaBufObj [i].totalBuffers   = numBufs [i] ;
                            dmaBufObj [i].freeBuffers    = numBufs [i] ;
                            dmaBufObj [i].startAddress   = (Uint32) bufHeader ;
                            dmaBufObj [i].bufDspAddress  = bufDspHeader ;
                            dmaBufObj [i].bufDspPhysAddress  = bufDspHeader ;
                            dmaBufObj [i].bufGppPhysAddress  = temp ;
                            dmaBufObj [i].nextFree       = 0 ;

                            bufHeader = (Uint8 *) (dmaBufObj [i].startAddress) ;
                            /* -------------------------------------------------
                             * Create the buffer pools.
                             * -------------------------------------------------
                             */
                            for (j = 0 ; j < numBufs [i] ; j++) {
                                ((DmaBufHeader *) bufHeader)->next = (DmaBufHeader *)
                                                SWAP_LONG (dmaBufObj [i].size * (j + 1),
                                                           dmaBufObj [i].wordSwap) ;
                                bufHeader += bufSize [i] ;
                            }

                            /* DMA the buffer area to DSP */
                            dmaArgs.dspId   = dspId ;
                            dmaArgs.srcAddr = temp ;
                            dmaArgs.dstAddr = dmaBufObj [i].bufDspAddress ;
                            dmaArgs.size    = (bufSize [i] * numBufs [i]) ;
                            status = DSP_control (dspId,
                                                  DSP_DmaCtrlCmd_GppToDsp,
                                                  &dmaArgs) ;

                            temp = temp + (bufSize [i] * numBufs [i]) ;
                            bufDspHeader =   bufDspHeader
                                           + BYTE_TO_MADU ((bufSize [i] * numBufs [i]),
                                                           dmaState->dspMaduSize) ;
                        }
                        else {
                            status = DSP_ERANGE ;
                            PRINT_Printf (
                                    "Error: Configured pool size is insufficient for "
                                    "passed parameters.\n"
                                    "Check /dsplink/config/all/CFG_<PLATFORM>.c\n") ;
                            SET_FAILURE_REASON ;
                        }
                    }
                }

                /* Free the allocated memory for numBufs and bufSize regardless of
                 * status.
                 */
                FREE_PTR (bufSize) ;
                FREE_PTR (numBufs) ;
            }
            else {
                irqFlags = SYNC_SpinLockStartEx (dmaState->lock) ;
                dmaState->refCount -- ;
                SYNC_SpinLockEndEx (dmaState->lock, irqFlags) ;
                SET_FAILURE_REASON ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            /* -----------------------------------------------------------------
             * Set the GPP Side DMA flag to initialize condition.
             * -----------------------------------------------------------------
             */
            ctrlPtr->isGppInit = (Uint16) INIT_VALUE ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        /* ---------------------------------------------------------------------
         * Update the pool parameters for translation. later to be used with
         * alloc and free, when called from user process.
         * ---------------------------------------------------------------------
         */
        poolOpenParams->physAddr = dmaState->bufPhysMemAddr ;
        poolOpenParams->virtAddr = dmaState->bufGppMemAddr  ;
        poolOpenParams->dspAddr  = dmaState->bufDspMemAddr  ;
        poolOpenParams->size     = dmaState->bufPoolSize    ;
    }

    TRC_1LEAVE ("DMAPOOL_open", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DMAPOOL_close
 *
 *  @desc   This function closes the buffer pool specified by the user.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_close (IN  ProcessorId       dspId,
               IN  Uint32            poolId,
               IN  Void *            object)
{
    DSP_STATUS       status = DSP_SOK ;
    Bool             last   = FALSE ;
    DMAPOOL_Object * dmaState ;
    DMAPOOL_Ctrl *   ctrlPtr ;
    Uint32           irqFlags ;

    TRC_3ENTER ("DMAPOOL_close", dspId, poolId, object) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId         != POOL_INVALIDID) ;
    DBC_Require (object         != NULL) ;

    /* Check if pool has been opened */
    if (object != NULL) {
        dmaState  = (DMAPOOL_Object *) object ;
        ctrlPtr   = dmaState->ctrlPtr ;

        irqFlags = SYNC_SpinLockStartEx (dmaState->lock) ;
        if (dmaState->refCount == 0) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
        else {
            dmaState->refCount-- ;
            if (dmaState->refCount == 0) {
                last = TRUE ;
                status = DSP_SCLOSED ;
            }
        }
        SYNC_SpinLockEndEx (dmaState->lock, irqFlags) ;

        /* Close the pool if its reference count has reached 0. */
        if (last == TRUE) {
            /* -------------------------------------------------------------
             * Since GPP has the ownership of DMA, it does not waits for the
             * DSP to complete its operations.
             * -------------------------------------------------------------
             */
            ctrlPtr->isGppInit = (Uint16) EXIT_VALUE ;
            /* -------------------------------------------------------------
             * Update the control structure.
             * -------------------------------------------------------------
             */
            ctrlPtr->numBufs = 0 ;
        }
    }

    TRC_1LEAVE ("DMAPOOL_close", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DMAPOOL_alloc
 *
 *  @desc   This function allocates a free buffer from the specified buffer pool
 *          and returns it to the user.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_alloc (IN  ProcessorId       dspId,
               IN  Uint32            poolId,
               IN  Void *            object,
               OUT Pvoid *           bufPtr,
               IN  Uint32            size)
{
    DSP_STATUS       status = DSP_SOK ;
    Uint32           i      = 0       ;
    Uint32           poolIndx = 0     ;
    Bool             found  = FALSE   ;
    DMAPOOL_Object * dmaState         ;
    DMAPOOL_Ctrl *   ctrlPtr          ;
    DmaBufObj *      dmaBufObj        ;    /* Holds the buffer handles */
    MPCS_ShObj *     mpcsObj          ;
    DmaBufHeader *   bufHeader        ;
    DspDmaArgs       dmaArgs          ;

    TRC_5ENTER ("DMAPOOL_alloc", dspId, poolId, object, bufPtr, size) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId != POOL_INVALIDID) ;
    DBC_Require (object != NULL) ;
    DBC_Require (bufPtr != NULL) ;
    DBC_Require (size   != 0) ;

    dmaState  = (DMAPOOL_Object *) object ;

    /* Check if pool is opened. */
    if (dmaState->refCount == 0) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }
    else {
        ctrlPtr = dmaState->ctrlPtr ;
        mpcsObj = (MPCS_ShObj *) &(dmaState->dmaShmObj->mpcsObj) ;
        dmaBufObj = (DmaBufObj *) dmaState->ctrlPtr->dmaBufObjs ;

        /* ---------------------------------------------------------------------
         * Convert the size to DSP addressable size.
         * ---------------------------------------------------------------------
         */
        size  = DSPLINK_ALIGN (size, BUS_WIDTH) ;
        size /= dmaState->dspMaduSize ;

        for (i = 0 ; (i < ctrlPtr->numBufs) && !found ; i++) {
            if (dmaState->dmaShmObj->exactMatchReq == FALSE) {
                /* Look for the nearest match for the requested size */
                if (SWAP_LONG (dmaBufObj [i].size, dmaState->wordSwap) >= size){
                    /* Found a match for required size. */
                    found = TRUE ;
                    poolIndx = i ;
                }
             }
             else {
                /* Look for the exact match for the requested size */
                if (SWAP_LONG (dmaBufObj [i].size, dmaState->wordSwap) == size){
                    /* Found a match for required size. */
                    found = TRUE ;
                    poolIndx = i ;
                }
            }
        }

        if (found == TRUE) {
            /* -----------------------------------------------------------------
             * Enter the critical section.
             * -----------------------------------------------------------------
             */
            LDRV_MPCS_enter (mpcsObj) ;

            if (dmaBufObj [poolIndx].freeBuffers != 0) {
                /* DMA the buffer area from DSP */
                dmaArgs.dspId   = dspId ;
                dmaArgs.srcAddr =(  dmaBufObj [poolIndx].bufDspPhysAddress
                                  +  dmaBufObj [poolIndx].nextFree) ;
                dmaArgs.dstAddr =(  dmaBufObj [poolIndx].bufGppPhysAddress
                                  +  dmaBufObj [poolIndx].nextFree) ;
                dmaArgs.size    = dmaBufObj [poolIndx].size ;
                status = DSP_control (dspId,
                                      DSP_DmaCtrlCmd_DspToGpp,
                                      &dmaArgs) ;

                /* Get the pointer to the buffer */
                bufHeader = (DmaBufHeader *)
                                         (  dmaBufObj [poolIndx].nextFree
                                          + dmaBufObj [poolIndx].startAddress) ;
                /* Update the next free pointer */
                dmaBufObj [poolIndx].nextFree =
                                            SWAP_LONG ((Uint32) bufHeader->next,
                                            dmaState->wordSwap) ;
                /* Decrement the free buffer count */
                dmaBufObj [poolIndx].freeBuffers-- ;
                *bufPtr = bufHeader ;
            }
            else {
                status = DSP_EMEMORY ;
                SET_FAILURE_REASON ;
            }

            /* -----------------------------------------------------------------
             * leave the critical section.
             * -----------------------------------------------------------------
             */
            LDRV_MPCS_leave (mpcsObj) ;
        }
        else {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
    }

    DBC_Ensure (  (DSP_SUCCEEDED (status) && *bufPtr != NULL)
                || DSP_FAILED (status)) ;
    DBC_Ensure (   (DSP_FAILED (status) && *bufPtr == NULL)
                || DSP_SUCCEEDED (status)) ;

    TRC_1LEAVE ("DMAPOOL_alloc", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DMAPOOL_free
 *
 *  @desc   This function frees the buffer specified by the user, and returns it
 *          to the buffer pool.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_free (IN  ProcessorId       dspId,
              IN  Uint32            poolId,
              IN  Void *            object,
              IN  Void *            buf,
              IN  Uint32            size)
{
    DSP_STATUS       status = DSP_SOK ;
    Uint32           i      = 0       ;
    Bool             found  = FALSE   ;
    Uint32           poolIndx = 0     ;
    Uint32           bufOffset  = 0   ;
    DMAPOOL_Object * dmaState         ;
    DMAPOOL_Ctrl *   ctrlPtr          ;
    DmaBufObj *      dmaBufObj        ;   /* Holds the buffer handles */
    MPCS_ShObj *     mpcsObj          ;
    DspDmaArgs       dmaArgs          ;

    TRC_5ENTER ("DMAPOOL_free", dspId, poolId, object, buf, size) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId != POOL_INVALIDID) ;
    DBC_Require (object != NULL) ;
    DBC_Require (buf    != NULL) ;
    DBC_Require (size   != 0) ;

    dmaState  = (DMAPOOL_Object *) object ;

    /* Check if pool is opened. */
    if (dmaState->refCount == 0) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }
    else {
        mpcsObj = (MPCS_ShObj *) &(dmaState->dmaShmObj->mpcsObj) ;
        ctrlPtr = dmaState->ctrlPtr ;
        dmaBufObj = (DmaBufObj *) dmaState->ctrlPtr->dmaBufObjs ;

        /* ---------------------------------------------------------------------
         * Convert the size to DSP addressable size.
         * ---------------------------------------------------------------------
         */
        size  = DSPLINK_ALIGN (size, BUS_WIDTH) ;
        size /= dmaState->dspMaduSize ;

        for (i = 0 ; (i < ctrlPtr->numBufs) && !found ; i++) {
            if (dmaState->dmaShmObj->exactMatchReq == FALSE) {
                /* Look for the nearest match for the requested size */
                if (SWAP_LONG (dmaBufObj [i].size, dmaState->wordSwap) >= size) {
                    /* Found a match for required size. */
                    found = TRUE ;
                    size  = dmaBufObj [i].size ;
                    poolIndx = i ;
                }
             }
             else {
                /* Look for the exact match for the requested size */
                if (SWAP_LONG (dmaBufObj [i].size, dmaState->wordSwap) == size) {
                    /* Found a match for required size. */
                    found = TRUE ;
                    poolIndx = i ;
                }
            }
        }

        if (found == TRUE) {
            /* -----------------------------------------------------------------
             * Enter the critical section.
             * -----------------------------------------------------------------
             */
            LDRV_MPCS_enter (mpcsObj) ;

            bufOffset = (Uint32) buf - dmaBufObj [poolIndx].startAddress ;
            if ((  bufOffset
                 % MADU_TO_BYTE (size, dmaBufObj [poolIndx].dspMaduSize)) == 0){
                /* Create the link to next buffer */
                ((DmaBufHeader *) buf)->next =
                              ((DmaBufHeader *) dmaBufObj [poolIndx].nextFree) ;

#if defined (DDSP_PROFILE)
                ((DmaBufHeader *) buf)->usedOnce = DMABUF_ALLOCSTAMP ;
#endif /* if defined (DDSP_PROFILE) */

                /* DMA the buffer to DSP */
                dmaArgs.dspId   = dspId ;
                dmaArgs.dstAddr =(  dmaBufObj [poolIndx].bufDspPhysAddress
                                  + bufOffset) ;
                dmaArgs.srcAddr =(  dmaBufObj [poolIndx].bufGppPhysAddress
                                  + bufOffset) ;
                dmaArgs.size    = dmaBufObj [poolIndx].size ;
                status = DSP_control (dspId,
                                      DSP_DmaCtrlCmd_GppToDsp,
                                      &dmaArgs) ;

                dmaBufObj [poolIndx].nextFree = bufOffset ;
                dmaBufObj [poolIndx].freeBuffers++ ;
            }
            else {
                /* invalid buffer being freed */
                status = DSP_EMEMORY ;
                SET_FAILURE_REASON ;
            }

            /* -----------------------------------------------------------------
             * leave the critical section.
             * -----------------------------------------------------------------
             */
            LDRV_MPCS_leave (mpcsObj) ;
        }
        else {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("DMAPOOL_free", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DMAPOOL_writeback
 *
 *  @desc   This function writes the content of GPP buffer to DSP buffer (with
 *          offset in sync).
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_writeback (IN  ProcessorId       dspId,
                   IN  Uint32            poolId,
                   IN  Pvoid             object,
                   IN  Pvoid             buf,
                   IN  Uint32            size)
{
    DSP_STATUS       status = DSP_SOK ;
    Uint32           i      = 0 ;
    Uint32           poolIndx = 0     ;
    Bool             found  = FALSE ;
    DMAPOOL_Object * dmaState         ;
    DMAPOOL_Ctrl *   ctrlPtr          ;
    DmaBufObj *      dmaBufObj        ;    /* Holds the buffer handles */
    MPCS_ShObj *     mpcsObj          ;
    DspDmaArgs       dmaArgs          ;
    Uint32           srcAddr ;
    Uint32           dstAddr ;

    TRC_5ENTER ("DMAPOOL_writeback", dspId, poolId, object, buf, size) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId  != POOL_INVALIDID) ;
    DBC_Require (object  != NULL) ;
    DBC_Require (buf     != NULL) ;
    DBC_Require (size    >  0) ;

    dmaState  = (DMAPOOL_Object *) object ;

    /* Check if pool is opened. */
    if (dmaState->refCount == 0) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }
    else {
        ctrlPtr = dmaState->ctrlPtr ;
        mpcsObj = (MPCS_ShObj *) &(dmaState->dmaShmObj->mpcsObj) ;
        dmaBufObj = (DmaBufObj *) dmaState->ctrlPtr->dmaBufObjs ;

        /* ---------------------------------------------------------------------
         * Convert the size to DSP addressable size.
         * ---------------------------------------------------------------------
         */
        size  = DSPLINK_ALIGN (size, BUS_WIDTH) ;
        size /= dmaState->dspMaduSize ;

        for (i = 0 ; (i < ctrlPtr->numBufs) && !found ; i++) {
            if (   (((Uint32) buf) >= dmaBufObj [i].startAddress)
                && (((Uint32) buf) <  (  dmaBufObj [i].startAddress
                                       + (  dmaBufObj [i].size
                                          * dmaBufObj [i].totalBuffers)))) {
                found = TRUE ;
                poolIndx = i ;
            }
        }

        if (found == TRUE) {
            /* -----------------------------------------------------------------
             * Enter the critical section.
             * -----------------------------------------------------------------
             */
            LDRV_MPCS_enter (mpcsObj) ;

            /* -----------------------------------------------------------------
             * DMA the buffer.
             * -----------------------------------------------------------------
             */
            srcAddr =   dmaBufObj [poolIndx].bufGppPhysAddress
                     + ((Uint32) buf - dmaBufObj [poolIndx].startAddress) ;
            dstAddr =   dmaBufObj [poolIndx].bufDspPhysAddress
                     + ((Uint32) buf - dmaBufObj [poolIndx].startAddress) ;
            dmaArgs.dspId   = dspId ;
            dmaArgs.srcAddr = srcAddr ;
            dmaArgs.dstAddr = dstAddr ;
            dmaArgs.size    = size ;
            status = DSP_control (dspId,
                                  DSP_DmaCtrlCmd_GppToDsp,
                                  &dmaArgs) ;

            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            /* -----------------------------------------------------------------
             * leave the critical section.
             * -----------------------------------------------------------------
             */
            LDRV_MPCS_leave (mpcsObj) ;
        }
        else {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("DMAPOOL_writeback", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DMAPOOL_invalidate
 *
 *  @desc   This function read the content of DSP buffer into GPP buffer (with
 *          offset in sync).
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_invalidate (IN  ProcessorId       dspId,
                    IN  Uint32            poolId,
                    IN  Pvoid             object,
                    IN  Pvoid             buf,
                    IN  Uint32            size)
{
    DSP_STATUS       status = DSP_SOK ;
    Uint32           i      = 0 ;
    Uint32           poolIndx = 0     ;
    Bool             found  = FALSE ;
    DMAPOOL_Object * dmaState         ;
    DMAPOOL_Ctrl *   ctrlPtr          ;
    DmaBufObj *      dmaBufObj        ;    /* Holds the buffer handles */
    MPCS_ShObj *     mpcsObj          ;
    DspDmaArgs       dmaArgs          ;
    Uint32           srcAddr ;
    Uint32           dstAddr ;

    TRC_5ENTER ("DMAPOOL_invalidate", dspId, poolId, object, buf, size) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId  != POOL_INVALIDID) ;
    DBC_Require (object  != NULL) ;
    DBC_Require (buf     != NULL) ;
    DBC_Require (size    >  0) ;

    dmaState  = (DMAPOOL_Object *) object ;

    /* Check if pool is opened. */
    if (dmaState->refCount == 0) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }
    else {
        ctrlPtr = dmaState->ctrlPtr ;
        mpcsObj = (MPCS_ShObj *) &(dmaState->dmaShmObj->mpcsObj) ;
        dmaBufObj = (DmaBufObj *) dmaState->ctrlPtr->dmaBufObjs ;

        /* ---------------------------------------------------------------------
         * Convert the size to DSP addressable size.
         * ---------------------------------------------------------------------
         */
        size  = DSPLINK_ALIGN (size, BUS_WIDTH) ;
        size /= dmaState->dspMaduSize ;

        for (i = 0 ; (i < ctrlPtr->numBufs) && !found ; i++) {
            if (   (((Uint32) buf) >= dmaBufObj [i].startAddress)
                && (((Uint32) buf) <  (  dmaBufObj [i].startAddress
                                       + (  dmaBufObj [i].size
                                          * dmaBufObj [i].totalBuffers)))) {
                found = TRUE ;
                poolIndx = i ;
            }
        }

        if (found == TRUE) {
            /* -----------------------------------------------------------------
             * Enter the critical section.
             * -----------------------------------------------------------------
             */
            LDRV_MPCS_enter (mpcsObj) ;

            /* -----------------------------------------------------------------
             * DMA the buffer.
             * -----------------------------------------------------------------
             */
            dstAddr =   dmaBufObj [poolIndx].bufGppPhysAddress
                     + ((Uint32) buf - dmaBufObj [poolIndx].startAddress) ;
            srcAddr =   dmaBufObj [poolIndx].bufDspPhysAddress
                     + ((Uint32) buf - dmaBufObj [poolIndx].startAddress) ;
            dmaArgs.dspId   = dspId ;
            dmaArgs.srcAddr = srcAddr ;
            dmaArgs.dstAddr = dstAddr ;
            dmaArgs.size    = size ;
            status = DSP_control (dspId,
                                  DSP_DmaCtrlCmd_DspToGpp,
                                  &dmaArgs) ;

            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            /* -----------------------------------------------------------------
             * leave the critical section.
             * -----------------------------------------------------------------
             */
            LDRV_MPCS_leave (mpcsObj) ;
        }
        else {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("DMAPOOL_invalidate", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DMAPOOL_xltBuf
 *
 *  @desc   This function translate buffers from different address spaces.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_xltBuf (IN  ProcessorId      dspId,
                IN  Uint32           poolId,
                IN  Void *           object,
                IN  Pvoid            buf,
                OUT Pvoid *          cBuf,
                IN  POOL_AddrXltFlag xltFlag)
{
    DSP_STATUS       status = DSP_SOK ;
    DMAPOOL_Object * dmaState ;

    TRC_6ENTER ("DMAPOOL_xltBuf", dspId, poolId, object, buf, cBuf, xltFlag) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId != POOL_INVALIDID) ;
    DBC_Require (object != NULL) ;
    DBC_Require (buf    != NULL) ;

    dmaState  = (DMAPOOL_Object *) object ;

    /* Check if pool is opened. */
    if (dmaState->refCount == 0) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }
    else {
        if (xltFlag == KNL_TO_DSP) {
            *cBuf =  (Pvoid) (  (dmaState->bufDspMemAddr)
                              + ((Uint32) buf - dmaState->bufGppMemAddr)) ;
        }

        if (xltFlag == DSP_TO_KNL) {
            *cBuf =  (Pvoid) (  (dmaState->bufGppMemAddr)
                              + ((Uint32) buf - dmaState->bufDspMemAddr)) ;
        }
    }

    TRC_1LEAVE ("DMAPOOL_xltBuf", status) ;

    return status ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   DMAPOOL_instrument
 *
 *  @desc   This function gets the instrument data for the DMA.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DMAPOOL_instrument (IN  ProcessorId     dspId,
                    IN  Uint32          poolId,
                    IN  Void *          object,
                    IN  Pvoid           params)
{
    DSP_STATUS       status = DSP_SOK ;
    Uint32           i      = 0 ;
    DMAPOOL_Object * dmaState ;
    DMAPOOL_Ctrl *   ctrlPtr ;
    DMAPOOL_Stats *  dmaStats ;
    DmaBufObj *       dmaBufObj ;    /* Holds the buffer handles */
    MPCS_ShObj *     mpcsObj ;
    Uint32           mpBufAddr ;

    TRC_4ENTER ("DMAPOOL_instrument", dspId, poolId, object, params) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId != POOL_INVALIDID) ;
    DBC_Require (object != NULL) ;
    DBC_Require (params != NULL) ;

    dmaStats = (DMAPOOL_Stats *) params ;
    dmaState = (DMAPOOL_Object *) object ;

    /* Check if pool is opened. */
    if (dmaState->refCount == 0) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }
    else {
        ctrlPtr  = dmaState->ctrlPtr ;
        mpcsObj  = (MPCS_ShObj *) &(dmaState->dmaShmObj->mpcsObj) ;
        dmaBufObj = dmaState->ctrlPtr->dmaBufObjs ;

        for (i = 0 ; i < ctrlPtr->numBufs ; i++) {
            /* -----------------------------------------------------------------
             * Enter the critical section.
             * -----------------------------------------------------------------
             */
            LDRV_MPCS_enter (mpcsObj) ;

            DMABUF_GetStats (&dmaBufObj [i], &dmaStats->mpBufStats [i]) ;

            /* -----------------------------------------------------------------
             * leave the critical section.
             * -----------------------------------------------------------------
             */
            LDRV_MPCS_leave (mpcsObj) ;
        }

        /* ---------------------------------------------------------------------
         * Update the buffer pool count.
         * ---------------------------------------------------------------------
         */
        dmaStats->dmaBufObjCount = ctrlPtr->numBufs ;
    }

    TRC_1LEAVE ("DMAPOOL_instrument", status) ;

    return status ;
}
#endif /* if defined (DDSP_PROFILE) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
