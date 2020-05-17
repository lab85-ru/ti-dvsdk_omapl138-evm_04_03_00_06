/*
 * vpif_evmInit.c
 *
 * This file contains VPIF application related EVM (platform) specific routines
 * implementation. This is part of the EVM Initialization library (evmInit)
 * which contains pinmux enabling routines for peripherals used by the
 * application, and may contain related device pre-driver initialization
 * routines.
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
 *  \file   vpif_evmInit.c
 *
 *  \brief  This file contains the board specific code for enabling the use of
 *          vpif driver.
 *
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include <iom.h>
#include <gio.h>
#include <assert.h>

#include <ti/pspiom/cslr/soc_C6748.h>
#include <ti/pspiom/cslr/cslr_syscfg0_C6748.h>

#include <ti/pspiom/i2c/I2c.h>
#include <ti/pspiom/platforms/evm6748/I2c_evmInit.h>
#include <ti/pspiom/vpif/VpifDebug.h>

#include "ti/pspiom/platforms/evm6748/Vpif_evmInit.h"

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */
#define KICK0R                      (*((volatile Uint32 *)(0x01C14038u)))
#define KICK0R_VPIF_ENABLE          0x83E70B13u
#define KICK1R                      (*((volatile Uint32 *)(0x01C1403Cu)))
#define KICK1R_VPIF_ENABLE          0x95a4f1e0u

/* Mask and value of the pinmux registers for vpif 0                         */
#define PINMUX14_VPIF0_CH0_MASK                 0x000000F0u
#define PINMUX14_VPIF0_CH0_ENABLE               0x11111101u
#define PINMUX15_VPIF0_CH0_MASK                 0xFFFFFF00u
#define PINMUX15_VPIF0_CH0_ENABLE               0x00000011u

#define PINMUX14_VPIF0_CH1_MASK                 0xFFFFFF0Fu
#define PINMUX14_VPIF0_CH1_ENABLE               0x00000010u
#define PINMUX15_VPIF0_CH1_MASK                 0x000000FFu
#define PINMUX15_VPIF0_CH1_ENABLE               0x11111100u
#define PINMUX16_VPIF0_CH1_MASK                 0xFFFFFF00u
#define PINMUX16_VPIF0_CH1_ENABLE               0x00000011u

#define PINMUX16_VPIF0_CH2_MASK                 0x000000FFu
#define PINMUX16_VPIF0_CH2_ENABLE               0x11111100u
#define PINMUX17_VPIF0_CH2_MASK                 0xFFFFFF00u
#define PINMUX17_VPIF0_CH2_ENABLE               0x00000011u
#define PINMUX19_VPIF0_CH2_MASK                 0xFFFF00FFu
#define PINMUX19_VPIF0_CH2_ENABLE               0x00001100u

#define PINMUX17_VPIF0_CH3_MASK                 0x000000FFu
#define PINMUX17_VPIF0_CH3_ENABLE               0x11111100u
#define PINMUX18_VPIF0_CH3_MASK                 0xFFFFFF00u
#define PINMUX18_VPIF0_CH3_ENABLE               0x00000011u
#define PINMUX19_VPIF0_CH3_MASK                 0xFF00FFFFu
#define PINMUX19_VPIF0_CH3_ENABLE               0x00110000u

#define PINMUX14_VPIF0_BOTH_CAP_MASK            0x00000000u
#define PINMUX14_VPIF0_BOTH_CAP_ENABLE          0x11111111u
#define PINMUX15_VPIF0_BOTH_CAP_MASK            0x00000000u
#define PINMUX15_VPIF0_BOTH_CAP_ENABLE          0x11111111u
#define PINMUX16_VPIF0_BOTH_CAP_MASK            0xFFFFFF00u
#define PINMUX16_VPIF0_BOTH_CAP_ENABLE          0x00000011u

