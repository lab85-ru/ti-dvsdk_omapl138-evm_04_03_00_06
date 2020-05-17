/*============================================================================*/
/*  Copyright (c) 2010, Texas Instruments Incorporated                        */
/*  All rights reserved.                                                      */
/*                                                                            */
/*                                                                            */
/*  Name: c6accelw_opencv.c                                                   */
/*                                                                            */
/*  Description:                                                              */
/*  This function implements the ARM wrapper functions for OpenCV in C6ACCEL  */
/*                                                                            */
/*  Version: 0.0.1                                                            */
/*============================================================================*/

/* Codec Engine include files: */
#include <xdc/std.h>
#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/osal/Memory.h>
#include <ti/sdo/ce/universal/universal.h>

/* Run Time lib include files: */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>



/* These will be static and internal in full implementation*/
static UNIVERSAL_Params uniParams;

/* Create default heap memory configuration for wrapper
 Use non-cached as temp workaround for stability issue with Memory_alloc()*/
static Memory_AllocParams wrapperMemParams = {
    Memory_CONTIGHEAP,
    Memory_NONCACHED,
    Memory_DEFAULTALIGNMENT,
    0
};

/* Wrapper API includes */
 #include "c6accelw_i.h"
 #include "c6accelw.h"
 

/* Include the header for C6ACCEL codec support*/
#include "ti/c6accel/iC6Accel_ti.h"

/*Parameter definition */
#define BUFALIGN 128
#define MAX_SEQ_LIST_SIZE 256

/* Structure for context saving in case of asynchronous process calls */
static T_ASYNC_CALL_PARAMS c6accelAsyncParams;

/*C6accel object definition to pass the engine and universal handle*/
typedef struct C6accel_Object{
    Engine_Handle hEngine;
    UNIVERSAL_Handle hUni;
    //fields related to this instance...
    E_CALL_TYPE callType;
}C6accel_Object;

