/** ============================================================================
 *  @file   ldrv_data.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/DATA/
 *
 *  @desc   Implements the interface exposed by LDRV_DATA subcomponent.
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
#include <gpptypes.h>
#include <dsplink.h>
#include <_intobject.h>
#include <errbase.h>
#include <_safe.h>
#include <linkcfgdefs.h>

#include <_linkdefs.h>
#include <loaderdefs.h>
#include <_dspdefs.h>

#if defined (CHNL_COMPONENT)
#include <_datadefs.h>
#endif /* if defined (CHNL_COMPONENT) */

#if defined (PROC_COMPONENT)
#include <procdefs.h>
#endif /* if defined (PROC_COMPONENT) */
#if defined (MSGQ_COMPONENT)
#include <msgqdefs.h>
#endif /* if defined (MSGQ_COMPONENT) */

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <mem.h>
#include <print.h>

/*  ----------------------------------- Generic Functions           */
#include <gen_utils.h>


/*  ----------------------------------- Link Driver                 */
#include <cfgmap.h>
#include <ldrv_pool.h>
#include <ldrv_chnl.h>
#include <ldrv_data.h>
#include <ldrv_smm.h>
#include <ldrv.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_DATA

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_LDRV_DATA, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_DATA_Info
 *
 *  @desc   This structure contains information about a Data driver associated
 *          with a specific Data driver ID.
 *
 *  @field  interface
 *              Function pointer interface table for the data driver.
 *  ----------------------------------------------------------------------------
 */
typedef struct LDRV_DATA_Info_tag {
    DATA_Interface * interface ;
} LDRV_DATA_Info ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_DATA_Object
 *
 *  @desc   This structure defines the LDRV DATA object, which contains all state
 *          information required by the LDRV DATA component.
 *
 *  @field  ctrlPtr
 *              Pointer to the LDRV DATA shared memory control region.
 *  @field  dspCtrlAddr
 *              Address of the LDRV DATA shared memoyr control region in DSP
 *              address space.
 *  @field  numDataDrivers
 *              Number of Data drivers towards the specific DSP.
 *  @field  dataInfo
 *              Pointer to the array of Data driver information objects.
 *  @field  dataTable
 *              Pointer to the table of data driver objects.
 *  ----------------------------------------------------------------------------
 */
typedef struct LDRV_DATA_Object_tag {
    LDRV_DATA_Ctrl *   ctrlPtr ;
    Uint32             dspCtrlAddr ;
    Uint16             numDataDrivers ;
    LDRV_DATA_Info *   dataInfo ;
    LINKCFG_DataDrv *  dataTable ;
} LDRV_DATA_Object ;


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_DATA_State
 *
 *  @desc   State object for the LDRV_DATA component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC LDRV_DATA_Object LDRV_DATA_State [MAX_DSPS] ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_DATA_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool LDRV_DATA_IsInitialized [MAX_DSPS] ;


