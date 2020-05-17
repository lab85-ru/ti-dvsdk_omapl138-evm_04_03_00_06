/*
 * main_multi_edma.c
 *
 * This file contains the test / demo code to demonstrate the EDMA3 driver
 * functionality on DSP/BIOS 5.
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

#include <std.h>

#include "common.h"

extern EDMA3_DRV_Handle hEdma[];
extern const unsigned int numEdma3Instances;

EDMA3_DRV_Result edma3MemToMemCpytest (EDMA3_DRV_Handle hEdma);

/*
 * Local References
 */
static void tskHeartBit(void);
void echo(void);

void main()
{
}


static void printWelcomeBanner(void)
    {
    /* Welcome Message */
    printf("***************************************************************\n\r");
    printf("*                                                             *\n\r");
    printf("*                            ****                             *\n\r");
    printf("*                            ****                             *\n\r");
    printf("*                            ******o***                       *\n\r");
    printf("*                      ********_///_****                      *\n\r");
    printf("*                      ***** /_//_/ ****                      *\n\r");
    printf("*                       ** ** (__/ ****                       *\n\r");
    printf("*                           *********                         *\n\r");
    printf("*                            ****                             *\n\r");
    printf("*                            ***                              *\n\r");
    printf("*                                                             *\n\r");
    printf("*                     TI EDMA3 LOW LEVEL DRIVER               *\n\r");
    printf("*                                                             *\n\r");
    printf("*                                                             *\n\r");
    printf("*       For issues on TI EDMA3 LLD, contact TII PSP Team      *\n\r");
    printf("*                                                             *\n\r");
    printf("*                                                             *\n\r");
    printf("*                                                             *\n\r");
    printf("***************************************************************\n\r");
    printf("\r\n\r\n");
    }


void echo()
    {
    EDMA3_DRV_Result edmaResult = EDMA3_DRV_SOK;
    unsigned int edma3Id;

    /* Print the Welcome Message */
    printWelcomeBanner();

    /* Initialize EDMA3 first */
    edmaResult = edma3init();
    if (edmaResult != EDMA3_DRV_SOK)
        {
        printf("\nEDMA3 Initialization FAILED, error code: %d\n", edmaResult);
        }
    else
        {
        printf("\nEDMA3 Initialization PASSED\n");
        }

    if (EDMA3_DRV_SOK == edmaResult)
        {
        /* Edma Test for memory to memory copy */
        printf("\nStarting EDMA3 Test memory to memory copy\n\n");

		for (edma3Id = 0; edma3Id < numEdma3Instances; edma3Id++)
			{
		    edmaResult = edma3MemToMemCpytest(hEdma[edma3Id]);

		    if (EDMA3_DRV_SOK != edmaResult)
		       {
		       /* Report EDMA Error */
		       printf("\nedma3MemToMemCpytest() failed for EDMA3# %d\n",edma3Id);
		       }
		    else
		       {
		        printf("\nedma3MemToMemCpytest() passed for EDMA3# %d\n",edma3Id);
		       }
			}
    	}

	/* De-init EDMA3 now. */
    if (EDMA3_DRV_SOK == edmaResult)
        {
    	edmaResult = edma3deinit();
    	}

    if (edmaResult != EDMA3_DRV_SOK)
        {
        printf("\necho: EDMA3 De-initialization FAILED, error code: %d\r\n", edmaResult);
        }
    else
        {
        printf("\necho: EDMA3 De-initialization PASSED\r\n");
        }

    /* Start the Heart Beat Print */
	tskHeartBit();

    return;
    }


