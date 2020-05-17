/*******************************************************************************
**+--------------------------------------------------------------------------+**
 *                                                                            **
 * Copyright (c) 2010, Texas Instruments Incorporated                         **
 * All rights reserved.                                                       **
 *                                                                            **
**+--------------------------------------------------------------------------+**/
/*******************************************************************************
 *  ======== iC6Accel_ti.h ========                                           **
 *   Application-Codec interface header file                                  **
 *   Defines Function IDs, Input arguments and parameter structures           **
 *   for kernels in C6Accel                                                   **
*******************************************************************************/
/* Include files */
#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/iuniversal.h>

#ifndef ti_sdo_codecs_C6Accel_C6ACCEL_TI_
#define ti_sdo_codecs_C6Accel_C6ACCEL_TI_

#ifdef __cplusplus
extern "C" {
#endif

/* Codec engine allows passing 16 inbut and output buffers
limiting the number of function call in C6Accel to 16 in each universal
process call*/ 
#define   MAX_FXN_CALLS 16

/********************************************************************************************
*    Function IDs for library functions                                                     *
*                                                                                           *
*    Format of the function Id uses                                                         *
*    * 31-24 bits are used to identify the vendor (Default vendor TI with vendor code:0x01) *
*    * 23-16 bits are used to identify the type of function                                 * 
*         ** 0x01 : DSP library                                                             *
*         ** 0x02 : IMAGE Library                                                           *
*         ** 0x03 : MATH Library                                                            *
*         ** 0x04 : ANALYTICS Library                                                       *       
*         ** 0x05 : MEDICAL Library                                                         *
*         ** 0x06 : POWER CONTROL Library                                                   *
*         ** 0x07 : AUDIO SPEECH Library                                                    *
*    * 15-0 bits are used to identify the function within the library                       *
*                                                                                           *
*********************************************************************************************/
/* Common Null ID */
#define NULL_INDEX                        0x00000000

/****************************************************************************/  
/*                                                                    */
/*           DSP Library functions                                    */
/*     31-24 bits   Vendor ID                : 0x01 (TI)              */           
/*     23-16 bits   Function type/Library ID : 0x01 (DSPLIB)          */  
/*     15-0 bits are used to identify the function within the library */        
/**********************************************************************/
#define DSP_FFT16x16_FXN_ID               0x01010000
#define DSP_IFFT16x16_FXN_ID              0x01010001
#define DSP_MAT_MUL_FXN_ID                0x01010002
#define DSP_FIR_GEN_FXN_ID                0x01010003
#define DSP_AUTOCOR_FXN_ID                0x01010004
#define DSP_FIRLMS2_FXN_ID                0x01010005
#define DSP_FFT16x16_IMRE_FXN_ID          0x01010006
#define DSP_FFT16x16R_FXN_ID              0x01010007
#define DSP_FFT16x32_FXN_ID               0x01010008
#define DSP_FFT32x32_FXN_ID               0x01010009
#define DSP_FFT32x32S_FXN_ID              0x0101000A
#define DSP_IFFT16x16_IMRE_FXN_ID         0x0101000B
#define DSP_IFFT16x32_FXN_ID              0x0101000C
#define DSP_IFFT32x32_FXN_ID              0x0101000D
#define DSP_FIR_CPLX_FXN_ID               0x0101000E
#define DSP_FIR_CPLX_HM4X4_FXN_ID         0x0101000F
#define DSP_FIR_GEN_HM17_RA8X8_FXN_ID     0x01010010
#define DSP_FIR_R4_FXN_ID                 0x01010011
#define DSP_FIR_R8_FXN_ID                 0x01010012
#define DSP_FIR_R8_HM16_RM8A8X8_FXN_ID    0x01010013
#define DSP_FIR_SYM_FXN_ID                0x01010014
#define DSP_IIR_FXN_ID                    0x01010015
#define DSP_IIR_LAT_FXN_ID                0x01010016
#define DSP_DOTP_SQR_FXN_ID               0x01010017
#define DSP_DOTPROD_FXN_ID                0x01010018
#define DSP_MAXVAL_FXN_ID                 0x01010019
#define DSP_MAXIDX_FXN_ID                 0x0101001A
#define DSP_MINVAL_FXN_ID                 0x0101001B
#define DSP_MUL32_FXN_ID                  0x0101001C
#define DSP_NEG32_FXN_ID                  0x0101001D
#define DSP_RECIP16_FXN_ID                0x0101001E
#define DSP_VECSUMSQ_FXN_ID               0x0101001F
#define DSP_W_VEC_FXN_ID                  0x01010020
#define DSP_MAT_TRANS_FXN_ID              0x01010021
#define DSP_BEXP_FXN_ID                   0x01010022
#define DSP_BLK_ESWAP16_FXN_ID            0x01010023
#define DSP_BLK_ESWAP32_FXN_ID            0x01010024
#define DSP_BLK_ESWAP64_FXN_ID            0x01010025
#define DSP_BLK_MOVE_FXN_ID               0x01010026
#define DSP_FLTOQ15_FXN_ID                0x01010027
#define DSP_MINERROR_FXN_ID               0x01010028
#define DSP_Q15tOFL_FXN_ID                0x01010029
#define DSPF_TW_GEN_FXN_ID                0x0101002A
#define DSP_GEN_TWIDDLE_FFT16x16_FXN_ID   0x0101002B
#define DSP_GEN_TWIDDLE_FFT32x32_FXN_ID   0x0101002C


/* Floating point DSPLIB kernels */
#define DSPF_SP_FFTSPXSP_FXN_ID           0x01010400
#define DSPF_SP_IFFTSPXSP_FXN_ID          0x01010401
#define DSPF_SP_DOTPROD_FXN_ID            0x01010402
#define DSPF_SP_AUTOCOR_FXN_ID            0x01010403
#define DSPF_SP_FIR_GEN_FXN_ID            0x01010404
#define DSPF_SP_FIR_CPLX_FXN_ID           0x01010405
#define DSPF_SP_MAT_MUL_FXN_ID            0x01010406
#define DSPF_SP_MAT_TRANS_FXN_ID          0x01010407
#define DSPF_SP_IIR_FXN_ID                0x01010408
#define DSPF_SP_CONV_FXN_ID               0x01010409
#define DSPF_SP_VECMUL_FXN_ID             0x0101040A
#define DSPF_SP_VECRECIP_FXN_ID           0x0101040B
#define DSPF_SP_VECSUM_SQ_FXN_ID          0x0101040C
#define DSPF_SP_W_VEC_FXN_ID              0x0101040D
#define DSPF_SP_BIQUAD_FXN_ID             0x0101040E
#define DSPF_SP_MAT_MUL_CPLX_FXN_ID       0x0101040F
#define DSPF_COMPLXTOREALNIMG_FXN_ID      0x01010410

/**********************************************************************/  
/*                                                                    */
/*           Image Library functions                                  */
/*     31-24 bits   Vendor ID                : 0x01 (TI)              */           
/*     23-16 bits   Function type/Library ID : 0x02 (IMGLIB)          */  
/*     15-0 bits are used to identify the function within the library */        
/**********************************************************************/
#define IMG_BOUNDARY_8_FXN_ID             0x01020000
#define IMG_BOUNDARY_16S_FXN_ID           0x01020001
#define IMG_CLIPPING_16S_FXN_ID           0x01020002
#define IMG_YC_DEMUX_BE16_16_FXN_ID       0x01020003
#define IMG_YC_DEMUX_LE16_16_FXN_ID       0x01020004
#define IMG_DILATE_BIN_FXN_ID             0x01020005
#define IMG_ERODE_BIN_FXN_ID              0x01020006
#define IMG_ERRDIF_BIN_8_FXN_ID           0x01020007
#define IMG_ERRDIF_BIN_16_FXN_ID          0x01020008
#define IMG_HISTOGRAM_8_FXN_ID            0x01020009
#define IMG_HISTOGRAM_16_FXN_ID           0x0102000A
#define IMG_MEDIAN_3X3_8_FXN_ID           0x0102000B
#define IMG_PERIMETER_8_FXN_ID            0x0102000C
#define IMG_PERIMETER_16_FXN_ID           0x0102000D
#define IMG_PIX_EXPAND_FXN_ID             0x0102000E
#define IMG_PIX_SAT_FXN_ID                0x0102000F
#define IMG_SOBEL_3X3_8_FXN_ID            0x01020010
#define IMG_SOBEL_3X3_16S_FXN_ID          0x01020011
#define IMG_SOBEL_3X3_16_FXN_ID           0x01020012
#define IMG_SOBEL_5X5_16S_FXN_ID          0x01020013
#define IMG_SOBEL_7X7_16S_FXN_ID          0x01020014
#define IMG_THR_GT2MAX_8_FXN_ID           0x01020015
#define IMG_THR_GT2MAX_16_FXN_ID          0x01020016
#define IMG_THR_GT2THR_8_FXN_ID           0x01020017
#define IMG_THR_GT2THR_16_FXN_ID          0x01020018
#define IMG_THR_LE2MIN_8_FXN_ID           0x01020019
#define IMG_THR_LE2MIN_16_FXN_ID          0x0102001A
#define IMG_THR_LE2THR_8_FXN_ID           0x0102001B
#define IMG_THR_LE2THR_16_FXN_ID          0x0102001C
#define IMG_YCBCR422P_RGB565_FXN_ID       0x0102001D
#define IMG_CONV_3X3_I8_C8S_FXN_ID        0x0102001E
#define IMG_CONV_3X3_I16S_C16S_FXN_ID     0x0102001F
#define IMG_CONV_3X3_I16_C16S_FXN_ID      0x01020020
#define IMG_CONV_5X5_I8_C8S_FXN_ID        0x01020021
#define IMG_CONV_5X5_I16S_C16S_FXN_ID     0x01020022
#define IMG_CONV_5X5_I8_C16S_FXN_ID       0x01020023
#define IMG_CONV_7X7_I8_C8S_FXN_ID        0x01020024
#define IMG_CONV_7X7_I16S_C16S_FXN_ID     0x01020025
#define IMG_CONV_7X7_I8_C16S_FXN_ID       0x01020026
#define IMG_CONV_11X11_I8_C8S_FXN_ID      0x01020027
#define IMG_CONV_11X11_I16S_C16S_FXN_ID   0x01020028
#define IMG_CORR_3X3_I8_C16S_FXN_ID       0x01020029
#define IMG_CORR_3X3_I16S_C16S_FXN_ID     0x0102002A
#define IMG_CORR_3X3_I8_C8_FXN_ID         0x0102002B
#define IMG_CORR_3X3_I16_C16S_FXN_ID      0x0102002C
#define IMG_CORR_5X5_I16S_C16S_FXN_ID     0x0102002D
#define IMG_CORR_11X11_I16S_C16S_FXN_ID   0x0102002E
#define IMG_CORR_11X11_I8_C16S_FXN_ID     0x0102002F
#define IMG_CORR_GEN_I16S_C16S_FXN_ID     0x01020030
#define IMG_CORR_GEN_IQ_FXN_ID            0x01020031
#define IMG_MEDIAN_3X3_16S_FXN_ID         0x01020032
#define IMG_MEDIAN_3X3_16_FXN_ID          0x01020033
#define IMG_YC_DEMUX_BE16_8_FXN_ID        0x01020034
#define IMG_YC_DEMUX_LE8_C_FXN_ID         0x01020035
#define IMG_FDCT_8X8_FXN_ID               0x01020036
#define IMG_IDCT_8X8_12Q4_FXN_ID          0x01020037
#define IMG_MAD_8X8_FXN_ID                0x01020038
#define IMG_MAD_16X16_FXN_ID              0x01020039
#define IMG_MPEG2_VLD_INTRA_FXN_ID        0x0102003A
#define IMG_MPEG2_VLD_INTER_FXN_ID        0x0102003B
#define IMG_QUANTIZE_FXN_ID               0x0102003C
#define IMG_SAD_8X8_FXN_ID                0x0102003D
#define IMG_SAD_16X16_FXN_ID              0x0102003E
#define IMG_WAVE_HORZ_FXN_ID              0x0102003F
#define IMG_WAVE_VERT_FXN_ID              0x01020040

/*Low level IMGLIB functions*/
#define IMG_MULS_16S_FXN_ID               0x01020041
#define IMG_MULS_8_FXN_ID                 0x01020042
#define IMG_ADDS_16S_FXN_ID               0x01020043
#define IMG_ADDS_8_FXN_ID                 0x01020044
#define IMG_SUBS_16S_FXN_ID               0x01020045
#define IMG_SUBS_8_FXN_ID                 0x01020046
#define IMG_NOT_16_FXN_ID                 0x01020047
#define IMG_NOT_8_FXN_ID                  0x01020048
#define IMG_ANDS_16_FXN_ID                0x01020049
#define IMG_ANDS_8_FXN_ID                 0x0102004A
#define IMG_ORS_16_FXN_ID                 0x0102004B
#define IMG_ORS_8_FXN_ID                  0x0102004C
#define IMG_AND_16_FXN_ID                 0x0102004D 
#define IMG_AND_8_FXN_ID                  0x0102004E
#define IMG_OR_16_FXN_ID                  0x0102004F 
#define IMG_OR_8_FXN_ID                   0x01020050
#define IMG_MUL_16S_FXN_ID                0x01020051
#define IMG_MUL_8_FXN_ID                  0x01020052
#define IMG_ADD_16S_FXN_ID                0x01020053
#define IMG_ADD_8_FXN_ID                  0x01020054
#define IMG_SUB_16S_FXN_ID                0x01020055
#define IMG_SUB_8_FXN_ID                  0x01020056

/*Additional IMGLIB KERNELS*/
#define IMG_YUV420PL_RGB565_FXN_ID        0x01020057
#define IMG_YCBCR422PL16_RGB565_FXN_ID    0x01020058
#define IMG_YUV420PL16_RGB565_FXN_ID      0x01020059

#define IMG_CONV_7X7_I8_C8S_FRAME_FXN_ID        0x01020100
#define IMG_CONV_11X11_I8_C8S_FRAME_FXN_ID      0x01020101

/* Additional Frame based IMG kernels for better performance */
#define IMG_MEDIAN_3X3_8_FRAME_FXN_ID     0x0102005A
#define IMG_MEDIAN_3X3_16S_FRAME_FXN_ID   0x0102005B
#define IMG_MEDIAN_3X3_16_FRAME_FXN_ID    0x0102005C

/* Optimized version of existing IMG lib kernels */
#define IMG_CONV_3X3_I8_C8S_FRAME_FXN_ID        0x0102005D
#define IMG_CONV_3X3_I16S_C16S_FRAME_FXN_ID     0x0102005E
#define IMG_CORR_3X3_I8_C16S_FRAME_FXN_ID       0x0102005F
#define IMG_CORR_3X3_I16S_C16S_FRAME_FXN_ID     0x01020060
#define IMG_CORR_3X3_I8_C8_FRAME_FXN_ID         0x01020061
#define IMG_YCBCR422SP_TO_YCBCR420PL_FXN_ID     0x01020062
#define IMG_YCBCR422SP_TO_YCBCR422ILE_FXN_ID    0x01020063
#define IMG_YCBCR422PL_TO_YCBCR422SP_FXN_ID     0x01020064
#define IMG_SOBEL_3X3_8_16_FXN_ID               0x01020065
#define IMG_RGB_Y_FXN_ID                        0x01020066
#define IMG_YC_DEMUX_BE8_FXN_ID			0x01020067
#define IMG_YCBCR420PL_TO_YCBCR422SP_FXN_ID     0x01020068

/************************************************************************/  
/*                                                                      */
/*           MATH Library functions                                     */
/*     31-24 bits   Vendor ID                : 0x01 (TI)                */           
/*     23-16 bits   Function type/Library ID : 0x03 (IQMATHLIB)         */  
/*     15-0 bits are used to identify the function within the library   */        
/************************************************************************/
#define MATH_IQNMPY_FXN_ID                0x01030000
#define MATH_IQNSQRT_FXN_ID               0x01030001
#define MATH_IQNSIN_FXN_ID                0x01030002
#define MATH_IQNCOS_FXN_ID                0x01030003
#define MATH_IQNDIV_FXN_ID                0x01030004
#define MATH_IQNATAN2_FXN_ID              0x01030005
#define MATH_IQNEXP_FXN_ID                0x01030006
#define MATH_IQNLOG_FXN_ID                0x01030007
#define MATH_IQNRMPY_FXN_ID               0x01030008
#define MATH_IQNRSMPY_FXN_ID              0x01030009
#define MATH_IQNASIN_FXN_ID               0x0103000A
#define MATH_IQNACOS_FXN_ID               0x0103000B
#define MATH_IQNSINPU_FXN_ID              0x0103000C
#define MATH_IQNCOSPU_FXN_ID              0x0103000D
#define MATH_IQNATAN2PU_FXN_ID            0x0103000E
#define MATH_IQNMPYIQX_FXN_ID             0x0103000F
#define MATH_IQNMPYI32INT_FXN_ID          0x01030010
#define MATH_IQNMPYI32FRAC_FXN_ID         0x01030011
#define MATH_IQNABS_FXN_ID                0x01030012
#define MATH_IQNTOF_FXN_ID                0x01030013
#define MATH_IQTOIQN_FXN_ID               0x01030014
#define MATH_IQNPOW_FXN_ID                0x01030015
#define MATH_IQNISQRT_FXN_ID              0x01030016
#define MATH_IQNMAG_FXN_ID                0x01030017

/* FASTRTS LIB kernels for C64x devices */
#define MATH_ADDSP_FXN_ID                 0x01030018
#define MATH_SUBSP_FXN_ID                 0x01030019
#define MATH_MPYSP_FXN_ID                 0x0103001A
#define MATH_DIVSP_FXN_ID                 0x0103001B
#define MATH_RECIPSP_FXN_ID               0x0103001C
#define MATH_INTSP_FXN_ID                 0x0103001D
#define MATH_SPINT_FXN_ID                 0x0103001E
#define MATH_SPUINT_FXN_ID                0x0103001F
#define MATH_SQRTSP_FXN_ID                0x01030020
#define MATH_UINTSP_FXN_ID                0x01030021

/*Format conversion kernels in IQMATH lib*/
#define MATH_FTOIQN_FXN_ID                0x01030022
#define MATH_IQN_FXN_ID                   0x01030023
#define MATH_IQXTOIQY_FXN_ID              0x01030024
#define MATH_IQNINT_FXN_ID                0x01030025
#define MATH_IQNFRAC_FXN_ID               0x01030026

/* Fast RTS functions from 674x math library */
#define MATH_ATANDP_FXN_ID                0x01030200
#define MATH_ATANSP_FXN_ID                0x01030201
#define MATH_ATAN2DP_FXN_ID               0x01030202
#define MATH_ATAN2SP_FXN_ID               0x01030203
#define MATH_COSDP_FXN_ID                 0x01030204
#define MATH_COSSP_FXN_ID                 0x01030205
#define MATH_DIVDP_FXN_ID                 0x01030206
#define MATH_EXPDP_FXN_ID                 0x01030207
#define MATH_EXPSP_FXN_ID                 0x01030208
#define MATH_EXP2DP_FXN_ID                0x01030209
#define MATH_EXP2SP_FXN_ID                0x0103020A
#define MATH_EXP10DP_FXN_ID               0x0103020B
#define MATH_EXP10SP_FXN_ID               0x0103020C
#define MATH_LOGDP_FXN_ID                 0x0103020D
#define MATH_LOGSP_FXN_ID                 0x0103020E
#define MATH_LOG2DP_FXN_ID                0x0103021F
#define MATH_LOG2SP_FXN_ID                0x01030220
#define MATH_LOG10DP_FXN_ID               0x01030211
#define MATH_LOG10SP_FXN_ID               0x01030212
#define MATH_POWDP_FXN_ID                 0x01030213
#define MATH_POWSP_FXN_ID                 0x01030214
#define MATH_RECIPF_FXN_ID                0x01030215
#define MATH_RECIPDP_FXN_ID               0x01030216
#define MATH_RSQRTDP_FXN_ID               0x01030217
#define MATH_RSQRTSP_FXN_ID               0x01030218
#define MATH_SINDP_FXN_ID                 0x01030219
#define MATH_SINSP_FXN_ID                 0x0103021A
#define MATH_SQRTDP_FXN_ID                0x0103021B
#define MATH_SQRT_FXN_ID                  0x0103021C
#define MATH_D2S_FXN_ID                   0x0103021D
#define MATH_S2D_FXN_ID                   0x0103021E
#define MATH_D2I_FXN_ID                   0x0103021F
#define MATH_I2D_FXN_ID                   0x01030220
#define MATH_F2D_FXN_ID                   0x01030221
#define MATH_D2F_FXN_ID                   0x01030222

/**********************************************************************/  
/*                                                                    */
/*           ANALYTICS Library functions                              */
/*     31-24 bits   Vendor ID                : 0x01 (TI)              */           
/*     23-16 bits   Function type/Library ID : 0x04 (ANALYTICSLIB)    */  
/*     15-0 bits are used to identify the function within the library */        
/**********************************************************************/
#define VLIB_DILATE_BIN_MASK_FXN_ID                0x01040001
#define VLIB_ERODE_BIN_MASK_FXN_ID                 0x01040002

#define VLIB_XYGRADIENTSANDMAGNITUDE_FXN_ID        0x01040100 
#define VLIB_NONMAXIMUMSUPPRESSIONCANNY_FXN_ID     0x01040101 
#define VLIB_DOUBLETHRESHOLDING_FXN_ID             0x01040102 
#define VLIB_EDGERELAXATION_FXN_ID                 0x01040103 
#define VLIB_CANNYEDGE_FXN_ID        		   0x01041000 
#define VLIB_DILATE_BIN_SQUARE_FXN_ID		   0x01040104
#define VLIB_DILATE_BIN_CROSS_FXN_ID		   0x01040105
#define VLIB_ERODE_BIN_SQUARE_FXN_ID		   0x01040107
#define VLIB_ERODE_BIN_CROSS_FXN_ID		   0x01040108
#define VLIB_ERODE_BIN_SINGLEPIXEL_FXN_ID	   0x01040110
#define VLIB_PACKMASK32_FXN_ID			   0x01040111
#define VLIB_UNPACKMASK32_FXN_ID		   0x01040112
#define VLIB_HOUGHLINEFROMLIST_FXN_ID              0x01040113

#define VLIB_DILATE_BIN_SQUARE_OPT_FXN_ID	   0x01040201
#define VLIB_ERODE_BIN_SQUARE_OPT_FXN_ID	   0x01040202

#define VLIB_INITMEANWITHLUMAS16_FXN_ID            0x01040003
#define VLIB_INITMEANWITHLUMAS32_FXN_ID            0x01040004
#define VLIB_INITVARWITHCONSTS16_FXN_ID            0x01040005
#define VLIB_INITVARWITHCONSTS32_FXN_ID            0x01040006
#define VLIB_UPDATEEWRMEANS16_FXN_ID               0x01040007
#define VLIB_UPDATEEWRMEANS32_FXN_ID               0x01040008
#define VLIB_UPDATEEWRVARIANCES16_FXN_ID           0x01040009   
#define VLIB_UPDATEEWRVARIANCES32_FXN_ID           0x0104000A
#define VLIB_UPDATEUWRMEANS16_FXN_ID               0x0104000B
#define VLIB_UPDATEUWRVARIANCES16_FXN_ID           0x0104000C

#define VLIB_SUBTRACTBACKGROUNDS16_FXN_ID          0x0104000D
#define VLIB_SUBTRACTBACKGROUNDS32_FXN_ID          0x0104000E
#define VLIB_MIXTUREOFGAUSSIANSS16_FXN_ID          0x0104000F
#define VLIB_MIXTUREOFGAUSSIANSS32_FXN_ID          0x01040010

#define VLIB_IMAGEPYRAMID8_FXN_ID                  0x01040011
#define VLIB_IMAGEPYRAMID16_FXN_ID                 0x01040012
#define VLIB_INTEGRALIMAGE8_FXN_ID                 0x01040018
#define VLIB_INTEGRALIMAGE16_FXN_ID                0x0104001A
#define VLIB_HOUGHFORLINES_FXN_ID                  0x0104001B
#define VLIB_HARRISSCORE_7x7_FXN_ID                0x0104001C
#define VLIB_NONMAXSUPPRESS_3x3_16S_FXN_ID         0x0104001D
#define VLIB_NONMAXSUPPRESS_5x5_16S_FXN_ID         0x0104001E
#define VLIB_NONMAXSUPPRESS_7x7_16S_FXN_ID         0x0104001F
#define VLIB_TRACKFEATURESLUCASKANADE_7x7_FXN_ID   0x01040020

#define VLIB_EXTRACTLUMAFROMUYUV_FXN_ID            0x01040021
#define VLIB_CONVERTUYVYINT_TO_YUVPL_FXN_ID        0x01040022

#define VLIB_CONVERTUYVYINT_TO_YUV420PL_FXN_ID     0x01040023
#define VLIB_CONVERTUYVYINT_TO_HSLPL_FXN_ID        0x01040024
#define VLIB_CONVERTUYVYINT_TO_LABPL_FXN_ID        0x01040025
#define VLIB_CONVERTUYVYINT_TO_RGBPL_FXN_ID        0x01040026
#define VLIB_CONVERTUYVYINT_TO_LABPL_LUT_FXN_ID    0x01040027
#define VLIB_CONVERTUYVYSEMIPL_TO_YUVPL_FXN_ID     0x01040028
#define VLIB_INITUYVYINT_TO_LABPL_LUT_FXN_ID       0x01040029
#define VLIB_NORMALFLOW_16_FXN_ID                  0x0104002A
#define VLIB_CONVERTUYVYPL_TO_YUVINT_FXN_ID        0x0104002B
 
#define VLIB_GAUSS5X5PYRAMIDKERNEL_8_FXN_ID         0x0104002C
#define VLIB_GAUSS5X5PYRAMIDKERNEL_16_FXN_ID        0x0104002D
#define VLIB_GRADIENTH5X5PYRAMIDKERNEL_8_FXN_ID     0x0104002E
#define VLIB_GRADIENTV5X5PYRAMIDKERNEL_8_FXN_ID     0x0104002F
#define VLIB_RECURSIVEFILTERHORIZ1STORDER_FXN_ID    0x01040030
#define VLIB_RECURSIVEFILTERHORIZ1STORDERS16_FXN_ID 0x01040031
#define VLIB_RECURSIVEFILTERVERT1STORDER_FXN_ID     0x01040032
#define VLIB_RECURSIVEFILTERVERT1STORDERS16_FXN_ID  0x01040033
#define VLIB_LEGENDREMOMENTS_FXN_ID                 0x01040034
#define VLIB_LEGENDREMOMENTS_INIT_FXN_ID            0x01040035
#define VLIB_HISTOGRAM_1D_INIT_U8_FXN_ID            0x01040036
#define VLIB_HISTOGRAM_1D_U8_FXN_ID                 0x01040037
#define VLIB_WEIGHTEDHISTOGRAM_1D_U8_FXN_ID         0x01040038
#define VLIB_HISTOGRAM_1D_INIT_U16_FXN_ID           0x01040039
#define VLIB_HISTOGRAM_1D_U16_FXN_ID                0x0104003A
#define VLIB_WEIGHTEDHISTOGRAM_1D_U16_FXN_ID        0x0104003B
#define VLIB_HISTOGRAM_ND_U16_FXN_ID                0x0104003C
#define VLIB_WEIGHTEDHISTOGRAM_ND_U16_FXN_ID        0x0104003D
#define VLIB_BHATTACHARYADISTANCE_U32_FXN_ID        0x0104003E
#define VLIB_L1DISTANCES16_FXN_ID                   0x0104003F
#define VLIB_DISPARITY_SAD_FIRSTROW16_FXN_ID        0x01040040
#define VLIB_DISPARITY_SAD16_FXN_ID                 0x01040041
#define VLIB_DISPARITY_SAD_FIRSTROW8_FXN_ID         0x01040042
#define VLIB_DISPARITY_SAD8_FXN_ID                  0x01040043
#define VLIB_SIMPLEX_FXN_ID                         0x01040044
#define VLIB_SIMPLEX_3D_FXN_ID                      0x01040045
#define VLIB_KALMANFILTER_4X6_PREDICT_FXN_ID        0x01040046
#define VLIB_KALMANFILTER_4X6_CORRECT_FXN_ID        0x01040047
#define VLIB_KALMANFILTER_2X4_PREDICT_FXN_ID        0x01040048
#define VLIB_KALMANFILTER_2X4_CORRECT_FXN_ID        0x01040049

#define VLIB_CALCCONNECTEDCOMPONENTSMAXBUFFERSIZE_FXN_ID   0x0104004A
#define VLIB_INITCONNECTEDCOMPONENTSLIST_FXN_ID            0x0104004B
#define VLIB_CREATECONNECTEDCOMPONENTSLIST_FXN_ID          0x0104004C
#define VLIB_GETNUMCCS_FXN_ID                              0x0104004D
#define VLIB_GETCCFEATURES_FXN_ID                          0x0104004E
#define VLIB_CREATECCMAP8BIT_FXN_ID                        0x0104004F

/**********************************************************************/  
/*                                                                    */
/*           MEDICAL Library functions                              */
/*     31-24 bits   Vendor ID                : 0x01 (TI)              */           
/*     23-16 bits   Function type/Library ID : 0x05 (MEDICALLIB)    */  
/*     15-0 bits are used to identify the function within the library */        
/**********************************************************************/
/* To be added in Future Releases */

/**********************************************************************/  
/*                                                                     */
/*           POWER CONTROL Library functions                           */
/*     31-24 bits   Vendor ID                : 0x01 (TI)               */           
/*     23-16 bits   Function type/Library ID : 0x06 (PWRCNTRLLIB)      */  
/*     15-0 bits are used to identify the function within the library  */        
/**********************************************************************/
/* To be added in Future Releases */

/**********************************************************************/  
/*                                                                     */
/*           AUDIO SPEECH Library functions                            */
/*     31-24 bits   Vendor ID                : 0x01 (TI)               */           
/*     23-16 bits   Function type/Library ID : 0x07 (AUDSPECHLIB)      */  
/*     15-0 bits are used to identify the function within the library  */        
/**********************************************************************/
/* To be added in Future Releases */

/******************************************************************
*  Constants to be shared by the codec and the application code   *
*******************************************************************/
/* Input buffer ID */
 #define INBUF0  0
 #define INBUF1  1
 #define INBUF2  2
 #define INBUF3  3
 #define INBUF4  4
 #define INBUF5  5
 #define INBUF6  6
 #define INBUF7  7
 #define INBUF8  8
 #define INBUF9  9
 #define INBUF10 10
 #define INBUF11 11
 #define INBUF12 12
 #define INBUF13 13
 #define INBUF14 14
 #define INBUF15 15


/* Output buffer ID  */
 #define OUTBUF0  0
 #define OUTBUF1  1
 #define OUTBUF2  2
 #define OUTBUF3  3
 #define OUTBUF4  4
 #define OUTBUF5  5
 #define OUTBUF6  6
 #define OUTBUF7  7
 #define OUTBUF8  8
 #define OUTBUF9  9
 #define OUTBUF10 10
 #define OUTBUF11 11
 #define OUTBUF12 12
 #define OUTBUF13 13
 #define OUTBUF14 14
 #define OUTBUF15 15

/************************************************************************ 
* Error check extention is introduced in return type of the Universal   *
* Process call by defining the following error msg                      *
* Error codes                                                           *
*                                                                       *
*   *  IUNIVERSAL_EPARAMFAIL : Error in parameters passed               *
*   *  IUNIVERSAL_EFXNIDFAIL : Error on Function ID or NULL ID passed   *
*                                                                       *
*************************************************************************/
#define IUNIVERSAL_EPARAMFAIL IUNIVERSAL_EFAIL-5
#define IUNIVERSAL_EFXNIDFAIL IUNIVERSAL_EFAIL-6
    
/************************* Common input structures ********************************************
*  Fxn_struct:
*  Fxn_struct is a structure used to pass the function Id and
*  the parameters to the function being called in the API call.
*  Every function in the API call will be contained in a seperate 
*  fxn_struct. Multiple function calls are made using a single API call 
*  by passing an array of Fxn_stuct in the extended Input Argument structure (IC6Accel_InArgs) 
************************************************************************************************/
typedef struct{
                int FxnID;
                int  Param_ptr_offset;                       // This is the offset of the param structure within overall inArgs
               }Fxn_struct;


/**********************Extended Input Structure IC6Accel_InArgs****************************************
* IC6xPP_InArgs is an extended input argumet structure that follows the 
 structure format as defined in InBufs and OutBufs. It carries with it,
 the number of function calls defined in the API call and the an Array of Fxn_structs (defined abover)
 to carry the parameters attributed to the corresponding function call
*******************************************************************************************************/
typedef struct {
                int size;
                int Num_fxns;
                Fxn_struct fxn[MAX_FXN_CALLS];
} IC6Accel_InArgs ;


/************************Parameter structure for the DSP Library functions*******************************
*    
*  * All input and output arrays/buffers are passed using inBufs abd outBufs in the Iuniversal framework
*  * Parameter structures carry inBufs IDs and outBuf IDs of parameter arrays and the scalar parameters  
*  
*  Naming Convention for Array IDs: [Format:  Prefix_ArrayID]
*                 
*                Prefix: parameter name as specified in library documentation
*                ArrayID#:      InArrID : Input array ID
*                               OutArrID: Output arrayID
*                                 # : Order Number in set of arrays in the parameter set 
*********************************************************************************************************/                            

/* Function Call: void DSP_fft16x16(short * restrict w, int nx, short * restrict x, short * restrict y)*/
typedef struct DSP_fft16x16_Params{
                                    unsigned int w_InArrID1;    /* twiddle factor */
                                    int  nx;                    /* Number of points*/
                                    unsigned int x_InArrID2;    /* Input signal*/
                                    unsigned int y_OutArrID1;   /* FFT output */
                                   }DSP_fft16x16_Params;

/* Function call: void DSP_ifft16x16(short *w, int nx, short *x, short *y)                              */
typedef struct DSP_ifft16x16_Params{
                                     unsigned int w_InArrID1;    /* twiddle factor */
                                     int  nx;                    /* Number of points*/
                                     unsigned int x_InArrID2;    /* Input signal*/
                                     unsigned int y_OutArrID1;   /* IFFT output */
                                   }DSP_ifft16x16_Params;

/* Function call : void DSP_mat_mul(short * restrict x, int r1, int c1, short * restrict y, int c2, short
* restrict r, int qs)                                                                                   */
typedef struct DSP_mat_mul_Params{ 
                                   unsigned int x_InArrID1;
                                   int  r1;
                                   int  c1;
                                   unsigned int y_InArrID2;
                                   int  c2;
                                   unsigned int r_OutArrID1;
                                   int  qs;
                                  }DSP_mat_mul_Params;

/* Function call : DSP_fir_gen (short * restrict x, short * restrict h, short * restrict r, int nh,int nr)*/
typedef struct DSP_fir_gen_Params{ 
                                   unsigned int x_InArrID1;
                                   unsigned int h_InArrID2;
                                   unsigned int r_OutArrID1;
                                   int  nh;
                                   int  nr;
                                  }DSP_fir_gen_Params;

/*Function call : DSP_autocor(short * restrict r, short * restrict x, int nx, int nr)*/
typedef struct DSP_autocor_Params{ 
                                   unsigned int r_OutArrID1;
                                   unsigned int x_InArrID1;
                                   int  nx;
                                   int  nr;
                                  }DSP_autocor_Params;

/*Function call : DSP_firlms2(short *h, short *x, short b, int nh)*/
typedef struct DSP_firlms2_Params{ 
                                   unsigned int  h_InArrID1;
                                   unsigned int  x_InArrID2;
                                   short b;
                                   int   nh;
                                   unsigned int  OutArrID1;
                                  }DSP_firlms2_Params;

/*Function call : void DSP_fft16x16_imre(short *w, int nx, short *x, short *y)  */
typedef struct DSP_fft16x16_imre_Params{
                                         unsigned int w_InArrID1;    /* twiddle factor */
                                         int  nx;                    /* Number of points*/
                                         unsigned int x_InArrID2;    /* Input signal*/
                                         unsigned int y_OutArrID1;   /* FFT output */
                                       }DSP_fft16x16_imre_Params;

/* Function call : void DSP_fft16x16r(int nx, short *x, short *w, unsigned char *brev, short *y, 
                                                                           int offset, int n_max)*/
typedef struct DSP_fft16x16r_Params{
                                     int  nx;
                                     unsigned int x_InArrID1;
                                     unsigned int w_InArrID2;
                                     unsigned int OutArrID1;
                                     int  radix;
                                     int  offset;
                                     int  nmax;
                                    }DSP_fft16x16r_Params;

/* Function call : void DSP_fft16x32(short *w, int nx, int *x, int *y)*/
typedef struct DSP_fft16x32_Params{
                                    unsigned int w_InArrID1;
                                    int  nx;
                                    unsigned int x_InArrID2;
                                    unsigned int y_OutArrID1;
                                   }DSP_fft16x32_Params;

/* Function call : DSP_fft32x32(short *w, int nx, short *x, short *y) */
typedef struct DSP_fft32x32_Params{
                                    unsigned int w_InArrID1;
                                    int  nx;
                                    unsigned int x_InArrID2;
                                    unsigned int y_OutArrID1;
                                   }DSP_fft32x32_Params;

/* Function call : void DSP_fft32x32s(int *w, int nx, int *x, int *y)*/
typedef struct DSP_fft32x32s_Params{
                                     unsigned int w_InArrID1;
                                     int  nx;
                                     unsigned int x_InArrID2;
                                     unsigned int y_OutArrID1;
                                    }DSP_fft32x32s_Params;

/* Function call : void DSP_ifft16x16_imre(short * restrict w, int nx, short * restrict x, short * restrict y)*/
typedef struct DSP_ifft16x16_imre_Params{
                                         unsigned int w_InArrID1; /* twiddle factor */
                                         int  nx;                 /* Number of points*/
                                         unsigned int x_InArrID2; /* Input signal*/
                                         unsigned int Y_OutArrID1;/* FFT output */
                                        }DSP_ifft16x16_imre_Params;

/* Function call : void DSP_ifft16x32(short * restrict w, int nx, int * restrict x, int * restrict y)*/
typedef struct DSP_ifft16x32_Params{
                                     unsigned int w_InArrID1;
                                     int  nx;
                                     unsigned int x_InArrID2;
                                     unsigned int y_OutArrID1;
                                    }DSP_ifft16x32_Params;

/* Function call : DSP_ifft32x32(short *w, int nx, short *x, short *y) */
typedef struct DSP_ifft32x32_Params{
                                     unsigned int w_InArrID1;
                                     int  nx;
                                     unsigned int x_InArrID2;
                                     unsigned int y_OutArrID1;
                                    }DSP_ifft32x32_Params;

/* Function call : void DSP_fir_cplx (short * restrict x, short * restrict h, short * restrict r, int nh, int nr)*/
typedef struct DSP_fir_cplx_Params{
                                    unsigned int x_InArrID1;
                                    unsigned int h_InArrID2;
                                    unsigned int r_OutArrID1;
                                    int  nh;
                                    int  nr;
                                   }DSP_fir_cplx_Params;

/* Function call : DSP_fir_cplx _hM4X4(short * restrict x, short * restrict h, short * restrict r, int nh, int nr) */
typedef struct DSP_fir_cplx_hM4X4_Params{
                                          unsigned int x_InArrID1;
                                          unsigned int h_InArrID2;
                                          unsigned int r_OutArrID1;
                                          int  nh;
                                          int  nr;
                                         }DSP_fir_cplx_hM4X4_Params;

/* Function call : void DSP_fir_gen_hM17_rA8X8 (short * restrict x, short * restrict h, short * restrict r,
                                                                                        int nh, int nr)*/
typedef struct DSP_fir_gen_hM17_rA8X8_Params{
                                              unsigned int InArrID1;
                                              unsigned int InArrID2;
                                              unsigned int OutArrID1;
                                              int nh;
                                              int nr;
                                             }DSP_fir_gen_hM17_rA8X8_Params;

/* Function call : void DSP_fir_r4 (short * restrict x, short * restrict h, short * restrict r, int nh,
                                                                                               int nr)*/
typedef struct DSP_fir_r4_Params{
                                  unsigned int x_InArrID1;
                                  unsigned int h_InArrID2;
                                  unsigned int r_OutArrID1;
                                  int  nh;
                                  int  nr;
                                 }DSP_fir_r4_Params;

/* Function call : void DSP_fir_r8 (short * restrict x, short * h, short * restrict r, int nh, int nr)*/
typedef struct DSP_fir_r8_Params{
                                  unsigned int x_InArrID1;
                                  unsigned int h_InArrID2;
                                  unsigned int r_OutArrID1;
                                  int  nh;
                                  int  nr;
                                 }DSP_fir_r8_Params;

/* Function call : void DSP_fir_r8_hM16_rM8A8X8 (short * restrict x, short * h, short * restrict r,
                                                                                 int nh, int nr)*/
typedef struct DSP_fir_r8_hM16_rM8A8X8_Params{
                                               unsigned int InArrID1;
                                               unsigned int InArrID2;
                                               unsigned int OutArrID1;
                                               int  nh;
                                               int  nr;
                                              }DSP_fir_r8_hM16_rM8A8X8_Params;

/* Function call : void DSP_fir_sym (short * restrict x, short * restrict h, short * restrict r, int nh,
                                                                                        int nr, int s)*/
typedef struct DSP_fir_sym_Params{
                                   unsigned int InArrID1;
                                   unsigned int InArrID2;
                                   unsigned int OutArrID1;
                                   int  nh;
                                   int  nr;
                                   int  s;
                                  }DSP_fir_sym_Params;

/* Function call : short DSP_iir(short x, short * restrict h, int nh, short * restrict b) */
/* function modified to work on multiple samples of input*/
typedef struct DSP_iir_Params{
                                unsigned int  x_InArrID1;
                                unsigned int  h_InArrID2;
                                int   nh;
                                unsigned int  b_InArrID3;
                                unsigned int  Ret_OutArrID1;
                                int   n;
                               }DSP_iir_Params;

/* Function call : DSP_iir_lat(short * restrict x, int nx, short * restrict k, int nk, int * restrict b,
                              short * restrict r)*/
typedef struct DSP_iir_lat_Params{
                                   unsigned int InArrID1;
                                   int  nx;
                                   unsigned int InArrID2;
                                   int  nk;
                                   unsigned int InArrID3;
                                   unsigned int OutArrID1;
                                  }DSP_iir_lat_Params;

/* Function call : int DSP_dotp_sqr(int G, short * restrict x, short * restrict y, int * restrict r, int nx)*/
typedef struct DSP_dotp_sqr_Params{
                                    int  G;
                                    unsigned int x_InArrID1;
                                    unsigned int y_InArrID2;
                                    unsigned int r_OutArrID1;
                                    int  nx;
                                    unsigned int Ret_outArrID2;
                                   }DSP_dotp_sqr_Params;

/* Function call : int DSP_dotprod(short * restrict x, short * restrict y, int nx)*/
typedef struct DSP_dotprod_Params{
                                    unsigned int x_InArrID1;
                                    unsigned int y_InArrID2;
                                    int  nx;
                                    unsigned int Ret_outArrID1;
                                   }DSP_dotprod_Params;

/* Function call : short DSP_maxval (short *x, int nx)*/
typedef struct DSP_maxval_Params{
                                  unsigned int x_InArrID1;
                                  int nx;
                                  unsigned int OutArrID1;
                                 }DSP_maxval_Params;

/* Function call : int DSP_maxidx (short *x, int nx)*/
typedef struct DSP_maxidx_Params{
                                  unsigned int InArrID1;
                                  int nx;
                                  unsigned int OutArrID1;
                                 }DSP_maxidx_Params;

/* Function call : short DSP_minval (short *x, int nx)*/
typedef struct DSP_minval_Params{
                                  unsigned int InArrID1;
                                  int nx;
                                  unsigned int OutArrID1;
                                 }DSP_minval_Params;

/* Function call : void DSP_mul32(int * restrict x, int * restrict y, int * restrict r, short nx)*/
typedef struct DSP_mul32_Params{
                                 unsigned int InArrID1;
                                 int nx;
                                 unsigned int OutArrID1;
                                }DSP_mul32_Params;

/* Function call : void DSP_neg32(int * restrict x, int * restrict r, short nx)*/
typedef struct DSP_neg32_Params{
                                 unsigned int InArrID1;
                                 int nx;
                                 unsigned int OutArrID1;
                                }DSP_neg32_Params;

/* Function call : void DSP_recip16(short * restrict x, short * restrict rfrac, short * restrict rexp,
                                    short nx)*/
typedef struct DSP_recip16_Params{
                                   unsigned int InArrID1;
                                   unsigned int InArrID2;
                                   unsigned int OutArrID1;
                                   short nx;                                     
                                  }DSP_recip16_Params;

/* Function call : int DSP_vecsumsq (short *x, int nx)*/
typedef struct DSP_vecsumsq_Params{
                                    unsigned int InArrID1;
                                    int nx;
                                    unsigned int OutArrID1;
                                   }DSP_vecsumsq_Params;

/* Function call : void DSP_w_vec(short * restrict x, short * restrict y, short m, short * restrict r,
                                  short nr)*/
typedef struct DSP_w_vec_Params{
                                 unsigned int  x_InArrID1;
                                 unsigned int  y_InArrID2;
                                 short m;
                                 unsigned int  r_OutArrID1;
                                 int   nr;
                                }DSP_w_vec_Params;

/* Function call : void DSP_mat_trans(short * restrict x, short rows, short columns, short * restrict r)*/
typedef struct DSP_mat_trans_Params{
                                     unsigned int  x_InArrID1;
                                     short r1;
                                     short c1;
                                     unsigned int  OutArrID1;
                                    }DSP_mat_trans_Params;

/* Function call : short DSP_bexp(const int *x, short nx)*/
typedef struct DSP_bexp_Params{
                                unsigned int x_InArrID1;
                                short nx;
                                unsigned int OutArrID1;
                               }DSP_bexp_Params;

/* Function call : void blk_eswap16(void * restrict x, void * restrict r, int nx)*/
typedef struct blk_eswap16_Params{
                                   unsigned int x_InArrID1;
                                   unsigned int r_OutArrID1;
                                   int  nx;
                                  }blk_eswap16_Params;

/* Function call : void blk_eswap32(void * restrict x, void * restrict r, int nx)*/
typedef struct blk_eswap32_Params{
                                    unsigned int InArrID1;
                                    unsigned int OutArrID1;
                                    int  nx;
                                   }blk_eswap32_Params;

/* Function call : void blk_eswap64(void * restrict x, void * restrict r, int nx)*/
typedef struct blk_eswap64_Params{
                                   unsigned int InArrID1;
                                   unsigned int OutArrID1;
                                   int  nx;
                                  }blk_eswap64_Params;

/* Function call : void DSP_blk_move(short * restrict x, short * restrict r, int nx)*/
typedef struct DSP_blk_move_Params{
                                    unsigned int InArrID1;
                                    unsigned int OutArrID1;
                                    int  nx;
                                   }DSP_blk_move_Params;

/* Function call : void DSP_fltoq15(float * restrict x, short * restrict r, short nx)*/
typedef struct DSP_fltoq15_Params{
                                   unsigned int InArrID1;
                                   unsigned int OutArrID1;
                                   int  nx;
                                  }DSP_fltoq15_Params;

/* Function call : int minerror (short * restrict GSP0_TABLE, short * restrict errCoefs, int * restrict max_index)*/
typedef struct minerror_Params{
                                unsigned int GSP0TABLE_InArrID1;
                                unsigned int errCoefs_InArrID2;
                                int  max_index;
                                unsigned int OutArrID1;
                               }minerror_Params;

/* Function call : void DSP_q15tofl(short * restrict x, float * restrict r, short nx)*/
typedef struct DSP_q15tofl_Params{
                                   unsigned int InArrID1;
                                   unsigned int OutArrID1;
                                   int nx;
                                  }DSP_q15tofl_Params;

/*************************************************************************************/
/* Floating point DSP library for C67+ devices                                       */
/*************************************************************************************/

/*Function call: void DSPF_sp_autocor(float *restrict r, float *restrict x, const int nx, const int nr) */
typedef struct DSPF_sp_autocor_Params{ 
                                       unsigned int r_OutArrID1;
                                       unsigned int x_InArrID1;
                                       int  nx;
                                       int  nr;
                                       }DSPF_sp_autocor_Params;

/*Function call: void DSPF_sp_biquad(float *restrict x, float *b, float *a, float *delay, float *restrict y, const int n) */
typedef struct DSPF_sp_biquad_Params{
                                        unsigned int x_InArrID1;
                                        unsigned int b_InArrID2;
                                        unsigned int a_InArrID3;
                                        unsigned int delay_InArrID4;
                                        unsigned int y_OutArrID1;
                                        int n;
                                      }DSPF_sp_biquad_Params;

/*Function call: void DSPF_sp_convol(const float *x, const float *h, float *restrict y, const short nh, const short ny)*/
typedef struct DSPF_sp_convol_Params{
                                     unsigned int x_InArrID1;
                                     unsigned int h_InArrID2;
                                     unsigned int y_OutArrID1;
                                     short nh;
                                     short ny;
                                    }DSPF_sp_convol_Params;

/*Function call: float DSPF_sp_dotprod(const float * x, const float * y, const int n)*/
typedef struct DSPF_sp_dotprod_Params{
                                      unsigned int x_InArrID1;
                                      unsigned int y_InArrID2;
                                      int n;
                                      unsigned int RetOut_OutArrID1;
                                    }DSPF_sp_dotprod_Params;

/*Function call: void DSPF_sp_fftSPxSP(int N, float *ptr_x, float *ptr_w, float *ptr_y, unsigned char *brev, int n_min, int offset, int n_max)*/
typedef struct DSPF_sp_fftSPxSP_Params{
                                       int N;
                                       unsigned int x_InArrID1;
                                       unsigned int w_InArrID2;
                                       unsigned int y_OutArrID1;
                                       unsigned int brev_InArrID3;
                                       int n_min;
                                       int offset;
                                       int n_max;
                                      }DSPF_sp_fftSPxSP_Params;
  
/*Function call: void DSPF_sp_fir_cplx(const float * x, const float * h, float * restrict y, int nh, int ny)*/
typedef struct DSPF_sp_fir_cplx_Params{
                                       unsigned int x_InArrID1;
                                       unsigned int h_InArrID2;
                                       unsigned int y_OutArrID1;
                                       int nh;
                                       int ny;
                                      }DSPF_sp_fir_cplx_Params;

/*Function call: void DSPF_sp_fir_gen(const float * restrict x, const float * restrict h, float * restrict y, int nh, int ny)*/
typedef struct DSPF_sp_fir_gen_Params{
                                      unsigned int x_InArrID1;
                                      unsigned int h_InArrID2;
                                      unsigned int y_OutArrID1;
                                      int nh;
                                      int ny;
                                      }DSPF_sp_fir_gen_Params;

/*Function call: void DSPF_sp_ifftSPxSP(int N, float *ptr_x, float *ptr_w, float *ptr_y, unsigned char *brev, int n_min, int offset, int n_max)*/
typedef struct DSPF_sp_ifftSPxSP_Params{
                                       int N;
                                       unsigned int x_InArrID1;
                                       unsigned int w_InArrID2;
                                       unsigned int y_OutArrID1;
                                       unsigned int brev_InArrID3;
                                       int n_min;
                                       int offset;
                                       int n_max;
                                       }DSPF_sp_ifftSPxSP_Params;

/*Function call: void DSPF_sp_iir(float *restrict y1, const float * x, float *restrict y2, const float * hb, const float * ha, int n)*/
typedef struct DSPF_sp_iir_Params{
                                  unsigned int y1_OutArrID1;
                                  unsigned int x_InArrID1;
                                  unsigned int y2_OutArrID2;
                                  unsigned int hb_InArrID2;
                                  unsigned int ha_InArrID3;
                                  int n;
                                  }DSPF_sp_iir_Params;

/*Function call: void DSPF_sp_mat_mul(float *x1, const int r1, const int c1, float *x2, const int c2, float *restrict y)*/
typedef struct DSPF_sp_mat_mul_Params{
                                      unsigned int x1_InArrID1;
                                      int  r1;
                                      int  c1;
                                      unsigned int x2_InArrID2;
                                      int  c2;
                                      unsigned int y_OutArrID1;
                                     }DSPF_sp_mat_mul_Params;

/*Function call: void DSPF_sp_mat_mul_cplx(float *x1, const int r1, const int c1, float *x2, const int c2, float *restrict y)*/
typedef struct DSPF_sp_mat_mul_cplx_Params{
                                      unsigned int x1_InArrID1;
                                      int  r1;
                                      int  c1;
                                      unsigned int x2_InArrID2;
                                      int  c2;
                                      unsigned int y_OutArrID1;
                                     }DSPF_sp_mat_mul_cplx_Params;

/*Function call: void DSPF_sp_mat_trans(const float *restrict x, const int rows, const int cols, float *restrict y)*/
typedef struct DSPF_sp_mat_trans_Params{
                                        unsigned int  x_InArrID1;
                                        short rows;
                                        short cols;
                                        unsigned int  y_OutArrID1;
                                       } DSPF_sp_mat_trans_Params;

/*Function call: void DSPF_sp_vecmul(const float * x1, const float * x2, float *restrict y, const int n)*/
typedef struct DSPF_sp_vecmul_Params{
                                     unsigned int  x1_InArrID1;
                                     unsigned int  x2_InArrID2;
                                     unsigned int  y_OutArrID1;
                                     int n;
                                     }DSPF_sp_vecmul_Params;

/*Function call: void DSPF_sp_vecrecip(const float * x, float *restrict y, const int n)*/
typedef struct DSPF_sp_vecrecip_Params{
                                       unsigned int  x_InArrID1;
                                       unsigned int  y_OutArrID1;
                                       int n;
                                      }DSPF_sp_vecrecip_Params;

/*Function call: float DSPF_sp_vecsum_sq(const float * x, const int n)*/
typedef struct DSPF_sp_vecsum_sq_Params{
                                        unsigned int  x_InArrID1;
                                        int n;
                                        unsigned int  OutRet_OutArrID1;
                                        }DSPF_sp_vecsum_sq_Params;

/*Function call: void DSPF_sp_w_vec(const float *x1, const float *x2, const float m, float *restrict y, const int n)*/
typedef struct DSPF_sp_w_vec_Params{
                                     unsigned int  x1_InArrID1;
                                     unsigned int  x2_InArrID2;
                                     float m;
                                     unsigned int  y_OutArrID1;
                                     int n;
                                    }DSPF_sp_w_vec_Params;

typedef struct DSPF_complxtorealnimg_Params{
                                            unsigned int  cplx_InArrID1;
                                            unsigned int  real_OutArrID1;
                                            unsigned int  img_OutArrID2;
                                            int n;
                                           }DSPF_complxtorealnimg_Params;

/* ***********************************************************************************/
/* Parameter stucture definitions for the Image Library functions                    */
/*************************************************************************************/
/* Function call : void IMG_sobel_3x3_8(const unsigned char *in_data, unsigned char *out_data, short cols, short rows)*/
typedef struct IMG_sobel_3x3_8_Params{
                                      unsigned int indata_InArrID1;
                                      unsigned int outdata_OutArrID1; 
                                      int Col;
                                      int Row;
                                     }IMG_sobel_3x3_8_Params;

/* Function call : void IMG_sobel_3x3_8(const unsigned char *in_data, unsigned char *out_data, short cols, short rows)*/
typedef struct IMG_sobel_3x3_8_16_Params{
                                      unsigned int indata_InArrID1;
                                      unsigned int outdata_OutArrID1; 
                                      int Col;
                                      int Row;
                                     }IMG_sobel_3x3_8_16_Params;

/* Function call : void IMG_sobel_3x3_16s (const short *restrict in, const short *restrict out, short cols,
                                           short rows)*/
typedef struct IMG_sobel_3x3_16_Params{
                                       unsigned int indata_InArrID1;
                                       unsigned int outdata_OutArrID1; 
                                       short Col;
                                       short Row;
                                     }IMG_sobel_3x3_16_Params;

/* Function call : IMG_histogram_8 (const unsigned char * restrict in_data, int n, short accumulate,
unsigned short * restrict t_hist, unsigned short * restrict hist)*/
typedef struct IMG_histogram_8_Params{
                                      unsigned int  indata_InArrID1;
                                      int   n;
                                      short accumulate;
                                      unsigned int  t_hist_OutArrID1;
                                      unsigned int  hist_OutArrID2; 
                                     }IMG_histogram_8_Params;

/* Function call : void IMG_histogram_16(unsigned short *restrict in, short *restrict hist, short *restrict
t_hist, int n, int accumulate, int img_bits)*/
typedef struct IMG_histogram_16_Params{
                                       unsigned int  in_InArrID1;
                                       int   n;
                                       short accumulate;
                                       unsigned int  t_hist_OutArrID1;
                                       unsigned int  hist_OutArrID2;
                                       int   img_bits; 
                                      }IMG_histogram_16_Params;

/* Function call : void IMG_median_3x3_8(const unsigned char * restrict in_data, int cols, unsigned char *restrict out_data)*/
                                         
typedef struct IMG_median_3x3_8_Params{
                                       unsigned int InArrID1;
                                       int  Col;
                                       unsigned int OutArrID1; 
                                      }IMG_median_3x3_8_Params;

/* Function call : void IMG_median_3x3_16 (const short *restrict i_data, int n, short *restrict o_data)*/
typedef struct IMG_median_3x3_16_Params{
                                        unsigned int idata_InArrID1;
                                        int  nwidth;
                                        unsigned int odata_OutArrID1; 
                                      }IMG_median_3x3_16_Params;

/* Function call : void IMG_median_3x3_16s (const short *restrict i_data, int n, short *restrict o_data)*/
typedef struct IMG_median_3x3_16s_Params{
                                         unsigned int idata_InArrID1;
                                         int  nwidth;
                                         unsigned int odata_OutArrID1; 
                                      }IMG_median_3x3_16s_Params;

/* Additional functions that allow raw and columns to be specified */
typedef struct IMG_median_3x3_8_Frame_Params{
                                       unsigned int InArrID1;
                                       int  Row;
                                       int  Col;
                                       unsigned int OutArrID1;
                                      }IMG_median_3x3_8_Frame_Params;

/* Function call : void IMG_median_3x3_16 (const short *restrict i_data, int n, short *restrict o_data)*/
typedef struct IMG_median_3x3_16_Frame_Params{
                                        unsigned int idata_InArrID1;
                                        int  Row;
                                        int  nwidth;
                                        unsigned int odata_OutArrID1;
                                      }IMG_median_3x3_16_Frame_Params;

/* Function call : void IMG_median_3x3_16s (const short *restrict i_data, int n, short *restrict o_data)*/
typedef struct IMG_median_3x3_16s_Frame_Params{
                                         unsigned int idata_InArrID1;
                                         int  Row;
                                         int  nwidth;
                                         unsigned int odata_OutArrID1;
                                      }IMG_median_3x3_16s_Frame_Params;


/* Function call : IMG_ycbcr422p_rgb565(const short * restrict coeff, const unsigned char * restrict
                                        y_data, const unsigned char * restrict cb_data, 
                                        const unsigned char * restrict cr_data,
                                        unsigned short * restrict rgb_data, unsigned num_pixels)*/
typedef struct IMG_ycbcr422pl_to_rgb565_Params{
                                               unsigned int     coeff_InArrID1;
                                               unsigned int     y_data_InArrID2;
                                               unsigned int     cb_data_InArrID3;
                                               unsigned int     cr_data_InArrID4;
                                               unsigned int     rgb_data_OutArrID1;
                                               unsigned num_pixels;
                                              }IMG_ycbcr422pl_to_rgb565_Params;


/* Function call : IMG_yuv420pl_to_rgb565(const short *coeff,int Height, int Width, unsigned char *pbuf_y,
                               unsigned char *pbuf_cb,unsigned char *pbuf_cr,unsigned short *pbuf_rgb)*/
typedef struct IMG_yuv420pl_to_rgb565_Params{
                                             unsigned int     coeff_InArrID1;
                                             int      Height;
                                             int      Width;
                                             unsigned int     y_data_InArrID2;
                                             unsigned int     cb_data_InArrID3;
                                             unsigned int     cr_data_InArrID4;
                                             unsigned int     rgb_data_OutArrID1;
                                            }IMG_yuv420pl_to_rgb565_Params;


typedef struct IMG_yuv420pl16_to_rgb565_Params{
                                               unsigned int     coeff_InArrID1;
                                               int      Height;
                                               int      Width;
                                               unsigned int     y_data_InArrID2;
                                               unsigned int     cb_data_InArrID3;
                                               unsigned int     cr_data_InArrID4;
                                               unsigned int     rgb_data_OutArrID1;
                                              }IMG_yuv420pl16_to_rgb565_Params;

/* Function call : IMG_ycbcr422pl16_rgb565(const short * restrict coeff, const unsigned char * restrict
                                        y_data, const unsigned char * restrict cb_data, 
                                        const unsigned char * restrict cr_data,
                                        unsigned short * restrict rgb_data, unsigned num_pixels)*/
typedef struct IMG_ycbcr422pl16_to_rgb565_Params{
                                                 unsigned int     coeff_InArrID1;
                                                 int      Height;
                                                 int      Width;
                                                 unsigned int     y_data_InArrID2;
                                                 unsigned int     cb_data_InArrID3;
                                                 unsigned int     cr_data_InArrID4;
                                                 unsigned int     rgb_data_OutArrID1;
                                                 unsigned num_pixels;
                                                }IMG_ycbcr422pl16_to_rgb565_Params;

/* Function call : IMG_conv_3´3_i8_c8s(const unsigned char * restrict in_data, unsigned char *
restrict out_data, int cols, const char * restrict mask, int shift)*/
typedef struct IMG_conv_3x3_i8_c8s_Params{
                                           unsigned int in_data_InArrID1;
                                           unsigned int out_data_OutArrID1;
                                           int  cols;
                                           unsigned int mask_InArrID2;
                                           int  shift;
                                          }IMG_conv_3x3_i8_c8s_Params;

/* Function call : void IMG_conv_3x3_i16s_c16s (const short *restrict imgin_ptr, short *restrict
imgout_ptr, short width, short pitch, const short *restrict mask_ptr, short shift)*/
typedef struct IMG_conv_3x3_i16s_c16s_Params{
                                             unsigned int in_data_InArrID1;
                                             unsigned int out_data_OutArrID1;
                                             short width;
                                             short pitch;
                                             unsigned int mask_InArrID2;
                                             short shift;
                                            }IMG_conv_3x3_i16s_c16s_Params;

/* Function call : IMG_conv_3´3_i8_c8s(const unsigned char * restrict in_data, unsigned char *
restrict out_data, int cols, const char * restrict mask, int shift)*/
typedef struct IMG_conv_3x3_i8_c8s_Frame_Params{
                                           unsigned int in_data_InArrID1;
                                           unsigned int out_data_OutArrID1;
                                           int  rows;
                                           int  cols;
                                           unsigned int mask_InArrID2;
                                           int  shift;
                                          }IMG_conv_3x3_i8_c8s_Frame_Params;

/* Function call : void IMG_conv_3x3_i16s_c16s (const short *restrict imgin_ptr, short *restrict
imgout_ptr, short width, short pitch, const short *restrict mask_ptr, short shift)*/
typedef struct IMG_conv_3x3_i16s_c16s_Frame_Params{
                                             unsigned int in_data_InArrID1;
                                             unsigned int out_data_OutArrID1;
                                             short rows;
                                             short width;
                                             short pitch;
                                             unsigned int mask_InArrID2;
                                             short shift;
                                            }IMG_conv_3x3_i16s_c16s_Frame_Params;


/* Function call :IMG_corr_3x3_i8_c8(const unsigned char *inptr, int *restrict outptr, int n_out, int
x_dim, const unsigned char *mask, const short shift, int round)*/
typedef struct IMG_corr_3x3_i8_c8_Params{
                                          unsigned int in_InArrID1;
                                          unsigned int out_OutArrID1;
                                          int n_out;
                                          int x_dim;
                                          unsigned int mask_InArrID2;
                                          short shift;
                                          int round;
                                         }IMG_corr_3x3_i8_c8_Params;

/* Function call : IMG_corr_3x3_i16_c16s (const short *restrict imgin_ptr, int *restrict imgout_ptr,
short width, short pitch, const short *restrict mask_ptr, short shift, int round)*/
typedef struct IMG_corr_3x3_i16s_c16s_Params{
                                              unsigned int   imgin_InArrID1;
                                              unsigned int   imgout_OutArrID1;
                                              short  width;
                                              short  pitch;
                                              unsigned int   mask_InArrID2;
                                              short  shift;
                                              int  round;
                                             }IMG_corr_3x3_i16s_c16s_Params;

/* Function call : void IMG_corr_3x3_i8_c16s (const unsigned char *restrict imgin_ptr, int *restrict
                  imgout_ptr, short width, short pitch, const short *restrict mask_ptr, int round)*/
typedef struct IMG_corr_3x3_i8_c16s_Params{
                                            unsigned int   imgin_InArrID1;
                                            unsigned int   imgout_OutArrID1;
                                            short  width;
                                            short  pitch;
                                            unsigned int   mask_InArrID2;
                                          }IMG_corr_3x3_i8_c16s_Params;

/* Function call :IMG_corr_3x3_i8_c8(const unsigned char *inptr, int *restrict outptr, int n_out, int
x_dim, const unsigned char *mask, const short shift, int round)*/
typedef struct IMG_corr_3x3_i8_c8_Frame_Params{
                                          unsigned int in_InArrID1;
                                          unsigned int out_OutArrID1;
                                          int n_out;
                                          int rows;
                                          int x_dim;
                                          unsigned int mask_InArrID2;
                                          short shift;
                                          int round;
                                         }IMG_corr_3x3_i8_c8_Frame_Params;

/* Function call : IMG_corr_3x3_i16_c16s (const short *restrict imgin_ptr, int *restrict imgout_ptr,
short width, short pitch, const short *restrict mask_ptr, short shift, int round)*/
typedef struct IMG_corr_3x3_i16s_c16s_Frame_Params{
                                              unsigned int   imgin_InArrID1;
                                              unsigned int   imgout_OutArrID1;
                                              short  rows;
                                              short  width;
                                              short  pitch;
                                              unsigned int   mask_InArrID2;
                                              short  shift;
                                              int  round;
                                             }IMG_corr_3x3_i16s_c16s_Frame_Params;

/* Function call : void IMG_corr_3x3_i8_c16s (const unsigned char *restrict imgin_ptr, int *restrict
                  imgout_ptr, short width, short pitch, const short *restrict mask_ptr, int round)*/
typedef struct IMG_corr_3x3_i8_c16s_Frame_Params{
                                            unsigned int   imgin_InArrID1;
                                            unsigned int   imgout_OutArrID1;
                                            short  rows;
                                            short  width;
                                            short  pitch;
                                            unsigned int   mask_InArrID2;
                                          }IMG_corr_3x3_i8_c16s_Frame_Params;

/* Function call : IMG_fdct_8x8(short *fdct_data, unsigned num_fdcts)*/
typedef struct IMG_fdct_8x8_Params{
                                    unsigned int     in_InArrID1;
                                    unsigned num_fdcts;
                                   }IMG_fdct_8x8_Params;

/* Function call : void IMG_mulS_8(short * restrict imgR,int * restrict imgW,short constData,int count )*/
typedef struct IMG_mulS_8_Params{
                                   unsigned int imgR_InArrID1;
                                   unsigned int imgW_OutArrID1;
                                   char constData;
                                   int  count;
                                  }IMG_mulS_8_Params;

/* Function call : void IMG_mulS_16s(short * restrict imgR,int * restrict imgW, short constData, int count )*/
typedef struct IMG_mulS_16s_Params{
                                   unsigned int imgR_InArrID1;
                                   unsigned int imgW_OutArrID1;
                                   short constData;
                                   int  count;
                                  }IMG_mulS_16s_Params;

/* Function call : void IMG_addS_8(char * restrict imgR, char * restrict imgW, char constData,int count ) */ 
typedef struct IMG_addS_8_Params{
                                  unsigned int imgR_InArrID1;
                                  unsigned int imgW_OutArrID1;
                                  char constData;
                                  int  count;
                                 }IMG_addS_8_Params;

/* Function call : void IMG_addS_16s(short * restrict imgR,short * restrict imgW, short constData, int count) */              
typedef struct IMG_addS_16s_Params{
                                   unsigned int  imgR_InArrID1;
                                   unsigned int  imgW_OutArrID1;
                                   short constData;
                                   int   count;
                                  }IMG_addS_16s_Params;

/* Function call : void IMG_subS_8(char * restrict imgR,char * restrict imgW, char constData,int count )*/
typedef struct IMG_subS_8_Params{
                                  unsigned int imgR_InArrID1;
                                  unsigned int imgW_OutArrID1;
                                  char constData;
                                  int  count;
                                 }IMG_subS_8_Params;

/* Function call : void IMG_subS_16s(short * restrict imgR, short * restrict imgW, short constData,int count)*/           
typedef struct IMG_subS_16s_Params{
                                   unsigned int  imgR_InArrID1;
                                   unsigned int  imgW_OutArrID1;
                                   short constData;
                                   int   count;
                                  }IMG_subS_16s_Params;

/* Function call : void IMG_yc_demux_le16_16 (int n, const unsigned short * yc, short *restrict y, short
*restrict cr, short *restrict cb)*/
typedef struct IMG_yc_demux_le16_16_Params{
                                           int n;
                                           unsigned int yc_InArrID1;
                                           unsigned int y_OutArrID1;
                                           unsigned int cr_OutArrID2;
                                           unsigned int cb_OutArrID3; 
                                          }IMG_yc_demux_le16_16_Params;

/* Function call :  void yc_demux_le8_c(int n, const unsigned char *restrict yc,unsigned char *restrict y, 
                                       unsigned char *restrict cr, unsigned char *restrict cb     )*/
typedef struct IMG_yc_demux_le8_c_Params{
                                           int n;
                                           unsigned int yc_InArrID1;
                                           unsigned int y_OutArrID1;
                                           unsigned int cr_OutArrID2;
                                           unsigned int cb_OutArrID3; 
                                          }IMG_yc_demux_le8_c_Params;

/* Function call : void IMG_dilate_bin(const unsigned char * restrict in_data, unsigned char * restrict
out_data, const char * restrict mask, int cols) */
typedef struct IMG_dilate_bin_Params{
                                      unsigned int InArrID1;
                                      unsigned int OutArrID1;
                                      unsigned int InArrID2; 
                                      int  Col;
                                     }IMG_dilate_bin_Params;


/* Function call : void IMG_erode_bin(const unsigned char * restrict in_data, unsigned char * restrict
out_data, const char * restrict mask, int cols)*/
typedef struct IMG_erode_bin_Params{
                                      unsigned int InArrID1;
                                      unsigned int OutArrID1;
                                      unsigned int InArrID2; 
                                      int  Col;
                                     }IMG_erode_bin_Params;

/* Function call : void IMG_pix_sat(int n, const short * restrict in_data, unsigned char * restrict out_data) */
typedef struct IMG_pix_sat_Params{
                                  int  Col;
                                  unsigned int InArrID1;
                                  unsigned int OutArrID1; 
                                 }IMG_pix_sat_Params;

/* void ycbcr422sp_to_ycbcr420pl(
    const unsigned char * y_src,    
    const unsigned char * cbcr_src, 
    unsigned char * restrict y_dst,
    unsigned char * restrict cb_dst,
    unsigned char * restrict cr_dst,
    unsigned int num_lines, 
    unsigned int width,
    unsigned int src_pitch, 
    unsigned int dst_y_pitch,
    unsigned int dst_cbcr_pitch
)
*/
typedef struct IMG_ycbcr422sp_to_ycbcr420pl_Params{
                                            unsigned int y_src_InArrID1;
                                            unsigned int cbcr_src_InArrID2;
                                            unsigned int y_dst_OutArrID1;
                                            unsigned int cb_dst_OutArrID2;
                                            unsigned int cr_dst_OutArrID3;
                                            unsigned int num_lines; 
                                            unsigned int width;
                                            unsigned int src_pitch; 
                                            unsigned int dst_y_pitch;
                                            unsigned int dst_cbcr_pitch;
                                            }IMG_ycbcr422sp_to_ycbcr420pl_Params;
                                             
/* void ycbcr422pl_to_ycbcr422sp 
(
    const unsigned char * y_dst,    
    const unsigned char * cbcr_dst, 
    unsigned char * restrict y_src,
    unsigned char * restrict cb_src,
    unsigned char * restrict cr_src,
    unsigned int num_lines, 
    unsigned int width,
    unsigned int dst_pitch, 
    unsigned int src_y_pitch,
    unsigned int src_cbcr_pitch
)*/
typedef struct IMG_ycbcr422pl_to_ycbcr422sp_Params{
                                            unsigned int y_dst_OutArrID1;
                                            unsigned int cbcr_dst_OutArrID2;
                                            unsigned int y_src_InArrID1;
                                            unsigned int cb_src_InArrID2;
                                            unsigned int cr_src_InArrID3;
                                            unsigned int num_lines; 
                                            unsigned int width;
                                            unsigned int dst_pitch; 
                                            unsigned int src_y_pitch;
                                            unsigned int src_cbcr_pitch;
                                            }IMG_ycbcr422pl_to_ycbcr422sp_Params;
/* void ycbcr420pl_to_ycbcr422sp 
(
    const unsigned char * y_dst,    
    const unsigned char * cbcr_dst, 
    unsigned char * restrict y_src,
    unsigned char * restrict cb_src,
    unsigned char * restrict cr_src,
    unsigned int num_lines, 
    unsigned int width,
    unsigned int dst_pitch, 
    unsigned int src_y_pitch,
    unsigned int src_cbcr_pitch
)*/
typedef struct IMG_ycbcr420pl_to_ycbcr422sp_Params{
                                            unsigned int y_dst_OutArrID1;
                                            unsigned int cbcr_dst_OutArrID2;
                                            unsigned int y_src_InArrID1;
                                            unsigned int cb_src_InArrID2;
                                            unsigned int cr_src_InArrID3;
                                            unsigned int num_lines; 
                                            unsigned int width;
                                            unsigned int dst_pitch; 
                                            unsigned int src_y_pitch;
                                            unsigned int src_cbcr_pitch;
                                            }IMG_ycbcr420pl_to_ycbcr422sp_Params;

/* void ycbcr422sp_to_ycbcr422ile 
(
    const unsigned char * y_src,    
    const unsigned char * cbcr_src, 
    unsigned char * restrict ycbcr_dst,
    unsigned int num_lines, 
    unsigned int width,
    unsigned int src_pitch, 
    unsigned int dst_ycbcr_pitch
) */
typedef struct IMG_ycbcr422sp_to_ycbcr422ile_Params{
                                            unsigned int y_src_InArrID1;
                                            unsigned int cbcr_src_InArrID2;
                                            unsigned int ycbcr_dst_OutArrID1;
                                            unsigned int num_lines; 
                                            unsigned int width;
                                            unsigned int src_pitch;
                                            unsigned int dst_ycbcr_pitch;
                                            }IMG_ycbcr422sp_to_ycbcr422ile_Params;

typedef struct IMG_RGB_To_Y_Params{
                                            unsigned int src_InArrID1;
                                            unsigned int dst_OutArrID1;
                                            unsigned int count; 
                                            }IMG_RGB_To_Y_Params;

/*
void yc_demux_be8_c
(
    int n,                               // Number of luma pixels    
    const unsigned char *restrict yc,    // Interbeaved luma/chroma  
    unsigned char       *restrict y,     // Luma plane (8-bit)       
    unsigned char       *restrict cr,    // Cr chroma plane (8-bit)  
    unsigned char       *restrict cb     // Cb chroma plane (8-bit)  
)
*/
typedef struct IMG_yc_demux_be8_Params{
					    int n;
                                            unsigned int yc_InArrID1;
                                            unsigned int y_OutArrID1;
                                            unsigned int cb_OutArrID2;
					    unsigned int cr_OutArrID3;
                                           }IMG_yc_demux_be8_Params;
/*void IMG_pix_expand(int n, const unsigned char * restrict in_data, short * restrict
out_data)
*/
typedef struct IMG_pix_expand_Params{
					    int n;
                                            unsigned int indata_InArrID1;
                                            unsigned int outdata_OutArrID1;
                                           }IMG_pix_expand_Params;

typedef struct IMG_thr_gt2thr_8_Params{
                                      unsigned int indata_InArrID1;
                                      unsigned int outdata_OutArrID1; 
                                      short Col;
                                      short Row;
				      unsigned int Threshold;
                                     }IMG_thr_gt2thr_8_Params;


/* Function call : IMG_conv_7x7_i8_c8s(const unsigned char * restrict in_data, unsigned char *
restrict out_data, short cols, short pitch, const char * restrict mask_ptr, short shift)*/
typedef struct IMG_conv_7x7_i8_c8s_Params{
                                           unsigned int indata_InArrID1;
                                           unsigned int outdata_OutArrID1;
                                           int  width;
                                           int  pitch;
                                           unsigned int indata_InArrID2;
                                           int shift;
                                          }IMG_conv_7x7_i8_c8s_Params;

/* Function call : IMG_conv_7x7_i8_c8s(const unsigned char * restrict in_data, unsigned char *
restrict out_data, short cols, short pitch, const char * restrict mask_ptr, short shift)*/
typedef struct IMG_conv_7x7_i8_c8s_frame_Params{
                                           unsigned int indata_InArrID1;
                                           unsigned int outdata_OutArrID1;
                                           int  height;
                                           int  width;
                                           unsigned int indata_InArrID2;
                                           int shift;
                                          }IMG_conv_7x7_i8_c8s_frame_Params;

/* Function call : IMG_conv_11x11_i8_c8s(const unsigned char * restrict in_data, unsigned char *
restrict out_data, short cols, short pitch, const char * restrict mask_ptr, short shift)*/
typedef struct IMG_conv_11x11_i8_c8s_Params{
                                           unsigned int indata_InArrID1;
                                           unsigned int outdata_OutArrID1;
                                           int  width;
                                           int  pitch;
                                           unsigned int indata_InArrID2;
                                           int shift;
                                          }IMG_conv_11x11_i8_c8s_Params;

/* Function call : IMG_conv_11x11_i8_c8s(const unsigned char * restrict in_data, unsigned char *
restrict out_data, short cols, short pitch, const char * restrict mask_ptr, short shift)*/
typedef struct IMG_conv_11x11_i8_c8s_frame_Params{
                                           unsigned int indata_InArrID1;
                                           unsigned int outdata_OutArrID1;
                                           int  height;
                                           int  width;
                                           unsigned int indata_InArrID2;
                                           int shift;
                                          }IMG_conv_11x11_i8_c8s_frame_Params;
/* ***********************************************************************************/
/* Parameter structure and pointer to the structure for the IQ math Library functions*/
/*************************************************************************************/
//#if DEVICE_TYPE == DEVICE_FIXED

/* Function call : I32_IQ _IQNmpy(I32_IQ f1, I32_IQ f2, U32_IQ q_format);*/
typedef struct IQNmpy_Params{
                              unsigned int   InArrID1;
                              unsigned int   InArrID2;
                              unsigned int qfmt;
                              unsigned int   OutArrID1;
                              unsigned int   n;
                             }IQNmpy_Params;

/* Function call : I32_IQ _IQNrmpy(I32_IQ x, I32_IQ y, Uword32 qfmt); */
typedef struct IQNrmpy_Params{
                              unsigned int InArrID1;
                              unsigned int InArrID2;
                              unsigned int qfmt;
                              unsigned int OutArrID1;
                              unsigned int   n;
                             }IQNrmpy_Params;

/* Function call : I32_IQ _IQNrsmpy(I32_IQ x, I32_IQ y, U32_IQ qfmt); */
typedef struct IQNrsmpy_Params{ 
                               unsigned int InArrID1;
                               unsigned int InArrID2;
                               unsigned int qfmt;
                               unsigned int OutArrID1;
                               unsigned int   n;
                              }IQNrsmpy_Params;

/* Function call : I32_IQ _IQNmpyI32int(I32_IQ x, I32_IQ y, U32_IQ qfmt);*/
typedef struct IQNmpyI32int_Params{
                                    unsigned int  InArrID1;
                                    unsigned int     InArrID2;
                                    unsigned int  qfmt;
                                    unsigned int  OutArrID1;
                                    unsigned int   n;
                                   }IQNmpyI32int_Params;

/* Function call : I32_IQ _IQNmpyI32frac(I32_IQ x, I32_IQ y, U32_IQ qfmt);*/
typedef struct IQNmpyI32frac_Params{
                                    unsigned int  InArrID1;
                                    unsigned int     InArrID2;
                                    unsigned int  qfmt;
                                    unsigned int  OutArrID1;
                                    unsigned int   n;
                                   }IQNmpyI32frac_Params;

/* Function call : I32_IQ  _IQNmpyIQx(I32_IQ in1, I32_IQ qfmt1,I32_IQ in2, I32_IQ qfmt2, U32_IQ qfmt);*/
typedef struct IQNmpyIQX_Params{
                                unsigned int   InArrID1;
                                int            qfmt1;
                                unsigned int   InArrID2;
                                int            qfmt2;
                                unsigned int   qfmt;
                                unsigned int   OutArrID1;
                                unsigned int   n;
                               }IQNmpyIQX_Params;

/* Function call : I32_IQ _IQNdiv(I32_IQ num, I32_IQ den, Uword32 qfmt);*/
typedef struct IQNdiv_Params{
                              unsigned int  num_InArrID1;
                              unsigned int  den_InArrID2;
                              unsigned int  qfmt;
                              unsigned int  OutArrID1;
                              unsigned int  n;
                             }IQNdiv_Params;

/* Function call : I32_IQ _IQNmag(I32_IQ x, I32_IQ y, U32_IQ qfmt);*/
typedef struct IQNmag_Params{
                              unsigned int  InArrID1;
                              unsigned int  InArrID2;
                              unsigned int  qfmt;
                              unsigned int  OutArrID1;
                              unsigned int    n;
                             }IQNmag_Params;

/* Function call : I32_IQ _IQNsqrt(I32_IQ x, U32_IQ qfmt);*/
typedef struct IQNsqrt_Params{
                              unsigned int   InArrID1;
                              unsigned int   qfmt;
                              unsigned int   OutArrID1;
                              unsigned int     n;
                             }IQNsqrt_Params;

/* Function call : I32_IQ _IQNisqrt(I32_IQ x, U32_IQ qfmt); */
typedef struct IQNisqrt_Params{
                              unsigned int InArrID1;
                              unsigned int qfmt;
                              unsigned int OutArrID1;
                              unsigned int n;
                             }IQNisqrt_Params;

/* Function call : I32_IQ _IQNsin(I32_IQ in, U32_IQ qfmt);*/
typedef struct IQNsin_Params{
                              unsigned int InArrID1;
                              unsigned int qfmt;
                              unsigned int OutArrID1;
                              unsigned int n;
                             }IQNsin_Params;

/* Function call : I32_IQ _IQNasin(I32_IQ in, U32_IQ qfmt);*/
typedef struct IQNasin_Params{
                              unsigned int InArrID1;
                              unsigned int qfmt;
                              unsigned int OutArrID1;
                              unsigned int n;
                             }IQNasin_Params;

/* Function call : I32_IQ _IQNsinPU(I32_IQ in, U32_IQ qfmt);*/
typedef struct IQNsinPU_Params{
                               unsigned int InArrID1;
                               unsigned int qfmt;
                               unsigned int OutArrID1;
                               unsigned int   n;
                             }IQNsinPU_Params;

/* Function call : I32_IQ _IQNcos(I32_IQ in, U32_IQ qfmt);*/
typedef struct IQNcos_Params{
                              unsigned int InArrID1;
                              unsigned int qfmt;
                              unsigned int OutArrID1;
                              unsigned int   n;
                             }IQNcos_Params;

/* Function call : I32_IQ _IQNacos(I32_IQ in, U32_IQ qfmt);*/
typedef struct IQNacos_Params{
                              unsigned int  InArrID1;
                              unsigned int  qfmt;
                              unsigned int  OutArrID1;
                              unsigned int    n;
                             }IQNacos_Params;

/* Function call : I32_IQ _IQNcosPU(I32_IQ in, U32_IQ qfmt); */
typedef struct IQNcosPU_Params{
                               unsigned int InArrID1;
                               unsigned int qfmt;
                               unsigned int OutArrID1;
                               unsigned int    n;
                              }IQNcosPU_Params;

/* Function call : I32_IQ _IQNatan2(I32_IQ y, I32_IQ x, U32_IQ qfmt); */
typedef struct IQNatan2_Params{
                              unsigned int y_InArrID1;
                              unsigned int x_InArrID2;
                              unsigned int qfmt;
                              unsigned int OutArrID1;
                              unsigned int   n;
                             }IQNatan2_Params;

/* Function call : I32_IQ _IQNatan2PU(I32_IQ y, I32_IQ x, U32_IQ qfmt);*/
typedef struct IQNatan2PU_Params{
                                 unsigned int y_InArrID1;
                                 unsigned int x_InArrID2;
                                 unsigned int qfmt;
                                 unsigned int OutArrID1;
                                 unsigned int   n;
                                }IQNatan2PU_Params;

/* Function call : I32_IQ _IQNexp(I32_IQ a, U32_IQ qfmt); */
typedef struct IQNexp_Params{
                              unsigned int InArrID1;
                              unsigned int qfmt;
                              unsigned int OutArrID1;
                              unsigned int   n;
                             }IQNexp_Params;

/* Function call : I32_IQ _IQNlog(I32_IQ a, U32_IQ qfmt);*/
typedef struct IQNlog_Params{
                              unsigned int InArrID1;
                              unsigned int qfmt;
                              unsigned int OutArrID1;
                              unsigned int   n;
                             }IQNlog_Params;

/* Function call : I32_IQ _IQNlog(I32_IQ a, U32_IQ qfmt);*/
typedef struct IQNabs_Params{
                              unsigned int InArrID1;
                              unsigned int qfmt;
                              unsigned int OutArrID1;
                              unsigned int   n;
                             }IQNabs_Params;

/* Function call : I32_IQ _IQNpow(I32_IQ x, I32_IQ y, U32_IQ qfmt);*/
typedef struct IQNpow_Params{
                              unsigned int A_InArrID1;
                              unsigned int B_InArrID2;
                              unsigned int qfmt;
                              int OutArrID1;
                              unsigned int   n;
                             }IQNpow_Params;

/* Function call : float _IQNtoF(I32_IQ input, U32_IQ qfmt); */
typedef struct IQNtoF_Params{
                              unsigned int InArrID1;
                              unsigned int qfmt;
                              unsigned int  OutArrID1;
                              unsigned int   n;
                             }IQNtoF_Params;

/* Function call : I32_IQ _FtoIQN(float input, U32_IQ qfmt); */
typedef struct FtoIQN_Params{
                              unsigned int InArrID1;
                              unsigned int qfmt;
                              unsigned int OutArrID1;
                              unsigned int   n;
                             }FtoIQN_Params;

typedef struct IQN_Params{
                              unsigned int InArrID1;
                              unsigned int qfmt;
                              unsigned int OutArrID1;
                              unsigned int   n;
                             }IQN_Params;

typedef struct IQNint_Params{
                              unsigned int InArrID1;
                              unsigned int qfmt;
                              unsigned int OutArrID1;
                              unsigned int   n;
                             }IQNint_Params;

typedef struct IQNfrac_Params{
                              unsigned int InArrID1;
                              unsigned int qfmt;
                              unsigned int OutArrID1;
                              unsigned int   n;
                             }IQNfrac_Params;

typedef struct IQXtoIQY_Params{
                               unsigned int InArrID1;
                               int x;
                               int y;
                               unsigned int OutArrID1;
                               unsigned int   n;
                             }IQXtoIQY_Params;



/* ***********************************************************************************/
/* Parameter structure defintiions for the FastRTS library functions                 */
/* ***********************************************************************************/

/* Function call : float addsp_i(float x, float y) */
typedef struct addsp_i_Params{
                               unsigned int InArrID1;
                               unsigned int InArrID2;
                               unsigned int OutArrID1;
                               unsigned int  n;     
                              }addsp_i_Params;

/* Function call : float subsp_i(float x, float y) */
typedef struct subsp_i_Params{
                               unsigned int InArrID1;
                               unsigned int InArrID2;
                               unsigned int OutArrID1;
                               unsigned int  n;     
                              }subsp_i_Params;

/* Function call : float mpysp_i(float x, float y) */
typedef struct mpysp_i_Params{
                               unsigned int InArrID1;
                               unsigned int InArrID2;
                               unsigned int OutArrID1;
                               unsigned int n;     
                              }mpysp_i_Params;

/* Function call : float recipsp_i(float x)*/
typedef struct divsp_i_Params{
                               unsigned int InArrID1;
                               unsigned int InArrID2;
                               unsigned int OutArrID1;
                               unsigned int n;     
                              }divsp_i_Params;

/* Function call : float recipsp_i(float x)*/
typedef struct recipsp_i_Params{
                               unsigned int x_InArrID1;
                               unsigned int OutArrID1;
                               unsigned int n;     
                              }recipsp_i_Params;

/* Function call : float sqrtsp_i(float x)*/
typedef struct sqrtsp_i_Params{
                               unsigned int x_InArrID1;
                               unsigned int OutArrID1;
                               unsigned int n;     
                              }sqrtsp_i_Params;

/* Function call : Float intsp_i(int x)*/
typedef struct intsp_i_Params{
                               unsigned int x_InArrID1;
                               unsigned int OutArrID1;
                               unsigned int n;     
                              }intsp_i_Params;

/* Function call : int intsp_i(float x) */
typedef struct spint_i_Params{
                               unsigned int x_InArrID1;
                               unsigned int   OutArrID1;
                               unsigned int  n;   
                              }spint_i_Params;

/* Function call : float uintsp_i(unsigned int x)*/
typedef struct uintsp_i_Params{
                               unsigned int x_InArrID1;
                               unsigned int OutArrID1;
                               unsigned int n;     
                              }uintsp_i_Params;

/* Function call : Unsigned int spuint_i(float x)*/
typedef struct spuint_i_Params{
                               unsigned int x_InArrID1;
                               unsigned int OutArrID1;
                               unsigned int n;      
                              }spuint_i_Params;

/* Function call : double atandp(double z) */
typedef struct atandp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }atandp_Params;

