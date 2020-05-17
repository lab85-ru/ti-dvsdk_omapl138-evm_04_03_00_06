/*
 * upp_evmInit.c
 *
 * This file contains uPP device related EVM (platform) specific routines
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
 *  \file   upp_evmInit.c
 *
 *  \brief  This file contains the board specific code for enabling the use of
 *          uPP device.
 *
 *  (C) Copyright 2009, Texas Instruments, Inc
 *
 */
#include <std.h>
#include <gio.h>
#include <assert.h>
#include "ti/pspiom/platforms/evm6748/Upp_evmInit.h"
#include "ti/pspiom/cslr/soc_C6748.h"
#include "ti/pspiom/cslr/cslr_syscfg0_C6748.h"
#include "ti/pspiom/i2c/I2c.h"

/*============================================================================*/
/*                          MACRO DEFINITIONS                                 */
/*============================================================================*/

/* KICK0 and KICK1 registers for locking unlocking the pinmux register write  */
#define KICK0_REGISTER      (*(volatile Uint32 *)0x01C14038u)
#define KICK1_REGISTER      (*(volatile Uint32 *)0x01C1403Cu)

#define KICK0_ENABLE_KEY    0x83E70B13u
#define KICK1_ENABLE_KEY    0x95a4f1e0u

/* Pinmux configurations for the uPP and I2c                                  */
#define PINMUX4_I2C_ENABLE      0x00002200u
#define PINMUX13_UPP_ENABLE     0x44440000u
#define PINMUX14_UPP_ENABLE     0x44444400u
#define PINMUX15_UPP_ENABLE     0x44444444u
#define PINMUX16_UPP_ENABLE     0x44444444u
#define PINMUX17_UPP_ENABLE     0x44444444u
#define PINMUX18_UPP_ENABLE     0x00444444u

/* I2c device address                                                         */
#define I2C_TCA6416_ADDR        (0x40 >> 1)  /** IO expander address          */
#define I2C_CDCE913_ADDR        (0xCA >> 1)  /** External clock input         */

/** IO expander commands                                                      */
#define I2C_EXP_CMD_READ_PORT0  (0x00u)  /* Cmd to TCA6416 to read from port0 */
#define I2C_EXP_CMD_WRITE_PORT0 (0x02u)  /* Cmd to TCA6416 to write to port0  */
#define I2C_EXP_CMD_POL_PORT0   (0x04u)  /* Cmd to TCA6416 configure polarity */
#define I2C_EXP_CMD_CFG_PORT0   (0x06u)  /* Cmd to TCA6416 to configure port0 */

/* Mask to select the 3 pins required for selecting the devices to be enabled */
#define I2C_EXP_MASK_UPP        (0xE0u)

#define BUFFER_SIZE             4



#define BYTE_READ_WRITE_BIT     (0x80u)


/* ========================================================================== */
/*                          GLOBAL REFERENCES                                 */
/* ========================================================================== */
/*
 * I2C0 device params. To be filled in uppUserI2cInit function which
 * is called before driver creation
 */
I2c_Params   uppI2cParams;

/*============================================================================*/
/*                            LOCAL FUNCTIONS                                 */
/*============================================================================*/

static Void uppConfigureI2c(Void);
static Int32 uppConfigureCdce913(Void);
static Int32 uppConfigureIoExp(Void);
static Int32 uppI2cInit(String i2cDevName);
static Int32 uppI2cDeInit(Void);
static Int32 uppI2cWriteReg(Uint8        slaveAddr,
                            Uint8        regAddr,
                            const Uint8 *buffer,
                            Uint32       length);
static Int32 uppI2cReadReg(Uint8     slaveAddr,
                           Uint8     regAddr,
                           Uint8    *buffer,
                           Uint32    length);


/*============================================================================*/
/*                            LOCAL VARIABLES                                 */
/*============================================================================*/

/* channel handles for the I2c Tx and Rx channels */
static GIO_Handle i2cOutHandle = NULL;
static GIO_Handle i2cInHandle  = NULL;

/*============================================================================*/
/*                          FUNCTION DEFINTIONS                               */
/*============================================================================*/

