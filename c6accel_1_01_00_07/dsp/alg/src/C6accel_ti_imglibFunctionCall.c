/*==================================================================== */
/*  Copyright (c) 2010, Texas Instruments Incorporated                 */
/*  All rights reserved.                                               */
/*                                                                     */
/*                                                                     */
/* ======== C6accel_ti_imglibFunctionCall.c ========                   */
/* This file is used to invoke IMGLib kernels based on the function ID */
/*   passed by the application to C6Accel xdais algorithm              */
/*                                                                     */
/*                                                                     */
/*  Version: 0.0.1                                                     */
/*==================================================================== */

/* Include Files */
#include <xdc/std.h>
#include <string.h>
#include <ti/xdais/dm/iuniversal.h>

/* Header Intrinsic to xdais algorithm*/
#include "C6Accel.h"

/* Application-Algorithm Interface file includes */
#include "iC6Accel_ti.h"
#include "iC6Accel_ti_priv.h"

int C6ACCEL_TI_imglibFunctionCall(void *pFnArray,int fxnID,XDM1_BufDesc *inBufs, XDM1_BufDesc *outBufs)
{
 Int32 fxnidMsk= FXN_ID_MASK;

          switch ( fxnID & fxnidMsk){
                 case (SOBEL_3X3_8_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_sobel_3x3_8_Params *C6ACCEL_TI_IMG_sobel_3x3_8_paramPtr;
                    C6ACCEL_TI_IMG_sobel_3x3_8_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_IMG_sobel_3x3_8_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_IMG_sobel_3x3_8_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_IMG_sobel_3x3_8_paramPtr->Col)%2 != 0)|
                        (((C6ACCEL_TI_IMG_sobel_3x3_8_paramPtr->Col)*((C6ACCEL_TI_IMG_sobel_3x3_8_paramPtr->Row)-2))%8 !=0)){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       IMG_sobel_3x3_8((const unsigned char *)inBufs->descs[C6ACCEL_TI_IMG_sobel_3x3_8_paramPtr->indata_InArrID1].buf,
                                       (unsigned char *)outBufs->descs[C6ACCEL_TI_IMG_sobel_3x3_8_paramPtr->outdata_OutArrID1].buf,
                                        C6ACCEL_TI_IMG_sobel_3x3_8_paramPtr->Col,
                                        C6ACCEL_TI_IMG_sobel_3x3_8_paramPtr->Row);
                     }
                 break;
                 case (SOBEL_3X3_8_16_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_sobel_3x3_8_16_Params *C6ACCEL_TI_IMG_sobel_3x3_8_16_paramPtr;
                    C6ACCEL_TI_IMG_sobel_3x3_8_16_paramPtr= pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_IMG_sobel_3x3_8_16_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_IMG_sobel_3x3_8_16_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_IMG_sobel_3x3_8_16_paramPtr->Col)%2 != 0)|
                        (((C6ACCEL_TI_IMG_sobel_3x3_8_16_paramPtr->Col)*((C6ACCEL_TI_IMG_sobel_3x3_8_16_paramPtr->Row)-2))%8 !=0)){
                           return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       IMG_sobel_3x3_8_16((const unsigned char *)inBufs->descs[C6ACCEL_TI_IMG_sobel_3x3_8_16_paramPtr->indata_InArrID1].buf,
                                       (unsigned short *)outBufs->descs[C6ACCEL_TI_IMG_sobel_3x3_8_16_paramPtr->outdata_OutArrID1].buf,
                                        C6ACCEL_TI_IMG_sobel_3x3_8_16_paramPtr->Col,
                                        C6ACCEL_TI_IMG_sobel_3x3_8_16_paramPtr->Row);
                     }
                 case (SOBEL_3X3_16_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_sobel_3x3_16_Params *C6ACCEL_TI_IMG_sobel_3x3_16_paramPtr;
                    C6ACCEL_TI_IMG_sobel_3x3_16_paramPtr = pFnArray;
                    /*Parameter check*/
                    if(((C6ACCEL_TI_IMG_sobel_3x3_16_paramPtr->indata_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_sobel_3x3_16_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_sobel_3x3_16_paramPtr->Col)%2 != 0)|
                       ((C6ACCEL_TI_IMG_sobel_3x3_16_paramPtr->Row) < 3)|
                       (((C6ACCEL_TI_IMG_sobel_3x3_16_paramPtr->Col)*((C6ACCEL_TI_IMG_sobel_3x3_16_paramPtr->Row)-2))%8 !=0)){
                          return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                        IMG_sobel_3x3_16((const unsigned short *)inBufs->descs[C6ACCEL_TI_IMG_sobel_3x3_16_paramPtr->indata_InArrID1].buf,
                                         (unsigned short *)outBufs->descs[C6ACCEL_TI_IMG_sobel_3x3_16_paramPtr->outdata_OutArrID1].buf,
                                          C6ACCEL_TI_IMG_sobel_3x3_16_paramPtr->Col,
                                          C6ACCEL_TI_IMG_sobel_3x3_16_paramPtr->Row);
                    }
                 break;
                 case (HISTOGRAM_8_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_histogram_8_Params *C6ACCEL_TI_IMG_histogram_8_paramPtr;
                    C6ACCEL_TI_IMG_histogram_8_paramPtr = pFnArray;
                    /*Parameter check*/
                    if (((C6ACCEL_TI_IMG_histogram_8_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_IMG_histogram_8_paramPtr->t_hist_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_IMG_histogram_8_paramPtr->hist_OutArrID2)>OUTBUF15)|
                        ((C6ACCEL_TI_IMG_histogram_8_paramPtr->n)%8 != 0)|
                         (outBufs->descs[C6ACCEL_TI_IMG_histogram_8_paramPtr->hist_OutArrID2].bufSize<(256*sizeof(short)))|
                         (outBufs->descs[C6ACCEL_TI_IMG_histogram_8_paramPtr->t_hist_OutArrID1].bufSize<(1024*sizeof(short)))){
                             return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else
                     /* Call underlying kernel */
                        IMG_histogram_8((unsigned char *)inBufs->descs[C6ACCEL_TI_IMG_histogram_8_paramPtr->indata_InArrID1].buf,
                                         C6ACCEL_TI_IMG_histogram_8_paramPtr->n,
                                         C6ACCEL_TI_IMG_histogram_8_paramPtr->accumulate,
                                         (short *)outBufs->descs[C6ACCEL_TI_IMG_histogram_8_paramPtr->t_hist_OutArrID1].buf,
                                         (short *)outBufs->descs[C6ACCEL_TI_IMG_histogram_8_paramPtr->hist_OutArrID2].buf);

                    }
                 break;
                 case (HISTOGRAM_16_FXN_ID ):{
                    /* Unmarshal Parameters */
                    IMG_histogram_16_Params *C6ACCEL_TI_IMG_histogram_16_paramPtr;
                    C6ACCEL_TI_IMG_histogram_16_paramPtr = pFnArray;
                    /* Parameter check */
                    if (((C6ACCEL_TI_IMG_histogram_16_paramPtr->in_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_IMG_histogram_16_paramPtr->t_hist_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_IMG_histogram_16_paramPtr->hist_OutArrID2)>OUTBUF15)|
                        ((C6ACCEL_TI_IMG_histogram_16_paramPtr->n)%8 != 0)|
                        ((C6ACCEL_TI_IMG_histogram_16_paramPtr->img_bits) <1)){
                          return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       IMG_histogram_16_c((unsigned short*)inBufs->descs[C6ACCEL_TI_IMG_histogram_16_paramPtr->in_InArrID1].buf,
                                           C6ACCEL_TI_IMG_histogram_16_paramPtr->n,
                                           C6ACCEL_TI_IMG_histogram_16_paramPtr->accumulate,
                                          (short *)outBufs->descs[C6ACCEL_TI_IMG_histogram_16_paramPtr->t_hist_OutArrID1].buf,
                                          (short *)outBufs->descs[C6ACCEL_TI_IMG_histogram_16_paramPtr->hist_OutArrID2].buf,
                                           C6ACCEL_TI_IMG_histogram_16_paramPtr->img_bits);
                    }
                 break;
                 case (MEDIAN_3X3_8_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_median_3x3_8_Params *C6ACCEL_TI_IMG_median_3x3_8_paramPtr;
                    C6ACCEL_TI_IMG_median_3x3_8_paramPtr = pFnArray;
                    /*Parameter check*/
                    if(((C6ACCEL_TI_IMG_median_3x3_8_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_median_3x3_8_paramPtr->OutArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_median_3x3_8_paramPtr->Col)%4 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       IMG_median_3x3_8((unsigned char *)inBufs->descs[C6ACCEL_TI_IMG_median_3x3_8_paramPtr->InArrID1].buf,
                                        C6ACCEL_TI_IMG_median_3x3_8_paramPtr->Col,
                                        (unsigned char *)outBufs->descs[C6ACCEL_TI_IMG_median_3x3_8_paramPtr->OutArrID1].buf);
                    }
                 break;
                 case (MEDIAN_3X3_16_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_median_3x3_16_Params *C6ACCEL_TI_IMG_median_3x3_16_paramPtr;
                    C6ACCEL_TI_IMG_median_3x3_16_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_median_3x3_16_paramPtr->idata_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_median_3x3_16_paramPtr->odata_OutArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_median_3x3_16_paramPtr->nwidth) < 4)|
                       ((C6ACCEL_TI_IMG_median_3x3_16_paramPtr->nwidth)%4 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       IMG_median_3x3_16((const unsigned short *)inBufs->descs[C6ACCEL_TI_IMG_median_3x3_16_paramPtr->idata_InArrID1].buf,
                                          C6ACCEL_TI_IMG_median_3x3_16_paramPtr->nwidth,
                                         (unsigned short *)outBufs->descs[C6ACCEL_TI_IMG_median_3x3_16_paramPtr->odata_OutArrID1].buf);
                    }
                  break;
                  case (MEDIAN_3X3_16S_FXN_ID):{
                     /* Unmarshal Parameters */
                     IMG_median_3x3_16s_Params *C6ACCEL_TI_IMG_median_3x3_16s_paramPtr;
                     C6ACCEL_TI_IMG_median_3x3_16s_paramPtr  = pFnArray;
                     /* Parameter check */
                     if (((C6ACCEL_TI_IMG_median_3x3_16s_paramPtr->idata_InArrID1)>INBUF15)|
                         ((C6ACCEL_TI_IMG_median_3x3_16s_paramPtr->odata_OutArrID1)>INBUF15)|
                         ((C6ACCEL_TI_IMG_median_3x3_16s_paramPtr->nwidth) < 4)|
                         ((C6ACCEL_TI_IMG_median_3x3_16s_paramPtr->nwidth) % 4 != 0)){
                           return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else
                     /* Call underlying kernel */
                        IMG_median_3x3_16s((const short *)inBufs->descs[C6ACCEL_TI_IMG_median_3x3_16s_paramPtr->idata_InArrID1].buf,
                                           C6ACCEL_TI_IMG_median_3x3_16s_paramPtr->nwidth,
                                           (short *)outBufs->descs[C6ACCEL_TI_IMG_median_3x3_16s_paramPtr->odata_OutArrID1].buf);
                    }
                  break;
                  case (MEDIAN_3X3_8_FRAME_FXN_ID):{
                     /* Unmarshal Parameters */
                     IMG_median_3x3_8_Frame_Params *C6ACCEL_TI_IMG_median_3x3_8_Frame_paramPtr;
                     C6ACCEL_TI_IMG_median_3x3_8_Frame_paramPtr = pFnArray;
                     /*Parameter check*/
                     if(((C6ACCEL_TI_IMG_median_3x3_8_Frame_paramPtr->InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_IMG_median_3x3_8_Frame_paramPtr->OutArrID1)>INBUF15)|
                        ((C6ACCEL_TI_IMG_median_3x3_8_Frame_paramPtr->Row) <= 0)|
                        ((C6ACCEL_TI_IMG_median_3x3_8_Frame_paramPtr->Col)%4 != 0)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{ // Use Row to loop through image line by line using existing DSP API
                         int r;
                         /* Call underlying kernel */
                         unsigned char *pInChar=(unsigned char*)(inBufs->descs[C6ACCEL_TI_IMG_median_3x3_8_Frame_paramPtr->InArrID1].buf);
                         unsigned char *pOutChar=(unsigned char*)(outBufs->descs[C6ACCEL_TI_IMG_median_3x3_8_Frame_paramPtr->OutArrID1].buf);
                         for(r=0;r<C6ACCEL_TI_IMG_median_3x3_8_Frame_paramPtr->Row;r++){
                             IMG_median_3x3_8(pInChar+r*C6ACCEL_TI_IMG_median_3x3_8_Frame_paramPtr->Col,
                                          C6ACCEL_TI_IMG_median_3x3_8_Frame_paramPtr->Col,
                                          pOutChar+r*C6ACCEL_TI_IMG_median_3x3_8_Frame_paramPtr->Col);
                          }
                      }
                   }
                  break;
                  case (MEDIAN_3X3_16_FRAME_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_median_3x3_16_Frame_Params *C6ACCEL_TI_IMG_median_3x3_16_Frame_paramPtr;
                    C6ACCEL_TI_IMG_median_3x3_16_Frame_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_median_3x3_16_Frame_paramPtr->idata_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_median_3x3_16_Frame_paramPtr->odata_OutArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_median_3x3_16_Frame_paramPtr->Row) <= 0)|
                       ((C6ACCEL_TI_IMG_median_3x3_16_Frame_paramPtr->nwidth) < 4)|
                       ((C6ACCEL_TI_IMG_median_3x3_16_Frame_paramPtr->nwidth)%4 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else
                      { // Use Row to loop through image line by line using existing DSP API
                        int r;
                        unsigned short *pInShort=(unsigned short*)(inBufs->descs[C6ACCEL_TI_IMG_median_3x3_16_Frame_paramPtr->idata_InArrID1].buf);
                        unsigned short *pOutShort=(unsigned short*)(outBufs->descs[C6ACCEL_TI_IMG_median_3x3_16_Frame_paramPtr->odata_OutArrID1].buf);
                        /* Call underlying kernel */
                        for(r=0;r<C6ACCEL_TI_IMG_median_3x3_16_Frame_paramPtr->Row;r++){
                            IMG_median_3x3_16(pInShort+r*C6ACCEL_TI_IMG_median_3x3_16_Frame_paramPtr->nwidth,
                                              C6ACCEL_TI_IMG_median_3x3_16_Frame_paramPtr->nwidth,
                                              pOutShort+r*C6ACCEL_TI_IMG_median_3x3_16_Frame_paramPtr->nwidth);
                          }
                       }
                    }
                  break;
                  case (MEDIAN_3X3_16S_FRAME_FXN_ID):{
                     /* Unmarshal Parameters */
                     IMG_median_3x3_16s_Frame_Params *C6ACCEL_TI_IMG_median_3x3_16s_Frame_paramPtr;
                     C6ACCEL_TI_IMG_median_3x3_16s_Frame_paramPtr  = pFnArray;
                     /* Parameter check */
                     if (((C6ACCEL_TI_IMG_median_3x3_16s_Frame_paramPtr->idata_InArrID1)>INBUF15)|
                         ((C6ACCEL_TI_IMG_median_3x3_16s_Frame_paramPtr->odata_OutArrID1)>INBUF15)|
                         ((C6ACCEL_TI_IMG_median_3x3_16s_Frame_paramPtr->Row) <= 0)|
                         ((C6ACCEL_TI_IMG_median_3x3_16s_Frame_paramPtr->nwidth) < 4)|
                         ((C6ACCEL_TI_IMG_median_3x3_16s_Frame_paramPtr->nwidth) % 4 != 0)){
                           return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else
                       { // Use Row to loop through image line by line using existing DSP API
                        int r;
                        short *pInShort=(short*)(inBufs->descs[C6ACCEL_TI_IMG_median_3x3_16s_Frame_paramPtr->idata_InArrID1].buf);
                        short *pOutShort=(short*)(outBufs->descs[C6ACCEL_TI_IMG_median_3x3_16s_Frame_paramPtr->odata_OutArrID1].buf);
                        /* Call underlying kernel */
                        for(r=0;r<C6ACCEL_TI_IMG_median_3x3_16s_Frame_paramPtr->Row;r++){
                           IMG_median_3x3_16s( pInShort+r*C6ACCEL_TI_IMG_median_3x3_16s_Frame_paramPtr->nwidth,
                                               C6ACCEL_TI_IMG_median_3x3_16s_Frame_paramPtr->nwidth,
                                               pOutShort+r*C6ACCEL_TI_IMG_median_3x3_16s_Frame_paramPtr->nwidth);
                        }
                     }
                   }
                 break;
                 case (CONV_3X3_I8_C8S_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_conv_3x3_i8_c8s_Params *C6ACCEL_TI_IMG_conv_3x3_i8_c8s_paramPtr;
                    C6ACCEL_TI_IMG_conv_3x3_i8_c8s_paramPtr = pFnArray;
                    /*Parameter check*/
                    if(((C6ACCEL_TI_IMG_conv_3x3_i8_c8s_paramPtr->in_data_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_conv_3x3_i8_c8s_paramPtr->out_data_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_conv_3x3_i8_c8s_paramPtr->mask_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_conv_3x3_i8_c8s_paramPtr->cols)%8 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       IMG_conv_3x3_i8_c8s((unsigned char *)inBufs->descs[C6ACCEL_TI_IMG_conv_3x3_i8_c8s_paramPtr->in_data_InArrID1].buf,
                                           (unsigned char *)outBufs->descs[C6ACCEL_TI_IMG_conv_3x3_i8_c8s_paramPtr->out_data_OutArrID1].buf,
                                           C6ACCEL_TI_IMG_conv_3x3_i8_c8s_paramPtr->cols,
                                           (const char *)inBufs->descs[C6ACCEL_TI_IMG_conv_3x3_i8_c8s_paramPtr->mask_InArrID2].buf,
                                           C6ACCEL_TI_IMG_conv_3x3_i8_c8s_paramPtr->shift);
                     }
                  break;
                  case (CONV_3X3_I16S_C16S_FXN_ID):{
                     /* Unmarshal Parameters */
                     IMG_conv_3x3_i16s_c16s_Params *C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr;
                     C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr->in_data_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr->out_data_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr->mask_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr->pitch)<(C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr->width))|
                        ((C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr->width)< 2)|
                        ((C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr->width)%2 != 0)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                      else
                      /* Call underlying kernel */
                         IMG_conv_3x3_i16s_c16s((short *)inBufs->descs[C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr->in_data_InArrID1].buf,
                                                (short *)outBufs->descs[C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr->out_data_OutArrID1].buf,
                                                C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr->width,
                                                C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr->pitch,
                                                (const short *)inBufs->descs[C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr->mask_InArrID2].buf,
                                                C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_paramPtr->shift);
                  }
                  break;
                  case (CONV_3X3_I8_C8S_FRAME_FXN_ID):{
                     /* Unmarshal Parameters */
                     IMG_conv_3x3_i8_c8s_Frame_Params *C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr;
                     C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr = pFnArray;
                     /*Parameter check*/
                     if(((C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr->in_data_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr->out_data_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr->mask_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr->rows)<= 0)|
                        ((C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr->cols)%8 != 0)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else
                       { // Use Row to loop through image line by line using existing DSP API
                        int r;
                        unsigned char *pInChar=(unsigned char*)inBufs->descs[C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr->in_data_InArrID1].buf;
                        unsigned char *pOutChar=(unsigned char*)outBufs->descs[C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr->out_data_OutArrID1].buf;
                        /* Call underlying kernel */
                        for(r=0;r<C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr->rows;r++){
                            IMG_conv_3x3_i8_c8s(pInChar + r*C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr->cols,
                                                pOutChar + r*C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr->cols,
                                                C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr->cols,
                                               (const char *)inBufs->descs[C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr->mask_InArrID2].buf,
                                               C6ACCEL_TI_IMG_conv_3x3_i8_c8s_Frame_paramPtr->shift);
                         }
                      }
                   }
                  break;
                  case (CONV_3X3_I16S_C16S_FRAME_FXN_ID):{
                     /* Unmarshal Parameters */
                     IMG_conv_3x3_i16s_c16s_Frame_Params *C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr;
                     C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->in_data_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->out_data_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->mask_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->pitch)<(C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->width))|
                        ((C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->rows)<= 0)|
                        ((C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->width)< 2)|
                        ((C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->width)%2 != 0)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else
                       { // Use Row to loop through image line by line using existing DSP API
                        int r;
                        short *pInShort=(short*)inBufs->descs[C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->in_data_InArrID1].buf;
                        short *pOutShort=(short*)outBufs->descs[C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->out_data_OutArrID1].buf;
                        /* Call underlying kernel */
                        for(r=0;r<C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->rows;r++){
                            IMG_conv_3x3_i16s_c16s((short *)pInShort+r*C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->pitch,
                                                  (short *)pOutShort+r*C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->pitch,
                                                  C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->width,
                                                  C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->pitch,
                                                  (const short *)inBufs->descs[C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->mask_InArrID2].buf,
                                                  C6ACCEL_TI_IMG_conv_3x3_i16s_c16s_Frame_paramPtr->shift);
                          }
                       }
                    }
                 break;
                 case (CORR_3X3_I8_C8_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_corr_3x3_i8_c8_Params *C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr;
                    C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr  = pFnArray;
                    /*Parameter check*/
                    if(((C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr->in_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr->mask_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr->out_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr->n_out)%4 != 0)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr->x_dim)%2 != 0)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr->x_dim)<4)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                      IMG_corr_3x3_i8_c8((const unsigned char *)inBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr->in_InArrID1].buf,
                                         (int *)outBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr->out_OutArrID1].buf,
                                         C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr->n_out,
                                         C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr->x_dim,
                                         (const unsigned char *)inBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr->mask_InArrID2].buf,
                                         C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr->shift,
                                         C6ACCEL_TI_IMG_corr_3x3_i8_c8_paramPtr->round);
                  }
                 break;
                 case (CORR_3X3_I16S_C16S_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_corr_3x3_i16s_c16s_Params *C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr;
                    C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr  = pFnArray;
                    /*Parameter check*/
                    if(((C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr->imgin_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr->mask_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr->imgout_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr->pitch)<(C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr->width))|
                       ((C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr->width)%2 != 0)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr->width)<2)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else
                     /* Call underlying kernel */
                        IMG_corr_3x3_i16s_c16s((const short *)inBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr->imgin_InArrID1].buf,
                                               (int *)outBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr->imgout_OutArrID1].buf,
                                               C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr->width,
                                               C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr->pitch,
                                               (const short *)inBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr->mask_InArrID2].buf,
                                               C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr->shift,
                                               C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_paramPtr->round);
                  }
                 break;
                 case (CORR_3X3_I8_C16S_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_corr_3x3_i8_c16s_Params *C6ACCEL_TI_IMG_corr_3x3_i8_c16s_paramPtr;
                    C6ACCEL_TI_IMG_corr_3x3_i8_c16s_paramPtr  = pFnArray;
                    /*Parameter check*/
                    if(((C6ACCEL_TI_IMG_corr_3x3_i8_c16s_paramPtr->imgin_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c16s_paramPtr->mask_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c16s_paramPtr->imgout_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c16s_paramPtr->pitch)<(C6ACCEL_TI_IMG_corr_3x3_i8_c16s_paramPtr->width))|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c16s_paramPtr->width)%2 != 0)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c16s_paramPtr->width)<2)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else
                     /* Call underlying kernel */
                       IMG_corr_3x3_i8_c16s((const unsigned char *)inBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i8_c16s_paramPtr->mask_InArrID2].buf,
                                            (int *)outBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i8_c16s_paramPtr->imgout_OutArrID1].buf,
                                            C6ACCEL_TI_IMG_corr_3x3_i8_c16s_paramPtr->width,
                                            C6ACCEL_TI_IMG_corr_3x3_i8_c16s_paramPtr->pitch,
                                            (const short *)inBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i8_c16s_paramPtr->mask_InArrID2].buf);
                  }
                 break;
                 case (CORR_3X3_I8_C8_FRAME_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_corr_3x3_i8_c8_Frame_Params *C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr;
                    C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr  = pFnArray;
                    /*Parameter check*/
                    if(((C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->in_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->mask_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->out_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->n_out)%4 != 0)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->rows) <= 0)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->x_dim)%2 != 0)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->x_dim)<4)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else{ // Use Row to loop through image line by line using existing DSP API
                          int r;
                          unsigned char *pInChar=(unsigned char*)inBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->in_InArrID1].buf;
                          int *pOutInt          =(int*)outBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->out_OutArrID1].buf;
                          /* Call underlying kernel */
                          for(r=0;r<C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->rows;r++){

                             IMG_corr_3x3_i8_c8(pInChar + r*C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->x_dim,
                                                pOutInt + r*C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->x_dim,
                                                C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->n_out,
                                                C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->x_dim,
                                                (const unsigned char *)inBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->mask_InArrID2].buf,
                                                C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->shift,
                                                C6ACCEL_TI_IMG_corr_3x3_i8_c8_Frame_paramPtr->round);
                          }
                      }
                  }
                 break;
                 case (CORR_3X3_I16S_C16S_FRAME_FXN_ID):{
                     /* Unmarshal Parameters */
                     IMG_corr_3x3_i16s_c16s_Frame_Params *C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr;
                     C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr  = pFnArray;
                     /*Parameter check*/
                     if(((C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->imgin_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->mask_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->imgout_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->pitch)<(C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->width))|
                        ((C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->rows) <= 0)|
                        ((C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->width)%2 != 0)|
                        ((C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->width)<2)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                      else{ // Use Row to loop through image line by line using existing DSP API
                           int r;
                           short *pInShort=(short*)inBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->imgin_InArrID1].buf;
                           int *pOutInt          =(int*)outBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->imgout_OutArrID1].buf;
                           /* Call underlying kernel */
                           for(r=0;r<C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->rows;r++){
                               IMG_corr_3x3_i16s_c16s(pInShort + r * C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->pitch,
                                                     (int *)pOutInt + r * C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->pitch,
                                                     C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->width,
                                                     C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->pitch,
                                                     (const short *)inBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->mask_InArrID2].buf,
                                                     C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->shift,
                                                     C6ACCEL_TI_IMG_corr_3x3_i16s_c16s_Frame_paramPtr->round);
                            }
                       }
                   }
                 break;
                 case (CORR_3X3_I8_C16S_FRAME_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_corr_3x3_i8_c16s_Frame_Params *C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr;
                    C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr  = pFnArray;
                    /*Parameter check*/
                    if(((C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->imgin_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->mask_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->imgout_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->pitch)<(C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->width))|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->width)%2 != 0)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->rows) <= 0)|
                       ((C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->width)<2)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else{ // Use Row to loop through image line by line using existing DSP API
                          int r;
                          unsigned char *pInChar=(unsigned char*)inBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->mask_InArrID2].buf;
                          int *pOutInt          =(int*)outBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->imgout_OutArrID1].buf;
                          /* Call underlying kernel */
                          for(r=0;r<C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->rows;r++){
                              IMG_corr_3x3_i8_c16s(pInChar + r*C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->pitch,
                                                   pOutInt + r*C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->pitch,
                                                   C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->width,
                                                   C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->pitch,
                                                   (const short *)inBufs->descs[C6ACCEL_TI_IMG_corr_3x3_i8_c16s_Frame_paramPtr->mask_InArrID2].buf);
                            }
                       }
                   }
                 break;
                 case (MULS_8_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_mulS_8_Params *C6ACCEL_TI_IMG_mulS_8_paramPtr;
                    C6ACCEL_TI_IMG_mulS_8_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_mulS_8_paramPtr->imgR_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_mulS_8_paramPtr->imgW_OutArrID1)>INBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       IMG_mulS_8((unsigned char *)inBufs->descs[C6ACCEL_TI_IMG_mulS_8_paramPtr->imgR_InArrID1].buf,
                                  (short *)outBufs->descs[C6ACCEL_TI_IMG_mulS_8_paramPtr->imgW_OutArrID1].buf,
                                   C6ACCEL_TI_IMG_mulS_8_paramPtr->constData,
                                   C6ACCEL_TI_IMG_mulS_8_paramPtr->count);
                     }
                 break;
                 case (MULS_16S_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_mulS_16s_Params *C6ACCEL_TI_IMG_mulS_16s_paramPtr;
                    C6ACCEL_TI_IMG_mulS_16s_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_mulS_16s_paramPtr->imgR_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_mulS_16s_paramPtr->imgW_OutArrID1)>INBUF15)){
                       return(IUNIVERSAL_EPARAMFAIL);
                    }
                    else
                    /* Call underlying kernel */
                      IMG_mulS_16s((short *)inBufs->descs[C6ACCEL_TI_IMG_mulS_16s_paramPtr->imgR_InArrID1].buf,
                                   (int *)outBufs->descs[C6ACCEL_TI_IMG_mulS_16s_paramPtr->imgW_OutArrID1].buf,
                                   (short)C6ACCEL_TI_IMG_mulS_16s_paramPtr->constData,
                                   C6ACCEL_TI_IMG_mulS_16s_paramPtr->count);
                    }
                 break;
                 case (ADDS_8_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_addS_8_Params *C6ACCEL_TI_IMG_addS_8_paramPtr;
                    C6ACCEL_TI_IMG_addS_8_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_addS_8_paramPtr->imgR_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_addS_8_paramPtr->imgW_OutArrID1)>INBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       IMG_addS_8((char *)inBufs->descs[C6ACCEL_TI_IMG_addS_8_paramPtr->imgR_InArrID1].buf,
                                  (char *)outBufs->descs[C6ACCEL_TI_IMG_addS_8_paramPtr->imgW_OutArrID1].buf,
                                  C6ACCEL_TI_IMG_addS_8_paramPtr->constData,
                                  C6ACCEL_TI_IMG_addS_8_paramPtr->count);
                    }
                 break;
                 case (ADDS_16S_FXN_ID):{ 
                    /* Unmarshal Parameters */   
                    IMG_addS_16s_Params *C6ACCEL_TI_IMG_addS_16s_paramPtr;
                    C6ACCEL_TI_IMG_addS_16s_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_addS_16s_paramPtr->imgR_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_addS_16s_paramPtr->imgW_OutArrID1)>INBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       IMG_addS_16s((short *)inBufs->descs[C6ACCEL_TI_IMG_addS_16s_paramPtr->imgR_InArrID1].buf,
                                    (short *)outBufs->descs[C6ACCEL_TI_IMG_addS_16s_paramPtr->imgW_OutArrID1].buf,
                                    C6ACCEL_TI_IMG_addS_16s_paramPtr->constData,
                                    C6ACCEL_TI_IMG_addS_16s_paramPtr->count);
                    }
                 break;
                 case (SUBS_8_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_subS_8_Params *C6ACCEL_TI_IMG_subS_8_paramPtr;
                    C6ACCEL_TI_IMG_subS_8_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_subS_8_paramPtr->imgR_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_subS_8_paramPtr->imgW_OutArrID1)>INBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                      IMG_subS_8((char *)inBufs->descs[C6ACCEL_TI_IMG_subS_8_paramPtr->imgR_InArrID1].buf,
                                 (char *)outBufs->descs[C6ACCEL_TI_IMG_subS_8_paramPtr->imgW_OutArrID1].buf,
                                  C6ACCEL_TI_IMG_subS_8_paramPtr->constData,
                                  C6ACCEL_TI_IMG_subS_8_paramPtr->count);
                    }
                 break;
                 case (SUBS_16S_FXN_ID):{ 
                    /* Unmarshal Parameters */   
                    IMG_subS_16s_Params *C6ACCEL_TI_IMG_subS_16s_paramPtr;
                    C6ACCEL_TI_IMG_subS_16s_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_subS_16s_paramPtr->imgR_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_subS_16s_paramPtr->imgW_OutArrID1)>INBUF15)){
                       return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       IMG_subS_16s((short *)inBufs->descs[C6ACCEL_TI_IMG_subS_16s_paramPtr->imgR_InArrID1].buf,
                                    (short *)outBufs->descs[C6ACCEL_TI_IMG_subS_16s_paramPtr->imgW_OutArrID1].buf,
                                    C6ACCEL_TI_IMG_subS_16s_paramPtr->constData,
                                    C6ACCEL_TI_IMG_subS_16s_paramPtr->count);
                     }
                 break;
                 case (YC_DEMUX_LE16_16_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_yc_demux_le16_16_Params *C6ACCEL_TI_IMG_yc_demux_le16_16_paramPtr;
                    C6ACCEL_TI_IMG_yc_demux_le16_16_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_yc_demux_le16_16_paramPtr->yc_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_yc_demux_le16_16_paramPtr->y_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_yc_demux_le16_16_paramPtr->cr_OutArrID2)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_yc_demux_le16_16_paramPtr->cb_OutArrID3)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_yc_demux_le16_16_paramPtr->n)%16 !=0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else
                     /* Call underlying kernel */
                        IMG_yc_demux_le16_16(C6ACCEL_TI_IMG_yc_demux_le16_16_paramPtr->n,
                                            (const unsigned short *)inBufs->descs[C6ACCEL_TI_IMG_yc_demux_le16_16_paramPtr->yc_InArrID1].buf,
                                            (short *) outBufs->descs[C6ACCEL_TI_IMG_yc_demux_le16_16_paramPtr->y_OutArrID1].buf,
                                            (short *) outBufs->descs[C6ACCEL_TI_IMG_yc_demux_le16_16_paramPtr->cr_OutArrID2].buf,
                                            (short *) outBufs->descs[C6ACCEL_TI_IMG_yc_demux_le16_16_paramPtr->cb_OutArrID3].buf);
                    }
                 break;
                 case (YC_DEMUX_LE8_C_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_yc_demux_le8_c_Params *C6ACCEL_TI_IMG_yc_demux_le8_c_paramPtr;
                    C6ACCEL_TI_IMG_yc_demux_le8_c_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_yc_demux_le8_c_paramPtr->yc_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_yc_demux_le8_c_paramPtr->y_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_yc_demux_le8_c_paramPtr->cr_OutArrID2)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_yc_demux_le8_c_paramPtr->cb_OutArrID3)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_yc_demux_le8_c_paramPtr->n)%16 !=0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       IMG_yc_demux_le8_c(C6ACCEL_TI_IMG_yc_demux_le8_c_paramPtr->n,
                                          (const unsigned char *)inBufs->descs[C6ACCEL_TI_IMG_yc_demux_le8_c_paramPtr->yc_InArrID1].buf,
                                          (unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_yc_demux_le8_c_paramPtr->y_OutArrID1].buf,
                                          (unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_yc_demux_le8_c_paramPtr->cr_OutArrID2].buf,
                                          (unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_yc_demux_le8_c_paramPtr->cb_OutArrID3].buf);
                    }
                 break;
                 case (YCBCR422PL_RGB565_FXN_ID):{
                     /* Unmarshal Parameters */
                     IMG_ycbcr422pl_to_rgb565_Params   *C6ACCEL_TI_IMG_ycbcr422pl_to_rgb565_paramPtr;
                     C6ACCEL_TI_IMG_ycbcr422pl_to_rgb565_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_IMG_ycbcr422pl_to_rgb565_paramPtr->coeff_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_IMG_ycbcr422pl_to_rgb565_paramPtr->y_data_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_IMG_ycbcr422pl_to_rgb565_paramPtr->cb_data_InArrID3)>INBUF15)|
                        ((C6ACCEL_TI_IMG_ycbcr422pl_to_rgb565_paramPtr->cr_data_InArrID4)>INBUF15)|
                        ((C6ACCEL_TI_IMG_ycbcr422pl_to_rgb565_paramPtr->rgb_data_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_IMG_ycbcr422pl_to_rgb565_paramPtr->num_pixels)%8 != 0)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                      else
                      /* Call underlying kernel */
                         IMG_ycbcr422pl_to_rgb565((const short *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl_to_rgb565_paramPtr->coeff_InArrID1].buf,
                                                  (const unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl_to_rgb565_paramPtr->y_data_InArrID2].buf,
                                                  (const unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl_to_rgb565_paramPtr->cb_data_InArrID3].buf,
                                                  (const unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl_to_rgb565_paramPtr->cr_data_InArrID4].buf,
                                                  (unsigned short *) outBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl_to_rgb565_paramPtr->rgb_data_OutArrID1].buf,
                                                  C6ACCEL_TI_IMG_ycbcr422pl_to_rgb565_paramPtr->num_pixels);
                    }
                 break;
                 case (YCBCR422PL16_RGB565_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_ycbcr422pl16_to_rgb565_Params *C6ACCEL_TI_IMG_ycbcr422pl16_to_rgb565_paramPtr;
                    C6ACCEL_TI_IMG_ycbcr422pl16_to_rgb565_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_ycbcr422pl16_to_rgb565_paramPtr->coeff_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422pl16_to_rgb565_paramPtr->y_data_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422pl16_to_rgb565_paramPtr->cb_data_InArrID3)>INBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422pl16_to_rgb565_paramPtr->cr_data_InArrID4)>INBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422pl16_to_rgb565_paramPtr->rgb_data_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422pl16_to_rgb565_paramPtr->num_pixels)%8 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else
                     /* Call underlying kernel */
                        IMG_ycbcr422pl16_to_rgb565_c((const short *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl16_to_rgb565_paramPtr->coeff_InArrID1].buf,
                                                     (short *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl16_to_rgb565_paramPtr->y_data_InArrID2].buf,
                                                     (short *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl16_to_rgb565_paramPtr->cb_data_InArrID3].buf,
                                                     (short *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl16_to_rgb565_paramPtr->cr_data_InArrID4].buf,
                                                     (unsigned short *) outBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl16_to_rgb565_paramPtr->rgb_data_OutArrID1].buf,
                                                     C6ACCEL_TI_IMG_ycbcr422pl16_to_rgb565_paramPtr->num_pixels);
                    }
                 break;
                 case (YUV420PL_TO_RGB565_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_yuv420pl_to_rgb565_Params  *C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr;
                    C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr->coeff_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr->y_data_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr->cb_data_InArrID3)>INBUF15)|
                       ((C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr->cr_data_InArrID4)>INBUF15)|
                       ((C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr->rgb_data_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr->Height)%2 != 0)|
                       ((C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr->Width)%8 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else
                     /* Call underlying kernel */
                        IMG_yuv420pl_to_rgb565((const short *) inBufs->descs[C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr->coeff_InArrID1].buf,
                                               C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr->Height,
                                               C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr->Width,
                                               (unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr->y_data_InArrID2].buf,
                                               (unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr->cb_data_InArrID3].buf,
                                               (unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr->cr_data_InArrID4].buf,
                                               (unsigned short *) outBufs->descs[C6ACCEL_TI_IMG_yuv420pl_to_rgb565_paramPtr->rgb_data_OutArrID1].buf);
                   }
                 break;
                 case (YUV420PL16_TO_RGB565_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_yuv420pl16_to_rgb565_Params *C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr;
                    C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr->coeff_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr->y_data_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr->cb_data_InArrID3)>INBUF15)|
                       ((C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr->cr_data_InArrID4)>INBUF15)|
                       ((C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr->rgb_data_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr->Height)%2 != 0)|
                       ((C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr->Width)%2 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else
                     /* Call underlying kernel */
                        IMG_yuv420pl16_to_rgb565((const short *) inBufs->descs[C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr->coeff_InArrID1].buf,
                                                  C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr->Height,
                                                  C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr->Width,
                                                  (short *) inBufs->descs[C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr->y_data_InArrID2].buf,
                                                  (short *) inBufs->descs[C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr->cb_data_InArrID3].buf,
                                                  (short *) inBufs->descs[C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr->cr_data_InArrID4].buf,
                                                  (unsigned short *) outBufs->descs[C6ACCEL_TI_IMG_yuv420pl16_to_rgb565_paramPtr->rgb_data_OutArrID1].buf);
                  }
                 break;
                 case (YCBCR422SP_TO_YCBCR420PL_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_ycbcr422sp_to_ycbcr420pl_Params  *C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr;
                    C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->y_src_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->cbcr_src_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->y_dst_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->cb_dst_OutArrID2)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->cr_dst_OutArrID3)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->width)%16 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else
                     /* Call underlying kernel */
                        ycbcr422sp_to_ycbcr420pl((const unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->y_src_InArrID1].buf,
                                                 (const unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->cbcr_src_InArrID2].buf,
                                                 (unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->y_dst_OutArrID1].buf,
                                                 (unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->cb_dst_OutArrID2].buf,
                                                 (unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->cr_dst_OutArrID3].buf,
                                                 C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->num_lines,
                                                 C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->width,
                                                 C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->src_pitch,
                                                 C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->dst_y_pitch,
                                                 C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr420pl_paramPtr->dst_cbcr_pitch);
                                           
                                               
                  }
                 break;
                 case (YCBCR422PL_TO_YCBCR422SP_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_ycbcr422pl_to_ycbcr422sp_Params  *C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr;
                    C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->y_src_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->cr_src_InArrID3)>INBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->cb_src_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->y_dst_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->cbcr_dst_OutArrID2)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->width)%16 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else
                     /* Call underlying kernel */
                        ycbcr422pl_to_ycbcr422sp((unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->y_dst_OutArrID1].buf,
                                                 (unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->cbcr_dst_OutArrID2].buf,
                                                 (unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->y_src_InArrID1].buf,
                                                 (unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->cb_src_InArrID2].buf,
                                                 (unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->cr_src_InArrID3].buf,
                                                 C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->num_lines,
                                                 C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->width,
                                                 C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->dst_pitch,
                                                 C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->src_y_pitch,
                                                 C6ACCEL_TI_IMG_ycbcr422pl_to_ycbcr422sp_paramPtr->src_cbcr_pitch);
                                           
                                               
                  }
                 break;
                 case (YCBCR420PL_TO_YCBCR422SP_FXN_ID):
				 {
                    /* Unmarshal Parameters */
                    IMG_ycbcr420pl_to_ycbcr422sp_Params  *C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr;
                    C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->y_src_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->cr_src_InArrID3)>INBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->cb_src_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->y_dst_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->cbcr_dst_OutArrID2)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->width)%16 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else
                     /* Call underlying kernel */
                        ycbcr420pl_to_ycbcr422sp((unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->y_dst_OutArrID1].buf,
                                                 (unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->cbcr_dst_OutArrID2].buf,
                                                 (unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->y_src_InArrID1].buf,
                                                 (unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->cb_src_InArrID2].buf,
                                                 (unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->cr_src_InArrID3].buf,
                                                 C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->num_lines,
                                                 C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->width,
                                                 C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->dst_pitch,
                                                 C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->src_y_pitch,
                                                 C6ACCEL_TI_IMG_ycbcr420pl_to_ycbcr422sp_paramPtr->src_cbcr_pitch);
                                           
                                               
                  }
                 break;
                 case (YCBCR422SP_TO_YCBCR422ILE_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_ycbcr422sp_to_ycbcr422ile_Params  *C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr422ile_paramPtr;
                    C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr422ile_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr422ile_paramPtr->y_src_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr422ile_paramPtr->cbcr_src_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr422ile_paramPtr->ycbcr_dst_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr422ile_paramPtr->width)%16 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else
                     /* Call underlying kernel */
                        ycbcr422sp_to_ycbcr422ile((unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr422ile_paramPtr->y_src_InArrID1].buf,
                                                 (unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr422ile_paramPtr->cbcr_src_InArrID2].buf,
                                                 (unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr422ile_paramPtr->ycbcr_dst_OutArrID1].buf,
                                                 C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr422ile_paramPtr->num_lines,
                                                 C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr422ile_paramPtr->width,
                                                 C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr422ile_paramPtr->src_pitch,
                                                 C6ACCEL_TI_IMG_ycbcr422sp_to_ycbcr422ile_paramPtr->dst_ycbcr_pitch);
                                           
                                               
                  }
                 break;
                 case(RGB_Y_FXN_ID):
		 {
		  /* Unmarshal Parameters */
                  IMG_RGB_To_Y_Params  *C6ACCEL_TI_IMG_RGB_To_Y_paramPtr;
                  C6ACCEL_TI_IMG_RGB_To_Y_paramPtr = pFnArray;
		  if(((C6ACCEL_TI_IMG_RGB_To_Y_paramPtr->src_InArrID1)>INBUF15)|
                     ((C6ACCEL_TI_IMG_RGB_To_Y_paramPtr->dst_OutArrID1)>OUTBUF15)|
                     ((C6ACCEL_TI_IMG_RGB_To_Y_paramPtr->count)%8 != 0)){
		       return(IUNIVERSAL_EPARAMFAIL);
                   }
                  else
		  /* Call underlying kernel */
                  RGB_To_Y((unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_RGB_To_Y_paramPtr->src_InArrID1].buf,
		            (unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_RGB_To_Y_paramPtr->dst_OutArrID1].buf,		
		             C6ACCEL_TI_IMG_RGB_To_Y_paramPtr->count);
		 }
		 break;
		 case(YC_DEMUX_BE8_FXN_ID):
		 {
		  /* Unmarshal Parameters */
                  IMG_yc_demux_be8_Params  *C6ACCEL_TI_IMG_yc_demux_be8_paramPtr;
                  C6ACCEL_TI_IMG_yc_demux_be8_paramPtr = pFnArray;
		  if(((C6ACCEL_TI_IMG_yc_demux_be8_paramPtr->yc_InArrID1)>INBUF15)|
                     ((C6ACCEL_TI_IMG_yc_demux_be8_paramPtr->y_OutArrID1)>OUTBUF15)|
		     ((C6ACCEL_TI_IMG_yc_demux_be8_paramPtr->cb_OutArrID2)>OUTBUF15)|
		     ((C6ACCEL_TI_IMG_yc_demux_be8_paramPtr->cr_OutArrID3)>OUTBUF15)|
                     ((C6ACCEL_TI_IMG_yc_demux_be8_paramPtr->n)%8 != 0)){
		       return(IUNIVERSAL_EPARAMFAIL);
                   }
                   else
		     /* Call underlying kernel */
                      yc_demux_be8_c(C6ACCEL_TI_IMG_yc_demux_be8_paramPtr->n,
                                     (unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_yc_demux_be8_paramPtr->yc_InArrID1].buf,
		                     (unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_yc_demux_be8_paramPtr->y_OutArrID1].buf,		
	                             (unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_yc_demux_be8_paramPtr->cb_OutArrID2].buf,
		                     (unsigned char *) outBufs->descs[C6ACCEL_TI_IMG_yc_demux_be8_paramPtr->cr_OutArrID3].buf);
                   }
		 break;
                 case(PIX_EXPAND_FXN_ID):
		 {
		  /* Unmarshal Parameters */
                  IMG_pix_expand_Params  *C6ACCEL_TI_IMG_pix_expand_paramPtr;
                  C6ACCEL_TI_IMG_pix_expand_paramPtr = pFnArray;
		  if(((C6ACCEL_TI_IMG_pix_expand_paramPtr->indata_InArrID1)>INBUF15)|
                     ((C6ACCEL_TI_IMG_pix_expand_paramPtr->outdata_OutArrID1)>OUTBUF15)|
		     ((C6ACCEL_TI_IMG_pix_expand_paramPtr->n)%16 != 0)){
		       return(IUNIVERSAL_EPARAMFAIL);
                   }
                   else
		     /* Call underlying kernel */
                      IMG_pix_expand(C6ACCEL_TI_IMG_pix_expand_paramPtr->n,
                                     (unsigned char *) inBufs->descs[C6ACCEL_TI_IMG_pix_expand_paramPtr->indata_InArrID1].buf,
		                     (short *) outBufs->descs[C6ACCEL_TI_IMG_pix_expand_paramPtr->outdata_OutArrID1].buf		
	                             );
                   }
		 break;
                 case (CONV_7X7_I8_C8S_FXN_ID):{//ESSAI NICO
                    /* Unmarshal Parameters */
                    IMG_conv_7x7_i8_c8s_Params *C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr;
                    C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr = pFnArray;
                    /*Parameter check*/
                    if(((C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr->indata_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr->indata_InArrID2)>INBUF15)|
                       //((C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr->width)>(C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr->pitch))|
                       ((C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr->width)<2)|
                       ((C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr->width)%2 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       IMG_conv_7x7_i8_c8s((unsigned char *)inBufs->descs[C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr->indata_InArrID1].buf,
                                           (unsigned char *)outBufs->descs[C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr->outdata_OutArrID1].buf,
                                           (short)C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr->width,
                                           (short)C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr->pitch,
                                           (const char *)inBufs->descs[C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr->indata_InArrID2].buf,
                                           (short)C6ACCEL_TI_IMG_conv_7x7_i8_c8s_paramPtr->shift);
                     }
		  break;
                  case (CONV_7X7_I8_C8S_FRAME_FXN_ID):{//ESSAI NICO
                    /* Unmarshal Parameters */
                    IMG_conv_7x7_i8_c8s_frame_Params *C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr;
                    C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr = pFnArray;
                    /*Parameter check*/
                   if(((C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr->indata_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr->indata_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr->width)<2)|
                       ((C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr->width)%2 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
  				int i;
				char size_conv = 7;
				const unsigned char *pInChar=(unsigned char*)inBufs->descs[C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr->indata_InArrID1].buf;
                     		unsigned char *pOutChar=(unsigned char*)outBufs->descs[C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr->outdata_OutArrID1].buf;
				for(i=0;i<C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr->height;i++){
                    /* Call underlying kernel */
                       IMG_conv_7x7_i8_c8s(pInChar+i*C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr->width,
                                           pOutChar+i*(C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr->width)+((size_conv-1)/2),
                                          (C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr->width ) - ((size_conv-1)/2),
                                           C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr->width,
                                           (const char *)inBufs->descs[C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr->indata_InArrID2].buf,
                                           C6ACCEL_TI_IMG_conv_7x7_i8_c8s_frame_paramPtr->shift);
				}
			}
                     }
                  break;
                  //Function added by Nicolas G
                case (CONV_11X11_I8_C8S_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_conv_11x11_i8_c8s_Params *C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr;
                    C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr = pFnArray;
                    /*Parameter check*/
                    if(((C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr->indata_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr->indata_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr->width)>(C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr->pitch))|
                       ((C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr->width)<2)|
                       ((C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr->width)%2 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       IMG_conv_11x11_i8_c8s((unsigned char *)inBufs->descs[C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr->indata_InArrID1].buf,
                                           (unsigned char *)outBufs->descs[C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr->outdata_OutArrID1].buf,
                                           C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr->width,
                                           C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr->pitch,
                                           (const char *)inBufs->descs[C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr->indata_InArrID2].buf,
                                           C6ACCEL_TI_IMG_conv_11x11_i8_c8s_paramPtr->shift);
                     }

		break;
                case (CONV_11X11_I8_C8S_FRAME_FXN_ID):{
                    /* Unmarshal Parameters */
                    IMG_conv_11x11_i8_c8s_frame_Params *C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr;
                    C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr = pFnArray;
                    /*Parameter check*/
                     if(((C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr->indata_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                       ((C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr->indata_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr->width)<2)|
                       ((C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr->width)%2 != 0)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
  				int i;
				char size_conv = 11;
				unsigned char *pInChar=(unsigned char*)inBufs->descs[C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr->indata_InArrID1].buf;
                     		unsigned char *pOutChar=(unsigned char*)outBufs->descs[C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr->outdata_OutArrID1].buf;
				for(i=0;i<C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr->height;i++){
                   			 /* Call underlying kernel */
                      			 IMG_conv_11x11_i8_c8s(pInChar+i*C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr->width,
		                                   pOutChar+i*(C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr->width)+((size_conv-1)/2),
		                                  ( C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr->width ) - ((size_conv-1)/2),
		                                   C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr->width,
		                                   (const char *)inBufs->descs[C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr->indata_InArrID2].buf,
		                                   C6ACCEL_TI_IMG_conv_11x11_i8_c8s_frame_paramPtr->shift);
				}
			}
                     }

                  break;
                 //Function added by Nicolas G
		case (THR_GT2THR_8_FXN_ID ):{
                    /* Unmarshal Parameters */
                    IMG_thr_gt2thr_8_Params *C6ACCEL_TI_IMG_thr_gt2thr_8_paramPtr;
                    C6ACCEL_TI_IMG_thr_gt2thr_8_paramPtr = pFnArray;
                    /* Parameter check */
                    if (((C6ACCEL_TI_IMG_thr_gt2thr_8_paramPtr->indata_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_IMG_thr_gt2thr_8_paramPtr->outdata_OutArrID1)>OUTBUF15)|
                        ((C6ACCEL_TI_IMG_thr_gt2thr_8_paramPtr->Threshold)>255)|
                        (((C6ACCEL_TI_IMG_thr_gt2thr_8_paramPtr->Col)*(C6ACCEL_TI_IMG_thr_gt2thr_8_paramPtr->Row))%16 !=0)){
                          return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else
                    /* Call underlying kernel */
                       IMG_thr_gt2thr_8((const unsigned char *)inBufs->descs[C6ACCEL_TI_IMG_thr_gt2thr_8_paramPtr->indata_InArrID1].buf,
                                     	  (unsigned char *)outBufs->descs[C6ACCEL_TI_IMG_thr_gt2thr_8_paramPtr->outdata_OutArrID1].buf,
                                           C6ACCEL_TI_IMG_thr_gt2thr_8_paramPtr->Col,
                                           C6ACCEL_TI_IMG_thr_gt2thr_8_paramPtr->Row,
                                           C6ACCEL_TI_IMG_thr_gt2thr_8_paramPtr->Threshold);
                    }
                 break;
                 default:
                    /*Error caused due to passing of an invalid ID*/
                         return(IUNIVERSAL_EFXNIDFAIL);
          }

    return 0;
}

/*=============================================================================*/
/*                  End of File C6accel_ti_imglibFunctionCall.c                */
/*                              Version : 0.0.1                                */
/*=============================================================================*/
