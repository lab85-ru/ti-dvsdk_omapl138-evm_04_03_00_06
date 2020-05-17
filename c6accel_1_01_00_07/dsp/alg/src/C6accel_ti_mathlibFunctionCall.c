/*==================================================================== */
/*  Copyright (c) 2010, Texas Instruments Incorporated                 */
/*  All rights reserved.                                               */
/*                                                                     */
/*                                                                     */
/* ======== C6ACCEL_TI_mathlibFunctionCall.c ========                  */
/* This file is used to invoke MATHLib kernels based on the function ID*/
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

int C6ACCEL_TI_mathlibFunctionCall(void *pFnArray,int fxnID,XDM1_BufDesc *inBufs, XDM1_BufDesc *outBufs)
{
 int j;
 Int32 fxnidMsk= FXN_ID_MASK;
         /* MATHLIB functions */
           switch ( fxnID & fxnidMsk){
                 case(IQNMPY_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *inArrRdr2, *outArrRdr;
                    IQNmpy_Params  *C6ACCEL_TI_IQNmpy_paramPtr;
                    C6ACCEL_TI_IQNmpy_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNmpy_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNmpy_paramPtr->InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IQNmpy_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNmpy_paramPtr->InArrID1].buf;
                         inArrRdr2 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNmpy_paramPtr->InArrID2].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNmpy_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNmpy_paramPtr-> n); j++){
                             outArrRdr[j] = _IQNmpy(inArrRdr1[j], inArrRdr2[j], C6ACCEL_TI_IQNmpy_paramPtr->qfmt);
                          }
                       }
                    }
                 break;
                 case(IQNSQRT_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *outArrRdr;
                    IQNsqrt_Params  *C6ACCEL_TI_IQNsqrt_paramPtr;
                    C6ACCEL_TI_IQNsqrt_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNsqrt_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNsqrt_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                        inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNsqrt_paramPtr->InArrID1].buf;
                        outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNsqrt_paramPtr->OutArrID1].buf;
                        for(j=0 ; j < (C6ACCEL_TI_IQNsqrt_paramPtr-> n); j++){
                            outArrRdr[j] = _IQNsqrt(inArrRdr1[j], C6ACCEL_TI_IQNsqrt_paramPtr->qfmt);
                         }
                     }
                  }
                 break;
                 case(IQNSIN_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *outArrRdr;
                    IQNsin_Params  *C6ACCEL_TI_IQNsin_paramPtr;
                    C6ACCEL_TI_IQNsin_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNsin_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNsin_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNsin_paramPtr->InArrID1].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNsin_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNsin_paramPtr-> n); j++){
                             outArrRdr[j] = _IQNsin(inArrRdr1[j], C6ACCEL_TI_IQNsin_paramPtr->qfmt);
                           }
                       }
                   }
                 break;
                 case(IQNCOS_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *outArrRdr;
                    IQNcos_Params   *C6ACCEL_TI_IQNcos_paramPtr;
                    C6ACCEL_TI_IQNcos_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNcos_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNcos_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNcos_paramPtr->InArrID1].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNcos_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNcos_paramPtr-> n); j++){
                             outArrRdr[j] = _IQNcos(inArrRdr1[j], C6ACCEL_TI_IQNcos_paramPtr->qfmt);
                          }
                      }
                  }
                 break;
                 case(IQNDIV_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *inArrRdr2, *outArrRdr;
                    IQNdiv_Params  *C6ACCEL_TI_IQNdiv_paramPtr;
                    C6ACCEL_TI_IQNdiv_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNdiv_paramPtr->num_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNdiv_paramPtr->den_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IQNdiv_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNdiv_paramPtr->num_InArrID1].buf;
                         inArrRdr2 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNdiv_paramPtr->den_InArrID2].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNdiv_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNdiv_paramPtr->n); j++){
                             outArrRdr[j] = _IQNdiv(inArrRdr1[j], inArrRdr2[j], C6ACCEL_TI_IQNdiv_paramPtr->qfmt);
                          }
                      }
                  }
                 break;
                 case(IQNATAN2_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *inArrRdr2, *outArrRdr;
                    IQNatan2_Params *C6ACCEL_TI_IQNatan2_paramPtr;
                    C6ACCEL_TI_IQNatan2_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNatan2_paramPtr->y_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNatan2_paramPtr->x_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IQNatan2_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNatan2_paramPtr->y_InArrID1].buf;
                         inArrRdr2 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNatan2_paramPtr->x_InArrID2].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNatan2_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNatan2_paramPtr->n); j++){
                             outArrRdr[j] = _IQNatan2(inArrRdr1[j], inArrRdr2[j], C6ACCEL_TI_IQNatan2_paramPtr->qfmt);
                          }
                      }
                  }
                 break;
                 case(IQNEXP_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *outArrRdr;
                    IQNexp_Params  *C6ACCEL_TI_IQNexp_paramPtr;
                    C6ACCEL_TI_IQNexp_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNexp_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNexp_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNexp_paramPtr->InArrID1].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNexp_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNexp_paramPtr-> n); j++){
                             outArrRdr[j] = _IQNexp(inArrRdr1[j], C6ACCEL_TI_IQNexp_paramPtr->qfmt);
                           }
                       }
                   }
                 break;
                 case(IQNLOG_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *outArrRdr;
                    IQNlog_Params *C6ACCEL_TI_IQNlog_paramPtr;
                    C6ACCEL_TI_IQNlog_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNlog_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNlog_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNlog_paramPtr->InArrID1].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNlog_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNlog_paramPtr-> n); j++){
                             outArrRdr[j] = _IQNlog(inArrRdr1[j], C6ACCEL_TI_IQNlog_paramPtr->qfmt);
                          }
                      }
                  }
                 break;
                 case(IQNABS_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *outArrRdr;
                    IQNabs_Params *C6ACCEL_TI_IQNabs_paramPtr;
                    C6ACCEL_TI_IQNabs_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNabs_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNabs_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNabs_paramPtr->InArrID1].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNabs_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNabs_paramPtr-> n); j++){
                              outArrRdr[j] = _IQNabs(inArrRdr1[j]);
                          }
                      }
                  }
                 break;
                 case(IQNRMPY_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *inArrRdr2, *outArrRdr;
                    IQNrmpy_Params *C6ACCEL_TI_IQNrmpy_paramPtr;
                    C6ACCEL_TI_IQNrmpy_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNrmpy_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNrmpy_paramPtr->InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IQNrmpy_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNrmpy_paramPtr->InArrID1].buf;
                         inArrRdr2 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNrmpy_paramPtr->InArrID2].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNrmpy_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNrmpy_paramPtr-> n); j++){
                             outArrRdr[j] = _IQNrmpy(inArrRdr1[j], inArrRdr2[j], C6ACCEL_TI_IQNrmpy_paramPtr->qfmt);
                           }
                       }
                  }
                 break;
                 case(IQNRSMPY_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *inArrRdr2, *outArrRdr;
                    IQNrsmpy_Params *C6ACCEL_TI_IQNrsmpy_paramPtr;
                    C6ACCEL_TI_IQNrsmpy_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNrsmpy_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNrsmpy_paramPtr->InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IQNrsmpy_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNrsmpy_paramPtr->InArrID1].buf;
                         inArrRdr2 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNrsmpy_paramPtr->InArrID2].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNrsmpy_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNrsmpy_paramPtr-> n); j++){
                              outArrRdr[j] = _IQNrsmpy(inArrRdr1[j], inArrRdr2[j], C6ACCEL_TI_IQNrsmpy_paramPtr->qfmt);
                          }
                      }
                  }
                 break;
                 case(IQNASIN_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *outArrRdr;
                    IQNasin_Params *C6ACCEL_TI_IQNasin_paramPtr;
                    C6ACCEL_TI_IQNasin_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNasin_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNasin_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNasin_paramPtr->InArrID1].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNasin_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNasin_paramPtr-> n); j++){
                             outArrRdr[j] = _IQNasin(inArrRdr1[j], C6ACCEL_TI_IQNasin_paramPtr->qfmt);
                          }
                      }
                  }
                 break;
                 case(IQNACOS_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *outArrRdr;
                    IQNacos_Params  *C6ACCEL_TI_IQNacos_paramPtr;
                    C6ACCEL_TI_IQNacos_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNacos_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNacos_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNacos_paramPtr->InArrID1].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNacos_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNacos_paramPtr-> n); j++){
                             outArrRdr[j] = _IQNacos(inArrRdr1[j], C6ACCEL_TI_IQNacos_paramPtr->qfmt);
                          }
                      }
                  }
                 break;
                 case(IQNSINPU_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *outArrRdr;
                    IQNsinPU_Params *C6ACCEL_TI_IQNsinPU_paramPtr;
                    C6ACCEL_TI_IQNsinPU_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNsinPU_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNsinPU_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                        /* Call underlying kernel */
                        inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNsinPU_paramPtr->InArrID1].buf;
                        outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNsinPU_paramPtr->OutArrID1].buf;
                        for(j=0 ; j < (C6ACCEL_TI_IQNsinPU_paramPtr-> n); j++){
                            outArrRdr[j] = _IQNsinPU(inArrRdr1[j], C6ACCEL_TI_IQNsinPU_paramPtr->qfmt);
                         }
                      }
                   }
                 break;
                 case(IQNCOSPU_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *outArrRdr;
                    IQNcosPU_Params *C6ACCEL_TI_IQNcosPU_paramPtr;
                    C6ACCEL_TI_IQNcosPU_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNcosPU_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNcosPU_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNcosPU_paramPtr->InArrID1].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNcosPU_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNcosPU_paramPtr-> n); j++){
                             outArrRdr[j] = _IQNcosPU(inArrRdr1[j], C6ACCEL_TI_IQNcosPU_paramPtr->qfmt);
                          }
                      }
                  }
                 break;
                 case(IQNATAN2PU_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *inArrRdr2, *outArrRdr;
                    IQNatan2PU_Params *C6ACCEL_TI_IQNatan2PU_paramPtr;
                    C6ACCEL_TI_IQNatan2PU_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNatan2PU_paramPtr->y_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNatan2PU_paramPtr->x_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IQNatan2PU_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNatan2PU_paramPtr->y_InArrID1].buf;
                         inArrRdr2 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNatan2PU_paramPtr->x_InArrID2].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNatan2PU_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNatan2PU_paramPtr->n); j++){
                              outArrRdr[j] = _IQNatan2PU(inArrRdr1[j], inArrRdr2[j], C6ACCEL_TI_IQNatan2PU_paramPtr->qfmt);
                           }
                       }
                   }
                 break;
                 case(IQNMPYIQX_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *inArrRdr2, *outArrRdr;
                    IQNmpyIQX_Params *C6ACCEL_TI_IQNmpyIQX_paramPtr;
                    C6ACCEL_TI_IQNmpyIQX_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNmpyIQX_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNmpyIQX_paramPtr->InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IQNmpyIQX_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNmpyIQX_paramPtr->InArrID1].buf;
                         inArrRdr2 = (int *)inBufs->descs[C6ACCEL_TI_IQNmpyIQX_paramPtr->InArrID2].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNmpyIQX_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNmpyIQX_paramPtr->n); j++){
                             outArrRdr[j] = _IQNmpyIQx(inArrRdr1[j],
                                                       C6ACCEL_TI_IQNmpyIQX_paramPtr->qfmt1,
                                                       inArrRdr2[j],
                                                       C6ACCEL_TI_IQNmpyIQX_paramPtr->qfmt2,
                                                       C6ACCEL_TI_IQNmpyIQX_paramPtr->qfmt);
                          }
                      }
                  }
                 break;
                 case(IQNMPYI32INT_FXN_ID):{
                     /* Unmarshal Parameters */
                     I32_IQ *inArrRdr1, *inArrRdr2, *outArrRdr;
                     IQNmpyI32int_Params *C6ACCEL_TI_IQNmpyI32int_paramPtr;
                     C6ACCEL_TI_IQNmpyI32int_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_IQNmpyI32int_paramPtr->InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_IQNmpyI32int_paramPtr->InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_IQNmpyI32int_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNmpyI32int_paramPtr->InArrID1].buf;
                          inArrRdr2 = (int *)inBufs->descs[C6ACCEL_TI_IQNmpyI32int_paramPtr->InArrID2].buf;
                          outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNmpyI32int_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_IQNmpyI32int_paramPtr->n); j++){
                              outArrRdr[j] = _IQNmpyI32int(inArrRdr1[j], inArrRdr2[j], C6ACCEL_TI_IQNmpyI32int_paramPtr->qfmt);
                           }
                       }
                   }
                 break;
                 case(IQNMPYI32FRAC_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *inArrRdr2, *outArrRdr;
                    IQNmpyI32frac_Params *C6ACCEL_TI_IQNmpyI32frac_paramPtr;
                    C6ACCEL_TI_IQNmpyI32frac_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNmpyI32frac_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNmpyI32frac_paramPtr->InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IQNmpyI32frac_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNmpyI32frac_paramPtr->InArrID1].buf;
                         inArrRdr2 = (int *)inBufs->descs[C6ACCEL_TI_IQNmpyI32frac_paramPtr->InArrID2].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNmpyI32frac_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNmpyI32frac_paramPtr->n); j++){
                             outArrRdr[j] = _IQNmpyI32frac(inArrRdr1[j], inArrRdr2[j], C6ACCEL_TI_IQNmpyI32frac_paramPtr->qfmt);
                          }
                     }
                 }
                 break;
                 case(IQNPOW_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *inArrRdr2, *outArrRdr;
                    IQNpow_Params *C6ACCEL_TI_IQNpow_paramPtr;
                    C6ACCEL_TI_IQNpow_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNpow_paramPtr->A_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNpow_paramPtr->B_InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IQNpow_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNpow_paramPtr->A_InArrID1].buf;
                         inArrRdr2 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNpow_paramPtr->B_InArrID2].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNpow_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNpow_paramPtr-> n); j++){
                             outArrRdr[j] = _IQNpow(inArrRdr1[j], inArrRdr2[j], C6ACCEL_TI_IQNpow_paramPtr->qfmt);
                          }
                      }
                  }
                 break;
                 case(IQNISQRT_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *outArrRdr;
                    IQNisqrt_Params *C6ACCEL_TI_IQNisqrt_paramPtr;
                    C6ACCEL_TI_IQNisqrt_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNisqrt_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNisqrt_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                        /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNisqrt_paramPtr->InArrID1].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNisqrt_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNisqrt_paramPtr-> n); j++){
                             outArrRdr[j] = _IQNisqrt(inArrRdr1[j], C6ACCEL_TI_IQNisqrt_paramPtr->qfmt);
                           }
                      }
                  }
                 break;
                 case(IQNMAG_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *inArrRdr2, *outArrRdr;
                    IQNmag_Params *C6ACCEL_TI_IQNmag_paramPtr;
                    C6ACCEL_TI_IQNmag_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNmag_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNmag_paramPtr->InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_IQNmag_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNmag_paramPtr->InArrID1].buf;
                         inArrRdr2 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNmag_paramPtr->InArrID2].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNmag_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNmag_paramPtr-> n); j++){
                             outArrRdr[j] = _IQNmag(inArrRdr1[j], inArrRdr2[j], C6ACCEL_TI_IQNmag_paramPtr->qfmt);
                          }
                      }
                  }
                 break;
                 case(IQNTOF_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1;
                    float *out;
                    IQNtoF_Params *C6ACCEL_TI_IQNtoF_paramPtr;
                    C6ACCEL_TI_IQNtoF_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNtoF_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNtoF_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                        /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNtoF_paramPtr->InArrID1].buf;
                         out = (float *)outBufs->descs[C6ACCEL_TI_IQNtoF_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNtoF_paramPtr-> n); j++){
                             out[j] = _IQNtoF(inArrRdr1[j],C6ACCEL_TI_IQNtoF_paramPtr->qfmt);
                          }
                      }
                  }
                 break;
                 case(FTOIQN_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *outArrRdr;
                    float *in1;
                    FtoIQN_Params *C6ACCEL_TI_FtoIQN_paramPtr;
                    C6ACCEL_TI_FtoIQN_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_FtoIQN_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_FtoIQN_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         in1 = (float *)inBufs->descs[C6ACCEL_TI_FtoIQN_paramPtr->InArrID1].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_FtoIQN_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_FtoIQN_paramPtr-> n); j++){
                             outArrRdr[j] = _FtoIQN(in1[j], C6ACCEL_TI_FtoIQN_paramPtr->qfmt);
                           }
                      }
                  }
                 break;
                 case(IQN_FXN_ID):{
                    /* Unmarshal Parameters */
                    I32_IQ *inArrRdr1, *outArrRdr;
                    IQN_Params *C6ACCEL_TI_IQN_paramPtr;
                    C6ACCEL_TI_IQN_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQN_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQN_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQN_paramPtr->InArrID1].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQN_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQN_paramPtr-> n); j++){
                             outArrRdr[j] = (I32_IQ)(inArrRdr1[j] * pow((double)2,(double)C6ACCEL_TI_IQN_paramPtr->qfmt));
                           }
                       }
                   }
                 break;
                 case(IQXTOIQY_FXN_ID):{
                    I32_IQ *inArrRdr1, *outArrRdr;
                    /* Unmarshal Parameters */
                    IQXtoIQY_Params *C6ACCEL_TI_IQXtoIQY_paramPtr;
                    C6ACCEL_TI_IQXtoIQY_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQXtoIQY_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQXtoIQY_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQXtoIQY_paramPtr->InArrID1].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQXtoIQY_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQXtoIQY_paramPtr-> n); j++){
                             outArrRdr[j] = _IQXtoIQY(inArrRdr1[j],C6ACCEL_TI_IQXtoIQY_paramPtr->x,C6ACCEL_TI_IQXtoIQY_paramPtr->y);
                           }
                       }
                   }
                 break;
                 case(IQNINT_FXN_ID):{
                    I32_IQ *inArrRdr1, *outArrRdr;
                    /* Unmarshal Parameters */
                    IQNint_Params *C6ACCEL_TI_IQNint_paramPtr;
                    C6ACCEL_TI_IQNint_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNint_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNint_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNint_paramPtr->InArrID1].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNint_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNint_paramPtr-> n); j++){
                             outArrRdr[j] = _IQNint(inArrRdr1[j], C6ACCEL_TI_IQNint_paramPtr->qfmt);
                           }
                       }
                   }
                 break;
                 case(IQNFRAC_FXN_ID):{
                    I32_IQ *inArrRdr1, *outArrRdr;
                    /* Unmarshal Parameters */
                    IQNfrac_Params *C6ACCEL_TI_IQNfrac_paramPtr;
                    C6ACCEL_TI_IQNfrac_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_IQNfrac_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_IQNfrac_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                     else{
                         /* Call underlying kernel */
                         inArrRdr1 = (I32_IQ *)inBufs->descs[C6ACCEL_TI_IQNfrac_paramPtr->InArrID1].buf;
                         outArrRdr = (I32_IQ *)outBufs->descs[C6ACCEL_TI_IQNfrac_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_IQNfrac_paramPtr-> n); j++){
                             outArrRdr[j] = _IQNfrac(inArrRdr1[j], C6ACCEL_TI_IQNfrac_paramPtr->qfmt);
                           }
                       }
                   }
                 break;
                 /* FASTRTS LIB functions */
                 case (ADDSP_FXN_ID):{
                    float  *in1, *in2, *out;
                    /* Unmarshal Parameters */
                    addsp_i_Params *C6ACCEL_TI_addsp_i_paramPtr;
                    C6ACCEL_TI_addsp_i_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_addsp_i_paramPtr->InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_addsp_i_paramPtr->InArrID2)>INBUF15)|
                       ((C6ACCEL_TI_addsp_i_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                         /* Call underlying kernel */
                         in1 = (float *)inBufs->descs[C6ACCEL_TI_addsp_i_paramPtr->InArrID1].buf;
                         in2 = (float *)inBufs->descs[C6ACCEL_TI_addsp_i_paramPtr->InArrID2].buf;
                         out = (float *)outBufs->descs[C6ACCEL_TI_addsp_i_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_addsp_i_paramPtr-> n); j++){
                            out[j] = addsp_i(in1[j], in2[j]);
                           }
                       }
                   }
                 break;
                 case (SUBSP_FXN_ID):{
                     float  *in1, *in2, *out;
                     /* Unmarshal Parameters */
                     subsp_i_Params *C6ACCEL_TI_subsp_i_paramPtr;
                     C6ACCEL_TI_subsp_i_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_subsp_i_paramPtr->InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_subsp_i_paramPtr->InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_subsp_i_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                      else{
                          /* Call underlying kernel */
                           in1 = (float *)inBufs->descs[C6ACCEL_TI_subsp_i_paramPtr->InArrID1].buf;
                           in2 = (float *)inBufs->descs[C6ACCEL_TI_subsp_i_paramPtr->InArrID2].buf;
                           out = (float *)outBufs->descs[C6ACCEL_TI_subsp_i_paramPtr->OutArrID1].buf;
                           for(j=0 ; j < (C6ACCEL_TI_subsp_i_paramPtr-> n); j++){
                              out[j] = subsp_i(in1[j], in2[j]);
                             }
                         }
                    }
                 break;
                 case (MPYSP_FXN_ID):{
                     float  *in1, *in2, *out;
                     /* Unmarshal Parameters */
                     mpysp_i_Params *C6ACCEL_TI_mpysp_i_paramPtr;
                     C6ACCEL_TI_mpysp_i_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_mpysp_i_paramPtr->InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_mpysp_i_paramPtr->InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_mpysp_i_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                      else{
                           /* Call underlying kernel */
                           in1 = (float *)inBufs->descs[C6ACCEL_TI_mpysp_i_paramPtr->InArrID1].buf;
                           in2 = (float *)inBufs->descs[C6ACCEL_TI_mpysp_i_paramPtr->InArrID2].buf;
                           out = (float *)outBufs->descs[C6ACCEL_TI_mpysp_i_paramPtr->OutArrID1].buf;
                           for(j=0 ; j < (C6ACCEL_TI_mpysp_i_paramPtr-> n); j++){
                              out[j] = mpysp_i(in1[j], in2[j]);
                            }
                        }
                   }
                 break;
                 case (DIVSP_FXN_ID):{
                     float  *in1, *in2, *out;
                     /* Unmarshal Parameters */
                     divsp_i_Params  *C6ACCEL_TI_divsp_i_paramPtr;
                     C6ACCEL_TI_divsp_i_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_divsp_i_paramPtr->InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_divsp_i_paramPtr->InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_divsp_i_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                      else{
                           /* Call underlying kernel */
                           in1 = (float *)inBufs->descs[C6ACCEL_TI_divsp_i_paramPtr->InArrID1].buf;
                           in2 = (float *)inBufs->descs[C6ACCEL_TI_divsp_i_paramPtr->InArrID2].buf;
                           out = (float *)outBufs->descs[C6ACCEL_TI_divsp_i_paramPtr->OutArrID1].buf;
                           for(j=0 ; j < (C6ACCEL_TI_divsp_i_paramPtr-> n); j++){
                              out[j] = divsp_i(in1[j], in2[j]);
                             }
                        }
                   }
                 break;
                 case (RECIPSP_FXN_ID):{
                     float  *in1, *out;
                     /* Unmarshal Parameters */
                     recipsp_i_Params *C6ACCEL_TI_recipsp_i_paramPtr;
                     C6ACCEL_TI_recipsp_i_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_recipsp_i_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_recipsp_i_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (float *)inBufs->descs[C6ACCEL_TI_recipsp_i_paramPtr->x_InArrID1].buf;
                          out = (float *)outBufs->descs[C6ACCEL_TI_recipsp_i_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_recipsp_i_paramPtr-> n); j++){
                              out[j] = recipsp_i(in1[j]);
                           }
                       }
                   }
                 break;
                 case (INTSP_FXN_ID):{
                     I32_IQ *inArrRdr1;
                     float *out;
                     /* Unmarshal Parameters */
                     intsp_i_Params *C6ACCEL_TI_intsp_i_paramPtr;
                     C6ACCEL_TI_intsp_i_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_intsp_i_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_intsp_i_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                         inArrRdr1 = (int *)inBufs->descs[C6ACCEL_TI_intsp_i_paramPtr->x_InArrID1].buf;
                         out = (float *)outBufs->descs[C6ACCEL_TI_intsp_i_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_intsp_i_paramPtr-> n); j++){
                             out[j] = intsp_i(inArrRdr1[j]);
                           }
                       }
                   }
                 break;
                 case (SPINT_FXN_ID):{
                     I32_IQ *outArrRdr;
                     float *in1;
                     /* Unmarshal Parameters */
                     spint_i_Params *C6ACCEL_TI_spint_i_paramPtr;
                     C6ACCEL_TI_spint_i_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_spint_i_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_spint_i_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                         in1 = (float *)inBufs->descs[C6ACCEL_TI_spint_i_paramPtr->x_InArrID1].buf;
                         outArrRdr = (int *)outBufs->descs[C6ACCEL_TI_spint_i_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_spint_i_paramPtr-> n); j++){
                             outArrRdr[j] = spint_i(in1[j]);
                          }
                       }
                   }
                 break;
                 case (SPUINT_FXN_ID):{
                    I32_IQ *outArrRdr;
                    float *in1;
                    /* Unmarshal Parameters */
                    spuint_i_Params *C6ACCEL_TI_spuint_i_paramPtr;
                    C6ACCEL_TI_spuint_i_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_spuint_i_paramPtr->x_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_spuint_i_paramPtr->OutArrID1)>OUTBUF15)){
                       return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         in1 = (float *)inBufs->descs[C6ACCEL_TI_spuint_i_paramPtr->x_InArrID1].buf;
                         outArrRdr = (int *)outBufs->descs[C6ACCEL_TI_spuint_i_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_spuint_i_paramPtr-> n); j++){
                             outArrRdr[j] = spuint_i(in1[j]);
                          }
                      }
                  }
                 break;
                 case (SQRTSP_FXN_ID):{
                    float  *in1, *out;
                    /* Unmarshal Parameters */
                    sqrtsp_i_Params *C6ACCEL_TI_sqrtsp_i_paramPtr;
                    C6ACCEL_TI_sqrtsp_i_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_sqrtsp_i_paramPtr->x_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_sqrtsp_i_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                         /* Call underlying kernel */
                         in1 = (float *)inBufs->descs[C6ACCEL_TI_sqrtsp_i_paramPtr->x_InArrID1].buf;
                         out = (float *)outBufs->descs[C6ACCEL_TI_sqrtsp_i_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_sqrtsp_i_paramPtr-> n); j++){
                             out[j] = sqrtsp_i(in1[j]);
                          }
                      }
                  }
                 break;
                 case (UINTSP_FXN_ID):{
                    I32_IQ *inArrRdr1;
                    float *out;
                    /* Unmarshal Parameters */
                    uintsp_i_Params *C6ACCEL_TI_uintsp_i_paramPtr;
                    C6ACCEL_TI_uintsp_i_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_uintsp_i_paramPtr->x_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_uintsp_i_paramPtr->OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                        /* Call underlying kernel */
                         inArrRdr1 = (int *)inBufs->descs[C6ACCEL_TI_uintsp_i_paramPtr->x_InArrID1].buf;
                         out = (float *)outBufs->descs[C6ACCEL_TI_uintsp_i_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_uintsp_i_paramPtr-> n); j++){
                             out[j] = uintsp_i(inArrRdr1[j]);
                          }
                      }
                   }
                 break;
                 case (D2S_FXN_ID):{
                    /* Unmarshal Parameters */
                    d2s_Params *C6ACCEL_TI_d2s_paramPtr;
                    C6ACCEL_TI_d2s_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_d2s_paramPtr->d_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_d2s_paramPtr->s_OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                        /* Call underlying kernel */
                         d2s_vec((double *)inBufs->descs[C6ACCEL_TI_d2s_paramPtr->d_InArrID1].buf,
                                          (short *)outBufs->descs[C6ACCEL_TI_d2s_paramPtr->s_OutArrID1].buf,
                                          C6ACCEL_TI_d2s_paramPtr->n);
                          
                      }
                   }
                 break;
                 case (S2D_FXN_ID):{
                    /* Unmarshal Parameters */
                    s2d_Params *C6ACCEL_TI_s2d_paramPtr;
                    C6ACCEL_TI_s2d_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_s2d_paramPtr->s_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_s2d_paramPtr->d_OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                        /* Call underlying kernel */
                         s2d_vec((double *)outBufs->descs[C6ACCEL_TI_s2d_paramPtr->d_OutArrID1].buf,
                                          (short *)inBufs->descs[C6ACCEL_TI_s2d_paramPtr->s_InArrID1].buf,
                                          C6ACCEL_TI_s2d_paramPtr->n);
                          
                      }
                   }
                 break;
                 case (F2D_FXN_ID):{
                    /* Unmarshal Parameters */
                    f2d_Params *C6ACCEL_TI_f2d_paramPtr;
                    C6ACCEL_TI_f2d_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_f2d_paramPtr->f_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_f2d_paramPtr->d_OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                        /* Call underlying kernel */
                         f2d_vec((double *)outBufs->descs[C6ACCEL_TI_f2d_paramPtr->d_OutArrID1].buf,
                                          (float *)inBufs->descs[C6ACCEL_TI_f2d_paramPtr->f_InArrID1].buf,
                                          C6ACCEL_TI_f2d_paramPtr->n);
                          
                      }
                   }
                 break;
                 case (D2F_FXN_ID):{
                    /* Unmarshal Parameters */
                    d2f_Params *C6ACCEL_TI_d2f_paramPtr;
                    C6ACCEL_TI_d2f_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_d2f_paramPtr->d_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_d2f_paramPtr->f_OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                        /* Call underlying kernel */
                         d2f_vec((double *)inBufs->descs[C6ACCEL_TI_d2f_paramPtr->d_InArrID1].buf,
                                          (float *)outBufs->descs[C6ACCEL_TI_d2f_paramPtr->f_OutArrID1].buf,
                                          C6ACCEL_TI_d2f_paramPtr->n);
                          
                      }
                   }
                 break;
                 case (D2I_FXN_ID):{
                    /* Unmarshal Parameters */
                    d2i_Params *C6ACCEL_TI_d2i_paramPtr;
                    C6ACCEL_TI_d2i_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_d2i_paramPtr->d_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_d2i_paramPtr->i_OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                        /* Call underlying kernel */
                         d2i_vec((double *)inBufs->descs[C6ACCEL_TI_d2i_paramPtr->d_InArrID1].buf,
                                          (int *)outBufs->descs[C6ACCEL_TI_d2i_paramPtr->i_OutArrID1].buf,
                                          C6ACCEL_TI_d2i_paramPtr->n);
                          
                      }
                   }
                 break;
                 case (I2D_FXN_ID):{
                    /* Unmarshal Parameters */
                    i2d_Params *C6ACCEL_TI_i2d_paramPtr;
                    C6ACCEL_TI_i2d_paramPtr = pFnArray;
                    /* Parameter check */
                    if(((C6ACCEL_TI_i2d_paramPtr->i_InArrID1)>INBUF15)|
                       ((C6ACCEL_TI_i2d_paramPtr->d_OutArrID1)>OUTBUF15)){
                         return(IUNIVERSAL_EPARAMFAIL);
                     }
                    else{
                        /* Call underlying kernel */
                         i2d_vec((double *)outBufs->descs[C6ACCEL_TI_i2d_paramPtr->d_OutArrID1].buf,
                                          (int *)inBufs->descs[C6ACCEL_TI_i2d_paramPtr->i_InArrID1].buf,
                                          C6ACCEL_TI_i2d_paramPtr->n);
                          
                      }
                   }
                 break;
                 #ifdef DEVICE_FLOAT
                 case (ATANSP_FXN_ID):{
                     float  *in1, *out;
                     /* Unmarshal Parameters */
                     atansp_Params *C6ACCEL_TI_atansp_paramPtr;
                     C6ACCEL_TI_atansp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_atansp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_atansp_paramPtr->OutArrID1)>OUTBUF15)){
                        return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (float *)inBufs->descs[C6ACCEL_TI_atansp_paramPtr->z_InArrID1].buf;
                          out = (float *)outBufs->descs[C6ACCEL_TI_atansp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_atansp_paramPtr->n); j++){
                              out[j] = atansp(in1[j]);
                           }
                      }
                  }
                 break;
                 case (COSSP_FXN_ID):{
                        float  *in1, *out;
                        /* Unmarshal Parameters */
                        cossp_Params *C6ACCEL_TI_cossp_paramPtr;
                        C6ACCEL_TI_cossp_paramPtr = pFnArray;
                        /* Parameter check */
                         if(((C6ACCEL_TI_cossp_paramPtr->z_InArrID1)>INBUF15)|
                            ((C6ACCEL_TI_cossp_paramPtr->OutArrID1)>OUTBUF15)){
                             return(IUNIVERSAL_EPARAMFAIL);
                          }
                         else{
                              /* Call underlying kernel */
                              in1 = (float *)inBufs->descs[C6ACCEL_TI_cossp_paramPtr->z_InArrID1].buf;
                              out = (float *)outBufs->descs[C6ACCEL_TI_cossp_paramPtr->OutArrID1].buf;
                              for(j=0 ; j < (C6ACCEL_TI_cossp_paramPtr-> n); j++){
                                out[j] = cossp(in1[j]);
                             }
                         }
                    }
                 break;
                 case (SINSP_FXN_ID):{
                     float  *in1, *out;
                     /* Unmarshal Parameters */
                     sinsp_Params *C6ACCEL_TI_sinsp_paramPtr;
                     C6ACCEL_TI_sinsp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_sinsp_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_sinsp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (float *)inBufs->descs[C6ACCEL_TI_sinsp_paramPtr->x_InArrID1].buf;
                          out = (float *)outBufs->descs[C6ACCEL_TI_sinsp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_sinsp_paramPtr-> n); j++){
                                out[j] = sinsp(in1[j]);
                            }
                        }
                    }
                 break;
                 case (EXPSP_FXN_ID):{
                     float  *in1, *out;
                     /* Unmarshal Parameters */
                     expsp_Params *C6ACCEL_TI_expsp_paramPtr;
                     C6ACCEL_TI_expsp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_expsp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_expsp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (float *)inBufs->descs[C6ACCEL_TI_expsp_paramPtr->z_InArrID1].buf;
                          out = (float *)outBufs->descs[C6ACCEL_TI_expsp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_expsp_paramPtr->n); j++){
                              out[j] = expsp(in1[j]);
                            }
                        }
                    }
                 break;
                 case (LOGSP_FXN_ID):{
                     float  *in1, *out;
                     /* Unmarshal Parameters */
                     logsp_Params *C6ACCEL_TI_logsp_paramPtr;
                     C6ACCEL_TI_logsp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_logsp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_logsp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (float *)inBufs->descs[C6ACCEL_TI_logsp_paramPtr->z_InArrID1].buf;
                          out = (float *)outBufs->descs[C6ACCEL_TI_logsp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_logsp_paramPtr-> n); j++){
                              out[j] = logsp(in1[j]);
                           }
                       }
                  }
                 break;
                 case (POWSP_FXN_ID):{
                     float  *in1, *in2, *out;
                     /* Unmarshal Parameters */
                     powsp_Params *C6ACCEL_TI_powsp_paramPtr;
                     C6ACCEL_TI_powsp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_powsp_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_powsp_paramPtr->y_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_powsp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (float *)inBufs->descs[C6ACCEL_TI_powsp_paramPtr->x_InArrID1].buf;
                          in2 = (float *)inBufs->descs[C6ACCEL_TI_powsp_paramPtr->y_InArrID2].buf;
                          out = (float *)outBufs->descs[C6ACCEL_TI_powsp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_powsp_paramPtr-> n); j++){
                              out[j] = powsp(in1[j],in2[j]);
                           }
                       }
                  }
                 break;
                 case (ATANDP_FXN_ID):{
                     double  *in1, *out;
                     /* Unmarshal Parameters */
                     atandp_Params *C6ACCEL_TI_atandp_paramPtr;
                     C6ACCEL_TI_atandp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_atandp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_atandp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (double *)inBufs->descs[C6ACCEL_TI_atandp_paramPtr->z_InArrID1].buf;
                          out = (double *)outBufs->descs[C6ACCEL_TI_atandp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_atandp_paramPtr->n); j++){
                              out[j] = atandp(in1[j]);
                           }
                       }
                  }
                 break;
                 case (COSDP_FXN_ID):{
                     double  *in1, *out;
                     /* Unmarshal Parameters */
                     cosdp_Params *C6ACCEL_TI_cosdp_paramPtr;
                     C6ACCEL_TI_cosdp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_cosdp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_cosdp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                      else{
                         /* Call underlying kernel */
                         in1 = (double *)inBufs->descs[C6ACCEL_TI_cosdp_paramPtr->z_InArrID1].buf;
                         out = (double *)outBufs->descs[C6ACCEL_TI_cosdp_paramPtr->OutArrID1].buf;
                         for(j=0 ; j < (C6ACCEL_TI_cosdp_paramPtr-> n); j++){
                                out[j] = cosdp(in1[j]);
                           }
                       }
                  }
                 break;
                 case (SINDP_FXN_ID):{
                     double  *in1, *out;
                     /* Unmarshal Parameters */
                     sindp_Params *C6ACCEL_TI_sindp_paramPtr;
                     C6ACCEL_TI_sindp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_sindp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_sindp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (double *)inBufs->descs[C6ACCEL_TI_sindp_paramPtr->z_InArrID1].buf;
                          out = (double *)outBufs->descs[C6ACCEL_TI_sindp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_sindp_paramPtr-> n); j++){
                              out[j] = sindp(in1[j]);
                           }
                      }
                  }
                 break;
                 case (EXPDP_FXN_ID):{
                     double  *in1, *out;
                     /* Unmarshal Parameters */
                     expdp_Params *C6ACCEL_TI_expdp_paramPtr;
                     C6ACCEL_TI_expdp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_expdp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_expdp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (double *)inBufs->descs[C6ACCEL_TI_expdp_paramPtr->z_InArrID1].buf;
                          out = (double *)outBufs->descs[C6ACCEL_TI_expdp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_expdp_paramPtr->n); j++){
                              out[j] = expdp(in1[j]);
                            }
                        }
                    }
                 break;
                 case (LOGDP_FXN_ID):{
                     double  *in1, *out;
                     /* Unmarshal Parameters */
                     logdp_Params *C6ACCEL_TI_logdp_paramPtr;
                     C6ACCEL_TI_logdp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_logdp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_logdp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (double *)inBufs->descs[C6ACCEL_TI_logdp_paramPtr->z_InArrID1].buf;
                          out = (double *)outBufs->descs[C6ACCEL_TI_logdp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_logdp_paramPtr-> n); j++){
                              out[j] = logdp(in1[j]);
                            }
                       }
                  }
                 break;
                 case (POWDP_FXN_ID):{
                     double  *in1,*in2, *out;
                     /* Unmarshal Parameters */
                     powdp_Params *C6ACCEL_TI_powdp_paramPtr;
                     C6ACCEL_TI_powdp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_powdp_paramPtr->x_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_powdp_paramPtr->y_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_powdp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (double *)inBufs->descs[C6ACCEL_TI_powdp_paramPtr->x_InArrID1].buf;
                          in2 = (double *)inBufs->descs[C6ACCEL_TI_powdp_paramPtr->y_InArrID2].buf;
                          out = (double *)outBufs->descs[C6ACCEL_TI_powdp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_powdp_paramPtr-> n); j++){
                              out[j] = powdp(in1[j],in2[j]);
                            }
                       }
                  }
                 break;
                 case (RECIPDP_FXN_ID):{
                     double  *in1,*out;
                     /* Unmarshal Parameters */
                     recipdp_Params *C6ACCEL_TI_recipdp_paramPtr;
                     C6ACCEL_TI_recipdp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_recipdp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_recipdp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (double *)inBufs->descs[C6ACCEL_TI_recipdp_paramPtr->z_InArrID1].buf;
                          out = (double *)outBufs->descs[C6ACCEL_TI_recipdp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_recipdp_paramPtr-> n); j++){
                              out[j] = recipdp(in1[j]);
                            }
                       }
                  }
                 break;
                 case (RSQRTDP_FXN_ID):{
                     double  *in1, *out;
                     /* Unmarshal Parameters */
                     rsqrtdp_Params *C6ACCEL_TI_rsqrtdp_paramPtr;
                     C6ACCEL_TI_rsqrtdp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_rsqrtdp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_rsqrtdp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (double *)inBufs->descs[C6ACCEL_TI_rsqrtdp_paramPtr->z_InArrID1].buf;
                          out = (double *)outBufs->descs[C6ACCEL_TI_rsqrtdp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_rsqrtdp_paramPtr-> n); j++){
                              out[j] = rsqrtdp(in1[j]);
                            }
                       }
                  }
                 break;
                 case (SQRTDP_FXN_ID):{
                     double  *in1,*out;
                     /* Unmarshal Parameters */
                     sqrtdp_Params *C6ACCEL_TI_sqrtdp_paramPtr;
                     C6ACCEL_TI_sqrtdp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_sqrtdp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_sqrtdp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (double *)inBufs->descs[C6ACCEL_TI_sqrtdp_paramPtr->z_InArrID1].buf;
                          out = (double *)outBufs->descs[C6ACCEL_TI_sqrtdp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_sqrtdp_paramPtr-> n); j++){
                              out[j] = sqrtdp(in1[j]);
                            }
                       }
                  }
                 break;
                 case (EXP2DP_FXN_ID):{
                     double  *in1,*out;
                     /* Unmarshal Parameters */
                     exp2dp_Params *C6ACCEL_TI_exp2dp_paramPtr;
                     C6ACCEL_TI_exp2dp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_exp2dp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_exp2dp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (double *)inBufs->descs[C6ACCEL_TI_exp2dp_paramPtr->z_InArrID1].buf;
                          out = (double *)outBufs->descs[C6ACCEL_TI_exp2dp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_exp2dp_paramPtr-> n); j++){
                              out[j] = exp2dp(in1[j]);
                            }
                       }
                  }
                 break;
                 case (EXP10DP_FXN_ID):{
                     double  *in1,*out;
                     /* Unmarshal Parameters */
                     exp10dp_Params *C6ACCEL_TI_exp10dp_paramPtr;
                     C6ACCEL_TI_exp10dp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_exp10dp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_exp10dp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (double *)inBufs->descs[C6ACCEL_TI_exp10dp_paramPtr->z_InArrID1].buf;
                          out = (double *)outBufs->descs[C6ACCEL_TI_exp10dp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_exp10dp_paramPtr-> n); j++){
                              out[j] = exp10dp(in1[j]);
                            }
                       }
                  }
                 break;
                 case (ATAN2DP_FXN_ID):{
                     double  *in1,*in2, *out;
                     /* Unmarshal Parameters */
                     atan2dp_Params *C6ACCEL_TI_atan2dp_paramPtr;
                     C6ACCEL_TI_atan2dp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_atan2dp_paramPtr->y_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_atan2dp_paramPtr->x_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_atan2dp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (double *)inBufs->descs[C6ACCEL_TI_atan2dp_paramPtr->y_InArrID1].buf;
                          in2 = (double *)inBufs->descs[C6ACCEL_TI_atan2dp_paramPtr->x_InArrID2].buf;
                          out = (double *)outBufs->descs[C6ACCEL_TI_atan2dp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_atan2dp_paramPtr-> n); j++){
                              out[j] = atan2dp(in1[j],in2[j]);
                            }
                       }
                  }
                 break;
                 case (RECIPF_FXN_ID):{
                     float  *in1,*out;
                     /* Unmarshal Parameters */
                     recipsp_Params *C6ACCEL_TI_recipsp_paramPtr;
                     C6ACCEL_TI_recipsp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_recipsp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_recipsp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (float *)inBufs->descs[C6ACCEL_TI_recipsp_paramPtr->z_InArrID1].buf;
                          out = (float *)outBufs->descs[C6ACCEL_TI_recipsp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_recipsp_paramPtr-> n); j++){
                              out[j] = recipsp(in1[j]);
                            }
                       }
                  }
                 break;
                 case (RSQRTSP_FXN_ID):{
                     float  *in1, *out;
                     /* Unmarshal Parameters */
                     rsqrtsp_Params *C6ACCEL_TI_rsqrtsp_paramPtr;
                     C6ACCEL_TI_rsqrtsp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_rsqrtsp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_rsqrtsp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (float *)inBufs->descs[C6ACCEL_TI_rsqrtsp_paramPtr->z_InArrID1].buf;
                          out = (float *)outBufs->descs[C6ACCEL_TI_rsqrtsp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_rsqrtsp_paramPtr-> n); j++){
                              out[j] = rsqrtsp(in1[j]);
                            }
                       }
                  }
                 break;
                 case (SQRT_FXN_ID):{
                     float  *in1,*out;
                     /* Unmarshal Parameters */
                     sqrtsp_Params *C6ACCEL_TI_sqrtsp_paramPtr;
                     C6ACCEL_TI_sqrtsp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_sqrtsp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_sqrtsp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (float *)inBufs->descs[C6ACCEL_TI_sqrtsp_paramPtr->z_InArrID1].buf;
                          out = (float *)outBufs->descs[C6ACCEL_TI_sqrtsp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_sqrtsp_paramPtr-> n); j++){
                              out[j] = sqrtsp(in1[j]);
                            }
                       }
                  }
                 break;
                 case (EXP2SP_FXN_ID):{
                     float  *in1,*out;
                     /* Unmarshal Parameters */
                     exp2sp_Params *C6ACCEL_TI_exp2sp_paramPtr;
                     C6ACCEL_TI_exp2sp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_exp2sp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_exp2sp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (float *)inBufs->descs[C6ACCEL_TI_exp2sp_paramPtr->z_InArrID1].buf;
                          out = (float *)outBufs->descs[C6ACCEL_TI_exp2sp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_exp2sp_paramPtr-> n); j++){
                              out[j] = exp2sp(in1[j]);
                            }
                       }
                  }
                 break;
                 case (EXP10SP_FXN_ID):{
                     float  *in1,*out;
                     /* Unmarshal Parameters */
                     exp10sp_Params *C6ACCEL_TI_exp10sp_paramPtr;
                     C6ACCEL_TI_exp10sp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_exp10sp_paramPtr->z_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_exp10sp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (float *)inBufs->descs[C6ACCEL_TI_exp10sp_paramPtr->z_InArrID1].buf;
                          out = (float *)outBufs->descs[C6ACCEL_TI_exp10sp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_exp10sp_paramPtr-> n); j++){
                              out[j] = exp10sp(in1[j]);
                            }
                       }
                  }
                 break;
                 case (ATAN2SP_FXN_ID):{
                     float  *in1,*in2, *out;
                     /* Unmarshal Parameters */
                     atan2sp_Params *C6ACCEL_TI_atan2sp_paramPtr;
                     C6ACCEL_TI_atan2sp_paramPtr = pFnArray;
                     /* Parameter check */
                     if(((C6ACCEL_TI_atan2sp_paramPtr->y_InArrID1)>INBUF15)|
                        ((C6ACCEL_TI_atan2sp_paramPtr->x_InArrID2)>INBUF15)|
                        ((C6ACCEL_TI_atan2sp_paramPtr->OutArrID1)>OUTBUF15)){
                          return(IUNIVERSAL_EPARAMFAIL);
                      }
                     else{
                          /* Call underlying kernel */
                          in1 = (float *)inBufs->descs[C6ACCEL_TI_atan2sp_paramPtr->y_InArrID1].buf;
                          in2 = (float *)inBufs->descs[C6ACCEL_TI_atan2sp_paramPtr->x_InArrID2].buf;
                          out = (float *)outBufs->descs[C6ACCEL_TI_atan2sp_paramPtr->OutArrID1].buf;
                          for(j=0 ; j < (C6ACCEL_TI_atan2sp_paramPtr-> n); j++){
                              out[j] = atan2sp(in1[j],in2[j]);
                            }
                       }
                  }
                 break;
                 #endif
                 default:
                    /*Error caused due to passing of an invalid ID*/
                    return(IUNIVERSAL_EFXNIDFAIL);
                  }
    return 0;
}

/*=============================================================================*/
/*                  End of File C6accel_ti_mathlibFunctionCall.c                */
/*                              Version : 0.0.1                                */
/*=============================================================================*/