#define PINMUX16_VPIF0_BOTH_DISP_MASK           0x000000FFu
#define PINMUX16_VPIF0_BOTH_DISP_ENABLE         0x11111100u
#define PINMUX17_VPIF0_BOTH_DISP_MASK           0x00000000u
#define PINMUX17_VPIF0_BOTH_DISP_ENABLE         0x11111111u
#define PINMUX18_VPIF0_BOTH_DISP_MASK           0xFFFFFF00u
#define PINMUX18_VPIF0_BOTH_DISP_ENABLE         0x00000011u
#define PINMUX19_VPIF0_BOTH_DISP_MASK           0xFF0000FFu
#define PINMUX19_VPIF0_BOTH_DISP_ENABLE         0x00111100u

#define I2C_CDCE913_ADDR            (0xCA >> 1)

/* Refer to TCA6416 I2C expander datasheet for mode details                   */
 /* i2c address of TCA6416 expander  */
#define I2C_TCA6416_ADDR            (0x40 >> 1)
 /* Cmd to TCA6416 to read from port0*/
#define I2C_EXP_CMD_READ_PORT0      (0x00u)
 /* Cmd to TCA6416 to write to port0*/
#define I2C_EXP_CMD_WRITE_PORT0     (0x02u)
 /* Cmd to TCA6416 to configure port0*/
#define I2C_EXP_CMD_CONFIG_PORT0    (0x06u)
#define I2C_EXP_MASK_VIDEO          (0xE0u)

#define BYTE_READ_WRITE_BIT         (0x80u)

/* ========================================================================== */
/*                          GLOBAL REFERENCES                                 */
/* ========================================================================== */
/*
 * I2C0 device params. To be filled in vpifUserI2cInit function which
 * is called before driver creation
 */
I2c_Params   vpifI2cParams;

/* ========================================================================== */
/*                          STATIC VARIABLES                                  */
/* ========================================================================== */
static GIO_Handle      i2cEvmOutHandle;           /* handle to the TX channel */
static GIO_Handle      i2cEvmInHandle;            /* handle to the RX channel */

/* ========================================================================== */
/*                       LOCAL FUNCTION PROTOTYPES                            */
/* ========================================================================== */
static Void vpifConfig(EvmInit_VpifChannel channelNo);

static Int32 vpifConfigVideoInput(Void);

static Int32 vpifConfigCameraInput(Void);

static Int32 vpifConfigOutputClock();

static Int32 vpifConfigI2cWriteReg(Uint8 slaveAddr,
                                   Uint8 subAddr,
                                   const Uint8 *buffer);

static Int32 vpifConfigI2cReadReg(Uint8 slaveAddr,
                                  Uint8 subAddr,
                                  Uint8 *buffer);

static Int32 vpifConfigI2cInit(String i2cDevName);

static Int32 vpifConfigI2cDeInit(Void);

/* ========================================================================== */
/*                       GLOBAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
Void vpifUserI2cInit();

/* ========================================================================== */
/*                        LOCAL FUNCTION DEFINITIONS                          */
/* ========================================================================== */
/**
 *  \brief Configures PINMUX for VPIF device
 *
 *  \param NONE
 *
 *  \return  NONE
 */
