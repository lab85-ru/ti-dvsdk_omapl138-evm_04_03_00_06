/*
 * bios_edma3_drv_sample_tci6488_cfg.h
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

/** Number of Event Queues available */
#define EDMA3_NUM_EVTQUE                                (6u)

/** Number of Transfer Controllers available */
#define EDMA3_NUM_TC                                    (6u)

/** Transfer Completion Interrupt for Shadow Region 0 */
#define EDMA3_CC_XFER_COMPLETION_INT_0					(62u)
/** Transfer Completion Interrupt for Shadow Region 1 */
#define EDMA3_CC_XFER_COMPLETION_INT_1					(63u)
/** Transfer Completion Interrupt for Shadow Region 2 */
#define EDMA3_CC_XFER_COMPLETION_INT_2					(64u)
/** Transfer Completion Interrupt for Shadow Region 3 */
#define EDMA3_CC_XFER_COMPLETION_INT_3					(65u)
/** Transfer Completion Interrupt for Shadow Region 4 */
#define EDMA3_CC_XFER_COMPLETION_INT_4					(66u)
/** Transfer Completion Interrupt for Shadow Region 5 */
#define EDMA3_CC_XFER_COMPLETION_INT_5					(67u)
/** Transfer Completion Interrupt for Shadow Region 6 */
#define EDMA3_CC_XFER_COMPLETION_INT_6					(68u)
/** Transfer Completion Interrupt for Shadow Region 7 */
#define EDMA3_CC_XFER_COMPLETION_INT_7					(69u)

/**
 * Unlike Transfer Completion Interrupts, CC Error Interrupt is NOT configured
 * through the C64x+ Megamodule Interrupt Controller. Rather, this event is
 * mapped to a specific core through the core specific CIC (Chip Interrupt
 * Controller). Each CIC can take 64 input events and route each of these to
 * any of the distinct 16 output pins by programming the event mux registers
 * (EVTMUX) output event selection fields (EVTMUXx[CIC_EVTy], where x = 0,1,2,3
 * and y = 0,1,….15).
 * TPCC_ERRINT (36) is given as input event to each of the CIC. CIC will be
 * programmed to route this event to the output pin CICn_EVT0. CICn_EVT0 (80)
 * is given as input to the C64x+ Megamodule Interrupt Controller.
 */
#define EDMA3_CICn_EVT0									(80u)

/** Interrupt no. for CC Error */
#define EDMA3_CC_ERROR_INT                              (36u)

/** Interrupt no. for TCs Error */
#define EDMA3_TC0_ERROR_INT                             (38u)
#define EDMA3_TC1_ERROR_INT                             (39u)
#define EDMA3_TC2_ERROR_INT                             (40u)
#define EDMA3_TC3_ERROR_INT                             (41u)
#define EDMA3_TC4_ERROR_INT                             (42u)
#define EDMA3_TC5_ERROR_INT                             (43u)
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
/* Transfer completion interrupt for DSP 0 lies in ECM block 1. */
#define EDMA3_HWI_INT_XFER_COMP_0						(8u)
/* Transfer completion interrupt for DSP 1 lies in ECM block 1. */
#define EDMA3_HWI_INT_XFER_COMP_1						(8u)
/* Transfer completion interrupt for DSP 2 lies in ECM block 2. */
#define EDMA3_HWI_INT_XFER_COMP_2						(9u)

/** Since CC Error Interrupt is routed through System Event 0 (Combined) from
 * CICn, and System event 0 (CICn_EVT0) lies in ECM block 2.
 */
#define EDMA3_HWI_INT_CC_ERR							(9u)

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
#define EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0          (0xFF3FFFFCu)


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
#define EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1          (0xFFFFE7F7u)

/* End of File */