int C6accel_VLIB_dilate_bin_mask ( C6accel_Handle hC6accel,
                                  const unsigned char *restrict indata,
                                  unsigned char       *restrict outdata,
                                  const char          *restrict mask,
                                  short                cols,
                                  int                  pitch
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_dilate_bin_mask_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_dilate_bin_mask_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 2;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(indata,0,(pitch*cols)*sizeof(char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(mask,1,(3*3)*sizeof(char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(outdata,0,(pitch*cols)*sizeof(char));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_DILATE_BIN_MASK_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_dilate_bin_mask_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->indata_InArrID1=  INBUF0;
    fp0->msk_InArrID2   =  INBUF1;
    fp0->outdata_OutArrID1= OUTBUF0;
    fp0->pitch = pitch;
    fp0->cols = cols;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

int C6accel_VLIB_erode_bin_mask ( C6accel_Handle hC6accel,
                                  const unsigned char *restrict indata,
                                  unsigned char       *restrict outdata,
                                  const char          *restrict mask,
                                  short                cols,
                                  int                  pitch
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_erode_bin_mask_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_erode_bin_mask_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 2;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(indata,0,(pitch*cols));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(mask,1,(3*3)*sizeof(char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(outdata,0,(pitch*cols));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_ERODE_BIN_MASK_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_erode_bin_mask_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->indata_InArrID1=  INBUF0;
    fp0->msk_InArrID2   =  INBUF1;
    fp0->outdata_OutArrID1= OUTBUF0;
    fp0->pitch = pitch;
    fp0->cols = cols;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

int C6accel_VLIB_initMeanWithLumaS16 ( C6accel_Handle hC6accel,
                                  short*restrict runningMean,
                                  const unsigned char  *restrict lumaFrame,
                                  int                  pixelCount
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_initMeanWithLumaS16_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_initMeanWithLumaS16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 1;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(lumaFrame,0,(pixelCount)*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(runningMean,0,(pixelCount)*sizeof(short));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_INITMEANWITHLUMAS16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_initMeanWithLumaS16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->lumaFrame_InArrID1=  INBUF0;
    fp0->runningMean_OutArrID1= OUTBUF0;
    fp0->pixelCount = pixelCount;
    
    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

int C6accel_VLIB_initMeanWithLumaS32 ( C6accel_Handle hC6accel,
                                  int *restrict runningMean,
                                  unsigned char          *restrict lumaFrame,
                                  int                  pixelCount
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_initMeanWithLumaS32_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_initMeanWithLumaS32_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 1;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(lumaFrame,0,(pixelCount)*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(runningMean,0,(pixelCount)*sizeof(int));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_INITMEANWITHLUMAS32_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_initMeanWithLumaS32_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->lumaFrame_InArrID1=  INBUF0;
    fp0->runningMean_OutArrID1= OUTBUF0;
    fp0->pixelCount = pixelCount;
    
    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

int C6accel_VLIB_updateEWRMeanS16 ( C6accel_Handle hC6accel,
                                  short*restrict runningMean,
                                  const unsigned char  *restrict newluma,
                                  const unsigned int   *restrict mask32packed,
                                  const short                  weight,
                                  int                  pixelCount
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_updateEWRMeanS16_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_updateEWRMeanS16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 2;
    inOutBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(newluma,0,(pixelCount)*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(mask32packed,1,sizeof(int)*(pixelCount)/32);
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(runningMean,0,(pixelCount)*sizeof(short));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_UPDATEEWRMEANS16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_updateEWRMeanS16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->newLuma_InArrID1=  INBUF0;
    fp0->mask32packed_InArrID2 =  INBUF1;
    fp0->runningMean_InOutArrID1 = OUTBUF0;
    fp0->weight = weight;
    fp0->pixelCount = pixelCount;
  
    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{

      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc,NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

int C6accel_VLIB_updateEWRMeanS32 ( C6accel_Handle hC6accel,
                                  int *restrict runningMean,
                                  const unsigned char  *restrict newluma,
                                  const unsigned int   *restrict mask32packed,
                                  const int                  weight,
                                  unsigned int                  pixelCount
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_updateEWRMeanS32_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_updateEWRMeanS32_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 2;
    inOutBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(newluma,0,(pixelCount)*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(mask32packed,1,sizeof(int)*(pixelCount)/32);
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(runningMean,0,(pixelCount)*sizeof(int));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_UPDATEEWRMEANS32_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_updateEWRMeanS32_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->newLuma_InArrID1=  INBUF0;
    fp0->mask32packed_InArrID2 =  INBUF1;
    fp0->runningMean_InOutArrID1 = OUTBUF0;
    fp0->weight = weight;
    fp0->pixelCount = pixelCount;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

int C6accel_VLIB_initVarWithConstS16 ( C6accel_Handle hC6accel,
                                  short*restrict runningVar,
                                  const short constVar,
                                  unsigned int pixelCount
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_initVarWithConstS16_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_initVarWithConstS16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 0;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(runningVar,0,(pixelCount)*sizeof(short));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_INITVARWITHCONSTS16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_initVarWithConstS16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->runningVar_OutArrID1= OUTBUF0;
    fp0->pixelCount = pixelCount;
    fp0->constVar = constVar;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {
       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc,NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

int C6accel_VLIB_initVarWithConstS32 ( C6accel_Handle hC6accel,
                                     int *restrict runningVar,
                                          const int constVar,
                                       unsigned int pixelCount
                                     )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_initVarWithConstS32_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_initVarWithConstS32_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 0;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
     CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(runningVar,0,(pixelCount)*sizeof(int));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_INITVARWITHCONSTS32_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_initVarWithConstS32_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->runningVar_OutArrID1= OUTBUF0;
    fp0->pixelCount = pixelCount;
    fp0->constVar = constVar;
    
    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

int C6accel_VLIB_updateEWRVarianceS16 ( C6accel_Handle hC6accel,
                                  short*restrict runningVar,
                                  short*restrict runningMean,
                                  const unsigned char  *restrict newLuma,
                                  const unsigned int   *restrict mask32packed,
                                  const short                  weight,
                                  int                  pixelCount
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
     XDM1_BufDesc               inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_updateEWRVarianceS16_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_updateEWRVarianceS16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 2;
    inOutBufDesc.numBufs = 2;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(newLuma,0,(pixelCount)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(mask32packed,1,sizeof(int)*(pixelCount)/32);
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(runningVar,0,(pixelCount)*sizeof(short));
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(runningMean,1,(pixelCount)*sizeof(short));
     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_UPDATEEWRVARIANCES16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_updateEWRVarianceS16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->newLuma_InArrID1=  INBUF0;
    fp0->mask32packed_InArrID2 =  INBUF1;
    fp0->runningVar_InOutArrID1 = OUTBUF0;
    fp0->runningMean_InOutArrID2 = OUTBUF1;
    fp0->weight = weight;
    fp0->pixelCount = pixelCount;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc ,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

int C6accel_VLIB_updateEWRVarianceS32 ( C6accel_Handle hC6accel,
                                  int *restrict runningVar,
                                  int *restrict runningMean,
                                  const unsigned char  *restrict newLuma,
                                  const unsigned int   *restrict mask32packed,
                                  const short                  weight,
                                  int                  pixelCount
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_updateEWRVarianceS32_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_updateEWRVarianceS32_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 2;
    inOutBufDesc.numBufs = 2;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(newLuma,0,(pixelCount)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(mask32packed,1,sizeof(int)*(pixelCount)/32);
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(runningVar,0,(pixelCount)*sizeof(int));
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(runningMean,1,(pixelCount)*sizeof(int));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_UPDATEEWRVARIANCES32_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_updateEWRVarianceS32_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->newLuma_InArrID1=  INBUF0;
    fp0->mask32packed_InArrID2 =  INBUF1;
    fp0->runningVar_InOutArrID1 = OUTBUF0;
    fp0->runningMean_InOutArrID2 = OUTBUF1;
    fp0->weight = weight;
    fp0->pixelCount = pixelCount;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}



int C6accel_VLIB_updateUWRVarianceS16 ( C6accel_Handle hC6accel,
                                  short * updatedVar,
                                  short * previousVar,
                                  short * updatedMean,
                                  short * previousMean,
                                  const unsigned char  * newestData,
                                  const unsigned int   *restrict newestMask32packed,
                                  const unsigned int   *restrict oldestMask32packed,
                                  int                  pixelCount,
                                  int                  frameCount
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
     XDM1_BufDesc               inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_updateUWRVarianceS16_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_updateUWRVarianceS16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 5;
    inOutBufDesc.numBufs = 2;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(previousMean,0,(pixelCount)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(previousVar,1,(pixelCount)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(newestData,2,(pixelCount)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(newestMask32packed,3,sizeof(int)*(pixelCount)/32);
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(oldestMask32packed,4,sizeof(int)*(pixelCount)/32);

    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(updatedVar,0,(pixelCount)*sizeof(short));
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(updatedMean,1,(pixelCount)*sizeof(short));
    
    /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_UPDATEUWRVARIANCES16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_updateUWRVarianceS16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->previousMean_InArrID1=  INBUF0;
    fp0->previousVar_InArrID2=  INBUF1;
    fp0->newestData_InArrID3=  INBUF2;
    fp0->newestMask32packed_InArrID4 =  INBUF3;
    fp0->oldestMask32packed_InArrID5 =  INBUF4;
    fp0->updatedVar_InOutArrID1 = OUTBUF0;
    fp0->updatedMean_InOutArrID2 = OUTBUF1;
    fp0->frameCount = frameCount;
    fp0->pixelCount = pixelCount;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc ,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

int C6accel_VLIB_subtractBackgroundS16 ( C6accel_Handle hC6accel,
                                  unsigned int  *restrict mask32packed,
                                  unsigned char  * newLuma,
                                  short * runningMean,
                                  short * runningVar,
                                  short thresholdGlobal,
                                  short thresholdFactor,
                                  unsigned int                  pixelCount
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
     XDM1_BufDesc               inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_subtractBackgroundS16_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_subtractBackgroundS16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 3;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(newLuma,0,(pixelCount)*sizeof(char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(runningMean,1,(pixelCount)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(runningVar,2,(pixelCount)*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(mask32packed,0,sizeof(int)*(pixelCount)/32);
    
    /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_SUBTRACTBACKGROUNDS16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_subtractBackgroundS16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->newLuma_InArrID1=  INBUF0;
    fp0->runningMean_InArrID2=  INBUF1;
    fp0->runningVar_InArrID3=  INBUF2;
    fp0->mask32packed_OutArrID1 = OUTBUF0;
    fp0->thresholdGlobal = thresholdGlobal;
    fp0->thresholdFactor = thresholdFactor;
    fp0->PixelCount = pixelCount;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc,&outBufDesc, NULL ,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

int C6accel_VLIB_subtractBackgroundS32 ( C6accel_Handle hC6accel,
                                  unsigned int  *restrict mask32packed,
                                  unsigned char  * newLuma,
                                  int * runningMean,
                                  int * runningVar,
                                  int thresholdGlobal,
                                  int thresholdFactor,
                                  unsigned int                  pixelCount
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
     XDM1_BufDesc               inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_subtractBackgroundS32_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_subtractBackgroundS32_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 3;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(newLuma,0,(pixelCount)*sizeof(char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(runningMean,1,(pixelCount)*sizeof(int));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(runningVar,2,(pixelCount)*sizeof(int));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(mask32packed,0,sizeof(int)*(pixelCount)/32);
    
    /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_SUBTRACTBACKGROUNDS32_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_subtractBackgroundS32_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->newLuma_InArrID1 =  INBUF0;
    fp0->runningMean_InArrID2 =  INBUF1;
    fp0->runningVar_InArrID3 =  INBUF2;
    fp0->mask32packed_OutArrID1 = OUTBUF0;
    fp0->thresholdGlobal = thresholdGlobal;
    fp0->thresholdFactor = thresholdFactor;
    fp0->PixelCount = pixelCount;
    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc,&outBufDesc, NULL ,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

int C6accel_VLIB_updateUWRMeanS16 ( C6accel_Handle hC6accel,
                                  short *restrict updatedMean,
                                  short *restrict previousMean,
                                  unsigned char  * newestData,
                                  unsigned char  * oldestData,
                                  unsigned int   * newestMask32packed,
                                  unsigned int   * oldestMask32packed,
                                  int                  pixelCount,
                                  unsigned char frameCount
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_updateUWRMeanS16_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_updateUWRMeanS16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 4;
    inOutBufDesc.numBufs = 2;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(newestData,0,(pixelCount)*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(oldestData,1,(pixelCount)*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(newestMask32packed,2,sizeof(int)*(pixelCount)/32);
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(oldestMask32packed,3,sizeof(int)*(pixelCount)/32);
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(updatedMean,0,(pixelCount)*sizeof(short));
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(previousMean,1,(pixelCount)*sizeof(short));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_UPDATEUWRMEANS16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_updateUWRMeanS16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->newestData_InArrID1=  INBUF0;
    fp0->oldestData_InArrID2=  INBUF1;
    fp0->newestMask32packed_InArrID3 =  INBUF2;
    fp0->oldestMask32packed_InArrID4 =  INBUF3;
    fp0->updatedMean_InOutArrID1 = OUTBUF0;
    fp0->previousMean_InOutArrID2 = OUTBUF1;
    fp0->pixelCount = pixelCount;
    fp0->frameCount = frameCount;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}



/*Function added by Nicolas G*/

/*
 int C6accel_VLIB_xyGradientsAndMagnitude(  C6accel_Handle hC6accel,
				unsigned char * restrict pInBlk, 
				signed short * restrict pBufGradX,
				signed short * restrict pBufGradY,
				signed short * restrict pBufMag,
				unsigned short width, 
				unsigned short height
                              );

Return value:  API returns status of the function call.
               ==1 Pass
               <0  Fail

*/
int C6accel_VLIB_xyGradientsAndMagnitude
(	C6accel_Handle hC6accel,
				unsigned char * restrict pInBlk, 
				signed short * restrict pBufGradX,
				signed short * restrict pBufGradY,
				signed short * restrict pBufMag,
				unsigned short width, 
				unsigned short height   
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;

    int status;
    /* Define pointer to function parameter structure */
    VLIB_xyGradientsAndMagnitude_Params      *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size

    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_xyGradientsAndMagnitude_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 1;
    outBufDesc.numBufs = 3;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pInBlk,0,width * height*sizeof(char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pBufGradX,0,width * height*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pBufGradY,1,width * height*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pBufMag,2,width * height*sizeof(short));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_XYGRADIENTSANDMAGNITUDE_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_xyGradientsAndMagnitude_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pInBlk_InArrID1= INBUF0;
    fp0->pBufGradX_OutArrID1= OUTBUF0;
    fp0->pBufGradY_OutArrID2= OUTBUF1;
    fp0->pBufMag_OutArrID3= OUTBUF2;
    fp0->width= width;
    fp0->height= height;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}


/*Function added by Nicolas G*/

/*
 int C6accel_nonMaximumSuppressionCanny(  C6accel_Handle hC6accel,
				    short * restrict pInMag, 
	                            short * restrict pInGradX, 
	                            short * restrict pInGradY, 
	                            unsigned char * restrict pOut,
	                            unsigned short width,
                                    unsigned short pitch,
	                            unsigned short height
                              );

Return value:  API returns status of the function call.
               ==1 Pass
               <0  Fail

*/
int C6accel_VLIB_nonMaximumSuppressionCanny
(   C6accel_Handle hC6accel,
				    short * restrict pInMag, 
	                            short * restrict pInGradX, 
	                            short * restrict pInGradY, 
	                            unsigned char * restrict pOut,
	                            unsigned short width,
                                    unsigned short pitch,
	                            unsigned short height
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;

    int status;
    /* Define pointer to function parameter structure */
    VLIB_nonMaximumSuppressionCanny_Params      *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size

    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_nonMaximumSuppressionCanny_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 3;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pInMag,0,pitch * height*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pInGradX,1,pitch * height*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pInGradY,2,pitch * height*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pOut,0,pitch * height*sizeof(char));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_NONMAXIMUMSUPPRESSIONCANNY_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_nonMaximumSuppressionCanny_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->indata_InArrID1= INBUF0;
    fp0->indata_InArrID2= INBUF1;
    fp0->indata_InArrID3= INBUF2;
    fp0->outdata_OutArrID1= OUTBUF0;
    fp0->Col= width;
    fp0->Pitch= pitch;
    fp0->Row= height;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}



/*Function added by Nicolas G*/
/*

 int C6accel_VLIB_doublethresholding(  C6accel_Handle hC6accel,
				signed short * restrict pInMag, 
				unsigned char *edgeMap, 
				unsigned int * restrict strongEdgeListPtr,
				int * numStrongEdges,
				unsigned short width, 
				unsigned short pitch, 
				unsigned short height,
				unsigned char loThresh,
				unsigned char hiThresh,
				unsigned int  block_offset
                              );

Return value:  API returns status of the function call.
               ==1 Pass
               <0  Fail

*/
int C6accel_VLIB_doublethresholding
(   C6accel_Handle hC6accel,
				signed short * restrict pInMag, 
				unsigned char *edgeMap, 
				unsigned int * restrict strongEdgeListPtr,
				int * numStrongEdges,
				unsigned short width, 
				unsigned short pitch, 
				unsigned short height,
				unsigned char loThresh,
				unsigned char hiThresh,
				unsigned int  block_offset
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;

    int status;
    /* Define pointer to function parameter structure */
    VLIB_doublethresholding_Params      *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size

    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_doublethresholding_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 1;
    outBufDesc.numBufs = 3;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pInMag,0,width * height*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(edgeMap,0,width * height*sizeof(char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(strongEdgeListPtr,1,width * height*sizeof(int));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(numStrongEdges,2,1*sizeof(int));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_DOUBLETHRESHOLDING_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_doublethresholding_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->indata_InArrID1= INBUF0;
    fp0->outdata_OutArrID1= OUTBUF0;
    fp0->outdata_OutArrID2= OUTBUF1;
    fp0->outdata_OutArrID3= OUTBUF2;
    fp0->Col= width;
    fp0->Pitch= pitch;
    fp0->Row= height;
    fp0->loThresh= loThresh;
    fp0->hiThresh= hiThresh;
    fp0->block_offset= block_offset;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}



/*Function added by Nicolas G*/
/*


 int C6accel_VLIB_edgeRelaxation(  C6accel_Handle hC6accel,
					unsigned char *edgeMap, 
					unsigned int * restrict strongEdgeListPtr,
					int * numStrongEdges,

					unsigned short width
                              );

Return value:  API returns status of the function call.
               ==1 Pass

               <0  Fail


*/
int C6accel_VLIB_edgeRelaxation
(   C6accel_Handle hC6accel,
					unsigned char *edgeMap, 
					unsigned int * restrict strongEdgeListPtr,
					int * numStrongEdges,
					unsigned short width
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;

    int status;
    /* Define pointer to function parameter structure */
    VLIB_edgeRelaxation_Params      *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size

    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_edgeRelaxation_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 2;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(strongEdgeListPtr,0,width * width*sizeof(int));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(numStrongEdges,1,1*sizeof(int));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(edgeMap,0,width * width*sizeof(char));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_EDGERELAXATION_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_edgeRelaxation_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->indata_InArrID1= INBUF0;
    fp0->indata_InArrID2= INBUF1;
    fp0->outdata_OutArrID1= OUTBUF0;
    fp0->Col= width;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }


    RELEASE_CODEC_ENGINE;

    return status;

}


/*Function added by Nicolas G*/
/*
 int C6accel_VLIB_cannyEdge
(   C6accel_Handle hC6accel,
					const unsigned char * restrict input,
					unsigned char loThresh,
					unsigned char hiThresh,
					 short * restrict inout1, 
					 short * restrict inout2, 
					 short * restrict inout3, 
					unsigned char * restrict output, 
					unsigned short width,
					unsigned short height
                              );
Return value:  API returns status of the function call.
               ==1 Pass
               <0  Fail

Optimized function to allow the user to make only 1 call to c6accel instead of 5

*/
int C6accel_VLIB_cannyEdge
(   C6accel_Handle hC6accel,
					const unsigned char * restrict input,
					unsigned char loThresh,
					unsigned char hiThresh,
					 short * restrict inout1, 
					 short * restrict inout2, 
					 short * restrict inout3, 
					unsigned char * restrict output, 
					unsigned short width,
					unsigned short height
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;

    int status;
    /* Define pointer to function parameter structure */
    VLIB_cannyEdge_Params      *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size

    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_cannyEdge_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 1;
    outBufDesc.numBufs = 4;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(input,0,width*height*sizeof(char));
    Memory_cacheWbInv((Ptr)inout1, width*height*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(inout1,0,width*height*sizeof(short));
    Memory_cacheWbInv((Ptr)inout2, width*height*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(inout2,1,width*height*sizeof(short));
    Memory_cacheWbInv((Ptr)inout3, width*height*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(inout3,2,width*height*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(output,3,width*height*sizeof(char));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_CANNYEDGE_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_cannyEdge_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->input_InArrID1= INBUF0;
    fp0->loThresh= loThresh;
    fp0->hiThresh= hiThresh;
    fp0->workBuf1_OutArrID1= OUTBUF0;
    fp0->workBuf2_OutArrID2= OUTBUF1;
    fp0->workBuf3_OutArrID3= OUTBUF2;
    fp0->output_OutArrID4= OUTBUF3;
    fp0->width= width;
    fp0->height= height;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

/*Function added by Nicolas G*/
/*

 int C6accel_VLIB_packMask32(  C6accel_Handle hC6accel,
					unsigned char * maskImage, 
					unsigned int * restrict mask32packed,
					int pixelCount,
			);

Return value:  API returns status of the function call.
               ==1 Pass
               <0  Fail


*/

 int C6accel_VLIB_packMask32(  C6accel_Handle hC6accel,
					const unsigned char * restrict maskImage, 
					unsigned int * restrict mask32packed,
					unsigned int pixelCount
			)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;

    int status;
    /* Define pointer to function parameter structure */
    VLIB_packMask32_Params      *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size

    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_packMask32_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 1;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(maskImage,0,pixelCount*sizeof(char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(mask32packed,0,(pixelCount/32)*sizeof(int));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_PACKMASK32_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_packMask32_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->indata_InArrID1= INBUF0;
    fp0->outdata_OutArrID1= OUTBUF0;
    fp0->pixelCount= pixelCount;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}



/*Function added by Nicolas G*/
/*

 int C6accel_VLIB_unpackMask32(  C6accel_Handle hC6accel,
					unsigned char * maskImage, 
					unsigned int * restrict mask32packed,
					int pixelCount,
			);

Return value:  API returns status of the function call.
               ==1 Pass
               <0  Fail


*/

 int C6accel_VLIB_unpackMask32(  C6accel_Handle hC6accel,
					const unsigned int * restrict mask32packed,
					unsigned char * restrict maskImage, 
					unsigned int pixelCount
			)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;

    int status;
    /* Define pointer to function parameter structure */
    VLIB_unpackMask32_Params      *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size

    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_unpackMask32_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 1;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(mask32packed,0,(pixelCount/32)*sizeof(int));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(maskImage,0,pixelCount*sizeof(char));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_UNPACKMASK32_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_unpackMask32_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->indata_InArrID1= INBUF0;
    fp0->outdata_OutArrID1= OUTBUF0;
    fp0->pixelCount= pixelCount;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}




/*Function added by Nicolas G*/
/*

 int C6accel_VLIB_dilate_bin_square(  C6accel_Handle hC6accel,
					const unsigned char * restrict input, 
					unsigned char * restrict output,
					unsigned int cols,
					unsigned int pitch
			);

Return value:  API returns status of the function call.
               ==1 Pass
               <0  Fail


*/

 int C6accel_VLIB_dilate_bin_square(  C6accel_Handle hC6accel,
					const unsigned char * restrict input, 
					unsigned char * restrict output,
					unsigned int cols,
					unsigned int pitch
			)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;

    int status;
    /* Define pointer to function parameter structure */
    VLIB_dilate_bin_square_Params      *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size

    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_dilate_bin_square_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 1;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(input,0,(cols/pitch+2)*pitch*sizeof(char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(output,0,cols*sizeof(char));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_DILATE_BIN_SQUARE_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_dilate_bin_square_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->indata_InArrID1= INBUF0;
    fp0->outdata_OutArrID1= OUTBUF0;
    fp0->Col= cols;
    fp0->Pitch= pitch;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

/*Function added by Nicolas G*/
/*

 int C6accel_VLIB_dilate_bin_cross(  C6accel_Handle hC6accel,
					const unsigned char * restrict input, 
					unsigned char * restrict output,
					unsigned int cols,
					unsigned int pitch
			);

Return value:  API returns status of the function call.
               ==1 Pass
               <0  Fail


*/

 int C6accel_VLIB_dilate_bin_cross(  C6accel_Handle hC6accel,
					const unsigned char * restrict input, 
					unsigned char * restrict output,
					unsigned int cols,
					unsigned int pitch
			)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;

    int status;
    /* Define pointer to function parameter structure */
    VLIB_dilate_bin_cross_Params      *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size

    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_dilate_bin_cross_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 1;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(input,0,(cols/pitch+2)*pitch*sizeof(char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(output,0,cols*sizeof(char));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_DILATE_BIN_CROSS_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_dilate_bin_cross_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->indata_InArrID1= INBUF0;
    fp0->outdata_OutArrID1= OUTBUF0;
    fp0->Col= cols;
    fp0->Pitch= pitch;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}



/*Function added by Nicolas G*/
/*
 int C6accel_VLIB_erode_bin_square(  C6accel_Handle hC6accel,
					const unsigned char * restrict input, 
					unsigned char * restrict output,
					unsigned int cols,
					unsigned int pitch
			);

Return value:  API returns status of the function call.
               ==1 Pass
               <0  Fail


*/

 int C6accel_VLIB_erode_bin_square(  C6accel_Handle hC6accel,
					const unsigned char * restrict input, 
					unsigned char * restrict output,
					unsigned int cols,
					unsigned int pitch
			)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;

    int status;
    /* Define pointer to function parameter structure */
    VLIB_erode_bin_square_Params      *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size

    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_erode_bin_square_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 1;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(input,0,(cols/pitch+2)*pitch*sizeof(char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(output,0,cols*sizeof(char));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_ERODE_BIN_SQUARE_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_erode_bin_square_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->indata_InArrID1= INBUF0;
    fp0->outdata_OutArrID1= OUTBUF0;
    fp0->Col= cols;
    fp0->Pitch= pitch;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}


/*Function added by Nicolas G*/
/*

 int C6accel_VLIB_erode_bin_cross(  C6accel_Handle hC6accel,
					const unsigned char * restrict input, 
					unsigned char * restrict output,
					unsigned int cols,
					unsigned int pitch
			);

Return value:  API returns status of the function call.
               ==1 Pass
               <0  Fail


*/

 int C6accel_VLIB_erode_bin_cross(  C6accel_Handle hC6accel,
					const unsigned char * restrict input, 
					unsigned char * restrict output,
					unsigned int cols,
					unsigned int pitch)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;

    int status;
    /* Define pointer to function parameter structure */
    VLIB_erode_bin_cross_Params      *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size

    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_erode_bin_cross_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 1;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(input,0,(cols/pitch+2)*pitch*sizeof(char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(output,0,cols*sizeof(char));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_ERODE_BIN_CROSS_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_erode_bin_cross_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->indata_InArrID1= INBUF0;
    fp0->outdata_OutArrID1= OUTBUF0;
    fp0->Col= cols;
    fp0->Pitch= pitch;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

/*Function added by Nicolas G*/
/*

  int C6accel_VLIB_erode_bin_square_opt(  C6accel_Handle hC6accel,
					const unsigned char * restrict input,
					unsigned int * restrict inout1, 
					unsigned int * restrict inout2, 
					unsigned char * restrict output, 
					unsigned int width,
					unsigned int height
			);

Return value:  API returns status of the function call.
               ==1 Pass
               <0  Fail

optimization for 1 call to pack dilateBinSquare unpack

*/

 int C6accel_VLIB_erode_bin_square_opt(  C6accel_Handle hC6accel,
					const unsigned char * restrict input,
					unsigned int * restrict inout1, 
					unsigned int * restrict inout2, 
					unsigned char * restrict output, 
					unsigned int width,
					unsigned int height
			)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;

    int status;
    /* Define pointer to function parameter structure */
    VLIB_erode_bin_square_opt_Params      *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size

    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_erode_bin_square_opt_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 1;
    outBufDesc.numBufs = 3;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(input,0,width*height*sizeof(char));
    Memory_cacheWbInv((Ptr)inout1, (width*height/32)*sizeof(int));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(inout1,0,(width*height/32)*sizeof(int));
    Memory_cacheWbInv((Ptr)inout2, (width*height/32)*sizeof(int));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(inout2,1,(width*height/32)*sizeof(int));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(output,2,width*height*sizeof(char));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_ERODE_BIN_SQUARE_OPT_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_erode_bin_square_opt_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->indata_InArrID1= INBUF0;
    fp0->outdata_OutArrID1= OUTBUF0;
    fp0->outdata_OutArrID2= OUTBUF1;
    fp0->outdata_OutArrID3= OUTBUF2;
    fp0->width= width;
    fp0->height= height;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

/*Function added by Nicolas G*/
/*
C6accel_VLIB_dilate_bin_square_opt(  C6accel_Handle hC6accel,
					const unsigned char * restrict input,

					unsigned int * restrict inout1, 
					unsigned int * restrict inout2, 
					unsigned char * restrict output, 
					unsigned int width,
					unsigned int height
			);

Return value:  API returns status of the function call.
               ==1 Pass
               <0  Fail

optimization for 1 call to pack dilateBinSquare unpack

*/

 int C6accel_VLIB_dilate_bin_square_opt(  C6accel_Handle hC6accel,
					const unsigned char * restrict input,
					unsigned int * restrict inout1, 
					unsigned int * restrict inout2, 
					unsigned char * restrict output, 
					unsigned int width,
					unsigned int height
			)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;

    int status;
    /* Define pointer to function parameter structure */
    VLIB_dilate_bin_square_opt_Params      *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size

    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_dilate_bin_square_opt_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 1;
    outBufDesc.numBufs = 3;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(input,0,width*height*sizeof(char));
    Memory_cacheWbInv((Ptr)inout1, (width*height/32)*sizeof(int));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(inout1,0,(width*height/32)*sizeof(int));
    Memory_cacheWbInv((Ptr)inout2, (width*height/32)*sizeof(int));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(inout2,1,(width*height/32)*sizeof(int));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(output,2,width*height*sizeof(char));

     /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_DILATE_BIN_SQUARE_OPT_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_dilate_bin_square_opt_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->indata_InArrID1= INBUF0;
    fp0->outdata_OutArrID1= OUTBUF0;
    fp0->outdata_OutArrID2= OUTBUF1;
    fp0->outdata_OutArrID3= OUTBUF2;
    fp0->width= width;
    fp0->height= height;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

Int C6accel_VLIB_convertUYVYint_to_YUVpl(
    C6accel_Handle hC6accel,
    const unsigned char * yc,    
    int width,
    int pitch,
    int height, 
    unsigned char * restrict y,
    unsigned char * restrict cr,
    unsigned char * restrict cb  
)
{

    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_convertUYVYint_to_YUVpl_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 1;
    outBufDesc.numBufs = 3;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(yc,0,2*height*pitch*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL( y,0,height*width*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(cr,1,height*width/2*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(cb,2,height*width/2*sizeof(unsigned char));
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_convertUYVYint_to_YUVpl_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_CONVERTUYVYINT_TO_YUVPL_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_convertUYVYint_to_YUVpl_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->yc_InArrID1     = INBUF0;
    fp0->y_OutArrID1     = OUTBUF0;
    fp0->cr_OutArrID2    = OUTBUF1;
    fp0->cb_OutArrID3    = OUTBUF2;
    fp0->pitch           = pitch;
    fp0->height          = height;
    fp0->width           = width;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_convertUYVYint_to_YUV420pl(
    C6accel_Handle hC6accel,
    const unsigned char * yc,    
    int width,
    int pitch,
    int height, 
    unsigned char * restrict y,
    unsigned char * restrict cr,
    unsigned char * restrict cb  
)
{

    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_convertUYVYint_to_YUV420pl_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 1;
    outBufDesc.numBufs = 3;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(yc,0,2*height*pitch*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL( y,0,height*width*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(cr,1,height*width/4*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(cb,2,height*width/4*sizeof(unsigned char));
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_convertUYVYint_to_YUV420pl_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_CONVERTUYVYINT_TO_YUV420PL_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_convertUYVYint_to_YUV420pl_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->yc_InArrID1     = INBUF0;
    fp0->y_OutArrID1     = OUTBUF0;
    fp0->cr_OutArrID2    = OUTBUF1;
    fp0->cb_OutArrID3    = OUTBUF2;
    fp0->pitch           = pitch;
    fp0->height          = height;
    fp0->width           = width;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_convertUYVYint_to_HSLpl(
    C6accel_Handle hC6accel,
    const unsigned char * yc,    
    int width,
    int pitch,
    int height, 
    short *coeff,
    unsigned short *div_table,
    unsigned short * restrict H,
    unsigned char * restrict S,
    unsigned char * restrict L  
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_convertUYVYint_to_HSLpl_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 3;
    outBufDesc.numBufs = 3;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(yc,0,2*height*pitch*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(coeff,1,5*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(div_table,2,510*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(H,0,height*width*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(S,1,height*width*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(L,2,height*width*sizeof(unsigned char));
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_convertUYVYint_to_HSLpl_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_CONVERTUYVYINT_TO_HSLPL_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_convertUYVYint_to_HSLpl_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->yc_InArrID1        = INBUF0;
    fp0->coeff_InArrID2     = INBUF1;
    fp0->div_table_InArrID3 = INBUF2;
    fp0->H_OutArrID1        = OUTBUF0;
    fp0->S_OutArrID2        = OUTBUF1;
    fp0->L_OutArrID3        = OUTBUF2;
    fp0->pitch              = pitch;
    fp0->height             = height;
    fp0->width              = width;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {
       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;
    return status;  

}

Int C6accel_VLIB_convertUYVYint_to_LABpl(
    C6accel_Handle hC6accel,
    const unsigned char * yc,    
    int width,
    int pitch,
    int height, 
    short *coeff,
    float *whitePoint,
    float *restrict L,
    float *restrict a,
    float *restrict b  
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_convertUYVYint_to_LABpl_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 3;
    outBufDesc.numBufs = 3;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(yc,0,2*height*pitch*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(coeff,1,5*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(whitePoint,2,3*sizeof(float));
    
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(L,0,height*width*sizeof(float));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(a,1,height*width*sizeof(float));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(b,2,height*width*sizeof(float));
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_convertUYVYint_to_LABpl_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_CONVERTUYVYINT_TO_LABPL_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_convertUYVYint_to_LABpl_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->yc_InArrID1         = INBUF0;
    fp0->coeff_InArrID2      = INBUF1;
    fp0->whitePoint_InArrID3 = INBUF2;
    fp0->L_OutArrID1         = OUTBUF0;
    fp0->a_OutArrID2         = OUTBUF1;
    fp0->b_OutArrID3         = OUTBUF2;
    fp0->pitch               = pitch;
    fp0->height              = height;
    fp0->width               = width;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {
       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;

           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;
    return status;  

}


Int C6accel_VLIB_convertUYVYint_to_RGBpl(
    C6accel_Handle hC6accel,
    const unsigned char * yc,    
    int width,
    int pitch,
    int height, 
    short *coeff,
    unsigned char *restrict R,
    unsigned char *restrict G,
    unsigned char *restrict B  
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_convertUYVYint_to_RGBpl_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 3;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(yc,0,2*height*pitch*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(coeff,1,5*sizeof(short));
    
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(R,0,height*width*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(G,1,height*width*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(B,2,height*width*sizeof(unsigned char));
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_convertUYVYint_to_RGBpl_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_CONVERTUYVYINT_TO_RGBPL_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_convertUYVYint_to_RGBpl_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->yc_InArrID1         = INBUF0;
    fp0->coeff_InArrID2      = INBUF1;

    fp0->R_OutArrID1         = OUTBUF0;
    fp0->G_OutArrID2         = OUTBUF1;
    fp0->B_OutArrID3         = OUTBUF2;

    fp0->pitch               = pitch;
    fp0->height              = height;
    fp0->width               = width;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {
       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;

           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;
    return status;  

}


Int C6accel_VLIB_convertUYVYint_to_LABpl_LUT(
    C6accel_Handle hC6accel,
    const unsigned char * yc,    
    int width,
    int pitch,
    int height, 
    int d,
    unsigned short *restrict LabLUT,
    unsigned short *restrict L,
    unsigned short  *restrict a,
    unsigned short  *restrict b  
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_convertUYVYint_to_LABpl_LUT_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 3;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(yc,0,2*height*pitch*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(LabLUT,1,(39304/2)*sizeof(short));
        
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(L,0,height*width*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(a,1,height*width*sizeof(unsigned short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(b,2,height*width*sizeof(unsigned short));
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_convertUYVYint_to_LABpl_LUT_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_CONVERTUYVYINT_TO_LABPL_LUT_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_convertUYVYint_to_LABpl_LUT_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->yc_InArrID1         = INBUF0;
    fp0->LabLUT_InArrID2      = INBUF1;
    
    fp0->L_OutArrID1         = OUTBUF0;
    fp0->a_OutArrID2         = OUTBUF1;
    fp0->b_OutArrID3         = OUTBUF2;
    fp0->d                   = d;
    fp0->pitch               = pitch;
    fp0->height              = height;
    fp0->width               = width;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {
       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;

           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;
    return status;  

}

Int C6accel_VLIB_initUYVYint_to_LABpl_LUT(
    C6accel_Handle hC6accel,
    int d,
    short *coeff,
    float *whitePoint,
    unsigned short *restrict LabLUT
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_initUYVYint_to_LABpl_LUT_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(coeff,0,5*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(whitePoint,1,3*sizeof(float));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(LabLUT,0,39304/2*sizeof(short));
        
      
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_initUYVYint_to_LABpl_LUT_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_INITUYVYINT_TO_LABPL_LUT_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_initUYVYint_to_LABpl_LUT_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->coeff_InArrID1         = INBUF0;
    fp0->whitePoint_InArrID2      = INBUF1;
    
    fp0->Lab_OutArrID1         = OUTBUF0;
    fp0->d               = d;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {
       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;

           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;
    return status;  

}

Int C6accel_VLIB_convertUYVYsemipl_to_YUVpl(
    C6accel_Handle hC6accel,
    const unsigned char * crcb,    
    int width,
    int pitch,
    int height, 
    unsigned char *restrict cr,
    unsigned char  *restrict cb
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_convertUYVYsemipl_to_YUVpl_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 1;
    outBufDesc.numBufs = 2;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(crcb,0,height*pitch*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(cr,0,height*width/2*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(cb,1,height*width/2*sizeof(unsigned char));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_convertUYVYsemipl_to_YUVpl_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_CONVERTUYVYSEMIPL_TO_YUVPL_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_convertUYVYsemipl_to_YUVpl_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->crcb_InArrID1        = INBUF0;
    fp0->cr_OutArrID1         = OUTBUF0;
    fp0->cb_OutArrID2         = OUTBUF1;
    fp0->pitch                = pitch;
    fp0->height               = height;
    fp0->width                = width;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {
       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;

           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;
    return status;  

}

Int C6accel_VLIB_convertUYVYpl_to_YUVint(
    C6accel_Handle hC6accel,
    unsigned char * restrict y,
    unsigned char * restrict cr,
    unsigned char * restrict cb, 
    int width,
    int pitch,
    int height,
    const unsigned char * yc   
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_convertUYVYpl_to_YUVint_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 3;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(yc,0,2*height*width*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL( y,0,height*pitch*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(cr,1,height*width/2*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(cb,2,height*width/2*sizeof(unsigned char));
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_convertUYVYpl_to_YUVint_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_CONVERTUYVYPL_TO_YUVINT_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_convertUYVYpl_to_YUVint_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->yc_OutArrID1     = OUTBUF0;
    fp0->y_InArrID1       = INBUF0;
    fp0->cr_InArrID2      = INBUF1;
    fp0->cb_InArrID3      = INBUF2;
    fp0->pitch            = pitch;
    fp0->height           = height;
    fp0->width            = width;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_extractLumaFromUYUV(
    C6accel_Handle hC6accel,
    const unsigned char * inputImage,    
    int inputWidth,
    int inputPitch,
    int inputHeight, 
    unsigned char * restrict outputImage
      
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_extractLumaFromUYUV_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 1;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(inputImage,0,2*inputHeight*inputPitch*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(outputImage,0,inputHeight*inputWidth*sizeof(unsigned char));
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_extractLumaFromUYUV_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_EXTRACTLUMAFROMUYUV_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_extractLumaFromUYUV_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->inputImage_InArrID1     = INBUF0;
    fp0->outputImage_OutArrID1       = OUTBUF0;
    
    fp0->inputPitch       = inputPitch;
    fp0->inputHeight      = inputHeight;
    fp0->inputWidth       = inputWidth;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_integralImage8(
    C6accel_Handle hC6accel,
    char * pIn,    
    int inCols,
    int inRows,
    unsigned int * restrict pLastLine,
    unsigned int * restrict pOut 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_integralImage8_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 1;
    outBufDesc.numBufs = 2;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pIn,0,inCols*inRows*sizeof( char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL( pLastLine,1,inCols*sizeof(int));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pOut,0,inCols*inRows*sizeof(int));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_integralImage8_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_INTEGRALIMAGE8_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_integralImage8_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pOut_OutArrID1      = OUTBUF0;
    fp0->pLastLine_OutArrID2 = OUTBUF1;
    fp0->pIn_InArrID1        = INBUF0;
    fp0->inCols              = inCols;
    fp0->inRows              = inRows;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_integralImage16(
    C6accel_Handle hC6accel,
    short * pIn,    
    int inCols,
    int inRows,
    unsigned int * restrict pLastLine,
    unsigned int * restrict pOut 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_integralImage16_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 1;
    outBufDesc.numBufs = 2;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pIn,0,inCols*inRows*sizeof( short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL( pLastLine,1,inCols*sizeof(int));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pOut,0,inCols*inRows*sizeof(int));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_integralImage16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_INTEGRALIMAGE16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_integralImage16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pOut_OutArrID1      = OUTBUF0;
    fp0->pLastLine_OutArrID2 = OUTBUF1;
    fp0->pIn_InArrID1        = INBUF0;
    fp0->inCols              = inCols;
    fp0->inRows              = inRows;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}


Int C6accel_VLIB_imagePyramid8(
    C6accel_Handle hC6accel,
    char * pIn,    
    int inCols,
    int inRows,
    char * pOut 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_imagePyramid8_Params *fp0;
    XDAS_Int8                  *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 1;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pIn,0,inCols*inRows*sizeof(char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pOut,0,(((inCols*inRows)*21)/64)*sizeof(char));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_imagePyramid8_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_IMAGEPYRAMID8_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_imagePyramid8_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pOut_OutArrID1      = OUTBUF0;
    fp0->pIn_InArrID1        = INBUF0;
    fp0->inCols              = inCols;
    fp0->inRows              = inRows;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_imagePyramid16(
    C6accel_Handle hC6accel,
    unsigned short * pIn,    
    int inCols,
    int inRows,
    unsigned short * restrict pOut 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_imagePyramid16_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg */
    inBufDesc.numBufs  = 1;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pIn,0,inCols*inRows*sizeof( short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pOut,0,inCols*inRows*sizeof(int));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_imagePyramid16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_IMAGEPYRAMID16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_imagePyramid16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pOut_OutArrID1      = OUTBUF0;
    fp0->pIn_InArrID1        = INBUF0;
    fp0->inCols              = inCols;
    fp0->inRows              = inRows;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_gauss5x5PyramidKernel_8(
    C6accel_Handle hC6accel,
    char  *pIn, 
    short *pB,   
    int inCols,
    int pitch,
    int inRows,
    char * pOut 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_gauss5x5PyramidKernel_8_Params *fp0;
    XDAS_Int8                  *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pIn,0,5 * (inCols+4)*sizeof(char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pB,1,inCols*5*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pOut,0,inCols/2*sizeof(char));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_gauss5x5PyramidKernel_8_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_GAUSS5X5PYRAMIDKERNEL_8_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_gauss5x5PyramidKernel_8_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pOut_OutArrID1      = OUTBUF0;
    fp0->pIn_InArrID1        = INBUF0;
    fp0->pB_InArrID2         = INBUF1;
    fp0->inCols              = inCols;
    fp0->inRows              = inRows;
    fp0->pitch               = pitch;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_gauss5x5PyramidKernel_16(
    C6accel_Handle hC6accel,
    unsigned short * pIn, 
    unsigned int * pB,    
    int inCols,
    int pitch,
    int inRows,
    unsigned short * restrict pOut 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_gauss5x5PyramidKernel_16_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pIn,0,5 * (inCols+4)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pB,1,inCols*5*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pOut,0,inCols/2*sizeof(short));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_gauss5x5PyramidKernel_16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_GAUSS5X5PYRAMIDKERNEL_16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_gauss5x5PyramidKernel_16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pOut_OutArrID1      = OUTBUF0;
    fp0->pIn_InArrID1        = INBUF0;
    fp0->pB_InArrID2         = INBUF1;
    fp0->inCols              = inCols;
    fp0->inRows              = inRows;
    fp0->pitch               = pitch;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {
       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_gradientH5x5PyramidKernel_8(
    C6accel_Handle hC6accel,
    char  *pIn, 
    short *pB,   
    int inCols,
    int pitch,
    int inRows,
    char * pOut 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_gradientH5x5PyramidKernel_8_Params *fp0;
    XDAS_Int8                  *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pIn,0,5 * (inCols+4)*sizeof(char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pB,1,inCols*5*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pOut,0,inCols/2*sizeof(char));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_gradientH5x5PyramidKernel_8_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_GRADIENTH5X5PYRAMIDKERNEL_8_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_gradientH5x5PyramidKernel_8_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pOut_OutArrID1      = OUTBUF0;
    fp0->pIn_InArrID1        = INBUF0;
    fp0->pB_InArrID2         = INBUF1;
    fp0->inCols              = inCols;
    fp0->inRows              = inRows;
    fp0->pitch               = pitch;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_gradientV5x5PyramidKernel_8(
    C6accel_Handle hC6accel,
    char  *pIn, 
    short *pB,   
    int inCols,
    int pitch,
    int inRows,
    char * pOut 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_gradientV5x5PyramidKernel_8_Params *fp0;
    XDAS_Int8                  *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pIn,0,inCols*inRows*sizeof( char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pB,1,inCols*inRows*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pOut,0,inCols*inRows*sizeof(char));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_gradientV5x5PyramidKernel_8_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_GRADIENTV5X5PYRAMIDKERNEL_8_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_gradientV5x5PyramidKernel_8_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pOut_OutArrID1      = OUTBUF0;
    fp0->pIn_InArrID1        = INBUF0;
    fp0->pB_InArrID2         = INBUF1;
    fp0->inCols              = inCols;
    fp0->inRows              = inRows;
    fp0->pitch               = pitch;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_legendreMoments(
    C6accel_Handle hC6accel,
    short  *Im, 
    short *Lmoments,   
    short *LPoly,
    int   Order,
    int  imH,
    short Constant 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_legendreMoments_Params *fp0;
    XDAS_Int8                  *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 1;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(Im,0,imH*imH*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(LPoly,1,imH*(Order+1)*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(Lmoments,0,(Order+1)*(Order+1)*sizeof(short));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_legendreMoments_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID=  VLIB_LEGENDREMOMENTS_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_legendreMoments_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->Lmoments_OutArrID1      = OUTBUF0;
    fp0->Im_InArrID1        = INBUF0;
    fp0->LPoly_InArrID2         = INBUF1;
    fp0->imH                 = imH;
    fp0->Order               = Order;
    fp0->Constant            = Constant;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_legendreMoments_Init(
    C6accel_Handle hC6accel,
    short *LPoly,
    int   Order,
    int  imH,
    short *Constant 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_legendreMoments_Init_Params *fp0;
    XDAS_Int8                  *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 0;
    outBufDesc.numBufs = 2;

    /* Fill in input/output buffer descriptor parameters */
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(LPoly,0,imH*(Order+1)*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(Constant,1,sizeof(short));
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_legendreMoments_Init_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_LEGENDREMOMENTS_INIT_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_legendreMoments_Init_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
   
    fp0->LPoly_OutArrID1    = INBUF0;
    fp0->imH                = imH;
    fp0->Order              = Order;
    fp0->Constant_OutArrID2 = INBUF1;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}


Int C6accel_VLIB_recursiveFilterHoriz1stOrder(
    C6accel_Handle hC6accel,
    char  *out, 
    char  *in,   
    int   width,
    int   height,
    short weight 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_recursiveFilterHoriz1stOrder_Params *fp0;
    XDAS_Int8                  *pAlloc;
    char * buffer;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 1;
    buffer = (char *)Memory_alloc(4*width*sizeof(char), &wrapperMemParams);
    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(in,0,width*height*sizeof(char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(buffer,1,4*width*sizeof(char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(out,0,width*height*sizeof(char));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_recursiveFilterHoriz1stOrder_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_RECURSIVEFILTERHORIZ1STORDER_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_recursiveFilterHoriz1stOrder_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pOut_OutArrID1      = OUTBUF0;
    fp0->pIn_InArrID1        = INBUF0;
    fp0->buffer_InArrID2    = INBUF1;
    fp0->width              = width;
    fp0->height             = height;
    fp0->weight             = weight;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(buffer,4*width*sizeof(char) , &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_recursiveFilterVert1stOrder(
    C6accel_Handle hC6accel,
    char  *out, 
    char  *in,   
    int   width,
    int   height,
    short weight 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_recursiveFilterVert1stOrder_Params *fp0;
    XDAS_Int8                  *pAlloc;
   char * buffer;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 1;
    buffer = (char *)Memory_alloc(4*width*sizeof(char), &wrapperMemParams);
    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(in,0,width*height*sizeof(char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(buffer,1,4*width*sizeof(char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(out,0,width*height*sizeof(char));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_recursiveFilterVert1stOrder_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_RECURSIVEFILTERVERT1STORDER_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_recursiveFilterVert1stOrder_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pOut_OutArrID1      = OUTBUF0;
    fp0->pIn_InArrID1        = INBUF0;
    fp0->buffer_InArrID2    = INBUF1;
    fp0->width              = width;
    fp0->height             = height;
    fp0->weight             = weight;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(buffer,4*width*sizeof(char) , &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_recursiveFilterHoriz1stOrderS16(
    C6accel_Handle hC6accel,
    short  *out, 
    short  *in,   
    int   width,
    int   height,
    short weight 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_recursiveFilterHoriz1stOrderS16_Params *fp0;
    XDAS_Int8                  *pAlloc;
    short *buffer;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 1;
    buffer = (short *)Memory_alloc(4*width*sizeof(short), &wrapperMemParams);

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(in,0,width*height*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(buffer,1,4*width*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(out,0,width*height*sizeof(short));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_recursiveFilterHoriz1stOrderS16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_RECURSIVEFILTERHORIZ1STORDERS16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_recursiveFilterHoriz1stOrderS16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pOut_OutArrID1      = OUTBUF0;
    fp0->pIn_InArrID1        = INBUF0;
    fp0->buffer_InArrID2    = INBUF1;
    fp0->width              = width;
    fp0->height             = height;
    fp0->weight             = weight;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(buffer,4*width*sizeof(short) , &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_recursiveFilterVert1stOrderS16(
    C6accel_Handle hC6accel,
    short  *out, 
    short  *in,   
    int   width,
    int   height,
    short weight 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_recursiveFilterVert1stOrderS16_Params *fp0;
    XDAS_Int8                  *pAlloc;
    short *buffer;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 1;
    buffer = (short *)Memory_alloc(4*width*sizeof(short), &wrapperMemParams);

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(in,0,width*height*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(buffer,1,4*width*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(out,0,width*height*sizeof(short));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_recursiveFilterVert1stOrderS16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_RECURSIVEFILTERVERT1STORDERS16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_recursiveFilterVert1stOrderS16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pOut_OutArrID1      = OUTBUF0;
    fp0->pIn_InArrID1        = INBUF0;
    fp0->buffer_InArrID2    = INBUF1;
    fp0->width              = width;
    fp0->height             = height;
    fp0->weight             = weight;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(buffer,4*width*sizeof(short) , &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}



int C6accel_VLIB_mixtureOfGaussiansS16 ( C6accel_Handle hC6accel,
                                  char *inputImage,
                                  unsigned short *currentWts,
                                  unsigned short *currentMeans,
                                  unsigned short *currentVars,
                                  char *compIndex,
                                  char *intBuffer,
                                  unsigned int *fgMask,
                                  int imageSize,
                                  unsigned short updateRate1,
                                  unsigned short updateRate2,
                                  unsigned short mdThreshold,
                                  unsigned short bsThreshold,
                                  unsigned short initialWt,
                                  unsigned short initialVar
                                                
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
     XDM1_BufDesc               inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_mixtureOfGaussiansS16_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_mixtureOfGaussiansS16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 2;
    outBufDesc.numBufs = 1;
    inOutBufDesc.numBufs = 4;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(inputImage,0,(imageSize)*sizeof(char));
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(currentWts,0,sizeof(short)*imageSize*3);
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(currentMeans,1,sizeof(short)*imageSize*3);
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(currentVars,2,sizeof(short)*imageSize*3);
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(compIndex,3,(imageSize)*sizeof(char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(intBuffer,1,sizeof(unsigned char)*imageSize);
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(fgMask,0,sizeof(int)*(imageSize)/32);
     
    /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_MIXTUREOFGAUSSIANSS16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_mixtureOfGaussiansS16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->inputIm_InArrID1     =  INBUF0;
    fp0->currentWts_InOutArrID1 =  INBUF0;
    fp0->currentMeans_InOutArrID2  =  INBUF1;
    fp0->currentVars_InOutArrID3 =  INBUF2;
    fp0->compIndex_InOutArrID4  =  INBUF3;
    fp0->intBuffer_InArrID1 = INBUF1;
    fp0->fgmask_OutArrID2 = OUTBUF0;
    fp0->imageSize = imageSize;
    fp0->updateRate1 = updateRate1;
    fp0->updateRate2 = updateRate2;
    fp0->mdThreshold = mdThreshold;
    fp0->bsThreshold = bsThreshold;
    fp0->initialWt = initialWt;
    fp0->initialVar = initialVar;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc,&outBufDesc, &inOutBufDesc ,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

int C6accel_VLIB_mixtureOfGaussiansS32 ( C6accel_Handle hC6accel,
                                  char *inputImage,
                                  unsigned short *currentWts,
                                  unsigned int *currentMeans,
                                  unsigned int *currentVars,
                                  char *compIndex,
                                  char *intBuffer,
                                  unsigned int *fgMask,
                                  int imageSize,
                                  unsigned short updateRate1,
                                  unsigned int updateRate2,
                                  unsigned int mdThreshold,
                                  unsigned short bsThreshold,
                                  unsigned short initialWt,
                                  unsigned int initialVar
                                                
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDM1_BufDesc               inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     /* Define pointer to function parameter structure */
    VLIB_mixtureOfGaussiansS32_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_mixtureOfGaussiansS32_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous heap memory allocation for the extended input structure */
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs = 2;
    outBufDesc.numBufs = 1;
    inOutBufDesc.numBufs = 4;

    /* Fill in input/output buffer descriptor parameters and manage ARM cache*/
    /* See wrapper_c6accel_i.h for more details of operation                 */
    
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(inputImage,0,(imageSize)*sizeof(char));
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(currentWts,0,sizeof(short)*imageSize*3);
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(currentMeans,1,sizeof(int)*imageSize*3);
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(currentVars,2,sizeof(int)*imageSize*3);
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(compIndex,3,(imageSize)*sizeof(char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(intBuffer,1,sizeof(unsigned char)*imageSize);
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(fgMask,0,sizeof(int)*(imageSize)/32);
     
    /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_MIXTUREOFGAUSSIANSS32_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_mixtureOfGaussiansS32_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->inputIm_InArrID1     =  INBUF0;
    fp0->currentWts_InOutArrID1 =  INBUF0;
    fp0->currentMeans_InOutArrID2  =  INBUF1;
    fp0->currentVars_InOutArrID3 =  INBUF2;
    fp0->compIndex_InOutArrID4  =  INBUF3;
    fp0->intBuffer_InArrID1 = INBUF1;
    fp0->fgmask_OutArrID2 = OUTBUF0;
    fp0->imageSize = imageSize;
    fp0->updateRate1 = updateRate1;
    fp0->updateRate2 = updateRate2;
    fp0->mdThreshold = mdThreshold;
    fp0->bsThreshold = bsThreshold;
    fp0->initialWt = initialWt;
    fp0->initialVar = initialVar;

    /* Call the actual algorithm */
    if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc,&outBufDesc, &inOutBufDesc ,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

int C6accel_VLIB_normalFlow_16( C6accel_Handle hC6accel,
                                  short *inDiff,
                                  short *Emag,
                                  short *Ex,
                                  short *Ey,
                                  short *LUT,
                                  short T,
                                  unsigned int imW,
                                  unsigned int imH,
                                  short *normalFlowU,
                                  short *normalFlowV                                                
                                  )
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
     XDM1_BufDesc               inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    int status;
     // Define pointer to function parameter structure 
    VLIB_normalFlow_16_Params  *fp0;
    XDAS_Int8 *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    // Allocate the InArgs structure as it varies in size
    //(Needs to be changed everytime we make a API call)
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_normalFlow_16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    // Request contiguous heap memory allocation for the extended input structure 
    pAlloc = (XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

    // Initialize .size fields for dummy input and output arguments 
    uniOutArgs.size = sizeof(uniOutArgs);

    // Set up buffers to pass buffers in and out to alg  
    inBufDesc.numBufs = 5;
    outBufDesc.numBufs = 2;

    // Fill in input/output buffer descriptor parameters and manage ARM cache
    // See wrapper_c6accel_i.h for more details of operation                 
    
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(inDiff,0,(imW*imH)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(Emag,1,sizeof(short)*imW*imH);
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(Ex,2,sizeof(short)*imW*imH);
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(Ey,3,sizeof(short)*imW*imH);
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(LUT,4,(255)*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(normalFlowU,0,sizeof(short)*(imW*imH));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(normalFlowV,1,sizeof(short)*(imW*imH));

     
    // Initialize the extended InArgs structure
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

    // Set function Id and parameter pointers for first function call 
    CInArgs->fxn[0].FxnID= VLIB_NORMALFLOW_16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    // Initialize pointers to function parameters 
    fp0 = (VLIB_normalFlow_16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    // Fill in the fields in the parameter structure 
    fp0->inDiff_InArrID1     =  INBUF0;
    fp0->Emag_InArrID2 =  INBUF1;
    fp0->Ex_InArrID3  =  INBUF2;
    fp0->Ey_InArrID4 =  INBUF3;
    fp0->LUT_InArrID5  =  INBUF4;
    fp0->normalFlowU_OutArrID1 = OUTBUF0;
    fp0->normalFlowV_OutArrID2 = OUTBUF1;
    fp0->T = T;
    fp0->numPixels = imW*imH;
    
    // Call the actual algorithm 
    if (hC6accel->callType == ASYNC)
      {

       // Update async structure 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           // Context Saving 
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           // Asynchronous Call to the actual algorithm
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc,&outBufDesc, NULL ,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      // Synchronous Call to the actual algorithm 
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      // Free the InArgs structure 
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;

}

Int C6accel_VLIB_harrisScore_7x7(
    C6accel_Handle hC6accel,
    short * gradX,
    short *gradY,    
    int width,
    int height,
    short *restrict harrisscore,
    short k
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    char* buffer;
    /* Define pointer to function parameter structure */
    VLIB_harrisScore_7x7_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 1;
    inOutBufDesc.numBufs =1;

    buffer = (char *)Memory_alloc(160*width*sizeof(char), &wrapperMemParams);
    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(gradX,0,width*height*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(gradY,1,width*height*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(harrisscore,0,width*height*sizeof(short));
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(buffer,0,160*width*sizeof(char));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_harrisScore_7x7_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_HARRISSCORE_7x7_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_harrisScore_7x7_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->harrisscore_OutArrID1 = OUTBUF0;
    fp0->buffer_InOutArrID1    = OUTBUF0;
    fp0->gradX_InArrID1        = INBUF0;
    fp0->gradY_InArrID2        = INBUF1;
    fp0->width                 = width;
    fp0->height                = height;
    fp0->k =k;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc,&inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(buffer,96*width*sizeof(char) , &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_houghLineFromList(
    C6accel_Handle hC6accel,
    unsigned short * pEdgeMapList,
    unsigned short *pOutHoughSpace,
    unsigned short outBlkWidth,
    unsigned short outBlkHeight,
    unsigned int listSize,    
    unsigned short thetaRange,
    unsigned short rhoMaxLength,
    short *pSIN,
    short *pCOS    
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    short* ping, *pang, *peng, *pong;
    /* Define pointer to function parameter structure */
    VLIB_houghLineFromList_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 3;
    outBufDesc.numBufs = 1;
    inOutBufDesc.numBufs =4;

    ping = (short *)Memory_alloc(rhoMaxLength*sizeof(short), &wrapperMemParams);
    pong = (short *)Memory_alloc(rhoMaxLength*sizeof(short), &wrapperMemParams);
    pang = (short *)Memory_alloc(rhoMaxLength*sizeof(short), &wrapperMemParams);
    peng = (short *)Memory_alloc(rhoMaxLength*sizeof(short), &wrapperMemParams);
    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pEdgeMapList,0,2*listSize*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pSIN,1,rhoMaxLength*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pCOS,2,rhoMaxLength*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pOutHoughSpace,0,thetaRange*rhoMaxLength*sizeof(short));
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(ping,0,rhoMaxLength*sizeof(short));
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pong,1,rhoMaxLength*sizeof(short));
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pang,2,rhoMaxLength*sizeof(short));
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(peng,3,rhoMaxLength*sizeof(short));
    
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_houghLineFromList_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_HOUGHLINEFROMLIST_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_houghLineFromList_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pOutHoughSpace_OutArrID1 = OUTBUF0;

    fp0->ping_InOutArrID1         = OUTBUF0;
    fp0->pong_InOutArrID2         = OUTBUF1;
    fp0->pang_InOutArrID3         = OUTBUF2;
    fp0->peng_InOutArrID4         = OUTBUF3;

    fp0->pEdgeMapList_InArrID1    = INBUF0;
    fp0->pSIN_InArrID2            = INBUF1;
    fp0->pCOS_InArrID3            = INBUF2;

    fp0->listSize                 = listSize;
    fp0->thetaRange               = thetaRange;
    fp0->rhoMaxLength             = rhoMaxLength;
    fp0->outBlkWidth              = outBlkWidth;
    fp0->outBlkHeight             = outBlkHeight;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc,&inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc,&inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(ping,rhoMaxLength*sizeof(short) , &wrapperMemParams);
      Memory_free(pong,rhoMaxLength*sizeof(short) , &wrapperMemParams);
      Memory_free(peng,rhoMaxLength*sizeof(short) , &wrapperMemParams);
      Memory_free(pang,rhoMaxLength*sizeof(short) , &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_trackFeaturesLucasKanade_7x7(
    C6accel_Handle hC6accel,
    const char * restrict im1,
    const char * restrict im2,
    const short *restrict gradX,
    const short *restrict gradY,
    int width,
    int height,
    int nfeatures,
    short * restrict x,
    short * restrict y,
    short *outx,
    short *outy,
    int max_iters
    
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    char* scratch;
    /* Define pointer to function parameter structure */
    VLIB_trackFeaturesLucasKanade_7x7_Params *fp0;
    XDAS_Int8                    *pAlloc;

    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 6;
    outBufDesc.numBufs = 0;
    inOutBufDesc.numBufs =3;

    scratch = (char *)Memory_alloc(384*sizeof(char), &wrapperMemParams);
    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(im1,0,width*height*sizeof(char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(im2,1,width*height*sizeof(char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(gradX,2,width*height*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(gradY,3,width*height*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(x,4,nfeatures*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(y,5,nfeatures*sizeof(short));

    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(outx,0,nfeatures*sizeof(short));
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(outy,1,nfeatures*sizeof(short));
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(scratch,2,384*sizeof(char));
     
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_trackFeaturesLucasKanade_7x7_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_TRACKFEATURESLUCASKANADE_7x7_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_trackFeaturesLucasKanade_7x7_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    
    fp0->outx_InOutArrID1     = OUTBUF0;
    fp0->outy_InOutArrID2     = OUTBUF1;
    fp0->scratch_InOutArrID3  = OUTBUF2;

    fp0->im1_InArrID1         = INBUF0;
    fp0->im2_InArrID2         = INBUF1;
    fp0->gradX_InArrID3       = INBUF2;
    fp0->gradY_InArrID4       = INBUF3;
    fp0->x_InArrID5           = INBUF4;
    fp0->y_InArrID6           = INBUF5;

    fp0->width                = width;
    fp0->height               = height;
    fp0->nfeatures            = nfeatures;
    fp0->max_iters            = max_iters;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc,&inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(scratch,384*sizeof(short) , &wrapperMemParams);
      
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_histogram_1D_Init_U8(
    C6accel_Handle hC6accel,
    char  *binEdges, 
    int numBins,
    char *histArray
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_histogram_1D_Init_U8_Params *fp0;
    XDAS_Int8                  *pAlloc;
    
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 1;
    outBufDesc.numBufs = 1;
    

    /* Fill in input/output buffer descriptor parameters */
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(histArray,0,256*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(binEdges,0,numBins*sizeof(char));
    
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_histogram_1D_Init_U8_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_HISTOGRAM_1D_INIT_U8_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_histogram_1D_Init_U8_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->binEdges_InArrID1  = INBUF0;
    fp0->histArray_OutArrID1  = OUTBUF0;
    fp0->numBins                = numBins;
   
   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_histogram_1D_Init_U16(
    C6accel_Handle hC6accel,
    short  *binEdges, 
    int numBins,
    short *histArray
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_histogram_1D_Init_U16_Params *fp0;
    XDAS_Int8                  *pAlloc;
    short * buffer;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 1;
    outBufDesc.numBufs = 1;
    
    /* Fill in input/output buffer descriptor parameters */
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(histArray,0,256*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(binEdges,0,numBins*sizeof(short));
    
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_histogram_1D_Init_U16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_HISTOGRAM_1D_INIT_U16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_histogram_1D_Init_U16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->binEdges_InArrID1  = INBUF0;
    fp0->histArray_OutArrID1  = OUTBUF0;
    fp0->numBins             = numBins;
   
   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_histogram_1D_U8(
    C6accel_Handle hC6accel,
    char * X,
    int numX,
    int numBins,
    unsigned short binWeight,
    char *histArray,
    unsigned short * H
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_histogram_1D_U8_Params *fp0;
    XDAS_Int8                  *pAlloc;
    short * internalH1, *internalH2, *internalH3;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 5;
    outBufDesc.numBufs = 1;
 
    internalH1 = (short *)Memory_alloc(numBins*sizeof(unsigned short), &wrapperMemParams);
    internalH2 = (short *)Memory_alloc(numBins*sizeof(unsigned short), &wrapperMemParams);
    internalH3 = (short *)Memory_alloc(numBins*sizeof(unsigned short), &wrapperMemParams);
    memset(internalH1,0,numBins*sizeof(unsigned short));
    memset(internalH2,0,numBins*sizeof(unsigned short));
    memset(internalH3,0,numBins*sizeof(unsigned short));

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(X,0,numX*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(histArray,1,256*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(internalH1,2,numBins*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(internalH2,3,numBins*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(internalH3,4,numBins*sizeof(unsigned short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(H,0,numBins*sizeof(unsigned short));
    
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_histogram_1D_U8_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_HISTOGRAM_1D_U8_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_histogram_1D_U8_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->H_OutArrID1          = OUTBUF0;
    fp0->X_InArrID1           = INBUF0;
    fp0->histArray_InArrID2   = INBUF1;
    fp0->internalH1_InArrID3  = INBUF2;
    fp0->internalH2_InArrID4  = INBUF3;
    fp0->internalH3_InArrID5  = INBUF4;
    fp0->numBins              = numBins;
    fp0->numX                 = numX;
    fp0->binWeight            = binWeight;
   
   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(internalH1,numBins*sizeof(unsigned short), &wrapperMemParams);
      Memory_free(internalH2,numBins*sizeof(unsigned short), &wrapperMemParams);
      Memory_free(internalH3,numBins*sizeof(unsigned short), &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_histogram_1D_U16(
    C6accel_Handle hC6accel,
    short * X,
    int numX,
    int numBins,
    unsigned short binWeight,
    short *histArray,
    unsigned short * H
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_histogram_1D_U16_Params *fp0;
    XDAS_Int8                  *pAlloc;
    short * internalH1, *internalH2, *internalH3;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 3;
    outBufDesc.numBufs = 1;
 
    internalH1 = (short *)Memory_alloc(numBins*sizeof(unsigned short), &wrapperMemParams);
    
    memset(internalH1,0,numBins*sizeof(unsigned short));
    
    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(X,0,numX*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(histArray,1,256*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(internalH1,2,numBins*sizeof(unsigned short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(H,0,numBins*sizeof(unsigned short));
    
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_histogram_1D_U16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_HISTOGRAM_1D_U16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_histogram_1D_U16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->H_OutArrID1          = OUTBUF0;
    fp0->X_InArrID1           = INBUF0;
    fp0->histArray_InArrID2   = INBUF1;
    fp0->internalH1_InArrID3  = INBUF2;
    fp0->numBins              = numBins;
    fp0->numX                 = numX;
    fp0->binWeight            = binWeight;
   
   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(internalH1,numBins*sizeof(unsigned short), &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}


Int C6accel_VLIB_weightedHistogram_1D_U8(
    C6accel_Handle hC6accel,
    char * X,
    int numX,
    int numBins,
    unsigned short *binWeight,
    char *histArray,
    unsigned short * H
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_weightedHistogram_1D_U8_Params *fp0;
    XDAS_Int8                  *pAlloc;
    short * internalH1, *internalH2, *internalH3;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 6;
    outBufDesc.numBufs = 1;
 
    internalH1 = (short *)Memory_alloc(numBins*sizeof(unsigned short), &wrapperMemParams);
    internalH2 = (short *)Memory_alloc(numBins*sizeof(unsigned short), &wrapperMemParams);
    internalH3 = (short *)Memory_alloc(numBins*sizeof(unsigned short), &wrapperMemParams);
    memset(internalH1,0,numBins*sizeof(unsigned short));
    memset(internalH2,0,numBins*sizeof(unsigned short));
    memset(internalH3,0,numBins*sizeof(unsigned short));

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(X,0,numX*sizeof(unsigned char));
    
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(binWeight,1,numX*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(histArray,2,256*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(internalH1,3,numBins*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(internalH2,4,numBins*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(internalH3,5,numBins*sizeof(unsigned short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(H,0,numBins*sizeof(unsigned short));
    
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_weightedHistogram_1D_U8_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_WEIGHTEDHISTOGRAM_1D_U8_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_weightedHistogram_1D_U8_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->H_OutArrID1          = OUTBUF0;
    fp0->X_InArrID1           = INBUF0;
    fp0->binWeight_InArrID2  = INBUF1;
    fp0->histArray_InArrID3   = INBUF2;
    fp0->internalH1_InArrID4  = INBUF3;
    fp0->internalH2_InArrID5  = INBUF4;
    fp0->internalH3_InArrID6  = INBUF5;
    fp0->numBins              = numBins;
    fp0->numX                 = numX;
    
   
   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(internalH1,numBins*sizeof(unsigned short), &wrapperMemParams);
      Memory_free(internalH2,numBins*sizeof(unsigned short), &wrapperMemParams);
      Memory_free(internalH3,numBins*sizeof(unsigned short), &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_weightedHistogram_1D_U16(
    C6accel_Handle hC6accel,
    unsigned short * X,
    int numX,
    int numBins,
    unsigned short *binWeight,
    unsigned short *histArray,
    unsigned short * H
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_weightedHistogram_1D_U16_Params *fp0;
    XDAS_Int8    *pAlloc;
    short * internalH1;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 4;
    outBufDesc.numBufs = 1;
 
    internalH1 = (short *)Memory_alloc(numBins*sizeof(short), &wrapperMemParams);
    
    memset(internalH1,0,numBins*sizeof(short));
   

    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(X,0,numX*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(binWeight,1,numX*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(histArray,2,256*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(internalH1,3,numBins*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(H,0,numBins*sizeof(short));
    
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_weightedHistogram_1D_U16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_WEIGHTEDHISTOGRAM_1D_U16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_weightedHistogram_1D_U16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->H_OutArrID1          = OUTBUF0;
    fp0->X_InArrID1           = INBUF0;
    fp0->binWeight_InArrID2   = INBUF1;
    fp0->histArray_InArrID3   = INBUF2;
    fp0->internalH1_InArrID4  = INBUF3;
    fp0->numBins              = numBins;
    fp0->numX                 = numX;
    
   
   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(internalH1,numBins*sizeof(unsigned short), &wrapperMemParams);
      
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}


Int C6accel_VLIB_weightedHistogram_nD_U16(
    C6accel_Handle hC6accel,
    short * X,
    int numX,
    int dimX,
    unsigned short *binWeight,
    unsigned short *numBins,
    unsigned short *normVals,
    unsigned short * H
){
    XDM1_BufDesc                inBufDesc;
     XDM1_BufDesc               outBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_weightedHistogram_nD_U16_Params *fp0;
    XDAS_Int8    *pAlloc;
    unsigned short * internalH1, *internalH2;
    unsigned short i, total;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 6;
    inOutBufDesc.numBufs = 1;
    total=1;
    for(i=0;i<dimX;i++)
       total *= numBins[i];

    internalH1 = (unsigned short *)Memory_alloc(numX*sizeof(unsigned short), &wrapperMemParams);
    internalH2 = (unsigned short *)Memory_alloc(total*sizeof(unsigned short), &wrapperMemParams);

    memset(internalH1,0,numX*sizeof(unsigned short));
    memset(internalH2,0,total*sizeof(unsigned short));
   
     /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(H,0,total*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(X,0,numX*dimX*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(binWeight,1,numX*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(numBins,2,dimX*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(normVals,3,dimX*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(internalH1,4,numX*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(internalH2,5,total*sizeof(unsigned short));
    
    
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_weightedHistogram_nD_U16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_WEIGHTEDHISTOGRAM_ND_U16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_weightedHistogram_nD_U16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
   
    fp0->X_InArrID1           = INBUF0;
    fp0->binWeight_InArrID2   = INBUF1;
    fp0->numBins_InArrID3     = INBUF2;
    fp0->normVals_InArrID6    = INBUF3;
    fp0->internalH1_InArrID4  = INBUF4;
    fp0->internalH2_InArrID5  = INBUF5;
    fp0->H_InOutArrID1        = OUTBUF0;
    fp0->dimX                 = dimX;
    fp0->numX                 = numX;
    
   
   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc,  NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc,   NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
     
      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(internalH1,numX*sizeof(unsigned short), &wrapperMemParams);
      Memory_free(internalH2,total*sizeof(unsigned short), &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_histogram_nD_U16(
    C6accel_Handle hC6accel,
    short * X,
    int numX,
    int dimX,
    unsigned short binWeight,
    unsigned short *numBins,
    unsigned short *normVals,
    unsigned short * H
){
    XDM1_BufDesc                inBufDesc;
     XDM1_BufDesc               outBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_histogram_nD_U16_Params *fp0;
    XDAS_Int8    *pAlloc;
    unsigned short * internalH1, *internalH2;
    unsigned short i, total;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 5;
    inOutBufDesc.numBufs = 1;
    total=1;
    for(i=0;i<dimX;i++)
       total *= numBins[i];

    internalH1 = (unsigned short *)Memory_alloc(numX*sizeof(unsigned short), &wrapperMemParams);
    internalH2 = (unsigned short *)Memory_alloc(total*sizeof(unsigned short), &wrapperMemParams);

    memset(internalH1,0,numX*sizeof(unsigned short));
    memset(internalH2,0,total*sizeof(unsigned short));
   
     /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(H,0,total*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(X,0,numX*dimX*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(numBins,1,dimX*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(normVals,2,dimX*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(internalH1,3,numX*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(internalH2,4,total*sizeof(unsigned short));
    
    
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_histogram_nD_U16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_HISTOGRAM_ND_U16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_histogram_nD_U16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
   
    fp0->X_InArrID1           = INBUF0;
    fp0->numBins_InArrID2     = INBUF1;
    fp0->normVals_InArrID3    = INBUF2;
    fp0->internalH1_InArrID4  = INBUF3;
    fp0->internalH2_InArrID5  = INBUF4;
    fp0->H_InOutArrID1        = OUTBUF0;
    fp0->dimX                 = dimX;
    fp0->numX                 = numX;
    fp0->binWeight            = binWeight;
   
   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc,  NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc,   NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
     
      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(internalH1,numX*sizeof(unsigned short), &wrapperMemParams);
      Memory_free(internalH2,total*sizeof(unsigned short), &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_bhattacharyaDistance_U32(
    C6accel_Handle hC6accel,
    unsigned short  *X,
    unsigned short  *Y,   
    int   N,
    unsigned int *D 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_bhattacharyaDistance_U32_Params *fp0;
    XDAS_Int8                  *pAlloc;
   char * buffer;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 1;
    
    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(X,0,N*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(Y,1,N*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(D,0,sizeof(unsigned int));
    
  
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_bhattacharyaDistance_U32_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_BHATTACHARYADISTANCE_U32_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_bhattacharyaDistance_U32_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->D_OutArrID1      = OUTBUF0;
    fp0->X_InArrID1       = INBUF0;
    fp0->Y_InArrID2       = INBUF1;
    fp0->N                = N;
   
    

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
      
      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

Int C6accel_VLIB_L1DistanceS16(
    C6accel_Handle hC6accel,
    unsigned short  *X,
    unsigned short  *Y,   
    int              N,
    unsigned int *L1D 
)
{
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_L1DistanceS16_Params *fp0;
    XDAS_Int8                  *pAlloc;
   char * buffer;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 2;
    outBufDesc.numBufs = 1;
    
    /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(X,0,N*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(Y,1,N*sizeof(short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(L1D,0,N*sizeof(int));
    
      /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_L1DistanceS16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_L1DISTANCES16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_L1DistanceS16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->L1D_OutArrID1      = OUTBUF0;
    fp0->X_InArrID1       = INBUF0;
    fp0->Y_InArrID2       = INBUF1;
    fp0->N                = N;
    

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

 Int C6accel_VLIB_disparity_SAD_firstRow8(
    C6accel_Handle hC6accel,
    unsigned char *restrict pLeft,
    unsigned char *restrict pRight,
    unsigned short *restrict pCost,
    unsigned short *restrict pMinCost,
    char *pScratch,
    char *restrict pDisparity,
    int displacement,
    int width,
    int height,
    int pitch,
    int windowSize,
    int blockSize,
    int arrayPad,
    int dmin,
    int dmax
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_disparity_SAD_firstRow8_Params *fp0;
    XDAS_Int8    *pAlloc;
  
    unsigned short i, total;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);
    
    
    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 5;
    inOutBufDesc.numBufs = 1;
      
     /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pLeft,0,(windowSize+blockSize)*width*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pRight,1,(windowSize+blockSize)*width*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pCost,2,(width+arrayPad)*(dmax-dmin+2)*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pMinCost,3,(width+arrayPad)*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pScratch,4,(width+windowSize+2+arrayPad)*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pDisparity,0,(width+height)*arrayPad*sizeof(unsigned char));
    
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_disparity_SAD_firstRow8_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_DISPARITY_SAD_FIRSTROW8_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_disparity_SAD_firstRow8_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pDisparity_OutArrID1     = OUTBUF0;
    fp0->pLeft_InArrID1           = INBUF0;
    fp0->pRight_InArrID2          = INBUF1;
    fp0->pCost_InArrID3           = INBUF2;
    fp0->pMinCost_InArrID4        = INBUF3;
    fp0->displacement             = displacement;
    fp0->width                    = width;
    fp0->pitch                    = pitch;
    fp0->windowSize               = windowSize;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}          

Int C6accel_VLIB_disparity_SAD8(
    C6accel_Handle hC6accel,
    unsigned char *restrict pLeft,
    unsigned char *restrict pRight,
    unsigned short *restrict pCost,
    unsigned short *restrict pMinCost,
    char *restrict pDisparity,
    int displacement,
    int width,
    int height,
    int pitch,
    int windowSize,
    int blockSize,
    int arrayPad,
    int dmin,
    int dmax
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_disparity_SAD8_Params *fp0;
    XDAS_Int8    *pAlloc;
  
    unsigned short i, total;
    ACQUIRE_CODEC_ENGINE;
   
  

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 4;
    inOutBufDesc.numBufs = 1;
      
     /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pLeft,0,(windowSize+blockSize)*width*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pRight,1,(windowSize+blockSize)*width*sizeof(unsigned char));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pCost,2,(width+arrayPad)*(dmax-dmin+2)*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pMinCost,3,(width+arrayPad)*sizeof(unsigned short));
   
    CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pDisparity,0,(width+height)*arrayPad*sizeof(unsigned char));
    
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_disparity_SAD8_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_DISPARITY_SAD8_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_disparity_SAD8_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pDisparity_OutArrID1     = OUTBUF0;
    fp0->pLeft_InArrID1           = INBUF0;
    fp0->pRight_InArrID2          = INBUF1;
    fp0->pCost_InArrID3           = INBUF2;
    fp0->pMinCost_InArrID4        = INBUF3;
    fp0->pScratch_InArrID5        = INBUF4;
    fp0->displacement             = displacement;
    fp0->width                    = width;
    fp0->pitch                    = pitch;
    fp0->windowSize               = windowSize;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}   

 Int C6accel_VLIB_disparity_SAD_firstRow16(
    C6accel_Handle hC6accel,
    unsigned short *restrict pLeft,
    unsigned short *restrict pRight,
    unsigned short *restrict pCost,
    unsigned short *restrict pMinCost,
    char *restrict pDisparity,
    int displacement,
    int width,
    int height,
    int pitch,
    int windowSize,
    int blockSize,
    int arrayPad,
    int dmin,
    int dmax
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_disparity_SAD_firstRow16_Params *fp0;
    XDAS_Int8    *pAlloc;
   
    unsigned short i, total;
    ACQUIRE_CODEC_ENGINE;

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 4;
    outBufDesc.numBufs = 1;
      
     /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pLeft,0,(windowSize+blockSize)*width*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pRight,1,(windowSize+blockSize)*width*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pCost,2,(width+arrayPad)*(dmax-dmin+2)*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pMinCost,3,(width+arrayPad)*sizeof(unsigned short));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pDisparity,0,(width+height)*arrayPad*sizeof(unsigned char));
    
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_disparity_SAD_firstRow16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_DISPARITY_SAD_FIRSTROW16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_disparity_SAD_firstRow16_Params *)((XDAS_Int8*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pDisparity_OutArrID1     = OUTBUF0;
    fp0->pLeft_InArrID1           = INBUF0;
    fp0->pRight_InArrID2          = INBUF1;
    fp0->pCost_InArrID3           = INBUF2;
    fp0->pMinCost_InArrID4        = INBUF3;
    fp0->displacement             = displacement;
    fp0->width                    = width;
    fp0->pitch                    = pitch;
    fp0->windowSize               = windowSize;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
     
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}          

Int C6accel_VLIB_disparity_SAD16(
    C6accel_Handle hC6accel,
    unsigned short *restrict pLeft,
    unsigned short *restrict pRight,
    unsigned short *restrict pCost,
    unsigned short *restrict pMinCost,
    char *restrict pDisparity,
    int displacement,
    int width,
    int height,
    int pitch,
    int windowSize,
    int blockSize,
    int arrayPad,
    int dmin,
    int dmax
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_disparity_SAD16_Params *fp0;
    XDAS_Int8    *pAlloc;
    unsigned char *pScratch;
    unsigned short i, total;
    ACQUIRE_CODEC_ENGINE;
   
    pScratch = (char *)Memory_alloc((width+windowSize+2+arrayPad)*sizeof(unsigned char), &wrapperMemParams);
    memset(pScratch,0,(width+windowSize+2+arrayPad)*sizeof(unsigned short));

    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 5;
    outBufDesc.numBufs = 1;
      
     /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pLeft,0,(windowSize+blockSize)*width*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pRight,1,(windowSize+blockSize)*width*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pCost,2,(width+arrayPad)*(dmax-dmin+2)*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pMinCost,3,(width+arrayPad)*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pScratch,4,(width+windowSize+2+arrayPad)*sizeof(unsigned char));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(pDisparity,0,(width+height)*arrayPad*sizeof(unsigned char));
    
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_disparity_SAD16_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_DISPARITY_SAD16_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_disparity_SAD16_Params *)((XDAS_Int16*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->pDisparity_OutArrID1     = OUTBUF0;
    fp0->pLeft_InArrID1           = INBUF0;
    fp0->pRight_InArrID2          = INBUF1;
    fp0->pCost_InArrID3           = INBUF2;
    fp0->pMinCost_InArrID4        = INBUF3;
    
    fp0->displacement             = displacement;
    fp0->width                    = width;
    fp0->pitch                    = pitch;
    fp0->windowSize               = windowSize;

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);

      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(pScratch, (width+windowSize+2+arrayPad)*sizeof(unsigned char), &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}       




Int C6accel_VLIB_simplex_rosenbrock32(
    C6accel_Handle hC6accel,
    short *restrict start,
    short *restrict init_step,
    int N,
    int MaxIteration,
    int EPSILON,
    void * addtlArgs,
    short *restrict minPoint,
    int minValue
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    /* Define pointer to function parameter structure */
    VLIB_simplex_Params *fp0;
    XDAS_Int8    *pAlloc;
    short *v,*f,*vr,*ve,*vc, *vm, *pTotal;
    unsigned short i, total;
    ACQUIRE_CODEC_ENGINE;
   
    pTotal = (short *)Memory_alloc((N+N+N+N+N+1+N+1)*sizeof(short), &wrapperMemParams);
    v=pTotal;
    f=v+(N+1);
    vr=f+(N+1);
    ve=vr+N;
    vc=ve+N;
    vm=vc+N;
    
    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 9;
    outBufDesc.numBufs = 2;
      
     /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(start,0,N*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(init_step,1,N*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(v,2,(N+1)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(f,3,(N+1)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(vr,4,(N)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(ve,5,(N)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(vc,6,(N)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(vm,7,(N)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(addtlArgs,8,sizeof(argsStruct));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(minPoint,0,N*sizeof(short));
        
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_simplex_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_SIMPLEX_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_simplex_Params *)((XDAS_Int16*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->minPoint_OutArrID1   = OUTBUF0;
    fp0->minValue     = minValue;
    
    fp0->start_InArrID2       = INBUF0;
    fp0->init_step_InArrID3   = INBUF1;
    fp0->v_InArrID4           = INBUF2;
    fp0->f_InArrID5           = INBUF3;
    fp0->vr_InArrID6          = INBUF4;
    fp0->ve_InArrID7          = INBUF5;
    fp0->vc_InArrID8          = INBUF6;
    fp0->vm_InArrID9          = INBUF7;
    fp0->addtlArgs_InArrID10      = INBUF8;
    fp0->N                    = N;
    fp0->N_INV                = (short)(32767/N);
    fp0->MaxIteration         = MaxIteration;
    fp0->EPSILON              = EPSILON;
    fp0->minValue             = minValue;
    fp0->minPoint_OutArrID1   = OUTBUF0; 

   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
      Memory_cacheInv(&fp0->minValue,sizeof(int));

      minValue= fp0->minValue;
      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(pTotal, (N+N+N+N+N+1+N+1)*sizeof(short), &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}       

Int C6accel_VLIB_simplex_rosenbrock32_3D(
    C6accel_Handle hC6accel,
    short *restrict start,
    short *restrict init_step,
    int MaxIteration,
    int EPSILON,
    void * addtlArgs,
    short *restrict minPoint,
    int minValue
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    int N=3;
    /* Define pointer to function parameter structure */
    VLIB_simplex_Params *fp0;
    XDAS_Int8    *pAlloc;
    short *v,*f,*vr,*ve,*vc, *vm, *pTotal;
    unsigned short i, total;
    ACQUIRE_CODEC_ENGINE;
   
    pTotal = (short *)Memory_alloc((N+N+N+N+N+1+N+1)*sizeof(short), &wrapperMemParams);
    v=pTotal;
    f=v+(N+1);
    vr=f+(N+1);
    ve=vr+N;
    vc=ve+N;
    vm=vc+N;
    
    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 9;
    outBufDesc.numBufs = 2;
      
     /* Fill in input/output buffer descriptor parameters */
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(start,0,N*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(init_step,1,N*sizeof(unsigned short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(v,2,(N+1)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(f,3,(N+1)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(vr,4,(N)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(ve,5,(N)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(vc,6,(N)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(vm,7,(N)*sizeof(short));
    CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(addtlArgs,8,sizeof(argsStruct));
    CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(minPoint,0,N*sizeof(short));
        
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_simplex_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_SIMPLEX_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_simplex_Params *)((XDAS_Int16*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->minPoint_OutArrID1   = OUTBUF0;
    fp0->minValue     = minValue;
    
    fp0->start_InArrID2       = INBUF0;
    fp0->init_step_InArrID3   = INBUF1;
    fp0->v_InArrID4           = INBUF2;
    fp0->f_InArrID5           = INBUF3;
    fp0->vr_InArrID6          = INBUF4;
    fp0->ve_InArrID7          = INBUF5;
    fp0->vc_InArrID8          = INBUF6;
    fp0->vm_InArrID9          = INBUF7;
    fp0->addtlArgs_InArrID10  = INBUF8;
    fp0->MaxIteration         = MaxIteration;
    fp0->EPSILON              = EPSILON;
    fp0->minValue             = minValue;
    fp0->minPoint_OutArrID1   = OUTBUF0;
   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, NULL,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
      Memory_cacheInv(&fp0->minValue,sizeof(int));

      minValue= fp0->minValue;
      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      Memory_free(pTotal, (N+N+N+N+N+1+N+1)*sizeof(short), &wrapperMemParams);
     }

    RELEASE_CODEC_ENGINE;

    return status;  

} 

VLIB_kalmanFilter_2x4 * C6accel_Kalmanfilter2x4_create(void){
VLIB_kalmanFilter_2x4 *K;

K = (VLIB_kalmanFilter_2x4 *)Memory_alloc(sizeof(VLIB_kalmanFilter_2x4),  &wrapperMemParams);
memset((void *)K, 0,sizeof(VLIB_kalmanFilter_2x4));
return K;


}


VLIB_kalmanFilter_4x6 * C6accel_Kalmanfilter4x6_create(void){
VLIB_kalmanFilter_4x6 *K;

K = (VLIB_kalmanFilter_4x6 *)Memory_alloc(sizeof(VLIB_kalmanFilter_4x6),  &wrapperMemParams);
K->scaleFactor = 1;
memset((void *)K, 0,sizeof(VLIB_kalmanFilter_4x6));
return K;


}


int C6accel_Kalmanfilter2x4_release(VLIB_kalmanFilter_2x4 *K){

Memory_free(K, sizeof(VLIB_kalmanFilter_2x4),  &wrapperMemParams);
return 1;
}


int C6accel_Kalmanfilter4x6_release(VLIB_kalmanFilter_4x6 *K){


Memory_free(K, sizeof(VLIB_kalmanFilter_4x6),  &wrapperMemParams);
return 1;


}

Int C6accel_VLIB_kalmanFilter_2x4_Predict(
    C6accel_Handle hC6accel,
    VLIB_kalmanFilter_2x4 *KF
      
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    int N=3;
    /* Define pointer to function parameter structure */
    VLIB_kalmanFilter_2x4_Predict_Params *fp0;
    XDAS_Int8    *pAlloc;
    
    ACQUIRE_CODEC_ENGINE;
   
    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 0;
    inOutBufDesc.numBufs = 1;
      
     /* Fill in input/output buffer descriptor parameters */
     CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(KF,0,sizeof(VLIB_kalmanFilter_2x4));
        
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_kalmanFilter_2x4_Predict_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_KALMANFILTER_2X4_PREDICT_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_kalmanFilter_2x4_Predict_Params *)((XDAS_Int16*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->KF_InOutArrID1   = OUTBUF0;
    
    
   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, NULL, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
      
      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      
     }

    RELEASE_CODEC_ENGINE;

    return status;  

} 

Int C6accel_VLIB_kalmanFilter_4x6_Predict(
    C6accel_Handle hC6accel,
    VLIB_kalmanFilter_4x6 *KF
      
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    int N=3;
    /* Define pointer to function parameter structure */
    VLIB_kalmanFilter_4x6_Predict_Params *fp0;
    XDAS_Int8    *pAlloc;
    
    ACQUIRE_CODEC_ENGINE;
   
    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 0;
    inOutBufDesc.numBufs = 1;
      
     /* Fill in input/output buffer descriptor parameters */
     CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(KF,0,sizeof(VLIB_kalmanFilter_4x6));
        
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_kalmanFilter_4x6_Predict_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_KALMANFILTER_4X6_PREDICT_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_kalmanFilter_4x6_Predict_Params *)((XDAS_Int16*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->KF_InOutArrID1   = OUTBUF0;
    
    
   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, NULL,&inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, NULL,&inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
      
      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      
     }

    RELEASE_CODEC_ENGINE;

    return status;  

} 

Int C6accel_VLIB_kalmanFilter_2x4_Correct(
    C6accel_Handle hC6accel,
    VLIB_kalmanFilter_2x4 *KF,
    short *Z,
    short * residual
      
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    int N=3;
    /* Define pointer to function parameter structure */
    VLIB_kalmanFilter_2x4_Correct_Params *fp0;
    XDAS_Int8    *pAlloc;
    
    ACQUIRE_CODEC_ENGINE;
   
    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 1;
    outBufDesc.numBufs = 1;
    inOutBufDesc.numBufs = 1;

     /* Fill in input/output buffer descriptor parameters */
     CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(KF,0,sizeof(VLIB_kalmanFilter_2x4));
     CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(Z,0,2*sizeof(short));
     CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(residual,0,2*sizeof(short));  
 
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_kalmanFilter_2x4_Correct_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
   // printf("CInArgs = %x\n", CMEM_getPhys(pAlloc));
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_KALMANFILTER_2X4_CORRECT_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_kalmanFilter_2x4_Correct_Params *)((XDAS_Int16*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->KF_InOutArrID1   = OUTBUF0;
    fp0->Z_InArrID1   = OUTBUF0;
    fp0->residual_OutArrID1   = OUTBUF0; 
   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
      
      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      
     }

    RELEASE_CODEC_ENGINE;

    return status;  

} 

Int C6accel_VLIB_kalmanFilter_4x6_Correct(
    C6accel_Handle hC6accel,
    VLIB_kalmanFilter_4x6 *KF,
    short *Z,
    short * residual
      
){
    XDM1_BufDesc                inBufDesc;
    XDM1_BufDesc                outBufDesc;
    XDM1_BufDesc                inOutBufDesc;
    XDAS_Int32                  InArg_Buf_size;
    IC6Accel_InArgs             *CInArgs;
    UNIVERSAL_OutArgs           uniOutArgs;
    Int                         status;
    int N=3;
    /* Define pointer to function parameter structure */
    VLIB_kalmanFilter_4x6_Correct_Params *fp0;
    XDAS_Int8    *pAlloc;
    
    ACQUIRE_CODEC_ENGINE;
   
    /* Initialize .size fields for dummy input and output arguments */
    uniOutArgs.size = sizeof(uniOutArgs);

    /* Set up buffers to pass buffers in and out to alg  */
    inBufDesc.numBufs  = 1;
    outBufDesc.numBufs = 1;
    inOutBufDesc.numBufs = 1;

     /* Fill in input/output buffer descriptor parameters */
     CACHE_WB_INV_INPUT_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(KF,0,sizeof(VLIB_kalmanFilter_4x6));
     CACHE_WB_INV_INPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(Z,0,4*sizeof(short));
     CACHE_INV_OUTPUT_BUFFERS_AND_SETUP_FOR_C6ACCEL(residual,0,4*sizeof(short));  
 
    /* Allocate the InArgs structure as it varies in size
    (Needs to be changed everytime we make a API call)*/
    InArg_Buf_size=  sizeof(Fxn_struct)+
                     sizeof(VLIB_kalmanFilter_4x6_Correct_Params)+
                     sizeof(CInArgs->size)+
                     sizeof(CInArgs->Num_fxns);

    /* Request contiguous memeory allocation for the extended input structure */
    pAlloc=(XDAS_Int8 *)Memory_alloc(InArg_Buf_size, &wrapperMemParams);
    CInArgs= (IC6Accel_InArgs *)pAlloc;

   /* Initialize the extended InArgs structure */
    CInArgs->Num_fxns=1;
    CInArgs->size= InArg_Buf_size;

   /* Set function Id and parameter pointers for first function call */
    CInArgs->fxn[0].FxnID= VLIB_KALMANFILTER_4X6_CORRECT_FXN_ID;
    CInArgs->fxn[0].Param_ptr_offset=sizeof(CInArgs->size)+sizeof(CInArgs->Num_fxns)+sizeof(Fxn_struct);

    /* Initialize pointers to function parameters */
    fp0 = (VLIB_kalmanFilter_4x6_Correct_Params *)((XDAS_Int16*)CInArgs + CInArgs->fxn[0].Param_ptr_offset);

    /* Fill in the fields in the parameter structure */
    fp0->KF_InOutArrID1   = OUTBUF0;
    fp0->Z_InArrID1   = OUTBUF0;
    fp0->residual_OutArrID1   = OUTBUF0; 
    
   /* Call the actual algorithm */
   if (hC6accel->callType == ASYNC)
      {

       /* Update async structure */ 
       if (c6accelAsyncParams.asyncCallCount!=0){ 
            status = UNIVERSAL_EFAIL;
            printf("Async call failed as %d are still pending\n");
          }
       else{
           /* Context Saving */
           c6accelAsyncParams.asyncCallCount++;
           memcpy(&(c6accelAsyncParams.inBufs),&inBufDesc, sizeof (XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.outBufs), &outBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inOutBufs), &inOutBufDesc,sizeof(XDM1_BufDesc));
           memcpy(&(c6accelAsyncParams.inArgs), CInArgs,sizeof(UNIVERSAL_InArgs));
           memcpy(&(c6accelAsyncParams.outArgs),&uniOutArgs,sizeof(UNIVERSAL_OutArgs));
           c6accelAsyncParams.pBuf = pAlloc;
           c6accelAsyncParams.pBufSize = InArg_Buf_size;
           /* Asynchronous Call to the actual algorithm */
           status = UNIVERSAL_processAsync(hC6accel->hUni, &inBufDesc, &outBufDesc, &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
           }
      }
    else{
      /* Synchronous Call to the actual algorithm */
      status = UNIVERSAL_process(hC6accel->hUni, &inBufDesc, &outBufDesc,  &inOutBufDesc,(UNIVERSAL_InArgs *)CInArgs, &uniOutArgs);
      
      /* Free the InArgs structure */
      Memory_free(pAlloc, InArg_Buf_size, &wrapperMemParams);
      
     }

    RELEASE_CODEC_ENGINE;

    return status;  

}

VLIB_CC* C6accel_VLIB_CC_create(void){

VLIB_CC * K;
K = (VLIB_CC *)Memory_alloc(sizeof(VLIB_CC),  &wrapperMemParams);
return K;


}


 


