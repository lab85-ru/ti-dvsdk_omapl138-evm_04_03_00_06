/*
 * audioSample_io.c
 *
 * This file contains the test / demo code to demonstrate the Audio component 
 * driver functionality on DSP/BIOS 5.
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


/** \file     audioSample_io.c
 *
 *  \brief    sample application for demostration of audio playing
 *
 *  This file contains the implementation of the sample appliation for the
 *  demonstration of audio playing through the audio interface layer.
 *
 *             (C) Copyright 2009, Texas Instruments, Inc
 */

/* ========================================================================== */
/*                            INCLUDE FILES                                   */
/* ========================================================================== */

#include "stdio.h"
#include "std.h"
#include "sio.h"
#include "iom.h"
#include "mem.h"
#include "log.h"
#include "ti/pspiom/platforms/evmOMAPL138/audio/Audio.h"
#include "ti/pspiom/mcasp/Mcasp.h"
#include "ti/sdo/edma3/drv/edma3_drv.h"

/* ========================================================================== */
/*                          IMPORTED VARIABLES                                */
/* ========================================================================== */

extern Int edma3init();
extern EDMA3_DRV_Handle hEdma[];
extern LOG_Obj trace;

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */

/*
 * Buffers placed in external memory are aligned on a 128 bytes boundary.
 * In addition, the buffer should be of a size multiple of 128 bytes for
 * the cache work optimally on the C6x.
 */
#define BUFLEN                  2560         /* number of samples in the frame */
#define BUFALIGN                128 /* alignment of buffer for use of L2 cache */


/** Number of serializers configured for record */
#define RX_NUM_SERIALIZER       (1u)
#define TX_NUM_SERIALIZER       (1u)

#define BUFSIZE                 (BUFLEN * sizeof(Ptr))

#define NUM_BUFS                4   /* Num Bufs to be issued and reclaimed */

/* inStream and outStream are stream handles created in main */
static SIO_Handle inStream, outStream;

/* Function prototype */
static Void createStreams();
static Void prime();

Ptr buf[NUM_BUFS * 2];

Mcasp_HwSetupData mcaspRcvSetup = {
        /* .rmask    = */ 0xFFFFFFFF, /* All the data bits are to be used     */
        /* .rfmt     = */ 0x000080F0, /*
                                       * 0 bit delay from framsync
                                       * MSB first
                                       * No extra bit padding
                                       * Padding bit (ignore)
                                       * slot Size is 32
                                       * Reads from DMA port
                                       * NO rotation
                                       */
        /* .afsrctl  = */ 0x00000000, /* burst mode,
                                       * Frame sync is one bit
                                       * Rising edge is start of frame
                                       * externally generated frame sync
                                       */
        /* .rtdm     = */ 0x00000001, /* slot 1 is active (DSP)               */
        /* .rintctl  = */ 0x00000003, /* sync error and overrun error         */
        /* .rstat    = */ 0x000001FF, /* reset any existing status bits       */
        /* .revtctl  = */ 0x00000000, /* DMA request is enabled or disabled   */
        {
             /* .aclkrctl  = */ 0x00000000,
             /* .ahclkrctl = */ 0x00000000,
             /* .rclkchk   = */ 0x00000000
        }
} ;

Mcasp_HwSetupData mcaspXmtSetup = {
        /* .xmask    = */ 0xFFFFFFFF, /* All the data bits are to be used     */
        /* .xfmt     = */ 0x000080F0, /*
                                       * 0 bit delay from framsync
                                       * MSB first
                                       * No extra bit padding
                                       * Padding bit (ignore)
                                       * slot Size is 32
                                       * Reads from DMA port
                                       * NO rotation
                                       */
        /* .afsxctl  = */ 0x00000000, /* burst mode,
                                       * Frame sync is one bit
                                       * Rising edge is start of frame
                                       * externally generated frame sync
                                       */
        /* .xtdm     = */ 0x00000001, /* slot 1 is active (DSP)               */
        /* .xintctl  = */ 0x00000007, /* sync error,overrun error,clK error   */
        /* .xstat    = */ 0x000001FF, /* reset any existing status bits       */
        /* .xevtctl  = */ 0x00000000, /* DMA request is enabled or disabled   */
        {
             /* .aclkxctl  = */ 0x00000000,
             /* .ahclkxctl = */ 0x00000000,
             /* .xclkchk   = */ 0x00000000
        },

};


/* McBsp channel parameters                                  */
Mcasp_ChanParams  mcasp_chanparam[Audio_NUM_CHANS]=
{
    {
        0x0001,                    /* number of serialisers      */
        {Mcasp_SerializerNum_12, }, /* serialiser index           */
        &mcaspRcvSetup,
        TRUE,
        Mcasp_OpMode_TDM,          /* Mode (TDM/DIT)             */
        Mcasp_WordLength_32,
        NULL,
        0,
        NULL,
        NULL,
        1,                        /* number of TDM channels      */
        Mcasp_BufferFormat_1SER_1SLOT,
        TRUE,
        TRUE
    },
    {
        0x0001,                   /* number of serialisers       */
        {Mcasp_SerializerNum_11,},
        &mcaspXmtSetup,
        TRUE,
        Mcasp_OpMode_TDM,
        Mcasp_WordLength_32,      /* word width                  */
        NULL,
        0,
        NULL,
        NULL,
        1,                        /* number of TDM channels      */
        Mcasp_BufferFormat_1SER_1SLOT,
        TRUE,
        TRUE
    }
};

