/** ============================================================================
 *  @file   ips.c
 *
 *  @path   $(DSPLINK)/dsp/src/base/ips/DspBios/
 *
 *  @desc   Implements the Inter Processor Signaling component.
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
#include <hwi.h>
#include <que.h>
#include <gbl.h>
#include <sys.h>
#include <string.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <_bitops.h>
#include <dbc.h>
#include <failure.h>
#include <hal_cache.h>
#include <hal_interrupt.h>
#include <dsplinkips.h>
#include <ips.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define FILEID               FID_IPS_C

/** ============================================================================
 *  @const  IPS_MEM_ALIGN
 *
 *  @desc   Default alignment to be used for local memory allocations.
 *  ============================================================================
 */
#define IPS_MEM_ALIGN  0

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


/** ============================================================================
 *  @name   IPS_EventListener
 *
 *  @desc   Defines the Event Listener object, which contains information for
 *          each registered listener for an event.
 *
 *  @field  element
 *              Structure that allows it to be used by LIST.
 *  @field  fnIpsCbck
 *              Callback function for the event.
 *  @field  cbckArg
 *              Parameters passed to the callback.
 *  ============================================================================
 */
typedef struct IPS_EventListener_tag {
    QUE_Elem    element   ;
    FnIpsCbck   fnIpsCbck ;
    Ptr         cbckArg   ;
} IPS_EventListener ;

/** ============================================================================
 *  @name   IPS_EventList
 *
 *  @desc   Defines the Event object, which contains the event-specific
 *          information.
 *
 *  @field  eventHandlerCount
 *              Number of listener attached to the event.
 *  @field  listeners
 *              Queue of listeners.
 *  ============================================================================
 */
typedef struct IPS_EventList_tag {
    Uint32      eventHandlerCount ;
    QUE_Obj     listeners         ;
} IPS_EventList ;

/** ============================================================================
 *  @name   IPS_Object
 *
 *  @desc   Defines the IPS object.
 *
 *  @field  ipsEventList
 *              Pointer to array containing information about registered
 *              listeners and events.
 *  @field  reservedEventMask
 *              Mask for reserved event numbers.
 *  @field  ipsCtrlPtr
 *              Pointer to the control structure with information about the IPS
 *              component.
 *  @field  ctrlPtr
 *              Pointer to the IPS control structure.
 *  ============================================================================
 */
typedef struct IPS_Object_tag {
    IPS_EventList *   ipsEventList ;
    Uint32            reservedEventMask ;
    DSPLINKIPS_Ctrl * ipsCtrlPtr ;
    IPS_Ctrl *        ctrlPtr ;
} IPS_Object ;


/** ----------------------------------------------------------------------------
 *  @name   IPS_object
 *
 *  @desc   Array of IPS objects connected to the GPP.
 *  ----------------------------------------------------------------------------
 */
static IPS_Object IPS_object [MAX_IPS] ;

/*  ----------------------------------------------------------------------------
 *  @name   IPS_isInitialized
 *
 *  @desc   Flag for tracking initialization of component.
 *  ----------------------------------------------------------------------------
 */
static Bool IPS_isInitialized [MAX_IPS] = {FALSE, FALSE, FALSE, FALSE,
                                           FALSE, FALSE, FALSE, FALSE,
                                           FALSE, FALSE, FALSE, FALSE,
                                           FALSE, FALSE, FALSE, FALSE} ;


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
 *  @ret    None
 *
 *  @enter  None.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static
Void
IPS_ISR (IN  Ptr refData) ;


/** ----------------------------------------------------------------------------
 *  @func   IPS_queSearchElement
 *
 *  @desc   This function searchs for a element the List.
 *
 *  @arg    list.
 *              Pointer to List.
 *  @arg    checkObj
 *              Listener object to be matched with queue element.
 *  @arg    listener
 *              Pointer to store found element.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ----------------------------------------------------------------------------
 */
static
Void
IPS_queSearchElement (IN  QUE_Handle           list,
                      IN  IPS_EventListener *  checkObj,
                      OUT IPS_EventListener ** listener) ;


/** ============================================================================
 *  @func   IPS_init
 *
 *  @desc   This function initializes the IPS component.
 *
 *  @modif  IPS_isInitialized [ipsId]
 *  ============================================================================
 */
