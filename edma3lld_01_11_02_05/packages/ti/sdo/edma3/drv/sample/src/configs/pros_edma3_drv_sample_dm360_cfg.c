/*
 * pros_edma3_drv_sample_dm360_cfg.c
 *
 * SoC specific EDMA3 hardware related information like number of transfer
 * controllers, various interrupt ids etc. It is used while interrupts
 * enabling / disabling. It needs to be ported for different SoCs.
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include <ti/sdo/edma3/drv/edma3_drv.h>

/** Number of Event Queues available */
#define EDMA3_NUM_EVTQUE                                4u

/** Number of Transfer Controllers available */
#define EDMA3_NUM_TC                                    4u

/** Interrupt no. for Transfer Completion */
#define EDMA3_CC_XFER_COMPLETION_INT                     (16u)
/** Interrupt no. for CC Error */
#define EDMA3_CC_ERROR_INT                            (17u)
/** Interrupt no. for TC 0 Error */
#define EDMA3_TC0_ERROR_INT                           (18u)
/** Interrupt no. for TC 1 Error */
#define EDMA3_TC1_ERROR_INT                           (19u)
/** Interrupt no. for TC 2 Error */
#define EDMA3_TC2_ERROR_INT                           (61u)
/** Interrupt no. for TC 3 Error */
#define EDMA3_TC3_ERROR_INT                           (62u)
/** Interrupt no. for TC 4 Error */
#define EDMA3_TC4_ERROR_INT                           (0u)
/** Interrupt no. for TC 5 Error */
#define EDMA3_TC5_ERROR_INT                           (0u)
/** Interrupt no. for TC 6 Error */
#define EDMA3_TC6_ERROR_INT                           (0u)
/** Interrupt no. for TC 7 Error */
#define EDMA3_TC7_ERROR_INT                           (0u)


/**
 * \brief Mapping of DMA channels 0-31 to Hardware Events from
 * various peripherals, which use EDMA for data transfer.
 * All channels need not be mapped, some can be free also.
 * 1: Mapped
 * 0: Not mapped
 *
 * This mapping will be used to allocate DMA channels when user passes
 * EDMA3_DRV_DMA_CHANNEL_ANY as dma channel id (for eg to do memory-to-memory
 * copy). The same mapping is used to allocate the TCC when user passes
 * EDMA3_DRV_TCC_ANY as tcc id (for eg to do memory-to-memory copy).
 *
 * To allocate more DMA channels or TCCs, one has to modify the event mapping.
 */
#define EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0          (0xFFFFFFFFu)
/**
 * \brief Mapping of DMA channels 32-63 to Hardware Events from
 * various peripherals, which use EDMA for data transfer.
 * All channels need not be mapped, some can be free also.
 * 1: Mapped
 * 0: Not mapped
 *
 * This mapping will be used to allocate DMA channels when user passes
 * EDMA3_DRV_DMA_CHANNEL_ANY as dma channel id (for eg to do memory-to-memory
 * copy). The same mapping is used to allocate the TCC when user passes
 * EDMA3_DRV_TCC_ANY as tcc id (for eg to do memory-to-memory copy).
 *
 * To allocate more DMA channels or TCCs, one has to modify the event mapping.
 */
#define EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1          (0xFFFF0000u)

/* Variable which will be used internally for referring number of Event Queues. */
unsigned int numEdma3EvtQue = EDMA3_NUM_EVTQUE;

/* Variable which will be used internally for referring number of TCs. */
unsigned int numEdma3Tc = EDMA3_NUM_TC;

/**
 * Variable which will be used internally for referring transfer completion
 * interrupt.
 */
unsigned int ccXferCompInt = EDMA3_CC_XFER_COMPLETION_INT;

/**
 * Variable which will be used internally for referring channel controller's
 * error interrupt.
 */
unsigned int ccErrorInt = EDMA3_CC_ERROR_INT;

/**
 * Variable which will be used internally for referring transfer controllers'
 * error interrupts.
 */
unsigned int tcErrorInt[8] =    {
                                EDMA3_TC0_ERROR_INT, EDMA3_TC1_ERROR_INT,
                                EDMA3_TC2_ERROR_INT, EDMA3_TC3_ERROR_INT,
                                EDMA3_TC4_ERROR_INT, EDMA3_TC5_ERROR_INT,
                                EDMA3_TC6_ERROR_INT, EDMA3_TC7_ERROR_INT
                                };

