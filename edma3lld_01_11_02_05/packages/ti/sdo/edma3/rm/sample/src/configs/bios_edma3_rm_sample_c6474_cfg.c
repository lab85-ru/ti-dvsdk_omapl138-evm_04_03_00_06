/*
 * bios_edma3_rm_sample_c6474_cfg.c
 *
 * SoC specific EDMA3 hardware related information like number of transfer
 * controllers, various interrupt ids etc. It is used while interrupt enabling
 * or disabling. It needs to be ported for different SoCs.
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
#include <ti/sdo/edma3/rm/sample/src/configs/bios_edma3_rm_sample_c6474_cfg.h>

/* Variable which will be used internally for referring number of Event Queues. */
unsigned int numEdma3EvtQue = EDMA3_NUM_EVTQUE;

/* Variable which will be used internally for referring number of TCs. */
unsigned int numEdma3Tc = EDMA3_NUM_TC;

/**
 * Variable which will be used internally for referring transfer completion
 * interrupt.
 */
unsigned int ccXferCompInt[8] = {
								EDMA3_CC_XFER_COMPLETION_INT_0,
								EDMA3_CC_XFER_COMPLETION_INT_1,
								EDMA3_CC_XFER_COMPLETION_INT_2,
								EDMA3_CC_XFER_COMPLETION_INT_3,
								EDMA3_CC_XFER_COMPLETION_INT_4,
								EDMA3_CC_XFER_COMPLETION_INT_5,
								EDMA3_CC_XFER_COMPLETION_INT_6,
								EDMA3_CC_XFER_COMPLETION_INT_7
								};

/**
 * Variables which will be used internally for referring the hardware interrupt
 * for various EDMA3 interrupts.
 */
unsigned int hwIntXferComp[3] = {
								EDMA3_HWI_INT_XFER_COMP_0,
								EDMA3_HWI_INT_XFER_COMP_1,
								EDMA3_HWI_INT_XFER_COMP_2
								};
unsigned int hwIntCcErr = EDMA3_HWI_INT_CC_ERR;

/**
 * Variable which will be used internally for referring to CC error interrupt.
 * It will be used to program the CIC registers.
 */
unsigned int ccErrorIntCic = EDMA3_CC_ERROR_INT;
/**
 * Variable which will be used internally for referring to CC error interrupt.
 * It will be used to enable CC error interrupt in ECM .
 */
unsigned int ccErrorInt = EDMA3_CICn_EVT0;

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

/** Whether global configuration required for EDMA3 or not.
 * This configuration should be done only once for the EDMA3 hardware by
 * any of the masters.
 */
unsigned int gblCfgReqdArray [3] = {
									0,	/* Master, will do the global init */
									1,	/* Slave, will not do the global init  */
									1	/* Slave, will not do the global init  */
									};