Int
IPS_init (IN  Uint32      procId,
          IN  Uint32      ipsId,
          IN  Uint32      shAddr)
{
    Int               status = SYS_OK ;
    Uint32            i      = 0 ;
    IPS_EventList *   ipsEventList ;
    DSPLINKIPS_Ctrl * ipsCtrlPtr ;
    IPS_Ctrl *        ctrlPtr ;

    (Void) procId ;

    if (IPS_isInitialized [ipsId] == FALSE) {
        /* By the time that the DSP has started, it can be assumed that GPP has
         * already initialized IPS address fields within the control structure.
         */
        IPS_object [ipsId].ipsCtrlPtr = ipsCtrlPtr = (DSPLINKIPS_Ctrl *)shAddr ;
        IPS_object [ipsId].ctrlPtr    = ctrlPtr
                                      = (IPS_Ctrl *) ipsCtrlPtr->dspAddr ;
        IPS_object [ipsId].reservedEventMask = 0 ;

        /* Initialize the listener list for each event and eventHandlerCount to
         * zero.
         */
        IPS_object [ipsId].ipsEventList = MEM_calloc (DSPLINK_SEGID,
                                            (  sizeof (IPS_EventList)
                                             * ipsCtrlPtr->config.numIpsEvents),
                                            IPS_MEM_ALIGN) ;
        if (IPS_object [ipsId].ipsEventList != MEM_ILLEGAL) {
            ipsEventList = IPS_object [ipsId].ipsEventList ;
            for (i = 0 ; (i < ipsCtrlPtr->config.numIpsEvents) ; i++) {
                ctrlPtr->dspAddrDspRegChart [i].regEventNo = (Uint32) -1 ;
                ipsEventList [i].eventHandlerCount = 0 ;
                QUE_new (&ipsEventList [i].listeners) ;
            }

            if (ipsCtrlPtr->config.dspIntId != (Uint32) -1) {
                /* Check whether GPP->DSP IPS is supported. */
                HAL_intRegister (ipsCtrlPtr->config.dspIntId,
                                 ipsCtrlPtr->config.dspIntVectorId,
                                 (Fxn) IPS_ISR,
                                 &(IPS_object [ipsId])) ;
            }

            IPS_isInitialized [ipsId] = TRUE ;
        }
        else {
            status = SYS_EALLOC ;
            SET_FAILURE_REASON (status) ;
        }
    }

    return status ;
}

/** ============================================================================
 *  @func   IPS_exit
 *
 *  @desc   This function Finalizes the IPS component.
 *
 *  @modif  IPS_isInitialized [ipsId]
 *  ============================================================================
 */
Void
IPS_exit (IN  Uint32    procId,
          IN  Uint32    ipsId)
{
    DBC_require (IPS_isInitialized [ipsId] == TRUE) ;

    (Void) procId ;

    if (IPS_isInitialized [ipsId] == TRUE) {
        /* Reset eventHandlerCount for all events. */
        if (IPS_object [ipsId].ipsEventList != NULL) {
            MEM_free (DSPLINK_SEGID,
                      IPS_object [ipsId].ipsEventList,
                      sizeof (IPS_EventList)) ;
            IPS_object [ipsId].ipsEventList = NULL ;
        }

        IPS_object [ipsId].ctrlPtr    = NULL ;
        IPS_object [ipsId].ipsCtrlPtr = NULL ;
        IPS_object [ipsId].reservedEventMask = 0 ;
        IPS_isInitialized [ipsId] = FALSE ;
    }
}


/** ============================================================================
 *  @func   IPS_register
 *
 *  @desc   This function registers a callback from an event.
 *
 *  @modif  ipsEventList
 *  ============================================================================
 */
