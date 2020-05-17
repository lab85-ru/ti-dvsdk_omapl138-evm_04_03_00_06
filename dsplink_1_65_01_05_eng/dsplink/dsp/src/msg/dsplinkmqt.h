/** ============================================================================
 *  @file   dsplinkmqt.h
 *
 *  @path   $(DSPLINK)/dsp/src/msg/
 *
 *  @desc   Defines the interface of the DSPLINK Message Queue Transport manager
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


#if !defined (DSPLINKMQT_H)
#define DSPLINKMQT_H


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   DSPLINKMQT_Config
 *
 *  @desc   This structure defines the configuration information shared between
 *          the MQT instance on the GPP and DSP-sides.
 *
 *  @field  maxMsgSize
 *              Maximum size of message supported by MQT. May be -1 if there is
 *              no limit on maximum message size for the MQT.
 *  @field  ipsId
 *              ID of the IPS to be used (if any). A value of -1 indicates that
 *              no IPS is required by the MQT.
 *  @field  ipsEventNo
 *              IPS Event number associated with MQT (if any). A value of -1
 *              indicates that no IPS is required by the MQT.
 *  @field  arg1
 *              First optional argument for this MQT.
 *              The significance of this argument is specific to the MQT.
 *  @field  arg2
 *              Second optional argument for this MQT.
 *              The significance of this argument is specific to the MQT.
 *  ============================================================================
 */
typedef struct DSPLINKMQT_Config_tag {
    Uint32             maxMsgSize ;
    Uint32             ipsId ;
    Uint32             ipsEventNo ;
    Uint32             arg1 ;
    Uint32             arg2 ;
} DSPLINKMQT_Config ;

/** ============================================================================
 *  @name   DSPLINKMQT_Ctrl
 *
 *  @desc   This structure defines the control structure required by the
 *          DSPLINK MQT instance.
 *
 *  @field  dspAddr
 *              DSP address of the shared control structure for the MQT
 *              instance.
 *  @field  config
 *              Configuration structure for the MQT instance.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct DSPLINKMQT_Ctrl_tag {
    Uint32             dspAddr ;
    DSPLINKMQT_Config  config ;
    Uint16             padding [DSPLINKMQT_CTRL_PADDING] ;
} DSPLINKMQT_Ctrl ;


/** ============================================================================
 *  @func   DSPLINKMQT_init
 *
 *  @desc   This function initializes the DSPLINKMQT component.
 *
 *  @arg    procId
 *              Processor Identifier with which the MQT communicates.
 *  @arg    shAddr
 *              Address in shared memory for the MQT control structure.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid argument.
 *          SYS_EALLOC
 *              Memory allocation failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
Int
DSPLINKMQT_init (IN Uint16 procId, IN Uint32 shAddr) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* if !defined (DSPLINKMQT_H) */
