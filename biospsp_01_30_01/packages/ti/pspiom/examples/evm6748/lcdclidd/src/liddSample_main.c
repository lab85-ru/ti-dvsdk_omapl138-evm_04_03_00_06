/*
 * liddSample_main.c
 *
 * This file contains the test / demo code to demonstrate the LCDC LIDD driver 
 * functinality on DSP/BIOS 5. This demonstration is based on a character LCD
 * display.
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

/** \file   liddSample_main.c
 *
 *  \brief  This file contains the test / demo code to demonstrate the
 *          lidd driver implemented for DSP/BIOS
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 *  \author     PSG
 *  \version    0.1
 */

#include <std.h>
#include <stdio.h>
#include <string.h>
#include <log.h>
#include <tsk.h>
#include <ecm.h>
#include <iom.h>
#include <gio.h>

#include "ti/pspiom/lcdclidd/Lidd.h"
#include "ti/pspiom/psc/Psc.h"
#include "ti/pspiom/platforms/evm6748/Lcdlidd_evmInit.h"
#include "ti/pspiom/cslr/cslr_psc_C6748.h"

#define LIDD_NUM_IOP  10
#define BUFSIZE       24

const Lidd_DisplayConf dispConf = {
        /* The character LCD display type                                     */
        Lidd_DisplayType_Hitachi,
        /* Strobe signal timing configuration for the CS0 (primary) device    */
        {
            0,/* cs0Timing.rSu      */
            1,/* cs0Timing.rStrobe  */
            1,/* cs0Timing.rHold    */
            0,/* cs0Timing.wSu      */
            1,/* cs0Timing.wStrobe  */
            1,/* cs0Timing.wHold    */
            1 /* cs0Timing.ta       */
        },
        /* Strobe signal timing configuration for the CS1 (secondaty) device  */
        {
            0,/* cs1Timing.rSu      */
            1,/* cs1Timing.rStrobe  */
            1,/* cs1Timing.rHold    */
            0,/* cs1Timing.wSu      */
            1,/* cs1Timing.wStrobe  */
            1,/* cs1Timing.wHold    */
            1 /* cs1Timing.ta       */
        },
		1u
};

/*
 * External references
 */
extern LOG_Obj trace;
extern Int32   runLiddSample(Void);

/*
 * Global references
 */

/* Lidd channel handle */
GIO_Handle liddChanHandle = NULL;
Lidd_Params  liddParams;

/*
 * Local References
 */
static Void tskHeartBeat(Void);

void main()
{
    return;
}


Void liddSampleTask()
{
    Int32           retVal = IOM_COMPLETED;

    /* I2c stream variables */
    Int             status       = 340;

    /* Lidd stream variables */
    GIO_Attrs       liddGioAttrs;
    Lidd_ChanParams liddChanParams;


    /* call the EVM specific function to configure the LCDC                   */
    configureLcdlidd();

    /*Intialize the Lidd stream parameters                                    */
    liddGioAttrs          = GIO_ATTRS;
    liddGioAttrs.nPackets = LIDD_NUM_IOP;

    liddChanParams.controller = Lidd_Controller_LIDD;
    liddChanParams.chanConf   = (Ptr)&dispConf;

    /* Create a channel to the Lidd driver                                    */
    liddChanHandle =
        GIO_create("/LIDD0", IOM_OUTPUT, &status, &liddChanParams, &liddGioAttrs);

    if(NULL == liddChanHandle)
    {
        LOG_printf(&trace, "ERROR: CHANNEL CREATION TO LIDD DRIVER FAILED\n");
        return;
    }

    /* Run Lidd sample application */
    retVal = runLiddSample();

    if (IOM_COMPLETED != retVal)
    {
        LOG_printf(&trace, "SAMPLE APPLICATION COMPLETION FAILED\n");
    }
    else
    {
        LOG_printf(&trace, "SAMPLE APPLICATION COMPLETION SUCCESS\n");
    }

    /* Start the Heart Beat Print */
    tskHeartBeat();

    return;
}



/*
 * LCDC0 init function called when creating the driver.
 */
void userLiddInit()
{
    /* call the driver initialization function */
    Lidd_init();

    liddParams = Lidd_PARAMS;
    liddParams.devConf.clkFreqHz = 1181102;
    liddParams.devConf.hwiNum = 9;
    liddParams.devConf.numLines = 2;
    liddParams.devConf.numCharPerLine = 24;
    liddParams.devConf.addressArray[0] = 0;
    liddParams.devConf.addressArray[1] = 64;
    liddParams.devConf.addressArray[2] = 0;
    liddParams.devConf.addressArray[3] = 0;
}



Void tskHeartBeat(Void)
{
    static Uint32 counter = 0;

    while (counter < 0xFFFFFFFF)
    {
        TSK_sleep(1000u);    /* Add large delay */
        LOG_printf(&trace, "!!! HearBeat");
        LOG_printf(&trace, "\n");
        counter++;
    }
}
