/** ============================================================================
 *  @file   ips.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/IPS/
 *
 *  @desc   Implements the Inter Processor Signaling component.
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


/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <_dsplink.h>
#include <procdefs.h>
#include <linkcfgdefs.h>

/*  ----------------------------------- Trace & Debug                 */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Profiling                     */
#include <profile.h>

/*  ----------------------------------- OSAL Headers                  */
#include <osal.h>

/*  ----------------------------------- Generic Functions             */
#include <list.h>

/*  ----------------------------------- Link Driver                   */
#include <ldrv_utils.h>
#include <dsp.h>
#include <ldrv_smm.h>
#include <ldrv.h>
#include <ips.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent ID.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_IPS_IPS

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  TRC_SetReason (status, FID_C_LDRV_IPS_IPS, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @const  UP
 *
 *  @desc   Flag indicating event is set.
 *  ============================================================================
 */
#define UP    1

/** ============================================================================
 *  @const  DOWN
 *
 *  @desc   Flag indicating event is not set.
 *  ============================================================================
 */
#define DOWN  0


/** ----------------------------------------------------------------------------
 *  @name   IPS_EventListener
 *
 *  @desc   Defines the Event Listener object, which contains the
 *          listener-specific information.
 *
 *  @field  element
 *              Structure that allows it to be used by LIST.
 *  @field  fnIpsCbck
 *              Callback function for the event.
 *  @field  cbckArg
 *              Parameters passed to the callback.
 *  ----------------------------------------------------------------------------
 */
typedef struct IPS_EventListener_tag {
    ListElement    element   ;
    FnIpsCbck      fnIpsCbck ;
    Pvoid          cbckArg   ;
} IPS_EventListener ;

/** ----------------------------------------------------------------------------
 *  @name   IPS_EventList
 *
 *  @desc   Defines the Event Listener object, which contains information about
 *          all listener registered currently.
 *
 *  @field  eventHandlerCount
 *              Number of listener attached to the event.
 *  @field  listeners
 *              Pointer to the first event listener.
 *  ----------------------------------------------------------------------------
 */
typedef struct IPS_EventList_tag {
    Uint32      eventHandlerCount ;
    List      * listeners   ;
} IPS_EventList ;


/** ----------------------------------------------------------------------------
 *  @name   IPS_Object
 *
 *  @desc   This structure defines the IPS object, which contains all state
 *          information required by the IPS component.
 *
 *  @field  dspId
 *              ID of the processor to which this IPS object connects.
 *  @field  ipsId
 *              ID of the IPS.
 *  @field  reservedEventMask
 *              Mask for reserved event numbers.
 *  @field  ipsEventList
 *              Pointer to array containing information about registered
 *              listeners and events.
 *  @field  ctrlPtr
 *              Pointer to the IPS control structure.
 *  @field  dspCtrlAddr
 *              Address of the IPS shared memory control region in DSP address
 *              space.
 *  @field  ctrlSize
 *              Size of the IPS shared memory control region.
 *  @field  isrObject
 *              IPS Isr object.
 *  @field  ipsCfgObject
 *              Pointer to the configuration IPS object.
 *  @field  lock
 *              Lock for using IPS resources.
 *  @field  ipsInstrument
 *              Variable containing profiling information.
 *              This field is defined only if either profiling or debug is
 *              enabled.
 *  ----------------------------------------------------------------------------
 */
typedef struct IPS_Object_tag {
    ProcessorId           dspId             ;
    Uint32                ipsId             ;
    Uint32                reservedEventMask ;
    IPS_EventList *       ipsEventList      ;
    IPS_Ctrl *            ctrlPtr           ;
    Uint32                dspCtrlAddr       ;
    Uint32                ctrlSize          ;
    IsrObject *           isrObject         ;
    LINKCFG_Ips *         ipsCfgObject      ;
    Pvoid                 lock              ;
#if defined (DDSP_PROFILE)
    IPS_Instrument        ipsInstrument    ;
#endif /* #if defined (DDSP_PROFILE) */
} IPS_Object ;

/** ----------------------------------------------------------------------------
 *  @name   IPS_State
 *
 *  @desc   State object for the IPS component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC IPS_Object IPS_State [MAX_DSPS] [MAX_IPS] ;

/*  ----------------------------------------------------------------------------
 *  @name   IPS_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool IPS_IsInitialized [MAX_DSPS] [MAX_IPS] ;


/** ----------------------------------------------------------------------------
 *  @func   IPS_ISR
 *
 *  @desc   This function implements the interrupt service routine for the
 *          interrupt received from the DSP.
 *
 *  @arg    refData
 *              Argument to the interrupt handler allowing the interrupt
 *              handler to identify the associated driver information.
 *
 *  @ret    DSP_EIRQNONE
 *             only when DSP has not generated interrupts (IN SHARED IRQ case)
 *
 *  @enter  refData must not be NULL.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
IPS_ISR (IN Pvoid refData) ;


/** ----------------------------------------------------------------------------
 *  @func   IPS_interruptDsp
 *
 *  @desc   This function sends an interrupt to the DSP.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    IPS_clearDspInterrupt ()
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
IPS_interruptDsp (IN ProcessorId dspId, IN Uint32 ipsId) ;


/** ----------------------------------------------------------------------------
 *  @func   IPS_clearDspInterrupt
 *
 *  @desc   This function clears the DSP interrupt.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    IPS_interruptDsp ()
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
IPS_clearDspInterrupt (IN ProcessorId dspId, IN Uint32 ipsId) ;


/** ----------------------------------------------------------------------------
 *  @func   IPS_matchCbck
 *
 *  @desc   This function matches an event listener with a list element.
 *
 *  @arg    elem
 *              List element to be matched.
 *  @arg    data
 *              Data to be matched for element to be searched within the list.
 *
 *  @ret    TRUE
 *              The element matches the required criteria.
 *          FALSE
 *              The element does not match the required criteria.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Bool
IPS_matchCbck (IN ListElement * elem, IN Pvoid data) ;


/** ============================================================================
 *  @func   IPS_init
 *
 *  @desc   This function initializes the IPS component.
 *
 *  @modif  IPS_State [dspId][ipsId]
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_init (IN  ProcessorId dspId,
          IN  Uint32      ipsId,
          OUT Uint32 *    shDspAddr)
{
    DSP_STATUS          status       = DSP_SOK ;
    Uint32              i            = 0 ;
    IPS_Ctrl *          ctrlPtr      = NULL ;
    Uint32              gppIntId     = 0xFFFFFFFFl ;
    Bool                sharedIsr    = FALSE ;
    InterruptObject     intObj ;
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_DspConfig * dspCfg ;
    LINKCFG_LinkDrv *   linkDrv ;
    IPS_Object *        ipsState ;
    Uint32              numIpsEvents ;

    TRC_3ENTER ("IPS_init", dspId, ipsId, shDspAddr) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (shDspAddr != NULL) ;

    if (IPS_IsInitialized [dspId][ipsId] == FALSE) {
        ipsState  = &(IPS_State [dspId][ipsId]) ;
        dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
        dspObj   = dspCfg->dspObject ;
        linkDrv  = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;

#if defined (DDSP_PROFILE)
        ipsState->ipsInstrument.registerCount      = 0 ;
        ipsState->ipsInstrument.unregisterCount    = 0 ;
        ipsState->ipsInstrument.eventSentCount     = 0 ;
        ipsState->ipsInstrument.eventOccurredCount = 0 ;
#endif /* if defined (DDSP_PROFILE) */

        ipsState->dspId        = dspId ;
        ipsState->ipsId        = ipsId ;
        ipsState->reservedEventMask = 0 ;
        ipsState->ipsEventList = NULL ;
        ipsState->ctrlPtr      = NULL ;
        ipsState->dspCtrlAddr  = (Uint32) NULL ;
        ipsState->ctrlSize     = 0 ;
        ipsState->isrObject    = NULL ;
        ipsState->ipsCfgObject =
                             &(dspCfg->ipsTables [linkDrv->ipsTableId][ipsId]) ;

        /* Create IPS lock */
        ipsState->lock = SYNC_SpinLockCreate () ;

        numIpsEvents = ipsState->ipsCfgObject->numIpsEvents ;

        /*  --------------------------------------------------------------------
         *  Calculate size of shared memory required for the IPS component.
         *  --------------------------------------------------------------------
         */
        ipsState->ctrlSize = DSPLINK_ALIGN ((   sizeof (IPS_Ctrl)
                                             +  (   sizeof (IPS_EventEntry)
                                                 *  numIpsEvents
                                                 *  2)
                                             +  (   sizeof (IPS_EventRegEntry)
                                                 * numIpsEvents
                                                 * 2)),
                                            DSPLINK_BUF_ALIGN) ;

        /*  --------------------------------------------------------------------
         *  Allocate shared memory required for the IPS component.
         *  --------------------------------------------------------------------
         */
        status = LDRV_SMM_alloc (dspId,
                                 ipsState->ipsCfgObject->memEntry,
                                 NULL, /* Physical address not required. */
                                 &(ipsState->dspCtrlAddr),
                                 (Uint32 *) ((Pvoid) &(ipsState->ctrlPtr)),
                                 ipsState->ctrlSize) ;
        if (DSP_SUCCEEDED (status)) {
            *shDspAddr = ipsState->dspCtrlAddr ;
            TRC_1PRINT (TRC_LEVEL4,
                        "IPS Ctrl GPP address [0x%x]\n",
                        ipsState->ctrlPtr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "IPS Ctrl DSP address [0x%x]\n",
                        ipsState->dspCtrlAddr) ;
            TRC_1PRINT (TRC_LEVEL4,
                        "IPS Ctrl size        [0x%x]\n",
                        ipsState->ctrlSize) ;

            ctrlPtr = ipsState->ctrlPtr ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            /*  ----------------------------------------------------------------
             *  Initialize the shared memory control region.
             *  ----------------------------------------------------------------
             */
            ctrlPtr->gppEventChart        = (IPS_EventEntry *) ((Uint32)
                                                (ctrlPtr)
                                            +   sizeof (IPS_Ctrl)) ;
            ctrlPtr->dspAddrGppEventChart = (IPS_EventEntry *) ((Uint32)
                                                (ipsState->dspCtrlAddr)
                                            +   sizeof (IPS_Ctrl)) ;

            ctrlPtr->dspEventChart        = (IPS_EventEntry *) ((Uint32)
                                                (ctrlPtr->gppEventChart)
                                            +   (   sizeof (IPS_EventEntry)
                                                 *  numIpsEvents)) ;
            ctrlPtr->dspAddrDspEventChart = (IPS_EventEntry *) ((Uint32)
                                                (ctrlPtr->dspAddrGppEventChart)
                                            +   (   sizeof (IPS_EventEntry)
                                                 *  numIpsEvents)) ;

            ctrlPtr->gppRegChart          = (IPS_EventRegEntry *) ((Uint32)
                                                (ctrlPtr->dspEventChart)
                                            +   (   sizeof (IPS_EventEntry)
                                                 *  numIpsEvents)) ;
            ctrlPtr->dspAddrDspRegChart   = (IPS_EventRegEntry *) ((Uint32)
                                                (ctrlPtr->dspAddrDspEventChart)
                                            +   (   sizeof (IPS_EventEntry)
                                                 *  numIpsEvents)
                                            +   (   sizeof (IPS_EventRegEntry)
                                                 *  numIpsEvents)) ;

            ctrlPtr->gppRegMask.mask = 0 ;
            ctrlPtr->dspRegMask.mask = 0 ;

            /* Allocate memory for the event listener objects. */
            status = MEM_Calloc ((Void **) &(ipsState->ipsEventList),
                                 (sizeof (IPS_EventList) * numIpsEvents),
                                 MEM_DEFAULT) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            /* Initialize the event and registration charts and create the
             * listener list for each event.
             */
            for (i = 0 ; (i < numIpsEvents) && DSP_SUCCEEDED (status) ; i++) {
                ctrlPtr->gppEventChart [i].flag       = 0 ;
                ctrlPtr->dspEventChart [i].flag       = 0 ;
                ctrlPtr->gppRegChart [i].regEventNo = 0xFFFFFFFFl ;

                status = LIST_Create (&(ipsState->ipsEventList [i].listeners)) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
        }

        if (ipsState->ipsCfgObject->gppIntId == 0xFFFFFFFFl) {
            DSP_intCtrl (dspId,
                         ipsState->ipsCfgObject->gppIntId,
                         DSP_IntCtrlCmd_GetIntId,
                         &gppIntId) ;
            if (gppIntId != 0xFFFFFFFFl) {
                sharedIsr = TRUE ;
                ipsState->ipsCfgObject->gppIntId = gppIntId ;
            }
            else {
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
        }

        /* If IPS is supported in DSP->GPP direction, create the ISR for DSP
         * interrupt and install it.
         */
        if (    DSP_SUCCEEDED (status)
            &&  (ipsState->ipsCfgObject->gppIntId != 0xFFFFFFFFl)) {
            intObj.intId = ipsState->ipsCfgObject->gppIntId ;
            intObj.dspId = dspId ;
            if (sharedIsr == TRUE) {
                intObj.shared = TRUE ;
            }
            else {
                intObj.shared = FALSE ;
            }
            intObj.checkFunc = IPS_checkIntGen ;
            intObj.param     = ipsState ;
            status = ISR_Create (&IPS_ISR,
                                 (Pvoid) ipsState,
                                 &intObj,
                                 &(ipsState->isrObject)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                status = ISR_Install (NULL, ipsState->isrObject) ;
                if (DSP_SUCCEEDED (status)) {
                    status = DSP_intCtrl (dspId,
                                          ipsState->ipsCfgObject->gppIntId,
                                          DSP_IntCtrlCmd_Enable,
                                          NULL) ;
                }
                else {
                    status = DSP_EINVALIDARG ;
                }
            }
        }
        else {
            status = DSP_EINVALIDARG ;
        }

        IPS_IsInitialized [dspId][ipsId] = TRUE ;
        if (DSP_FAILED (status)) {
            /* IPS_exit executes cleanup only if the initialized flag is TRUE. */
            IPS_exit (dspId, ipsId) ;
            IPS_IsInitialized [dspId][ipsId] = FALSE ;
        }
    }

    DBC_Ensure (   (   DSP_SUCCEEDED (status)
                   && (IPS_IsInitialized [dspId][ipsId] == TRUE))
                || DSP_FAILED (status)) ;

    TRC_1LEAVE ("IPS_init", status) ;

    return status ;
}

/** ============================================================================
 *  @func   IPS_exit
 *
 *  @desc   This function Finalizes the IPS component.
 *
 *  @modif  ipsState->isInitialized
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_exit (IN ProcessorId dspId,
          IN Uint32      ipsId)
{
    DSP_STATUS          status    = DSP_SOK ;
    DSP_STATUS          tmpStatus = DSP_SOK ;
    IPS_EventListener * listener  = NULL ;
    IPS_Ctrl *          ctrlPtr ;
    IPS_EventList *     ipsEventList ;
    IPS_Object *        ipsState ;
    Uint32              i ;

    TRC_2ENTER ("IPS_exit", dspId, ipsId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (IPS_IsInitialized [dspId][ipsId] == TRUE) {
        ipsState = &(IPS_State [dspId][ipsId]) ;

        if (ipsState->ipsCfgObject->gppIntId != 0xFFFFFFFFl) {
            /* Disable the DSP interrupt, Uninstall the ISR and delete it. */
            tmpStatus = DSP_intCtrl (dspId,
                                     ipsState->ipsCfgObject->gppIntId,
                                     DSP_IntCtrlCmd_Disable,
                                     NULL) ;
            if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            if (ipsState->isrObject != NULL) {
                tmpStatus = ISR_Uninstall (ipsState->isrObject) ;
                if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }

                tmpStatus = ISR_Delete (ipsState->isrObject) ;
                if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }

                ipsState->isrObject = NULL ;
            }
        }

        /* Remove all listeners from the list and set eventHandlerCount to
         * zero.
         */
        DBC_Assert (ipsState->ipsEventList != NULL) ;
        ipsEventList = ipsState->ipsEventList ;
        if (ipsEventList != NULL) {
            for (i = 0 ; i < ipsState->ipsCfgObject->numIpsEvents ; i++) {
                DBC_Assert (ipsEventList [i].eventHandlerCount == 0) ;
                ipsEventList [i].eventHandlerCount = 0 ;
                if (ipsEventList [i].listeners != NULL) {
                    while (  (LIST_IsEmpty (ipsEventList [i].listeners) != TRUE)
                           && (DSP_SUCCEEDED (tmpStatus))) {
                        tmpStatus = LIST_GetHead (ipsEventList [i].listeners,
                                                  (ListElement **) &listener) ;
                        if ((DSP_SUCCEEDED (tmpStatus)) && (listener != NULL)) {
                            tmpStatus = FREE_PTR (listener) ;
                            if (    (DSP_SUCCEEDED (status))
                                &&  (DSP_FAILED (tmpStatus))) {
                                status = tmpStatus ;
                                SET_FAILURE_REASON ;
                            }
                        }
                    }

                    tmpStatus = LIST_Delete (ipsEventList [i].listeners) ;
                    if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }

                    ipsEventList [i].listeners = NULL ;
                }
            }

            /* Allocate memory for the event listener objects. */
            tmpStatus = FREE_PTR (ipsState->ipsEventList) ;
            if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        DBC_Assert (ipsState->ctrlPtr != NULL) ;
        ctrlPtr = ipsState->ctrlPtr ;
        if (ctrlPtr != NULL) {
            /*  ----------------------------------------------------------------
             *  Free shared memory allocated for the IPS component.
             *  ----------------------------------------------------------------
             */
            tmpStatus = LDRV_SMM_free (dspId,
                                       ipsState->ipsCfgObject->memEntry,
                                       ipsState->ctrlSize) ;
            if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
            ctrlPtr->gppRegMask.mask = 0 ;
            ctrlPtr->dspRegMask.mask = 0 ;
            ipsState->ctrlPtr = NULL ;
        }

        /* Delete IPS lock */
        SYNC_SpinLockDelete (ipsState->lock) ;

        ipsState->dspCtrlAddr = (Uint32) NULL ;
        ipsState->ctrlSize    = 0 ;
        ipsState->reservedEventMask = 0 ;

