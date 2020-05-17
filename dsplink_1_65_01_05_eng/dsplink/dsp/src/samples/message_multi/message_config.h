/** ============================================================================
 *  @file   message_config.h
 *
 *  @path   $(DSPLINK)/dsp/src/samples/message_multi/
 *
 *  @desc   Header file containing configuration for the message_multi sample.
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


#if !defined (MESSAGE_CONFIG_)
#define MESSAGE_CONFIG_


/*  ----------------------------------- DSP/BIOS Headers            */
#include <msgq.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  MAX_APPS
 *
 *  @desc   Maximum number of application instances that can be used.
 *  ============================================================================
 */
#define MAX_APPS            16

/** ============================================================================
 *  @const  NUM_POOLS
 *
 *  @desc   Number of MESSAGE_Pools configured in the system.
 *  ============================================================================
 */
#define NUM_POOLS          (MAX_APPS + 1)

/** ============================================================================
 *  @const  MQT_POOL_ID
 *
 *  @desc   Pool ID of the common pool to be used for MSGQ transport.
 *  ============================================================================
 */
#define MQT_POOL_ID         0

/** ============================================================================
 *  @const  MESSAGE_IPS_ID
 *
 *  @desc   IPS ID to be used with NOTIFY module.
 *  ============================================================================
 */
#define MESSAGE_IPS_ID      0

/** ============================================================================
 *  @const  MESSAGE_IPS_EVENTNO_BASE
 *
 *  @desc   Base IPS event number to be used witin the MESSAGE_IPS_ID.
 *  ============================================================================
 */
#define MESSAGE_IPS_EVENTNO_BASE 5

/** ============================================================================
 *  @const  APP_BUFFER_SIZE
 *
 *  @desc   Messaging buffer used by the application.
 *  ============================================================================
 */
#define APP_BUFFER_SIZE  (DSPLINK_ALIGN (sizeof (MSGQ_MsgHeader),  \
                                         DSPLINK_BUF_ALIGN))


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MESSAGE_CONFIG_) */