/**
 *  \brief   Main sample test case which will call other EDMA3 test cases.
 *              If one wants to call Edma3 test cases, include this main
 *              test case only.
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
EDMA3_DRV_Result edma3MemToMemCpytest (EDMA3_DRV_Handle hEdma)
{
    EDMA3_DRV_Result result = EDMA3_DRV_SOK;

    if (hEdma == NULL)
        {
        result = EDMA3_DRV_E_INVALID_PARAM;
        }

    /* Edma test without linking, async, incr mode */
    if (result == EDMA3_DRV_SOK)
        {
        result = edma3_test(hEdma, MAX_ACOUNT, MAX_BCOUNT, MAX_CCOUNT, EDMA3_DRV_SYNC_A);

	    if (result == EDMA3_DRV_SOK)
	        {
	        printf ("edma3_test (without linking) Passed\r\n");
	        }
	    else
	        {
	        printf ("edma3_test (without linking) Failed\r\n");
	        }
		}

	/* Edma test with linking, async, incr mode */
    if (result == EDMA3_DRV_SOK)
        {
        result = edma3_test_with_link(hEdma, MAX_ACOUNT, MAX_BCOUNT, MAX_CCOUNT, EDMA3_DRV_SYNC_A);

	    if (result == EDMA3_DRV_SOK)
	        {
	        printf ("edma3_test_with_link Passed\r\n");
	        }
	    else
	        {
	        printf ("edma3_test_with_link Failed\r\n");
	        }
		}

	/* Qdma test, async, incr mode */
    if (result == EDMA3_DRV_SOK)
        {
        result = qdma_test(hEdma, MAX_ACOUNT, MAX_BCOUNT, MAX_CCOUNT, EDMA3_DRV_SYNC_A);

	    if (result == EDMA3_DRV_SOK)
	        {
	        printf ("qdma_test Passed\r\n");
	        }
	    else
	        {
	        printf ("qdma_test Failed\r\n");
	        }
		}

	/* Qdma test with linking, async, incr mode */
    if (result == EDMA3_DRV_SOK)
        {
        result = qdma_test_with_link (hEdma, MAX_ACOUNT, MAX_BCOUNT, MAX_CCOUNT, EDMA3_DRV_SYNC_A);

	    if (result == EDMA3_DRV_SOK)
	        {
	        printf ("qdma_test_with_link Passed\r\n");
	        }
	    else
	        {
	        printf ("qdma_test_with_link Failed\r\n");
	        }
		}

	/* DMA channels with chaining test. */
    if (result == EDMA3_DRV_SOK)
        {
        result = edma3_test_with_chaining(hEdma, MAX_ACOUNT, MAX_BCOUNT, MAX_CCOUNT, EDMA3_DRV_SYNC_A);

        if (result == EDMA3_DRV_SOK)
            {
            printf ("edma3_test_with_chaining Passed\r\n");
            }
        else
            {
            printf ("edma3_test_with_chaining Failed\r\n");
            }
        }

	/* DMA channels using Polling mode test. */
    if (result == EDMA3_DRV_SOK)
        {
        result = edma3_test_poll_mode(hEdma, MAX_ACOUNT, MAX_BCOUNT, MAX_CCOUNT, EDMA3_DRV_SYNC_A);

        if (result == EDMA3_DRV_SOK)
            {
            printf ("edma3_test_poll_mode Passed\r\n");
            }
        else
            {
            printf ("edma3_test_poll_mode Failed\r\n");
            }
        }

	/* DMA channels in using ping-pong buffers test. */
    if (result == EDMA3_DRV_SOK)
        {
        result = edma3_test_ping_pong_mode(hEdma);

        if (result == EDMA3_DRV_SOK)
            {
            printf ("edma3_test_ping_pong_mode Passed\r\n");
            }
        else
            {
            printf ("edma3_test_ping_pong_mode Failed\r\n");
            }
        }

	/* Misc tests. */
    if (result == EDMA3_DRV_SOK)
        {
	    result = edma3_misc_test(hEdma);

        if (result == EDMA3_DRV_SOK)
            {
            printf ("edma3_misc_test Passed\r\n");
            }
        else
            {
            printf ("edma3_misc_test Failed\r\n");
            }
        }

    return result;
}


void tskHeartBit()
    {
	unsigned int counter = 0u;

    while (counter < 0x1000000u)
        {
        printf("\r\n\r\n!!! EDMA3 LLD HrtBt %x", counter);
        counter++;
		TSK_sleep(10000);
        }
    }
