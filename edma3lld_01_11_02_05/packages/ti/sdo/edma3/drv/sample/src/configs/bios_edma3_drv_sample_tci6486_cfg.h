/*
 * bios_edma3_drv_sample_tci6486_cfg.h
 *
 * SoC specific EDMA3 hardware related information like number of transfer
 * controllers, various interrupt ids etc. It is used while interrupts
 * enabling / disabling. It needs to be ported for different SoCs.
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
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

/** Number of Event Queues available */
#define EDMA3_NUM_EVTQUE                                (4u)

/** Number of Transfer Controllers available */
#define EDMA3_NUM_TC                                    (4u)

/** Transfer Completion Interrupt for Shadow Region 0, rounted to Core 0 only */
#define EDMA3_CC_XFER_COMPLETION_INT_0					(15u)
/** Transfer Completion Interrupt for Shadow Region 1, rounted to Core 1 only */
#define EDMA3_CC_XFER_COMPLETION_INT_1					(15u)
/** Transfer Completion Interrupt for Shadow Region 2, rounted to Core 2 only */
#define EDMA3_CC_XFER_COMPLETION_INT_2					(15u)
/** Transfer Completion Interrupt for Shadow Region 3, rounted to Core 3 only */
#define EDMA3_CC_XFER_COMPLETION_INT_3					(15u)
/** Transfer Completion Interrupt for Shadow Region 4, rounted to Core 4 only */
#define EDMA3_CC_XFER_COMPLETION_INT_4					(15u)
/** Transfer Completion Interrupt for Shadow Region 5, rounted to Core 5 only */
#define EDMA3_CC_XFER_COMPLETION_INT_5					(15u)
/** Transfer Completion Interrupt for Shadow Region 6, rounted to all six cores */
#define EDMA3_CC_XFER_COMPLETION_INT_6					(67u)
/** Transfer Completion Interrupt for Shadow Region 7, rounted to all six cores */
#define EDMA3_CC_XFER_COMPLETION_INT_7					(68u)

/** Interrupt no. for CC Error */
#define EDMA3_CC_ERROR_INT                              (57u)

/** Interrupt no. for TCs Error */
#define EDMA3_TC0_ERROR_INT                             (59u)
#define EDMA3_TC1_ERROR_INT                             (60u)
#define EDMA3_TC2_ERROR_INT                             (61u)
#define EDMA3_TC3_ERROR_INT                             (62u)
#define EDMA3_TC4_ERROR_INT                             (0u)
#define EDMA3_TC5_ERROR_INT                             (0u)
#define EDMA3_TC6_ERROR_INT                             (0u)
#define EDMA3_TC7_ERROR_INT                             (0u)

/**
* EDMA3 interrupts (transfer completion, CC error etc.) correspond to different
* ECM events (SoC specific). These ECM events come
* under ECM block XXX (handling those specific ECM events). Normally, block
* 0 handles events 4-31 (events 0-3 are reserved), block 1 handles events
* 32-63 and so on. This ECM block XXX (or interrupt selection number XXX)
* is mapped to a specific HWI_INT YYY in the tcf file.
* For TCI6488, following mappings has been done:
* ECM Block 0 (Events 04-31) --> HWI Interrupt 7
* ECM Block 1 (Events 32-63) --> HWI Interrupt 8
* ECM Block 2 (Events 64-95) --> HWI Interrupt 9
* ECM Block 3 (Events 96-127) --> HWI Interrupt 10
* These defines below specify which ECM event is mapped to which HWI interrupt.
* Define EDMA3_HWI_INT_XFER_COMP[n] to specific HWI_INT, corresponding
* to transfer completion interrupt, on that particular DSP core.
* Define EDMA3_HWI_INT_CC_ERR to specific HWI_INT, corresponding
* to CC error interrupts.
* Define EDMA3_HWI_INT_TC_ERR to specific HWI_INT, corresponding
* to TC error interrupts.
*/
/* Transfer completion interrupt for DSP 0 lies in ECM block 0. */
#define EDMA3_HWI_INT_XFER_COMP_0						(7u)
/* Transfer completion interrupt for DSP 1 lies in ECM block 0. */
#define EDMA3_HWI_INT_XFER_COMP_1						(7u)
/* Transfer completion interrupt for DSP 2 lies in ECM block 0. */
#define EDMA3_HWI_INT_XFER_COMP_2						(7u)
/* Transfer completion interrupt for DSP 3 lies in ECM block 0. */
#define EDMA3_HWI_INT_XFER_COMP_3						(7u)
/* Transfer completion interrupt for DSP 4 lies in ECM block 0. */
#define EDMA3_HWI_INT_XFER_COMP_4						(7u)
/* Transfer completion interrupt for DSP 5 lies in ECM block 0. */
#define EDMA3_HWI_INT_XFER_COMP_5						(7u)

/** CC Error Interrupt lies in ECM block 1.                     */
#define EDMA3_HWI_INT_CC_ERR							(8u)

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
#define EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1          (0x00000000u)

/* End of File */

