/*
 * audioSample.c
 *
 * This file contains the test / demo code to demonstrate the McASP driver DIT 
 * mode functionality on DSP/BIOS 5.
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

/** \file   audioSample_main.c
 *
 *  \brief    sample application for demonstration of audio driver usage
 *
 *  This file contains the implementation of the sample appliation for the
 *  demonstration of audio playing through the audio interface layer.
 *
 *             (C) Copyright 2008, Texas Instruments, Inc
 *
 *  \author    Platform Support Group
 *
 *  \version
 *
 *      0.1  Created  [25/06/2008] created newly for OMAPL137 pre silicon platform.
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
#include "string.h"

#include "ti/pspiom/psc/Psc.h"
#include "ti/pspiom/mcasp/Mcasp.h"
#include "ti/pspiom/platforms/evmOMAPL137/McaspDit_evmInit.h"
#include "ti/sdo/edma3/drv/edma3_drv.h"

/* ========================================================================== */
/*                          IMPORTED VARIABLES                                */
/* ========================================================================== */
extern Int edma3init();
extern EDMA3_DRV_Handle hEdma;
extern LOG_Obj trace;

/* ========================================================================== */
/*                           MACRO DEFINTIONS                                 */
/* ========================================================================== */


#define NUM_BUFS   3
#define BUFALIGN   128  /* alignment of buffer for L2 cache                   */

/* Number of serilizer to be used.                                            */
#define TX_NUM_SERIALIZER                      (1u)

/*  Current sine tone would require 64 bytes for a complete
 * tone. We plan to transmit 99 cycles per IO request
 * no of bytes = size for 2 channel * no of cycles = 64 * 4 * 99
 */
#define PSP_99_SINE_CYCLES_BYTE_COUNT          (25344u)


#pragma DATA_ALIGN(txBufferForPkt0,128u)
static Int32           txBufferForPkt0[(PSP_99_SINE_CYCLES_BYTE_COUNT * TX_NUM_SERIALIZER)];

#pragma DATA_ALIGN(txBufferForPkt1,128u)
static Int32           txBufferForPkt1[(PSP_99_SINE_CYCLES_BYTE_COUNT * TX_NUM_SERIALIZER)];

#pragma DATA_ALIGN(txBufferForPkt2,128u)
static Int32           txBufferForPkt2[(PSP_99_SINE_CYCLES_BYTE_COUNT * TX_NUM_SERIALIZER)];



/* The configuration required for the DIT mode                                */
Mcasp_HwSetupData mcasp2XmtSetup = {
        /* .xmask    = */ 0x00FFFFFF, /* All the data bits are to be used     */
        /* .xfmt     = */ 0x000000F0, /*
                                       * 0 bit delay from framsync
                                       * MSB first
                                       * No extra bit padding
                                       * Padding bit (ignore)
                                       * slot Size is 32
                                       * Reads from DMA port
                                       * NO rotation
                                       */
        /* .afsxctl  = */ 0x0000C002, /* burst mode,
                                       * Frame sync is one bit
                                       * Rising edge is start of frame
                                       * externally generated frame sync
                                       */
        /* .xtdm     = */ 0xFFFFFFFF, /* All slots are active (DIT)           */
        /* .xintctl  = */ 0x00000001, /* sync error,overrun error,clK error   */
        /* .xstat    = */ 0x000001FF, /* reset any existing status bits       */
        /* .xevtctl  = */ 0x00000000, /* DMA request is enabled or disabled   */
        {
             /* .aclkxctl  = */ 0x000000A0,  /* Async.internal clock          */
             /* .ahclkxctl = */ 0x00008007,  /* external high clock,No div    */
             /* .xclkchk   = */ 0x00000000
        },

};

Mcasp_ChanParams  mcasp2_chanparam=
{
    0x0001,                   /* number of TDM channels  */
    {Mcasp_SerializerNum_0,},
    &mcasp2XmtSetup,          /* word width              */
    TRUE,                     /* pointer to output buffer*/
    Mcasp_OpMode_DIT,         /* output buf size         */
    Mcasp_WordLength_32,
    NULL,
    0,
    NULL,
    NULL,
    0,
    Mcasp_BufferFormat_1SER_MULTISLOT_INTERLEAVED,
    TRUE,
    FALSE
};


void prime(void);

/* ========================================================================== */
/*                                LOCAL VARIABLES                             */
/* ========================================================================== */


