/*
 * Copyright (C) 2010Texas Instruments Incorporated - http://www.ti.com/ 
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
*/
/*==============================================================================*/
/* File Name:  appMain.c                                                        */
/* This is main app file that sets the benchmarking mechanism and calls         */
/*  functions to benchmark DSP kernels and showcase the DSP performance         */
/*                                                                              */
/*                                                                              */
/*  Version: 0.0.1                                                              */
/*                                                                              */
/*==============================================================================*/

/* Standard includes */
#include <stdio.h>
#include <math.h>

/* Codec Engine and xdc includes */ 
#include <xdc/std.h>
#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/CERuntime.h>
#include <ti/sdo/ce/osal/Memory.h>

/* system time include*/
#include <sys/time.h>

/* Test application include */
#include "appMain.h"

/* Include C6ACCEL headers */
#include "../c6accelw/c6accelw.h"
#include "dsp_benchmarks_fxns.h"

/* Define Engine ,algorithm and application names */

/* Pick engine name based on platform */
//  Engine name for OMAP3530 defined 
//  for future extension of the demo
#if (PLATFORM == 3530)
 #define ENGINENAME "omap3530"
 #warning ENGINENAME "omap3530"
 #define DSP_ISA  "64P"
#elif (PLATFORM == 138)
 #define ENGINENAME "omapl138" 
 #warning ENGINENAME "omapl138"
 #define DEVICE_FLOAT  
 #define DSP_ISA  "674X"
#endif
#define ALGNAME "c6accel"
#define APPNAME "app"


/* Align on cache-line boundary since encoder uses DMA */
#define BUFALIGN 128

/* Align buffers to this cache line size (in bytes)*/
#define BUFSIZEALIGN            128

/* Number of points for FFT */
#define N       65536

/* Parameter for dot prod */
#define Nx      2400
#define Nr      32

//Parameters for MAT MUL
#define ROW1    (8)
#define COL1    (8)
#define COL2    (8)
#define SHIFT   (0)     //shift after matrix multiplication

/* Maximum elements in test vectors defined for MATH functions*/
#define NMAX     255

/*Parameters for fir gen
 Force Nout to be a multiple of 4
*/
#define NCOEFF   8
#define NXIN     120
#define NOUT     ((NXIN-NCOEFF+1) & 0xFC)

/*Maximum width and height for IMGLIB kernels*/
#define WIDTH   640
#define HEIGHT  480

/* define largest image sizes in use (ship.yuv)*/
#define YUV_WIDTH 800
#define YUV_HEIGHT 600

/* Size buffers for max */
#define MAX_HEIGHT YUV_HEIGHT
#define MAX_WIDTH  YUV_WIDTH

/* Q format for IQMath test functions */
#define GLOBAL_Q 16
#define Q1 12

/* Defines for Flaoting point RTS functions*/
#define BUFSIZE  128

/* Defines for floating point Autocorrelation and convolution kernel*/
#define NumX 256
#define NumR 64
#define NumH 8

/*Defines for the floating point matrix multiplication functions */
#define R1  64
#define C1  64
#define C2  64

/*Define to test floating point dotproduct function */
#define Nelements 1024
#define Mfactor 0.3333333
#define NPAD 4

/* Defines for floating point fft kerenls */
#define Npt 8192
#define rad 2

/* Memory buffers used for test */
Int8 *pSrcBuf_16bpp=NULL;
Int8 *pRefBuf_16bpp=NULL;
Int8 *pOutBuf_16bpp=NULL;
Int8 *pWorkingBuf_16bpp=NULL;
Int8 *pWorkingBuf2_16bpp=NULL;
#ifdef DEVICE_FLOAT
Int8 *pWorkingBuf3_16bpp=NULL;
#endif

/* DSP frequency in  MHz */
int DSP_freq;
void Get_dspFreq();

/* Following vars/code are required by MACROs defined in benchmark.h*/
FILE* benchmarkFd;

/* Codec Engine engine and codec labels, defined in cfg file: */
static String algName      = ALGNAME;
static String engineName   = ENGINENAME;
static String progName     = APPNAME;

/* This object is used in MACRO in benchmark.h */
#include "timeObj.h"
Time_Object sTime;
UInt32      time;

void Time_reset(Time_Object *sTime)
{
 struct timeval tv;

    if (gettimeofday(&tv, NULL) == -1) {
        return;
    }

    sTime->original = tv;
    sTime->previous = tv;

    return;
}

