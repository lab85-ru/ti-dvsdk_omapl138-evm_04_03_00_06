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
#include <iom.h>
#include <gio.h>
#include <assert.h>

#include "ti/pspiom/psc/Psc.h"
#include "ti/pspiom/i2c/I2c.h"
#include "ti/pspiom/gpio/Gpio.h"
#include "ti/pspiom/cslr/cslr_psc_C6747.h"
#include "ti/pspiom/platforms/evm6747/Mmcsd_evmInit.h"
#include "ti/pspiom/cslr/cslr_syscfg_C6747.h"

#define PROG_I2C_TIMEOUT            2000u

#define KICK0R                      (*((volatile Uint32 *)(0x01C14038u)))
#define KICK0R_MMCSD_ENABLE         0x83E70B13u
#define KICK1R                      (*((volatile Uint32 *)(0x01C1403Cu)))
#define KICK1R_MMCSD_ENABLE         0x95a4f1e0u
/* Address of the pinmux registers for mmcsd                                  */
#define PINMUX13                    (*((volatile Uint32 *)(0x01C14154u)))
#define PINMUX13_MMCSD_ENABLE       0x22000000u
#define PINMUX14                    (*((volatile Uint32 *)(0x01C14158u)))
#define PINMUX14_MMCSD_ENABLE       0x00222222u
#define PINMUX15                    (*((volatile Uint32 *)(0x01C1415Cu)))
#define PINMUX15_MMCSD_ENABLE       0x20000000u
#define PINMUX16                    (*((volatile Uint32 *)(0x01C14160u)))
#define PINMUX16_MMCSD_ENABLE       0x00000002u
/* Address of the pinmux 8 register for i2c                                   */
#define PINMUX8                     (*((volatile Uint32 *)(0x01C14140u)))
/* enable the pinmux registers for I2c                                        */
#define PINMUX8_I2C_ENABLE          0x00022000u
/* Address of the pinmux register for gpio                                    */
#define PINMUX18                    (*((volatile Uint32 *)(0x01C14168u)))
#define PINMUX18_GPIO_CD_ENABLE     0x00000008u
#define PINMUX17                    (*((volatile Uint32 *)(0x01C14164u)))
#define PINMUX17_GPIO_WP_ENABLE     0x80000000u

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
Void mmcsdPscInit(Void);

static Int32 selectI2cExpanderMmcsd(Void);

static Void configMmcsd();

static Void gpioExampleTask(PSP_mmcsdGpioInputIsr  mmcsdGpioInputIsr);

/*
 * I2C0 device params. To be filled in mmcsdUserI2cInit function which
 * is called before driver creation
 */
I2c_Params   mmcsdI2cParams;

extern const Gpio_Params Gpio_PARAMS;
/*
 * I2C0 init function called when creating the driver.
 */
Void mmcsdUserI2cInit()
{
    I2c_init();

    mmcsdI2cParams = I2c_PARAMS;
    mmcsdI2cParams.busFreq = 200000u;
    mmcsdI2cParams.hwiNumber = 8u;

    /* configure I2c for edma mode                                            */
    mmcsdI2cParams.opMode = I2c_OpMode_INTERRUPT;

    mmcsdPscInit();
}



