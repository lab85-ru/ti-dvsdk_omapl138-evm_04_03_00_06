/** ============================================================================
 *  @file   scale.h
 *
 *  @path   $(DSPLINK)/dsp/src/samples/scale/
 *
 *  @desc   Header file for scale application.
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


#ifndef SCALE_
#define SCALE_


/*  ----------------------------------- DSP/BIOS Headers            */
#include <msgq.h>


#ifdef __cplusplus
extern "C" {
#endif


/** ============================================================================
 *  @const  SCALE_REQUEST_MSGID
 *
 *  @desc   Id to denote a scaling change.
 *  ============================================================================
 */
#define SCALE_REQUEST_MSGID   1

/** ============================================================================
 *  @name   SCALE_ScaleMsg
 *
 *  @desc   Structure used to pass the scaling factor from the GPP to the DSP.
 *
 *  @field  msgHeader
 *              Required first field of a message.
 *  @field  scalingFactor
 *              Used to scale the output buffer values.
 *  ============================================================================
 */
typedef struct SCALE_ScaleMsg_tag {
    MSGQ_MsgHeader msgHeader;
    Uint16         scalingFactor;
} SCALE_ScaleMsg;


#ifdef __cplusplus
}
#endif /* extern "C" */


#endif /* SCALE_ */
