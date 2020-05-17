/*
 * vpifSample_main.c
 *
 * This file contains the test / demo code to demonstrate the VPIF driver
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

/**
 *  \file   vpifSample_main.c
 *
 *  \brief  This file contains the test / demo code to demonstrate the
 *          VPIF driver implemented for DSP/BIOS
 *
 *          This application tests the loop-back of video data. The application
 *          takes the BT.656 capture data and displays it using VPIF. The
 *          default display connection is COMPOSITE and capture connection
 *          is SVIDEO. Different other flavours can be configured using
 *          DISPLAY_CONNECTOR, CAPTURE_CONNECTOR, VIDEO_MODE, VIDEO_STORAGE etc.
 *
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include "ti/pspiom/vpif/Vpif.h"

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */

/* ========================================================================== */
/*                          EXTERNAL REFERENCES                               */
/* ========================================================================== */

/* ========================================================================== */
/*                          GLOBAL REFERENCES                                 */
/* ========================================================================== */
/*
 * VPIF0 device params. To be filled in userVpif0Init function which
 * is called before driver creation
 * Note: This variable is used in BIOS TCF file.
 */
Vpif_Params   vpifParams;

/* ========================================================================== */
/*                       LOCAL FUNCTION PROTOTYPES                            */
/* ========================================================================== */

/* ========================================================================== */
/*                       GLOBAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
Void userVpif0Init();

/* ========================================================================== */
/*                        LOCAL FUNCTION DEFINITIONS                          */
/* ========================================================================== */

/* ========================================================================== */
/*                        GLOBAL FUNCTION DEFINITIONS                         */
/* ========================================================================== */
/*
 * VPIF0 init function called when creating the driver.
 */
Void userVpif0Init()
{
    Vpif_init();
    vpifParams = Vpif_PARAMS;
    vpifParams.hwiNumber = 9u;
    vpifParams.dmaReqSize = Vpif_DmaReqSize_256BYTE;
}

/**
 *  \fn    Void main(Void)
 *
 *  \brief Main function for the VPIF sample application.
 *
 *   Do not call any driver's IO operation here, since interrupts are disabled
 *   in this function
 *
 *  \param  None
 *
 *  \return None
 */
Void main()
{
    return;
}

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
