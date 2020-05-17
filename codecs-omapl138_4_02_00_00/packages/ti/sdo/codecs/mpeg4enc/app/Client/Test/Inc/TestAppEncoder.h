/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/*!
********************************************************************************
  @file     TestAppEncoder.h
  @brief    This is the top level client header file that drives the H264
            (Baseline Profile) Video Encoder Call using XDM Interface
  @author   Multimedia Codecs TI India
  @version  0.0 - Jan 24,2006    initial version
********************************************************************************
*/

#ifndef _TESTAPP_ENCODER_
#define _TESTAPP_ENCODER_

#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/xdm.h >
#include <ti/xdais/dm/ivideo.h>
#include <ti/xdais/dm/ividenc1.h>			//XDM1.0 

/* Macro definitions for constants and Buffer sizes                           */
#define IMAGE_WIDTH         720      //!< Width of the Display Buffer      
#define IMAGE_HEIGHT        576      //!< Height of the Display Buffer 
#define INPUT_BUFFER_SIZE   IMAGE_WIDTH*IMAGE_HEIGHT*2 //!< Input Buf Size
#define OUTPUT_BUFFER_SIZE  512*1024 //!< Upto 512 KBytes 
#define STRING_SIZE         256      //!< Size of strings used for filenames
#define NUM_ALGS            1        //!< Number of Algorithms
#define MV_DATA_SIZE ((720*576)>>7)  // Size of MV data array

/*!
@struct sTokenMapping 
@brief  Token Mapping structure for parsing codec specific configuration file
@param  tokenName : String name in the configuration file
@param  place     : Place Holder for the data
*/

typedef struct sTokenMapping{
  XDAS_Int8 *tokenName;
  XDAS_Void *place;
} sTokenMapping;

/*!
@struct motion_mbdata 
@brief  Structure for accessing Motion vector data
@param  MVx : Horizontal Motion vector
@param  MVy : vertical Motion vector
@param  SSE : Sum of Squared Error between the reference and best match
*/
typedef struct motion_mbdata 
{
       short MVx;      
       short MVy;      
       int SSE;          
}motion_mbdata ;

/* Function Declarations                                                      */
/*!
********************************************************************************
	@fn									XDAS_Void  TestApp_EnableCache(void)
	@brief							Enable cache settings for system
  @param   void       None
	@return							None
********************************************************************************
*/

XDAS_Void  TestApp_EnableCache(void);

/*!
********************************************************************************
  @fn									      XDAS_Int32 TestApp_ReadInputData(FILE *finFile,
                                   IVIDEO1_BufDescIn* inputBufDesc);
	@brief							      Reads Input Data from FILE
  @param  finFile[IN]       Input File Handle
  @param  inputBufDesc[IN]  Buffer Descriptor for input buffers
	@return							      Status of the operation ( PASS / FAIL )
********************************************************************************
*/
XDAS_Int32 TestApp_ReadInputData(FILE *finFile,
                                 IVIDEO1_BufDescIn* inputBufDesc);

/*!
********************************************************************************
  @fn									      XDAS_Void TestApp_WriteOutputData(FILE *fOutFile, 
                                  XDM_BufDesc * outputBufDesc,  
                                  IVIDENC1_OutArgs *outArgs);
	@brief							      Writes Output Data into FILE
  @param  fOutFile[IN]      Output File Handle
  @param  outputBufDesc[IN] Buffer Descriptor for output buffers
  @param  outArgs[IN]       Info for args related to output buf(like numBytes)
	@return							      NONE
********************************************************************************
*/

XDAS_Void TestApp_WriteOutputData(FILE *fOutFile, 
                                  XDM_BufDesc * outputBufDesc,  
                                  IVIDENC1_OutArgs *outArgs);

/*!
********************************************************************************
  @fn									      XDAS_Int32 TestApp_CompareOutputData(FILE *fRefFile, 
                                     XDM_BufDesc * outputBufDesc, 
                                     IVIDENC1_OutArgs *outArgs, 
                                     XDAS_Int32 offset);
	@brief							      Compares Output Data with Reference
  @param  fRefFile[IN]      Reference File Handle
  @param  outputBufDesc[IN] Buffer Descriptor for output buffers
  @param  outArgs[IN]       Info for args related to output buf(like numBytes)
	@return							      NONE
********************************************************************************
*/
XDAS_Int32 TestApp_CompareOutputData(FILE *fRefFile, 
                                     XDM_BufDesc * outputBufDesc, 
                                     IVIDENC1_OutArgs *outArgs);
/*!
********************************************************************************
  @fn									      XDAS_Void TestApp_SetInitParams(
                                IVIDENC1_Params *params);
	@brief							      Set creation time parameters
  @param  params[IN]        Creation parameters
	@return							      NONE
********************************************************************************
*/

XDAS_Void TestApp_SetInitParams(IVIDENC1_Params *params);

/*!
********************************************************************************
  @fn									      XDAS_Void TestApp_SetDynamicParams(
                                IVIDENC1_DynamicParams *dynamicParams);
	@brief							      Set run time parameters
  @param  dynamicParams[IN] run time parameters
	@return							      NONE
********************************************************************************
*/

XDAS_Void TestApp_SetDynamicParams(IVIDENC1_DynamicParams *dynamicParams);

/*!
********************************************************************************
  @fn									      XDAS_Int32 readparamfile(FILE * fname) ;
	@brief							      Parses codec specific parameter file and populates
                            configurable parameters
  @param  fname[IN]         parameter FILE handle
	@return							      NONE
********************************************************************************
*/
XDAS_Int32 readparamfile(FILE * fname) ;

#endif //_TESTAPP_ENCODER_

/******************************************************************************/
/*    Copyright (c) 2006 Texas Instruments, Incorporated                      */
/*    All Rights Reserved                                                     */
/******************************************************************************/

