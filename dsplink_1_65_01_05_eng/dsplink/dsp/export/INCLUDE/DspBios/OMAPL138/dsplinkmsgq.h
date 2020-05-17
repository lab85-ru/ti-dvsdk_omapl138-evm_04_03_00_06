/** ============================================================================
 *  @file   dsplinkmsgq.h
 *
 *  @path   $(DSPLINK)/dsp/inc/
 *
 *  @desc   Defines constants, data types and structures used by
 *          MSGQ component in DSP/BIOS(TM) LINK.
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


#if !defined (DSPLINKMSGQ_)
#define DSPLINKMSGQ_


/*  ----------------------------------- DSP/BIOS Headers            */
#if defined (MSGQ_COMPONENT)
#include <msgq.h>
#endif /* #if defined (MSGQ_COMPONENT) */



#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


#if defined (MSGQ_COMPONENT)
/** ============================================================================
 *  @name   MSGQ
 *
 *  @desc   MSGQ variable defined by MSGQ module. This is temporary until
 *          this definition is exported by DSP/BIOS configuration tool.
 *  ============================================================================
 */
extern MSGQ_Config  * MSGQ ;


/** ============================================================================
 *  @name   MSGQ
 *
 *  @desc   This function initializes the  transport betweeen GPP and DSP
 *          dynamically after DSPLINK_init in a task.
 *
 *  @arg    procID
 *              ID of the processor with which the MQT transport is to be
 *              opened.
 *  @arg    transports
 *              Transport to be shared with the processor.
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
Int
MSGQ_transportOpen (IN  Uint16 procId,
                    IN  MSGQ_TransportObj *transports) ;
#endif /* if defined (MSGQ_COMPONENT) */



#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* if !defined (DSPLINKMSGQ_) */
