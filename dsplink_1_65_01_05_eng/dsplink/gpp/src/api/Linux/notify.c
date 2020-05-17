/** ============================================================================
 *  @file   notify.c
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


/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <linkcfgdefs.h>
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug                 */
#include <_trace.h>

#if defined (PROFILE_COMPONENT)
#include <profile.h>
#endif /* if defined (PROFILE_COMPONENT) */

/*  ----------------------------------- User API                      */
#include <notify.h>
#include <_notify.h>
#if defined (RINGIO_COMPONENT)
#include <ringio.h>
#include <_ringio.h>
#endif /* if defined (RINGIO_COMPONENT) */

/*  ----------------------------------- OSAL Header                   */
#include <drvdefs.h>
#include <drv_api.h>


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
 *  @desc   State object for the NOTIFY component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
NOTIFY_Object NOTIFY_state [MAX_DSPS] ;


/** ============================================================================
 *  @func   NOTIFY_register
 *
 *  @desc   This function registers a callback from an event.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_register (IN     ProcessorId   dspId,
                 IN     Uint32        ipsId,
                 IN     Uint32        eventNo,
                 IN     FnNotifyCbck  fnNotifyCbck,
                 IN     Pvoid         cbckArg)
{
    DSP_STATUS          status = DSP_SOK ;
    CMD_Args            args             ;
    LINKCFG_Dsp *       dspObj           ;
    LINKCFG_LinkDrv *   linkDrv          ;
    LINKCFG_Ips *       ipsTable         ;
#if defined (RINGIO_COMPONENT)
    LINKCFG_RingIo *    ringIOObject ;
#endif /* if defined (RINGIO_COMPONENT) */
#if defined (MSGQ_COMPONENT)
    LINKCFG_Mqt *       mqtObject ;
