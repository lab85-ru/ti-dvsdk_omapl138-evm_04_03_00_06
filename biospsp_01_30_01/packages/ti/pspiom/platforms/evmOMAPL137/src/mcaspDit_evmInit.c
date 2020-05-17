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
#include <ti/pspiom/platforms/evmOMAPL137/McaspDit_evmInit.h>
#include "ti/pspiom/cslr/cslr_syscfg_OMAPL137.h"

/*============================================================================*/
/*                          MACRO DEFINITIONS                                 */
/*============================================================================*/


/* pin mux registers used by the mcasp                                        */
#define PINMUX9              (*((volatile Uint32 *)0x01C14144u))
#define PINMUX11             (*((volatile Uint32 *)0x01C1414Cu))


/*
 * Function to enable the pinmux for the mcasp device in the soc.
 *
 */
void configureMcasp(void)
{
    /*KICK0R and KICK1R registers needs to be enabled for this PINMUX setting *
     * to be successful. This needs to be ensured, by the bootloader or GEL   *
     * file, and should be done by only one entity else race conditions in    *
     * SYSCFG registers accesses could result                                 */

    /* enable the pinmux for the mcasp device 2   */
    PINMUX9   &= (~CSL_SYSCFG_PINMUX9_PINMUX9_11_8_MASK);
    PINMUX9   |= 0x00000200u;
    
    PINMUX11  &= (~CSL_SYSCFG_PINMUX11_PINMUX11_19_16_MASK);
    PINMUX11  |= 0x00040000u;

}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
