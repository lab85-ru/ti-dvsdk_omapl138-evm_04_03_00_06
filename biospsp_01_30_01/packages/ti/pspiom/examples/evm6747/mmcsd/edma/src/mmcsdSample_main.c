/*
 * mmcsdSample_main.c
 *
 * This file contains the test / demo code to demonstrate the MMCSD driver func-
 * tionality in DMA mode of operation on DSP/BIOS 5.
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

/** \file   mmcsdSample_main.c
 *
 *  \brief  Configures and Initialize the mmcsd driver
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#include <std.h>
#include <hwi.h>
#include <tsk.h>
#include <iom.h>
#include <log.h>
#include "ti/pspiom/platforms/evm6747/Mmcsd_evmInit.h"

/*
 * External references
 */
extern LOG_Obj    trace;
extern Void runMmcsdSample(Void);
#ifdef Mmcsd_GPIO_CDWP_ENABLE
    extern Void mmcsdGpioInputIsr(Void);
#endif

/** main() function for the application
 *
 * Do not call any driver's IO operation here, since interrupts are disabled
 * in this function
 */
Void main()
{
    return;
}

Void tskHeartBit(Void)
{
    static Uint32 counter = 0;

    while (counter < 0xFFFFFFFF)
    {
        TSK_sleep(1000);    /* Add large delay */
        LOG_printf(&trace, " PSP HrtBt %d", counter);
        counter++;
    }
}

/**
 *  \brief Demonstrates use of PSP drivers.
 */
Void echo()
{
    Int32 result;

    /*Configure MMCSD*/
#ifdef Mmcsd_GPIO_CDWP_ENABLE
    configureMmcsd(mmcsdGpioInputIsr);
#else
    configureMmcsd(NULL);
#endif

    result = mmcsdStorageInit();
    if (IOM_COMPLETED == result)
    {

        LOG_printf(&trace, "MMCSD_SAMPLE : Starting Sample Application"
                            " Using BLOCK MEDIA");

        runMmcsdSample();
        result = mmcsdStorageDeInit();
        if (IOM_COMPLETED == result)
        {
            LOG_printf(&trace, " MMCSD_SAMPLE: mmcsdStorageDeInit success");
        }
        else
        {
            LOG_printf(&trace, " MMCSD_SAMPLE: mmcsdStorageDeInit failed");
        }

        LOG_printf(&trace, "MMCSD_SAMPLE : Sample Application Ends"
                            " Using BLOCK MEDIA");
    }
    else
    {
        LOG_printf(&trace, "MMCSD_SAMPLE : mmcsdStorageInit fails.");
    }

    /* Start the Heart Beat Print */
   tskHeartBit();
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
