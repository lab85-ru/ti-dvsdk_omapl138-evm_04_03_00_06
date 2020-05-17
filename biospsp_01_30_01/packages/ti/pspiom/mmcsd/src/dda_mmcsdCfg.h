/*
 * dda_mmcsdCfg.h
 *
 * This file contains MMCSD Configuration macros and definitions used for device 
 * specific hardware initialization settings.
 * MMCSD Driver provides Driver Layer Interface to do operations on the MMCSD  
 * peripheral like device initialization, channel creation, control commands for 
 * peripheral specific operations etc
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

/** \file   dda_mmcsdCfg.h
 *
 *  \brief  MMCSD DDA Configuration file for BIOS
 *
 *  This file contains the device specific hardware initialization settings.
 *  These settings are passed onto the driver.
 *  Any default hardware settings should go in these configuration file.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */


#ifndef _DDA_MMCSDCFG_H_
#define _DDA_MMCSDCFG_H_

#if !defined(CHIP_C6747) && !defined(CHIP_OMAPL137) && !defined(CHIP_C6748) && !defined(CHIP_OMAPL138)
#error "No chip type defined! (Must use -DCHIP_C6747 or -DCHIP_OMAPL137 or -DCHIP_C6748 or -DCHIP_OMAPL138)"
#endif

#if (defined(CHIP_C6748) || defined(CHIP_OMAPL138)) && !defined(BIOS_PWRM_ENABLE)
#error "Wrong Chip ID defined for BIOS_PWRM_ENABLE."
#endif

#if (defined(CHIP_C6747) || defined(CHIP_OMAPL137)) && defined(BIOS_PWRM_ENABLE)
#error "Do not use Bios PWRM module for this SoC"
#endif

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#ifdef CHIP_C6747
    #include <ti/pspiom/cslr/soc_C6747.h>
#endif

#ifdef CHIP_OMAPL137
    #include <ti/pspiom/cslr/soc_OMAPL137.h>
#endif

#ifdef CHIP_C6748
    #include <ti/pspiom/cslr/soc_C6748.h>
#endif

#ifdef CHIP_OMAPL138
    #include <ti/pspiom/cslr/soc_OMAPL138.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                             MACRO DEFINITIONS                              */
/* ========================================================================== */
#define PSP_MMCSD_NUM_INSTANCES         CSL_MMCSD_PER_CNT
/**< MMCSD Instances on hardware. it should be CSL_MMCSD_PER_CNT*/

#if (PSP_MMCSD_NUM_INSTANCES > 0x0)
#define MMCSD_INST_ID0                  CSL_MMCSD_0
/**< MMCSD Instance 0. it should be CSL_MMCSD_0                               */
#endif

#if (PSP_MMCSD_NUM_INSTANCES > 0x1)
#define MMCSD_INST_ID1                  CSL_MMCSD_1
/**< MMCSD Instance 1. it should be CSL_MMCSD_1                               */
#endif

#define MMCSD_MAX_WORDS_IN_FIFO         16U
/**< MMCSD FIFO size : No of words in FIFO. Current value is for 64 bytes FIFO*/
#define MMCSD_INIT_FREQ_MAX             400000U
/**< The maximum init frequency during card identification                    */
#define MMCSD_INIT_CLOCK                215000U
/**< MMCSD Init clock in Hz(card identification mode should be < 400KHz)      */
#define MMC_CLOCK                       20000000U
/**< MMC clock in Hz during data transfer mode                                */
#define SD_CLOCK                        25000000U
/**< SD clock in Hz during data transfer mode                                 */
#define SD_HIGH_SPEED_CLOCK             50000000U
/**< SD High Speed clock in Hz                                                */
#define MMCSD_TIME_OUT_COUNT            0xFFFFFFU
/**< MMCSD card timeout count                                                 */
#define DDC_MMCSD_IO_DEPTH              20U
/**< Asynchronous IO Depth. Specifies the max number of IOs
 * that can be pending at any time                                            */
#define DDC_MMCSD_NUM_OF_TRIES          0x1000U
/**< Num of times it will try to read OCR when initializing the SD/MMC card   */
#define PSP_MMCSD_DEV_TASKPRIO          5
/**< Task Priority. The priority of the driver task should be greater than the
application task                                                              */
#define PSP_MMCSD_DEV_STACK_SIZE_BYTE   0x2000U
/**< Stack size in bytes                                                      */
#define MMCSD_FIFO_SIZE_32_BYTES        32U
/**< MMCSD FIFO size is 32 bytes                                              */
#define MMCSD_FIFO_SIZE_64_BYTES        64U
/**< MMCSD FIFO size is 64 bytes                                              */
#define FIFO_WIDTH_IN_BYTES             4U
/**< FIFO width in bytes                                                      */

#ifdef __cplusplus
}
#endif

#endif /* _DDA_MMCSDCFG_H_ */
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