#endif /* if defined (MSGQ_COMPONENT) */
    LINKCFG_DspConfig * dspConfig ;

    TRC_5ENTER ("NOTIFY_register",
                dspId,
                ipsId,
                eventNo,
                fnNotifyCbck,
                cbckArg) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (fnNotifyCbck != NULL) ;


    if ((IS_VALID_PROCID (dspId) == FALSE) || (fnNotifyCbck == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        dspConfig    = PROC_linkCfgPtr->dspConfigs [dspId] ;
        dspObj       = dspConfig->dspObject ;
        linkDrv      = &(dspConfig->linkDrvObjects [dspObj->linkDrvId]) ;
        ipsTable     = &(dspConfig->ipsTables [linkDrv->ipsTableId][0]) ;

        if (ipsId >= linkDrv->numIpsEntries) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        else if (eventNo >= ipsTable [ipsId].numIpsEvents) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        else {
#if defined (RINGIO_COMPONENT)
            ringIOObject = &(dspConfig->ringIoObjects [linkDrv->ringIoTableId]) ;
            if (   (ipsId == ringIOObject->ipsId)
                && (eventNo == ringIOObject->ipsEventNo)) {
                if (fnNotifyCbck != &RingIO_notifyCallback) {
                    status = DSP_ERESOURCE ;
                    SET_FAILURE_REASON ;
                }
            }
#endif /* if defined (RINGIO_COMPONENT) */

            if (DSP_SUCCEEDED (status)) {

#if defined (MSGQ_COMPONENT)
                /* Check has been added to ensure an error is returned if user
                 * tries to use the ips id and event number reserved for MQT
                 * before MSGQ_transportOpen is called.
                 * TBD: This check should be moved to the IPS module.
                 */
                mqtObject = &(dspConfig->mqtObjects [linkDrv->mqtId]) ;
                if (   (ipsId == mqtObject->ipsId)
                    && (eventNo == mqtObject->ipsEventNo)) {
                    status = DSP_ERESOURCE ;
                    SET_FAILURE_REASON ;
                }
#endif /* if defined (MSGQ_COMPONENT) */

                if (DSP_SUCCEEDED (status)) {
                    args.apiArgs.notifyRegisterArgs.eventNo      = eventNo ;
                    args.apiArgs.notifyRegisterArgs.fnNotifyCbck = fnNotifyCbck ;
                    args.apiArgs.notifyRegisterArgs.cbckArg = cbckArg ;
                    args.apiArgs.notifyRegisterArgs.dspId   = dspId ;
                    args.apiArgs.notifyRegisterArgs.ipsId   = ipsId ;

                    status = DRV_INVOKE (DRV_handle, CMD_NOTIFY_REGISTER, &args) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
            }
        }
    }

    TRC_1LEAVE ("NOTIFY_register", status) ;

    return status ;
}


/** ============================================================================
 *  @func   NOTIFY_unregister
 *
 *  @desc   This function unregisters a callback from an event.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_unregister (IN     ProcessorId    dspId,
                   IN     Uint32         ipsId,
                   IN     Uint32         eventNo,
                   IN     FnNotifyCbck   fnNotifyCbck,
                   IN     Pvoid          cbckArg)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;
    LINKCFG_Dsp *       dspObj    ;
    LINKCFG_LinkDrv *   linkDrv   ;
    LINKCFG_Ips *       ipsTable  ;
    LINKCFG_DspConfig * dspConfig ;

    TRC_5ENTER ("NOTIFY_unregister",
                dspId,
                ipsId,
                eventNo,
                fnNotifyCbck,
                cbckArg) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (fnNotifyCbck != NULL) ;


    if ((IS_VALID_PROCID (dspId) == FALSE) || (fnNotifyCbck == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        dspConfig    = PROC_linkCfgPtr->dspConfigs [dspId] ;
        dspObj       = dspConfig->dspObject ;
        linkDrv      = &(dspConfig->linkDrvObjects [dspObj->linkDrvId]) ;
        ipsTable     = &(dspConfig->ipsTables [linkDrv->ipsTableId][0]) ;

        if (ipsId >= linkDrv->numIpsEntries) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        else if (eventNo >= ipsTable [ipsId].numIpsEvents) {
                status = DSP_EINVALIDARG ;
                SET_FAILURE_REASON ;
        }
        else {
            /* Remove the listener information into the listener list. and
             * decrease the eventHandlerCount by 1.
             */
            args.apiArgs.notifyUnregisterArgs.eventNo      = eventNo ;
            args.apiArgs.notifyUnregisterArgs.fnNotifyCbck = fnNotifyCbck;
            args.apiArgs.notifyUnregisterArgs.cbckArg      = cbckArg ;
            args.apiArgs.notifyUnregisterArgs.dspId        = dspId ;
            args.apiArgs.notifyUnregisterArgs.ipsId        = ipsId ;

            status = DRV_INVOKE (DRV_handle, CMD_NOTIFY_UNREGISTER, &args) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("NOTIFY_unregister", status) ;

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
EXPORT_API
DSP_STATUS
NOTIFY_notify (IN     ProcessorId dspId,
               IN     Uint32      ipsId,
               IN     Uint32      eventNo,
               IN     Uint32      payload)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;
    LINKCFG_Dsp *       dspObj     ;
    LINKCFG_LinkDrv *   linkDrv    ;
    LINKCFG_Ips *       ipsTable   ;
    LINKCFG_DspConfig * dspConfig ;

    TRC_4ENTER ("NOTIFY_notify", dspId, ipsId, eventNo, payload) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if ((IS_VALID_PROCID (dspId) == FALSE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        dspConfig    = PROC_linkCfgPtr->dspConfigs [dspId] ;
        dspObj       = dspConfig->dspObject ;
        linkDrv      = &(dspConfig->linkDrvObjects [dspObj->linkDrvId]) ;
        ipsTable     = &(dspConfig->ipsTables [linkDrv->ipsTableId][0]) ;

        if (ipsId >= linkDrv->numIpsEntries) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        else if (eventNo >= ipsTable [ipsId].numIpsEvents) {
                status = DSP_EINVALIDARG ;
                SET_FAILURE_REASON ;
        }
        else {
            args.apiArgs.notifyNotifyArgs.eventNo = eventNo ;
            args.apiArgs.notifyNotifyArgs.payload = payload ;
            args.apiArgs.notifyNotifyArgs.dspId   = dspId ;
            args.apiArgs.notifyNotifyArgs.ipsId   = ipsId ;

            status = DRV_INVOKE (DRV_handle, CMD_NOTIFY_NOTIFY, &args) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("NOTIFY_notify", status) ;

    return status ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   NOTIFY_instrument
 *
 *  @desc   Gets the instrumentation information related to IPS's
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_instrument (IN  ProcessorId       dspId,
                   IN  Uint32            ipsId,
                   OUT IPS_Instrument *  retVal)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;
    LINKCFG_Dsp *       dspObj           ;
    LINKCFG_LinkDrv *   linkDrv          ;
    LINKCFG_Ips *       ipsTable         ;
    LINKCFG_DspConfig * dspConfig ;

    TRC_3ENTER ("NOTIFY_instrument", dspId, ipsId, retVal) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (retVal != NULL) ;


    if (   ((IS_VALID_PROCID (dspId) == FALSE))
        || (retVal == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }


    if (DSP_SUCCEEDED (status)) {
        dspConfig    = PROC_linkCfgPtr->dspConfigs [dspId] ;
        dspObj       = &(dspConfig->dspObjects [dspId]) ;
        linkDrv      = &(dspConfig->linkDrvObjects [dspObj->linkDrvId]) ;
        ipsTable     = &(dspConfig->ipsTables [linkDrv->ipsTableId][0]) ;

        if (ipsId >= linkDrv->numIpsEntries) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        else {
            args.apiArgs.ipsInstrumentArgs.dspId    = dspId ;
            args.apiArgs.ipsInstrumentArgs.ipsId    = ipsId ;
            args.apiArgs.ipsInstrumentArgs.ipsStats = retVal ;

            status = DRV_INVOKE (DRV_handle, CMD_NOTIFY_INSTRUMENT, &args) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("NOTIFY_instrument", status) ;

    return status ;
}


#endif /* if defined (DDSP_PROFILE) */

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