/**
 * \brief    Function to enable the pinmux for the uPP device.It also enables
 *           the DAC on the UI daughter card so that the uPP can be interfaced
 *           to the DAC.
 *
 * \param    None
 *
 * \return   IOM_COMPLETED if sucessful in configuring the DAC
 *           IOM error code in case of an failure
 */
Int32 configureUpp(Void)
{
    Int32    status = IOM_COMPLETED;

    CSL_SyscfgRegsOvly sysCfgRegs = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;

    /* Enable write access to PINMUX and CFG registers in KICK0R and KICK1R   */
    KICK0_REGISTER = KICK0_ENABLE_KEY;
    KICK1_REGISTER = KICK1_ENABLE_KEY;

    /* configure the pinmux for the uPP device                                */
    sysCfgRegs->PINMUX13 &= ~(CSL_SYSCFG_PINMUX13_PINMUX13_19_16_MASK |
                              CSL_SYSCFG_PINMUX13_PINMUX13_23_20_MASK |
                              CSL_SYSCFG_PINMUX13_PINMUX13_27_24_MASK |
                              CSL_SYSCFG_PINMUX13_PINMUX13_31_28_MASK);
    sysCfgRegs->PINMUX14 &= ~(CSL_SYSCFG_PINMUX14_PINMUX14_3_0_MASK |
                              CSL_SYSCFG_PINMUX14_PINMUX14_11_8_MASK |
                              CSL_SYSCFG_PINMUX14_PINMUX14_15_12_MASK |
                              CSL_SYSCFG_PINMUX14_PINMUX14_19_16_MASK |
                              CSL_SYSCFG_PINMUX14_PINMUX14_23_20_MASK |
                              CSL_SYSCFG_PINMUX14_PINMUX14_27_24_MASK |
                              CSL_SYSCFG_PINMUX14_PINMUX14_31_28_MASK);

    sysCfgRegs->PINMUX15 &= ~(CSL_SYSCFG_PINMUX15_PINMUX15_3_0_MASK |
                              CSL_SYSCFG_PINMUX15_PINMUX15_7_4_MASK |
                              CSL_SYSCFG_PINMUX15_PINMUX15_11_8_MASK |
                              CSL_SYSCFG_PINMUX15_PINMUX15_15_12_MASK |
                              CSL_SYSCFG_PINMUX15_PINMUX15_19_16_MASK |
                              CSL_SYSCFG_PINMUX15_PINMUX15_23_20_MASK |
                              CSL_SYSCFG_PINMUX15_PINMUX15_27_24_MASK |
                              CSL_SYSCFG_PINMUX15_PINMUX15_31_28_MASK);

    sysCfgRegs->PINMUX16 &= ~(CSL_SYSCFG_PINMUX16_PINMUX16_3_0_MASK |
                              CSL_SYSCFG_PINMUX16_PINMUX16_7_4_MASK |
                              CSL_SYSCFG_PINMUX16_PINMUX16_11_8_MASK |
                              CSL_SYSCFG_PINMUX16_PINMUX16_15_12_MASK |
                              CSL_SYSCFG_PINMUX16_PINMUX16_19_16_MASK |
                              CSL_SYSCFG_PINMUX16_PINMUX16_23_20_MASK |
                              CSL_SYSCFG_PINMUX16_PINMUX16_27_24_MASK |
                              CSL_SYSCFG_PINMUX16_PINMUX16_31_28_MASK);

    sysCfgRegs->PINMUX17 &= ~(CSL_SYSCFG_PINMUX17_PINMUX17_3_0_MASK |
                              CSL_SYSCFG_PINMUX17_PINMUX17_7_4_MASK |
                              CSL_SYSCFG_PINMUX17_PINMUX17_11_8_MASK |
                              CSL_SYSCFG_PINMUX17_PINMUX17_15_12_MASK |
                              CSL_SYSCFG_PINMUX17_PINMUX17_19_16_MASK |
                              CSL_SYSCFG_PINMUX17_PINMUX17_23_20_MASK |
                              CSL_SYSCFG_PINMUX17_PINMUX17_27_24_MASK |
                              CSL_SYSCFG_PINMUX17_PINMUX17_31_28_MASK);

    sysCfgRegs->PINMUX18 &= ~(CSL_SYSCFG_PINMUX18_PINMUX18_3_0_MASK |
                              CSL_SYSCFG_PINMUX18_PINMUX18_7_4_MASK |
                              CSL_SYSCFG_PINMUX18_PINMUX18_11_8_MASK |
                              CSL_SYSCFG_PINMUX18_PINMUX18_15_12_MASK |
                              CSL_SYSCFG_PINMUX18_PINMUX18_19_16_MASK |
                              CSL_SYSCFG_PINMUX18_PINMUX18_23_20_MASK);

    sysCfgRegs->PINMUX13 |= PINMUX13_UPP_ENABLE;
    sysCfgRegs->PINMUX14 |= PINMUX14_UPP_ENABLE;
    sysCfgRegs->PINMUX15 |= PINMUX15_UPP_ENABLE;
    sysCfgRegs->PINMUX16 |= PINMUX16_UPP_ENABLE;
    sysCfgRegs->PINMUX17 |= PINMUX17_UPP_ENABLE;
    sysCfgRegs->PINMUX18 |= PINMUX18_UPP_ENABLE;

    return (status);
}