static Void vpifConfig(EvmInit_VpifChannel channelNo)
{
    CSL_SyscfgRegsOvly vpifSysCfgRegs = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;

    assert(NULL != vpifSysCfgRegs);

    /*Enable write access to PINMUX and CFG registers in KICK0R and KICK1R    */
    KICK0R = KICK0R_VPIF_ENABLE;
    KICK1R = KICK1R_VPIF_ENABLE;

    /* enable the pinmux configuration for the VPIF device                    */
    switch (channelNo)
    {
        case EvmInit_VpifChannel_0:
            vpifSysCfgRegs->PINMUX14 &= PINMUX14_VPIF0_CH0_MASK;
            vpifSysCfgRegs->PINMUX14 |= PINMUX14_VPIF0_CH0_ENABLE;
            vpifSysCfgRegs->PINMUX15 &= PINMUX15_VPIF0_CH0_MASK;
            vpifSysCfgRegs->PINMUX15 |= PINMUX15_VPIF0_CH0_ENABLE;
            break;

        case EvmInit_VpifChannel_1:
            vpifSysCfgRegs->PINMUX14 &= PINMUX14_VPIF0_CH1_MASK;
            vpifSysCfgRegs->PINMUX14 |= PINMUX14_VPIF0_CH1_ENABLE;
            vpifSysCfgRegs->PINMUX15 &= PINMUX15_VPIF0_CH1_MASK;
            vpifSysCfgRegs->PINMUX15 |= PINMUX15_VPIF0_CH1_ENABLE;
            vpifSysCfgRegs->PINMUX16 &= PINMUX16_VPIF0_CH1_MASK;
            vpifSysCfgRegs->PINMUX16 |= PINMUX16_VPIF0_CH1_ENABLE;
            break;

        case EvmInit_VpifChannel_2:
            vpifSysCfgRegs->PINMUX16 &= PINMUX16_VPIF0_CH2_MASK;
            vpifSysCfgRegs->PINMUX16 |= PINMUX16_VPIF0_CH2_ENABLE;
            vpifSysCfgRegs->PINMUX17 &= PINMUX17_VPIF0_CH2_MASK;
            vpifSysCfgRegs->PINMUX17 |= PINMUX17_VPIF0_CH2_ENABLE;
            vpifSysCfgRegs->PINMUX19 &= PINMUX19_VPIF0_CH2_MASK;
            vpifSysCfgRegs->PINMUX19 |= PINMUX19_VPIF0_CH2_ENABLE;
            break;

        case EvmInit_VpifChannel_3:
            vpifSysCfgRegs->PINMUX17 &= PINMUX17_VPIF0_CH3_MASK;
            vpifSysCfgRegs->PINMUX17 |= PINMUX17_VPIF0_CH3_ENABLE;
            vpifSysCfgRegs->PINMUX18 &= PINMUX18_VPIF0_CH3_MASK;
            vpifSysCfgRegs->PINMUX18 |= PINMUX18_VPIF0_CH3_ENABLE;
            vpifSysCfgRegs->PINMUX19 &= PINMUX19_VPIF0_CH3_MASK;
            vpifSysCfgRegs->PINMUX19 |= PINMUX19_VPIF0_CH3_ENABLE;
            break;

        case EvmInit_VpifChannel_BOTHCAPCH:
            vpifSysCfgRegs->PINMUX14 &= PINMUX14_VPIF0_BOTH_CAP_MASK;
            vpifSysCfgRegs->PINMUX14 |= PINMUX14_VPIF0_BOTH_CAP_ENABLE;
            vpifSysCfgRegs->PINMUX15 &= PINMUX15_VPIF0_BOTH_CAP_MASK;
            vpifSysCfgRegs->PINMUX15 |= PINMUX15_VPIF0_BOTH_CAP_ENABLE;
            vpifSysCfgRegs->PINMUX16 &= PINMUX16_VPIF0_BOTH_CAP_MASK;
            vpifSysCfgRegs->PINMUX16 |= PINMUX16_VPIF0_BOTH_CAP_ENABLE;
            break;

        case EvmInit_VpifChannel_BOTHDISPCH:
            vpifSysCfgRegs->PINMUX16 &= PINMUX16_VPIF0_BOTH_DISP_MASK;
            vpifSysCfgRegs->PINMUX16 |= PINMUX16_VPIF0_BOTH_DISP_ENABLE;
            vpifSysCfgRegs->PINMUX17 &= PINMUX17_VPIF0_BOTH_DISP_MASK;
            vpifSysCfgRegs->PINMUX17 |= PINMUX17_VPIF0_BOTH_DISP_ENABLE;
            vpifSysCfgRegs->PINMUX18 &= PINMUX18_VPIF0_BOTH_DISP_MASK;
            vpifSysCfgRegs->PINMUX18 |= PINMUX18_VPIF0_BOTH_DISP_ENABLE;
            vpifSysCfgRegs->PINMUX19 &= PINMUX19_VPIF0_BOTH_DISP_MASK;
            vpifSysCfgRegs->PINMUX19 |= PINMUX19_VPIF0_BOTH_DISP_ENABLE;
            break;

        default:
            DBG_PRINT_ERR("EvmInit : Invalid case\r\n");
            break;
    }
}



