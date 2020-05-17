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
/*========================================================================================= */
/*                                                                                          */
/*   Name: dsp_benchmark_fxn.h                                                              */
/*                                                                                          */
/*   Description: File contains function prototypes for benchmark DSP                       */
/*                                                                                          */
/*==========================================================================================*/ 

#ifndef C6ACCEL_TESTFXNS_H
#define C6ACCEL_TESTFXNS_H

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

/* Function prototypes for digital signal processing kernels*/
Int dsp_benchmark_DSP_FFT(C6accel_Handle hC6accel,unsigned int n);
Int dsp_benchmark_DSP_IFFT(C6accel_Handle hC6accel,unsigned int n);
Int dsp_benchmark_DSP_AUTOCOR(C6accel_Handle hC6accel,unsigned int nx,unsigned int nr);
Int dsp_benchmark_DSP_FIR(C6accel_Handle hC6accel, unsigned int n_outputs, unsigned int n_coeffs); 
Int dsp_benchmark_DSP_IIR(C6accel_Handle hC6accel, unsigned int n_inputs, unsigned int n_coeffs);


/* Function prototypes for image processing kernel tests */
Int dsp_benchmark_IMG_sobel(C6accel_Handle hC6accel,unsigned int inWidth, unsigned int inHeight);
Int dsp_benchmark_IMG_histogram(C6accel_Handle hC6accel,unsigned int inWidth, unsigned int inHeight);
Int dsp_benchmark_IMG_median(C6accel_Handle hC6accel,unsigned int inWidth, unsigned int inHeight);
Int dsp_benchmark_IMG_adds(C6accel_Handle hC6accel,unsigned int inWidth, unsigned int inHeight);
Int dsp_benchmark_IMG_YC_demux(C6accel_Handle hC6accel,unsigned int inWidth, unsigned int inHeight);

/* Function prototypes for MATH kernels */
Int dsp_benchmark_MATH_RTSARITH(C6accel_Handle hC6accel,unsigned int n);
Int dsp_benchmark_MATH_RTSCONV(C6accel_Handle hC6accel,unsigned int n);

/* Function prototypes for Floating point DSP kernels */
Int dsp_benchmark_MATH_RTSFLT(C6accel_Handle hC6accel,unsigned int n) ;
Int dsp_benchmark_MATH_RTSFLTDP(C6accel_Handle hC6accel,unsigned int n) ;
Int dsp_benchmark_DSPF_AUTOCOR(C6accel_Handle hC6accel, int nx, int nr);
Int dsp_benchmark_DSPF_CONVOL(C6accel_Handle hC6accel,short nh,short ny);
Int dsp_benchmark_DSPF_MATFXNS(C6accel_Handle hC6accel, int column1, int row1,int column2 );
Int dsp_benchmark_DSPF_MAT_MUL_CPLX(C6accel_Handle hC6accel, int column1, int row1,int column2 );
Int dsp_benchmark_DSPF_MAT_TRANS(C6accel_Handle hC6accel, int r1, int c1 );
Int dsp_benchmark_DSPF_DOTPRODFXNS(C6accel_Handle hC6accel,unsigned int n);
Int dsp_benchmark_DSPF_FIR(C6accel_Handle hC6accel,unsigned int ny,unsigned int nh);
Int dsp_benchmark_DSPF_FIR_CPLX(C6accel_Handle hC6accel, int ny, int nh);
Int dsp_benchmark_DSPF_IIR(C6accel_Handle hC6accel, int n);
Int dsp_benchmark_DSPF_VECMUL(C6accel_Handle hC6accel,unsigned int n );
Int dsp_benchmark_DSPF_VECRECIP(C6accel_Handle hC6accel,unsigned int n );
Int dsp_benchmark_DSPF_VECSUM_SQ(C6accel_Handle hC6accel,unsigned int n );
Int dsp_benchmark_DSPF_W_VEC(C6accel_Handle hC6accel,float M, unsigned int n );
Int dsp_benchmark_DSPF_sp_fftSPxSP(C6accel_Handle hC6accel,int n,int n_min,int offset, int n_max);
Int dsp_benchmark_DSPF_sp_ifftSPxSP(C6accel_Handle hC6accel,int n,int n_min,int offset, int n_max);
Int dsp_benchmark_DSPF_BIQUAD(C6accel_Handle hC6accel,unsigned int n);

/* Functions used to validate results */
extern unsigned int compare8bitArrays(unsigned char *pSrc1, unsigned char *pSrc2,unsigned int length);
extern unsigned int compare16bitArrays(unsigned short *pSrc1, unsigned short *pSrc2,unsigned int length);
extern unsigned int compare32bitArrays(unsigned int *pSrc1, unsigned int *pSrc2,unsigned int length);


#endif

/*=============================================================================*/
/*                  End of File C6Accel_testfxns.h                         */
/*                              Version : 0.0.1                                */
/*=============================================================================*/
