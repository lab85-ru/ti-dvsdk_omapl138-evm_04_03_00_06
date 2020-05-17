/*
 * mcbspSampleSlave_io.c
 *
 * This file contains the test / demo code to demonstrate the McBSP driver
 * slave functionality on DSP/BIOS 5.
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
 * \file    mcbspSampleSlave_io.c
 *
 * \brief   McBSP evm to evm communication sample application
 *
 *          This file demonstrates the use of Mcbsp is by using an
 *          EVM to EVM communication setup.This file configures one of the evm
 *          in slave mode.
 *
 *          (C) Copyright 2009, Texas Instruments, Inc
 *
 * \author  PSP
 *
 * \version 0.1
 */

/* ========================================================================== */
/*                            INCLUDE FILES                                   */
/* ========================================================================== */

#include <std.h>
#include <sio.h>
#include <iom.h>
#include <log.h>
#include <tsk.h>
#include <string.h>
#include <ti/sdo/edma3/drv/edma3_drv.h>
#include "ti/pspiom/mcbsp/mcbsp.h"


/* ========================================================================== */
/*                        IMPORTED VARIABLES                                  */
/* ========================================================================== */

extern LOG_Obj trace;
extern EDMA3_DRV_Handle hEdma[];

/* ========================================================================== */
/*                        EXTERNAL FUNCTIONS                                  */
/* ========================================================================== */

extern Int edma3init();

/* ========================================================================== */
/*                         MACRO DEFINITIONS                                  */
/* ========================================================================== */

#define NUM_BUFS              1
#define BUFALIGN              128           /* align buffers to 128 bytes     */
#define BUFSIZE               1024          /* 1K of data transceive          */
#define LOOP_COUNT            100

#define DEFAULT_BITPERSAMPLE  8             /* number of bits per slot        */
#define NUM_OF_CHANNELS       1             /* Number of slots to be used     */

/* ========================================================================== */
/*                          LOCAL VARIABLES                                   */
/* ========================================================================== */

/* Handles for the TX and RX channels                     */
static SIO_Handle mcbspInHandle  = NULL;

/* array to hold the pointer to the allocated buffers     */
Ptr buf[NUM_BUFS];

Uint32 pktCount = 0;

/**< settings to configure the TX or RX hardware sections                 */
Mcbsp_DataConfig mcbspChanConfig =
{
    Mcbsp_Phase_SINGLE,
    Mcbsp_WordLength_8,
    Mcbsp_WordLength_8,     /* Dont care                */
    NUM_OF_CHANNELS,
    NUM_OF_CHANNELS,        /* Dont care                */
    Mcbsp_FrmSync_DETECT,
    Mcbsp_DataDelay_1_BIT,
    Mcbsp_Compand_OFF_MSB_FIRST,
    Mcbsp_BitReversal_DISABLE,
    Mcbsp_IntMode_ON_SYNCERR,
    Mcbsp_RxJust_RZF,    /* Dont care for TX         */
    Mcbsp_DxEna_OFF
};


/**< clock setup for the RX or the TX section                             */
Mcbsp_ClkSetup mcbspClkConfig =
{
    Mcbsp_FsClkMode_EXTERNAL,
    96000,                   /* 96KHz                   */
    Mcbsp_TxRxClkMode_EXTERNAL,
    Mcbsp_FsPol_ACTIVE_HIGH,
    Mcbsp_ClkPol_RISING_EDGE
};

/**< Multi channel setup                                                  */
Mcbsp_McrSetup mcbspMultiChanCtrl =
{
    Mcbsp_McmMode_ALL_CHAN_ENABLED_UNMASKED,
    Mcbsp_PartitionMode_CHAN_0_15,
    Mcbsp_PartitionMode_CHAN_16_31,
    Mcbsp_PartitionMode_2
};

Mcbsp_ChanParams mcbspChanparam =
{
    Mcbsp_WordLength_8,   /* wordlength configured    */
    NULL,                 /* loop job buffer internal */
    0,                    /* user loopjob length      */
    NULL,                 /* global error callback    */
    NULL,                 /* edma Handle              */
    1,                    /* EDMA event queue         */
    8,                    /* hwi number               */
    Mcbsp_BufferFormat_1SLOT,
    TRUE,                 /* FIFO mode enabled        */
    &mcbspChanConfig,     /* channel configuration    */
    &mcbspClkConfig,      /* clock configuration      */
    &mcbspMultiChanCtrl,  /* multi channel control    */
    0x00,
    0x00,
    0x00,
    0x00
};

/* ========================================================================== */
/*                             LOCAL FUNCTIONS                                */
/* ========================================================================== */

static Void mcbspCreateStreams(Void);
static Void mcbspDriverPrime(Void);

/* ========================================================================== */
/*                           FUNCTION DEFINTIONS                              */
/* ========================================================================== */

/*
 * \brief   This function demostrates the use of Mcbsp using an EVM to EVM
 *          communication setup.
 *
 * \param   None
 *
 * \return  None
 */
Void mcbspStartDemo(Void)
{
    Ptr                rcv       = NULL;
    Uint32             count     = 0;
    Int32              nmadus1   = 0;
    Uint32             i         = 0;
    Bool               fail      = FALSE;

    /* create the streams required for the transactions                       */
    mcbspCreateStreams();

    /* prime the driver with packets                                          */
    mcbspDriverPrime();

    /* reclaim each packet and resubmit for "LOOP_COUNT" number of iteration  */
    for (count = 0; count < LOOP_COUNT; count++)
    {
        /* Reclaim FULL buffer from the input stream to be reused             */
        nmadus1 = SIO_reclaim(mcbspInHandle,(Ptr *)&rcv,NULL);

        if (nmadus1 < 0)
        {
            LOG_printf(&trace,"Error reclaiming empty buffer from the streams");
        }
        else
        {
            /* compare the received data                                      */
            for (i = 0; i < BUFSIZE; i++)
            {
                if (((Uint8 *)rcv)[i] != (i % 0x100))
                {
                    LOG_printf(&trace,"Iteration %d",count);
                    LOG_printf(&trace,"Data compare failed %x %x",
                        (i % 0x100),((Uint8 *)rcv)[i]);
                    fail = TRUE;
                    break;
                }
            }

            if (TRUE == fail)
            {
                break;
            }

            /* issue the received data to the output stream                   */
            if (IOM_COMPLETED != SIO_issue(mcbspInHandle, rcv, BUFSIZE, NULL))
            {
                LOG_printf(&trace,"Error issuing buffer to the stream");
            }
            pktCount++;
        }
    }

    if (FALSE == fail)
    {
        LOG_printf(&trace,"Sample Application completed sucessfully...");
    }
    else
    {
        LOG_printf(&trace,"Sample Application failed...");
    }
}

/*
 * \brief   Function to submit the requests to the driver
 *
 * \param   None
 *
 * \return  None
 */
static Void mcbspDriverPrime(Void)
{
    Uint32 count = 0;

    for (count = 0; count < NUM_BUFS; count++)
    {
        if (IOM_COMPLETED
            != SIO_issue(mcbspInHandle, (Ptr)buf[count], BUFSIZE, NULL))
        {
            SYS_abort("Issue to output stream failed\n");
        }
    }

    LOG_printf(&trace,"Request submitted to Mcbsp driver.");
}


/*
 * \brief    Function to create the required streams for the reception of
 *           Mcbsp data.
 *
 * \params   None
 *
 * \return   None
 */
static Void mcbspCreateStreams(Void)
{
    SIO_Attrs      sioAttrs  = SIO_ATTRS;
    Uint32         count     = 0;

    sioAttrs.nbufs = NUM_BUFS;
    sioAttrs.align = BUFALIGN;
    sioAttrs.model = SIO_ISSUERECLAIM;

    /* create the channel for the RX operation                                */
    mcbspInHandle = SIO_create("/dioMcbspIN", SIO_INPUT, BUFSIZE, &sioAttrs);

    if (NULL == mcbspInHandle)
    {
        LOG_printf(&trace,"RX Stream creation Failed\n");
        SYS_abort("Stream Creation Failed\n");
    }

    /* create the buffers required for the RX operation                       */
    for (count = 0; count < (NUM_BUFS); count++)
    {
        buf[count] = (Ptr)MEM_calloc(0, BUFSIZE, BUFALIGN);

        if (NULL == buf[count])
        {
            LOG_printf(&trace,"Memory allocaton failed\n");
            SYS_abort("MEM_calloc failed.\n");
        }
    }
}

/*
 * \brief entry point function for the mcbsp demo task.
 *
 *       -> intializes the EDMA driver
 *       -> creates the required channels for the demonstration of the MCBSP
 *          working in evm to evm communication mode.
 *
 * \params  None
 *
 * \return  None.
 */
Void mcbspDemoTask(Void)
{
    /* initialise the edma library                                            */
    if (IOM_COMPLETED != edma3init())
    {
        LOG_printf(&trace,"EDMA intialisation failed");
    }
    else
    {
        LOG_printf(&trace,"EDMA intialised");

        /* update the edma handle to the channel parameters                   */
        mcbspChanparam.edmaHandle = hEdma[0];

        /* call the function to create the channel & demo the transceive      *
         * operation                                                          */
        mcbspStartDemo();
    }
}

/*============================================================================*/
/*                              END OF FILE                                   */
/*============================================================================*/