Int
IPS_register (IN   Uint32    procId,
              IN   Uint32    ipsId,
              IN   Uint32    eventNo,
              IN   FnIpsCbck fnIpsCbck,
              IN   Ptr       cbckArg)
{
    Int                 status        = SYS_OK ;
    Bool                done          = FALSE  ;
    Bool                firstReg      = FALSE  ;
    Bool                isNotReserved = FALSE  ;
    IPS_EventListener * eventListener ;
    IPS_EventList *     ipsEventList  ;
    IPS_EventEntry *    dspEventChart ;
    Uint32              irqFlags      ;
    IPS_Ctrl *          ctrlPtr       ;
    IPS_EventRegEntry * regChart      ;
    Int32               i             ;
    Int32               j             ;

    DBC_require (IPS_isInitialized [ipsId] == TRUE) ;
    DBC_require (fnIpsCbck != NULL) ;

    (Void) procId ;

#if (PHYINTERFACE != PCI_INTERFACE)
    if (IPS_object [ipsId].ipsCtrlPtr->config.dspIntId == (Uint32) -1) {
        /* Check whether GPP->DSP IPS is supported. */
        status = SYS_EMODE ;
    }
    else
#endif
    if (   (fnIpsCbck != NULL)
        && (eventNo < IPS_object [ipsId].ipsCtrlPtr->config.numIpsEvents)) {

        isNotReserved = IPS_isNotReserved (procId, ipsId, eventNo) ;

        if (isNotReserved == TRUE) {
            ctrlPtr = IPS_object [ipsId].ctrlPtr ;
            eventListener = MEM_calloc (DSPLINK_SEGID,
                    sizeof (IPS_EventListener),
                    IPS_MEM_ALIGN) ;
            if (eventListener != MEM_ILLEGAL) {
                irqFlags = HWI_disable () ;
                ipsEventList = IPS_object [ipsId].ipsEventList ;
                eventListener->fnIpsCbck = fnIpsCbck ;
                eventListener->cbckArg   = cbckArg   ;
                /* Check if this is the first registration for this event. */
                if (QUE_empty (&ipsEventList [eventNo].listeners)) {
                    firstReg = TRUE ;
                    dspEventChart = ctrlPtr->dspAddrDspEventChart ;
                    /* Clear the unserviced event as there are no listeners for
                     * the pending event.
                     */
                    dspEventChart [eventNo].flag = DOWN ;
                    HAL_cacheWbInv ((Ptr) &(dspEventChart [eventNo]),
                                    sizeof (IPS_EventEntry)) ;
                }
                QUE_enqueue (&ipsEventList [eventNo].listeners,
                        &(eventListener->element)) ;
                ipsEventList [eventNo].eventHandlerCount++ ;

                /* Add an entry for the registered event into the Event Registration
                 * Chart, in ascending order of event numbers (and priorities).
                 */
                if (firstReg == TRUE) {
                    regChart = ctrlPtr->dspAddrDspRegChart ;
                    for (i = 0 ;
                            (i < IPS_object [ipsId].ipsCtrlPtr->config.numIpsEvents)
                            && !done ;
                            i++) {
                        /* Find the correct slot in the registration array. */
                        if (regChart [i].regEventNo == (Uint32) -1) {
                            for (j = (i - 1) ; j >= 0 ; j--) {
                                if (eventNo < regChart [j].regEventNo) {
                                    regChart [j + 1].regEventNo =
                                        regChart [j].regEventNo ;
                                    regChart [j + 1].reserved =
                                        regChart [j].reserved ;
                                    i = j ;
                                }
                                else {
                                    /* End the loop, slot found. */
                                    j = -1 ;
                                }
                            }

                            regChart [i].regEventNo = eventNo ;
                            done = TRUE ;
                        }
                    }

                    if (done) {
                        SET_BIT (ctrlPtr->dspRegMask.mask, eventNo) ;
                    }
                    else {
                        status = SYS_EINVAL ;
                        QUE_remove (&(eventListener->element)) ;
                    }
                }
                HWI_restore (irqFlags) ;

                HAL_cacheWbInv ((Ptr) &(ctrlPtr->dspRegMask.mask),
                        sizeof (IPS_EventRegMask)) ;
            }
            else {
                status = SYS_EALLOC ;
            }
        }
        else {
            status = SYS_EBUSY ;
        }
    }
    else {
        status = SYS_EINVAL ;
    }

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
Int
IPS_unregister (IN  Uint32      procId,
                IN  Uint32      ipsId,
                IN  Uint32      eventNo,
                IN  FnIpsCbck   fnIpsCbck,
                IN  Ptr         cbckArg)
{
    Int                 status   = SYS_OK ;
    IPS_EventListener * listener = NULL   ;
    Bool                done     = FALSE  ;
    Uint32              numIpsEvents      ;
    IPS_EventList *     ipsEventList      ;
    Uint32              irqFlags          ;
    IPS_EventRegEntry * regChart          ;
    IPS_EventEntry *    dspEventChart     ;
    IPS_Ctrl *          ctrlPtr           ;
    IPS_EventListener   unregInfo         ;
    Int32               i                 ;
    Int32               j                 ;

    DBC_require (IPS_isInitialized [ipsId] == TRUE) ;
    numIpsEvents = IPS_object [ipsId].ipsCtrlPtr->config.numIpsEvents ;
    DBC_require (fnIpsCbck != NULL) ;

    (Void) procId ;

    ctrlPtr = IPS_object [ipsId].ctrlPtr ;

    if (IPS_object [ipsId].ipsCtrlPtr->config.dspIntId == (Uint32) -1) {
        /* Check whether GPP->DSP IPS is supported. */
        status = SYS_EMODE ;
    }
    else {
        irqFlags = HWI_disable () ;
        /* Remove the listener information into the listener list. and decrease
         * the eventHandlerCount by 1.
         */
        ipsEventList = IPS_object [ipsId].ipsEventList ;

        unregInfo.fnIpsCbck = fnIpsCbck ;
        unregInfo.cbckArg   = cbckArg ;
        IPS_queSearchElement (&ipsEventList [eventNo].listeners,
                              &unregInfo,
                              &listener) ;
        HWI_restore (irqFlags) ;

        if (listener != NULL) {
            irqFlags = HWI_disable () ;
            QUE_remove (&(listener->element)) ;
            ipsEventList [eventNo].eventHandlerCount-- ;

            MEM_free (DSPLINK_SEGID, listener, sizeof (IPS_EventListener)) ;

            /* Remove the entry from Registration Chart. The entries are stored
             * in ascending order of event numbers (and priority).
             * The entry must be removed only if this is the last unregistration
             * for this event number.
             */
            if (QUE_empty (&ipsEventList [eventNo].listeners)) {
                CLEAR_BIT (ctrlPtr->dspRegMask.mask, eventNo) ;
                HAL_cacheWbInv ((Ptr) &(ctrlPtr->dspRegMask.mask),
                                sizeof (IPS_EventRegMask)) ;
                dspEventChart = ctrlPtr->dspAddrDspEventChart ;
                /* Clear the unserviced event as there are no listeners for
                 * the pending event.
                 */
                dspEventChart [eventNo].flag = DOWN ;
                HAL_cacheWbInv ((Ptr) &(dspEventChart [eventNo]),
                                sizeof (IPS_EventEntry)) ;
                regChart = ctrlPtr->dspAddrDspRegChart ;
                for (i = 0 ; (i < numIpsEvents) && !done ; i++) {
                    /* Find the correct slot in the registration array. */
                    if (eventNo == regChart [i].regEventNo) {
                        regChart [i].regEventNo = (Uint32) -1 ;
                        for (j = (i + 1) ;
                                 (regChart [j].regEventNo != (Uint32) -1)
                             &&  (j != numIpsEvents) ;
                             j++) {
                            regChart [j - 1].regEventNo =
                                                       regChart [j].regEventNo ;
                            regChart [j - 1].reserved   = regChart [j].reserved;
                        }

                        if (j == numIpsEvents) {
                            regChart [j - 1].regEventNo = (Uint32) -1 ;
                        }

                        done = TRUE ;
                    }
                }
            }
            HWI_restore (irqFlags) ;
        }
        else {
            status = SYS_ENOTFOUND ;
        }
    }

    return status ;
}


/** ============================================================================
 *  @func   IPS_notify
 *
 *  @desc   This function notifys the other side about an event.
 *
 *  @modif  ipsEventChart
 *  ============================================================================
 */
Int
IPS_notify (IN  Uint32      procId,
            IN  Uint32      ipsId,
            IN  Uint32      eventNo,
            IN  Uint32      payload,
            IN  Bool        waitEvent)
{
    Int              status  = SYS_OK ;
    IPS_EventEntry * gppEventChart ;
    Uint32           irqFlags ;
    IPS_Ctrl *       ctrlPtr  ;

    DBC_require (IPS_isInitialized [ipsId] == TRUE) ;
    DBC_require (eventNo < IPS_object [ipsId].ipsCtrlPtr->config.numIpsEvents) ;

    (Void) procId ;

    ctrlPtr = IPS_object [ipsId].ctrlPtr ;
    gppEventChart = ctrlPtr->dspAddrGppEventChart ;

#if (PHYINTERFACE != PCI_INTERFACE)
    if (IPS_object [ipsId].ipsCtrlPtr->config.gppIntId == (Uint32) -1) {
        /* Check whether DSP->GPP IPS is supported. */
        status = SYS_EMODE ;
    }
    else
#endif
    if (eventNo < IPS_object [ipsId].ipsCtrlPtr->config.numIpsEvents) {
        HAL_cacheInv ((Ptr) &(ctrlPtr->gppRegMask.mask),
                      sizeof (IPS_EventRegMask)) ;
        if (!TEST_BIT (ctrlPtr->gppRegMask.mask, eventNo)) {
            status = SYS_ENODEV ;
        }
        else {
            irqFlags = HWI_disable () ;

            if (waitEvent == TRUE) {
                /* -------------------------------------------------------------
                 * Wait for completion of previous event from other side.
                 * -------------------------------------------------------------
                 */
                do {
                    HWI_restore (irqFlags) ;
                    irqFlags = HWI_disable () ;
                    HAL_cacheInv ((Ptr) &(gppEventChart [eventNo]),
                                  sizeof (IPS_EventEntry)) ;
                }
                while (gppEventChart [eventNo].flag != DOWN) ;
                /* Recheck gpp RegMask. As Other  side might have cleared
                 * gppRegMask.mask when multiple  threads  on DSP side
                 * looping here
                 */
                HAL_cacheInv ((Ptr) &(ctrlPtr->gppRegMask.mask),
                      sizeof (IPS_EventRegMask)) ;
                if (!TEST_BIT (ctrlPtr->gppRegMask.mask, eventNo)) {
                    status = SYS_ENODEV ;
                }
            }

            if (status == SYS_OK) {

                /* -------------------------------------------------------------
                 * Set the event bit field and payload.
                 * -------------------------------------------------------------
                 */
                gppEventChart [eventNo].payload = payload ;
                gppEventChart [eventNo].flag    = UP ;
                HAL_cacheWbInv ((Ptr) &(gppEventChart [eventNo]),
                                sizeof (IPS_EventEntry)) ;

                /* -------------------------------------------------------------
                 * Send an interrupt with the event information to the GPP
                 * -------------------------------------------------------------
                 */
                HAL_intSend (IPS_object [ipsId].ipsCtrlPtr->config.gppIntId,
                             eventNo) ;
            }
            HWI_restore (irqFlags) ;

        }
    }
    else {
        status = SYS_EINVAL ;
    }

    return status ;
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
static
Void
IPS_ISR (IN Ptr refData)
{
    Uint32              payload   = 0 ;
    Uint32              i         = 0 ;
    QUE_Elem  *         temp          ;
    Uint32              j             ;
    IPS_EventEntry *    dspEventChart ;
    IPS_Object *        ipsObject ;
    IPS_EventRegEntry * regChart ;
    IPS_Ctrl *          ctrlPtr ;
    Uint32              eventNo ;

    ipsObject = (IPS_Object *) refData ;
    DBC_require (ipsObject != NULL) ;

    /* Clear the GPP interrupt */
    HAL_intClear (ipsObject->ipsCtrlPtr->config.dspIntId) ;

    ctrlPtr       = ipsObject->ctrlPtr ;
    regChart      = ctrlPtr->dspAddrDspRegChart ;
    dspEventChart = ctrlPtr->dspAddrDspEventChart ;

    /* Execute the loop till no asserted event is found for one complete loop
     * through all registered events.
     */
    do {
        /* Check if the entry is a valid registered event. */
        eventNo = regChart [i].regEventNo ;
        if (eventNo != (Uint32) -1) {
            /* Determine the current high priority event. */
            HAL_cacheInv ((Ptr) &(dspEventChart [eventNo]),
                          sizeof (IPS_EventEntry)) ;
            /* Check for the event. */
            if (dspEventChart [eventNo].flag == UP) {
                payload = dspEventChart [eventNo].payload ;

                /* acknowledge the event. */
                dspEventChart [eventNo].flag = DOWN ;
                HAL_cacheWbInv ((Ptr) &(dspEventChart [eventNo]),
                                sizeof (IPS_EventEntry)) ;
                /* Call the callbacks associated with the event. */
                temp = QUE_head (&(ipsObject->ipsEventList [eventNo].listeners));
                for (j = 0 ;
                     j < ipsObject->ipsEventList [eventNo].eventHandlerCount;
                     j++) {
                    if (    temp
                        !=  &(ipsObject->ipsEventList [eventNo].listeners)) {
                        ((IPS_EventListener *) temp)->fnIpsCbck (
                                          eventNo,
                                          ((IPS_EventListener *) temp)->cbckArg,
                                          (Ptr) payload) ;
                        temp = QUE_next (temp) ;
                    }
                }
                /* reinitialize the event check counter. */
                i = 0 ;
            }
            else {
                /* check for next event. */
                i++ ;
            }
        }
    }
    while (eventNo != (Uint32) -1) ;
}

/** ----------------------------------------------------------------------------
 *  @func   IPS_queSearchElement
 *
 *  @desc   This function searchs for a element the List.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
static
Void
IPS_queSearchElement (IN  QUE_Handle           list,
                      IN  IPS_EventListener *  checkObj,
                      OUT IPS_EventListener ** listener)
{
    QUE_Elem    *       temp   = NULL  ;
    IPS_EventListener * lTemp  = NULL  ;
    Bool                found  = FALSE ;

    DBC_require (list     != NULL) ;
    DBC_require (checkObj != NULL) ;
    DBC_require (listener != NULL) ;

    if (listener != NULL) {
        *listener = NULL ;

        if ((list != NULL) && (checkObj != NULL)) {
            if (!QUE_empty (list)) {
                temp = QUE_head (list) ;
                while ((found == FALSE) && (temp != list)) {
                    lTemp = (IPS_EventListener *) (temp) ;
                    if (    (lTemp->fnIpsCbck == checkObj->fnIpsCbck)
                        &&  (lTemp->cbckArg   == checkObj->cbckArg)) {
                        found = TRUE ;
                    }
                    else {
                        temp = QUE_next (temp) ;
                    }
                }

                if (found == TRUE) {
                    *listener = lTemp ;
                }
            }
        }

    }
}


/** ============================================================================
 *  @func   IPS_setReserved
 *
 *  @desc   This function reserves specified ips Id, event number.
 *
 *  @modif  IPS_State.
 *  ============================================================================
 */
Int
IPS_setReserved (IN   Uint32    procId,
                 IN   Uint32    ipsId,
                 IN   Uint32    eventNo)
{
    Int           status = SYS_OK ;
    IPS_Object *  ipsState ;

    (Void) procId ;

    ipsState  = &(IPS_object [ipsId]) ;

    if (TEST_BIT (ipsState->reservedEventMask, eventNo) != 1u) {
        SET_BIT (ipsState->reservedEventMask, eventNo) ;
    }
    else {
        status = SYS_EBUSY ;
    }

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
Int
IPS_clearReserved (IN     Uint32   procId,
                   IN     Uint32   ipsId,
                   IN     Uint32   eventNo)
{
    IPS_Object *  ipsState ;
    Int           status   = SYS_OK ;

    (Void) procId ;

    ipsState  = &(IPS_object [ipsId]) ;
    if (TEST_BIT (ipsState->reservedEventMask, eventNo) == 1u) {
        CLEAR_BIT (ipsState->reservedEventMask, eventNo) ;
    }
    else {
        status = SYS_EINVAL ;
    }

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
Bool
IPS_isNotReserved (IN     Uint32   procId,
                   IN     Uint32   ipsId,
                   IN     Uint32   eventNo)
{
    IPS_Object *  ipsState ;
    Bool          retVal = FALSE ;

    (Void) procId ;

    ipsState  = &(IPS_object [ipsId]) ;
    if (TEST_BIT (ipsState->reservedEventMask, eventNo) != 1u) {
        retVal = TRUE ;
    }

    return retVal ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
