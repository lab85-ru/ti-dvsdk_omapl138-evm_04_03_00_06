/** ============================================================================
 *  @file   dcpy_data.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/DATA/
 *
 *  @desc   Implementation of the DCPY Data driver using Shared Memory.
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
#include <dcpy_data.h>
#include <ldrv_mplist.h>
#include <ldrv_mpcs.h>
#include <ldrv_pool.h>
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
#define CHNLID_INVALID  (ChannelId) 0xFFFF


/** ============================================================================
 *  @name   DCPYDATA_Object
 *
 *  @desc   Defines the DCPY Data driver object, which contains all the
 *          component-specific information.
 *
 *  @field  dspId
 *              Processor Identifier.
 *  @field  dataDrvId
 *              Data driver Identifier.
 *  @field  wordSwap
 *              Indicates whether word-swap is enabled for the DSP MMU.
 *  @field  dpcObj
 *              DPC object for the the DCPY data driver.
 *  @field  ctrlPtr
 *              Pointer to shared memory control structure.
 *  @field  ctrlSize
 *              Size of the DCPY Data driver shared memory control region.
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
typedef struct DCPYDATA_Object_tag {
    Uint32              dspId ;
    Uint32              dataDrvId ;
    Bool                wordSwap ;
    DpcObject *         dpcObj  ;
    DCPYDATA_Ctrl *     ctrlPtr ;
    Uint32              ctrlSize ;
    LINKCFG_DataDrv *   dataCfgObject ;
    Uint32              lastOutput ;
    Uint32              outputMask ;
    Uint32              ongoingOutputMask ;
} DCPYDATA_Object  ;


/** ----------------------------------------------------------------------------
 *  @name   DCPYDATA_State
 *
 *  @desc   State object for the IPS component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC DCPYDATA_Object DCPYDATA_State [MAX_DSPS] [MAX_DCPY_DATA] ;

/*  ----------------------------------------------------------------------------
 *  @name   DCPYDATA_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool DCPYDATA_IsInitialized [MAX_DSPS] [MAX_DCPY_DATA] ;


/** ============================================================================
 *  @name   DCPYDATA_Interface
 *
 *  @desc   DCPY Data Driver interface.
 *  ============================================================================
 */
DATA_Interface DCPYDATA_Interface = {
    &DCPYDATA_init,
    &DCPYDATA_exit,
    &DCPYDATA_openChannel,
    &DCPYDATA_closeChannel,
    &DCPYDATA_cancelIo,
    &DCPYDATA_request
#if defined (DDSP_DEBUG)
    ,&DCPYDATA_debug
#endif /* if defined (DDSP_DEBUG) */
} ;


