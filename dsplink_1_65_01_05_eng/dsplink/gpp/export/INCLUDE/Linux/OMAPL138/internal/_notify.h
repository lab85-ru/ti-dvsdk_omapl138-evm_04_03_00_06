/** ============================================================================
 *  @file   _notify.h
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Defines the interface of the Notify component.
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


#if !defined (_NOTIFY_H)
#define _NOTIFY_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   NOTIFY_Object
 *
 *  @desc   This structure defines the NOTIFY object, which contains all state
 *          information required by the NOTIFY component.
 *
 *  @field  dspId
 *              ID of the processor to which this NOTIFY object connects.
 *  @field  maxIpsEvents
 *              Array of maximum number of IPS events supported for each IPS.
 *  @field  ringIoIpsId
 *              ID of the IPS used by the RingIO component for this DSP.
 *              Defined only if RINGIO_COMPONENT is enabled.
 *  @field  ringIoIpsEventNo
 *              Event number of the IPS used by the RingIO component for this
 *              DSP. Defined only if RINGIO_COMPONENT is enabled.
 *  @field  pData
 *              platform specific data holder.
 *  ============================================================================
 */
typedef struct NOTIFY_Object_tag {
    ProcessorId          dspId ;
    Uint32               maxIpsEvents [MAX_IPS] ;
#if defined (RINGIO_COMPONENT)
    Uint32               ringIoIpsId ;
    Uint32               ringIoIpsEventNo ;
#endif /* #if defined (RINGIO_COMPONENT) */
    Uint32               pData ;
} NOTIFY_Object ;


/** ============================================================================
 *  @func   _NOTIFY_init
 *
 *  @desc   This function initializes the _NOTIFY component.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None.
 *
 *  @see    _NOTIFY_Exit ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_NOTIFY_init (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   _NOTIFY_exit
 *
 *  @desc   This function finalizes the _NOTIFY component.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EWRONGSTATE
 *              IPS not initialized.
 *
 *  @enter  dspId must be valid.
 *          NOTIFY_initializeCount must be greater than 0.
 *
 *  @leave  On success, the component must be finalized.
 *
 *  @see    _NOTIFY_Init ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_NOTIFY_exit (IN ProcessorId dspId) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (_NOTIFY_H) */