/**
 * \brief   I2C init function called when creating the driver.This function
 *          initializes the I2c driver. This function needs to be called
 *          before bindDev()
 *
 * \param   None
 *
 * \return  None
 */
Void uppUserI2cInit()
{
    I2c_init();

    uppI2cParams = I2c_PARAMS;
    uppI2cParams.busFreq = 200000;
    uppI2cParams.hwiNumber = 8;

    /* configure I2c for interrupt mode                                       */
    uppI2cParams.opMode = I2c_OpMode_INTERRUPT;
}

/**
 * \brief    This function configures the I2c Bus which is used to enable the
 *           DAC configured to use with the uPP.It first sets the pinmux for the
 *           I2c instance 0 followed by creating a channel for the I2c device.
 *           It then writes the appropriate mask to the IO expander to enable
 *           the DAC.
 *
 * \param    None
 *
 * \return   IOM_COMPLETED if sucessful in configuring the DAC
 *           IOM error code in case of an failure
 */
Int32 configureUppDacAdc(Void)
{
    Int32  status = IOM_COMPLETED;

    /* configure the pinumx for the I2c                                       */
    uppConfigureI2c();

    /* create the channels for the required i2c device instance               */
    status = uppI2cInit("/I2C0");

    if (IOM_COMPLETED == status)
    {
        status = uppConfigureIoExp();
    }

    /* configure the clock source for the ADC                                 */
    uppConfigureCdce913();

    /* delete the I2c channels                                                */
    uppI2cDeInit();

    return (status);
}

/**
 * \brief   This function configures the Pinmux for the I2c device instance 0.
 *
 * \param   None
 *
 * \return  None
 */
static Void uppConfigureI2c(Void)
{
    CSL_SyscfgRegsOvly   sysCfgRegs  = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;

    /* Enable write access to PINMUX and CFG registers in KICK0R and KICK1R   */
    KICK0_REGISTER = KICK0_ENABLE_KEY;
    KICK1_REGISTER = KICK1_ENABLE_KEY;

    /* enable the pinmux registers for I2c                                    */
    sysCfgRegs->PINMUX4 &= (~(CSL_SYSCFG_PINMUX4_PINMUX4_11_8_MASK |
                              CSL_SYSCFG_PINMUX4_PINMUX4_15_12_MASK));

    sysCfgRegs->PINMUX4 |= PINMUX4_I2C_ENABLE;

}

/**
 * \brief    This function creates the channel for the I2c required to
 *           communicate with the IO expander available on the UI daughter card
 *
 * \param    None
 *
 * \return   IOM_COMPLETED if sucessful in creating the channels
 *           IOM error code in case of an failure
 */
static Int32 uppI2cInit(String i2cDevName)
{
    Int32           status    = IOM_COMPLETED;
    GIO_Attrs       gioAttrs  = GIO_ATTRS;
    I2c_ChanParams  chanParams;

    assert(NULL != i2cDevName);

    /* Initialize channel attributes.                                         */
    gioAttrs.nPackets = 2;
    chanParams.hEdma = NULL;
    chanParams.masterOrSlave = I2c_CommMode_MASTER;

    /* Create the I2C Channels for the TX and RX communication                */
    i2cOutHandle = GIO_create(i2cDevName,
                       IOM_OUTPUT,
                       &status,
                       &chanParams,
                       &gioAttrs);


    i2cInHandle = GIO_create(i2cDevName,
                       IOM_INPUT,
                       &status,
                       &chanParams,
                       &gioAttrs);

    /* check if both the channels are created successfully                    */
    if ((NULL == i2cOutHandle) || (NULL == i2cInHandle))
    {
        status = IOM_EBADMODE;
    }
    return (status);
}

