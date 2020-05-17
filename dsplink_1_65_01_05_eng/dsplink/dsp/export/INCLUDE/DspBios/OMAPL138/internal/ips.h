/** ============================================================================
 *  @file   ips.h
 *
 *  @path   $(DSPLINK)/dsp/src/base/ips/
 *
 *  @desc   Defines the interface of the Inter Processor Signaling component.
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


#if !defined (IPS_H_)
#define IPS_H_


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>


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
 *              Fixed argument optionally registered with the IPS component
 *              along with the callback function.
 *  @arg    info
 *              Optional run-time information provided to the upper layer by the
 *              IPS component. This information is specific to the IPS being
 *              implemented and passed by sending-side if required.
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
                           IN     Ptr      arg,
                           IN     Ptr      info) ;


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
                             IN     Ptr      arg,
                             IN     Ptr      info,
                             IN     Ptr      extra) ;


/** ============================================================================
 *  @func   IPS_init
 *
 *  @desc   This function initializes the IPS component.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    shAddr
 *              Address in shared memory for the IPS control information.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid argument.
 *          SYS_EALLOC
 *              Memory allocation failure.
 *
 *  @enter  None.
 *
 *  @leave  On success, the component must be initialized.
 *
 *  @see    IPS_exit ()
 *  ============================================================================
 */
Int
IPS_init (IN  Uint32      procId,
          IN  Uint32      ipsId,
          IN  Uint32      shAddr) ;


/** ============================================================================
 *  @func   IPS_exit
 *
 *  @desc   This function finalizes the IPS component.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *
 *  @ret    None.
 *
 *  @enter  The IPS component must be initialized before calling this
 *          function.
 *
 *  @leave  On success, the component must be finalized.
 *
 *  @see    IPS_init ()
 *  ============================================================================
 */
Void
IPS_exit (IN  Uint32    procId,
          IN  Uint32    ipsId) ;


/** ============================================================================
 *  @func   IPS_register
 *
 *  @desc   This function registers a callback for a specific event with the
 *          IPS component.
 *
 *  @arg    procId
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
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid argument.
 *          SYS_EALLOC
 *              Memory allocation failure.
 *          SYS_EMODE
 *              IPS is not supported for GPP->DSP mode.
 *          SYS_EBUSY
 *              Specified ips id, event number is reserved for LINK protocols
 *              like MSGQ, CHNL, RingIO etc.
 *
 *  @enter  The IPS component must be initialized before calling this
 *          function.
 *          The fnIpsCbck argument must be valid.
 *          The event must be supported by the IPS component.
 *
 *  @leave  On success, the event must be registered with the IPS component.
 *
 *  @see    FnIpsCbck, IPS_unregister ()
 *  ============================================================================
 */
Int
IPS_register (IN   Uint32    procId,
              IN   Uint32    ipsId,
              IN   Uint32    eventNo,
              IN   FnIpsCbck fnIpsCbck,
              IN   Ptr       cbckArg) ;


/** ============================================================================
 *  @func   IPS_registerEx
 *
 *  @desc   This function registers a callback for a specific event with the
 *          IPS component.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    eventNo
 *              Event No to be registered.
 *  @arg    fnIpsCbckEx
 *              Callback function to be registered for the specified event.
 *  @arg    cbckArg
 *              Optional argument to the callback function to be registered for
 *              the specified event. This argument shall be passed to each
 *              invocation of the callback function.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid argument.
 *          SYS_EALLOC
 *              Memory allocation failure.
 *          SYS_EMODE
 *              IPS is not supported for GPP->DSP mode.
 *          SYS_EBUSY
 *              Specified ips id, event number is reserved for LINK protocols
 *              like MSGQ, CHNL, RingIO etc.
 *
 *  @enter  The IPS component must be initialized before calling this
 *          function.
 *          The fnIpsCbck argument must be valid.
 *          The event must be supported by the IPS component.
 *
 *  @leave  On success, the event must be registered with the IPS component.
 *
 *  @see    FnIpsCbck, IPS_unregister ()
 *  ============================================================================
 */
Int
IPS_registerEx (IN  Uint32      procId,
                IN  Uint32      ipsId,
                IN  Uint32      eventNo,
                IN  FnIpsCbckEx fnIpsCbckEx,
                IN  Ptr         cbckArg) ;


