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

#include <std.h>
#include <log.h>
#include <iom.h>
#include <gio.h>
#include <assert.h>

#include "ti/pspiom/platforms/evm6748/lcdlidd_evmInit.h"
#include "ti/pspiom/cslr/cslr_syscfg0_C6748.h"
#include "ti/pspiom/cslr/soc_C6748.h"
#include "ti/pspiom/cslr/cslr_mdio.h"
#include "ti/pspiom/cslr/cslr_gpio.h"
#include "ti/pspiom/i2c/I2c.h"


Void liddUserI2cInit();

/*============================================================================*/
/*                          LOCAL FUNCTIONS                                   */
/*============================================================================*/
static Int32 liddConfigI2cWriteReg(Uint8 slaveAddr,
                               Uint8 subAddr,
                               const Uint8 *buffer,
                               Uint32 length);
static Int32 liddConfigI2cReadReg(Uint8 slaveAddr,
                              Uint8 subAddr,
                              Uint8 *buffer,
                              Uint32 length);
static Int32 liddConfigI2cInit(String i2cDevName);
static void configLiddDisplayEnable(void);
static Int32 liddConfigLiddOutput(void);
static Int32 configI2cDeInit(void);
static void configLiddPinmux(void);


/*============================================================================*/
/*                          GLOBAL DEFINITIONS                                */
/*============================================================================*/
static GIO_Handle      i2cEvmOutHandle;           /* handle to the TX channel */
static GIO_Handle      i2cEvmInHandle;            /* handle to the RX channel */

/*
 * I2C0 device params. To be filled in liddUserI2cInit function which
 * is called before driver creation
 */
I2c_Params   liddI2cParams;
/*============================================================================*/
/*                          MACRO DEFINITIONS                                 */
/*============================================================================*/


/* KICK0 and KICK1 registers for lockin unlocking the pinmux register write   */
#define KICK0_REGISTER          (*(volatile Uint32 *)0x01C14038u)
#define KICK1_REGISTER          (*(volatile Uint32 *)0x01C1403Cu)

#define KICK0_ENABLE_KEY        0x83E70B13u
#define KICK1_ENABLE_KEY        0x95a4f1e0u

#define PINMUX4_I2C_ENABLE      0x00002200u

/* Refer to TCA6416 I2C expander datasheet for mode details                   */
#define LIDD_I2C_TCA6416_ADDR    (0x40 >> 1) /* i2c addr of TCA6416 expander  */
#define I2C_EXP_CMD_READ_PORT0   (0x00u) /* Cmd to TCA6416 to read from port0 */
#define I2C_EXP_CMD_WRITE_PORT0  (0x02u) /* Cmd to TCA6416 to write to port0  */
#define I2C_EXP_CMD_CONFIG_PORT0 (0x06u) /* Cmd to TCA6416 to configure port0 */
#define I2C_EXP_MASK_LIDD        (0xE0u)

#define BUFFER_SIZE                 4

/*
 * Function to configure the lcd lidd PINMUX    
 */
static void configLiddPinmux(void)
{
    CSL_SyscfgRegsOvly sysCfgRegs = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;    
        
    Uint32 savePinmux16 = 0;
    Uint32 savePinmux17 = 0;
    Uint32 savePinmux18 = 0;
    Uint32 savePinmux19 = 0;
    
    /*Enable write access to PINMUX and CFG registers in KICK registers       */
    KICK0_REGISTER = KICK0_ENABLE_KEY;
    KICK1_REGISTER = KICK1_ENABLE_KEY;

    /* enable pinmux for LCDC device                                          */
    savePinmux16 = (sysCfgRegs->PINMUX16 &
                   ~(CSL_SYSCFG_PINMUX16_PINMUX16_11_8_MASK  |
                     CSL_SYSCFG_PINMUX16_PINMUX16_15_12_MASK |
                     CSL_SYSCFG_PINMUX16_PINMUX16_19_16_MASK |
                     CSL_SYSCFG_PINMUX16_PINMUX16_23_20_MASK |
                     CSL_SYSCFG_PINMUX16_PINMUX16_27_24_MASK |
                     CSL_SYSCFG_PINMUX16_PINMUX16_31_28_MASK));
                    
    savePinmux17 = (sysCfgRegs->PINMUX17 & 0x00000000); /* All pins are used */
    
    savePinmux18 = (sysCfgRegs->PINMUX18 &
                   ~(CSL_SYSCFG_PINMUX18_PINMUX18_3_0_MASK   |
                     CSL_SYSCFG_PINMUX18_PINMUX18_7_4_MASK   |
                     CSL_SYSCFG_PINMUX18_PINMUX18_27_24_MASK |
                     CSL_SYSCFG_PINMUX18_PINMUX18_31_28_MASK ));
                    
    savePinmux19 = (sysCfgRegs->PINMUX19 &
                   ~(CSL_SYSCFG_PINMUX19_PINMUX19_3_0_MASK  |
                     CSL_SYSCFG_PINMUX19_PINMUX19_7_4_MASK  |
                     CSL_SYSCFG_PINMUX19_PINMUX19_27_24_MASK|
                     CSL_SYSCFG_PINMUX19_PINMUX19_31_28_MASK));
    
    sysCfgRegs->PINMUX16 = (0x22222200 | savePinmux16);
    sysCfgRegs->PINMUX17 = (0x22222222 | savePinmux17);
    sysCfgRegs->PINMUX18 = (0x22000022 | savePinmux18);
    sysCfgRegs->PINMUX19 = (0x02000022 | savePinmux19);
        
    return;    
}

