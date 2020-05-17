/** ============================================================================
 *  @file   message_config.c
 *
 *  @path   $(DSPLINK)/dsp/src/samples/message_multi/
 *
 *  @desc   Source file containing configuration for the message_multi sample.
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
#include <pool.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>

#if defined (MSGQ_ZCPY_LINK)
#include <zcpy_mqt.h>
#if ( PHYINTERFACE == PCI_INTERFACE) ||( PHYINTERFACE == VLYNQ_INTERFACE)
#include <dma_pool.h>
#else
#include <sma_pool.h>
#endif /* if (PHYINTERFACE == PCI_INTERFACE) */
#endif /* if defined (MSGQ_ZCPY_LINK) */

/*  ----------------------------------- Sample Headers              */
#include <message_config.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  NUM_MSG_QUEUES
 *
 *  @desc   Number of local message queues
 *  ============================================================================
 */
#define NUM_MSG_QUEUES     MAX_APPS


/** ============================================================================
 *  Definitions required for the sample that vary based on physical link
 *  being used for the MQT.
 *  ============================================================================
 */
/** ============================================================================
 *  @name   MESSAGE_MqtParams
 *
 *  @desc   MQT parameters
 *  ============================================================================
 */
ZCPYMQT_Params  MESSAGE_MqtParams = {MQT_POOL_ID} ;

/** ============================================================================
 *  @name   MESSAGE_MsgQueues
 *
 *  @desc   Array of local message queues
 *  ============================================================================
 */
static MSGQ_Obj MESSAGE_MsgQueues [NUM_MSG_QUEUES] ;


/** ============================================================================
 *  @name   MESSAGE_Transports
 *
 *  @desc   Array of transports.
 *  ============================================================================
 */
MSGQ_TransportObj MESSAGE_Transports [MAX_PROCESSORS] =
{
#if (MAX_PROCESSORS == 2) /* For One DSP  and one GPP processor */
     MSGQ_NOTRANSPORT,    /* Represents the local processor */
#elif (MAX_PROCESSORS == 3 )/* For two DSP and one GPP processor */
     MSGQ_NOTRANSPORT,
     MSGQ_NOTRANSPORT,
#else
#error ****ERROR:MESSAGE_Transports needs to be configured for MAX_PROCESSORS
#endif
#if defined (DSP_BOOTMODE_NOBOOT)
     MSGQ_NOTRANSPORT,    /* Represents the local processor */
#else
     {
        &ZCPYMQT_init,      /* Init Function                 */
        &ZCPYMQT_FXNS,      /* Transport interface functions */
        &MESSAGE_MqtParams, /* Transport params              */
        NULL,               /* Filled in by transport        */
        ID_GPP              /* Processor Id                  */
     }
#endif
} ;

/** ============================================================================
 *  @name   MSGQ_config
 *
 *  @desc   MSGQ configuration information.
 *          MSGQ_config is a required global variable.
 *  ============================================================================
 */
MSGQ_Config MSGQ_config =
{
    MESSAGE_MsgQueues,
    MESSAGE_Transports,
    NUM_MSG_QUEUES,
    MAX_PROCESSORS,
    0,
    MSGQ_INVALIDMSGQ,
    POOL_INVALIDID
} ;


/** ============================================================================
 *  @name   MESSAGE_PoolParams
 *
 *  @desc   Pool parameters for the array of pools
 *  ============================================================================
 */
#if ( PHYINTERFACE == PCI_INTERFACE) ||( PHYINTERFACE == VLYNQ_INTERFACE)
DMAPOOL_Params MESSAGE_PoolParams [NUM_POOLS] =
#else
SMAPOOL_Params MESSAGE_PoolParams [NUM_POOLS] =
#endif /* if (PHYINTERFACE == PCI_INTERFACE) */
{
    {
        MQT_POOL_ID,       /* Pool ID */
        TRUE               /* Exact Match Requirement */
    }
};


/** ============================================================================
 *  @name   MESSAGE_Pools
 *
 *  @desc   Array of pools.
 *  ============================================================================
 */
POOL_Obj MESSAGE_Pools [NUM_POOLS] =
{
#if defined (DSP_BOOTMODE_NOBOOT)
    POOL_NOENTRY,
#else
    {
#if ( PHYINTERFACE == PCI_INTERFACE) ||( PHYINTERFACE == VLYNQ_INTERFACE)
        &DMAPOOL_init,
        (POOL_Fxns *) &DMAPOOL_FXNS,
#else
        &SMAPOOL_init,               /* Init Function                        */
        (POOL_Fxns *) &SMAPOOL_FXNS, /* Pool interface functions             */
#endif
        &MESSAGE_PoolParams [MQT_POOL_ID], /* Pool params                    */
        NULL                         /* Pool object: Set within pool impl.   */
    },
#endif
    POOL_NOENTRY,
    POOL_NOENTRY,
    POOL_NOENTRY,
    POOL_NOENTRY,
    POOL_NOENTRY,
    POOL_NOENTRY,
    POOL_NOENTRY,
    POOL_NOENTRY,
    POOL_NOENTRY,
    POOL_NOENTRY,
    POOL_NOENTRY,
    POOL_NOENTRY,
    POOL_NOENTRY,
    POOL_NOENTRY,
    POOL_NOENTRY,
    POOL_NOENTRY
} ;

/** ============================================================================
 *  @name   POOL_config
 *
 *  @desc   POOL configuration information.
 *          POOL_config is a required global variable.
 *  ============================================================================
 */
POOL_Config POOL_config = {MESSAGE_Pools, NUM_POOLS} ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
