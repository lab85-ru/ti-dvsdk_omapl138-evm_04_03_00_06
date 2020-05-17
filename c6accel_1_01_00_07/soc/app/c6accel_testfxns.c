/*================================================================================*/
/*   Copyright (c) 2010, Texas Instruments Incorporated                           */
/*   All rights reserved.                                                         */
/*                                                                                */
/*   Name: C6Accel_testfxns.c                                                     */
/*                                                                                */
/*   Descriptions:                                                                */
/*   File contains code to test kernels in the C6Accel codec                      */
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
#include "c6accel_test_inputs.h"

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

/* Generate a floating point sine wave containing numSinWaves signals with n samples*/
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

/*Function declaration for comparison functions to validate result*/
unsigned int compare8bitArrays(unsigned char *pSrc1, unsigned char *pSrc2,unsigned int length);
unsigned int compare16bitArrays(unsigned short *pSrc1, unsigned short *pSrc2,unsigned int length);
unsigned int compare32bitArrays(unsigned int *pSrc1, unsigned int *pSrc2,unsigned int length);
unsigned int comparefloatArrays(float *pSrc1, float *pSrc2,unsigned int length);
unsigned int comparedoubleArrays(double *pSrc1, double *pSrc2,unsigned int length);

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
char ref_ifft_32x32[]="test_files/ifft_32x32_64k_sine_wave_64kHz_sampling_ref.dat";

/*Filenames for autocorrelation testing*/
char ref_autocor[]="test_files/autocor_64k_sine_wave_64kHz_sampling_ref.dat";
char ref_src_autocor[]="test_files/src_autocor_64k_sine_wave_64kHz_sampling_ref.dat";

/*Filenames for dotprod testing*/
char ref_dotprod[]="test_files/dotprod_ref.dat";
char ref_dotp_sqr[]="test_files/dotp_sqr_ref.dat";

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
char ref_histogram_16[]="test_files/histogram_16_valve_640x480_luma8bpp_ref.bin";

/* Filenames for median testing*/
char ref_median_3x3_8[]="test_files/median_3x3_8_valve_640x480_luma8bpp_ref.bin";
char ref_median_3x3_16[]="test_files/median_3x3_16_valve_640x480_luma8bpp_ref.bin";
char ref_median_3x3_16s[]="test_files/median_3x3_16s_valve_640x480_luma8bpp_ref.bin";

/* Filenames for conv testing*/
char ref_conv_3x3_8[]="test_files/conv_3x3_8_valve_640x480_luma8bpp_ref.pgm";
char ref_conv_3x3_16s[]="test_files/conv_3x3_16s_valve_640x480_luma8bpp_ref.dat";

/* Filenames for corr testing*/
char ref_corr_3x3_8[]="test_files/corr_3x3_8_valve_640x480_luma8bpp_ref.pgm";
char ref_corr_3x3_16s[]="test_files/corr_3x3_16s_valve_640x480_luma8bpp_ref.dat";

/* Filenames for Sobel testing */
char ref_sobel_3x3_8[]="test_files/sobel_3x3_8_valve_640x480_luma8bpp_ref.pgm";
char ref_sobel_3x3_16[]="test_files/sobel_3x3_16_valve_640x480_luma8bpp_ref.pgm";

/* Filenames for muls testing */
char ref_muls_8[]="test_files/muls_8_valve_640x480_luma8bpp_ref.pgm";
char ref_muls_16[]="test_files/muls_16_valve_640x480_luma8bpp_ref.pgm";

/* Filenames for adds testing */
char ref_adds_8[]="test_files/adds_8_valve_640x480_luma8bpp_ref.pgm";
char ref_adds_16[]="test_files/adds_16_valve_640x480_luma8bpp_ref.pgm";

/* Filenames for subs testing */
char ref_subs_8[]="test_files/subs_8_valve_640x480_luma8bpp_ref.pgm";
char ref_subs_16[]="test_files/subs_16_valve_640x480_luma8bpp_ref.pgm";

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


/*File name to store ref output for arithmetic rts math operations*/
char ref_addsp_i[]="test_files/addsp_ref.dat";
char ref_subsp_i[]="test_files/subsp_ref.dat";
char ref_mpysp_i[]="test_files/mpysp_ref.dat";
char ref_divsp_i[]="test_files/divsp_ref.dat";
char ref_sqrtsp_i[]="test_files/sqrtsp_ref.dat";
char ref_recipsp_i[]="test_files/recipsp_ref.dat";

/* File name to store ref output for conversion rts functions */
char ref_intsp_i[]="test_files/intsp_ref.dat";
char ref_uintsp_i[]="test_files/uintsp_ref.dat";
char ref_spint_i[]="test_files/spint_ref.dat";
char ref_spuint_i[]="test_files/spuint_ref.dat";

/* File name to store ref output for IQMATH conversion kernels */
char ref_iqn[]="test_files/iqn_ref.dat";
char ref_ftoiqn[]="test_files/ftoiqn_ref.dat";
char ref_iqntof[]="test_files/iqntof_ref.dat";
char ref_iqxtoiqy[]="test_files/iqxtoiqy_ref.dat";
char ref_iqnint[]="test_files/iqnint_ref.dat";
char ref_iqnfrac[]="test_files/iqnfrac_ref.dat";

/*File name to store ref output for IQMATH mathematical kernels*/
char ref_iqnmpy[]="test_files/iqnmpy_ref.dat";
char ref_iqnmpyIQx[]="test_files/iqnmpyIQx_ref.dat";
char ref_iqnrmpy[]="test_files/iqnrmpy_ref.dat";
char ref_iqnrsmpy[]="test_files/iqnrsmpy_ref.dat";
char ref_iqnmpyI32int[]="test_files/iqnmpyI32int_ref.dat";
char ref_iqnmpyI32frac[]="test_files/iqnmpyI32frac_ref.dat";
char ref_iqndiv[]="test_files/iqndiv_ref.dat";

/*File name to store ref output fot Miscellaneous arithmetic kernels*/
char ref_iqnlog[]="test_files/iqnlog_ref.dat";
char ref_iqnabs[]="test_files/iqnabs_ref.dat";
char ref_iqnsqrt[]="test_files/iqnsqrt_ref.dat";
char ref_iqnisqrt[]="test_files/iqnisqrt_ref.dat";
char ref_iqnexp[]="test_files/iqnexp_ref.dat";
char ref_iqnpow[]="test_files/iqnpow_ref.dat";
char ref_iqnmag[]="test_files/iqnmag_ref.dat";

/*File name to store ref output for IQMATH trignometric kernels*/
char ref_iqnsin[]="test_files/iqnsin_ref.dat";
char ref_iqnsinPU[]="test_files/iqnsinPU_ref.dat";
char ref_iqncos[]="test_files/iqncos_ref.dat";
char ref_iqncosPU[]="test_files/iqncosPU_ref.dat";
char ref_iqnatan2[]="test_files/iqnatan2_ref.dat";
char ref_iqnatan2PU[]="test_files/iqnatan2PU_ref.dat";

/* Floating point Kernel test vectors */
char ref_autocor_x[]="test_files/flt_autocor_input_ref.dat";
char ref_autocor_rslt[]="test_files/flt_autocor_rslt_ref.dat";

char ref_biquad_xinput[]="test_files/flt_biquad_xinput_ref.dat";
char ref_biquad_y[]="test_files/flt_biquad_y_ref.dat";
char ref_biquad_ha[]="test_files/flt_biquad_ha_ref.dat";
char ref_biquad_hb[]="test_files/flt_biquad_hb_ref.dat";
char ref_biquad_delay[]="test_files/flt_biquad_delay_ref.dat";

char ref_convol_xinput[]="test_files/flt_convol_xinput_ref.dat";
char ref_convol_rslt[]="test_files/flt_convol_rslt_ref.dat";
char ref_convol_hinput[]="test_files/flt_convol_hinput_ref.dat";

char ref_dotprod_x1[]="test_files/flt_dotprod_x1_ref.dat";
char ref_dotprod_x2[]="test_files/flt_dotprod_x2_ref.dat";
char ref_dotprod_rslt[]="test_files/flt_dotprod_rslt_ref.dat";

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

/* Single precision RTS functions test vectors */
char ref_atan2sp_x1[]="test_files/flt_atan2sp_x1_ref.dat";
char ref_atan2sp_x2[]="test_files/flt_atan2sp_x2_ref.dat";
char ref_atan2sp_rslt[]="test_files/flt_atan2sp_rslt_ref.dat";

char ref_atansp_x1[]="test_files/flt_atansp_x1_ref.dat";
char ref_atansp_rslt[]="test_files/flt_atansp_rslt_ref.dat";

char ref_cossp_x1[]="test_files/flt_cossp_x1_ref.dat";
char ref_cossp_rslt[]="test_files/flt_cossp_rslt_ref.dat";

char ref_divsp_x1[]="test_files/flt_divsp_x1_ref.dat";
char ref_divsp_rslt[]="test_files/flt_divsp_rslt_ref.dat";

char ref_exp2sp_x1[]="test_files/flt_exp2sp_x1_ref.dat";
char ref_exp2sp_rslt[]="test_files/flt_exp2sp_rslt_ref.dat";

char ref_exp10sp_x1[]="test_files/flt_exp10sp_x1_ref.dat";
char ref_exp10sp_rslt[]="test_files/flt_exp10sp_rslt_ref.dat";

char ref_expsp_x1[]="test_files/flt_expsp_x1_ref.dat";
char ref_expsp_rslt[]="test_files/flt_expsp_rslt_ref.dat";

char ref_log2sp_x1[]="test_files/flt_log2sp_x1_ref.dat";
char ref_log2sp_rslt[]="test_files/flt_log2sp_rslt_ref.dat";

char ref_log10sp_x1[]="test_files/flt_log10sp_x1_ref.dat";
char ref_log10sp_rslt[]="test_files/flt_log10sp_rslt_ref.dat";

char ref_logsp_x1[]="test_files/flt_logsp_x1_ref.dat";
char ref_logsp_rslt[]="test_files/flt_logsp_rslt_ref.dat";

char ref_powsp_x1[]="test_files/flt_powsp_x1_ref.dat";
char ref_powsp_x2[]="test_files/flt_powsp_x2_ref.dat";
char ref_powsp_rslt[]="test_files/flt_powsp_rslt_ref.dat";

char ref_recipsp_x1[]="test_files/flt_recipsp_x1_ref.dat";
char ref_recipsp_rslt[]="test_files/flt_recipsp_rslt_ref.dat";

char ref_rsqrtsp_x1[]="test_files/flt_rsqrtsp_x1_ref.dat";
char ref_rsqrtsp_rslt[]="test_files/flt_rsqrtsp_rslt_ref.dat";

char ref_sinsp_x1[]="test_files/flt_sinsp_x1_ref.dat";
char ref_sinsp_rslt[]="test_files/flt_sinsp_rslt_ref.dat";

char ref_sqrtsp_x1[]="test_files/flt_sqrtsp_x1_ref.dat";
char ref_sqrtsp_rslt[]="test_files/flt_sqrtsp_rslt_ref.dat";

/* Double precision RTS functions test vectors */
char ref_atan2dp_x1[]="test_files/flt_atan2dp_x1_ref.dat";
char ref_atan2dp_x2[]="test_files/flt_atan2dp_x2_ref.dat";
char ref_atan2dp_rslt[]="test_files/flt_atan2dp_rslt_ref.dat";

char ref_atandp_x1[]="test_files/flt_atandp_x1_ref.dat";
char ref_atandp_rslt[]="test_files/flt_atandp_rslt_ref.dat";

char ref_cosdp_x1[]="test_files/flt_cosdp_x1_ref.dat";
char ref_cosdp_rslt[]="test_files/flt_cosdp_rslt_ref.dat";

char ref_divdp_x1[]="test_files/flt_divdp_x1_ref.dat";
char ref_divdp_rslt[]="test_files/flt_divdp_rslt_ref.dat";

char ref_exp2dp_x1[]="test_files/flt_exp2dp_x1_ref.dat";
char ref_exp2dp_rslt[]="test_files/flt_exp2dp_rslt_ref.dat";

char ref_exp10dp_x1[]="test_files/flt_exp10dp_x1_ref.dat";
char ref_exp10dp_rslt[]="test_files/flt_exp10dp_rslt_ref.dat";

char ref_expdp_x1[]="test_files/flt_expdp_x1_ref.dat";
char ref_expdp_rslt[]="test_files/flt_expdp_rslt_ref.dat";

char ref_log2dp_x1[]="test_files/flt_log2dp_x1_ref.dat";
char ref_log2dp_rslt[]="test_files/flt_log2dp_rslt_ref.dat";

char ref_log10dp_x1[]="test_files/flt_log10dp_x1_ref.dat";
char ref_log10dp_rslt[]="test_files/flt_log10dp_rslt_ref.dat";

char ref_logdp_x1[]="test_files/flt_logdp_x1_ref.dat";
char ref_logdp_rslt[]="test_files/flt_logdp_rslt_ref.dat";

char ref_powdp_x1[]="test_files/flt_powdp_x1_ref.dat";
char ref_powdp_x2[]="test_files/flt_powdp_x2_ref.dat";
char ref_powdp_rslt[]="test_files/flt_powdp_rslt_ref.dat";

char ref_recipdp_x1[]="test_files/flt_recipdp_x1_ref.dat";
char ref_recipdp_rslt[]="test_files/flt_recipdp_rslt_ref.dat";

char ref_rsqrtdp_x1[]="test_files/flt_rsqrtdp_x1_ref.dat";
char ref_rsqrtdp_rslt[]="test_files/flt_rsqrtdp_rslt_ref.dat";

char ref_sindp_x1[]="test_files/flt_sindp_x1_ref.dat";
char ref_sindp_rslt[]="test_files/flt_sindp_rslt_ref.dat";

char ref_sqrtdp_x1[]="test_files/flt_sqrtdp_x1_ref.dat";
char ref_sqrtdp_rslt[]="test_files/flt_sqrtdp_rslt_ref.dat";

void VLIB_erode_bin_mask 
(
    const unsigned char * in_data, 
    unsigned char       * out_data, 
    const char          * mask, 
    int cols,
    int pitch
);

void VLIB_dilate_bin_mask 
(
    const unsigned char * in_data, 
    unsigned char       * out_data, 
    const char          * mask, 
    int cols,
    int pitch
);

/*
 * Test for FFT kernels
 */

int c6accel_test_DSP_FFT(C6accel_Handle hC6accel,unsigned int n)
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

    /*Test C6accel_DSP_fft16x16() */

    /*Generate the 16 bit fixed version of sine */
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

    LOG_STRING("C6accel_DSP_fft16x16(),");
    START_BENCHMARK();

    /* Call the fft16x16 function in C6Accel*/
    C6accel_DSP_fft16x16 (hC6accel,twiddleTable16, n,inputArray16,(short*)pOutBuf_16bpp);

    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSP_fft16x16() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference from test file*/
    fread(pRefBuf_16bpp,  2*n, sizeof(short), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp, (unsigned short*)pRefBuf_16bpp,2*n) != 1)
         {printf("C6accel_DSP_fft16x16() failed\n");
          return 0;
          }

     fclose(refFd);

    /* Test C6accel_DSP_fft32x32() */
    /* Read in N bytes of 16 bit input signal for tests*/
    /* Generate the 32 bit fixed version of sine */
    inputArray32 = (int *)pSrcBuf_16bpp;

    if ((refFd1 = fopen(ref_src_fft32x32,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_src_fft32x32);
       return (0);
       }

    fread(inputArray32,  2*n, sizeof(int), refFd1);
    fclose(refFd1);

    /* Generate the twiddle table, use pWorkingBuf_16bpp for the table */
      twiddleTable32 = (int *)pWorkingBuf_16bpp;

      if ((refFd1 = fopen(ref_twiddle_fft32x32,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_twiddle_fft32x32);
       return (0);
       }

    fread(twiddleTable32,  2*n, sizeof(int), refFd1);
    fclose(refFd1);

    if ((refFd = fopen(ref_fft_32x32,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_fft_32x32);
       return (0);
      }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,2*n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,2*n*sizeof(int));

    LOG_STRING("C6accel_DSP_fft32x32(),");
    START_BENCHMARK();

    /* Call the fft32x32 in the C6Accel */
    C6accel_DSP_fft32x32 (hC6accel,twiddleTable32, n,inputArray32,(int *)pOutBuf_16bpp);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSP_fft32x32() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

   /* read in reference output created on the dsp using the same input */
    fread(pRefBuf_16bpp,  2*n, sizeof(int), refFd);

    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int*)pOutBuf_16bpp, (unsigned int*)pRefBuf_16bpp,2*n) != 1){
         printf("C6accel_DSP_fft32x32() failed\n");
         return 0;
     }

      fclose(refFd);

    printf("Tests for FFT functions successful\n");
    return (1);
}

/*
 * Test all Inverse FFT kernels in c6accel
 */
int c6accel_test_DSP_IFFT(C6accel_Handle hC6accel,unsigned int n)
{
    FILE  *refFd, *refFd1;
    short *twiddleTable16;
    short *inputArray16;
    int   *inputArray32;
    int   *twiddleTable32;
    double scale = 2147483647.5;

    /* Test C6accel_DSP_ifft16x16() */
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

    LOG_STRING("C6accel_DSP_ifft16x16(),");
    START_BENCHMARK();

    /* Call the ifft16x16 kernel in the codec */
    C6accel_DSP_ifft16x16(hC6accel,twiddleTable16, n,inputArray16,(short*)pOutBuf_16bpp);
    END_AND_WRITE_BENCHMARK();

    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSP_ifft16x16() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /*  read in reference output */
    fread(pRefBuf_16bpp,  2*n, sizeof(short), refFd);

    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp, (unsigned short*)pRefBuf_16bpp,2*n) != 1){
         printf("C6accel_DSP_ifft16x16() failed\n");
         return 0;
       }

    fclose(refFd);

   /*Test C6accel_DSP_ifft32x32()*/
    inputArray32 = (int *)pSrcBuf_16bpp;
    if ((refFd1 = fopen(ref_fft_32x32,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_fft_32x32);



       return (0);
       }

    fread(inputArray32,  2*n, sizeof(int), refFd1);
    fclose(refFd1);

    /* Generate the twiddle table, use pWorkingBuf_16bpp for the table */
    twiddleTable32 = (int *)pWorkingBuf_16bpp;
    gen_twiddle_ifft32x32(twiddleTable32, n, scale);

    if ((refFd = fopen(ref_ifft_32x32,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_ifft_32x32);
       return (0);
     }

    /*  Clear output arrays */
    memset (pOutBuf_16bpp,0x00,2*n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,2*n*sizeof(int));

    LOG_STRING("C6accel_DSP_ifft32x32(),");
    START_BENCHMARK();

    /* Call the 32 bit ifft in c6accel */
    C6accel_DSP_ifft32x32(hC6accel,twiddleTable32, n,inputArray32,(int*)pOutBuf_16bpp);

    END_AND_WRITE_BENCHMARK();
    
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSP_ifft32x32() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  2*n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level*/
    if (compare32bitArrays((unsigned int*)pOutBuf_16bpp, (unsigned int*)pRefBuf_16bpp,2*n) != 1){
         printf("C6accel_DSP_ifft32x32() failed\n");
         return 0;
      }

    fclose(refFd);
    printf("Tests for Invers FFT functions successful\n");
    return (1);
}


/*
 * Test for autocorrelation kernels in c6accel
 */
int c6accel_test_DSP_AUTOCOR(C6accel_Handle hC6accel,unsigned int nx ,unsigned int nr)
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

    /* Test C6accel_DSP_autocor(), Generate the 16 bit fixed version of sine */
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
    LOG_STRING("C6accel_DSP_autocor(),");
    START_BENCHMARK();

    /* Call autocorrelation function in the C6Accel */
    C6accel_DSP_autocor(hC6accel,(short*)pOutBuf_16bpp,(short*)inputArray16,nx,nr);
    END_AND_WRITE_BENCHMARK();

    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSP_autocor() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /*  read the reference output from file */
    fread(pRefBuf_16bpp,  nr, sizeof(short), refFd);
    LOG_STRING("\n");

    /* Function passed or failed the test */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp, (unsigned short*)pRefBuf_16bpp, nr) != 1){
         printf("C6accel_DSP_autocor() failed\n");
         return 0;
     }

    fclose(refFd);
    printf("Test for Auto Correlation function successful\n");
    return (1);
}

/*
 * Test for dot product kernels in c6accel
 */
int c6accel_test_DSP_DOTPROD(C6accel_Handle hC6accel,unsigned int nx)
{
    FILE  *refFd;
    short *mArray16, *nArray16;
    int i, Gout =0;

    /* Generate the 16 bit fixed version of sine */
    mArray16 = (short *)pSrcBuf_16bpp;
    nArray16 = (short *)pWorkingBuf2_16bpp;

    for (i=0;i<nx;i++){
        mArray16[i]     = m[i];
        nArray16[i]     = n[i];
    }

    if ((refFd = fopen(ref_dotprod,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_dotprod);
       return (0);
    }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00, 2*sizeof(int));
    memset (pRefBuf_16bpp,0x00, 2*sizeof(int));

    LOG_STRING("C6accel_DSP_dotprod(),");
    START_BENCHMARK();

    /* Call the dot product function in the C6Accel codec */
    C6accel_DSP_dotprod(hC6accel,(short*)mArray16,(short*)nArray16,(int *)pOutBuf_16bpp,nx);
    END_AND_WRITE_BENCHMARK();
    
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSP_dotprod() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* Read reference output from file */
    fread(pRefBuf_16bpp, 1, sizeof(int), refFd);

    LOG_STRING("\n");
    if( *(int *)pRefBuf_16bpp != *(int *)pOutBuf_16bpp ){
      printf("Dot prod failed\n ");
      return 0;
    }

    fclose(refFd);

    /* TEST C6Accel_DSP_dotp_sqr kernel */

    if ((refFd = fopen(ref_dotp_sqr,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_dotprod);
       return (0);
    }

    /* Clear output arrays*/
    memset (pOutBuf_16bpp,0x00, 2*sizeof(int));
    memset (pRefBuf_16bpp,0x00, 2*sizeof(int));

    LOG_STRING("C6accel_DSP_DSP_dotp_sqr(),");
    START_BENCHMARK();

    /* Call the DSP dotp sqr kernel in the C6Accel */
    C6accel_DSP_dotp_sqr(hC6accel,Gout,(short*)mArray16,(short*)nArray16,(int*)(pOutBuf_16bpp+sizeof(Gout)),(int *)pOutBuf_16bpp,nx);

    END_AND_WRITE_BENCHMARK();
    
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSP_dotp_sqr() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* Read reference ouput from test files */
      fread(pRefBuf_16bpp, 2, sizeof(int), refFd);

    LOG_STRING("\n");

    /* Compare output from C6Accel with reference output to determine success of the test */
    if((*(int *)pRefBuf_16bpp != *(int *)pOutBuf_16bpp) ||
       (*(int*)(pRefBuf_16bpp+sizeof(Gout))!= *(int*)(pOutBuf_16bpp+sizeof(Gout)))){
      printf("Dot product square failed\n");
      return 0;
    }

    fclose(refFd);

    printf("Tests for DoT Product functions successful\n");
    return (1);
}

/*
 * Test for matrix multiplication kernels in c6accel
 */
int c6accel_test_DSP_MATMUL(C6accel_Handle hC6accel,unsigned int r1,unsigned int c1, unsigned int c2,unsigned int s)
{
    FILE *refFd;
    short *xArray16, *yArray16;
    int i;

    /* Generate the 16 bit input */
    xArray16 = (short *)pSrcBuf_16bpp;
    yArray16 = (short *)pWorkingBuf2_16bpp;

    for (i=0;i<r1*c1;i++){
        xArray16[i]     = x[i];
    }
    for (i=0;i<c2*c1;i++){
        yArray16[i]     = y[i];
    }

    if ((refFd = fopen(ref_mat_mul,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_mat_mul);
       return (0);
       }

    /* Clear output arrays*/
    memset (pOutBuf_16bpp,0x00, r1*c2*sizeof(short));
    memset (pRefBuf_16bpp,0x00, r1*c2*sizeof(short));

    LOG_STRING("C6accel_DSP_mat_mul(),");
    START_BENCHMARK();

    /* Call matrix multiplication kernel in the C6Accel codec*/
    C6accel_DSP_mat_mul(hC6accel,xArray16,r1,c1,yArray16,c2,(short *)pOutBuf_16bpp,s);
    END_AND_WRITE_BENCHMARK();
    
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSP_mat_mul() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* Read reference input from test file*/
    fread(pRefBuf_16bpp, r1*c2, sizeof(short), refFd);

    LOG_STRING("\n");

    /* Compare the output to the reference output to determine outcome of test */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp, (unsigned short*)pRefBuf_16bpp, r1*c2) != 1){
         printf("C6accel_DSP_mat_mul() failed\n");
         return 0;
       }

     fclose(refFd);

     printf("Tests for Matrix multiplication successful\n");
     return (1);
}

/*
 * Test for FIR filter generation kernels in c6accel
 */
int c6accel_test_DSP_FIR(C6accel_Handle hC6accel,unsigned int nr,unsigned int nh)
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

    LOG_STRING("C6accel_DSP_fir_gen(),");
    START_BENCHMARK();

    /* Call the DSP_fir_gen function in C6Accel*/
    C6accel_DSP_fir_gen(hC6accel,xArray16, hArray16,(short *)pOutBuf_16bpp,nh,nr);
    END_AND_WRITE_BENCHMARK();
    
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSP_fir_gen() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    fread(pRefBuf_16bpp, nr, sizeof(short), refFd);
    LOG_STRING("\n");

    /* Validate Results by comparing output from C6accel call to the reference output */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp, (unsigned short*)pRefBuf_16bpp, nr) != 1){
         printf("C6accel_DSP_fir_gen() failed\n");
         return 0;
     }

     fclose(refFd);

    /*Test fir_r4 kernel in c6accel*/
    if ((refFd = fopen(ref_fir_r4,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_fir_r4);
       return (0);
       }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x0000, nr);
    memset (pRefBuf_16bpp,0x0000, nr);

    LOG_STRING("C6accel_DSP_fir_r4(),");
    START_BENCHMARK();

    C6accel_DSP_fir_r4(hC6accel,xArray16, hArray16,(short *)pOutBuf_16bpp,nh,nr);
    END_AND_WRITE_BENCHMARK();
    
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSP_fir_r4() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    fread(pRefBuf_16bpp, nr , sizeof(short), refFd);

    LOG_STRING("\n");
    /* Validate results from c6accel with reference output */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp, (unsigned short*)pRefBuf_16bpp, nr) != 1){
         printf("C6accel_DSP_fir_r4() failed\n");
         return 0;
       }

     fclose(refFd);

    /* Test fir_r8 kernel in c6accel */
    if ((refFd = fopen(ref_fir_r8,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_fir_r8);
       return (0);
       }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00, nr*sizeof(short));
    memset (pRefBuf_16bpp,0x00, nr*sizeof(short));

    LOG_STRING("C6accel_DSP_fir_r8(),");
    START_BENCHMARK();

    C6accel_DSP_fir_r8(hC6accel,xArray16, hArray16,(short *)pOutBuf_16bpp,nh,nr);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSP_fir_r8() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    fread(pRefBuf_16bpp, nr, sizeof(short), refFd);
    LOG_STRING("\n");

    /* Validate result from c6accel to reference output */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp, (unsigned short*)pRefBuf_16bpp, nr) != 1){
         printf("C6accel_DSP_fir_r8() failed\n");
         return 0;
       }

    fclose(refFd);

    printf("Tests for FIR Filtering functions successful\n");
    return (1);
}

/*
 * Test for IIR filter generation kernels in c6accel
 */
int c6accel_test_DSP_IIR(C6accel_Handle hC6accel,unsigned int Nin,unsigned int nh)
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

    LOG_STRING("C6accel_DSP_iir(),");
    START_BENCHMARK();

    /* Call IIR kernel in C6Accel */
    C6accel_DSP_iir(hC6accel,xArray16, hArray16,nh,b,(short *)pOutBuf_16bpp,Nin);

    END_AND_WRITE_BENCHMARK();
     
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSP_iir() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    fread(pRefBuf_16bpp, 1, Nin*sizeof(short), refFd);

    LOG_STRING("\n");

    /*Validate the result by comparing result to the reference output */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp, (unsigned short*)pRefBuf_16bpp, Nin) != 1){
         printf("C6accel_DSP_iir() failed\n");
         return 0;
       }

     fclose(refFd);

     printf("Test for IIR Filter successful\n");
     return (1);
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
        input data is first created using floating point dataType. The
        floating point data type is then converted to the appropriate
        fixed point notation
    */
    /* Clear the input floating point array */
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
 * Test all possible IMG Histogram combinations in this function
 */
int c6accel_test_IMG_histogram(C6accel_Handle hC6accel,unsigned int inputWidth, unsigned int inputHeight)
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


    LOG_STRING("C6accel_IMG_histogram_8(),");
    START_BENCHMARK();

    /* Call the histogram */
    C6accel_IMG_histogram_8(hC6accel,pSrcBuf_16bpp,inputWidth*inputHeight,1,
            (unsigned short*)pWorkingBuf_16bpp,
            (unsigned short*)pOutBuf_16bpp);

    END_AND_WRITE_BENCHMARK();
    
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_histogram_8() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* reset temp histogram*/
    memset (pWorkingBuf_16bpp,0x00,1024*sizeof(short));

    /* read in reference */
    fread(pRefBuf_16bpp,  (1<<IMG_BITS_HISTOGRAM_16), sizeof(short), refFd);

    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp, (unsigned short*)pRefBuf_16bpp,256) != 1)
         {printf("C6accel_IMG_histogram_8() failed\n");
          return 0;
          }

    fclose(refFd);

    /* Test for IMG_histogram_16() */
    /* Open reference output file for 16 bit histogram kernel */
    if ((refFd = fopen(ref_histogram_16,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_histogram_16);
       return (0);
       }

    /* Will reuse 8 bit source file and copy to a 16bpp version with a shift to make it 12 bit data */
    p16bitBuf= (unsigned short *)pWorkingBuf2_16bpp;
    for(i=0;i< inputWidth*inputHeight;i++)
         {p16bitBuf[i] = ((unsigned short)pSrcBuf_16bpp[i])<<(IMG_BITS_HISTOGRAM_16-8);
         }

    /* Clear working and output arrays */
    memset (pWorkingBuf_16bpp,0x00,4*(1<<IMG_BITS_HISTOGRAM_16)*sizeof(short));

    memset (pOutBuf_16bpp,0x00,(1<<IMG_BITS_HISTOGRAM_16)*sizeof(short));
    memset (pRefBuf_16bpp,0x00,(1<<IMG_BITS_HISTOGRAM_16)*sizeof(short));

    LOG_STRING("C6accel_IMG_histogram_16(),");
    START_BENCHMARK();
    /* Call the histogram */
    C6accel_IMG_histogram_16(hC6accel,p16bitBuf,inputWidth*inputHeight,1,
            (short*)pWorkingBuf_16bpp,(short*)pOutBuf_16bpp,IMG_BITS_HISTOGRAM_16 /*img_bits == 12bit data */);
    END_AND_WRITE_BENCHMARK();
    
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_histogram_16() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* reset temp histogram */
    memset (pWorkingBuf_16bpp,0x00,4*(1<<IMG_BITS_HISTOGRAM_16)*sizeof(short));

    /* read in reference */
    fread(pRefBuf_16bpp,  (1<<IMG_BITS_HISTOGRAM_16), sizeof(short), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit level */
     if (compare16bitArrays((unsigned short*)pOutBuf_16bpp,(unsigned short*)pRefBuf_16bpp,(1<<IMG_BITS_HISTOGRAM_16)) != 1)
         {printf("C6accel_IMG_histogram_16() failed\n");
          return 0;
          }

     fclose(srcFd);
     fclose(refFd);

     printf("Tests for Histogram functions successful\n");
     return (1);
}

/*
 * Test all possible IMG Median combinations in this function
 */
int c6accel_test_IMG_median(C6accel_Handle hC6accel,unsigned int inputWidth, unsigned int inputHeight)
{
    unsigned int                i;
#ifndef USE_NEW_FRAME_APIS
    unsigned int                row;
#endif
    FILE *srcFd, *refFd;

    short* p16sbitBufIn,*p16sbitBufOut;

    /* Test for IMG_median()*/
    /* Open source image file */
    if ((srcFd = fopen(srcFileName,"rb")) == NULL) {
       printf("Failed to open srcFd on %s\n",srcFileName);
       return (0);
       }
    /* Open reference output ref/DSP output file */
    if ((refFd = fopen(ref_median_3x3_8,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_median_3x3_8);
       return (0);
       }

    /* Read in source file */
    fread(pSrcBuf_16bpp,inputWidth*inputHeight,1,srcFd);

    /* Clear working and output arrays */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight);
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight);

    LOG_STRING("C6accel_IMG_median_3x3_8(),");
    START_BENCHMARK();

     /*  Call the median filter one line at a time
         The 3x3 array read lines n,n+1,n+2 from input image to calculate output line n.
         Therefore, the last 2 rows of the image cannot be calculated as they read past the original image.
         So only calulate on height-2 rows for valid data.   */
#ifdef USE_NEW_FRAME_APIS
    C6accel_IMG_median_3x3_8_Frame(hC6accel,pSrcBuf_16bpp,inputHeight-2,inputWidth,pOutBuf_16bpp);
#else
    for (row=0;row<(inputHeight-2);row++)
         C6accel_IMG_median_3x3_8(hC6accel,pSrcBuf_16bpp+row*inputWidth,inputWidth,pOutBuf_16bpp+row*inputWidth);
#endif
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_median_3x3_8() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  inputWidth*(inputHeight-2), sizeof(char), refFd);
    LOG_STRING("\n");


    /* Validate output between native DSP/ref and C6Accel on 8 bit char level */
    if (compare8bitArrays(pOutBuf_16bpp, pRefBuf_16bpp,inputWidth*(inputHeight-2)) != 1)
         {printf("C6accel_IMG_median_3x3_8() failed\n");
          return 0;
          }

     fclose(refFd);

    /* test IMG_median_3x3_16() in c6accel */

    /*Open reference output for 16 bit median kernel */
    if ((refFd = fopen(ref_median_3x3_16,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_median_3x3_16);
       return (0);
       }

    /* Will reuse 8 bit source file and copy to a 16bpp version with a shift to make it 12 bit data */
    p16sbitBufIn= (short *)pWorkingBuf_16bpp;
    p16sbitBufOut= (short *)pOutBuf_16bpp;

    for(i=0;i< inputWidth*inputHeight;i++){
       p16sbitBufIn[i] = ((unsigned short)pSrcBuf_16bpp[i])<<4;
    }

    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));

    LOG_STRING("C6accel_IMG_median_3x3_16(),");
    START_BENCHMARK();

     /*  Call the median filter one line at a time
         The 3x3 array read lines n,n+1,n+2 from input image to calculate output line n.
         Therefore, the last 2 rows of the image cannot be calculated as they read past the original image.
         So only calulate on height-2 rows for valid data.   */
 #ifdef USE_NEW_FRAME_APIS
       C6accel_IMG_median_3x3_16_Frame(hC6accel,p16sbitBufIn,inputHeight-2,inputWidth,p16sbitBufOut);

 #else
    for (row=0;row<inputHeight-2;row++)
       C6accel_IMG_median_3x3_16(hC6accel,p16sbitBufIn+row*inputWidth,inputWidth,p16sbitBufOut+row*inputWidth);
 #endif
    END_AND_WRITE_BENCHMARK();
    
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_median_3x3_16() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference */
    fread(pRefBuf_16bpp, inputWidth*(inputHeight-2), sizeof(short), refFd);
    LOG_STRING("\n");


    /* Validate output between native DSP/ref and C6Accel on 16 bit char level */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp, (unsigned short*)pRefBuf_16bpp,inputWidth*(inputHeight-2)) != 1)
         {printf("C6accel_IMG_median_16() failed\n");
          return 0;
          }

    fclose(srcFd);
    fclose(refFd);

    /* Test for IMG_median_3x3_16() in c6accel */
    /* Read reference output for 16 bit signed median kernel in c6accel */
    if ((refFd = fopen(ref_median_3x3_16s,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_median_3x3_16s);
       return (0);
       }

    /* Will reuse 8 bit source file and copy to a 16bpp version with a shift to make it 12 bit data*/
    p16sbitBufIn= (short *)pWorkingBuf_16bpp;
    p16sbitBufOut= (short *)pOutBuf_16bpp;

    for(i=0;i< inputWidth*inputHeight;i++){
       p16sbitBufIn[i] = ((short)pSrcBuf_16bpp[i])<<4;
    }

    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));

    LOG_STRING("C6accel_IMG_median_3x3_16s(),");
    START_BENCHMARK();

      /* Call the median filter one line at a time
         The 3x3 array read lines n,n+1,n+2 from input image to calculate output line n.
         Therefore, the last 2 rows of the image cannot be calculated as they read past the original image.
         So only calulate on height-2 rows for valid data.   */
#ifdef USE_NEW_FRAME_APIS
         C6accel_IMG_median_3x3_16s_Frame(hC6accel,p16sbitBufIn,inputHeight-2,inputWidth,p16sbitBufOut);

#else
    for (row=0;row<inputHeight-2;row++)
         C6accel_IMG_median_3x3_16s(hC6accel,p16sbitBufIn+row*inputWidth,inputWidth,p16sbitBufOut+row*inputWidth);
#endif
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_median_3x3_16s() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp, inputWidth*(inputHeight-2), sizeof(short), refFd);

    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit char level */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp, (unsigned short*)pRefBuf_16bpp,inputWidth*(inputHeight-2)) != 1)
         {printf("C6accel_IMG_median_16s() failed\n");
          return 0;
          }

     fclose(refFd);

     printf("Test for Median Filtering functions successful\n");
     return (1);
}



/*
 * Test all possible IMG CONV combinations in this function
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

int c6accel_test_IMG_conv(C6accel_Handle hC6accel,unsigned int inputWidth, unsigned int inputHeight)
{
    unsigned int   row,i;
    FILE           *srcFd, *refFd;
    short          *p16bitInBuf;
    short          *p16bitOutBuf;
    /* Pointer to hold a CMem for the mask*/
    short          *p16GaussianCmem;

    /* Allocate CMEM memory for 3x3 short mask*/
    p16GaussianCmem = Memory_alloc(18, &testfxnsMemParams);

    /* Test IMG_sobel_3x3_8() */
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
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight);

    /* Set Up required Gaussian */
    memcpy( p16GaussianCmem,gaussian_3x3_8s_1Q7,9);

    LOG_STRING("C6accel_IMG_conv_3x3_i8_c8s(),");
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
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_conv_3x3_i8_c8s() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference */
    fread(pRefBuf_16bpp, inputWidth*(inputHeight-2), sizeof(unsigned char), refFd);

    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 8 bit char level
     As undefined border of 2 pixel exists on right hand side and bottom
    */
    for(row=0;row<inputHeight-2;row++){
       if (compare8bitArrays(pOutBuf_16bpp+row*inputWidth, pRefBuf_16bpp+row*inputWidth,inputWidth-2) != 1){
          printf("C6accel_IMG_conv_3x3_i8_c8s() failed on row %d\n",row);
          return 0;
        }
     }

    fclose(refFd);

    /*  IMG_conv_3x3_i16s_c16s()*/
    if ((refFd = fopen(ref_conv_3x3_16s,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_conv_3x3_16s);
       return (0);
       }

    /*  Will reuse 8 bit source file and copy to a 16bpp version with a shift to make it 12 bit data*/
      p16bitInBuf= (short *)pWorkingBuf_16bpp;
      for(i=0;i< inputWidth*inputHeight;i++){
          p16bitInBuf[i] = ((unsigned short)pSrcBuf_16bpp[i])<<4;
       }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));

    /*  Set Up required Gaussian */
    memcpy( p16GaussianCmem,gaussian_3x3_16s_1Q15,18);

    LOG_STRING("C6accel_IMG_conv_3x3_i16s_c16s(),");
    START_BENCHMARK();
    /* Call the conv codec, the 16 bit version, width must be <65536 and so just do single lines here
       More optimal would be to do say 80 lines at a time (80*640 = 51200)
       The 3x3 array read lines n,n+1,n+2 from input image to calculate output line n.
       Therefore, the last 2 rows of the image cannot be calculated as they read past the original image.
       So only calulate on height-2 rows for valid data.   */

    p16bitOutBuf = (short*)pOutBuf_16bpp;
#ifdef USE_NEW_FRAME_APIS
    C6accel_IMG_conv_3x3_i16s_c16s_Frame(hC6accel,p16bitInBuf,
                                       p16bitOutBuf,
                                       inputHeight-2,
                                       inputWidth, inputWidth,
                                       p16GaussianCmem, 15);
#else
    /* Call the conv codec, the 16 bit version, width must be <65536 and so just do single lines here
       More optimal would be to do say 80 lines at a time (80*640 = 51200)                             */
    for(row=0;row<(inputHeight-2);row++)
        C6accel_IMG_conv_3x3_i16s_c16s(hC6accel,p16bitInBuf+(row*inputWidth),
                                       p16bitOutBuf+(row*inputWidth),
                                       inputWidth, inputWidth,
                                       p16GaussianCmem, 15);
#endif
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_conv_3x3_i16s_c16s() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp, inputWidth*(inputHeight-2), sizeof(unsigned short), refFd);

    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 8 bit char level
     As undefined border of 2 pixel exists on right hand side and bottom
    */
    for(row=0;row<inputHeight-2;row++){
      unsigned short *p16Out;
      unsigned short *p16Ref;

      p16Out = (unsigned short*)pOutBuf_16bpp;
      p16Ref = (unsigned short*)pRefBuf_16bpp;

      if (compare16bitArrays(p16Out+row*inputWidth, p16Ref+row*inputWidth,inputWidth-2) != 1) {
        printf("C6accel_IMG_conv_3x3_i16s_c16s() failed on row %d\n",row);
        return 0;
       }
    }

     fclose(srcFd);
     fclose(refFd);

     /*  Release Gaussian CMEM */
     Memory_free(p16GaussianCmem,18,&testfxnsMemParams);

     printf("Tests for Convolution functions successful\n");
     return (1);
}


int c6accel_test_IMG_corr(C6accel_Handle hC6accel,unsigned int inputWidth, unsigned int inputHeight)
{
    unsigned int   i,row;
    FILE           *srcFd, *refFd;
    short          *p16bitBufIn;
    int            *p32bitBufOut;

    /* Pointer to hold a CMem for the mask*/
    short                  *p16GaussianCmem;

    /* Allocate CMEM heap memory for 3x3 short mask*/
     p16GaussianCmem = Memory_alloc(18,&testfxnsMemParams);

    /* Test IMG_corr_3x3_8() kernel */
    /* Open Source Image file */
    if ((srcFd = fopen(srcFileName,"rb")) == NULL) {
       printf("Failed to open srcFd on %s\n",srcFileName);
       return (0);
     }
    if ((refFd = fopen(ref_corr_3x3_8,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_corr_3x3_8);
       return (0);
     }

    /* Read in source file */
    fread(pSrcBuf_16bpp,inputWidth*inputHeight,1,srcFd);

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(unsigned int));
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(unsigned int));
    /* Set Up required Gaussian */
    memcpy( p16GaussianCmem,gaussian_3x3_8_1Q7,9);

    LOG_STRING("C6accel_IMG_corr_3x3_i8_c8(),");
    START_BENCHMARK();
    /* The 3x3 array read lines n,n+1,n+2 from input image to calculate output line n.
       Therefore, the last 2 rows of the image cannot be calculated as they read past the original image.
       So only calulate on height-2 rows for valid data.   */
    p32bitBufOut=(int*)pOutBuf_16bpp;
#ifdef USE_NEW_FRAME_APIS
       C6accel_IMG_corr_3x3_i8_c8_Frame(hC6accel,pSrcBuf_16bpp,
                                  p32bitBufOut,
                                  inputHeight-2,inputWidth,inputWidth, (unsigned char*)p16GaussianCmem,7,0);
#else
    /* Call the corr codec, width (ie total number of o/p pixels) must be <65536 and so just do single lines here
       More optimal would be to do say 80 lines at a time (80*640 = 51200)
     */
    for(row=0;row<inputHeight-2;row++)
       C6accel_IMG_corr_3x3_i8_c8(hC6accel,pSrcBuf_16bpp+row*inputWidth,
                                  p32bitBufOut+row*inputWidth,
                                  inputWidth,inputWidth, (unsigned char*)p16GaussianCmem,7,0);
#endif
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_corr_3x3_i8_c8() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp, inputWidth*(inputHeight-2), sizeof(unsigned int), refFd);

    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 32 bit int level
       As undefined border of 2 pixel exists on right hand side and bottom
    */
    for(row=0;row<inputHeight-2;row++){
       unsigned int *p32Out;
       unsigned int *p32Ref;

       p32Out = (unsigned int*)pOutBuf_16bpp;
       p32Ref = (unsigned int*)pRefBuf_16bpp;

      if (compare32bitArrays(p32Out+row*inputWidth, p32Ref+row*inputWidth, inputWidth-2) != 1){
         printf("C6accel_IMG_corr_3x3_i8_c8s() failed on row %d\n",row);
         return 0;
       }
    }
    fclose(refFd);

    /* Test IMG_corr_3x3_i16s_c16s() in C6accel*/
    if ((refFd = fopen(ref_corr_3x3_16s,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_corr_3x3_16s);
       return (0);
     }

    /* Will reuse 8 bit source file and copy to a 16bpp version with a shift to make it 12 bit data */
      p16bitBufIn= (short *)pWorkingBuf_16bpp;
      for(i=0;i< inputWidth*inputHeight;i++){
         p16bitBufIn[i] = ((short)pSrcBuf_16bpp[i])<<4;
       }

    /* Clear output arrays*/
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(int));
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(int));
    /* Set Up required Gaussian */
    memcpy( p16GaussianCmem,gaussian_3x3_16s_1Q15,18);

    LOG_STRING("C6accel_IMG_corr_3x3_i16s_c16s(),");
    START_BENCHMARK();

    /* Call the corr codec, width (ie total number of o/p pixels) must be <65536 and so just do single lines here
       More optimal would be to do say 80 lines at a time (80*640 = 51200)
       The 3x3 array read lines n,n+1,n+2 from input image to calculate output line n.
       Therefore, the last 2 rows of the image cannot be calculated as they read past the original image.
       So only calulate on height-2 rows for valid data.   */
    p32bitBufOut=(int *)pOutBuf_16bpp;
#ifdef USE_NEW_FRAME_APIS
       C6accel_IMG_corr_3x3_i16s_c16s_Frame(hC6accel,p16bitBufIn,
                                      p32bitBufOut,
                                      inputHeight-2,inputWidth, inputWidth,
                                      p16GaussianCmem, 15,0);
#else
    /* Call the corr codec, width (ie total number of o/p pixels) must be <65536 and so just do single lines here
       More optimal would be to do say 80 lines at a time (80*640 = 51200)                                        */
    for(row=0;row<inputHeight-2;row++)
       C6accel_IMG_corr_3x3_i16s_c16s(hC6accel,p16bitBufIn+row*inputWidth,
                                      p32bitBufOut+row*inputWidth,
                                      inputWidth, inputWidth,
                                      p16GaussianCmem, 15,0);
#endif
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_corr_3x3_i16s_c16s() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference */
    fread(pRefBuf_16bpp, inputWidth*(inputHeight-2), sizeof(int), refFd);
    LOG_STRING("\n");


    /* Validate output between native DSP/ref and C6Accel on 32 bit int level
     As undefined border of 2 pixel exists on right hand side and bottom
     */
    for(row=0;row<inputHeight-2;row++){
       unsigned int *p32Out;
       unsigned int *p32Ref;

       p32Out = (unsigned int*)pOutBuf_16bpp;
       p32Ref = (unsigned int*)pRefBuf_16bpp;

      if (compare32bitArrays(p32Out+row*inputWidth, p32Ref+row*inputWidth, inputWidth-2) != 1){
         printf("C6accel_IMG_corr_3x3_16s() failed on row %d\n",row);
         return 0;
       }
    }

     fclose(srcFd);
     fclose(refFd);
     // Release Gaussian CMEM
     Memory_free(p16GaussianCmem,18,&testfxnsMemParams);

     printf("Tests for Correlation functions successful\n");
     return (1);
}

/*
 * Test all possible IMG Sobel combinations in this function
 */
int c6accel_test_IMG_sobel(C6accel_Handle hC6accel,unsigned int inputWidth, unsigned int inputHeight)
{
    unsigned int                i,row;
    FILE *srcFd, *refFd;
    unsigned short *p16bitBuf;

    /* Test IMG_sobel_3x3_8() in c6accel */
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
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight);

    LOG_STRING("C6accel_IMG_sobel_3x3_8(),");
    START_BENCHMARK();

    /* Call the sobel codec to perform edge detection */
    /* Sobel function handles the adjustment 1 line border with undefined pixels internally */
    C6accel_IMG_sobel_3x3_8(hC6accel,pSrcBuf_16bpp,pOutBuf_16bpp,inputWidth, inputHeight);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_sobel_3x3_8() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(unsigned char), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 8 bit char level
       As undefined border of 1 pixel exists must test on a line by line basis
     */
    for(row=0;row<inputHeight-2;row++) {
       if (compare8bitArrays(pOutBuf_16bpp+row*inputWidth+1, pRefBuf_16bpp+row*inputWidth+1,inputWidth-2) != 1){
         printf("C6accel_IMG_sobel_3x3_8() failed on row %d\n",row);
         return 0;
        }
     }

     fclose(refFd);

    /* Test IMG_sobel_3x3_16() in c6accel */
    if ((refFd = fopen(ref_sobel_3x3_16,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_sobel_3x3_16);
       return (0);
       }

    /* Will reuse 8 bit source file and copy to a 16bpp version with a shift to make it 12 bit data*/
    p16bitBuf= (unsigned short *)pWorkingBuf_16bpp;
    for(i=0;i< inputWidth*inputHeight;i++)
         {p16bitBuf[i] = ((unsigned short)pSrcBuf_16bpp[i])<<4;
         }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));

    LOG_STRING("C6accel_IMG_sobel_3x3_16(),");
    START_BENCHMARK();
    /* Call the sobel codec to perform edge detection */
    /* Sobel function handles the adjustment 1 line border with undefined pixels internally */
    C6accel_IMG_sobel_3x3_16(hC6accel,(unsigned short*)pWorkingBuf_16bpp,(unsigned short*)pOutBuf_16bpp,inputWidth, inputHeight);

    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_sobel_3x3_16() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    
    /* read in reference */
    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(unsigned short), refFd);

    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level
     As undefined border of 1 pixel exists must test on a line by line basis
     */
    for(row=1;row<inputHeight-1;row++){
       unsigned short* p16bitOut, *p16bitRef;
       p16bitOut = (unsigned short*)pOutBuf_16bpp;
       p16bitRef = (unsigned short*)pRefBuf_16bpp;

       if (compare16bitArrays(p16bitOut+row*inputWidth+1, p16bitRef+row*inputWidth+1,inputWidth-2) != 1){
         printf("C6accel_IMG_sobel_3x3_16() failed on row %d\n",row);
         return 0;
        }
     }

     fclose(srcFd);
     fclose(refFd);

     printf("Tests for Sobel Filter successful\n");
     return (1);
}

/*
 * Test all possible IMG muls kernels in c6accel
 */
int c6accel_test_IMG_muls(C6accel_Handle hC6accel,unsigned int inputWidth, unsigned int inputHeight)
{
    unsigned int                i;
    FILE *srcFd, *refFd;
    short *p16bitBuf;

    /* Test IMG_muls_8() in C6accel*/
    if ((srcFd = fopen(srcFileName,"rb")) == NULL) {
       printf("Failed to open srcFd on %s\n",srcFileName);
       return (0);
     }
    if ((refFd = fopen(ref_muls_8,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_muls_8);
       return (0);
     }

    /* Read in source file */
    fread(pSrcBuf_16bpp,inputWidth*inputHeight,1,srcFd);

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight);
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight);

    LOG_STRING("C6accel_IMG_muls_8(),");
    START_BENCHMARK();
    /* Call the mulS_8 to multiply by 2 */
    C6accel_IMG_mulS_8(hC6accel,pSrcBuf_16bpp,(short*)pOutBuf_16bpp,2,inputWidth*inputHeight);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_mulS_8() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference */
    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(short), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 8 bit char level*/
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp, (unsigned short*)pRefBuf_16bpp,inputWidth*inputHeight) != 1){
      printf("C6accel_IMG_muls_8() failed\n");
      return 0;
    }
    fclose(refFd);

    /*Test the IMG_muls_16() in c6accel*/
    if ((refFd = fopen(ref_muls_16,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_muls_16);
       return (0);
     }

    /* Will reuse 8 bit source file and copy to a 16bpp version with a shift to make it 12 bit data */
    p16bitBuf= (short *)pWorkingBuf_16bpp;
    for(i=0;i< inputWidth*inputHeight;i++)
         {p16bitBuf[i] = ((short)pSrcBuf_16bpp[i])<<4;
         }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(int));
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(int));

    LOG_STRING("C6accel_IMG_muls_16(),");
    START_BENCHMARK();

    /* Call the muls to multiply by 2 */
    C6accel_IMG_mulS_16s(hC6accel,p16bitBuf,(int*)pOutBuf_16bpp,2,inputWidth*inputHeight);

    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_mulS_16s() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(int), refFd);

    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int*)pOutBuf_16bpp,(unsigned int*) pRefBuf_16bpp,inputWidth*inputHeight) != 1){
       printf("C6accel_IMG_muls_16() failed\n");
       return 0;
     }

     fclose(srcFd);
     fclose(refFd);

     printf("Test for Image multiplication functions successful\n");
     return (1);
}

/*
 * Test all possible IMG adds combinations in this function
 */
int c6accel_test_IMG_adds(C6accel_Handle hC6accel,unsigned int inputWidth, unsigned int inputHeight)
{
    unsigned int                i;
    FILE *srcFd, *refFd;
    short *p16bitBuf,*p16bitInBuf;

    /* Test IMG_adds_8() in c6accel */
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
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight);

    /* Inplace Adding a constant to an image Source = OutBuf
      Read in source file to output buffer
     */
    fread(pOutBuf_16bpp,inputWidth*inputHeight,1,srcFd);

    /* Read in source file */
    fread(pSrcBuf_16bpp,inputWidth*inputHeight,1,srcFd);

    LOG_STRING("C6accel_IMG_adds_8(),");
    START_BENCHMARK();

    /* Call the addS_8 to add 1 */
    C6accel_IMG_addS_8(hC6accel,(char*)pOutBuf_16bpp,(char*)pOutBuf_16bpp,1,inputWidth*inputHeight);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_addS_8() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /*  read in reference output */
    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(char), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 8 bit char level */
    if (compare8bitArrays(pOutBuf_16bpp, pRefBuf_16bpp,inputWidth*inputHeight) != 1){

      printf("C6accel_IMG_adds_8() failed\n");
      return 0;
    }
    fclose(refFd);

    /* Test IMG_adds_16() in c6accel */
    /* Open reference ouput file */
    if ((refFd = fopen(ref_adds_16,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_adds_16);
       return (0);
    }

    /* Clear output arrays*/
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));

    /* Will reuse 8 bit source file and copy to a 16bpp version with a shift to make it 12 bit data*/
      p16bitBuf= (short *)pWorkingBuf_16bpp;
    /*Enable inplace processing */
    p16bitInBuf= (short *)pOutBuf_16bpp;
    for(i=0;i< inputWidth*inputHeight;i++){
       p16bitBuf[i] = ((short)pSrcBuf_16bpp[i])<<4;
       p16bitInBuf[i] = p16bitBuf[i];
    }

    LOG_STRING("C6accel_IMG_adds_16(),");
    START_BENCHMARK();
    /* Call the adds_16 to add 1*/
    C6accel_IMG_addS_16s(hC6accel,p16bitInBuf,(short*)pOutBuf_16bpp,1,inputWidth*inputHeight);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_addS_16s() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference file */
    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(short), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp,(unsigned short*) pRefBuf_16bpp,inputWidth*inputHeight) != 1){
      printf("C6accel_IMG_adds_16() failed\n");
      return 0;
    }

    fclose(srcFd);
    fclose(refFd);

    printf("Tests for Image addition functions successful\n");
    return (1);
}

/*
 * Test all possible IMG subs combinations in this function
 */
int c6accel_test_IMG_subs(C6accel_Handle hC6accel,unsigned int inputWidth, unsigned int inputHeight)
{
    unsigned int                i;
    FILE *srcFd, *refFd;
    short *p16bitBuf,*p16bitInBuf;

    /* Test IMG_subs_8() in c6accel */
    /*Open source image file */
    if ((srcFd = fopen(srcFileName,"rb")) == NULL) {
       printf("Failed to open srcFd on %s\n",srcFileName);
       return (0);
     }
    if ((refFd = fopen(ref_subs_8,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_subs_8);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight);
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight);

    /* Inplace Adding a constant to an image Source = OutBuf
       Read in source file to output buffer
     */
    fread(pOutBuf_16bpp,inputWidth*inputHeight,1,srcFd);

    /* Read in source file */
    fread(pSrcBuf_16bpp,inputWidth*inputHeight,1,srcFd);

    LOG_STRING("C6accel_IMG_subs_8(),");
    START_BENCHMARK();
    /* Call the subS_8 to sub 1 */
    C6accel_IMG_subS_8(hC6accel,(char*)pOutBuf_16bpp,(char*)pOutBuf_16bpp,1,inputWidth*inputHeight);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_subS_8() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference output file */
    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(short), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 8 bit char level */
    if (compare8bitArrays(pOutBuf_16bpp, pRefBuf_16bpp,inputWidth*inputHeight) != 1){
       printf("C6accel_IMG_subs_8() failed\n");
       return 0;
     }

    fclose(refFd);

    /* Test IMG_subs_16() in c6accel */
    if ((refFd = fopen(ref_subs_16,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_subs_16);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));

    /* Will reuse 8 bit source file and copy to a 16bpp version with a shift to make it 12 bit data */
    p16bitBuf= (short *)pWorkingBuf_16bpp;

    /*Enable inplace processing*/
    p16bitInBuf= (short *)pOutBuf_16bpp;
    for(i=0;i< inputWidth*inputHeight;i++){
       p16bitBuf[i] = ((short)pSrcBuf_16bpp[i])<<4;
       p16bitInBuf[i] = p16bitBuf[i];
     }

    LOG_STRING("C6accel_IMG_subs_16s(),");
    START_BENCHMARK();
    /* Call the subs_16 to add 1*/
    C6accel_IMG_subS_16s(hC6accel,p16bitInBuf,(short*)pOutBuf_16bpp,1,inputWidth*inputHeight);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_subS_16s() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference ouput from file */
    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(short), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp,(unsigned short*) pRefBuf_16bpp,inputWidth*inputHeight) != 1){
       printf("C6accel_IMG_subs_16() failed\n");
       return 0;
    }

    fclose(srcFd);
    fclose(refFd);

    printf("Tests for Image substraction functions successful\n");
    return (1);
}

/*
 * Test all possible IMG yuv demux + PL420 to RGB combinations in this function
 */

const short yuv2rgb_coeff[] = { 0x2000, 0x2BDD, -0x0AC5, -0x1658, 0x3770 };

int c6accel_test_IMG_YC_demux(C6accel_Handle hC6accel,unsigned int inputWidth, unsigned int inputHeight)
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

    /* Test IMG_YC_demux() kenrel in c6accel */
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
    LOG_STRING("C6accel_IMG_yc_demux_le8_c(),");

    pY = pOutBuf_16bpp;
    pCb = pY + inputWidth*inputHeight;
    pCr = pCb + (inputWidth*inputHeight)/2;
    START_BENCHMARK();
    /* Call the yc_demux_be8 to generate 422*/
    C6accel_IMG_yc_demux_le8_8(hC6accel,inputWidth*inputHeight,(unsigned char*)pSrcBuf_16bpp,pY, pCr, pCb);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_yc_demux_le8_8() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference output */
    fread(pRefBuf_16bpp, inputWidth*inputHeight*2, sizeof(char), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 8 bit char level */
    if (compare8bitArrays(pOutBuf_16bpp, pRefBuf_16bpp,inputWidth*inputHeight*2) != 1){
      printf("C6accel_IMG_yc_demux_le8_c() failed\n");
      return 0;
    }

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

    LOG_STRING("C6accel_IMG_yuv422pl_to_rgb565(),");
    START_BENCHMARK();

    /* Call the _ycbcr422pl_to_rgb565 as we have 422P data in memory */
    C6accel_IMG_ycbcr422pl_to_rgb565(hC6accel,pYUV2RGB_Cmem,pY, pCb, pCr,(unsigned short*)pOutBuf_16bpp,inputHeight*inputWidth);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_ycbcr422pl_to_rgb565() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference output */
    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(short), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp,(unsigned short*) pRefBuf_16bpp,inputWidth*inputHeight) != 1){
      printf("C6accel_IMG_yuv422pl_to_rgb565() failed\n");
      return 0;
    }

    fclose(refFd);

    /*Test for 420pltoRGB565 kernel in c6accel */
    /* Open reference output file */
    if ((refFd = fopen(ref_yuv420_rgb_565,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_rgb_565);
       return (0);
     }

     /* Now we have YUV422 planar in memory will use for 422 to RGB function
       Convert the 422pl inputs from previous test to 420pl data
       by subsampling the chrominance data horizontally by 2
     */

     /*Do in place subsampling of Cb and Cr */
     for (i=0,j=0;i<inputWidth*inputHeight/4;i++,j+=2){
         *(pCb+i)= *(pCb+j);
         *(pCr+i)= *(pCr+j);
        }

     /* Clear output arrays for 16 bit rgb */
     memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));
     memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));

     LOG_STRING("C6accel_IMG_yuv420pl_to_rgb565(),");
     START_BENCHMARK();

     /* Call the _yun420pl_to_rgb565 as we have 420P data in memory */
     C6accel_IMG_yuv420pl_to_rgb565(hC6accel,pYUV2RGB_Cmem,inputHeight,inputWidth,pY, pCb, pCr,(unsigned short*)pOutBuf_16bpp);
     END_AND_WRITE_BENCHMARK();
     if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_yuv420pl_to_rgb565 wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference */
    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(short), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp,(unsigned short*) pRefBuf_16bpp,inputWidth*inputHeight) != 1){
      printf("C6accel_IMG_yuv420pl_to_rgb565() failed\n");
      return 0;
    }

    fclose(refFd);

    /* Test IMG_yc_demux_le16_16() kernel in c6accel */
    if ((refFd = fopen(ref_yc_demux_16,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_yc_demux_16);
       return (0);
    }

    /* Will reuse 8 bit 422planar source file and copy to a 16bpp YUYV version*/
    p16bitBuf= (short *)pWorkingBuf_16bpp;
    for(i=0;i< inputWidth*inputHeight*2;i++){
       p16bitBuf[i] = ((short)pSrcBuf_16bpp[i]);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*2*sizeof(short));
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*2*sizeof(short));

    LOG_STRING("C6accel_IMG_yc_demux_le16_16(),");
    psY = (short*)pOutBuf_16bpp;
    psCb = psY + inputWidth*inputHeight;
    psCr = psCb + (inputWidth*inputHeight)/2;
    START_BENCHMARK();

    /* Call the yc_demux_le16_16 */
    C6accel_IMG_yc_demux_le16_16(hC6accel,inputWidth*inputHeight,(unsigned short*)p16bitBuf,psY, psCr, psCb);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_yc_demux_le16_16() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference output */
    fread(pRefBuf_16bpp, inputWidth*inputHeight*2, sizeof(short), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp,(unsigned short*) pRefBuf_16bpp,inputWidth*inputHeight*2) != 1){
       printf("C6accel_IMG_yc_demux_le16_16() failed\n");
       return 0;
     }

     fclose(srcFd);
     fclose(refFd);

     /* Now we have YUV422 16 bit planar in memory will use for 422 to RGB function */
    if ((refFd = fopen(ref_rgb_565_yuv16,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_rgb_565_yuv16);
       return (0);
     }

    /* Pick up source buffer from reference buffer of previous step*/
    memcpy(pSrcBuf_16bpp,pRefBuf_16bpp,inputWidth*inputHeight*sizeof(short)*2);

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));

    psY = (short*)pSrcBuf_16bpp;
    psCb = psY + inputWidth*inputHeight;
    psCr = psCb + (inputWidth*inputHeight)/2;

    LOG_STRING("C6accel_IMG_yuv422pl16_to_rgb565(),");
    START_BENCHMARK();

    /* Call the _ycbcr422pl_to_rgb565 as we have 422P data in memory */
    C6accel_IMG_ycbcr422pl16_to_rgb565(hC6accel,pYUV2RGB_Cmem,psY, psCb, psCr,(unsigned short*)pOutBuf_16bpp,inputHeight*inputWidth);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_yuv422pl16_to_rgb565() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference output from file */

    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(short), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp,(unsigned short*) pRefBuf_16bpp,inputWidth*inputHeight) != 1){
      printf("C6accel_IMG_yuv420pl16_to_rgb565() failed\n");
      return 0;
    }
    fclose(refFd);

    if ((refFd = fopen(ref_yuv420pl16_rgb_565,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_yuv420pl16_rgb_565);
       return (0);
     }

    /* Now we have YUV422 planar in memory will use for 422 to RGB function
    Convert the 422pl inputs from previous test to 420pl data
    by subsampling the chrominance data horizontally by 2
    */
    /* Do in place subsampling of Cb and Cr */
    for (i=0,j=0;i<inputWidth*inputHeight/4;i++,j+=2){
         *(psCb+i)= *(psCb+j);
         *(psCr+i)= *(psCr+j);
     }

    /* Clear output arrays for 16 bit rgb */
    memset (pOutBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));
    memset (pRefBuf_16bpp,0x00,inputWidth*inputHeight*sizeof(short));

    LOG_STRING("C6accel_IMG_yuv420pl16_to_rgb565(),");
    START_BENCHMARK();

    /* Call the _yun420pl_to_rgb565 as we have 420P data in memory */
    C6accel_IMG_yuv420pl16_to_rgb565(hC6accel,pYUV2RGB_Cmem,inputHeight,inputWidth,psY, psCb, psCr,(unsigned short*)pOutBuf_16bpp);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_yuv420pl16_to_rgb565() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference output*/
    fread(pRefBuf_16bpp, inputWidth*inputHeight, sizeof(short), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level */
    if (compare16bitArrays((unsigned short*)pOutBuf_16bpp,(unsigned short*) pRefBuf_16bpp,inputWidth*inputHeight) != 1){
      printf("C6accel_IMG_yuv420pl16_to_rgb565() failed\n");
      return 0;
     }
    fclose(refFd);

    /* Free YUV2RGB CMEM array*/
    Memory_free(pYUV2RGB_Cmem,sizeof(short)*5,&testfxnsMemParams);
    printf("Tests for Color Space Conversion functions successful\n");
    return (1);
}

int c6accel_test_IMG_YUV422PLtoYUV422SP(C6accel_Handle hC6accel,unsigned int num_lines, 
    unsigned int width,
    unsigned int dst_pitch, 
    unsigned int src_y_pitch,
    unsigned int src_cbcr_pitch)
{
    unsigned int                i,j;
    FILE *srcFd, *refFd;
    short *p16bitBuf;
    unsigned char *y_dst, *cbcr_dst;
    unsigned char *input_y, *input_cb, *input_cr;
    int status;

    
    /* Create a CMem heap array that can be translated for DSP */
    unsigned char *pCbCr_Cmem;
   
    pCbCr_Cmem = Memory_alloc(num_lines*dst_pitch*sizeof(unsigned char),&testfxnsMemParams);
       
    input_y = (unsigned char *)pSrcBuf_16bpp;
    input_cb = (unsigned char *)pWorkingBuf_16bpp;
    input_cr = (unsigned char *)pWorkingBuf2_16bpp;
    y_dst=(unsigned char *)pRefBuf_16bpp;
    /* Test IMG_YUV422PLtoYUV422SP() kenrel in c6accel */
    if ((refFd = fopen(ref_input_y_422pl,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_input_y_422pl);
        
     }
    fread(input_y, num_lines*src_y_pitch, sizeof(unsigned char), refFd);
    fclose(refFd);

    if ((refFd = fopen(ref_input_cb_422pl,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_input_cb_422pl);
         
      }
     fread(input_cb, num_lines*src_cbcr_pitch, sizeof(unsigned char), refFd);
     fclose(refFd);

     if ((refFd = fopen(ref_input_cr_422pl,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_input_cr_422pl);
         
      }
      fread(input_cr, num_lines*src_cbcr_pitch, sizeof(unsigned char), refFd);
      fclose(refFd);

    memset (pOutBuf_16bpp,0x00,num_lines*dst_pitch*sizeof(unsigned char));
    memset (pRefBuf_16bpp,0x00,num_lines*dst_pitch*sizeof(unsigned char));
    LOG_STRING("C6accel_IMG_ycbcr422pl_to_ycbcr422sp(),");
    y_dst = (unsigned char *)pOutBuf_16bpp;
    cbcr_dst = (unsigned char *)pCbCr_Cmem;
    START_BENCHMARK();
    
    /* Call the ycbcr422pl_to_ycbcr422sp*/
    status = C6accel_IMG_ycbcr422pl_to_ycbcr422sp( hC6accel,y_dst, cbcr_dst,input_y,input_cb,input_cr,num_lines,width,dst_pitch,src_y_pitch,src_cbcr_pitch);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_ycbcr422pl_to_ycbcr422sp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    if ((refFd = fopen(ref_dst_cbcr_422sp,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_dst_cbcr_422sp);
         
     }
    /* read in reference output */
    fread(pRefBuf_16bpp, num_lines*dst_pitch, sizeof(unsigned char), refFd);
    LOG_STRING("\n");
    
    /* Validate output between native DSP/ref and C6Accel on 8 bit char level */
    if (compare8bitArrays(pCbCr_Cmem, pRefBuf_16bpp,num_lines*dst_pitch) != 1){
      printf("C6accel_IMG_ycbcr422pl_to_ycbcr422sp() failed\n");
      return 0;
    }
    printf("C6accel_IMG_ycbcr422pl_to_ycbcr422sp() successful\n");
    fclose(refFd);
    /* Free YUV2RGB CMEM array*/
    Memory_free(pCbCr_Cmem,sizeof(unsigned char)*num_lines*dst_pitch,&testfxnsMemParams);
    return 1;
}

int c6accel_test_IMG_YUV422SPtoYUV422ILE(C6accel_Handle hC6accel,unsigned int num_lines, 
    unsigned int width,
    unsigned int src_pitch, 
    unsigned int dst_ycbcr_pitch
)
{
    unsigned int                i,j;
    FILE *srcFd, *refFd;
    short *p16bitBuf;
    unsigned char *ycbcr_dst;
    unsigned char *src_y, *src_cbcr;
    
    src_y = (unsigned char *)pSrcBuf_16bpp;
    src_cbcr = (unsigned char *)pWorkingBuf_16bpp;
    ycbcr_dst=(unsigned char *)pRefBuf_16bpp;
    /* Test IMG_YUV422SPtoYUV422ILE() kenrel in c6accel */
     if ((refFd = fopen(ref_src_y_422sp,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_src_y_422sp);
         
     }
     fread(src_y, num_lines*src_pitch, sizeof(unsigned char), refFd);
     fclose(refFd);

     if ((refFd = fopen(ref_src_cbcr_422sp,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_src_cbcr_422sp);
         
      }
     fread(src_cbcr,num_lines*src_pitch, sizeof(unsigned char), refFd);
     fclose(refFd);
       
    memset (pOutBuf_16bpp,0x00,num_lines*dst_ycbcr_pitch*sizeof(unsigned char));
    memset (pRefBuf_16bpp,0x00,num_lines*dst_ycbcr_pitch*sizeof(unsigned char));
    LOG_STRING("C6accel_IMG_ycbcr422sp_to_ycbcr422ile(),");
    ycbcr_dst = (unsigned char *)pOutBuf_16bpp;
    
    START_BENCHMARK();
    /* Call the ycbcr422sp_to_ycbcr422ile*/
    C6accel_IMG_ycbcr422sp_to_ycbcr422ile( hC6accel,src_y,src_cbcr,ycbcr_dst,num_lines,width,src_pitch,dst_ycbcr_pitch);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_ycbcr422sp_to_ycbcr422ile() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    if ((refFd = fopen(ref_dst_ycbcr_422ile,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_dst_ycbcr_422ile);
         
     }

    /* read in reference output */
    fread(pRefBuf_16bpp, num_lines*dst_ycbcr_pitch, sizeof(unsigned char), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 8 bit char level */
    if (compare8bitArrays(pOutBuf_16bpp, pRefBuf_16bpp,num_lines*dst_ycbcr_pitch) != 1){
      printf("C6accel_IMG_ycbcr422sp_to_ycbcr422ile() failed\n");
      return 0;
    }
    printf("C6accel_IMG_ycbcr422sp_to_ycbcr422ile() successful\n");
    fclose(refFd);
    return 1;
}

int c6accel_test_IMG_YUV422SPtoYUV420PL(C6accel_Handle hC6accel,unsigned int num_lines, 
    unsigned int width,
    unsigned int src_pitch, 
    unsigned int dst_y_pitch,
    unsigned int dst_cbcr_pitch
)
{
    unsigned int            i,j;
    FILE *srcFd, *refFd;
    short *p16bitBuf;
    int status;
    unsigned char *src_y,*src_cbcr;
    unsigned char *y_dst, *cb_dst, *cr_dst;
    /* Create a CMem heap array that can be translated for DSP */
    unsigned char *pCr_Cmem;
    
    pCr_Cmem = Memory_alloc(num_lines*dst_cbcr_pitch*sizeof(unsigned char),&testfxnsMemParams);
    src_y = (unsigned char *)pSrcBuf_16bpp;
    src_cbcr = (unsigned char *)pWorkingBuf_16bpp;
    y_dst = (unsigned char *)pWorkingBuf2_16bpp;
    cb_dst = (unsigned char *)pOutBuf_16bpp;
    cr_dst = (unsigned char *)pCr_Cmem;
    
     /* Test IMG_YC_demux() kenrel in c6accel */
     if ((refFd = fopen(ref_src_y_422sp,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_src_y_422sp);
         
     }
     fread(src_y, num_lines*src_pitch, sizeof(unsigned char), refFd);
     fclose(refFd);

     if ((refFd = fopen(ref_src_cbcr_422sp,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_src_cbcr_422sp);
         
      }
     fread(src_cbcr,num_lines*src_pitch, sizeof(unsigned char), refFd);
     fclose(refFd);
       
    
    memset (pRefBuf_16bpp,0x00,num_lines*dst_cbcr_pitch*sizeof(unsigned char));
    LOG_STRING("C6accel_IMG_ycbcr422sp_to_ycbcr420pl(),");
    
    START_BENCHMARK();
    /* Call the ycbcr422sp_to_ycbcr420pl*/
    status=C6accel_IMG_ycbcr422sp_to_ycbcr420pl( hC6accel,src_y,src_cbcr,y_dst,cb_dst,cr_dst,num_lines,width,src_pitch,dst_y_pitch,dst_cbcr_pitch);
   
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_IMG_ycbcr422pl_to_ycbcr422sp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    if ((refFd = fopen(ref_dst_cb_420pl,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_dst_cb_420pl);
         
     }
     fread(pRefBuf_16bpp, dst_cbcr_pitch, sizeof(unsigned char), refFd);
 
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 8 bit char level */
    if (compare8bitArrays(pOutBuf_16bpp, pRefBuf_16bpp,num_lines*dst_cbcr_pitch/2) != 1){
      printf("C6accel_IMG_ycbcr422sp_to_ycbcr420pl() failed\n");
      return 0;
    }
    
    printf("C6accel_IMG_ycbcr422sp_to_ycbcr420pl successful\n");
    Memory_free(pCr_Cmem,num_lines*dst_cbcr_pitch*sizeof(unsigned char),&testfxnsMemParams);
    fclose(refFd);
    return 1;
}

/*
 * Test function for arithmetic rts functions in this function
 */
int c6accel_test_MATH_RTSARITH(C6accel_Handle hC6accel,unsigned int n)
{
 FILE *refFd;
 float *in1;
 float *in2;
 float *out;
 int i;
  /* Generate N bytes of 16 bit raw sine wave for use in tests
     Reuse pWorkingBuf2_16bpp for this
   */
    in1 = (float *)pWorkingBuf2_16bpp;
    in2 = (float *)pSrcBuf_16bpp;
    out = (float *)pRefBuf_16bpp;

   /*Read inputs */
    for (i=0;i<n;i++){
        in1[i] = xin[i];
        in2[i] = yin[i];
    }
    /* Open reference output files */
    if ((refFd = fopen(ref_addsp_i,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_addsp_i);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_addsp(),");
    START_BENCHMARK();

    C6accel_MATH_addsp_i(hC6accel,in1,in2,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_addsp_i() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_addsp() failed\n");
       return 0;
     }

    fclose(refFd);

    /* Test subsp_i kernel in c6accel */
    if ((refFd = fopen(ref_subsp_i,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_subsp_i);
       return (0);
     }

    /* Clear output arrays*/
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_subsp(),");
    START_BENCHMARK();

    C6accel_MATH_subsp_i(hC6accel,in1,in2,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_subsp_i() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_subsp() failed\n");
      return 0;
    }

    fclose(refFd);

   /* Test mpysp_i kernel in c6accel */
   /* Open reference output file */
    if ((refFd = fopen(ref_mpysp_i,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_mpysp_i);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_mpysp(),");
    START_BENCHMARK();

    C6accel_MATH_mpysp_i(hC6accel,in1,in2,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_mpysp_i() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_mpysp() failed\n");
      return 0;
    }

    fclose(refFd);

    /* Test divsp_i kernel in c6accel */
    if ((refFd = fopen(ref_divsp_i,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_divsp_i);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_divsp(),");
    START_BENCHMARK();

    C6accel_MATH_divsp_i(hC6accel,in1,in2,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_divsp_i() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_divsp() failed\n");
       return 0;
     }

    fclose(refFd);

    /* Test recipsp_i kernel in c6accel */
    if ((refFd = fopen(ref_recipsp_i,"rb")) == NULL) {
      printf("Failed to open refFd on %s\n",ref_recipsp_i);
      return (0);
    }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_recipsp(),");
    START_BENCHMARK();

    C6accel_MATH_recipsp_i(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_recipsp_i() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
     /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_recipsp() failed\n");
       return 0;
     }

     close(refFd);

    /* Test sqrtsp_i kernel in c6accel */
    if ((refFd = fopen(ref_sqrtsp_i,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_sqrtsp_i);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_sqrtsp(),");
    START_BENCHMARK();

    C6accel_MATH_sqrtsp_i(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_sqrtsp_i() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_sqrtsp() failed\n");
      return 0;
    }

    fclose(refFd);

    printf("Test for Arithmetic RTS functions successful\n");
    return 1;
}

/*
 * Test function for conversion rts kernels in this function
 */
int c6accel_test_MATH_RTSCONV(C6accel_Handle hC6accel,unsigned int n)
{
 FILE  *refFd;
 int   *in1, *intOut;
 unsigned int *uin1, *uintOut;
 float *out, *fltIn;
 int i;

    /* Use Contiguous working buffers for inputs */
    in1 = (int *)pWorkingBuf2_16bpp;
    out = (float *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        in1[i] = zin[i];
     }

    if ((refFd = fopen(ref_intsp_i,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_intsp_i);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_intsp(),");
    START_BENCHMARK();

    C6accel_MATH_intsp_i(hC6accel,(int *)in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_intsp_i() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_intsp() failed\n");
      return 0;
    }
    fclose(refFd);

    /* Test uintsp kernel in c6accel */
    /* Read input using working buffers */
    uin1 = (unsigned int *)pWorkingBuf2_16bpp;
    out = (float *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        uin1[i] = win[i];
     }
    /* Open reference output from file */
    if ((refFd = fopen(ref_uintsp_i,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_uintsp_i);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_uintsp(),");
    START_BENCHMARK();

    C6accel_MATH_uintsp_i(hC6accel,(unsigned int *)uin1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_uintsp_i() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_uintsp() failed\n");
       return 0;
    }
    fclose(refFd);

    /* Test spint kernel in c6accel */
    fltIn = (float *)pWorkingBuf2_16bpp;
    intOut = (int *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        fltIn[i] = xin[i];
     }

    /*Open reference output file*/
    if ((refFd = fopen(ref_spint_i,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_spint_i);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_spint(),");
    START_BENCHMARK();

    C6accel_MATH_spint_i(hC6accel,(float *)fltIn,(int *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_spint_i() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference output from file */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_spint() failed\n");
       return 0;
     }
    fclose(refFd);

    /* Test spuint kernel in c6accel*/
    fltIn = (float *)pWorkingBuf2_16bpp;
    uintOut = (unsigned int *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        fltIn[i] = xin[i];
    }
    /* open reference output */
    if ((refFd = fopen(ref_spuint_i,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_spuint_i);
       return (0);
       }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_spuint(),");
    START_BENCHMARK();

    C6accel_MATH_spuint_i(hC6accel,(float *)fltIn,(unsigned int *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_spuint_i() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_spuint() failed\n");
      return 0;
     }
    fclose(refFd);

    printf("Tests for Conversion RTS functions successful\n");
    return 1;
}

/*
 * Test function for IQMath conversion kernels in this function
 */
int c6accel_test_MATH_IQCONV(C6accel_Handle hC6accel,unsigned int n, unsigned int qfmt, unsigned int qfmt2)
{
 FILE  *refFd;
 int *IQ_Out, *IQ_In;
 int *in1, *intOut;
 float *fltOut, *fltIn;
 int i;

    /* Reuse pWorkingBuf2_16bpp for this*/
    fltIn = (float *)pWorkingBuf2_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    /* Set input vectors */
    for (i=0;i<n;i++){
        fltIn[i] = xin[i];
     }
    /* open reference output file */
    if ((refFd = fopen(ref_ftoiqn,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_ftoiqn);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_FTOIQ(),");
    START_BENCHMARK();

    C6accel_MATH_FTOIQN(hC6accel,(float *)fltIn,qfmt,(int *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_FTOIQN() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference Output */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_FtoIQN() failed\n");
       return 0;
    }
   fclose(refFd);

   /* Reuse pWorkingBuf2_16bpp for this */
    in1 = (int *)pWorkingBuf2_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        in1[i] = zin[i];
     }

    if ((refFd = fopen(ref_iqn,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqn);
       return (0);
    }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQN(),");
    START_BENCHMARK();

    C6accel_MATH_IQN(hC6accel,(int *)in1,qfmt,(int *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQN() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_IQN() failed\n");
      return 0;
    }
    fclose(refFd);

   /* Test IQ format to float conversion kernel */
   /* Reuse pWorkingBuf2_16bpp for this*/
    IQ_In = (int *)pWorkingBuf2_16bpp;
    fltOut = (float *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        IQ_In[i] = IQ16_in1[i];
     }

    /*Open reference output file */
    if ((refFd = fopen(ref_iqntof,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqntof);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_IQNTOF(),");
    START_BENCHMARK();

    C6accel_MATH_IQNtoF(hC6accel,(int *)IQ_In,qfmt,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNtoF() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1)
         {printf("C6accel_MATH_IQNTOF() failed\n");
          return 0;
          }
    fclose(refFd);

    /*Test for iq to int format conversion kernel */
    /* Reuse pWorkingBuf2_16bpp for this */
    IQ_In = (int *)pWorkingBuf2_16bpp;
    intOut = (int *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        IQ_In[i] = IQ16_in1[i];
     }

    if ((refFd = fopen(ref_iqnint,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnint);
       return (0);
       }

    /* Clear output arrays*/
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNint(),");
    START_BENCHMARK();

    C6accel_MATH_IQNint(hC6accel,(int *)IQ_In,qfmt,(int *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNint() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_IQNint() failed\n");
       return 0;
     }
    fclose(refFd);

    /* Test for iq to fraction conversion kernel */
    /* Reuse pWorkingBuf2_16bpp for this */
    IQ_In = (int *)pWorkingBuf2_16bpp;
    IQ_Out =(int *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        IQ_In[i] = IQ16_in1[i];
     }
    /* Open reference output file */
    if ((refFd = fopen(ref_iqnfrac,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnfrac);
       return (0);
       }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNfrac(),");
    START_BENCHMARK();

    C6accel_MATH_IQNfrac(hC6accel,(int *)IQ_In,qfmt,(int *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNfrac() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_IQNfrac() failed\n");
       return 0;
     }
    fclose(refFd);

   /* Test for IQXtoIQY conversion kernel in c6accel*/
   /* Reuse pWorkingBuf2_16bpp for this */
    IQ_In = (int *)pWorkingBuf2_16bpp;
    IQ_Out =(int *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        IQ_In[i] = IQ16_in1[i];
     }
    /* Open reference output file */
    if ((refFd = fopen(ref_iqxtoiqy,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqxtoiqy);
       return (0);
       }

    /* Clear output arrays*/
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQXtoIQY(),");
    START_BENCHMARK();

    C6accel_MATH_IQXtoIQY(hC6accel,(int *)IQ_In,qfmt,qfmt2,(int *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQXtoIQY() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1)
         {printf("C6accel_MATH_IQNfrac() failed\n");
          return 0;
          }
     fclose(refFd);

    printf("Tests for IQmath Conversion functions successful\n");
    return 1;
}

/*
 * Test function for IQMATH math functions in this function
 */
int c6accel_test_MATH_IQMATH(C6accel_Handle hC6accel,unsigned int n, unsigned int qfmt)
{
 FILE  *refFd;
 int *IQ_Out, *IQ_In1, *IQ_In2;
 int i;

    /* Reuse pWorkingBuf2_16bpp for this*/
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in1[i];
     }
    /* Open reference output file */
    if ((refFd = fopen(ref_iqnlog,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnlog);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNlog(),");
    START_BENCHMARK();

    C6accel_MATH_IQNlog(hC6accel,(int *)IQ_In1,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNlog() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_IQNlog() failed\n");
      return 0;
    }
    fclose(refFd);

    /* Test for IQabs kernel in c6accel */
    /* Reuse pWorkingBuf2_16bpp for this */
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in2[i];
     }
    /* Open reference output file */
    if ((refFd = fopen(ref_iqnabs,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnabs);
       return (0);
    }

    /* Clear output arrays*/
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNabs(),");
    START_BENCHMARK();

    C6accel_MATH_IQNabs(hC6accel,(int *)IQ_In1,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNabs() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_IQNabs() failed\n");
       return 0;
     }
     fclose(refFd);


    /* Test for IQsqrt kernelin c6accel */
    /* Reuse pWorkingBuf2_16bpp for reading the inputs */
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in5[i];
     }

    if ((refFd = fopen(ref_iqnsqrt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnsqrt);
       return (0);
       }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNsqrt(),");
    START_BENCHMARK();

    C6accel_MATH_IQNsqrt(hC6accel,(int *)IQ_In1,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNsqrt() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_IQNsqrt() failed\n");
       return 0;
    }
     fclose(refFd);

    /* Test IQMATH inverse sqrt in c6accel */
    /* Reuse pWorkingBuf2_16bpp for this*/
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in5[i];
     }

    /* Open refernce output file */
    if ((refFd = fopen(ref_iqnisqrt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnisqrt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNisqrt(),");
    START_BENCHMARK();

    C6accel_MATH_IQNisqrt(hC6accel,(int *)IQ_In1,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNisqrt() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1)
         {printf("C6accel_MATH_IQNisqrt() failed\n");
          return 0;
          }
    fclose(refFd);

    /* Test IQMath exponential function in c6accel */
    /* Reuse pWorkingBuf2_16bpp for this */
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in3[i];
     }

    /* Open reference test output file */
    if ((refFd = fopen(ref_iqnexp,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnexp);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNexp(),");
    START_BENCHMARK();

    C6accel_MATH_IQNexp(hC6accel,(int *)IQ_In1,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNexp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_IQNexp() failed\n");
      return 0;
    }

    fclose(refFd);

    /* Test for power function in IQMath */
    /*  Reuse pWorkingBuf2_16bpp to read inputs */
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_In2 = (int *)pWorkingBuf2_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in3[i];
        IQ_In2[i] = IQ16_in4[i];
     }

    /* Open reference test output file */
    if ((refFd = fopen(ref_iqnpow,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnpow);
       return (0);

     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNpow(),");
    START_BENCHMARK();

    C6accel_MATH_IQNpow(hC6accel,(int *)IQ_In1,(int *)IQ_In2,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNpow() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_IQNpow() failed\n");
      return 0;
    }
    fclose(refFd);

    /* Test for IQNmag kernel in c6accel */
    /* Reuse pWorkingBuf2_16bpp for this */
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_In2 = (int *)pWorkingBuf2_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in3[i];
        IQ_In2[i] = IQ16_in4[i];
     }
    /* Open reference ouptu file */
    if ((refFd = fopen(ref_iqnmag,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnmag);
       return (0);
       }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNmag(),");
    START_BENCHMARK();

    C6accel_MATH_IQNmag(hC6accel,(int *)IQ_In1,(int *)IQ_In2,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNmag() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_IQNmag() failed\n");
      return 0;
    }
    fclose(refFd);

   printf("Tests for Mathematical IQmath functions successful\n");
   return 1;
}

/*
 * Test function for arithmetic IQMath kernel.
 */
int c6accel_test_MATH_IQARITH(C6accel_Handle hC6accel,unsigned int n, unsigned int qfmt)
{
 FILE  *refFd;
 int *IQ_Out, *IQ_In1, *IQ_In2;
 int i;
 int *intIn, *intOut;

  /* Test for IQMath multiplication kernel */
  /* Reuse pWorkingBuf2_16bpp for this     */
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_In2 = (int *)pWorkingBuf2_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;


    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in3[i];
        IQ_In2[i] = IQ16_in4[i];
     }
    /* Open reference ouput file */
    if ((refFd = fopen(ref_iqnmpy,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnmpy);
       return (0);
       }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));


    LOG_STRING("C6accel_MATH_IQNmpy(),");
    START_BENCHMARK();

    C6accel_MATH_IQNmpy(hC6accel,(int *)IQ_In1,(int *)IQ_In2,(int *)pOutBuf_16bpp,qfmt,n);

    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNmpy() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_IQNmpy() failed\n");
      return 0;
    }
    fclose(refFd);

    /*Test for IQNmpyIQx in c6accel*/
    /* Reuse pWorkingBuf2_16bpp for this*/
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_In2 = (int *)pWorkingBuf2_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    /*Ser input vectors*/
    for (i=0;i<n;i++){
        IQ_In1[i] = IQ24_in[i];
        IQ_In2[i] = IQ12_in[i];
     }

    /*Open reference ouput file */
    if ((refFd = fopen(ref_iqnmpyIQx,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnmpyIQx);
       return (0);
       }

    /* Clear output arrays*/
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNmpyIQX(),");
    START_BENCHMARK();

    C6accel_MATH_IQNmpyIQx(hC6accel,(int *)IQ_In1,24,(int *)IQ_In2,12,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNmpyIQx() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_IQNmpyIQX() failed\n");
      return 0;
    }
    fclose(refFd);

    /* Test IQNrmpy kernel in c6accel */
    /* Reuse pWorkingBuf2_16bpp for this*/
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_In2 = (int *)pWorkingBuf2_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    /* Set test input */
    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in3[i];
        IQ_In2[i] = IQ16_in4[i];
     }

    /*Open reference output file */
    if ((refFd = fopen(ref_iqnrmpy,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnrmpy);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNrmpy(),");
    START_BENCHMARK();

    C6accel_MATH_IQNrmpy(hC6accel,(int *)IQ_In1,(int *)IQ_In2,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNrmpy() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_IQNrmpy() failed\n");
      return 0;
     }
     fclose(refFd);

    /*Test IQNrsmpy in c6accel */
    /* Reuse pWorkingBuf2_16bpp for this */
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_In2 = (int *)pWorkingBuf2_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    /* Set test input vectors */
    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in3[i];
        IQ_In2[i] = IQ16_in4[i];
     }

    /* Open reference output file */
    if ((refFd = fopen(ref_iqnrsmpy,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnrsmpy);
       return (0);
     }

     /* Clear output arrays*/
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNrsmpy(),");
    START_BENCHMARK();

    C6accel_MATH_IQNrsmpy(hC6accel,(int *)IQ_In1,(int *)IQ_In2,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNrsmpy() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_IQNrsmpy() failed\n");
       return 0;
     }
    fclose(refFd);

    /* Reuse pWorkingBuf2_16bpp for this */
    IQ_In1 = (int *)pSrcBuf_16bpp;
    intIn = (int *)pWorkingBuf2_16bpp;
    intOut = (int *)pRefBuf_16bpp;

    /*Setup input test vectors */
    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in3[i];
        intIn[i] = win[i];
     }
    /* Open reference test output file */
    if ((refFd = fopen(ref_iqnmpyI32int,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnmpyI32int);
       return (0);
       }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNmpyI32int(),");
    START_BENCHMARK();

    C6accel_MATH_IQNmpyI32int(hC6accel,(int *)IQ_In1,(int *)intIn,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNmpyI32int() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_IQNmpyI32int() failed\n");
      return 0;
     }
    fclose(refFd);

    /*Test IQNmpyI32frac kernel in c6accel */
    /* Reuse pWorkingBuf2_16bpp for this */
    IQ_In1 = (int *)pSrcBuf_16bpp;
    intIn = (int *)pWorkingBuf2_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    /*Set test input vectors  */
    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in3[i];
        intIn[i] = win[i];
     }

    /* open test output file */
    if ((refFd = fopen(ref_iqnmpyI32frac,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnmpyI32frac);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNmpyI32frac(),");
    START_BENCHMARK();

    C6accel_MATH_IQNmpyI32frac(hC6accel,(int *)IQ_In1,(int *)intIn,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_IQNmpyI32frac() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_IQNmpyI32frac() failed\n");
       return 0;
    }

    fclose(refFd);

    /* Test for IQmath divion kernel */
    /* Reuse pWorkingBuf2_16bpp for this */
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_In2 = (int *)pWorkingBuf2_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;
    /*Set test input vectors */
    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in3[i];
        IQ_In2[i] = IQ16_in4[i];
     }
    /* Open reference output file  */
    if ((refFd = fopen(ref_iqndiv,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqndiv);
       return (0);
       }

   /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNdiv(),");
    START_BENCHMARK();

    C6accel_MATH_IQNdiv(hC6accel,(int *)IQ_In1,(int *)IQ_In2,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_IQNdiv() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /*read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_IQNdiv() failed\n");
       return 0;
     }
    fclose(refFd);

    printf("Tests for Arithmetic IQmath functions successful\n");
    return 1;
}

/*
 * Test function for trignometric kernels in c6accel
 */
int c6accel_test_MATH_IQTRIG(C6accel_Handle hC6accel,unsigned int n, unsigned int qfmt)
{
 FILE  *refFd;
 int *IQ_Out, *IQ_In1, *IQ_In2;
 int i;

   /* Test for IQmath trignometric function */
   /* Reuse pWorkingBuf2_16bpp for this */
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

   /*Set tet input vectors */
    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in5[i];
     }

    /* Open reference output file */
    if ((refFd = fopen(ref_iqnsin,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnsin);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNsin(),");
    START_BENCHMARK();

    C6accel_MATH_IQNsin(hC6accel,(int *)IQ_In1,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_IQNsin() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
      printf("C6accel_MATH_IQNsin() failed\n");
      return 0;
    }
    fclose(refFd);

    /*Test for IQmathcos kernel */
    /* Reuse pWorkingBuf2_16bpp for this*/
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    /* Set test input vectors */
    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in5[i];
     }
    /* Open reference output file */

    if ((refFd = fopen(ref_iqncos,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqncos);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNcos(),");
    START_BENCHMARK();

    C6accel_MATH_IQNcos(hC6accel,(int *)IQ_In1,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_IQNcos() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_IQNcos() failed\n");
       return 0;
    }
    fclose(refFd);


    /*Test for IQsinPU kernel */
   /* Reuse pWorkingBuf2_16bpp for this*/
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;
    /* Set test input vectors */
    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in5[i];
     }
    /* Open reference test output vectors*/
    if ((refFd = fopen(ref_iqnsinPU,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnsinPU);
       return (0);
       }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNsinPU(),");
    START_BENCHMARK();

    C6accel_MATH_IQNsinPU(hC6accel,(int *)IQ_In1,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_IQNsinPU() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1) {
       printf("C6accel_MATH_IQNsinPU() failed\n");
       return 0;
     }
     fclose(refFd);

    /* Reuse pWorkingBuf2_16bpp for this*/
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    /* Set test input vectors */
    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in5[i];
     }

    /* Open reference test vector file */
    if ((refFd = fopen(ref_iqncosPU,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqncosPU);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNcosPU(),");
    START_BENCHMARK();

    C6accel_MATH_IQNcosPU(hC6accel,(int *)IQ_In1,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_IQNcosPU() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_IQNcosPU() failed\n");
       return 0;
     }
    fclose(refFd);

    /* Test IQatan2 kernel in c6accel */
   /* Reuse pWorkingBuf2_16bpp for settingup the input vectors */
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_In2 = (int *)pWorkingBuf2_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    /* Setup test input vectors */
    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in3[i];
        IQ_In2[i] = IQ16_in4[i];
     }

    /* Open reference test output file */
    if ((refFd = fopen(ref_iqnatan2,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnatan2);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNatan2(),");
    START_BENCHMARK();

    C6accel_MATH_IQNatan2(hC6accel,(int *)IQ_In1,(int *)IQ_In2,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_IQNatan2() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /*  Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_IQNatan2() failed\n");
       return 0;
     }
    fclose(refFd);

    /* Test IQatan2PU kernel in C6accel */
    /* Reuse pWorkingBuf2_16bpp for setting up test inputs */
    IQ_In1 = (int *)pSrcBuf_16bpp;
    IQ_In2 = (int *)pWorkingBuf2_16bpp;
    IQ_Out = (int *)pRefBuf_16bpp;

    /* Setup input test vectors */
    for (i=0;i<n;i++){
        IQ_In1[i] = IQ16_in3[i];
        IQ_In2[i] = IQ16_in4[i];
     }
    /* Open test reference output file */
    if ((refFd = fopen(ref_iqnatan2PU,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_iqnatan2PU);
       return (0);
    }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(int));
    memset (pRefBuf_16bpp,0x00,n*sizeof(int));

    LOG_STRING("C6accel_MATH_IQNatan2PU(),");
    START_BENCHMARK();

    C6accel_MATH_IQNatan2PU(hC6accel,(int *)IQ_In1,(int *)IQ_In2,(int *)pOutBuf_16bpp,qfmt,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_IQNatan2PU() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference */
    fread(pRefBuf_16bpp,  n, sizeof(int), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level */
    if (compare32bitArrays((unsigned int *)pOutBuf_16bpp, (unsigned int *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_IQNatan2PU() failed\n");
       return 0;
     }
    fclose(refFd);

    printf("Tests for Trignometric IQmath functions successful\n");
    return 1;
}

#if (PLATFORM == 138)
/* Test for Floating point kernels */
/*
 * Test function for arithmetic rts single precision functions in this function
 */
int c6accel_test_MATH_RTSFLT(C6accel_Handle hC6accel,unsigned int n)
{
 FILE *refFd;
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

   if ((refFd = fopen(ref_cossp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_cossp_x1);
         }
    fread(in1, n, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_cossp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_cossp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_cossp(),");
    START_BENCHMARK();

    C6accel_MATH_cossp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_sinsp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_cossp() failed\n");
       return 0;
     }

    fclose(refFd);

   /*Read inputs */
   if ((refFd = fopen(ref_sinsp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_sinsp_x1);
         }
    fread(in1, n, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_sinsp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_sinsp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_sinsp(),");
    START_BENCHMARK();

    C6accel_MATH_sinsp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_sinsp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_sinsp() failed\n");
       return 0;
     }

    fclose(refFd);

    /*Read inputs */
   if ((refFd = fopen(ref_atansp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_atansp_x1);
         }
    fread(in1, n, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_atansp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_atansp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_atansp(),");
    START_BENCHMARK();

    C6accel_MATH_atansp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_atansp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_atansp() failed\n");
       return 0;
     }

    fclose(refFd);

    /*Read inputs */
   if ((refFd = fopen(ref_logsp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_logsp_x1);
         }
    fread(in1, n, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_logsp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_logsp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_logsp(),");
    START_BENCHMARK();

    C6accel_MATH_logsp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_logsp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_logsp() failed\n");
       return 0;
     }

    fclose(refFd);

   /*Read inputs */
   if ((refFd = fopen(ref_expsp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_expsp_x1);
         }
    fread(in1, n, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_expsp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_expsp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_expsp(),");
    START_BENCHMARK();

    C6accel_MATH_expsp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_expsp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_expsp() failed\n");
       return 0;
     }

    fclose(refFd);

    /*Read inputs */
   if ((refFd = fopen(ref_exp2sp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_exp2sp_x1);
         }
    fread(in1, n, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_exp2sp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_exp2sp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_exp2sp(),");
    START_BENCHMARK();

    C6accel_MATH_exp2sp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_exp2sp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_exp2sp() failed\n");
       return 0;
     }

    fclose(refFd);

    /*Read inputs */
   if ((refFd = fopen(ref_exp10sp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_exp10sp_x1);
         }
    fread(in1, n, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_exp10sp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_exp10sp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_exp10sp(),");
    START_BENCHMARK();

    C6accel_MATH_exp10sp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_exp10sp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_exp10sp() failed\n");
       return 0;
     }

    fclose(refFd);

    /*Read inputs */
   if ((refFd = fopen(ref_recipsp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_recipsp_x1);
         }
    fread(in1, n, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_recipsp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_recipsp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_recipsp(),");
    START_BENCHMARK();

    C6accel_MATH_recipsp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_recipsp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_recipsp() failed\n");
       return 0;
     }

    fclose(refFd);

   /*Read inputs */
   if ((refFd = fopen(ref_sqrtsp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_sqrtsp_x1);
         }
    fread(in1, n, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_sqrtsp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_sqrtsp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_sqrtsp(),");
    START_BENCHMARK();

    C6accel_MATH_sqrtsp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_sqrtsp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_sqrtsp() failed\n");
       return 0;
     }
    fclose(refFd);

   /*Read inputs */
   if ((refFd = fopen(ref_rsqrtsp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_rsqrtsp_x1);
         }
    fread(in1, n, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_rsqrtsp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_rsqrtsp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_rsqrtsp(),");
    START_BENCHMARK();

    C6accel_MATH_rsqrtsp(hC6accel,in1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_rsqrtsp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }

    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_rsqrtsp() failed\n");
       return 0;
     }

    fclose(refFd);

   /*Read inputs */
   if ((refFd = fopen(ref_atan2sp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_atan2sp_x1);
         }
    fread(in1, n, sizeof(float), refFd);
    fclose(refFd);

    if ((refFd = fopen(ref_atan2sp_x2,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_atan2sp_x2);
         }
    fread(in2, n, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_atan2sp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_atan2sp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_atan2sp(),");
    START_BENCHMARK();

    C6accel_MATH_atan2sp(hC6accel,(float *)in1,(float *)in2,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,  C6accel_MATH_atan2sp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_atan2sp() failed\n");
       return 0;
     }

    fclose(refFd);

    /*Read inputs */
   if ((refFd = fopen(ref_powsp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_powsp_x1);
         }
    fread(in1, n, sizeof(float), refFd);
    fclose(refFd);

    if ((refFd = fopen(ref_powsp_x2,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_powsp_x2);
         }
    fread(in2, n, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_powsp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_powsp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(float));
    memset (pRefBuf_16bpp,0x00,n*sizeof(float));

    LOG_STRING("C6accel_MATH_powsp(),");
    START_BENCHMARK();

    C6accel_MATH_powsp(hC6accel,(float *)in1,(float *)in2,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_powsp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_powsp() failed\n");
       return 0;
     }

    fclose(refFd);

    printf("Tests for Floating point RTS single precision functions successful\n");
    return 1;
}

/*

 * Test function for arithmetic rts double precision functions in this function
 */
int c6accel_test_MATH_RTSFLTDP(C6accel_Handle hC6accel,unsigned int n)
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

   /*Read inputs */

   if ((refFd = fopen(ref_cosdp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_cosdp_x1);
         }
    fread(in1, n, sizeof(double), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_cosdp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_cosdp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    LOG_STRING("C6accel_MATH_cosdp(),");
    START_BENCHMARK();

    C6accel_MATH_cosdp(hC6accel,in1,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_cosdp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(double), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparedoubleArrays((double *)pOutBuf_16bpp, (double *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_cosdp() failed\n");
       return 0;
     }

    fclose(refFd);

   /*Read inputs */
   if ((refFd = fopen(ref_sindp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_sindp_x1);
         }
    fread(in1, n, sizeof(double), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_sindp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_sindp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    LOG_STRING("C6accel_MATH_sindp(),");
    START_BENCHMARK();

    C6accel_MATH_sindp(hC6accel,in1,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_sindp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(double), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparedoubleArrays((double *)pOutBuf_16bpp, (double *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_sindp() failed\n");
       return 0;
     }

    fclose(refFd);

    /*Read inputs */
   if ((refFd = fopen(ref_atandp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_atandp_x1);
         }
    fread(in1, n, sizeof(double), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_atandp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_atandp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    LOG_STRING("C6accel_MATH_atandp(),");
    START_BENCHMARK();

    C6accel_MATH_atandp(hC6accel,in1,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_atandp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(double), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparedoubleArrays((double *)pOutBuf_16bpp, (double *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_atandp() failed\n");
       return 0;
     }

    fclose(refFd);

    /*Read inputs */
   if ((refFd = fopen(ref_logdp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_logdp_x1);
         }
    fread(in1, n, sizeof(double), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_logdp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_logdp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    LOG_STRING("C6accel_MATH_logdp(),");
    START_BENCHMARK();

    C6accel_MATH_logdp(hC6accel,in1,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_IQNatan2PU() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(double), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparedoubleArrays((double *)pOutBuf_16bpp, (double *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_logdp() failed\n");
       return 0;
     }

    fclose(refFd);

   /*Read inputs */
   if ((refFd = fopen(ref_expdp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_expdp_x1);
         }
    fread(in1, n, sizeof(double), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_expdp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_expdp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    LOG_STRING("C6accel_MATH_expdp(),");
    START_BENCHMARK();

    C6accel_MATH_expdp(hC6accel,in1,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_expdp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(double), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparedoubleArrays((double *)pOutBuf_16bpp, (double *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_expdp() failed\n");
       return 0;
     }

    fclose(refFd);

    /*Read inputs */
   if ((refFd = fopen(ref_exp2dp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_exp2dp_x1);
         }
    fread(in1, n, sizeof(double), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_exp2dp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_exp2dp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    LOG_STRING("C6accel_MATH_exp2dp(),");
    START_BENCHMARK();

    C6accel_MATH_exp2dp(hC6accel,in1,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_exp2dp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(double), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparedoubleArrays((double *)pOutBuf_16bpp, (double *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_exp2dp() failed\n");
       return 0;
     }

    fclose(refFd);

    /*Read inputs */
   if ((refFd = fopen(ref_exp10dp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_exp10dp_x1);
         }
    fread(in1, n, sizeof(double), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_exp10dp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_exp10dp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    LOG_STRING("C6accel_MATH_exp10dp(),");
    START_BENCHMARK();

    C6accel_MATH_exp10dp(hC6accel,in1,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_exp10dp() wait for callback,");


         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(double), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparedoubleArrays((double *)pOutBuf_16bpp, (double *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_exp10dp() failed\n");
       return 0;
     }

    fclose(refFd);

   /*Read inputs */
   if ((refFd = fopen(ref_sqrtdp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_sqrtdp_x1);
         }
    fread(in1, n, sizeof(double), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_sqrtdp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_sqrtdp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    LOG_STRING("C6accel_MATH_sqrtdp(),");
    START_BENCHMARK();

    C6accel_MATH_sqrtdp(hC6accel,in1,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_sqrtdp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(double), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparedoubleArrays((double *)pOutBuf_16bpp, (double *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_sqrtdp() failed\n");
       return 0;
     }
    fclose(refFd);

   /*Read inputs */
   if ((refFd = fopen(ref_rsqrtdp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_rsqrtdp_x1);
         }
    fread(in1, n, sizeof(double), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_rsqrtdp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_rsqrtdp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    LOG_STRING("C6accel_MATH_rsqrtdp(),");
    START_BENCHMARK();

    C6accel_MATH_rsqrtdp(hC6accel,in1,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_rsqrtdp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(double), refFd);
    LOG_STRING("\n");
   
    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparedoubleArrays((double *)pOutBuf_16bpp, (double *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_rsqrtdp() failed\n");
       return 0;
     }

    fclose(refFd);

   /*Read inputs */
   if ((refFd = fopen(ref_atan2dp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_atan2dp_x1);
         }
    fread(in1, n, sizeof(double), refFd);
    fclose(refFd);

    if ((refFd = fopen(ref_atan2dp_x2,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_atan2dp_x2);
         }
    fread(in2, n, sizeof(double), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_atan2dp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_atan2dp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    LOG_STRING("C6accel_MATH_atan2dp(),");
    START_BENCHMARK();

    C6accel_MATH_atan2dp(hC6accel,(double *)in1,(double *)in2,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_MATH_atan2dp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(double), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparedoubleArrays((double *)pOutBuf_16bpp, (double *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_atan2dp() failed\n");
       return 0;
     }

    fclose(refFd);

    /*Read inputs */
   if ((refFd = fopen(ref_powdp_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_powdp_x1);
         }
    fread(in1, n, sizeof(double), refFd);
    fclose(refFd);

    if ((refFd = fopen(ref_powdp_x2,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_powdp_x2);

         }
    fread(in2, n, sizeof(double), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_powdp_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_powdp_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00,n*sizeof(double));
    memset (pRefBuf_16bpp,0x00,n*sizeof(double));

    LOG_STRING("C6accel_MATH_powdp(),");
    START_BENCHMARK();

    C6accel_MATH_powdp(hC6accel,(double *)in1,(double *)in2,(double *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_MATH_powdp() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  n, sizeof(double), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparedoubleArrays((double *)pOutBuf_16bpp, (double *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_MATH_powdp() failed\n");
       return 0;
     }

    fclose(refFd);

    printf("Tests for Floating point RTS double precision functions successful\n");
    return 1;
}

/*
 * Test function for floating point autocorrelation function
*/
int c6accel_test_DSPF_AUTOCOR(C6accel_Handle hC6accel, int Nx, int Nr)
{
 FILE *refFd;
 float *in1;
 int i;

/* Initialize Source and Output buffers to contiguous memory allocated   */
    in1 = (float *)pSrcBuf_16bpp;
    
   /*Read inputs */
   if ((refFd = fopen(ref_autocor_x,"rb")) == NULL) {
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

    LOG_STRING("C6accel_DSPF_autocor(),");
    START_BENCHMARK();

    C6accel_DSPF_sp_autocor(hC6accel,(float *)pOutBuf_16bpp,(float *)in1,Nx, Nr);

    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" , C6accel_DSPF_sp_autocor() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  Nr, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on float level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,Nr) != 1){
       printf("C6accel_DSPF_autocor() failed\n");
       return 0;
     }
    
    fclose(refFd);

    printf("Tests for Floating point DSP autocorrelation functions successful\n");
    return 1;
}

/*
 * Test function for floating point convolution function
*/
int c6accel_test_DSPF_CONVOL(C6accel_Handle hC6accel,short nh,short ny)
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
   
    if ((refFd = fopen(ref_convol_hinput,"rb")) == NULL) {
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

    LOG_STRING("C6accel_DSPF_convol(),");
    START_BENCHMARK();

    C6accel_DSPF_sp_convol(hC6accel,(float *)inX,(float *)inH,(float *)pOutBuf_16bpp,(short)nh, (short)ny);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_convol() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  ny, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on float level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,ny) != 1){
       printf("C6accel_DSPF_convol() failed\n");
       return 0;
     }

    fclose(refFd);

    printf("Tests for Floating point DSP convolution functions successful\n");
    return 1;
}


/*
 * Test function for floating point Matrix multiplication functions
*/
int c6accel_test_DSPF_MATFXNS(C6accel_Handle hC6accel, int c1, int r1, int c2 )
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

    LOG_STRING("C6accel_DSPF_sp_mat_mul(),");
    START_BENCHMARK();

    C6accel_DSPF_sp_mat_mul(hC6accel,inX1,r1, c1,inX2,c2,(float *)pOutBuf_16bpp);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_mat_mul() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  r1*c2, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp, r1*c2) != 1){
       printf("C6accel_DSPF_mat_mul() failed\n");
       return 0;
     }

    fclose(refFd);



    printf("Tests for Floating point DSP matrix functions successful\n");
    return 1;
}


// Test for Matrix Multiplication of complex matrices
int c6accel_test_DSPF_MAT_MUL_CPLX(C6accel_Handle hC6accel,int c1,int r1,int c2 )
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

// Test for Matrix Multiplication of complex matrices
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

    LOG_STRING("C6accel_DSPF_sp_mat_mul_cplx(),");
    START_BENCHMARK();

    C6accel_DSPF_sp_mat_mul_cplx(hC6accel,inX1,r1, c1,inX2,c2,(float *)pOutBuf_16bpp);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_mat_mul_cplx() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  2*r1*c2, sizeof(float), refFd);
    LOG_STRING("\n");
    
    /* Validate output between native MATH/ref and C6Accel on float level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,2*r1*c2) != 1){
       printf("C6accel_DSPF_mat_mul_cplx() failed\n");
       return 0;
     }

    fclose(refFd);

    printf("Tests for Floating point DSP Matrix Multiplication functions successful\n");
    return 1;
}
/*
 * Test function for floating point Matrix transpose
*/
int c6accel_test_DSPF_MAT_TRANS(C6accel_Handle hC6accel, int r1, int c1 )
{
 FILE *refFd;
 float *inX1;
 int i;

/* Initialize Source and Output buffers to contiguous memory allocated   */
    inX1 = (float *)pSrcBuf_16bpp;
    
   // Test for Matrix Transpose of complex matrices
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

    LOG_STRING("C6accel_DSPF_sp_trans(),");
    START_BENCHMARK();

    C6accel_DSPF_sp_mat_trans(hC6accel,inX1,r1, c1,(float *)pOutBuf_16bpp);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_mat_trans() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  r1*c1, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on float level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,r1*c1) != 1){
       printf("C6accel_DSPF_mat_trans() failed\n");
       return 0;
     }

    fclose(refFd);

    printf("Test for Floating point DSP matrix transpose function successful\n");
    return 1;
}

/*
 * Test function for floating point vector dot product functions
*/
int c6accel_test_DSPF_DOTPRODFXNS(C6accel_Handle hC6accel,unsigned int n)
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

   /*Read inputs */
   if ((refFd = fopen(ref_dotprod_x1,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_dotprod_x1);
         }
    fread(inX1, n, sizeof(float), refFd);
    fclose(refFd);

    if ((refFd = fopen(ref_dotprod_x2,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_dotprod_x2);
         }
    fread(inX2, n, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_dotprod_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_dotprod_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00, n*sizeof(float));
    memset (pRefBuf_16bpp,0x00, n*sizeof(float));

    LOG_STRING("C6accel_DSPF_sp_dotprod(),");
    START_BENCHMARK();

    C6accel_DSPF_sp_dotprod(hC6accel,inX1,inX2,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_dotprod() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp,  1, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,1) != 1){
       printf("C6accel_DSPF_sp_dotprod() failed\n");
       return 0;
     }

    fclose(refFd);

    printf("Test for Floating point DSP vector dot product function executed successful\n");
    return 1;
}


/*
 * Test for FIR filter generation kernels in c6accel
 */
int c6accel_test_DSPF_FIR(C6accel_Handle hC6accel,unsigned int ny,unsigned int nh)
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
   if ((refFd = fopen(ref_fir_gen_xinput,"rb")) == NULL) {
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

    LOG_STRING("C6accel_DSPF_sp_fir_gen(),");
    START_BENCHMARK();

    /* Call the DSP_fir_gen function in C6Accel*/
    C6accel_DSPF_sp_fir_gen(hC6accel,inX, inH,(float *)pOutBuf_16bpp,nh,ny);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_fir_gen() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    fread(pRefBuf_16bpp, ny, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,ny) != 1){
       printf("C6accel_DSPF_sp_fir_gen() failed\n");
       return 0;
     }
    printf("Test for Floating point DSP FIR filter successful\n");
     fclose(refFd);
     return 1;
}

int c6accel_test_DSPF_FIR_CPLX(C6accel_Handle hC6accel, int ny, int nh)
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

    LOG_STRING("C6accel_DSPF_sp_fir_cplx(),");
    START_BENCHMARK();

    /* Call the DSP_fir_cplx function in C6Accel*/
    status = C6accel_DSPF_sp_fir_cplx(hC6accel,(float *)inX, inH,(float *)pOutBuf_16bpp,nh,ny);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_fir_cplx() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    fread(pRefBuf_16bpp, 2*ny, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native MATH/ref and C6Accel on float level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,2*ny) != 1){
       printf("C6accel_DSPF_sp_fir_cplx() failed\n");
       return 0;
     }

    fclose(refFd);

    printf("Test for Floating point Complex FIR Filtering successful\n");
    return (1);
}

/*
 * Test for IIR filter generation kernels in c6accel
 */
int c6accel_test_DSPF_IIR(C6accel_Handle hC6accel, int n)
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
    if ((refFd = fopen(ref_iir_xinput,"rb")) == NULL) {
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

    LOG_STRING("C6accel_DSPF_sp_iir(),");
    START_BENCHMARK();

    /* Call IIR kernel in C6Accel */
    status = C6accel_DSPF_sp_iir(hC6accel,(float *)pOutBuf_16bpp,inX,y2,hb,ha,n);
   
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_iir() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    fread(pRefBuf_16bpp, n+4,sizeof(float), refFd);

    LOG_STRING("\n");

   /* Validate output between native MATH/ref and C6Accel on float level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n+4) != 1){
       printf("C6accel_DSPF_sp_iir() failed\n");
       return 0;
     }

     fclose(refFd);

     printf("Tests for Floating point DSP IIR Filter successful\n");
     return (1);
}

/*
 * Test function for floating point vector multiplication functions
*/
int c6accel_test_DSPF_VECMUL(C6accel_Handle hC6accel,unsigned int n )
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

   /*Read inputs */
   if ((refFd = fopen(ref_vecmul_x1input,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_vecmul_x1input);
         }
    fread(inX1, n, sizeof(float), refFd);
    fclose(refFd);

    if ((refFd = fopen(ref_vecmul_x2input,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_vecmul_x2input);
         }
    fread(inX2, n, sizeof(float), refFd);
    fclose(refFd);

    /* Open reference output files */
    if ((refFd = fopen(ref_vecmul_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_vecmul_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00, n*sizeof(float));
    memset (pRefBuf_16bpp,0x00, n*sizeof(float));

    LOG_STRING("C6accel_DSPF_sp_vecmul(),");
    START_BENCHMARK();

    C6accel_DSPF_sp_vecmul(hC6accel,inX1,inX2,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_vecmul() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp, n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp, n) != 1){
       printf("C6accel_DSPF_vecmul() failed\n");
       return 0;
     }

    fclose(refFd);

     printf("Test for Floating point DSP vector multiplication successful\n");
    return 1;
}

/*
 * Test function for floating point vector multiplication functions
*/
int c6accel_test_DSPF_VECRECIP(C6accel_Handle hC6accel,unsigned int n )
{
 FILE *refFd;
 float *inX1;
 float *out;
 int i;

/* Initialize Source and Output buffers to contiguous memory allocated   */
    inX1 = (float *)pSrcBuf_16bpp;
    out = (float *)pRefBuf_16bpp;


 //Test Vector sum function in C6accel
    /*Read inputs */
   if ((refFd = fopen(ref_vecrecip_xinput,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_vecrecip_xinput);
         }
    fread(inX1, n, sizeof(float), refFd);
    fclose(refFd);


    /* Open reference output files */
    if ((refFd = fopen(ref_vecrecip_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_vecrecip_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00, n*sizeof(float));
    memset (pRefBuf_16bpp,0x00, n*sizeof(float));

    LOG_STRING("C6accel_DSPF_sp_vecrecip(),");
    START_BENCHMARK();

    C6accel_DSPF_sp_vecrecip(hC6accel,inX1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_vecrecip() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp, n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp, n) != 1){
       printf("C6accel_DSPF_vecrecip() failed\n");
       return 0;
     }

    fclose(refFd);
    printf("Test for Floating point DSP vector reciprocal function successful\n");
    return 1;
}


/*
 * Test function for floating point vector sum of squares functions
*/
int c6accel_test_DSPF_VECSUM_SQ(C6accel_Handle hC6accel,unsigned int n )
{
 FILE *refFd;
 float *inX1;
 float *out;
 int i;

/* Initialize Source and Output buffers to contiguous memory allocated   */
    inX1 = (float *)pSrcBuf_16bpp;
    out = (float *)pRefBuf_16bpp;


 //Test Vector sum function in C6accel
    /*Read inputs */
   if ((refFd = fopen(ref_vecsum_sq_xinput,"rb")) == NULL) {

        printf("Failed to open refFd on %s\n",ref_vecsum_sq_xinput);
         }
    fread(inX1, n, sizeof(float), refFd);
    fclose(refFd);


    /* Open reference output files */
    if ((refFd = fopen(ref_vecsum_sq_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_vecsum_sq_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00, n*sizeof(float));
    memset (pRefBuf_16bpp,0x00, n*sizeof(float));

    LOG_STRING("C6accel_DSPF_sp_vecsum_sq(),");
    START_BENCHMARK();

    C6accel_DSPF_sp_vecsum_sq(hC6accel,inX1,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_vecsum_sq() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp, n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp, n) != 1){
       printf("C6accel_DSPF_vecsum_sq() failed\n");
       return 0;
     }

    fclose(refFd);
    printf("Test for Floating point DSP vector reciprocal function successful\n");
    return 1;
}

/*
 * Test function for floating point weighted vector sum functions
*/
int c6accel_test_DSPF_W_VEC(C6accel_Handle hC6accel,float M, unsigned int n )
{
 FILE *refFd;
 float *inX1;
 float *inX2;
 float *out;
 int i;

/* Initialize Source and Output buffers to contiguous memory allocated   */
    inX1 = (float *)pSrcBuf_16bpp;
    out = (float *)pRefBuf_16bpp;
    inX2 = (float *)pWorkingBuf2_16bpp;


 //Test Vector sum function in C6accel
    /*Read inputs */
   if ((refFd = fopen(ref_w_vec_x1input,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_w_vec_x1input);
         }
    fread(inX1, n, sizeof(float), refFd);
    fclose(refFd);

    /*Read inputs */
   if ((refFd = fopen(ref_w_vec_x2input,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_w_vec_x2input);
         }
    fread(inX2, n, sizeof(float), refFd);
    fclose(refFd);

     /* Open reference output files */
    if ((refFd = fopen(ref_w_vec_rslt,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_w_vec_rslt);
       return (0);
     }

    /* Clear output arrays */
    memset (pOutBuf_16bpp,0x00, n*sizeof(float));
    memset (pRefBuf_16bpp,0x00, n*sizeof(float));

    LOG_STRING("C6accel_DSPF_sp_w_vec(),");
    START_BENCHMARK();

    C6accel_DSPF_sp_w_vec(hC6accel,inX1,inX2,M,(float *)pOutBuf_16bpp,n);
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_w_vec() wait for callback,");


         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference*/
    fread(pRefBuf_16bpp, n, sizeof(float), refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit short level */
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp, n) != 1){
       printf("C6accel_DSPF_w_vec() failed\n");
       return 0;
     }

    fclose(refFd);
    printf("Test for Floating point DSP vector weighted sum function successful\n");
    return 1;
}

int c6accel_test_DSPF_sp_fftSPxSP(C6accel_Handle hC6accel,int n,int n_min,int offset, int n_max)
{
    FILE  *refFd,*refFd1;
    float *w;
    float *inX;
    unsigned char *brev;
    int    i;

    /*Test C6accel_DSPF_sp_fftSPxSP() */
    memset (pSrcBuf_16bpp,0xA5,2*n*sizeof(float));
    memset (pWorkingBuf_16bpp,0xA5,2*n*sizeof(float));
    memset (pWorkingBuf2_16bpp,0xA5,2*n*sizeof(float));

    /*Generate the 16 bit fixed version of sine */
    inX = (float *)pSrcBuf_16bpp;
    if ((refFd1 = fopen(ref_fftSPxSP_xinput,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_fftSPxSP_xinput);
       return (0);
       }

    fread(inX, sizeof(float),2*n, refFd1);
    fclose(refFd1);

    /* Read in the twiddle table, use pWorkingBuf_16bpp for the table */
    w = (float*)pWorkingBuf_16bpp;

    if ((refFd1 = fopen(ref_fftSPxSP_twiddle,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_fftSPxSP_twiddle);
       return (0);
       }
    fread(w, sizeof(float),2*n, refFd1);
    fclose(refFd1);

    /* Read in the twiddle table, use pWorkingBuf_16bpp for the table */
    brev = (unsigned char*)pWorkingBuf2_16bpp;

    if ((refFd1 = fopen(ref_fftSPxSP_brev,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_fftSPxSP_brev);
       return (0);
       }
    fread(brev,  sizeof(unsigned char),64, refFd1);
    fclose(refFd1);


    if ((refFd = fopen(ref_fftSPxSP_rslt,"rb")) == NULL){
       printf("Failed to open refFd on %s\n",ref_fftSPxSP_rslt);
       return (0);
       }

    /* Clear output arrays*/
    memset (pOutBuf_16bpp,0xA5,2*n*sizeof(float));
    memset (pRefBuf_16bpp,0xA5,2*n*sizeof(float));

    LOG_STRING("C6accel_DSPF_sp_fftSPxSP(),");
    START_BENCHMARK();

    /* Call the DSPF_sp_fftSPxSP function in C6Accel*/
    C6accel_DSPF_sp_fftSPxSP (hC6accel,n,(float *)inX,(float*)w,(float*)pOutBuf_16bpp,(unsigned char*)brev,n_min, offset, n_max);

    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_fftSPxSP() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference from test file*/
    fread(pRefBuf_16bpp,  sizeof(float),2*n, refFd);
    LOG_STRING("\n");

    /* Validate output between native DSP/ref and C6Accel on 16 bit float level */
    if (comparefloatArrays((float*)pOutBuf_16bpp, (float*)pRefBuf_16bpp,2*n) != 1)
         {printf("C6accel_DSPF_sp_fftSPxSP() failed\n");
          return 0;
         }

     fclose(refFd);
    memset (pOutBuf_16bpp,0xA5,2*n*sizeof(float));
    memset (pRefBuf_16bpp,0xA5,2*n*sizeof(float)); 
    memset (pSrcBuf_16bpp,0xA5,2*n*sizeof(float));
    memset (pWorkingBuf_16bpp,0xA5,2*n*sizeof(float));
    memset (pWorkingBuf2_16bpp,0xA5,2*n*sizeof(float));
    printf("Test for Floating point DSP FFT function successful\n");
    return 1;
}

int c6accel_test_DSPF_sp_ifftSPxSP(C6accel_Handle hC6accel,int n,int n_min,int offset, int n_max)
{
    FILE  *refFd,*refFd1;
    float *w;
    float *inX;
    unsigned char *brev;
    int    i,status;

    /*Test C6accel_DSPF_sp_ifftSPxSP() */
    memset (pSrcBuf_16bpp,0xA5,2*n*sizeof(float));
    memset (pWorkingBuf_16bpp,0xA5,2*n*sizeof(float));
    memset (pWorkingBuf2_16bpp,0xA5,2*n*sizeof(float));

    /*Generate the 16 bit fixed version of sine */
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

    LOG_STRING("C6accel_DSPF_sp_ifftSPxSP(),");
    START_BENCHMARK();

    /* Call the DSPF_sp_ifftSPxSP function in C6Accel*/
    status= C6accel_DSPF_sp_ifftSPxSP (hC6accel,n,(float*)inX,(float*)w,(float*)pOutBuf_16bpp,(unsigned char*)brev,n_min, offset, n_max);
    
    END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_ifftSPxSP() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    /* read in reference from test file*/
    fread(pRefBuf_16bpp,  2*n, sizeof(float), refFd);
    LOG_STRING("\n");

    fclose(refFd);

    printf("Test for Floating point DSP Inverse FFT function successful\n");
    return 1;
}

/*
 * Test for BIQUAD filter generation kernels in c6accel
 */
int c6accel_test_DSPF_BIQUAD(C6accel_Handle hC6accel,unsigned int n)
{
    FILE *refFd;
    float *inX ;
    float *hb, *ha;
    float *delay;
    int i,status;
    float y2;
    /* Create a CMem heap array that can be translated for DSP */
    short *pdelay_Cmem;
    pdelay_Cmem = Memory_alloc(2*sizeof(float),&testfxnsMemParams);

    /*  Generate the 16 bit input  */
    inX = (float *)pSrcBuf_16bpp;
    ha = (float *)pWorkingBuf2_16bpp;
    hb =(float*)pWorkingBuf_16bpp;
    delay =(float*)pdelay_Cmem;

     /*Read inputs */
    if ((refFd = fopen(ref_biquad_xinput,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_biquad_xinput);
         }
    fread(inX, n, sizeof(float), refFd);
    fclose(refFd);

    if ((refFd = fopen(ref_biquad_ha,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_biquad_ha);
         }
    fread(ha, 3, sizeof(float), refFd);
    fclose(refFd);

    if ((refFd = fopen(ref_biquad_hb,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_biquad_hb);
         }
    fread(hb, 3, sizeof(float), refFd);
    fclose(refFd);

    if ((refFd = fopen(ref_biquad_delay,"rb")) == NULL) {
        printf("Failed to open refFd on %s\n",ref_biquad_delay);
         }
    fread( delay, 2, sizeof(float), refFd);
    fclose(refFd);


    if ((refFd = fopen(ref_biquad_y,"rb")) == NULL) {
       printf("Failed to open refFd on %s\n",ref_biquad_y);
       return (0);
      }

    /*  Clear output arrays */
    memset (pOutBuf_16bpp,0x00, n*sizeof(float));
    memset (pRefBuf_16bpp,0x00, n*sizeof(float));

    LOG_STRING("C6accel_DSPF_sp_biquad(),");
    START_BENCHMARK();

    /* Call BIQUAD kernel in C6Accel */
   status = C6accel_DSPF_sp_biquad(hC6accel,inX,hb,ha,delay,(float *)pOutBuf_16bpp,n);
   
   END_AND_WRITE_BENCHMARK();
    if (C6Accel_readCallType(hC6accel) == ASYNC){
       // Now wait for the callback
         LOG_STRING(" ,C6accel_DSPF_sp_biquad() wait for callback,");
         START_BENCHMARK();
         C6accel_waitAsyncCall(hC6accel);
         END_AND_WRITE_BENCHMARK();
    }
    fread(pRefBuf_16bpp, 1,n*sizeof(float), refFd);

    LOG_STRING("\n");

   /* Validate output between native DSP/ref and C6Accel on float level*/
    if (comparefloatArrays((float *)pOutBuf_16bpp, (float *)pRefBuf_16bpp,n) != 1){
       printf("C6accel_DSPF_sp_biquad() failed\n");
       return 0;
     }

     fclose(refFd);

     printf("Test for Floating point DSP BIQUAD Filter successful\n");
     return (1);
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

/* VLIB_API tests*/

void Create32BitPackedBinImage(unsigned char *pInLuma,
                                unsigned short  inCols,
                                unsigned short  inRows,
                                unsigned int *pOutPackedBin,
                                unsigned char  threshold)
{
  unsigned char shift;
  unsigned char bin;
  unsigned int word;
  unsigned int pos;
  unsigned int i, j;

  // initialize variables
  shift      = 0;
  word       = 0;
  pos        = 0;
  // loop through all luma pixels
  for(i = 0; i < inRows; i++)
     for(j = 0; j < inCols; j++)
        {
            if (pInLuma[inCols*i+j] < threshold)
                bin = 0;
            else
                bin = 1;

            word |= (bin << (31-shift));

            if (shift == 31) 
            {
                pOutPackedBin[pos++] = word;
                word                 = 0;
                shift                = 0;
            }
            else
                shift++;
        }
}

/* Function to reverse bits of num */
unsigned int reverseBits(unsigned int num) {     
unsigned int  NO_OF_BITS = sizeof(num) * 8;     
unsigned int reverse_num = 0, i, temp;     
  for (i = 0; i < NO_OF_BITS; i++)     {   
      temp = (num & (1 << i));        
       if(temp)             
           reverse_num |= (1 << ((NO_OF_BITS - 1) - i));   
    }       
  return reverse_num; 
}


void Unpack32BitPackedBinImage(unsigned int *pPackedBin,
                                unsigned short  img_w,
                                unsigned short  img_h,
                                unsigned char *pUnpackedBin)
{
    unsigned int i,j;

    // bit reversal so that the following code correctly displays naturally ordered data (assumed by VLIB)
    for (i=0; i<img_w*img_h/32; i++)
	{
        pPackedBin[i] = reverseBits(pPackedBin[i]);
	}


	// unpack the bit-packed image
    for(i = 0; i < img_h; i++)
            for(j = 0; j < img_w/8; j++)
           	{
				if(*((unsigned char *)pPackedBin + img_w/8*i + j) & 0x00000001)
                  *(pUnpackedBin + img_w*i + 8*j + 0)     = 1;
				else
                  *(pUnpackedBin + img_w*i + 8*j + 0)     = 0;

				if(*((unsigned char *)pPackedBin + img_w/8*i + j) & 0x00000002)
                  *(pUnpackedBin + img_w*i + 8*j + 1)     = 1;
				else
                  *(pUnpackedBin + img_w*i + 8*j + 1)     = 0;

				if(*((unsigned char *)pPackedBin + img_w/8*i + j) & 0x00000004)
                  *(pUnpackedBin + img_w*i + 8*j + 2)     = 1;
				else
                  *(pUnpackedBin + img_w*i + 8*j + 2)     = 0;

				if(*((unsigned char *)pPackedBin + img_w/8*i + j) & 0x00000008)
                  *(pUnpackedBin + img_w*i + 8*j + 3)     = 1;
				else
                  *(pUnpackedBin + img_w*i + 8*j + 3)     = 0;

				if(*((unsigned char *)pPackedBin + img_w/8*i + j) & 0x00000010)
                  *(pUnpackedBin + img_w*i + 8*j + 4)     = 1;
				else
                  *(pUnpackedBin + img_w*i + 8*j + 4)     = 0;

				if(*((unsigned char *)pPackedBin + img_w/8*i + j) & 0x00000020)
                  *(pUnpackedBin + img_w*i + 8*j + 5)     = 1;
				else
                  *(pUnpackedBin + img_w*i + 8*j + 5)     = 0;

				if(*((unsigned char *)pPackedBin + img_w/8*i + j) & 0x00000040)
                  *(pUnpackedBin + img_w*i + 8*j + 6)     = 1;
				else
                  *(pUnpackedBin + img_w*i + 8*j + 6)     = 0;

				if(*((unsigned char *)pPackedBin + img_w/8*i + j) & 0x00000080)
                  *(pUnpackedBin + img_w*i + 8*j + 7)     = 1;
				else
                  *(pUnpackedBin + img_w*i + 8*j + 7)     = 0;
            }
    // again bit reversal to return the input data to original form
    for (i=0; i<img_w*img_h/32; i++)
	{
        pPackedBin[i] = reverseBits(pPackedBin[i]);
	}

}

unsigned char C6accel_VLIB_testDilationMask(C6accel_Handle hC6accel)
{

  unsigned int i,j;
  unsigned char Success;
  unsigned char *ptr_DilatedPackedData,*ptr_ErodedPackedData,*ptr_Mask;
  unsigned int *ptr_PackedData;
  unsigned char ComputedDilatedData[32*64],ComputedErodedData[32*64];
  int status;
  ptr_PackedData=  (unsigned int *)pSrcBuf_16bpp;
  ptr_DilatedPackedData = (unsigned char *)pOutBuf_16bpp;
  ptr_Mask = (unsigned char *)pWorkingBuf2_16bpp;
  
  memcpy( ptr_Mask,TestMask,9);
  
  // threshold and pack data
  Create32BitPackedBinImage(ErosionData, 64, 32, ptr_PackedData, 1);

  LOG_STRING("C6accel_VLIB_dilate_bin_mask(),");
  START_BENCHMARK();
  status=C6accel_VLIB_dilate_bin_mask(hC6accel,(const unsigned char *) ptr_PackedData , (unsigned char *) ptr_DilatedPackedData+ 64/8, ptr_Mask, (64*30), 64);
  END_AND_WRITE_BENCHMARK();

  LOG_STRING("\n");

  // unpack computed result
  Unpack32BitPackedBinImage((unsigned int *)ptr_DilatedPackedData, 64, 32, ComputedDilatedData );  

  // compare result with reference result
  Success = 1;
  for(i = 1; i < 31; i++)
  {
      for(j = 0; j < 62; j++)
      {
                  //printf("Reference: %x Computed:%x\n",ReferenceDilatedDataSquare[64*i + j],ComputedDilatedData[64*i + j]);
		  if (ReferenceDilatedDataSquare[64*i + j] != ComputedDilatedData[64*i + j])   // assuming TestMask = ones(3,3)
		      Success = 0;
      }
  }
  if (Success == 1)
      printf("VLIB_dilate_bin_mask passed the test! \n");
  else
      printf("VLIB_dilate_bin_mask failed the test! \n");
  
  LOG_STRING("C version VLIB_dilate_bin_mask(),");
  START_BENCHMARK();
     VLIB_dilate_bin_mask((const unsigned char *) ptr_PackedData , (unsigned char *) ptr_DilatedPackedData+ 64/8, ptr_Mask, (64*30), 64); 
  END_AND_WRITE_BENCHMARK();
  LOG_STRING("\n");

  // threshold and pack data
  Create32BitPackedBinImage(ErosionData, 64, 32, ptr_PackedData, 1);

  LOG_STRING("C6accel_VLIB_dilate_bin_mask(),");
  START_BENCHMARK();
  status=C6accel_VLIB_dilate_bin_square(hC6accel,(const unsigned char *) ptr_PackedData , (unsigned char *) ptr_DilatedPackedData+ 64/8, (64*30), 64);
  END_AND_WRITE_BENCHMARK();

  LOG_STRING("\n");

  // unpack computed result
  Unpack32BitPackedBinImage((unsigned int *)ptr_DilatedPackedData, 64, 32, ComputedDilatedData );  

  // compare result with reference result
  Success = 1;
  for(i = 1; i < 31; i++)
  {
      for(j = 0; j < 62; j++)
      {
                  //printf("Reference: %x Computed:%x\n",ReferenceDilatedDataSquare[64*i + j],ComputedDilatedData[64*i + j]);
		  if (ReferenceDilatedDataSquare[64*i + j] != ComputedDilatedData[64*i + j])   // assuming TestMask = ones(3,3)
		      Success = 0;
      }
  }
  if (Success == 1)
      printf("VLIB_dilate_bin_square passed the test! \n");
  else
      printf("VLIB_dilate_bin_square failed the test! \n");


  // threshold and pack data
  Create32BitPackedBinImage(ErosionData, 64, 32, ptr_PackedData, 1);

  LOG_STRING("C6accel_VLIB_dilate_bin_mask(),");
  START_BENCHMARK();
  status=C6accel_VLIB_dilate_bin_cross(hC6accel,(const unsigned char *) ptr_PackedData , (unsigned char *) ptr_DilatedPackedData+ 64/8, (64*30), 64);
  END_AND_WRITE_BENCHMARK();

  LOG_STRING("\n");

  // unpack computed result
  Unpack32BitPackedBinImage((unsigned int *)ptr_DilatedPackedData, 64, 32, ComputedDilatedData );  

  // compare result with reference result
  Success = 1;
  for(i = 1; i < 31; i++)
  {
      for(j = 0; j < 62; j++)
      {
                  //printf("Reference: %x Computed:%x\n",ReferenceDilatedDataSquare[64*i + j],ComputedDilatedData[64*i + j]);
		  if (ReferenceDilatedDataCross[64*i + j] != ComputedDilatedData[64*i + j])   // assuming TestMask = ones(3,3)
		      Success = 0;
      }
  }
  if (Success == 1)
      printf("VLIB_dilate_bin_cross passed the test! \n");
  else
      printf("VLIB_dilate_bin_cross failed the test! \n");

  // threshold and pack data
  Create32BitPackedBinImage(ErosionData, 64, 32, ptr_PackedData, 1);
  ptr_ErodedPackedData = (unsigned char *)pOutBuf_16bpp;
  LOG_STRING("C6accel_VLIB_erode_bin_mask(),");
  START_BENCHMARK();
  status=C6accel_VLIB_erode_bin_mask(hC6accel,(const unsigned char *) ptr_PackedData , (unsigned char *) ptr_ErodedPackedData+ 64/8, ptr_Mask, (64*30), 64);
  END_AND_WRITE_BENCHMARK();

  LOG_STRING("\n");

  // unpack computed result
  Unpack32BitPackedBinImage((unsigned int *)ptr_ErodedPackedData, 64, 32, ComputedErodedData );  

  // compare result with reference result
  Success = 1;
  for(i = 1; i < 31; i++)
  {
      for(j = 0; j < 62; j++)
      {
                  //printf("Reference: %x Computed:%x\n",ReferenceDilatedDataSquare[64*i + j],ComputedDilatedData[64*i + j]);
		  if (ReferenceErodedDataSquare[64*i + j] != ComputedErodedData[64*i + j])   // assuming TestMask = ones(3,3)
		      Success = 0;
      }
  }
  if (Success == 1)
      printf("VLIB_erode_bin_mask passed the test! \n");
  else
      printf("VLIB_erode_bin_mask failed the test! \n");
  LOG_STRING("C version VLIB_dilate_bin_mask(),");
  START_BENCHMARK();
     VLIB_erode_bin_mask((const unsigned char *) ptr_PackedData , (unsigned char *) ptr_DilatedPackedData+ 64/8, ptr_Mask, (64*30), 64); 
  END_AND_WRITE_BENCHMARK();
  LOG_STRING("\n");

  // threshold and pack data
  Create32BitPackedBinImage(ErosionData, 64, 32, ptr_PackedData, 1);
  ptr_ErodedPackedData = (unsigned char *)pOutBuf_16bpp;
  LOG_STRING("C6accel_VLIB_erode_bin_mask(),");
  START_BENCHMARK();
  status=C6accel_VLIB_erode_bin_square(hC6accel,(const unsigned char *) ptr_PackedData , (unsigned char *) ptr_ErodedPackedData+ 64/8, (64*30), 64);
  END_AND_WRITE_BENCHMARK();

  LOG_STRING("\n");

  // unpack computed result
  Unpack32BitPackedBinImage((unsigned int *)ptr_ErodedPackedData, 64, 32, ComputedErodedData );  

  // compare result with reference result
  Success = 1;
  for(i = 1; i < 31; i++)
  {
      for(j = 0; j < 62; j++)
      {
                  //printf("Reference: %x Computed:%x\n",ReferenceDilatedDataSquare[64*i + j],ComputedDilatedData[64*i + j]);
		  if (ReferenceErodedDataSquare[64*i + j] != ComputedErodedData[64*i + j])   // assuming TestMask = ones(3,3)
		      Success = 0;
      }
  }
  if (Success == 1)
      printf("VLIB_erode_bin_square passed the test! \n");
  else
      printf("VLIB_erode_bin_square failed the test! \n");

  // threshold and pack data
  Create32BitPackedBinImage(ErosionData, 64, 32, ptr_PackedData, 1);
  ptr_ErodedPackedData = (unsigned char *)pOutBuf_16bpp;
  LOG_STRING("C6accel_VLIB_erode_bin_cross(),");
  START_BENCHMARK();
  status=C6accel_VLIB_erode_bin_cross(hC6accel,(const unsigned char *) ptr_PackedData , (unsigned char *) ptr_ErodedPackedData+ 64/8, (64*30), 64);
  END_AND_WRITE_BENCHMARK();

  LOG_STRING("\n");

  // unpack computed result
  Unpack32BitPackedBinImage((unsigned int *)ptr_ErodedPackedData, 64, 32, ComputedErodedData );  

  // compare result with reference result
  Success = 1;
  for(i = 1; i < 31; i++)
  {
      for(j = 0; j < 62; j++)
      {
                  //printf("Reference: %x Computed:%x\n",ReferenceDilatedDataSquare[64*i + j],ComputedDilatedData[64*i + j]);
		  if (ReferenceErodedDataCross[64*i + j] != ComputedErodedData[64*i + j])   // assuming TestMask = ones(3,3)
                      Success = 0;
      }
  }
  if (Success == 1)
      printf("VLIB_erode_bin_cross passed the test! \n");
  else
      printf("VLIB_erode_bin_cross failed the test! \n");

  return Success;
}

void VLIB_erode_bin_mask 
(
    const unsigned char * in_data, 
    unsigned char       * out_data, 
    const char          * mask, 
    int cols,
    int pitch
)
{
    int i;
    int64_t p0l, p3l, p6l;
    unsigned p0, p1, p2, p3, p4, p5, p6, p7, p8, r;

    unsigned in00, in01, in10, in11, in20, in21; // BK 
    unsigned * in;                               // BK


    /* -------------------------------------------------------------------- */
    /*  Iterate over the input, processing 32 pixels per iteration.         */
    /* -------------------------------------------------------------------- */
    for (i = 0; i < cols/8; i += 4)
    {
        /* ---------------------------------------------------------------- */
        /*  Load in our 34-bit by 3-bit context for applying the 3x3 mask.  */
        /* ---------------------------------------------------------------- */
        in = (unsigned *) (in_data + i);      // BK

		in00 = reverseBits(*in);                    // BK 
        in01 = reverseBits(*(in+1));                // BK 
		in10 = reverseBits(*(in + pitch/32));         // BK 
        in11 = reverseBits(*(in + pitch/32 + 1));     // BK 
		in20 = reverseBits(*(in + pitch/16));         // BK 
        in21 = reverseBits(*(in + pitch/16 + 1));     // BK 

        p0l = (in00) | ((( int64_t)in01 << 32)); // BK
        p3l = (in10) | ((( int64_t)in11 << 32)); // BK
        p6l = (in20) | ((( int64_t)in21 << 32)); // BK

        /* ---------------------------------------------------------------- */
        /*  Generate 3 offset copies of each row so that we can perform     */
        /*  ANDs between pixels that are neighbors.                         */
        /* ---------------------------------------------------------------- */
        p0 = p0l;   p1 = p0l >> 1;   p2 = p0l >> 2;
        p3 = p3l;   p4 = p3l >> 1;   p5 = p3l >> 2;
        p6 = p6l;   p7 = p6l >> 1;   p8 = p6l >> 2;

        /* ---------------------------------------------------------------- */
        /*  Now sum the filtered pixels together by ORing.                  */
        /* ---------------------------------------------------------------- */
        r = ~0;
        if (mask[0] >= 0) r &= p0;
        if (mask[1] >= 0) r &= p1;
        if (mask[2] >= 0) r &= p2;
        if (mask[3] >= 0) r &= p3;
        if (mask[4] >= 0) r &= p4;
        if (mask[5] >= 0) r &= p5;
        if (mask[6] >= 0) r &= p6;
        if (mask[7] >= 0) r &= p7;
        if (mask[8] >= 0) r &= p8;

        /* ---------------------------------------------------------------- */
        /*  Write the result as four bytes.                                 */
        /* ---------------------------------------------------------------- */
		*((unsigned *) (out_data + i)) = reverseBits(r); // BK 
    }
}

void VLIB_dilate_bin_mask 
(
    const unsigned char * in_data, 
    unsigned char       * out_data, 
    const char          * mask, 
    int cols,
    int pitch
)
{
    int i;
    int64_t p0l, p3l, p6l;
    unsigned p0, p1, p2, p3, p4, p5, p6, p7, p8, r;

	unsigned in00, in01, in10, in11, in20, in21; // BK 
    unsigned * in;                               // BK


    /* -------------------------------------------------------------------- */
    /*  Iterate over the input, processing 32 pixels per iteration.         */
    /* -------------------------------------------------------------------- */
    for (i = 0; i < cols/8; i += 4)
    {
        /* ---------------------------------------------------------------- */
        /*  Load in our 34-bit by 3-bit context for applying the 3x3 mask.  */
        /* ---------------------------------------------------------------- */
        in = (unsigned *) (in_data + i);      // BK

		in00 = reverseBits(*in);                    // BK 
        in01 = reverseBits(*(in+1));                // BK 
		in10 = reverseBits(*(in + pitch/32));         // BK 
        in11 = reverseBits(*(in + pitch/32 + 1));     // BK 
		in20 = reverseBits(*(in + pitch/16));         // BK 
        in21 = reverseBits(*(in + pitch/16 + 1));     // BK 

        p0l = (in00) | ((( int64_t)in01 << 32)); // BK
        p3l = (in10) | ((( int64_t)in11 << 32)); // BK
        p6l = (in20) | ((( int64_t)in21 << 32)); // BK

        /* ---------------------------------------------------------------- */
        /*  Generate 3 offset copies of each row so that we can perform     */
        /*  ANDs between pixels that are neighbors.                         */
        /* ---------------------------------------------------------------- */
        p0 = p0l;   p1 = p0l >> 1;   p2 = p0l >> 2;
        p3 = p3l;   p4 = p3l >> 1;   p5 = p3l >> 2;
        p6 = p6l;   p7 = p6l >> 1;   p8 = p6l >> 2;

        /* ---------------------------------------------------------------- */
        /*  Now sum the filtered pixels together by ORing.                  */
        /* ---------------------------------------------------------------- */
        r = 0;
        if (mask[0] >= 0) r |= p0;
        if (mask[1] >= 0) r |= p1;
        if (mask[2] >= 0) r |= p2;
        if (mask[3] >= 0) r |= p3;
        if (mask[4] >= 0) r |= p4;
        if (mask[5] >= 0) r |= p5;
        if (mask[6] >= 0) r |= p6;
        if (mask[7] >= 0) r |= p7;
        if (mask[8] >= 0) r |= p8;

        /* ---------------------------------------------------------------- */
        /*  Write the result as four bytes.                                 */
        /* ---------------------------------------------------------------- */
		*((unsigned *) (out_data + i)) = reverseBits(r); // BK 
    }
}


// Background Model Constants
#define IIR_ALPHA_S32         0x00001000    // SQ0.31
#define IIR_ALPHA_S16         0x10       	// SQ0.15
#define CAMERA_NOISE_S32      0x00A00000    // SQ16.15
#define CAMERA_NOISE_S16      0x0A00     	// SQ12.3
#define THRESHOLD_FACTOR_S32  0x31fffff9    // SQ4.27
#define THRESHOLD_FACTOR_S16  0x31ff        // SQ4.11


unsigned char C6accel_VLIB_testEWRMeanS16(C6accel_Handle hC6accel)
{
int i, errorCount=0;
short result1,result2;

//Test vectors
int trueMean[32] = {0 << 7, 1 << 7, 2 << 7, 3 << 7, 4 << 7, 5 << 7, 6 << 7, 7 << 7, 
						  8 << 7, 9 << 7, 10 << 7, 11 << 7, 12 << 7, 13 << 7, 14 << 7, 15 << 7, 
						  16 << 7, 17 << 7, 18 << 7, 19 << 7, 20 << 7, 21 << 7, 22 << 7, 23 << 7, 
						  24 << 7, 25 << 7, 26 << 7, 27 << 7, 28 << 7, 29 << 7, 30 << 7, 31 << 7};

unsigned char temp[32] = {0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 16, 16, 16, 16, 16, 16, 
					  32, 32, 32, 32, 32, 32, 32, 32, 64, 64, 64, 64, 64, 64};


short trueRunningMean[32] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
								 0x0800, 0x0800, 0x0800, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 0x07FF, 
								 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000,
								 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x2000, 0x0000, 0x0000};
short *runningMean = (short *)pOutBuf_16bpp;
unsigned char * luma;
unsigned int *mask32packed;
short bgAdaptRate     = IIR_ALPHA_S16;
luma= (unsigned char*)Memory_alloc( 32, &testfxnsMemParams);
mask32packed = (unsigned int*)Memory_alloc( sizeof(int), &testfxnsMemParams);

mask32packed[0]= 0x03E0FFFF;

for(i=0;i<32;i++)
   luma[i]= i;

C6accel_VLIB_initMeanWithLumaS16(hC6accel, runningMean, luma, 32);


// extract the mask bits one by one	and compare
	for (i=0; i<32; i++) {

		result1 = runningMean[i];
		result2 = trueMean[i];

		if (result1 != result2) {
			printf("Error: the initialized mean value for pixel no. %d is incorrect!\n", i);
			errorCount++;
			}
		}

	if (errorCount)
    	printf("Total of %d errors: VLIB_initMeanWithLumaS16 failed the test!\n", errorCount);
	else
	    printf("VLIB_initMeanWithLumaS16 passed the test!\n");


   for(i=0;i<32;i++)
       luma[i]= temp[i];

	C6accel_VLIB_initMeanWithLumaS16(hC6accel, runningMean, luma, 32);

   for(i=0;i<32;i++)
       luma[i]= i;

        // call the function
	C6accel_VLIB_updateEWRMeanS16(hC6accel,
                                      runningMean,	 		// running mean
			              luma, 				// newest luma frame
				      mask32packed,     		// current foreground mask
			   	      bgAdaptRate,		 	// old-new mixing ratio
			   	      32);      			// no. of pixels to be processed

        // verify the results
	errorCount = 0;

	// extract the mask bits one by one	and compare
	for (i=0; i<32; i++) {
        	result1 = runningMean[i];
	        result2 = trueRunningMean[i];
		if (result1 != result2) {
			printf("Error: the running mean value for pixel no. %d is incorrect!\n", i);
                        printf("runningMean= %d ; TrueMean = %d\n",runningMean[i],trueRunningMean[i]);
			errorCount++;
		}
	}

	if (errorCount)
    	printf("Total of %d errors: VLIB_updateEWRMeanS16 failed the test!\n", errorCount);
	else
	    printf("VLIB_updateEWRMeanS16 passed the test!\n");

 /*  Release Gaussian CMEM */
 Memory_free(luma,32, &testfxnsMemParams);
 Memory_free(mask32packed,sizeof(int), &testfxnsMemParams);


return 1;
}


unsigned char C6accel_VLIB_testEWRMeanS32(C6accel_Handle hC6accel)
{
int i, errorCount=0;
int result1,result2;

//Test vectors
int trueMean[32] = {0 << 23, 1 << 23, 2 << 23, 3 << 23, 4 << 23, 5 << 23, 6 << 23, 7 << 23, 
						  8 << 23, 9 << 23, 10 << 23, 11 << 23, 12 << 23, 13 << 23, 14 << 23, 15 << 23, 
						  16 << 23, 17 << 23, 18 << 23, 19 << 23, 20 << 23, 21 << 23, 22 << 23, 23 << 23, 
						  24 << 23, 25 << 23, 26 << 23, 27 << 23, 28 << 23, 29 << 23, 30 << 23, 31 << 23};

unsigned char temp[32] = {0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 16, 16, 16, 16, 16, 16, 
					  32, 32, 32, 32, 32, 32, 32, 32, 64, 64, 64, 64, 64, 64};


int trueRunningMean[32] = { 0x00000000, 0x00000010, 0x00000020, 0x00000030, 
	 		    				   0x00000040, 0x00000050, 0x00000000, 0x00000000, 
							   0x08000000, 0x08000000, 0x08000000, 0x07FFFFAF,
							   0x07FFFFBF, 0x07FFFFCF, 0x07FFFFDF, 0x07FFFFEF,
							   0x10000000, 0x10000000, 0x10000000, 0x10000000,
							   0x10000000, 0x10000000, 0x10000000, 0x10000000,
							   0x20000000, 0x20000000, 0x20000000, 0x20000000,
						   	   0x20000000, 0x20000000, 0x00000000, 0x00000000};

int *runningMean = (int *)pOutBuf_16bpp;
unsigned char * luma;
unsigned int *mask32packed;
int bgAdaptRate     = IIR_ALPHA_S32;
luma= (unsigned char*)Memory_alloc( 32, &testfxnsMemParams);
mask32packed = (unsigned int*)Memory_alloc( sizeof(int), &testfxnsMemParams);

mask32packed[0]= 0x03E0FFFF;

for(i=0;i<32;i++)
   luma[i]= i;

  C6accel_VLIB_initMeanWithLumaS32(hC6accel, runningMean, luma, 32);


// extract the mask bits one by one	and compare
	for (i=0; i<32; i++) {

		result1 = runningMean[i];
		result2 = trueMean[i];

		if (result1 != result2) {
			printf("Error: the initialized mean value for pixel no. %d is incorrect!\n", i);
			errorCount++;
			}
	 }

	if (errorCount)
    	printf("Total of %d errors: VLIB_initMeanWithLumaS32 failed the test!\n", errorCount);
	else
	    printf("VLIB_initMeanWithLumaS32 passed the test!\n");


   for(i=0;i<32;i++)
       luma[i]= temp[i];

	C6accel_VLIB_initMeanWithLumaS32(hC6accel, runningMean, luma, 32);

   for(i=0;i<32;i++)
       luma[i]= i;

        // call the function
	C6accel_VLIB_updateEWRMeanS32(hC6accel,
                                      runningMean,	 		// running mean
			              luma, 				// newest luma frame
				      mask32packed,     		// current foreground mask
			   	      bgAdaptRate,		 	// old-new mixing ratio
			   	      32);      			// no. of pixels to be processed

        // verify the results
	errorCount = 0;

	// extract the mask bits one by one	and compare
	for (i=0; i<32; i++) {
        	result1 = runningMean[i];
	        result2 = trueRunningMean[i];
		if (result1 != result2) {
			printf("Error: the running mean value for pixel no. %d is incorrect!\n", i);
                        printf("runningMean= %x ; TrueMean = %x\n",runningMean[i],trueRunningMean[i]);
			errorCount++;
		}
	}

	if (errorCount)

    	printf("Total of %d errors: VLIB_updateEWRMeanS32 failed the test!\n", errorCount);
	else
	    printf("VLIB_updateEWRMeanS32 passed the test!\n");

 /*  Release Gaussian CMEM */
 Memory_free(luma,32, &testfxnsMemParams);
 Memory_free(mask32packed,sizeof(int), &testfxnsMemParams);

return 1;
}


// Background Model Constants
#define CAMERA_NOISE_S32      0x00A00000    // SQ16.15
#define CAMERA_NOISE_S16      0x0A00     	// SQ12.3
#define THRESHOLD_FACTOR_S32  0x31fffff9    // SQ4.27
#define THRESHOLD_FACTOR_S16  0x31ff        // SQ4.11

unsigned char C6accel_VLIB_testEWRVarianceS32(C6accel_Handle hC6accel)
{
int i, errorCount=0;
int result1,result2;
int constVar  = 0x0A000000;


//Test vectors
int trueVar[32] = {0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000,
					   0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000,
					   0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000,
					   0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000, 0x0A000000};


// test input vectors
	unsigned char newLuma[32]   = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 
	                      17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

	unsigned char temp[32]      = {200, 200, 200, 200, 200, 200, 200, 200, 16, 16, 16, 16, 16, 16, 16, 16, 
			 		       32, 32, 32, 32, 32, 32, 32, 32, 64, 64, 64, 64, 64, 64};



int trueRunningVar[32] = {0x000509C3, 0x000509AA, 0x00050991, 0x00050978,
							  0x00050960, 0x00050947, 0x00050000, 0x00050000,
							  0x00050000, 0x00050000, 0x00050000, 0x00050000,
							  0x00050000, 0x0004FFFF, 0x0004FFFF, 0x0004FFFF,
							  0x00050000, 0x00050000, 0x00050000, 0x00050000,
							  0x00050000, 0x00050000, 0x00050000, 0x00050000,
							  0x00050000, 0x00050000, 0x00050000, 0x00050000,
							  0x00050000, 0x00050000, 0x00050000, 0x00050000};

int *runningVar = (int *)pOutBuf_16bpp;
int *runningMean = (int *)pRefBuf_16bpp;

unsigned char * luma;
unsigned int *mask32packed;
int bgAdaptRate     = IIR_ALPHA_S32;
luma= (unsigned char*)Memory_alloc( 32, &testfxnsMemParams);
mask32packed = (unsigned int*)Memory_alloc( sizeof(int), &testfxnsMemParams);
int thresholdGlobal = CAMERA_NOISE_S32/32;

mask32packed[0]= 0x03E0FFFF;


  C6accel_VLIB_initVarWithConstS32(hC6accel, runningVar, constVar, 32);


// extract the mask bits one by one	and compare
	for (i=0; i<32; i++) {

		result1 = runningVar[i];
		result2 = trueVar[i];

		if (result1 != result2) {
			printf("Error: the initialized mean value for pixel no. %d is incorrect!\n", i);
			errorCount++;
			}
	 }

	if (errorCount)
    	printf("Total of %d errors: VLIB_initVarWithConstS32 failed the test!\n", errorCount);
	else
	    printf("VLIB_initVarWithConstS32 passed the test!\n");


   for(i=0;i<32;i++)
       luma[i]= temp[i];

	C6accel_VLIB_initMeanWithLumaS32(hC6accel, runningMean, luma, 32);

        C6accel_VLIB_initVarWithConstS32(hC6accel, runningVar, thresholdGlobal, 32);
   for(i=0;i<32;i++)
       luma[i]= i;

        // call the function
	C6accel_VLIB_updateEWRVarianceS32(hC6accel,
                                      runningVar,
                                      runningMean,	 		// running mean
			              luma, 				// newest luma frame
				      mask32packed,     		// current foreground mask
			   	      bgAdaptRate,		 	// old-new mixing ratio
			   	      32);      			// no. of pixels to be processed

        // verify the results
	errorCount = 0;

	// extract the mask bits one by one	and compare
	for (i=0; i<32; i++) {
        	result1 = runningVar[i];
	        result2 = trueRunningVar[i];

		if (result1 != result2) {
			printf("Error: the running mean value for pixel no. %d is incorrect!\n", i);
                        printf("runningVar= %x ; TrueVar = %x\n",runningVar[i],trueRunningVar[i]);
			errorCount++;
		}
	}

	if (errorCount)

    	printf("Total of %d errors: VLIB_updateEWRMeanS32 failed the test!\n", errorCount);
	else
	    printf("VLIB_updateEWRVarianceS32 passed the test!\n");

        /*  Release Gaussian CMEM */
 Memory_free(luma,32, &testfxnsMemParams);
 Memory_free(mask32packed,sizeof(int), &testfxnsMemParams);


return 1;
}

unsigned char C6accel_VLIB_testEWRVarianceS16(C6accel_Handle hC6accel)
{
int i, errorCount=0;
int result1,result2;
short constVar  = 0x0A00;


//Test vectors
short trueVar[32] = {0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00,
						 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00,
					 	 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00,
						 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00, 0x0A00};



// test input vectors
	unsigned char newLuma[32]   = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 
	                      17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

	unsigned char temp[32]      = {200, 200, 200, 200, 200, 200, 200, 200, 16, 16, 16, 16, 16, 16, 16, 16, 
		 		       32, 32, 32, 32, 32, 32, 32, 32, 64, 64, 64, 64, 64, 64};

short trueRunningVar[32] = {0x0063, 0x0063, 0x0063, 0x0062, 0x0062, 0x0062, 0x0050, 0x0050,
								0x0050, 0x0050, 0x0050, 0x0050, 0x004F, 0x004F, 0x004F, 0x004F,
								0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 0x0050,
								0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 0x0050};


short *runningVar = (short *)pOutBuf_16bpp;
short *runningMean = (short *)pRefBuf_16bpp;
int pixelcount =32;
unsigned char * luma;
unsigned int *mask32packed;
short bgAdaptRate     = IIR_ALPHA_S16;
luma= (unsigned char*)Memory_alloc( pixelcount, &testfxnsMemParams);
mask32packed = (unsigned int*)Memory_alloc( (pixelcount*sizeof(int))/32, &testfxnsMemParams);
short thresholdGlobal = CAMERA_NOISE_S16/32;

mask32packed[0]= 0x03E0FFFF;


  C6accel_VLIB_initVarWithConstS16(hC6accel, runningVar, constVar, pixelcount);


  // extract the mask bits one by one	and compare
    for (i=0; i<pixelcount; i++) {

		result1 = runningVar[i];
		result2 = trueVar[i];

		if (result1 != result2) {
			printf("Error: the initialized mean value for pixel no. %d is incorrect!\n", i);
			errorCount++;
			}
	 }

	if (errorCount)
            printf("Total of %d errors: VLIB_initVarWithConstS16 failed the test!\n", errorCount);
	else
	    printf("VLIB_initVarWithConstS16 passed the test!\n");
      

   for(i=0;i<32;i++)
       luma[i]= temp[i];

	C6accel_VLIB_initMeanWithLumaS16(hC6accel, runningMean, luma, pixelcount);

        C6accel_VLIB_initVarWithConstS16(hC6accel, runningVar, thresholdGlobal, pixelcount);

   for(i=0;i<32;i++)
       luma[i]= i;

        // call the function
	C6accel_VLIB_updateEWRVarianceS16(hC6accel,
                                      runningVar,
                                      runningMean,	 		// running mean
			              luma, 				// newest luma frame
				      mask32packed,     		// current foreground mask
			   	      bgAdaptRate,		 	// old-new mixing ratio
			   	      pixelcount);      			// no. of pixels to be processed

        // verify the results
	errorCount = 0;

	// extract the mask bits one by one	and compare
	for (i=0; i<pixelcount; i++) {
        	result1 = runningVar[i];
	        result2 = trueRunningVar[i];

		if (result1 != result2) {
			printf("Error: the running mean value for pixel no. %d is incorrect!\n", i);
                        printf("runningVar= %x ; TrueVar = %x\n",runningVar[i],trueRunningVar[i]);
			errorCount++;
		}
	}

	if (errorCount)

    	printf("Total of %d errors: VLIB_updateEWRMeanS16 failed the test!\n", errorCount);
	else
	    printf("VLIB_updateEWRVarianceS16 passed the test!\n");
        
         /*  Release Gaussian CMEM */
 Memory_free(luma,32, &testfxnsMemParams);
 Memory_free(mask32packed,sizeof(int), &testfxnsMemParams);


return 1;
}


unsigned char C6accel_VLIB_testUWRMeanS16(C6accel_Handle hC6accel)
{
int i, errorCount=0;
short result1,result2;

// test input vectors
unsigned char newestLuma[32]   = {0, 11, 22, 33, 44, 55, 66, 77, 88, 99, 10, 11, 12, 13, 14, 15, 16, 
	            	          17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

unsigned char oldestLuma[32]   = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 
	            	          170, 180, 190, 200, 210, 220, 230, 240, 250, 6, 7, 8, 9, 0, 1};

unsigned char temp[32] = {0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 16, 16, 16, 16, 16, 16, 
					  32, 32, 32, 32, 32, 32, 32, 32, 64, 64, 64, 64, 64, 64};


short trueUpdatedMean[32] = {0x0000, 0x000E, 0x001C, 0x0000, 0x0000, 0x0046, 0x0000, 0x0000,
								 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800,
								 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000,
								 0x2000, 0x2000, 0x2118, 0x2000, 0x2000, 0x2118, 0x01A4, 0x01A4};

short *previousMean = (short *)pOutBuf_16bpp;
short *updatedMean = (short *)pRefBuf_16bpp;
unsigned char * luma;
unsigned int *oldestmask32packed, *newestmask32packed;
short bgAdaptRate     = IIR_ALPHA_S16;
luma= (unsigned char*)Memory_alloc( 32, &testfxnsMemParams);
oldestmask32packed = (unsigned int*)Memory_alloc( sizeof(int), &testfxnsMemParams);
newestmask32packed = (unsigned int*)Memory_alloc( sizeof(int), &testfxnsMemParams);
 
oldestmask32packed[0] = (unsigned int)0x03E0FFFF;
newestmask32packed[0] = (unsigned int)0x1840F00F;

   for(i=0;i<32;i++)
       luma[i]= temp[i];

	C6accel_VLIB_initMeanWithLumaS16(hC6accel, previousMean, luma, 32);

   for(i=0;i<32;i++)
       luma[i]= i;

        // call the function
	C6accel_VLIB_updateUWRMeanS16(hC6accel,
                                      updatedMean,
                                      previousMean,
                                      newestLuma,
                                      oldestLuma,
                                      newestmask32packed,     		
			   	      oldestmask32packed,		 	
			   	      32,10);      			

        // verify the results
	errorCount = 0;

	// extract the mask bits one by one	and compare
	for (i=0; i<32; i++) {
        	result1 = updatedMean[i];
	        result2 = trueUpdatedMean[i];
		if (result1 != result2) {
			printf("Error: the running mean value for pixel no. %d is incorrect!\n", i);
                        printf("runningMean= %d ; TrueMean = %d\n",updatedMean[i],trueUpdatedMean[i]);
			errorCount++;
		}
	}

	if (errorCount)
    	printf("Total of %d errors: VLIB_updateUWRMeanS16 failed the test!\n", errorCount);
	else
	    printf("VLIB_updateUWRMeanS16 passed the test!\n");

        /*  Release Gaussian CMEM */
        Memory_free(luma,32, &testfxnsMemParams);
        Memory_free(oldestmask32packed,sizeof(int), &testfxnsMemParams);
        Memory_free(newestmask32packed,sizeof(int), &testfxnsMemParams);


return 1;
}

unsigned char C6accel_VLIB_testUWRVarianceS16(C6accel_Handle hC6accel)
{
int i, errorCount=0;
short result1,result2;

// test input vectors
unsigned char temp[32] = {0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 16, 16, 16, 16, 16, 16, 
					  32, 32, 32, 32, 32, 32, 32, 32, 64, 64, 64, 64, 64, 64};

short trueUpdatedVar[32] = {0x0046, 0x0046, 0x0049, 0x0050, 0x0050, 0x0059, 0x0050, 0x0050, 
								0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 
								0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 0x0050, 
								0x0050, 0x0050, 0x057E, 0x0050, 0x0050, 0x04B8, 0x0303, 0x0335};

short trueUpdatedMean[32] = {0x0000, 0x000E, 0x001C, 0x0000, 0x0000, 0x0046, 0x0000, 0x0000,
								 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800,
								 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000,
								 0x2000, 0x2000, 0x2118, 0x2000, 0x2000, 0x2118, 0x01A4, 0x01A4};

short *previousMean = (short *)pOutBuf_16bpp;
short *updatedMean = (short *)pRefBuf_16bpp;
short *previousVar = (short *)pWorkingBuf_16bpp;
short *updatedVar = (short *)pWorkingBuf2_16bpp;
unsigned char * luma;
unsigned int *oldestmask32packed, *newestmask32packed;
short bgAdaptRate     = IIR_ALPHA_S16;
luma= (unsigned char*)Memory_alloc( 32, &testfxnsMemParams);
oldestmask32packed = (unsigned int*)Memory_alloc( sizeof(int), &testfxnsMemParams);
newestmask32packed = (unsigned int*)Memory_alloc( sizeof(int), &testfxnsMemParams);
short thresholdGlobal = CAMERA_NOISE_S16/32;

oldestmask32packed[0] = (unsigned int)0x03E0FFFF;
newestmask32packed[0] = (unsigned int)0x1840F00F;

   for(i=0;i<32;i++)
       luma[i]= temp[i];

	C6accel_VLIB_initMeanWithLumaS16(hC6accel, previousMean, luma, 32);
        
        // initialize the running variance "image"
	C6accel_VLIB_initVarWithConstS16(hC6accel,previousVar, thresholdGlobal, 32);


   for(i=0;i<32;i++)
       luma[i]= i;

        // call the function
	C6accel_VLIB_updateUWRVarianceS16(hC6accel,
                                      updatedVar,
                                      previousVar,
                                      updatedMean,
                                      previousMean,
                                      luma,
                                      newestmask32packed,     		
			   	      oldestmask32packed,		 	
			   	      32,10);      			

        // verify the results
	errorCount = 0;
        

	// extract the mask bits one by one	and compare
	for (i=0; i<32; i++) {
        	result1 = updatedVar[i];
	        result2 = trueUpdatedVar[i];
		if (result1 != result2) {
			printf("Error: the running mean value for pixel no. %d is incorrect!\n", i);
                        printf("runningVar= %d ; TrueVar = %d\n",updatedVar[i],trueUpdatedVar[i]);
			errorCount++;
		}
	}

	if (errorCount)
    	printf("Total of %d errors: VLIB_updateUWRVarianceS16 failed the test!\n", errorCount);
	else
	    printf("VLIB_updateUWRVarianceS16 passed the test!\n");

        /*  Release Gaussian CMEM */
 Memory_free(luma,32, &testfxnsMemParams);
 Memory_free(oldestmask32packed,sizeof(int), &testfxnsMemParams);
 Memory_free(newestmask32packed,sizeof(int), &testfxnsMemParams);


return 1;
}


int C6accel_VLIB_testUnpackMask32(C6accel_Handle hC6accel) {
        int pixelcount =64;
	// test input vectors
	unsigned char trueMaskImage[64] = {0,0,0,0,   1,1,1,1,   0,0,0,0,   1,0,1,0,   0,0,0,0,   0,1,0,1,   0,0,0,0,   1,1,1,1, 
                                       1,1,1,1,   0,0,0,0,   1,0,1,0,   0,0,0,0,   0,1,0,1,   0,0,0,0,   1,1,1,1,   0,0,0,0};

	unsigned int *mask32packed =  (unsigned int*)pRefBuf_16bpp;
        mask32packed[0]=0x0F0A050F; mask32packed[1]=0xF0A050F0;
	
	unsigned char *maskImage= pOutBuf_16bpp;;

	int i, errorCount;
	unsigned char result1, result2;

	// call the function
	C6accel_VLIB_unpackMask32(hC6accel,mask32packed, maskImage, 64);

	// verify the results
	errorCount = 0;

	// extract the mask bits one by one	and compare
	for (i=0; i<64; i++) {

		result1 = maskImage[i];
		result2 = trueMaskImage[i];

		if (result1 != result2) {
			printf("Error: the extracted binary mask value for pixel no. %d is incorrect!\n", i);
			errorCount++;
			}
		}

	if (errorCount)
    	printf("Total of %d errors: VLIB_unpackMask32 failed the test!\n", errorCount);
	else
	    printf("VLIB_unpackMask32 passed the test!\n");

return 1;
}

int C6accel_VLIB_testpackMask32(C6accel_Handle hC6accel) {
        int pixelcount =64;
	// test input vectors
	unsigned char trueMaskImage[64] = {0,0,0,0,   1,1,1,1,   0,0,0,0,   1,0,1,0,   0,0,0,0,   0,1,0,1,   0,0,0,0,   1,1,1,1, 
                                       1,1,1,1,   0,0,0,0,   1,0,1,0,   0,0,0,0,   0,1,0,1,   0,0,0,0,   1,1,1,1,   0,0,0,0};

	unsigned int *mask32packed =  (unsigned int*)pRefBuf_16bpp;
        unsigned int trueMask32packed[2] = {0x0F0A050F, 0xF0A050F0};
	
	unsigned char *maskImage= pOutBuf_16bpp;;

	int i, errorCount;
	unsigned char result1, result2;

	// call the function
	C6accel_VLIB_packMask32(hC6accel, maskImage,mask32packed, 64);

        // verify the results
	errorCount = 0;

	// extract the mask bits one by one	and compare
	for (i=0; i<2; i++) {

		result1 = mask32packed[i];
		result2 = trueMask32packed[i];

		if (result1 != result2) {
			printf("Error: the extracted binary mask value for pixel no. %d is incorrect!\n", i);
			errorCount++;
			}
		}

	if (errorCount)
    	printf("Total of %d errors: VLIB_unpackMask32 failed the test!\n", errorCount);
	else
	    printf("VLIB_packMask32 passed the test!\n");

return 1;
}

unsigned char C6accel_VLIB_testsubtractBackgroundS16(C6accel_Handle hC6accel)
{
int i, errorCount=0;
short result1,result2;
int pixelCount =32;
// test input vectors
unsigned char temp[32] = {0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 16, 16, 16, 16, 16, 16, 
					  32, 32, 32, 32, 32, 32, 32, 32, 64, 64, 64, 64, 64, 64};

short *runningMean = (short *)pOutBuf_16bpp;
short *runningVar = (short *)pRefBuf_16bpp;
unsigned char * luma;
unsigned int *mask32packed;
luma= (unsigned char*)Memory_alloc( 32, &testfxnsMemParams);
mask32packed = (unsigned int*)Memory_alloc(pixelCount*sizeof(int)/32, &testfxnsMemParams);

short thresholdGlobal = CAMERA_NOISE_S16/32;
short thresholdFactor = THRESHOLD_FACTOR_S16/2;

// expected output vector
unsigned int trueMask32packed = 0x0000FEFF;


   for(i=0;i<32;i++)
       luma[i]= temp[i];

	C6accel_VLIB_initMeanWithLumaS16(hC6accel, runningMean, luma, 32);
        
        // initialize the running variance "image"
	C6accel_VLIB_initVarWithConstS16(hC6accel, runningVar, thresholdGlobal, 32);


   for(i=0;i<32;i++)
       luma[i]= i;

        // call the function
	C6accel_VLIB_subtractBackgroundS16(hC6accel,
                                      mask32packed,
                                      luma,
                                      runningMean,
                                      runningVar,
                                      thresholdGlobal,
                                      thresholdFactor,     		
			   	      32);     			

        // verify the results
	errorCount = 0;
        

	// extract the mask bits one by one	and compare
	for (i=0; i<32; i++) {
        	result1 = (mask32packed[0] >> (31-i)) & 1;
                result2 = (trueMask32packed >> (31-i)) & 1;

		if (result1 != result2) {
			printf("Error: the running mean value for pixel no. %d is incorrect!\n", i);
                        errorCount++;
		}
	}

	if (errorCount)
    	printf("Total of %d errors: VLIB_subtractBackgroundS16 failed the test!\n", errorCount);
	else
	    printf("VLIB_subtractBackGroundS16 passed the test!\n");

        /*  Release Gaussian CMEM */
        Memory_free(luma,32, &testfxnsMemParams);
        Memory_free(mask32packed,sizeof(int), &testfxnsMemParams);
 


return 1;
}

unsigned char C6accel_VLIB_testsubtractBackgroundS32(C6accel_Handle hC6accel)
{
int i, errorCount=0;
int result1,result2;
int pixelCount =32;
// test input vectors
unsigned char temp[32] = {0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 16, 16, 16, 16, 16, 16, 
					  32, 32, 32, 32, 32, 32, 32, 32, 64, 64, 64, 64, 64, 64};

int *runningMean = (int *)pOutBuf_16bpp;
int *runningVar = (int *)pRefBuf_16bpp;
unsigned char * luma;
unsigned int *mask32packed;
luma= (unsigned char*)Memory_alloc( 32, &testfxnsMemParams);
mask32packed = (unsigned int*)Memory_alloc(pixelCount*sizeof(int)/32, &testfxnsMemParams);
mask32packed[0] = 0x0;
int thresholdGlobal = CAMERA_NOISE_S32/32;
int thresholdFactor = THRESHOLD_FACTOR_S32/2;

// expected output vector
unsigned int trueMask32packed = 0x03E0FFFF;


   for(i=0;i<32;i++)
       luma[i]= temp[i];

	C6accel_VLIB_initMeanWithLumaS32(hC6accel, runningMean, luma, 32);
        
        // initialize the running variance "image"
	C6accel_VLIB_initVarWithConstS32(hC6accel, runningVar, thresholdGlobal, 32);

    for(i=0;i<32;i++)
       luma[i]= i;

        // call the function
	C6accel_VLIB_subtractBackgroundS32(hC6accel,
                                      mask32packed,
                                      luma,
                                      runningMean,
                                      runningVar,
                                      thresholdGlobal,
                                      thresholdFactor,     		
			   	      32);     			

        // verify the results
	errorCount = 0;
        
	// extract the mask bits one by one	and compare
	for (i=0; i<32; i++) {
        	result1 = (mask32packed[0] >> (31-i)) & 1;
                result2 = (trueMask32packed >> (31-i)) & 1;

		if (result1 != result2) {
			printf("Error: the backround subtract mask value for pixel no. %d is incorrect!\n", i);
                        errorCount++;
		}
	}

	if (errorCount)
    	printf("Total of %d errors: VLIB_subtractBackgroundS32 failed the test!\n", errorCount);
	else
	    printf("VLIB_subtractBackGroundS32 passed the test!\n");

        /*  Release Gaussian CMEM */
        Memory_free(luma,32, &testfxnsMemParams);
        Memory_free(mask32packed,sizeof(int), &testfxnsMemParams);
 


return 1;
}

int C6accel_VLIB_testCannyEdgeDetector(C6accel_Handle hC6accel,unsigned char LevelOfFeedback)
{
	int i, j;
        int *numItems=(int  *)Memory_alloc(1*sizeof(int), &testfxnsMemParams);;

	

        unsigned char *ptr_gTITexasLogo = (unsigned char *)pSrcBuf_16bpp;
        //unsigned char gTITexasLogoCannyRef[4096];
        short  *pBufGradX= (short  *)pRefBuf_16bpp;
        short  *pBufGradY = (short  *)pWorkingBuf_16bpp;
        short  *pBufMag = (short  *)pWorkingBuf2_16bpp;
        unsigned char *pBufOut = (unsigned char *)pOutBuf_16bpp;
        char *ptr_gaussian_7x7 = (char *)Memory_alloc(49*sizeof(char), &testfxnsMemParams);

        unsigned char * pIn = ptr_gTITexasLogo;
	short * restrict pGx = pBufGradX;
	short * restrict pGy = pBufGradY;
	short * restrict pMag = pBufMag;
        unsigned char *pOut = pBufOut;

	unsigned short errors  = 0;
	unsigned short width   = 64; 
	unsigned short height  = 64; 
	unsigned char  hiThres = 10; 
	unsigned char  loThres = 2;  

        unsigned short new_width;
        unsigned short new_height;

        unsigned int * listptr;

        unsigned short conv_window_size=7;
        
	// 3x3 Gaussian kernel with SQ0.8 coefficients
	char gaussian_7x7[49] = 
	{
		0, 0,  0,  0,  0, 0, 0,
		0, 0,  0,  0,  0, 0, 0,
		0, 0, 16, 32, 16, 0, 0,
		0, 0, 32, 64, 32, 0, 0,
		0, 0, 16, 32, 16, 0, 0,
		0, 0,  0,  0,  0, 0, 0,
		0, 0,  0,  0,  0, 0, 0
	};
        
       for(i=0;i<49;i++)
        ptr_gaussian_7x7[i]= gaussian_7x7[i];

        for(i=0;i<4096;i++)
        ptr_gTITexasLogo[i]= gTITexasLogo[i]; 

        /////////////////////////////////////////////////////////////////////////////
	//
	//   STEP 0) Allocate memory & Load sample 64x64 8-bit grayscale input image 
	//
	/////////////////////////////////////////////////////////////////////////////

	// initialize memory
	memset(pBufGradX, 0, width*height*sizeof(short));
	memset(pBufGradY, 0, width*height*sizeof(short));
	memset(pBufMag,   0, width*height*sizeof(short));
	memset(pBufOut,   0, width*height*sizeof(unsigned char));

        pIn    = ptr_gTITexasLogo;
        pOut = pBufOut + width*(conv_window_size-1)/2 + (conv_window_size-1)/2;  // 3 = (7-1)/2
	pGx  = pBufGradX + width*(conv_window_size-1)/2 + (conv_window_size-1)/2;
	pGy  = pBufGradY + width*(conv_window_size-1)/2 + (conv_window_size-1)/2;
	pMag = pBufMag   + width*(conv_window_size-1)/2 + (conv_window_size-1)/2;

        if(LevelOfFeedback>0){
	// Display image
	    printf("\nDisplay the original 8-bit input image:\n\n");
	    for (i=0; i < height; i++) {
		    for (j=0; j < width; j++)
			    printf("%3d", pIn[i*width +j]);
		    printf("\n");
     	      }
         }

        new_height = height-(conv_window_size-1);
        new_width = width -(conv_window_size-1);

	//Requires IMGLIB
	C6accel_IMG_conv_7x7_i8_c8s(hC6accel,pIn, 
		pBufOut,  
		width * (new_height) - 6, 
		width, 
		ptr_gaussian_7x7, 
		8);
    
        
    if(LevelOfFeedback>0)
    {
	// Display smoothed image
	printf("\nDisplay the smoothed 8-bit input image:\n\n");
	for (i=0; i < height; i++) {
		for (j=0; j < width; j++)
			printf("%3d", pBufOut[i*width +j]);
		printf("\n");
	}

    }
   
   pGx  +=(width + 1);
   pGy  +=(width + 1);
   pMag +=(width + 1);
   new_height -= 2;

	C6accel_VLIB_xyGradientsAndMagnitude(hC6accel,pBufOut, 
		pGx, 
		pGy, 
		pMag, 
		width, 
		new_height);
   
    if(LevelOfFeedback>0)
    {
	// Display gradient magnitude "image"
	printf("\nDisplay the 16-bit Gradient Magnitude image:\n\n");
	for (i=0; i < height; i++) {
		for (j=0; j < width; j++)
			printf("%3d", pBufMag[i*width +j]);
		printf("\n");
	}
    }

	for (i=0; i < width * 5; i++)
	{
		pBufOut[i] = 0;
		pBufOut[width * (height-5) + i] = 0;
	}

    memset(pBufOut,   0, width*height*sizeof(unsigned char));

    new_width -= 4;
    new_height -= 2;
    pGx  +=(width + 1);
    pGy  +=(width + 1);
    pOut +=(2*width + 2);

  	C6accel_VLIB_nonMaximumSuppressionCanny(hC6accel,pMag, 
		pGx, 
		pGy,
		pOut,
		new_width,
		width,
		new_height);
	//printf("Non Max Supp Press Enter\n");
    //    getchar();	
     if(LevelOfFeedback>0)
    {						
	// Display edge map after non-maximum suppresion
	printf("\nDisplay the edge map after Non-Maximum Suppression:\n\n");
	for (i=0; i < height; i++) {
		for (j=0; j < width; j++)
			printf("%3d", pBufOut[i*width +j]);
		printf("\n");
	}
    }

    listptr   = (unsigned int *) pRefBuf_16bpp;
    pMag +=(width + 1);
    
     C6accel_VLIB_doublethresholding(hC6accel,pMag, 
                     pOut, 
                     listptr,
                     numItems,
                     new_width,
                     width,                      
                     new_height,
                     loThres,
                     hiThres,
		     0
                     );
   
     C6accel_VLIB_edgeRelaxation(hC6accel,pOut, 
    		     listptr,
                     numItems,
                     width);
     

	/////////////////////////////////////////////////////////////////////////////
	// Clean-Up: Set all the remaining 'possible edges' to non-edges
	/////////////////////////////////////////////////////////////////////////////
	for (i=width*5; i < width*(height-10); i++) 
	{
		pBufOut[i] = (pBufOut[i] >> 7) * 255;
	}

      errors =0;
	// Display edge map after hysteresis
    if(LevelOfFeedback>0)
	printf("\nDisplay the final edge map after Hysteresis:\n\n");
	for (i=0; i < height; i++) {
		for (j=0; j < width; j++){
			if (pBufOut[i*width +j] == gTITexasLogoCannyRef[i*width +j]){
                             if(LevelOfFeedback>0)
				printf("%3d", pBufOut[i*width +j]);
                         }
			else{
                            if(LevelOfFeedback>0)
				printf("%3d#%d", pBufOut[i*width +j],gTITexasLogoCannyRef[i*width +j] );
                                errors++;
			}
		}
        if(LevelOfFeedback>0)
		printf("\n");
	}
        
    	if (errors < 3)
		printf("VLIB Canny functions Test results:\nIMG_conv_7x7: passed the test!\nVLIB_xyGradient: passed the test!\nVLIB_nonMaximumSuppressionCanny: passed the test!\nVLIB_doublethresholding: passed the test!\nVLIB_edgeRelaxation: passed the test!\n\n");
	else
		printf("Canny functions failed the test!\n");

        printf("Calling Chained Canny API:\n");
        memset(pBufOut,   0, width*height*sizeof(unsigned char));  
        memset(pWorkingBuf_16bpp,   0, width*height*sizeof(short)); 
        memset(pWorkingBuf2_16bpp,   0, width*height*sizeof(short)); 
        memset(pRefBuf_16bpp,   0, width*height*sizeof(unsigned char)); 
      
        C6accel_VLIB_cannyEdge( hC6accel,pIn,loThres,hiThres, (short *)pWorkingBuf_16bpp, (short *)pWorkingBuf2_16bpp, (short *)pRefBuf_16bpp, (unsigned char *)pBufOut, width,height);
     //  printf("Press Enter\n");
     //   getchar();
     // Display edge map 
       errors = 0;
      if(LevelOfFeedback>0)
	printf("\nDisplay the final edge map after Hysteresis:\n\n");
	for (i=0; i < height; i++) {
		for (j=0; j < width; j++){
			if (pBufOut[i*width +j] == gTITexasLogoCannyRef[i*width +j]){
                             if(LevelOfFeedback>0)
				printf("%3d", pBufOut[i*width +j]);
                         }
			else{
                            if(LevelOfFeedback>0)
                                printf("%3d", pBufOut[i*width +j]);
				//printf("%3d#%d", pBufOut[i*width +j],gTITexasLogoCannyRef[i*width +j] );
                                errors++;
			}
		}
        if(LevelOfFeedback>0)
		printf("\n");
	}
     
     //   printf("error= %d\n",errors);
    	if (errors < 3)
		printf("Chained Canny edge detection function passed the test!\n");
	else
		printf("Chained Canny edge detection function failed the test!\n");

       // Memory_free(ptr_gTITexasLogo,4096*sizeof(unsigned char), &testfxnsMemParams);
       // Memory_free(pBufGradX,4096*sizeof(short), &testfxnsMemParams);
       // Memory_free(pBufGradY, 4096*sizeof(short), &testfxnsMemParams);
       // Memory_free(pBufMag,4096*sizeof(short), &testfxnsMemParams);
       // Memory_free(pBufOut,4096*sizeof(unsigned char), &testfxnsMemParams);
        Memory_free(ptr_gaussian_7x7,49*sizeof(char), &testfxnsMemParams);
        Memory_free(numItems,1*sizeof(int), &testfxnsMemParams);
	return 1;
}

int C6accel_VLIB_testExtractLumaFromUYUV(C6accel_Handle hC6accel,unsigned char LevelOfFeedback) 
{

	// test input vectors - entered in UYVY format here so that it ends up in YUYV format in memory
	unsigned char YUV422Data[64] = {151, 57, 116, 170, 	9, 247, 208, 140, 	150, 60, 88, 77, 		4, 6, 162, 6, 
					31, 143, 178, 3, 	135, 91, 54, 154, 	193, 161, 20, 162, 		137, 150, 128, 224, 
					214, 113, 9, 28, 	53, 211, 98, 217, 	149, 233, 231, 127, 	        115, 203, 177, 42, 
					62, 155, 3, 103, 	127, 16, 135, 131, 	211, 158, 9, 2, 		106, 227, 249, 255};

	// expected output vector - 
	// take a look at the actual memory content to be convinced that this is the right answer
	unsigned char trueLumaData[32] = {57, 170,			247, 140,			60, 77, 				6, 6, 
					  143, 3, 			91, 154, 			161, 162, 				150, 224,
					  113, 28, 			211, 217,  			233, 127,  				203, 42, 
			          	  155, 103, 			16, 131, 			158, 2, 			        227, 255};
 
        unsigned char * pIn= (unsigned char *)pSrcBuf_16bpp; 
	unsigned char *pOut= (unsigned char *)pOutBuf_16bpp; //extractedLumaData

	// local variables
	int i, errorCount;
    	unsigned char result1, result2;
        
	// initialize I/O buffer	
	for (i=0; i<32; i++)
		pOutBuf_16bpp[i] = 0;

        // initialize I/O buffer	
	for (i=0; i<64; i++)
		pIn[i] = YUV422Data[i];

	// call the function
	C6accel_VLIB_extractLumaFromUYUV(hC6accel,pIn, 				// input YUV422 image
						   8,				// width of input image
						   8,				// pitch of input image
						   4,				// height of input image
						   pOut); 		        // luma-only output image

	printf("\n\nTesting the function: VLIB_extractLumaFromUYUV...\n");

	// verify the results
	errorCount = 0;

	// extract the mask bits one by one	and compare
	for (i=0; i<32; i++) {

		result1 = pOut[i];
		result2 = trueLumaData[i];

		if (result1 != result2) {
			printf("Error: the extracted luma for pixel no. %d is incorrect! calc: %d, true: %d\n", i, pOut[i],trueLumaData[i]);
			errorCount++;
			}
		}

	if (errorCount)
    	printf("Total of %d errors:C6accel test for VLIB_extractLumaFromUYUV failed the test!\n", errorCount);
	else
	    printf("C6accel test for VLIB_extractLumaFromUYUV passed the test!\n");

return 1;
}

int C6accel_VLIB_testConvertUYVYsemipl_to_YUVpl(C6accel_Handle hC6accel,unsigned char LevelOfFeedback)
{

	// test input vectors - entered in UYVY format here so that it ends up in YUYV format in memory
	unsigned char CrCbData[64] = {151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6,  31, 143, 178,   3, 135,  91,  54, 154, 193, 161,  20, 162, 137, 150, 128, 224, 
						  214, 113,   9,  28,  53, 211,  98, 217, 149, 233, 231, 127, 115, 203, 177,  42,  62, 155,   3, 103, 127,  16, 135, 131, 211, 158,   9,   2, 106, 227, 249, 255};

	// expected output vector - 
	// take a look at the actual memory content to be convinced that this is the right answer

	unsigned char trueCr[32] =     {151, 116,   9, 208, 150,  88,   4, 162,  31, 178, 135,  54, 193,  20, 137, 128,  
						    214,   9,  53,  98, 149, 231, 115, 177,  62,   3, 127, 135, 211,   9, 106, 249};
	unsigned char trueCb[32] =     { 57, 170, 247, 140,  60,  77,   6,   6, 143,   3,  91, 154, 161, 162, 150, 224, 
						    113,  28, 211, 217, 233, 127, 203,  42, 155, 103,  16, 131, 158,   2, 227, 255};

//	unsigned char trueCr[32] =     {151, 116,   9, 208, 150,  88,   4, 162, 214,   9,  53,  98, 149, 231, 115, 177};
//	unsigned char trueCb[32] =     { 57, 170, 247, 140,  60,  77,   6,   6, 113,  28, 211, 217, 233, 127, 203,  42};

        unsigned char *pInCrCb = (unsigned char *)pSrcBuf_16bpp; //32 elements
	unsigned char *extractedCr =(unsigned char *)pOutBuf_16bpp; //32 elements
	unsigned char *extractedCb =(unsigned char *)pRefBuf_16bpp; //32 elements

	// local variables
	int i, errorCount;
	unsigned char result1, result2;
        

        // initialize Input buffer	
	for (i=0; i<64; i++)
	{
		pInCrCb[i] = CrCbData[i];
	}

	// initialize Output buffer	
	for (i=0; i<32; i++)
	{
		extractedCr[i] = 0;
		extractedCb[i] = 0;
	}

	// call the function
	C6accel_VLIB_convertUYVYsemipl_to_YUVpl(hC6accel,pInCrCb, 				// input YUV422 image
						   32,						// width of input image
						   32,						// pitch of input image
						   2,						// height of input image
						   extractedCr, 		// Cr output image
						   extractedCb); 		// Cr output image

	printf("\n\nTesting the function: VLIB_convertUYVYsemipl_to_YUVpl...\n");

	// verify the results
	errorCount = 0;

	// extract the mask bits one by one	and compare
	for (i=0; i<32; i++) {
		result1 = extractedCr[i];
		result2 = trueCr[i];

		if (result1 != result2) {
			printf("Error: the extracted value for byte no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}

	for (i=0; i<32; i++) {
	// for (i=0; i<16; i++) {

		result1 = extractedCb[i];
		result2 = trueCb[i];

		if (result1 != result2) {
			printf("Error: the extracted value for byte no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}


	if (errorCount)
    	printf("Total of %d errors: VLIB_convertUYVYsemipl_to_YUVpl failed the test!\n", errorCount);
	else
	    printf("VLIB_convertUYVYsemipl_to_YUVpl passed the test!\n");

return 1;
}

int C6accel_VLIB_testConvertUYVYpl_to_YUVint(C6accel_Handle hC6accel,unsigned char LevelOfFeedback) 
{

	// test vectors - entered in UYVY format here so that it ends up in YUYV format in memory
	unsigned char trueYUV422Data[2*64] = {151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6,  31, 143, 178,   3, 135,  91,  54, 154, 193, 161,  20, 162, 137, 150, 128, 224, 
						    214, 113,   9,  28,  53, 211,  98, 217, 149, 233, 231, 127, 115, 203, 177,  42,  62, 155,   3, 103, 127,  16, 135, 131, 211, 158,   9,   2, 106, 227, 249, 255,
						    151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6,  31, 143, 178,   3, 135,  91,  54, 154, 193, 161,  20, 162, 137, 150, 128, 224, 
						    214, 113,   9,  28,  53, 211,  98, 217, 149, 233, 231, 127, 115, 203, 177,  42,  62, 155,   3, 103, 127,  16, 135, 131, 211, 158,   9,   2, 106, 227, 249, 255};

	// take a look at the actual memory content to be convinced that this is the right answer
	unsigned char LumaData[2*32] = 
                           {      57,      170,      247,      140,       60,       77,        6,        6,      143,        3,       91,      154,      161,      162,      150,      224, 
						         113,       28,      211,      217,      233,      127,      203,       42,      155,      103,       16,      131,      158,        2,      227,      255,
						          57,      170,      247,      140,       60,       77,        6,        6,      143,        3,       91,      154,      161,      162,      150,      224, 
						         113,       28,      211,      217,      233,      127,      203,       42,      155,      103,       16,      131,      158,        2,      227,      255};

	unsigned char CbData[32] =     {151,                  9,                150,                  4,                 31,                135,                193,                137,                
						    214,                 53,                149,                115,                 62,                127,                211,                106,
						    151,                  9,                150,                  4,                 31,                135,                193,                137,                
						    214,                 53,                149,                115,                 62,                127,                211,                106 };

	unsigned char CrData[32] =    {           116,                208,                 88,                162,                178,                 54,                 20,                128,      
						                9,                 98,                231,                177,                  3,                135,                  9,                249,
						              116,                208,                 88,                162,                178,                 54,                 20,                128,      
						                9,                 98,                231,                177,                  3,                135,                  9,                249};

	unsigned char *interleavedYc= (unsigned char *)pOutBuf_16bpp; //Size=[2*64];
        unsigned char *pInY = (unsigned char *)pSrcBuf_16bpp;
        unsigned char *pInCr = (unsigned char *)pWorkingBuf_16bpp;
        unsigned char *pInCb = (unsigned char *)pWorkingBuf2_16bpp;
     

	// local variables
	int i, errorCount;
	unsigned char result1, result2;

	// initialize I/O buffer	
	for (i=0; i<2*64; i++){
		interleavedYc[i] = 0;
        }

        for (i=0; i<32; i++){
              pInY[2*i] = LumaData[2*i];
              pInY[2*i+1]= LumaData[2*i+1];
              pInCr[i] = CrData[i];
              pInCb[i] = CbData[i];
          }            

	// call the function
	C6accel_VLIB_convertUYVYpl_to_YUVint(hC6accel, pInY, 		// luma input image
						   pInCr, 	               	// Cr input image
						   pInCb,   	        	// Cr input image
						   32,						// width of input image
						   32,						// pitch of input image
						   2,						// height of input image
						   interleavedYc);			// output YUV422 image
						   

	printf("\n\nTesting the function: VLIB_convertUYVYpl_to_YUVint...\n");

	// verify the results
	errorCount = 0;

	// extract the mask bits one by one	and compare
	for (i=0; i<2*64; i++) {


		result1 = interleavedYc[i];
		result2 = trueYUV422Data[i];

		if (result1 != result2) {
			printf("Error: the data at byte no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}


	if (errorCount)
    	printf("Total of %d errors: VLIB_convertUYVYpl_to_YUVint failed the test!\n", errorCount);
	else
	    printf("VLIB_convertUYVYpl_to_YUVint passed the test!\n");
        
        //Test YUVint to YUV planar
        for (i=0; i<32; i++){
              pInY[2*i] = 0;
              pInY[2*i+1]= 0;
              pInCr[i] = 0;
              pInCb[i] = 0;
          }  
        // call the function
	C6accel_VLIB_convertUYVYint_to_YUVpl(hC6accel,interleavedYc, 				// input YUV422 image
						   32,						// width of input image
						   32,						// pitch of input image
						   2,						// height of input image
						   pInY, 		// luma output image
						   pInCr, 		// Cr output image
						   pInCb); 		// Cr output image
         printf("\n\nTesting the function: VLIB_convertUYVYint_to_YUVpl...\n");

	// verify the results
	errorCount = 0;

	// extract the mask bits one by one	and compare
	for (i=0; i<64; i++) {

		result1 = pInY[i];
		result2 = LumaData[i];

		if (result1 != result2) {
			printf("Error: the data at byte no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
		}
	}
        for (i=0; i<32; i++) {

		result1 = pInCr[i];
		result2 = CrData[i];

		if (result1 != result2) {
			printf("Error: the data at byte no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
		}
	}
        for (i=0; i<32; i++) {

		result1 = pInCb[i];
		result2 = CbData[i];

		if (result1 != result2) {
			printf("Error: the data at byte no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
		}
	}
	if (errorCount)
    	printf("Total of %d errors: VLIB_convertUYVYint_to_YUVpl failed the test!\n", errorCount);
	else
	    printf("VLIB_convertUYVYint_to_YUVpl passed the test!\n");

return 1;
}

int C6accel_VLIB_testConvertUYVYint_to_YUV420pl(C6accel_Handle hC6accel,unsigned char LevelOfFeedback) 
{

	// test input vectors - entered in UYVY format here so that it ends up in YUYV format in memory
	unsigned char YUV422Data[64] = {151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6,  31, 143, 178,   3, 135,  91,  54, 154, 193, 161,  20, 162, 137, 150, 128, 224, 
						    214, 113,   9,  28,  53, 211,  98, 217, 149, 233, 231, 127, 115, 203, 177,  42,  62, 155,   3, 103, 127,  16, 135, 131, 211, 158,   9,   2, 106, 227, 249, 255};

	// expected output vector - 
	// take a look at the actual memory content to be convinced that this is the right answer
	unsigned char trueLumaData[32] = 
                           {      57,      170,      247,      140,       60,       77,        6,        6,      143,        3,       91,      154,      161,      162,      150,      224, 
						         113,       28,      211,      217,      233,      127,      203,       42,      155,      103,       16,      131,      158,        2,      227,      255};

	unsigned char trueCb[32] =     {183,                 31,                150,                 60,                 47,                131,                202,                122};
	                     // 151,                  9,                150,                  4,                 31,                135,                193,                137,                
						 // 214,                 53,                149,                115,                 62,                127,                211,                106 };

	unsigned char trueCr[32] =    {            63,                153,                160,                170,                 91,                 95,                 15,                189};
	                               // 116,                208,                 88,                162,                178,                 54,                 20,                128,      
						           //   9,                 98,                231,                177,                  3,                135,                  9,                249};

	
   
	// local variables
	int i, errorCount;
	unsigned char result1, result2;
        unsigned char *interleavedYc= (unsigned char *)pOutBuf_16bpp; //Size=[2*64];
        unsigned char *extractedLumaData = (unsigned char *)pSrcBuf_16bpp;
        unsigned char *extractedCr = (unsigned char *)pWorkingBuf_16bpp;
        unsigned char *extractedCb = (unsigned char *)pWorkingBuf2_16bpp;

	// initialize I/O buffer	
	for (i=0; i<32; i++){
		extractedLumaData[i] = 0;
                interleavedYc[2*i]=YUV422Data[2*i];
                interleavedYc[2*i+1]=YUV422Data[2*i+1];
         } 

	// call the function
	C6accel_VLIB_convertUYVYint_to_YUV420pl(hC6accel,interleavedYc, 				// input YUV422 image
						   16,						// width of input image
						   16,						// pitch of input image
						   2,						// height of input image
						   extractedLumaData, 		// luma output image
						   extractedCr, 		// Cr output image
						   extractedCb); 		// Cr output image

	printf("\n\nTesting the function: VLIB_ConvertUYVYint_to_YUV420pl...\n");

	// verify the results
	errorCount = 0;

	// extract the mask bits one by one	and compare
	for (i=0; i<32; i++) {

		result1 = extractedLumaData[i];
		result2 = trueLumaData[i];

		if (result1 != result2) {
			printf("Error: the extracted luma for pixel no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}

	for (i=0; i<8; i++) {

		result1 = extractedCr[i];
		result2 = trueCr[i];

		if (result1 != result2) {
			printf("Error: the extracted Cr no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}


	for (i=0; i<8; i++) {

		result1 = extractedCb[i];
		result2 = trueCb[i];

		if (result1 != result2) {
			printf("Error: the extracted Cb no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}

	if (errorCount)
    	printf("Total of %d errors: VLIB_ConvertUYVYint_to_YUV420pl failed the test!\n", errorCount);
	else
	    printf("VLIB_ConvertUYVYint_to_YUV420pl passed the test!\n");

return(errorCount);
}

int C6accel_VLIB_testConvertUYVYint_to_RGBpl(C6accel_Handle hC6accel,unsigned char LevelOfFeedback) 
{

	// test input vectors - entered in UYVY format here so that it ends up in YUYV format in memory
	unsigned char YUV422Data[64] = {151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6,  31, 143, 178,   3, 135,  91,  54, 154, 193, 161,  20, 162, 137, 150, 128, 224, 
						    214, 113,   9,  28,  53, 211,  98, 217, 149, 233, 231, 127, 115, 203, 177,  42,  62, 155,   3, 103, 127,  16, 135, 131, 211, 158,   9,   2, 106, 227, 249, 255};

	// expected output vector - 
	// take a look at the actual memory content to be convinced that this is the right answer
	unsigned char trueR[32] = { 24, 137, 255, 233, 0, 6, 36, 36, 195, 55, 0, 36, 0, 0, 134, 208, 0, 0, 153, 159, 255, 252, 254, 93, 0, 0, 9, 124, 0, 0, 255, 255};
	unsigned char trueG[32] = { 41, 154, 215, 108, 64, 81, 7, 7, 124, 0, 124, 187, 198, 199, 130, 204, 151, 66, 241, 247, 138, 32, 157, 0, 248, 196, 0, 110, 197, 41, 133, 161};
	unsigned char trueB[32] = { 80, 193, 24, 0, 82, 99, 0, 0, 0, 0, 87, 150, 255, 255, 149, 223, 245, 160, 65, 71, 253, 147, 164, 3, 24, 0, 0, 113, 255, 129, 172, 200};

	
        unsigned char *interleavedYc= (unsigned char *)pSrcBuf_16bpp; //Size=[2*64];
        unsigned char *extractedR = (unsigned char *)pOutBuf_16bpp;
        unsigned char *extractedG = (unsigned char *)pWorkingBuf_16bpp;
        unsigned char *extractedB = (unsigned char *)pWorkingBuf2_16bpp;
        short *coeff = (short *)Memory_alloc(5*sizeof(short), &testfxnsMemParams);
	short coeff_arr[] = { 0x2000, 0x2BDD, -0x0AC5, -0x1658, 0x3770 }; 
       
	// local variables
	int i, errorCount;
	unsigned char result1, result2;

	// initialize I/O buffer	
	for (i=0; i<32; i++)
	{
		extractedR[i] = 0;
		extractedG[i] = 0;
		extractedB[i] = 0;
                interleavedYc[2*i]=YUV422Data[2*i];
                interleavedYc[2*i+1]=YUV422Data[2*i+1];
        }
        for(i=0;i<5;i++)
            coeff[i]= coeff_arr[i];

	// call the function
	C6accel_VLIB_convertUYVYint_to_RGBpl(hC6accel,interleavedYc, 		// input YUV422 image
						   16,						// width of input image
						   16,						// pitch of input image
						   2,						// height of input image
						   coeff_arr, 
						   extractedR,
						   extractedG, 	
						   extractedB); 		

	printf("\n\nTesting the function: VLIB_convertUYVYint_to_RGBpl...\n");

	// verify the results
	errorCount = 0;

	for (i=0; i<32; i++) {

		result1 = extractedR[i];
		result2 = trueR[i];

		if (result1 != result2) {
			printf("Error: the extracted R no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}

	for (i=0; i<32; i++) {

		result1 = extractedG[i];
		result2 = trueG[i];

		if (result1 != result2) {
			printf("Error: the extracted G no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}

	for (i=0; i<32; i++) {

		result1 = extractedB[i];
		result2 = trueB[i];

		if (result1 != result2) {
			printf("Error: the extracted B no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}

        Memory_free(coeff, 5*sizeof(short), &testfxnsMemParams);
	if (errorCount)
    	printf("Total of %d errors: VLIB_convertUYVYint_to_RGBpl failed the test!\n", errorCount);
	else
	    printf("VLIB_convertUYVYint_to_RGBpl passed the test!\n");


return(errorCount);
}



int C6accel_VLIB_testConvertUYVYint_to_LABpl(C6accel_Handle hC6accel,unsigned char LevelOfFeedback) 
{

	// test input vectors - entered in UYVY format here so that it ends up in YUYV format in memory
	unsigned char YUV422Data[64] = {151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6,  31, 143, 178,   3, 135,  91,  54, 154, 193, 161,  20, 162, 137, 150, 128, 224, 
						    214, 113,   9,  28,  53, 211,  98, 217, 149, 233, 231, 127, 115, 203, 177,  42,  62, 155,   3, 103, 127,  16, 135, 131, 211, 158,   9,   2, 106, 227, 249, 255};


//	unsigned char trueR[32] = { 24, 137, 255, 233, 0, 6, 36, 36, 195, 55, 0, 36, 0, 0, 134, 208, 0, 0, 153, 159, 255, 252, 254, 93, 0, 0, 9, 124, 0, 0, 255, 255};
//	unsigned char trueG[32] = { 41, 154, 215, 108, 64, 81, 7, 7, 124, 0, 124, 187, 198, 199, 130, 204, 151, 66, 241, 247, 138, 32, 157, 0, 248, 196, 0, 110, 197, 41, 133, 161};
//	unsigned char trueB[32] = { 80, 193, 24, 0, 82, 99, 0, 0, 0, 0, 87, 150, 255, 255, 149, 223, 245, 160, 65, 71, 253, 147, 164, 3, 24, 0, 0, 113, 255, 129, 172, 200};

	// expected output vector - 
	// take a look at the actual memory content to be convinced that this is the right answer
    float trueL[32] = { 17.265034, 63.574043, 86.963959, 59.938698, 24.558899, 31.405392, 4.765028, 4.765028, 58.058479, 7.330656, 45.775894, 68.106667, 74.575233, 74.864967, 55.282509, 82.873566, 60.537140, 30.483486, 87.028114, 89.085182, 73.892006, 55.931030, 74.744598, 17.155056, 85.603165, 69.097565, 0.524044, 47.792004, 74.285027, 20.711773, 70.190643, 76.634476};
    float trueA[32] = { 7.589161, 3.009796, -1.693904, 44.134914, -10.802895, -13.673991, 12.377151, 12.377151, 20.145357, 26.917166, -38.985950, -46.491325, -21.300167, -21.813095, 5.399912, 4.924804, 0.970393, 21.399885, -52.812366, -52.864582, 59.851051, 82.322533, 36.903618, 38.394405, -83.941193, -70.691193, 2.366193, 6.026894, -20.784527, 25.984793, 50.326050, 39.896996};
    float trueB[32] = { -26.081396, -22.099293, 84.643967, 68.164642, -15.977430, -16.349543, 7.369337, 7.369337, 64.340759, 11.588656, 12.075769, 8.083177, -39.376877, -38.926254, -9.634387, -8.895719, -56.056023, -55.621086, 71.813759, 71.931549, -38.316608, -5.809820, 11.777973, 25.310669, 79.130211, 68.226204, 0.833717, 0.334072, -39.828537, -52.491127, 0.810838, -5.225980}; 


	//float *extractedL; //Size =[32];
	//float *extractedA; //Size =[32];
	//float *extractedB; //Size= [32];
        unsigned char *interleavedYc= (unsigned char *)pSrcBuf_16bpp; //Size=[2*64];
        float *extractedL = (float *)pOutBuf_16bpp;
        float *extractedA = (float *)pWorkingBuf_16bpp;
        float *extractedB = (float *)pWorkingBuf2_16bpp;
        float *pWhitePoint = (float *)Memory_alloc(3*sizeof(float), &testfxnsMemParams);
        short *coeff=(short *)Memory_alloc(5*sizeof(short), &testfxnsMemParams);
	short coeff_arr[] = { 0x2000, 0x2BDD, -0x0AC5, -0x1658, 0x3770 }; 
	float whitePoint[3] = {0.950456, 1.0, 1.088754}; // D65

	// local variables
	int i, errorCount;
	float result1, result2;
	int absdiff;

	// initialize I/O buffer	
	for (i=0; i<32; i++)
	{
		extractedL[i] = 0;
		extractedA[i] = 0;
		extractedB[i] = 0;
                interleavedYc[2*i]=YUV422Data[2*i];
                interleavedYc[2*i+1]=YUV422Data[2*i+1];
        }

        for(i=0; i<5;i++)
                coeff[i]=coeff_arr[i];

        for(i=0;i<3;i++)
                 pWhitePoint[i] = whitePoint[i];

	// call the function
	C6accel_VLIB_convertUYVYint_to_LABpl(hC6accel,interleavedYc, 		// input YUV422 image
						   16,						// width of input image
						   16,						// pitch of input image
						   2,						// height of input image
						   coeff, 
						   pWhitePoint,
						   extractedL,
						   extractedA, 	
						   extractedB); 		

	printf("\n\nTesting the function: VLIB_convertUYVYint_to_LABpl...\n");

	// verify the results
	errorCount = 0;

	for (i=0; i<32; i++) 
	{

		result1 = extractedL[i];
		result2 = trueL[i];
		absdiff = abs(result1 - result2)*1000;

		if ( absdiff > 0 ) 
		    {
			printf("Error: the extracted L for pixel no. %d is incorrect! %f \t %f \n", i, result1 , result2);

			errorCount++;

			}
	}

	for (i=0; i<32; i++) {

		result1 = extractedA[i];
		result2 = trueA[i];
		absdiff = abs(result1 - result2)*1000;

		if ( absdiff > 0 )  {
			printf("Error: the extracted A for pixel no. %d is incorrect! %f \t %f \n", i, result1, result2);
			errorCount++;
			}
		}

	for (i=0; i<32; i++) {

		result1 = extractedB[i];
		result2 = trueB[i];
		absdiff = abs(result1 - result2)*1000;

		if ( absdiff > 0 )  {
			printf("Error: the extracted B for pixel no. %d is incorrect! %f \t %f \n", i, result1, result2);
			errorCount++;
			}
		}
         Memory_free(coeff, 5*sizeof(short), &testfxnsMemParams);
         Memory_free(pWhitePoint, 3*sizeof(float), &testfxnsMemParams);

	if (errorCount)
    	printf("Total of %d errors: VLIB_convertUYVYint_to_LABpl failed the test!\n", errorCount);
	else
	    printf("VLIB_convertUYVYint_to_LABpl passed the test!\n");


return(errorCount);
}


const unsigned short div_table_DDR2[510] = {0, 32768, 16384, 10922, 8192, 6553, 5461, 4681, 4096, 3640, 3276, 2978, 2730, 2520, 2340, 2184, 
 2048, 1927, 1820, 1724, 1638, 1560, 1489, 1424, 1365, 1310, 1260, 1213, 1170, 1129, 1092, 1057, 
 1024, 992, 963, 936, 910, 885, 862, 840, 819, 799, 780, 762, 744, 728, 712, 697, 
 682, 668, 655, 642, 630, 618, 606, 595, 585, 574, 564, 555, 546, 537, 528, 520, 
 512, 504, 496, 489, 481, 474, 468, 461, 455, 448, 442, 436, 431, 425, 420, 414, 
 409, 404, 399, 394, 390, 385, 381, 376, 372, 368, 364, 360, 356, 352, 348, 344, 
 341, 337, 334, 330, 327, 324, 321, 318, 315, 312, 309, 306, 303, 300, 297, 295, 
 292, 289, 287, 284, 282, 280, 277, 275, 273, 270, 268, 266, 264, 262, 260, 258, 
 256, 254, 252, 250, 248, 246, 244, 242, 240, 239, 237, 235, 234, 232, 230, 229, 
 227, 225, 224, 222, 221, 219, 218, 217, 215, 214, 212, 211, 210, 208, 207, 206, 
 204, 203, 202, 201, 199, 198, 197, 196, 195, 193, 192, 191, 190, 189, 188, 187, 
 186, 185, 184, 183, 182, 181, 180, 179, 178, 177, 176, 175, 174, 173, 172, 171, 
 170, 169, 168, 168, 167, 166, 165, 164, 163, 163, 162, 161, 160, 159, 159, 158, 
 157, 156, 156, 155, 154, 153, 153, 152, 151, 151, 150, 149, 148, 148, 147, 146, 
 146, 145, 144, 144, 143, 143, 142, 141, 141, 140, 140, 139, 138, 138, 137, 137, 
 136, 135, 135, 134, 134, 133, 133, 132, 132, 131, 131, 130, 130, 129, 129, 128, 
 128, 127, 127, 126, 126, 125, 125, 124, 124, 123, 123, 122, 122, 121, 121, 120, 
 120, 120, 119, 119, 118, 118, 117, 117, 117, 116, 116, 115, 115, 114, 114, 114, 
 113, 113, 112, 112, 112, 111, 111, 111, 110, 110, 109, 109, 109, 108, 108, 108, 
 107, 107, 107, 106, 106, 106, 105, 105, 105, 104, 104, 104, 103, 103, 103, 102, 
 102, 102, 101, 101, 101, 100, 100, 100, 99, 99, 99, 98, 98, 98, 98, 97, 
 97, 97, 96, 96, 96, 96, 95, 95, 95, 94, 94, 94, 94, 93, 93, 93, 
 93, 92, 92, 92, 92, 91, 91, 91, 91, 90, 90, 90, 90, 89, 89, 89, 
 89, 88, 88, 88, 88, 87, 87, 87, 87, 86, 86, 86, 86, 86, 85, 85, 
 85, 85, 84, 84, 84, 84, 84, 83, 83, 83, 83, 82, 82, 82, 82, 82, 
 81, 81, 81, 81, 81, 80, 80, 80, 80, 80, 79, 79, 79, 79, 79, 78, 
 78, 78, 78, 78, 78, 77, 77, 77, 77, 77, 76, 76, 76, 76, 76, 76, 
 75, 75, 75, 75, 75, 74, 74, 74, 74, 74, 74, 73, 73, 73, 73, 73, 
 73, 72, 72, 72, 72, 72, 72, 72, 71, 71, 71, 71, 71, 71, 70, 70, 
 70, 70, 70, 70, 70, 69, 69, 69, 69, 69, 69, 68, 68, 68, 68, 68, 
 68, 68, 67, 67, 67, 67, 67, 67, 67, 67, 66, 66, 66, 66, 66, 66, 
 66, 65, 65, 65, 65, 65, 65, 65, 65, 64, 64, 64, 64, 64};


int C6accel_VLIB_testConvertUYVYint_to_HSLpl(C6accel_Handle hC6accel,unsigned char LevelOfFeedback)
{

	// test input vectors - entered in UYVY format here so that it ends up in YUYV format in memory
	unsigned char YUV422Data[64] = {151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6,  31, 143, 178,   3, 135,  91,  54, 154, 193, 161,  20, 162, 137, 150, 128, 224, 
						    214, 113,   9,  28,  53, 211,  98, 217, 149, 233, 231, 127, 115, 203, 177,  42,  62, 155,   3, 103, 127,  16, 135, 131, 211, 158,   9,   2, 106, 227, 249, 255};

	// expected output vector - 
	// take a look at the actual memory content to be convinced that this is the right answer
	unsigned short trueH[32] = { 222, 222, 49, 28, 193, 192, 12, 12, 38, 0, 162, 165, 194, 193, 253, 253, 203, 215, 90, 90, 301, 329, 356, 358, 126, 120, 0, 347, 194, 221, 341, 335};
	unsigned char trueS[32] = { 53, 31, 100, 100, 100, 88, 100, 100, 100, 100, 100, 67, 100, 100, 8, 22, 100, 100, 86, 91, 100, 97, 97, 100, 100, 100, 100, 5, 100, 100, 100, 100};
	unsigned char trueL[32] = { 20, 65, 55, 46, 16, 21, 7, 7, 38, 11, 24, 44, 50, 50, 55, 84, 48, 31, 60, 62, 77, 56, 81, 18, 49, 38, 2, 46, 50, 25, 76, 82};

	//short coeff[] = { 0x2000, 0x2BDD, -0x0AC5, -0x1658, 0x3770 };
        unsigned char *interleavedYc= (unsigned char *)pSrcBuf_16bpp; //Size=[64];
        unsigned short *extractedH = (short *)pOutBuf_16bpp;
        unsigned char *extractedS = (unsigned char *)pWorkingBuf_16bpp;
        unsigned char *extractedL = (unsigned char *)pWorkingBuf2_16bpp;
        unsigned short *div_table = (unsigned short *)pRefBuf_16bpp;
        
        short *coeff=(short *)Memory_alloc(5*sizeof(short), &testfxnsMemParams);
	short coeff_arr[] = { 0x2000, 0x2BDD, -0x0AC5, -0x1658, 0x3770 }; 
	

	// local variables
	int i, errorCount;
	unsigned char result1, result2;

	// initialize I/O buffer	
	for (i=0; i<32; i++){
		extractedH[i] = 0;
		extractedS[i] = 0;
		extractedL[i] = 0;
                interleavedYc[2*i]=YUV422Data[2*i];
                interleavedYc[2*i+1]=YUV422Data[2*i+1];
        }

        for (i=0;i<5;i++)
            coeff[i] = coeff_arr[i];

        for (i=0;i<510; i++)
          div_table[i] = div_table_DDR2[i];
       
	// call the function
	C6accel_VLIB_convertUYVYint_to_HSLpl(hC6accel,interleavedYc, 		// input YUV422 image
						   16,				// width of input image
						   16,				// pitch of input image
						   2,				// height of input image
                                                   coeff, 
						   div_table,
						   extractedH,
						   extractedS, 	
						   extractedL); 		

	printf("\n\nTesting the function: VLIB_convertUYVYint_to_HSLpl...\n");

	// verify the results
	errorCount = 0;

	for (i=0; i<32; i++) {

		result1 = extractedH[i];
		result2 = trueH[i];

		if (result1 != result2) {
			printf("Error: the extracted H luma for pixel no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}

	for (i=0; i<32; i++) {

		result1 = extractedS[i];
		result2 = trueS[i];

		if (result1 != result2) {
			printf("Error: the extracted S luma for pixel no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}

	for (i=0; i<32; i++) {

		result1 = extractedL[i];
		result2 = trueL[i];

		if (result1 != result2) {
			printf("Error: the extracted L luma for pixel no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}

        Memory_free(coeff, 5*sizeof(short), &testfxnsMemParams);
	if (errorCount)
    	printf("Total of %d errors: VLIB_convertUYVYint_to_HSLpl failed the test!\n", errorCount);
	else
	    printf("VLIB_convertUYVYint_to_HSLpl passed the test!\n");


return(errorCount);
}

unsigned short LabExt[39304/2];     // 4x 2 bytes (faster writing)

int C6accel_VLIB_testConvertUYVYint_to_LABpl_LUT(C6accel_Handle hC6accel,unsigned char LevelOfFeedback)
{
	// test input vectors - entered in UYVY format here so that it ends up in YUYV format in memory
	// unsigned char YUV422Data[8] = {88,80,96,80,88+4,80+4,96+4,80+4};
	unsigned char YUV422Data[64] = { 80,  80,  80,  80,   
	                         84,  84,  84,  84,  
	                         80,  80,  80,  80,   
	                         84,  84,  84,  84,  
	                         80,  80,  80,  80,   
	                         84,  84,  84,  84,  
	                         80,  80,  80,  80,   
	                         84,  84,  84,  84,  
	                         80,  80,  80,  80,   
	                         84,  84,  84,  84,  
	                         80,  80,  80,  80,   
	                         84,  84,  84,  84,  
	                         80,  80,  80,  80,   
	                         84,  84,  84,  84,  
	                         80,  80,  80,  80,   
	                         84,  84,  84,  84};  

	// expected output vector - 
	// take a look at the actual memory content to be convinced that this is the right answer
        unsigned short trueL[32] = { 24487,24487,24519,24519,24487,24487,24519,24519,24487,24487,24519,24519,24487,24487,24519,24519,24487,24487,24519,24519,24487,24487,24519,24519,24487,24487,24519,24519,24487,24487,24519,24519};
        unsigned short trueA[32] = { 16978,16978,17357,17357,16978,16978,17357,17357,16978,16978,17357,17357,16978,16978,17357,17357,16978,16978,17357,17357,16978,16978,17357,17357,16978,16978,17357,17357,16978,16978,17357,17357};
        unsigned short trueB[32] = { 41613,41613,41036,41036,41613,41613,41036,41036,41613,41613,41036,41036,41613,41613,41036,41036,41613,41613,41036,41036,41613,41613,41036,41036,41613,41613,41036,41036,41613,41613,41036,41036};

	unsigned char *interleavedYc= (unsigned char *)pSrcBuf_16bpp; //Size=[2*64];
        unsigned short *extractedL = (unsigned short *)pOutBuf_16bpp;
        unsigned short *extractedA = (unsigned short *)pWorkingBuf_16bpp;
        unsigned short *extractedB = (unsigned short *)pWorkingBuf2_16bpp;
        float *pWhitePoint = (float *)Memory_alloc(3*sizeof(float), &testfxnsMemParams);
        unsigned short *ptr_LabExt= (unsigned short *)pRefBuf_16bpp;
        short *coeff=(short *)Memory_alloc(5*sizeof(short), &testfxnsMemParams);

	// short coeff[] = { 0x2000, 0x2BDD, -0x0AC5, -0x1658, 0x3770 }; 
        short coeff_arr[] = { 0x2543, 0x3313, -0x0C8A, -0x1A04, 0x408D };         
	float whitePoint[3] = {0.950456, 1.0, 1.088754}; // D65
        
	// local variables
	int i, errorCount;
	unsigned short result1, result2;
	int d;

	// initialize I/O buffer	
	// initialize I/O buffer	
	for (i=0; i<32; i++)
	{
		extractedL[i] = 0;
		extractedA[i] = 0;
		extractedB[i] = 0;
                interleavedYc[2*i]=YUV422Data[2*i];
                interleavedYc[2*i+1]=YUV422Data[2*i+1];
        }

        for(i=0; i<5;i++)
                coeff[i]=coeff_arr[i];

        for(i=0;i<3;i++)
                 pWhitePoint[i] = whitePoint[i];

        printf("\n\nTesting the function: VLIB_convertUYVYint_to_LABpl_LUT...\n");


	d = 4;
        C6accel_VLIB_initUYVYint_to_LABpl_LUT(hC6accel, d, coeff, pWhitePoint, ptr_LabExt);

        printf("\n\nCompleted initialization.\n");

        C6accel_VLIB_convertUYVYint_to_LABpl_LUT(hC6accel,interleavedYc, 16, 16, 2, d, ptr_LabExt, extractedL,extractedA, extractedB);

	// verify the results
	errorCount = 0;

	for (i=0; i<32; i++) {

		result1 = extractedL[i];
		result2 = trueL[i];

		if (result1 != result2){
			printf("Error: the extracted L for pixel no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}

	for (i=0; i<32; i++) {

		result1 = extractedA[i];
		result2 = trueA[i];

		if (result1 != result2) {
			printf("Error: the extracted a for pixel no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}

	for (i=0; i<32; i++) {

		result1 = extractedB[i];
		result2 = trueB[i];

		if (result1 != result2) {
			printf("Error: the extracted b for pixel no. %d is incorrect! %d \t %d \n", i, result1, result2);
			errorCount++;
			}
		}

        Memory_free(coeff, 5*sizeof(short), &testfxnsMemParams);
        Memory_free(pWhitePoint, 3*sizeof(float), &testfxnsMemParams);
	if (errorCount)
    	printf("Total of %d errors: VLIB_convertUYVYint_to_LABpl_LUT failed the test!\n", errorCount);
	else
	    printf("VLIB_convertUYVYint_to_LABpl2 passed the test!\n");


return(errorCount);
}

void C6accel_VLIB_testHistogram_1D_U8(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){

	int i,k=0;
	int status;

	// Input data 
	unsigned char X[100] = {
		148,   107,   131,    85,   110,    57,   147,   193,   135,   163,    53,    96,   199,   173,   117,   144,   202,    15,   153,    12,   105,
	    77,   222,     3,   195,   247,   252,   201,   111,   127,    54,   164,    81,   244,   185,   105,   189,    68,   112,   238,   174,    54,
   		214,   160,    34,    52,   154,   160,    94,   146,   115,    11,     6,    79,     3,    97,   174,    23,     9,   156,   155,     4,     4,
	    48,   149,    14,    93,   161,   182,   176,    21,   115,   112,    90,    39,   172,   178,   185,   121,   141,    30,   114,   182,   227,
    	69,    64,   220,    59,   205,   231,    59,    61,    12,    19,   163,    48,   215,    44,    43,   253
    };

	// Ground-truth histogram
	unsigned short GT_H[10] = {14, 7, 12, 8, 13, 11, 13, 11, 6, 0};

	// Number of bins
	int numB = 10;

	// Bin edges
	unsigned char binEdges[10] = {0,  26,  52,  78, 104, 130, 156, 182, 208, 234};

	// Number of elements
	int numX = 100;

	// Internal buffer
	unsigned char *internalBuffer= (unsigned char *)pWorkingBuf_16bpp;// Size =256
        unsigned char *ptr_X= (unsigned char *)pSrcBuf_16bpp; //Size=[2*64];
        unsigned short *ptr_H = (unsigned short *)pWorkingBuf2_16bpp;//(unsigned short *)pOutBuf_16bpp;
        
        unsigned char *pBinEdges = (unsigned char *)pRefBuf_16bpp;
	// Histogram
	unsigned short H[10];//, tempH1[10], tempH2[10], tempH3[10];

	unsigned short binWeight = 1;
        for (i=0;i<10;i++){
           pBinEdges[i] = binEdges[i];
           ptr_H[i] = 0;
        }
        
        for (i=0;i<100;i++)
            ptr_X[i] = X[i];

        memset(internalBuffer, 0, sizeof(unsigned char)*256);

	status = C6accel_VLIB_histogram_1D_Init_U8(hC6accel,pBinEdges,numB, internalBuffer);

	memset(H, 0, sizeof(unsigned short)*numB);
	

	// Compute histogram
	status = C6accel_VLIB_histogram_1D_U8(hC6accel	,ptr_X, 
								  numX, 
								  numB,
								  binWeight,
								  internalBuffer, 							  
								  ptr_H);


	if(status != 0)
		printf("Error in VLIB_histogram_1D_U8\n");

	// Print out the histogram
    if(LevelOfFeedback>0)
    {
	printf("Computed histogram:\n");
	for(i=0;i<numB;i++)
		printf("%d ", ptr_H[i]);
	printf("\n\n");
	}	
 	
 	// Verify output against ground truth
        printf("\n\nTesting the function: VLIB_histogram_1D_U8...\n");
	for(i=0;i<numB;i++){
		if(GT_H[i] != ptr_H[i]){
            if(LevelOfFeedback>0)
			   printf("Error in bin %d\n", i);
			k = 1;
		}
	}

	if(k==0)
		printf("VLIB_histogram_1D_U8 passed the test! \n");
         else
        printf("VLIB_histogram_1D_U8 failed the test! \n");	

	

}	

void C6accel_VLIB_testHistogram_1D_U16(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){

	int i,k=0;
	int status;

	// Input data 
	unsigned short X[100] = {
		148,   107,   131,    85,   110,    57,   147,   193,   135,   163,    53,    96,   199,   173,   117,   144,   202,    15,   153,    12,   105,
	    77,   222,     3,   195,   247,   252,   201,   111,   127,    54,   164,    81,   244,   185,   105,   189,    68,   112,   238,   174,    54,
   		214,   160,    34,    52,   154,   160,    94,   146,   115,    11,     6,    79,     3,    97,   174,    23,     9,   156,   155,     4,     4,
	    48,   149,    14,    93,   161,   182,   176,    21,   115,   112,    90,    39,   172,   178,   185,   121,   141,    30,   114,   182,   227,
    	69,    64,   220,    59,   205,   231,    59,    61,    12,    19,   163,    48,   215,    44,    43,   253
    };

	// Ground-truth histogram
	unsigned short GT_H[10] = {14, 7, 12, 8, 13, 11, 13, 11, 6, 0};

	// Number of bins
	int numB = 10;

	unsigned short binWeight = 1;

	// Bin edges
	unsigned short binEdges[10] = {0,  26,  52,  78, 104, 130, 156, 182, 208, 234};

	// Number of elements
	int numX = 100;

	// Internal buffer
	unsigned short *internalBuffer= (unsigned short *)pWorkingBuf_16bpp;// Size =256
        unsigned short *ptr_X= (unsigned short *)pSrcBuf_16bpp; //Size=[2*64];
        unsigned short *ptr_H = (unsigned short *)pWorkingBuf2_16bpp;//(unsigned short *)pOutBuf_16bpp;
        unsigned short *pBinEdges = (unsigned short *)pRefBuf_16bpp;
      //  unsigned short *pInternalH =  (unsigned short *)(pRefBuf_16bpp+ 10*sizeof(unsigned short));

	// Histogram
	//unsigned short H[10], internalH[10];
        for (i=0;i<10;i++){
           pBinEdges[i] = binEdges[i];
           ptr_H[i] =0;
         }
        
        for (i=0;i<100;i++)
            ptr_X[i] = X[i];

	memset(internalBuffer, 0, sizeof(unsigned short)*256);
	memset(ptr_H, 0,sizeof(unsigned short)*10);

	C6accel_VLIB_histogram_1D_Init_U16(hC6accel	,pBinEdges,
								numB,
								internalBuffer );

	//memset(pInternalH, 0, sizeof(unsigned short)*numB);
	memset(ptr_H, 0, sizeof(unsigned short)*numB);

	// Compute histogram
	status = C6accel_VLIB_histogram_1D_U16(hC6accel	,          ptr_X, 
							   	   numX, 
								   numB,
								   binWeight,
							  	   internalBuffer, 							  
							           ptr_H);


	if(status != 0)
		printf("Error in VLIB_histogram_1D_U16\n");


    if(LevelOfFeedback>0)
    {
	// Print out the histogram
	printf("Computed histogram:\n");
	for(i=0;i<numB;i++)
		printf("%d ", ptr_H[i]);
	printf("\n\n");
	}	

	// Verify output against ground truth
        printf("\n\nTesting the function: VLIB_histogram_1D_U16...\n");
	for(i=0;i<numB;i++){
		if(GT_H[i] != ptr_H[i]){
            if(LevelOfFeedback>0)
			printf("Error in bin %d\n", i);
			k = 1;
		}
	}

	if(k==0)
		printf("VLIB_histogram_1D_U16 passed the test! \n");
        else
        printf("VLIB_histogram_1D_U16 failed the test! \n");	

}	

void C6accel_VLIB_testHistogram_nD_U16(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){

	int i,k=0;
	int status;

	// Input data 
	unsigned short X[100] = {
		148,   107,   131,    85,   110,    57,   147,   193,   135,   163,    53,    96,   199,   173,   117,   144,   202,    15,   153,    12,   105,
	         77,   222,     3,   195,   247,   252,   201,   111,   127,    54,   164,    81,   244,   185,   105,   189,    68,   112,   238,   174,    54,
   		214,   160,    34,    52,   154,   160,    94,   146,   115,    11,     6,    79,     3,    97,   174,    23,     9,   156,   155,     4,     4,
	         48,   149,    14,    93,   161,   182,   176,    21,   115,   112,    90,    39,   172,   178,   185,   121,   141,    30,   114,   182,   227,
    	         69,    64,   220,    59,   205,   231,    59,    61,    12,    19,   163,    48,   215,    44,    43,   253
        };

	// Ground-truth histogram
	unsigned short GT_H[48] = {
	    1,     1,     0,     0,     0,     1,     1,     1,     2,     1,     0,     0,     0,     0,     1,     2,     1,     1,     0,     0,     0,
	    0,     0,     0,     0,     1,     1,     2,     2,     2,     0,     0,     1,     0,     0,     1,     1,     0,     0,     0,     1,     0,
    	0,     0,     0,     0,     0,     0};

	// Number of dimensions
	int dimX = 4;

	// Number of vectors 
	int numX = 25;

	// Number of bins
	unsigned short numB[4] = {2, 3, 4, 2};

	// Normalization values
	unsigned short normVals[4] = {256, 256, 256, 256}; // 65536/256

	// Bin weight
	unsigned short binWeight = 1;

	unsigned short *pNumB = (unsigned short *)pWorkingBuf2_16bpp;
        unsigned short *ptr_X= (unsigned short *)pSrcBuf_16bpp; //Size=[100];
        unsigned short *ptr_H = (unsigned short *)pOutBuf_16bpp;//size =48
        unsigned short *pNormVals =  (unsigned short *)pRefBuf_16bpp;


	memset(ptr_H, 0, sizeof(unsigned short)*48);
	
        for (i=0;i<4;i++){
           pNormVals[i] = normVals[i];
           pNumB[i]= numB[i];
        }

        for (i=0;i<100;i++)
            ptr_X[i] = X[i];
     
        status = 0;
	// Compute histogram
	status = C6accel_VLIB_histogram_nD_U16(hC6accel, ptr_X, 
								  numX, 
								  dimX, 
								  binWeight,
								  pNumB, 
								  pNormVals, 
								  ptr_H);

	if(status != 0)
		printf("Error in VLIB_histogram_nD_U16\n");

	// Print out the histogram
        if(LevelOfFeedback>0){
	   printf("Computed histogram:\n");
	for(i=0;i<48;i++)
		printf("%d ", ptr_H[i]);
	   printf("\n\n");
	}	

	// Verify output against ground truth
        printf("\n\nTesting the function: VLIB_histogram_nD_U16...\n");
	for(i=0;i<48;i++){
             	if(GT_H[i] != ptr_H[i]){
                  if(LevelOfFeedback>0)
                 	printf("Error in bin %d\n", i);
			k = 1;
		}
	}

	if(k==0)
		printf("VLIB_histogram_nD_U16 passed the test! \n");
        else
               printf("VLIB_histogram_nD_U16 failed the test! \n");	

}	

	

void C6accel_VLIB_testWeightedHistogram_1D_U8(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){

	int i,k=0;
	int status;
        // Input data 
	static unsigned char X[100] = {
	   148,   107,   131,    85,   110,    57,   147,   193,   135,   163,    53,    96,   199,   173,   117,   144,   202,    15,   153,    12,   105,
	    77,   222,     3,   195,   247,   252,   201,   111,   127,    54,   164,    81,   244,   185,   105,   189,    68,   112,   238,   174,    54,
   	   214,   160,    34,    52,   154,   160,    94,   146,   115,    11,     6,    79,     3,    97,   174,    23,     9,   156,   155,     4,     4,
	    48,   149,    14,    93,   161,   182,   176,    21,   115,   112,    90,    39,   172,   178,   185,   121,   141,    30,   114,   182,   227,
    	    69,    64,   220,    59,   205,   231,    59,    61,    12,    19,   163,    48,   215,    44,    43,   253
        };

	// Ground-truth histogram
	static unsigned short GT_H[10] = {14, 7, 12, 8, 13, 11, 13, 11, 6, 0};

	// Number of bins
	static int numB = 10;

	// Bin edges
	static unsigned char binEdges[10] = {0,  26,  52,  78, 104, 130, 156, 182, 208, 234};

	// Number of elements
	static int numX = 100;

	// Internal buffer
	static unsigned char internalBuffer[256];

	// Histogram
	static unsigned short H[10];//, tempH1[10], tempH2[10], tempH3[10];

	static unsigned short binWeight[100];
        unsigned char  *pBinEdges = (unsigned char *)pWorkingBuf_16bpp; //Size = 10
        unsigned char  *ptr_X= (unsigned char *)pSrcBuf_16bpp; //Size=[100];
        unsigned short *ptr_H = (unsigned short *)pWorkingBuf2_16bpp;//size =48
        unsigned char  *pInternalBuffer =  (unsigned char *)pRefBuf_16bpp; //Size =256
        unsigned short *pBinWeight = (unsigned short *)pOutBuf_16bpp; //size =100

	for(i=0;i<numX;i++){
		pBinWeight[i] = 1;
                ptr_X[i] = X[i];
         }
        for(i=0;i<numB;i++)
             pBinEdges[i] =binEdges[i];

	memset(pInternalBuffer, 0, sizeof(unsigned char)*256);
        memset(ptr_H, 0, sizeof(unsigned short)*numB);

	status = C6accel_VLIB_histogram_1D_Init_U8(hC6accel,
                                                   pBinEdges,
						       numB,
						   pInternalBuffer );

	// Compute histogram
	status = C6accel_VLIB_weightedHistogram_1D_U8(hC6accel,ptr_X, 
					       		     numX, 
							     numB,
					      	       pBinWeight,
						  pInternalBuffer, 							  
						       	  ptr_H);


	if(status != 0)
		printf("Error in VLIB_weightedHistogram_1D_U8\n");

	// Print out the histogram
       if(LevelOfFeedback>0)
       {
	  printf("Computed histogram:\n");
	for(i=0;i<numB;i++)
		printf("%d ", ptr_H[i]);
	printf("\n\n");
	}	

	// Verify output against ground truth
        printf("\n\nTesting the function: VLIB_weightedHistogram_1D_U8...\n");
	for(i=0;i<numB;i++){
		if(GT_H[i] != ptr_H[i]){
            if(LevelOfFeedback>0)
			   printf("Error in bin %d\n", i);
			k = 1;
		}
	}


  	if(k==0)
		printf("VLIB_weightedHistogram_1D_U8 passed the test! \n");
        else
                printf("VLIB_weightedHistogram_1D_U8 failed the test! \n");	

}

void C6accel_VLIB_testWeightedHistogram_1D_U16(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){

	int i,k=0;
	int status;

	// Input data 
	unsigned short X[100] = {
            148,   107,   131,    85,   110,    57,   147,   193,   135,   163,    53,    96,   199,   173,   117,   144,   202,    15,   153,    12,   105,
	    77,   222,     3,   195,   247,   252,   201,   111,   127,    54,   164,    81,   244,   185,   105,   189,    68,   112,   238,   174,    54,
   	    214,   160,    34,    52,   154,   160,    94,   146,   115,    11,     6,    79,     3,    97,   174,    23,     9,   156,   155,     4,     4,
	    48,   149,    14,    93,   161,   182,   176,    21,   115,   112,    90,    39,   172,   178,   185,   121,   141,    30,   114,   182,   227,
    	    69,    64,   220,    59,   205,   231,    59,    61,    12,    19,   163,    48,   215,    44,    43,   253
        };

	// Ground-truth histogram
	unsigned short GT_H[10] = {14, 7, 12, 8, 13, 11, 13, 11, 6, 0};

	// Number of bins
	int numB = 10;

	// Bin edges
	unsigned short binEdges[10] = {0,  26,  52,  78, 104, 130, 156, 182, 208, 234};

	// Number of elements
	int numX = 100;

	// Internal buffer
	unsigned short internalBuffer[256];

	// Histogram
	unsigned short H[10], internalH[10];

	unsigned short binWeight[100];
        unsigned short  *pBinEdges = (unsigned short *)pWorkingBuf_16bpp; //Size = 10
        unsigned short  *ptr_X= (unsigned short *)pSrcBuf_16bpp; //Size=[100];
        unsigned short *ptr_H = (unsigned short *)pWorkingBuf2_16bpp;//size =48
        unsigned short  *pInternalBuffer =  (unsigned short *)pRefBuf_16bpp; //Size =256
        unsigned short *pBinWeight = (unsigned short *)pOutBuf_16bpp; //size =100
	for(i=0;i<numX;i++){
		pBinWeight[i] = 1;
                ptr_X[i] = X[i];
         }
         for(i=0;i<numB;i++)
             pBinEdges[i] = binEdges[i];

	memset(pInternalBuffer, 0, sizeof(unsigned short)*256);
        memset(ptr_H, 0, sizeof(unsigned short)*numB);

	//memset(internalBuffer, 0, sizeof(internalBuffer));
	C6accel_VLIB_histogram_1D_Init_U16(hC6accel,	pBinEdges,
						        numB,
							pInternalBuffer );

	// Compute histogram
	status = C6accel_VLIB_weightedHistogram_1D_U16(hC6accel, ptr_X, 
								 numX, 
								 numB,
								 pBinWeight,
								 pInternalBuffer, 							  
								 ptr_H);


	if(status != 0)
		printf("Error in VLIB_weightedHistogram_1D_U16\n");

	// Print out the histogram
        if(LevelOfFeedback>0){
	 printf("Computed histogram:\n");
	for(i=0;i<numB;i++)
		printf("%d ", ptr_H[i]);
	printf("\n\n");
	}	

	// Verify output against ground truth
        printf("\n\nTesting the function: VLIB_weightedHistogram_1D_U16...\n");

	for(i=0;i<numB;i++){
		if(GT_H[i] != ptr_H[i]){
            if(LevelOfFeedback>0)
			   printf("Error in bin %d\n", i);
			k = 1;
		}
	}

  	if(k==0)
		printf("VLIB_weightedHistogram_1D_U16 passed the test! \n");
        else
                printf("VLIB_weightedHistogram_1D_U16 failed the test! \n");	

	

}	

void C6accel_VLIB_testWeightedHistogram_nD_U16(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){

	int i,k=0;
	int status;

	// Input data 
	unsigned short X[100] = {
		148,   107,   131,    85,   110,    57,   147,   193,   135,   163,    53,    96,   199,   173,   117,   144,   202,    15,   153,    12,   105,
	    77,   222,     3,   195,   247,   252,   201,   111,   127,    54,   164,    81,   244,   185,   105,   189,    68,   112,   238,   174,    54,
   		214,   160,    34,    52,   154,   160,    94,   146,   115,    11,     6,    79,     3,    97,   174,    23,     9,   156,   155,     4,     4,
	    48,   149,    14,    93,   161,   182,   176,    21,   115,   112,    90,    39,   172,   178,   185,   121,   141,    30,   114,   182,   227,
    	69,    64,   220,    59,   205,   231,    59,    61,    12,    19,   163,    48,   215,    44,    43,   253
    };

	// Ground-truth histogram
	unsigned short GT_H[48] = {
	    1,     1,     0,     0,     0,     1,     1,     1,     2,     1,     0,     0,     0,     0,     1,     2,     1,     1,     0,     0,     0,
	    0,     0,     0,     0,     1,     1,     2,     2,     2,     0,     0,     1,     0,     0,     1,     1,     0,     0,     0,     1,     0,
    	0,     0,     0,     0,     0,     0};

	// Number of dimensions
	int dimX = 4;

	// Number of vectors 
	int numX = 25;

	// Number of bins
	unsigned short numB[4] = {2, 3, 4, 2};

	// Normalization values
	unsigned short normVals[4] = {256, 256, 256, 256}; // 65536/256

	// Internal buffer
	unsigned short internalBuffer[25];

	// Histogram
	unsigned short H[48];

	// Internal buffer 2
	unsigned short internalH[48];

	// Bin weight
	unsigned short *pBinWeight = (unsigned short *)pWorkingBuf_16bpp; // Size =25

        unsigned short *pNumB = (unsigned short *)pWorkingBuf2_16bpp;
        unsigned short *ptr_X= (unsigned short *)pSrcBuf_16bpp; //Size=[100];
        unsigned short *ptr_H = (unsigned short *)pOutBuf_16bpp;//size =48
        unsigned short *pNormVals =  (unsigned short *)pRefBuf_16bpp;
        

	memset(ptr_H, 0, sizeof(unsigned short)*48);
	
        for (i=0;i<4;i++){
           pNormVals[i] = normVals[i];
           pNumB[i]= numB[i];
        }

        for (i=0;i<100;i++)
            ptr_X[i] = X[i];

	for(i=0;i<25;i++)
		pBinWeight[i] = 1; 
 
	
	memset(ptr_H, 0, sizeof(unsigned short)*48);
	


	// Compute histogram
	status = C6accel_VLIB_weightedHistogram_nD_U16(hC6accel, ptr_X, 
										  numX, 
										  dimX, 
										  pBinWeight,
										  pNumB, 
										  pNormVals, 
										  ptr_H);

	if(status != 0)
		printf("Error in VLIB_weightedHistogram_nD_U16\n");

	// Print out the histogram
    if(LevelOfFeedback>0)
    {
	printf("Computed histogram:\n");
	for(i=0;i<48;i++)
		printf("%d ", ptr_H[i]);
	printf("\n\n");
	}	

	// Verify output against ground truth
    printf("\n\nTesting the function: VLIB_weightedHistogram_nD_U16...\n");
	for(i=0;i<48;i++){
		if(GT_H[i] != ptr_H[i]){
            if(LevelOfFeedback>0)
			  printf("Error in bin %d\n", i);
			k = 1;
		}
	}

  	if(k==0)
		printf("VLIB_weightedHistogram_nD_U16 passed the test! \n");
    else
        printf("VLIB_weightedHistogram_nD_U16 failed the test! \n");	

	
	

}	

static void CreatePattern1(unsigned char *p, int width, int height)
{
	int i;
	for (i = 0; i < width*height; i++)
	{
		int x = i%8;
		int y = i/8;

		p[i] = 12*(x | y) + 127;
	}
}


int C6accel_VLIB_testRecursiveFilterHoriz1stOrder(C6accel_Handle hC6accel,unsigned char LevelOfFeedback)
{
	int i;

	unsigned char GroundTruth[]={128, 139, 151, 163, 175, 187, 199, 210,
					     139, 141, 162, 164, 185, 188, 209, 211,
					     152, 161, 153, 164, 197, 209, 200, 210,
					     163, 163, 163, 166, 208, 211, 211, 211,
					     176, 187, 198, 208, 178, 187, 199, 210,
					     187, 188, 209, 209, 188, 189, 210, 211,
					     200, 209, 200, 209, 200, 209, 200, 210,
					     211, 211, 211, 211, 211, 211, 211, 211};

	int errorCount = 0;
        unsigned char *IM= (unsigned char *)pSrcBuf_16bpp; //Size=[100];
        unsigned char *OUT = (unsigned char *)pOutBuf_16bpp;
	CreatePattern1(IM,8,8);

	printf("\nTesting VLIB_recursiveFilterHoriz1stOrder...\n");

	C6accel_VLIB_recursiveFilterHoriz1stOrder(hC6accel,OUT, IM, 8, 8, 0x6fff);

	for (i = 0; i < 8*8; i++)
	{
		if (OUT[i] != GroundTruth[i]){
			printf("Error: the output value for pixel no. %d is incorrect! %d, %d \n", i, OUT[i], GroundTruth[i]);
			errorCount++;	
		}
	}

	if (errorCount)
    	printf("Total of %d errors: VLIB_recursiveFilterHoriz1stOrder failed the test!\n", errorCount);
	else
	    printf("VLIB_recursiveFilterHoriz1stOrder passed the test!\n");

	return 0;
}


static void CreatePattern2(short *p, int width, int height)
{
	int i;
	for (i = 0; i < width*height; i++)
	{
		int x = i%8;
		int y = i/8;

		p[i] = 12*(x | y) + 127 + 5000;
	}
}


int C6accel_VLIB_testRecursiveFilterHoriz1stOrderS16(C6accel_Handle hC6accel,unsigned char LevelOfFeedback)
{
	int i;

	short GroundTruth[]={5128, 5139, 5151, 5163, 5175, 5187, 5199, 5210,
		 				 5139, 5141, 5162, 5165, 5186, 5189, 5210, 5211,
						 5152, 5162, 5153, 5165, 5197, 5210, 5201, 5211,
						 5163, 5163, 5164, 5166, 5208, 5211, 5211, 5211,
						 5176, 5187, 5199, 5208, 5178, 5188, 5199, 5210,
						 5187, 5189, 5210, 5210, 5189, 5189, 5210, 5211,
						 5200, 5210, 5200, 5210, 5200, 5210, 5201, 5211,
						 5211, 5211, 5211, 5211, 5211, 5211, 5211, 5211};

	int errorCount = 0;
        unsigned short *IM16= (unsigned short *)pSrcBuf_16bpp; //Size=[8x8];
        unsigned short *OUT16 = (unsigned short *)pOutBuf_16bpp;
	CreatePattern2(IM16,8,8);

	printf("\nTesting VLIB_recursiveFilterHoriz1stOrderS16...\n");

	C6accel_VLIB_recursiveFilterHoriz1stOrderS16(hC6accel, OUT16, IM16, 8, 8, 0x6fff);

	for (i = 0; i < 8*8; i++)
	{
		if (OUT16[i] != GroundTruth[i])
		{
			printf("Error: the output value for pixel no. %d is incorrect!\n", i);
			errorCount++;	
		}
	}

	if (errorCount)
    	printf("Total of %d errors: VLIB_recursiveFilterHoriz1stOrderS16 failed the test!\n", errorCount);
	else
	    printf("VLIB_recursiveFilterHoriz1stOrderS16 passed the test!\n");

	return 0;
}

int C6accel_VLIB_testRecursiveFilterVert1stOrder(C6accel_Handle hC6accel,unsigned char LevelOfFeedback)
{
	int i;


	unsigned char GroundTruth[]={128, 139, 152, 163, 176, 187, 200, 211,
					     139, 141, 161, 163, 187, 188, 209, 211,
					     151, 162, 153, 163, 198, 209, 200, 211,
					     163, 164, 164, 166, 208, 209, 209, 211,
					     175, 185, 197, 208, 178, 188, 200, 211,
					     187, 188, 209, 211, 187, 189, 209, 211,
					     199, 209, 200, 211, 199, 210, 200, 211,
					     210, 211, 210, 211, 210, 211, 210, 211};

	int errorCount = 0;
        unsigned char *IM= (unsigned char *)pSrcBuf_16bpp; //Size=[8x8];
        unsigned char *OUT = (unsigned char *)pOutBuf_16bpp;
	CreatePattern1(IM,8,8);

	printf("\nTesting VLIB_recursiveFilterVert1stOrder...\n");

	C6accel_VLIB_recursiveFilterVert1stOrder(hC6accel,OUT, IM, 8, 8, 0x6fff);

	for (i = 0; i < 8*8; i++)
	{
		if (OUT[i] != GroundTruth[i])
		{
			printf("Error: the output value for pixel no. %d is incorrect!\n", i);
			errorCount++;	
		}
	}

	if (errorCount)
    	printf("Total of %d errors: VLIB_recursiveFilterVert1stOrder failed the test!\n", errorCount);
	else
	    printf("VLIB_recursiveFilterVert1stOrder passed the test!\n");

	return 0;
}

int C6accel_VLIB_testRecursiveFilterVert1stOrderS16(C6accel_Handle hC6accel,unsigned char LevelOfFeedback)
{
	int i;


	short GroundTruth[]={5128, 5139, 5152, 5163, 5176, 5187, 5200, 5211,
					     5139, 5141, 5162, 5163, 5187, 5189, 5210, 5211,
					     5151, 5162, 5153, 5164, 5199, 5210, 5200, 5211,
					     5163, 5165, 5165, 5166, 5208, 5210, 5210, 5211,
					     5175, 5186, 5197, 5208, 5178, 5189, 5200, 5211,
					     5187, 5189, 5210, 5211, 5188, 5189, 5210, 5211,
					     5199, 5210, 5201, 5211, 5199, 5210, 5201, 5211,
					     5210, 5211, 5211, 5211, 5210, 5211, 5211, 5211};

	int errorCount = 0;
        unsigned short *IM16= (unsigned short *)pSrcBuf_16bpp; //Size=[8x8];
        unsigned short *OUT16 = (unsigned short *)pOutBuf_16bpp;
	CreatePattern2(IM16,8,8);

	printf("\nTesting VLIB_recursiveFilterVert1stOrderS16...\n");

	C6accel_VLIB_recursiveFilterVert1stOrderS16(hC6accel,OUT16, IM16, 8, 8, 0x6fff);

	for (i = 0; i < 8*8; i++)
	{
		if (OUT16[i] != GroundTruth[i])
		{
			printf("Error: the output value for pixel no. %d is incorrect!\n", i);
			errorCount++;	
		}
	}

	if (errorCount)
    	printf("Total of %d errors: VLIB_recursiveFilterVert1stOrderS16 failed the test!\n", errorCount);
	else
	    printf("VLIB_recursiveFilterVert1stOrderS16 passed the test!\n");

	return 0;
}

unsigned char InData_IntegralImage8[16*4] = {	151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6, 
									 	 31, 143, 178,   3, 135,  91,  54, 154, 193, 161,  20, 162, 137, 150, 128, 224, 
					  					214, 113,   9,  28,  53, 211,  98, 217, 149, 233, 231, 127, 115, 203, 177,  42, 
					   					 62, 155,   3, 103, 127,  16, 135, 131, 211, 158,   9,   2, 106, 227, 249, 255};
// expected output calculated off-line in Matlab
unsigned int trueOutData_IntegralImage8[16*4] = { 151, 208,  324,  494,  503,  750,  958, 1098, 1248, 1308, 1396, 1473, 1477, 1483, 1645, 1651, 
											182, 382,  676,  849,  993, 1331, 1593, 1887, 2230, 2451, 2559, 2798, 2939, 3095, 3385, 3615, 
											396, 709, 1012, 1213, 1410, 1959, 2319, 2830, 3322, 3776, 4115, 4481, 4737, 5096, 5563, 5835, 
											458, 926, 1232, 1536, 1860, 2425, 2920, 3562, 4265, 4877, 5225, 5593, 5955, 6541, 7257, 7784};
//unsigned int lastLine_IntegralImage8[16];
//unsigned int calculatedOutData_IntegralImage8[16*4];



int C6accel_VLIB_testIntegralImage8(C6accel_Handle hC6accel,unsigned char LevelOfFeedback)
{


	// local variables
	int i, errorCount;
	unsigned int result1, result2;
        unsigned char *IM= (unsigned char *)pSrcBuf_16bpp; //Size=[16x4];
        unsigned int  *OUT = (unsigned int *)pOutBuf_16bpp;//Size=[16x4];
        unsigned int  *pLastLine= (unsigned int *)pWorkingBuf_16bpp; //Size =16

	// initialize the lastLine buffer	
	for (i=0; i<16; i++)
		pLastLine[i] = 0;

        for(i=0; i<16*4; i++)
               IM[i] = InData_IntegralImage8[i];
         
      	// call the function
	C6accel_VLIB_integralImage8(hC6accel, IM, 				// input 8-bit data
						16,									// width of input image
						4,									// height of input image
						pLastLine,                      // the last line buffer
						OUT); 	// 32-bit output data


	printf("\nTesting the function: VLIB_integralImage8...\n");

	// verify the results
	errorCount = 0;

	// extract results one by one and compare
	for (i=0; i<16*4; i++) {

		result1 = OUT[i];
		result2 = trueOutData_IntegralImage8[i];

		if (result1 != result2) {
			printf("Error: Pixel no. %d is incorrect!\n", i);
			errorCount++;
			}
		}

	if (errorCount)
    	printf("Total of %d errors: VLIB_integralImage8 failed the test!\n\n", errorCount);
	else
	    printf("VLIB_integralImage8 passed the test!\n\n");


return(errorCount);
}

// test input 
unsigned short InData_IntegralImage16[16*4] = { 604, 228, 464, 680,  36, 988, 832, 560, 600, 240, 352, 308,  16,  24, 648,  24,
                        				124, 572, 712,  12, 540, 364, 216, 616, 772, 644,  80, 648, 548, 600, 512, 896,
                       					856, 452,  36, 112, 212, 844, 392, 868, 596, 932, 924, 508, 460, 812, 708, 168,
                        				248, 620,  12, 412, 508,  64, 540, 524, 844, 632,  36,   8, 424, 908, 996,1020
};
// expected output calculated off-line in Matlab
unsigned int trueOutData_IntegralImage16[16*4] = { 604,   832,  1296,  1976,  2012,  3000,  3832,  4392,  4992,  5232,  5584,  5892,  5908,  5932,  6580,  6604,
                          					 728,  1528,  2704,  3396,  3972,  5324,  6372,  7548,  8920,  9804, 10236, 11192, 11756, 12380, 13540, 14460,
                           					1584,  2836,  4048,  4852,  5640,  7836,  9276, 11320, 13288, 15104, 16460, 17924, 18948, 20384, 22252, 23340,
                           					1832,  3704,  4928,  6144,  7440,  9700, 11680, 14248, 17060, 19508, 20900, 22372, 23820, 26164, 29028, 31136
};


int C6accel_VLIB_testIntegralImage16(C6accel_Handle hC6accel,unsigned char LevelOfFeedback) {


	// local variables
	int i, errorCount;
	unsigned int result1, result2;
        unsigned short *IM= (unsigned short *)pSrcBuf_16bpp; //Size=[16x4];
        unsigned int  *OUT = (unsigned int *)pOutBuf_16bpp;//Size=[16x4];
        unsigned int  *pLastLine= (unsigned int *)pWorkingBuf_16bpp; //Size =16

	// initialize the lastLine buffer	
	for (i=0; i<16; i++)
		pLastLine[i] = 0;

        for(i=0; i<16*4; i++)
               IM[i] = InData_IntegralImage16[i];

	// call the function
	C6accel_VLIB_integralImage16(hC6accel, IM, 				// input 16-bit data
						16,				// width of input image
						4,				// height of input image
						pLastLine,                      // the last line buffer
						OUT); 	// 32-bit output data

	printf("\nTesting the function: VLIB_integralImage16...\n");

	// verify the results
	errorCount = 0;

	// extract results one by one and compare
	for (i=0; i<16*4; i++) {

		result1 = OUT[i];
		result2 = trueOutData_IntegralImage16[i];

		if (result1 != result2) {
			printf("Error: Pixel no. %d is incorrect!\n", i);
			errorCount++;
			}
		}

	if (errorCount)
    	printf("Total of %d errors: VLIB_integralImage16 failed the test!\n\n", errorCount);
	else
	    printf("VLIB_integralImage16 passed the test!\n\n");

return(errorCount);
}

// test input 
unsigned char InData_Pyramid8[16*8] = {151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6, 
				 			    31, 143, 178,   3, 135,  91,  54, 154, 193, 161,  20, 162, 137, 150, 128, 224, 
					 	 	   214, 113,   9,  28,  53, 211,  98, 217, 149, 233, 231, 127, 115, 203, 177,  42, 
							    62, 155,   3, 103, 127,  16, 135, 131, 211, 158,   9,   2, 106, 227, 249, 255,
							   151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6, 
						 	    31, 143, 178,   3, 135,  91,  54, 154, 193, 161,  20, 162, 137, 150, 128, 224, 
							   214, 113,   9,  28,  53, 211,  98, 217, 149, 233, 231, 127, 115, 203, 177,  42, 
							    62, 155,   3, 103, 127,  16, 135, 131, 211, 158,   9,   2, 106, 227, 249, 255
							    };
// expected output calculated off-line in Matlab
unsigned char trueOutData_Pyramid8[8*4 + 4*2 + 2*1] = {  95, 116, 120, 139, 141, 86, 74, 130, 
												136, 35, 101, 145, 187, 92, 162, 180, 
												 95, 116, 120, 139, 141, 86, 74, 130, 
												136, 35, 101, 145, 187, 92, 162, 180, 
												 95, 126, 126, 136, 

												 95, 126, 126, 136, 
												110, 131};

int C6accel_VLIB_testImagePyramid8(C6accel_Handle hC6accel,unsigned char LevelOfFeedback) {


	// local variables
	int i, errorCount;
	unsigned int result1, result2;
        unsigned char * OUT = (unsigned char *)pOutBuf_16bpp;//[8*4 + 4*2 + 2*1]
        unsigned char * IN  = (unsigned char *)pSrcBuf_16bpp; 


        for(i=0; i<16*8; i++)
               IN[i] = InData_Pyramid8[i];

	// call the function
	C6accel_VLIB_imagePyramid8(hC6accel,IN, 			   	// input 8-bit data
				            16,								// width of input image
					     8,								// height of input image
				            OUT); 	// 8-bit output data


	printf("\nTesting the function: VLIB_imagePyramid8...\n");

	// verify the results
	errorCount = 0;

	// extract results one by one and compare
	for (i=0; i<(8*4 + 4*2 + 2*1); i++) {

		result1 = OUT[i];
		result2 = trueOutData_Pyramid8[i];

		if (result1 != result2) {
			printf("Error: Pixel no. %d is incorrect!\n", i);
			errorCount++;
			}
		}


	if (errorCount)
    	    printf("Total of %d errors: VLIB_imagePyramid8 failed the test!\n\n", errorCount);
	else
	    printf("VLIB_imagePyramid8 passed the test!\n\n");

	
return(errorCount);
}

unsigned short InData_Pyramid16[16*8] = {1208,  456,  928, 1360,   72, 1976, 1664, 1120, 1200,  480,  704,  616,   32,   48, 1296,   48,
							   248, 1144, 1424,   24, 1080,  728,  432, 1232, 1544, 1288,  160, 1296, 1096, 1200, 1024, 1792,
							  1712,  904,   72,  224,  424, 1688,  784, 1736, 1192, 1864, 1848, 1016,  920, 1624, 1416,  336,
							   496, 1240,   24,  824, 1016,  128, 1080, 1048, 1688, 1264,   72,   16,  848, 1816, 1992, 2040,
							  1208,  456,  928, 1360,   72, 1976, 1664, 1120, 1200,  480,  704,  616,   32,   48, 1296,   48,
							   248, 1144, 1424,   24, 1080,  728,  432, 1232, 1544, 1288,  160, 1296, 1096, 1200, 1024, 1792,
							  1712,  904,   72,  224,  424, 1688,  784, 1736, 1192, 1864, 1848, 1016,  920, 1624, 1416,  336,
							   496, 1240,   24,  824, 1016,  128, 1080, 1048, 1688, 1264,   72,   16,  848, 1816, 1992, 2040};
// expected output calculated off-line in Matlab
unsigned short trueOutData_Pyramid16[8*4 + 4*2 + 2*1] = { 	 764,  934,  964, 1112, 1128,  694,  594, 1040,
												1088,  286,  814, 1162, 1502,  738, 1302, 1446,
												 764,  934,  964, 1112, 1128,  694,  594, 1040,
												1088,  286,  814, 1162, 1502,  738, 1302, 1446,
												 768, 1013, 1015, 1095,
												 768, 1013, 1015, 1095,
												 890, 1055};
// output 
//unsigned short calculatedOutData_Pyramid16[8*4 + 4*2 + 2*1];


int C6accel_VLIB_testImagePyramid16(C6accel_Handle hC6accel,unsigned char LevelOfFeedback) {


	// local variables
	int i, errorCount;
	unsigned int result1, result2;
        unsigned short * OUT = (unsigned short *)pOutBuf_16bpp;//[8*4 + 4*2 + 2*1]
        unsigned short * IN  = (unsigned short *)pSrcBuf_16bpp; 


        for(i=0; i<16*8; i++)
               IN[i] = InData_Pyramid16[i];

	// call the function
	C6accel_VLIB_imagePyramid16(hC6accel, IN, 			    // input 16-bit data
					      16,								// width of input image
					       8,								// height of input image
					      OUT); 	// 16-bit output data


	printf("\nTesting the function: VLIB_imagePyramid16...\n");

	// verify the results
	errorCount = 0;

	// extract results one by one and compare
	for (i=0; i<(8*4 + 4*2 + 2*1); i++) {
		result1 = OUT[i];
		result2 = trueOutData_Pyramid16[i];

		if (result1 != result2) {
			printf("Error: Pixel no. %d is incorrect!\n", i);
			errorCount++;
		}
	}


	if (errorCount)
    	printf("Total of %d errors: VLIB_imagePyramid16 failed the test!\n\n", errorCount);
	else
	    printf("VLIB_imagePyramid16 passed the test!\n\n");

	

return(errorCount);
}


unsigned char InData_Gauss5x5PyramidKernel_8[16*5] = {151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6, 
				 			                   31, 143, 178,   3, 135,  91,  54, 154, 193, 161,  20, 162, 137, 150, 128, 224, 
					 	 	                  214, 113,   9,  28,  53, 211,  98, 217, 149, 233, 231, 127, 115, 203, 177,  42, 
							                   62, 155,   3, 103, 127,  16, 135, 131, 211, 158,   9,   2, 106, 227, 249, 255,
						                	  151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6 };

// temporary buffer
unsigned short Temp_Gauss5x5PyramidKernel_8[12*5];

// expected output calculated off-line in Matlab
unsigned char trueOutData_Gauss5x5PyramidKernel_8[6] = { 82, 92, 134, 165, 125, 124 }; // 82.2891, 92.0078, 134.3516, 165.4766, 125.0469, 124.3438

int C6accel_VLIB_testGauss5x5PyramidKernel_8(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){


	// local variables
	int i, errorCount;
	unsigned int result1, result2;
        unsigned char * OUT = (unsigned char *)pOutBuf_16bpp;
        unsigned char * IN  = (unsigned char *)pSrcBuf_16bpp; 
        unsigned short * temp = (unsigned short *)pWorkingBuf_16bpp;

        for(i=0; i<16*5; i++)
               IN[i] = InData_Gauss5x5PyramidKernel_8[i];

        for(i=0;i<12*5; i++)
               temp[i] = 0;

	// call the function
	C6accel_VLIB_gauss5x5PyramidKernel_8(hC6accel, IN, 	   			// input 8-bit data
	                                                 temp,        			// temp buffer
						         16-4,											// width of input image
						         16,											// pitch of input image
							 5,
						         OUT); 	// 8-bit output data


	printf("\nTesting the function: VLIB_gauss5x5PyramidKernel_8...\n");

	// verify the results
	errorCount = 0;

	// extract results one by one and compare
	for (i=0; i<4; i++) {

		result1 = OUT[i];
		result2 = trueOutData_Gauss5x5PyramidKernel_8[i];

		if (result1 != result2) {
			printf("Error: Pixel no. %d is incorrect! \t %x \t %x  \n", i, result1, result2);
			errorCount++;
			}
		}


	if (errorCount)
    	printf("Total of %d errors: VLIB_gauss5x5PyramidKernel_8 failed the test!\n\n", errorCount);
	else
	    printf("VLIB_gauss5x5PyramidKernel_8 passed the test!\n\n");

	
return(errorCount);
}

// test input 
unsigned short InData_Gauss5x5PyramidKernel_16[16*5] = {151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6, 
				 			                   31, 143, 178,   3, 135,  91,  54, 154, 193, 161,  20, 162, 137, 150, 128, 224, 
					 	 	                  214, 113,   9,  28,  53, 211,  98, 217, 149, 233, 231, 127, 115, 203, 177,  42, 
							                   62, 155,   3, 103, 127,  16, 135, 131, 211, 158,   9,   2, 106, 227, 249, 255,
						                	  151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6 };

// temporary buffer
unsigned int Temp_Gauss5x5PyramidKernel_16[12*5];

// expected output calculated off-line in Matlab
unsigned short trueOutData_Gauss5x5PyramidKernel_16[6] = { 82, 92, 134, 165, 125, 124 }; // 82.2891, 92.0078, 134.3516, 165.4766, 125.0469, 124.3438

// output 
//unsigned short calculatedOutData_Gauss5x5PyramidKernel_16[6];

int C6accel_VLIB_testGauss5x5PyramidKernel_16(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){


	// local variables
	int i, errorCount;
	unsigned int result1, result2;
        unsigned short * OUT = (unsigned short *)pOutBuf_16bpp;
        unsigned short * IN  = (unsigned short *)pSrcBuf_16bpp; 
        unsigned int * temp = (unsigned int *)pWorkingBuf_16bpp;

        for(i=0; i<16*5; i++)
               IN[i] = InData_Gauss5x5PyramidKernel_8[i];

        for(i=0;i<12*5; i++)
               temp[i] = 0;

	// call the function
	C6accel_VLIB_gauss5x5PyramidKernel_16(hC6accel,IN, 	   			// input 8-bit data
	                                                 temp,        			// temp buffer
						         16-4,											// width of input image
						         16,											// pitch of input image
							  5,
						          OUT); 	// 8-bit output data


	printf("\nTesting the function: VLIB_gauss5x5PyramidKernel_16...\n");

	// verify the results
	errorCount = 0;

	// extract results one by one and compare
	for (i=0; i<4; i++) {

		result1 = OUT[i];
		result2 = trueOutData_Gauss5x5PyramidKernel_16[i];

		if (result1 != result2) {
			printf("Error: Pixel no. %d is incorrect! \t %x \t %x  \n", i, result1, result2);
			errorCount++;
			}
		}


	if (errorCount)
    	printf("Total of %d errors: VLIB_gauss5x5PyramidKernel_16 failed the test!\n\n", errorCount);
	else
	    printf("VLIB_gauss5x5PyramidKernel_16 passed the test!\n\n");

	

return(errorCount);
}

// test input 
unsigned char InData_Gradient5x5PyramidKernel_8[16*5] = {151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6, 
				 			                      31, 143, 178,   3, 135,  91,  54, 154, 193, 161,  20, 162, 137, 150, 128, 224, 
					 	 	                     214, 113,   9,  28,  53, 211,  98, 217, 149, 233, 231, 127, 115, 203, 177,  42, 
							                      62, 155,   3, 103, 127,  16, 135, 131, 211, 158,   9,   2, 106, 227, 249, 255,
						                	     151,  57, 116, 170,   9, 247, 208, 140, 150,  60,  88,  77,   4,   6, 162,   6 };

// temporary buffer
short Temp_Gradient5x5PyramidKernel_8[12*5];

// expected output calculated off-line in Matlab
unsigned char trueOutData_GradientH5x5PyramidKernel_8[6] = { 149, 103, 109, 128, 156, 100 }; 
unsigned char trueOutData_GradientV5x5PyramidKernel_8[6] = { 137, 129, 126, 127, 139, 134 }; 

// output 
//unsigned char calculatedOutData_Gradient5x5PyramidKernel_8[6];

int C6accel_VLIB_testGradient5x5PyramidKernel_8(C6accel_Handle hC6accel,unsigned char LevelOfFeedback) {


	// local variables
	int i, errorCount;
	unsigned int result1, result2;
        unsigned char * OUT = (unsigned char *)pOutBuf_16bpp;
        unsigned char * IN  = (unsigned char *)pSrcBuf_16bpp; 
        unsigned short * temp = (unsigned short *)pWorkingBuf_16bpp;

        for(i=0; i<16*5; i++)
               IN[i] = InData_Gradient5x5PyramidKernel_8[i];

        for(i=0;i<12*5; i++)
               temp[i] = 0;

	// call the function
	C6accel_VLIB_gradientH5x5PyramidKernel_8(hC6accel, IN ,	   		// input 8-bit data
	                                                 temp,        			// temp buffer
						         16-4,											// width of input image
						         16,											// pitch of input image
							 5,
						         OUT); 	// 8-bit output data


	printf("\nTesting the function: VLIB_gradientH5x5PyramidKernel_8...\n");

	// verify the results
	errorCount = 0;

	// extract results one by one and compare
	for (i=0; i<4; i++) {

		result1 = OUT[i];
		result2 = trueOutData_GradientH5x5PyramidKernel_8[i];

		if (result1 != result2) {
			printf("Error: Pixel no. %d is incorrect! \t %x \t %x  \n", i, result1, result2);
			errorCount++;
			}
		}


	if (errorCount)
    	printf("Total of %d errors: VLIB_gradientH5x5PyramidKernel_8 failed the test!\n\n", errorCount);
	else
	    printf("VLIB_gradientH5x5PyramidKernel_8 passed the test!\n\n");

	///////////////////////////////////

	// call the function
	C6accel_VLIB_gradientV5x5PyramidKernel_8(  hC6accel, IN, 	   		// input 8-bit data
	                                                 temp,        			// temp buffer
						         16-4,											// width of input image
						         16,											// pitch of input image
						          5,
						          OUT); 	// 8-bit output data


	printf("\nTesting the function: VLIB_gradientV5x5PyramidKernel_8...\n");

	// verify the results
	errorCount = 0;

	// extract results one by one and compare
	for (i=0; i<4; i++) {

		result1 = OUT[i];
		result2 = trueOutData_GradientV5x5PyramidKernel_8[i];

		if (result1 != result2) {
			printf("Error: Pixel no. %d is incorrect! \t %x \t %x  \n", i, result1, result2);
			errorCount++;
			}
		}


	if (errorCount)
    	printf("Total of %d errors: VLIB_GradientV5x5PyramidKernel_8 failed the test!\n\n", errorCount);
	else
	    printf("VLIB_gradientV5x5PyramidKernel_8 passed the test!\n\n");

	

return(errorCount);
}


float getFractional32(int A, int point, int sign){

	int x, b, p, first,pt;
	unsigned int a;
	float c=0;
	int bin[32];

	memset(bin, 0, 32*sizeof(int));

	a = (unsigned int)A;

	// get binary notation
	x=31;
	while(a>0){
		b = a % 2;
		a = a / 2;
		bin[x--] = b;
	}


	if(sign)
		first = 1;
	else
		first = 0;

	pt = 32-point;
	for(x=pt-1,p=0;x>=first;x--,p++)
		c = c + (float)bin[x]*pow(2.0, (float)p);

	for(x=pt,p=-1;x<32;x++,p--)
		c = c + (float)bin[x]*pow(2.0, (float)p);

	if((sign==1) && (A<0))
		c = -pow(2.0,31-point)+c;
	
	return(c);
 
}

#define MAX_U16_B 65536

void C6accel_VLIB_testBhattacharyaDistance_U32(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){
	
	int i;
	unsigned short H1[] = {1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 1, 0, 5, 0, 0, 0, 0, 1, 0, 3, 0, 2, 0, 0, 3, 0, 6, 0, 0, 3, 0, 6, 0, 10, 0, 0, 18, 0, 20, 0, 24, 0, 0, 35, 0, 46, 0, 53, 0, 0, 81, 0, 99, 0, 0, 132, 0, 150, 0, 212, 0, 0, 218, 0, 203, 0, 213, 0, 0, 220, 0, 249, 0, 0, 248, 0, 219, 0, 227, 0, 0, 231, 0, 234, 0, 214, 0, 0, 218, 0, 229, 0, 0, 207, 0, 202, 0, 198, 0, 0, 213, 0, 188, 0, 149, 0, 0, 147, 0, 150, 0, 118, 0, 0, 106, 0, 80, 0, 0, 96, 0, 80, 0, 69, 0, 0, 73, 0, 70, 0, 75, 0, 0, 78, 0, 76, 0, 0, 74, 0, 85, 0, 80, 0, 0, 84, 0, 79, 0, 96, 0, 0, 108, 0, 92, 0, 0, 108, 0, 93, 0, 101, 0, 0, 114, 0, 116, 0, 124, 0, 0, 112, 0, 120, 0, 120, 0, 0, 108, 0, 117, 0, 0, 100, 0, 116, 0, 99, 0, 0, 98, 0, 78, 0, 114, 0, 0, 109, 0, 108, 0, 0, 93, 0, 99, 0, 84, 0, 0, 71, 0, 70, 0, 62, 0, 0, 84, 0, 99, 0, 0, 111, 0, 110, 0, 117, 0, 0, 137, 0, 146, 0, 172, 0, 0, 134, 0, 168, 0, 146, 0, 0, 146, 0, 155, 0, 0, 120, 0, 144, 0, 113, 0, 0, 123, 0, 97, 0, 84, 0, 0, 73, 0, 85, 0, 0, 57, 0, 43, 0, 40, 0, 0, 27, 0, 27, 0, 27, 0, 0, 24, 0, 24, 0, 0, 14, 0, 19, 0, 16, 0, 0, 14, 0, 17, 0, 14, 0, 0, 10, 0, 12, 0, 11, 0, 0, 9, 0, 13, 0, 0, 12, 0, 16, 0, 7, 0, 0, 15, 0, 5, 0, 10, 0, 0, 10, 0, 7, 0, 0, 10, 0, 10, 0, 7, 0, 0, 10, 0, 4, 0, 9, 0, 0, 9, 0, 4, 0, 0, 7, 0, 6, 0, 10, 0, 0, 9, 0, 12, 0, 3, 0, 0, 8, 0, 9, 0, 8, 0, 0, 4, 0, 2, 0, 0, 3, 0, 10, 0, 7, 0, 0, 9, 0, 8, 0, 5, 0, 0, 8, 0, 5, 0, 0, 8, 0, 10, 0, 9, 0, 0, 10, 0, 9, 0, 5, 0, 0, 11, 0, 8, 0, 0, 17, 0, 16, 0, 10, 0, 0, 11, 0, 14, 0, 21, 0, 0, 21, 0, 22, 0, 29, 0, 0, 22, 0, 36, 0, 0, 23, 0, 34, 0, 47, 0, 0, 46, 0, 66, 0, 107, 0, 0, 203, 0, 171, 0, 0, 815, 0, 325, 0, 260, 0, 0, 97, 0, 37, 0, 26, 0, 0, 24, 0, 16, 0, 0, 23, 0, 17, 0, 9, 0, 0, 3, 0, 7, 0, 5, 0, 0, 0, 0, 2, 0, 2};
	unsigned short H2[] = {1, 0, 0, 0, 0, 3, 0, 2, 0, 0, 4, 0, 19, 0, 0, 23, 0, 55, 0, 0, 89, 0, 104, 0, 0, 154, 0, 0, 178, 0, 254, 0, 0, 270, 0, 316, 0, 0, 375, 0, 394, 0, 0, 401, 0, 381, 0, 0, 389, 0, 397, 0, 0, 419, 0, 0, 353, 0, 339, 0, 0, 317, 0, 275, 0, 0, 265, 0, 249, 0, 0, 208, 0, 212, 0, 0, 193, 0, 165, 0, 0, 177, 0, 0, 184, 0, 142, 0, 0, 141, 0, 125, 0, 0, 121, 0, 105, 0, 0, 99, 0, 105, 0, 0, 71, 0, 61, 0, 0, 70, 0, 0, 55, 0, 47, 0, 0, 46, 0, 45, 0, 0, 41, 0, 37, 0, 0, 29, 0, 39, 0, 0, 41, 0, 0, 39, 0, 33, 0, 0, 36, 0, 30, 0, 0, 29, 0, 37, 0, 0, 19, 0, 28, 0, 0, 30, 0, 24, 0, 0, 28, 0, 0, 25, 0, 34, 0, 0, 30, 0, 29, 0, 0, 33, 0, 24, 0, 0, 20, 0, 32, 0, 0, 32, 0, 35, 0, 0, 42, 0, 0, 25, 0, 37, 0, 0, 37, 0, 41, 0, 0, 33, 0, 44, 0, 0, 43, 0, 42, 0, 0, 48, 0, 47, 0, 0, 55, 0, 0, 70, 0, 81, 0, 0, 104, 0, 105, 0, 0, 107, 0, 94, 0, 0, 127, 0, 108, 0, 0, 113, 0, 88, 0, 0, 79, 0, 0, 88, 0, 73, 0, 0, 67, 0, 60, 0, 0, 60, 0, 52, 0, 0, 38, 0, 27, 0, 0, 33, 0, 0, 24, 0, 31, 0, 0, 25, 0, 30, 0, 0, 15, 0, 11, 0, 0, 17, 0, 10, 0, 0, 7, 0, 11, 0, 0, 14, 0, 0, 16, 0, 8, 0, 0, 6, 0, 14, 0, 0, 13, 0, 7, 0, 0, 7, 0, 8, 0, 0, 14, 0, 11, 0, 0, 6, 0, 0, 11, 0, 8, 0, 0, 8, 0, 6, 0, 0, 6, 0, 8, 0, 0, 6, 0, 9, 0, 0, 8, 0, 9, 0, 0, 11, 0, 0, 12, 0, 8, 0, 0, 9, 0, 14, 0, 0, 5, 0, 8, 0, 0, 9, 0, 6, 0, 0, 9, 0, 4, 0, 0, 10, 0, 0, 12, 0, 9, 0, 0, 14, 0, 8, 0, 0, 9, 0, 14, 0, 0, 10, 0, 11, 0, 0, 15, 0, 0, 6, 0, 8, 0, 0, 7, 0, 9, 0, 0, 6, 0, 9, 0, 0, 12, 0, 9, 0, 0, 8, 0, 7, 0, 0, 12, 0, 0, 15, 0, 9, 0, 0, 15, 0, 16, 0, 0, 21, 0, 24, 0, 0, 15, 0, 25, 0, 0, 28, 0, 28, 0, 0, 35, 0, 0, 41, 0, 58, 0, 0, 74, 0, 150, 0, 0, 349, 0, 362, 0, 0, 689, 0, 1114, 0, 0, 71, 0, 77, 0, 0, 46, 0, 0, 31, 0, 29, 0, 0, 35, 0, 13, 0, 0, 7, 0, 7, 0, 0, 5, 0, 2, 0, 0, 4, 0, 1};
	int N = 512;
	unsigned short normH = MAX_U16_B/15360;
	unsigned int gt_UBD = 3445728529u;
	unsigned int *UBD = (unsigned int *)pWorkingBuf_16bpp;;
        unsigned short * pH1 = (unsigned short *)pOutBuf_16bpp;
        unsigned short * pH2  = (unsigned short *)pSrcBuf_16bpp; 

        for(i=0;i<512; i++){
                pH1[i] = H1[i];
                pH2[i] = H2[i];
         }
                

    if(LevelOfFeedback>0)
    {
	printf("Histogram 1: ");
	for(i=0;i<N;i++)
		printf("%d, ", H1[i]);
	printf("\n");

	printf("Histogram 2: ");
	for(i=0;i<N;i++)
		printf("%d, ", H2[i]);
	printf("\n");

    }
	// normalize histograms to make sure they sum to 1
	for(i=0;i<N;i++){
		pH1[i] = pH1[i] * normH;
		pH2[i] = pH2[i] * normH;
	}

	C6accel_VLIB_bhattacharyaDistance_U32(hC6accel, pH1, pH2, N, UBD);
        
    if(LevelOfFeedback>0)
        printf("Bhattacharya Distance = %f\n", getFractional32(UBD[0], 32, 0));

    printf("\n\nTesting the function: VLIB_bhattacharyaDistance_U32...\n");
	if(UBD[0] == gt_UBD)
            printf("VLIB_bhattacharyaDistance_U32 passed the test!\n");
        else
	    printf("VLIB_bhattacharyaDistance_U32 failed the test!\n");
	
}

void C6accel_VLIB_testL1DistanceS16(C6accel_Handle hC6accel,unsigned char LevelOfFeedback)
{
	
	int i;
	short H1[] = {100, 200, -100, 5, 23, 10, 4, 33}; 
	short H2[] = {28, -10, -100, 32767, -32757, 0, 1, -28}; 
	int N = 8;
	unsigned int gt_L1D = 65885;
	unsigned int *L1D = (unsigned int *)pRefBuf_16bpp;;
        unsigned short * pH1 = (unsigned short *)pOutBuf_16bpp;
        unsigned short * pH2  = (unsigned short *)pSrcBuf_16bpp; 

    if(LevelOfFeedback>0)
    {
	printf("Vector 1: ");
	for(i=0;i<N;i++)
		printf("%d, ", H1[i]);
	printf("\n");

	printf("Vector 2: ");
	for(i=0;i<N;i++)
		printf("%d, ", H2[i]);
	printf("\n");
    }
        for(i=0;i<8; i++){
                pH1[i] = H1[i];
                pH2[i] = H2[i];
         } 

	C6accel_VLIB_L1DistanceS16(hC6accel, pH1, pH2, N, L1D);

    printf("\n\nTesting the function: VLIB_L1DistanceS16...\n");
    if(LevelOfFeedback>0)
	printf("L1 Distance = %d\n", L1D[0]);

    if(L1D[0] == gt_L1D)
		   printf("VLIB_L1DistanceS16 functions passed the test\n");
	else
		    printf("VLIB_L1DistanceS16 functions failed the test\n");

}

static void CreatePattern3(unsigned char *p, int width, int height )
{
	int i;
	for (i = 0; i < width*height; i++)
	{
		int x = i%64;
		int y = i/64;

		p[i] = (x | y) / 2 + 127;
	}
}


int C6accel_VLIB_testHarrisScore_7x7(C6accel_Handle hC6accel,unsigned char LevelOfFeedback)
{
	int i;

	int inds[] = {64*8+12, 64*23+30, 64*40+20, 64*15+48};
	int truth[] = {4680, 0, 4794, 10091};

	int errorCount = 0;
        short *GRADMAG = ( short *)pWorkingBuf2_16bpp;
        short *GRADY = ( short *)pWorkingBuf_16bpp;
        short * GRADX = ( short *)pOutBuf_16bpp;
        unsigned char * IMAGE1  = (unsigned char *)pSrcBuf_16bpp; 
        short * OUTM  = ( short *)pRefBuf_16bpp;  
	CreatePattern3(IMAGE1,64,64);

	C6accel_VLIB_xyGradientsAndMagnitude(hC6accel, IMAGE1, GRADX+64+1, GRADY+64+1, GRADMAG+64+1, 64, 64); //Pointer starts from the image one row and one column less

	printf("Testing VLIB_harrisScore_7x7...\n");

	C6accel_VLIB_harrisScore_7x7(hC6accel, GRADX, GRADY, 64, 64, OUTM, 1310);

	for (i = 0; i < 4; i++)
	{
		if (OUTM[inds[i]] != truth[i])
		{
			printf("Error: the corner value for pixel no. %d is incorrect!, %d, %d\n", i, OUTM[inds[i]], truth[i]);
			errorCount++;	
		}
	}

	if (errorCount)
    	printf("Total of %d errors: VLIB_harrisScore_7x7 failed the test!\n\n", errorCount);
	else
	    printf("VLIB_harrisScore_7x7 passed the test!\n\n");

	return 0;
}

static void CreatePattern4(unsigned char *p, int width, int height)
{
	int i;
	for (i = 0; i < width*height; i++)
	{
		float x = i%64;
		float y = i/64;

		//int x1 = x / 100 - (y * 99) / 100;
		//int y1 = (x * 99) / 100 + y / 100;
		int x1 = x-1;
		int y1 = y-1;

		p[i] = (x1 | y1) / 2 + 127;
	}
}

#define NFEATURES 4
int C6accel_VLIB_testTrackFeaturesLucasKanade_7x7(C6accel_Handle hC6accel)
{	
	// Sample a bunch of points in the interior of the image
	short X[NFEATURES] = {15.3f * 16, 25.2f * 16, 49.0f * 16, 20.2f * 16};
	short Y[NFEATURES] = {14.7f * 16, 32.9f * 16, 32.1f * 16, 40.9f * 16};

	
        short *GRADMAG = ( short *)pWorkingBuf2_16bpp;
        short *GRADY = ( short *)pWorkingBuf_16bpp;
        short * GRADX = ( short *)pOutBuf_16bpp;
        unsigned char * IMAGE1  = (unsigned char *)pSrcBuf_16bpp; 
        unsigned char * IMAGE2 = (unsigned char *)pRefBuf_16bpp;  
        short * mX; //Size =[NFEATURES];
	short * mY; //Sie =[NFEATURES];
        short *pX, *pY;

	// True values (from the PC/C++ mirror version which has been sanity 
	// sanity checked using real images
	short trueX[NFEATURES] = {17, 14, 11, 15};
	short trueY[NFEATURES] = {17, 16, 14, 15};

	//unsigned char BUFFER[384];

	int i;
	int errorCount = 0;

	// Synthetic motion sequence
	CreatePattern3(IMAGE1,64, 64);
        CreatePattern4(IMAGE2,64,64);
	
	

	// Compute the gradient using the library function
	C6accel_VLIB_xyGradientsAndMagnitude(hC6accel, IMAGE1, GRADX+64+1, GRADY+64+1, GRADMAG+64+1, 64, 64); //Pointer starts from the image one row and one column less
        
        mX = ( short *)pWorkingBuf2_16bpp;
        mY = ( short *)(mX+ NFEATURES*sizeof(short));
        pX =  ( short *)(mY+ NFEATURES*sizeof(short));
        pY =  ( short *)(pX+ NFEATURES*sizeof(short));
        
	printf("Testing VLIB_trackFeaturesLucasKanade_7x7...\n");


        // Initial guesses for the feature points
	for (i = 0; i < NFEATURES; i++)
	{
		mX[i] = X[i];
		mY[i] = Y[i];
                pX[i] = X[i];
                pY[i] = Y[i];
	}

	C6accel_VLIB_trackFeaturesLucasKanade_7x7(hC6accel,IMAGE1, IMAGE2, GRADX, GRADY, 64, 64, NFEATURES, pX, pY, mX, mY, 10);

	for (i = 0; i < NFEATURES; i++)
	{
		//printf("mX[%d]: %d   mY[%d]: %d\n", i, (int)(mX[i] - X[i]), i, (int)(mY[i]-Y[i]));
		if ((mX[i] - pX[i]) != trueX[i] || (mY[i]-pY[i]) != trueY[i])
		{
			printf("Error: the location for feature no. %d is incorrect!\n", i);
			errorCount++;	
		}
	}

	if (errorCount)
    	printf("Total of %d errors: VLIB_trackFeaturesLucasKanade_7x7 failed the test!\n", errorCount);
	else
	    printf("VLIB_trackFeaturesLucasKanade_7x7 passed the test!\n");


	return 0;
}

extern short TableSineBK_Minus80toPlus80resZeroPointSix_Q8[267];
extern short TableCosineBK_Minus80toPlus80resZeroPointSix_Q8[267];
extern unsigned short pEdgeMapList[2*3835];
extern unsigned short trueOutHoughSpace[267*267];

//unsigned short calculatedOutHoughSpace[267*267];
//unsigned short ping[267]; 
//unsigned short pong[267];
//unsigned short pang[267]; 
//unsigned short peng[267];


int 
VLIB_InitializeTrigLUTs( short* restrict pSin, 
                         short* restrict pCos,
                         float ThetaMin,
                         float ThetaMax,
                         float NumThetaPoints )
{

	unsigned int i;
    float Theta;

    for(i = 0; i < NumThetaPoints; i++)
    {
        Theta = ThetaMin + i*(ThetaMax - ThetaMin)/(NumThetaPoints-1);

		*(pSin + i) = (short)(floor(256*sin(Theta*0.017453) + 0.5));
		*(pCos + i) = (short)(floor(256*cos(Theta*0.017453) + 0.5));

		// pTRIG[i] = (*(pCos + i)) | ((int)(*(pSin + i)) << 16);;

    }
  	return 0;
}




int C6accel_VLIB_testHoughForLines(C6accel_Handle hC6accel) {

	int i, errorCount;
	short result1, result2;

        short *SinLUT = (short *)pRefBuf_16bpp;
        short *CosLUT = (short *)pWorkingBuf_16bpp;
        unsigned short *calculatedOutHoughSpace = ( short *)pOutBuf_16bpp;
        unsigned short *Edgelist = ( short *)pSrcBuf_16bpp;

        VLIB_InitializeTrigLUTs(SinLUT, CosLUT, -80, 79.6, 267);

	// verify the results
	errorCount = 0;

	// extract results one by one and compare
	for (i=0; i<267; i++) {

		result1 = SinLUT[i];
		result2 = TableSineBK_Minus80toPlus80resZeroPointSix_Q8[i];

		if (result1 != result2) {
			printf("Error: Sin value at location %d is incorrect!\n", i);
			errorCount++;
			}
		}

	// extract results one by one and compare
	for (i=0; i<267; i++) {

		result1 = CosLUT[i];
		result2 = TableCosineBK_Minus80toPlus80resZeroPointSix_Q8[i];

		if (result1 != result2) {
			printf("Error: Cos value at location %d is incorrect!\n", i);
			errorCount++;
			}
		}

    	printf("\nTesting the function: VLIB_houghLineFromList...\n");

    for (i=0; i<(267*267); i++) {
        *(calculatedOutHoughSpace + i) = 0;
     }

    for(i=0; i<2*3835; i++)
        Edgelist[i] = pEdgeMapList[i]; 		
		
	C6accel_VLIB_houghLineFromList( hC6accel,
                                    Edgelist, 
                                    calculatedOutHoughSpace, 
                                    640, // outBlkWidth              
                                    480, // outBlkHeight             
				    3835,   // listSize
                                    267, // thetaRange, 
                                    267, // rhoMaxLength
				    SinLUT, 
                         	    CosLUT 
                               	);

	// verify the results
	errorCount = 0;

	// extract results one by one and compare
	for (i=0; i<(267*267); i++) {

		result1 = calculatedOutHoughSpace[i];
		result2 = trueOutHoughSpace[i];

		if (abs(result1-result2)>2) {
			
                        printf("i: %d, result1: %d, result2%d\n", i, result1,result2);
                       	errorCount++;
			}
	}

	if (errorCount)
    	printf("Total of %d errors: VLIB_houghLineFromList failed the test!\n", errorCount);
	else
	    printf("VLIB_houghLineFromList passed the test!\n\n");

	return(errorCount);
}


float getFractional16(short A, int point, int sign){

	int x, b, p, first,pt;
	unsigned short a;
	float c=0;
	int bin[16];


	memset(bin, 0, 16*sizeof(int));

	a = (unsigned short)A;
//	if(a<0)
//		a = -a;

	// get binary notation
	x=15;
	while(a>0){
		b = a % 2;
		a = a / 2;
		bin[x--] = b;
	}


	if(sign)
		first = 1;
	else
		first = 0;

	pt = 16-point;
	for(x=pt-1,p=0;x>=first;x--,p++)
		c = c + (float)bin[x]*pow(2.0, (float)p);

	for(x=pt,p=-1;x<16;x++,p--)
		c = c + (float)bin[x]*pow(2.0, (float)p);

	if((sign==1) && (A<0))
		c = -pow(2.0,15-point)+c;

	
	return(c);

}

float rIm[400];
void LegendreMoments_imageReconstruct(float* rIm, int im_h, int im_w, int order, short *Pr, short *Pc, short *L);

	 // Input Image
	unsigned char origIm[400] = {
	 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	 0,     0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,
	 0,     0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,
	 0,     0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,
	 0,     0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,
	 0,     0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,
	 0,     0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,
	 0,     0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,
	 0,     0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,
	 0,     0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,
	 0,     0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,
	 0,     0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,
	 0,     0,     0,     0,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,   255,     0,     0,     0,     0,
	 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0
	};



void C6accel_VLIB_testLegendreMoments(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){


	int i, j, k=0;
	float diffSum, MSE;


	// Image dimension
	int ImH = 20;

	// Maximum order 
	int Order = 6;
	 
	short *Factor1= (short *)pWorkingBuf2_16bpp;

	// Array to hold the Legendre moments
	short *LegMoments = (short *)pRefBuf_16bpp; //Size =[49];

	// Array to hold the Legendre polynomials
	short *LPoly = (short *)pWorkingBuf_16bpp;    //Size (6+1)*(20)    

	// Ground-truth Legendre Moments
	short GT_LMoments[49] = {
		12888,	-45,	-19540,	-56,	1755,	-77,	11024,
		-36,	-54,	-30,	-126,	-162,	-198,	-312,
		-19560,	-135,	29300,	-350,	-3195,	-605,	-17485,
		0,	0,	0,	0,	0,	0,	0,
		1728,	-243,	-3150,	-630,	-648,	-990,	468,
		0,	0,	0,	0,	0,	0,	0,
		10920,	-351,	-17290,	-1001,	234,	-1287,	8112
	};

	// Array for the normalized image
	short *Im= ( short *)pSrcBuf_16bpp; //Size =[400]

	// normalize the image
	for(i=0;i<ImH;i++)
		for(j=0;j<ImH;j++)
			Im[i*ImH+j] = (short)((32767/255) * origIm[i*ImH+j]);

	// Set the moments and the polynomial to 0
	memset(LegMoments, 0, (Order+1)*(Order+1)*sizeof(short));
	memset(LPoly, 0, (Order+1)*(ImH)*sizeof(short));

        if(LevelOfFeedback>0)
       {
	 printf("Order = %d\n", Order);
	 printf("ImH = %d\n", ImH);	   
       }

	printf("\nInitializing Legendre Polynomial\n");
	C6accel_VLIB_legendreMoments_Init(hC6accel,LPoly, Order, ImH, Factor1);

	printf("\nComputing Legendre Moments\n");
	C6accel_VLIB_legendreMoments(hC6accel,Im, LegMoments, LPoly, Order, ImH, Factor1[0]);

	// In subsequent calls to VLIB_legendreMoments where Im is different, but ImH is the same, 
	//	it is beneficial to retain the same LPoly array. LPoly depends only on the ImH. 

    if(LevelOfFeedback>0)
    {

	printf("\nLegendre Polynomial:\n");
	for(i=0;i<=Order;i++){
		for(j=0;j<ImH;j++){
			printf("%.2f ", getFractional16(LPoly[i*(ImH)+j], 15, 1));
		}
		printf("\n");
	}

	printf("\nLegendre Moments:\n");
	for(i=0;i<=Order;i++){
		for(j=0;j<=Order;j++){
			if ((i+j) <= Order)
				printf("%.5f ", getFractional16(LegMoments[i*(Order+1)+j], 15, 1));
		}
		printf("\n");
	}

    }
	// Reconstructing the image
	memset(rIm, 0, sizeof(float)*400);
	LegendreMoments_imageReconstruct(rIm, ImH, ImH, Order, LPoly, LPoly, LegMoments);

	// Compute the mean squared error
	diffSum = 0;
	for(i=0;i<ImH*ImH;i++)
		diffSum = diffSum + ( (float)origIm[i]/255.0 - rIm[i]) * ( (float)origIm[i]/255.0 - rIm[i] );
	
	MSE = diffSum / 400;
    if(LevelOfFeedback>0)
       printf("\nMean Square Error (between original image and reconstructed image) = %.5f\n", MSE);

	// Verify if outputs match ground truth
        printf("\n\nTesting the function: VLIB_legendreMoments...\n");
	for(i=0;i<=Order;i++){
		for(j=0;j<=Order;j++){
			if(LegMoments[i*Order+j] != GT_LMoments[i*Order+j]){
                if(LevelOfFeedback>0)
				printf("Discrepancy in Legendre Moment, row %d, col %d\n", i, j);
				k = 1;
			}
		} 
	}

	if(k==0)
		printf("VLIB_legendreMoments passed the test! \n");
    else
        printf("VLIB_legendreMoments failed the test! \n");	

	
}


void LegendreMoments_imageReconstruct(float* rIm, int im_h, int im_w, int order, short *Pr, short *Pc, short *L){

	int i, j, p, q;
	int temp1, temp3;
	short temp2;

	// reconstruct from the moments
	for(i=0;i<im_h;i++){
		for(j=0;j<im_w;j++){
			rIm[i*im_w + j] = 0;
			for(p=0;p<=order;p++){
				for(q=0;q<=p;q++){
					// S0.15 * S0.15 = SS0.30
					temp1 = (int)(*(Pc+((p-q)*im_w+j))) * (int)(*(Pr+(q*im_h+i)));

					// S0.15
					temp2 = (short)(temp1 >> 15);

					// S0.15 * S0.15 = SS0.30
					temp3 = (int)*(L+((p-q)*(order+1)+q)) *  (int)temp2;

					rIm[i*im_w+j] = rIm[i*im_w+j] +  getFractional16((short)(temp3 >> 15), 15, 1);
					
				}
			}

		}			
	}

}

#define imW 8
#define imH 8

void C6accel_VLIB_testNormalFlow(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){

	int i, j, errors=0;
	short *normalFlowX = (short *)pRefBuf_16bpp; //size= [imW*imH];
	short *normalFlowY = ( short *)pOutBuf_16bpp; //Size =[imW*imH];

	// Ground-truth Arrays
	//---------------------
	short gt_nFlowX[] = {
		0, 	0, 	-2, -2, 0, 	2, -1, -3, 
		-4, 0, 	8, 	1, 	1, 	1, 	2, 	1, 
		-282, 229, 	155, 	0, 	11, 55,	-10, -176, 
		129, 	0, 	0, 	0, 	0, 	11, 0, 	144, 
		0, 	-15, 	110, 	9, 	0, 	0, 	0, 	0, 
		0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 
		0, 	0, 	0, 	-39, 	0, 	0, 	0, 	0, 
		0, 	0, 	0, 	-160, 	0, 	0, 	0, 	0 
	};
	short gt_nFlowY[] = {
		-27, 	-34, 	-34, 	-25, 	-26, 	-28, 	-36, 	-29, 
		83, 	89, 	83, 	84, 	84, 	82, 	80, 	89, 
		234, 	564, 	117, 	285, 	244, 	244, 	254, 	311, 
		-22, 	0, 	0, 	0, 	0, 	2, 	0, 	-50, 
		0, 	108, 	39, 	4, 	0, 	0, 	0, 	0, 
		0, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 
		0, 	0, 	0, 	50, 	0, 	0, 	0, 	0, 
		0, 	0, 	0, 	-76, 	0, 	0, 	0, 	0, 
	};


	// Input Arrays
	//--------------
	// Image difference array
       short * pImDiff =  ( short *)pSrcBuf_16bpp;
	short imDiff[] = {
	   -22,    -29,    -30,    -24,    -24,    -29,    -38,    -32, 
	   117,    119,    121,    127,    128,    128,    130,    138, 
	   114,     65,     94,    110,     95,    100,     95,     98, 
	   -39,    -22,      0,      0,    -22,      0,      0,    -24, 
	     0,     36,     36,      0,      0,      0,      0,      0, 
	     0,      0,      0,      0,      0,      0,      0,      0, 
	     0,      0,      0,    -28,      0,      0,      0,      0, 
	     0,      0,      0,    -29,      0,    -21,      0,      0 
	};

        

	// Gradient magnitude array
        short * pEmag =  ( short *)pWorkingBuf_16bpp;
	short Emag[] = {
	   101,    105,    110,    114,    116,    124,    132,    133,  
	   181,    174,    187,    196,    195,    202,    210,    201,  
	    38,     15,     60,     50,     50,     53,     48,     35,  
	    38,     10,     59,      3,      6,     12,      9,     21,  
	    62,     43,     42,     13,     10,      7,      6,     19,  
	    19,     29,     15,     42,     17,      3,      5,      5,  
	     6,     33,     43,     54,     48,     18,     20,     15,  
	     6,     26,     50,     21,     45,      9,      5,      9  
	};

	// X-direction gradient array
        short * pEx = ( short *)(pEmag+ sizeof(short)*imW*imH);
	short Ex[] = {
	     2,      2,     -4,     -5,      0,     10,     -1,    -13,
	     8,     -1,    -19,     -3,     -2,     -3,     -5,     -3,
	    28,     -6,    -46,      0,     -2,    -12,      2,     17,
	    38,     -9,    -59,     -2,     -3,    -12,      9,     21,
	    32,      6,    -42,    -12,     -1,     -6,      3,      8,
	    16,     25,     -3,    -26,    -10,      0,     -2,      0,
	     5,     33,     37,    -31,    -35,      5,     -2,     -4,
	     6,     26,     50,    -19,    -45,      5,     -5,     -9
	};

	// Y-direction gradient array
        short * pEy = ( short *)(pEx+ sizeof(short)*imW*imH);
	short Ey[] = {
	  -101,   -105,   -110,   -114,   -116,   -124,   -132,   -133, 
	  -181,   -174,   -187,   -196,   -195,   -202,   -210,   -201, 
	   -23,    -15,    -35,    -50,    -50,    -53,    -48,    -30, 
	    -6,     -5,     -1,     -2,     -6,     -3,     -1,     -7, 
	   -51,    -43,    -15,     -6,    -10,     -4,     -5,    -18, 
	   -10,    -14,     15,     31,     13,      3,      5,      5, 
	     3,      3,     21,     41,     29,     18,     20,     15, 
	     2,     -4,    -15,     -9,      5,      7,      2,     -1 
	};

        for(i=0; i<imW*imH;i++){
              pImDiff[i] = imDiff[i];
              pEx[i] = Ex[i];
              pEy[i]  = Ey[i];
              pEmag[i] =Emag[i];
         }

	// Look-Up Table for values in Emag array
       short *pLUT =  ( short *)pWorkingBuf2_16bpp; 
	short LUT[] = {
		0, 32767, 16383, 10922, 8191, 6553, 5461, 4681, 4095, 3640, 3276, 2978, 2730, 2520, 2340, 2184, 2047, 1927, 1820, 1724, 1638, 1560, 1489, 
		1424, 1365, 1310, 1260, 1213, 1170, 1129, 1092, 1057, 1023, 992, 963, 936, 910, 885, 862, 840, 819, 799, 780, 762, 744, 728, 712, 697, 
		682, 668, 655, 642, 630, 618, 606, 595, 585, 574, 564, 555, 546, 537, 528, 520, 511, 504, 496, 489, 481, 474, 468, 461, 455, 448, 442, 
		436, 431, 425, 420, 414, 409, 404, 399, 394, 390, 385, 381, 376, 372, 368, 364, 360, 356, 352, 348, 344, 341, 337, 334, 330, 327, 324, 
		321, 318, 315, 312, 309, 306, 303, 300, 297, 295, 292, 289, 287, 284, 282, 280, 277, 275, 273, 270, 268, 266, 264, 262, 260, 258, 255, 
		254, 252, 250, 248, 246, 244, 242, 240, 239, 237, 235, 234, 232, 230, 229, 227, 225, 224, 222, 221, 219, 218, 217, 215, 214, 212, 211, 
		210, 208, 207, 206, 204, 203, 202, 201, 199, 198, 197, 196, 195, 193, 192, 191, 190, 189, 188, 187, 186, 185, 184, 183, 182, 181, 180, 
		179, 178, 177, 176, 175, 174, 173, 172, 171, 170, 169, 168, 168, 167, 166, 165, 164, 163, 163, 162, 161, 160, 159, 159, 158, 157, 156, 
		156, 155, 154, 153, 153, 152, 151, 151, 150, 149, 148, 148, 147, 146, 146, 145, 144, 144, 143, 143, 142, 141, 141, 140, 140, 139, 138
	};
        for(i=0; i< 255; i++)
            pLUT[i] = LUT[i]; 

	 C6accel_VLIB_normalFlow_16(hC6accel,  pImDiff,
						pEmag,
						pEx,
						pEy,
						pLUT,
						10, 
						imW,
                                                imH,
						normalFlowX,
						normalFlowY);
       

    if(LevelOfFeedback>0)
    {
	printf("Normal Flow: X direction\n");
	for(i=0;i<imH;i++){
		for(j=0;j<imW;j++){
			printf("%f, ", getFractional16(normalFlowX[i*imW+j], 7, 1));
		}
		printf("\n");
	}
	printf("\n");

	printf("Normal Flow: Y direction\n");
	for(i=0;i<imH;i++){
		for(j=0;j<imW;j++){
			printf("%f, ", getFractional16(normalFlowY[i*imW+j], 7, 1));
		}
		printf("\n");
	}
	printf("\n");
    }

	// Verify computed result against pre-stored ground truth
    if(LevelOfFeedback>0)
	printf("Verifying normal flow X direction...\n");
	for(i=0;i<imH;i++){
		for(j=0;j<imW;j++){
			if(normalFlowX[i*imW+j] != gt_nFlowX[i*imW+j])
                {
                if(LevelOfFeedback>0)
				   printf("\tDiscrepancy in row=%d, col=%d! (value=%d)\n", i, j,normalFlowX[i*imW+j] - gt_nFlowX[i*imW+j]);
                errors=errors+1;
                }
		}
	}	

    if(LevelOfFeedback>0)
	printf("Verifying normal flow Y direction...\n");
	for(i=0;i<imH;i++){
		for(j=0;j<imW;j++){
			if(normalFlowY[i*imW+j] != gt_nFlowY[i*imW+j])
             {
             if(LevelOfFeedback>0)
				printf("\tDiscrepancy in row=%d, col=%d!  (value=%d) \n", i, j,normalFlowY[i*imW+j] - gt_nFlowX[i*imW+j]);
                errors=errors+1;
             }
		}
	}	

    printf("\n\nTesting the function: VLIB_normalFlow_16...\n");
	if(errors == 0)
		printf("VLIB_normalFlow_16 passed the test! \n");
    else
      printf("VLIB_normalFlow_16 failed the test! \n");

	
}


#define MAX_U16 65535
#define MAX_S16 32768
#define MAX_U32 4294967295
#define MAX_S32 2147483647
#define MAX_8p8 256
#define MAX_0p16 1
#define MAX_12p4 4096
#define MAX_4p12 16
#define MAX_16p16 65536
#define MAX_13p2 8192
#define MAX_10p5 1024


int C6accel_VLIB_testKalmanFilter_2x4(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){

	int i, index;
	short *Z =  ( short *)pSrcBuf_16bpp; //Size =2
        short *Residual =  ( short *)pRefBuf_16bpp;//Size =2


	// 50 measurements
	short Y[] = {1, 99, 2, 84, 3, 98, 4, 100, 5, 103, 6, 109, 7, 88, 8, 87, 9, 120, 10, 127, 11, 109, 12, 94, 13, 105, 14, 101, 15, 79, 16, 100, 17, 98, 
	18, 104, 19, 100, 20, 64, 21, 88, 22, 85, 23, 92, 24, 81, 25, 97, 26, 90, 27, 105, 28, 93, 29, 105, 30, 86, 31, 80, 32, 90, 33, 106, 34, 111, 
	35, 93, 36, 115, 37, 98, 38, 101, 39, 97, 40, 94, 41, 102, 42, 105, 43, 97, 44, 73, 45, 121, 46, 90, 47, 119, 48, 110, 49, 73, 50, 99};  

	// state after 50 iterations
	short testResult[] = {1288, 3294, 5, 2};

	
	// declare instance of VLIB_kalmanFilter_2x4
	//VLIB_kalmanFilter_2x4 KF= ;
	VLIB_kalmanFilter_2x4 *KFtest = (VLIB_kalmanFilter_2x4 *)C6accel_Kalmanfilter2x4_create();
       
	// initialize struct elements to 0
	memset(KFtest->transition, 0, sD_2x4*sD_2x4*sizeof(short));
	memset(KFtest->errorCov, 0, sD_2x4*sD_2x4*sizeof(short));
	memset(KFtest->predictedErrorCov, 0, sD_2x4*sD_2x4*sizeof(short));
	memset(KFtest->processNoiseCov, 0, sD_2x4*sD_2x4*sizeof(short));
	memset(KFtest->measurementNoiseCov, 0, mD_2x4*mD_2x4*sizeof(short));
	memset(KFtest->measurement, 0, sD_2x4*mD_2x4*sizeof(short));
	memset(KFtest->state, 0, sD_2x4*sizeof(short));
	memset(KFtest->predictedState, 0, sD_2x4*sizeof(short));
	memset(KFtest->kalmanGain, 0, sD_2x4*mD_2x4*sizeof(short));
	memset(KFtest->temp1, 0, sD_2x4*sD_2x4*sizeof(short));
	memset(KFtest->temp2, 0, sD_2x4*sD_2x4*sizeof(short));
	memset(KFtest->temp3, 0, sD_2x4*sD_2x4*sizeof(short));

	// initialize matrices
	for(i=0;i<sD_2x4;i++){		
		index = i*sD_2x4 + i;
		KFtest->errorCov[index] = 1000 * MAX_S16/MAX_13p2;
		KFtest->transition[index] = 1;				
		if(i < mD_2x4)			
			KFtest->measurement[index] = 1;		
	}
	KFtest->measurementNoiseCov[0] = 8100;// * MAX_S16 / MAX_13p2;
	KFtest->measurementNoiseCov[3] = 8100;// * MAX_S16 / MAX_13p2;

	KFtest->transition[2] = 1;
	KFtest->transition[7] = 1;

	KFtest->processNoiseCov[0] = 0.333 * MAX_S16 / MAX_13p2;
	KFtest->processNoiseCov[1] = 0.50 * MAX_S16 / MAX_13p2;
	KFtest->processNoiseCov[4] = 0.50 * MAX_S16 / MAX_13p2;
	KFtest->processNoiseCov[5] = 1.0 * MAX_S16 / MAX_13p2;
	KFtest->processNoiseCov[10] = 0.333 * MAX_S16 / MAX_13p2;
	KFtest->processNoiseCov[11] = 0.50 * MAX_S16 / MAX_13p2;
	KFtest->processNoiseCov[14] = 0.50 * MAX_S16 / MAX_13p2;
	KFtest->processNoiseCov[15] = 1.0 * MAX_S16 / MAX_13p2; 
	

	// set initial state to all 0s
	KFtest->state[0] = 0 * MAX_S16 / MAX_10p5;
	KFtest->state[1] = 0 * MAX_S16 / MAX_10p5;
	KFtest->state[2] = 0 * MAX_S16 / MAX_10p5;
	KFtest->state[3] = 0 * MAX_S16 / MAX_10p5;
 
       printf("KFtest: %x\n", CMEM_getPhys(KFtest));

	for(i=0;i<50;i++){

		// kalman predict
		C6accel_VLIB_kalmanFilter_2x4_Predict(hC6accel, KFtest );

		// get measurement
		memcpy(Z, Y+2*i, sizeof(short)*2);
		Z[0] = Z[0] * MAX_S16 / MAX_10p5;
		Z[1] = Z[1] * MAX_S16 / MAX_10p5;
		printf("Z0: %d Z1: %d \n",Z[0],Z[1] );
                printf("S1:%d ,S2:%d,S3: %d,S4:  %d\n", KFtest->state[0],KFtest->state[1],KFtest->state[2],KFtest->state[3] );
                // kalman correct
		C6accel_VLIB_kalmanFilter_2x4_Correct(hC6accel, KFtest, Z, Residual );

	}

    printf("\n\nTesting the function: VLIB_kalmanFilter_2x4 functions...\n");
    printf("%d ; %d ;%d ; %d ;\n",KFtest->state[0], KFtest->state[1],KFtest->state[2],KFtest->state[3]);

	// Compare computed state with stored state after 50 iterarions
	if(	(KFtest->state[0] == testResult[0]) && (KFtest->state[1] == testResult[1]) && (KFtest->state[2] == testResult[2]) && (KFtest->state[3] == testResult[3]) )
			printf("VLIB_kalmanFilter_2x4 functions passed the test!\n");
	else
		    printf("VLIB_kalmanFilter_2x4 functions failed the test!\n");
       C6accel_Kalmanfilter2x4_release(KFtest);
	return(0);

}


void C6accel_VLIB_testKalmanFilter_4x6(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){

	int i, index,k=0;
	short *Z =  ( short *)pWorkingBuf_16bpp; //Size =4
        short *Residual =  ( short *)pWorkingBuf2_16bpp;//Size =4
	int ONE_10p5 = 32, ONE_8p7 = 128;
	

	// 40 measurements
	float Z_4d[] = {
		198.02166, 191.51310, 0.02166, -0.48690, 
		198.04332, 191.02619, -0.04676, -0.55468, 
		197.12638, 190.00981, 0.04910, -0.31363, 
		191.89442, 189.29131, -0.05757, -0.41719, 
		196.29224, 187.58754, 0.00071, -0.53284, 
		189.69887, 189.89169, -0.12065, -0.19300, 
		191.85917, 188.00355, -0.19423, -0.29695, 
		190.74741, 186.47214, -0.09994, -0.41414, 
		191.36860, 183.70520, -0.03562, -0.34991, 
		193.59680, 182.67648, -0.03485, -0.53857, 
		189.19614, 181.39511, -0.03619, -0.26380, 
		189.83059, 180.91812, -0.00407, -0.28239, 
		191.19647, 179.72601, -0.03320, -0.41788, 
		189.21421, 179.10453, -0.01257, -0.42735, 
		191.06095, 178.03412, 0.02349, -0.73560, 
		197.74546, 177.60512, 0.10241, -0.49075, 
		190.03110, 176.66377, -0.03401, -0.28241, 
		187.97061, 175.52268, -0.00364, -0.61763, 
		189.77532, 174.62560, -0.07576, -0.51476, 
		186.61644, 172.95318, 0.01319, -0.31625, 
		190.05131, 173.33247, -0.05049, -0.27207, 
		189.53904, 171.52721, 0.13402, -0.34824, 

		189.04910, 171.41190, 0.00385, -0.47197, 
		185.65925, 170.00469, -0.05449, -0.53155, 
		181.72684, 168.80935, -0.09800, -0.29756, 
		186.34611, 168.59224, 0.03105, -0.30999, 
		184.79498, 167.78362, -0.10969, -0.25863, 
		186.86506, 166.54807, -0.04857, -0.37734, 
		183.91980, 167.16654, -0.03806, -0.35661, 
		193.33650, 165.72949, 0.01352, -0.54391, 
		191.36045, 165.62516, 0.02420, -0.48507, 
		185.20242, 164.07087, -0.08011, -0.44853, 
		186.04691, 163.44381, 0.02307, -0.29161, 
		188.84002, 162.62880, -0.04647, -0.25991, 
		191.03213, 160.84097, -0.01212, -0.09877, 
		186.28901, 161.57979, 0.04904, -0.47048, 
		186.07557, 161.08044, -0.00450, -0.49053, 
		187.55553, 159.58203, -0.01533, -0.36993, 
		184.62809, 159.02573, -0.06119, -0.30155, 
		184.40178, 158.58919, -0.01462, -0.40464
	};


	// state after 40 iterations
	short testResult[] = {5944, 4943, -13, -43, -3, -3};

	// declare instance of VLIB_KalmanFilter_46
	VLIB_kalmanFilter_4x6 *KFtest = (VLIB_kalmanFilter_4x6 *)C6accel_Kalmanfilter4x6_create();

	// Initialize KalmanFilter structure
	memset(KFtest->transition, 0, sD_4x6*sD_4x6*sizeof(short));
	memset(KFtest->errorCov, 0, sD_4x6*sD_4x6*sizeof(short));
	memset(KFtest->predictedErrorCov, 0, sD_4x6*sD_4x6*sizeof(short));
	memset(KFtest->processNoiseCov, 0, sD_4x6*sD_4x6*sizeof(short));
	memset(KFtest->measurementNoiseCov, 0, mD_4x6*mD_4x6*sizeof(short));
	memset(KFtest->measurement, 0, sD_4x6*mD_4x6*sizeof(short));
	memset(KFtest->state, 0, sD_4x6*sizeof(short));
	memset(KFtest->predictedState, 0, sD_4x6*sizeof(short));
	memset(KFtest->kalmanGain, 0, sD_4x6*mD_4x6*sizeof(short));
	memset(KFtest->temp1, 0, sD_4x6*sD_4x6*sizeof(short));
	memset(KFtest->temp2, 0, sD_4x6*sD_4x6*sizeof(short));
	memset(KFtest->temp3, 0, sD_4x6*sD_4x6*sizeof(short));

	KFtest->scaleFactor = 5;

	for(i=0;i<sD_4x6;i++){		
		index = i*sD_4x6 + i;
		KFtest->errorCov[index] = 1000 * MAX_S16/MAX_13p2; // MAX_10p5;
		KFtest->transition[index] = 1 * MAX_S16/MAX_13p2; 				
		if(i < mD_4x6)			
			KFtest->measurement[index] = 1;		
	}
	KFtest->transition[2] = 1 * MAX_S16/MAX_13p2; 
	KFtest->transition[4] = 0.5 * MAX_S16/MAX_13p2;  
	KFtest->transition[9] = 1 * MAX_S16/MAX_13p2; 
	KFtest->transition[11] = 0.5 * MAX_S16/MAX_13p2; 
	KFtest->transition[16] = 1 * MAX_S16/MAX_13p2; 
	KFtest->transition[23] = 1 * MAX_S16/MAX_13p2; 

	KFtest->measurementNoiseCov[0] = 2500; //2500; //8100;// * MAX_S16 / MAX_13p2;
	KFtest->measurementNoiseCov[5] = 2500; //2500; //8100;// * MAX_S16 / MAX_13p2;
	KFtest->measurementNoiseCov[10] = 100;
	KFtest->measurementNoiseCov[15] = 100;


	KFtest->processNoiseCov[0] = 0.25 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[1] = 0.25 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[2] = 0.25 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[6] = 0.25 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[7] = 0.2500 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[8] = 0.5000 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[12] = 0.25 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[13] = 0.5000 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[14] = 1.000 * MAX_S16/MAX_13p2;

	KFtest->processNoiseCov[21] = 0.25 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[22] = 0.25 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[23] = 0.25 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[27] = 0.25 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[28] = 0.2500 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[29] = 0.5000 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[33] = 0.25 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[34] = 0.5000 * MAX_S16/MAX_13p2;
	KFtest->processNoiseCov[35] = 1.000 * MAX_S16/MAX_13p2;


	// set initial measurement
	KFtest->state[0] = 190 * ONE_10p5;
	KFtest->state[1] = 190 * ONE_10p5;
	KFtest->state[2] = 0 * ONE_8p7;
	KFtest->state[3] = 0 * ONE_8p7;
	KFtest->state[4] = 0 * ONE_8p7;
	KFtest->state[5] = 0 * ONE_8p7;

	for(i=0;i<40;i++){

		// kalman predict
		C6accel_VLIB_kalmanFilter_4x6_Predict(hC6accel, KFtest );

		// get measurement		
		Z[0] = Z_4d[i*mD_4x6+0] * ONE_10p5;
		Z[1] = Z_4d[i*mD_4x6+1] * ONE_10p5;
		Z[2] = Z_4d[i*mD_4x6+2] * ONE_8p7;
		Z[3] = Z_4d[i*mD_4x6+3] * ONE_8p7;
			
		// kalman correct
		C6accel_VLIB_kalmanFilter_4x6_Correct(hC6accel, KFtest, Z, Residual );

	}

	// Compare computed state with stored state after 40 iterarions
    printf("\n\nTesting the function: VLIB_kalmanFilter_4x6 functions...\n");

	for(i=0;i<6;i++)
	{
		if(	(KFtest->state[i] != testResult[i]) )
        {
            if(LevelOfFeedback>0)
			  printf("\tDiscrepancy in %d dimension of state, Orig=%d, calculated=%d\n",i, testResult[i],KFtest->state[i] );
              k=1;
        }

	}

    if(k==0)
			printf("VLIB_kalmanFilter_4x6 functions passed the test\n");
	else
		    printf("VLIB_kalmanFilter_4x6 functions failed the test\n");


       C6accel_Kalmanfilter4x6_release(KFtest);
	return;

}

#define NUM_PIXELS 32
#define NUM_IMAGES 10
extern unsigned char MOG_imageData[320];
extern int MOG_16_model[3210];

void C6accel_VLIB_testMixtureOfGaussiansS16(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){

	int i, k;
	char mode[3];
	int COUNTER = 0, numError = 0; 
	int VLIB_error;

	// DECLARE BUFFERS
	// input image	
	unsigned char *inputIm;
	// means
	short *currentMeans;
	// variances
	short *currentVars;
	// weights
	short *currentWts;
	// component index
	unsigned char *compIndex;
	// internal buffer
	unsigned char *intBuffer;
	// foreground mask
	unsigned int *fgMask;

	// DECLARE AND INITIALIZE MIXTURE OF GAUSSIANS PARAMETERS
	// update rate for weights, SQ0.15
	short updateRate1 = 0.001 * MAX_S16/MAX_0p16;
	// update rate for means and variances, SQ0.31
	short updateRate2 = 0.001 * MAX_S16/MAX_0p16;
	// mahalanobis distance threshold, SQ4.27
	short mdThreshold = (2.5*2.5) * (MAX_S16/MAX_4p12);
	// background subtraction threshold, SQ0.15
	short bsThreshold = 0.9 * MAX_S16/MAX_0p16;
	// initial weight for new component, SQ0.15
	short initialWt   = 0.001 * (MAX_S16/MAX_0p16);
	// initial variance for new component, SQ16.15
	short initialVar  = 320 * (MAX_S16/MAX_12p4);

	// Most VLIB functions require their I/O buffers to be 64-bit aligned in memory. 
	// As we allocate memory, we need to ensure that the buffers are aligned properly.
	
	inputIm = (unsigned char *)pWorkingBuf_16bpp; // Memory_alloc( sizeof(unsigned char)*NUM_PIXELS, &testfxnsMemParams);  
	currentMeans = (short *)pRefBuf_16bpp; //Memory_alloc( sizeof(short)*NUM_PIXELS*3, &testfxnsMemParams);
	currentVars = (short *)(pOutBuf_16bpp);//Memory_alloc( sizeof(short)*NUM_PIXELS*3, &testfxnsMemParams);
	currentWts = (short *)pSrcBuf_16bpp;//Memory_alloc( sizeof(short)*NUM_PIXELS*3, &testfxnsMemParams);
	compIndex = (unsigned char *)(pRefBuf_16bpp+sizeof(short)*NUM_PIXELS*3);//Memory_alloc( sizeof(unsigned char)*NUM_PIXELS, &testfxnsMemParams);
	intBuffer = (unsigned char *)pWorkingBuf2_16bpp;//Memory_alloc( sizeof(unsigned char)*NUM_PIXELS, &testfxnsMemParams);
	fgMask = (unsigned int *)(pWorkingBuf2_16bpp+sizeof(unsigned char)*NUM_PIXELS);//Memory_alloc(sizeof(unsigned int)*(NUM_PIXELS/32),&testfxnsMemParams);

	// INITIALIZE MIXTURE OF GAUSSIANS BUFFERS TO 0
	for(i=0;i<NUM_PIXELS*3;i++){
		currentMeans[i] = 0;
		currentVars[i] = 0;
		currentWts[i] = 0;
	};


    if(LevelOfFeedback>0)
    {
	// Format diagnostic output 
	printf("Luma  FG    Mean-1   Varn-1   Wght-1        Mean-2   Varn-2   Wght-2       Mean-3   Varn-3   Wght-3\n");
	printf("-----------------------------------------------------------------------------------------------------\n");
    }

	// PROCESS IMAGES
	for(i=0;i<NUM_IMAGES;i++){
		// Load in new image
		memcpy(inputIm, MOG_imageData+i*NUM_PIXELS, NUM_PIXELS);

		// MOG BACKGROUND MODELING AND SUBTRACTION
		VLIB_error = C6accel_VLIB_mixtureOfGaussiansS16(hC6accel, inputIm, 
										currentWts,
										currentMeans,
										currentVars,
										compIndex,
										intBuffer,
										fgMask,
										NUM_PIXELS,
										updateRate1,
										updateRate2,
										mdThreshold,
										bsThreshold,
										initialWt,
					 					initialVar);

		if (VLIB_error != 0)
			printf("Error in VLIB_MOG_1Channel_16");
		
		// Display state of the MOG model
		mode[0] = ' ';
		mode[1] = ' ';
		mode[2] = ' ';
		mode[compIndex[0]] = '*';
        
        if(LevelOfFeedback>0)
        {
		printf("%4d  %d  %c %8.4f %8.4f %8.6f   %c %8.4f %8.4f %8.6f   %c %8.4f %8.4f %8.6f\n", inputIm[0], (fgMask[0] & 0x80000000)>>31,
			mode[0], getFractional16(currentMeans[0], 7, 1), getFractional16(currentVars[0], 3, 1), getFractional16(currentWts[0], 15, 1),
			mode[1], getFractional16(currentMeans[0+(NUM_PIXELS)], 7, 1), getFractional16(currentVars[0+(NUM_PIXELS)], 3, 1), getFractional16(currentWts[0+(NUM_PIXELS)], 15, 1),
			mode[2], getFractional16(currentMeans[0+2*(NUM_PIXELS)], 7, 1), getFractional16(currentVars[0+2*(NUM_PIXELS)], 3, 1), getFractional16(currentWts[0+2*(NUM_PIXELS)], 15, 1));

        }
		// CHECK ACCURACY OF RESULTS
		for(k=0;k<3*NUM_PIXELS;k++){
			if (currentMeans[k] != MOG_16_model[COUNTER++]){
				printf("\tError in Mean: Image %d, Pixel %d, Mode %d\n", i, k%32, k/32);
				numError++;
			}
			if (currentVars[k] != MOG_16_model[COUNTER++]){
				printf("\tError in Variance: Image %d, Pixel %d, Mode %d\n", i, k%32, k/32);
				numError++;
			}
			if (currentWts[k] != MOG_16_model[COUNTER++]){
				printf("\tError in Weight: Image %d, Pixel %d, Mode %d\n", i, k%32, k/32);
				numError++;
			}
		}
		for(k=0;k<NUM_PIXELS;k++){
			if (compIndex[k] != MOG_16_model[COUNTER++]){
				printf("\tError in Component index: Image %d, Pixel %d\n", i, k%32);
				numError++;
			}
		}
		for(k=0;k<NUM_PIXELS;k++){
			if( ((fgMask[0]>>k) & 0x00000001) != ((MOG_16_model[COUNTER]>>k) & 0x00000001) ){
				printf("\tError in Foreground Mask: Image %d, Pixel %d\n", i, k);
				numError++;
			}
           
		}
		COUNTER++;
	        			
	}
      
   printf("\n\nTesting the function: VLIB_mixtureOfGaussiansS16...\n");
	if(numError == 0)
		printf("VLIB_mixtureOfGaussiansS16 passed the test! \n");
    else
      printf("VLIB_mixtureOfGaussiansS16 failed the test! \n");

} 


extern int MOG_32_model[3210];


void C6accel_VLIB_testMixtureOfGaussiansS32(C6accel_Handle hC6accel,unsigned char LevelOfFeedback){

	int i, k;
	char mode[3];
	int COUNTER = 0, numError = 0; 
	int VLIB_error;


	// DECLARE BUFFERS
	// input image	
	unsigned char *inputIm;
	// means
	int *currentMeans;
	// variances
	int *currentVars;
	// weights
	short *currentWts;
	// component index
	unsigned char *compIndex;
	// internal buffer
	unsigned char *intBuffer;
	// foreground mask
	unsigned int *fgMask;

	// DECLARE AND INITIALIZE MIXTURE OF GAUSSIANS PARAMETERS
	// update rate for weights, SQ0.15
	short updateRate1 = 0.001 * MAX_S16/MAX_0p16;
	// update rate for means and variances, SQ0.31
	int updateRate2 = 0.001 * MAX_S32/MAX_0p16;
	// mahalanobis distance threshold, SQ4.27
	int mdThreshold = (2.5*2.5) * (MAX_S32/MAX_4p12);
	// background subtraction threshold, SQ0.15
	short bsThreshold = 0.9 * MAX_S16/MAX_0p16;
	// initial weight for new component, SQ0.15
	short initialWt   = 0.001 * (MAX_S16/MAX_0p16);
	// initial variance for new component, SQ16.15
	int initialVar  = 320 * (MAX_S32/MAX_16p16);

	// Most VLIB functions require their I/O buffers to be 64-bit aligned in memory. 
	// As we allocate memory, we need to ensure that the buffers are aligned properly.

	inputIm = (unsigned char *)pWorkingBuf_16bpp; // Memory_alloc( sizeof(unsigned char)*NUM_PIXELS, &testfxnsMemParams);  
	currentMeans = (int *)pRefBuf_16bpp; //Memory_alloc( sizeof(short)*NUM_PIXELS*3, &testfxnsMemParams);
	currentVars = (int *)(pOutBuf_16bpp);//Memory_alloc( sizeof(short)*NUM_PIXELS*3, &testfxnsMemParams);
	currentWts = (short *)pSrcBuf_16bpp;//Memory_alloc( sizeof(short)*NUM_PIXELS*3, &testfxnsMemParams);
	compIndex = (unsigned char *)(pRefBuf_16bpp+sizeof(int)*NUM_PIXELS*3);//Memory_alloc( sizeof(unsigned char)*NUM_PIXELS, &testfxnsMemParams);
	intBuffer = (unsigned char *)pWorkingBuf2_16bpp;//Memory_alloc( sizeof(unsigned char)*NUM_PIXELS, &testfxnsMemParams);
	fgMask = (unsigned int *)(pWorkingBuf2_16bpp+sizeof(unsigned char)*NUM_PIXELS);//Memory_alloc(sizeof(unsigned int)*(NUM_PIXELS/32),&testfxnsMemParams);


	// INITIALIZE MIXTURE OF GAUSSIANS BUFFERS TO 0
	for(i=0;i<NUM_PIXELS*3;i++){
		currentMeans[i] = 0;
		currentVars[i] = 0;
		currentWts[i] = 0;
	};

    if(LevelOfFeedback>0)
    {
	// Format diagnostic output 
	printf("Luma  FG    Mean-1   Varn-1   Wght-1        Mean-2   Varn-2   Wght-2       Mean-3   Varn-3   Wght-3\n");
	printf("-----------------------------------------------------------------------------------------------------\n");
    }

	// PROCESS IMAGES
	for(i=0;i<NUM_IMAGES;i++){

		// Load in new image
		memcpy(inputIm, MOG_imageData+i*NUM_PIXELS, NUM_PIXELS);

		// MOG BACKGROUND MODELING AND SUBTRACTION
		                  C6accel_VLIB_mixtureOfGaussiansS32(hC6accel, inputIm, 
										currentWts,
										currentMeans,
										currentVars,
										compIndex,
										intBuffer,
										fgMask,
										NUM_PIXELS,
										updateRate1,
										updateRate2,
										mdThreshold,
										bsThreshold,
										initialWt,
					 					initialVar);

		if (VLIB_error != 0)
			printf("Error in VLIB_MOG_1Channel_32");
		
		// Display state of the MOG model
		mode[0] = ' ';
		mode[1] = ' ';
		mode[2] = ' ';
		mode[compIndex[0]] = '*';

       if(LevelOfFeedback>0)
       {
		printf("%4d  %d  %c %8.4f %8.4f %8.6f   %c %8.4f %8.4f %8.6f   %c %8.4f %8.4f %8.6f\n", inputIm[0], (fgMask[0] & 0x80000000)>>31,
			mode[0], getFractional32(currentMeans[0], 23, 1), getFractional32(currentVars[0], 15, 1), getFractional16(currentWts[0], 15, 1),
			mode[1], getFractional32(currentMeans[0+(NUM_PIXELS)], 23, 1), getFractional32(currentVars[0+(NUM_PIXELS)], 15, 1), getFractional16(currentWts[0+(NUM_PIXELS)], 15, 1),
			mode[2], getFractional32(currentMeans[0+2*(NUM_PIXELS)], 23, 1), getFractional32(currentVars[0+2*(NUM_PIXELS)], 15, 1), getFractional16(currentWts[0+2*(NUM_PIXELS)], 15, 1));
       }
 
		// CHECK ACCURACY OF RESULTS
		for(k=0;k<3*NUM_PIXELS;k++){
			if (currentMeans[k] != MOG_32_model[COUNTER++]){
				printf("\tError in Mean: Image %d, Pixel %d, Mode %d\n", i, k%32, k/32);
				numError++;
			}
			if (currentVars[k] != MOG_32_model[COUNTER++]){
				printf("\tError in Variance: Image %d, Pixel %d, Mode %d\n", i, k%32, k/32);
				numError++;
			}
			if (currentWts[k] != MOG_32_model[COUNTER++]){
				printf("\tError in Weight: Image %d, Pixel %d, Mode %d\n", i, k%32, k/32);
				numError++;
			}
		}
		for(k=0;k<NUM_PIXELS;k++){
			if (compIndex[k] != MOG_32_model[COUNTER++]){
				printf("\tError in Component index: Image %d, Pixel %d\n", i, k%32);
				numError++;
			}
		}
		for(k=0;k<NUM_PIXELS;k++){
			if( ((fgMask[0]>>k) & 0x00000001) != ((MOG_32_model[COUNTER]>>k) & 0x00000001) ){
				printf("\tError in Foreground Mask: Image %d, Pixel %d\n", i, k);
				numError++;
			}
		}
		COUNTER++;
			
	}

   printf("\n\nTesting the function: VLIB_mixtureOfGaussiansS32...\n");
	if(numError == 0)
		printf("VLIB_mixtureOfGaussiansS32 passed the test! \n");
    else
       printf("VLIB_mixtureOfGaussiansS32 failed the test! \n");


}


