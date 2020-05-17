/*
 * spiSample_main.c
 *
 * This file contains the test / demo code to demonstrate the SPI driver functi- 
 * onality in DMA-interrupt mode of operation on DSP/BIOS 5.
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

/** \file    spiSample_main.c
 *
 *  \brief   This file contains the init function for SPI driver implemented for
 *           DSP/BIOS
 *
 *           (C) Copyright 2008, Texas Instruments, Inc
 *
 *  \author  PSG
 *
 *  \version 0.1   created newly for the C6748 platform.
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include "stdio.h"
#include "string.h"
#include "std.h"
#include "log.h"
#include "tsk.h"
#include "idl.h"
#include "ecm.h"
#include "iom.h"
#include "sio.h"
#include "ti/pspiom/spi/Spi.h"
#include "ti/pspiom/psc/Psc.h"
#include "ti/pspiom/platforms/evm6748/Spi_evmInit.h"

extern LOG_Obj trace;
/* ========================================================================== */
/*                       IMPORTED FUNCTIONS                                   */
/* ========================================================================== */
extern void start_spi_sample();
extern void spi_initParams(void);



/* ========================================================================== */
/*                          LOCAL FUNCTION PROTOTYPE                          */
/* ========================================================================== */

static void tskHeartBit(void);

/* ========================================================================== */
/*                           FUNCTION DEFINITIONS                             */
/* ========================================================================== */
/*
 * SPI init function called when creating the driver.
 */
void SpiUserInit()
{
    spi_initParams();
    Spi_init();

}

/*
 *  \fn    void echo(void)
 *
 *  \brief Entry point for the "echo" task.
 *
 *   This function is the main entry point for the SPI sample application's task.
 *   it calls the spi sample applcation to demostrate the usage of the SPI driver
 *
 *  \param  None
 *
 *  \return None
 */
void echo(void)
{
    char yesOrno = 0;

    /* Warn of the erase                                                      */
    printf("\nThis example will erase the contents of the flash...\n");
    printf("Enter Y or y to continue\n");

    scanf("%c",&yesOrno);

    if((yesOrno == 'Y') || (yesOrno == 'y'))
    {
        /* Initialize pinmux and evm related configurations */
        configureSpi();

        /* call the function for the SPI application test                     */
        start_spi_sample();

        /* Start the Heart Beat Print                                         */
        tskHeartBit();
    }

    return;
}


/*
 *  \fn    void main(void)
 *
 *  \brief Main function for the Spi sample application.
 *
 *   This function is the main entry point for the SPI sample application.
 *   This sample applciation demostrates the usage of SPI device for communication
 *   with a spi flash device. it demostrates a read and write operations.
 *
 *  \param  None
 *
 *  \return None
 */
void main(void)
{
    return;
}

/*
 *  \fn    void tskHeartBit(void)
 *
 *  \brief Function to print a debug message to screen.
 *
 *  \param  None
 *
 *  \return None
 */
void tskHeartBit(void)
{
    static Uint32 counter = 0;

    while (counter < 0xFFFFFFFF)
    {
        TSK_sleep(1000u);

        LOG_printf(&trace,"\r\n\r\n!!! PSP HrtBt %l", counter);
        counter++;
    }
}

/* ========================================================================== */
/*                                END OF FILE                                 */
/* ========================================================================== */

