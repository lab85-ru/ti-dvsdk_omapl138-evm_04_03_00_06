/*
 * lcdcraster_evmInit.c
 *
 * This file contains LCDC Raster application related EVM (platform) specific 
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
 *  \file   lcdraster_evmInit.c
 *
 *  \brief  This file contains the board specific code for enabling the use of
 *          lcd raster driver.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#include <std.h>
#include <gio.h>
#include <log.h>
#include <iom.h>
#include <assert.h>
#include <pwrm.h>
#include <pwrm6748.h>

extern LOG_Obj trace;

#include "ti/pspiom/platforms/evm6748/lcdraster_evmInit.h"
#include "ti/pspiom/gpio/Gpio.h"
#include "ti/pspiom/cslr/cslr_syscfg0_C6748.h"
#include "ti/pspiom/cslr/cslr_gpio.h"
#include "ti/pspiom/cslr/soc_C6748.h"

CSL_SyscfgRegsOvly rasterSysCfgRegs = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;


/*============================================================================*/
/*                          LOCAL FUNCTIONS                                   */
/*============================================================================*/
static void configRasterPinmux(void);
static void configRasterGpioPins(void);
static void configRasterDisplayEnable(void);
static void rasterDisplayDelay(Uint32 delayCount);

/*============================================================================*/
/*                          MACRO DEFINITIONS                                 */
/*============================================================================*/


/* KICK0 and KICK1 registers for lockin unlocking the pinmux register write   */
#define KICK0_REGISTER      (*(volatile Uint32 *)0x01C14038u)
#define KICK1_REGISTER      (*(volatile Uint32 *)0x01C1403Cu)

#define KICK0_ENABLE_KEY    0x83E70B13u
#define KICK1_ENABLE_KEY    0x95a4f1e0u

#define GPIO_BANK_2         2
#define GPIO_BANK_8         8
#define GPIO2_8_PIN         41
#define GPIO2_15_PIN        48

/*============================================================================*/
/*                          CLOBAL DEFINITIONS                                */
/*============================================================================*/

static Gpio_Handle gpio0;

/*
 * Function to configure the lcd raster PINMUX    
 */
static void configRasterPinmux(void)
{
    Uint32 savePinmux16 = 0;
    Uint32 savePinmux17 = 0;
    Uint32 savePinmux18 = 0;
    Uint32 savePinmux19 = 0;
    
    /*Enable write access to PINMUX and CFG registers in KICK0R and KICK1R    */
    KICK0_REGISTER = KICK0_ENABLE_KEY;
    KICK1_REGISTER = KICK1_ENABLE_KEY;

    /* enable pinmux for LCDC device                                          */
    savePinmux16 = (rasterSysCfgRegs->PINMUX16 &
                   ~(CSL_SYSCFG_PINMUX16_PINMUX16_11_8_MASK  |
                     CSL_SYSCFG_PINMUX16_PINMUX16_15_12_MASK |
                     CSL_SYSCFG_PINMUX16_PINMUX16_19_16_MASK |
                     CSL_SYSCFG_PINMUX16_PINMUX16_23_20_MASK |
                     CSL_SYSCFG_PINMUX16_PINMUX16_27_24_MASK |
                     CSL_SYSCFG_PINMUX16_PINMUX16_31_28_MASK));
                    
    savePinmux17 = (rasterSysCfgRegs->PINMUX17 & 0x00000000); /* All pins are used */
    
    savePinmux18 = (rasterSysCfgRegs->PINMUX18 &
                   ~(CSL_SYSCFG_PINMUX18_PINMUX18_3_0_MASK   |
                     CSL_SYSCFG_PINMUX18_PINMUX18_7_4_MASK   |
                     CSL_SYSCFG_PINMUX18_PINMUX18_27_24_MASK |
                     CSL_SYSCFG_PINMUX18_PINMUX18_31_28_MASK));
                    
    savePinmux19 = (rasterSysCfgRegs->PINMUX19 &
                   ~(CSL_SYSCFG_PINMUX19_PINMUX19_3_0_MASK   |
                     CSL_SYSCFG_PINMUX19_PINMUX19_7_4_MASK   |
                     CSL_SYSCFG_PINMUX19_PINMUX19_27_24_MASK |
                     CSL_SYSCFG_PINMUX19_PINMUX19_31_28_MASK ));
    
    rasterSysCfgRegs->PINMUX16 = (0x22222200 | savePinmux16);
    rasterSysCfgRegs->PINMUX17 = (0x22222222 | savePinmux17);
    rasterSysCfgRegs->PINMUX18 = (0x22000022 | savePinmux18);
    rasterSysCfgRegs->PINMUX19 = (0x02000022 | savePinmux19);    
}


/*
 *  Function to enable display panel power and the backlight for the LCD display
 *  panel                                                              
 */