/** ----------------------------------------------------------------------------
 *  @func   DCPYDATA_callback
 *
 *  @desc   This function implements the callback invoked by the DCPY DATA
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
DCPYDATA_callback (IN Uint32 ipsEventNo, IN     Pvoid arg, IN     Pvoid info) ;


/** ----------------------------------------------------------------------------
 *  @func   DCPYDATA_send
 *
 *  @desc   Sends the specified data buffer to the DSP.
 *
 *  @arg    dcpyDataState
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
DCPYDATA_send (IN DCPYDATA_Object * dcpyDataState, IN Pvoid buffer) ;


/** ----------------------------------------------------------------------------
 *  @func   DCPYDATA_dpc
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
 *  @see    DCPYDATA_init
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Void
DCPYDATA_dpc (IN Pvoid refData) ;


/** ----------------------------------------------------------------------------
 *  @func   DCPYDATA_getNextOutputChannel
 *
 *  @desc   Find the channel that has data to send to DSP.
 *
 *  @arg    dspId
 *              Processor idenfitier.
 *  @arg    dcpyDataState
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
DCPYDATA_getNextOutputChannel (IN  ProcessorId       dspId,
                               IN  DCPYDATA_Object * dcpyDataState,
                               IN  Uint32            mask) ;


/** ----------------------------------------------------------------------------
 *  @func   DCPYDATA_clearChannelInfo
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
 *  @see    DCPYDATA_openChannel (), DCPYDATA_closeChannel (),
 *          DCPYDATA_cancelIo (),
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
DCPYDATA_clearChannelInfo (IN  ProcessorId dspId,
                           IN  ChannelId   chnlId) ;


/** ============================================================================
 *  @func   DCPYDATA_init
 *
 *  @desc   This function allocates and initializes resources used by the
 *          DCPY Data Driver.
 *
 *  @modif  DCPYDATA_State [dspId][dataDrvId]
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYDATA_init (IN ProcessorId dspId,
               IN  Uint32     dataDrvId,
               OUT Uint32 *   shDspAddr)
{
    DSP_STATUS          status  = DSP_SOK ;
    DCPYDATA_Ctrl *     ctrlPtr = NULL    ;
    LINKCFG_Dsp *       dspObj            ;
    LINKCFG_LinkDrv *   linkDrv           ;
    LINKCFG_DataDrv *   dataObject        ;
    DCPYDATA_Object *   dcpyDataState     ;
    MPCS_ShObj *        mpcsObj           ;
    DCPYDATA_ChnlIrp *  freeChirps        ;
    Uint32              i                 ;
    LINKCFG_DspConfig * dspCfg            ;

    TRC_3ENTER ("DCPYDATA_init", dspId, dataDrvId, shDspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (shDspAddr != NULL) ;

    if (DCPYDATA_IsInitialized [dspId][dataDrvId] == FALSE) {
        dcpyDataState = &(DCPYDATA_State [dspId][dataDrvId]) ;
        dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj   = dspCfg->dspObject ;
        linkDrv    = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
        dataObject = &(dspCfg->dataTables [linkDrv->dataTableId][dataDrvId]) ;

        /*  --------------------------------------------------------------------
         *  Initialize the DCPY Data driver state object.
         *  --------------------------------------------------------------------
         */
        dcpyDataState->dspId             = dspId ;
        dcpyDataState->dataDrvId         = dataDrvId ;
        dcpyDataState->wordSwap          = dspObj->wordSwap ;
        dcpyDataState->dpcObj            = NULL ;
        dcpyDataState->ctrlPtr           = NULL ;
        dcpyDataState->dataCfgObject     = dataObject ;
        dcpyDataState->lastOutput        = 0x0 ;
        dcpyDataState->outputMask        = 0x0 ;
        dcpyDataState->ongoingOutputMask = 0x0 ;

        /* Calculate the size of shared memory control region.
         * No need to align to cache-line size, since the structure sizes
         * are aligned.
         */
        dcpyDataState->ctrlSize =     sizeof (DCPYDATA_Ctrl)
                                  +   (     sizeof (DCPYDATA_ChnlIrp)
                                        *   dataObject->queuePerChnl
                                        *   dataObject->numChannels) ;

        /*  --------------------------------------------------------------------
         *  Allocate shared memory required for the DCPY DATA component.
         *  --------------------------------------------------------------------
         */
        status = LDRV_SMM_alloc (dspId,
                                 dataObject->memEntry,
                                 NULL, /* Physical address not required. */
                                 shDspAddr,
                                 (Uint32 *) &(dcpyDataState->ctrlPtr),
                                 dcpyDataState->ctrlSize) ;
        if (DSP_SUCCEEDED (status)) {
            TRC_1PRINT (TRC_LEVEL4,
                        "DCPYDATA Ctrl GPP address [0x%x]\n",
                        dcpyDataState->ctrlPtr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "DCPYDATA Ctrl DSP address [0x%x]\n",
                        *shDspAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "DCPYDATA Ctrl size        [0x%x]\n",
                        dcpyDataState->ctrlSize) ;

            ctrlPtr = dcpyDataState->ctrlPtr ;
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
                freeChirps = (DCPYDATA_ChnlIrp *)
                                 (((Uint32) ctrlPtr) + sizeof (DCPYDATA_Ctrl)) ;
                TRC_1PRINT (TRC_LEVEL4,
                            "DCPYDATA freeChirps address [0x%x]\n",
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
            status = DPC_Create (DCPYDATA_dpc,
                                 dcpyDataState,
                                 &(dcpyDataState->dpcObj)) ;
            if (DSP_SUCCEEDED (status)) {
                /* -------------------------------------------------------------
                 * Register Callback function with IPS component
                 * -------------------------------------------------------------
                 */
                status = IPS_register (dspId,
                                       dataObject->ipsId,
                                       dataObject->ipsEventNo,
                                       &DCPYDATA_callback,
                                       dcpyDataState) ;
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

        DCPYDATA_IsInitialized [dspId][dataDrvId] = TRUE ;
        if (DSP_FAILED (status)) {
            /* DCPYDATA_exit executes cleanup only if the initialized flag is
             * TRUE.
             */
            DCPYDATA_exit (dspId, dataDrvId) ;
            DCPYDATA_IsInitialized [dspId][dataDrvId] = FALSE ;
        }
    }

    TRC_1LEAVE ("DCPYDATA_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DCPYDATA_exit
 *
 *  @desc   This function releases the resources allocated earlier by this
 *          DCPY data driver in call to DCPYDATA_init ().
 *
 *  @modif  DCPYDATA_State [dspId][dataDrvId]
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYDATA_exit (IN  ProcessorId dspId,
               IN  Uint32      dataDrvId)
{
    DSP_STATUS         status      = DSP_SOK ;
    DSP_STATUS         tmpStatus   = DSP_SOK ;
    DCPYDATA_Ctrl *    ctrlPtr     = NULL ;
    DCPYDATA_Object *  dcpyDataState ;
    Uint8              i ;

    TRC_2ENTER ("DCPYDATA_exit", dspId, dataDrvId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (DCPYDATA_IsInitialized [dspId][dataDrvId] == TRUE) {
        dcpyDataState = &(DCPYDATA_State [dspId][dataDrvId]) ;

        /* ---------------------------------------------------------------------
         * Un-Register Callback function with IPS component
         * ---------------------------------------------------------------------
         */
        status = IPS_unregister (dspId,
                                 dcpyDataState->dataCfgObject->ipsId,
                                 dcpyDataState->dataCfgObject->ipsEventNo,
                                 &DCPYDATA_callback,
                                 dcpyDataState) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }

        tmpStatus = IPS_clearReserved (dspId,
                                    dcpyDataState->dataCfgObject->ipsId,
                                    dcpyDataState->dataCfgObject->ipsEventNo) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        /*  --------------------------------------------------------------------
         * Cancel the DPC and delete the DPC.
         *  --------------------------------------------------------------------
         */
        if (dcpyDataState->dpcObj != NULL) {
            tmpStatus = DPC_Cancel (dcpyDataState->dpcObj) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            tmpStatus = DPC_Delete (dcpyDataState->dpcObj) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            dcpyDataState->dpcObj = NULL ;
        }

        ctrlPtr = dcpyDataState->ctrlPtr ;
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
         *  Free shared memory required for the DCPY DATA component.
         *  --------------------------------------------------------------------
         */
        if (dcpyDataState->ctrlPtr != NULL) {
            tmpStatus = LDRV_SMM_free (dspId,
                                       dcpyDataState->dataCfgObject->memEntry,
                                       dcpyDataState->ctrlSize) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
            dcpyDataState->ctrlPtr = NULL ;
        }

        /*  --------------------------------------------------------------------
         *  Clear other fields of the DCPY Data driver state object.
         *  --------------------------------------------------------------------
         */
        dcpyDataState->lastOutput        = 0x0 ;
        dcpyDataState->outputMask        = 0x0 ;
        dcpyDataState->ongoingOutputMask = 0x0 ;

        DCPYDATA_IsInitialized [dspId][dataDrvId] = FALSE ;
    }

    TRC_1LEAVE ("DCPYDATA_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DCPYDATA_openChannel
 *
 *  @desc   This function opens a channel to be used for data transfer.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYDATA_openChannel (IN ProcessorId dspId, IN ChannelId chnlId)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("DCPYDATA_openChannel", dspId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (IS_VALID_CHNLID (dspId, chnlId)) ;

    status = DCPYDATA_clearChannelInfo (dspId, chnlId) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("DCPYDATA_openChannel", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DCPYDATA_closeChannel
 *
 *  @desc   This function closes the physical channel corresponding to the
 *          specified logical channel.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYDATA_closeChannel (IN ProcessorId dspId, IN ChannelId chnlId)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("DCPYDATA_closeChannel", dspId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (IS_VALID_CHNLID (dspId, chnlId)) ;

    status = DCPYDATA_clearChannelInfo (dspId, chnlId) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("DCPYDATA_closeChannel", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DCPYDATA_cancelIo
 *
 *  @desc   This function cancels IO on a channel.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYDATA_cancelIo (IN ProcessorId dspId, IN ChannelId chnlId)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("DCPYDATA_cancelIo", dspId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (IS_VALID_CHNLID (dspId, chnlId)) ;

    status = DCPYDATA_clearChannelInfo (dspId, chnlId) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("DCPYDATA_cancelIo", status) ;

    return status ;
}

/** ============================================================================
 *  @func   DCPYDATA_request
 *
 *  @desc   This function submits an IO request to the DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYDATA_request (IN ProcessorId dspId, IN ChannelId chnlId)
{
    DSP_STATUS        status = DSP_SOK ;
    ChannelMode       chnlMode ;
    DCPYDATA_Object * dcpyDataState ;
    Uint32            dataDrvId ;

    TRC_2ENTER ("DCPYDATA_request", dspId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (IS_VALID_CHNLID (dspId, chnlId)) ;

    status = LDRV_DATA_getDataDrvId (dspId, chnlId, &dataDrvId) ;
    if (DSP_SUCCEEDED (status)) {
        dcpyDataState = &(DCPYDATA_State [dspId][dataDrvId]) ;

        chnlMode = LDRV_CHNL_getChannelMode (dspId, chnlId) ;

        /*  Adjust the channel ID to the actual channel ID. */
        chnlId = chnlId - dcpyDataState->dataCfgObject->baseChnlId ;

        if (chnlMode == ChannelMode_Input) {
            /*  ----------------------------------------------------------------
             *  Input Channel
             *  ----------------------------------------------------------------
             */
            TRC_0PRINT (TRC_LEVEL1, "INPUT Channel. Writing free mask\n") ;

            dcpyDataState->ctrlPtr->gppFreeMask [chnlId].bitValue = 1 ;

            /* Notify DSP that GPP is ready for transfer */
            status = IPS_notify (dspId,
                                 dcpyDataState->dataCfgObject->ipsId,
                                 dcpyDataState->dataCfgObject->ipsEventNo,
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

            SET_BIT (dcpyDataState->outputMask, chnlId)  ;

            /* Schedule DPC for output data */
            status = DPC_Schedule (dcpyDataState->dpcObj) ;
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

    TRC_1LEAVE ("DCPYDATA_request", status) ;

    return status ;
}


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   DCPYDATA_debug
 *
 *  @desc   Prints information related to DCPY Link Driver.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Void
DCPYDATA_debug (IN  ProcessorId dspId,
                IN  Uint32      dataDrvId)
{
    DCPYDATA_Object * dcpyDataState ;
    Uint8             i             ;

    TRC_2ENTER ("DCPYDATA_debug", dspId, dataDrvId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (DCPYDATA_IsInitialized [dspId][dataDrvId] == TRUE) {
        dcpyDataState = &(DCPYDATA_State [dspId][dataDrvId]) ;

        /*  --------------------------------------------------------------------
         *  Print the DCPY Data driver state object information.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_0PRINT (TRC_LEVEL4, "DCPY DATA state object information:\n") ;
        TRC_1PRINT (TRC_LEVEL4, "    dspId             = [0x%x]\n", dspId) ;
        TRC_1PRINT (TRC_LEVEL4, "    dataDrvId         = [0x%x]\n", dataDrvId) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    wordSwap          = [0x%x]\n",
                    dcpyDataState->wordSwap) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dpcObj            = [0x%x]\n",
                    dcpyDataState->dpcObj) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ctrlPtr           = [0x%x]\n",
                    dcpyDataState->ctrlPtr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ctrlSize          = [0x%x]\n",
                    dcpyDataState->ctrlSize) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dataCfgObject     = [0x%x]\n",
                    dcpyDataState->dataCfgObject) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    lastOutput        = [0x%x]\n",
                    dcpyDataState->lastOutput) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    outputMask        = [0x%x]\n",
                    dcpyDataState->outputMask) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ongoingOutputMask = [0x%x]\n",
                    dcpyDataState->ongoingOutputMask) ;

        /*  --------------------------------------------------------------------
         *  Print the DCPY Data driver shared memory control information
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_0PRINT (TRC_LEVEL4, "DCPY Data shared control information:\n") ;
        for (i = 0 ; i < MAX_SUPPORTED_CHANNELS ; i++) {
            TRC_2PRINT (TRC_LEVEL4,
                        "    gppFreeMask [%d]      = [0x%x]\n",
                        i, dcpyDataState->ctrlPtr->gppFreeMask [i].bitValue) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    dspFreeMask [%d]       = [0x%x]\n",
                        i, dcpyDataState->ctrlPtr->dspFreeMask [i].bitValue) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    gppOutputFull [%d]     = [0x%x]\n",
                        i, dcpyDataState->ctrlPtr->gppOutputFull [i].bitValue) ;
            TRC_2PRINT (TRC_LEVEL4,
                        "    dspOutputFull [%d]     = [0x%x]\n",
                        i, dcpyDataState->ctrlPtr->dspOutputFull [i].bitValue) ;
        }
    }
    else {
        TRC_2PRINT (TRC_LEVEL4,
                    "DCPY DATA component for DSP ID %d Data driver ID %d"
                    " is not initialized.\n",
                    dspId,
                    dataDrvId) ;
    }

    TRC_0LEAVE ("DCPYDATA_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


/** ----------------------------------------------------------------------------
 *  @func   DCPYDATA_callback
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
DCPYDATA_callback (IN Uint32 ipsEventNo, IN     Pvoid arg, IN     Pvoid info)
{
    DCPYDATA_Object * dataObj = (DCPYDATA_Object *) arg ;

    TRC_3ENTER ("DCPYDATA_callback", ipsEventNo, arg, info) ;

    DBC_Require (arg != NULL) ;

    /* Schedule DPC for data transfer */
    DPC_Schedule (dataObj->dpcObj) ;

    TRC_0LEAVE ("DCPYDATA_callback") ;
}


/**  ---------------------------------------------------------------------------
 *  @func   DCPYDATA_dpc
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
DCPYDATA_dpc (IN Pvoid refData)
{
    DCPYDATA_Object *   dcpyDataState = (DCPYDATA_Object *) refData ;
    LDRV_CHNL_Irp   *   chirp         = NULL           ;
    LDRV_CHNL_Irp   *   inpChirp      = NULL           ;
    DSP_STATUS          status        = DSP_SOK        ;
    Uint16              poolId        = POOL_INVALIDID ;
    DCPYDATA_Ctrl *     ctrlPtr                        ;
    LINKCFG_Dsp *       dspObj                         ;
    ProcessorId         dspId                          ;
    ChannelId           chnlId                         ;
    CHNL_State          chnlState                      ;
    ChannelMode         chnlMode                       ;
    LINKCFG_DspConfig * dspCfg                         ;


    TRC_1ENTER ("DCPYDATA_dpc", refData) ;

    DBC_Require (refData != NULL) ;

    dspId   = dcpyDataState->dspId ;
    ctrlPtr = dcpyDataState->ctrlPtr ;
    dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj   = dspCfg->dspObject ;

    while (!LDRV_MPLIST_isEmpty (dcpyDataState->dspId,
                                 (List *) &(ctrlPtr->fmDspList))) {
        LDRV_MPCS_enter ((MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;
        LDRV_MPLIST_getHead  (dcpyDataState->dspId,
                              (List *) &(ctrlPtr->fmDspList),
                              (ListElement **) &chirp) ;
        LDRV_MPCS_leave ((MPCS_ShObj *) &(ctrlPtr->csFmDspList)) ;

        if (chirp != NULL) {
            /* Convert the buffer pointer to GPP address space. */
            chirp->buffer = DSP_addrConvert (dcpyDataState->dspId,
                                             SWAP_LONG (chirp->buffer,
                                                       dcpyDataState->wordSwap),
                                             DspToGpp) ;

            /* Adjust the buffer size using the DSP MADU size */
            chirp->size =  SWAP_LONG (chirp->size, dcpyDataState->wordSwap)
                         * dspObj->maduSize ;
            chirp->iocStatus = SWAP_LONG (chirp->iocStatus,
                                          dspObj->wordSwap) ;
            chirp->chnlId = SWAP_LONG (chirp->chnlId, dcpyDataState->wordSwap) ;

            chnlId = chirp->chnlId ;
            status = LDRV_POOL_getPoolId (dcpyDataState->dspId,
                                          (Pvoid)chirp->buffer,
                                          AddrType_Knl,
                                          &poolId) ;
            if (IS_VALID_POOLID(poolId)) {
	            /* actual content of this chrip is pointed by chirp->buffer,
	             * Get it in GPP address space.
	             */
                LDRV_POOL_invalidate (poolId,
                                      (Pvoid) chirp->buffer,
                                      chirp->size) ;
            }

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
                /*  --------------------------------------------------------
                 *  If the channel is an input channel, send a CHIRP
                 *  from channel to complete buffer exchange.
                 *  --------------------------------------------------------
                 */
                if (LDRV_CHNL_hasMoreChirps (dspId, chnlId) == FALSE) {
                    if (chnlMode == ChannelMode_Input) {
                        ctrlPtr->gppFreeMask [chnlId].bitValue = 0 ;
                    }
                    else {
                        CLEAR_BIT(dcpyDataState->outputMask, chnlId) ;
                    }
                }

                if (chnlMode == ChannelMode_Input) {
                    ctrlPtr->dspOutputFull [chnlId].bitValue = 0 ;
                    DCPYDATA_send (dcpyDataState, inpChirp);
                }
                else {
                    /* Indicate that this data transfer on this channel
                     * is complete
                     */
                    CLEAR_BIT (dcpyDataState->ongoingOutputMask, chnlId) ;
                }

                inpChirp->buffer = chirp->buffer ;
                inpChirp->chnlId = chirp->chnlId ;
                inpChirp->size   = chirp->size   ;
                if (inpChirp->size == 0) {
                    /*  ----------------------------------------------------
                     *  Buffer length of zero bytes indicates EOS.
                     *  This Asertion fails if DSP send EOS more
                     *  than once on a given channel
                     *  ----------------------------------------------------
                     */
                    DBC_Assert ((chnlState & ChannelState_EOS) == 0) ;

                    /*  ----------------------------------------------------
                     *  Update channel state & IO Completion status
                     *  ----------------------------------------------------
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
                LDRV_MPLIST_putTail  (dcpyDataState->dspId,
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
                        DCPYDATA_send (dcpyDataState, chirp);
                    }
                }
            }
        }
    }

    /*  ------------------------------------------------------------------------
     *  Initiate the next data transfer on an output channel.
     *  ------------------------------------------------------------------------
     */
    chnlId = DCPYDATA_getNextOutputChannel (dspId,
                                      dcpyDataState,
                                      (  dcpyDataState->outputMask
                                       & ~(dcpyDataState->ongoingOutputMask))) ;

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
            SET_BIT (dcpyDataState->ongoingOutputMask, chnlId) ;

            status = DCPYDATA_send (dcpyDataState, chirp) ;
            if (DSP_FAILED (status)) {
                 /* Failed to send  the chirp to DSP */
                 /* Reset the gppOutputFull and ongoingOutputMask fields */
                 ctrlPtr->gppOutputFull [chnlId].bitValue = 0 ;
                 CLEAR_BIT (dcpyDataState->ongoingOutputMask, chnlId) ;
                 /* Reset the chirp status to LDRV_CHNL_IOCSTATUS_PENDING*/
                 chirp->iocStatus = LDRV_CHNL_IOCSTATE_PENDING ;
            }
            else {
                 /* Change the ioc status of the chirp to
                  * LDRV_CHNL_IOCSTATUS_INPROGRESS
                  */
                chirp->iocStatus = LDRV_CHNL_IOCSTATE_INPROGRESS ;
            }
        }
    }

    TRC_0LEAVE ("DCPYDATA_dpc") ;
}


/** ----------------------------------------------------------------------------
 *  @func   DCPYDATA_getNextOutputChannel
 *
 *  @desc   Find the channel that has data to send to DSP.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
ChannelId
DCPYDATA_getNextOutputChannel (IN  ProcessorId       dspId,
                               IN  DCPYDATA_Object * dcpyDataState,
                               IN  Uint32            mask)
{
    Bool            done     = FALSE ;
    ChannelId       selectId = CHNLID_INVALID ;
    ChannelId       startId ;
    DCPYDATA_Ctrl * ctrlPtr ;

    TRC_3ENTER ("DCPYDATA_getNextOutputChannel", dspId, dcpyDataState, mask) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    ctrlPtr = (dcpyDataState->ctrlPtr) ;

    if (mask != 0) {
        startId  = dcpyDataState->lastOutput ;
        selectId = startId ;
        do {
            selectId = selectId + 1 ;
            if (selectId == dcpyDataState->dataCfgObject->numChannels) {
                selectId = 0 ;
            }

            if (   ((mask & (1 << selectId)) != 0)
                && (ctrlPtr->dspFreeMask [selectId].bitValue == 1)){
                dcpyDataState->lastOutput = selectId ;
                done = TRUE ;
            }
        } while ((selectId != startId) && (done == FALSE)) ;
    }

    if (done == FALSE) {
        selectId = CHNLID_INVALID ;
    }

    TRC_1LEAVE ("DCPYDATA_getNextOutputChannel", selectId) ;

    return selectId ;
}


/** ----------------------------------------------------------------------------
 *  @func   DCPYDATA_clearChannelInfo
 *
 *  @desc   Clear the channel information based on its mode.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
DCPYDATA_clearChannelInfo (IN  ProcessorId dspId,
                           IN  ChannelId   chnlId)
{
    DSP_STATUS        status = DSP_SOK ;
    ChannelMode       chnlMode ;
    Uint32            dataDrvId ;
    DCPYDATA_Object * dcpyDataState ;

    TRC_2ENTER ("DCPYDATA_clearChannelInfo", dspId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (IS_VALID_CHNLID (dspId, chnlId)) ;

    status = LDRV_DATA_getDataDrvId (dspId, chnlId, &dataDrvId) ;
    if (DSP_SUCCEEDED (status)) {
        dcpyDataState = &(DCPYDATA_State [dspId][dataDrvId]) ;

        /* Adjust the channel ID to the actual channel ID. */
        chnlId = chnlId - dcpyDataState->dataCfgObject->baseChnlId ;

        chnlMode = LDRV_CHNL_getChannelMode (dspId, chnlId) ;

        if (chnlMode == ChannelMode_Input) {
            dcpyDataState->ctrlPtr->gppFreeMask [chnlId].bitValue = 0 ;
        }
        else if (chnlMode == ChannelMode_Output) {
            CLEAR_BIT (dcpyDataState->outputMask, chnlId) ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("DCPYDATA_clearChannelInfo", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   DCPYDATA_send
 *
 *  @desc   Sends the specified data buffer to the DSP.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
DSP_STATUS
DCPYDATA_send (DCPYDATA_Object * dcpyDataState, IN Pvoid buffer)
{
    DSP_STATUS          status = DSP_SOK ;
    Uint16              poolId = POOL_INVALIDID ;
    LDRV_CHNL_Irp *     fmChirp ;
    LDRV_CHNL_Irp *     chirp ;
    ProcessorId         dspId ;
    DCPYDATA_Ctrl *     ctrlPtr ;
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_DspConfig * dspCfg ;


    TRC_2ENTER ("DCPYDATA_send", dcpyDataState, buffer) ;

    DBC_Require (buffer != NULL) ;

    dspId   = dcpyDataState->dspId ;
    dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj   = dspCfg->dspObject ;
    ctrlPtr = dcpyDataState->ctrlPtr ;

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
                               dcpyDataState->wordSwap) ;
    /* Adjust the buffer size using the DSP MADU size */
    chirp->size   = SWAP_LONG ((fmChirp->size / dspObj->maduSize),
                               dspObj->wordSwap) ;
    chirp->chnlId = SWAP_LONG (fmChirp->chnlId, dspObj->wordSwap) ;

    LDRV_POOL_getPoolId (dspId,
                         (Pvoid)fmChirp->buffer,
                         AddrType_Knl,
                         &poolId) ;
    DBC_Assert (DSP_SUCCEEDED (status)) ;

    LDRV_POOL_writeback (poolId,
                         (Pvoid) fmChirp->buffer,
                         fmChirp->size) ;

    /*  Send the buffer to the DSP */
    LDRV_MPCS_enter ((MPCS_ShObj *) &(ctrlPtr->csToDspList)) ;
    LDRV_MPLIST_putTail  (dspId,
                          (List *) &(ctrlPtr->toDspList),
                          (ListElement *) chirp) ;

    status = IPS_notify (dspId,
                         dcpyDataState->dataCfgObject->ipsId,
                         dcpyDataState->dataCfgObject->ipsEventNo,
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

    TRC_1LEAVE ("DCPYDATA_send", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* if defined (__cplusplus) */
