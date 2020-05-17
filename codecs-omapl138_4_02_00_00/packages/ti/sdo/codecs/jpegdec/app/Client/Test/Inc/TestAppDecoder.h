/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/*!
********************************************************************************
  @file     TestAppDecoder.h
  @brief    This is the top level client header file that drives the H264
            (Baseline Profile) Video Decoder Call using XDM Interface
  @author   Multimedia Codecs TI India
  @version  0.1 - Jan 24,2006    initial version
********************************************************************************
*/

#ifndef _TESTAPP_DECODER_
#define _TESTAPP_DECODER_

#include <ti/bios/include/std.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/xdm.h>
#include "ti/xdais/dm/iimgdec1.h"

#define IMAGE_WIDTH            2048      /* Width of the Display Buffer       */      
#define IMAGE_HEIGHT           1600      /* Height of the Display Buffer      */
#define INPUT_BUFFER_SIZE      0x200000  /* Upto 131072 Bytes File size       */
#define OUTPUT_BUFFER_SIZE     IMAGE_WIDTH*IMAGE_HEIGHT*2
#define STRING_SIZE            256
#define NUM_ALGS                 1      /* Number of Algorithms              */

/*!
@struct sTokenMapping 
@brief  Token Mapping structure for parsing codec specific configuration file
@param  tokenName : String name in the configuration file
@param  place     : Place Holder for the data
*/

typedef struct _sTokenMapping {
  XDAS_Int8 *tokenName;
  XDAS_Void *place;
} sTokenMapping;

/* Function Declarations */
/*!
********************************************************************************
	@fn									XDAS_Void  TestApp_EnableCache(void)
	@brief							Enable cache settings for system
  @param   void       None
	@return							None
********************************************************************************
*/
XDAS_Void   TestApp_EnableCache(void);

/*!
********************************************************************************
  @fn									      XDAS_Int32 TestApp_ReadByteStream(FILE *finFile);
	@brief							      Reads Input Data from FILE
  @param  finFile[IN]       Input File Handle
  @param  inputBufDesc[IN]  Buffer Descriptor for input buffers
	@return							      Status of the operation ( PASS / FAIL )
********************************************************************************
*/
XDAS_Int32  TestApp_ReadByteStream(FILE *fInFile);

/*!
********************************************************************************
  @fn									      XDAS_Void TestApp_WriteOutputData(FILE *fOutFile, 
                                  XMI_BufDesc * outputBufDesc,  
                                  IIMGDEC_OutArgs *outArgs);
	@brief							      Writes Output Data into FILE
  @param  fOutFile[IN]      Output File Handle
  @param  outputBufDesc[IN] Buffer Descriptor for output buffers
  @param  outArgs[IN]       Info for args related to output buf(like numBytes)
	@return							      NONE
********************************************************************************
*/
XDAS_Void   TestApp_WriteOutputData(FILE *fOutFile, XDM1_BufDesc * outputBufDesc, 
                                    IIMGDEC1_OutArgs *outArgs);


/*!
********************************************************************************
  @fn									      XDAS_Int32 TestApp_CompareOutputData(FILE *fRefFile, 
                                     XMI_BufDesc * outputBufDesc, 
                                     IIMGDEC_OutArgs *outArgs, 
                                     XDAS_Int32 offset);
	@brief							      Compares Output Data with Reference
  @param  fRefFile[IN]      Reference File Handle
  @param  outputBufDesc[IN] Buffer Descriptor for output buffers
  @param  outArgs[IN]       Info for args related to output buf(like numBytes)
	@return							      NONE
********************************************************************************
*/
XDAS_Int32  TestApp_CompareOutputData(FILE *fRefFile, XDM1_BufDesc * outputBufDesc, 
                                      IIMGDEC1_OutArgs *outArgs);

/*!
********************************************************************************
  @fn									      XDAS_Void TestApp_SetInitParams(
                                IIMGDEC_Params *params);
	@brief							      Set creation time parameters
  @param  params[IN]        Creation parameters
	@return							      NONE
********************************************************************************
*/

XDAS_Void   TestApp_SetInitParams(IIMGDEC1_Params *Params);

/*!
********************************************************************************
  @fn									      XDAS_Void TestApp_SetDynamicParams(
                                IIMGDEC_DynamicParams *dynamicParams);
	@brief							      Set run time parameters
  @param  dynamicParams[IN] run time parameters
	@return							      NONE
********************************************************************************
*/
XDAS_Void   TestApp_SetDynamicParams(IIMGDEC1_DynamicParams *DynamicParams);


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

#endif //_TESTAPP_DECODER_

/******************************************************************************/
/*    Copyright (c) 2006 Texas Instruments, Incorporated                      */
/*    All Rights Reserved                                                     */
/******************************************************************************/
