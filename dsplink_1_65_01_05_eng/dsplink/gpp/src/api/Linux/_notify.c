/** ============================================================================
 *  @file   _notify.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/Linux/
 *
 *  @desc   Implemetation of NOTIFY.
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


/*  ----------------------------------- OS Specific Headers         */
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <linkcfgdefs.h>
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug                 */
#include <_trace.h>

/*  ----------------------------------- OSAL Header                   */
#include <drvdefs.h>
#include <drv_api.h>

/*  ----------------------------------- User API                      */
#include <notify.h>
#include <_notify.h>
#if defined (RINGIO_COMPONENT)
#include <ringio.h>
#include <_ringio.h>
#endif /* if defined (RINGIO_COMPONENT) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   COMPONENT_ID
 *
 *  @desc   Component ID of this module. That will be used by TRC macros.
 *  ============================================================================
 */
#define COMPONENT_ID        ID_API_NOTIFY

/** ============================================================================
 *  @name   SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_NOTIFY, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @macro  LINK_DRIVER
 *
 *  @desc   Name of the dsplink driver.
 *  ============================================================================
 */
#define LINK_DRIVER         "/dev/dsplink"


/** ============================================================================
 *  @name   EVENT_Packet
 *
 *  @desc   Structure of Event Packet read from notify kernel-side.
 *
 *  @field  reserved
 *              Reserved.
 *  @field  eventNo
 *              Event Number.
 *  @field  data
 *              Data associated with event.
 *  @field  func
 *              User callback function.
 *  @field  param
 *              User callback argument.
 *  @field  isExit
 *              Tells whether to terminate notify thread.
 *  ============================================================================
 */
typedef struct EVENT_Packet_tag {
    Uint32       reserved [2] ;
    Uint32       eventNo      ;
    Uint32       data         ;
    FnNotifyCbck func         ;
    Pvoid        param        ;
#if defined(RINGIO_COMPONENT)
    Uint32       isRingio ;
#endif /* if defined(RINGIO_COMPONENT) */
} EVENT_Packet ;


/** ============================================================================
 *  @name   PROC_linkCfgPtr
 *
 *  @desc   Global pointer containing DSPLINK configuration information for the
 *          driver on the user-side.
 *  ============================================================================
 */
extern LINKCFG_Object * PROC_linkCfgPtr ;

/** ----------------------------------------------------------------------------
 *  @name   NOTIFY_state
 *
 *  @desc   Extern deckaration of the state object for the NOTIFY component
 *          containing all information required by it.
 *  ----------------------------------------------------------------------------
 */
extern NOTIFY_Object NOTIFY_state [MAX_DSPS] ;

/** ============================================================================
 *  @name   DSPLINK_isSignal
 *
 *  @desc   Global flag to indicate whether execution is in signal context.
 *          Driver on the user-side.
 *  ============================================================================
 */
extern Bool DSPLINK_isSignal ;



