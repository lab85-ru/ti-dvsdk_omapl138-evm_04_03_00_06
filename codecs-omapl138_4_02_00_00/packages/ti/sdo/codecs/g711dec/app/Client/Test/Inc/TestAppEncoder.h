/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/*!
********************************************************************************
  @file     TestAppEncoder.h
  @brief    This is the top level client header file that drives the G711
            Speech Encoder Call using XDM Interface
  @author   Multimedia Codecs TI India
  @version  0.1 - Jan 30,2006    initial version
********************************************************************************
*/

#ifndef _TESTAPP_ENCODER_
#define _TESTAPP_ENCODER_

/* XDAIS header files */
#include <xdas.h>

/* XDM header files */
#include <xdm.h>
#include <ispeech1.h>
#include <isphenc1.h>

/* G711 Interface header files */
#include <g711enc_tii.h>

#define NUMBER_OF_SAMPLES   80
#define INPUT_BUFFER_SIZE   (NUMBER_OF_SAMPLES*2) /* speech samples in bytes */
#define OUTPUT_BUFFER_SIZE  NUMBER_OF_SAMPLES     /* codewords size in bytes */

/*!
********************************************************************************
  @fn                       XDAS_Int32  TestApp_ReadInputData(FILE *finFile,
                                          XDM1_SingleBufDesc *inputBufDesc);
  @brief                    Reads Data into input buffer
  @param  finFile[IN]       Input File Handle
  @param  inputBufDesc[IN]  Buffer Descriptor for input buffer
  @return                   Returns the bytes read
********************************************************************************
*/
XDAS_Int32   TestApp_ReadInputData(FILE *finFile,
                                    XDM1_SingleBufDesc *inputBufDesc);

/*!
********************************************************************************
  @fn                       XDAS_Void TestApp_WriteOutputData(FILE *fOutFile,
                                        XDM1_SingleBufDesc *outputBufDesc,
                                        ISPHENC1_OutArgs *outArgs);
  @brief                    Writes Output Data into FILE
  @param  fOutFile[IN]      Output File Handle
  @param  outputBufDesc[IN] Buffer Descriptor for output buffers
  @param  outArgs[IN]       Info for args related to output buf(like numBytes)
  @return                   NONE
********************************************************************************
*/
XDAS_Void   TestApp_WriteOutputData(FILE *fOutFile,
                                    XDM1_SingleBufDesc *outputBufDesc,
                                    ISPHENC1_OutArgs *outArgs);

/*!
********************************************************************************
  @fn                       XDAS_Int32 TestApp_CompareOutputData(FILE *fRefFile,
                                        XDM1_SingleBufDesc *outputBufDesc, 
                                        ISPHENC1_OutArgs *outArgs);
  @brief                    Compares Output Data with Reference
  @param  fRefFile[IN]      Reference File Handle
  @param  outputBufDesc[IN] Buffer Descriptor for output buffers
  @param  outArgs[IN]       Info for args related to output buf(like numBytes)
  @return                   Returns the status of the comparision
********************************************************************************
*/
XDAS_Int32  TestApp_CompareOutputData(FILE *fRefFile,
                                      XDM1_SingleBufDesc *outputBufDesc,
                                      ISPHENC1_OutArgs *outArgs);

/*!
********************************************************************************
  @fn                       XDAS_Void TestApp_SetInitParams(
                                        ISPHENC1_Params *params);
  @brief                    Set creation time parameters
  @param  params[IN]        Creation parameters
  @return                   NONE
********************************************************************************
*/

XDAS_Void   TestApp_SetInitParams(ISPHENC1_Params *Params);

/*!
********************************************************************************
  @fn                       XDAS_Void TestApp_SetDynamicParams(
                                        ISPHENC1_DynamicParams *dynamicParams);
  @brief                    Set run time parameters
  @param  dynamicParams[IN] run time parameters
  @return                   NONE
********************************************************************************
*/
XDAS_Void   TestApp_SetDynamicParams(ISPHENC1_DynamicParams *DynamicParams);

#endif //_TESTAPP_ENCODER_

/******************************************************************************/
/*    Copyright (c) 2006 Texas Instruments, Incorporated                      */
/*    All Rights Reserved                                                     */
/******************************************************************************/