/**< Output channel handle of the McASP instance 0 */
static SIO_Handle  opChanHandle;

/* The Mcasp data packets  */
static Mcasp_PktAddrPayload    mcasp2TxPkt0;
static Mcasp_PktAddrPayload    mcasp2TxPkt1;
static Mcasp_PktAddrPayload    mcasp2TxPkt2;

/* Data that describes a Sine Tone */
static Int16 _sineWavePattern[] = {0,0,6401,6401,12555,12555,18226,18226,23195,23195, \
27269,27269,30293,30293,32150,32150,32767,32767,32122,32122,30240,30240,27191, \
27191,23096,23096,18110,18110,12426,12426,6263,6263,-140,-140,-6538,-6538,     \
-12685,-12685,-18342,-18342,-23293,-23293,-27347,-27347,-30346,-30346,-32177,  \
-32177,-32767,-32767,-32094,-32094,-30185,-30185,-27113,-27113,-22996,-22996,  \
-17993,-17993,-12296,-12296,-6126,-6126};

static Uint32  sizeOf0PktData          = 0x0;
static Uint32  sizeOf1PktData          = 0x0;
static Uint32  sizeOf2PktData          = 0x0;

volatile Uint32 mcasp2TxBufferToBeUsed  = 0xFFu;

/*
 * Mcasp device params. To be filled in userMcaspInit function which
 * is called before driver creation
 */
Mcasp_Params mcasp2Params;


/* mcasp2 module LPSC number      */
#define PSC_MCASP2_LPSC  9

/* ========================================================================== */
/*                           FUNCTION DEFINITIONS                             */
/* ========================================================================== */


/**
 *  \brief  Void main(Void)
 *
 *   Main function of the sample application. This function enables
 *   the mcasp2 instance in the power sleep controller and also
 *   enables the pinmux for the mcasp 2 instance.
 *
 *  \param  None
 *  \return None
 */
Void main(Void)
{
    LOG_printf(&trace,"\r\nAudio Sample Main\n");

    /* configure the mcasp 2 board specific information                       */
    configureMcasp();
    return;
}


/*
 * Mcasp init function called when creating the driver.
 */
void userMcasp2Init()
{
    /* power on the Mcasp 2 instance in the PSC   */
    Psc_ModuleClkCtrl(Psc_DevId_1,PSC_MCASP2_LPSC,TRUE);

    Mcasp_init();

    /* configure the parameters for the DIT mode of Mcasp instance 2 */
    mcasp2Params = Mcasp_PARAMS;
    mcasp2Params.hwiNumber = 8;
    mcasp2Params.enablecache = TRUE;
    mcasp2Params.isDataBufferPayloadStructure = TRUE;

    mcasp2Params.mcaspHwSetup.glb.pdir = 0x14000001;
    mcasp2Params.mcaspHwSetup.glb.ditCtl= 0x00000001;
    mcasp2Params.mcaspHwSetup.tx.clk.clkSetupClk   = 0x00000060;
    mcasp2Params.mcaspHwSetup.tx.clk.clkSetupHiClk = 0x00008000;

}

/**
 * \brief  configureDITTransmission
 *
 *         This function configures McASP instance 2 transmission section to
 *         send audio data for a sine tone at 48KHz and 16 bits per sample.
 *
 * \param  hEdma           Handle to EDMA driver
 * \param  noOfSerilizer   Number of serilizer that is required to be used.
 *
 * \return                 DriverTypes_COMPLETEDif success or suitable error
 *                         code
 *
 */
static Int32  configureDITTransmission(EDMA3_DRV_Handle hEdma)
{
    SIO_Attrs  sioAttrs;

    sioAttrs       = SIO_ATTRS;
    sioAttrs.nbufs = NUM_BUFS;
    sioAttrs.align = BUFALIGN;
    sioAttrs.model = SIO_ISSUERECLAIM;

    mcasp2_chanparam.edmaHandle = hEdma;

    /* open the I/O streams */
    opChanHandle = SIO_create("/dioMcaspOUT",
                              SIO_OUTPUT,
                              sizeof(Mcasp_PktAddrPayload),
                              &sioAttrs);

    if (opChanHandle == NULL)
    {
        LOG_printf(&trace,"\r\nCreate output stream FAILED.\n");
        return IOM_EBADARGS;
    }

    return IOM_COMPLETED;
}


