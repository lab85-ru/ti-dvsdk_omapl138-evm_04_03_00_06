/*========================================================================================= */
/* Copyright (c) 2010, Texas Instruments Incorporated                                       */
/*   All rights reserved.                                                                   */
/*                                                                                          */
/*   Name: C6Accel_testfxns.h                                                               */
/*                                                                                          */
/*   Description: File contains function prototypes for test functions                      */
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
Int c6accel_test_DSP_FFT(C6accel_Handle hC6accel,unsigned int n);
Int c6accel_test_DSP_IFFT(C6accel_Handle hC6accel,unsigned int n);
Int c6accel_test_DSP_AUTOCOR(C6accel_Handle hC6accel,unsigned int nx,unsigned int nr);
Int c6accel_test_DSP_DOTPROD(C6accel_Handle hC6accel,unsigned int nr);
Int c6accel_test_DSP_MATMUL(C6accel_Handle hC6accel,unsigned int row1, unsigned int column1, 
                            unsigned int column2,unsigned int shift);
Int c6accel_test_DSP_FIR(C6accel_Handle hC6accel, unsigned int n_outputs, unsigned int n_coeffs); 
Int c6accel_test_DSP_IIR(C6accel_Handle hC6accel, unsigned int n_inputs, unsigned int n_coeffs);


/* Function prototypes for image processing kernel tests */
Int c6accel_test_IMG_sobel(C6accel_Handle hC6accel,unsigned int inWidth, unsigned int inHeight);
Int c6accel_test_IMG_histogram(C6accel_Handle hC6accel,unsigned int inWidth, unsigned int inHeight);
Int c6accel_test_IMG_conv(C6accel_Handle hC6accel,unsigned int inWidth, unsigned int inHeight);
Int c6accel_test_IMG_corr(C6accel_Handle hC6accel,unsigned int inWidth, unsigned int inHeight);
Int c6accel_test_IMG_median(C6accel_Handle hC6accel,unsigned int inWidth, unsigned int inHeight);
Int c6accel_test_IMG_muls(C6accel_Handle hC6accel,unsigned int inWidth, unsigned int inHeight);
Int c6accel_test_IMG_adds(C6accel_Handle hC6accel,unsigned int inWidth, unsigned int inHeight);
Int c6accel_test_IMG_subs(C6accel_Handle hC6accel,unsigned int inWidth, unsigned int inHeight);
Int c6accel_test_IMG_YC_demux(C6accel_Handle hC6accel,unsigned int inWidth, unsigned int inHeight);
int c6accel_test_IMG_YUV422PLtoYUV422SP(C6accel_Handle hC6accel,unsigned int num_lines, unsigned int width,
                                        unsigned int dst_pitch, unsigned int src_y_pitch,unsigned int src_cbcr_pitch);

/* Function prototypes for MATH kernels */
Int c6accel_test_MATH_RTSARITH(C6accel_Handle hC6accel,unsigned int n);
Int c6accel_test_MATH_RTSCONV(C6accel_Handle hC6accel,unsigned int n);
Int c6accel_test_MATH_IQCONV(C6accel_Handle hC6accel,unsigned int n, unsigned int Qfmt, unsigned int Qfmt2);
Int c6accel_test_MATH_IQMATH(C6accel_Handle hC6accel,unsigned int n, unsigned int Qfmt);
Int c6accel_test_MATH_IQARITH(C6accel_Handle hC6accel,unsigned int n, unsigned int Qfmt);
Int c6accel_test_MATH_IQTRIG(C6accel_Handle hC6accel,unsigned int n, unsigned int Qfmt); 

/* Function prototypes for Floating point DSP kernels */
Int c6accel_test_MATH_RTSFLT(C6accel_Handle hC6accel,unsigned int n) ;
Int c6accel_test_MATH_RTSFLTDP(C6accel_Handle hC6accel,unsigned int n) ;
Int c6accel_test_DSPF_AUTOCOR(C6accel_Handle hC6accel, int nx, int nr);
Int c6accel_test_DSPF_CONVOL(C6accel_Handle hC6accel,short nh,short ny);
Int c6accel_test_DSPF_MATFXNS(C6accel_Handle hC6accel, int column1, int row1,int column2 );
Int c6accel_test_DSPF_MAT_MUL_CPLX(C6accel_Handle hC6accel, int column1, int row1,int column2 );
Int c6accel_test_DSPF_MAT_TRANS(C6accel_Handle hC6accel, int r1, int c1 );
Int c6accel_test_DSPF_DOTPRODFXNS(C6accel_Handle hC6accel,unsigned int n);
Int c6accel_test_DSPF_FIR(C6accel_Handle hC6accel,unsigned int ny,unsigned int nh);
Int c6accel_test_DSPF_FIR_CPLX(C6accel_Handle hC6accel, int ny, int nh);
Int c6accel_test_DSPF_IIR(C6accel_Handle hC6accel, int n);
Int c6accel_test_DSPF_VECMUL(C6accel_Handle hC6accel,unsigned int n );
Int c6accel_test_DSPF_VECRECIP(C6accel_Handle hC6accel,unsigned int n );
Int c6accel_test_DSPF_VECSUM_SQ(C6accel_Handle hC6accel,unsigned int n );
Int c6accel_test_DSPF_W_VEC(C6accel_Handle hC6accel,float M, unsigned int n );
Int c6accel_test_DSPF_sp_fftSPxSP(C6accel_Handle hC6accel,int n,int n_min,int offset, int n_max);
Int c6accel_test_DSPF_sp_ifftSPxSP(C6accel_Handle hC6accel,int n,int n_min,int offset, int n_max);
Int c6accel_test_DSPF_BIQUAD(C6accel_Handle hC6accel,unsigned int n);

/* Functions used to validate results */
extern unsigned int compare8bitArrays(unsigned char *pSrc1, unsigned char *pSrc2,unsigned int length);
extern unsigned int compare16bitArrays(unsigned short *pSrc1, unsigned short *pSrc2,unsigned int length);
extern unsigned int compare32bitArrays(unsigned int *pSrc1, unsigned int *pSrc2,unsigned int length);


#endif

/*=============================================================================*/
/*                  End of File C6Accel_testfxns.h                         */
/*                              Version : 0.0.1                                */
/*=============================================================================*/