/** ============================================================================
 *  @func   LDRV_DATA_moduleInit
 *
 *  @desc   This function initializes the LDRV_DATA module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_moduleInit (Void)
{
    DSP_STATUS         status = DSP_SOK ;
    Uint32             dspId ;

    TRC_0ENTER ("LDRV_DATA_moduleInit") ;

    for (dspId = 0 ; (dspId < MAX_DSPS) && DSP_SUCCEEDED (status) ; dspId++) {
        /* Initialize the IsInitialized flags for the DSP. */
        LDRV_DATA_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_DATA_moduleInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_DATA_moduleExit
 *
 *  @desc   This function finalizes the LDRV_DATA module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_moduleExit (Void)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    Uint32     dspId ;

    TRC_0ENTER ("LDRV_DATA_moduleExit") ;

    /* Finalize the IsInitialized flags for all DSPs. */
    for (dspId = 0 ; dspId < MAX_DSPS ; dspId++) {
        if (LDRV_DATA_IsInitialized [dspId] == TRUE) {
            tmpStatus = LDRV_DATA_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("LDRV_DATA_moduleExit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_DATA_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_init (IN ProcessorId dspId, OUT Uint32 * shDspAddr)
{
    DSP_STATUS          status   = DSP_SOK ;
    LDRV_DATA_Ctrl *    dataCtrl = NULL ;
    LDRV_DATA_Object *  dataState ;
    LINKCFG_Dsp *      dspObj ;
    LINKCFG_DspConfig * dspCfg ;
    LINKCFG_LinkDrv *  linkDrv ;
    LINKCFG_DataDrv *  dataTable ;
    Uint16              i ;
    Int32              strCmpResult ;
    Uint32             dataId ;
    Uint32             mapId ;
    Uint32             numChannels ;

    TRC_2ENTER ("LDRV_DATA_init", dspId, shDspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (shDspAddr != NULL) ;

    dataState = &(LDRV_DATA_State [dspId]) ;
    dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj    = dspCfg->dspObject ;
    linkDrv   = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
    dataTable = &(dspCfg->dataTables [linkDrv->dataTableId][0]) ;

    *shDspAddr = (Uint32) NULL ;
    if (LDRV_DATA_IsInitialized [dspId] == FALSE) {
        /*  --------------------------------------------------------------------
         * Validate the setup configuration parameters for the Data driver.
         *  --------------------------------------------------------------------
         */
        /* Initialize the state object. */
        dataState->ctrlPtr        = NULL ;
        dataState->dspCtrlAddr    = (Uint32) NULL ;
        dataState->numDataDrivers = linkDrv->numDataDrivers ;
        dataState->dataTable      = dataTable ;

        status = MEM_Calloc ((Void **) &(dataState->dataInfo),
                          (sizeof (LDRV_DATA_Info) * dataState->numDataDrivers),
                          MEM_DEFAULT) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            numChannels = 0 ;

            /* For each Data driver, check if the configured driver is available
             * and set the interface table if the configuration is valid.
             */
            for (dataId = 0 ;
                 (dataId < dataState->numDataDrivers) && DSP_SUCCEEDED (status);
                 dataId++) {
                for (mapId = 0 ;
                     mapId < CFGMAP_Config [dspId]->numDataDrivers ;
                     mapId++) {
                    status = GEN_Strcmp (dataTable [dataId].name,
                                         CFGMAP_Config [dspId]->dataObjects [mapId].name,
                                         &strCmpResult) ;
                    DBC_Assert (DSP_SUCCEEDED (status)) ;
                    if (strCmpResult == 0) {
                        dataState->dataInfo [dataId].interface  =
                                   CFGMAP_Config [dspId]->dataObjects [mapId].interface ;
                        break ;
                    }
                }

                if (mapId == CFGMAP_Config [dspId]->numDataDrivers) {
                    /* Configured Data driver is not available. */
                    PRINT_Printf ("Configuration error:"
                                 " Incorrect Data driver name specified [%s]\n",
                                 dataTable [dataId].name) ;
                    status = DSP_ECONFIG ;
                    SET_FAILURE_REASON ;
                }
                else if (   (dataTable [dataId].memEntry != (Uint32) -1)
                       && (dataTable [dataId].memEntry >= dspObj->memEntries)) {
                    /* Check if the memEntry is in valid range. */
                    PRINT_Printf ("Configuration error:"
                           " Incorrect Data driver memEntry specified [0x%x]\n",
                           dataTable [dataId].memEntry) ;
                    status = DSP_ECONFIG ;
                    SET_FAILURE_REASON ;
                }
                else if (dataTable [dataId].poolId >= linkDrv->numPools) {
                    /* Check if the poolId is in valid range. */
                    PRINT_Printf ("Configuration error:"
                             " Incorrect Data driver poolId specified [0x%x]\n",
                             dataTable [dataId].poolId) ;
                    status = DSP_ECONFIG ;
                    SET_FAILURE_REASON ;
                }
                else if (   dataTable [dataId].queuePerChnl
                         >  LDRV_LinkCfgPtr->gppObject->maxChnlQueue) {
                    /* Check if the queuePerChnl is in valid range. */
                    PRINT_Printf ("Configuration error:"
                       " Incorrect Data driver queuePerChnl specified [0x%x]\n",
                       dataTable [dataId].queuePerChnl) ;
                    status = DSP_ECONFIG ;
                    SET_FAILURE_REASON ;
                }
                else if (    (dataTable [dataId].ipsId != (Uint32) -1)
                         &&  (    dataTable [dataId].ipsId
                              >=  linkDrv->numIpsEntries)) {
                    /* Check if the ipsId is in valid range. */
                    PRINT_Printf ("Configuration error:"
                              " Incorrect Data driver ipsId specified [0x%x]\n",
                              dataTable [dataId].ipsId) ;
                    status = DSP_ECONFIG ;
                    SET_FAILURE_REASON ;
                }
                else if (   (dataTable [dataId].ipsId      != (Uint32) -1)
                         && (dataTable [dataId].ipsEventNo != (Uint32) -1)
                         && (   dataTable [dataId].ipsEventNo
                             >= dspCfg->ipsTables [linkDrv->ipsTableId]
                                     [dataTable [dataId].ipsId].numIpsEvents)) {
                    /* Check if the ipsEventNo is in valid range. */
                    PRINT_Printf ("Configuration error:"
                         " Incorrect Data driver ipsEventNo specified [0x%x]\n",
                         dataTable [dataId].ipsEventNo) ;
                    status = DSP_ECONFIG ;
                    SET_FAILURE_REASON ;
                }
                else {
                    numChannels += dataTable [dataId].numChannels ;
                }
            }

            /* Check if the numChannels is in valid range. */
            if (DSP_SUCCEEDED (status) && (numChannels > MAX_CHANNELS)) {
                PRINT_Printf ("Configuration error:"
                              " Total numChannels [%d] for all data drivers for"
                              " DSP ID [%d] exceeds maximum limit [%d].\n",
                              numChannels,
                              dspId,
                              MAX_CHANNELS) ;
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }

            if (DSP_FAILED (status)) {
                FREE_PTR (dataState->dataInfo) ;
            }
        }
    }

    if (   (LDRV_DATA_IsInitialized [dspId] == FALSE)
        && (DSP_SUCCEEDED (status))) {
        /*  --------------------------------------------------------------------
         *  Allocate shared memory required for the LDRV_DATA component.
         *  Use the memory entry of the Link driver for the LDRV_DATA
         *  component.
         *  --------------------------------------------------------------------
         */
        status = LDRV_SMM_alloc (dspId,
                                 linkDrv->memEntry,
                                 NULL, /* Physical address not required.  */
                                 &(dataState->dspCtrlAddr),
                                 (Uint32 *) &(dataState->ctrlPtr),
                                 DSPLINK_ALIGN ((  (sizeof (LDRV_DATA_Ctrl)
                                                 * dataState->numDataDrivers)),
                                                DSPLINK_BUF_ALIGN)) ;
        if (DSP_SUCCEEDED (status)) {
            *shDspAddr = dataState->dspCtrlAddr ;
            TRC_1PRINT (TRC_LEVEL4,
                        "LDRV_DATA Ctrl GPP address [0x%x]\n",
                        dataState->ctrlPtr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "LDRV_DATA Ctrl DSP address [0x%x]\n",
                        dataState->dspCtrlAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "LDRV_DATA Ctrl size        [0x%x]\n",
                        DSPLINK_ALIGN ((    (sizeof (LDRV_DATA_Ctrl)
                                        *   dataState->numDataDrivers)),
                                       DSPLINK_BUF_ALIGN)) ;

            dataCtrl = dataState->ctrlPtr ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        /* ---------------------------------------------------------------------
         * Initialize the DATA Driver(s).
         * ---------------------------------------------------------------------
         */
        for (i = 0 ;
             (i < dataState->numDataDrivers) && DSP_SUCCEEDED (status) ;
             i++) {
            /*  ----------------------------------------------------------------
             *  Write shared configuration information into the ctrl structure.
             *  ----------------------------------------------------------------
             */
            dataCtrl [i].config.baseChnlId   = dataTable [i].baseChnlId ;
            dataCtrl [i].config.numChannels  = dataTable [i].numChannels ;
            dataCtrl [i].config.maxBufSize   = dataTable [i].maxBufSize ;
            /* Adjust the pool Id */
            dataTable [i].poolId = (  ((dspId << 8u) & 0xFF00u)
                                    | (dataTable [i].poolId & 0x00FFu)) ;
            dataCtrl [i].config.poolId       = dataTable [i].poolId ;
            dataCtrl [i].config.queuePerChnl = dataTable [i].queuePerChnl ;
            dataCtrl [i].config.ipsId        = dataTable [i].ipsId ;
            dataCtrl [i].config.ipsEventNo   = dataTable [i].ipsEventNo ;
            dataCtrl [i].config.arg1         = dataTable [i].arg1 ;
            dataCtrl [i].config.arg2         = dataTable [i].arg2 ;

            /*  ----------------------------------------------------------------
             *  Initialize the Data driver towards the DSP and get the address
             *  of its shared memory control region in DSP address space.
             *  ----------------------------------------------------------------
             */
            status = (dataState->dataInfo [i].interface->init) (dspId,
                                                      i,
                                                      &(dataCtrl [i].dspAddr)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        LDRV_DATA_IsInitialized [dspId] = TRUE ;
        if (DSP_FAILED (status)) {
            /* LDRV_DATA_exit executes cleanup only if the initialized flag is
             * TRUE.
             */
            LDRV_DATA_exit (dspId) ;
            LDRV_DATA_IsInitialized [dspId] = FALSE ;
        }
    }

    TRC_1LEAVE ("LDRV_DATA_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_DATA_exit
 *
 *  @desc   This function finalizes the LDRV DATA module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_exit (IN ProcessorId dspId)
{
    DSP_STATUS          status      = DSP_SOK ;
    DSP_STATUS          tmpStatus   = DSP_SOK ;
    LINKCFG_DspConfig * dspCfg ;
    LDRV_DATA_Object *  dataState ;
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_LinkDrv *   linkDrv ;
    Uint16              i ;

    TRC_1ENTER ("LDRV_DATA_exit", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    dataState = &(LDRV_DATA_State [dspId]) ;
    dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj    = dspCfg->dspObject ;
    linkDrv   = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;

    if (LDRV_DATA_IsInitialized [dspId] == TRUE) {
        if (dataState->dataInfo != NULL) {
            /* -----------------------------------------------------------------
             * Finalize the Data driver(s).
             * -----------------------------------------------------------------
             */
            for (i = 0 ; (i < dataState->numDataDrivers) ; i++) {
                if (dataState->dataInfo [i].interface != NULL) {
                    tmpStatus = (dataState->dataInfo [i].interface->exit) (
                                                                         dspId,
                                                                         i) ;
                    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
            }
        }

        /*  --------------------------------------------------------------------
         *  Free shared memory allocated for the LDRV_DATA component.
         *  Use the memory entry of the Link driver for the LDRV_DATA component.
         *  --------------------------------------------------------------------
         */
        if (dataState->ctrlPtr != NULL) {
            tmpStatus = LDRV_SMM_free (dspId,
                                       linkDrv->memEntry,
                                       DSPLINK_ALIGN (( (sizeof (LDRV_DATA_Ctrl)
                                                  * dataState->numDataDrivers)),
                                                  DSPLINK_BUF_ALIGN)) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        if (LDRV_DATA_State [dspId].dataInfo != NULL) {
            tmpStatus = FREE_PTR (LDRV_DATA_State [dspId].dataInfo) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        LDRV_DATA_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_DATA_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_DATA_openChannel
 *
 *  @desc   Open a channel for input/output.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_openChannel (IN ProcessorId dspId, IN ChannelId chnlId)
{
    DSP_STATUS          status = DSP_SOK ;
    LDRV_DATA_Object *  dataState ;
    Uint32              dataDrvId ;

    TRC_2ENTER ("LDRV_DATA_openChannel", dspId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Assert  (LDRV_DATA_IsInitialized [dspId] == TRUE) ;

    dataState = &(LDRV_DATA_State [dspId]) ;

    status = LDRV_DATA_getDataDrvId (dspId, chnlId, &dataDrvId) ;
    if (DSP_SUCCEEDED (status)) {
        status = (dataState->dataInfo [dataDrvId].interface->openChannel)(dspId,
                                                                       chnlId) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_DATA_openChannel", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_DATA_closeChannel
 *
 *  @desc   Close a channel.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_closeChannel (IN ProcessorId dspId, IN ChannelId chnlId)
{
    DSP_STATUS          status = DSP_SOK ;
    LDRV_DATA_Object *  dataState ;
    Uint32              dataDrvId ;

    TRC_2ENTER ("LDRV_DATA_closeChannel", dspId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Assert  (LDRV_DATA_IsInitialized [dspId] == TRUE) ;

    dataState = &(LDRV_DATA_State [dspId]) ;

    status = LDRV_DATA_getDataDrvId (dspId, chnlId, &dataDrvId) ;
    if (DSP_SUCCEEDED (status)) {
        status = (dataState->dataInfo [dataDrvId].interface->closeChannel) (
                                                                 dspId,
                                                                 chnlId) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_DATA_closeChannel", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_DATA_allocateBuffer
 *
 *  @desc   Allocates an array of buffers of specified size and returns them
 *          to the client.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_allocateBuffer (IN  ProcessorId dspId,
                          IN  ChannelId   chnlId,
                          OUT Char8 **    bufArray,
                          IN  Uint32      size,
                          IN  Uint32      numBufs)
{
    DSP_STATUS          status = DSP_SOK ;
    LDRV_DATA_Object *  dataState ;
    Uint32              dataDrvId ;
    PoolId              poolId ;
    Uint32              i ;

    TRC_5ENTER ("LDRV_DATA_allocateBuffer",
                dspId,
                chnlId,
                bufArray,
                size,
                numBufs) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (bufArray != NULL) ;
    DBC_Require (numBufs  <= MAX_ALLOC_BUFFERS) ;
    DBC_Assert  (LDRV_DATA_IsInitialized [dspId] == TRUE) ;

    dataState = &(LDRV_DATA_State [dspId]) ;

    status = LDRV_DATA_getDataDrvId (dspId, chnlId, &dataDrvId) ;
    if (DSP_SUCCEEDED (status)) {
        poolId = dataState->dataTable [dataDrvId].poolId ;
        for (i = 0 ; DSP_SUCCEEDED (status) && i < numBufs ; i++) {
            status = LDRV_POOL_alloc (poolId, (Pvoid *) &(bufArray[i]), size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        if (DSP_FAILED (status)) {
            LDRV_DATA_freeBuffer (dspId, chnlId, bufArray, size, numBufs) ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_DATA_allocateBuffer", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_DATA_freeBuffer
 *
 *  @desc   Frees buffer(s) allocated by LDRV_DATA_allocateBuffer.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_freeBuffer (IN ProcessorId dspId,
                      IN ChannelId   chnlId,
                      IN Char8 **    bufArray,
                      IN Uint32      size,
                      IN Uint32      numBufs)
{
    DSP_STATUS          status = DSP_SOK ;
    LDRV_DATA_Object *  dataState ;
    Uint32              dataDrvId ;
    Uint32              poolId ;
    Uint32              i ;

    TRC_4ENTER ("LDRV_DATA_freeBuffer", dspId, chnlId, bufArray, numBufs) ;

    DBC_Assert  (LDRV_DATA_IsInitialized [dspId] == TRUE) ;

    dataState = &(LDRV_DATA_State [dspId]) ;

    status = LDRV_DATA_getDataDrvId (dspId, chnlId, &dataDrvId) ;
    if (DSP_SUCCEEDED (status)) {
        poolId = dataState->dataTable [dataDrvId].poolId ;
        for (i = 0 ; i < numBufs ; i++) {
            status = LDRV_POOL_free (poolId, (Pvoid) bufArray [i], size) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_DATA_freeBuffer", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_DATA_cancel
 *
 *  @desc   Cancel a channel.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_cancel (IN ProcessorId dspId, IN ChannelId chnlId)
{
    DSP_STATUS          status = DSP_SOK ;
    LDRV_DATA_Object *  dataState ;
    Uint32              dataDrvId ;

    TRC_2ENTER ("LDRV_DATA_cancel", dspId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Assert  (LDRV_DATA_IsInitialized [dspId] == TRUE) ;

    dataState = &(LDRV_DATA_State [dspId]) ;

    status = LDRV_DATA_getDataDrvId (dspId, chnlId, &dataDrvId) ;
    if (DSP_SUCCEEDED (status)) {
        status = (dataState->dataInfo [dataDrvId].interface->cancelChannel) (
                                                                  dspId,
                                                                  chnlId) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_DATA_cancel", status) ;

    return status;
}


/** ============================================================================
 *  @func   LDRV_DATA_request
 *
 *  @desc   This function sends an IO request on specified channel to the
 *          data driver.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_request (IN ProcessorId dspId, IN ChannelId chnlId)
{
    DSP_STATUS          status = DSP_SOK ;
    LDRV_DATA_Object *  dataState ;
    Uint32              dataDrvId ;

    TRC_2ENTER ("LDRV_DATA_request", dspId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Assert  (LDRV_DATA_IsInitialized [dspId] == TRUE) ;

    dataState = &(LDRV_DATA_State [dspId]) ;

    status = LDRV_DATA_getDataDrvId (dspId, chnlId, &dataDrvId) ;
    if (DSP_SUCCEEDED (status)) {
        status = (dataState->dataInfo [dataDrvId].interface->request) (dspId,
                                                                       chnlId) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_DATA_request", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_DATA_getPoolId
 *
 *  @desc   Returns the pool id to be used for allocating buffer for the channel
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_getPoolId (IN  ProcessorId    dspId,
                     IN  ChannelId      chnlId,
                     OUT Uint32 *       poolId)
{
    DSP_STATUS          status = DSP_SOK ;
    LDRV_DATA_Object *  dataState ;
    Uint32              dataDrvId ;

    TRC_3ENTER ("LDRV_DATA_getPoolId", dspId, chnlId, poolId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (poolId != NULL) ;
    DBC_Assert  (LDRV_DATA_IsInitialized [dspId] == TRUE) ;

    dataState = &(LDRV_DATA_State [dspId]) ;

    status = LDRV_DATA_getDataDrvId (dspId, chnlId, &dataDrvId) ;
    if (DSP_SUCCEEDED (status)) {
        *poolId = dataState->dataTable [dataDrvId].poolId ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_DATA_getPoolId", status) ;

    return status ;

}


/** ============================================================================
 *  @func   LDRV_DATA_getDataDrvId
 *
 *  @desc   Get the data link driver identifier for a given channel.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_getDataDrvId (IN  ProcessorId  dspId,
                        IN  ChannelId    chnlId,
                        OUT Uint32 *     dataDrvId)
{
    DSP_STATUS          status  = DSP_SOK ;
    LDRV_DATA_Object *  dataState ;
    LINKCFG_DataDrv *   dataObj ;
    Uint16              i ;

    TRC_3ENTER ("LDRV_DATA_getDataDrvId", dspId, chnlId, dataDrvId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (dataDrvId != NULL) ;
    DBC_Assert  (LDRV_DATA_IsInitialized [dspId] == TRUE) ;

    dataState = &(LDRV_DATA_State [dspId]) ;

    if (dataDrvId != NULL) {
        *dataDrvId  = (Uint32) -1 ; /* Initialize to invalid */
        for (i = 0 ; i < dataState->numDataDrivers ; i++) {
            dataObj = &(dataState->dataTable [i]) ;
            if (IS_RANGE_VALID (chnlId,
                                dataObj->baseChnlId,
                                (dataObj->baseChnlId + dataObj->numChannels))) {
                *dataDrvId = i ;
            }
        }

        /* Specified channel ID was not in the expected range. */
        if (*dataDrvId == (Uint32) -1) {
            status = DSP_ENOTFOUND ;
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_DATA_getDataDrvId", status) ;

    return status ;
}


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_DATA_debug
 *
 *  @desc   Prints the current status of this subcomponent.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_DATA_debug (IN  ProcessorId  dspId)
{
    LDRV_DATA_Object *  dataState ;
    Uint16              i ;

    TRC_1ENTER ("LDRV_DATA_debug", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    dataState = &(LDRV_DATA_State [dspId]) ;

    if (LDRV_DATA_IsInitialized [dspId] == TRUE) {
        /*  --------------------------------------------------------------------
         *  Print the LDRV DATA component state information.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_1PRINT (TRC_LEVEL4,
                    "LDRV DATA component state information for DSP ID %d:\n",
                    dspId) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ctrlPtr          = [0x%x].\n",
                    dataState->ctrlPtr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dspCtrlAddr      = [0x%x].\n",
                    dataState->dspCtrlAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    numDataDrivers   = [0x%x].\n",
                    dataState->numDataDrivers) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dataInfo         = [0x%x].\n",
                    dataState->dataInfo) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dataTable        = [0x%x].\n",
                    dataState->dataTable) ;
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;

        /*  --------------------------------------------------------------------
         *  Print the information for the shared memory regions.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "Shared memory region information:\n") ;
        for (i = 0 ; (i < dataState->numDataDrivers) ; i++) {
            TRC_2PRINT (TRC_LEVEL4,
                        "    Data driver ID %d: dspAddr             = [0x%x]\n",
                        i,
                        dataState->ctrlPtr [i].dspAddr) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    Data driver ID %d: config.baseChnlId   = [0x%x]\n",
                        i,
                        dataState->ctrlPtr [i].config.baseChnlId) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    Data driver ID %d: config.numChannels  = [0x%x]\n",
                        i,
                        dataState->ctrlPtr [i].config.numChannels) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    Data driver ID %d: config.maxBufSize   = [0x%x]\n",
                        i,
                        dataState->ctrlPtr [i].config.maxBufSize) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    Data driver ID %d: config.poolId       = [0x%x]\n",
                        i,
                        dataState->ctrlPtr [i].config.poolId) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    Data driver ID %d: config.queuePerChnl = [0x%x]\n",
                        i,
                        dataState->ctrlPtr [i].config.queuePerChnl) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    Data driver ID %d: config.ipsId        = [0x%x]\n",
                        i,
                        dataState->ctrlPtr [i].config.ipsId) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    Data driver ID %d: config.ipsEventNo   = [0x%x]\n",
                        i,
                        dataState->ctrlPtr [i].config.ipsEventNo) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    Data driver ID %d: config.arg1         = [0x%x]\n",
                        i,
                        dataState->ctrlPtr [i].config.arg1) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    Data driver ID %d: config.arg2         = [0x%x]\n",
                        i,
                        dataState->ctrlPtr [i].config.arg2) ;

            /*  ----------------------------------------------------------------
             *  Call the debug function for the specific Data driver.
             *  ----------------------------------------------------------------
             */
            TRC_1PRINT (TRC_LEVEL4,
                        "Platform-specifc information for Data driver ID %d:\n",
                        i) ;
            (dataState->dataInfo [i].interface->debug) (dspId, i) ;
        }
    }
    else {
        TRC_1PRINT (TRC_LEVEL4,
                    "LDRV_DATA component for DSP ID %d is not initialized.\n",
                    dspId) ;
    }

    TRC_0LEAVE ("LDRV_DATA_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif
