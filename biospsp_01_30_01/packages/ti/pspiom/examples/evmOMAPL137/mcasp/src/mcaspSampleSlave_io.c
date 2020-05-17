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
 * \file     mcaspSampleSlave_io.c
 *
 * \brief    sample application for demostrating Mcasp evm to EVM communication
 *
 *           This file contains the implementation of the mcasp acting as a 
 *           slave receiver. it also compares the received  data and prints the 
 *           appropriate error messages.
 *
 *           (C) Copyright 2008, Texas Instruments, Inc
 */

/* ========================================================================== */
/*                            INCLUDE FILES                                   */
/* ========================================================================== */

#include <stdio.h>
#include <string.h>
#include <std.h>
#include <sio.h>
#include <iom.h>
#include <mem.h>
#include <log.h>
#include <ti/pspiom/mcasp/Mcasp.h>
#include <ti/sdo/edma3/drv/edma3_drv.h>

/* ========================================================================== */
/*                          IMPORTED VARIABLES                                */
/* ========================================================================== */

extern Int edma3init();
extern EDMA3_DRV_Handle hEdma;
extern LOG_Obj trace;

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */

/**
 * Buffers placed in external memory are aligned on a 128 bytes boundary.
 * In addition, the buffer should be of a size multiple of 128 bytes for
 * the cache work optimally on the C6x.
 */
#define BUFLEN                  2560 /* number of samples in the frame        */
#define BUFALIGN                128  /* alignment of buffer for L2 cache      */

#define BUFSIZE                 (BUFLEN * 2)

#define LOOP_COUNT              100

/* inStream is SIO handles created in main */
static SIO_Handle inStream;

/* Function prototype */
static Void createStreams();
static Void prime();

Ptr buf[4];


Uint32 numOfBufs = 0x00;

Mcasp_HwSetupData mcaspRcvSetup = 
{ 
    /* .rmask    = */ 0xFFFFFFFF,      /* all Data bits are used              */
    /* .rfmt     = */ 0x000080F0,      /* 1 bit dly,MSB first,16 bit,No Rotate*/
    /* .afsrctl  = */ 0x00000080,      /* internally generated fsync          */
    /* .rtdm     = */ 0x00000001,      /* only one slot active                */
    /* .rintctl  = */ 0x00000003,      /* underrun and sync error active      */
    /* .rstat    = */ 0x000001FF,      /* reset any existing status bits      */
    /* .revtctl  = */ 0x00000000,      /* DMA mode                            */
    {
         /* .aclkrctl  = */ 0x00000000,
         /* .ahclkrctl = */ 0x00000000,
         /* .rclkchk   = */ 0x00000000
    }
};


/* McAsp channel parameters                                */
Mcasp_ChanParams  mcasp_chanparam=
{
    0x0001,                     /* number of serializers   */
    {Mcasp_SerializerNum_5,},
    (Mcasp_HwSetupData *)&mcaspRcvSetup,
    TRUE,
    Mcasp_OpMode_TDM,           /* TDM MODE                */
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

    mcasp_chanparam.edmaHandle = hEdma;

    /* open the I/O streams for receive      */
    inStream = SIO_create("/dioMcaspIN", SIO_INPUT, BUFSIZE, &sioAttrs);

    if (NULL != inStream)
    {
        /* check if the driver is compiled in Non loop job mode for this app  */
        status = SIO_ctrl(inStream,Mcasp_IOCTL_QUERY_LOOPJOB_MODE,(Arg)&loopJobMode);

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

    /* Allocate buffers for the SIO buffer exchanges                          */
    for(count = 0; count < (numOfBufs); count ++)
    {
        buf[count] = (Ptr)MEM_calloc(0, BUFSIZE, BUFALIGN);
        
        if(NULL == buf[count])
        {
            LOG_printf(&trace,"ERROR : MEM_calloc failed.");
            SYS_abort("MEM_calloc failed.");
        }
    }

    for(count = 0; count < numOfBufs; count ++)
    {
        /* Issue the buffers to the input stream                              */
        SIO_issue(inStream, buf[count], BUFSIZE, NULL);
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
    volatile int count     = 0;
    volatile int tempCount = 0;
    Int          nmadus    = 0;
    Uint8       *tempPtr   = NULL;
    Ptr          rcv       = NULL;
    Uint32       option    = 0x00;
    Int32        status    = IOM_COMPLETED;

    /* initialise the edma library                                            */
    edma3init();

    for (option = 0x00; option < 2;  option++)
    {
        /* Call createStream function to create I/O streams                   */
        createStreams(option);

        /* Call prime function to do priming                                  */
        prime();

        /* loop to continously receive data                                   */
        for (count = 0; count < LOOP_COUNT; count++)
        {
            /* Reclaim full buffer from the input stream to be reused         */
            nmadus = SIO_reclaim(inStream, (Ptr *)&rcv, NULL);
        
            if (nmadus < 0)
            {
                LOG_printf(&trace,
                    "Error reclaiming empty buffer from the output stream");
            }

            tempPtr = (Uint8 *)rcv;
        
            /* compare the data and log a message in case of an error         */
            for (tempCount = 0; tempCount < (BUFSIZE); tempCount++)
            {
                if (tempPtr[tempCount] != (tempCount % 0x100))
                {
                    LOG_printf(&trace,
                        "Data compare Failed. iteration %d\n", count);
					break;
                }
            }

            memset(tempPtr,0x00,BUFSIZE);

            /* Issue empty buffer to the input stream                         */
            if (SIO_issue(inStream, rcv, BUFSIZE, NULL) != SYS_OK)
            {
                LOG_printf(&trace,
                    "Failed to issue empty buffer to stream");
            }
        }

        /* reclaim the remaining buffers                                      */
        for (count = 0; count < (numOfBufs); count++)
        {
            nmadus = SIO_reclaim(inStream, (Ptr *)&rcv, NULL);
    
            if (nmadus < 0)
            {
                LOG_printf(&trace,
                    "Error reclaiming empty buffer from the output stream");
            }
        }

        status = SIO_delete(inStream);

        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace,"ERROR : Channel delete Failed.");
            SYS_abort("Error deleting the channel");
        }
        else
        {
            LOG_printf(&trace,"    RX Data compare pass");
        }
    }
    LOG_printf(&trace,"Sample application completed");
}
