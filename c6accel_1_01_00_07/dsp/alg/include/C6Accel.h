/*******************************************************************************
**+--------------------------------------------------------------------------+**
 *                                                                            **
 * Copyright (c) 2010, Texas Instruments Incorporated                         **
 * All rights reserved.                                                       **
 *                                                                            **
 *           ======== C6ACCEL.h ========                                      **
 *           Universal "C6ACCEL" algorithm header file.                       **
 *                                                                            **
 *  This file contains includes for DSP library and definitions of functions  **
 *  in C6Accel specific to the xdais algorithm                                **
*******************************************************************************/
/* Standard library files */
#include<math.h>

/*Include files for the DSP SW Libraries*/
#include "dsplib64plus.h"
#include "imglib64plus.h"
#include "C64PLIBPLUS.h" 
#include "VLIB_prototypes.h"
#include "VLIB_testUtils.h"

/* Defining INLINE_C to activate inlining for fastrts functions */
#define INLINE_C
#include "fastrts_i.h"
#include "IQmath.h"

/* Macro includes floating point kernels */
#ifdef DEVICE_FLOAT
#include "fastrts67x.h"
#include "dsplib674x.h"
#endif

/* Codec Interface file */
#include "iC6Accel_ti.h"

/*Function definition that invoke kernels based on function IDs*/
int C6ACCEL_TI_dsplibFunctionCall(void *pFnArray,int fxnID,XDM1_BufDesc *inBufs, XDM1_BufDesc *outBufs);
int C6ACCEL_TI_imglibFunctionCall(void *pFnArray,int fxnID,XDM1_BufDesc *inBufs, XDM1_BufDesc *outBufs);
int C6ACCEL_TI_mathlibFunctionCall(void *pFnArray,int fxnID,XDM1_BufDesc *inBufs, XDM1_BufDesc *outBufs);
int C6ACCEL_TI_vlibFunctionCall(void *pFnArray,int fxnID,XDM1_BufDesc *inBufs, XDM1_BufDesc *outBufs, XDM1_BufDesc *inOutBufs);
/***********************************************************************************/
/* Format of Function ID                                                           */
/*  32-24 bits indicate the Vendor ID                                              */
/*        Default Vendor: TI Vendor code:01                                        */
/*                        Other vendor code: 02                                    */
/*  23-16 bits indicate the type of function (library that it is associated with)  */
/*        Function type codes                                                      */
/*        01 : DSP Library                                                         */
/*        02 : IMG Library                                                         */
/*        03 : MATH Library                                                        */
/*        04 : ANALYTICS Library                                                   */
/*        05 : MEDICAL Library                                                     */
/*        06 : POWER CONTROL Library                                               */
/*        07 : AUDIO SPEECH Library                                                */
/*   15-0: bits indicate function specific ID within the library                   */
/*                                                                                 */
/*                                                                                 */
/***********************************************************************************/
/*Vendor IDs*/
#define TI                                  0x00000001

/*Exmaple provided to show how a new vendor ID can be added*/
#define OTHER                               0x00000002

#define VENDOR_TAG_SHIFT                    0x00000018
#define VENDOR_MASK                         0xFF000000

/*Library IDs*/
#define DSPLIB_FXN_ID                       0x00000001
#define IMGLIB_FXN_ID                       0x00000002
#define MATHLIB_FXN_ID                      0x00000003
#define ANALYTICSLIB_FXN_ID                 0x00000004
#define MEDICALLIB_FXN_ID                   0x00000005
#define PWRCNTRLLIB_FXN_ID                  0x00000006
#define AUDSPECHLIB_FXN_ID                  0x00000007

/* Function type describes the nature of function */
#define FXNTYP_TAG_SHIFT                    0x00000010
#define FXN_TYP_MASK                        0x00FF0000

/*Function specific ID */
#define FXN_ID_MASK                         0x0000FFFF

