/** ============================================================================
 *  @file   dsplinkips.h
 *
 *  @path   $(DSPLINK)/dsp/src/base/ips/
 *
 *  @desc   Defines the interface of the DSPLINK Inter Processor Signaling
 *          manager component.
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


#if !defined (DSPLINKIPS_H)
#define DSPLINKIPS_H


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   DSPLINKIPS_Config
 *
 *  @desc   This structure defines the configuration information shared between
 *          the IPS instances on the GPP and DSP-sides.
 *
 *  @field  numIpsEvents
 *              Number of events supported by the IPS.
 *  @field  gppIntId
 *              Interrupt Number to used by the IPS on GPP-side.
 *  @field  dspIntId
 *              Interrupt Number to used by the IPS on DSP-side.
 *  @field  dspIntVectorId
 *              Interrupt Vector Number to used by the IPS on DSP-side.
 *  @field  arg1
 *              First IPS-specific optional argument.
 *  @field  arg2
 *              Second IPS-specific optional argument.
 *  ============================================================================
 */
typedef struct DSPLINKIPS_Config_tag {
    Uint32       numIpsEvents ;
    Uint32       gppIntId ;
    Uint32       dspIntId ;
    Uint32       dspIntVectorId ;
    Uint32       arg1 ;
    Uint32       arg2 ;
} DSPLINKIPS_Config ;

/** ============================================================================
 *  @name   DSPLINKIPS_Ctrl
 *
 *  @desc   This structure defines the control structure required by the
 *          DSPLINK IPS component for each IPS instance
 *
 *  @field  dspAddr
 *              DSP address of the shared control structure for the IPS
 *              instance.
 *  @field  config
 *              Configuration structure for the IPS instance.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct DSPLINKIPS_Ctrl_tag {
    Uint32             dspAddr ;
    DSPLINKIPS_Config  config ;
    Uint16             padding [DSPLINKIPS_CTRL_PADDING] ;
} DSPLINKIPS_Ctrl ;


/** ============================================================================
 *  @func   DSPLINKIPS_init
 *
 *  @desc   This function initializes the DSPLINKIPS component.
 *
 *  @arg    procId
 *              Processor Identifier with which the IPS instances are shared.
 *  @arg    numIps
 *              Number of IPS instances configured.
 *  @arg    shAddr
 *              Address in shared memory for the IPS control structure.
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
DSPLINKIPS_init (IN Uint16 procId, IN Uint32 numIps, IN Uint32 shAddr) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* if !defined (DSPLINKIPS_H) */