/* Configure the pionmux for the I2C instance we are using                    */
void liddConfigureI2c(void)
{
   CSL_SyscfgRegsOvly sysCfgRegs = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;
    Uint32 savePinmux4 = 0;
     
    /* enable the pinmux registers for I2c                                    */
    savePinmux4 = (sysCfgRegs->PINMUX4 & 
                   ~(CSL_SYSCFG_PINMUX4_PINMUX4_11_8_MASK |
                     CSL_SYSCFG_PINMUX4_PINMUX4_15_12_MASK));

    sysCfgRegs->PINMUX4 = (PINMUX4_I2C_ENABLE | savePinmux4);

}


/*
 *  Function to configure the lcd lidd module in the PINMUX registers and the
 *  PSC module.
 */
void configureLcdlidd(void)
{
    configLiddPinmux();
    
    configLiddDisplayEnable();
    
    return;
}


/* Enable LIDD Onput via gpio expander                                        */
static Int32 liddConfigLiddOutput(Void)
{
    Uint8 i2cRegVal[2] = { 0, 0 };
    Int32 liddStatus = IOM_COMPLETED;

    liddConfigI2cInit("/I2C0");
    /* Get config Reg contents */
    liddStatus = liddConfigI2cReadReg(LIDD_I2C_TCA6416_ADDR,
                    I2C_EXP_CMD_CONFIG_PORT0,
                    &i2cRegVal[0],
                    1u);

    if (I2C_EXP_MASK_LIDD == (I2C_EXP_MASK_LIDD & i2cRegVal[0]))
    {
        i2cRegVal[0] &= ~I2C_EXP_MASK_LIDD;
        /* Set config reg to output */
        liddStatus = liddConfigI2cWriteReg(LIDD_I2C_TCA6416_ADDR,
                        I2C_EXP_CMD_CONFIG_PORT0,
                        &i2cRegVal[0],
                        1u);
    }

    /* SELA = 0, SELB = 0, SELC = 0            */
    i2cRegVal[0] = 0x0u;
    /* write port 0 contents                                                  */
    liddStatus = liddConfigI2cWriteReg(LIDD_I2C_TCA6416_ADDR,
                    I2C_EXP_CMD_WRITE_PORT0,
                    &i2cRegVal[0],
                    1u);

    configI2cDeInit();

    return liddStatus;
}


static Int32 liddConfigI2cWriteReg(Uint8 slaveAddr,
                               Uint8 subAddr,
                               const Uint8 *buffer,
                               Uint32 length)
{
    Uint8 writeBuffer[BUFFER_SIZE + 1];
    Int32 writeStatus = 340;
    I2c_DataParam writeBuf;

    /* Validate the input parameters                                      */
    assert(NULL != buffer);
    /* Write to Slave Device */
    if ((buffer != NULL) && (length <= BUFFER_SIZE))
    {
        writeBuffer[0] = subAddr;
        memcpy(&writeBuffer[1], buffer, length);

        writeBuf.buffer    = &writeBuffer[0];
        writeBuf.bufLen    = length + 1u;
        writeBuf.flags     = I2c_DEFAULT_WRITE;
        writeBuf.param     = NULL;
        writeBuf.slaveAddr = slaveAddr;
        writeStatus = GIO_submit(i2cEvmOutHandle,
                          IOM_WRITE,
                          &writeBuf,
                          &writeBuf.bufLen,
                          NULL);
    }
    return writeStatus;
}


static Int32 liddConfigI2cReadReg(Uint8 slaveAddr,
                              Uint8 subAddr,
                              Uint8 *buffer,
                              Uint32 length)
{
    Int32 readStatus = 340;
    I2c_DataParam readBuf;
    Int32 readCount = 0;
    Uint8 cmd[2];

    /* Validate the input parameters                                          */
    assert(NULL != buffer);

    cmd[0] = subAddr;
    /* Write Subaddress of Slave Device */
    for (readCount = 0; readCount < length; readCount++)
    {
        readBuf.buffer    = &cmd[0];
        readBuf.bufLen    = 1u;
        readBuf.flags     = I2c_WRITE | I2c_MASTER | I2c_START;
        readBuf.param     = NULL;
        readBuf.slaveAddr = slaveAddr;
        readStatus = GIO_submit(i2cEvmOutHandle,
                                IOM_WRITE,
                                &readBuf,
                                &readBuf.bufLen,
                                NULL);

        /* Read from Slave Device */
        if (IOM_COMPLETED == readStatus)
        {
            readBuf.buffer    = &buffer[readCount];
            readBuf.bufLen    = 1u;
            readBuf.flags     = I2c_READ | I2c_MASTER | I2c_STOP | I2c_RESTART;
            readBuf.param     = NULL;
            readBuf.slaveAddr = slaveAddr;
            readStatus = GIO_submit(i2cEvmInHandle,
                                    IOM_READ,
                                    &readBuf,
                                    &readBuf.bufLen,
                                    NULL);
        }

        if (readStatus != IOM_COMPLETED)
        {
            break;
        }

        subAddr++;
    }
    return readStatus;
}



static Int32 liddConfigI2cInit(String i2cDevName)
{
    Int32 initRetVal = IOM_COMPLETED;
    GIO_Attrs gioAttrs = GIO_ATTRS;
    I2c_ChanParams chanParams;
    Int32 initStatus = 340;

    assert(NULL != i2cDevName);

    /*
     * Initialize channel attributes.
     */
    gioAttrs.nPackets = 2;
    chanParams.hEdma = NULL;
    chanParams.masterOrSlave = I2c_CommMode_MASTER;

    /* Create the I2C Channels for the TX and RX communication                */
    i2cEvmOutHandle = GIO_create(i2cDevName,
                          IOM_OUTPUT,
                          &initStatus,
                          &chanParams,
                          &gioAttrs);

    i2cEvmInHandle = GIO_create(i2cDevName,
                         IOM_INPUT,
                         &initStatus,
                         &chanParams,
                         &gioAttrs);

    if ((NULL == i2cEvmOutHandle) || (NULL == i2cEvmInHandle))
    {
        initRetVal = IOM_EBADIO;
    }
    return initRetVal;
}


static Int32 configI2cDeInit(Void)
{
    Int32 deInitRetVal = IOM_COMPLETED;
    Int32 deInitStatus = 340;

    /* Close i2c driver and delete instance */
    deInitStatus = GIO_delete(i2cEvmOutHandle);
    if (IOM_COMPLETED != deInitStatus)
    {
        deInitRetVal = IOM_EBADIO;
    }
    else
    {
        deInitStatus = GIO_delete(i2cEvmInHandle);
        if (IOM_COMPLETED != deInitStatus)
        {
            deInitRetVal = IOM_EBADIO;
        }
    }
    return deInitRetVal;
}


/**
 *  \brief I2C init function called when creating the driver.
 */
Void liddUserI2cInit()
{
    I2c_init();

    liddI2cParams = I2c_PARAMS;
    liddI2cParams.busFreq = 200000u;
    liddI2cParams.hwiNumber = 8u;

    /* Configure I2C for interrupt mode                                       */
    liddI2cParams.opMode = I2c_OpMode_INTERRUPT;

    /* No I2C 0 PSC for C6748                                              */
}



/**
 *  \brief Enables the PINMUX for LIDD device, enables the channel path of
 *         corresponding LIDD channel and configures the clocks for LIDD channel
 */
static Void configLiddDisplayEnable(Void)
{
    /*i2c pinmux - I2C needed to configure the IO expander                    */
    liddConfigureI2c();

    /* Now configure the TCA IO expander to enable the character LCD          */
    liddConfigLiddOutput();
 
    return;
}


/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
