/** ============================================================================
 *  @file   ips.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/IPS/
 *
 *  @desc   Defines the interface of the Inter Processor Signaling component.
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


#if !defined (IPS_H)
#define IPS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <ips.h>

#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   IPS_EventEntry
 *
 *  @desc   Defines the structure of event entry within the event chart.
 *          Each entry contains occured event-specific information.
 *          This is shared between GPP and DSP.
 *
 *  @field  flag
 *              Indicating event is present or not.
 *  @field  payload
 *              Variable containing data associated with each occured event.
 *  @field  reserved
 *              Reserved field to contain additional information about the
 *              event entry.
 *  @field  padding
 *              Padding.
 *  ============================================================================
 */
typedef struct IPS_EventEntry_tag {
    volatile Uint32  flag     ;
    volatile Uint32  payload  ;
    volatile Uint32  reserved ;
    ADD_PADDING(padding, IPS_EVENT_ENTRY_PADDING)
} IPS_EventEntry ;

/** ============================================================================
 *  @name   IPS_EventRegMask
 *
 *  @desc   Defines the mask indicating registered events on the processor.
 *          This is shared between GPP and DSP.
 *
 *  @field  mask
 *              Indicating event is registered.
 *  @field  padding
 *              Padding.
 *  ============================================================================
 */
typedef struct IPS_EventRegMask_tag {
    volatile Uint32 mask ;
    ADD_PADDING (padding, DSPLINK_32BIT_PADDING)
} IPS_EventRegMask ;

/** ============================================================================
 *  @name   IPS_EventRegEntry
 *
 *  @desc   Defines the structure of event registration entry within the Event
 *          Registration Chart.
 *          Each entry contains registered event-specific information.
 *
 *  @field  regEventNo
 *              Index into the event chart, indicating the registered event.
 *  @field  reserved
 *              Reserved field to contain additional information about the
 *              registered event.
 *  ============================================================================
 */
typedef struct IPS_EventRegEntry_tag {
    Uint32     regEventNo ;
    Uint32     reserved ;
} IPS_EventRegEntry ;

/** ============================================================================
 *  @name   IPS_Info
 *
 *  @desc   Defines the structure for getting IPS information.
 *
 *  @field  physAddr
 *              Physical address of IPS control structures.
 *  @field  size
 *              Size of the control structure.
 *  ============================================================================
 */
typedef struct IPS_Info_tag {
    Uint32     physAddr ;
    Uint32     size ;
} IPS_Info ;

/** ============================================================================
 *  @name   IPS_Ctrl
 *
 *  @desc   Defines the IPS control structure, which contains all information
 *          shared between the GPP and DSP IPS.
 *
 *  @field  gppRegChart
 *              GPP address of GPP-side event registration chart.
 *  @field  dspAddrDspRegChart
 *              DSP address of DSP-side event registration chart.
 *  @field  gppEventChart
 *              GPP address of GPP-side event chart.
 *  @field  dspEventChart
 *              GPP address of DSP-side event chart.
 *  @field  dspAddrGppEventChart
 *              DSP address of GPP-side event chart.
 *  @field  dspAddrDspEventChart
 *              DSP address of DSP-side event chart.
 *  @field  padding
 *              Padding for alignment.
 *  @field  gppRegMask
 *              GPP registration mask.
 *  @field  dspRegMask
 *              DSP registration mask
 *  ============================================================================
 */
typedef struct IPS_Ctrl_tag {
    IPS_EventRegEntry *  gppRegChart ;
    IPS_EventRegEntry *  dspAddrDspRegChart ;
    IPS_EventEntry *     gppEventChart ;
    IPS_EventEntry *     dspEventChart ;
    IPS_EventEntry *     dspAddrGppEventChart ;
    IPS_EventEntry *     dspAddrDspEventChart ;
    ADD_PADDING(padding, IPS_CTRL_PADDING)

    IPS_EventRegMask     gppRegMask ;
    IPS_EventRegMask     dspRegMask ;
} IPS_Ctrl ;


