/** ============================================================================
 *  @file   linklog.h
 *
 *  @path   $(DSPLINK)/dsp/src/base/gen/
 *
 *  @desc   Defines constants, data types and structures used by
 *          DSP/BIOS(TM) LINK LOG component.
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


#if !defined (LINKLOG_H)
#define LINKLOG_H


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @name   DsplinkLogEventType
 *
 *  @desc   Defines the types of event available for LOG.
 *
 *  @field  GD_MSGQ_PUT
 *              GPP->DSP MSG Transfer  - MSGQ_Put call.
 *  @field  GD_MSGQ_SND
 *              GPP->DSP MSG Transfer  - GPP sends interrupt.
 *  @field  GD_MSGQ_ISR
 *              GPP->DSP MSG Transfer  - DSP receives interrupt.
 *  @field  GD_MSGQ_QUE
 *              GPP->DSP MSG Transfer  - Message queued at DSP.
 *  @field  DG_MSGQ_PUT
 *              DSP->GPP MSG Transfer  - MSGQ_Put call.
 *  @field  DG_MSGQ_SND
 *              DSP->GPP MSG Transfer  - DSP sends interrupt.
 *  @field  DG_MSGQ_ISR
 *              DSP->GPP MSG Transfer  - GPP receives interrupt.
 *  @field  DG_MSGQ_QUE
 *              DSP->GPP MSG Transfer  - Message queued at GPP.
 *  @field  GD_CHNL_I_START
 *              GPP->DSP CHNL Transfer - Entring inside ISSUE call.
 *  @field  GD_CHNL_I_QUE
 *              GPP->DSP CHNL Transfer - Buffer is queued in internal structure
 *              on GPP.
 *  @field  GD_CHNL_I_COMPLETE
 *              GPP->DSP CHNL Transfer - ISSUE call completed.
 *  @field  GD_CHNL_XFER_START
 *              GPP->DSP CHNL Transfer - Initiating a buffer transfer by GPP.
 *  @field  GD_CHNL_XFER_PROCESSING
 *              GPP->DSP CHNL Transfer - Actual transfer of buffer is going to
 *              take place.
 *  @field  GD_CHNL_XFER_COMPLETE
 *              GPP->DSP CHNL Transfer - Buffer transfer is complete.
 *  @field  GD_CHNL_R_START
 *              GPP->DSP CHNL Transfer - Entring RECLAIM call.
 *  @field  GD_CHNL_R_PEND
 *              GPP->DSP CHNL Transfer - Wait on a semaphore.
 *  @field  GD_CHNL_R_POST
 *              GPP->DSP CHNL Transfer - posting the Semaphore.
 *  @field  GD_CHNL_R_COMPLETE
 *              GPP->DSP CHNL Transfer - RECLAIM call completed.
 *  @field  DG_CHNL_I_QUE
 *              DSP->GPP CHNL Transfer - Buffer is queued in internal structure
 *              on DSP.
 *  @field  DG_CHNL_XFER_START
 *              DSP->GPP CHNL Transfer - Initiating a buffer transfer by DSP.
 *  @field  DG_CHNL_XFER_PROCESSING
 *              DSP->GPP CHNL Transfer - Actual transfer of buffer is going to
 *              take place.
 *  @field  DG_CHNL_XFER_COMPLETE
 *              DSP->GPP CHNL Transfer - Buffer transfer is complete.
 *  @field  DG_CHNL_R_PEND
 *              DSP->GPP CHNL Transfer - Wait on a semaphore.
 *  @field  DG_CHNL_R_POST
 *              DSP->GPP CHNL Transfer - posting the Semaphore.
 *  ============================================================================
 */
typedef enum DsplinkLogEventType_tag {
    GD_MSGQ_PUT             = 0,
    GD_MSGQ_SND             = 1,
    GD_MSGQ_ISR             = 2,
    GD_MSGQ_QUE             = 3,
    DG_MSGQ_PUT             = 4,
    DG_MSGQ_SND             = 5,
    DG_MSGQ_ISR             = 6,
    DG_MSGQ_QUE             = 7,
    GD_CHNL_I_START         = 8,
    GD_CHNL_I_QUE           = 9,
    GD_CHNL_I_COMPLETE      = 10,
    GD_CHNL_XFER_START      = 11,
    GD_CHNL_XFER_PROCESSING = 12,
    GD_CHNL_XFER_COMPLETE   = 13,
    GD_CHNL_R_START         = 14,
    GD_CHNL_R_PEND          = 15,
    GD_CHNL_R_POST          = 16,
    GD_CHNL_R_COMPLETE      = 17,
    DG_CHNL_I_QUE           = 18,
    DG_CHNL_XFER_START      = 19,
    DG_CHNL_XFER_PROCESSING = 20,
    DG_CHNL_XFER_COMPLETE   = 21,
    DG_CHNL_R_PEND          = 22,
    DG_CHNL_R_POST          = 23
} DsplinkLogEventType ;


/** ============================================================================
 *  @func   DSPLINKLOG_logEvent
 *
 *  @desc   Logs a event
 *
 *  @arg    eventType
 *              Event type.
 *  @arg    id
 *              msg Id used to differentiate SoCrates' msgs.
 *  @arg    queId
 *              queue Id.
 *  @arg    addr
 *              Address of buffer.
 *  @arg    size
 *              size of the buffer.
 *  @arg    arg1
 *              Argument1.
 *  @arg    arg2
 *              Argument2.
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
Void
DSPLINKLOG_logEvent (DsplinkLogEventType eventType,
                     Uint32 id,
                     Uint32 queId,
                     Uint32 addr,
                     Uint32 size,
                     Uint32 arg1,
                     Uint32 arg2) ;


#if defined (__cplusplus)
}
#endif /* extern "C" */


#endif /* LINKLOG_H */
