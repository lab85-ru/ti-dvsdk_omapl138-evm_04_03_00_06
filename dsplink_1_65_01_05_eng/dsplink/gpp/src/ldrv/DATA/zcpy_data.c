/** ============================================================================
 *  @file   zcpy_data.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/DATA/
 *
 *  @desc   Implementation of the ZCPY Data driver using Shared Memory.
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
#include <chnldefs.h>
#include <linkcfgdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <dpc.h>
#include <isr.h>
#include <ips.h>

/*  ----------------------------------- Generic Functions           */
#include <list.h>

/*  ----------------------------------- User API                    */
#include <mpcs.h>

/*  ----------------------------------- Link Driver                 */
#include <ldrv_data.h>
#include <ldrv_chnl.h>
#include <ldrv_chirps.h>
#include <ips.h>
#include <dsp.h>
#include <zcpy_data.h>
#include <ldrv_mplist.h>
#include <ldrv_mpcs.h>
#include <ldrv_smm.h>
#include <ldrv.h>

/*  ----------------------------------- LOG  Headers                 */
#if defined (LOG_COMPONENT)
#include <linklog.h>
#endif /* if defined (LOG_COMPONENT) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_DATA_ZCPY

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  TRC_SetReason (status,                          \
                                           FID_C_LDRV_DATA_ZCPY,            \
                                           __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @const  CHNLID_INVALID
 *
 *  @desc   Indicates invalid channel id.
 *  ============================================================================
 */
#define CHNLID_INVALID  (ChannelId) 0xFFFFl


/** ============================================================================
 *  @name   ZCPYDATA_Object
 *
 *  @desc   Defines the ZCPY Data driver object, which contains all the
 *          component-specific information.
 *
 *  @field  dspId
 *              Processor Identifier.
 *  @field  dataDrvId
 *              Data driver Identifier.
 *  @field  wordSwap
 *              Indicates whether word-swap is enabled for the DSP MMU.
 *  @field  dpcObj
 *              DPC object for the the ZCPY data driver.
 *  @field  ctrlPtr
 *              Pointer to shared memory control structure.
 *  @field  ctrlSize
 *              Size of the ZCPY Data driver shared memory control region.
 *  @field  dataCfgObject
 *              Pointer to the configuration Data driver object.
 *  @field  lastOutput
 *              Stores the channel Id on which the last output was performed.
 *  @field  outputMask
 *              Indicates on which channels output buffer is available.
 *  @field  ongoingOutputMask
 *              Indicates on which channels output data transfer is ongoing.
 *  ============================================================================
 */
typedef struct ZCPYDATA_Object_tag {
    Uint32              dspId ;
    Uint32              dataDrvId ;
    Bool                wordSwap ;
    DpcObject *         dpcObj  ;
    ZCPYDATA_Ctrl *     ctrlPtr ;
    Uint32              ctrlSize ;
    LINKCFG_DataDrv *   dataCfgObject ;
    Uint32              lastOutput ;
    Uint32              outputMask ;
    Uint32              ongoingOutputMask ;
} ZCPYDATA_Object  ;


/** ----------------------------------------------------------------------------
 *  @name   ZCPYDATA_State
 *
 *  @desc   State object for the IPS component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC ZCPYDATA_Object ZCPYDATA_State [MAX_DSPS] [MAX_ZCPY_DATA] ;

/*  ----------------------------------------------------------------------------
 *  @name   ZCPYDATA_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool ZCPYDATA_IsInitialized [MAX_DSPS] [MAX_ZCPY_DATA] ;


/** ============================================================================
 *  @name   ZCPYDATA_Interface
 *
 *  @desc   ZCPY Data Driver interface.
 *  ============================================================================
 */
DATA_Interface ZCPYDATA_Interface = {
    &ZCPYDATA_init,
    &ZCPYDATA_exit,
    &ZCPYDATA_openChannel,
    &ZCPYDATA_closeChannel,
    &ZCPYDATA_cancelIo,
    &ZCPYDATA_request
#if defined (DDSP_DEBUG)
    ,&ZCPYDATA_debug
#endif /* if defined (DDSP_DEBUG) */
} ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_callback
 *
 *  @desc   This function implements the callback invoked by the ZCPY DATA
 *          component on receiving a data transfer request from the remote
 *          processor.
 *
 *  @arg    ipsEventNo
 *              Event number associated with the callback.
 *  @arg    arg
 *              Argument registered with the SHMIPS component along with the
 *              callback function.
 *  @arg    info
 *              Pointer to CHIRP received from the DSP.
 *
 *  @ret    None.
 *
 *  @enter  info must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Void
ZCPYDATA_callback (IN Uint32 ipsEventNo, IN     Pvoid arg, IN     Pvoid info) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_send
 *
 *  @desc   Sends the specified data buffer to the DSP.
 *
 *  @arg    zcpyDataState
 *              Pointer to the data driver state object.
 *  @arg    buffer
 *              Pointer to the buffer to be sent to the DSP.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
ZCPYDATA_send (IN ZCPYDATA_Object * zcpyDataState, IN Pvoid buffer) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_dpc
 *
 *  @desc   This function is the delayed procedure for performing actual data
 *          transfer on the physical link between GPP and target DSP.
 *          It runs at a priority level between interrupt service routines and
 *          normal tasks/processes/threads.
 *
 *  @arg    refData
 *              Argument to the delayed procedure call. It identifies the
 *              associated link driver information.
 *
 *  @ret    None
 *
 *  @enter  refData must not be NULL.
 *
 *  @leave  None
 *
 *  @see    ZCPYDATA_init
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Void
ZCPYDATA_dpc (IN Pvoid refData) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_getNextOutputChannel
 *
 *  @desc   Find the channel that has data to send to DSP.
 *
 *  @arg    dspId
 *              Processor idenfitier.
 *  @arg    zcpyDataState
 *              Pointer to the data driver state object.
 *  @arg    mask
 *              Mask to be used for searching for ready channels.
 *
 *  @ret    ChannelId of the selected channel having data to send to DSP.
 *          CHNLID_INVALID if there is no channel with data.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None
 *
 *  @see    DataOutput
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
ChannelId
ZCPYDATA_getNextOutputChannel (IN  ProcessorId       dspId,
                               IN  ZCPYDATA_Object * zcpyDataState,
                               IN  Uint32            mask) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_clearChannelInfo
 *
 *  @desc   Clear the channel information based on its mode.
 *
 *  @arg    dspId
 *              Processor idenfitier.
 *  @arg    chnlId
 *              Channel identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          chnlId must be valid.
 *
 *  @leave  None
 *
 *  @see    ZCPYDATA_openChannel (), ZCPYDATA_closeChannel (),
 *          ZCPYDATA_cancelIo (),
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
ZCPYDATA_clearChannelInfo (IN  ProcessorId dspId,
                           IN  ChannelId   chnlId) ;


/** ============================================================================
 *  @func   ZCPYDATA_init
 *
 *  @desc   This function allocates and initializes resources used by the
 *          ZCPY Data Driver.
 *
 *  @modif  ZCPYDATA_State [dspId][dataDrvId]
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYDATA_init (IN ProcessorId dspId,
               IN  Uint32     dataDrvId,
               OUT Uint32 *   shDspAddr)
{
    DSP_STATUS         status  = DSP_SOK ;
    ZCPYDATA_Ctrl *    ctrlPtr = NULL ;
    LINKCFG_DspConfig * dspCfg ;
    LINKCFG_Dsp *      dspObj ;
    LINKCFG_LinkDrv *  linkDrv ;
    LINKCFG_DataDrv *  dataObject ;
    ZCPYDATA_Object *  zcpyDataState ;
    MPCS_ShObj *       mpcsObj ;
    ZCPYDATA_ChnlIrp * freeChirps ;
    Uint32             i ;

    TRC_3ENTER ("ZCPYDATA_init", dspId, dataDrvId, shDspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (shDspAddr != NULL) ;

    if (ZCPYDATA_IsInitialized [dspId][dataDrvId] == FALSE) {
        zcpyDataState = &(ZCPYDATA_State [dspId][dataDrvId]) ;
        dspCfg        = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj        = dspCfg->dspObject ;
        linkDrv       = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
        dataObject    =&(dspCfg->dataTables [linkDrv->dataTableId][dataDrvId]) ;

        /*  --------------------------------------------------------------------
         *  Initialize the ZCPY Data driver state object.
         *  --------------------------------------------------------------------
         */
        zcpyDataState->dspId             = dspId ;
        zcpyDataState->dataDrvId         = dataDrvId ;
        zcpyDataState->wordSwap          = dspObj->wordSwap ;
        zcpyDataState->dpcObj            = NULL ;
        zcpyDataState->ctrlPtr           = NULL ;
        zcpyDataState->dataCfgObject     = dataObject ;
        zcpyDataState->lastOutput        = 0x0 ;
        zcpyDataState->outputMask        = 0x0 ;
        zcpyDataState->ongoingOutputMask = 0x0 ;

        /* Calculate the size of shared memory control region.
         * No need to align to cache-line size, since the structure sizes
         * are aligned.
         */
        zcpyDataState->ctrlSize =     sizeof (ZCPYDATA_Ctrl)
                                  +   (     sizeof (ZCPYDATA_ChnlIrp)
                                        *   dataObject->queuePerChnl
                                        *   dataObject->numChannels) ;

        /*  --------------------------------------------------------------------
         *  Allocate shared memory required for the ZCPY DATA component.
         *  --------------------------------------------------------------------
         */
        status = LDRV_SMM_alloc (dspId,
                                 dataObject->memEntry,
                                 NULL, /* Physical address not required. */
                                 shDspAddr,
                                 (Uint32 *) &(zcpyDataState->ctrlPtr),
                                 zcpyDataState->ctrlSize) ;
        if (DSP_SUCCEEDED (status)) {
            TRC_1PRINT (TRC_LEVEL4,
                        "ZCPYDATA Ctrl GPP address [0x%x]\n",
                        zcpyDataState->ctrlPtr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "ZCPYDATA Ctrl DSP address [0x%x]\n",
                        *shDspAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "ZCPYDATA Ctrl size        [0x%x]\n",
                        zcpyDataState->ctrlSize) ;

            ctrlPtr = zcpyDataState->ctrlPtr ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            for (i = 0 ; i < MAX_SUPPORTED_CHANNELS ; i++) {
                ctrlPtr->gppFreeMask [i].bitValue = 0x0 ;
                ctrlPtr->dspFreeMask [i].bitValue = 0x0 ;
                ctrlPtr->gppOutputFull [i].bitValue = 0x0 ;
                ctrlPtr->dspOutputFull [i].bitValue = 0x0 ;
            }
            /*  ----------------------------------------------------------------
             *  Create the freeChirps list in shared memory.
             *  ----------------------------------------------------------------
             */
            LDRV_MPLIST_clear (dspId, (List *) &(ctrlPtr->freeChirps)) ;
            mpcsObj = (MPCS_ShObj *) &(ctrlPtr->csFreeChirps) ;
            status  = LDRV_MPCS_create (dspId, mpcsObj) ;
            if (DSP_SUCCEEDED (status)) {
                /*  Initialize the free CHIRP list with the queuePerChnl number
                 *  of CHIRPS.
                 */
                freeChirps = (ZCPYDATA_ChnlIrp *)
                                 (((Uint32) ctrlPtr) + sizeof (ZCPYDATA_Ctrl)) ;
                TRC_1PRINT (TRC_LEVEL4,
                            "ZCPYDATA freeChirps address [0x%x]\n",
                            freeChirps) ;

                LDRV_MPCS_enter (mpcsObj) ;

                for (i = 0 ;
                     i < (dataObject->queuePerChnl * dataObject->numChannels) ;
                     i++) {
                    LDRV_MPLIST_putTail (dspId,
                                         (List *)        &(ctrlPtr->freeChirps),
                                         (ListElement *) &(freeChirps [i])) ;
                }
                LDRV_MPCS_leave (mpcsObj) ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            /*  ----------------------------------------------------------------
             *  Initialize shared lists and MPCS objects for the data driver.
             *  ----------------------------------------------------------------
             */
            LDRV_MPLIST_clear (dspId, (List *) &(ctrlPtr->toDspList)) ;
            LDRV_MPLIST_clear (dspId, (List *) &(ctrlPtr->fmDspList)) ;

            /*  Initialize the shared critical section objects */
            status = LDRV_MPCS_create (dspId,
                                       (MPCS_ShObj *) &(ctrlPtr->csToDspList)) ;
            if (DSP_SUCCEEDED (status)) {
                status = LDRV_MPCS_create (dspId,
                                       (MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            /*  ----------------------------------------------------------------
             *  Create the DPC Object.
             *  ----------------------------------------------------------------
             */
            status = DPC_Create (ZCPYDATA_dpc,
                                 zcpyDataState,
                                 &(zcpyDataState->dpcObj)) ;
            if (DSP_SUCCEEDED (status)) {
                /* -------------------------------------------------------------
                 * Register Callback function with IPS component
                 * -------------------------------------------------------------
                 */
                status = IPS_register (dspId,
                                       dataObject->ipsId,
                                       dataObject->ipsEventNo,
                                       &ZCPYDATA_callback,
                                       zcpyDataState) ;
                if (DSP_SUCCEEDED (status)) {
                    status = IPS_setReserved (dspId,
                                              dataObject->ipsId,
                                              dataObject->ipsEventNo) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }

        ZCPYDATA_IsInitialized [dspId][dataDrvId] = TRUE ;
        if (DSP_FAILED (status)) {
            /* ZCPYDATA_exit executes cleanup only if the initialized flag is
             * TRUE.
             */
            ZCPYDATA_exit (dspId, dataDrvId) ;
            ZCPYDATA_IsInitialized [dspId][dataDrvId] = FALSE ;
        }
    }

    TRC_1LEAVE ("ZCPYDATA_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   ZCPYDATA_exit
 *
 *  @desc   This function releases the resources allocated earlier by this
 *          ZCPY data driver in call to ZCPYDATA_init ().
 *
 *  @modif  ZCPYDATA_State [dspId][dataDrvId]
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYDATA_exit (IN  ProcessorId dspId,
               IN  Uint32      dataDrvId)
{
    DSP_STATUS         status      = DSP_SOK ;
    DSP_STATUS         tmpStatus   = DSP_SOK ;
    ZCPYDATA_Ctrl *    ctrlPtr     = NULL ;
    ZCPYDATA_Object *  zcpyDataState ;
    Uint8              i ;

    TRC_2ENTER ("ZCPYDATA_exit", dspId, dataDrvId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (ZCPYDATA_IsInitialized [dspId][dataDrvId] == TRUE) {
        zcpyDataState = &(ZCPYDATA_State [dspId][dataDrvId]) ;

        /* ---------------------------------------------------------------------
         * Un-Register Callback function with IPS component
         * ---------------------------------------------------------------------
         */
        status = IPS_unregister (dspId,
                                 zcpyDataState->dataCfgObject->ipsId,
                                 zcpyDataState->dataCfgObject->ipsEventNo,
                                 &ZCPYDATA_callback,
                                 zcpyDataState) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }

        tmpStatus = IPS_clearReserved (dspId,
                                       zcpyDataState->dataCfgObject->ipsId,
                                       zcpyDataState->dataCfgObject->ipsEventNo) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        /*  --------------------------------------------------------------------
         * Cancel the DPC and delete the DPC.
         *  --------------------------------------------------------------------
         */
        if (zcpyDataState->dpcObj != NULL) {
            tmpStatus = DPC_Cancel (zcpyDataState->dpcObj) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            tmpStatus = DPC_Delete (zcpyDataState->dpcObj) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            zcpyDataState->dpcObj = NULL ;
        }

        ctrlPtr = zcpyDataState->ctrlPtr ;
        if (ctrlPtr != NULL) {
            /*  ----------------------------------------------------------------
             *  Finalize shared lists and MPCS objects for the data driver.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_MPCS_delete (dspId,
                                       (MPCS_ShObj *) &(ctrlPtr->csToDspList)) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            tmpStatus = LDRV_MPCS_delete (dspId,
                                       (MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            LDRV_MPLIST_clear (dspId, (List *) &(ctrlPtr->toDspList)) ;
            LDRV_MPLIST_clear (dspId, (List *) &(ctrlPtr->fmDspList)) ;

            tmpStatus = LDRV_MPCS_delete (dspId,
                                      (MPCS_ShObj *) &(ctrlPtr->csFreeChirps)) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            LDRV_MPLIST_clear (dspId, (List *) &(ctrlPtr->freeChirps)) ;

            /*  ----------------------------------------------------------------
             *  Clear the data driver shared memory control structure.
             *  ----------------------------------------------------------------
             */
            for (i = 0 ; i < MAX_SUPPORTED_CHANNELS ; i++) {
                ctrlPtr->gppFreeMask [i].bitValue = 0x0 ;
                ctrlPtr->dspFreeMask [i].bitValue = 0x0 ;
                ctrlPtr->gppOutputFull [i].bitValue = 0x0 ;
                ctrlPtr->dspOutputFull [i].bitValue = 0x0 ;
            }
        }

        /*  --------------------------------------------------------------------
         *  Free shared memory required for the ZCPY DATA component.
         *  --------------------------------------------------------------------
         */
        if (zcpyDataState->ctrlPtr != NULL) {
            tmpStatus = LDRV_SMM_free (dspId,
                                       zcpyDataState->dataCfgObject->memEntry,
                                       zcpyDataState->ctrlSize) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
            zcpyDataState->ctrlPtr = NULL ;
        }

        /*  --------------------------------------------------------------------
         *  Clear other fields of the ZCPY Data driver state object.
         *  --------------------------------------------------------------------
         */
        zcpyDataState->lastOutput        = 0x0 ;
        zcpyDataState->outputMask        = 0x0 ;
        zcpyDataState->ongoingOutputMask = 0x0 ;

        ZCPYDATA_IsInitialized [dspId][dataDrvId] = FALSE ;
    }

    TRC_1LEAVE ("ZCPYDATA_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   ZCPYDATA_openChannel
 *
 *  @desc   This function opens a channel to be used for data transfer.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYDATA_openChannel (IN ProcessorId dspId, IN ChannelId chnlId)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("ZCPYDATA_openChannel", dspId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (IS_VALID_CHNLID (dspId, chnlId)) ;

    status = ZCPYDATA_clearChannelInfo (dspId, chnlId) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("ZCPYDATA_openChannel", status) ;

    return status ;
}


/** ============================================================================
 *  @func   ZCPYDATA_closeChannel
 *
 *  @desc   This function closes the physical channel corresponding to the
 *          specified logical channel.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYDATA_closeChannel (IN ProcessorId dspId, IN ChannelId chnlId)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("ZCPYDATA_closeChannel", dspId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (IS_VALID_CHNLID (dspId, chnlId)) ;

    status = ZCPYDATA_clearChannelInfo (dspId, chnlId) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("ZCPYDATA_closeChannel", status) ;

    return status ;
}


/** ============================================================================
 *  @func   ZCPYDATA_cancelIo
 *
 *  @desc   This function cancels IO on a channel.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYDATA_cancelIo (IN ProcessorId dspId, IN ChannelId chnlId)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("ZCPYDATA_cancelIo", dspId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (IS_VALID_CHNLID (dspId, chnlId)) ;

    status = ZCPYDATA_clearChannelInfo (dspId, chnlId) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("ZCPYDATA_cancelIo", status) ;

    return status ;
}

/** ============================================================================
 *  @func   ZCPYDATA_request
 *
 *  @desc   This function submits an IO request to the DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYDATA_request (IN ProcessorId dspId, IN ChannelId chnlId)
{
    DSP_STATUS        status = DSP_SOK ;
    ChannelMode       chnlMode ;
    ZCPYDATA_Object * zcpyDataState ;
    Uint32            dataDrvId ;

    TRC_2ENTER ("ZCPYDATA_request", dspId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (IS_VALID_CHNLID (dspId, chnlId)) ;

    status = LDRV_DATA_getDataDrvId (dspId, chnlId, &dataDrvId) ;
    if (DSP_SUCCEEDED (status)) {
        zcpyDataState = &(ZCPYDATA_State [dspId][dataDrvId]) ;

        chnlMode = LDRV_CHNL_getChannelMode (dspId, chnlId) ;

        /*  Adjust the channel ID to the actual channel ID. */
        chnlId = chnlId - zcpyDataState->dataCfgObject->baseChnlId ;

        if (chnlMode == ChannelMode_Input) {
            /*  ----------------------------------------------------------------
             *  Input Channel
             *  ----------------------------------------------------------------
             */
            TRC_0PRINT (TRC_LEVEL1, "INPUT Channel. Writing free mask\n") ;

            zcpyDataState->ctrlPtr->gppFreeMask [chnlId].bitValue = 1 ;

            /* Notify DSP that GPP is ready for transfer */
            status = IPS_notify (dspId,
                                 zcpyDataState->dataCfgObject->ipsId,
                                 zcpyDataState->dataCfgObject->ipsEventNo,
                                 (Uint16) 0,
                                 FALSE) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else if (chnlMode == ChannelMode_Output) {
            /*  ----------------------------------------------------------------
             *  Output Channel
             *  ----------------------------------------------------------------
             */
            TRC_0PRINT (TRC_LEVEL1, "OUTPUT Channel. Writing avail mask\n") ;

            SET_BIT (zcpyDataState->outputMask, chnlId)  ;

            /* Schedule DPC for output data */
            status = DPC_Schedule (zcpyDataState->dpcObj) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("ZCPYDATA_request", status) ;

    return status ;
}


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   ZCPYDATA_debug
 *
 *  @desc   Prints information related to ZCPY Link Driver.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Void
ZCPYDATA_debug (IN  ProcessorId dspId,
                IN  Uint32      dataDrvId)
{
    ZCPYDATA_Object * zcpyDataState ;
    Uint8             i             ;

    TRC_2ENTER ("ZCPYDATA_debug", dspId, dataDrvId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (ZCPYDATA_IsInitialized [dspId][dataDrvId] == TRUE) {
        zcpyDataState = &(ZCPYDATA_State [dspId][dataDrvId]) ;

        /*  --------------------------------------------------------------------
         *  Print the ZCPY Data driver state object information.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_0PRINT (TRC_LEVEL4, "ZCPY DATA state object information:\n") ;
        TRC_1PRINT (TRC_LEVEL4, "    dspId             = [0x%x]\n", dspId) ;
        TRC_1PRINT (TRC_LEVEL4, "    dataDrvId         = [0x%x]\n", dataDrvId) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    wordSwap          = [0x%x]\n",
                    zcpyDataState->wordSwap) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dpcObj            = [0x%x]\n",
                    zcpyDataState->dpcObj) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ctrlPtr           = [0x%x]\n",
                    zcpyDataState->ctrlPtr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ctrlSize          = [0x%x]\n",
                    zcpyDataState->ctrlSize) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dataCfgObject     = [0x%x]\n",
                    zcpyDataState->dataCfgObject) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    lastOutput        = [0x%x]\n",
                    zcpyDataState->lastOutput) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    outputMask        = [0x%x]\n",
                    zcpyDataState->outputMask) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ongoingOutputMask = [0x%x]\n",
                    zcpyDataState->ongoingOutputMask) ;

        /*  --------------------------------------------------------------------
         *  Print the ZCPY Data driver shared memory control information
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_0PRINT (TRC_LEVEL4, "ZCPY Data shared control information:\n") ;
        for (i = 0 ; i < MAX_SUPPORTED_CHANNELS ; i++) {
            TRC_2PRINT (TRC_LEVEL4,
                        "    gppFreeMask [%d]      = [0x%x]\n",
                        i, zcpyDataState->ctrlPtr->gppFreeMask [i].bitValue) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    dspFreeMask [%d]       = [0x%x]\n",
                        i, zcpyDataState->ctrlPtr->dspFreeMask [i].bitValue) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    gppOutputFull [%d]     = [0x%x]\n",
                        i, zcpyDataState->ctrlPtr->gppOutputFull [i].bitValue) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    dspOutputFull [%d]     = [0x%x]\n",
                        i, zcpyDataState->ctrlPtr->dspOutputFull [i].bitValue) ;
        }
    }
    else {
        TRC_2PRINT (TRC_LEVEL4,
                    "ZCPY DATA component for DSP ID %d Data driver ID %d"
                    " is not initialized.\n",
                    dspId,
                    dataDrvId) ;
    }

    TRC_0LEAVE ("ZCPYDATA_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_callback
 *
 *  @desc   This function implements the callback invoked by the IPS
 *          component on receiving a data transfer request from the remote
 *          processor.
 *
 *          info == '0' since the DSP side driver doesn't have any payload for
 *                      data streaming.
 *
 *          Internally ptr is used for determining two modes in which this
 *          function can be invoked. 'ptr' is obtained from the driver-list
 *          fmDspList.
 *
 *          if 'ptr' is non NULL - it means that the DSP has sent a chirp for
 *          input/ output. This function queues the chirp locally and schedules
 *          a DPC for processing these chirps.
 *
 *          if 'ptr' is NULL - it means that the DSP is initiating a transfer
 *          on GPP output channel. This case occurs when GPP became ready
 *          on output channel before the DSP was ready.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Void
ZCPYDATA_callback (IN Uint32 ipsEventNo, IN     Pvoid arg, IN     Pvoid info)
{
    ZCPYDATA_Object * dataObj = (ZCPYDATA_Object *) arg ;

    TRC_3ENTER ("ZCPYDATA_callback", ipsEventNo, arg, info) ;

    DBC_Require (arg != NULL) ;

    /* Schedule DPC for data transfer */
    DPC_Schedule (dataObj->dpcObj) ;

    TRC_0LEAVE ("ZCPYDATA_callback") ;
}


/**  ---------------------------------------------------------------------------
 *  @func   ZCPYDATA_dpc
 *
 *  @desc   This function is the delayed procedure for performing actual data
 *          transfer on the physical link between GPP and target DSP.
 *          It runs at a priority level between interrupt service routines and
 *          normal tasks/processes/threads.
 *
 *          This function performs the following on:
 *
 *          Input channel:
 *              DSP had initiated data transfer by sending a chirp (handled in
 *              _Callback). This function completes the pointer exchange by:
 *              1. Getting a 'request' chirp on the channel.
 *              2. Sending it to the DSP to complete the pointer exchange.
 *              3. Copy the data in received chirp to the 'request' chirp.
 *              4. Completing the IO for the channel with the 'request' chirp.
 *
 *          Output channel:
 *              If a valid chirp is received on output channel, it means that
 *              the pointer exchange is complete. This function completes
 *              pointer exchange by:
 *              1. Getting a 'request' chirp on the channel.
 *              2. Copy the data in received chipr to the 'request' chirp.
 *              3. Completing the IO for the channel with the 'request' chirp.
 *
 *              This function also handles the case where the DSP has sent
 *              only a notification to indicate that it is ready on its input
 *              channel.
 *
 *          This function also initiates data transfer on its output channel
 *          after ensuring that both GPP and DSP are ready on a channel.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Void
ZCPYDATA_dpc (IN Pvoid refData)
{
    ZCPYDATA_Object *  zcpyDataState = (ZCPYDATA_Object *) refData ;
    LDRV_CHNL_Irp   *  chirp         = NULL ;
    LDRV_CHNL_Irp   *  inpChirp      = NULL ;
    DSP_STATUS         status        = DSP_SOK ;
    ZCPYDATA_Ctrl *    ctrlPtr ;
    LINKCFG_Dsp *      dspObj ;
    ProcessorId        dspId ;
    ChannelId          chnlId ;
    CHNL_State         chnlState ;
    ChannelMode        chnlMode ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_1ENTER ("ZCPYDATA_dpc", refData) ;

    DBC_Require (refData != NULL) ;

    dspId   = zcpyDataState->dspId ;
    ctrlPtr = zcpyDataState->ctrlPtr ;
    dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj   = dspCfg->dspObject ;

    while (!LDRV_MPLIST_isEmpty (zcpyDataState->dspId,
                                 (List *) &(ctrlPtr->fmDspList))) {
        LDRV_MPCS_enter ((MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;
        LDRV_MPLIST_getHead  (zcpyDataState->dspId,
                              (List *) &(ctrlPtr->fmDspList),
                              (ListElement **) &chirp) ;
        LDRV_MPCS_leave ((MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;

        if (chirp != NULL) {
            /* Convert the buffer pointer to GPP address space. */
            chirp->buffer = DSP_addrConvert (zcpyDataState->dspId,
                                             SWAP_LONG (chirp->buffer,
                                                       zcpyDataState->wordSwap),
                                             DspToGpp) ;

            /* Adjust the buffer size using the DSP MADU size */
            chirp->size =  SWAP_LONG (chirp->size, zcpyDataState->wordSwap)
                         * dspObj->maduSize ;
            chirp->iocStatus = SWAP_LONG (chirp->iocStatus,
                                          dspObj->wordSwap) ;
            chirp->chnlId = SWAP_LONG (chirp->chnlId, zcpyDataState->wordSwap) ;

            chnlId = chirp->chnlId ;
            DBC_Assert (IS_VALID_CHNLID (dspId, chnlId)) ;

#if defined (LOG_DG_CHNL_XFER_PROCESSING)
            /* Log the event */
            DSPLINKLOG_LogEvent (DG_CHNL_XFER_PROCESSING,
                                 0,
                                 (((dspId << 16) & 0xFFFF0000) | chnlId ),
                                 chirp->buffer,
                                 chirp->size,
                                 0,
                                 0) ;
#endif /* if defined (LOG_DG_CHNL_XFER_PROCESSING) */

            chnlState = LDRV_CHNL_getChannelState (dspId, chnlId) ;
            chnlMode = LDRV_CHNL_getChannelMode (dspId, chnlId) ;
            inpChirp = LDRV_CHNL_getRequestChirp (dspId, chnlId) ;
            if (inpChirp != NULL) {

                /*  ------------------------------------------------------------
                 *  If the channel is an input channel, send a CHIRP
                 *  from channel to complete buffer exchange.
                 *  ------------------------------------------------------------
                 */
                if (LDRV_CHNL_hasMoreChirps (dspId, chnlId) == FALSE) {
                    if (chnlMode == ChannelMode_Input) {
                        ctrlPtr->gppFreeMask [chnlId].bitValue = 0 ;
                    }
                    else {
                        CLEAR_BIT(zcpyDataState->outputMask, chnlId) ;
                    }
                }

                if (chnlMode == ChannelMode_Input) {
                    ctrlPtr->dspOutputFull [chnlId].bitValue = 0 ;
                    ZCPYDATA_send (zcpyDataState, inpChirp);
                }
                else {
                    /* Indicate that this data transfer on this channel
                     * is complete
                     */
                    CLEAR_BIT (zcpyDataState->ongoingOutputMask, chnlId) ;
                }

                inpChirp->buffer = chirp->buffer ;
                inpChirp->chnlId = chirp->chnlId ;
                inpChirp->size   = chirp->size   ;
                if (inpChirp->size == 0) {
                    /*  --------------------------------------------------------
                     *  Buffer length of zero bytes indicates EOS.
                     *  This Asertion fails if DSP send EOS more
                     *  than once on a given channel
                     *  --------------------------------------------------------
                     */
                    DBC_Assert ((chnlState & ChannelState_EOS) == 0) ;

                    /*  --------------------------------------------------------
                     *  Update channel state & IO Completion status
                     *  --------------------------------------------------------
                     */
                    chnlState           = ChannelState_EOS ;
                    inpChirp->iocStatus = LDRV_CHNL_IOCSTATE_EOS ;

                    LDRV_CHNL_setChannelState (dspId,
                                               chnlId,
                                               chnlState) ;
                }
                else {
                    inpChirp->iocStatus = LDRV_CHNL_IOCSTATE_COMPLETE ;
                }

#if defined (LOG_DG_CHNL_XFER_COMPLETE)
                /* Log the event */
                DSPLINKLOG_LogEvent (
                        DG_CHNL_XFER_COMPLETE,
                        0,
                        (((dspId << 16) & 0xFFFF0000) | (chnlId & 0xFFFF)),
                        chirp->buffer,
                        chirp->size,
                        0,
                        0) ;
#endif /* if defined (LOG_DG_CHNL_XFER_COMPLETE) */
                LDRV_CHNL_addIOCompletion (dspId, chnlId, inpChirp) ;
                /*  ------------------------------------------------------------
                 *  Send the used chirp back to the free list
                 *  ------------------------------------------------------------
                 */
                LDRV_MPCS_enter ((MPCS_ShObj *) &(ctrlPtr->csFreeChirps)) ;
                LDRV_MPLIST_putTail  (zcpyDataState->dspId,
                                  (List *) &(ctrlPtr->freeChirps),
                                  (ListElement *) chirp) ;
                LDRV_MPCS_leave ((MPCS_ShObj *) &(ctrlPtr->csFreeChirps)) ;

            }
            else {
                DBC_Assert (   (chnlState == ChannelState_Closed)
                            || (   (chnlState & ChannelState_EOS)
                                == ChannelState_EOS)
                            || (chnlState == ChannelState_Idled)) ;
                /* This condtion is possible only for input channel.
                 * For Output channel this condition should not come.
                 */
                if (   (chnlState == ChannelState_Closed)
                    || ((chnlState & ChannelState_EOS) == ChannelState_EOS)
                    || (chnlState == ChannelState_Idled)) {
                    DBC_Assert (chnlMode == ChannelMode_Input) ;
                    if (chnlMode == ChannelMode_Input) {
                        /*  ----------------------------------------------------
                         *  Race condition handling: Channel was closed after
                         *  DSP checked and sent a chirp to GPP.
                         *  ----------------------------------------------------
                         */
                        chirp->iocStatus = LDRV_CHNL_IOCSTATE_CANCELED ;
                        ZCPYDATA_send (zcpyDataState, chirp);
                    }
                }
            }
        }
    }

    /*  ------------------------------------------------------------------------
     *  Initiate the next data transfer on an output channel.
     *  ------------------------------------------------------------------------
     */
    chnlId = ZCPYDATA_getNextOutputChannel (dspId,
                                      zcpyDataState,
                                      (  zcpyDataState->outputMask
                                       & ~(zcpyDataState->ongoingOutputMask))) ;

    if (chnlId != CHNLID_INVALID) {
        if (ctrlPtr->gppOutputFull [chnlId].bitValue == 0) {
            /*  ----------------------------------------------------------------
             *  Get the information about the next chirp.
             *  ----------------------------------------------------------------
             */
            chirp = LDRV_CHNL_nextRequestChirp (dspId, chnlId) ;

            /*  ----------------------------------------------------------------
             *  Send the CHIRP to DSP.
             *  ----------------------------------------------------------------
             */
#if defined (LOG_GD_CHNL_XFER_START)
            /* Log the event */
            DSPLINKLOG_LogEvent (GD_CHNL_XFER_START,
                          0,
                          (((dspId << 16) & 0xFFFF0000) | (chnlId & 0xFFFF)),
                          (Uint32) chirp->buffer,
                          chirp->size,
                          0,
                          0) ;
#endif /* if defined (LOG_GD_CHNL_XFER_START) */

            ctrlPtr->gppOutputFull [chnlId].bitValue = 1 ;

            /* Indicate that the transfer on this channel is ongoing.
             * This channel would not be considered for next output
             * till the previous data transfer on it is complete.
             */
            SET_BIT (zcpyDataState->ongoingOutputMask, chnlId) ;

            status = ZCPYDATA_send (zcpyDataState, chirp) ;
            if (DSP_FAILED (status)) {
                 /* Failed to send  the chirp to DSP */
                 /* Reset the gppOutputFull and ongoingOutputMask fields */
                 ctrlPtr->gppOutputFull [chnlId].bitValue = 0 ;
                 CLEAR_BIT (zcpyDataState->ongoingOutputMask, chnlId) ;
                 /* Reset the chirp status to LDRV_CHNL_IOCSTATUS_PENDING*/
                 chirp->iocStatus = LDRV_CHNL_IOCSTATE_PENDING ;
            }
            else {
                 /* Change the iocstatus of the chirp to
                  * LDRV_CHNL_IOCSTATUS_INPROGRESS
                  */
                chirp->iocStatus = LDRV_CHNL_IOCSTATE_INPROGRESS ;
            }
        }
    }

    TRC_0LEAVE ("ZCPYDATA_dpc") ;
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_getNextOutputChannel
 *
 *  @desc   Find the channel that has data to send to DSP.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
ChannelId
ZCPYDATA_getNextOutputChannel (IN  ProcessorId       dspId,
                               IN  ZCPYDATA_Object * zcpyDataState,
                               IN  Uint32            mask)
{
    Bool            done     = FALSE ;
    ChannelId       selectId = CHNLID_INVALID ;
    ZCPYDATA_Ctrl * ctrlPtr ;
    ChannelId       startId ;

    TRC_3ENTER ("ZCPYDATA_getNextOutputChannel", dspId, zcpyDataState, mask) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    ctrlPtr = (zcpyDataState->ctrlPtr) ;

    if (mask != 0) {
        startId  = zcpyDataState->lastOutput ;
        selectId = startId ;
        do {
            selectId = selectId + 1 ;
            if (selectId == zcpyDataState->dataCfgObject->numChannels) {
                selectId = 0 ;
            }

            if (   ((mask & (1 << selectId)) != 0)
                && (ctrlPtr->dspFreeMask [selectId].bitValue == 1)){
                zcpyDataState->lastOutput = selectId ;
                done = TRUE ;
            }
        } while ((selectId != startId) && (done == FALSE)) ;
    }

    if (done == FALSE) {
        selectId = CHNLID_INVALID ;
    }

    TRC_1LEAVE ("ZCPYDATA_getNextOutputChannel", selectId) ;

    return selectId ;
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_clearChannelInfo
 *
 *  @desc   Clear the channel information based on its mode.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
ZCPYDATA_clearChannelInfo (IN  ProcessorId dspId,
                           IN  ChannelId   chnlId)
{
    DSP_STATUS        status = DSP_SOK ;
    ChannelMode       chnlMode ;
    Uint32            dataDrvId ;
    ZCPYDATA_Object * zcpyDataState ;

    TRC_2ENTER ("ZCPYDATA_clearChannelInfo", dspId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (IS_VALID_CHNLID (dspId, chnlId)) ;

    status = LDRV_DATA_getDataDrvId (dspId, chnlId, &dataDrvId) ;
    if (DSP_SUCCEEDED (status)) {
        zcpyDataState = &(ZCPYDATA_State [dspId][dataDrvId]) ;

        /* Adjust the channel ID to the actual channel ID. */
        chnlId = chnlId - zcpyDataState->dataCfgObject->baseChnlId ;

        chnlMode = LDRV_CHNL_getChannelMode (dspId, chnlId) ;

        if (chnlMode == ChannelMode_Input) {
            zcpyDataState->ctrlPtr->gppFreeMask [chnlId].bitValue = 0 ;
        }
        else if (chnlMode == ChannelMode_Output) {
            CLEAR_BIT (zcpyDataState->outputMask, chnlId) ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("ZCPYDATA_clearChannelInfo", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_send
 *
 *  @desc   Sends the specified data buffer to the DSP.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
ZCPYDATA_send (ZCPYDATA_Object * zcpyDataState, IN Pvoid buffer)
{
    DSP_STATUS       status = DSP_SOK ;
    LDRV_CHNL_Irp *  fmChirp ;
    LDRV_CHNL_Irp *  chirp ;
    ProcessorId      dspId ;
    ZCPYDATA_Ctrl *  ctrlPtr ;
    LINKCFG_Dsp *    dspObj ;
    LINKCFG_DspConfig * dspCfg ;

    TRC_2ENTER ("ZCPYDATA_send", zcpyDataState, buffer) ;

    DBC_Require (buffer != NULL) ;

    dspId   = zcpyDataState->dspId ;
    dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj   = dspCfg->dspObject ;
    ctrlPtr = zcpyDataState->ctrlPtr ;

    /*  Get a free CHIRP from the shared memory */
    LDRV_MPCS_enter ((MPCS_ShObj *) &(ctrlPtr->csFreeChirps)) ;
    LDRV_MPLIST_getHead (dspId,
                         (List *) &(ctrlPtr->freeChirps),
                         (ListElement **) &chirp) ;
    LDRV_MPCS_leave ((MPCS_ShObj *) &(ctrlPtr->csFreeChirps)) ;

    DBC_Assert (chirp != NULL) ;

    /* Initialize the CHIRP with buffer details */
    fmChirp = (LDRV_CHNL_Irp *) buffer ;
    chirp->arg       = fmChirp->arg ;
    chirp->iocStatus = SWAP_LONG (fmChirp->iocStatus, dspObj->wordSwap) ;

    chirp->buffer = SWAP_LONG (DSP_addrConvert (dspId,
                                                fmChirp->buffer,
                                                GppToDsp),
                               zcpyDataState->wordSwap) ;
    /* Adjust the buffer size using the DSP MADU size */
    chirp->size   = SWAP_LONG ((fmChirp->size / dspObj->maduSize),
                               dspObj->wordSwap) ;
    chirp->chnlId = SWAP_LONG (fmChirp->chnlId, dspObj->wordSwap) ;

    /*  Send the buffer to the DSP */
    LDRV_MPCS_enter ((MPCS_ShObj *) &(ctrlPtr->csToDspList)) ;
    LDRV_MPLIST_putTail  (dspId,
                          (List *) &(ctrlPtr->toDspList),
                          (ListElement *) chirp) ;

    status = IPS_notify (dspId,
                         zcpyDataState->dataCfgObject->ipsId,
                         zcpyDataState->dataCfgObject->ipsEventNo,
                         (Uint16) 0,
                         FALSE) ;
    if (DSP_FAILED (status)) {
        /* Remove the queued data buffer on failure */
        LDRV_MPLIST_removeElement  (dspId,
                                    (List *) &(ctrlPtr->toDspList),
                                    (ListElement *) chirp) ;
        SET_FAILURE_REASON ;
    }

    LDRV_MPCS_leave ((MPCS_ShObj *) &(ctrlPtr->csToDspList)) ;

    TRC_1LEAVE ("ZCPYDATA_send", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* if defined (__cplusplus) */
