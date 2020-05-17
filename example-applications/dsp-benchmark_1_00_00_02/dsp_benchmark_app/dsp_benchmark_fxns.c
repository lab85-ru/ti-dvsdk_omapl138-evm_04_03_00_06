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
/*================================================================================*/
/*                                                                                */
/*   Name: dsp_benchmark_fxns.c                                                   */
/*                                                                                */
/*   Descriptions:                                                                */
/*   File contains code to benchmark DSP for key functionalities                  */
/*                                                                                */
/* Note: All DSP only benchmarks mentioned in this file are obtained from ti.com  */
/*                                                                                */
/*      Version: 0.0.1                                                            */
/*================================================================================*/

/* This define uses the new frame based (ie row and col parameters) that are optimised for C6Accel
 as they only request one operation on all rows rather than row operations*/
#define USE_NEW_FRAME_APIS

/*XDC and codec engine includes*/
#include <xdc/std.h>
#include <ti/sdo/ce/osal/Memory.h>

/* Run Time lib include files: */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

/* Declare MACROS that will be used for benchmarking*/
#include "benchmark.h"

/* Include C6ACCEL headers*/
#include "../c6accelw/c6accelw.h"
#include "dsp_benchmark_inputs.h"

/* Create default heap memory configuration for test functions */
static Memory_AllocParams testfxnsMemParams =
{
    Memory_CONTIGHEAP,
    Memory_CACHED,
    Memory_DEFAULTALIGNMENT,
    0
};

/* Global definitions */
/* Number of samples for which FFT needs to be calculated */

/* Number of unique sine waves in input data */
#define NUM_SIN_WAVES 4

/* PI definition*/
#  define PI 3.14159265358979323846

/* Define constant for number of image bits in 16 bit histogram case */
#define IMG_BITS_HISTOGRAM_16   12

/*
    Scale need to be applied (1/2^SCALE) to input data for 16x32 and 32x32
    FFT for their output to match 16x16. This is dure to the inherent
    scaling present in 16x16 kernel
*/
#define SCALE    3

/* Generate a Floating point sine wave containing numSinWaves signals with n samples*/
Void generateInput (Int32 numSinWaves, float *x_ref_float, Int32 n);
static short d2s(double d);
int gen_twiddle_fft16x16 (short *w,int n);
static int d2i(double d);
int gen_twiddle_fft32x32(int *w, int n, double scale);
int gen_twiddle_ifft32x32(int *w, int n, double scale);
int gen_twiddle_ifft16x16(short *w, int n);
float randfloat();
float randflt(float min, float max);
int randint();
int randuint(int max);
int randInt(int min, int max);

/* Intermediate buffers */
extern unsigned char *pSrcBuf_16bpp;
extern unsigned char *pRefBuf_16bpp;
extern unsigned char *pOutBuf_16bpp;
extern unsigned char *pWorkingBuf_16bpp;
extern unsigned char *pWorkingBuf2_16bpp;
#if (PLATFORM == 138)
extern unsigned char *pWorkingBuf3_16bpp;
#endif

/* Define all the test file paths*/
char ref_fft_16x16[]="test_files/fft_16x16_64k_sine_wave_64kHz_sampling_ref.dat";
char ref_fft_32x32[]="test_files/fft_32x32_64k_sine_wave_64kHz_sampling_ref.dat";

/* Filenames for ifft testing*/
char ref_twiddle_fft16x16[]="test_files/ref_twiddle_fft16x16.dat";
char ref_twiddle_ifft16x16[]="test_files/ref_twiddle_ifft16x16.dat";
char ref_twiddle_fft32x32[]="test_files/ref_twiddle_fft32x32.dat";
char ref_twiddle_ifft32x32[]="test_files/ref_twiddle_ifft32x32.dat";
char ref_src_fft32x32[]="test_files/input_64k_source2_combined_sine_wave_64kHz_sampling_ref.dat";
char ref_src_fft[]="test_files/input_64k_source_combined_sine_wave_64kHz_sampling_ref.dat";

char ref_ifft_16x16[]="test_files/ifft_16x16_64k_sine_wave_64kHz_sampling_ref.dat";
//char ref_ifft_32x32[]="test_files/ifft_32x32_64k_sine_wave_64kHz_sampling_ref.dat";

/*Filenames for autocorrelation testing*/
char ref_autocor[]="test_files/autocor_64k_sine_wave_64kHz_sampling_ref.dat";
char ref_src_autocor[]="test_files/src_autocor_64k_sine_wave_64kHz_sampling_ref.dat";



/*Filenames for matrix multiplcation testing*/
char ref_mat_mul[]="test_files/mat_mul_ref.dat";

/*Filenames for fir generation testing*/
char ref_fir_gen[]="test_files/fir_gen_ref.dat";
char ref_fir_r4[]="test_files/fir_r4_ref.dat";
char ref_fir_r8[]="test_files/fir_r8_ref.dat";

/*Filenames for iir filter*/
char ref_iir[]="test_files/fir_iir.dat";

/* Generic 8 bit source file */
char srcFileName[]="test_files/valve_640x480_luma8bpp.pgm";

/* Filenames for histogram testing */
char ref_histogram_8[]="test_files/histogram_8_valve_640x480_luma8bpp_ref.bin";

/* Filenames for conv testing*/
char ref_conv_3x3_8[]="test_files/conv_3x3_8_valve_640x480_luma8bpp_ref.pgm";
char ref_conv_3x3_16s[]="test_files/conv_3x3_16s_valve_640x480_luma8bpp_ref.dat";

/* Filenames for corr testing*/
char ref_corr_3x3_8[]="test_files/corr_3x3_8_valve_640x480_luma8bpp_ref.pgm";
char ref_corr_3x3_16s[]="test_files/corr_3x3_16s_valve_640x480_luma8bpp_ref.dat";

/* Filenames for Sobel testing */
char ref_sobel_3x3_8[]="test_files/sobel_3x3_8_valve_640x480_luma8bpp_ref.pgm";
char ref_sobel_3x3_16[]="test_files/sobel_3x3_16_valve_640x480_luma8bpp_ref.pgm";

/* Filenames for adds testing */
char ref_adds_8[]="test_files/adds_8_valve_640x480_luma8bpp_ref.pgm";
char ref_adds_16[]="test_files/adds_16_valve_640x480_luma8bpp_ref.pgm";

/* Filenames for yc_demux and other CSC kernel testing */
char src_yuyv[]="test_files/ship.yuv";
char ref_yc_demux_8[]="test_files/yc_demux_8_ship_800x600_luma8bpp_ref_422p.yuv";
char ref_yc_demux_16[]="test_files/yc_demux_16_ship_800x600_luma8bpp_ref_422p.yuv";
char ref_rgb_565[]="test_files/rgb_565_ship_800x600_luma8bpp_ref.rgb";
char ref_rgb_565_yuv16[]="test_files/rgb_565_ship_800x600_luma8bpp_ref_yuv16.rgb";
char ref_yuv420_rgb_565[]="test_files/yuv420_rgb_565_ship_800x600_luma8bpp_ref.rgb";
char ref_yuv420pl16_rgb_565[]="test_files/yuv420pl16_rgb_565_ship_800x600_luma8bpp_ref.rgb";

char ref_input_y_422pl[]="test_files/ref_src_y_422pl.dat";
char ref_input_cb_422pl[]="test_files/ref_src_cb_422pl.dat";
char ref_input_cr_422pl[]="test_files/ref_src_cr_422pl.dat";
char ref_dst_y_422sp[]="test_files/ref_dst_y_422sp.dat";
char ref_dst_cbcr_422sp[]="test_files/ref_dst_cbcr_422sp.dat";
char ref_src_y_422sp[]="test_files/ref_src_y_422sp.dat";
char ref_src_cbcr_422sp[]="test_files/ref_src_cbcr_422sp.dat";
char ref_dst_y_420pl[]="test_files/ref_dst_y_420pl.dat";
char ref_dst_cb_420pl[]="test_files/ref_dst_cb_420pl.dat";

char ref_dst_cr_420pl[]="test_files/ref_dst_cr_420pl.dat";
char ref_dst_ycbcr_422ile[]="test_files/ref_dst_ycbcr_422ile.dat";

/* Floating point Kernel test vectors */
char ref_autocor_x[]="test_files/flt_autocor_input_ref.dat";
char ref_autocor_rslt[]="test_files/flt_autocor_rslt_ref.dat";

char ref_convol_xinput[]="test_files/flt_convol_xinput_ref.dat";
char ref_convol_rslt[]="test_files/flt_convol_rslt_ref.dat";
char ref_convol_hinput[]="test_files/flt_convol_hinput_ref.dat";

