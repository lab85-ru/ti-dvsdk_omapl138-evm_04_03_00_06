/*
 * i2c_evmInit.c
 *
 * This file contains I2C application related EVM (platform) specific routines
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
 *  \file   i2c_evmInit.c
 *
 *  \brief  This file contains the board specific code for enabling the use of
 *          i2c driver.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#include <std.h>
#include <iom.h>
#include <ecm.h>
#include <gio.h>
#include <log.h>
#include <c64.h>
#include <hwi.h>

#include <ti/pspiom/i2c/I2c.h>

#include <ti/pspiom/platforms/evmOMAPL137/I2c_evmInit.h>
#include "ti/pspiom/cslr/cslr_syscfg_OMAPL137.h"


/* Address of the pinmux 8 register                                           */
#define PINMUX8                 (*((volatile Uint32 *)(0x01C14140u)))
#define PINMUX8_I2C_ENABLE      0x00022000u



void configureI2c(void)
{
    /*KICK0R and KICK1R registers needs to be enabled for this PINMUX setting *
     * to be successful. This needs to be ensured, by the bootloader or GEL   *
     * file, and should be done by only one entity else race conditions in    *
     * SYSCFG registers accesses could result                                 */

    /* enable the pinmux registers for I2c  */
    PINMUX8 &= (~(CSL_SYSCFG_PINMUX8_PINMUX8_15_12_MASK |
                  CSL_SYSCFG_PINMUX8_PINMUX8_19_16_MASK));
    PINMUX8 |= PINMUX8_I2C_ENABLE;

}
