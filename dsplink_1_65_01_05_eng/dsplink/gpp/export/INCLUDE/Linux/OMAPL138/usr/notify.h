/** ============================================================================
 *  @file   notify.h
 *
 *  @path   $(DSPLINK)/gpp/inc/usr/
 *
 *  @desc   Defines the interface of the Notify component.
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