static Int32 selectI2cExpanderMmcsd(Void)
{
    GIO_Attrs       gioAttrs = GIO_ATTRS;
    I2c_ChanParams  chanParams;
    Int             status = 340;
    Uint8           data[4] = {0};
    GIO_Handle      i2cOutHandle;              /* handle to the TX channel */
    GIO_Handle      i2cInHandle;               /* handle to the RX channel */
    I2c_DataParam   dataBuffer;

    /*
     * Initialize channel attributes.
     */
    gioAttrs.nPackets = 2;
    gioAttrs.timeout = PROG_I2C_TIMEOUT;  /*Just in case there is no UI board*/
    chanParams.hEdma = NULL;
    chanParams.masterOrSlave = I2c_CommMode_MASTER;

    /* Configure the I2C IO Expander                                          */
    i2cOutHandle = GIO_create("/I2C0",
                       IOM_OUTPUT,
                       &status,
                       &chanParams,
                       &gioAttrs);
    i2cInHandle  = GIO_create("/I2C0",
                       IOM_INPUT,
                       &status,
                       &chanParams,
                       &gioAttrs);
    assert(NULL != i2cOutHandle);
    assert(NULL != i2cInHandle);
    if (IOM_COMPLETED == status)
    {
        dataBuffer.slaveAddr = 0x3Fu;
        dataBuffer.buffer    = data;
        dataBuffer.flags     = I2c_READ | I2c_MASTER | I2c_START | I2c_STOP;
        dataBuffer.bufLen    = 1u;
        i2cInHandle->timeout = PROG_I2C_TIMEOUT;
                                    /* Just in case there is no UI board*/
        status = GIO_submit(i2cInHandle,
                     IOM_READ,
                     &dataBuffer,
                     &dataBuffer.bufLen,
                     NULL);
    }

    if (IOM_COMPLETED == status)/* If fails then Most likely no UI board      */
    {
        data[0] = data[0] & 0xBF;
        dataBuffer.flags = (I2c_WRITE | I2c_MASTER | I2c_START | I2c_STOP);
        i2cOutHandle->timeout = PROG_I2C_TIMEOUT;
                                    /* Just in case there is no UI board*/
        status = GIO_submit(i2cOutHandle,
                     IOM_WRITE,
                     &dataBuffer,
                     &dataBuffer.bufLen,
                     NULL);
    }
    GIO_delete (i2cOutHandle);
    GIO_delete (i2cInHandle);
    return status;
}

static Void configMmcsd()
{
    /*Enable write access to PINMUX and CFG registers in KICK0R and KICK1R    */
    KICK0R = KICK0R_MMCSD_ENABLE;
    KICK1R = KICK1R_MMCSD_ENABLE;

    /* enable the pinmux configuration for the MMCSD device                   */
    PINMUX13 &= (~(CSL_SYSCFG_PINMUX13_PINMUX13_27_24_MASK |
                   CSL_SYSCFG_PINMUX13_PINMUX13_31_28_MASK));
    PINMUX13 |= PINMUX13_MMCSD_ENABLE;

    PINMUX14 &= (~(CSL_SYSCFG_PINMUX14_PINMUX14_3_0_MASK |
                   CSL_SYSCFG_PINMUX14_PINMUX14_7_4_MASK |
                   CSL_SYSCFG_PINMUX14_PINMUX14_11_8_MASK |
                   CSL_SYSCFG_PINMUX14_PINMUX14_15_12_MASK |
                   CSL_SYSCFG_PINMUX14_PINMUX14_19_16_MASK |
                   CSL_SYSCFG_PINMUX14_PINMUX14_23_20_MASK));
    PINMUX14 |= PINMUX14_MMCSD_ENABLE;

    PINMUX15 &= (~(CSL_SYSCFG_PINMUX15_PINMUX15_31_28_MASK));
    PINMUX15 |= PINMUX15_MMCSD_ENABLE;

    PINMUX16 &= (~(CSL_SYSCFG_PINMUX16_PINMUX16_3_0_MASK));
    PINMUX16 |= PINMUX16_MMCSD_ENABLE;

    /* enable the pinmux registers for I2c                                    */
    PINMUX8 &= (~(CSL_SYSCFG_PINMUX8_PINMUX8_15_12_MASK |
                  CSL_SYSCFG_PINMUX8_PINMUX8_19_16_MASK));
    PINMUX8 |= PINMUX8_I2C_ENABLE;

    /* enable the pinmux registers for GPIO                                   */
    /* mux between EMA_CS5 and GPIO2_2 : enable GPIO2_2                       */
    PINMUX18 &= (~(CSL_SYSCFG_PINMUX18_PINMUX18_3_0_MASK));    
    PINMUX18 |= PINMUX18_GPIO_CD_ENABLE;
    
    /* mux between EMA_CS4 and GPIO2_1 : enable GPIO2_1                       */
    PINMUX17 &= (~(CSL_SYSCFG_PINMUX17_PINMUX17_31_28_MASK));    
    PINMUX17 |= PINMUX17_GPIO_WP_ENABLE;

}