/** ============================================================================
 *  @name   FnIpsCbck
 *
 *  @desc   Signature of the callback function to be registered with the IPS
 *          component.
 *
 *  @arg    eventNo
 *              Event number associated with the callback being invoked.
 *  @arg    arg
 *              Fixed argument registered with the IPS component along with
 *              the callback function.
 *  @arg    info
 *              Run-time information provided to the upper layer by the IPS
 *              component. This information is specific to the IPS being
 *              implemented.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    IPS_register ()
 *  ============================================================================
 */
typedef Void (*FnIpsCbck) (IN     Uint32   eventNo,
                           IN     Pvoid    arg,
                           IN     Pvoid    info) ;


/** ============================================================================
 *  @name   FnIpsCbckEx
 *
 *  @desc   Signature of the callback function to be registered with the IPS
 *          component (with extra argument to be passed at the time of calling).
 *
 *  @arg    eventNo
 *              Event number associated with the callback being invoked.
 *  @arg    arg
 *              Fixed argument registered with the IPS component along with
 *              the callback function.
 *  @arg    info
 *              Run-time information provided to the upper layer by the IPS
 *              component. This information is specific to the IPS being
 *              implemented.
 *  @arg    extra
 *              Extra run-time information provided to the upper layer by the
 *              IPS component. This information is specific to the IPS being
 *              implemented.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    IPS_registerEx ()
 *  ============================================================================
 */
typedef Void (*FnIpsCbckEx) (IN     Uint32   eventNo,
                             IN     Pvoid    arg,
                             IN     Pvoid    info,
                             IN     Pvoid    extra) ;


