/*
 * audioSample_main.c
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

/** \file   audioSample_main.c
 *
 *  \brief    sample application for demonstration of audio driver usage
 *
 *  This file contains the implementation of the sample appliation for the
 *  demonstration of audio playing through the audio interface layer.
 *
 *             (C) Copyright 2008, Texas Instruments, Inc
 */

/* ========================================================================== */
/*                            INCLUDE FILES                                   */
/* ========================================================================== */

#include <stdio.h>
#include <std.h>
#include <log.h>
#include "ti/pspiom/psc/Psc.h"
#include "ti/pspiom/cslr/cslr_psc_C6747.h"
#include "ti/pspiom/mcasp/Mcasp.h"
#include "ti/pspiom/cslr/cslr_psc_C6747.h"
#include "ti/pspiom/platforms/codec/Aic31.h"
#include "ti/pspiom/platforms/evm6747/audio/Audio.h"
#include "ti/pspiom/platforms/evm6747/Audio_evmInit.h"

extern LOG_Obj trace;

/* ========================================================================== */
/*                           MACRO DEFINTIONS                                 */
/* ========================================================================== */

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

/* ========================================================================== */
/*                           FUNCTION DEFINITIONS                             */
/* ========================================================================== */


/**
 *  \brief  Void main(Void)
 *
 *   Main function of the sample application. This function enables
 *   the mcasp instance in the power sleep controller and also
 *   enables the pinmux for the mcasp 1 instance.
 *
 *  \param  None
 *  \return None
 */
Void main(Void)
{
    LOG_printf(&trace,"\r\nAudio Sample Main\n");

    /* call the function to configure the evm specific options                */
    configureAudio();

    return;
}


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
    Mcasp_init();
    audioMcaspParams = Mcasp_PARAMS;
    audioMcaspParams.hwiNumber = 8;
}

/*
 * Audio init function called when creating the driver.
 */
void audioUserAudioInit()
{
    Audio_init();
    audioParams = Audio_PARAMS;
    audioParams.adDevType = Audio_DeviceType_McASP;
    audioParams.adDevName = "/mcasp1";
    audioParams.acNumCodecs = 1;
    audioParams.acDevName[0] = "/aic310";
}

/* ========================================================================== */
/*                                END OF FILE                                 */
/* ========================================================================== */
