/*==================================================================== */
/*  Copyright (c) 2010, Texas Instruments Incorporated                 */
/*  All rights reserved.                                               */
/*                                                                     */
/*                                                                     */
/* ======== C6ACCEL_TI_dsplibFunctionCall.c ========                   */
/* This file is used to invoke DSPLib kernels based on the function ID */
/*   passed by the application to C6Accel xdais algorithm              */
/*                                                                     */
/*                                                                     */
/*  Version: 0.0.1                                                     */
/*==================================================================== */

/* Include Files */
#include <xdc/std.h>
#include <string.h>
#include <ti/sdo/ce/osal/Memory.h>
#include <ti/xdais/dm/iuniversal.h>

/* Header Intrinsic to xdais algorithm*/
#include "C6Accel.h"

/* Application-Algorithm Interface file includes */
#include "iC6Accel_ti.h"
#include "iC6Accel_ti_priv.h"

/* Global function to determine if number is power of two*/
static unsigned short IsPowOfTwo( unsigned long number);


int C6ACCEL_TI_dsplibFunctionCall(void *pFnArray,int fxnID,XDM1_BufDesc *inBufs, XDM1_BufDesc *outBufs)
{
 int j;
 Int32 fxnidMsk= FXN_ID_MASK;

         switch ( fxnID & fxnidMsk){
                 case (FFT16x16_FXN_ID):{
                   /* Unmarshal Parameters */
                   DSP_fft16x16_Params *C6ACCEL_TI_DSP_fft16x16_paramPtr;
                   C6ACCEL_TI_DSP_fft16x16_paramPtr= pFnArray;
                   /* Parameter check */
                   if (((C6ACCEL_TI_DSP_fft16x16_paramPtr->w_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_DSP_fft16x16_paramPtr->x_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_DSP_fft16x16_paramPtr->y_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSP_fft16x16_paramPtr->nx)<16)|
                       ((C6ACCEL_TI_DSP_fft16x16_paramPtr->nx)>65536)|
                       (IsPowOfTwo(C6ACCEL_TI_DSP_fft16x16_paramPtr->nx) != TRUE)){
                         return(IUNIVERSAL_EPARAMFAIL);
                    }
                    else{ 
                       /* Call underlying kernel */
                       DSP_fft16x16( (short *)inBufs->descs[C6ACCEL_TI_DSP_fft16x16_paramPtr->w_InArrID1].buf,
                                      C6ACCEL_TI_DSP_fft16x16_paramPtr->nx,
                                     (short *)inBufs->descs[C6ACCEL_TI_DSP_fft16x16_paramPtr->x_InArrID2].buf,
                                     (short *)outBufs->descs[C6ACCEL_TI_DSP_fft16x16_paramPtr->y_OutArrID1].buf);
                       // wb/inv cache for input due to inplace processing 
                          Memory_cacheWbInv(inBufs->descs[C6ACCEL_TI_DSP_fft16x16_paramPtr->x_InArrID2].buf, inBufs->descs[C6ACCEL_TI_DSP_fft16x16_paramPtr->x_InArrID2].bufSize);
                      }
                  }
                 break;
                 case (IFFT16x16_FXN_ID ):{
                    /* Unmarshal Parameters */  
                    DSP_ifft16x16_Params *C6ACCEL_TI_DSP_ifft16x16_paramPtr;
                    C6ACCEL_TI_DSP_ifft16x16_paramPtr= pFnArray;
                    /* Parameter check */
                    if (((C6ACCEL_TI_DSP_ifft16x16_paramPtr->w_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSP_ifft16x16_paramPtr->x_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_DSP_ifft16x16_paramPtr->y_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_DSP_ifft16x16_paramPtr->nx)<16)|
                        ((C6ACCEL_TI_DSP_ifft16x16_paramPtr->nx)>65536)|
                        (IsPowOfTwo(C6ACCEL_TI_DSP_ifft16x16_paramPtr->nx) != TRUE)){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else{
                        /* Call underlying kernel */
                        DSP_ifft16x16((short *)inBufs->descs[C6ACCEL_TI_DSP_ifft16x16_paramPtr->w_InArrID1].buf,
                                     C6ACCEL_TI_DSP_ifft16x16_paramPtr->nx,
                                     (short *)inBufs->descs[C6ACCEL_TI_DSP_ifft16x16_paramPtr->x_InArrID2].buf,
                                     (short *)outBufs->descs[C6ACCEL_TI_DSP_ifft16x16_paramPtr->y_OutArrID1].buf);
                        // wb/inv cache for input due to inplace processing 
                           Memory_cacheWbInv(inBufs->descs[C6ACCEL_TI_DSP_ifft16x16_paramPtr->x_InArrID2].buf,inBufs->descs[C6ACCEL_TI_DSP_ifft16x16_paramPtr->x_InArrID2].bufSize);
                      }
                   }   
                 break;
                 case (MAT_MUL_FXN_ID ):{
                    /* Unmarshal Parameters */
                    DSP_mat_mul_Params  *C6ACCEL_TI_DSP_mat_mul_paramPtr;
                    C6ACCEL_TI_DSP_mat_mul_paramPtr= pFnArray;
                    /*Parameter check
                    * 1.InArr, OutArr ID checks
                    * 2.Check for constraints on matrix dimensions
                    * 3.Condition to check if col of Input 1 is equal to row of Input 2:
                    * (buf_size Input1)/(buf_size Input2) = (rows of Input1)/(cols of Input 2)
                    */
                    if (((C6ACCEL_TI_DSP_mat_mul_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSP_mat_mul_paramPtr->y_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_DSP_mat_mul_paramPtr->r_OutArrID1)>OUTBUF15)|
                        ((float)((inBufs->descs[C6ACCEL_TI_DSP_mat_mul_paramPtr->x_InArrID1].bufSize)/
                        (inBufs->descs[C6ACCEL_TI_DSP_mat_mul_paramPtr->y_InArrID2].bufSize))!=
                        ((float)(C6ACCEL_TI_DSP_mat_mul_paramPtr->r1/C6ACCEL_TI_DSP_mat_mul_paramPtr->c2)))|
                        ((C6ACCEL_TI_DSP_mat_mul_paramPtr->c2)<4)|((C6ACCEL_TI_DSP_mat_mul_paramPtr->c2 )% 4 != 0)|
                        ((C6ACCEL_TI_DSP_mat_mul_paramPtr->c1)<1)|((C6ACCEL_TI_DSP_mat_mul_paramPtr->r1)% 2 != 0)|
                        ((C6ACCEL_TI_DSP_mat_mul_paramPtr->r1)< 8)|((C6ACCEL_TI_DSP_mat_mul_paramPtr->c1)>32767)|
                        ((C6ACCEL_TI_DSP_mat_mul_paramPtr->c2)>32767)|((C6ACCEL_TI_DSP_mat_mul_paramPtr->r1)>32767)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else
                        /* Call underlying kernel */
                        DSP_mat_mul((short *)inBufs->descs[C6ACCEL_TI_DSP_mat_mul_paramPtr->x_InArrID1].buf,
                                      C6ACCEL_TI_DSP_mat_mul_paramPtr->r1,
                                      C6ACCEL_TI_DSP_mat_mul_paramPtr->c1,
                                      (short *)inBufs->descs[C6ACCEL_TI_DSP_mat_mul_paramPtr->y_InArrID2].buf,
                                      C6ACCEL_TI_DSP_mat_mul_paramPtr->c2,
                                      (short *)outBufs->descs[C6ACCEL_TI_DSP_mat_mul_paramPtr->r_OutArrID1].buf,
                                      C6ACCEL_TI_DSP_mat_mul_paramPtr->qs);
                    }
                 break;
                 case (FIR_GEN_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSP_fir_gen_Params  *C6ACCEL_TI_DSP_fir_gen_paramPtr;
                    C6ACCEL_TI_DSP_fir_gen_paramPtr = pFnArray;
                    /* Parameter check */
                    if (((C6ACCEL_TI_DSP_fir_gen_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSP_fir_gen_paramPtr->h_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_DSP_fir_gen_paramPtr->r_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_DSP_fir_gen_paramPtr->nr)%4 != 0)| ((C6ACCEL_TI_DSP_fir_gen_paramPtr->nr)<4)){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                       /* Call underlying kernel */
                        DSP_fir_gen((short *)inBufs->descs[C6ACCEL_TI_DSP_fir_gen_paramPtr->x_InArrID1].buf,
                                    (short *)inBufs->descs[C6ACCEL_TI_DSP_fir_gen_paramPtr->h_InArrID2].buf,
                                    (short *)outBufs->descs[C6ACCEL_TI_DSP_fir_gen_paramPtr->r_OutArrID1].buf,
                                    C6ACCEL_TI_DSP_fir_gen_paramPtr->nh,
                                    C6ACCEL_TI_DSP_fir_gen_paramPtr->nr);
                   }
                 break;
                 case (AUTOCOR_FXN_ID):{
                   /* Unmarshal Parameters */
                   DSP_autocor_Params *C6ACCEL_TI_DSP_autocor_paramPtr;
                   C6ACCEL_TI_DSP_autocor_paramPtr = pFnArray;
                   /* Parameter check */
                   if (((C6ACCEL_TI_DSP_autocor_paramPtr->x_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_DSP_autocor_paramPtr->r_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSP_autocor_paramPtr->nx)%8 != 0)|
                       ((C6ACCEL_TI_DSP_autocor_paramPtr->nr)%4 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                    }
                    else
                     /* Call underlying kernel */
                        DSP_autocor((short *)outBufs->descs[C6ACCEL_TI_DSP_autocor_paramPtr->r_OutArrID1].buf,
                                    (short *)inBufs->descs[C6ACCEL_TI_DSP_autocor_paramPtr->x_InArrID1].buf,
                                     C6ACCEL_TI_DSP_autocor_paramPtr->nx,
                                     C6ACCEL_TI_DSP_autocor_paramPtr->nr);
                  }
                 break;
                 case (FFT32x32_FXN_ID ):{
                    /* Unmarshal Parameters */
                    DSP_fft32x32_Params  *C6ACCEL_TI_DSP_fft32x32_paramPtr;
                    C6ACCEL_TI_DSP_fft32x32_paramPtr= pFnArray;
                    /* Parameter check*/
                    if (((C6ACCEL_TI_DSP_fft32x32_paramPtr->w_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_DSP_fft32x32_paramPtr->x_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_DSP_fft32x32_paramPtr->y_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSP_fft32x32_paramPtr->nx)<16)|
                       ((C6ACCEL_TI_DSP_fft32x32_paramPtr->nx)>65536)|
                       (IsPowOfTwo(C6ACCEL_TI_DSP_fft32x32_paramPtr->nx) != TRUE)){
                          return(IUNIVERSAL_EPARAMFAIL);
                    }
                    else{
                     /* Call underlying kernel */
                       DSP_fft32x32((int *)inBufs->descs[C6ACCEL_TI_DSP_fft32x32_paramPtr->w_InArrID1].buf,
                                     C6ACCEL_TI_DSP_fft32x32_paramPtr->nx,
                                     (int *)inBufs->descs[C6ACCEL_TI_DSP_fft32x32_paramPtr->x_InArrID2].buf,
                                     (int *)outBufs->descs[C6ACCEL_TI_DSP_fft32x32_paramPtr->y_OutArrID1].buf);
                      // wb/inv cache for input due to inplace processing 
                        Memory_cacheWbInv(inBufs->descs[C6ACCEL_TI_DSP_fft32x32_paramPtr->x_InArrID2].buf,inBufs->descs[C6ACCEL_TI_DSP_fft32x32_paramPtr->x_InArrID2].bufSize);
                      }
                  } 
                 break;
                 case (IFFT32x32_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSP_ifft32x32_Params *C6ACCEL_TI_DSP_ifft32x32_paramPtr;
                    C6ACCEL_TI_DSP_ifft32x32_paramPtr = pFnArray;
                    /* Parameter check */
                    if (((C6ACCEL_TI_DSP_ifft32x32_paramPtr->w_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSP_ifft32x32_paramPtr->x_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_DSP_ifft32x32_paramPtr->y_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_DSP_ifft32x32_paramPtr->nx)<16)|
                        ((C6ACCEL_TI_DSP_ifft32x32_paramPtr->nx)>65536)|
                        (IsPowOfTwo(C6ACCEL_TI_DSP_ifft32x32_paramPtr->nx) != TRUE)){
                            return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                    /* Call underlying kernel */
                       DSP_ifft32x32((int *)inBufs->descs[C6ACCEL_TI_DSP_ifft32x32_paramPtr->w_InArrID1].buf,
                                      C6ACCEL_TI_DSP_ifft32x32_paramPtr->nx,
                                     (int *)inBufs->descs[C6ACCEL_TI_DSP_ifft32x32_paramPtr->x_InArrID2].buf,
                                     (int *)outBufs->descs[C6ACCEL_TI_DSP_ifft32x32_paramPtr->y_OutArrID1].buf);
                    // wb/inv cache for input due to inplace processing 
                        Memory_cacheWbInv(inBufs->descs[C6ACCEL_TI_DSP_ifft32x32_paramPtr->x_InArrID2].buf,inBufs->descs[C6ACCEL_TI_DSP_ifft32x32_paramPtr->x_InArrID2].bufSize);
                      }
                   }
                 break;
                 case (FIR_R4_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSP_fir_r4_Params *C6ACCEL_TI_DSP_fir_r4_paramPtr;
                    C6ACCEL_TI_DSP_fir_r4_paramPtr = pFnArray;
                    /* Parameter check */
                    if (((C6ACCEL_TI_DSP_fir_r4_paramPtr->x_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_DSP_fir_r4_paramPtr->h_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_DSP_fir_r4_paramPtr->r_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSP_fir_r4_paramPtr->nh)%4 != 0)|
                       ((C6ACCEL_TI_DSP_fir_r4_paramPtr->nh) < 8)|
                       ((C6ACCEL_TI_DSP_fir_r4_paramPtr->nr)%4 != 0)){
                          return(IUNIVERSAL_EPARAMFAIL);
                    }
                    else
                    /* Call underlying kernel */
                      DSP_fir_r4((short *) inBufs->descs[C6ACCEL_TI_DSP_fir_r4_paramPtr->x_InArrID1].buf,
                                 (short *) inBufs->descs[C6ACCEL_TI_DSP_fir_r4_paramPtr->h_InArrID2].buf,
                                 (short *) outBufs->descs[C6ACCEL_TI_DSP_fir_r4_paramPtr->r_OutArrID1].buf,
                                 C6ACCEL_TI_DSP_fir_r4_paramPtr->nh,
                                 C6ACCEL_TI_DSP_fir_r4_paramPtr->nr);
                   }
                 break;
                 case (FIR_R8_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSP_fir_r8_Params *C6ACCEL_TI_DSP_fir_r8_paramPtr;
                    C6ACCEL_TI_DSP_fir_r8_paramPtr = pFnArray;
                    /* Parameter check*/
                    if (((C6ACCEL_TI_DSP_fir_r8_paramPtr->x_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_DSP_fir_r8_paramPtr->h_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_DSP_fir_r8_paramPtr->r_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSP_fir_r8_paramPtr->nh)%8 != 0)|
                       ((C6ACCEL_TI_DSP_fir_r8_paramPtr->nh) < 8)|
                       ((C6ACCEL_TI_DSP_fir_r8_paramPtr->nr)%4 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                    }
                    else
                    /* Call underlying kernel */
                      DSP_fir_r8((short *) inBufs->descs[C6ACCEL_TI_DSP_fir_r8_paramPtr->x_InArrID1].buf,
                                 (short *) inBufs->descs[C6ACCEL_TI_DSP_fir_r8_paramPtr->h_InArrID2].buf,
                                 (short *) outBufs->descs[C6ACCEL_TI_DSP_fir_r8_paramPtr->r_OutArrID1].buf,
                                  C6ACCEL_TI_DSP_fir_r8_paramPtr->nh,
                                  C6ACCEL_TI_DSP_fir_r8_paramPtr->nr);
                  } 
                 break;
                 case (IIR_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSP_iir_Params *C6ACCEL_TI_DSP_iir_paramPtr;
                    C6ACCEL_TI_DSP_iir_paramPtr = pFnArray;
                    /* Parameter check*/
                    if(((C6ACCEL_TI_DSP_iir_paramPtr->h_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_DSP_iir_paramPtr->b_InArrID3)>INBUF15)|
                       ((C6ACCEL_TI_DSP_iir_paramPtr->x_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_DSP_iir_paramPtr->Ret_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSP_iir_paramPtr->nh)%8 != 0)|
                       ((C6ACCEL_TI_DSP_iir_paramPtr->nh) < 8)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       for (j = 0; j< C6ACCEL_TI_DSP_iir_paramPtr->n ; j++){
                         ((short *)outBufs->descs[C6ACCEL_TI_DSP_iir_paramPtr->Ret_OutArrID1].buf)[j] = DSP_iir(((short *)inBufs->descs[C6ACCEL_TI_DSP_iir_paramPtr->x_InArrID1].buf)[j],
                                                                                         (short *)inBufs->descs[C6ACCEL_TI_DSP_iir_paramPtr->h_InArrID2].buf,
                                                                                          C6ACCEL_TI_DSP_iir_paramPtr->nh,
                                                                                         (short *)inBufs->descs[C6ACCEL_TI_DSP_iir_paramPtr->b_InArrID3].buf);
                     }
                  }
                 break;
                 case (DOTP_SQR_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSP_dotp_sqr_Params *C6ACCEL_TI_DSP_dotp_sqr_paramPtr;
                    C6ACCEL_TI_DSP_dotp_sqr_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_DSP_dotp_sqr_paramPtr->x_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_DSP_dotp_sqr_paramPtr->y_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_DSP_dotp_sqr_paramPtr->r_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSP_dotp_sqr_paramPtr->Ret_outArrID2)>OUTBUF15)|
                       ((C6ACCEL_TI_DSP_dotp_sqr_paramPtr->nx)%4 != 0)|
                       ((C6ACCEL_TI_DSP_dotp_sqr_paramPtr->nx) < 12)){
                          return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else
                        /* Call underlying kernel */
                        ((int *)outBufs->descs[C6ACCEL_TI_DSP_dotp_sqr_paramPtr->Ret_outArrID2].buf)[0] =
                        DSP_dotp_sqr(C6ACCEL_TI_DSP_dotp_sqr_paramPtr->G,
                                     (short *)inBufs->descs[C6ACCEL_TI_DSP_dotp_sqr_paramPtr->x_InArrID1].buf,
                                     (short *)inBufs->descs[C6ACCEL_TI_DSP_dotp_sqr_paramPtr->y_InArrID2].buf,
                                     (int *)outBufs->descs[C6ACCEL_TI_DSP_dotp_sqr_paramPtr->r_OutArrID1].buf,
                                     C6ACCEL_TI_DSP_dotp_sqr_paramPtr->nx);
                  }
                 break;
                 case (DOTPROD_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSP_dotprod_Params *C6ACCEL_TI_DSP_dotprod_paramPtr;
                    C6ACCEL_TI_DSP_dotprod_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_DSP_dotprod_paramPtr->x_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_DSP_dotprod_paramPtr->y_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_DSP_dotprod_paramPtr->Ret_outArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSP_dotprod_paramPtr->nx)%4 != 0)){
                          return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                       /* Call underlying kernel */
                       ((int *)outBufs->descs[C6ACCEL_TI_DSP_dotprod_paramPtr->Ret_outArrID1].buf)[0] =
                       DSP_dotprod((short *)inBufs->descs[C6ACCEL_TI_DSP_dotprod_paramPtr->x_InArrID1].buf,
                                   (short *)inBufs->descs[C6ACCEL_TI_DSP_dotprod_paramPtr->y_InArrID2].buf,
                                    C6ACCEL_TI_DSP_dotprod_paramPtr->nx);
                  } 
                 break;
                 case (TW_GEN_FXN_ID):{
                    /* Unmarshal Parameters */
                    tw_gen_Params *C6ACCEL_TI_DSP_tw_gen_paramPtr;
                    C6ACCEL_TI_DSP_tw_gen_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_DSP_tw_gen_paramPtr->tw_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSP_tw_gen_paramPtr->n)%2 != 0)){
                          return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                       /* Call underlying kernel */
                       tw_gen((float *)outBufs->descs[C6ACCEL_TI_DSP_tw_gen_paramPtr->tw_OutArrID1].buf,
                                                                     C6ACCEL_TI_DSP_tw_gen_paramPtr->n);
                   } 
                 break;
                 case (GEN_TWIDDLE_FFT16x16_FXN_ID):{
                    /* Unmarshal Parameters */
                    gen_twiddle_fft16x16_Params *C6ACCEL_TI_DSP_gen_twiddle_fft16x16_paramPtr;
                    C6ACCEL_TI_DSP_gen_twiddle_fft16x16_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_DSP_gen_twiddle_fft16x16_paramPtr->twiddle_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSP_gen_twiddle_fft16x16_paramPtr->n)%2 != 0)){
                          return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                       /* Call underlying kernel */
                       gen_twiddle_fft16x16((short *)outBufs->descs[C6ACCEL_TI_DSP_gen_twiddle_fft16x16_paramPtr->twiddle_OutArrID1].buf,
                                                                     C6ACCEL_TI_DSP_gen_twiddle_fft16x16_paramPtr->n);
                   } 
                 break;
                 case (GEN_TWIDDLE_FFT32x32_FXN_ID):{
                    /* Unmarshal Parameters */
                    gen_twiddle_fft32x32_Params *C6ACCEL_TI_DSP_gen_twiddle_fft32x32_paramPtr;
                    C6ACCEL_TI_DSP_gen_twiddle_fft32x32_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_DSP_gen_twiddle_fft32x32_paramPtr->twiddle_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSP_gen_twiddle_fft32x32_paramPtr->n)%2 != 0)){
                          return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                       /* Call underlying kernel */
                       gen_twiddle_fft32x32((int *)outBufs->descs[C6ACCEL_TI_DSP_gen_twiddle_fft32x32_paramPtr->twiddle_OutArrID1].buf,
                                                                     C6ACCEL_TI_DSP_gen_twiddle_fft32x32_paramPtr->n,
                                                                     C6ACCEL_TI_DSP_gen_twiddle_fft32x32_paramPtr->scale);
                   } 
                 break;
                #ifdef DEVICE_FLOAT
                 case (F_SP_FFTSPXSP_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSPF_sp_fftSPxSP_Params  *C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr;
                    C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr = pFnArray;
                    /* Parameter check */
                    if (((C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->w_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->y_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->brev_InArrID3)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->N)<8)|
                        ((C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->N)>65536)|
                        (IsPowOfTwo(C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->N) != TRUE)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                    /* Call underlying kernel */
                       DSPF_sp_fftSPxSP(C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->N,(float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->x_InArrID1].buf,
                                     (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->w_InArrID2].buf,
                                     (float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->y_OutArrID1].buf,
                                     (unsigned char *)inBufs->descs[C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->brev_InArrID3].buf,
                                     C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->n_min,
                                     C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->offset,
                                     C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->n_max);
                     // wb/inv cache for input due to inplace processing 
                        Memory_cacheWbInv(inBufs->descs[C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->x_InArrID1].buf,inBufs->descs[C6ACCEL_TI_DSPF_sp_fftSPxSP_paramPtr->x_InArrID1].bufSize);
                      }
                  }
                 break;
                 case (F_SP_IFFTSPXSP_FXN_ID ):{
                    /* Unmarshal Parameters */
                    DSPF_sp_ifftSPxSP_Params *C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr;
                    C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr = pFnArray;
                    /* Parameter check */
                    if (((C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->w_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->y_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->brev_InArrID3)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->N)<8)|
                        ((C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->N)>65536)|
                        (IsPowOfTwo(C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->N) != TRUE)){
                          return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                     /* Call underlying kernel */
                       DSPF_sp_ifftSPxSP(C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->N,(float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->x_InArrID1].buf,
                                     (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->w_InArrID2].buf,
                                     (float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->y_OutArrID1].buf,
                                     (unsigned char *)inBufs->descs[C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->brev_InArrID3].buf,
                                     C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->n_min,
                                     C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->offset,
                                     C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->n_max);
                     // wb/inv cache for input due to inplace processing 
                        Memory_cacheWbInv(inBufs->descs[C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->x_InArrID1].buf,inBufs->descs[C6ACCEL_TI_DSPF_sp_ifftSPxSP_paramPtr->x_InArrID1].bufSize);
                      }
                  }
                 break;
                 case (F_SP_AUTOCOR_FXN_ID ):{
                    /* Unmarshal Parameters */
                    DSPF_sp_autocor_Params *C6ACCEL_TI_DSPF_sp_autocor_paramPtr;
                    C6ACCEL_TI_DSPF_sp_autocor_paramPtr = pFnArray;
                    /* Parameter check */
                    if (((C6ACCEL_TI_DSPF_sp_autocor_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_autocor_paramPtr->r_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_autocor_paramPtr->nr)%4 != 0)|((C6ACCEL_TI_DSPF_sp_autocor_paramPtr->nr)<0)|
                        (IsPowOfTwo(C6ACCEL_TI_DSPF_sp_autocor_paramPtr->nx) != TRUE)|
                        ((C6ACCEL_TI_DSPF_sp_autocor_paramPtr->nx)<(C6ACCEL_TI_DSPF_sp_autocor_paramPtr->nr))){
                          return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                     /* Call underlying kernel */
                       DSPF_sp_autocor((float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_autocor_paramPtr->r_OutArrID1].buf,
                                       (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_autocor_paramPtr->x_InArrID1].buf,
                                       C6ACCEL_TI_DSPF_sp_autocor_paramPtr->nx,
                                       C6ACCEL_TI_DSPF_sp_autocor_paramPtr->nr);
                  }
                 break;
                 case (F_SP_CONV_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSPF_sp_convol_Params *C6ACCEL_TI_DSPF_sp_convol_paramPtr;
                    C6ACCEL_TI_DSPF_sp_convol_paramPtr = pFnArray;
                    /* Parameter check */
                    if (((C6ACCEL_TI_DSPF_sp_convol_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_convol_paramPtr->h_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_convol_paramPtr->y_OutArrID1)>OUTBUF15)|
                        (IsPowOfTwo(C6ACCEL_TI_DSPF_sp_convol_paramPtr->ny) != TRUE)|
                        ((C6ACCEL_TI_DSPF_sp_convol_paramPtr->ny)<0)|
                        (IsPowOfTwo(C6ACCEL_TI_DSPF_sp_convol_paramPtr->nh) != TRUE)|
                        ((C6ACCEL_TI_DSPF_sp_convol_paramPtr->nh)<0)){
                          return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                     /* Call underlying kernel */
                       DSPF_sp_convol((float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_convol_paramPtr->x_InArrID1].buf,
                                      (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_convol_paramPtr->h_InArrID2].buf,
                                      (float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_convol_paramPtr->y_OutArrID1].buf,
                                       C6ACCEL_TI_DSPF_sp_convol_paramPtr->nh,
                                       C6ACCEL_TI_DSPF_sp_convol_paramPtr->ny);
                  }
                 break;
                 case (F_SP_BIQUAD_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSPF_sp_biquad_Params *C6ACCEL_TI_DSPF_sp_biquad_paramPtr;
                    C6ACCEL_TI_DSPF_sp_biquad_paramPtr = pFnArray;
                    /* Parameter check */
                    if (((C6ACCEL_TI_DSPF_sp_biquad_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_biquad_paramPtr->a_InArrID3)>INBUF15)|




                        ((C6ACCEL_TI_DSPF_sp_biquad_paramPtr->b_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_biquad_paramPtr->delay_InArrID4)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_biquad_paramPtr->y_OutArrID1)>OUTBUF15)|
                        ((inBufs->descs[C6ACCEL_TI_DSPF_sp_biquad_paramPtr->a_InArrID3].bufSize)!=3*sizeof(float))|
                        ((inBufs->descs[C6ACCEL_TI_DSPF_sp_biquad_paramPtr->b_InArrID2].bufSize)!=3*sizeof(float))|
                        (IsPowOfTwo(C6ACCEL_TI_DSPF_sp_biquad_paramPtr->n) != TRUE)){
                          return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                     /* Call underlying kernel */
                       DSPF_sp_biquad((float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_biquad_paramPtr->x_InArrID1].buf,
                                      (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_biquad_paramPtr->b_InArrID2].buf,
                                      (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_biquad_paramPtr->a_InArrID3].buf,
                                      (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_biquad_paramPtr->delay_InArrID4].buf,
                                      (float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_biquad_paramPtr->y_OutArrID1].buf,
                                       C6ACCEL_TI_DSPF_sp_biquad_paramPtr->n);
                  }
                 break;
                 case (F_SP_DOTPROD_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSPF_sp_dotprod_Params *C6ACCEL_TI_DSPF_sp_dotprod_paramPtr;
                    C6ACCEL_TI_DSPF_sp_dotprod_paramPtr = pFnArray;
                    /* Parameter check */
                    if (((C6ACCEL_TI_DSPF_sp_dotprod_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_dotprod_paramPtr->y_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_dotprod_paramPtr->RetOut_OutArrID1)>OUTBUF15)|
                        (IsPowOfTwo(C6ACCEL_TI_DSPF_sp_dotprod_paramPtr->n) != TRUE)|
                        ((C6ACCEL_TI_DSPF_sp_dotprod_paramPtr->n)<0)){
                          return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                     /* Call underlying kernel */
                       ((float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_dotprod_paramPtr->RetOut_OutArrID1].buf)[0]=
                       DSPF_sp_dotprod((float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_dotprod_paramPtr->x_InArrID1].buf,
                                       (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_dotprod_paramPtr->y_InArrID2].buf,
                                        C6ACCEL_TI_DSPF_sp_dotprod_paramPtr->n);
                  }
                 break;
                 case (F_SP_FIR_GEN_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSPF_sp_fir_gen_Params  *C6ACCEL_TI_DSPF_sp_fir_gen_paramPtr;
                    C6ACCEL_TI_DSPF_sp_fir_gen_paramPtr = pFnArray;
                    /* Parameter check */
                    if (((C6ACCEL_TI_DSPF_sp_fir_gen_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_fir_gen_paramPtr->h_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_fir_gen_paramPtr->y_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_fir_gen_paramPtr->nh)%4 != 0)| ((C6ACCEL_TI_DSPF_sp_fir_gen_paramPtr->nh)<=0)|
                        ((C6ACCEL_TI_DSPF_sp_fir_gen_paramPtr->ny)%4 != 0)| ((C6ACCEL_TI_DSPF_sp_fir_gen_paramPtr->ny)<=0)){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                       /* Call underlying kernel */
                        DSPF_sp_fir_gen((float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_fir_gen_paramPtr->x_InArrID1].buf,
                                        (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_fir_gen_paramPtr->h_InArrID2].buf,
                                        (float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_fir_gen_paramPtr->y_OutArrID1].buf,
                                        C6ACCEL_TI_DSPF_sp_fir_gen_paramPtr->nh,
                                        C6ACCEL_TI_DSPF_sp_fir_gen_paramPtr->ny);
                  }
                 break;
                 case (F_SP_FIR_CPLX_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSPF_sp_fir_cplx_Params  *C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr;
                    C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr = pFnArray;
                    /* Parameter check */
                    if (((C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->h_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->y_OutArrID1)>OUTBUF15)|
                        ((inBufs->descs[C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->h_InArrID2].bufSize)!=(2*(C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->nh)*sizeof(float)))|
                        ((outBufs->descs[C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->y_OutArrID1].bufSize)!=(2*(C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->ny)*sizeof(float)))|
                        ((C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->nh)<=0)|
                        ((C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->ny)%2 != 0)| ((C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->ny)<=0)){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                       /* Call underlying kernel */
                        DSPF_sp_fir_cplx((float *)(inBufs->descs[C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->x_InArrID1].buf+ 2*sizeof(float)*((C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->nh)-1)),
                                        (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->h_InArrID2].buf,
                                        (float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->y_OutArrID1].buf,
                                        C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->nh,
                                        C6ACCEL_TI_DSPF_sp_fir_cplx_paramPtr->ny);
                  }
                 break;
                 case (F_SP_IIR_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSPF_sp_iir_Params *C6ACCEL_TI_DSPF_sp_iir_paramPtr;
                    C6ACCEL_TI_DSPF_sp_iir_paramPtr = pFnArray;
                    /* Parameter check*/
                    if(((C6ACCEL_TI_DSPF_sp_iir_paramPtr->x_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_DSPF_sp_iir_paramPtr->hb_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_DSPF_sp_iir_paramPtr->ha_InArrID3)>INBUF15)|
                       ((C6ACCEL_TI_DSPF_sp_iir_paramPtr->y1_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSPF_sp_iir_paramPtr->y2_OutArrID2)>OUTBUF15)|
                       (outBufs->descs[C6ACCEL_TI_DSPF_sp_iir_paramPtr->y1_OutArrID1].bufSize !=((C6ACCEL_TI_DSPF_sp_iir_paramPtr->n)+4)*sizeof(float))|
                       (outBufs->descs[C6ACCEL_TI_DSPF_sp_iir_paramPtr->y2_OutArrID2].bufSize !=(C6ACCEL_TI_DSPF_sp_iir_paramPtr->n)*sizeof(float))|
                       (inBufs->descs[C6ACCEL_TI_DSPF_sp_iir_paramPtr->x_InArrID1].bufSize !=((C6ACCEL_TI_DSPF_sp_iir_paramPtr->n)+4)*sizeof(float))|
                       (IsPowOfTwo(C6ACCEL_TI_DSPF_sp_iir_paramPtr->n) != TRUE)|
                       ((C6ACCEL_TI_DSPF_sp_iir_paramPtr->n) < 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                        DSPF_sp_iir((float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_iir_paramPtr->y1_OutArrID1].buf,
                                    (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_iir_paramPtr->x_InArrID1].buf,
                                    (float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_iir_paramPtr->y2_OutArrID2].buf,
                                    (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_iir_paramPtr->hb_InArrID2].buf,
                                    (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_iir_paramPtr->ha_InArrID3].buf,
                                    C6ACCEL_TI_DSPF_sp_iir_paramPtr->n);
                     
                  }
                 break;
                 case (F_SP_MAT_MUL_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSPF_sp_mat_mul_Params  *C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr;
                    C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr= pFnArray;
                    /*Parameter check
                    * 1.InArr, OutArr ID checks
                    * 2.Check for constraints on matrix dimensions
                    * 3.Condition to check if col of Input 1 is equal to row of Input 2:
                    * (buf_size Input1)/(buf_size Input2) = (rows of Input1)/(cols of Input 2)
                    */
                    if (((C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr->x1_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr->x2_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr->y_OutArrID1)>OUTBUF15)|
                        ((float)((inBufs->descs[C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr->x1_InArrID1].bufSize)/
                        (inBufs->descs[C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr->x2_InArrID2].bufSize))!=
                        ((float)(C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr->r1/C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr->c2)))){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else
                        /* Call underlying kernel */
                        DSPF_sp_mat_mul((float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr->x1_InArrID1].buf,
                                        C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr->r1,
                                        C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr->c1,
                                        (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr->x2_InArrID2].buf,
                                        C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr->c2,
                                        (float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_mat_mul_paramPtr->y_OutArrID1].buf);
                    }
                 break;
                 case (F_SP_MAT_MUL_CPLX_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSPF_sp_mat_mul_cplx_Params  *C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr;
                    C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr= pFnArray;
                    /*Parameter check
                    * 1.InArr, OutArr ID checks
                    * 2.Check for constraints on matrix dimensions
                    * 3.Condition to check if col of Input 1 is equal to row of Input 2:
                    * (buf_size Input1)/(buf_size Input2) = (rows of Input1)/(cols of Input 2)
                    */
                    if (((C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr->x1_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr->x2_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr->y_OutArrID1)>OUTBUF15)|
                        ((float)((inBufs->descs[C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr->x1_InArrID1].bufSize)/
                        (inBufs->descs[C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr->x2_InArrID2].bufSize))!=
                        ((float)(C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr->r1/C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr->c2)))){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else
                        /* Call underlying kernel */
                        DSPF_sp_mat_mul_cplx((float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr->x1_InArrID1].buf,
                                        C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr->r1,
                                        C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr->c1,
                                        (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr->x2_InArrID2].buf,
                                        C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr->c2,
                                        (float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_mat_mul_cplx_paramPtr->y_OutArrID1].buf);
                    }
                 break;
                 case (F_SP_MAT_TRANS_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSPF_sp_mat_trans_Params  *C6ACCEL_TI_DSPF_sp_mat_trans_paramPtr;
                    C6ACCEL_TI_DSPF_sp_mat_trans_paramPtr= pFnArray;
                    /*Parameter check
                    * 1.InArr, OutArr ID checks
                    * 2.Check for constraints on matrix dimensions
                    */
                    if (((C6ACCEL_TI_DSPF_sp_mat_trans_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_mat_trans_paramPtr->y_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_DSPF_sp_mat_trans_paramPtr->rows)<2)|
                        ((C6ACCEL_TI_DSPF_sp_mat_trans_paramPtr->cols)<2)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else
                        /* Call underlying kernel */
                        DSPF_sp_mat_trans((float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_mat_trans_paramPtr->x_InArrID1].buf,
                                          C6ACCEL_TI_DSPF_sp_mat_trans_paramPtr->rows,
                                          C6ACCEL_TI_DSPF_sp_mat_trans_paramPtr->cols,
                                          (float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_mat_trans_paramPtr->y_OutArrID1].buf);
                   }
                 break;
                 case (F_SP_VECMUL_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSPF_sp_vecmul_Params *C6ACCEL_TI_DSPF_sp_vecmul_paramPtr;
                    C6ACCEL_TI_DSPF_sp_vecmul_paramPtr = pFnArray;
                    /* Parameter check*/
                    if(((C6ACCEL_TI_DSPF_sp_vecmul_paramPtr->x1_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_DSPF_sp_vecmul_paramPtr->x2_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_DSPF_sp_vecmul_paramPtr->y_OutArrID1)>OUTBUF15)|
                       (IsPowOfTwo(C6ACCEL_TI_DSPF_sp_vecmul_paramPtr->n) != TRUE)|
                       ((C6ACCEL_TI_DSPF_sp_vecmul_paramPtr->n) < 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                        DSPF_sp_vecmul((float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_vecmul_paramPtr->x1_InArrID1].buf,
                                       (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_vecmul_paramPtr->x2_InArrID2].buf,
                                       (float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_vecmul_paramPtr->y_OutArrID1].buf,
                                       C6ACCEL_TI_DSPF_sp_vecmul_paramPtr->n);
          
                  }
                 break;
                 case (F_SP_VECRECIP_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSPF_sp_vecrecip_Params *C6ACCEL_TI_DSPF_sp_vecrecip_paramPtr;
                    C6ACCEL_TI_DSPF_sp_vecrecip_paramPtr = pFnArray;
                    /* Parameter check*/
                    if(((C6ACCEL_TI_DSPF_sp_vecrecip_paramPtr->x_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_DSPF_sp_vecrecip_paramPtr->y_OutArrID1)>OUTBUF15)|
                       (IsPowOfTwo(C6ACCEL_TI_DSPF_sp_vecrecip_paramPtr->n) != TRUE)|
                       ((C6ACCEL_TI_DSPF_sp_vecrecip_paramPtr->n) < 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                        DSPF_sp_vecrecip((float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_vecrecip_paramPtr->x_InArrID1].buf,
                                         (float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_vecrecip_paramPtr->y_OutArrID1].buf,
                                         C6ACCEL_TI_DSPF_sp_vecrecip_paramPtr->n);
          
                  }

                 break;
                 case (F_SP_VECSUM_SQ_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSPF_sp_vecsum_sq_Params *C6ACCEL_TI_DSPF_sp_vecsum_sq_paramPtr;
                    C6ACCEL_TI_DSPF_sp_vecsum_sq_paramPtr = pFnArray;
                    /* Parameter check*/
                    if(((C6ACCEL_TI_DSPF_sp_vecsum_sq_paramPtr->x_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_DSPF_sp_vecsum_sq_paramPtr->OutRet_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSPF_sp_vecsum_sq_paramPtr->n) < 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                         ((float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_vecsum_sq_paramPtr->OutRet_OutArrID1].buf)[0]=
                         DSPF_sp_vecsum_sq((float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_vecsum_sq_paramPtr->x_InArrID1].buf,
                                            C6ACCEL_TI_DSPF_sp_vecsum_sq_paramPtr->n);
          
                  }
                 break;
                 case (F_SP_W_VEC_FXN_ID):{
                    /* Unmarshal Parameters */
                    DSPF_sp_w_vec_Params *C6ACCEL_TI_DSPF_sp_w_vec_paramPtr;
                    C6ACCEL_TI_DSPF_sp_w_vec_paramPtr = pFnArray;
                    /* Parameter check*/
                    if(((C6ACCEL_TI_DSPF_sp_w_vec_paramPtr->x1_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_DSPF_sp_w_vec_paramPtr->x2_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_DSPF_sp_w_vec_paramPtr->y_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSPF_sp_w_vec_paramPtr->n) < 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                         DSPF_sp_w_vec((float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_w_vec_paramPtr->x1_InArrID1].buf,
                                       (float *)inBufs->descs[C6ACCEL_TI_DSPF_sp_w_vec_paramPtr->x2_InArrID2].buf,
                                       C6ACCEL_TI_DSPF_sp_w_vec_paramPtr->m,
                                       (float *)outBufs->descs[C6ACCEL_TI_DSPF_sp_w_vec_paramPtr->y_OutArrID1].buf,
                                       C6ACCEL_TI_DSPF_sp_w_vec_paramPtr->n);
          
                  }
                 break;
		 case (F_COMPLXTOREALNIMG_FXN_ID):{
                    
                    DSPF_complxtorealnimg_Params *C6ACCEL_TI_DSPF_complxtorealnimg_paramPtr;
                    C6ACCEL_TI_DSPF_complxtorealnimg_paramPtr = pFnArray;
                    
                    if(((C6ACCEL_TI_DSPF_complxtorealnimg_paramPtr->cplx_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_DSPF_complxtorealnimg_paramPtr->real_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_DSPF_complxtorealnimg_paramPtr->img_OutArrID2)>OUTBUF15)|
                       ((C6ACCEL_TI_DSPF_complxtorealnimg_paramPtr->n) < 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    
                         complxtorealnimg((float *)inBufs->descs[C6ACCEL_TI_DSPF_complxtorealnimg_paramPtr->cplx_InArrID1].buf,
                                          (float *)outBufs->descs[C6ACCEL_TI_DSPF_complxtorealnimg_paramPtr->real_OutArrID1].buf,
                                          (float *)outBufs->descs[C6ACCEL_TI_DSPF_complxtorealnimg_paramPtr->img_OutArrID2].buf,
                                          C6ACCEL_TI_DSPF_complxtorealnimg_paramPtr->n);
          
                  }
                 break;
               #endif
                 default:
                   /*Error caused due to passing of an invalid ID*/
                      return(IUNIVERSAL_EFXNIDFAIL);
               }
    return 0;
}


/****************************************************************************
**                      Algorithm specific Function defintions
*****************************************************************************/
/* Function to check the condition if a number is power of 2         *
 *   This function is useful in checking the N parameter passed to   *
 *   FFT and Inverse FFT functions are power of 2                    */

static U16_IQ IsPowOfTwo( Uword40 x)
{
   return (x != 0) && ((x & (x - 1)) == 0);
}

/*=============================================================================*/
/*                  End of File C6accel_ti_dsplibFunctionCall.c                */
/*                              Version : 0.0.1                                */
/*=============================================================================*/