/** ----------------------------------------------------------------------------
 *  @func   NOTIFY_eventWorker
 *
 *  @desc   This is the worker thread for polling on events.
 *
 *  @arg    arg
 *              Don't care.
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
STATIC
NORMAL_API
void
NOTIFY_eventWorker (IN     void * arg) ;


/** ============================================================================
 *  @func   _NOTIFY_init
 *
 *  @desc   This function initializes the NOTIFY component.
 *
 *  @modif  none
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_NOTIFY_init (IN ProcessorId dspId)
{
    DSP_STATUS          status = DSP_SOK ;
    CMD_Args            args ;
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_LinkDrv *   linkDrv ;
    LINKCFG_Ips *       ipsObj ;
    Uint32              i ;
#if defined (RINGIO_COMPONENT)
    LINKCFG_RingIo *    ringIoObj ;
#endif /* #if defined (RINGIO_COMPONENT) */
    LINKCFG_DspConfig * dspConfig ;

    TRC_1ENTER ("_NOTIFY_init", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    dspConfig    = PROC_linkCfgPtr->dspConfigs [dspId] ;
    dspObj       = dspConfig->dspObject ;
    linkDrv   = &(dspConfig->linkDrvObjects [dspObj->linkDrvId]) ;

    NOTIFY_state [dspId].dspId = dspId ;

    for (i = 0 ; i < linkDrv->numIpsEntries ; i++) {
        ipsObj = &(dspConfig->ipsTables [linkDrv->ipsTableId][i]) ;
        NOTIFY_state [dspId].maxIpsEvents [i] = ipsObj->numIpsEvents ;
    }
#if defined (RINGIO_COMPONENT)
    ringIoObj = &(dspConfig->ringIoObjects [linkDrv->ringIoTableId]) ;
    NOTIFY_state [dspId].ringIoIpsId      = ringIoObj->ipsId ;
    NOTIFY_state [dspId].ringIoIpsEventNo = ringIoObj->ipsEventNo ;
#endif /* #if defined (RINGIO_COMPONENT) */

    args.apiArgs.notifyInitializeArgs.dspId = dspId ;

    status = DRV_INVOKE (DRV_handle, CMD_NOTIFY_INITIALIZE, &args) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }
    else {

        /* Create the signal worker thread, assuming PTHREAD */
        pthread_create ((pthread_t *) &NOTIFY_state [dspId].pData,
                NULL,
                (Pvoid) NOTIFY_eventWorker,
                (Pvoid) DRV_handle) ;
        if (NOTIFY_state [dspId].pData == (Uint32) NULL) {
            status = DSP_EFAIL ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("_NOTIFY_init", status) ;

    return status ;
}

/** ============================================================================
 *  @func   _NOTIFY_exit
 *
 *  @desc   This function finalizes the NOTIFY component.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_NOTIFY_exit (IN ProcessorId dspId)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_1ENTER ("_NOTIFY_exit", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    args.apiArgs.notifyFinalizeArgs.dspId = dspId ;

    status = DRV_INVOKE (DRV_handle, CMD_NOTIFY_FINALIZE, &args) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    if (NOTIFY_state [dspId].pData != (Uint32) NULL) {
        if (DSPLINK_isSignal == FALSE) {
            pthread_join ((pthread_t) NOTIFY_state [dspId].pData, NULL) ;
        }
    }

    TRC_1LEAVE ("_NOTIFY_exit", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   NOTIFY_eventWorker
 *
 *  @desc   This is the worker thread which polls for events.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
void
NOTIFY_eventWorker (IN     void * arg)
{
    DSP_STATUS      status = DSP_SOK ;
    Uint32          nRead  = 0 ;
    EVENT_Packet    packet ;
    Int32           drvHandle ;
    sigset_t        blockSet ;

    if (sigfillset (&blockSet) != 0) {
        perror ("Event worker thread error in sigfillset") ;
        return;
    }

    if (pthread_sigmask (SIG_BLOCK, &blockSet, NULL) != 0) {
        perror ("Event worker thread error in setting sigmask") ;
        return;
    }

    drvHandle = open (LINK_DRIVER, O_RDWR | O_SYNC) ;
    if (drvHandle < 0) {
        perror ("Event worker thread") ;
        return;
    }

    while (DSP_SUCCEEDED (status)) {
        memset (&packet, 0, sizeof (EVENT_Packet)) ;
        nRead = read (drvHandle, &packet, sizeof (EVENT_Packet)) ;
        if (nRead == sizeof (EVENT_Packet)) {
            /* check for termination packet */
            if (packet.eventNo  == (Uint32) -1) {
                close (drvHandle) ;
                return;
            }
#if defined (RINGIO_COMPONENT)
            if (packet.isRingio == 1) {
                RingIO_notifyCallback (NOTIFY_state [0].ringIoIpsEventNo,
                                       packet.param,
                                       (Pvoid)packet.data) ;
            }
            else {
#endif
                if (packet.func != NULL) {
                    packet.func (packet.eventNo,
                                 packet.param,
                                 (Pvoid)packet.data) ;
                }
#if defined (RINGIO_COMPONENT)
            }
#endif
        }
    }
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