/* Function call : float atansp(float z);*/
typedef struct atansp_Params{
                             unsigned int z_InArrID1;
                             unsigned int OutArrID1;
                             unsigned int   n;
                             }atansp_Params;

/* Function call : double atan2dp( double y, double x );*/
typedef struct  atan2dp_Params{
                              unsigned int y_InArrID1;
                              unsigned int x_InArrID2;
                              unsigned int OutArrID1;
                              unsigned int   n;
                             }atan2dp_Params;

/* Function call : float atan2sp( float y, float x );*/
typedef struct atan2sp_Params{
                              unsigned int y_InArrID1;
                              unsigned int x_InArrID2;
                              unsigned int OutArrID1;
                              unsigned int   n;
                             }atan2sp_Params;

/* Function call : double cosdp( double z );*/
typedef struct cosdp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }cosdp_Params;


/* Function call : float cossp( float z );*/
typedef struct cossp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }cossp_Params;


/* Function call : double exp( double z ) ;*/
typedef struct expdp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }expdp_Params; 
/* Function call : float expf( float z ) ;*/
typedef struct expsp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }expsp_Params;

/* Function call : double exp10( double z ) ;*/
typedef struct exp10dp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }exp10dp_Params;

/* Function call : float exp10sp( float z );*/
typedef struct exp10sp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }exp10sp_Params;