Audio_ChannelConfig audioChanParamsIN =
{
    /*  channel 0 (RX)                         */
    (Ptr)&mcasp_chanparam[0], 
    {   /* codec [0]                           */
        {
            44100,  /* sampling rate for codec */
               30,  /* gain (%) for codec      */
             0x00,
             0x00
        }
    }
};

Audio_ChannelConfig audioChanParamsOUT =
{        
    /*  channel 1 (TX)                         */
    (Ptr)&mcasp_chanparam[1],      
    {
        /* codec [1]                           */
        {
            44100,  /* sampling rate           */
               70,  /* gain (%) for codec      */
             0x00,
             0x00
        }
    }
};

/*
 * ======== createStreams ========
 */
static Void createStreams()
{
    SIO_Attrs sioAttrs;

    sioAttrs       = SIO_ATTRS;
    sioAttrs.nbufs = NUM_BUFS;
    sioAttrs.align = BUFALIGN;
    sioAttrs.model = SIO_ISSUERECLAIM;

    mcasp_chanparam[0].edmaHandle = hEdma[0];
    mcasp_chanparam[1].edmaHandle = hEdma[0];

    /* open the I/O streams */
    outStream = SIO_create("/dioAudioOUT", SIO_OUTPUT, BUFSIZE, &sioAttrs);

    if (outStream == NULL)
    {
        LOG_printf(&trace,"\r\nCreate output stream FAILED.\n");
        return;
    }

    inStream = SIO_create("/dioAudioIN", SIO_INPUT, BUFSIZE, &sioAttrs);

    if (inStream == NULL)
    {
        LOG_printf(&trace,"\r\nCreate input stream FAILED.\n");
        return;
    }
}

/*
 * ======== prime ========
 */
static Void prime()
{
    Int32        count = 0;

    /* Allocate buffers for the SIO buffer exchanges                          */
    for(count = 0; count < (NUM_BUFS ); count ++)
    {
        buf[count] = (Ptr)MEM_calloc(0, BUFSIZE * RX_NUM_SERIALIZER, BUFALIGN);
        if(NULL == buf[count])
        {
            LOG_printf(&trace,"\r\nMEM_calloc failed.\n");
        }
    }

    /* Allocate buffers for the SIO buffer exchanges                          */
    for(count = NUM_BUFS; count < (NUM_BUFS * 2); count ++)
    {
        buf[count] = (Ptr)MEM_calloc(0, BUFSIZE * TX_NUM_SERIALIZER, BUFALIGN);
        if(NULL == buf[count])
        {
            LOG_printf(&trace,"\r\nMEM_calloc failed.\n");
        }
    }

    for(count = 0; count < NUM_BUFS; count ++)
    {
        /* Issue the first & second empty buffers to the input stream         */
        SIO_issue(inStream, buf[count], BUFSIZE * RX_NUM_SERIALIZER, NULL);
    }

    for(count = NUM_BUFS; count < (NUM_BUFS * 2); count ++)
    {
        SIO_issue(outStream, buf[count], BUFSIZE * TX_NUM_SERIALIZER,NULL);
    }
}

/*
 * ======== echo ========
 * This function copies from the input SIO to the output SIO. You could
 * easily replace the copy function with a signal processing algorithm.
 */
Void Audio_echo_Task()
{
    volatile int i32Count;
    Int nmadus = 0;
    Ptr rcv,xmt;

    /* initialise the edma library                                            */
    edma3init();

    /* Call createStream function to create I/O streams                       */
    createStreams();

    /* Call prime function to do priming                                      */
    prime();

    /* Forever loop to continously receviec and transmit audio data           */
    for (i32Count = 0; i32Count >= 0; i32Count++)
    {
        nmadus = SIO_reclaim(inStream, (Ptr *)&rcv, NULL);
        
        /* Reclaim full buffer from the input stream                          */
        if (nmadus < 0)
        {
            LOG_printf(&trace,"\r\nError reclaiming full buffer from the input stream\n");
        }

        /* Reclaim empty buffer from the output stream to be reused           */
        nmadus = SIO_reclaim(outStream, (Ptr *)&xmt, NULL);
        if (nmadus < 0)
        {
            LOG_printf(&trace,"\r\nError reclaiming empty buffer from the output stream\n");
        }

        /* copy the receive information to the transmit buffer                */
        memcpy(xmt,rcv,BUFSIZE * RX_NUM_SERIALIZER);

        /* Issue full buffer to the output stream                             */
        if (SYS_OK != SIO_issue(outStream, xmt, BUFSIZE * TX_NUM_SERIALIZER, NULL))
        {
            LOG_printf(&trace,"\r\nFailed to issue empty buffer to stream\n");
        }

        /* Issue an empty buffer to the input stream                          */
        if (SYS_OK != SIO_issue(inStream, rcv, BUFSIZE * RX_NUM_SERIALIZER, NULL))
        {
            LOG_printf(&trace,"\r\nFailed to issue empty buffer to stream\n");
        }
    }
}