//DSPLIB fxn IDs
#define FFT16x16_FXN_ID                     0x00000000
#define IFFT16x16_FXN_ID                    0x00000001
#define MAT_MUL_FXN_ID                      0x00000002
#define FIR_GEN_FXN_ID                      0x00000003
#define AUTOCOR_FXN_ID                      0x00000004
#define FIRLMS2_FXN_ID                      0x00000005
#define FFT16x16_IMRE_FXN_ID                0x00000006
#define FFT16x16R_FXN_ID                    0x00000007
#define FFT16x32_FXN_ID                     0x00000008
#define FFT32x32_FXN_ID                     0x00000009
#define FFT32x32S_FXN_ID                    0x0000000A
#define IFFT16x16_IMRE_FXN_ID               0x0000000B
#define IFFT16x32_FXN_ID                    0x0000000C
#define IFFT32x32_FXN_ID                    0x0000000D
#define FIR_CPLX_FXN_ID                     0x0000000E
#define FIR_CPLX_HM4X4_FXN_ID               0x0000000F
#define FIR_GEN_HM17_RA8X8_FXN_ID           0x00000010
#define FIR_R4_FXN_ID                       0x00000011
#define FIR_R8_FXN_ID                       0x00000012
#define FIR_R8_HM16_RM8A8X8_FXN_ID          0x00000013
#define FIR_SYM_FXN_ID                      0x00000014
#define IIR_FXN_ID                          0x00000015
#define IIR_LAT_FXN_ID                      0x00000016
#define DOTP_SQR_FXN_ID                     0x00000017
#define DOTPROD_FXN_ID                      0x00000018
#define MAXVAL_FXN_ID                       0x00000019
#define MAXIDX_FXN_ID                       0x0000001A
#define MINVAL_FXN_ID                       0x0000001B
#define MUL32_FXN_ID                        0x0000001C
#define NEG32_FXN_ID                        0x0000001D
#define RECIP16_FXN_ID                      0x0000001E
#define VECSUMSQ_FXN_ID                     0x0000001F
#define W_VEC_FXN_ID                        0x00000020
#define MAT_TRANS_FXN_ID                    0x00000021
#define BEXP_FXN_ID                         0x00000022
#define BLK_ESWAP16_FXN_ID                  0x00000023
#define BLK_ESWAP32_FXN_ID                  0x00000024
#define BLK_ESWAP64_FXN_ID                  0x00000025
#define BLK_MOVE_FXN_ID                     0x00000026
#define FLTOQ15_FXN_ID                      0x00000027
#define MINERROR_FXN_ID                     0x00000028
#define Q15tOFL_FXN_ID                      0x00000029
#define TW_GEN_FXN_ID                       0x00000030
#define GEN_TWIDDLE_FFT16x16_FXN_ID         0x00000031
#define GEN_TWIDDLE_FFT32x32_FXN_ID         0x00000032

/* Floating point DSPLIB kernels */
#define F_SP_FFTSPXSP_FXN_ID                0x00000400
#define F_SP_IFFTSPXSP_FXN_ID               0x00000401
#define F_SP_DOTPROD_FXN_ID                 0x00000402
#define F_SP_AUTOCOR_FXN_ID                 0x00000403
#define F_SP_FIR_GEN_FXN_ID                 0x00000404
#define F_SP_FIR_CPLX_FXN_ID                0x00000405
#define F_SP_MAT_MUL_FXN_ID                 0x00000406
#define F_SP_MAT_TRANS_FXN_ID               0x00000407
#define F_SP_IIR_FXN_ID                     0x00000408
#define F_SP_CONV_FXN_ID                    0x00000409
#define F_SP_VECMUL_FXN_ID                  0x0000040A
#define F_SP_VECRECIP_FXN_ID                0x0000040B
#define F_SP_VECSUM_SQ_FXN_ID               0x0000040C
#define F_SP_W_VEC_FXN_ID                   0x0000040D
#define F_SP_BIQUAD_FXN_ID                  0x0000040E
#define F_SP_MAT_MUL_CPLX_FXN_ID            0x0000040F
#define F_COMPLXTOREALNIMG_FXN_ID           0x00000410