/**
 * \brief   prepareDataForSineTone
 *
 *          This function prepares IO request packets, fills them with data for
 *          sine tone.
 *
 * \return  DriverTypes_COMPLETED
 *
 */
static Int32 prepareDataForSineTone(void)
{
   Int32                  rtnVal             = IOM_COMPLETED;
   Int32                 *tempPtr            = NULL;
   Int16                 *sineToneDataPtr    = NULL;
   Uint32                 counter            = 0x0;
   Uint32                 sineToneCount      = 99u;
   Uint32                 noOfSamplePerCycle = 64u;
   Uint32                 noOfSerilizer      = TX_NUM_SERIALIZER;
   Mcasp_PktAddrPayload  *tempPktPtr         = NULL;

   /* Initialize the buffers                                                  */
   memset((Ptr)(&mcasp2TxPkt0), 0x0, sizeof(mcasp2TxPkt0));
   memset((Ptr)(&mcasp2TxPkt1), 0x0, sizeof(mcasp2TxPkt1));
   memset((Ptr)(&mcasp2TxPkt2), 0x0, sizeof(mcasp2TxPkt2));

   /* initialize the buffers                                                  */
   memset((Ptr)(&txBufferForPkt0), 0x00, sizeof(txBufferForPkt0));
   memset((Ptr)(&txBufferForPkt1), 0x00, sizeof(txBufferForPkt1));
   memset((Ptr)(&txBufferForPkt2), 0x00, sizeof(txBufferForPkt2));

   /* Start with buffer 1 */
   tempPtr     = txBufferForPkt0;
   tempPktPtr  = &mcasp2TxPkt0;

   while (counter < NUM_BUFS)
   {
       /* Align the buffer to cache line length */
       /* Frame the IO request packet */
       tempPktPtr->addr            = (Uint32 *)tempPtr;
       tempPktPtr->chStat          = NULL;
       tempPktPtr->userData        = NULL;
       tempPktPtr->writeDitParams  = FALSE;

       /* The tempPtr is not yet on the aligned boundary */
       tempPtr = (Int32 *)tempPktPtr->addr;

       /* For all serilizers */
       while(noOfSerilizer)
       {
           /* Store 99 cycles of sine wave */
           while(sineToneCount)
           {
               /* Initialize sineToneDataPtr to start */
               sineToneDataPtr = _sineWavePattern;
               while(noOfSamplePerCycle)
               {
                   /* Store the data */
                   *tempPtr = (Uint32) *sineToneDataPtr;

                   noOfSamplePerCycle--;
                   tempPtr++;
                   sineToneDataPtr++;
               }
               sineToneCount--;
               /* Let the sine tone loop start again */
               noOfSamplePerCycle = 64u;
           }
           noOfSerilizer--;
           /* Let no of sine tones start again */
           sineToneCount = 99u;
       }

       counter++;
       noOfSerilizer = TX_NUM_SERIALIZER;

       if (counter == (0x01u))
       {
           tempPtr     = txBufferForPkt1;
           tempPktPtr  = &mcasp2TxPkt1;
       }
       else
       {
           tempPtr     = txBufferForPkt2;
           tempPktPtr  = &mcasp2TxPkt2;
       }
   }
   /* No of sample for 2 slots * no of bytes per sample * no of sine waves
      Note that 16 bit sample is represented in 32 bit word, since McASP
      would require 32 bits of data to be written irrespective of the
      configured slot width                                                    */

   sizeOf0PktData = (64u) * (4u) * (99u);
   sizeOf1PktData = sizeOf0PktData;
   sizeOf2PktData = sizeOf0PktData;

   /* Indicate buffers are READY                                               */
   mcasp2TxBufferToBeUsed = 0x0;

   return rtnVal;
}


/**
 * Length of buffer to be transmitted when no data is available for
 * transmission.
 *
 * CAUTION: The length of this buffer should large enough to satisfy the data
 *             requirements of all the enabled serializer and slots.
 *             In the case of PCM1602A by default we have 3 serializer enabled
 *             and two slots in each where each slot is 32 bit wide.
 *             Note that size is specified for 1 serailizer only and the driver
 *             would expect null tone for 3 serializers. This memory is
 *             is allocated and initialized in McASP instance creation function.
 *             Hence <no of slots> * <word width>
 *             which is <2> * <4>
 *
 *             In case of AIC23
 *             <no of slots> * <word width>
 *             i.e. <2> * <4>
 *
 */