/**
 * \brief    This function creates the channel for the I2c required to enable
 *           the DAC in the IO expander in the UI daughter card.
 *
 * \param    None
 *
 * \return   IOM_COMPLETED if sucessful in configuring the DAC
 *           IOM error code in case of an failure
 */
static Int32 uppConfigureIoExp(Void)
{
    Uint8  i2cRegVal[2] = {0,0};
    Int32  status       = IOM_COMPLETED;

    /* read the current configuration of the TCA6416 port 0                   */
    status = uppI2cReadReg(
                 I2C_TCA6416_ADDR,
                 I2C_EXP_CMD_CFG_PORT0,
                 &i2cRegVal[0],
                 1u);

    /* set the pins 7,6,5 as outputs                                          */
    i2cRegVal[0] &= ~(I2C_EXP_MASK_UPP);

    status |= uppI2cWriteReg(
                 I2C_TCA6416_ADDR,
                 I2C_EXP_CMD_CFG_PORT0,
                 &i2cRegVal[0],
                 1u);

    /* ADC and DAC selection  SELA = 0, SELB = 0, SELC = 1                    */
    i2cRegVal[0] = 0x20;

    /* write port 0 contents                                                  */
    status |= uppI2cWriteReg(
                 I2C_TCA6416_ADDR,
                 I2C_EXP_CMD_WRITE_PORT0,
                 &i2cRegVal[0],
                 1u);

    return (status);
}


/**
 * \brief    This function configures the clock source for the ADC.
 *
 * \param    None
 *
 * \return   IOM_COMPLETED if sucessful in configuring the DAC
 *           IOM error code in case of an failure
 */
static Int32 uppConfigureCdce913(Void)
{
    Uint8  i2cRegVal[2] = {0,0};
    Int32  status       = IOM_COMPLETED;
    Uint8  opClkFreq[5] = {0x6Du, 0xF4, 0x60, 0x8A, 0x27}; /* 5 MHz */


    i2cRegVal[0] = 0x14u | BYTE_READ_WRITE_BIT;
    i2cRegVal[1] = opClkFreq[0];
    status |= uppI2cWriteReg(I2C_CDCE913_ADDR, i2cRegVal[0], &i2cRegVal[1], 1);

    i2cRegVal[0] = 0x18u | BYTE_READ_WRITE_BIT;
    i2cRegVal[1] = opClkFreq[1];
    status |= uppI2cWriteReg(I2C_CDCE913_ADDR, i2cRegVal[0], &i2cRegVal[1], 1);

    i2cRegVal[0] = 0x19u | BYTE_READ_WRITE_BIT;
    i2cRegVal[1] = opClkFreq[2];
    status |= uppI2cWriteReg(I2C_CDCE913_ADDR, i2cRegVal[0], &i2cRegVal[1], 1);

    i2cRegVal[0] = 0x1Au | BYTE_READ_WRITE_BIT;
    i2cRegVal[1] = opClkFreq[3];
    status |= uppI2cWriteReg(I2C_CDCE913_ADDR, i2cRegVal[0], &i2cRegVal[1], 1);

    i2cRegVal[0] = 0x1Bu | BYTE_READ_WRITE_BIT;
    i2cRegVal[1] = opClkFreq[4];
    status |= uppI2cWriteReg(I2C_CDCE913_ADDR, i2cRegVal[0], &i2cRegVal[1], 1);

    i2cRegVal[0] = 0x16u | BYTE_READ_WRITE_BIT;
    i2cRegVal[1] = (Uint8)(46 & 0x7F);
    status |= uppI2cWriteReg(I2C_CDCE913_ADDR, i2cRegVal[0], &i2cRegVal[1], 1);

    return (status);
}