/* Function call : double exp2dp( double z );*/
typedef struct exp2dp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }exp2dp_Params;

/* Function call : float exp2sp( float z );*/
typedef struct exp2sp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }exp2sp_Params;

/* Function call : double logdp( double z );*/
typedef struct logdp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }logdp_Params;

/* Function call : float logsp( float z );*/
typedef struct logsp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }logsp_Params;

/* Function call : double log10dp( double z );*/
typedef struct log10dp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }log10dp_Params;

/* Function call : float log10sp( float z );*/
typedef struct log10sp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }log10sp_Params;

/* Function call : double log2dp( double z );*/
typedef struct log2dp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }log2dp_Params;

/* Function call : float log2sp( float z ) ;*/
typedef struct log2sp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }log2sp_Params;
/* Function call : double powdp(double x double y);*/
typedef struct  powdp_Params{
                              unsigned int x_InArrID1;
                              unsigned int y_InArrID2;
                              unsigned int OutArrID1;
                              unsigned int   n;
                             }powdp_Params;
/* Function call : float powsp(float x, float y);*/
typedef struct  powsp_Params{
                              unsigned int x_InArrID1;
                              unsigned int y_InArrID2;
                              unsigned int OutArrID1;
                              unsigned int   n;
                             }powsp_Params;

