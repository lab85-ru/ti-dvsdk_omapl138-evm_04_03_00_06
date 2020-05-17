/*
 * rasterSample_main.c
 *
 * This file contains the test / demo code to demonstrate the LCDC Raster driver 
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

/** \file   rasterSample_main.c
 *
 *  \brief  This file contains the test / demo code to demonstrate the
 *          LCDC driver implemented for DSP/BIOS
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 *  \author     PSP Team
 *  \version    0.1
 */

#include <std.h>

#include "ti/pspiom/psc/Psc.h"
#include "ti/pspiom/cslr/cslr_psc_OMAPL137.h"
#include "ti/pspiom/lcdcraster/Raster.h"


/*
* Global references 
*/
Raster_Params  rasterParams;

/*
 *  Main function of the lcd raster application. Does the pinmuxing for the 
 *  raster module by calling the evm sepcific function.
 */

void main()
{
    return;
}


/*
 * LCDC0 init function called when creating the driver.
 */
void userRasterInit()
{
    Raster_init();

    rasterParams = RasterDefault_PARAMS;
    rasterParams.devConf.clkFreqHz = 5000000;
    /* The Raster can only be configured in this mode                         */
    rasterParams.devConf.opMode = Raster_OpMode_DMAINTERRUPT; 
    rasterParams.devConf.hwiNum = 9;
    rasterParams.devConf.dma.fbMode = Raster_DmaFb_DOUBLE;
    rasterParams.devConf.dma.burstSize = Raster_DmaBurstSize_16;
    rasterParams.devConf.dma.bigEndian = FALSE;
    rasterParams.devConf.dma.eofInt = TRUE;
}


/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
