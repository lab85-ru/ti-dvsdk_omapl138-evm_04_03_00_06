/** ============================================================================
 *  @file   notify.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Defines the interface of the Notify component.
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


#if !defined (NOTIFY_H)
#define NOTIFY_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Profiling                   */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE)*/

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   FnNotifyCbck
 *
 *  @desc   Signature of the callback function to be registered with the NOTIFY
 *          component.
 *
 *  @arg    eventNo
 *              Event number associated with the callback being invoked.
 *  @arg    arg
 *              Fixed argument registered with the IPS component along with
 *              the callback function.
 *  @arg    info
 *              Run-time information provided to the upper layer by the NOTIFY
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
typedef Void (*FnNotifyCbck) (IN     Uint32 eventNo,
                              IN     Pvoid  arg,
                              IN     Pvoid  info) ;


/** ============================================================================
 *  @func   NOTIFY_register
 *
 *  @desc   This function registers a callback for a specific event with the
 *          NOTIFY component.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    eventNo
 *              Event No to be registered.
 *  @arg    fnNotifyCbck
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
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_ENOTSUPPORTED
 *              IPS is not supported in DSP->GPP direction.
 *          DSP_RESOURCE
 *              Specified ips id, event number is reserved for LINK protocols
 *              like MSGQ, CHNL, RingIO etc.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  The fnNotifyCbck argument must be valid.
 *          The event must be supported by the IPS component.
 *          dspId must be valid.
 *          PROC_attach has been successful.
 *
 *  @leave  On success, the event must be registered with the IPS component.
 *
 *  @see    FnNotifyCbck, NOTIFY_unregister ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_register (IN     ProcessorId    dspId,
                 IN     Uint32         ipsId,
                 IN     Uint32         eventNo,
                 IN     FnNotifyCbck   fnNotifyCbck,
                 IN     Pvoid          cbckArg) ;


/** ============================================================================
 *  @func   NOTIFY_unregister
 *
 *  @desc   This function unregisters a callback for a specific event with the
 *          NOTIFY component.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    event
 *              Event to be unregistered.
 *  @arg    fnNotifyCbck
 *              Callback function to be registered for the specified event.
 *  @arg    cbckArg
 *              Optional argument to the callback function registered for the
 *              the specified event.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EWRONGSTATE
 *              IPS not initialized.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_ENOTSUPPORTED
 *              IPS is not supported in DSP->GPP direction.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  The event must be supported by the NOTIFY component.
 *          The event must have been registered with the NOTIFY component
 *          earlier.
 *          The fnNotifyCbck argument must be valid.
 *          All Notifications are complete.
 *
 *  @leave  On success, the event must be unregistered from the IPS component
 *
 *  @see    NOTIFY_register ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_unregister (IN     ProcessorId    dspId,
                   IN     Uint32         ipsId,
                   IN     Uint32         eventNo,
                   IN     FnNotifyCbck   fnNotifyCbck,
                   IN     Pvoid          cbckArg) ;


/** ============================================================================
 *  @func   NOTIFY_notify
 *
 *  @desc   This function sends a notification of an event to the DSP.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    eventno
 *              Event to be notified to the DSP.
 *  @arg    payload
 *              Data to be sent with Event (Optional).
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EWRONGSTATE
 *              IPS not initialized.
 *          DSP_ENOTSUPPORTED
 *              IPS is not supported in GPP->DSP direction.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *
 *  @enter  The event must be supported by the NOTIFY component.
 *          The notification component must be initialized.
 *          NOTIFY_register has been successful.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_notify (IN     ProcessorId dspId,
               IN     Uint32      ipsId,
               IN     Uint32      eventno,
               IN     Uint32      payload) ;


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   NOTIFY_instrument
 *
 *  @desc   Gets the instrumentation information related to IPS's
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    retVal
 *              Instrumentation structure.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *
 *  @enter  The dspId, retVal must be valid.
 *          PROC_setup and PROC_attach have been successful.
 *          Profiling has been selected in the configuration.
 *
 *
 *  @leave  None.
 *
 *  @see    None.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_instrument (IN  ProcessorId       dspId,
                   IN  Uint32            ipsId,
                   OUT IPS_Instrument *  retVal) ;
#endif /* defined (DDSP_PROFILE) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (NOTIFY_H) */