/*  IMGLIB fxn IDs  */
#define BOUNDARY_8_FXN_ID                   0x00000000
#define BOUNDARY_16S_FXN_ID                 0x00000001
#define CLIPPING_16S_FXN_ID                 0x00000002
#define YC_DEMUX_BE16_16_FXN_ID             0x00000003
#define YC_DEMUX_LE16_16_FXN_ID             0x00000004
#define DILATE_BIN_FXN_ID                   0x00000005
#define ERODE_BIN_FXN_ID                    0x00000006
#define ERRDIF_BIN_8_FXN_ID                 0x00000007
#define ERRDIF_BIN_16_FXN_ID                0x00000008
#define HISTOGRAM_8_FXN_ID                  0x00000009
#define HISTOGRAM_16_FXN_ID                 0x0000000A
#define MEDIAN_3X3_8_FXN_ID                 0x0000000B
#define PERIMETER_8_FXN_ID                  0x0000000C
#define PERIMETER_16_FXN_ID                 0x0000000D
#define PIX_EXPAND_FXN_ID                   0x0000000E
#define PIX_SAT_FXN_ID                      0x0000000F
#define SOBEL_3X3_8_FXN_ID                  0x00000010
#define SOBEL_3X3_16S_FXN_ID                0x00000011
#define SOBEL_3X3_16_FXN_ID                 0x00000012
#define SOBEL_5X5_16S_FXN_ID                0x00000013
#define SOBEL_7X7_16S_FXN_ID                0x00000014
#define THR_GT2MAX_8_FXN_ID                 0x00000015
#define THR_GT2MAX_16_FXN_ID                0x00000016
#define THR_GT2THR_8_FXN_ID                 0x00000017
#define THR_GT2THR_16_FXN_ID                0x00000018
#define THR_LE2MIN_8_FXN_ID                 0x00000019
#define THR_LE2MIN_16_FXN_ID                0x0000001A
#define THR_LE2THR_8_FXN_ID                 0x0000001B
#define THR_LE2THR_16_FXN_ID                0x0000001C
#define YCBCR422PL_RGB565_FXN_ID            0x0000001D
#define CONV_3X3_I8_C8S_FXN_ID              0x0000001E
#define CONV_3X3_I16S_C16S_FXN_ID           0x0000001F
#define CONV_3X3_I16_C16S_FXN_ID            0x00000020
#define CONV_5X5_I8_C8S_FXN_ID              0x00000021
#define CONV_5X5_I16S_C16S_FXN_ID           0x00000022
#define CONV_5X5_I8_C16S_FXN_ID             0x00000023
#define CONV_7X7_I8_C8S_FXN_ID              0x00000024
#define CONV_7X7_I16S_C16S_FXN_ID           0x00000025
#define CONV_7X7_I8_C16S_FXN_ID             0x00000026
#define CONV_11X11_I8_C8S_FXN_ID            0x00000027
#define CONV_11X11_I16S_C16S_FXN_ID         0x00000028
#define CORR_3X3_I8_C16S_FXN_ID             0x00000029
#define CORR_3X3_I16S_C16S_FXN_ID           0x0000002A
#define CORR_3X3_I8_C8_FXN_ID               0x0000002B
#define CORR_3X3_I16_C16S_FXN_ID            0x0000002C
#define CORR_5X5_I16S_C16S_FXN_ID           0x0000002D
#define CORR_11X11_I16S_C16S_FXN_ID         0x0000002E
#define CORR_11X11_I8_C16S_FXN_ID           0x0000002F
#define CORR_GEN_I16S_C16S_FXN_ID           0x00000030
#define CORR_GEN_IQ_FXN_ID_FXN_ID           0x00000031
#define MEDIAN_3X3_16S_FXN_ID               0x00000032
#define MEDIAN_3X3_16_FXN_ID                0x00000033
#define YC_DEMUX_BE16_8_FXN_ID              0x00000034
#define YC_DEMUX_LE8_C_FXN_ID               0x00000035
#define FDCT_8X8_FXN_ID                     0x00000036
#define IDCT_8X8_12Q4_FXN_ID                0x00000037
#define MAD_8X8_FXN_ID                      0x00000038
#define MAD_16X16_FXN_ID                    0x00000039
#define MPEG2_VLD_INTRA_FXN_ID              0x0000003A
#define MPEG2_VLD_INTER_FXN_ID              0x0000003B
#define QUANTIZE_FXN_ID                     0x0000003C
#define SAD_8X8_FXN_ID                      0x0000003D
#define SAD_16X16_FXN_ID                    0x0000003E
#define WAVE_HORZ_FXN_ID                    0x0000003F
#define WAVE_VERT_FXN_ID                    0x00000040