/* Driver Object Initialization Configuration */
EDMA3_RM_GblConfigParams sampleEdma3GblCfgParams =
    {
    /** Total number of DMA Channels supported by the EDMA3 Controller */
    64u,
    /** Total number of QDMA Channels supported by the EDMA3 Controller */
    8u,
    /** Total number of TCCs supported by the EDMA3 Controller */
    64u,
    /** Total number of PaRAM Sets supported by the EDMA3 Controller */
    256u,
    /** Total number of Event Queues in the EDMA3 Controller */
    6u,
    /** Total number of Transfer Controllers (TCs) in the EDMA3 Controller */
    6u,
    /** Number of Regions on this EDMA3 controller */
    8u,

    /**
     * \brief Channel mapping existence
     * A value of 0 (No channel mapping) implies that there is fixed association
     * for a channel number to a parameter entry number or, in other words,
     * PaRAM entry n corresponds to channel n.
     */
    1u,

    /** Existence of memory protection feature */
    1u,

    /** Global Register Region of CC Registers */
    (void *)0x02A00000u,
    /** Transfer Controller (TC) Registers */
        {
        (void *)0x02A20000u,
        (void *)0x02A28000u,
        (void *)0x02A30000u,
        (void *)0x02A38000u,
        (void *)0x02A40000u,
        (void *)0x02A48000u,
        (void *)NULL,
        (void *)NULL
        },
    /** Interrupt no. for Transfer Completion */
    EDMA3_CC_XFER_COMPLETION_INT_0,
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
        4u,
        5u,
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
        16u,
        16u,
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
        64u,
        64u,
        64u,
        0u,
        0u
        },

    /**
     * \brief Mapping from each DMA channel to a Parameter RAM set,
     * if it exists, otherwise of no use.
     */
        {
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
        EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP
        },

     /**
      * \brief Mapping from each DMA channel to a TCC. This specific
      * TCC code will be returned when the transfer is completed
      * on the mapped channel.
      */
        {
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
        EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
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
EDMA3_RM_InstanceInitConfig sampleInstInitConfig [3] =
{
  {
    /* Resources owned by DSP 0 */
     /* ownPaRAMSets */
    /* 31     0     63    32     95    64     127   96 */
    {0x00000000u, 0xFFFFFFFFu, 0x003FFFFFu, 0x00000000u,
    /* 159  128     191  160     223  192     255  224 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    /* 287  256     319  288     351  320     383  352 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    /* 415  384     447  416     479  448     511  480 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,},

    /* ownDmaChannels */
    /* 31     0     63    32 */
    {0x003FFFFFu, 0x00000000u},

    /* ownQdmaChannels */
    /* 31     0 */
    {0x00000007u},

    /* ownTccs */
    /* 31     0     63    32 */
    {0x003FFFFFu, 0x00000000u},

    /* Resources reserved by DSP 0 */
    /* resvdPaRAMSets */
    /* 31     0     63    32     95    64     127   96 */
    {0xFFFFFFFFu, 0x00000000u, 0x00000000u, 0x00000000u,
    /* 159  128     191  160     223  192     255  224 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    /* 287  256     319  288     351  320     383  352 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    /* 415  384     447  416     479  448     511  480 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,},

    /* resvdDmaChannels */
    /* 31     					  0  63    						 32 */
    {EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1},

    /* resvdQdmaChannels */
    /* 31     0 */
    {0x00000000u},

    /* resvdTccs */
    /* 31     					  0  63    						 32 */
    {EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1},
  },

  {
    /* Resources owned by DSP 1 */
    /* ownPaRAMSets */
    /* 31     0     63    32     95    64     127   96 */
    {0x00000000u, 0x00000000u, 0xFFC00000u, 0xFFFFFFFFu,
    /* 159  128     191  160     223  192     255  224 */
     0xFFFFFFFFu, 0x000007FFu, 0x00000000u, 0x00000000u,
    /* 287  256     319  288     351  320     383  352 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    /* 415  384     447  416     479  448     511  480 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,},

    /* ownDmaChannels */
    /* 31     0     63    32 */
   	{0xFFC00000u, 0x000007FFu},

    /* ownQdmaChannels */
    /* 31     0 */
    {0x00000038u},

    /* ownTccs */
    /* 31     0     63    32 */
   	{0xFFC00000u, 0x000007FFu},

    /* Resources reserved by DSP 1 */
    /* resvdPaRAMSets */
    /* 31     0     63    32     95    64     127   96 */
    {0xFFFFFFFFu, 0x00000000u, 0x00000000u, 0x00000000u,
    /* 159  128     191  160     223  192     255  224 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    /* 287  256     319  288     351  320     383  352 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    /* 415  384     447  416     479  448     511  480 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,},

    /* resvdDmaChannels */
    /* 31     					  0  63    						 32 */
    {EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1},

    /* resvdQdmaChannels */
    /* 31     0 */
    {0x00000000u},

    /* resvdTccs */
    /* 31     					  0  63    						 32 */
    {EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1},
  },

  {
    /* Resources owned by DSP 2 */
     /* ownPaRAMSets */
    /* 31     0     63    32     95    64     127   96 */
    {0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    /* 159  128     191  160     223  192     255  224 */
     0x00000000u, 0xFFFFF800u, 0xFFFFFFFFu, 0xFFFFFFFFu,
    /* 287  256     319  288     351  320     383  352 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    /* 415  384     447  416     479  448     511  480 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,},

    /* ownDmaChannels */
    /* 31     0     63    32 */
    {0x00000000u, 0xFFFFF800u},

    /* ownQdmaChannels */
    /* 31     0 */
    {0x000000C0u},

    /* ownTccs */
    /* 31     0     63    32 */
    {0x00000000u, 0xFFFFF800u},

    /* Resources reserved by DSP 2 */
    /* resvdPaRAMSets */
    /* 31     0     63    32     95    64     127   96 */
    {0xFFFFFFFFu, 0x00000000u, 0x00000000u, 0x00000000u,
    /* 159  128     191  160     223  192     255  224 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    /* 287  256     319  288     351  320     383  352 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    /* 415  384     447  416     479  448     511  480 */
     0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,},

    /* resvdDmaChannels */
    /* 31     0     63    32 */
    {EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1},

    /* resvdQdmaChannels */
    /* 31     0 */
    {0x00000000u},

    /* resvdTccs */
    /* 31     0     63    32 */
    {EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1},
  },
};

/* End of File */