/* Enable Video Input - enable video (active low) via gpio expander */
static Int32 vpifConfigVideoInput(Void)
{
    Uint8 vidRegVal[2] = { 0, 0 };
    Int32 vidStatus = IOM_COMPLETED;

    vpifConfigI2cInit("/I2C0");
    /* Get config Reg contents */
    vidStatus = vpifConfigI2cReadReg(I2C_TCA6416_ADDR,
                    I2C_EXP_CMD_CONFIG_PORT0,
                    &vidRegVal[0]);
    VPIF_DEBUG1("EvmInit : vpifConfigI2cReadReg Status: %d\n",vidStatus);

    vidRegVal[0] &= ~I2C_EXP_MASK_VIDEO;
    /* Set config reg to output */
    vidStatus = vpifConfigI2cWriteReg(I2C_TCA6416_ADDR,
                    I2C_EXP_CMD_CONFIG_PORT0,
                    &vidRegVal[0]);
    VPIF_DEBUG1("EvmInit : vpifConfigI2cWriteReg Status: %d\n",vidStatus);

    vidStatus = vpifConfigI2cReadReg(I2C_TCA6416_ADDR,
                    I2C_EXP_CMD_WRITE_PORT0,
                    &vidRegVal[0]);
    VPIF_DEBUG1("EvmInit : vpifConfigI2cReadReg Status: %d\n",vidStatus);

    /* UI_SELA = 1, UI_SELB = 1, UI_SELC = 0 */
    vidRegVal[0] &= ~I2C_EXP_MASK_VIDEO;
    vidRegVal[0] |= 0xC0u;
    /* write port 0 contents */
    vidStatus = vpifConfigI2cWriteReg(I2C_TCA6416_ADDR,
                    I2C_EXP_CMD_WRITE_PORT0,
                    &vidRegVal[0]);
    VPIF_DEBUG1("EvmInit : vpifConfigI2cWriteReg Status: %d\n",vidStatus);

    vpifConfigI2cDeInit();

    return vidStatus;
}

/* Enable Camera Input (active low), disable all others on the vpif bus. */
static Int32 vpifConfigCameraInput(Void)
{
    Uint8 camRegVal[2] = { 0, 0 };
    Int32 camStatus = IOM_COMPLETED;

    vpifConfigI2cInit("/I2C0");
    /* Get config Reg contents */
    camStatus = vpifConfigI2cReadReg(I2C_TCA6416_ADDR,
                    I2C_EXP_CMD_CONFIG_PORT0,
                    &camRegVal[0]);
    VPIF_DEBUG1("EvmInit : vpifConfigI2cReadReg Status: %d\n",camStatus);

    camRegVal[0] &= ~I2C_EXP_MASK_VIDEO;
    camStatus = vpifConfigI2cWriteReg(I2C_TCA6416_ADDR,
                    I2C_EXP_CMD_CONFIG_PORT0,
                    &camRegVal[0]);
    VPIF_DEBUG1("EvmInit : vpifConfigI2cWriteReg Status: %d\n",camStatus);

    camStatus = vpifConfigI2cReadReg(I2C_TCA6416_ADDR,
                    I2C_EXP_CMD_WRITE_PORT0,
                    &camRegVal[0]);
    VPIF_DEBUG1("EvmInit : vpifConfigI2cReadReg Status: %d\n",camStatus);

    /* UI_SELA = 1, UI_SELB = 0, UI_SELC = 1 */
    camRegVal[0] &= ~I2C_EXP_MASK_VIDEO;
    camRegVal[0] |= 0xA0u;
    /* Set port 0 contents */
    camStatus = vpifConfigI2cWriteReg(I2C_TCA6416_ADDR,
                    I2C_EXP_CMD_WRITE_PORT0,
                    &camRegVal[0]);
    VPIF_DEBUG1("EvmInit : vpifConfigI2cWriteReg Status: %d\n",camStatus);

    vpifConfigI2cDeInit();

    return camStatus;
}