/*Low level IMGLIB functions*/
#define MULS_16S_FXN_ID                     0x00000041
#define MULS_8_FXN_ID                       0x00000042
#define ADDS_16S_FXN_ID                     0x00000043
#define ADDS_8_FXN_ID                       0x00000044
#define SUBS_16S_FXN_ID                     0x00000045
#define SUBS_8_FXN_ID                       0x00000046
#define NOT_16_FXN_ID                       0x00000047
#define NOT_8_FXN_ID                        0x00000048
#define ANDS_16_FXN_ID                      0x00000049
#define ANDS_8_FXN_ID                       0x0000004A
#define ORS_16_FXN_ID                       0x0000004B
#define ORS_8_FXN_ID                        0x0000004C
#define AND_16_FXN_ID                       0x0000004D
#define AND_8_FXN_ID                        0x0000004E
#define OR_16_FXN_ID                        0x0000004F
#define OR_8_FXN_ID                         0x00000050
#define MUL_16S_FXN_ID                      0x00000051
#define MUL_8_FXN_ID                        0x00000052
#define ADD_16S_FXN_ID                      0x00000053
#define ADD_8_FXN_ID                        0x00000054
#define SUB_16S_FXN_ID                      0x00000055
#define SUB_8_FXN_ID                        0x00000056

/*Additional Image Lib kernels*/
#define YUV420PL_TO_RGB565_FXN_ID           0x00000057
#define YCBCR422PL16_RGB565_FXN_ID          0x00000058
#define YUV420PL16_TO_RGB565_FXN_ID         0x00000059

#define CONV_7X7_I8_C8S_FRAME_FXN_ID        0x00000100
#define CONV_11X11_I8_C8S_FRAME_FXN_ID      0x00000101


/* Optimized versions of kernels in standard TI libraries */
#define MEDIAN_3X3_8_FRAME_FXN_ID           0x0000005A
#define MEDIAN_3X3_16S_FRAME_FXN_ID         0x0000005B
#define MEDIAN_3X3_16_FRAME_FXN_ID          0x0000005C
#define CONV_3X3_I8_C8S_FRAME_FXN_ID        0x0000005D
#define CONV_3X3_I16S_C16S_FRAME_FXN_ID     0x0000005E
#define CORR_3X3_I8_C16S_FRAME_FXN_ID       0x0000005F
#define CORR_3X3_I16S_C16S_FRAME_FXN_ID     0x00000060
#define CORR_3X3_I8_C8_FRAME_FXN_ID         0x00000061
#define YCBCR422SP_TO_YCBCR420PL_FXN_ID     0x00000062
#define YCBCR422SP_TO_YCBCR422ILE_FXN_ID    0x00000063
#define YCBCR422PL_TO_YCBCR422SP_FXN_ID     0x00000064
#define SOBEL_3X3_8_16_FXN_ID               0x00000065
#define RGB_Y_FXN_ID                        0x00000066
#define YC_DEMUX_BE8_FXN_ID		    0x00000067
#define YCBCR420PL_TO_YCBCR422SP_FXN_ID     0x00000068

