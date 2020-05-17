/******************************************************************************
**+-------------------------------------------------------------------------+**
**|                            ****                                         |**
**|                            ****                                         |**
**|                            ******o***                                   |**
**|                      ********_///_****                                  |**
**|                      ***** /_//_/ ****                                  |**
**|                       ** ** (__/ ****                                   |**
**|                           *********                                     |**
**|                            ****                                         |**
**|                            ***                                          |**
**|                                                                         |**
**|         Copyright (c) 2008-2010 Texas Instruments Incorporated          |**
**|                        ALL RIGHTS RESERVED                              |**
**|                                                                         |**
**| Permission is herbiy granted to licensees of Texas Instruments          |**
**| Incorporated (TI) products to use this computer program for the sole    |**
**| purpose of implementing a licensee product based on TI products.        |**
**| No other rights to reproduce, use, or disseminate this computer         |**
**| program, whether in part or in whole, are granted.                      |**
**|                                                                         |**
**| TI makes no representation or warranties with respect to the            |**
**| performance of this computer program, and specifically disclaims        |**
**| any responsibility for any damages, special or consequential,           |**
**| connected with the use of this program.                                 |**
**|                                                                         |**
**+-------------------------------------------------------------------------+**
******************************************************************************/

/**
 * \file     mcaspSampleMaster_io.c
 *
 * \brief    sample application for demostrating Mcasp evm to EVM communication
 *
 *           This file contains the implementation of the mcasp acting as a 
 *           master transmitter. it sends a known pattern of data.
 *
 *           (C) Copyright 2008, Texas Instruments, Inc
 */

/* ========================================================================== */
/*                            INCLUDE FILES                                   */
/* ========================================================================== */

#include <stdio.h>
#include <std.h>
#include <sio.h>
#include <iom.h>
#include <mem.h>
#include <tsk.h>
#include <log.h>
#include <ti/pspiom/mcasp/Mcasp.h>
#include <ti/sdo/edma3/drv/edma3_drv.h>

/* ========================================================================== */
/*                          IMPORTED VARIABLES                                */
/* ========================================================================== */

extern Int edma3init();
extern EDMA3_DRV_Handle hEdma[];
extern LOG_Obj trace;

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */

/**
 * Buffers placed in external memory are aligned on a 128 bytes boundary.
 * In addition, the buffer should be of a size multiple of 128 bytes for
 * the cache work optimally on the C6x.
 */
#define BUFLEN                  2560 /* number of samples in the frame */
#define BUFALIGN                128  /* alignment of buffer for L2 cache */

#define BUFSIZE                 (BUFLEN * 2)

#define LOOP_COUNT              100


/* outStream is SIO handles created in main */
static SIO_Handle outStream;

/* Function prototype */
static Void createStreams();
static Void prime();

Ptr buf[4];

Uint32 numOfBufs = 0x00;


Mcasp_HwSetupData mcaspXmtSetup = 
{ 
    /* .xmask    = */ 0xFFFFFFFF,  /* all Data bits are used              */
    /* .xfmt     = */ 0x000080F0,  /* 1 bit dly,MSB first,16 bit,Rotate   */
    /* .afsxctl  = */ 0x00000002,  /* internally generated fsync          */
    /* .xtdm     = */ 0x00000001,  /* only 3 slots active                 */
    /* .xintctl  = */ 0x00000003,  /* underrun and sync error active      */
    /* .xstat    = */ 0x000001FF,  /* reset any existing status bits      */
    /* .xevtctl  = */ 0x00000000,  /* DMA mode                            */
    {
         /* .aclkxctl  = */ 0x00000020,  /* div = 1, clk = internal       */
         /* .ahclkxctl = */ 0x00008004,  /* div by 16                     */
         /* .xclkchk   = */ 0x00000000
    }
};


/* McAsp channel parameters                                */
Mcasp_ChanParams  mcasp_chanparam=
{
    0x0001,                     /* number of serializers   */
    {Mcasp_SerializerNum_9,},
    (Mcasp_HwSetupData *)&mcaspXmtSetup, 
    TRUE,                       /* pointer to output buffer*/
    Mcasp_OpMode_TDM,           /* TDM mode                */
    Mcasp_WordLength_32,        /* word width              */
    NULL,
    0,
    NULL,
    NULL,
    1,
    Mcasp_BufferFormat_1SER_1SLOT,
    TRUE,
    TRUE
};


/*
 * ======== createStreams ========
 */
