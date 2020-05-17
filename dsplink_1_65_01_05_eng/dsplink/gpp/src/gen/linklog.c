/** ============================================================================
 *  @file   linklog.c
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   This file implements the functions of LOG subcomponent.
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


/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#if defined (MSGQ_COMPONENT)
#include <msgqdefs.h>
#endif /* if defined (MSGQ_COMPONENT) */

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Generated LOG CFG header      */
#if defined (LOG_COMPONENT)
#include <log.h>
#include <linklog.h>
#endif /* if defined (LOG_COMPONENT) */

#if defined (__cplusplus)
extern "C" {
#endif

#if defined (LOG_COMPONENT)
/** ============================================================================
 *  @name   TRACE_OBJECT
 *
 *  @desc   External reference to trace object defined by application.
 *  ============================================================================
 */
extern LOG_Handle TRACE_OBJECT ;


/** ============================================================================
 *  @func   DSPLINKLOG_LogEvent
 *
 *  @desc   Logs a event
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
DSPLINKLOG_LogEvent (IN DsplinkLogEventType eventType,
                     IN Uint32 id,
                     IN Uint32 queId,
                     IN Uint32 addr,
                     IN Uint32 size,
                     IN Uint32 arg1,
                     IN Uint32 arg2)
{
    Uint32 argQueId ;
    Uint32 argAddr  ;

    TRC_5ENTER ("DSPLINKLOG_LogEvent",
                eventType,
                id,
                queId,
                addr,
                size) ;

    /* TBD comment for time being
     * DBC_Require (TRACE_OBJECT != NULL) ;
     */

    /*
     * Format for logging (in bits)
     * |<- que ID (32) ->|<- Address (32) ->|<- event (16) ->|<- size (16) ->|
     */
    if (!(   ((id >= ID_START) && (id <= ID_END))
#if defined (MSGQ_COMPONENT)
         || ((id >= MSGQ_INTERNALIDSSTART) && (id <= MSGQ_INTERNALIDSEND))
         || ((id >= MSGQ_MQTMSGIDSSTART)   && (id <= MSGQ_MQTMSGIDSEND))
#endif /* if defined (MSGQ_COMPONENT) */
         )) {
        argQueId      = (Uint32) queId ;
        argAddr       = (Uint32) addr ;

        (Void) arg1 ;
        (Void) arg2 ;

        LOG_event (TRACE_OBJECT,
                   argQueId,
                   argAddr,
                   (Uint32) eventType,
                   (Uint32) id,
                   (Uint32) size) ;
    }

    TRC_0LEAVE ("DSPLINKLOG_LogEvent") ;
}
#endif /* if defined (LOG_COMPONENT) */


#if defined (__cplusplus)
}
#endif /* extern "C" */
