/*
 * gpio_evmInit.c
 *
 * This file contains GPIO application related EVM (platform) specific routines
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
 *  \file   gpio_evmInit.c
 *
 *  \brief  This file contains the board specific code for enabling the use of
 *          GPIO driver.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#include "std.h"

#include "ti/pspiom/cslr/soc_OMAPL137.h"
#include "ti/pspiom/cslr/cslr_syscfg_OMAPL137.h"

#include "ti/pspiom/platforms/evmOMAPL137/gpio_evmInit.h"

/*============================================================================*/
/*                          MACRO DEFINITIONS                                 */
/*============================================================================*/


/*
 *  Function to configure the lcd LIDD module in the PINMUX registers and the
 *  PSC module.
 */
void configureGpio(void)
{
    /* sys config registers overlay                                           */
    CSL_SyscfgRegsOvly   sysRegs  = (CSL_SyscfgRegsOvly)(CSL_SYSCFG_0_REGS);    
    
    /*KICK0R and KICK1R registers needs to be enabled for this PINMUX setting *
     * to be successful. This needs to be ensured, by the bootloader or GEL   *
     * file, and should be done by only one entity else race conditions in    *
     * SYSCFG registers accesses could result                                 */

    /* mux between EMA_D8 and GPIO0_8 : enable GPIO0_8 (User Switch - "SW3-1")*/
    sysRegs->PINMUX14 &= (~(CSL_SYSCFG_PINMUX14_PINMUX14_27_24_MASK));
    sysRegs->PINMUX14 |= ( (CSL_SYSCFG_PINMUX14_PINMUX14_27_24_GPIO0_8) << \
                        (CSL_SYSCFG_PINMUX14_PINMUX14_27_24_SHIFT) );

    /* mux between EMA_D12 and GPIO0_12 : enable GPIO0_12 (User Led - "DS1")  */
    sysRegs->PINMUX15 &= (~(CSL_SYSCFG_PINMUX15_PINMUX15_11_8_MASK));
    sysRegs->PINMUX15 |= ( (CSL_SYSCFG_PINMUX15_PINMUX15_11_8_GPIO0_12) << \
                        (CSL_SYSCFG_PINMUX15_PINMUX15_11_8_SHIFT)  );

}


/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