/**********************************************************
*MATHLIB kernels for C64X devices
  * IQMATH on C64x+
  * FASTRTS on C64x devices
**********************************************************/
/*  IQMath fxn IDs  */
#define IQNMPY_FXN_ID                       0x00000000
#define IQNSQRT_FXN_ID                      0x00000001
#define IQNSIN_FXN_ID                       0x00000002
#define IQNCOS_FXN_ID                       0x00000003
#define IQNDIV_FXN_ID                       0x00000004
#define IQNATAN2_FXN_ID                     0x00000005
#define IQNEXP_FXN_ID                       0x00000006
#define IQNLOG_FXN_ID                       0x00000007
#define IQNRMPY_FXN_ID                      0x00000008
#define IQNRSMPY_FXN_ID                     0x00000009
#define IQNASIN_FXN_ID                      0x0000000A
#define IQNACOS_FXN_ID                      0x0000000B
#define IQNSINPU_FXN_ID                     0x0000000C
#define IQNCOSPU_FXN_ID                     0x0000000D
#define IQNATAN2PU_FXN_ID                   0x0000000E
#define IQNMPYIQX_FXN_ID                    0x0000000F
#define IQNMPYI32INT_FXN_ID                 0x00000010
#define IQNMPYI32FRAC_FXN_ID                0x00000011
#define IQNABS_FXN_ID                       0x00000012
#define IQNTOF_FXN_ID                       0x00000013
#define IQTOIQN_FXN_ID                      0x00000014
#define IQNPOW_FXN_ID                       0x00000015
#define IQNISQRT_FXN_ID                     0x00000016
#define IQNMAG_FXN_ID                       0x00000017
/*  FASTRTS fxn IDs   */
#define ADDSP_FXN_ID                        0x00000018
#define SUBSP_FXN_ID                        0x00000019
#define MPYSP_FXN_ID                        0x0000001A
#define DIVSP_FXN_ID                        0x0000001B
#define RECIPSP_FXN_ID                      0x0000001C
#define INTSP_FXN_ID                        0x0000001D
#define SPINT_FXN_ID                        0x0000001E
#define SPUINT_FXN_ID                       0x0000001F
#define SQRTSP_FXN_ID                       0x00000020
#define UINTSP_FXN_ID                       0x00000021


/*More format conversion kernels from IQmath */
#define FTOIQN_FXN_ID                       0x00000022
#define IQN_FXN_ID                          0x00000023
#define IQXTOIQY_FXN_ID                     0x00000024
#define IQNINT_FXN_ID                       0x00000025
#define IQNFRAC_FXN_ID                      0x00000026

/* Fast RTS function ids for floating point library */
#define ATANDP_FXN_ID                       0x00000200
#define ATANSP_FXN_ID                       0x00000201
#define ATAN2DP_FXN_ID                      0x00000202
#define ATAN2SP_FXN_ID                      0x00000203
#define COSDP_FXN_ID                        0x00000204
#define COSSP_FXN_ID                        0x00000205
#define DIVDP_FXN_ID                        0x00000206
#define EXPDP_FXN_ID                        0x00000207
#define EXPSP_FXN_ID                        0x00000208
#define EXP2DP_FXN_ID                       0x00000209
#define EXP2SP_FXN_ID                       0x0000020A
#define EXP10DP_FXN_ID                      0x0000020B
#define EXP10SP_FXN_ID                      0x0000020C
#define LOGDP_FXN_ID                        0x0000020D
#define LOGSP_FXN_ID                        0x0000020E
#define LOG2DP_FXN_ID                       0x0000021F
#define LOG2SP_FXN_ID                       0x00000220
#define LOG10DP_FXN_ID                      0x00000211
#define LOG10SP_FXN_ID                      0x00000212
#define POWDP_FXN_ID                        0x00000213
#define POWSP_FXN_ID                        0x00000214
#define RECIPF_FXN_ID                       0x00000215
#define RECIPDP_FXN_ID                      0x00000216
#define RSQRTDP_FXN_ID                      0x00000217
#define RSQRTSP_FXN_ID                      0x00000218
#define SINDP_FXN_ID                        0x00000219
#define SINSP_FXN_ID                        0x0000021A
#define SQRTDP_FXN_ID                       0x0000021B
#define SQRT_FXN_ID                         0x0000021C 
#define D2S_FXN_ID                          0x0103021D
#define S2D_FXN_ID                          0x0103021E
#define D2I_FXN_ID                          0x0103021F
#define I2D_FXN_ID                          0x01030220
#define F2D_FXN_ID                          0x01030221
#define D2F_FXN_ID                          0x01030222


