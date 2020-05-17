/*
 * vpifSample_conversion.c
 *
 * This file contains the code to convert bayer pattern data to YUV422.
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
 *  \file   vpifSample_conversion.c
 *
 *  \brief  This file contains the code to convert bayer pattern data to YUV422.
 *          The MT9T001 generates a Bayer color pattern. The even-numbered rows
 *          contain green and red color pixels, and odd-numbered rows contain
 *          blue and green color pixels. The even-numbered columns contain
 *          green and blue color pixels and odd-numbered columns contain red
 *          and green color pixels.
 *              Bayer Pattern:
 *              GBGBGBGB
 *              RGRGRGRG
 *              GBGBGBGB
 *              RGRGRGRG
 *
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */
#define RGB2YUV(r, g, b, y, u, v)\
  y = (306*r + 601*g + 117*b)  >> 10;\
  u = ((-172*r - 340*g + 512*b) >> 10)  + 128;\
  v = ((512*r - 429*g - 83*b) >> 10) + 128;\
  y = y < 0 ? 0 : y;\
  u = u < 0 ? 0 : u;\
  v = v < 0 ? 0 : v;\
  y = y > 255 ? 255 : y;\
  u = u > 255 ? 255 : u;\
  v = v > 255 ? 255 : v

/* Access even and odd pixels only */
#define EVEN_PIXELS(xx) 2*xx
#define ODD_PIXELS(xx) 2*xx + 1

/* ========================================================================== */
/*                          EXTERNAL REFERENCES                               */
/* ========================================================================== */

/* ========================================================================== */
/*                          GLOBAL REFERENCES                                 */
/* ========================================================================== */

/* ========================================================================== */
/*                       LOCAL FUNCTION PROTOTYPES                            */
/* ========================================================================== */
Void bayer8Bit2Yuv422(Uint8 *srcBuf,
                      Int32 numlinesCap,
                      Int32 lineszCap,
                      Int32 numlinesDis,
                      Int32 lineszDis,
                      Uint8 *destY,
                      Uint8 *destC);

/* ========================================================================== */
/*                       GLOBAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */

/* ========================================================================== */
/*                        LOCAL FUNCTION DEFINITIONS                          */
/* ========================================================================== */

/* ========================================================================== */
/*                        GLOBAL FUNCTION DEFINITIONS                         */
/* ========================================================================== */
/**
 *  \fn    bayer8Bit2Yuv422
 *
 *   Convert 8bit Raw Bayer pattern source data to YUV422 for VPIF display out.
 *   Bayer Pattern:
 *       GRGRGR
 *       BGBGBG
 *       GRGRGR
 *       BGBGBG
 *   Each 4x4 block of data is treated as a single pixel.
 *       GR - \  1 pixel of RGB
 *       BG - /
 *   The 2 green pixels are averaged together
 *   The R and B are extracted.
 *   After the MSBs are extracted from the RGB data,
 *   they are stored in 16 bit memory locations.
 *   This type of processing uses only 1/4 of the
 *   captured resolution, ie, 1/2 the number of pixels / line
 *   and 1/2 the lines
 *
 *  \return None
 */
Void bayer8Bit2Yuv422(Uint8 *srcBuf,
                      Int32 numlinesCap,
                      Int32 lineszCap,
                      Int32 numlinesDis,
                      Int32 lineszDis,
                      Uint8 *destY,
                      Uint8 *destC)
{
    register Uint8 r0, g0, b0, r1, g1, b1;
    Int32 ii, jj, kk, tmp;
    register Int32 y0, y1, u0, u1, v0, v1;
    Uint32 temp;
    Int32 numLines;
    Int32 numPixels;
    Int32 disPitch;
    Uint8 rPixel, gPixel[2], bPixel, gPxl;

    /* Find the effective number of lines to display */
    if(numlinesCap < numlinesDis)
    {
        numLines = numlinesCap;
    }
    else
    {
        if(numlinesCap/2 <= numlinesDis)
        {
            numLines = numlinesCap;
        }
        else
        {
            numLines = numlinesDis;
        }
    }

    /* Find the effective number of pixels / line to display */
    if(lineszCap/2 > lineszDis)
    {
        numPixels = lineszDis;
        disPitch = lineszDis/2;
    }
    else
    {
        numPixels = lineszCap/2;
        disPitch = lineszDis/2;
    }

    /* cycle through each line */
    for(jj = 0; jj < numLines; jj=jj+2)
    {
        kk = 0;
        /* cycle through the pixels */
        for(ii = 0; ii < numPixels; ii++)
        {
            /********************************************/
            /* Even Line */
            /* Green Pixel 0 */
            tmp = EVEN_PIXELS(ii) + lineszCap*jj;
            gPixel[0] = srcBuf[tmp];

            /* Red Pixel */
            tmp = ODD_PIXELS(ii) + lineszCap*jj;
            rPixel = srcBuf[tmp];
            /********************************************/
            /* increment line */
            jj++;
            /********************************************/
            /* Odd Line */
            /* Green Pixel 1 */
            tmp = ODD_PIXELS(ii) + lineszCap*jj;
            gPixel[1] = srcBuf[tmp];

            /* Blue Pixel */
            tmp = EVEN_PIXELS(ii) + lineszCap*jj;
            bPixel = srcBuf[tmp];
            /********************************************/

            /* Take average of green pixels */
            gPxl = (gPixel[0] + gPixel[1])/2;

            /* decrement the line */
            jj--;
            temp = kk++ + disPitch*jj;
            /* convert RGB888 to yuv422 Vpif format */
            if (ii%2 != 0)
            {
                r1 = rPixel;
                g1 = gPxl;
                b1 = bPixel;
            }
            else
            {
                r0 = rPixel;
                g0 = gPxl;
                b0 = bPixel;
            }
            if ((ii%2 != 0) && (ii!=0))
            {
                RGB2YUV(r1, g1, b1, y0, u0, v0);
                RGB2YUV(r0, g0, b0, y1, u1, v1);
                destY[temp] = y0;
                destC[temp] = (v0+v1) >> 1;
                destY[temp - 1] = y1;
                destC[temp - 1] = (u0+u1) >> 1;
            }
        } /* end lineszCap Loop */
    }/* end numlinesCap Loop */
}

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
