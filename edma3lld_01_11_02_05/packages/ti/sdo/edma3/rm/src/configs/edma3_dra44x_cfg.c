/*
 * edma3_dra44x_cfg.c
 *
 * EDMA3 Resource Manager Adaptation Configuration File (SoC Specific).
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

#include <ti/sdo/edma3/rm/edma3_rm.h>

/** Total number of DMA Channels supported by the EDMA3 Controller */
#define NUM_DMA_CHANNELS                        (64u)
/** Total number of QDMA Channels supported by the EDMA3 Controller */
#define NUM_QDMA_CHANNELS                       (8u)
/** Total number of TCCs supported by the EDMA3 Controller */
#define NUM_TCC                                 (64u)
/** Total number of PaRAM Sets supported by the EDMA3 Controller */
#define NUM_PARAM_SETS                          (128u)
/** Total number of Event Queues in the EDMA3 Controller */
#define NUM_EVENT_QUEUE                         (3u)
/** Total number of Transfer Controllers (TCs) in the EDMA3 Controller */
#define NUM_TC                                  (3u)
/** Number of Regions on this EDMA3 controller */
#define NUM_REGION                              (2u)

/**
 * \brief Channel mapping existence
 * A value of 0 (No channel mapping) implies that there is fixed association
 * for a channel number to a parameter entry number or, in other words,
 * PaRAM entry n corresponds to channel n.
 */
#define CHANNEL_MAPPING_EXISTENCE               (0u)
/** Existence of memory protection feature */
#define MEM_PROTECTION_EXISTENCE                (0u)

/** Global Register Region of CC Registers */
#define CC_BASE_ADDRESS                         (0x01C00000u)
/** Transfer Controller 0 Registers */
#define TC0_BASE_ADDRESS                        (0x01C10000u)
/** Transfer Controller 1 Registers */
#define TC1_BASE_ADDRESS                        (0x01C10400u)
/** Transfer Controller 2 Registers */
#define TC2_BASE_ADDRESS                        (0x01C10800u)
/** Transfer Controller 3 Registers */
#define TC3_BASE_ADDRESS                        NULL
/** Transfer Controller 4 Registers */
#define TC4_BASE_ADDRESS                        NULL
/** Transfer Controller 5 Registers */
#define TC5_BASE_ADDRESS                        NULL
/** Transfer Controller 6 Registers */
#define TC6_BASE_ADDRESS                        NULL
/** Transfer Controller 7 Registers */
#define TC7_BASE_ADDRESS                        NULL

/** Interrupt no. for Transfer Completion */
#define XFER_COMPLETION_INT                     (42u)
/** Interrupt no. for CC Error */
#define CC_ERROR_INT                            (43u)
/** Interrupt no. for TC 0 Error */
#define TC0_ERROR_INT                           (44u)
/** Interrupt no. for TC 1 Error */
#define TC1_ERROR_INT                           (45u)
/** Interrupt no. for TC 2 Error */
#define TC2_ERROR_INT                           (46u)
/** Interrupt no. for TC 3 Error */
#define TC3_ERROR_INT                           (0u)
/** Interrupt no. for TC 4 Error */
#define TC4_ERROR_INT                           (0u)
/** Interrupt no. for TC 5 Error */
#define TC5_ERROR_INT                           (0u)
/** Interrupt no. for TC 6 Error */
#define TC6_ERROR_INT                           (0u)
/** Interrupt no. for TC 7 Error */
#define TC7_ERROR_INT                           (0u)


/**
 * \brief Mapping of DMA channels 0-31 to Hardware Events from
 * various peripherals, which use EDMA for data transfer.
 * All channels need not be mapped, some can be free also.
 * 1: Mapped
 * 0: Not mapped
 *
 * This mapping will be used to allocate DMA channels when user passes
 * EDMA3_RM_DMA_CHANNEL_ANY as dma channel id (for eg to do memory-to-memory
 * copy). The same mapping is used to allocate the TCC when user passes
 * EDMA3_RM_TCC_ANY as tcc id (for eg to do memory-to-memory copy).
 *
 * To allocate more DMA channels or TCCs, one has to modify the event mapping.
 */
#define DMA_CHANNEL_TO_EVENT_MAPPING_0           (0xD833F03Fu)


/**
 * \brief Mapping of DMA channels 32-63 to Hardware Events from
 * various peripherals, which use EDMA for data transfer.
 * All channels need not be mapped, some can be free also.
 * 1: Mapped
 * 0: Not mapped
 *
 * This mapping will be used to allocate DMA channels when user passes
 * EDMA3_RM_DMA_CHANNEL_ANY as dma channel id (for eg to do memory-to-memory
 * copy). The same mapping is used to allocate the TCC when user passes
 * EDMA3_RM_TCC_ANY as tcc id (for eg to do memory-to-memory copy).
 *
 * To allocate more DMA channels or TCCs, one has to modify the event mapping.
 */
#define DMA_CHANNEL_TO_EVENT_MAPPING_1          (0x30FFC0FFu)


