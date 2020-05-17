/*
 * pros_edma3_drv_sample_main.c
 *
 * This file contains the test / demo code to demonstrate the EDMA3 driver
 * functionality on PrOS.
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

#include <ti/sdo/edma3/drv/sample/pros_edma3_drv_sample.h>

EDMA3_DRV_Result edma3MemToMemCpytest ();

static void tskHeartBit();

static void printWelcomeBanner(void)
    {
    /* Welcome Message */
    EDMA3_DEBUG_PRINTF("***************************************************************\n\r");
    EDMA3_DEBUG_PRINTF("*                                                             *\n\r");
    EDMA3_DEBUG_PRINTF("*                            ****                             *\n\r");
    EDMA3_DEBUG_PRINTF("*                            ****                             *\n\r");
    EDMA3_DEBUG_PRINTF("*                            ******o***                       *\n\r");
    EDMA3_DEBUG_PRINTF("*                      ********_///_****                      *\n\r");
    EDMA3_DEBUG_PRINTF("*                      ***** /_//_/ ****                      *\n\r");
    EDMA3_DEBUG_PRINTF("*                       ** ** (__/ ****                       *\n\r");
    EDMA3_DEBUG_PRINTF("*                           *********                         *\n\r");
    EDMA3_DEBUG_PRINTF("*                            ****                             *\n\r");
    EDMA3_DEBUG_PRINTF("*                            ***                              *\n\r");
    EDMA3_DEBUG_PRINTF("*                                                             *\n\r");
    EDMA3_DEBUG_PRINTF("*                     TI EDMA3 LOW LEVEL DRIVER               *\n\r");
    EDMA3_DEBUG_PRINTF("*                                                             *\n\r");
    EDMA3_DEBUG_PRINTF("*                                                             *\n\r");
    EDMA3_DEBUG_PRINTF("*       For issues on TI EDMA3 LLD, contact TII PSP Team      *\n\r");
    EDMA3_DEBUG_PRINTF("*                                                             *\n\r");
    EDMA3_DEBUG_PRINTF("*                                                             *\n\r");
    EDMA3_DEBUG_PRINTF("*                                                             *\n\r");
    EDMA3_DEBUG_PRINTF("***************************************************************\n\r");
    EDMA3_DEBUG_PRINTF("\r\n\r\n");
    }

void echo()
    {
    EDMA3_DRV_Result edmaResult = EDMA3_DRV_SOK;

    /* Print the Welcome Message */
    printWelcomeBanner();

    /* Initialize EDMA3 first */
    edmaResult = edma3init(0);
    if (edmaResult != EDMA3_DRV_SOK)
        {
        EDMA3_DEBUG_PRINTF("echo: edma3init() FAILED, error code: %d\r\n",
                                        edmaResult);
        }
    else
        {
        EDMA3_DEBUG_PRINTF("echo: edma3init() PASSED\r\n");
        }

    if (edmaResult == EDMA3_DRV_SOK)
        {
        /* Edma Test for memory to memory copy */
        EDMA3_DEBUG_PRINTF("\r\nEDMA3: Starting EDMA3 Test memory to memory copy\r\n");

        edmaResult = edma3MemToMemCpytest();

        if (EDMA3_DRV_SOK != edmaResult)
           {
            /* Report EDMA Error */
            EDMA3_DEBUG_PRINTF("\r\nEDMA3: edma3MemToMemCpytest() failed\r\n");
           }
        else
           {
            EDMA3_DEBUG_PRINTF("\r\nEDMA3: edma3MemToMemCpytest() passed\r\n");
           }

        EDMA3_DEBUG_PRINTF("\r\nEDMA3 : End EDMA3 Test memory to memory copy\r\n\n");
        }

    /* Start the Heart Beat Print */
    tskHeartBit();

    return;
    }



void tskHeartBit()
    {
    static unsigned int counter = 0u;

    while (counter < 0x1000000u)
        {
        TSK_sleep (1000u);
        EDMA3_DEBUG_PRINTF("\r\n\r\n!!! EDMA3 LLD HrtBt %x", counter);
        counter++;
        }
    }

