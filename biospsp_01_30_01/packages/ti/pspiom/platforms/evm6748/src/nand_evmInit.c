/*
 * Nand_evmInit.c
 *
 * This file contains NAND application related EVM (platform) specific routines
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
 *  \file   nand_evmInit.c
 *
 *  \brief  This file contains the board specific code for enabling the use of
 *          nand driver.
 *
 */

#include <std.h>

#include "ti/pspiom/cslr/soc_C6748.h"
#include "ti/pspiom/cslr/cslr_syscfg0_C6748.h"
#include "ti/pspiom/platforms/evm6748/Nand_evmInit.h"

/*============================================================================*/
/*                          MACRO DEFINITIONS                                 */
/*============================================================================*/
#define KICK0R                     (*((volatile Uint32 *)(0x01C14038u)))
#define KICK0R_NAND_ENABLE         0x83E70B13u
#define KICK1R                     (*((volatile Uint32 *)(0x01C1403Cu)))
#define KICK1R_NAND_ENABLE         0x95a4f1e0u
/* Address of the pinmux registers for nand                                   */
#define PINMUX7_NAND_MASK          0xFF00F00Fu
#define PINMUX7_NAND_ENABLE        0x00110110u
#define PINMUX9_NAND_ENABLE        0x11111111u
#define PINMUX12_NAND_MASK         0xF00FFFFFu
#define PINMUX12_NAND_ENABLE       0x01100000u

/* enable the NAND instance in the Pinmux */
Void configureNand(Void)
{
    CSL_SyscfgRegsOvly sysCfgRegs = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;
    /*Enable write access to PINMUX and CFG registers in KICK0R and KICK1R    */
    KICK0R = KICK0R_NAND_ENABLE;
    KICK1R = KICK1R_NAND_ENABLE;

    /* enable the pinmux configuration for the NAND device                    */
    /* Enable EMA_WAIT[0], EMA_OE, EMA_WE, EMA_CS[3] and EMA_CS[2] signal     */
    sysCfgRegs->PINMUX7 &= PINMUX7_NAND_MASK;
    sysCfgRegs->PINMUX7 = PINMUX7_NAND_ENABLE;

    /* Enable EMA_D[7:0]                                                      */
    sysCfgRegs->PINMUX9 = PINMUX9_NAND_ENABLE;

    /* Enable EMA_A[1] (NAND_ALE), EMA_A[2] (NAND_CLE)                        */
    sysCfgRegs->PINMUX12 &= PINMUX12_NAND_MASK;
    sysCfgRegs->PINMUX12 |= PINMUX12_NAND_ENABLE;
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