/** ============================================================================
 *  @func   IPS_init
 *
 *  @desc   This function initializes the IPS component.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    shDspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the IPS component.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be a valid processor ID.
 *          shDspAddr must be a valid pointer.
 *
 *  @leave  On success, the component must be initialized.
 *
 *  @see    IPS_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_init (IN  ProcessorId dspId,
          IN  Uint32      ipsId,
          OUT Uint32 *    shDspAddr) ;


/** ============================================================================
 *  @func   IPS_exit
 *
 *  @desc   This function finalizes the IPS component.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EWRONGSTATE
 *              IPS not initialized.
 *
 *  @enter  dspId must be a valid processor ID.
 *
 *  @leave  On success, the component must be finalized.
 *
 *  @see    IPS_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_exit (IN ProcessorId dspId,
          IN Uint32      ipsId) ;


/** ============================================================================
 *  @func   IPS_register
 *
 *  @desc   This function registers a callback for a specific event with the
 *          IPS component.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    eventNo
 *              Event No to be registered.
 *  @arg    fnIpsCbck
 *              Callback function to be registered for the specified event.
 *  @arg    cbckArg
 *              Optional argument to the callback function to be registered for
 *              the specified event. This argument shall be passed to each
 *              invocation of the callback function.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EWRONGSTATE
 *              IPS not initialized.
 *          DSP_ENOTSUPPORTED
 *              IPS is not supported in DSP->GPP direction.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be a valid processor ID.
 *          The fnIpsCbck argument must be valid.
 *          The IPS component must be initialized before calling this function.
 *
 *  @leave  On success, the event must be registered with the IPS component.
 *
 *  @see    FnIPSCbck, IPS_unregister ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_register (IN     ProcessorId dspId,
              IN     Uint32      ipsId,
              IN     Uint32      eventNo,
              IN     FnIpsCbck   fnIpsCbck,
              IN     Pvoid       cbckArg) ;


/** ============================================================================
 *  @func   IPS_registerEx
 *
 *  @desc   This function registers a callback for a specific event with the
 *          IPS component (callbacks are of type FnIpsCbckEx).
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    eventNo
 *              Event No to be registered.
 *  @arg    fnIpsCbck
 *              Callback function to be registered for the specified event.
 *  @arg    cbckArg
 *              Optional argument to the callback function to be registered for
 *              the specified event. This argument shall be passed to each
 *              invocation of the callback function.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EWRONGSTATE
 *              IPS not initialized.
 *          DSP_ENOTSUPPORTED
 *              IPS is not supported in DSP->GPP direction.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be a valid processor ID.
 *          The fnIpsCbck argument must be valid.
 *          The IPS component must be initialized before calling this function.
 *
 *  @leave  On success, the event must be registered with the IPS component.
 *
 *  @see    FnIPSCbck, IPS_unregister ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_registerEx (IN     ProcessorId dspId,
                IN     Uint32      ipsId,
                IN     Uint32      eventNo,
                IN     FnIpsCbckEx fnIpsCbckEx,
                IN     Pvoid       cbckArg) ;


/** ============================================================================
 *  @func   IPS_unregister
 *
 *  @desc   This function unregisters a callback for a specific event with the
 *          IPS component.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    eventNo
 *              Event to be unregistered.
 *  @arg    fnIpsCbck
 *              Callback function to be registered for the specified event.
 *  @arg    cbckArg
 *              Optional argument to the callback function registered for
 *              the specified event.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EWRONGSTATE
 *              IPS not initialized.
 *          DSP_ENOTFOUND
 *              Registered event not found.
 *          DSP_ENOTSUPPORTED
 *              IPS is not supported in DSP->GPP direction.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be a valid processor ID.
 *          The fnIpsCbck argument must be valid.
 *          The IPS component must be initialized before calling this function.
 *
 *  @leave  On success, the event must be unregistered from the IPS component
 *
 *  @see    IPS_register ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_unregister (IN     ProcessorId dspId,
                IN     Uint32      ipsId,
                IN     Uint32      eventNo,
                IN     FnIpsCbck   fnIpsCbck,
                IN     Pvoid       cbckArg) ;


/** ============================================================================
 *  @func   IPS_notify
 *
 *  @desc   This function sends a notification of an event to the DSP.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    eventNo
 *              Event to be notified to the DSP.
 *  @arg    payload
 *              Data to be sent with Event.
 *  @arg    waitEvent
 *              Decides whether to spin waiting for the previous event to be
 *              cleared, which ensures that payload is sent. This can be
 *              specified as FALSE if no payload is sent.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ENOTREADY
 *              Event is not registered on the remote processor.
 *          DSP_EWRONGSTATE
 *              IPS not initialized.
 *          DSP_ENOTSUPPORTED
 *              IPS is not supported in GPP->DSP direction.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be a valid processor ID.
 *          The IPS component must be initialized before calling this function.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_notify (IN ProcessorId dspId,
            IN Uint32      ipsId,
            IN Uint32      eventNo,
            IN Uint32      payload,
            IN Bool        waitEvent) ;


/** ============================================================================
 *  @func   IPS_notifyex
 *
 *  @desc   This function notifies the other side about an event and also carry
 *          extra payload.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    eventNo
 *              Event to be notified to the DSP.
 *  @arg    payload
 *              Data to be sent with Event.
 *  @arg    exPayload
 *              extra Data to be sent with Event.
 *  @arg    waitEvent
 *              Decides whether to spin waiting for the previous event to be
 *              cleared, which ensures that payload is sent. This can be
 *              specified as FALSE if no payload is sent.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ENOTREADY
 *              Event is not registered on the remote processor.
 *          DSP_EWRONGSTATE
 *              IPS not initialized.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be a valid processor ID.
 *          The IPS component must be initialized before calling this function.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_notifyex (IN ProcessorId dspId,
              IN Uint32      ipsId,
              IN Uint32      eventNo,
              IN Uint32      payload,
              IN Uint32      exPayload,
              IN Bool        waitEvent) ;


/** ============================================================================
 *  @func   IPS_isInitialized
 *
 *  @desc   This function is used to determine whether IPS is initialized for a
 *          dspId.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *
 *  @ret    TRUE
 *              IPS is initialized for a dspId.
 *          FALSE
 *              IPS is not initialized for a dspId.
 *
 *  @enter  None.
 *
 *  @leave  None..
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
Bool
IPS_isInitialized (IN ProcessorId dspId,
                   IN Uint32      ipsId) ;


/** ============================================================================
 *  @func   IPS_poll
 *
 *  @desc   This function waits for the event to be UP and returns the payload.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    eventNo
 *              Event to be notified to the DSP.
 *  @arg    payload
 *              Place holder for payload to be get.
 *  @arg    exPayload
 *              Place holder for extra payload to be get.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ENOTREADY
 *              Event is not registered on the remote processor.
 *          DSP_EWRONGSTATE
 *              IPS not initialized.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be a valid processor ID.
 *          The IPS component must be initialized before calling this function.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
IPS_poll (IN ProcessorId dspId,
          IN Uint32      ipsId,
          IN Uint32      eventNo,
          IN Uint32 *    payload,
          IN Uint32 *    exPayload) ;


/** ============================================================================
 *  @func   IPS_setReserved
 *
 *  @desc   This function reserves specified ips Id, event number.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier to be reserved.
 *  @arg    eventNo
 *              Event number to be reserved.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ERESOURCE
 *              Resource is already in use.
 *
 *  @enter  dspId must be a valid processor ID.
 *          The IPS component must be initialized before calling this function.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_setReserved (IN     ProcessorId   dspId,
                 IN     Uint32        ipsId,
                 IN     Uint32        eventNo) ;


/** ============================================================================
 *  @func   IPS_clearReserved
 *
 *  @desc   This function clears reserved specified ips Id, event number.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier to be cleared.
 *  @arg    eventNo
 *              Event number to be cleared.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid  ips id and event number have been provided.
 *
 *  @enter  dspId must be a valid processor ID.
 *          The IPS component must be initialized before calling this function.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_clearReserved (IN     ProcessorId   dspId,
                   IN     Uint32        ipsId,
                   IN     Uint32        eventNo) ;


/** ============================================================================
 *  @func   IPS_checkIntGen
 *
 *  @desc   This function implements check routine to check the interrupt
 *          generation. In case of shared interrupts, other devices can also
 *          generate same ints.
 *
 *  @arg    refData
 *              Argument to the interrupt handler allowing the interrupt
 *              handler to identify the associated driver information.
 *
 *  @ret    None
 *
 *  @enter  refData must not be NULL.
 *
 *  @leave  TRUE if our device has generated, otherwise FALSE.
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
Bool
IPS_checkIntGen (IN Pvoid refData) ;


/** ============================================================================
 *  @func   IPS_isNotReserved
 *
 *  @desc   This function checks if specified ips Id, event number is reserved or
 *          free.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier to be checked.
 *  @arg    eventNo
 *              Event number to be checked.
 *
 *  @ret    TRUE
 *              IPS entry is not reserved.
 *          FALSE
 *              IPS entry is reserved.
 *
 *  @enter  dspId must be a valid processor ID.
 *          The IPS component must be initialized before calling this function.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
Bool
IPS_isNotReserved (IN     ProcessorId   dspId,
                   IN     Uint32        ipsId,
                   IN     Uint32        eventNo) ;


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   IPS_debug
 *
 *  @desc   This function prints the debug information for a DSP.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *
 *  @ret    None.
 *
 *  @enter  dspId must be a valid processor ID.
 *
 *  @leave  None..
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
Void
IPS_debug (IN ProcessorId dspId,
           IN Uint32      ipsId) ;
#endif /* #if defined (DDSP_DEBUG) */


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   IPS_instrument
 *
 *  @desc   This function gets the profile information for an IPS.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    retVal
 *              Structure which contains IPS instrumentation information.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              IPS instrumentation structure is NULL.
 *
 *  @enter  dspId must be a valid processor ID.
 *          retVal must be a valid pointer.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
IPS_instrument (IN  ProcessorId       dspId,
                IN  Uint32            ipsId,
                OUT IPS_Instrument *  retVal) ;
#endif /* #if defined (DDSP_PROFILE) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (IPS_H) */