/* Function call : double recipdp( double z ); */
typedef struct recipdp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }recipdp_Params;

/* Function call : float recipsp( float z );*/
typedef struct recipsp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }recipsp_Params;

/* Function call : double rsqrtdp( double z );*/
typedef struct rsqrtdp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }rsqrtdp_Params;

/* Function call : float rsqrtsp( float z );*/
typedef struct rsqrtsp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }rsqrtsp_Params;

/* Function call : double sindp( double z);*/
typedef struct sindp_Params{
                           unsigned int z_InArrID1;
                           unsigned int OutArrID1;
                           unsigned int   n;
                           }sindp_Params;

/* Function call : float sinsp(float x);*/
typedef struct sinsp_Params{
                            unsigned int x_InArrID1;
                            unsigned int OutArrID1;
                            unsigned int   n;
                            }sinsp_Params;

/* Function call : double divdp( double x, double y );*/
typedef struct divdp_Params{
                             unsigned int x_InArrID1;
                             unsigned int y_InArrID2;
                             unsigned int OutArrID1;
                             unsigned int   n;
                             }divdp_Params;
/* Function call : float divsp( float x, float y );*/
typedef struct divsp_Params{
                             unsigned int x_InArrID1;
                             unsigned int y_InArrID2;
                             unsigned int OutArrID1;
                             unsigned int   n;
                             }divsp_Params;
                                                         
