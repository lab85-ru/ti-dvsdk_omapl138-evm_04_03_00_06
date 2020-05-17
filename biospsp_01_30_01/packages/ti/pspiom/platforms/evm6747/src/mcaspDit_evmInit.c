/*
 * mcaspDit_evmInit.c
 *
 * This file contains McASP DIT mode application related EVM (platform) specific
 * routines implementation. 
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
 *  \file   mcaspDit_evmInit.c
 *
 *  \brief  This file contains the board specific code for enabling the use of
 *          mcasp driver instance 2.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#include <std.h>
#include "ti/pspiom/platforms/evm6747/McaspDit_evmInit.h"
#include "ti/pspiom/cslr/cslr_syscfg_C6747.h"

/*============================================================================*/
/*                          MACRO DEFINITIONS                                 */
/*============================================================================*/


/* KICK0 and KICK1 registers for lockin unlocking the pinmux register write   */
#define KICK0_REGISTER      (*(volatile Uint32 *)0x01C14038u)
#define KICK1_REGISTER      (*(volatile Uint32 *)0x01C1403Cu)



/* pin mux registers used by the mcasp                                        */
#define PINMUX9              (*((volatile Uint32 *)0x01C14144u))
#define PINMUX11             (*((volatile Uint32 *)0x01C1414Cu))

#define KICK0_ENABLE_KEY    0x83E70B13u
#define KICK1_ENABLE_KEY    0x95a4f1e0u

/*
 * Function to enable the pinmux for the mcasp device in the soc.
 *
 */
void configureMcasp(void)
{
    /*Enable write access to PINMUX and CFG registers in KICK0R and KICK1R    */
    KICK0_REGISTER = KICK0_ENABLE_KEY;
    KICK1_REGISTER = KICK1_ENABLE_KEY;
    
    /* enable the pinmux for the mcasp device 2                               */
    PINMUX9 &= (~(CSL_SYSCFG_PINMUX9_PINMUX9_11_8_MASK));
    PINMUX9 |= 0x00000200u;

    PINMUX11 &= (~(CSL_SYSCFG_PINMUX11_PINMUX11_19_16_MASK));
    PINMUX11 |= 0x00040000u;
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
