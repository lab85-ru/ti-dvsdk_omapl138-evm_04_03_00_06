/** ============================================================================
 *  @file   _zcpy_mqt.h
 *
 *  @path   $(DSPLINK)/dsp/src/msg/DspBios/
 *
 *  @desc   Internal declarations for the ZCPY MQT for GPP-DSP communication.
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


#if !defined (_ZCPY_MQT_H)
#define _ZCPY_MQT_H


/*  ----------------------------------- DSP/BIOS Headers            */
#include <que.h>
#include <swi.h>
#include <msgq.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <mpcsdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  ZCPYMQT_CTRLCMD_LOCATE
 *
 *  @desc   Defines the control command msg ID for location of a remote MSGQ.
 *  ============================================================================
 */
#define ZCPYMQT_CTRLCMD_LOCATE    MSGQ_MQTMSGIDSSTART

/** ============================================================================
 *  @const  ZCPYMQT_CTRLCMD_LOCATEACK
 *
 *  @desc   Defines the control command msg ID for acknowledgement for location
 *          of a remote MSGQ.
 *  ============================================================================
 */
#define ZCPYMQT_CTRLCMD_LOCATEACK (MSGQ_MQTMSGIDSSTART + 1)

/** ============================================================================
 *  @const  ZCPYMQT_QUE_PADDING
 *
 *  @desc   Padding required for DSP L2 cache line alignment within ZCPYMQT
 *          control objects for aligning a LIST element.
 *  ============================================================================
 */
#define ZCPYMQT_QUE_PADDING  ((CACHE_L2_LINESIZE - sizeof (QUE_Elem)) / 2)


/** ============================================================================
 *  @name   ZCPYMQT_Ctrl
 *
 *  @desc   This structure defines the shared control structure required by the
 *          ZCPYMQT component.
 *
 *  @field  isOpen
 *              Value indicating whether GPP has opened its transport or not.
 *  @field  padding
 *              Padding field for isOpen member.
 *  @field  toDspList
 *              Holds the list of buffers to be sent to the DSP.
 *  @field  toDspPadding
 *              Padding for alignment.
 *  @field  fmDspList
 *              Holds the list of buffers to be received from the DSP.
 *  @field  fmDspPadding
 *              Padding for alignment.
 *  @field  csToDspList
 *              Shared critical section object for protection of operations by
 *              the two processors on the toDspList.
 *  @field  csFmDspList
 *              Shared critical section object for protection of operations by
 *              the two processors on the fmDspList.
 *  ============================================================================
 */
typedef struct ZCPYMQT_Ctrl_tag {
    volatile Uint16      isOpen    ;
    ADD_PADDING (padding, DSPLINK_16BIT_PADDING)

    volatile QUE_Elem    toDspList ;
             Uint16      toDspPadding [ZCPYMQT_QUE_PADDING] ;
    volatile QUE_Elem    fmDspList ;
             Uint16      fmDspPadding [ZCPYMQT_QUE_PADDING] ;
    volatile MPCS_ShObj  csToDspList ;
    volatile MPCS_ShObj  csFmDspList ;
} ZCPYMQT_Ctrl ;

/** ============================================================================
 *  @name   ZCPYMQT_CtrlMsg
 *
 *  @desc   Defines the format of the control messages that are sent between the
 *          ZCPY MQTs on different processors.
 *          This structure is common between the GPP and the DSP, and must be
 *          maintained as the same on both. To ensure this, padding must be
 *          added as required within the structure.
 *
 *  @field  msgHeader
 *              Fixed message header required for all messages.
 *  @field  ctrlMsg
 *              Defines the format of the different control messages.
 *              locateMsg:    semHandle   -> Semaphore handle for sync locate
 *                            replyQueue  -> Reply MSGQ handle for async locate
 *                            arg         -> User-defined value passed to locate
 *                            poolId      -> Pool ID to alloc async response msg
 *                            padding     -> Padding for alignment.
 *                            msgqName    -> Name of the MSGQ to be located on
 *                                           the remote processor.
 *              locateAckMsg: semHandle   -> Semaphore handle for sync locate
 *                            replyQueue  -> Reply MSGQ handle for async locate
 *                            arg         -> User-defined value passed to locate
 *                            poolId      -> Pool ID to alloc async response msg
 *                            padding     -> Padding for alignment.
 *                            msgqQueue   -> Handle to the MSGQ located on the
 *                                           remote processor.
 *  ============================================================================
 */
typedef struct ZCPYMQT_CtrlMsg_tag {
    MSGQ_MsgHeader msgHeader ;
    union {
        struct {
            Uint32      semHandle ;
            Uint32      replyQueue ;
            Uint32      arg ;
            Uint16      poolId ;
            Uint16      padding ;
            Uint16      msgqName [DSP_MAX_STRLEN] ;
        } locateMsg ;

        struct {
            Uint32      semHandle ;
            Uint32      replyQueue ;
            Uint32      arg ;
            Uint16      poolId ;
            Uint16      padding ;
            Uint32      msgqQueue ;
        } locateAckMsg ;
    } ctrlMsg ;
} ZCPYMQT_CtrlMsg ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (_ZCPY_MQT_H) */