static Void gpioExampleTask(PSP_mmcsdGpioInputIsr mmcsdGpioInputIsr)
{
    volatile Int32 statuspin2 = 0;
	Gpio_Params	 	gpioParams = Gpio_PARAMS;
    Gpio_Handle  	gpio0;
    Gpio_PinCmdArg 	pinCmdArg;
    Gpio_IntrCmdArg intrCmdArg;
    
    assert(NULL != mmcsdGpioInputIsr);

    /* update the gpio parameters to our needs */
    gpioParams.instNum = 0;
    gpioParams.BankParams[2].inUse = Gpio_InUse_No;
    gpioParams.BankParams[2].hwiNum  = 8;
    gpioParams.BankParams[2].PinConfInfo[1].inUse = Gpio_InUse_No;
    gpioParams.BankParams[2].PinConfInfo[2].inUse = Gpio_InUse_No;
    /* open the GPIO driver to get a handle to it */
    gpio0 = Gpio_open(&gpioParams);
    assert(NULL != gpio0);
    /* Configure GPIO(GPIO2_2_PIN) as an input                                */
	pinCmdArg.pin   = 35u;
	pinCmdArg.value = Gpio_Direction_Input;
	Gpio_setPinDir(gpio0, &pinCmdArg);

    /* For write protection */
    /* Configure GPIO(GPIO2_1_PIN) as an input                                */
	pinCmdArg.pin   = 34u;
	pinCmdArg.value = Gpio_Direction_Input;
	Gpio_setPinDir(gpio0, &pinCmdArg);

    /* Enable GPIO Bank interrupt for bank GPIO_BANK_2                        */
	Gpio_bankInterruptEnable(gpio0, 2u);

    /* Configure GPIO(GPIO2_2_PIN) to generate interrupt on rising edge       */
	Gpio_setRisingEdgeTrigger(gpio0,35u);

    /* Configure GPIO(GPIO2_2_PIN) to generate interrupt on falling edge      */
	Gpio_setFallingEdgeTrigger(gpio0,35u);

    /* Set the interrupt handler for GPIO2_2_PIN. However we cannot register
     * interrupts for individual pins in C6747, therefore register interrupt
     * for the associated bank(BANK0) as a whole
     */
	intrCmdArg.value = 2u;
	intrCmdArg.bankOrPin = Gpio_BankOrPin_isBank;
	intrCmdArg.isrHandler = (Gpio_Isr)mmcsdGpioInputIsr;
	Gpio_regIntHandler(gpio0, &intrCmdArg);

    /*At GPIO2_2_PIN bit value of 1 at the input reg represents that the card
    is removed and 0 means it is inserted*/
    statuspin2 = *((volatile Uint32 *)(0x01E26048u)) & 0x00000004u;
    if (0x00000004u == statuspin2)
    {
        /* Card is removed */
    }
    else
    {
        /* Card is inserted */
    }
}

Void configureMmcsd(PSP_mmcsdGpioInputIsr mmcsdGpioInputIsr)
{
    Int32 result;
    /* enable the MMCSD instance in the PSC module & Pinmux */
    configMmcsd();
    /*Configure i2c*/
    result = selectI2cExpanderMmcsd();
    if (IOM_COMPLETED != result)
    {
        /* Most likely no UI board, so just continue */
    }
    if(mmcsdGpioInputIsr != NULL)
    {
        gpioExampleTask(mmcsdGpioInputIsr);
    }
}

Void mmcsdPscInit(Void)
{
    /* Bring the GPIO module out of sleep state                               */
    Psc_ModuleClkCtrl(Psc_DevId_1, CSL_PSC_GPIO, TRUE);
}
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
