/** ============================================================================
 *  @file   linklog.c
 *
 *  @path   $(DSPLINK)/dsp/src/base/gen/
 *
 *  @desc   This file implements the functions of LOG subcomponent.
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


/*  ----------------------------------- Generated LOG CFG header      */
#include <log.h>
#include <linklog.h>

#if defined (MSGQ_COMPONENT)
#include <msgq.h>
#endif


#if defined (__cplusplus)
extern "C" {
#endif

#if defined (LOG_COMPONENT)
/** ============================================================================
 *  @name   TRACE_OBJECT
 *
 *  @desc   Handle to global TRACE_OBJECT.
 *  ============================================================================
 */
extern LOG_Obj TRACE_OBJECT ;


/** ============================================================================
 *  @func   DSPLINKLOG_logEvent
 *
 *  @desc   Logs a event.
 *
 *  @modif  None
 *  ============================================================================
 */
Void
DSPLINKLOG_logEvent (DsplinkLogEventType eventType,
                     Uint32 id,
                     Uint32 queId,
                     Uint32 addr,
                     Uint32 size,
                     Uint32 arg1,
                     Uint32 arg2)
{
    Arg argQueId ;
    Arg argAddr  ;
    Arg argSizenEvent ;

    /*
     * Format for logging (in bits)
     * |<- que ID (32) ->|<- Address (32) ->|<- event (16) ->|<- size (16) ->|
     */
    if (!(   ((id >= ID_START) && (id <= ID_END))
         || ((id >= MSGQ_INTERNALIDSSTART) && (id <= MSGQ_INTERNALIDSEND))
         || ((id >= MSGQ_MQTMSGIDSSTART)   && (id <= MSGQ_MQTMSGIDSEND)))) {
        argQueId      = (Arg) queId ;
        argAddr       = (Arg) addr ;
        argSizenEvent = (Arg) (  ((eventType << 16) & 0xFFFF0000)
                           | (size & 0xFFFF)) ;
        (Void) arg1 ;
        (Void) arg2 ;

        LOG_event (&TRACE_OBJECT,
                   argQueId,
                   argAddr,
                   argSizenEvent) ;
    }
}
#endif /* if defined (LOG_COMPONENT) */


#if defined (__cplusplus)
}
#endif /* extern "C" */
