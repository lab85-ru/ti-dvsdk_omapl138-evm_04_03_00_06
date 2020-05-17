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

#include "ti/pspiom/cslr/soc_C6748.h"
#include "ti/pspiom/cslr/cslr_syscfg0_C6748.h"
#include "ti/pspiom/cslr/soc_C6748.h"

#include "ti/pspiom/platforms/evm6748/gpio_evmInit.h"

/*============================================================================*/
/*                          MACRO DEFINITIONS                                 */
/*============================================================================*/
/* Address of the KICK0 and KICK1 register                                    */
#define KICK0                   (*((volatile Uint32 *)(0x01C14038u)))
#define KICK1                   (*((volatile Uint32 *)(0x01C1403Cu)))

#define KICK0_ENABLE_KEY    0x83E70B13u
#define KICK1_ENABLE_KEY    0x95a4f1e0u

/* Mask and value of the pinmux register for gpio GPIO[4]_0 and GPIO[4]_1
For ins rem use GPIO4_0_PIN and for wp use GPIO4_1_PIN                        */
#define PINMUX10_GPIO4_MASK   0x00FFFFFFu
#define PINMUX10_GPIO4_ENABLE 0x88000000u

/*
 *  Function to configure the lcd LIDD module in the PINMUX registers and the
 *  PSC module.
 */
void configureGpio(void)
{
    CSL_SyscfgRegsOvly sysCfgRegs = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;    
    
    /* Unlock BOOTCFG module before pinmux configuration                      */
    KICK0 = KICK0_ENABLE_KEY;
    KICK1 = KICK0_ENABLE_KEY;

    /* enable the pinmux registers for GPIO                                   */
    /* enable GPIO4_2 and GPIO4_1 for CD and INS support                      */
    sysCfgRegs->PINMUX10 &= PINMUX10_GPIO4_MASK;
    sysCfgRegs->PINMUX10 |= PINMUX10_GPIO4_ENABLE;                        
   
    return;
}


/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
