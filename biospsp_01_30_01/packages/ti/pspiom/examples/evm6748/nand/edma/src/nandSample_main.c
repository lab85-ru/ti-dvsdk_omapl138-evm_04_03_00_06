/*
 * nandSample_main.c
 *
 * This file contains the test / demo code to demonstrate the NAND driver func-
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

/** \file   nandSample_main.c
 *
 *  \brief  Configures and Initialize the nand driver
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#include <std.h>
#include <iom.h>
#include <tsk.h>
#include <log.h>

#include <pwrm.h>
#include <pwrm6748.h>

#include "ti/pspiom/platforms/evm6748/Nand_evmInit.h"

/*
 * External references
 */
extern LOG_Obj    trace;
Void runNandSample();

/** main() function for the application
 *
 * Do not call any driver's IO operation here, since interrupts are disabled
 * in this function
 */
Void main()
{
    /* Enable EDMA PSC*/
    PWRM_setDependency(PWRM_RSRC_EDMA3_CC_0);
    PWRM_setDependency(PWRM_RSRC_EDMA3_TC_0);
    PWRM_setDependency(PWRM_RSRC_EDMA3_TC_1);
    PWRM_setDependency(PWRM_RSRC_EDMA3_TC_2);
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
    /*Configure nand*/
    configureNand();

    result = nandStorageInit();
    if (IOM_COMPLETED == result)
    {

        LOG_printf(&trace, "NAND_SAMPLE : Starting Sample Application "
                            " Using BLOCK MEDIA");

        runNandSample();

        result = nandStorageDeInit();
        if (IOM_COMPLETED == result)
        {
            LOG_printf(&trace, " NAND_SAMPLE: nandStorageDeInit success");
        }
        else
        {
            LOG_printf(&trace, " NAND_SAMPLE: nandStorageDeInit failed");
        }

        LOG_printf(&trace, "NAND_SAMPLE : Sample Application Ends"
                            " Using BLOCK MEDIA");
    }
    else
    {
        LOG_printf(&trace, "NAND_SAMPLE : nandStorageInit fails.");
    }

    /* Start the Heart Beat Print */
   tskHeartBit();
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
