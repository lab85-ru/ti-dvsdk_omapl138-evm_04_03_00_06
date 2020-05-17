/*
 * i2cSample_main.c
 *
 * This file contains the test / demo code to demonstrate the I2C driver functi- 
 * nality in Interrupt mode of operation on DSP/BIOS 5.
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

/** \file   i2csample_main.c
 *
 *  \brief  This file contains the sample code to demonstrate the usage of the
 *          I2C driver implemented for DSP/BIOS
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 *  \author     PSG
 *
 *  \version    0.1  created newly for the C6747 platform
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include <std.h>

#include <stdio.h>
#include <string.h>

#include <tsk.h>
#include <ecm.h>
#include <log.h>

#include "ti/pspiom/i2c/I2c.h"
#include "ti/pspiom/psc/Psc.h"
#include "ti/pspiom/platforms/evm6747/I2c_evmInit.h"

extern LOG_Obj trace;

/* 
 * I2C0 device params. To be filled in user_i2c_init function which
 * is called before driver creation 
 */
I2c_Params   i2cParams;

extern void start_i2c_sample();
void echo(void);

static void tskHeartBeat();

void main()
{
    return;
}


void echo()
{
    /* Initialize pinmux and evm related configurations */
    configureI2c();

    LOG_printf(&trace,"\r\n I2C Sample Application\n\r");

    /* Run I2C sample application */
    start_i2c_sample();

    /* Start the Heart Beat Print */
    tskHeartBeat();

    return;
}

/*
 * I2C0 init function called when creating the driver.
 */
void user_i2c_init()
{
    I2c_init();

    i2cParams = I2c_PARAMS;
    i2cParams.busFreq = 200000;
    i2cParams.hwiNumber = 8;

    /* configure I2c for interrupt mode */
    i2cParams.opMode = I2c_OpMode_INTERRUPT;

}

void tskHeartBeat()
{
    static Uint32 counter = 0;

    while (counter < 0xFFFFFFFF)
    {
        TSK_sleep(1000u);    /* Add large delay */
        LOG_printf(&trace, "\r\n\r\n!!! PSP HrtBt %l", counter);
        counter++;
    }
}