static Void createStreams(Uint32 option)
{
    SIO_Attrs   sioAttrs;
    Bool        loopJobMode = TRUE;
    Int32       status      = IOM_COMPLETED;

    mcasp_chanparam.edmaHandle = hEdma[0];

    switch (option)
    {
        case 1:
            LOG_printf(&trace,"Primed Mode testing......");
            numOfBufs = 4;
            break;
        case 0:
            LOG_printf(&trace,"Non primed Mode testing..");
            numOfBufs = 1;
            break;
    }

    sioAttrs       = SIO_ATTRS;
    sioAttrs.nbufs = numOfBufs;
    sioAttrs.align = BUFALIGN;
    sioAttrs.model = SIO_ISSUERECLAIM;

    /* open the I/O streams for transmit only                                 */
    outStream = SIO_create("/dioMcaspOUT", SIO_OUTPUT, BUFSIZE, &sioAttrs);

    if (NULL != outStream)
    {
        /* check if the driver is compiled in Non loop job mode for this app  */
        status = SIO_ctrl(outStream,Mcasp_IOCTL_QUERY_LOOPJOB_MODE,(Arg)&loopJobMode);

        if ((IOM_COMPLETED != status) || (TRUE == loopJobMode))
        {
            LOG_printf(&trace,"ERROR : Driver not compiled for non loop job mode");
            status = IOM_EBADMODE;
        }
    }
    else
    {
        status = IOM_EBADARGS;
    }

    /* chcek if the creation of channel is successful                         */
    if (IOM_COMPLETED != status)
    {
        SYS_abort("Channel creation Failed.");
    }
}

/*
 * ======== prime ========
 */
static Void prime()
{
    Int32        count  = 0;
    Uint8       *buffer = NULL;
    Uint32       i      = 0;

    /* Allocate buffers for the SIO buffer exchanges                          */
    for(count = 0; count < (numOfBufs); count ++)
    {
        buf[count] = (Ptr)MEM_calloc(0, BUFSIZE , BUFALIGN);
        
        if (NULL == buf[count])
        {
            LOG_printf(&trace,"ERROR : MEM_calloc failed.");
            SYS_abort("MEM_calloc failed.");
        }
    }

    /* fill the buffer with known pattern  */
    for (i = 0; i < numOfBufs; i++)
    {
        buffer = (Uint8 *)buf[i];

        /* fill the buffer with known pattern and we will transmit the same   */
        for (count = 0; count < (BUFSIZE); count++)
        {
            buffer[count] = count % 0x100;
        }
    }

    for (count = 0; count < numOfBufs; count ++)
    {
        /* Issue the first & second empty buffers to the input stream         */
        SIO_issue(outStream, buf[count], BUFSIZE, NULL);
    }
}

/*
 * ======== Mcasp_echo_task ========
 *
 * This function continously receives the data from mcasp 1 and compares the
 * same.
 */
Void Mcasp_echo_task()
{
    volatile int count     = 0x00;
    Int          nmadus    = 0x00;
    Uint32       option    = 0x00;
    Ptr          xmt       = NULL;
    Int32        status    = IOM_COMPLETED;

    /* initialise the edma library                                            */
    edma3init();

    for (option = 0; option < 2; option++)
    {
        /* Call createStream function to create I/O streams                   */
        createStreams(option);

        /* Call prime function to do priming                                  */
        prime();

        /* Forever loop to continously transmit  data                         */
        for (count = 0; count < LOOP_COUNT; count++)
        {
            /* Reclaim empty buffer from the output stream to be reused       */
            nmadus = SIO_reclaim(outStream, (Ptr *)&xmt, NULL);
        
            if (nmadus < 0)
            {
                LOG_printf(&trace,
                    "Error reclaiming empty buffer from the output stream");
            }

            if (0 ==  option)
            {
                /* Non primed mode hence wait for some time for the slave to  *
                 * syncronize                                                 */
                TSK_sleep(100);
            }
        
            /* Issue full buffer to the output stream                         */
            if (SIO_issue(outStream, xmt, BUFSIZE , NULL) != SYS_OK)
            {
                LOG_printf(&trace,
                    "Failed to issue empty buffer to stream");
            }
        }

        /* reclaim the remaining buffers                                      */
        for (count = 0; count < (numOfBufs); count++)
        {
            nmadus = SIO_reclaim(outStream, (Ptr *)&xmt, NULL);
        
            if (nmadus < 0)
            {
                LOG_printf(&trace,
                    "Error reclaiming empty buffer from the output stream");
            }
        }

        status = SIO_delete(outStream);

        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace,"ERROR : Channel delete Failed.");
            SYS_abort("Error deleting the channel");
        }
        else
        {
            LOG_printf(&trace,"    TX Done");
            LOG_printf(&trace,"Check slave for the data receive status");
        }

        TSK_sleep(5000);
    }
    LOG_printf(&trace,"Sample application completed");
}