#define PSP_NULL_TONE_BUFFER_LENGTH            (8u)

/**
 * \brief  Audio_echo_Task
 *
 *         This task prepares the data for sine tone and plays the same when
 *         enabled.
 *
 *
 * \return none
 */
Void Audio_echo_Task(void)
{
    Int32           status  = IOM_COMPLETED;
    Ptr             xmt     = NULL;
    Int             nmadus  = 0;
    volatile Uint32 count = 0;
    volatile Bool   bForever = TRUE;

    LOG_printf(&trace,"\r\nStarting the Mcasp Sample application\n");

    /* initialise the edma library                                            */
    edma3init();

    if (NULL != hEdma)
    {
        /* initialise the Mcasp driver in the DIT mode                        */
        status = configureDITTransmission(hEdma);
        LOG_printf(&trace,"\r\nConfigured McASP2 to transmit\n");

        /* check if the initialisation od the Mcasp parameters is sucessful   */
        if (IOM_COMPLETED == status)
        {
            /* prepare the data to be sent out                                */
            prepareDataForSineTone();

            /* set the buffer to be loaded as 0x00                            */
            mcasp2TxBufferToBeUsed = 0x0;

            /* Prime McASP2 with data                                         */
            prime();

            /* Loop forver                                                    */
            while (bForever)
            {
                /* Reclaim empty buffer from the output stream to be reused   */
                nmadus = SIO_reclaim(opChanHandle,(Ptr *)&xmt,NULL);

                if (nmadus < 0)
                {
                    LOG_printf(&trace,"\r\nError reclaiming empty buffer from the output stream\n");
                }

                if (mcasp2TxBufferToBeUsed == 0x0)
                {
                    mcasp2TxBufferToBeUsed = 0x1u;

                    if (SYS_OK != SIO_issue(opChanHandle,
                                            &mcasp2TxPkt0,
                                            (sizeOf0PktData * TX_NUM_SERIALIZER),
                                            NULL))
                    {
                        LOG_printf(&trace,"\r\nFailed to issue empty buffer to stream \n");
                    }
                }
                else if (mcasp2TxBufferToBeUsed == 0x1u)
                {
                    mcasp2TxBufferToBeUsed = 0x2u;

                    if (SYS_OK != SIO_issue(opChanHandle,
                                            &mcasp2TxPkt1,
                                            (sizeOf1PktData * TX_NUM_SERIALIZER),
                                            NULL))
                    {
                        LOG_printf(&trace,"\r\nFailed to issue empty buffer to stream\n");
                    }
                }
                else
                {
                    mcasp2TxBufferToBeUsed = 0x0;

                    /* Issue the third buffers to the output stream           */
                    if (SYS_OK != SIO_issue(opChanHandle,
                                            &mcasp2TxPkt2,
                                            (sizeOf2PktData * TX_NUM_SERIALIZER),
                                            NULL))
                    {
                        LOG_printf(&trace,"\r\nFailed to issue empty buffer to stream \n");
                    }
                }
            }
        }
    }
    else
    {
        LOG_printf(&trace,"\r\nNull edma Handle detected\n");
    }
}

/*
 * ======== prime ========
 *
 * Function to prime the driver with data.
 */
void prime(void)
{
    /* Issue the first buffers to the output stream                           */
    if (SYS_OK != SIO_issue(opChanHandle,
                            &mcasp2TxPkt0,
                            (sizeOf0PktData * TX_NUM_SERIALIZER),
                            NULL))
    {
        LOG_printf(&trace,"\r\nFailed to issue empty buffer to stream\n");
    }

    /* Issue the second buffers to the output stream                          */
    if (SYS_OK != SIO_issue(opChanHandle,
                            &mcasp2TxPkt1,
                            (sizeOf1PktData * TX_NUM_SERIALIZER),
                            NULL))
    {
        LOG_printf(&trace,"\r\nFailed to issue empty buffer to stream\n");
    }

    /* Issue the third buffers to the output stream                           */
    if (SYS_OK != SIO_issue(opChanHandle,
                            &mcasp2TxPkt2,
                            (sizeOf2PktData * TX_NUM_SERIALIZER),
                            NULL))
    {
        LOG_printf(&trace,"\r\nFailed to issue empty buffer to stream\n");
    }
}
/* ========================================================================== */
/*                                END OF FILE                                 */
/* ========================================================================== */