/* Function call : double sqrtdp( double z );*/
typedef struct sqrtdp_Params{
                             unsigned int z_InArrID1;
                             unsigned int OutArrID1;
                             unsigned int   n;
                             }sqrtdp_Params;

/* Function call : float sqrtsp( float z );*/
typedef struct sqrtsp_Params{
                            unsigned int z_InArrID1;
                            unsigned int OutArrID1;
                            unsigned int   n;
                           }sqrtsp_Params;

/* Function call : void d2f( double *d,float *f, int n );*/
typedef struct d2f_Params{
                            unsigned int d_InArrID1;
                            unsigned int f_OutArrID1;
                            int   n;
                           }d2f_Params;

/* Function call : void f2d( double *d,float *f, int n );*/
typedef struct f2d_Params{
                            unsigned int f_InArrID1;
                            unsigned int d_OutArrID1;
                            int   n;
                           }f2d_Params;

/* Function call : void d2s( double *d,short *s, int n );*/
typedef struct d2s_Params{
                            unsigned int d_InArrID1;
                            unsigned int s_OutArrID1;
                            int   n;
                           }d2s_Params;

/* Function call : void s2d( double *d,short *s, int n );*/
typedef struct s2d_Params{
                            unsigned int s_InArrID1;
                            unsigned int d_OutArrID1;
                            int   n;
                           }s2d_Params;