/* Selects the clock for video or camera - initialize the clock synthesizer for
 use.
          Y1 (camera) = 27MHz
          Y2 (ADC) = 	user selectable
          Y3 (VIDEO) =  27MHz
*/
static Int32 vpifConfigOutputClock()
{
    Uint8 val2[2];
    Int32 outStatus = IOM_COMPLETED;
    Uint8 outputClkFreq[5] = {0xEDu, 0x00, 0x40u, 0x02u, 0x08u}; /* 27 MHz */

    vpifConfigI2cInit("/I2C0");

    val2[0] = 0x14u | BYTE_READ_WRITE_BIT;
    val2[1] = outputClkFreq[0];
    outStatus = vpifConfigI2cWriteReg(I2C_CDCE913_ADDR, val2[0], &val2[1]);
    VPIF_DEBUG1("EvmInit : vpifConfigI2cWriteReg Status: %d\n",outStatus);

    val2[0] = 0x18u | BYTE_READ_WRITE_BIT;
    val2[1] = outputClkFreq[1];
    outStatus = vpifConfigI2cWriteReg(I2C_CDCE913_ADDR, val2[0], &val2[1]);
    VPIF_DEBUG1("EvmInit : vpifConfigI2cWriteReg Status: %d\n",outStatus);

    val2[0] = 0x19u | BYTE_READ_WRITE_BIT;
    val2[1] = outputClkFreq[2];
    outStatus = vpifConfigI2cWriteReg(I2C_CDCE913_ADDR, val2[0], &val2[1]);
    VPIF_DEBUG1("EvmInit : vpifConfigI2cWriteReg Status: %d\n",outStatus);

    val2[0] = 0x1Au | BYTE_READ_WRITE_BIT;
    val2[1] = outputClkFreq[3];
    outStatus = vpifConfigI2cWriteReg(I2C_CDCE913_ADDR, val2[0], &val2[1]);
    VPIF_DEBUG1("EvmInit : vpifConfigI2cWriteReg Status: %d\n",outStatus);

    val2[0] = 0x1Bu | BYTE_READ_WRITE_BIT;
    val2[1] = outputClkFreq[4];
    outStatus = vpifConfigI2cWriteReg(I2C_CDCE913_ADDR, val2[0], &val2[1]);
    VPIF_DEBUG1("EvmInit : vpifConfigI2cWriteReg Status: %d\n",outStatus);

    vpifConfigI2cDeInit();

    return outStatus;
}



/* Write a byte of data from the slave for the subAddr mentioned */
static Int32 vpifConfigI2cWriteReg(Uint8 slaveAddr,
                                   Uint8 subAddr,
                                   const Uint8 *buffer)
{
    Int32 writeStatus = 340;
    I2c_DataParam writeBuf;
    Uint8 writeBuffer[2];

    /* Validate the input parameters                                      */
    assert(NULL != buffer);

    /* Write to Slave Device */
    writeBuffer[0] = subAddr;
    writeBuffer[1] = *buffer;

    writeBuf.buffer    = &writeBuffer[0];
    writeBuf.bufLen    = 2u;
    writeBuf.flags     = I2c_DEFAULT_WRITE;
    writeBuf.param     = NULL;
    writeBuf.slaveAddr = slaveAddr;
    writeStatus = GIO_submit(i2cEvmOutHandle,
                      IOM_WRITE,
                      &writeBuf,
                      &writeBuf.bufLen,
                      NULL);
    return writeStatus;
}

/* Read a byte of data from the slave for the subAddr mentioned */
static Int32 vpifConfigI2cReadReg(Uint8 slaveAddr,
                                  Uint8 subAddr,
                                  Uint8 *buffer)
{
    Int32 readStatus = 340;
    I2c_DataParam readBuf;
    Uint8 cmd[2];

    /* Validate the input parameters                                          */
    assert(NULL != buffer);

    cmd[0] = subAddr;
    /* Write Subaddress of Slave Device */

    /* The following the procedure to read data from encoder / decoder
    Step A: Write the address of the register to be read.
           Note: The I2C stop should not be issued.
    Step B: Issue I2C restart.
    Step C: Read the value of the register.
    */
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
        readBuf.buffer    = buffer;
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
    return readStatus;
}