#if defined (DDSP_PROFILE)
        ipsState->ipsInstrument.registerCount      = 0 ;
        ipsState->ipsInstrument.unregisterCount    = 0 ;
        ipsState->ipsInstrument.eventSentCount     = 0 ;
        ipsState->ipsInstrument.eventOccurredCount = 0 ;
#endif /* if defined (DDSP_PROFILE) */


        IPS_IsInitialized [dspId][ipsId] = FALSE ;
    }

    DBC_Ensure (   (   DSP_SUCCEEDED (status)
                   && (IPS_IsInitialized [dspId][ipsId] == FALSE))
                || DSP_FAILED (status)) ;

    TRC_1LEAVE ("IPS_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   IPS_register
 *
 *  @desc   This function registers a callback from an event.
 *
 *  @modif  ipsEventList
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_register (IN     ProcessorId   dspId,
              IN     Uint32        ipsId,
              IN     Uint32        eventNo,
              IN     FnIpsCbck     fnIpsCbck,
              IN     Pvoid         cbckArg)
{
    DSP_STATUS          status   = DSP_SOK ;
    Bool                done     = FALSE ;
    Bool                firstReg = FALSE ;
    Bool                isNotReserved = FALSE ;
    IPS_EventListener * eventListener ;
    IPS_EventList *     ipsEventList ;
    IPS_Ctrl *          ctrlPtr ;
    IPS_EventRegEntry * regChart ;
    IPS_EventEntry *    gppEventChart ;
    Uint32              irqFlags ;
    IPS_Object *        ipsState ;
    Int32               i ;
    Int32               j ;
    Int32               k ;
    Uint32              tempVar ;

    TRC_5ENTER ("IPS_register", dspId, ipsId, eventNo, fnIpsCbck, cbckArg) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (fnIpsCbck != NULL) ;

    if ((IS_VALID_PROCID (dspId) == FALSE) || (fnIpsCbck == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else if (IPS_IsInitialized [dspId][ipsId] != TRUE) {
        status = DSP_EWRONGSTATE ;
        SET_FAILURE_REASON ;
    }
    else {
        isNotReserved = IPS_isNotReserved (dspId, ipsId, eventNo) ;
        if (isNotReserved == TRUE) {
            ipsState = &(IPS_State [dspId][ipsId]) ;
            if (ipsState->ipsCfgObject->gppIntId == 0xFFFFFFFFl) {
            /* IPS is not supported in DSP->GPP direction. */
            status = DSP_ENOTSUPPORTED ;
            SET_FAILURE_REASON ;
        }
        else {
            DBC_Assert (ipsState->ctrlPtr != NULL) ;
            ctrlPtr = ipsState->ctrlPtr ;

            /* Put the listener information into the listener list. and increase
             * the eventHandlerCount by 1.
             */
            DBC_Assert (ipsState->ipsEventList != NULL) ;
            ipsEventList = ipsState->ipsEventList ;

            status = MEM_Alloc ((Void **) &eventListener,
                                sizeof (IPS_EventListener),
                                MEM_DEFAULT) ;
            if (DSP_SUCCEEDED (status)) {
                status = LIST_InitializeElement (&(eventListener->element)) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }

            irqFlags = SYNC_SpinLockStartEx (ipsState->lock) ;
            if (DSP_SUCCEEDED (status)) {
                eventListener->fnIpsCbck = fnIpsCbck ;
                eventListener->cbckArg   = cbckArg ;
                /* Check if this is the first registration for this event. */
                if (LIST_IsEmpty (ipsEventList [eventNo].listeners)) {
                    firstReg = TRUE ;
                    gppEventChart = ctrlPtr->gppEventChart ;
                    /* Clear the unserviced event as there are no listeners for
                     * the pending event
                     */
                    gppEventChart [eventNo].flag = DOWN ;
                }
                status = LIST_PutTail (ipsEventList [eventNo].listeners,
                                       &(eventListener->element)) ;

                if (DSP_SUCCEEDED (status)) {
#if defined (DDSP_PROFILE)
                    ipsState->ipsInstrument.registerCount++ ;
#endif /* if defined (DDSP_PROFILE) */
                    ipsEventList [eventNo].eventHandlerCount++ ;
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }

            /* Add an entry for the registered event into the Event Registration
             * Chart, in ascending order of event numbers (and priorities).
             * Entry must be added only if this is the first registration for
             * this event.
             */
            if (DSP_SUCCEEDED (status) && (firstReg == TRUE)) {
                regChart = ctrlPtr->gppRegChart ;
                for (i = 0 ;
                     ((i < ipsState->ipsCfgObject->numIpsEvents) && (done == FALSE)) ;
                     i++) {
                    /* Find the correct slot in the registration array. */
                    if (regChart [i].regEventNo == 0xFFFFFFFFl) {
                        regChart [i].regEventNo = eventNo ;
                        done = TRUE ;
                    }

                    /* Sort the list in ascending order */
                    if (done == TRUE) {
                        for (j = 0 ; j <= i ; j++) {
                            for (k = (j+1) ; k <= i ; k++) {
                                if (  (regChart [k].regEventNo)
                                    < (regChart [j].regEventNo)) {
                                    tempVar = (Uint32) regChart [k].regEventNo ;
                                    regChart [k].regEventNo =
                                                        regChart[j].regEventNo ;
                                    regChart[j].regEventNo = tempVar ;

                                    tempVar = (Uint32) regChart [k].reserved ;
                                    regChart [k].reserved =
                                                        regChart[j].reserved ;
                                    regChart[j].reserved = tempVar ;
                                }
                            }
                        }
                    }
                }

                if (done == TRUE) {
                    SET_BIT (ctrlPtr->gppRegMask.mask, eventNo) ;
                }
                else {
                    status = DSP_ERESOURCE ;
                    SET_FAILURE_REASON ;
                }
            }
            SYNC_SpinLockEndEx (ipsState->lock, irqFlags) ;

            if (DSP_FAILED (status)) {
                if (eventListener != NULL) {
                    LIST_RemoveElement (ipsEventList [eventNo].listeners,
                                        &(eventListener->element)) ;
                    FREE_PTR (eventListener) ;
                }
            }
        }
    }
        else {
            status = DSP_ERESOURCE ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("IPS_register", status) ;

    return status ;
}


/** ============================================================================
 *  @func   IPS_unregister
 *
 *  @desc   This function unregisters a callback from an event.
 *
 *  @modif  ipsEventList
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_unregister (IN     ProcessorId dspId,
                IN     Uint32      ipsId,
                IN     Uint32      eventNo,
                IN     FnIpsCbck   fnIpsCbck,
                IN     Pvoid       cbckArg)
{
    DSP_STATUS          status       = DSP_SOK ;
    DSP_STATUS          tmpStatus    = DSP_SOK ;
    IPS_EventListener * listener     = NULL ;
    IPS_EventList *     ipsEventList = NULL ;
    Uint32              irqFlags     = 0 ;
    Bool                done         = FALSE ;
    IPS_Ctrl *          ctrlPtr ;
    IPS_Object *        ipsState ;
    IPS_EventRegEntry * regChart ;
    IPS_EventListener   unregInfo ;
    IPS_EventEntry *    gppEventChart ;
    Int32               i ;
    Int32               j ;


    TRC_5ENTER ("IPS_unregister", dspId, ipsId, eventNo, fnIpsCbck, cbckArg) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (fnIpsCbck != NULL) ;

    if ((IS_VALID_PROCID (dspId) == FALSE) || (fnIpsCbck == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else if (IPS_IsInitialized [dspId][ipsId] != TRUE) {
        status = DSP_EWRONGSTATE ;
        SET_FAILURE_REASON ;
    }
    else {
        ipsState = &(IPS_State [dspId][ipsId]) ;
        if (ipsState->ipsCfgObject->gppIntId == 0xFFFFFFFFl) {
            /* IPS is not supported in DSP->GPP direction. */
            status = DSP_ENOTSUPPORTED ;
            SET_FAILURE_REASON ;
        }
        else {
            ctrlPtr = ipsState->ctrlPtr ;

            /* Remove the listener information from the listener list. and
             * decrease the eventHandlerCount by 1.
             */
            ipsEventList = ipsState->ipsEventList ;

            irqFlags = SYNC_SpinLockStartEx (ipsState->lock) ;
            unregInfo.fnIpsCbck = fnIpsCbck ;
            unregInfo.cbckArg   = cbckArg ;
            status = LIST_SearchElement ((List *)
                                             ipsEventList [eventNo].listeners,
                                         (Pvoid) &unregInfo,
                                         (ListElement **) &listener,
                                         (ListMatchFunc) (&IPS_matchCbck)) ;

            if (DSP_SUCCEEDED (status)) {
                status = LIST_RemoveElement (ipsEventList [eventNo].listeners,
                                             &(listener->element)) ;
                if (DSP_SUCCEEDED (status)) {
#if defined (DDSP_PROFILE)
                    ipsState->ipsInstrument.unregisterCount++ ;
#endif /* if defined (DDSP_PROFILE) */
                    ipsEventList [eventNo].eventHandlerCount-- ;
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }

            /* Remove the entry from Registration Chart. The entries are stored
             * in ascending order of event numbers (and priority).
             * The entry must be removed only if this is the last unregistration
             * for this event number.
             */
            if (    (status != DSP_ENOTFOUND)
                &&  (LIST_IsEmpty (ipsEventList [eventNo].listeners))) {
                CLEAR_BIT (ctrlPtr->gppRegMask.mask, eventNo) ;
                gppEventChart = ctrlPtr->gppEventChart ;
                /* Clear the unserviced event as there are no listeners for
                 * the pending event
                 */
                gppEventChart [eventNo].flag = DOWN ;
                regChart = ctrlPtr->gppRegChart ;
                for (i = 0 ;
                     ((i < ipsState->ipsCfgObject->numIpsEvents) && (done ==
                                                                     FALSE)) ;
                     i++) {
                    /* Find the correct slot in the registration array. */
                    if (eventNo == regChart [i].regEventNo) {
                        regChart [i].regEventNo = 0xFFFFFFFFl ;
                        for (j = (i + 1) ;
                                 (regChart [j].regEventNo != 0xFFFFFFFFl)
                             &&  (j != ipsState->ipsCfgObject->numIpsEvents) ;
                             j++) {
                            regChart [j - 1].regEventNo =
                                                      regChart [j].regEventNo ;
                            regChart [j - 1].reserved = regChart [j].reserved ;
                        }

                        if (j == ipsState->ipsCfgObject->numIpsEvents) {
                            regChart [j - 1].regEventNo = 0xFFFFFFFFl ;
                        }

                        done = TRUE ;
                    }
                }
            }
            SYNC_SpinLockEndEx (ipsState->lock, irqFlags) ;

            if (listener != NULL) {
                tmpStatus = FREE_PTR (listener) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
        }
    }

    TRC_1LEAVE ("IPS_unregister", status) ;

    return status ;
}


/** ============================================================================
 *  @func   IPS_notify
 *
 *  @desc   This function notifies the other side about an event.
 *
 *  @modif  ipsGppEventChart
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_notify (IN ProcessorId dspId,
            IN Uint32      ipsId,
            IN Uint32      eventNo,
            IN Uint32      payload,
            IN Bool        waitEvent)
{
    DSP_STATUS       status = DSP_SOK ;
    Uint32           i      = 0 ;
    IPS_EventEntry * dspEventChart ;
    Uint32           irqFlags ;
    IPS_Ctrl *       ctrlPtr  ;
    IPS_Object *     ipsState ;
    Uint32           maxPollCount ;

    TRC_5ENTER ("IPS_notify", dspId, ipsId, eventNo, payload, waitEvent) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if ((IS_VALID_PROCID (dspId) == FALSE)){
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else if (IPS_IsInitialized [dspId][ipsId] != TRUE) {
        status = DSP_EWRONGSTATE ;
        SET_FAILURE_REASON ;
    }
    else {
        ipsState = &(IPS_State [dspId][ipsId]) ;
        ctrlPtr = ipsState->ctrlPtr ;
        dspEventChart = ctrlPtr->dspEventChart ;
        maxPollCount  = ipsState->ipsCfgObject->arg1 ;

        if (ipsState->ipsCfgObject->dspIntId == 0xFFFFFFFFl) {
            /* IPS is not supported in GPP-DSP direction. */
            status = DSP_ENOTSUPPORTED ;
        }
        else if (TEST_BIT (ctrlPtr->dspRegMask.mask, eventNo) == 0u) {
            status = DSP_ENOTREADY ;
        }
        else {
            irqFlags = SYNC_SpinLockStartEx (ipsState->lock) ;

            /* ---------------------------------------------------------------------
             * Wait for completion of previous event from other side.
             * ---------------------------------------------------------------------
             */

            if (waitEvent == TRUE) {
                do {
                    SYNC_SpinLockEndEx (ipsState->lock, irqFlags) ;

                    if ((maxPollCount != 0xFFFFFFFFl) && (i == maxPollCount)) {
                        status = DSP_ETIMEOUT ;
                        SET_FAILURE_REASON ;
                    }
                    i++ ;

                    irqFlags = SYNC_SpinLockStartEx (ipsState->lock) ;
                } while ( (dspEventChart [eventNo].flag != DOWN)
                         &&   DSP_SUCCEEDED (status));
                /* Recheck dspRegMask.mask. As Other  side might have cleared
                 * dspRegMask.mask when multiple  threads  on GPP side
                 * looping in the while loop above.
                 */
                if (TEST_BIT (ctrlPtr->dspRegMask.mask, eventNo) == 0u) {
                    status = DSP_ENOTREADY ;
                }
            }

            if (DSP_SUCCEEDED (status)) {
                /* -----------------------------------------------------------------
                 * Set the event bit field and payload.
                 * -----------------------------------------------------------------
                 */
                dspEventChart [eventNo].payload = payload ;
                dspEventChart [eventNo].flag    = UP ;

                /* -----------------------------------------------------------------
                 * Send an interrupt with the event information to the DSP
                 * -----------------------------------------------------------------
                 */
                IPS_interruptDsp (dspId, ipsId) ;
            }

            SYNC_SpinLockEndEx (ipsState->lock, irqFlags) ;

    #if defined (DDSP_PROFILE)
            ipsState->ipsInstrument.eventSentCount++ ;
    #endif /* if defined (DDSP_PROFILE) */
        }
    }

    TRC_1LEAVE ("IPS_notify", status) ;

    return status ;
}


/** ============================================================================
 *  @func   IPS_isInitialized
 *
 *  @desc   This function used to determine whether IPS is initialized for a
 *          dspId.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Bool
IPS_isInitialized (IN ProcessorId dspId,
                   IN Uint32      ipsId)
{
    Bool retVal = FALSE ;

    TRC_2ENTER ("IPS_isInitialized", dspId, ipsId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (IPS_IsInitialized [dspId][ipsId] == TRUE) {
        retVal = TRUE ;
    }

    TRC_1LEAVE ("IPS_isInitialized", retVal) ;

    return retVal ;
}


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   IPS_debug
 *
 *  @desc   This function prints the debug information for a DSP.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Void
IPS_debug (IN ProcessorId dspId,
           IN Uint32      ipsId)
{
#if defined (TRACE_ENABLE)
    IPS_Object * ipsState ;
    IPS_Ctrl *   ctrlPtr ;
#endif /* if defined (TRACE_ENABLE) */

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    TRC_2ENTER ("IPS_debug", dspId, ipsId) ;

    if (IPS_IsInitialized [dspId][ipsId] == TRUE) {
#if defined (TRACE_ENABLE)
        ipsState = &(IPS_State [dspId][ipsId]) ;
        ctrlPtr  = ipsState->ctrlPtr ;
#endif /* if defined (TRACE_ENABLE) */

        /*  --------------------------------------------------------------------
         *  Print the IPS object information.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_0PRINT (TRC_LEVEL4, "IPS object information:\n") ;
        TRC_1PRINT (TRC_LEVEL4, "    dspId         = [0x%x]\n", dspId) ;
        TRC_1PRINT (TRC_LEVEL4, "    ipsId         = [0x%x]\n", ipsId) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ipsEventList  = [0x%x]\n",
                    ipsState->ipsEventList) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ctrlPtr       = [0x%x]\n",
                    ipsState->ctrlPtr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dspCtrlAddr   = [0x%x]\n",
                    ipsState->dspCtrlAddr) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ctrlSize      = [0x%x]\n",
                    ipsState->ctrlSize) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    isrObject     = [0x%x]\n",
                    ipsState->isrObject) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    ipsCfgObject  = [0x%x]\n",
                    ipsState->ipsCfgObject) ;

#if defined (DDSP_PROFILE)
        /*  ------------------------------------------------------------------------
         *  Print the IPS instrumentation information.
         *  ------------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_1PRINT (TRC_LEVEL4,
                    "IPS instrumentation information for DSP %d\n",
                    dspId) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    Registered:     [%d]\n",
                    ipsState->ipsInstrument.registerCount) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    Unregistered:   [%d]\n",
                    ipsState->ipsInstrument.unregisterCount) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    Sent Event:     [%d]\n",
                    ipsState->ipsInstrument.eventSentCount) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    Occurred Event: [%d]\n",
                    ipsState->ipsInstrument.eventOccurredCount) ;
#endif /* if defined (DDSP_PROFILE) */

        /*  --------------------------------------------------------------------
         *  Print the IPS shared memory control information
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_0PRINT (TRC_LEVEL4, "IPS shared control information:\n") ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    gppRegChart          = [0x%x]\n",
                    ctrlPtr->gppRegChart) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dspAddrDspRegChart   = [0x%x]\n",
                    ctrlPtr->dspAddrDspRegChart) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    gppEventChart        = [0x%x]\n",
                    ctrlPtr->gppEventChart) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dspEventChart        = [0x%x]\n",
                    ctrlPtr->dspEventChart) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dspAddrGppEventChart = [0x%x]\n",
                    ctrlPtr->dspAddrGppEventChart) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dspAddrDspEventChart = [0x%x]\n",
                    ctrlPtr->dspAddrDspEventChart) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    gppRegMask.mask      = [0x%x]\n",
                    ctrlPtr->gppRegMask.mask) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    dspRegMask.mask      = [0x%x]\n",
                    ctrlPtr->dspRegMask.mask) ;
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
    }
    else {
        TRC_2PRINT (TRC_LEVEL4,
                  "IPS component for DSP ID %d IPS ID %d is not initialized.\n",
                  dspId,
                  ipsId) ;
    }

    TRC_0LEAVE ("IPS_debug") ;
}
#endif /* #if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @func   IPS_checkIntGen
 *
 *  @desc   This function implements check routine to check the interrupt
 *          generation. In case of shared interrupts, other devices can also
 *          generate same ints.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
Bool
IPS_checkIntGen (IN Pvoid refData)
{
    DSP_STATUS   status     = DSP_SOK ;
    Bool         isAsserted = FALSE   ;
    IPS_Object * ipsState   = NULL    ;

    DBC_Require (refData != NULL) ;

    TRC_1ENTER ("IPS_checkIntGen", refData) ;

    if (refData != NULL) {
        ipsState = (IPS_Object *) refData ;
        /* In case of shared inetrrupts we need to know who has generated the interrupts,
         * So check DSP if it has generated the interrupts
         */
        status = DSP_intCtrl (ipsState->dspId,
                              ipsState->ipsCfgObject->dspIntId,
                              DSP_IntCtrlCmd_Check,
                              (Pvoid) &isAsserted) ;
        if (DSP_FAILED (status)) {
            isAsserted = FALSE ;
            SET_FAILURE_REASON ;
        }

        if (isAsserted == TRUE) {
            /* Clear the DSP interrupt */
            status = IPS_clearDspInterrupt (ipsState->dspId, ipsState->ipsId) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("IPS_checkIntGen", isAsserted) ;

    return isAsserted ;
}


/** ----------------------------------------------------------------------------
 *  @func   IPS_ISR
 *
 *  @desc   This function implements the interrupt service routine for the
 *          interrupt received from the DSP.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
IPS_ISR (IN Pvoid refData)
{
    DSP_STATUS          status  = DSP_SOK ;
    Uint32              payload = 0 ;
    Uint32              i       = 0 ;
    Uint32              eventNo ;
    ListElement  *      temp ;
    Uint32              j ;
    IPS_Object *        ipsState ;
    IPS_EventRegEntry * regChart ;
    IPS_EventEntry *    gppEventChart ;
    IPS_EventList *     ipsEventList ;
    IPS_Ctrl *          ctrlPtr ;

    ipsState = (IPS_Object *) refData ;
    DBC_Require (ipsState != NULL) ;

    SYNC_SpinLockStartEx (ipsState->lock) ;

    if (DSP_SUCCEEDED (status)) {
        ctrlPtr       = ipsState->ctrlPtr ;
        regChart      = ctrlPtr->gppRegChart ;
        gppEventChart = ctrlPtr->gppEventChart ;

        if (DSP_SUCCEEDED (status)) {
            /* Execute the loop till no asserted event is found for one complete
             * loop through all registered events.
             */
            do {
                /* Check if the entry is a valid registered event. */
                eventNo = regChart [i].regEventNo ;
                if (eventNo != 0xFFFFFFFFl) {
                    if (gppEventChart [eventNo].flag == UP) {
                        payload = gppEventChart [eventNo].payload ;

                        ipsEventList = ipsState->ipsEventList ;

                        gppEventChart [eventNo].flag = DOWN ;
                        /* Call the callbacks associated with the event. */
                        temp = ipsEventList [eventNo].listeners->head.next ;
                        for (j = 0 ;
                             (j < ipsEventList [eventNo].eventHandlerCount);
                             j++) {
                            ((IPS_EventListener *) temp)->fnIpsCbck (
                                  eventNo,
                                  ((IPS_EventListener *) temp)->cbckArg,
                                  (Pvoid) (payload)) ;

                            temp = temp->next ;
                        }

                        /* reinitialize the event check counter. */
                        i = 0 ;
    #if defined (DDSP_PROFILE)
                        ipsState->ipsInstrument.eventOccurredCount++ ;
    #endif /* if defined (DDSP_PROFILE) */
                    }
                    else {
                        /* check for next event. */
                        i++ ;
                    }
                }
            }
            while (   (eventNo != 0xFFFFFFFFl)
                   && (i < ipsState->ipsCfgObject->numIpsEvents)) ;
        }
    }
    SYNC_SpinLockEndEx (ipsState->lock, (Uint32) NULL) ;

    return status ;
}

