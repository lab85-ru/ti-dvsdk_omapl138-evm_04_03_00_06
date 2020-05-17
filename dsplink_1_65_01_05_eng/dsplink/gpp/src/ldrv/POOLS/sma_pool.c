/** ============================================================================
 *  @file   sma_pool.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/POOLS/
 *
 *  @desc   Implementation of SMA - Shared Memory Allocator.
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

/*  ----------------------------------- LDRV Headers                */
#include <ldrv_pool.h>
#include <sma_pool.h>
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
#define COMPONENT_ID    ID_LDRV_POOL_SMA

/** ============================================================================
 *  @name   SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON TRC_SetReason (status, FID_C_LDRV_POOL_SMA, __LINE__)
#else
#define SET_FAILURE_REASON  { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   INIT_VALUE
 *
 *  @desc   Value to indicate the initialization of SMA module.
 *  ============================================================================
 */
#define INIT_VALUE  0xDEAF

/** ============================================================================
 *  @name   EXIT_VALUE
 *
 *  @desc   Value to indicate the finalization of SMA module.
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
 *  @name   SMAPOOL_Object
 *
 *  @desc   This structure defines the state of the SMA.
 *
 *  @field  dspId
 *              ID of the processor with which the memory manager is shared.
 *  @field  size
 *              Length of the shared memory region.
 *  @field  gppAddr
 *              Virtual address of the shared memory to be used by SMA,
 *              in GPP address space.
 *  @field  dspAddr
 *              Address of the shared memory to be used by SMA, in DSP address
 *              space.
 *  @field  physAddr
 *              physical Address of the shared memory to be used by SMA.
 *  @field  dspMaduSize
 *              DSP elementary addressable unit size.
 *  @field  smaShmObj
 *              Pointer to the SMA shared memory object.
 *  @field  ctrlPtr
 *              Pointer to the SMA shared memory control structure.
 *  @field  wordSwap
 *              Indicates whether word-swap is enabled for the DSP MMU.
 *  @field  memEntry
 *              ID of the memory entry for the SMAPOOL shared memory region.
 *  @field  refCount
 *              Reference count for the pool.
 *  @field  lock
 *              Lock for using POOL resources.
 *  ============================================================================
 */
typedef struct SMAPOOL_Object_tag {
    ProcessorId      dspId         ;
    Uint32           size          ;
    Uint32           gppAddr       ;
    Uint32           dspAddr       ;
    Uint32           physAddr      ;
    Uint16           dspMaduSize   ;
    SMAPOOL_ShmObj * smaShmObj     ;
    SMAPOOL_Ctrl *   ctrlPtr       ;
    Bool             wordSwap      ;
    Uint32           memEntry      ;
    Uint32           refCount      ;
    Uint32           bufMemSecId   ;
    Uint32           bufPhysMemAddr ;
    Uint32           bufGppMemAddr ;
    Uint32           bufDspMemAddr ;
    Uint32           bufPoolSize   ;
    Pvoid            lock          ;
} SMAPOOL_Object ;



/** ============================================================================
 *  @name   SMAPOOL_Interface
 *
 *  @desc   SMA Pool interface table.
 *  ============================================================================
 */
POOL_Interface SMAPOOL_Interface = {
    &SMAPOOL_open,
    &SMAPOOL_close,
    &SMAPOOL_alloc,
    &SMAPOOL_free,
    NULL,
    NULL,
    NULL,
    &SMAPOOL_xltBuf
} ;