EDMA3_RM_GblConfigParams edma3GblCfgParams [EDMA3_MAX_EDMA3_INSTANCES] =
{
    {
    /** Total number of DMA Channels supported by the EDMA3 Controller */
    NUM_DMA_CHANNELS,
    /** Total number of QDMA Channels supported by the EDMA3 Controller */
    NUM_QDMA_CHANNELS,
    /** Total number of TCCs supported by the EDMA3 Controller */
    NUM_TCC,
    /** Total number of PaRAM Sets supported by the EDMA3 Controller */
    NUM_PARAM_SETS,
    /** Total number of Event Queues in the EDMA3 Controller */
    NUM_EVENT_QUEUE,
    /** Total number of Transfer Controllers (TCs) in the EDMA3 Controller */
    NUM_TC,
    /** Number of Regions on this EDMA3 controller */
    NUM_REGION,

    /**
     * \brief Channel mapping existence
     * A value of 0 (No channel mapping) implies that there is fixed association
     * for a channel number to a parameter entry number or, in other words,
     * PaRAM entry n corresponds to channel n.
     */
    CHANNEL_MAPPING_EXISTENCE,

    /** Existence of memory protection feature */
    MEM_PROTECTION_EXISTENCE,

    /** Global Register Region of CC Registers */
    (void *)(CC_BASE_ADDRESS),
    /** Transfer Controller (TC) Registers */
        {
        (void *)(TC0_BASE_ADDRESS),
        (void *)(TC1_BASE_ADDRESS),
        (void *)(TC2_BASE_ADDRESS),
        (void *)(TC3_BASE_ADDRESS),
        (void *)(TC4_BASE_ADDRESS),
        (void *)(TC5_BASE_ADDRESS),
        (void *)(TC6_BASE_ADDRESS),
        (void *)(TC7_BASE_ADDRESS)
        },
    /** Interrupt no. for Transfer Completion */
    XFER_COMPLETION_INT,
    /** Interrupt no. for CC Error */
    CC_ERROR_INT,
    /** Interrupt no. for TCs Error */
        {
        TC0_ERROR_INT,
        TC1_ERROR_INT,
        TC2_ERROR_INT,
        TC3_ERROR_INT,
        TC4_ERROR_INT,
        TC5_ERROR_INT,
        TC6_ERROR_INT,
        TC7_ERROR_INT
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
        0u,
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
        0u,
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
        64u,
        64u,
        64u,
        0u,
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
        0, 1u, 2u, 3u, 4u, 5u, 6u, 7u,
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
           0u, 1u, 2u, 3u,
           4u, 5u, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
           8u, 9u, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
           12u, 13u, 14u, 15u,
           16u, 17u,
           EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
           20u, 21u, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
           EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
           EDMA3_RM_CH_NO_TCC_MAP, 27u,
           28u, EDMA3_RM_CH_NO_TCC_MAP,
           30u, 31u,

           32u, 33u, 34u, 35u,
           36u, 37u, 38u, 39u,
           EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
           EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
           EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, 46u, 47u,
           48u, 49u, 50u, 51u,
           52u, 53u, 54u, 55u,
           EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
           EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
           60u, 61u,
           EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP
        },

    /**
     * \brief Mapping of DMA channels to Hardware Events from
     * various peripherals, which use EDMA for data transfer.
     * All channels need not be mapped, some can be free also.
     */
        {
        DMA_CHANNEL_TO_EVENT_MAPPING_0,
        DMA_CHANNEL_TO_EVENT_MAPPING_1
        }
    }
};


/* Default RM Instance Initialization Configuration */
EDMA3_RM_InstanceInitConfig defInstInitConfig [EDMA3_MAX_EDMA3_INSTANCES][NUM_REGION] =
{
        {
          {
            /* Resources owned by Region 0 */
            /* ownPaRAMSets */
            {0x1FF73003u, 0x19FFFE66u, 0xFFFFFFFFu, 0x0u,
            0x0u, 0x0u, 0x0u, 0x0u,
            0x0u, 0x0u, 0x0u, 0x0u,
            0x0u, 0x0u, 0x0u, 0x0u},

            /* ownDmaChannels */
            {0x1FF73003u, 0x19FFFE66u},

            /* ownQdmaChannels */
            {0x0u},

            /* ownTccs */
            {0x0u, 0x0u},

            /* Resources reserved by Region 0 */
            /* resvdPaRAMSets */
            {0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u,
            0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u},

            /* resvdDmaChannels */
            {0x0u, 0x0u},

            /* resvdQdmaChannels */
            {0x0u},

            /* resvdTccs */
            {0x0u, 0x0u},
          },

          {
            /* Resources owned by Region 1 */
            /* ownPaRAMSets */
            /* Since channel and paramsets are tied, mark them as owned,
               along with this, we would require couple of more paramsets
               linking and others. keeping 32 paramsets as owned */
            {DMA_CHANNEL_TO_EVENT_MAPPING_0, DMA_CHANNEL_TO_EVENT_MAPPING_1,
            0x00000000u, 0xFFFFFFFFu,
            0x0u, 0x0u, 0x0u, 0x0u,
            0x0u, 0x0u, 0x0u, 0x0u,
            0x0u, 0x0u, 0x0u, 0x0u},

            /* ownDmaChannels */
            /* Mark the channels used as owned and add some to take care
               when any channels is requested */
            {DMA_CHANNEL_TO_EVENT_MAPPING_0, (DMA_CHANNEL_TO_EVENT_MAPPING_1 | 0xFFFFFFFFu)},

            /* ownQdmaChannels */
            {0x00000080u},

            /* ownTccs */
            {0xFFFFFFC0u, 0xFFFFFFFFu},

            /* Resources reserved by Region 1 */
            /* resvdPaRAMSets */
            {DMA_CHANNEL_TO_EVENT_MAPPING_0,
            DMA_CHANNEL_TO_EVENT_MAPPING_1,
            0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u,
            0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u, 0x0u},

            /* resvdDmaChannels */
            {DMA_CHANNEL_TO_EVENT_MAPPING_0, DMA_CHANNEL_TO_EVENT_MAPPING_1},

            /* resvdQdmaChannels */
            {0x0u},

            /* resvdTccs */
            {DMA_CHANNEL_TO_EVENT_MAPPING_0, DMA_CHANNEL_TO_EVENT_MAPPING_1},
          }
        }
};

/* End of File */
