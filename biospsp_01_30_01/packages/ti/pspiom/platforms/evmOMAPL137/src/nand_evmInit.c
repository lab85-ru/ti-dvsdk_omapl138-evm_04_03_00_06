/*
 * Nand_evmInit.c
 *
 * This file contains NAND application related EVM (platform) specific routines
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
 *  \file   nand_evmInit.c
 *
 *  \brief  This file contains the board specific code for enabling the use of
 *          nand driver.
 *
 */

#include <std.h>
#include <iom.h>
#include <gio.h>
#include <assert.h>

#include "ti/pspiom/i2c/I2c.h"
#include "ti/pspiom/platforms/evmOMAPL137/Nand_evmInit.h"
#include "ti/pspiom/cslr/cslr_syscfg_OMAPL137.h"

/*============================================================================*/
/*                          MACRO DEFINITIONS                                 */
/*============================================================================*/
#define PROG_I2C_TIMEOUT            2000u

/* Address of the pinmux registers for nand                                   */
#define PINMUX13                    (*((volatile Uint32 *)(0x01C14154u)))
#define PINMUX13_NAND_MASK          0x00FFFFFFu
#define PINMUX13_NAND_ENABLE        0x11000000u
#define PINMUX14                    (*((volatile Uint32 *)(0x01C14158u)))
#define PINMUX14_NAND_MASK          0xFF000000u
#define PINMUX14_NAND_ENABLE        0x00111111u
#define PINMUX15                    (*((volatile Uint32 *)(0x01C1415Cu)))
#define PINMUX15_NAND_MASK          0x0FFFFFFFu
#define PINMUX15_NAND_ENABLE        0x10000000u
#define PINMUX16                    (*((volatile Uint32 *)(0x01C14160u)))
#define PINMUX16_NAND_MASK          0xFFFFFFF0u
#define PINMUX16_NAND_ENABLE        0x00000001u
#define PINMUX18                    (*((volatile Uint32 *)(0x01C14168u)))
#define PINMUX18_NAND_MASK          0xFF000F0Fu
#define PINMUX18_NAND_ENABLE        0x00111010u
#define PINMUX19                    (*((volatile Uint32 *)(0x01C1416Cu)))
#define PINMUX19_NAND_MASK          0xFFFFFFF0u
#define PINMUX19_NAND_ENABLE        0x00000001u
/* Address of the pinmux 8 register for i2c                                   */
#define PINMUX8                     (*((volatile Uint32 *)(0x01C14140u)))
/* enable the pinmux registers for I2c                                        */
#define PINMUX8_I2C_MASK            0xFFF00FFFu
#define PINMUX8_I2C_ENABLE          0x00022000u

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */

static Int32 selectI2cExpanderNand(Void);

static Void configNand();

/*
 * I2C0 device params. To be filled in nandUserI2cInit function which
 * is called before driver creation
 */
I2c_Params   nandI2cParams;

/*
 * I2C0 init function called when creating the driver.
 */
Void nandUserI2cInit()
{
    I2c_init();

    nandI2cParams = I2c_PARAMS;
    nandI2cParams.busFreq = 200000u;
    nandI2cParams.hwiNumber = 8u;

    /* configure I2c for edma mode                                            */
    nandI2cParams.opMode = I2c_OpMode_INTERRUPT;
}



static Int32 selectI2cExpanderNand(Void)
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
        data[0] = data[0] | 0x40;
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


/* enable the NAND instance in the Pinmux */
static Void configNand()
{
    /*KICK0R and KICK1R registers needs to be enabled for this PINMUX setting *
     * to be successful. This needs to be ensured, by the bootloader or GEL   *
     * file, and should be done by only one entity else race conditions in    *
     * SYSCFG registers accesses could result                                 */
    
    /* enable the pinmux configuration for the NAND device                    */
    /* Enable EMA_WAIT[0] signal                                              */
    PINMUX19 &= PINMUX19_NAND_MASK;
    PINMUX19 |= PINMUX19_NAND_ENABLE;

    /* Enable EMA_D[1:0]                                                      */
    PINMUX13 &= PINMUX13_NAND_MASK;
    PINMUX13 |= PINMUX13_NAND_ENABLE;

    /* Enable EMA_D[7:2]                                                      */
    PINMUX14 &= PINMUX14_NAND_MASK;
    PINMUX14 |= PINMUX14_NAND_ENABLE;

    /* Enable EMA_A[1] (NAND_ALE)                                             */
    PINMUX15 &= PINMUX15_NAND_MASK;
    PINMUX15 |= PINMUX15_NAND_ENABLE;

    /* Enable EMA_A[2] (NAND_CLE)                                             */
    PINMUX16 &= PINMUX16_NAND_MASK;
    PINMUX16 |= PINMUX16_NAND_ENABLE;

    /* Enable EMA_OE, EMA_WE, CS[3] and CS[2]                                 */
    PINMUX18 &= PINMUX18_NAND_MASK;
    PINMUX18 |= PINMUX18_NAND_ENABLE;

    /* enable the pinmux registers for I2c                                    */
    PINMUX8 &= PINMUX8_I2C_MASK;
    PINMUX8 |= PINMUX8_I2C_ENABLE;

}

Void configureNand()
{
    Int32 result;
    /* enable the NAND instance in the PSC module & Pinmux */
    configNand();
    /*Configure i2c*/
    result = selectI2cExpanderNand();
    if (IOM_COMPLETED != result)
    {
        /* Most likely no UI board, so just continue*/
    }
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