/** ============================================================================
 *  @func   IPS_unregister
 *
 *  @desc   This function unregisters a callback for a specific event with the
 *          IPS component.
 *
 *  @arg    procId
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
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_ENOTFOUND
 *              Registered event not found.
 *          SYS_EMODE
 *              IPS is not supported for GPP->DSP mode.
 *          SYS_EBUSY
 *              Specified ips id, event number is reserved for LINK protocols
 *              like MSGQ, CHNL, RingIO etc.
 *
 *
 *  @enter  The IPS component must be initialized before calling this
 *          function.
 *          The event must be supported by the IPS component.
 *          The event must have been registered with the IPS component
 *          earlier.
 *          The fnIpsCbck argument must be valid.
 *
 *  @leave  On success, the event must be unregistered from the IPS component
 *
 *  @see    IPS_register ()
 *  ============================================================================
 */
Int
IPS_unregister (IN  Uint32      procId,
                IN  Uint32      ipsId,
                IN  Uint32      eventNo,
                IN  FnIpsCbck   fnIpsCbck,
                IN  Ptr         cbckArg) ;


/** ============================================================================
 *  @func   IPS_notify
 *
 *  @desc   This function sends a notification of an event to the DSP.
 *
 *  @arg    procId
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
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_ENODEV
 *              Event is not registered on the remote processor.
 *          SYS_EINVAL
 *              Invalid argument.
 *          SYS_EMODE
 *              IPS is not supported for DSP->GPP mode.
 *
 *  @enter  The IPS component must be initialized before calling this
 *          function.
 *          The event must be supported by the IPS component.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
Int
IPS_notify (IN  Uint32      procId,
            IN  Uint32      ipsId,
            IN  Uint32      eventNo,
            IN  Uint32      payload,
            IN  Bool        waitEvent) ;


/** ============================================================================
 *  @func   IPS_notifyEx
 *
 *  @desc   This function sends a notification of an event to the DSP. (with
 *          extra payload)
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    eventNo
 *              Event to be notified to the DSP.
 *  @arg    payload
 *              Data to be sent with Event.
 *  @arg    exPayload
 *              Extra data to be sent with Event.
 *  @arg    waitEvent
 *              Decides whether to spin waiting for the previous event to be
 *              cleared, which ensures that payload is sent. This can be
 *              specified as FALSE if no payload is sent.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_ENODEV
 *              Event is not registered on the remote processor.
 *          SYS_EINVAL
 *              Invalid argument.
 *
 *  @enter  The IPS component must be initialized before calling this
 *          function.
 *          The event must be supported by the IPS component.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
Int
IPS_notifyEx (IN  Uint32      procId,
              IN  Uint32      ipsId,
              IN  Uint32      eventNo,
              IN  Uint32      payload,
              IN  Uint32      exPayload,
              IN  Bool        waitEvent) ;


/** ============================================================================
 *  @func   IPS_poll
 *
 *  @desc   This function checks for the event if UP, returns the payload. *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    eventNo
 *              Event to be notified to the DSP.
 *  @arg    payload
 *              Data to be get with Event.
 *  @arg    exPayload
 *              Extra data to be get with Event.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_ENODEV
 *              Event is not registered on the remote processor.
 *          SYS_EINVAL
 *              Invalid argument.
 *
 *  @enter  The IPS component must be initialized before calling this
 *          function.
 *          The event must be supported by the IPS component.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
Int
IPS_poll (IN  Uint32      procId,
          IN  Uint32      ipsId,
          IN  Uint32      eventNo,
          IN  Uint32 *    payload,
          IN  Uint32 *    exPayload) ;


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
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EBUSY
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
Int
IPS_setReserved (IN     Uint32   dspId,
                 IN     Uint32   ipsId,
                 IN     Uint32   eventNo) ;


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
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
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
Int
IPS_clearReserved (IN     Uint32   dspId,
                   IN     Uint32   ipsId,
                   IN     Uint32   eventNo) ;


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
 *              ips entry is not reserved.
 *          FALSE
 *              ips entry is reserved.
 *
 *  @enter  dspId must be a valid processor ID.
 *          The IPS component must be initialized before calling this function.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
Bool
IPS_isNotReserved (IN     Uint32   dspId,
                   IN     Uint32   ipsId,
                   IN     Uint32   eventNo) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (IPS_H_) */

