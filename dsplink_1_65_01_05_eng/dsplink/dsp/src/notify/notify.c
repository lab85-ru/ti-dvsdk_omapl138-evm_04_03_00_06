/** ============================================================================
 *  @file   notify.c
 *
 *  @path   $(DSPLINK)/dsp/src/notify/
 *
 *  @desc   Implements the interface of notify component.
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
#include <hwi.h>
#include <que.h>
#include <gbl.h>
#include <sys.h>
#include <swi.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <_dsplink.h>
#include <failure.h>
#include <dbc.h>
#include <ips.h>
#include <_notify.h>
#include <notify.h>
#include <dsplinkips.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define FILEID               FID_NOTIFY_C

/** ============================================================================
 *  @name   NOTIFY_ctrlPtr
 *
 *  @desc   Declaration of the pointer to the shared memory control structure
 *          for the NOTIFY component.
 *  ============================================================================
 */
extern DSPLINKIPS_Ctrl *  NOTIFY_ctrlPtr ;

/** ============================================================================
 *  @name   NOTIFY_numIps
 *
 *  @desc   Declaration of base address for the DSPLINK shared memory.
 *  ============================================================================
 */
extern Uint32 DSPLINK_shmBaseAddress ;


/** ============================================================================
 *  @func   NOTIFY_register
 *
 *  @desc   This function registers a callback from an event.
 *
 *  @modif  NOTIFY_eventLists
 *  ============================================================================
 */
Int
NOTIFY_register (IN  Uint32       procId,
                 IN  Uint32       ipsId,
                 IN  Uint32       eventNo,
                 IN  FnNotifyCbck fnNotifyCbck,
                 IN  Ptr          cbckArg)
{
    Int status               = SYS_OK ;
    DRV_Ctrl   * drvCtrlPtr  = (DRV_Ctrl *) DSPLINK_shmBaseAddress ;

    DBC_require (eventNo < NOTIFY_ctrlPtr [ipsId].config.numIpsEvents) ;
    DBC_require (fnNotifyCbck != NULL) ;

    (void) cbckArg ;

    if (ipsId >= drvCtrlPtr->config.numIpsEntries) {
        status = SYS_EINVAL;
    }
    else if (eventNo >= NOTIFY_ctrlPtr [ipsId].config.numIpsEvents) {
        status = SYS_EINVAL;
        SET_FAILURE_REASON (status) ;
    }
    else  if (fnNotifyCbck != NULL) {
        status = IPS_register (procId,
                               ipsId,
                               eventNo,
                               (FnIpsCbck) fnNotifyCbck,
                               cbckArg);
    }
    else {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }

    return status ;
}


/** ============================================================================
 *  @func   NOTIFY_unregister
 *
 *  @desc   This function unregisters a callback from an event.
 *
 *  @modif  NOTIFY_eventLists
 *  ============================================================================
 */
Int
NOTIFY_unregister (IN  Uint32       procId,
                   IN  Uint32       ipsId,
                   IN  Uint32       eventNo,
                   IN  FnNotifyCbck fnNotifyCbck,
                   IN  Void *       cbckArg)
{
    Int                    status   = SYS_OK ;
    DRV_Ctrl          * drvCtrlPtr  = (DRV_Ctrl *) DSPLINK_shmBaseAddress ;

    DBC_require (ipsId < drvCtrlPtr->config.numIpsEntries) ;
    DBC_require (eventNo < NOTIFY_ctrlPtr [ipsId].config.numIpsEvents) ;
    DBC_require (fnNotifyCbck != NULL) ;
    DBC_require (cbckArg      != NULL) ;


    if (ipsId >= drvCtrlPtr->config.numIpsEntries) {
        status = SYS_EINVAL;
        SET_FAILURE_REASON (status) ;
    }
    else if (eventNo >= NOTIFY_ctrlPtr [ipsId].config.numIpsEvents) {
        status = SYS_EINVAL;
        SET_FAILURE_REASON (status) ;
    }
    else {
        status = IPS_unregister (procId,
                             ipsId,
                             eventNo,
                             (FnIpsCbck) fnNotifyCbck,
                             cbckArg) ;

    }

    return status ;
}


/** ============================================================================
 *  @func   NOTIFY_notify
 *
 *  @desc   This function notifys the other side about an event.
 *
 *  @modif  ipsEventChart
 *  ============================================================================
 */
Int
NOTIFY_notify (IN  Uint32 procId,
               IN  Uint32 ipsId,
               IN  Uint32 eventNo,
               IN  Uint32 payload)
{
    Int status               = SYS_OK ;
    DRV_Ctrl   * drvCtrlPtr  = (DRV_Ctrl *) DSPLINK_shmBaseAddress ;

    DBC_require (ipsId < drvCtrlPtr->config.numIpsEntries) ;

    if (ipsId >= drvCtrlPtr->config.numIpsEntries) {
        status = SYS_EINVAL;
    }
    else if (eventNo >= NOTIFY_ctrlPtr [ipsId].config.numIpsEvents) {
        status = SYS_EINVAL;
        SET_FAILURE_REASON (status) ;
    }
    else {
        status = IPS_notify (procId, ipsId, eventNo, payload, TRUE) ;
    }

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

