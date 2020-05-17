/*
 * mmcsd_evmInit.c
 *
 * This file contains MMCSD application related EVM (platform) specific routines
 * implementation. 
 * This is part of the EVM Initialization library (evmInit) which contains pinmux
 * enabling routines for peripherals used by the application, and may contain 
 * related device pre-driver initialization routines.
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

/**
 *  \file   mcbsp_evmInit.c
 *
 *  \brief  This file contains the board specific code for enabling the use of
 *          mcbsp driver.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#include <std.h>
#include "ti/pspiom/platforms/evmOMAPL138/Mcbsp_evmInit.h"
#include "ti/pspiom/cslr/soc_OMAPL138.h"
#include "ti/pspiom/cslr/cslr_syscfg0_OMAPL138.h"

/*============================================================================*/
/*                          MACRO DEFINITIONS                                 */
/*============================================================================*/

/*
 * Function to enable the pinmux for the mcbsp device.
 *
 */
void configureMcbsp(void)
{
    CSL_SyscfgRegsOvly syscfgRegs = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;
    
    /*KICK0R and KICK1R registers needs to be enabled for this PINMUX setting *
     * to be successful. This needs to be ensured, by the bootloader or GEL   *
     * file, and should be done by only one entity else race conditions in    *
     * SYSCFG registers accesses could result                                 */


    syscfgRegs->PINMUX1 &= ~(CSL_SYSCFG_PINMUX1_PINMUX1_7_4_MASK |
                             CSL_SYSCFG_PINMUX1_PINMUX1_11_8_MASK |
                             CSL_SYSCFG_PINMUX1_PINMUX1_15_12_MASK |
                             CSL_SYSCFG_PINMUX1_PINMUX1_19_16_MASK |
                             CSL_SYSCFG_PINMUX1_PINMUX1_23_20_MASK |
                             CSL_SYSCFG_PINMUX1_PINMUX1_27_24_MASK |
                             CSL_SYSCFG_PINMUX1_PINMUX1_31_28_MASK);

    syscfgRegs->PINMUX1 = 0x22222220;

    /* set the emulation suspend source as DSP                                */
    syscfgRegs->SUSPSRC &= (~CSL_SYSCFG_SUSPSRC_MCBSP1SRC_MASK);
    syscfgRegs->SUSPSRC |= (CSL_SYSCFG_SUSPSRC_MCBSP1SRC_MASK);
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
