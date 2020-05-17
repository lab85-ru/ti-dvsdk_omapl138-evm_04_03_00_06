/** ============================================================================
 *  @file   notify_knl.h
 *
 *  @path   $(DSPLINK)/gpp/src/osal/
 *
 *  @desc   Defines the interface of the kernel side Notify component.
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


#if !defined (NOTIFY_KNL_H)
#define NOTIFY_KNl_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>
#include <notify.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Profiling                   */
#if defined (DDSP_PROFILE)
#include<profile.h>
#endif /* if defined (DDSP_PROFILE) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */



/** ============================================================================
 *  @func   NOTIFY_KnlKnlInitialize
 *
 *  @desc   This function initializes the kernel side NOTIFY component.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  On success, the component must be initialized.
 *
 *  @see    NOTIFY_KnlFinalize ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_KnlInitialize (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   NOTIFY_KnlFinalize
 *
 *  @desc   This function finalizes the kernel side NOTIFY component.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EWRONGSTATE
 *              IPS not initialized.
 *
 *  @enter  The NOTIFY component must be initialized before calling this
 *          function.
 *
 *  @leave  On success, the component must be finalized.
 *
 *  @see    NOTIFY_KnlInitialize ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_KnlFinalize (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   NOTIFY_KnlRegister
 *
 *  @desc   This function registers a callback for a specific event with the
 *          kernel side NOTIFY component.
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
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  The IPS component must be initialized before calling this
 *          function.
 *          The fnNotifyCbck argument must be valid.
 *          The event must be supported by the IPS component.
 *
 *  @leave  On success, the event must be registered with the IPS component.
 *
 *  @see    FnNotifyCbck, NOTIFY_KnlUnregister ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_KnlRegister (IN     ProcessorId    dspId,
                    IN     Uint32         ipsId,
                    IN     Uint32         eventNo,
                    IN     FnNotifyCbck   fnNotifyCbck,
                    IN     Pvoid          cbckArg) ;


/** ============================================================================
 *  @func   NOTIFY_KnlUnregister
 *
 *  @desc   This function unregisters a callback for a specific event with the
 *          kernel side NOTIFY component.
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
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  The NOTIFY component must be initialized before calling this
 *          function.
 *          The event must be supported by the NOTIFY component.
 *          The event must have been registered with the NOTIFY component
 *          earlier.
 *          The fnNotifyCbck argument must be valid.
 *
 *  @leave  On success, the event must be unregistered from the IPS component
 *
 *  @see    NOTIFY_KnlRegister ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_KnlUnregister (IN     ProcessorId    dspId,
                      IN     Uint32         ipsId,
                      IN     Uint32         eventNo,
                      IN     FnNotifyCbck   fnNotifyCbck,
                      IN     Pvoid          cbckArg) ;


/** ============================================================================
 *  @func   NOTIFY_KnlNotify
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
 *
 *  @enter  The kernel side NOTIFY component must be initialized before calling
 *          this function.
 *          The event must be supported by the kernel side component.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_KnlNotify (IN     ProcessorId dspId,
                  IN     Uint32      ipsId,
                  IN     Uint32      eventno,
                  IN     Uint32      payload) ;


/** ============================================================================
 *  @func   UEVENT_Initialize
 *
 *  @desc   This function initializes the UEVENT notification component.
 *
 *  @arg    None.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
Void
UEVENT_Initialize (Void) ;


/** ============================================================================
 *  @func   UEVENT_Finalize
 *
 *  @desc   This function finalize the UEVENT notification component.
 *
 *  @arg    None.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
Void
UEVENT_Finalize (Void) ;


/** ============================================================================
 *  @func   UEVENT_AddBufByPid
 *
 *  @desc   This function adds a data to a registered process.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    eventno
 *              Event to be notified to the DSP.
 *  @arg    payload
 *              Data to be sent with Event (Optional).
 *  @arg    fnNotifyCbck
 *              Callback function to be registered for the specified event.
 *  @arg    param
 *              Callback argument.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EWRONGSTATE
 *              IPS not initialized.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
UEVENT_AddBufByPid (IN     Uint32       pid,
                    IN     Uint32       eventNo,
                    IN     Uint32       data,
                    IN     FnNotifyCbck fnNotifyCbck,
                    IN     Pvoid        param,
                    IN     Uint32       isRingio) ;


/** ============================================================================
 *  @func   UEVENT_AddNewProcess
 *
 *  @desc   This function adds a new process for getting event nofiicatin.
 *
 *  @arg    None.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
UEVENT_AddNewProcess (Void) ;


/** ============================================================================
 *  @func   UEVENT_RemoveUserProcess
 *
 *  @desc   This function removes user process from the registered list.
 *
 *  @arg    None.
 *
 *  @ret    None.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
UEVENT_RemoveUserProcess (Void) ;


/** ============================================================================
 *  @func   UEVENT_GetBuf
 *
 *  @desc   This function gets a buf if there any.
 *
 *  @arg    eventno
 *              Event to be notified to the DSP.
 *  @arg    payload
 *              Data to be sent with Event (Optional).
 *
 *  @ret    sizeof the got buffer.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
Uint32
UEVENT_GetBuf (IN Pvoid packet) ;

#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   NOTIFY_KnlInstrument
 *
 *  @desc   This function gives the profile instrumentation
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_KnlInstrument (IN     ProcessorId      dspId,
                      IN     Uint32           ipsId,
                      OUT    IPS_Instrument * retVal) ;
#endif /* if defined (DDSP_PROFILE) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (NOTIFY_KNL_H) */