/**********************************************************

*VLIB kernels for C64X devices

**********************************************************/
#define DILATE_BIN_MASK_FXN_ID                 0x00000001
#define ERODE_BIN_MASK_FXN_ID                  0x00000002
#define INITMEANWITHLUMAS16_FXN_ID             0x00000003
#define INITMEANWITHLUMAS32_FXN_ID             0x00000004
#define INITVARWITHCONSTS16_FXN_ID             0x00000005
#define INITVARWITHCONSTS32_FXN_ID             0x00000006
#define UPDATEEWRMEANS16_FXN_ID                0x00000007
#define UPDATEEWRMEANS32_FXN_ID                0x00000008
#define UPDATEEWRVARIANCES16_FXN_ID            0x00000009   
#define UPDATEEWRVARIANCES32_FXN_ID            0x0000000A
#define UPDATEUWRMEANS16_FXN_ID                0x0000000B
#define UPDATEUWRVARIANCES16_FXN_ID            0x0000000C

#define SUBTRACTBACKGROUNDS16_FXN_ID           0x0000000D
#define SUBTRACTBACKGROUNDS32_FXN_ID           0x0000000E
#define MIXTUREOFGAUSSIANSS16_FXN_ID           0x0000000F
#define MIXTUREOFGAUSSIANSS32_FXN_ID           0x00000010

#define IMAGEPYRAMID8_FXN_ID                   0x00000011
#define IMAGEPYRAMID16_FXN_ID                  0x00000012
#define INTEGRALIMAGE8_FXN_ID                  0x00000018
#define INTEGRALIMAGE16_FXN_ID                 0x0000001A
#define HOUGHFORLINES_FXN_ID                   0x0000001B
#define HARRISSCORE_7x7_FXN_ID                 0x0000001C
#define NONMAXSUPPRESS_3x3_16S_FXN_ID          0x0000001D
#define NONMAXSUPPRESS_5x5_16S_FXN_ID          0x0000001E
#define NONMAXSUPPRESS_7x7_16S_FXN_ID          0x0000001F
#define TRACKFEATURESLUCASKANADE_7x7_FXN_ID    0x00000020

#define EXTRACTLUMAFROMUYUV_FXN_ID             0x00000021
#define CONVERTUYVYINT_TO_YUVPL_FXN_ID         0x00000022

#define CONVERTUYVYINT_TO_YUV420PL_FXN_ID      0x00000023
#define CONVERTUYVYINT_TO_HSLPL_FXN_ID         0x00000024
#define CONVERTUYVYINT_TO_LABPL_FXN_ID         0x00000025
#define CONVERTUYVYINT_TO_RGBPL_FXN_ID         0x00000026
#define CONVERTUYVYINT_TO_LABPL_LUT_FXN_ID     0x00000027
#define CONVERTUYVYSEMIPL_TO_YUVPL_FXN_ID      0x00000028
#define  INITUYVYINT_TO_LABPL_LUT_FXN_ID       0x00000029
#define  NORMALFLOW_16_FXN_ID                  0x0000002A
#define  CONVERTUYVYPL_TO_YUVINT_FXN_ID        0x0000002B

#define XYGRADIENTSANDMAGNITUDE_FXN_ID         0x00000100
#define NONMAXIMUMSUPPRESSIONCANNY_FXN_ID      0x00000101
#define DOUBLETHRESHOLDING_FXN_ID              0x00000102
#define EDGERELAXATION_FXN_ID                  0x00000103

#define CANNYEDGE_FXN_ID                       0x00001000