/**
 * \brief    This function deletes the channel of the I2c device created during
 *           the intialization of the DAC
 *
 * \param    None
 *
 * \return   IOM_COMPLETED if sucessful in configuring the DAC
 *           IOM_EBADMODE  in case of an failure
 */
static Int32 uppI2cDeInit(Void)
{
    Int32 status = IOM_COMPLETED;

    /* delete the i2c driver channel                                          */
    status = GIO_delete(i2cInHandle);

    /* delete the i2c driver second channel                                   */
    status |= GIO_delete(i2cOutHandle);

    /* channel deletion has failed.Hence return error status to the caller    */
    if (IOM_COMPLETED != status)
    {
        status = IOM_EBADMODE;
    }
    return (status);
}

/**
 * \brief   Function to write using the I2c bus.
 *
 * \param   slaveAddr  [IN]   address of the slave to be addressed.
 * \param   regAddr    [IN]   register to be written to.
 * \param   buffer     [IN]   data buffer from which the data is to be written
 * \param   length     [IN]   length of the data to be written.
 *
 * \return  IOM_COMPLETED if sucessful in configuring the DAC
 *          IOM error code in case of an failure
 */
static Int32 uppI2cWriteReg(Uint8        slaveAddr,
                            Uint8        regAddr,
                            const Uint8 *buffer,
                            Uint32       length)
{
    Uint8         buf[BUFFER_SIZE + 1] = {0};
    Int32         status               = IOM_COMPLETED;
    I2c_DataParam writeBuf             = {0};

    /* Validate the input parameters                                          */
    assert(NULL != buffer);

    /* Write data to Slave Device                                             */
    if ((buffer != NULL) && (length <= BUFFER_SIZE))
    {
        buf[0] = regAddr;
        memcpy(&buf[1], buffer, length);

        writeBuf.buffer    = &buf[0];
        writeBuf.bufLen    = length + 1u;
        writeBuf.flags     = I2c_DEFAULT_WRITE;
        writeBuf.param     = NULL;
        writeBuf.slaveAddr = slaveAddr;

        status = GIO_submit(
                     i2cOutHandle,
                     IOM_WRITE,
                     &writeBuf,
                     &writeBuf.bufLen,
                     NULL);
    }
    return (status);
}

/**
 * \brief   Function to read using the I2c bus.
 *
 * \param   slaveAddr  [IN]   address of the slave to be addressed.
 * \param   regAddr    [IN]   register to be read from.
 * \param   buffer     [OUT]  data buffer to write the data to.
 * \param   length     [IN]   length of the data to be read.
 *
 * \return  IOM_COMPLETED if sucessful in configuring the DAC
 *          IOM error code in case of an failure
 */
static Int32 uppI2cReadReg(Uint8     slaveAddr,
                           Uint8     regAddr,
                           Uint8    *buffer,
                           Uint32    length)
{
    Int32         status    = IOM_COMPLETED;
    I2c_DataParam readBuf   = {0};
    Int32         readCount = 0;
    Uint8         cmd[2]    = {0,0};

    /* Validate the input parameters                                          */
    assert(NULL != buffer);

    cmd[0] = regAddr;

    /* Write register address to be read from the slave device                */
    for (readCount = 0; readCount < length; readCount++)
    {
        readBuf.buffer    = &cmd[0];
        readBuf.bufLen    = 1u;
        readBuf.flags     = I2c_WRITE | I2c_MASTER | I2c_START;
        readBuf.param     = NULL;
        readBuf.slaveAddr = slaveAddr;

        status = GIO_submit(
                     i2cOutHandle,
                     IOM_WRITE,
                     &readBuf,
                     &readBuf.bufLen,
                     NULL);

        /* Read from Slave Device                                             */
        if (IOM_COMPLETED == status)
        {
            readBuf.buffer    = &buffer[readCount];
            readBuf.bufLen    = 1u;
            readBuf.flags     = I2c_READ | I2c_MASTER | I2c_STOP | I2c_RESTART;
            readBuf.param     = NULL;
            readBuf.slaveAddr = slaveAddr;
            status = GIO_submit(i2cInHandle,
                         IOM_READ,
                         &readBuf,
                         &readBuf.bufLen,
                         NULL);
        }

        if (IOM_COMPLETED != status)
        {
            break;
        }
        regAddr++;
    }
    return (status);
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */

