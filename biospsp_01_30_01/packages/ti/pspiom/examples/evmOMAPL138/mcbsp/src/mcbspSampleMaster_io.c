/*
 * mcbspSampleMaster_io.c
 *
 * This file contains the test / demo code to demonstrate the McBSP driver
 * master functionality on DSP/BIOS 5.
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
 * \file    mcbspSampleMaster_io.c
 *
 * \brief   McBSP evm to evm communication sample application
 *
 *          This file demonstrates the use of Mcbsp is by using an
 *          EVM to EVM communication setup.This file configures one of the evm
 *          in master mode.
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

#define NUM_BUFS              1             /* Max of 1 outstanding requests  */
#define BUFALIGN              128           /* align buffers to 128 bytes     */
#define BUFSIZE               1024          /* 1K of data transceive          */
#define LOOP_COUNT            100

#define DEFAULT_BITPERSAMPLE  8             /* number of bits per slot        */
#define NUM_OF_CHANNELS       1             /* Number of slots to be used     */

/* ========================================================================== */
/*                          LOCAL VARIABLES                                   */
/* ========================================================================== */

/* Handles for the TX channels                            */
static SIO_Handle mcbspOutHandle = NULL;

/* array to hold the pointer to the allocated buffers     */
Ptr buf[NUM_BUFS];

Uint32 pktCount = 0;

/**< settings to configure the TX or RX hardware sections                 */
Mcbsp_DataConfig mcbspChanConfig =
{
    Mcbsp_Phase_SINGLE,
    Mcbsp_WordLength_8,
    Mcbsp_WordLength_8,    /* Dont care for single phase*/
    NUM_OF_CHANNELS,
    NUM_OF_CHANNELS,       /* Dont care for single phase*/
    Mcbsp_FrmSync_DETECT,
    Mcbsp_DataDelay_1_BIT,
    Mcbsp_Compand_OFF_MSB_FIRST,
    Mcbsp_BitReversal_DISABLE,
    Mcbsp_IntMode_ON_SYNCERR,
    Mcbsp_RxJust_RZF,  /* Dont care for TX         */
    Mcbsp_DxEna_OFF
};

/**< clock setup for the TX section                     */
Mcbsp_ClkSetup mcbspClkConfig =
{
    Mcbsp_FsClkMode_INTERNAL,
    96000,                   /* 96KHz                   */
    Mcbsp_TxRxClkMode_INTERNAL,
    Mcbsp_FsPol_ACTIVE_HIGH,
    Mcbsp_ClkPol_RISING_EDGE
};

/**< Multi channel setup                                                      */
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
    Ptr                xmt       = NULL;
    Uint32             count     = 0;
    Int32              nmadus0   = 0;

    /* create the streams required for the transactions                       */
    mcbspCreateStreams();

    /* prime the driver                                                       */
    mcbspDriverPrime();

    /* reclaim each packet and resubmit for "LOOP_COUNT" number of iteration  */
    for (count = 0; count < LOOP_COUNT; count++)
    {
        /* Reclaim empty buffer from the output stream to be reused           */
        nmadus0 = SIO_reclaim(mcbspOutHandle,(Ptr *)&xmt,NULL);

        if (nmadus0 < 0)
        {
            LOG_printf(
                &trace,
                "Error reclaiming empty buffer from the streams %x %d",
                *(Uint32 *)xmt,
                nmadus0);
        }

        TSK_sleep(100);

        /* submit the request for the transmit data                           */
        if (IOM_COMPLETED != SIO_issue(mcbspOutHandle,xmt, BUFSIZE, NULL))
        {
            LOG_printf(&trace,"Error issuing buffer to the stream");
        }

        pktCount++;
    }
}


/*
 * \brief   Function to submit request the driver.Depending on the macro
 *          "NUM_BUFS" either the driver can be buffered with multiple requests
 *          or only one buffer by using the NUM_BUFS as 1.
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
            != SIO_issue(mcbspOutHandle, (Ptr)buf[count], BUFSIZE, NULL))
        {
            LOG_printf(&trace,"Issue to output stream failed.");
            SYS_abort("Issue to output stream failed\n");
        }
    }

    LOG_printf(&trace,"Requests submitted to the Mcbsp driver.");
}


/*
 * \brief    Function to create the required streams for the transmission of
 *           Mcbsp data.
 *
 * \params   NONE
 *
 * \return   NONE
 */
Void mcbspCreateStreams(Void)
{
    SIO_Attrs      sioAttrs  = SIO_ATTRS;
    Uint32         count     = 0;
    Uint32         tempCount = 0;

    sioAttrs.nbufs = NUM_BUFS;
    sioAttrs.align = BUFALIGN;
    sioAttrs.model = SIO_ISSUERECLAIM;

    /* Create a TX channel for the transmission                               */
    mcbspOutHandle = SIO_create("/dioMcbspOUT", SIO_OUTPUT, BUFSIZE, &sioAttrs);

    if (NULL == mcbspOutHandle)
    {
        LOG_printf(&trace,"Stream Creation Failed");
        SYS_abort("Stream Creation Failed\n");
    }

    /* create the buffers required for the TX  operation                      */
    for (count = 0; count < (NUM_BUFS ); count++)
    {
        buf[count] = (Ptr)MEM_calloc(0, BUFSIZE, BUFALIGN);

        if (NULL == buf[count])
        {
            LOG_printf(&trace,"MEM_calloc failed.");
            SYS_abort("MEM_calloc failed.\n");
        }
    }

    /* Fill the buffers with known data and transmit the same and check if the*
     * same pattern is received on the other EVM                              */
    for (count = 0; count < (NUM_BUFS); count++)
    {
        for (tempCount = 0; tempCount < BUFSIZE; tempCount++)
        {
            ((Uint8 *)buf[count])[tempCount] = (tempCount % 0x100);
        }
    }
}

/*
 * \brief entry point function for the mcbsp demo task.
 *
 *       -> Intializes the EDMA driver
 *       -> creates the required channels for the demonstration of the MCBSP
 *          working in evm to evm communication mode.
 *
 * \params  None
 *
 * \return  None.
 */
Void mcbspDemoTask(Void)
{
    Int32    status = IOM_COMPLETED;

    /* initialise the edma library                                            */
    status = edma3init();

    if (EDMA3_DRV_SOK != status)
    {
        LOG_printf(&trace,"EDMA intialisation failed");
    }
    else
    {
        LOG_printf(&trace,"EDMA intialised");

        /* update the edma handle to the channel parameters.Need to use       *
         * EDMA instance 0 only for Mcbsp 1                                   */
        mcbspChanparam.edmaHandle = hEdma[0];

        /* call the function to create the channel & demo the transceive      *
         * operation                                                          */
        mcbspStartDemo();

        LOG_printf(&trace,"Sample Application completed sucessfully...");
    }
}

/*============================================================================*/
/*                              END OF FILE                                   */
/*============================================================================*/