/** ----------------------------------------------------------------------------
 *  @func   IPS_interruptDsp
 *
 *  @desc   This function sends an interrupt to the DSP.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
IPS_interruptDsp (IN ProcessorId dspId, IN Uint32 ipsId)
{
    DSP_STATUS    status = DSP_SOK ;
    Uint32        temp   = 0 ;
    IPS_Object *  ipsState ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    TRC_2ENTER ("IPS_interruptDsp", dspId, ipsId) ;

    ipsState = &(IPS_State [dspId][ipsId]) ;
    status = DSP_intCtrl (dspId,
                          ipsState->ipsCfgObject->dspIntId,
                          DSP_IntCtrlCmd_Send,
                          (Pvoid) &temp) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("IPS_interruptDsp", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   IPS_clearDspInterrupt
 *
 *  @desc   This function clears the DSP interrupt.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
DSP_STATUS
IPS_clearDspInterrupt (IN ProcessorId dspId, IN Uint32 ipsId)
{
    DSP_STATUS    status = DSP_SOK ;
    Uint32        temp ;
    IPS_Object *  ipsState ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    TRC_2ENTER ("IPS_clearDspInterrupt", dspId, ipsId) ;

    ipsState = &(IPS_State [dspId][ipsId]) ;
    status = DSP_intCtrl (dspId,
                          ipsState->ipsCfgObject->gppIntId,
                          DSP_IntCtrlCmd_Clear,
                          (Pvoid) &temp) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("IPS_clearDspInterrupt", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   IPS_matchCbck
 *
 *  @desc   This function matches an event listener with a list element.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Bool
IPS_matchCbck (IN ListElement * elem, Pvoid data)
{
    Bool                retVal   = FALSE ;
    IPS_EventListener * listener = NULL  ;
    IPS_EventListener * checkObj = NULL  ;

    DBC_Require (elem != NULL) ;

    listener = (IPS_EventListener *) elem ;
    checkObj = (IPS_EventListener *) data ;
    if (    (listener->fnIpsCbck == checkObj->fnIpsCbck)
        &&  (listener->cbckArg   == checkObj->cbckArg)) {
        retVal = TRUE ;
    }

    return retVal ;
}


/** ============================================================================
 *  @func   IPS_setReserved.
 *
 *  @desc   This function reserves specified ips Id, event number.
 *
 *  @modif  IPS_State.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_setReserved (IN     ProcessorId   dspId,
                 IN     Uint32        ipsId,
                 IN     Uint32        eventNo)
{
    IPS_Object *  ipsState ;
    DSP_STATUS    status = DSP_SOK ;

    TRC_3ENTER ("IPS_setReserved", dspId, ipsId, eventNo) ;

    ipsState  = &(IPS_State [dspId][ipsId]) ;
    if (TEST_BIT (ipsState->reservedEventMask, eventNo) != 1u) {
        SET_BIT (ipsState->reservedEventMask, eventNo) ;
    }
    else {
        status = DSP_ERESOURCE ;
    }

    TRC_1LEAVE ("IPS_setReserved", status) ;

    return status ;
}


/** ============================================================================
 *  @func   IPS_clearReserved.
 *
 *  @desc   This function clears reserved specified ips Id, event number.
 *
 *  @modif  IPS_State.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_clearReserved (IN     ProcessorId   dspId,
                 IN     Uint32        ipsId,
                 IN     Uint32        eventNo)
{
    IPS_Object *  ipsState ;
    DSP_STATUS    status = DSP_SOK ;

    TRC_3ENTER ("IPS_clearReserved", dspId, ipsId, eventNo) ;

    ipsState  = &(IPS_State [dspId][ipsId]) ;
    if (TEST_BIT (ipsState->reservedEventMask, eventNo) == 1u) {
        CLEAR_BIT (ipsState->reservedEventMask, eventNo) ;
    }
    else {
        status = DSP_EINVALIDARG ;
    }

    TRC_1LEAVE ("IPS_clearReserved", status) ;

    return status ;
}


/** ============================================================================
 *  @func   IPS_isNotReserved
 *
 *  @desc   This function checks if specified ips Id, event number is reserved or
 *          free.
 *
 *  @modif  IPS_State.
 *  ============================================================================
 */