#define DILATE_BIN_SQUARE_FXN_ID               0x00000104
#define DILATE_BIN_CROSS_FXN_ID                0x00000105
#define ERODE_BIN_SQUARE_FXN_ID                0x00000107
#define ERODE_BIN_CROSS_FXN_ID                 0x00000108
#define ERODE_BIN_SINGLEPIXEL_FXN_ID           0x00000110
#define PACKMASK32_FXN_ID                      0x00000111
#define UNPACKMASK32_FXN_ID                    0x00000112
#define HOUGHLINEFROMLIST_FXN_ID               0x00000113

#define DILATE_BIN_SQUARE_OPT_FXN_ID           0x00000201
#define ERODE_BIN_SQUARE_OPT_FXN_ID            0x00000202

#define  GAUSS5X5PYRAMIDKERNEL_8_FXN_ID         0x0000002C
#define  GAUSS5X5PYRAMIDKERNEL_16_FXN_ID        0x0000002D
#define  GRADIENTH5X5PYRAMIDKERNEL_8_FXN_ID     0x0000002E
#define  GRADIENTV5X5PYRAMIDKERNEL_8_FXN_ID     0x0000002F
#define  RECURSIVEFILTERHORIZ1STORDER_FXN_ID    0x00000030
#define  RECURSIVEFILTERHORIZ1STORDERS16_FXN_ID 0x00000031
#define  RECURSIVEFILTERVERT1STORDER_FXN_ID     0x00000032
#define  RECURSIVEFILTERVERT1STORDERS16_FXN_ID  0x00000033
#define  LEGENDREMOMENTS_FXN_ID                 0x00000034
#define  LEGENDREMOMENTS_INIT_FXN_ID            0x00000035
#define  HISTOGRAM_1D_INIT_U8_FXN_ID            0x00000036
#define  HISTOGRAM_1D_U8_FXN_ID                 0x00000037
#define  WEIGHTEDHISTOGRAM_1D_U8_FXN_ID         0x00000038
#define  HISTOGRAM_1D_INIT_U16_FXN_ID           0x00000039
#define  HISTOGRAM_1D_U16_FXN_ID                0x0000003A
#define  WEIGHTEDHISTOGRAM_1D_U16_FXN_ID        0x0000003B
#define  HISTOGRAM_ND_U16_FXN_ID                0x0000003C
#define  WEIGHTEDHISTOGRAM_ND_U16_FXN_ID        0x0000003D
#define  BHATTACHARYADISTANCE_U32_FXN_ID        0x0000003E
#define  L1DISTANCES16_FXN_ID                   0x0000003F
#define  DISPARITY_SAD_FIRSTROW16_FXN_ID        0x00000040
#define  DISPARITY_SAD16_FXN_ID                 0x00000041
#define  DISPARITY_SAD_FIRSTROW8_FXN_ID         0x00000042
#define  DISPARITY_SAD8_FXN_ID                  0x00000043
#define  SIMPLEX_FXN_ID                         0x00000044
#define  SIMPLEX_3D_FXN_ID                      0x00000045
#define KALMANFILTER_4X6_PREDICT_FXN_ID         0x00000046
#define KALMANFILTER_4X6_CORRECT_FXN_ID         0x00000047
#define KALMANFILTER_2X4_PREDICT_FXN_ID         0x00000048
#define KALMANFILTER_2X4_CORRECT_FXN_ID         0x00000049

#define CALCCONNECTEDCOMPONENTSMAXBUFFERSIZE_FXN_ID   0x0000004A
#define INITCONNECTEDCOMPONENTSLIST_FXN_ID            0x0000004B
#define CREATECONNECTEDCOMPONENTSLIST_FXN_ID          0x0000004C
#define GETNUMCCS_FXN_ID                              0x0000004D
#define GETCCFEATURES_FXN_ID                          0x0000004E
#define CREATECCMAP8BIT_FXN_ID                        0x0000004F


/****************************************************************************
**                        END OF FILE                                      ** 
*****************************************************************************/