/* Function call : void d2i( double *d,int *i, int n );*/
typedef struct d2i_Params{
                            unsigned int d_InArrID1;
                            unsigned int i_OutArrID1;
                            int   n;
                           }d2i_Params;

/* Function call : void i2d( double *d,int *i, int n );*/
typedef struct i2d_Params{
                            unsigned int i_InArrID1;
                            unsigned int d_OutArrID1;
                            int   n;
                           }i2d_Params;

/*void tw_gen (float *w, int n) */
typedef struct tw_gen_Params{
                            unsigned int tw_OutArrID1;
                            int   n;
                           }tw_gen_Params;

/*void gen_twiddle_fft16x16 (short *w, int n) */
typedef struct gen_twiddle_fft16x16_Params{
                            unsigned int twiddle_OutArrID1;
                            int   n;
                           }gen_twiddle_fft16x16_Params;

/*void gen_twiddle_fft32x32 (short *w, int n) */
typedef struct gen_twiddle_fft32x32_Params{
                            unsigned int twiddle_OutArrID1;
                            int   n;
                            double scale;
                           }gen_twiddle_fft32x32_Params;


/* ***********************************************************************************/
/* Parameter structure defintiions for the VLIB library functions                    */
/* ***********************************************************************************/

typedef struct VLIB_dilate_bin_mask_Params{
					   unsigned int indata_InArrID1;
					   unsigned int outdata_OutArrID1;
					   unsigned int msk_InArrID2;
					   int cols;
					   int pitch;
                                          }VLIB_dilate_bin_mask_Params;