static void configRasterDisplayEnable(void)
{
    Gpio_PinCmdArg pinCmdArg;
    
    /* Configure the GPIO pins used for backlight and power of display panel  */
    configRasterGpioPins();
    
    /* 1. Disable power     */
    pinCmdArg.pin = GPIO2_8_PIN;
    pinCmdArg.value = 0;
    Gpio_setPinVal(gpio0,&pinCmdArg);
    
    rasterDisplayDelay(0xFFFF);        
    rasterDisplayDelay(0xFFFF);            
    
    /* 2. Disable backlight */
    pinCmdArg.pin = GPIO2_15_PIN;
    pinCmdArg.value = 0;
    Gpio_setPinVal(gpio0,&pinCmdArg);

    rasterDisplayDelay(0xFFFF);    
    rasterDisplayDelay(0xFFFF);
        
    /* 3. Enable power      */
    pinCmdArg.pin = GPIO2_8_PIN;
    pinCmdArg.value = 1;
    Gpio_setPinVal(gpio0,&pinCmdArg);

    rasterDisplayDelay(0xFFFF);        
    rasterDisplayDelay(0xFFFF);
    
    /* 4. Enable backlight  */
    pinCmdArg.pin = GPIO2_15_PIN;
    pinCmdArg.value = 1;
    Gpio_setPinVal(gpio0,&pinCmdArg);

    rasterDisplayDelay(0xFFFF);        
    rasterDisplayDelay(0xFFFF);
                
    return;
}

static void configRasterGpioPins(void)
{
    Gpio_Params         gpioParams = Gpio_PARAMS;
    Gpio_PinCmdArg      pinCmdArg;
    Uint32              savePinmux5 = 0, savePinmux18 = 0;
    Int32               status  = PWRM_SOK;
    
    /* Setup GPIO Pinmuxes                                                    */    
    savePinmux5 = (rasterSysCfgRegs->PINMUX5 &
                   ~(CSL_SYSCFG_PINMUX5_PINMUX5_3_0_MASK));
                   
    savePinmux18 = (rasterSysCfgRegs->PINMUX18 &
                   ~(CSL_SYSCFG_PINMUX18_PINMUX18_31_28_MASK));                   
    
                   
    savePinmux5 |= (CSL_SYSCFG_PINMUX5_PINMUX5_3_0_GPIO2_15 <<
                    CSL_SYSCFG_PINMUX5_PINMUX5_3_0_SHIFT);                   
                   
    savePinmux18 |= (CSL_SYSCFG_PINMUX18_PINMUX18_31_28_GPIO8_10 <<
                    CSL_SYSCFG_PINMUX18_PINMUX18_31_28_SHIFT);
    
    rasterSysCfgRegs->PINMUX5 = savePinmux5;
    rasterSysCfgRegs->PINMUX18 = savePinmux18;
    
    /* update the gpio parameters to our needs */
    gpioParams.instNum = 0;
    
    /*Configure GP2[15] for display panel backlight                           */
    gpioParams.BankParams[GPIO_BANK_2].inUse = Gpio_InUse_No;
    gpioParams.BankParams[GPIO_BANK_2].hwiNum  = -1; /* No interrupts required*/
    gpioParams.BankParams[GPIO_BANK_2].PinConfInfo[15].inUse = Gpio_InUse_No;
    
    /*Configure GP8[10] for display panel power                               */
    gpioParams.BankParams[GPIO_BANK_8].inUse = Gpio_InUse_No;
    gpioParams.BankParams[GPIO_BANK_8].hwiNum  = -1; /* No interrupts required*/
    gpioParams.BankParams[GPIO_BANK_8].PinConfInfo[10].inUse = Gpio_InUse_No;

    /* open the GPIO driver to get a handle to it */
    gpio0 = Gpio_open(&gpioParams);
    
    /* power on the GPIO device in the Power sleep controller                 */
    /* power on using bios PWRM API                                           */
    status = PWRM_setDependency(PWRM_RSRC_GPIO);
    
    if(PWRM_SOK != status)
    {
        LOG_printf(&trace,"Error pwering up GPIO module");
    }
    
    /* Configure GPIO(GPIO2_15_PIN) as an input                               */
    pinCmdArg.pin   = GPIO2_15_PIN;
    pinCmdArg.value = Gpio_Direction_Output;
    Gpio_setPinDir(gpio0, &pinCmdArg);    
    
    /* Configure GPIO(GPIO2_8_PIN) as an input                               */
    pinCmdArg.pin   = GPIO2_8_PIN;
    pinCmdArg.value = Gpio_Direction_Output;
    Gpio_setPinDir(gpio0, &pinCmdArg);        
}


/* A generic delay function */
static void rasterDisplayDelay(Uint32 delayCount)
{
    volatile Uint32 i = 0;

    for(i=0; i<delayCount; i++)

	return;
}


/*
 *  Function to configure the lcd raster module in the PINMUX registers and any
 *  specific platform functions to enable the raster display on the EVM
 */
void configureLcdraster(void)
{
    configRasterPinmux();
    
    configRasterDisplayEnable();
    
    return;
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
