/** ============================================================================
 *  @file   notify.h
 *
 *  @path   $(DSPLINK)/dsp/inc/
 *
 *  @desc   Defines the interface of notify component.
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


#if !defined NOTIFY_H_
#define NOTIFY_H_


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   FnNotifyCbck
 *
 *  @desc   Signature of the callback function to be registered with the Notify
 *          component.
 *
 *  @arg    eventNo
 *              Event number associated with the callback being invoked.
 *  @arg    arg
 *              Fixed argument registered with the NOTIFY component along with
 *              the callback function.
 *  @arg    info
 *              Run-time information provided to the upper layer by the NOTIFY
 *              component. This information is specific to the NOTIFY being
 *              implemented.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    NOTIFY_register ()
 *  ============================================================================
 */
typedef Void (*FnNotifyCbck) (IN  Uint32 eventNo, IN  Ptr arg, IN  Ptr info) ;


/** ============================================================================
 *  @func   NOTIFY_register
 *
 *  @desc   This function registers a callback for a specific event with the
 *          NOTIFY component.
 *
 *  @arg    procId
 *              Identifier of the processor from which the notifications shall
 *              be received.
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
 *  @enter  The NOTIFY component must be initialized before calling this
 *          function.
 *
 *  @leave  On success, the event must be registered with the NOTIFY component.
 *
 *  @see    FnNotifyCbck, NOTIFY_unregister ()
 *  ============================================================================
 */
Int
NOTIFY_register (IN  Uint32       procId,
                 IN  Uint32       ipsId,
                 IN  Uint32       eventNo,
                 IN  FnNotifyCbck fnNotifyCbck,
                 IN  Ptr          cbckArg) ;


/** ============================================================================
 *  @func   NOTIFY_unregister
 *
 *  @desc   This function unregisters a callback for a specific event with the
 *          NOTIFY component.
 *
 *  @arg    procId
 *              Identifier of the processor from which the notifications shall
 *              no longer be received.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    eventNo
 *              Event to be unregistered.
 *  @arg    fnNotifyCbck
 *              Callback function to be registered for the specified event.
 *  @arg    cbckArg
 *              Optional argument to the callback function registered for the
 *              the specified event.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_ENOTFOUND
 *              Registered event not found.
 *          SYS_EMODE
 *              IPS is not supported for GPP->DSP mode.
 *
 *  @enter  The NOTIFY component must be initialized before calling this
 *          function.
 *          The fnNotifyCbck argument must be valid.
 *
 *  @leave  On success, the event must be unregistered from the NOTIFY component
 *
 *  @see    NOTIFY_register ()
 *  ============================================================================
 */
Int
NOTIFY_unregister (IN  Uint32         procId,
                   IN  Uint32         ipsId,
                   IN  Uint32         eventNo,
                   IN  FnNotifyCbck   fnNotifyCbck,
                   IN  Void *         cbckArg) ;


/** ============================================================================
 *  @func   NOTIFY_notify
 *
 *  @desc   This function sends a notification of an event to the processor.
 *
 *  @arg    procId
 *              Identifier of the processor to which the notification is to be
 *              sent.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    eventNo
 *              Event to be notified to the processor.
 *  @arg    payload
 *              Data to be sent with Event.
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
 *  @enter  The NOTIFY component must be initialized before calling this
 *          function.
 *          The event must be supported by the NOTIFY component.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
Int
NOTIFY_notify (IN  Uint32      procId,
               IN  Uint32      ipsId,
               IN  Uint32      eventNo,
               IN  Uint32      payload) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (NOTIFY_H_) */

