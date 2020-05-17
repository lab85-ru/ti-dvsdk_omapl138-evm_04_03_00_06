/* --COPYRIGHT--,BSD
 * Copyright (c) 2010, Texas Instruments Incorporated
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

/*
 *  ======== deviceDriverInit.c ========
 */

#include <std.h>

/* DSP/BIOS interfaces files */
#include <pwrm.h>
#include <pwrmL138.h>

/* DSP Platform support package (PSP) interface files */
#include <ti/pspiom/mcasp/Mcasp.h>
#include <ti/pspiom/platforms/codec/Aic31.h>
#include <ti/pspiom/platforms/evmOMAPL138/audio/Audio.h>

/*
 * Mcasp device params. To be filled in userMcaspInit function which
 * is called before driver creation
 */
Mcasp_Params audioMcaspParams;

/*
 * Aic31 device params. To be filled in userAic31Init function which
 * is called before driver creation
 */
Aic31_Params audioAic31Params;

/*
 * Audio device params. To be filled in userAudioInit function which
 * is called before driver creation
 */
Audio_Params audioParams;

/*
 * Aic31 init function called when creating the driver.
 */
void audioUserAic31Init()
{
    Aic31_init();
    audioAic31Params = Aic31_PARAMS;
    audioAic31Params.acCtrlBusName = "/i2c0";
}

/*
 * Mcasp init function called when creating the driver.
 */
void audioUserMcaspInit()
{
    /* Enable EDMA PSC*/
    PWRM_setDependency(PWRM_RSRC_EDMA3_CC_0);
    PWRM_setDependency(PWRM_RSRC_EDMA3_TC_0);
    PWRM_setDependency(PWRM_RSRC_EDMA3_TC_1);
    PWRM_setDependency(PWRM_RSRC_EDMA3_TC_2);

    Mcasp_init();
    audioMcaspParams = Mcasp_PARAMS;
    audioMcaspParams.hwiNumber = 8;
    audioMcaspParams.pscPwrmEnable = FALSE;
}

/*
 * Audio init function called when creating the driver.
 */
void audioUserAudioInit()
{
    Audio_init();
    audioParams = Audio_PARAMS;
    audioParams.adDevType = Audio_DeviceType_McASP;
    audioParams.adDevName = "/mcasp0";
    audioParams.acNumCodecs = 1;
    audioParams.acDevName[0] = "/aic310";
}

/* McAsp HW setup for transmitting data */
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


/* McAsp channel parameters */
Mcasp_ChanParams  mcasp_chanparam[1] = {
    
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

/*
 * Audio driver configuration parameters
 * The sample rate need to match the sample 
 * rate of the PCM audio source file
 */
Audio_ChannelConfig audioChanParamsOUT = {        
    /*  channel 1 (TX) */
    (Ptr)&mcasp_chanparam,      
    {
        /* codec [1] */
        {
            48000,  /* sampling rate           */
            70,     /* gain (%) for codec      */
            0x00,
            0x00
        }
    }
};

