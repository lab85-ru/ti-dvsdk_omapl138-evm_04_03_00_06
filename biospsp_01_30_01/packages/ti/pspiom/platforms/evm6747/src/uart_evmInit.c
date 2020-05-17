/*
 * uart_evmInit.c
 *
 * This file contains UART application related EVM (platform) specific routines
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
 *  \file   uart_evmInit.c
 *
 *  \brief  This file contains the board specific code for enabling the use of
 *          uart driver.
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

#include "ti/pspiom/uart/Uart.h"
#include "ti/pspiom/platforms/evm6747/Uart_evmInit.h"
#include "ti/pspiom/cslr/cslr_syscfg_C6747.h"

/* Address of the pinmux 8 & 9 register                                       */
#define PINMUX8     (*((volatile Uint32 *)(0x01C14140u)))
#define PINMUX9     (*((volatile Uint32 *)(0x01C14144u)))

#define PINMUX8_UART_ENABLE       0x20000000u
#define PINMUX9_UART_ENABLE       0x00000002u

/* Address of the KICK0 and KICK1 register                                    */
#define KICK0                   (*((volatile Uint32 *)(0x01C14038u)))     
#define KICK1                   (*((volatile Uint32 *)(0x01C1403Cu)))

/* Unlock/Lock code for KICK0 and KICK1 register                              */
#define KICK0_UNLOCK_CODE       0x83E70B13u
#define KICK1_UNLOCK_CODE       0x95A4F1E0u


void configureUart(void)
{
    /* Unlock BOOTCFG module before pinmux configuration                      */
    KICK0 = KICK0_UNLOCK_CODE;
    KICK1 = KICK1_UNLOCK_CODE;

    /* enable the pinmux registers for Uart                                   */
    PINMUX8 &= (~(CSL_SYSCFG_PINMUX8_PINMUX8_31_28_MASK));    
    PINMUX8 |= PINMUX8_UART_ENABLE;

    PINMUX9 &= (~(CSL_SYSCFG_PINMUX9_PINMUX9_3_0_MASK));
    PINMUX9 |= PINMUX9_UART_ENABLE;
}