/** ============================================================================
 *  @func   SMAPOOL_init
 *
 *  @desc   This function initializes the SMA POOL component.
 *
 *  @modif  object.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SMAPOOL_init (IN  ProcessorId dspId,
              IN  Uint32      poolId,
              OUT Uint32 *    shDspAddr,
              OUT Pvoid *     object)
{
    DSP_STATUS          status   = DSP_SOK ;
    SMAPOOL_Object *    smaState = NULL ;
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_LinkDrv *   linkDrv ;
    LINKCFG_Pool *      poolObject ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_4ENTER ("SMAPOOL_init", dspId, poolId, shDspAddr, object) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId    != POOL_INVALIDID) ;
    DBC_Require (shDspAddr != NULL) ;
    DBC_Assert  (object    != NULL) ;

    dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj   = dspCfg->dspObject ;
    linkDrv    = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
    poolObject = &(dspCfg->poolTables [linkDrv->poolTableId][poolId]) ;

    /*  ------------------------------------------------------------------------
     *  Allocate memory for the SMAPOOL state object.s
     *  ------------------------------------------------------------------------
     */
    status = MEM_Calloc (object, sizeof (SMAPOOL_Object), MEM_DEFAULT) ;
    if (DSP_SUCCEEDED (status)) {
        smaState = (SMAPOOL_Object *) *object ;

        /* Initialize the state object fields. */
        smaState->dspId       = dspId ;
        smaState->size        = sizeof (SMAPOOL_ShmObj) ;
        smaState->dspMaduSize = dspObj->maduSize ;
        smaState->wordSwap    = dspObj->wordSwap ;
        smaState->memEntry    = poolObject->memEntry ;
        smaState->bufMemSecId = poolObject->poolMemEntry ;
        /* Create the lock */
        smaState->lock = SYNC_SpinLockCreate () ;

        /*  --------------------------------------------------------------------
         *  Allocate shared memory required for the SMAPOOL.
         *  --------------------------------------------------------------------
         */
        status = LDRV_SMM_alloc (dspId,
                                 smaState->memEntry,
                                 &(smaState->physAddr),
                                 &(smaState->dspAddr),
                                 &(smaState->gppAddr),
                                 smaState->size) ;
        if (DSP_SUCCEEDED (status)) {
            *shDspAddr = smaState->dspAddr ;
            TRC_1PRINT (TRC_LEVEL4,
                        "SMAPOOL shared memory GPP address      [0x%x]\n",
                        smaState->gppAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "SMAPOOL shared memory DSP address      [0x%x]\n",
                        smaState->dspAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "SMAPOOL shared memory physical address [0x%x]\n",
                        smaState->physAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "SMAPOOL shared memory size             [0x%x]\n",
                        smaState->size) ;
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        smaState->bufPoolSize = poolObject->poolSize ;
        status = LDRV_SMM_alloc (dspId,
                                 smaState->bufMemSecId,
                                 &(smaState->bufPhysMemAddr),
                                 &(smaState->bufDspMemAddr),
                                 &(smaState->bufGppMemAddr),
                                 smaState->bufPoolSize) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        /* ---------------------------------------------------------------------
         * check whether shared memory is mapped correctly.
         * ---------------------------------------------------------------------
         */
        if (smaState->gppAddr == 0) {
            status = DSP_EMEMORY ;
            SET_FAILURE_REASON ;
        }
        else {
            /* -----------------------------------------------------------------
             * Create the SMAPOOL_Ctrl structure in the shared memory block.
             * -----------------------------------------------------------------
             */
            smaState->smaShmObj = (SMAPOOL_ShmObj *) (smaState->gppAddr) ;
            smaState->ctrlPtr   = &(smaState->smaShmObj->ctrlPtr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "SMAPOOL Ctrl GPP address               [0x%x]\n",
                        smaState->ctrlPtr) ;

            /* -----------------------------------------------------------------
             * Create the MPCS Component.
             * -----------------------------------------------------------------
             */
            status = LDRV_MPCS_create (dspId, &(smaState->smaShmObj->mpcsObj)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    if (DSP_FAILED (status)) {
        SMAPOOL_exit (dspId, poolId, *object) ;
    }

    TRC_1LEAVE ("SMAPOOL_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SMAPOOL_exit
 *
 *  @desc   This function finalizes the SMA POOL component.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SMAPOOL_exit (IN  ProcessorId dspId,
              IN  Uint32      poolId,
              IN  Void *      object)
{
    DSP_STATUS        status    = DSP_SOK ;
    DSP_STATUS        tmpStatus = DSP_SOK ;
    SMAPOOL_Object *  smaState ;

    TRC_3ENTER ("SMAPOOL_exit", dspId, poolId, object) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId    != POOL_INVALIDID) ;

    smaState  = (SMAPOOL_Object *) object ;
    if (smaState != NULL) {
        if (smaState->smaShmObj != NULL) {
            /* -----------------------------------------------------------------
             * Finalize the MPCS object.
             * -----------------------------------------------------------------
             */
            status = LDRV_MPCS_delete (dspId, &(smaState->smaShmObj->mpcsObj)) ;
        }

        if (smaState->bufGppMemAddr != (Uint32) NULL) {
            /*  ----------------------------------------------------------------
             *  Free shared memory allocated for the SMAPOOL component.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_SMM_free (dspId,
                                       smaState->bufMemSecId,
                                       smaState->bufPoolSize) ;
            if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        if (smaState->gppAddr != (Uint32) NULL) {
            /*  ----------------------------------------------------------------
             *  Free shared memory allocated for the SMAPOOL component.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_SMM_free (dspId,
                                       smaState->memEntry,
                                       smaState->size) ;
            if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        /* Delete IPS lock */
        SYNC_SpinLockDelete (smaState->lock) ;

        tmpStatus = FREE_PTR (smaState) ;
        if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("SMAPOOL_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SMAPOOL_open
 *
 *  @desc   This function will create buffer handles and buffer pools.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SMAPOOL_open (IN  ProcessorId       dspId,
              IN  Uint32            poolId,
              IN  Void *            object,
              IN  POOL_OpenParams * poolOpenParams)
{
    DSP_STATUS       status   = DSP_SOK ;
    Uint32           i        = 0 ;
    Uint32           j        = 0 ;
    Uint16           count    = 0 ;
    Bool             first    = FALSE ;
    SMAPOOL_Ctrl *   ctrlPtr  = NULL ;
    SMAPOOL_Attrs *  smaAttrs ;
    SMAPOOL_Object * smaState ;
    Uint32           bufDspHeader ; /* Holds the start address of buffers in DSP
                                     * address space.
                                     */
    SmaBufObj *      smaBufObj ;    /* Holds the buffer handles */
    Uint8 *          bufHeader ;    /* Holds the buffer handles */
    Uint32 *         bufSize ;
    Uint32 *         numBufs ;
    Uint32           temp ;
    Uint32           irqFlags ;

    TRC_4ENTER ("SMAPOOL_open", dspId, poolId, object, poolOpenParams) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId         != POOL_INVALIDID) ;
    DBC_Require (object         != NULL) ;
    DBC_Require (poolOpenParams != NULL) ;

    smaState = (SMAPOOL_Object *) object ;

    irqFlags = SYNC_SpinLockStartEx (smaState->lock) ;
    if (smaState->refCount == 0) {
        first = TRUE ;
    }
    else {
        status = DSP_SALREADYOPENED ;
    }
    smaState->refCount++ ;

    SYNC_SpinLockEndEx (smaState->lock, irqFlags) ;

    if (first == TRUE) {
        smaAttrs = (SMAPOOL_Attrs *) poolOpenParams->params ;
        DBC_Require (smaAttrs != NULL) ;
        /* Attributes are required for the first call to SMAPOOL_open () */
        if (smaAttrs == NULL) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        else {
            DBC_Require (smaAttrs->numBufPools > 0) ;
            DBC_Require (smaAttrs->numBufPools <= MAX_SMABUFENTRIES) ;

            if (smaAttrs->numBufPools == 0) {
                status = DSP_EINVALIDARG ;
                SET_FAILURE_REASON ;
            }
            else {
                /* Check for pool sizes being cache line aligned */
                for (i = 0 ; i < smaAttrs->numBufPools ; i++) {
                    if  (smaAttrs->numBuffers [i] != 0u) {
                        for (j = 0 ; j < smaAttrs->numBuffers [i]; j++) {
                            if ((smaAttrs->bufSizes [i] % CACHE_L2_LINESIZE) != 0u) {
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
                ctrlPtr = smaState->ctrlPtr ;
                smaState->smaShmObj->exactMatchReq = (Uint16)
                                                     smaAttrs->exactMatchReq ;

                /* -------------------------------------------------------------
                 * Convert the buffer handle from DSP to GPP address space.
                 * -------------------------------------------------------------
                 */
                smaBufObj = (SmaBufObj *) smaState->ctrlPtr->smaBufObjs ;

                /* -------------------------------------------------------------
                 * Buffer header are created at a distance
                 * "sizeof(SmaBufObj) * MAX_SMABUFENTRIES", to provide a
                 * provision for adding new buffer pools later on.
                 * -------------------------------------------------------------
                 */
                bufHeader    =  (Uint8 *) smaState->bufGppMemAddr ;
                bufDspHeader =  smaState->bufDspMemAddr ;
                TRC_1PRINT (TRC_LEVEL4,
                            "SMAPOOL buffers start GPP address      [0x%x]\n",
                            bufHeader) ;
                TRC_1PRINT (TRC_LEVEL4,
                            "SMAPOOL buffers start DSP address      [0x%x]\n",
                            bufDspHeader) ;

                /* -------------------------------------------------------------
                 * Check for duplicate size entries in SMA attributes.
                 * -------------------------------------------------------------
                 */
                status = MEM_Alloc ((Void **) &bufSize,
                                    (smaAttrs->numBufPools * sizeof (Uint32)),
                                    MEM_DEFAULT) ;
                if (DSP_SUCCEEDED (status)) {
                    status = MEM_Calloc ((Void **) &numBufs,
                                        (smaAttrs->numBufPools * sizeof (Uint32)),
                                        MEM_DEFAULT) ;
                    if (DSP_SUCCEEDED (status)) {
                        for (i = 0 ; i < smaAttrs->numBufPools ; i++) {
                            if (numBufs [i] != (Uint32) -1) {
                                bufSize [count] = DSPLINK_ALIGN (
                                                         smaAttrs->bufSizes [i],
                                                         BUS_WIDTH) ;
                                DBC_Assert (bufSize [count] % BUS_WIDTH == 0) ;
                                numBufs [count] = smaAttrs->numBuffers [i] ;
                                for (j = i + 1 ; j < smaAttrs->numBufPools ; j++) {
                                    if (    bufSize [count]
                                        ==  DSPLINK_ALIGN (
                                                        smaAttrs->bufSizes [j],
                                                        BUS_WIDTH)) {
                                        numBufs [count] +=
                                                      smaAttrs->numBuffers [j] ;
                                        numBufs [j] = (Uint32) -1 ;
                                    }
                                }
                                count++ ;
                            }
                        }
                    }
                    else {
                        irqFlags = SYNC_SpinLockStartEx (smaState->lock) ;
                        smaState->refCount -- ;
                        SYNC_SpinLockEndEx (smaState->lock, irqFlags) ;
                        SET_FAILURE_REASON ;
                        FREE_PTR (bufSize) ;
                    }
                }
                else {
                    irqFlags = SYNC_SpinLockStartEx (smaState->lock) ;
                    smaState->refCount -- ;
                    SYNC_SpinLockEndEx (smaState->lock, irqFlags) ;
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

                    for (i = 0 ; (i < count) && DSP_SUCCEEDED (status) ; i++) {
                         if (   ((numBufs [i] * bufSize [i]) + (Uint32) bufHeader)
                             <= (smaState->bufGppMemAddr + smaState->bufPoolSize)) {
                            smaBufObj[i].dspMaduSize = smaState->dspMaduSize ;
                            smaBufObj[i].wordSwap    = smaState->wordSwap ;
                            smaBufObj [i].size = BYTE_TO_MADU (bufSize [i],
                                                            smaBufObj [i].dspMaduSize) ;
                            smaBufObj [i].size = SWAP_LONG (smaBufObj [i].size,
                                                            smaBufObj [i].wordSwap) ;
                            smaBufObj [i].totalBuffers   = numBufs [i] ;
                            smaBufObj [i].freeBuffers    = numBufs [i] ;
                            smaBufObj [i].startAddress   = (Uint32) bufHeader ;
                            smaBufObj [i].bufDspAddress  = bufDspHeader ;
                            smaBufObj [i].nextFree       = 0 ;

                            bufHeader = (Uint8 *) (smaBufObj [i].startAddress) ;
                            /* -------------------------------------------------
                             * Create the buffer pools.
                             * -------------------------------------------------
                             */
                            for (j = 0 ; j < numBufs [i] ; j++) {
                                ((SmaBufHeader *) bufHeader)->next = (SmaBufHeader *)
                                                SWAP_LONG (smaBufObj [i].size * (j + 1),
                                                           smaBufObj [i].wordSwap) ;
                                bufHeader += bufSize [i] ;
                            }

                            bufDspHeader =   bufDspHeader
                                           + BYTE_TO_MADU ((bufSize [i] * numBufs [i]),
                                                           smaState->dspMaduSize) ;
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
                irqFlags = SYNC_SpinLockStartEx (smaState->lock) ;
                smaState->refCount -- ;
                SYNC_SpinLockEndEx (smaState->lock, irqFlags) ;
                SET_FAILURE_REASON ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            /* -----------------------------------------------------------------
             * Set the GPP Side SMA flag to initialize condition.
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
        poolOpenParams->physAddr = smaState->bufPhysMemAddr ;
        poolOpenParams->virtAddr = smaState->bufGppMemAddr  ;
        poolOpenParams->dspAddr  = smaState->bufDspMemAddr  ;
        poolOpenParams->size     = smaState->bufPoolSize    ;
    }

    TRC_1LEAVE ("SMAPOOL_open", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SMAPOOL_close
 *
 *  @desc   This function closes the buffer pool specified by the user.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SMAPOOL_close (IN  ProcessorId       dspId,
               IN  Uint32            poolId,
               IN  Void *            object)
{
    DSP_STATUS       status = DSP_SOK ;
    Bool             last   = FALSE ;
    SMAPOOL_Object * smaState ;
    SMAPOOL_Ctrl *   ctrlPtr ;
    Uint32           irqFlags ;

    TRC_3ENTER ("SMAPOOL_close", dspId, poolId, object) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId         != POOL_INVALIDID) ;
    DBC_Require (object         != NULL) ;

    /* Check if pool has been opened */
    if (object != NULL) {
        smaState  = (SMAPOOL_Object *) object ;
        ctrlPtr   = smaState->ctrlPtr ;

        irqFlags = SYNC_SpinLockStartEx (smaState->lock) ;
        if (smaState->refCount == 0) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
        else {
            smaState->refCount-- ;
            if (smaState->refCount == 0) {
                last = TRUE ;
                status = DSP_SCLOSED ;
            }
        }
        SYNC_SpinLockEndEx (smaState->lock, irqFlags) ;

        /* Close the pool if its reference count has reached 0. */
        if (last == TRUE) {
            /* -------------------------------------------------------------
             * Since GPP has the ownership of SMA, it does not waits for the
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

    TRC_1LEAVE ("SMAPOOL_close", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SMAPOOL_alloc
 *
 *  @desc   This function allocates a free buffer from the specified buffer pool
 *          and returns it to the user.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SMAPOOL_alloc (IN  ProcessorId       dspId,
               IN  Uint32            poolId,
               IN  Void *            object,
               OUT Pvoid *           bufPtr,
               IN  Uint32            size)
{
    DSP_STATUS       status = DSP_SOK ;
    Uint32           i      = 0       ;
    Uint32           poolIndx = 0     ;
    Bool             found  = FALSE   ;
    SMAPOOL_Object * smaState         ;
    SMAPOOL_Ctrl *   ctrlPtr          ;
    SmaBufObj *      smaBufObj        ;    /* Holds the buffer handles */
    MPCS_ShObj *     mpcsObj          ;
    SmaBufHeader *   bufHeader        ;

    TRC_5ENTER ("SMAPOOL_alloc", dspId, poolId, object, bufPtr, size) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId != POOL_INVALIDID) ;
    DBC_Require (object != NULL) ;
    DBC_Require (bufPtr != NULL) ;
    DBC_Require (size   != 0) ;

    smaState  = (SMAPOOL_Object *) object ;

    /* Check if pool is opened. */
    if (smaState->refCount == 0) {
        status = DSP_EFAIL ;
        *bufPtr = NULL;
        SET_FAILURE_REASON ;
    }
    else {
        ctrlPtr = smaState->ctrlPtr ;
        mpcsObj = (MPCS_ShObj *) &(smaState->smaShmObj->mpcsObj) ;
        smaBufObj = (SmaBufObj *) smaState->ctrlPtr->smaBufObjs ;

        /* ---------------------------------------------------------------------
         * Convert the size to DSP addressable size.
         * ---------------------------------------------------------------------
         */
        size  = DSPLINK_ALIGN (size, BUS_WIDTH) ;
        size /= smaState->dspMaduSize ;

        for (i = 0 ; (i < ctrlPtr->numBufs) && (found == FALSE) ; i++) {
            if (smaState->smaShmObj->exactMatchReq == FALSE) {
                /* Look for the nearest match for the requested size */
                if (SWAP_LONG (smaBufObj [i].size, smaState->wordSwap) >= size){
                    /* Found a match for required size. */
                    found = TRUE ;
                    poolIndx = i ;
                }
             }
             else {
                /* Look for the exact match for the requested size */
                if (SWAP_LONG (smaBufObj [i].size, smaState->wordSwap) == size){
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

            if (smaBufObj [poolIndx].freeBuffers != 0) {
                bufHeader = (SmaBufHeader *)
                                         (  smaBufObj [poolIndx].nextFree
                                          + smaBufObj [poolIndx].startAddress) ;
                smaBufObj [poolIndx].nextFree = SWAP_LONG ((Uint32) bufHeader->next,
                                                    smaState->wordSwap) ;
                smaBufObj [poolIndx].freeBuffers-- ;
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

    DBC_Ensure (  (DSP_SUCCEEDED (status) && (*bufPtr != NULL))
                || DSP_FAILED (status)) ;
    DBC_Ensure (   (DSP_FAILED (status) && (*bufPtr == NULL))
                || DSP_SUCCEEDED (status)) ;

    TRC_1LEAVE ("SMAPOOL_alloc", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SMAPOOL_free
 *
 *  @desc   This function frees the buffer specified by the user, and returns it
 *          to the buffer pool.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SMAPOOL_free (IN  ProcessorId       dspId,
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
    SMAPOOL_Object * smaState         ;
    SMAPOOL_Ctrl *   ctrlPtr          ;
    SmaBufObj *      smaBufObj        ;   /* Holds the buffer handles */
    MPCS_ShObj *     mpcsObj          ;

    TRC_5ENTER ("SMAPOOL_free", dspId, poolId, object, buf, size) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId != POOL_INVALIDID) ;
    DBC_Require (object != NULL) ;
    DBC_Require (buf    != NULL) ;
    DBC_Require (size   != 0) ;

    smaState  = (SMAPOOL_Object *) object ;

    /* Check if pool is opened. */
    if (smaState->refCount == 0) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }
    else {
        mpcsObj = (MPCS_ShObj *) &(smaState->smaShmObj->mpcsObj) ;
        ctrlPtr = smaState->ctrlPtr ;
        smaBufObj = (SmaBufObj *) smaState->ctrlPtr->smaBufObjs ;

        /* ---------------------------------------------------------------------
         * Convert the size to DSP addressable size.
         * ---------------------------------------------------------------------
         */
        size  = DSPLINK_ALIGN (size, BUS_WIDTH) ;
        size /= smaState->dspMaduSize ;

        for (i = 0 ; (i < ctrlPtr->numBufs) && (found == FALSE) ; i++) {
            if (smaState->smaShmObj->exactMatchReq == FALSE) {
                /* Look for the nearest match for the requested size */
                if (SWAP_LONG (smaBufObj [i].size, smaState->wordSwap) >= size) {
                    /* Found a match for required size. */
                    found = TRUE ;
                    size  = smaBufObj [i].size ;
                    poolIndx = i ;
                }
             }
             else {
                /* Look for the exact match for the requested size */
                if (SWAP_LONG (smaBufObj [i].size, smaState->wordSwap) == size) {
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

            bufOffset = (Uint32) buf - smaBufObj [poolIndx].startAddress ;
            if ((  bufOffset
                 % MADU_TO_BYTE (size, smaBufObj [poolIndx].dspMaduSize)) == 0){
                ((SmaBufHeader *) buf)->next =
                              ((SmaBufHeader *) smaBufObj [poolIndx].nextFree) ;
#if defined (DDSP_PROFILE)
                ((SmaBufHeader *) buf)->usedOnce = SMABUF_ALLOCSTAMP ;
#endif /* if defined (DDSP_PROFILE) */
                smaBufObj [poolIndx].nextFree = bufOffset ;
                smaBufObj [poolIndx].freeBuffers++ ;
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

    TRC_1LEAVE ("SMAPOOL_free", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SMAPOOL_xltBuf
 *
 *  @desc   This function translate buffers from different address spaces.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SMAPOOL_xltBuf (IN  ProcessorId      dspId,
                IN  Uint32           poolId,
                IN  Void *           object,
                IN  Pvoid            buf,
                OUT Pvoid *          cBuf,
                IN  POOL_AddrXltFlag xltFlag)
{
    DSP_STATUS       status = DSP_SOK ;
    SMAPOOL_Object * smaState ;

    TRC_6ENTER ("SMAPOOL_xltBuf", dspId, poolId, object, buf, cBuf, xltFlag) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId != POOL_INVALIDID) ;
    DBC_Require (object != NULL) ;
    DBC_Require (buf    != NULL) ;

    smaState  = (SMAPOOL_Object *) object ;

    /* Check if pool is opened. */
    if (smaState->refCount == 0) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }
    else {
        if (xltFlag == KNL_TO_DSP) {
            *cBuf =  (Pvoid) (  (smaState->bufDspMemAddr)
                              + ((Uint32) buf - smaState->bufGppMemAddr)) ;
        }

        if (xltFlag == DSP_TO_KNL) {
            *cBuf =  (Pvoid) (  (smaState->bufGppMemAddr)
                              + ((Uint32) buf - smaState->bufDspMemAddr)) ;
        }
    }

    TRC_1LEAVE ("SMAPOOL_xltBuf", status) ;

    return status ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   SMAPOOL_instrument
 *
 *  @desc   This function gets the instrument data for the SMA.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SMAPOOL_instrument (IN  ProcessorId     dspId,
                    IN  Uint32          poolId,
                    IN  Void *          object,
                    IN  Pvoid           params)
{
    DSP_STATUS       status = DSP_SOK ;
    Uint32           i      = 0 ;
    SMAPOOL_Object * smaState ;
    SMAPOOL_Ctrl *   ctrlPtr ;
    SMAPOOL_Stats *  smaStats ;
    SmaBufObj *       smaBufObj ;    /* Holds the buffer handles */
    MPCS_ShObj *     mpcsObj ;
    Uint32           mpBufAddr ;

    TRC_4ENTER ("SMAPOOL_instrument", dspId, poolId, object, params) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId != POOL_INVALIDID) ;
    DBC_Require (object != NULL) ;
    DBC_Require (params != NULL) ;

    smaStats = (SMAPOOL_Stats *) params ;
    smaState = (SMAPOOL_Object *) object ;

    /* Check if pool is opened. */
    if (smaState->refCount == 0) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }
    else {
        ctrlPtr  = smaState->ctrlPtr ;
        mpcsObj  = (MPCS_ShObj *) &(smaState->smaShmObj->mpcsObj) ;
        smaBufObj = smaState->ctrlPtr->smaBufObjs ;

        for (i = 0 ; i < ctrlPtr->numBufs ; i++) {
            /* -----------------------------------------------------------------
             * Enter the critical section.
             * -----------------------------------------------------------------
             */
            LDRV_MPCS_enter (mpcsObj) ;

            SMABUF_GetStats (&smaBufObj [i], &smaStats->mpBufStats [i]) ;

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
        smaStats->smaBufObjCount = ctrlPtr->numBufs ;
    }

    TRC_1LEAVE ("SMAPOOL_instrument", status) ;

    return status ;
}
#endif /* if defined (DDSP_PROFILE) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