void Time_delta(Time_Object* sTime, UInt32 *deltaPtr)
{
    struct timeval tv;
    time_t         s;
    suseconds_t    us;

    if (gettimeofday(&tv, NULL) == -1) {
        return;
    }

    s = tv.tv_sec - sTime->previous.tv_sec;
    us = tv.tv_usec - sTime->previous.tv_usec;

    *deltaPtr = s * 1000000l + us;

    sTime->previous = tv;

    return;
}

/* Now include benchmark.h after it's supporting functions/vars are declared*/
#include "benchmark.h"

extern void CERuntime_init(void);

/*Define a C6Accel Handle to call the abstracted wrapper APIs*/
C6accel_Handle hC6 = NULL;

/******************************************************************************
 * appMain
 ******************************************************************************/
Int main(Int argc, Char *argv[])
{
    UInt32 framesize;
    Memory_AllocParams memParams = Memory_DEFAULTPARAMS;
    
    
    printf("**************************************************\n");
    printf("DSP Benchmark demo started.\n");
    printf("**************************************************\n");
    printf("\nBenchmarks highlight the performance of C%s DSPs\n", DSP_ISA);
    printf("Two simultaneous benchmark are provided: \n");
    printf("* DSP only benchmark:\n");
    printf("  Processing time on the DSP [@300MHz] \n");
    printf("* ARM side DSP benchmark: \n");
    printf("  Processing time when DSP is invoked from ARM\n"); 
    printf("  (using C6Accel)\n");
    
  
    
    /* This call must be made before the Memory_xxx() functions as it is required for the tracing functions
     in all the codec engine APIs that are used*/
    CERuntime_init();

    /* Reset timeObj used for benchmarking*/
    Time_reset(&sTime);

    /* Create call generates a C6ACCEL handle */
    hC6 = C6accel_create(engineName, NULL,algName, NULL);

    /*Check for failure*/
    if ( hC6 == NULL)
       {printf("%s: C6accel_create() failed \n",progName);
        goto end;
    }

    /* Create buffers for use by algorithms */

    /* Want to use cached & contiguous memory to get best performance from cortex when it also uses the buffers.*/
    memParams.flags = Memory_CACHED;
    memParams.type = Memory_CONTIGHEAP;

    /* Size all buffers for 6 bytes, to cope with worst case 16 bit 422Planar*/
    framesize = (MAX_WIDTH * MAX_HEIGHT * sizeof(Int32)*3/2);

    /* Create 16bit buffers for use by algorithms*/
    pSrcBuf_16bpp = Memory_alloc(framesize, &memParams);
    if (pSrcBuf_16bpp == NULL) {
        goto end;
    }
    else {
       Memory_cacheWbInv(pSrcBuf_16bpp, framesize);
    }
    
      pOutBuf_16bpp = Memory_alloc(framesize, &memParams);
    if (pOutBuf_16bpp == NULL) {
        goto end;
    }
    else {
       Memory_cacheWbInv(pOutBuf_16bpp, framesize);
    }
    
    pRefBuf_16bpp = Memory_alloc(framesize, &memParams);
    if (pRefBuf_16bpp == NULL) {
        goto end;
    }
    else {
       Memory_cacheWbInv(pRefBuf_16bpp, framesize);
    }

      pWorkingBuf_16bpp = Memory_alloc(framesize, &memParams);
    if (pWorkingBuf_16bpp == NULL) {
        goto end;
    }
    else {
       Memory_cacheWbInv(pWorkingBuf_16bpp, framesize);
    }
    
    pWorkingBuf2_16bpp = Memory_alloc(framesize, &memParams);
    if (pWorkingBuf2_16bpp == NULL) {
        goto end;
    }
    else {
       Memory_cacheWbInv(pWorkingBuf2_16bpp, framesize);
    }
  
   #ifdef DEVICE_FLOAT
   pWorkingBuf3_16bpp = Memory_alloc(framesize, &memParams);
    if (pWorkingBuf3_16bpp == NULL) {
        goto end;
    }
    else {
       Memory_cacheWbInv(pWorkingBuf3_16bpp, framesize);
    }
    #endif
    
    /*Get DSP Frequency(Need to figure out a way to dynamically sense DSP Frequency)*/
    Get_dspFreq();

    /* Call test functions for kernels in C6accel*/
    LOG_STRING("[+] Imaging Processing and Analytics benchmarks\n");
    printf("----------------------------------------------------\n");
    LOG_STRING("<>640x480 8bit/pixel b/w Test Image \n");
    
    dsp_benchmark_IMG_histogram(hC6,WIDTH,HEIGHT);
    dsp_benchmark_IMG_conv(hC6,WIDTH,HEIGHT);
    dsp_benchmark_IMG_sobel(hC6,WIDTH,HEIGHT);
    dsp_benchmark_IMG_adds(hC6,WIDTH,HEIGHT);
    
    LOG_STRING("\n<>800x600 8bit/pixel Test Image \n");
    dsp_benchmark_IMG_YC_demux(hC6,YUV_WIDTH,YUV_HEIGHT);
    
    LOG_STRING("[+]Signal Processing benchmarks\n");
    printf("----------------------------------------------------\n");
    LOG_STRING("<>64k sample fixed point test data \n");
    dsp_benchmark_DSP_FFT(hC6,N);
    dsp_benchmark_DSP_IFFT(hC6,N);
#ifndef DEVICE_FLOAT
    dsp_benchmark_DSP_AUTOCOR(hC6,Nx,Nr);
    dsp_benchmark_DSP_FIR(hC6,NOUT,NCOEFF);
    dsp_benchmark_DSP_IIR(hC6,NXIN,NCOEFF);
#endif

#ifdef DEVICE_FLOAT
    dsp_benchmark_DSPF_AUTOCOR(hC6,Npt,NumR);
    LOG_STRING("<>8k sample floating point test data \n");
    dsp_benchmark_DSPF_sp_fftSPxSP(hC6, Npt, rad, 0, Npt);
    dsp_benchmark_DSPF_sp_ifftSPxSP(hC6, Npt, rad, 0, Npt);
    dsp_benchmark_DSPF_IIR(hC6,  Npt);
    dsp_benchmark_DSPF_FIR(hC6, Npt,  4);   
#endif    
    printf("\n");

  #ifdef DEVICE_FLOAT
  //Test function calls for floating point kernels in C6accel
    printf("MATH processing benchmarks \n");
    printf("----------------------------------------------------\n");
    printf("(*) Single precision processing\n");
    LOG_STRING("<>8k sample single precision test data \n");
    dsp_benchmark_MATH_RTSFLT(hC6,Npt) ;
    printf("(*) Double precision processing\n");
    LOG_STRING("<>8k sample double precision test data \n");
    dsp_benchmark_MATH_RTSFLTDP(hC6,Npt);
    
  #endif 
    printf("For more information visit:\n");
    printf("[+] C674X DSP benchmarks:  \n");
    printf("http:focus.ti.com/dsp/docs/dspplatformscontento.tsp?familyId=1622&sectionId=2&tabId=2431\n\n");    
    printf("[+] C6Accel ARM+DSP benchmarks:\n");
    printf("http://processors.wiki.ti.com/index.php/C6Accel:_ARM_access_to_DSP_software_on_TI_SoCs\n");
end:
     // Tear down C6ACCEL
    if (hC6)
       C6accel_delete(hC6);

    if(pSrcBuf_16bpp)
        Memory_free(pSrcBuf_16bpp, framesize, &memParams);

    if(pOutBuf_16bpp)
        Memory_free(pOutBuf_16bpp, framesize, &memParams);

    if(pRefBuf_16bpp)
        Memory_free(pRefBuf_16bpp, framesize, &memParams);

    if(pWorkingBuf_16bpp)
        Memory_free(pWorkingBuf_16bpp, framesize, &memParams);

    if(pWorkingBuf2_16bpp)
        Memory_free(pWorkingBuf2_16bpp, framesize, &memParams);

    #ifdef DEVICE_FLOAT
    if(pWorkingBuf3_16bpp)
        Memory_free(pWorkingBuf3_16bpp, framesize, &memParams);
    #endif

    printf("**************************************************\n");
    printf("End of DSP benchmarks\n");
    printf("**************************************************\n");
    printf("\n");

   return (0);
}

/* Function to find DSP operating Frequency */
void Get_dspFreq(){
   
   /* Set to Max OMAPL operating frequency*/
   /* Need to figure out how to enable dynamic sensing of DSP operating Frequency from ARM */
   #if (PLATFORM == 138)
     DSP_freq = 300;
   #endif
   #if (PLATFORM == 3530)
     DSP_freq = 520;
   #endif
 
}
/*=============================================================================*/
/*                  End of File appMain.c                                      */
/*                              Version : 0.0.1                                */
/*=============================================================================*/