/* Driver Object Initialization Configuration */
EDMA3_DRV_GblConfigParams sampleEdma3GblCfgParams =
{
    /** Total number of DMA Channels supported by the EDMA3 Controller */
    64,
    /** Total number of QDMA Channels supported by the EDMA3 Controller */
    8,
    /** Total number of TCCs supported by the EDMA3 Controller */
    64,
    /** Total number of PaRAM Sets supported by the EDMA3 Controller */
    256,
    /** Total number of Event Queues in the EDMA3 Controller */
    4,
    /** Total number of Transfer Controllers (TCs) in the EDMA3 Controller */
    4,
    /** Number of Regions on this EDMA3 controller */
    4,

    /**
     * \brief Channel mapping existence
     * A value of 0 (No channel mapping) implies that there is fixed association
     * for a channel number to a parameter entry number or, in other words,
     * PaRAM entry n corresponds to channel n.
     */
    0,

    /** Existence of memory protection feature */
    0,

    /** Global Register Region of CC Registers */
    (void *)(0x01C00000u),
    /** Transfer Controller (TC) Registers */
        {
        (void *)(0x01C10000u),
        (void *)(0x01C10400u),
        (void *)(0x01C10800u),
        (void *)(0x01C10C00u),
        (void *)(NULL),
        (void *)(NULL),
        (void *)(NULL),
        (void *)(NULL)
        },
    /** Interrupt no. for Transfer Completion */
    EDMA3_CC_XFER_COMPLETION_INT,
    /** Interrupt no. for CC Error */
    EDMA3_CC_ERROR_INT,
    /** Interrupt no. for TCs Error */
        {
        EDMA3_TC0_ERROR_INT,
        EDMA3_TC1_ERROR_INT,
        EDMA3_TC2_ERROR_INT,
        EDMA3_TC3_ERROR_INT,
        EDMA3_TC4_ERROR_INT,
        EDMA3_TC5_ERROR_INT,
        EDMA3_TC6_ERROR_INT,
        EDMA3_TC7_ERROR_INT
        },

    /**
     * \brief EDMA3 TC priority setting
     *
     * User can program the priority of the Event Queues
     * at a system-wide level.  This means that the user can set the
     * priority of an IO initiated by either of the TCs (Transfer Controllers)
     * relative to IO initiated by the other bus masters on the
     * device (ARM, DSP, USB, etc)
     */
        {
        0u,
        1u,
        2u,
        3u,
        0u,
        0u,
        0u,
        0u
        },
    /**
     * \brief To Configure the Threshold level of number of events
     * that can be queued up in the Event queues. EDMA3CC error register
     * (CCERR) will indicate whether or not at any instant of time the
     * number of events queued up in any of the event queues exceeds
     * or equals the threshold/watermark value that is set
     * in the queue watermark threshold register (QWMTHRA).
     */
        {
        16u,
        16u,
        16u,
        16u,
        0u,
        0u,
        0u,
        0u
        },

    /**
     * \brief To Configure the Default Burst Size (DBS) of TCs.
     * An optimally-sized command is defined by the transfer controller
     * default burst size (DBS). Different TCs can have different
     * DBS values. It is defined in Bytes.
     */
        {
        32u,
        64u,
        16u,
        16u,
        0u,
        0u,
        0u,
        0u
        },

    /**
     * \brief Mapping from each DMA channel to a Parameter RAM set,
     * if it exists, otherwise of no use.
     */
        {
        0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u,
        8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u,
        16u, 17u, 18u, 19u, 20u, 21u, 22u, 23u,
        24u, 25u, 26u, 27u, 28u, 29u, 30u, 31u,
        32u, 33u, 34u, 35u, 36u, 37u, 38u, 39u,
        40u, 41u, 42u, 43u, 44u, 45u, 46u, 47u,
        48u, 49u, 50u, 51u, 52u, 53u, 54u, 55u,
        56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u
        },

     /**
      * \brief Mapping from each DMA channel to a TCC. This specific
      * TCC code will be returned when the transfer is completed
      * on the mapped channel.
      */
        {
        0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u,
        8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u,
        16u, 17u, 18u, 19u, 20u, 21u, 22u, 23u,
        24u, 25u, 26u, 27u, 28u, 29u, 30u, 31u,
        EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP,
		EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP,
        EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP,
		EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP, EDMA3_DRV_CH_NO_TCC_MAP,
        48u, 49u, 50u, 51u, 52u, 53u, 54u, 55u,
        56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u
        },


    /**
     * \brief Mapping of DMA channels to Hardware Events from
     * various peripherals, which use EDMA for data transfer.
     * All channels need not be mapped, some can be free also.
     */
        {
        EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0,
        EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1
        }
};


/* Driver Instance Initialization Configuration */
EDMA3_DRV_InstanceInitConfig sampleInstInitConfig =
{
    /* Resources owned by Region 0 */
    /* ownPaRAMSets */
    {0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
    0x0u, 0x0u, 0x0u, 0x0u,
    0x0u, 0x0u, 0x0u, 0x0u,
    0x0u, 0x0u, 0x0u, 0x0u},

    /* ownDmaChannels */
    {0xFFFFFFFFu, 0xFFFFFFFFu},

    /* ownQdmaChannels */
    {0x00000080u},

    /* ownTccs */
    {0xFFFFFFFFu, 0xFFFFFFFFu},

    /* Resources reserved by Region 1 */
    /* resvdPaRAMSets */
    {EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1,
    0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u,
    0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u},

    /* resvdDmaChannels */
    {EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1},

    /* resvdQdmaChannels */
    {0x0u},

    /* resvdTccs */
    {EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1},
};

/* End of File */