typedef struct VLIB_erode_bin_mask_Params{
					   unsigned int indata_InArrID1;
					   unsigned int outdata_OutArrID1;
					   unsigned int msk_InArrID2;
					   int cols;
					   int pitch;
                                          }VLIB_erode_bin_mask_Params;

typedef struct VLIB_initMeanWithLumaS16_Params{
					   unsigned int runningMean_OutArrID1;
					   unsigned int lumaFrame_InArrID1;
					   unsigned int pixelCount;
	                                   unsigned int output;//physical addr
                                          }VLIB_initMeanWithLumaS16_Params;

typedef struct VLIB_initVarWithConstS16_Params{
					   unsigned int runningVar_OutArrID1;
					   short constVar;
					   unsigned int pixelCount;
	                                   unsigned int output;//physical addr
                                          }VLIB_initVarWithConstS16_Params;

typedef struct VLIB_updateEWRMeanS16_Params{
					   unsigned int runningMean_InOutArrID1;
					   unsigned int newLuma_InArrID1;
                                           unsigned int mask32packed_InArrID2;
					   unsigned int pixelCount;
                                           short weight;
	                                   unsigned int output;//physical addr
                                          }VLIB_updateEWRMeanS16_Params;

typedef struct VLIB_updateEWRVarianceS16_Params{
                                           unsigned int runningVar_InOutArrID1;
					   unsigned int runningMean_InOutArrID2;
					   unsigned int newLuma_InArrID1;
                                           unsigned int mask32packed_InArrID2;
					   unsigned int pixelCount;
                                           short weight;
	                                   unsigned int output;//physical addr
                                          }VLIB_updateEWRVarianceS16_Params;

typedef struct VLIB_initMeanWithLumaS32_Params{
					   unsigned int runningMean_OutArrID1;
					   unsigned int lumaFrame_InArrID1;
					   unsigned int pixelCount;
                                           unsigned int output;//physical addr
					   }VLIB_initMeanWithLumaS32_Params;

typedef struct VLIB_initVarWithConstS32_Params{
					   unsigned int runningVar_OutArrID1;
					   int constVar;
					   unsigned int pixelCount;
	                                   unsigned int output;//physical addr
                                          }VLIB_initVarWithConstS32_Params;

typedef struct VLIB_updateEWRMeanS32_Params{
					   unsigned int runningMean_InOutArrID1;
					   unsigned int newLuma_InArrID1;
                                           unsigned int mask32packed_InArrID2;
					   unsigned int pixelCount;
                                           int weight;
	                                   unsigned int output;//physical addr
                                          }VLIB_updateEWRMeanS32_Params;

typedef struct VLIB_updateEWRVarianceS32_Params{
                                           unsigned int runningVar_InOutArrID1;
					   unsigned int runningMean_InOutArrID2;
					   unsigned int newLuma_InArrID1;
                                           unsigned int mask32packed_InArrID2;
					   unsigned int pixelCount;
                                           short weight;
	                                   unsigned int output;//physical addr
                                          }VLIB_updateEWRVarianceS32_Params;

typedef struct VLIB_updateUWRMeanS16_Params{
					   unsigned int updatedMean_InOutArrID1;
					   unsigned int previousMean_InOutArrID2;
                                           unsigned int newestData_InArrID1;
                                           unsigned int oldestData_InArrID2;
                                           unsigned int newestMask32packed_InArrID3;
                                           unsigned int oldestMask32packed_InArrID4;
					   unsigned int pixelCount;
                                           unsigned char frameCount;
	                                   unsigned int output;//physical addr
                                          }VLIB_updateUWRMeanS16_Params;

typedef struct VLIB_updateUWRVarianceS16_Params{
                                           unsigned int updatedVar_InOutArrID1;
                                           unsigned int updatedMean_InOutArrID2;
					   unsigned int previousMean_InArrID1;
                                           unsigned int previousVar_InArrID2;
                                           unsigned int newestData_InArrID3;
                                           unsigned int newestMask32packed_InArrID4;
                                           unsigned int oldestMask32packed_InArrID5;
					   unsigned int pixelCount;
                                           short frameCount;
	                                   unsigned int output;//physical addr
                                          }VLIB_updateUWRVarianceS16_Params;

typedef struct VLIB_subtractBackgroundS16_Params{
                                           unsigned int mask32packed_OutArrID1;
                                           unsigned int newLuma_InArrID1;
					   unsigned int runningMean_InArrID2;
                                           unsigned int runningVar_InArrID3;
                                           short thresholdGlobal;
                                           short thresholdFactor;
                                           unsigned int PixelCount;
	                                   int output;//physical addr
                                          }VLIB_subtractBackgroundS16_Params;

typedef struct VLIB_subtractBackgroundS32_Params{
                                           unsigned int mask32packed_OutArrID1;
                                           unsigned int newLuma_InArrID1;
					   unsigned int runningMean_InArrID2;
                                           unsigned int runningVar_InArrID3;
                                           int thresholdGlobal;
                                           int thresholdFactor;
                                           unsigned int PixelCount;
	                                   int output;//physical addr
                                          }VLIB_subtractBackgroundS32_Params;

typedef struct VLIB_mixtureOfGaussiansS16_Params{
                                           unsigned int inputIm_InArrID1;
                                           unsigned int currentWts_InOutArrID1;
					   unsigned int currentMeans_InOutArrID2;
                                           unsigned int currentVars_InOutArrID3;
                                           unsigned int compIndex_InOutArrID4;
                                           unsigned int intBuffer_InArrID1;
                                           unsigned int fgmask_OutArrID2;
                                           int imageSize;
                                           short updateRate1;
                                           short updateRate2;
	                                   short mdThreshold;
                                           short bsThreshold;
                                           short initialWt;
                                           short initialVar;
                                           unsigned int output; //physical addr
                                          }VLIB_mixtureOfGaussiansS16_Params;

typedef struct VLIB_mixtureOfGaussiansS32_Params{
                                           unsigned int inputIm_InArrID1;
                                           unsigned int currentWts_InOutArrID1;
					   unsigned int currentMeans_InOutArrID2;
                                           unsigned int currentVars_InOutArrID3;
                                           unsigned int compIndex_InOutArrID4;
                                           unsigned int intBuffer_InArrID1;
                                           unsigned int fgmask_OutArrID2;
                                           int imageSize;
                                           short updateRate1;
                                           int updateRate2;
	                                   int mdThreshold;
                                           short bsThreshold;
                                           short initialWt;
                                           int initialVar;
                                           unsigned int output; //physical addr
                                          }VLIB_mixtureOfGaussiansS32_Params;


typedef struct VLIB_trackFeaturesLucasKanade_7x7_Params{
                                           unsigned int im1_InArrID1;
                                           unsigned int im2_InArrID2;
					   unsigned int gradX_InArrID3;
                                           unsigned int gradY_InArrID4;
                                           unsigned int x_InArrID5;
                                           unsigned int y_InArrID6;
                                           unsigned int outx_InOutArrID1;
                                           unsigned int outy_InOutArrID2;
                                           int width;
                                           int height;
                                           int nfeatures;
                                           int max_iters;
	                                   unsigned int scratch_InOutArrID3;
                                           unsigned int output; //physical addr
                                          }VLIB_trackFeaturesLucasKanade_7x7_Params;

typedef struct VLIB_xyGradientsAndMagnitude_Params{
                                           unsigned int pInBlk_InArrID1;
                                           unsigned int pBufGradX_OutArrID1;
                                           unsigned int pBufGradY_OutArrID2;
                                           int width;
                                           int height;
                                           unsigned int pBufMag_OutArrID3;
                                           unsigned int output; //physical addr
                                          }VLIB_xyGradientsAndMagnitude_Params;

typedef struct VLIB_normalFlow_16_Params{
                                           unsigned int inDiff_InArrID1;
                                           unsigned int Emag_InArrID2;
                                           unsigned int Ex_InArrID3;
                                           unsigned int Ey_InArrID4;
                                           unsigned int LUT_InArrID5;
                                           short T;
                                           int numPixels;
                                           unsigned int normalFlowU_OutArrID1;
                                           unsigned int normalFlowV_OutArrID2; 
                                           
                                          }VLIB_normalFlow_16_Params;

typedef struct VLIB_harrisScore_7x7_Params{
                                           unsigned int gradX_InArrID1;
                                           unsigned int gradY_InArrID2;
                                           int width;
                                           int height;
                                           unsigned int harrisscore_OutArrID1;
                                           short k; 
                                           unsigned int buffer_InOutArrID1;
                                           unsigned int output; //physical addr
                                          }VLIB_harrisScore_7x7_Params;

typedef struct VLIB_houghLineFromList_Params{
                                           unsigned int pEdgeMapList_InArrID1;
                                           unsigned int pOutHoughSpace_OutArrID1;
                                           unsigned short outBlkWidth;
                                           unsigned short outBlkHeight;
                                           unsigned short thetaRange;
                                           unsigned short rhoMaxLength;
                                           unsigned int listSize;
                                           unsigned int pSIN_InArrID2;
                                           unsigned int pCOS_InArrID3;
                                           unsigned int ping_InOutArrID1;
                                           unsigned int pong_InOutArrID2;
                                           unsigned int pang_InOutArrID3;
                                           unsigned int peng_InOutArrID4;
                                           unsigned int output; //physical addr
                                          }VLIB_houghLineFromList_Params;

/* Function call : void VLIB_nonMaximumSuppressionCanny(const short *pInMag,const short *pInGradX,const short *pInGradY,unsigned char *pOut, unsigned short cols, unsigned short pitch, unsigned short rows)*/ 
typedef struct VLIB_nonMaximumSuppressionCanny_Params{ 
                                unsigned int indata_InArrID1;
				unsigned int indata_InArrID2;
				unsigned int indata_InArrID3;
				unsigned int outdata_OutArrID1;
				int Col;
				int Pitch;
				int Row;
                                    }VLIB_nonMaximumSuppressionCanny_Params; 

/* Function call : void VLIB_doublethresholding(const signed short * restrict pInMag, unsigned char *edgeMap, unsigned int * restrict strongEdgeListPtr, int * numStrongEdges, unsigned short width, unsigned short pitch, unsigned short height, unsigned char loThresh, unsigned char hiThresh, unsigned int  block_offset)*/ 
typedef struct VLIB_doublethresholding_Params{ 
                                unsigned int indata_InArrID1;
				unsigned int outdata_OutArrID1;
				unsigned int outdata_OutArrID2;
				unsigned int outdata_OutArrID3;
				int Col;
				int Pitch;
				int Row;
				int loThresh;
				int hiThresh;
				int  block_offset;
                                }VLIB_doublethresholding_Params; 

/* Function call : void VLIB_edgeRelaxation(unsigned char *edgeMap, unsigned int * restrict strongEdgeListPtr, int * numStrongEdges, unsigned short width)*/ 
typedef struct VLIB_edgeRelaxation_Params{ 
               			unsigned int outdata_OutArrID1; 
				unsigned int indata_InArrID1; 
				unsigned int indata_InArrID2; 
				int Col;
                                }VLIB_edgeRelaxation_Params; 
									
/* Function call : void VLIB_packMask32(unsigned char *maskImage,unsigned int *mask32packed, unsigned int pixelCount)*/ 
typedef struct VLIB_packMask32_Params{
				unsigned int indata_InArrID1; 
               			unsigned int outdata_OutArrID1; 
				int pixelCount;
				}VLIB_packMask32_Params;
	 										
/* Function call : void VLIB_unpackMask32(unsigned int *mask32packed, unsigned char *maskImage, unsigned int pixelCount)*/ 
typedef struct VLIB_unpackMask32_Params{
				unsigned int indata_InArrID1; 
                		unsigned int outdata_OutArrID1; 
				int pixelCount;
				}VLIB_unpackMask32_Params;
	 										
typedef struct VLIB_dilate_bin_square_opt_Params{
				unsigned int indata_InArrID1; 
				unsigned int outdata_OutArrID1; 
				unsigned int outdata_OutArrID2; 
				unsigned int outdata_OutArrID3; 
				int width;
				int height;
				}VLIB_dilate_bin_square_opt_Params;
	 											
typedef struct VLIB_dilate_bin_square_Params{
				unsigned int indata_InArrID1; 
                                unsigned int outdata_OutArrID1; 
				int Col;
				int Pitch;
				}VLIB_dilate_bin_square_Params;
	 									
typedef struct VLIB_dilate_bin_cross_Params{
				unsigned int indata_InArrID1; 
                                unsigned int outdata_OutArrID1; 
				int Col;
				int Pitch;
				}VLIB_dilate_bin_cross_Params;
	 									
	
typedef struct VLIB_erode_bin_square_opt_Params{
				unsigned int indata_InArrID1; 
                                unsigned int outdata_OutArrID1; 
                                unsigned int outdata_OutArrID2; 
                                unsigned int outdata_OutArrID3; 
				int width;
				int height;
				}VLIB_erode_bin_square_opt_Params;
	 										
typedef struct VLIB_erode_bin_square_Params{
				unsigned int indata_InArrID1; 
                                unsigned int outdata_OutArrID1; 
				int Col;
				int Pitch;
				}VLIB_erode_bin_square_Params;
	 									
typedef struct VLIB_erode_bin_cross_Params{
				unsigned int indata_InArrID1; 
                                unsigned int outdata_OutArrID1; 
				int Col;
				int Pitch;
				}VLIB_erode_bin_cross_Params;
	 									
	   	 									
typedef struct VLIB_erode_bin_singlePixel_Params{
				unsigned int indata_InArrID1; 
                                unsigned int outdata_OutArrID1; 
				int Col;
				int Pitch;
				}VLIB_erode_bin_singlePixel_Params;
	 									
typedef struct VLIB_cannyEdge_Params{ 

				unsigned int input_InArrID1; 
				unsigned int loThresh; 
				unsigned int hiThresh; 
				unsigned int workBuf1_OutArrID1; 
				unsigned int workBuf2_OutArrID2; 
				unsigned int workBuf3_OutArrID3; 
				unsigned int output_OutArrID4; 
				unsigned int width; 
				unsigned int height;
                                }VLIB_cannyEdge_Params; 

typedef struct VLIB_legendreMoments_Init_Params{
				unsigned int LPoly_OutArrID1; 
				char imH;
				char Order;
                                unsigned int Constant_OutArrID2;
                                //unsigned int output;
				}VLIB_legendreMoments_Init_Params;

typedef struct VLIB_legendreMoments_Params{
				unsigned int LPoly_InArrID2;
                                unsigned int Im_InArrID1; 
                                unsigned int Lmoments_OutArrID1; 
				int imH;
				int Order;
                                short Constant;
                                //unsigned int output;
				}VLIB_legendreMoments_Params;

typedef struct VLIB_imagePyramid8_Params{
				unsigned int pIn_InArrID1; 
                                unsigned int pOut_OutArrID1; 
				unsigned short inCols;
				unsigned short inRows;
                                unsigned int output;
				}VLIB_imagePyramid8_Params;

typedef struct VLIB_imagePyramid16_Params{
				unsigned int pIn_InArrID1; 
                                unsigned int pOut_OutArrID1; 
				unsigned short inCols;
				unsigned short inRows;
                                unsigned int output;
				}VLIB_imagePyramid16_Params;

typedef struct VLIB_gauss5x5PyramidKernel_8_Params{
				unsigned int pIn_InArrID1; 
                                unsigned int pB_InArrID2;
                                unsigned int pOut_OutArrID1; 
				unsigned short inCols;
				unsigned short inRows;
                                unsigned short pitch;
                                unsigned int output;
				}VLIB_gauss5x5PyramidKernel_8_Params;