static Int32 vpifConfigI2cInit(String i2cDevName)
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

    VPIF_DEBUG("EvmInit :Start of I2C sample application\n");

    /* Create the I2C Channels for the TX and RX communication                */
    i2cEvmOutHandle = GIO_create(i2cDevName,
                          IOM_OUTPUT,
                          &initStatus,
                          &chanParams,
                          &gioAttrs);
    VPIF_DEBUG1("EvmInit :GIO_create(out) returned status = %d\n", initStatus);
    i2cEvmInHandle = GIO_create(i2cDevName,
                         IOM_INPUT,
                         &initStatus,
                         &chanParams,
                         &gioAttrs);
    VPIF_DEBUG1("EvmInit :GIO_create(in) returned status = %d\n", initStatus);

    if ((NULL == i2cEvmOutHandle) || (NULL == i2cEvmInHandle))
    {
        DBG_PRINT_ERR("EvmInit :I2C Handle Create Failed.......!\n");
        initRetVal = IOM_EBADIO;
    }
    return initRetVal;
}


static Int32 vpifConfigI2cDeInit(Void)
{
    Int32 deInitRetVal = IOM_COMPLETED;
    Int32 deInitStatus = 340;

    /* Close i2c driver and delete instance */
    deInitStatus = GIO_delete(i2cEvmOutHandle);
    if (IOM_COMPLETED != deInitStatus)
    {
        DBG_PRINT_ERR("EvmInit : I2C outHandle Delete Failed.......!\n");
        deInitRetVal = IOM_EBADIO;
    }
    else
    {
        deInitStatus = GIO_delete(i2cEvmInHandle);
        if (IOM_COMPLETED != deInitStatus)
        {
            DBG_PRINT_ERR("EvmInit : I2C inHandle Delete Failed.......!\n");
            deInitRetVal = IOM_EBADIO;
        }
    }
    return deInitRetVal;
}


/* ========================================================================== */
/*                        GLOBAL FUNCTION DEFINITIONS                         */
/* ========================================================================== */
/**
 *  \brief I2C init function called when creating the driver.
 */
Void vpifUserI2cInit()
{
    I2c_init();

    vpifI2cParams = I2c_PARAMS;
    vpifI2cParams.busFreq = 400000u;
    vpifI2cParams.hwiNumber = 8u;

    /* Configure I2C for interrupt mode                                       */
    vpifI2cParams.opMode = I2c_OpMode_INTERRUPT;

    /* No I2C 0 PSC for C6748                                              */
}



/**
 *  \brief Enables the PINMUX for VPIF device, enables the channel path of
 *         corresponding VPIF channel and configures the clocks for VPIF channel
 *
 *  \param  channelNo  [IN]    channel number
 *          isHd       [IN]    HD mode
 *
 *  \return  NONE
 */
Void configureVpif0(EvmInit_VpifChannel channelNo, Bool isHd)
{
    if ((TRUE == isHd) || (EvmInit_VpifChannel_BOTHDISPCH == channelNo))
    {
        DBG_PRINT_ERR("EvmInit :Wrong parameters passed to configureVpif0\r\n");
    }
    /*
     * Configure i2c - Enable the pinmux registers for I2c, as it is used by
     * everyone here
     */
    configureI2c();
    /* Enable the VPIF instance Pinmux                                        */
    vpifConfig(channelNo);
    switch (channelNo)
    {
        case EvmInit_VpifChannel_0:
            vpifConfigVideoInput();
            break;

        case EvmInit_VpifChannel_1:
            vpifConfigVideoInput();
            break;

        case EvmInit_VpifChannel_2:
        case EvmInit_VpifChannel_3:
            /* make sure clock is 27MHz. */
            vpifConfigOutputClock();
            break;

        case EvmInit_VpifChannel_BOTHCAPCH:
            vpifConfigCameraInput();
            /* make sure clock is 27MHz. */
            vpifConfigOutputClock();
            break;

        case EvmInit_VpifChannel_BOTHDISPCH:
        default:
            DBG_PRINT_ERR("EvmInit : Invalid case\r\n");
            break;
    }
}


/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
