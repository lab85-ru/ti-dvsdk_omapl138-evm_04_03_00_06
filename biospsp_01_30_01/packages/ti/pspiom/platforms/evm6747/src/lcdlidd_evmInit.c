/*
 * lcdclidd_evmInit.c
 *
 * This file contains LCDC LIDD application related EVM (platform) specific 
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
 *  \file   lcdlidd_evmInit.c
 *
 *  \brief  This file contains the board specific code for enabling the use of
 *          lcd LIDD driver.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#include "std.h"
#include "log.h"
#include "iom.h"
#include "gio.h"
#include "assert.h"

#include "ti/pspiom/i2c/I2c.h"
#include "ti/pspiom/platforms/evm6747/lcdlidd_evmInit.h"
#include "ti/pspiom/cslr/cslr_syscfg_C6747.h"

static Int32 selectI2cExpanderLidd(void);
void liddUserI2cInit(void);

I2c_Params   liddI2cParams;
/*============================================================================*/
/*                          MACRO DEFINITIONS                                 */
/*============================================================================*/


/* KICK0 and KICK1 registers for lockin unlocking the pinmux register write   */
#define KICK0_REGISTER      (*(volatile Uint32 *)0x01C14038u)
#define KICK1_REGISTER      (*(volatile Uint32 *)0x01C1403Cu)

/* pinmux registers                                                           */
#define PINMUX14            (*((volatile unsigned int*)0x01C14158u))
#define PINMUX15            (*((volatile unsigned int*)0x01C1415Cu))
#define PINMUX16            (*((volatile unsigned int*)0x01C14160u))
#define PINMUX17            (*((volatile unsigned int*)0x01C14164u))

#define KICK0_ENABLE_KEY    0x83E70B13u
#define KICK1_ENABLE_KEY    0x95a4f1e0u


Int32 selectI2cExpanderLidd(void)
{
    /* I2c stream variables */
    GIO_Attrs       i2cGioAttrs;
    GIO_Handle      i2c_outHandle;
    GIO_Handle      i2c_inHandle;
    I2c_ChanParams  i2cChanParams;
    I2c_DataParam   dataBuffer;
    Uint8           data[4] = {0};
    Int             status       = 340;

    /* *** Configure the I2C IO Expander *** */
    i2cGioAttrs          = GIO_ATTRS;
    i2cGioAttrs.nPackets = 10;

    i2cChanParams.hEdma         = NULL;
    i2cChanParams.masterOrSlave = I2c_CommMode_MASTER;

    i2c_outHandle =
        GIO_create("/I2C0", IOM_OUTPUT, &status, &i2cChanParams, &i2cGioAttrs);
    i2c_inHandle  =
        GIO_create("/I2C0", IOM_INPUT, &status, &i2cChanParams, &i2cGioAttrs);

    assert(NULL != i2c_outHandle);
    assert(NULL != i2c_inHandle);

    dataBuffer.slaveAddr = 0x3Fu;
    dataBuffer.buffer    = data;
    dataBuffer.bufLen    = 1u;
    dataBuffer.flags     = I2c_READ | I2c_MASTER | I2c_START | I2c_STOP;

    status = GIO_submit(i2c_inHandle, IOM_READ, &dataBuffer, &dataBuffer.bufLen, NULL);

    data[0] &= 0xBFu;
    dataBuffer.flags     = I2c_WRITE | I2c_MASTER | I2c_START | I2c_STOP ;

    status = GIO_submit(i2c_outHandle, IOM_WRITE, &dataBuffer, &dataBuffer.bufLen, NULL);

    GIO_delete(i2c_outHandle);
    GIO_delete(i2c_inHandle);

    return 0;
}


/*
 *  Function to configure the lcd lidd module in the PINMUX registers and the
 *  PSC module.
 */
void configureLcdlidd(void)
{
    /*Enable write access to PINMUX and CFG registers in KICK0R and KICK1R    */
    KICK0_REGISTER = KICK0_ENABLE_KEY;
    KICK1_REGISTER = KICK1_ENABLE_KEY;

    /* enable pinmux for LCDC device                                          */
    PINMUX14 &= (~(CSL_SYSCFG_PINMUX14_PINMUX14_27_24_MASK |
                   CSL_SYSCFG_PINMUX14_PINMUX14_31_28_MASK));
    PINMUX14 |= 0x44000000u;

    PINMUX15 &= (~(CSL_SYSCFG_PINMUX15_PINMUX15_3_0_MASK |
                   CSL_SYSCFG_PINMUX15_PINMUX15_7_4_MASK |
                   CSL_SYSCFG_PINMUX15_PINMUX15_11_8_MASK |
                   CSL_SYSCFG_PINMUX15_PINMUX15_15_12_MASK |
                   CSL_SYSCFG_PINMUX15_PINMUX15_19_16_MASK |
                   CSL_SYSCFG_PINMUX15_PINMUX15_23_20_MASK |
                   CSL_SYSCFG_PINMUX15_PINMUX15_27_24_MASK));
    PINMUX15 |= 0x02444444u;

    PINMUX16 &= (~(CSL_SYSCFG_PINMUX16_PINMUX16_7_4_MASK |
                   CSL_SYSCFG_PINMUX16_PINMUX16_11_8_MASK |
                   CSL_SYSCFG_PINMUX16_PINMUX16_15_12_MASK |
                   CSL_SYSCFG_PINMUX16_PINMUX16_19_16_MASK |
                   CSL_SYSCFG_PINMUX16_PINMUX16_23_20_MASK |
                   CSL_SYSCFG_PINMUX16_PINMUX16_27_24_MASK |
                   CSL_SYSCFG_PINMUX16_PINMUX16_31_28_MASK));
    PINMUX16 |= 0x22222220u;

    PINMUX17 &= (~(CSL_SYSCFG_PINMUX17_PINMUX17_3_0_MASK |
                   CSL_SYSCFG_PINMUX17_PINMUX17_7_4_MASK |
                   CSL_SYSCFG_PINMUX17_PINMUX17_11_8_MASK |
                   CSL_SYSCFG_PINMUX17_PINMUX17_15_12_MASK |
                   CSL_SYSCFG_PINMUX17_PINMUX17_19_16_MASK));
    PINMUX17 |= 0x00022222u;

    selectI2cExpanderLidd();

}


/*
 * I2C0 init function called when creating the driver.
 */
void liddUserI2cInit(void)
{
    I2c_init();

    liddI2cParams = I2c_PARAMS;
    liddI2cParams.busFreq = 200000u;
    liddI2cParams.hwiNumber = 8u;

    /* configure I2c for interrupt mode */
    liddI2cParams.opMode = I2c_OpMode_INTERRUPT;
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
