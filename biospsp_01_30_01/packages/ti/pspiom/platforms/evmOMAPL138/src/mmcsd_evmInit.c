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
 *  \file   mmcsd_evmInit.c
 *
 *  \brief  This file contains the board specific code for enabling the use of
 *          mmcsd driver.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#include <std.h>
#include <assert.h>

#include <pwrm.h>
#include <pwrmL138.h>

#include "ti/pspiom/gpio/Gpio.h"
#include "ti/pspiom/cslr/soc_OMAPL138.h"
#include "ti/pspiom/cslr/cslr_syscfg0_OMAPL138.h"
#include "ti/pspiom/platforms/evmOMAPL138/Mmcsd_evmInit.h"

/* Mask and value of the pinmux registers for mmcsd 0                         */
#define PINMUX10_MMCSD0_MASK         0x00000000u
#define PINMUX10_MMCSD0_ENABLE       0x22222222u
#define PINMUX11_MMCSD0_MASK         0xFFFFFF00u
#define PINMUX11_MMCSD0_ENABLE       0x00000022u

/* Mask and value of the pinmux register for gpio GPIO[4]_0 and GPIO[4]_1
For ins rem use GPIO4_0_PIN and for wp use GPIO4_1_PIN                        */
#define PINMUX10_GPIO_MMCSD0_MASK   0x00FFFFFFu
#define PINMUX10_GPIO_MMCSD0_ENABLE 0x88000000u

#define GPIO_BANK_4                 4u

#define GPIO4_0_PIN                 65u
#define GPIO4_1_PIN                 66u

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Void configMmcsd0();

static Void gpioExampleTask0(PSP_mmcsdGpioInputIsr  mmcsdGpioInputIsr);

extern const Gpio_Params Gpio_PARAMS;

static Void configMmcsd0()
{
    CSL_SyscfgRegsOvly sysCfgRegs = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;

    /*KICK0R and KICK1R registers needs to be enabled for this PINMUX setting *
     * to be successful. This needs to be ensured, by the bootloader or GEL   *
     * file, and should be done by only one entity else race conditions in    *
     * SYSCFG registers accesses could result                                 */

    /* enable the pinmux configuration for the MMCSD device                   */
    sysCfgRegs->PINMUX10 &= PINMUX10_MMCSD0_MASK;
    sysCfgRegs->PINMUX10 |= PINMUX10_MMCSD0_ENABLE;
    sysCfgRegs->PINMUX11 &= PINMUX11_MMCSD0_MASK;
    sysCfgRegs->PINMUX11 |= PINMUX11_MMCSD0_ENABLE;

    /* enable the pinmux registers for GPIO                                   */
    /* enable GPIO4_2 and GPIO4_1 for CD and INS support                      */
    sysCfgRegs->PINMUX10 &= PINMUX10_GPIO_MMCSD0_MASK;
    sysCfgRegs->PINMUX10 |= PINMUX10_GPIO_MMCSD0_ENABLE;
}


static Void gpioExampleTask0(PSP_mmcsdGpioInputIsr mmcsdGpioInputIsr)
{
    volatile Int32  statuspin2 = 0;
    Gpio_Params     gpioParams = Gpio_PARAMS;
    Gpio_Handle     gpio0;
    Gpio_PinCmdArg  pinCmdArg;
    Gpio_IntrCmdArg intrCmdArg;

    assert(NULL != mmcsdGpioInputIsr);

    /* update the gpio parameters to our needs */
    gpioParams.instNum = 0;
    gpioParams.BankParams[GPIO_BANK_4].inUse = Gpio_InUse_No;
    gpioParams.BankParams[GPIO_BANK_4].hwiNum  = 8u;
    gpioParams.BankParams[GPIO_BANK_4].PinConfInfo[0].inUse = Gpio_InUse_No;
    gpioParams.BankParams[GPIO_BANK_4].PinConfInfo[1].inUse = Gpio_InUse_No;
    /* open the GPIO driver to get a handle to it */
    gpio0 = Gpio_open(&gpioParams);
    assert(NULL != gpio0);

    /* Configure GPIO(GPIO4_0_PIN) as an input                                */
    pinCmdArg.pin   = GPIO4_0_PIN;
    pinCmdArg.value = Gpio_Direction_Input;
    Gpio_setPinDir(gpio0, &pinCmdArg);

    /* For write protection */
    /* Configure GPIO(GPIO4_1_PIN) as an input                                */
    pinCmdArg.pin   = GPIO4_1_PIN;
    pinCmdArg.value = Gpio_Direction_Input;
    Gpio_setPinDir(gpio0, &pinCmdArg);

    /* Enable GPIO Bank interrupt for bank GPIO_BANK_4                        */
    Gpio_bankInterruptEnable(gpio0, GPIO_BANK_4);

    /* Configure GPIO(GPIO4_0_PIN) to generate interrupt on rising edge       */
    Gpio_setRisingEdgeTrigger(gpio0, GPIO4_0_PIN);

    /* Configure GPIO(GPIO4_0_PIN) to generate interrupt on falling edge      */
    Gpio_setFallingEdgeTrigger(gpio0, GPIO4_0_PIN);

    /* Set the interrupt handler for GPIO4_0_PIN. However we cannot register
     * interrupts for individual pins in OMAPL138, therefore  register interrupt
     * for the associated bank(BANK4) as a whole
     */
    intrCmdArg.value = GPIO_BANK_4;
    intrCmdArg.bankOrPin = Gpio_BankOrPin_isBank;
    intrCmdArg.isrHandler = (Gpio_Isr)mmcsdGpioInputIsr;
    Gpio_regIntHandler(gpio0, &intrCmdArg);

    /*At GPIO4_0_PIN bit value of 1 at the input reg represents that the card
    is removed and 0 means it is inserted*/
}

Void configureMmcsd0(PSP_mmcsdGpioInputIsr mmcsdGpioInputIsr)
{
    /* enable the Pin mux for MMCSD instance */
    configMmcsd0();

    if(mmcsdGpioInputIsr != NULL)
    {
        gpioExampleTask0(mmcsdGpioInputIsr);
    }
}

Void mmcsdPscInit(Void)
{
    /* Bring the GPIO module out of sleep state                               */
    PWRM_setDependency((PWRM_Resource)CSL_LPSC_NUMBER_GPIO_0);
}
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
