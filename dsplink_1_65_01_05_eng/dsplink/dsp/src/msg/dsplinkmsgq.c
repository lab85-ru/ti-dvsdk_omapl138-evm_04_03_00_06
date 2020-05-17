/** ============================================================================
 *  @file   dsplinkmsgq.c
 *
 *  @path   $(DSPLINK)/dsp/src/msg/
 *
 *  @desc   Implementation of the DSPLINK Message Queue Transport manager
 *          component.
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
#include <msgq.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <dbc.h>
#include <failure.h>
#include <hal_cache.h>
#include <dsplinkmsgq.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define FILEID               FID_DSPLINKMSQ_C


/** ============================================================================
 *  @func   MSGQ_transportOpen
 *
 *  @desc   This function initializes the  transport betweeen GPP and DSP
 *          dynamically after DSPLINK_init in a task.
 *
 *  @modif  MSGQ.
 *  ============================================================================
 */
Int
MSGQ_transportOpen (IN  Uint16 procId,
                    IN  MSGQ_TransportObj *transports)
{
    Int                   status  = SYS_OK ;
    MSGQ_TransportHandle  mqtHandle;

    MSGQ->transports[procId] = *transports ;
    if (MSGQ->transports[procId].initFxn == NULL) {
        status = SYS_EINVAL ;
    }

    if (status == SYS_OK) {
        MSGQ->transports[procId].initFxn();

        mqtHandle = &(MSGQ->transports[procId]);
        if (mqtHandle->fxns == NULL) {
            status = SYS_EINVAL ;
        }

        if (status == SYS_OK) {
            /* Call the transport's open function */
            status = mqtHandle->fxns->open(mqtHandle) ;
        }
    }

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