typedef struct VLIB_gauss5x5PyramidKernel_16_Params{
				unsigned int pIn_InArrID1; 
                                unsigned int pB_InArrID2;
                                unsigned int pOut_OutArrID1; 
				unsigned short inCols;
				unsigned short inRows;
                                unsigned short pitch;
                                unsigned int output;
				}VLIB_gauss5x5PyramidKernel_16_Params;

typedef struct VLIB_gradientH5x5PyramidKernel_8_Params{
				unsigned int pIn_InArrID1; 
                                unsigned int pB_InArrID2;
                                unsigned int pOut_OutArrID1; 
				unsigned short inCols;
				unsigned short inRows;
                                unsigned short pitch;
                                unsigned int output;
				}VLIB_gradientH5x5PyramidKernel_8_Params;

typedef struct VLIB_gradientV5x5PyramidKernel_8_Params{
				unsigned int pIn_InArrID1; 
                                unsigned int pB_InArrID2;
                                unsigned int pOut_OutArrID1; 
				unsigned short inCols;
				unsigned short inRows;
                                unsigned short pitch;
                                unsigned int output;
				}VLIB_gradientV5x5PyramidKernel_8_Params;

typedef struct VLIB_recursiveFilterHoriz1stOrder_Params{
				unsigned int pIn_InArrID1; 
                                unsigned int pOut_OutArrID1; 
				int width;
				int height;
                                short weight;
                                unsigned int buffer_InArrID2;
                                unsigned int output;
				}VLIB_recursiveFilterHoriz1stOrder_Params;

typedef struct VLIB_recursiveFilterHoriz1stOrderS16_Params{
				unsigned int pIn_InArrID1; 
                                unsigned int pOut_OutArrID1; 
				int width;
				int height;
                                short weight;
                                unsigned int buffer_InArrID2;
                                unsigned int output;
				}VLIB_recursiveFilterHoriz1stOrderS16_Params;

typedef struct VLIB_recursiveFilterVert1stOrder_Params{
				unsigned int pIn_InArrID1; 
                                unsigned int pOut_OutArrID1; 
				int width;
				int height;
                                short weight;
                                unsigned int buffer_InArrID2;
                                unsigned int output;
				}VLIB_recursiveFilterVert1stOrder_Params;

typedef struct VLIB_recursiveFilterVert1stOrderS16_Params{
				unsigned int pIn_InArrID1; 
                                unsigned int pOut_OutArrID1; 
				int width;
				int height;
                                short weight;
                                unsigned int buffer_InArrID2;
                                unsigned int output;
				}VLIB_recursiveFilterVert1stOrderS16_Params;

typedef struct VLIB_integralImage8_Params{
				unsigned int pIn_InArrID1; 
                                unsigned int pLastLine_OutArrID2;
                                unsigned int pOut_OutArrID1; 
				unsigned short inCols;
				unsigned short inRows;
                                unsigned int output;
				}VLIB_integralImage8_Params;

typedef struct VLIB_integralImage16_Params{
				unsigned int pIn_InArrID1; 
                                unsigned int pLastLine_OutArrID2;
                                unsigned int pOut_OutArrID1; 
				unsigned short inCols;
				unsigned short inRows;
                                unsigned int output;
				}VLIB_integralImage16_Params;

typedef struct VLIB_histogram_1D_Init_U8_Params{
				unsigned int binEdges_InArrID1; 
                                unsigned int histArray_OutArrID1; 
				int numBins;
                                unsigned int output;
				}VLIB_histogram_1D_Init_U8_Params;

typedef struct VLIB_histogram_1D_U8_Params{
				unsigned int X_InArrID1; 
                                unsigned int histArray_InArrID2; 
                                unsigned int internalH1_InArrID3;
                                unsigned int internalH2_InArrID4;
                                unsigned int internalH3_InArrID5;
                                unsigned int H_OutArrID1;
				int numBins;
                                int numX;
                                unsigned short binWeight;
                                unsigned int output;
				}VLIB_histogram_1D_U8_Params;

typedef struct VLIB_histogram_1D_U16_Params{
				unsigned int X_InArrID1; 
                                unsigned int histArray_InArrID2; 
                                unsigned int internalH1_InArrID3;
                                unsigned int H_OutArrID1;
                                unsigned short binWeight;
				int numBins;
                                int numX;
                                unsigned int output;
				}VLIB_histogram_1D_U16_Params;

typedef struct VLIB_weightedHistogram_1D_U8_Params{
				unsigned int X_InArrID1; 
                                unsigned int binWeight_InArrID2;
                                unsigned int histArray_InArrID3; 
                                unsigned int internalH1_InArrID4;
                                unsigned int internalH2_InArrID5;
                                unsigned int internalH3_InArrID6;
                                unsigned int H_OutArrID1;
				int numBins;
                                int numX;
                                unsigned int output;
				}VLIB_weightedHistogram_1D_U8_Params;

typedef struct VLIB_weightedHistogram_1D_U16_Params{
				unsigned int X_InArrID1; 
                                unsigned int binWeight_InArrID2;
                                unsigned int histArray_InArrID3; 
                                unsigned int internalH1_InArrID4;
                                unsigned int internalH2_InArrID5;
                                unsigned int internalH3_InArrID6;
                                unsigned int H_OutArrID1;
                                int numX;
				int numBins;
                                unsigned int output;
				}VLIB_weightedHistogram_1D_U16_Params;

typedef struct VLIB_histogram_1D_Init_U16_Params{
				unsigned int binEdges_InArrID1; 
                                unsigned int histArray_OutArrID1; 
				int numBins;
                                unsigned int output;
				}VLIB_histogram_1D_Init_U16_Params;


typedef struct VLIB_histogram_nD_U16_Params{
				unsigned int X_InArrID1; 
                                unsigned int numBins_InArrID2; 
                                unsigned int normVals_InArrID3;
                                unsigned int internalH1_InArrID4;
                                unsigned int internalH2_InArrID5;
                                unsigned int H_InOutArrID1;
                                unsigned short binWeight;
                                int dimX;
				int numX;
                                unsigned int output;
				}VLIB_histogram_nD_U16_Params;

typedef struct VLIB_weightedHistogram_nD_U16_Params{
				unsigned int X_InArrID1; 
                                unsigned int binWeight_InArrID2;
                                unsigned int numBins_InArrID3; 
                                unsigned int internalH1_InArrID4;
                                unsigned int internalH2_InArrID5;
                                unsigned int normVals_InArrID6;
                                unsigned int H_InOutArrID1;
				int dimX;
                                int numX;
                                unsigned int output;
				}VLIB_weightedHistogram_nD_U16_Params;

typedef struct VLIB_bhattacharyaDistance_U32_Params{ 
               			unsigned int X_InArrID1; 
				unsigned int Y_InArrID2; 
				unsigned int D_OutArrID1; 
				int N;
                                unsigned int output;
                                }VLIB_bhattacharyaDistance_U32_Params;

typedef struct VLIB_L1DistanceS16_Params{ 
               			unsigned int X_InArrID1; 
				unsigned int Y_InArrID2; 
				unsigned int L1D_OutArrID1; 
				int N;
                                unsigned int output;
                                }VLIB_L1DistanceS16_Params; 

typedef struct VLIB_extractLumaFromUYUV_Params{
				unsigned int inputImage_InArrID1; 
                                unsigned int outputImage_OutArrID1; 
				unsigned short inputWidth;
                                unsigned short inputPitch;
				unsigned short inputHeight;
                                unsigned int output;
				}VLIB_extractLumaFromUYUV_Params;

typedef struct VLIB_convertUYVYint_to_YUVpl_Params{
				unsigned int yc_InArrID1; 
                                unsigned short width;
                                unsigned short pitch;
				unsigned short height;
                                unsigned int y_OutArrID1; 
                                unsigned int cr_OutArrID2; 
                                unsigned int cb_OutArrID3; 
                                unsigned int output;
				}VLIB_convertUYVYint_to_YUVpl_Params;

typedef struct VLIB_convertUYVYint_to_YUV420pl_Params{
				unsigned int yc_InArrID1; 
                                unsigned short width;
                                unsigned short pitch;
				unsigned short height;
                                unsigned int y_OutArrID1; 
                                unsigned int cr_OutArrID2; 
                                unsigned int cb_OutArrID3; 
                                unsigned int output;
				}VLIB_convertUYVYint_to_YUV420pl_Params;

typedef struct VLIB_convertUYVYint_to_HSLpl_Params{
				unsigned int yc_InArrID1; 
                                unsigned short width;
                                unsigned short pitch;
				unsigned short height;
                                unsigned int coeff_InArrID2;
                                unsigned int div_table_InArrID3;
                                unsigned int H_OutArrID1; 
                                unsigned int S_OutArrID2; 
                                unsigned int L_OutArrID3; 
                                unsigned int output;
				}VLIB_convertUYVYint_to_HSLpl_Params;

typedef struct VLIB_convertUYVYint_to_LABpl_Params{
				unsigned int yc_InArrID1; 
                                unsigned short width;
                                unsigned short pitch;
				unsigned short height;
                                unsigned int coeff_InArrID2;
                                unsigned int whitePoint_InArrID3;
                                unsigned int L_OutArrID1; 
                                unsigned int a_OutArrID2; 
                                unsigned int b_OutArrID3; 
                                unsigned int output;
				}VLIB_convertUYVYint_to_LABpl_Params;

typedef struct VLIB_convertUYVYint_to_RGBpl_Params{
				unsigned int yc_InArrID1; 
                                unsigned short width;
                                unsigned short pitch;
				unsigned short height;
                                unsigned int coeff_InArrID2;
                                unsigned int R_OutArrID1; 
                                unsigned int G_OutArrID2; 
                                unsigned int B_OutArrID3; 
                                unsigned int output;
				}VLIB_convertUYVYint_to_RGBpl_Params;

typedef struct VLIB_convertUYVYint_to_LABpl_LUT_Params{
				unsigned int yc_InArrID1; 
                                unsigned short width;
                                unsigned short pitch;
				unsigned short height;
                                int d;
                                unsigned int LabLUT_InArrID2;
                                unsigned int L_OutArrID1; 
                                unsigned int a_OutArrID2; 
                                unsigned int b_OutArrID3; 
                                unsigned int output;
				}VLIB_convertUYVYint_to_LABpl_LUT_Params;

typedef struct VLIB_initUYVYint_to_LABpl_LUT_Params{
				int d;
                                unsigned int coeff_InArrID1;
                                unsigned int whitePoint_InArrID2; 
                                unsigned int Lab_OutArrID1; 
                                unsigned int output;
				}VLIB_initUYVYint_to_LABpl_LUT_Params;

typedef struct VLIB_convertUYVYsemipl_to_YUVpl_Params{
				unsigned int crcb_InArrID1; 
                                unsigned short width;
                                unsigned short pitch;
				unsigned short height;
                                unsigned int cr_OutArrID1; 
                                unsigned int cb_OutArrID2; 
                                unsigned int output;
				}VLIB_convertUYVYsemipl_to_YUVpl_Params;

typedef struct VLIB_convertUYVYpl_to_YUVint_Params{
				unsigned int yc_OutArrID1; 
                                unsigned short width;
                                unsigned short pitch;
				unsigned short height;
                                unsigned int y_InArrID1; 
                                unsigned int cr_InArrID2; 
                                unsigned int cb_InArrID3; 
                                unsigned int output;
				}VLIB_convertUYVYpl_to_YUVint_Params;

typedef struct VLIB_disparity_SAD_firstRow8_Params{
				unsigned int pLeft_InArrID1; 
                                unsigned int pRight_InArrID2;
                                unsigned int pCost_InArrID3; 
                                unsigned int pMinCost_InArrID4;
                                unsigned int pScratch_InArrID5;
                                unsigned int pDisparity_OutArrID1;
                                int displacement;
				int width;
                                int pitch;
                                int windowSize;
                                unsigned int output;
				}VLIB_disparity_SAD_firstRow8_Params;

typedef struct VLIB_disparity_SAD8_Params{
				unsigned int pLeft_InArrID1; 
                                unsigned int pRight_InArrID2;
                                unsigned int pCost_InArrID3; 
                                unsigned int pMinCost_InArrID4;
                                unsigned int pScratch_InArrID5;
                                unsigned int pDisparity_OutArrID1;
                                int displacement;
				int width;
                                int pitch;
                                int windowSize;
                                unsigned int output;
				}VLIB_disparity_SAD8_Params;

typedef struct VLIB_disparity_SAD_firstRow16_Params{
				unsigned int pLeft_InArrID1; 
                                unsigned int pRight_InArrID2;
                                unsigned int pCost_InArrID3; 
                                unsigned int pMinCost_InArrID4;
                                unsigned int pScratch_InArrID5;
                                unsigned int pDisparity_OutArrID1;
                                int displacement;
				int width;
                                int pitch;
                                int windowSize;
                                unsigned int output;
				}VLIB_disparity_SAD_firstRow16_Params;

typedef struct VLIB_disparity_SAD16_Params{
				unsigned int pLeft_InArrID1; 
                                unsigned int pRight_InArrID2;
                                unsigned int pCost_InArrID3; 
                                unsigned int pMinCost_InArrID4;
                                
                                unsigned int pDisparity_OutArrID1;
                                int displacement;
				int width;
                                int pitch;
                                int windowSize;
                                unsigned int output;
				}VLIB_disparity_SAD16_Params;

typedef struct VLIB_simplex_Params{
                            unsigned int func_InArrID1; 
                            unsigned int start_InArrID2;
                            unsigned int init_step_InArrID3; 
                            unsigned int v_InArrID4;
                            unsigned int f_InArrID5;
                            unsigned int vr_InArrID6;
                            unsigned int ve_InArrID7;
                            unsigned int vc_InArrID8;
                            unsigned int vm_InArrID9;
                            unsigned int addtlArgs_InArrID10;
                            unsigned int minPoint_OutArrID1;
                            int minValue;
                            int N;
                            short N_INV;
                            int MaxIteration;
                            int EPSILON;
                            }VLIB_simplex_Params;

typedef struct VLIB_simplex_3D_Params{
                            unsigned int func_InArrID1; 
                            unsigned int start_InArrID2;
                            unsigned int init_step_InArrID3; 
                            unsigned int v_InArrID4;
                            unsigned int f_InArrID5;
                            unsigned int vr_InArrID6;
                            unsigned int ve_InArrID7;
                            unsigned int vc_InArrID8;
                            unsigned int vm_InArrID9;
                            unsigned int addtlArgs_InArrID10;
                            unsigned int minPoint_OutArrID1;
                            int minValue;
                            int MaxIteration;
                            int EPSILON;
                            }VLIB_simplex_3D_Params;

typedef struct VLIB_kalmanFilter_4x6_Predict_Params{
                                                    unsigned int KF_InOutArrID1;
                                                   }VLIB_kalmanFilter_4x6_Predict_Params;

typedef struct VLIB_kalmanFilter_4x6_Correct_Params{
                                                    unsigned int KF_InOutArrID1;
                                                    unsigned int Z_InArrID1;
                                                    unsigned int residual_OutArrID1;
                                                    
                                                   }VLIB_kalmanFilter_4x6_Correct_Params;

typedef struct VLIB_kalmanFilter_2x4_Predict_Params{
                                                    unsigned int KF_InOutArrID1;
                                                   }VLIB_kalmanFilter_2x4_Predict_Params;

typedef struct VLIB_kalmanFilter_2x4_Correct_Params{
                                                    unsigned int KF_InOutArrID1;
                                                    unsigned int Z_InArrID1;
                                                    unsigned int residual_OutArrID1;
                                                    
                                                   }VLIB_kalmanFilter_2x4_Correct_Params;


typedef struct argsStruct{
	int n;
	int coeff;
} argsStruct;



/******************************************************************************
 *  ======== C6ACCEL_TI_IC6ACCEL ========
 *  Our implementation of the IUNIVERSAL interface
 *****************************************************************************/
extern IUNIVERSAL_Fxns C6ACCEL_TI_IC6ACCEL;
extern IALG_Fxns C6ACCEL_TI_IALG;

#ifdef __cplusplus
}
#endif

#endif
/*
 *  @(#) ti.C6ACCEL; 1, 0, 0,10; 5-7-2010 17:59:00; 
 */


