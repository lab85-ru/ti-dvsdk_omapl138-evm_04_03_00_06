/*
 * uartSample.c
 *
 * This file contains the test / demo code to demonstrate the UART driver functi- 
 * onality in Interrupt mode of operation on DSP/BIOS 5.
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

/** \file   uartSample.c
 *
 *  \brief  This file contains the test / demo code to demonstrate the
 *          UART driver implemented for DSP/BIOS in interrupt mode.
 *
 *  (C) Copyright 2006, Texas Instruments, Inc
 *
 *  \author     PSG
 *  \version    0.1
 */

#include <std.h>
#include <stdio.h>
#include <string.h>
#include <gio.h>
#include <log.h>
#include <tsk.h>
#include <stdio.h>
#include "ti/pspiom/uart/Uart.h"
#include "ti/pspiom/platforms/evmOMAPL138/Uart_evmInit.h"

/*
 * External references
 */
extern LOG_Obj    trace;

/*
 * UART0 device params. To be filled in uart0_dev_init function which
 * is called before driver creation
 */
Uart_Params   uartParams;

/*
 * Starting message printing string
 * Note: Buffer alignement is required only when working in DMA Mode.
 */
#pragma DATA_ALIGN(uartTestStringStart, 128);
static Int8 uartTestStringStart[128];

/* Tx memory buffer */
#pragma DATA_ALIGN(Uart_TxBuffer, 128);
static Int8  Uart_TxBuffer[1024];

/* UART handle for input channel */
GIO_Handle hUart_IN;

/* UART handle for output channel */
GIO_Handle hUart_OUT;

/* Local function prototypes */
static void genericUartTest();

/* ========================================================================== */
/*                        FUNCTION DEFINTIONS                                 */
/* ========================================================================== */

/**
 * \brief    Main function of the sample application.
 *           This function calls the APIs required for the initalisation of the
 *           PSC variables
 *
 * \param    None
 *
 * \return   None
 */
Void main(Void)
{
    return;
}

/**
 * \brief    Entry point for the "echo" task.
 *           This function calls the APIs required to demonstrate the UART
 *           demo sample application.
 *
 * \param    None
 *
 * \return   None
 */
Void echo(Void)
{
    GIO_Attrs gioAttrs      = GIO_ATTRS;
    Int32     echoTskStatus = 0;

    /* Initialize channel attributes.                                         */
    gioAttrs.nPackets = 2;

    /* Initialize pinmux and evm related configurations                       */
    configureUart();

   /* Initialize UART Currently is been used to display a string              */
    hUart_OUT = GIO_create("/UART0",IOM_OUTPUT,NULL,NULL,&gioAttrs);
    hUart_IN  = GIO_create("/UART0",IOM_INPUT,&echoTskStatus,NULL,&gioAttrs);

    if ((NULL == hUart_IN) || (NULL == hUart_OUT))
    {
        LOG_printf(&trace, "ERROR: Initialization of UART failed\n");
        return;
    }

    if ((NULL != hUart_IN) && (NULL != hUart_OUT))
    {
        /* Run UART sample application                                        */
        genericUartTest();

        /* Exit                                                               */
        SYS_exit(0);
    }
    return;
}


/**
 * \brief    Initialisation function.
 *           This function initializes the UART driver and also the required
 *           parameters for the creation of the device.
 *
 * \param    None
 *
 * \return   None
 */
void user_uart2_init()
{
    Uart_init();
    uartParams = Uart_PARAMS;
    uartParams.hwiNumber = 9;
    uartParams.opMode = Uart_OpMode_INTERRUPT;
    uartParams.rxThreshold = Uart_RxTrigLvl_1;
}

/**
 * \brief   Generic read write test
 *
 *          This Function is called to test generic UART test.In this function
 *          basic read/write functionality is tested.
 *
 * \param   None
 *
 * \return  None
 */
static Void genericUartTest(Void)
{
    Ptr     buf    = NULL;
    Int     status = 0;
    size_t  len    = 0;
    Int8   *str    = NULL;

    LOG_printf(&trace," Starting UART sample application \n\r");

    str = "UART Demo Starts:  INPUT a file of size 1000 bytes";

    /* Copy to start string to Cache aligned buffer                           */
    len = strlen(str);
    memcpy(uartTestStringStart,str,len);

    buf = uartTestStringStart;

    status = GIO_submit(hUart_OUT,IOM_WRITE, buf, &len, NULL);

    if(!((status == IOM_COMPLETED)||(status == IOM_PENDING)))
    {
        LOG_printf(&trace, "\r\n Error from GIO_write for UART Test string\n");
    }

    buf = Uart_TxBuffer;
    len = 1000u;
    status = GIO_submit(hUart_IN,IOM_READ,buf,&len,NULL);

    if (!((status == IOM_COMPLETED)||(status == IOM_PENDING)))
    {
        LOG_printf(&trace, "\r\n Error from GIO_read for 1000 bytes read\n");
    }

    buf = Uart_TxBuffer;
    len = 1000u;
    status = GIO_submit(hUart_OUT,IOM_WRITE,buf,&len,NULL);

    if (!((status == IOM_COMPLETED) || (status == IOM_PENDING)))
    {
        LOG_printf(&trace, "\r\n Error from GIO_write for 1000 bytes write\n");
    }

    LOG_printf(&trace, "UART sample application completed \n\r ");
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
