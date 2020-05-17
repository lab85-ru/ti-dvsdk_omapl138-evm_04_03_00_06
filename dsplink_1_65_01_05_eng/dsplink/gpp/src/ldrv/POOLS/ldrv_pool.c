/** ============================================================================
 *  @file   ldrv_pool.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/POOLS/
 *
 *  @desc   Defines the LDRV POOL component.
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
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Generic Functions           */
#include <gen_utils.h>

/*  ----------------------------------- OSAL Headers                */
#include <mem_os.h>
#include <mem.h>
#include <sync.h>

/*  ----------------------------------- LDRV Headers                */
#include <cfgmap.h>
#include <ldrv_pool.h>
#include <ldrv_smm.h>
#include <ldrv.h>
#include <_pool.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   COMPONENT_ID
 *
 *  @desc   Component ID of this module. That will be used by TRC macros.
 *  ============================================================================
 */
#define COMPONENT_ID    ID_LDRV_POOL


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  TRC_SetReason (status, FID_C_LDRV_POOL, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_POOL_Info
 *
 *  @desc   This structure contains information about a POOL associated with a
 *          specific pool ID.
 *
 *  @field  init
 *              Initialization function for the pool.
 *  @field  exit
 *              Finalization function for the pool.
 *  @field  interface
 *              Function pointer interface table for the pool.
 *  @field  object
 *              Pool implementation-specific object.
 *  ----------------------------------------------------------------------------
 */
typedef struct LDRV_POOL_Info_tag {
    FnPoolInit               init ;
    FnPoolExit               exit ;
    POOL_Interface *         interface ;
    Void *                   object ;
} LDRV_POOL_Info ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_POOL_Object
 *
 *  @desc   This structure defines the LDRV POOL object, which contains all
 *          state information required by the LDRV POOL component.
 *
 *  @field  ctrlPtr
 *              Pointer to the LDRV POOL shared memory control region.
 *  @field  dspCtrlAddr
 *              Address of the LDRV POOL shared memoyr control region in DSP
 *              address space.
 *  @field  numPools
 *              Number of pools in the system.
 *  @field  poolInfo
 *              Pointer to the array of pool information objects.
 *  ----------------------------------------------------------------------------
 */
typedef struct LDRV_POOL_Object_tag {
    LDRV_POOL_Ctrl *  ctrlPtr ;
    Uint32            dspCtrlAddr ;
    Uint16            numPools ;
    LDRV_POOL_Info *  poolInfo ;
} LDRV_POOL_Object ;


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_IPS_State
 *
 *  @desc   State object for the LDRV_IPS component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC LDRV_POOL_Object LDRV_POOL_State [MAX_PROCESSORS] ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_POOL_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool LDRV_POOL_IsInitialized [MAX_PROCESSORS] ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_POOL_addrConfig
 *
 *  @desc   Contains pool open param information for all the pools .
 *  ----------------------------------------------------------------------------
 */
STATIC POOL_AddrInfo LDRV_POOL_addrConfig [MAX_PROCESSORS][MAX_POOLENTRIES] ;


/** ============================================================================
 *  @func   LDRV_POOL_moduleInit
 *
 *  @desc   This function initializes the LDRV_POOL module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_POOL_moduleInit (Void)
{
    DSP_STATUS         status    = DSP_SOK ;
    Uint32             procId ;

    TRC_0ENTER ("LDRV_POOL_moduleInit") ;

    for (procId = 0 ; (procId < MAX_PROCESSORS) ; procId++) {
        /* Initialize the IsInitialized flag for the DSP. */
        LDRV_POOL_IsInitialized [procId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_POOL_moduleInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_POOL_moduleExit
 *
 *  @desc   This function finalizes the LDRV_POOL module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_POOL_moduleExit (Void)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    Uint32     procId ;

    TRC_0ENTER ("LDRV_POOL_moduleExit") ;

    /* Finalize the IsInitialized flags for all DSPs. */
    for (procId = 0 ; procId < MAX_PROCESSORS ; procId++) {
        if (LDRV_POOL_IsInitialized [procId] == TRUE) {
            tmpStatus = LDRV_POOL_exit (procId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("LDRV_POOL_moduleExit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_POOL_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_POOL_init (IN ProcessorId procId, OUT Uint32 * shDspAddr)
{
    DSP_STATUS          status    = DSP_SOK ;
    LDRV_POOL_Ctrl *    poolCtrl  = NULL    ;
    LINKCFG_LinkDrv *   linkDrv   = NULL    ;
    LINKCFG_Pool *      poolTable = NULL    ;
    LINKCFG_DspConfig * dspCfg    = NULL    ;
    LINKCFG_Dsp *       dspObj    = NULL    ;
    LDRV_POOL_Object *  poolState = NULL    ;
    LINKCFG_Gpp *       gppObj    = NULL    ;
    Uint32              poolId    = POOL_INVALIDID;
    Uint16              i ;
    Uint32              mapId ;
    Int32               strCmpResult ;

    TRC_2ENTER ("LDRV_POOL_init", procId, shDspAddr) ;

    DBC_Require (procId < MAX_PROCESSORS) ;
    DBC_Require (shDspAddr != NULL) ;

    poolState = &(LDRV_POOL_State [procId]) ;

    if (procId == ID_GPP) {
        gppObj = LDRV_LinkCfgPtr->gppObject ;
        DBC_Assert (gppObj != NULL) ;

        if (    (gppObj->poolTableId != (Uint32) -1)
            ||  (gppObj->numPools    != 0)) {
            /* GPP-only pools are currently not supported. */
            PRINT_Printf ("<1>Configuration error:"
                        " GPP-only pools are currently not supported %d.\n",
                        poolState->numPools) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
    }
    else {
        dspCfg   = LDRV_LinkCfgPtr->dspConfigs [procId] ;
        dspObj   = dspCfg->dspObject ;
        linkDrv   = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
        poolTable = &(dspCfg->poolTables [linkDrv->poolTableId][0]) ;

        poolState->numPools = linkDrv->numPools ;
        /*  --------------------------------------------------------------------
         * Validate the setup configuration parameters for the POOL.
         *  --------------------------------------------------------------------
         */
        if (poolState->numPools > MAX_POOLENTRIES) {
            /* Number of pools within pool table is more than max supported. */
            PRINT_Printf ("<1>Configuration error:"
                          " Number of pools [%d] within pool table is more than"
                          " max supported [%d]\n",
                          poolState->numPools,
                          MAX_POOLENTRIES) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else {
            /* Initialize the state object. */
            poolState->ctrlPtr     = NULL ;
            poolState->dspCtrlAddr = (Uint32) NULL ;

            if (poolState->numPools != 0) {
                status = MEM_Calloc ((Void **) &(poolState->poolInfo),
                                (sizeof (LDRV_POOL_Info) * poolState->numPools),
                                MEM_DEFAULT) ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            /* For each POOL, check if the configured POOL is available and set
             * the interface table if the configuration is valid.
             */
            for (poolId = 0 ;
                 (poolId < poolState->numPools) && DSP_SUCCEEDED (status) ;
                 poolId++) {
                for (mapId = 0 ;
                     mapId < CFGMAP_Config [procId]->numPools ;
                     mapId++) {
                    status = GEN_Strcmp (
                                poolTable [poolId].name,
                                CFGMAP_Config [procId]->poolObjects [mapId].name,
                                &strCmpResult) ;
                    DBC_Assert (DSP_SUCCEEDED (status)) ;
                    if (strCmpResult == 0) {
                        poolState->poolInfo [poolId].init  =
                              CFGMAP_Config [procId]->poolObjects [mapId].init ;
                        poolState->poolInfo [poolId].exit  =
                              CFGMAP_Config [procId]->poolObjects [mapId].exit ;
                        poolState->poolInfo [poolId].interface =
                         CFGMAP_Config [procId]->poolObjects [mapId].interface ;
                        break ;
                    }
                }

                if (mapId == CFGMAP_Config [procId]->numPools) {
                    /* Configured POOL is not available. */
                    PRINT_Printf ("<1>Configuration error:"
                                  " Incorrect POOL name specified [%s]\n",
                                  poolTable [poolId].name) ;
                    status = DSP_ECONFIG ;
                    SET_FAILURE_REASON ;
                }
                else {
                    /* Validation of configuration for POOL for GPP. */
                    if (procId == ID_GPP) {
                        if (   (poolTable [poolId].memEntry   != (Uint32) -1)
                            || (poolTable [poolId].ipsId      != (Uint32) -1)
                            || (poolTable [poolId].ipsEventNo != (Uint32) -1)) {
                            /* Check if the memEntry, ipsId, ipsEventNo have
                             * valid values. For GPP object, these must all be
                             * specified as -1.
                             */
                            PRINT_Printf ("<1>Configuration error:"
                                         " Incorrect POOL config specified\n") ;
                            status = DSP_ECONFIG ;
                            SET_FAILURE_REASON ;
                        }
                    }
                    else {
                        /* Validation of configuration for POOL for DSP. */
                        if (   (poolTable [poolId].memEntry != (Uint32) -1)
                            && (    poolTable [poolId].memEntry
                                >=  dspObj->memEntries)) {
                            /* Check if the memEntry is in valid range. */
                            PRINT_Printf ("<1>Configuration error:"
                                  " Incorrect POOL memEntry specified [0x%x]\n",
                                  poolTable [poolId].memEntry) ;
                            status = DSP_ECONFIG ;
                            SET_FAILURE_REASON ;
                        }
                        else if (   (poolTable [poolId].memEntry != (Uint32) -1)
                                 && (  poolTable [poolId].poolSize
                                     > dspCfg->memTables
                                          [dspObj->memTableId]
                                      [poolTable [poolId].poolMemEntry].size)) {
                            /* Check if the poolSize is in valid range. */
                            PRINT_Printf ("<1>Configuration error:"
                                  " Incorrect POOL poolSize specified [0x%x]\n",
                                  poolTable [poolId].poolSize) ;
                            status = DSP_ECONFIG ;
                            SET_FAILURE_REASON ;
                        }
                        else if (   (poolTable [poolId].ipsId != (Uint32) -1)
                                 && (    poolTable [poolId].ipsId
                                     >=  linkDrv->numIpsEntries)) {
                            /* Check if the ipsId is in valid range. */
                            PRINT_Printf ("<1>Configuration error:"
                                     " Incorrect POOL ipsId specified [0x%x]\n",
                                     poolTable [poolId].ipsId) ;
                            status = DSP_ECONFIG ;
                            SET_FAILURE_REASON ;
                        }
                        else if (   (poolTable [poolId].ipsId != (Uint32) -1)
                             && (   poolTable [poolId].ipsEventNo
                                 != (Uint32) -1)
                             && (   poolTable [poolId].ipsEventNo
                                 >= dspCfg->ipsTables
                                     [linkDrv->ipsTableId]
                                     [poolTable [poolId].ipsId].numIpsEvents)) {
                            /* Check if the ipsEventNo is in valid range. */
                            PRINT_Printf ("<1>Configuration error:"
                                " Incorrect POOL ipsEventNo specified [0x%x]\n",
                                poolTable [poolId].ipsEventNo) ;
                            status = DSP_ECONFIG ;
                            SET_FAILURE_REASON ;
                        }
                    }
                }
            }
        }

        if (DSP_FAILED (status)) {
            FREE_PTR (poolState->poolInfo) ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        *shDspAddr = (Uint32) NULL ;
        if (    (LDRV_POOL_IsInitialized [procId] == FALSE)
             && (DSP_SUCCEEDED (status))) {
            poolState = &(LDRV_POOL_State [procId]) ;

            if (procId != ID_GPP) {
                /*  ------------------------------------------------------------
                 *  Allocate shared memory required for the LDRV_POOL component.
                 *  Use the memory entry of the Link driver for the LDRV_POOL
                 *  component.
                 *  ------------------------------------------------------------
                 */
                status = LDRV_SMM_alloc (
                                    procId,
                                    linkDrv->memEntry,
                                    NULL, /* Physical address not required.  */
                                    &(poolState->dspCtrlAddr),
                                    (Uint32 *) ((Pvoid) &(poolState->ctrlPtr)),
                                     DSPLINK_ALIGN ((   (sizeof (LDRV_POOL_Ctrl)
                                                      *  poolState->numPools)),
                                                     DSPLINK_BUF_ALIGN)) ;
                if (DSP_SUCCEEDED (status)) {
                    if (poolState != NULL) {
                        *shDspAddr = poolState->dspCtrlAddr ;
                        TRC_1PRINT (TRC_LEVEL4,
                                "LDRV_POOL Ctrl GPP address [0x%x]\n",
                                poolState->ctrlPtr) ;
                        TRC_1PRINT (TRC_LEVEL4,
                                "LDRV_POOL Ctrl DSP address [0x%x]\n",
                                poolState->dspCtrlAddr) ;
                        TRC_1PRINT (TRC_LEVEL4,
                                "LDRV_POOL Ctrl size        [0x%x]\n",
                                DSPLINK_ALIGN ((    (sizeof (LDRV_POOL_Ctrl)
                                                *   poolState->numPools)),
                                               DSPLINK_BUF_ALIGN)) ;

                        poolCtrl = poolState->ctrlPtr ;
                        DBC_Assert (poolCtrl != NULL) ;
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }

            /* -----------------------------------------------------------------
             * Initialize the POOL component(s)
             * -----------------------------------------------------------------
             */
            for (i = 0 ;
                 (i < poolState->numPools) && DSP_SUCCEEDED (status) ;
                 i++) {
                /*  ------------------------------------------------------------
                 *  Write shared configuration information into the ctrl
                 *  structure.
                 *  ------------------------------------------------------------
                 */
                poolCtrl [i].config.poolSize   = poolTable [i].poolSize ;
                poolCtrl [i].config.ipsId      = poolTable [i].ipsId ;
                poolCtrl [i].config.ipsEventNo = poolTable [i].ipsEventNo ;
                poolCtrl [i].config.arg1       = poolTable [i].arg1 ;
                poolCtrl [i].config.arg2       = poolTable [i].arg2 ;

                /*  ------------------------------------------------------------
                 *  Initialize the POOL and get the address of its shared memory
                 *  control region in DSP address space (if required).
                 *  ------------------------------------------------------------
                 */
                status = (poolState->poolInfo [i].init) (procId,
                                            i,
                                            &(poolCtrl [i].dspAddr),
                                            &(poolState->poolInfo [i].object)) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }

            if (DSP_SUCCEEDED (status)) {
                LDRV_POOL_IsInitialized [procId] = TRUE ;
            }
        }
    }

    if (DSP_FAILED (status)) {
        /* LDRV_POOL_exit executes cleanup only if the initialized flag is
         * TRUE.
         */
        LDRV_POOL_exit (procId) ;
        LDRV_POOL_IsInitialized [procId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_POOL_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_POOL_exit
 *
 *  @desc   This function finalizes the LDRV POOL module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_POOL_exit (IN ProcessorId procId)
{
    DSP_STATUS          status      = DSP_SOK ;
    DSP_STATUS          tmpStatus   = DSP_SOK ;
    LINKCFG_LinkDrv *   linkDrv     = NULL    ;
    LINKCFG_DspConfig * dspCfg      = NULL    ;
    LINKCFG_Dsp *       dspObj      = NULL    ;
    LDRV_POOL_Object *  poolState   = NULL    ;
    Uint16              i ;

    TRC_1ENTER ("LDRV_POOL_exit", procId) ;

    DBC_Require (procId < MAX_PROCESSORS) ;

    if (procId != ID_GPP) {
        dspCfg   = LDRV_LinkCfgPtr->dspConfigs [procId] ;
        dspObj   = dspCfg->dspObject ;
        linkDrv  = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
    }

    if (LDRV_POOL_IsInitialized [procId] == TRUE) {
        poolState = &(LDRV_POOL_State [procId]) ;

        /* ---------------------------------------------------------------------
         * Finalize the POOL component(s)
         * ---------------------------------------------------------------------
         */
        for (i = 0 ; (i < poolState->numPools) ; i++) {
            tmpStatus  = (poolState->poolInfo [i].exit) (procId,
                                               i,
                                               poolState->poolInfo [i].object) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        if (procId != ID_GPP) {
            /*  ----------------------------------------------------------------
             *  Free shared memory allocated for the LDRV_POOL component.
             *  Use the memory entry of the Link driver for the LDRV_POOL
             *  component.
             *  ----------------------------------------------------------------
             */
            if (poolState->ctrlPtr != NULL) {
                tmpStatus = LDRV_SMM_free (procId,
                                      linkDrv->memEntry,
                                      DSPLINK_ALIGN ((  (sizeof (LDRV_POOL_Ctrl)
                                                      * poolState->numPools)),
                                                      DSPLINK_BUF_ALIGN)) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
        }

        if (LDRV_POOL_State [procId].poolInfo != NULL) {
            tmpStatus = FREE_PTR (LDRV_POOL_State [procId].poolInfo) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        LDRV_POOL_IsInitialized [procId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_POOL_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @name   LDRV_POOL_open
 *
 *  @desc   This function creates the pool for given pool id.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_open (IN PoolId poolId, IN POOL_OpenParams  * poolOpenParams)
{
    DSP_STATUS         status    = DSP_SOK ;
    LDRV_POOL_Object * poolState = NULL    ;
    LDRV_POOL_Info *   poolInfo  = NULL    ;
    Uint8              poolNo    = POOL_getPoolNo (poolId) ;
    ProcessorId        procId    = POOL_getProcId (poolId) ;
    POOL_AddrInfo *    poolAddrPtr ;

    TRC_2ENTER ("LDRV_POOL_open", poolId, poolOpenParams) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;
    DBC_Require (poolOpenParams != NULL) ;

    DBC_Assert  (IS_VALID_PROCID (procId)) ;
    DBC_Assert  (LDRV_POOL_IsInitialized [procId] == TRUE) ;

    poolState = &(LDRV_POOL_State [procId]) ;
    DBC_Assert (poolNo < poolState->numPools) ;

    /* Check if pool is configured. */
    if (poolNo < poolState->numPools) {
        poolInfo = &(poolState->poolInfo [poolNo]) ;
        status = poolInfo->interface->open (procId,
                                            poolNo,
                                            poolInfo->object,
                                            poolOpenParams) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            poolAddrPtr = &LDRV_POOL_addrConfig [procId][poolNo] ;
            poolAddrPtr->addr [AddrType_Usr] = 0 ;
            poolAddrPtr->addr [AddrType_Phy] = poolOpenParams->physAddr ;
            poolAddrPtr->addr [AddrType_Knl] = poolOpenParams->virtAddr ;
            poolAddrPtr->addr [AddrType_Dsp] = poolOpenParams->dspAddr ;
            poolAddrPtr->size                = poolOpenParams->size ;
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_POOL_open", status) ;

    return status ;
}


/** ============================================================================
 *  @name   LDRV_POOL_close
 *
 *  @desc   This function destroys the pool for given poolId.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_close (IN PoolId poolId)
{
    DSP_STATUS         status    = DSP_SOK ;
    LDRV_POOL_Object * poolState = NULL    ;
    LDRV_POOL_Info *   poolInfo  = NULL    ;
    Uint8              poolNo    = POOL_getPoolNo (poolId) ;
    ProcessorId        procId    = POOL_getProcId (poolId) ;

    TRC_1ENTER ("LDRV_POOL_close", poolId) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;

    DBC_Assert  (IS_VALID_PROCID (procId)) ;
    DBC_Assert  (LDRV_POOL_IsInitialized [procId] == TRUE) ;

    poolState = &(LDRV_POOL_State [procId]) ;
    DBC_Assert (poolNo < poolState->numPools) ;

    /* Check if pool is configured. */
    if (poolNo < poolState->numPools) {
        poolInfo = &(poolState->poolInfo [poolNo]) ;
        status = poolInfo->interface->close (procId,
                                             poolNo,
                                             poolInfo->object) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_POOL_close", status) ;

    return status ;
}


/** ============================================================================
 *  @name   LDRV_POOL_alloc
 *
 *  @desc   This function allocates a buffer from pool with given pool id, and
 *          returns the pointer to the user.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_alloc (IN  PoolId      poolId,
                 OUT Pvoid *     bufPtr,
                 IN  Uint32      size)
{
    DSP_STATUS         status    = DSP_SOK ;
    LDRV_POOL_Object * poolState = NULL    ;
    LDRV_POOL_Info *   poolInfo  = NULL    ;
    Uint8              poolNo    = POOL_getPoolNo (poolId) ;
    ProcessorId        procId    = POOL_getProcId (poolId) ;

    TRC_3ENTER ("LDRV_POOL_alloc", poolId, bufPtr, size) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;
    DBC_Require (bufPtr != NULL) ;
    DBC_Require (size > 0) ;

    DBC_Assert  (IS_VALID_PROCID (procId)) ;
    DBC_Assert  (LDRV_POOL_IsInitialized [procId] == TRUE) ;

    poolState = &(LDRV_POOL_State [procId]) ;
    DBC_Assert (poolNo < poolState->numPools) ;

    /* Check is needed to ensure that GPP-DSP pool onfiguration mismatch does
     * not cause a crash.
     */
    if (poolNo < poolState->numPools) {
        poolInfo = &(poolState->poolInfo [poolNo]) ;
        status = poolInfo->interface->alloc (procId,
                                             poolNo,
                                             poolInfo->object,
                                             bufPtr,
                                             size) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_POOL_alloc", status) ;

    return status ;
}


/** ============================================================================
 *  @name   LDRV_POOL_free
 *
 *  @desc   This function frees a buffer.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_free (IN  PoolId   poolId,
                OUT Pvoid    buf,
                IN  Uint32   size)
{
    DSP_STATUS         status    = DSP_SOK ;
    LDRV_POOL_Object * poolState = NULL    ;
    LDRV_POOL_Info *   poolInfo  = NULL    ;
    Uint8              poolNo    = POOL_getPoolNo (poolId) ;
    ProcessorId        procId    = POOL_getProcId (poolId) ;

    TRC_3ENTER ("LDRV_POOL_free", poolId, buf, size) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;
    DBC_Require (buf != NULL) ;
    DBC_Require (size > 0) ;

    DBC_Assert  (IS_VALID_PROCID (procId)) ;
    DBC_Assert  (LDRV_POOL_IsInitialized [procId] == TRUE) ;

    poolState = &(LDRV_POOL_State [procId]) ;
    DBC_Assert (poolNo < poolState->numPools) ;

    /* Check is needed to ensure that GPP-DSP pool configuration mismatch does
     * not cause a crash.
     */
    if (poolNo < poolState->numPools) {
        poolInfo = &(poolState->poolInfo [poolNo]) ;
        status = poolInfo->interface->free (procId,
                                            poolNo,
                                            poolInfo->object,
                                            buf,
                                            size) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_POOL_free", status) ;

    return status ;
}


/** ============================================================================
 *  @name   LDRV_POOL_invalidate
 *
 *  @desc   This function read the content of DSP buffer into GPP buffer (with
 *          offset in sync).
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_invalidate (IN  PoolId   poolId,
                      OUT Pvoid    buf,
                      IN  Uint32   size)
{
    DSP_STATUS         status    = DSP_SOK ;
    LDRV_POOL_Object * poolState = NULL    ;
    LDRV_POOL_Info *   poolInfo  = NULL    ;
    Uint8              poolNo    = POOL_getPoolNo (poolId) ;
    ProcessorId        procId    = POOL_getProcId (poolId) ;

    TRC_3ENTER ("LDRV_POOL_invalidate", poolId, buf, size) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;
    DBC_Require (buf != NULL) ;
    DBC_Require (size > 0) ;

    DBC_Assert  (IS_VALID_PROCID (procId)) ;
    DBC_Assert  (LDRV_POOL_IsInitialized [procId] == TRUE) ;

    poolState = &(LDRV_POOL_State [procId]) ;
    DBC_Assert (poolNo < poolState->numPools) ;

    /* Check is needed to ensure that GPP-DSP pool onfiguration mismatch does
     * not cause a crash.
     */
    if (   (!IS_VALID_PROCID (procId))
        && (LDRV_POOL_IsInitialized [procId] == TRUE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else
    if (poolNo < poolState->numPools) {
        poolInfo = &(poolState->poolInfo [poolNo]) ;
        /* Only call the function if it is implemented. This function may not
         * be required on all platforms.
         */
        if (poolInfo->interface->invalidate != NULL) {
            status = poolInfo->interface->invalidate (procId,
                                                      poolNo,
                                                      poolInfo->object,
                                                      buf,
                                                      size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_POOL_invalidate", status) ;

    return status ;
}


/** ============================================================================
 *  @name   LDRV_POOL_writeback
 *
 *  @desc   This function read the content of GPP buffer into DSP buffer (with
 *          offset in sync).
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_writeback (IN  PoolId   poolId,
                     OUT Pvoid    buf,
                     IN  Uint32   size)
{
    DSP_STATUS         status    = DSP_SOK ;
    LDRV_POOL_Object * poolState = NULL    ;
    LDRV_POOL_Info *   poolInfo  = NULL    ;
    Uint8              poolNo    = POOL_getPoolNo (poolId) ;
    ProcessorId        procId    = POOL_getProcId (poolId) ;

    TRC_3ENTER ("LDRV_POOL_writeback", poolId, buf, size) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;
    DBC_Require (buf != NULL) ;
    DBC_Require (size > 0) ;

    DBC_Assert  (IS_VALID_PROCID (procId)) ;
    DBC_Assert  (LDRV_POOL_IsInitialized [procId] == TRUE) ;

    poolState = &(LDRV_POOL_State [procId]) ;
    DBC_Assert (poolNo < poolState->numPools) ;

    /* Check is needed to ensure that GPP-DSP pool onfiguration mismatch does
     * not cause a crash.
     */
    if (   (!IS_VALID_PROCID (procId))
        && (LDRV_POOL_IsInitialized [procId] == TRUE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else
    if (poolNo < poolState->numPools) {
        poolInfo = &(poolState->poolInfo [poolNo]) ;
        /* Only call the function if it is implemented. This function may not
         * be required on all platforms.
         */
        if (poolInfo->interface->writeback != NULL) {
            status = poolInfo->interface->writeback (procId,
                                                     poolNo,
                                                     poolInfo->object,
                                                     buf,
                                                     size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_POOL_writeback", status) ;

    return status ;
}


/** ============================================================================
 *  @name   LDRV_POOL_xltBuf
 *
 *  @desc   This function translate buffers from different address spaces.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_xltBuf (IN  PoolId           poolId,
                  IN  Pvoid            buf,
                  OUT Pvoid *          cBuf,
                  IN  POOL_AddrXltFlag xltFlag)
{
    DSP_STATUS         status    = DSP_SOK ;
    LDRV_POOL_Object * poolState = NULL    ;
    LDRV_POOL_Info *   poolInfo  = NULL    ;
    Uint8              poolNo    = POOL_getPoolNo (poolId) ;
    ProcessorId        procId    = POOL_getProcId (poolId) ;

    TRC_3ENTER ("LDRV_POOL_xltbuf", poolId, buf, xltFlag) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;
    DBC_Require (buf != NULL) ;

    DBC_Assert  (IS_VALID_PROCID (procId)) ;
    DBC_Assert  (LDRV_POOL_IsInitialized [procId] == TRUE) ;

    poolState = &(LDRV_POOL_State [procId]) ;
    DBC_Assert (poolNo < poolState->numPools) ;

    /* Check is needed to ensure that GPP-DSP pool onfiguration mismatch does
     * not cause a crash.
     */
    if (poolNo < poolState->numPools) {
        poolInfo = &(poolState->poolInfo [poolNo]) ;
        status = poolInfo->interface->xltBuf (procId,
                                              poolNo,
                                              poolInfo->object,
                                              buf,
                                              cBuf,
                                              xltFlag) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_POOL_xltbuf", status) ;

    return status ;
}

/** ============================================================================
 *  @func   LDRV_POOL_getPoolId
 *
 *  @desc   This function gets the pool id of the buffer that was allocated from
 *          the pool.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_getPoolId (IN  ProcessorId procId,
                     IN  Pvoid       addr,
                     IN  AddrType    addrType,
                     OUT PoolId *    poolId)
{
    DSP_STATUS          status     = DSP_ENOTFOUND ;
    Uint32              fmAddrBase = (Uint32) NULL ;
    Bool                done       = FALSE         ;
    LDRV_POOL_Object *  poolState   = NULL         ;
    Uint32              size                       ;
    Uint32              bufPtr                     ;
    Uint16              i                          ;

    TRC_3ENTER ("LDRV_POOL_getPoolId",
                addr,
                addrType,
                poolId) ;

    DBC_Require (addr   != NULL) ;
    DBC_Require (poolId != NULL) ;
    DBC_Require (   (addrType == AddrType_Phy)
                 || (addrType == AddrType_Knl)
                 || (addrType == AddrType_Dsp)) ;

    if (poolId != NULL) {
        /* Initialize it to invalid  poolid */
        *poolId = POOL_INVALIDID ;
    }

    if (   (addr   == NULL)
        || (poolId == NULL)
        || (   (addrType != AddrType_Phy)
            && (addrType != AddrType_Knl)
            && (addrType != AddrType_Dsp))) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        poolState = &(LDRV_POOL_State [procId]) ;
        bufPtr    = (Uint32) (addr) ;
        for (i = 0 ; (i < poolState->numPools) && (done == FALSE) ; i++) {
            size = LDRV_POOL_addrConfig [procId][i].size ;
            fmAddrBase = LDRV_POOL_addrConfig [procId][i].addr [addrType] ;
            /* Check whether given address lies in current pool entry */
            if ((bufPtr >= fmAddrBase) && (bufPtr < (fmAddrBase + size))) {
                *poolId = (((procId << 8u) & 0xFF00u) | (i & 0x00FFu)) ;
                status = DSP_SOK ;
                done = TRUE ;
            }
        }
    }

    TRC_1LEAVE ("LDRV_POOL_getPoolId", status) ;

    return status ;
}

/** ============================================================================
 *  @name   LDRV_POOL_reconfigure
 *
 *  @desc   This function reconfigures the plugged memory pool.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_POOL_reconfigure (IN PoolId poolId,
                       IN Pvoid  args)
{
    DSP_STATUS         status    = DSP_SOK ;
    LDRV_POOL_Object * poolState = NULL    ;
    LDRV_POOL_Info *   poolInfo  = NULL    ;
    Uint8              poolNo    = POOL_getPoolNo (poolId) ;
    ProcessorId        procId    = POOL_getProcId (poolId) ;

    TRC_2ENTER ("LDRV_POOL_reconfigure", poolId, args) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;
    DBC_Require (args != NULL) ;

    DBC_Assert  (IS_VALID_PROCID (procId)) ;
    DBC_Assert  (LDRV_POOL_IsInitialized [procId] == TRUE) ;

    poolState = &(LDRV_POOL_State [procId]) ;
    DBC_Assert (poolNo < poolState->numPools) ;

    /* Check if pool is configured. */
    if (poolNo < poolState->numPools) {
        /* Check if the function entry is valid. */
        poolInfo = &(poolState->poolInfo [poolNo]) ;
        if (poolInfo->interface->reconfigure != NULL) {
            status = poolInfo->interface->reconfigure (procId,
                                                       poolNo,
                                                       poolInfo->object,
                                                       args) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            status = DSP_ENOTIMPL ;
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_POOL_reconfigure", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