char ref_fftSPxSP_xinput[]="test_files/flt_fftSPxSP_xinput_ref.dat";
char ref_fftSPxSP_rslt[]="test_files/flt_fftSPxSP_rslt_ref.dat";
char ref_fftSPxSP_brev[]="test_files/flt_fftSPxSP_brev_ref.dat";
char ref_fftSPxSP_twiddle[]="test_files/flt_fftSPxSP_twiddle_ref.dat";

char ref_fir_cplx_xinput[]="test_files/flt_fir_cplx_xinput_ref.dat";
char ref_fir_cplx_rslt[]="test_files/flt_fir_cplx_rslt_ref.dat";
char ref_fir_cplx_hinput[]="test_files/flt_fir_cplx_hinput_ref.dat";

char ref_fir_gen_xinput[]="test_files/flt_fir_gen_xinput_ref.dat";
char ref_fir_gen_rslt[]="test_files/flt_fir_gen_rslt_ref.dat";
char ref_fir_gen_hinput[]="test_files/flt_fir_gen_hinput_ref.dat";

char ref_ifftSPxSP_xinput[]="test_files/flt_ifftSPxSP_xinput_ref.dat";
char ref_ifftSPxSP_rslt[]="test_files/flt_ifftSPxSP_rslt_ref.dat";
char ref_ifftSPxSP_brev[]="test_files/flt_ifftSPxSP_brev_ref.dat";
char ref_ifftSPxSP_twiddle[]="test_files/flt_ifftSPxSP_twiddle_ref.dat";

char ref_iir_xinput[]="test_files/flt_iir_xinput_ref.dat";
char ref_iir_y1[]="test_files/flt_iir_y1_ref.dat";
char ref_iir_y2[]="test_files/flt_iir_y2_ref.dat";
char ref_iir_ha[]="test_files/flt_iir_ha_ref.dat";
char ref_iir_hb[]="test_files/flt_iir_hb_ref.dat";

char ref_mat_mul_x1[]="test_files/flt_mat_mul_x1_ref.dat";
char ref_mat_mul_x2[]="test_files/flt_mat_mul_x2_ref.dat";
char ref_mat_mul_rslt[]="test_files/flt_mat_mul_rslt_ref.dat";

char ref_mat_mul_cplx_x1[]="test_files/flt_mat_mul_cplx_x1_ref.dat";
char ref_mat_mul_cplx_x2[]="test_files/flt_mat_mul_cplx_x2_ref.dat";
char ref_mat_mul_cplx_rslt[]="test_files/flt_mat_mul_cplx_rslt_ref.dat";

char ref_mat_trans_xinput[]="test_files/flt_mat_trans_xinput_ref.dat";
char ref_mat_trans_rslt[]="test_files/flt_mat_trans_rslt_ref.dat";

char ref_vecmul_x1input[]="test_files/flt_vecmul_x1input_ref.dat";
char ref_vecmul_rslt[]="test_files/flt_vecmul_rslt_ref.dat";
char ref_vecmul_x2input[]="test_files/flt_vecmul_x2input_ref.dat";

char ref_vecrecip_xinput[]="test_files/flt_vecrecip_xinput_ref.dat";
char ref_vecrecip_rslt[]="test_files/flt_vecrecip_rslt_ref.dat";

char ref_vecsum_sq_xinput[]="test_files/flt_vecsum_sq_xinput_ref.dat";
char ref_vecsum_sq_rslt[]="test_files/flt_vecsum_sq_rslt_ref.dat";

char ref_w_vec_x1input[]="test_files/flt_w_vec_x1input_ref.dat";
char ref_w_vec_rslt[]="test_files/flt_w_vec_rslt_ref.dat";
char ref_w_vec_x2input[]="test_files/flt_w_vec_x2input_ref.dat";

extern int DSP_freq;
/*
 * Benchmark for FFT kernels
 */

int dsp_benchmark_DSP_FFT(C6accel_Handle hC6accel,unsigned int n)
{
    FILE  *refFd,*refFd1;
    float *pFlSineWave;
    short *twiddleTable16;
    short *inputArray16;
    int   *inputArray32;
    int   *twiddleTable32;
    int    i;

    /* Generate N bytes of 16 bit raw sine wave for use in tests
     Reuse pWorkingBuf2_16bpp for this */
    pFlSineWave = (float *)pWorkingBuf2_16bpp;
    generateInput (4, pFlSineWave, n);

    /*Benchmark C6accel_DSP_fft16x16() */

    /*Generate the 16 bit Fixed version of sine */
    inputArray16 = (short *)pSrcBuf_16bpp;
    if ((refFd1 = fopen(ref_src_fft,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_src_fft);
       return (0);
       }

    fread(inputArray16,  2*n, sizeof(short), refFd1);
    fclose(refFd1);

    /* Read in the twiddle table, use pWorkingBuf_16bpp for the table */
    twiddleTable16 = (short*)pWorkingBuf_16bpp;

    if ((refFd1 = fopen(ref_twiddle_fft16x16,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_twiddle_fft16x16);
       return (0);
       }
    fread(twiddleTable16,  2*n, sizeof(short), refFd1);
    fclose(refFd1);

    if ((refFd = fopen(ref_fft_16x16,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_fft_16x16);
       return (0);
       }

    /* Clear output arrays*/
    memset (pOutBuf_16bpp,0x00,2*n*sizeof(short));
    memset (pRefBuf_16bpp,0x00,2*n*sizeof(short));

    printf("DSP_fft16x16(16 bit Fixed point FFT)\n");
    printf("DSP: %d usecs     ", (int)((0.75*n*(log10(n)/log10(4.0))+38)/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    
    START_BENCHMARK();

    /* Call the fft16x16 function in C6Accel*/
    C6accel_DSP_fft16x16 (hC6accel,twiddleTable16, n,inputArray16,(short*)pOutBuf_16bpp);

    END_AND_WRITE_BENCHMARK();

    /* read in reference from test file*/
    fread(pRefBuf_16bpp,  2*n, sizeof(short), refFd);
    printf("usecs\n\n");

    return (1);
}

/*
 * Benchmark all Inverse FFT kernels in c6accel
 */
int dsp_benchmark_DSP_IFFT(C6accel_Handle hC6accel,unsigned int n)
{
    FILE  *refFd, *refFd1;
    short *twiddleTable16;
    short *inputArray16;
    int   *inputArray32;
    int   *twiddleTable32;
    double scale = 2147483647.5;

    /* Benchmark C6accel_DSP_ifft16x16() */
    inputArray16 = (short *)pSrcBuf_16bpp;
    if ((refFd1 = fopen(ref_fft_16x16,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_fft_16x16);
       return (0);
     }

    fread(inputArray16,  2*n, sizeof(short), refFd1);
    fclose(refFd1);

    /* Generate the twiddle table, use pWorkingBuf_16bpp for the table*/
    twiddleTable16 = (short*)pWorkingBuf_16bpp;
    gen_twiddle_fft16x16(twiddleTable16, n);

    if ((refFd = fopen(ref_ifft_16x16,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_ifft_16x16);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,2*n*sizeof(short));
    memset (pRefBuf_16bpp,0x00,2*n*sizeof(short));

    printf("DSP_ifft16x16(16 bit Fixed point inverse FFT)\n");
    printf("DSP: %d usecs     ",(int)((0.75*n*(log10(n)/log10(4.0))+38)/DSP_freq));
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    /* Call the ifft16x16 kernel in the codec */
    C6accel_DSP_ifft16x16(hC6accel,twiddleTable16, n,inputArray16,(short*)pOutBuf_16bpp);
    END_AND_WRITE_BENCHMARK();

    /*  read in reference output */
    fread(pRefBuf_16bpp,  2*n, sizeof(short), refFd);

    printf("usecs\n\n");

    fclose(refFd);

    return (1);
}


/*
 * Benchmark for autocorrelation kernels in c6accel
 */
int dsp_benchmark_DSP_AUTOCOR(C6accel_Handle hC6accel,unsigned int nx ,unsigned int nr)
{
    FILE  *refFd,*refFd1;
    float *pFlSineWave;
    short *inputArray16;
    int i;

    /* Generate N bytes of 16 bit raw sine wave for use in tests
     Reuse pWorkingBuf2_16bpp for this*/

     if ((refFd1 = fopen(ref_src_autocor,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_src_autocor);
       return (0);
     }

    /* Benchmark C6accel_DSP_autocor(), Generate the 16 bit Fixed version of sine */
    inputArray16 = (short *)pSrcBuf_16bpp;
    fread(inputArray16,  nx+nr, sizeof(short), refFd1);
    fclose(refFd1);

    if ((refFd = fopen(ref_autocor,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_autocor);
       return (0);
       }

   /*  Clear output arrays */
    memset (pOutBuf_16bpp,0x00, nr*sizeof(short));
    memset (pRefBuf_16bpp,0x00, nr*sizeof(short));

   /* Make log entry and make benchmarking entries */
    printf("DSP_autocor(16 bit Autocorrelation)\n");
    printf("DSP: %d usecs     ",((int)(20 + (2*nr) + (nx*nr/8))/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    /* Call autocorrelation function in the C6Accel */
    C6accel_DSP_autocor(hC6accel,(short*)pOutBuf_16bpp,(short*)inputArray16,nx,nr);
    END_AND_WRITE_BENCHMARK();

    /*  read the reference output from file */
    fread(pRefBuf_16bpp,  nr, sizeof(short), refFd);
    printf("usecs\n\n");

    fclose(refFd);
     return (1);
}


/*
 * Benchmark for FIR filter generation kernels in c6accel
 */
int dsp_benchmark_DSP_FIR(C6accel_Handle hC6accel,unsigned int nr,unsigned int nh)
{
    FILE *refFd;
    short *xArray16, *hArray16;
    int i;
    unsigned int nx = nr+nh-1;

    /* C6accel_DSP_fir_gen() test */
    /* Read 16 bit input */
    xArray16 = (short *)pSrcBuf_16bpp;
    hArray16 = (short *)pWorkingBuf2_16bpp;

    for (i=0;i<nx;i++){
        xArray16[i]     = x[i];
    }
    for (i=0;i<nh;i++){
        hArray16[i]     = h[i];
    }

    /* Open reference file to store reference output */
    if ((refFd = fopen(ref_fir_gen,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_fir_gen);
       return (0);
     }

    /* Clear output arrays before tests begins */
    memset (pOutBuf_16bpp,0x0000, nr);
    memset (pRefBuf_16bpp,0x0000, nr);

    printf("DSP_fir_gen()\n");
    printf("DSP: %d usecs     ",(int)((20 + (2*nr) + (nh*nr/8))/DSP_freq));
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    /* Call the DSP_fir_gen function in C6Accel*/
    C6accel_DSP_fir_gen(hC6accel,xArray16, hArray16,(short *)pOutBuf_16bpp,nh,nr);
    END_AND_WRITE_BENCHMARK();
    
    fread(pRefBuf_16bpp, nr, sizeof(short), refFd);
    printf("\n");

    fclose(refFd);

    /*Benchmark fir_r4 kernel in c6accel*/
    if ((refFd = fopen(ref_fir_r4,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_fir_r4);
       return (0);
       }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x0000, nr);
    memset (pRefBuf_16bpp,0x0000, nr);

    printf("DSP_fir_r4()");
    printf("DSP: %d usecs     ",(int)((nh*nr/8 + 22)/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_DSP_fir_r4(hC6accel,xArray16, hArray16,(short *)pOutBuf_16bpp,nh,nr);
    END_AND_WRITE_BENCHMARK();
    
    fread(pRefBuf_16bpp, nr , sizeof(short), refFd);

    printf("ussecs\n");
    fclose(refFd);

    /* Benchmark fir_r8 kernel in c6accel */
    if ((refFd = fopen(ref_fir_r8,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_fir_r8);
       return (0);
       }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00, nr*sizeof(short));
    memset (pRefBuf_16bpp,0x00, nr*sizeof(short));

    printf("DSP_fir_r8(),");
    START_BENCHMARK();

    C6accel_DSP_fir_r8(hC6accel,xArray16, hArray16,(short *)pOutBuf_16bpp,nh,nr);
    END_AND_WRITE_BENCHMARK();
    
    fread(pRefBuf_16bpp, nr, sizeof(short), refFd);
    printf("\n");

    fclose(refFd);

    return (1);
}

/*
 * Benchmark for IIR filter generation kernels in c6accel
 */
int dsp_benchmark_DSP_IIR(C6accel_Handle hC6accel,unsigned int Nin,unsigned int nh)
{
    FILE *refFd;
    short *xArray16, *hArray16;
    short *b;
    int i;

    /*  Generate the 16 bit input  */
    xArray16 = (short *)pSrcBuf_16bpp;
    hArray16 = (short *)pWorkingBuf2_16bpp;
    b =(short*)pWorkingBuf_16bpp;

    for (i=0;i<Nin;i++){
        xArray16[i]     = x[i];
    }
    for (i=0;i<nh;i++){
        hArray16[i]     = h[i];
    }

    if ((refFd = fopen(ref_iir,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iir);
       return (0);
       }
    /* set state vector */
    memset(b,0x00,(nh)*sizeof(short));

    /*  Clear output arrays */
    memset (pOutBuf_16bpp,0x00, Nin*sizeof(short));
    memset (pRefBuf_16bpp,0x00, Nin*sizeof(short));

    printf("DSP_iir(16 bit IIR Filter)\n");
    printf("DSP: %d usecs     ",(int)((Nin*4 + 25)/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    /* Call IIR kernel in C6Accel */
    C6accel_DSP_iir(hC6accel,xArray16, hArray16,nh,b,(short *)pOutBuf_16bpp,Nin);

    END_AND_WRITE_BENCHMARK();
     
    fread(pRefBuf_16bpp, 1, Nin*sizeof(short), refFd);

    printf("usecs\n");

    fclose(refFd);

     return (1);
}

/*
 * Benchmark function for Floating point autocorrelation function
*/
int dsp_benchmark_DSPF_AUTOCOR(C6accel_Handle hC6accel, int Nx, int Nr)
{
 FILE *refFd;
 float *in1;
 int i;

/* Initialize Source and Output buffers to contiguous memory allocated   */
    in1 = (float *)pSrcBuf_16bpp;
    
   /*Read inputs */
   if ((refFd = fopen(ref_fftSPxSP_xinput,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_autocor_x);
         }
    fread(in1, Nx+Nr, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_autocor_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_autocor_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,Nr*sizeof(float));
    memset (pRefBuf_16bpp,0x00,Nr*sizeof(float));

    printf("DSPF_autocor(Floaing point Autocorrelation)\n");
    printf("DSP: %d usecs     ",(int)((20 + (2*Nr) + (Nx*Nr/8))/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");

    START_BENCHMARK();

    C6accel_DSPF_sp_autocor(hC6accel,(float *)pOutBuf_16bpp,(float *)in1,Nx, Nr);

    END_AND_WRITE_BENCHMARK();
    
    /* read in reference*/
    fread(pRefBuf_16bpp,  Nr, sizeof(float), refFd);
    printf("usecs\n\n");

   fclose(refFd);

    return 1;
}

/*
 * Benchmark function for Floating point convolution function
*/
int dsp_benchmark_DSPF_CONVOL(C6accel_Handle hC6accel,short nh,short ny)
{
 FILE *refFd;
 float *inX;
 float  *inH;
 int i;

/* Initialize Source and Output buffers to contiguous memory allocated   */
    inX = (float *)pSrcBuf_16bpp;
    inH = (float *)pWorkingBuf2_16bpp;
    
   /*Read inputs */
   if ((refFd = fopen(ref_convol_xinput,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_convol_xinput);
         }
    fread(inX, nh+ny-1, sizeof(float), refFd);
    fclose(refFd);
   
    if ((refFd = fopen(ref_convol_xinput,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_convol_hinput);
         }
    fread(inH, nh, sizeof(float), refFd);
    fclose(refFd);
   
    /* Open reference output files */
    if ((refFd = fopen(ref_convol_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_convol_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,ny*sizeof(float));
    memset (pRefBuf_16bpp,0x00,ny*sizeof(float));

    printf("DSPF_convol(Floating Point Convolution)\n");
    printf("DSP: %d usecs     ",(int)((20 + (2*nh) + (nh*ny/8))/DSP_freq));
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_DSPF_sp_convol(hC6accel,(float *)inX,(float *)inH,(float *)pOutBuf_16bpp,(short)nh, (short)ny);
    END_AND_WRITE_BENCHMARK();
   
    /* read in reference*/
    fread(pRefBuf_16bpp,  ny, sizeof(float), refFd);
    printf("usecs\n\n");

    fclose(refFd);

    return 1;
}


/*
 * Benchmark function for Floating point Matrix multiplication functions
*/
int dsp_benchmark_DSPF_MATFXNS(C6accel_Handle hC6accel, int c1, int r1, int c2 )
{
 FILE *refFd;
 float *inX1;
 float  *inX2;
 float *out;
 int i;
    
    memset (pSrcBuf_16bpp,0x00, c1*r1*sizeof(float));
    memset (pWorkingBuf2_16bpp,0x00, c1*c2*sizeof(float));
/* Initialize Source and Output buffers to contiguous memory allocated   */
    inX1 = (float *)pSrcBuf_16bpp;
    inX2 = (float *)pWorkingBuf2_16bpp;
    out = (float *)pRefBuf_16bpp;

   /*Read inputs */
   if ((refFd = fopen(ref_mat_mul_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_mat_mul_x1);
         }
    fread(inX1, c1*r1, sizeof(float), refFd);
    fclose(refFd);
    
    if ((refFd = fopen(ref_mat_mul_x2,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_mat_mul_x2);
         }
    fread(inX2, c1*c2, sizeof(float), refFd);
    fclose(refFd);
    
    /* Open reference output files */
    if ((refFd = fopen(ref_mat_mul_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_mat_mul_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00, r1*c2*sizeof(float));
    memset (pRefBuf_16bpp,0x00, r1*c2*sizeof(float));

    printf("* DSPF_sp_mat_mul(Floating Point matrix multiplication)");
    printf("DSP: %d usecs     ",(int)(((0.5 * r1 * c1 * c2) + (6 * c2 * r1) + (4 * r1) + 22)/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_DSPF_sp_mat_mul(hC6accel,inX1,r1, c1,inX2,c2,(float *)pOutBuf_16bpp);
    END_AND_WRITE_BENCHMARK();
    
    /* read in reference*/
    fread(pRefBuf_16bpp,  r1*c2, sizeof(float), refFd);
    printf("usecs\n");

    fclose(refFd);
    return 1;
}


// Benchmark for Matrix Multiplication of complex matrices
int dsp_benchmark_DSPF_MAT_MUL_CPLX(C6accel_Handle hC6accel,int c1,int r1,int c2 )
{
 FILE *refFd;
 float *inX1;
 float  *inX2;
 float *out;
 int i;

/* Initialize Source and Output buffers to contiguous memory allocated   */
    inX1 = (float *)pSrcBuf_16bpp;
    inX2 = (float *)pWorkingBuf2_16bpp;
    out = (float *)pRefBuf_16bpp;

// Benchmark for Matrix Multiplication of complex matrices
    /*Read inputs */
   if ((refFd = fopen(ref_mat_mul_cplx_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_mat_mul_cplx_x1);
         }
    fread(inX1, 2*c1*r1, sizeof(float), refFd);
    fclose(refFd);
    
    if ((refFd = fopen(ref_mat_mul_cplx_x2,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_mat_mul_cplx_x2);
         }
    fread(inX2, 2*c1*c2, sizeof(float), refFd);
    fclose(refFd);
    
    /* Open reference output files */
    if ((refFd = fopen(ref_mat_mul_cplx_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_mat_mul_cplx_rslt);
       return (0);
     }
    
    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,2* r1*c2*sizeof(float));
    memset (pRefBuf_16bpp,0x00, 2*r1*c2*sizeof(float));

    printf("DSPF_sp_mat_mul_cplx(Floating Point Complex Matrix Multiplication)\n");
    printf("DSP: %d usecs     ",(int)((2*r1*c1*c2 + 33)/DSP_freq)  );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_DSPF_sp_mat_mul_cplx(hC6accel,inX1,r1, c1,inX2,c2,(float *)pOutBuf_16bpp);
    END_AND_WRITE_BENCHMARK();
    
    /* read in reference*/
    fread(pRefBuf_16bpp,  2*r1*c2, sizeof(float), refFd);
    printf("usecs\n");
 
    fclose(refFd);

    return 1;
}
/*
 * Benchmark function for Floating point Matrix transpose
*/
int dsp_benchmark_DSPF_MAT_TRANS(C6accel_Handle hC6accel, int r1, int c1 )
{
 FILE *refFd;
 float *inX1;
 int i;

/* Initialize Source and Output buffers to contiguous memory allocated   */
    inX1 = (float *)pSrcBuf_16bpp;
    
   // Benchmark for Matrix Transpose of complex matrices
    /*Read inputs */
   if ((refFd = fopen(ref_mat_trans_xinput,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_mat_trans_xinput);
         }
    fread(inX1, c1*r1, sizeof(float), refFd);
    fclose(refFd);

     /* Open reference output files */
    if ((refFd = fopen(ref_mat_trans_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_mat_trans_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00, r1*c1*sizeof(float));
    memset (pRefBuf_16bpp,0x00, r1*c1*sizeof(float));

    printf("DSPF_sp_trans(Floating Point matrix Transpose)\n");
    printf("DSP: %d usecs     ", (int)((c1 * (r1 + 3) + 31)/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    
    START_BENCHMARK();

    C6accel_DSPF_sp_mat_trans(hC6accel,inX1,r1, c1,(float *)pOutBuf_16bpp);
    END_AND_WRITE_BENCHMARK();
   
    /* read in reference*/
    fread(pRefBuf_16bpp,  r1*c1, sizeof(float), refFd);
    printf("usecs\n");

    fclose(refFd);
    return 1;
}



/*
 * Benchmark for FIR filter generation kernels in c6accel
 */
int dsp_benchmark_DSPF_FIR(C6accel_Handle hC6accel,unsigned int ny,unsigned int nh)
{
    FILE *refFd;
    float *inX, *inH;
    int i;
    unsigned int nx = ny+nh-1;

    /* C6accel_DSP_fir_gen() test */
    /* Read 16 bit input */
    inX= (float *)pSrcBuf_16bpp;
    inH = (float *)pWorkingBuf2_16bpp;

   /*Read inputs */
   if ((refFd = fopen(ref_fftSPxSP_xinput,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_fir_gen_xinput);
         }
    fread(inX, nx, sizeof(float), refFd);
    fclose(refFd);

    if ((refFd = fopen(ref_fir_gen_hinput,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_fir_gen_hinput);
         }
    fread(inH, nh, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference file to store reference output */
    if ((refFd = fopen(ref_fir_gen_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_fir_gen_rslt);
       return (0);
     }

    /* Clear output arrays before tests begins */
    memset (pOutBuf_16bpp,0x0000, ny);
    memset (pRefBuf_16bpp,0x0000, ny);

    printf("DSPF_sp_fir_gen(Floating Point FIR filter)\n");
    printf("DSP: %d usecs     ",(int)((43 + 10*(nh*ny/16))/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    /* Call the DSP_fir_gen function in C6Accel*/
    C6accel_DSPF_sp_fir_gen(hC6accel,inX, inH,(float *)pOutBuf_16bpp,nh,ny);
    END_AND_WRITE_BENCHMARK();
   
    fread(pRefBuf_16bpp, ny, sizeof(float), refFd);
    printf("usecs\n");

    fclose(refFd);
     return 1;
}

int dsp_benchmark_DSPF_FIR_CPLX(C6accel_Handle hC6accel, int ny, int nh)
{
    FILE *refFd;
    float *inX, *inH;
    int i,status;
    unsigned int nx = ny+nh-1;

    /* C6accel_DSP_fir_gen() test */
    /* Read 16 bit input */
    inX= (float *)pSrcBuf_16bpp;
    inH = (float *)pWorkingBuf2_16bpp;

    /* C6accel_DSP_fir_cplx() test */

   /*Read inputs */
   if ((refFd = fopen(ref_fir_cplx_xinput,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_fir_cplx_xinput);
         }
    fread(inX, 2*nx, sizeof(float), refFd);
    fclose(refFd);

    if ((refFd = fopen(ref_fir_cplx_hinput,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_fir_cplx_hinput);
         }
    fread(inH, 2*nh, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference file to store reference output */
    if ((refFd = fopen(ref_fir_cplx_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_fir_cplx_rslt);
       return (0);
     }

    /* Clear output arrays before tests begins */
    memset (pOutBuf_16bpp,0x0000, 2*ny);
    memset (pRefBuf_16bpp,0x0000, 2*ny);

    printf("DSPF_sp_fir_cplx(Floating point complex FIR Filter)\n");
    printf("DSP: %d usecs     ",(int)((2*nh*ny+33)/DSP_freq));
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    /* Call the DSP_fir_cplx function in C6Accel*/
    status = C6accel_DSPF_sp_fir_cplx(hC6accel,(float *)inX, inH,(float *)pOutBuf_16bpp,nh,ny);
    END_AND_WRITE_BENCHMARK();
   
    fread(pRefBuf_16bpp, 2*ny, sizeof(float), refFd);
    printf("usecs\n");

    fclose(refFd);

    return (1);
}

/*
 * Benchmark for IIR filter generation kernels in c6accel
 */
int dsp_benchmark_DSPF_IIR(C6accel_Handle hC6accel, int n)
{
    FILE *refFd;
    float *inX ;
    float *hb, *ha;
    int i,status;
    float *y2;

    /*  Generate the 16 bit input  */
    inX = (float *)pSrcBuf_16bpp;
    ha = (float *)pWorkingBuf2_16bpp;
    hb =(float*)pWorkingBuf_16bpp;
    y2 = (float*)pWorkingBuf3_16bpp;

     /*Read inputs */
    if ((refFd = fopen(ref_fftSPxSP_xinput,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_iir_xinput);
         }
    fread(inX, n+4, sizeof(float), refFd);
    fclose(refFd);

    if ((refFd = fopen(ref_iir_ha,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_iir_ha);
         }
    fread(ha, 5, sizeof(float), refFd);
    fclose(refFd);

    if ((refFd = fopen(ref_iir_hb,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_iir_hb);
         }
    fread(hb, 5, sizeof(float), refFd);
    fclose(refFd);


    if ((refFd = fopen(ref_iir_y1,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iir_y1);
       return (0);
      }

    /*  Clear output arrays */
    memset (pOutBuf_16bpp,0x00, n*sizeof(float));
    memset (pRefBuf_16bpp,0x00, n*sizeof(float));

    printf("DSPF_sp_iir(Floating Point IIR Filter)\n");
    printf("DSP: %d usecs     ",(int)((6*n+59)/DSP_freq));
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    /* Call IIR kernel in C6Accel */
    status = C6accel_DSPF_sp_iir(hC6accel,(float *)pOutBuf_16bpp,inX,y2,hb,ha,n);
   
    END_AND_WRITE_BENCHMARK();
    
    fread(pRefBuf_16bpp, n+4,sizeof(float), refFd);

    printf("usecs\n\n");

   fclose(refFd);

    return (1);
}


int dsp_benchmark_DSPF_sp_fftSPxSP(C6accel_Handle hC6accel,int n,int n_min,int offset, int n_max)
{
    FILE  *refFd,*refFd1;
    float *w;
    float *inX;
    unsigned char *brev;
    int    i;

       /*Benchmark C6accel_DSPF_sp_fftSPxSP() */

    /*Generate the 16 bit Fixed version of sine */
    inX = (float *)pSrcBuf_16bpp;
    if ((refFd1 = fopen(ref_fftSPxSP_xinput,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_fftSPxSP_xinput);
       return (0);
       }

    fread(inX,  2*n, sizeof(float), refFd1);
    fclose(refFd1);

    /* Read in the twiddle table, use pWorkingBuf_16bpp for the table */
    w = (float*)pWorkingBuf_16bpp;

    if ((refFd1 = fopen(ref_fftSPxSP_twiddle,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_fftSPxSP_twiddle);
       return (0);
       }
    fread(w,  2*n, sizeof(float), refFd1);
    fclose(refFd1);

    /* Read in the twiddle table, use pWorkingBuf_16bpp for the table */
    brev = (unsigned char*)pWorkingBuf2_16bpp;

    if ((refFd1 = fopen(ref_fftSPxSP_brev,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_fftSPxSP_brev);
       return (0);
       }
    fread(brev,  64, sizeof(unsigned char), refFd1);
    fclose(refFd1);


    if ((refFd = fopen(ref_fftSPxSP_rslt,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_fftSPxSP_rslt);
       return (0);
       }

    /* Clear output arrays*/
    memset (pOutBuf_16bpp,0xA5,n*sizeof(float));
    memset (pRefBuf_16bpp,0xA5,n*sizeof(float));

    printf("DSPF_sp_fftSPxSP(Single Precision FFT filter)\n");
    printf("DSP: %d usecs     ",(int)(((int)(2 * n * log(n)/log(2)) + 42)/DSP_freq));
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    /* Call the DSPF_sp_fftSPxSP function in C6Accel*/
    C6accel_DSPF_sp_fftSPxSP (hC6accel,n,inX,w,(float*)pOutBuf_16bpp,brev,n_min, offset, n_max);

    END_AND_WRITE_BENCHMARK();
   
    /* read in reference from test file*/
    fread(pRefBuf_16bpp,  2*n, sizeof(float), refFd);
    printf("usecs\n\n");

       fclose(refFd);
    
    return 1;
}

int dsp_benchmark_DSPF_sp_ifftSPxSP(C6accel_Handle hC6accel,int n,int n_min,int offset, int n_max)
{
    FILE  *refFd,*refFd1;
    float *w;
    float *inX;
    unsigned char *brev;
    int    i,status;

    /*Benchmark C6accel_DSPF_sp_ifftSPxSP() */
     Memory_cacheWbInv(pSrcBuf_16bpp,2*n*sizeof(float));
     Memory_cacheWbInv(pWorkingBuf_16bpp,2*n*sizeof(float));
    /*Generate the 16 bit Fixed version of sine */
    inX = (float *)pSrcBuf_16bpp;
    if ((refFd1 = fopen(ref_ifftSPxSP_xinput,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_ifftSPxSP_xinput);
       return (0);
       }

    fread(inX,  2*n, sizeof(float), refFd1);
    fclose(refFd1);

    /* Read in the twiddle table, use pWorkingBuf_16bpp for the table */
    w = (float*)pWorkingBuf_16bpp;

    if ((refFd1 = fopen(ref_ifftSPxSP_twiddle,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_ifftSPxSP_twiddle);
       return (0);
       }
    fread(w,  2*n, sizeof(float), refFd1);
    fclose(refFd1);

    /* Read in the twiddle table, use pWorkingBuf_16bpp for the table */
    brev = (unsigned char*)pWorkingBuf2_16bpp;

    if ((refFd1 = fopen(ref_ifftSPxSP_brev,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_ifftSPxSP_brev);
       return (0);
       }
    fread(brev,  64, sizeof(unsigned char), refFd1);
    fclose(refFd1);


    if ((refFd = fopen(ref_ifftSPxSP_rslt,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_ifftSPxSP_rslt);
       return (0);
       }

    /* Clear output arrays*/
    memset (pOutBuf_16bpp,0xA5,2*n*sizeof(float));
    memset (pRefBuf_16bpp,0xA5,2*n*sizeof(float));

    printf("DSPF_sp_ifftSPxSP(Single Precision Inverse FFT)\n");
    printf("DSP: %d usecs     ",(int)(((int)(2 * n * log(n)/log(2)) + 42)/DSP_freq));
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    /* Call the DSPF_sp_ifftSPxSP function in C6Accel*/
    status= C6accel_DSPF_sp_ifftSPxSP (hC6accel,n,inX,w,(float*)pOutBuf_16bpp,brev,n_min, offset, n_max);
    
    END_AND_WRITE_BENCHMARK();
    
    /* read in reference from test file*/
    fread(pRefBuf_16bpp,  2*n, sizeof(float), refFd);
    printf("usecs\n\n");

     fclose(refFd);
     return 1;
}

/*
    This function generates the input data and also updates the
    input data arrays used by the various FFT kernels
*/
Void generateInput (Int32 numSinWaves, float *x_ref_float, Int32 n) {
    Int32 i, j;
    float sinWaveIncFreq, sinWaveMag;
    /*
        Based on numSinWave information, create the input data. The
        input data is first created using Floating point dataType. The
        Floating point data type is then converted to the appropriate
        Fixed point notation
    */
    /* Clear the input Floating point array */
    for (i = 0; i < n; i++) {
        x_ref_float[2*i] = (float)0.0;
        x_ref_float[2*i + 1] = (float)0.0;
    }

    /* Calculate the incremental freq for each sin wave */
    sinWaveIncFreq = ((float)3.142)/(numSinWaves*(float)1.0);
    /* Calculate the magnitude for each sin wave */
    sinWaveMag = (float)1.0/(numSinWaves * (float)1.0*n);
    /* Create the input array as sum of the various sin wave data */
    for (j = 0; j < numSinWaves; j++) {
        for (i = 0; i < n; i++) {
            x_ref_float[2*i] += sinWaveMag * (float)cos(sinWaveIncFreq*j*i);
            x_ref_float[2*i + 1] = (float) 0.0;
        }
    }


}


/* ======================================================================== */
/*  D2S -- Truncate a 'double' to a 'short', with clamping.                 */
/* ======================================================================== */
static short d2s(double d)
{
    d = floor(0.5 + d);  // Explicit rounding to integer //
    if (d >=  32767.0) return  32767;
    if (d <= -32768.0) return -32768;
    return (short)d;
}


/* ======================================================================== */
/*  GEN_TWIDDLE -- Generate twiddle factors for TI's custom FFTs.           */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is called as follows:                                  */
/*                                                                          */
/*          int gen_twiddle_fft16x16(short *w, int n)                       */
/*                                                                          */
/*          short *w      Pointer to twiddle-factor array                   */
/*          int   n       Size of FFT                                       */
/*                                                                          */
/*      The routine will generate the twiddle-factors directly into the     */
/*      array you specify.  The array needs to be approximately 2*N         */
/*      elements long.  (The actual size, which is slightly smaller, is     */
/*      returned by the function.)                                          */
/* ======================================================================== */
int gen_twiddle_fft16x16(short *w, int n)
{
    int i, j, k;
    double M = 32767.5;

    for (j = 1, k = 0; j < n >> 2; j = j << 2) {
        for (i = 0; i < n >> 2; i += j << 1) {
            w[k + 11] =  d2s(M * cos(6.0 * PI * (i + j) / n));
            w[k + 10] =  d2s(M * sin(6.0 * PI * (i + j) / n));
            w[k +  9] =  d2s(M * cos(6.0 * PI * (i    ) / n));
            w[k +  8] =  d2s(M * sin(6.0 * PI * (i    ) / n));

            w[k +  7] = -d2s(M * cos(4.0 * PI * (i + j) / n));
            w[k +  6] = -d2s(M * sin(4.0 * PI * (i + j) / n));
            w[k +  5] = -d2s(M * cos(4.0 * PI * (i    ) / n));
            w[k +  4] = -d2s(M * sin(4.0 * PI * (i    ) / n));

            w[k +  3] =  d2s(M * cos(2.0 * PI * (i + j) / n));
            w[k +  2] =  d2s(M * sin(2.0 * PI * (i + j) / n));
            w[k +  1] =  d2s(M * cos(2.0 * PI * (i    ) / n));
            w[k +  0] =  d2s(M * sin(2.0 * PI * (i    ) / n));

            k += 12;
        }
    }
    return k;
}


/* ======================================================================== */
/*  D2S -- Truncate a 'double' to a 'int',   with clamping.                 */
/* ======================================================================== */
static int d2i(double d)
{
    if (d >=  2147483647.0) return (int)0x7FFFFFFF;
    if (d <= -2147483648.0) return (int)0x80000000;
    return (int)d;
}


/* ======================================================================== */
/*  GEN_TWIDDLE -- Generate twiddle factors for TI's custom FFTs.           */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is called as follows:                                  */
/*                                                                          */
/*          int gen_twiddle_fft32x32(short *w, int n, double scale)         */
/*                                                                          */
/*          int    *w     Pointer to twiddle-factor array                   */
/*          int    n      Size of FFT                                       */
/*          double scale  Scale factor to apply to values.                  */
/*                                                                          */
/*      The routine will generate the twiddle-factors directly into the     */
/*      array you specify.  The array needs to be approximately 2*N         */
/*      elements long.  (The actual size, which is slightly smaller, is     */
/*      returned by the function.)                                          */
/* ======================================================================== */
int gen_twiddle_fft32x32(int *w, int n, double scale)
{
    int i, j, k, s=0, t;

    for (j = 1, k = 0; j < n >> 2; j = j << 2, s++) {
        for (i = t=0; i < n >> 2; i += j, t++) {
            w[k +  5] = d2i(scale * cos(6.0 * PI * i / n));
            w[k +  4] = d2i(scale * sin(6.0 * PI * i / n));

            w[k +  3] = d2i(scale * cos(4.0 * PI * i / n));
            w[k +  2] = d2i(scale * sin(4.0 * PI * i / n));

            w[k +  1] = d2i(scale * cos(2.0 * PI * i / n));
            w[k +  0] = d2i(scale * sin(2.0 * PI * i / n));

            k += 6;
        }
    }
    return k;
}

/* ======================================================================== */
/*  GEN_TWIDDLE -- Generate twiddle factors for TI's custom FFTs.           */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is called as follows:                                  */
/*                                                                          */
/*          int gen_twiddle_ifft16x16(short *w, int n)                      */
/*                                                                          */
/*          short  *w     Pointer to twiddle-factor array                   */
/*          int    n      Size of FFT                                       */
/*                                                                          */
/*      The routine will generate the twiddle-factors directly into the     */
/*      array you specify.  The array needs to be approximately 2*N         */
/*      elements long.  (The actual size, which is slightly smaller, is     */
/*      returned by the function.)                                          */
/* ======================================================================== */
int gen_twiddle_ifft16x16(short *w, int n)
{
    int i, j, k;
    double M = 32767.5;

    for (j = 1, k = 0; j < n >> 2; j = j << 2) {
        for (i = 0; i < n >> 2; i += j << 1) {
            w[k + 11] =  d2s(M * cos(6.0 * PI * (i + j) / n));
            w[k + 10] = -d2s(M * sin(6.0 * PI * (i + j) / n));
            w[k +  9] =  d2s(M * cos(6.0 * PI * (i    ) / n));
            w[k +  8] = -d2s(M * sin(6.0 * PI * (i    ) / n));

            w[k +  7] = -d2s(M * cos(4.0 * PI * (i + j) / n));
            w[k +  6] =  d2s(M * sin(4.0 * PI * (i + j) / n));
            w[k +  5] = -d2s(M * cos(4.0 * PI * (i    ) / n));
            w[k +  4] =  d2s(M * sin(4.0 * PI * (i    ) / n));

            w[k +  3] =  d2s(M * cos(2.0 * PI * (i + j) / n));
            w[k +  2] = -d2s(M * sin(2.0 * PI * (i + j) / n));
            w[k +  1] =  d2s(M * cos(2.0 * PI * (i    ) / n));
            w[k +  0] = -d2s(M * sin(2.0 * PI * (i    ) / n));

            k += 12;
        }
    }
    return k;
}

/* ======================================================================== */
/*  GEN_TWIDDLE -- Generate twiddle factors for TI's custom FFTs.           */
/*                                                                          */
/*  USAGE                                                                   */
/*      This routine is called as follows:                                  */
/*                                                                          */
/*          int gen_twiddle_ifft32x32(short *w, int n, double scale)         */
/*                                                                          */
/*          int    *w     Pointer to twiddle-factor array                   */
/*          int    n      Size of FFT                                       */
/*          double scale  Scale factor to apply to values.                  */
/*                                                                          */
/*      The routine will generate the twiddle-factors directly into the     */
/*      array you specify.  The array needs to be approximately 2*N         */
/*      elements long.  (The actual size, which is slightly smaller, is     */
/*      returned by the function.)                                          */
/* ======================================================================== */
int gen_twiddle_ifft32x32(int *w, int n, double scale)
{
    int i, j, k, s=0, t;

    for (j = 1, k = 0; j < n >> 2; j = j << 2, s++) {
        for (i = t=0; i < n >> 2; i += j, t++) {
            w[k +  5] = d2i(scale * cos(6.0 * PI * i / n));
            w[k +  4] = d2i(scale * sin(6.0 * PI * i / n));

            w[k +  3] = d2i(scale * cos(4.0 * PI * i / n));
            w[k +  2] = d2i(scale * sin(4.0 * PI * i / n));

            w[k +  1] = d2i(scale * cos(2.0 * PI * i / n));
            w[k +  0] = d2i(scale * sin(2.0 * PI * i / n));

            k += 6;
        }
    }
    return k;
}


/*
 * Benchmark all possible IMG Histogram combinations in this function
 */
int dsp_benchmark_IMG_histogram(C6accel_Handle hC6accel,unsigned int inputWidth, unsigned int inputHeight)
{
    FILE *srcFd, *refFd;
    unsigned short *p16bitBuf;
    int i;

    /* Open source image file */
    if ((srcFd = fopen(srcFileName,"rb")) == NULL) {
       printf("Failed to open srcFd on %s\n",srcFileName);
       return (0);
       }

    if ((refFd = fopen(ref_histogram_8,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_histogram_8);
       return (0);
       }

    /* Read in source file */
    fread(pSrcBuf_16bpp,inputWidth*inputHeight,1,srcFd);

    /* Clear working and output arrays*/
    memset (pWorkingBuf_16bpp,0x00,1024*sizeof(short));

    memset (pOutBuf_16bpp,0x00,256*sizeof(short));
    memset (pRefBuf_16bpp,0x00,256*sizeof(short));

    printf("\n* IMG_histogram_8(8 bit Histogram)\n");
    printf("DSP: %d usecs     ",(int)(((10*inputWidth*inputHeight/8)+430)/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    /* Call the histogram */
    C6accel_IMG_histogram_8(hC6accel,pSrcBuf_16bpp,inputWidth*inputHeight,1,
            (unsigned short*)pWorkingBuf_16bpp,
            (unsigned short*)pOutBuf_16bpp);

    END_AND_WRITE_BENCHMARK();
    printf("usecs\n\n");
    /* reset temp histogram*/
    memset (pWorkingBuf_16bpp,0x00,1024*sizeof(short));

    /* read in reference */
    fread(pRefBuf_16bpp,  (1<<IMG_BITS_HISTOGRAM_16), sizeof(short), refFd);

    fclose(refFd);

    return (1);
}


/*
 * Benchmark all possible IMG CONV combinations in this function
 */

const signed char gaussian_3x3_8s_1Q7[]={-1,-1,-1,
                                -1,127,-1,
                                -1,-1,-1};

const unsigned char gaussian_3x3_8_1Q7[]={1,1,1,
                                          1,119,1,
                                          1,1,1};

const short gaussian_3x3_16s_1Q15[]={-256,-256, -256,
                                   -256,32512,-256,
                                   -256,-256, -256};

int dsp_benchmark_IMG_conv(C6accel_Handle hC6accel,unsigned int inputWidth, unsigned int inputHeight)
{
    unsigned int   row,i;
    FILE           *srcFd, *refFd;
    short          *p16bitInBuf;
    short          *p16bitOutBuf;
    /* Pointer to hold a CMem for the mask*/
    short          *p16GaussianCmem;

    /* Allocate CMEM memory for 3x3 short mask*/
    p16GaussianCmem = Memory_alloc(18, &testfxnsMemParams);

    /* Benchmark IMG_sobel_3x3_8() */
    /* Open source file */
    if ((srcFd = fopen(srcFileName,"rb")) == NULL) {
       printf("Failed to open srcFd on %s\n",srcFileName);
       return (0);
     }
    /* OPen reference output file*/
    if ((refFd = fopen(ref_conv_3x3_8,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_conv_3x3_8);
       return (0);
     }

    /* Read in source file*/
    fread(pSrcBuf_16bpp,inputWidth*inputHeight,1,srcFd);

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight);
   

    /* Set Up required Gaussian */
    memcpy( p16GaussianCmem,gaussian_3x3_8s_1Q7,9);

    printf("* IMG_conv_3x3_i8_c8s(8 bit Convolution with 3x3 mask)\n");
    printf("DSP: %d usecs     ",((inputHeight*((6*inputWidth/4)+40))/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();
    /* Call the conv_codec one line at a time
       The 3x3 array read lines n,n+1,n+2 from input image to calculate output line n.
       Therefore, the last 2 rows of the image cannot be calculated as they read past the original image.
       So only calulate on height-2 rows for valid data.   */

#ifdef USE_NEW_FRAME_APIS
        C6accel_IMG_conv_3x3_i8_c8s_Frame(hC6accel,pSrcBuf_16bpp,pOutBuf_16bpp,inputHeight-2,inputWidth, (char*)p16GaussianCmem,7);
#else

   /*  the 3x3 8 bit mode is line only !!            */
    for(row=0;row<inputHeight-2;row++)
        C6accel_IMG_conv_3x3_i8_c8s(hC6accel,pSrcBuf_16bpp+row*inputWidth,pOutBuf_16bpp+row*inputWidth,inputWidth, (signed char*)p16GaussianCmem,7);
#endif
 
    END_AND_WRITE_BENCHMARK();
   
    /* read in reference */
    fread(pRefBuf_16bpp, inputWidth*(inputHeight-2), sizeof(unsigned char), refFd);

    printf("usecs\n\n");

   fclose(srcFd);
   fclose(refFd);

     /*  Release Gaussian CMEM */
     Memory_free(p16GaussianCmem,18,&testfxnsMemParams);

    return (1);
}




/*
 * Benchmark all possible IMG Sobel combinations in this function
 */
int dsp_benchmark_IMG_sobel(C6accel_Handle hC6accel,unsigned int inputWidth, unsigned int inputHeight)
{
    unsigned int                i,row;
    FILE *srcFd, *refFd;
    unsigned short *p16bitBuf;

    /* Benchmark IMG_sobel_3x3_8() in c6accel */
    if ((srcFd = fopen(srcFileName,"rb")) == NULL) {
       printf("Failed to open srcFd on %s\n",srcFileName);
       return (0);
     }
    if ((refFd = fopen(ref_sobel_3x3_8,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_sobel_3x3_8);
       return (0);
     }

    /* Read in source file */
    fread(pSrcBuf_16bpp,inputWidth*inputHeight,1,srcFd);

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight);
   
    printf("* IMG_sobel_3x3_8(8 bit 3x3 Sobel Edge detection)\n");
    printf("DSP: %d usecs     ",(int)(((12*((inputWidth-2)*inputHeight)/8)+48)/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    /* Call the sobel codec to perform edge detection */
    /* Sobel function handles the adjustment 1 line border with undefined pixels internally */
    C6accel_IMG_sobel_3x3_8(hC6accel,pSrcBuf_16bpp,pOutBuf_16bpp,inputWidth, inputHeight);
    END_AND_WRITE_BENCHMARK();
    
    /* read in reference */
    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(unsigned char), refFd);
    printf("usecs\n\n");

    fclose(refFd);
    return (1);
}


/*
 * Benchmark all possible IMG adds combinations in this function
 */
int dsp_benchmark_IMG_adds(C6accel_Handle hC6accel,unsigned int inputWidth, unsigned int inputHeight)
{
    unsigned int                i;
    FILE *srcFd, *refFd;
    short *p16bitBuf,*p16bitInBuf;

    /* Benchmark IMG_adds_8() in c6accel */
    /* Open source image file */
    if ((srcFd = fopen(srcFileName,"rb")) == NULL) {
       printf("Failed to open srcFd on %s\n",srcFileName);
       return (0);
    }
    if ((refFd = fopen(ref_adds_8,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_adds_8);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight);
   

    /* Inplace Adding a constant to an image Source = OutBuf
      Read in source file to output buffer
     */
    fread(pOutBuf_16bpp,inputWidth*inputHeight,1,srcFd);

    /* Read in source file */
    fread(pSrcBuf_16bpp,inputWidth*inputHeight,1,srcFd);

    printf("IMG_adds_8(8 bit Image addition)\n");
    printf("DSP: %d usecs     ",(int)(((4*(inputWidth*inputHeight)/3)+48)/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    /* Call the addS_8 to add 1 */
    C6accel_IMG_addS_8(hC6accel,(char*)pOutBuf_16bpp,(char*)pOutBuf_16bpp,1,inputWidth*inputHeight);
    END_AND_WRITE_BENCHMARK();
    
    /*  read in reference output */
    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(char), refFd);
    printf("usecs\n");

   fclose(refFd);
   return (1);
}


/*
 * Benchmark all possible IMG yuv demux + PL420 to RGB combinations in this function
 */

const short yuv2rgb_coeff[] = { 0x2000, 0x2BDD, -0x0AC5, -0x1658, 0x3770 };

int dsp_benchmark_IMG_YC_demux(C6accel_Handle hC6accel,unsigned int inputWidth, unsigned int inputHeight)
{
    unsigned int                i,j;
    FILE *srcFd, *refFd;
    short *p16bitBuf;
    unsigned char *pY, *pCb, *pCr;
    short *psY, *psCb, *psCr;

    int tempInt;
    unsigned int *pInt;
    unsigned char *pChar;

    /* Create a CMem heap array that can be translated for DSP */
    short *pYUV2RGB_Cmem;
    pYUV2RGB_Cmem = Memory_alloc(5*sizeof(short),&testfxnsMemParams);

    /* Benchmark IMG_YC_demux() kenrel in c6accel */
    if ((srcFd = fopen(src_yuyv,"rb")) == NULL) {
       printf("Failed to open srcFd on %s\n",src_yuyv);
       return (0);
    }
    if ((refFd = fopen(ref_yc_demux_8,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_yc_demux_8);
       return (0);
    }

    /* Read in source file which is Bigendian 422 CbY1CrY0*/
    fread(pSrcBuf_16bpp,inputWidth*inputHeight,sizeof(short),srcFd);

    /* and swap to LE 422 Y0CrY1Cb*/
    pChar = pSrcBuf_16bpp;
    pInt = (unsigned int*)pSrcBuf_16bpp;
    for(i=0;i<inputWidth*inputHeight/2;i++){
       //Cb
       tempInt = (*pChar++) << 24;
       //Y1
       tempInt |= *pChar++ ;
       //Cr
       tempInt |=(*pChar++) << 8;
       //Y0
       tempInt |=(*pChar++)<< 16;

       *pInt++ = tempInt;
     }


    /* Clear output arrays
       split each to hold 3 separate arrays of chars
       y[h*w]
       cb[h*w/2]
       cr[h*w/2]
    */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*2*sizeof(char));
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*2*sizeof(char));
    printf("* IMG_yc_demux_le8_c(8 bit 422ILE to 422PL conversion)\n");
    printf("DSP: %d usecs     ",(int)(((15*(inputWidth*inputHeight)/7)+45)/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    pY = pOutBuf_16bpp;
    pCb = pY + inputWidth*inputHeight;
    pCr = pCb + (inputWidth*inputHeight)/2;
    START_BENCHMARK();
    /* Call the yc_demux_be8 to generate 422*/
    C6accel_IMG_yc_demux_le8_8(hC6accel,inputWidth*inputHeight,(unsigned char*)pSrcBuf_16bpp,pY, pCr, pCb);
    END_AND_WRITE_BENCHMARK();
   

    /* read in reference output */
    fread(pRefBuf_16bpp, inputWidth*inputHeight*2, sizeof(char), refFd);
    printf("usecs\n\n");

    fclose(refFd);

    /* Now we have YUV422 planar in memory will use for 422 to RGB function */
    if ((refFd = fopen(ref_rgb_565,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_rgb_565);
       return (0);
     }

    /* Pick up 422Pl source buffer from reference buffer of previous step*/
    memcpy(pWorkingBuf_16bpp,pRefBuf_16bpp,inputWidth*inputHeight*2*sizeof(char));
    /*Copy YUV2RGB to cmem pointer */
    memcpy(pYUV2RGB_Cmem,yuv2rgb_coeff,5*sizeof(short));

    /* Clear output arrays for 16 bit rgb */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));

    pY = pWorkingBuf_16bpp;
    pCb = pY + inputWidth*inputHeight;
    pCr = pCb + (inputWidth*inputHeight)/2;

    printf("* IMG_yuv422pl_to_rgb565(YUV422PL to RGB565 CSC)\n");
    printf("DSP: %d usecs     ",(int)(((15*(inputWidth*inputHeight)/8)+65)/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    /* Call the _ycbcr422pl_to_rgb565 as we have 422P data in memory */
    C6accel_IMG_ycbcr422pl_to_rgb565(hC6accel,pYUV2RGB_Cmem,pY, pCb, pCr,(unsigned short*)pOutBuf_16bpp,inputHeight*inputWidth);
    END_AND_WRITE_BENCHMARK();
   

    /* read in reference output */
    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(short), refFd);
    printf("usecs\n\n");

    fclose(refFd);

    /* Free YUV2RGB CMEM array*/
    Memory_free(pYUV2RGB_Cmem,sizeof(short)*5,&testfxnsMemParams);
    
    return (1);
}

#if (PLATFORM == 138)
/* Benchmark for Floating point kernels */
/*
 * Benchmark function for arithmetic rts single precision functions in this function
 */
int dsp_benchmark_MATH_RTSFLT(C6accel_Handle hC6accel,unsigned int n)
{
// FILE *refFd;
 float *in1;
 float *in2;
 float *out;
 int i;

/* Generate N bytes of 16 bit raw sine wave for use in tests
     Reuse pWorkingBuf2_16bpp for this
   */
    in1 = (float *)pSrcBuf_16bpp;
    in2 = (float *)pWorkingBuf2_16bpp;
    out = (float *)pRefBuf_16bpp;

   /*Read inputs */
   for (i=0; i< n; i++){
       in1[i] = randflt(-10.0, 10.0);
    }  
    
    for (i=0; i< n; i++){
    // There are some functions which do not permit 2nd input to be 0 (due to divide by zero condition)
       in2[i] = randflt(0.01, 10.0);
    } 
    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    printf("MATH_cossp(Single Precsion Cosine)\n");
    printf("DSP: %d usecs     ",(int)(((74*n/128))/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_MATH_cossp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
   
    printf("usecs\n\n");

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    printf("MATH_sinsp(Single Precision Sine)\n");
    printf("DSP: %d usecs     ",(int)(((69*n/128))/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_MATH_sinsp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    
    printf("usecs\n\n");

     /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    printf("MATH_atansp(Single Precision Arc Tan)\n");
    printf("DSP: %d usecs     ",(int)(((91*n/128))/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_MATH_atansp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
   
    
    printf("usecs\n\n");

     /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    printf("MATH_logsp(Single Precision log)\n");
    printf("DSP: %d usecs     ",(int)(((74*n/128))/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_MATH_expsp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    printf("usecs\n\n");

     /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    printf("MATH_expsp(Single Precision exponential)\n");
    printf("DSP: %d usecs     ",(int)(((82*n/128))/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_MATH_expsp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    printf("usecs\n\n");

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    printf("MATH_powsp(Single precision Power function)\n");
     printf("DSP: %d usecs     ",(int)(((181*n/128))/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_MATH_powsp(hC6accel,(float *)in1,(float *)in2,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    printf("usecs\n\n");

   return 1;
}

/*

 * Benchmark function for arithmetic rts double precision functions in this function
 */
int dsp_benchmark_MATH_RTSFLTDP(C6accel_Handle hC6accel,unsigned int n)
{
 FILE *refFd;
 double *in1;
 double *in2;
 double *out;
 int i;

 /* Generate N bytes of 16 bit raw sine wave for use in tests

     Reuse pWorkingBuf2_16bpp for this
   */
    in1 = (double *)pSrcBuf_16bpp;
    in2 = (double *)pWorkingBuf2_16bpp;
    out = (double *)pRefBuf_16bpp;
    
    for (i=0; i< n; i++){
    // There are some functions which do not permit 2nd input to be 0 (due to divide by zero condition)
       in1[i] = (double)(randflt(0.0, 10.0));
    } 
    
    for (i=0; i< n; i++){
    // There are some functions which do not permit 2nd input to be 0 (due to divide by zero condition)
       in2[i] = (double)(randflt(0.01, 10.0));
    } 
   
     /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    printf("MATH_cosdp(Double Precsion Cosine)\n");
    printf("DSP: %d usecs     ",(int)(((155*n/128))/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_MATH_cosdp(hC6accel,in1,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
   
    printf("usecs\n\n");

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    printf("MATH_sindp(Double Precision Sine)\n");
    printf("DSP: %d usecs     ",(int)(((150*n/128))/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_MATH_sindp(hC6accel,in1,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    
    printf("usecs\n\n");

     /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    printf("MATH_atandp(Double Precision Arc Tan)\n");
    printf("DSP: %d usecs     ",(int)((322*n/128)/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_MATH_atandp(hC6accel,in1,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
   
    
    printf("usecs\n\n");

     /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    printf("MATH_logdp(Double Precision log)\n");
    printf("DSP: %d usecs     ",(int)((302*n/128)/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_MATH_expdp(hC6accel,in1,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    printf("usecs\n\n");

     /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    printf("MATH_expdp(Double Precision exponential)\n");
    printf("DSP: %d usecs     ",(int)((229*n/128)/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_MATH_expdp(hC6accel,in1,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    printf("usecs\n\n");

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    printf("MATH_powdp(Double precision Power function)\n");
     printf("DSP: %d usecs     ",(int)((522*n/128)/DSP_freq) );
    printf("ARM+DSP ( C6Accel ) : ");
    START_BENCHMARK();

    C6accel_MATH_powdp(hC6accel,(double *)in1,(double *)in2,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    printf("usecs\n\n");
    return 1;
}



#endif

/* Generates a psuedo-random float between 0.0 and 0.999... */
float randfloat(){
    return (rand()/((float)(RAND_MAX)+1));
}

float randflt(float min, float max){
    if (min>max){
        return randfloat()*(min-max)+max;
    }
    else
    {
        return randfloat()*(max-min)+min;
    }
}

/* Generates a psuedo-random integer between 0 and 32767*/
int randint()
{
    return rand();
}

/* Generates a psuedo-random integer between 0 and max */
int randuint(int max)
{
    return (int)(max*rand()/(RAND_MAX+1.0));
}

/* Generates a psuedo-random integer between min and max */
int randInt(int min, int max)
{
    if (min>max){
        return max+(int)((min-max+1)*rand()/(RAND_MAX+1.0));
    }
    else{
        return min+(int)((max-min+1)*rand()/(RAND_MAX+1.0));
    }
}



