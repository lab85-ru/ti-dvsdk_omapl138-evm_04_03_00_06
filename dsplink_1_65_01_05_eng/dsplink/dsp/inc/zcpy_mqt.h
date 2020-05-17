/** ============================================================================
 *  @file   zcpy_mqt.h
 *
 *  @path   $(DSPLINK)/dsp/inc/
 *
 *  @desc   Defines the exported interface of the ZCPY MQT for GPP-DSP
 *          communication.
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


#ifndef ZCPY_MQT_
#define ZCPY_MQT_


/*  ----------------------------------- DSP/BIOS Headers            */
#include <msgq.h>
#include <msgq_mqt.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>


#ifdef __cplusplus
extern "C" {
#endif


/** ============================================================================
 *  @name   ZCPYMQT_Params
 *
 *  @desc   Defines the attributes for initialization of the ZCPY MQT.
 *
 *  @field  poolId
 *              Pool ID used for allocating control messages.
 *  ============================================================================
 */
typedef struct ZCPYMQT_Params_tag {
    Uint16  poolId ;
} ZCPYMQT_Params ;


/** ============================================================================
 *  @name   ZCPYMQT_FXNS
 *
 *  @desc   Declaration of the function table interface for the ZCPY MQT.
 *  ============================================================================
 */
extern MSGQ_TransportFxns ZCPYMQT_FXNS ;


/** ============================================================================
 *  @func   ZCPYMQT_init
 *
 *  @desc   Performs global initialization of the ZCPY MQT.
 *
 *  @arg    None
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
extern Void ZCPYMQT_init () ;


#ifdef __cplusplus
}
#endif /* extern "C" */


#endif /* ZCPY_MQT_ */