NORMAL_API
Bool
IPS_isNotReserved (IN     ProcessorId   dspId,
                   IN     Uint32        ipsId,
                   IN     Uint32        eventNo)
{
    IPS_Object *  ipsState ;
    Bool          retVal  = FALSE ;

    TRC_3ENTER ("IPS_isNotReserved", dspId, ipsId, eventNo) ;

    ipsState  = &(IPS_State [dspId][ipsId]) ;
    if (TEST_BIT (ipsState->reservedEventMask, eventNo) != 1u) {
        retVal = TRUE ;
    }

    TRC_1LEAVE ("IPS_isNotReserved", retVal) ;

    return retVal ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   IPS_instrument
 *
 *  @desc   This function returns the profile information for IPS's.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_instrument (IN ProcessorId        dspId,
                IN Uint32             ipsId,
                OUT IPS_Instrument *  retVal)
{
    DSP_STATUS status = DSP_SOK ;
    IPS_Object *  ipsState ;

    TRC_3ENTER ("IPS_instrument", dspId, ipsId, retVal) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (retVal != NULL) ;

    /*  ------------------------------------------------------------------------
     *  Get the IPS instrumentation information.
     *  ------------------------------------------------------------------------
     */
    if (retVal == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        ipsState = &(IPS_State [dspId][ipsId]) ;
        *retVal = ipsState->ipsInstrument ;
    }

    TRC_1LEAVE ("IPS_instrument", status) ;

    return status ;
}
#endif /* #if defined (DDSP_PROFILE) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

