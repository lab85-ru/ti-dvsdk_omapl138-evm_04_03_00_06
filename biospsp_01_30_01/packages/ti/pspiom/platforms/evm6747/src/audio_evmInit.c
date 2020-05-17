/*
 * audio_evmInit.c
 *
 * This file contains Audio application related EVM (platform) specific routines
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
 *  \file   audio_evmInit.c
 *
 *  \brief  This file contains the board specific code for enabling the use of
 *          audio driver.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#include <std.h>
#include "ti/pspiom/i2c/I2c.h"
#include "ti/pspiom/platforms/evm6747/Audio_evmInit.h"
#include "ti/pspiom/cslr/cslr_syscfg_C6747.h"

/*============================================================================*/
/*                          MACRO DEFINITIONS                                 */
/*============================================================================*/


/* KICK0 and KICK1 registers for lockin unlocking the pinmux register write   */
#define KICK0_REGISTER      (*(volatile Uint32 *)0x01C14038u)
#define KICK1_REGISTER      (*(volatile Uint32 *)0x01C1403Cu)



/* pin mux registers used by the mcasp and i2c devices                        */
#define PINMUX8              (*((volatile Uint32 *)0x01C14140u))
#define PINMUX11             (*((volatile Uint32 *)0x01C1414Cu))
#define PINMUX12             (*((volatile Uint32 *)0x01C14150u))
#define PINMUX13             (*((volatile Uint32 *)0x01C14154u))



#define KICK0_ENABLE_KEY    0x83E70B13u
#define KICK1_ENABLE_KEY    0x95a4f1e0u


/*
 * I2C device params. To be filled in userI2cInit function which
 * is called before driver creation
 */
I2c_Params audioI2cParams;

/*
 * Function to enable the pinmux for the mcasp and i2c devices in the soc.
 *
 */
void configureAudio(void)
{
    /*Enable write access to PINMUX and CFG registers in KICK0R and KICK1R    */
    KICK0_REGISTER = KICK0_ENABLE_KEY;
    KICK1_REGISTER = KICK1_ENABLE_KEY;
    
    /* write to the pinmux registers to enable the mcasp1 and i2c0            */
    PINMUX8 &= (~(CSL_SYSCFG_PINMUX8_PINMUX8_15_12_MASK |
                  CSL_SYSCFG_PINMUX8_PINMUX8_19_16_MASK |
                  CSL_SYSCFG_PINMUX8_PINMUX8_23_20_MASK |
                  CSL_SYSCFG_PINMUX8_PINMUX8_27_24_MASK));
    PINMUX8 |= 0x01122000u;  /* Mcasp1 and I2c0 */

    PINMUX11 &= (~(CSL_SYSCFG_PINMUX11_PINMUX11_23_20_MASK |
                   CSL_SYSCFG_PINMUX11_PINMUX11_27_24_MASK |
                   CSL_SYSCFG_PINMUX11_PINMUX11_31_28_MASK));
    PINMUX11 |= 0x11100000u;  /* Mcasp1          */

    PINMUX12 &= (~(CSL_SYSCFG_PINMUX12_PINMUX12_3_0_MASK |
                   CSL_SYSCFG_PINMUX12_PINMUX12_7_4_MASK |
                   CSL_SYSCFG_PINMUX12_PINMUX12_11_8_MASK |
                   CSL_SYSCFG_PINMUX12_PINMUX12_15_12_MASK |
                   CSL_SYSCFG_PINMUX12_PINMUX12_19_16_MASK |
                   CSL_SYSCFG_PINMUX12_PINMUX12_23_20_MASK |
                   CSL_SYSCFG_PINMUX12_PINMUX12_27_24_MASK |
                   CSL_SYSCFG_PINMUX12_PINMUX12_31_28_MASK));
    PINMUX12 |= 0x11111111u;  /* Mcasp1          */

    PINMUX13 &= (~(CSL_SYSCFG_PINMUX13_PINMUX13_3_0_MASK |
                   CSL_SYSCFG_PINMUX13_PINMUX13_7_4_MASK |
                   CSL_SYSCFG_PINMUX13_PINMUX13_11_8_MASK |
                   CSL_SYSCFG_PINMUX13_PINMUX13_15_12_MASK |
                   CSL_SYSCFG_PINMUX13_PINMUX13_19_16_MASK |
                   CSL_SYSCFG_PINMUX13_PINMUX13_23_20_MASK));

    PINMUX13 |= 0x00111111u;  /* Mcasp1          */
}


/*
 * I2c init function called when creating the driver.
 */
void audioUserI2cInit()
{
    I2c_init();
    audioI2cParams = I2c_PARAMS;
    audioI2cParams.hwiNumber = 8u;
    audioI2cParams.opMode = I2c_OpMode_INTERRUPT;
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
