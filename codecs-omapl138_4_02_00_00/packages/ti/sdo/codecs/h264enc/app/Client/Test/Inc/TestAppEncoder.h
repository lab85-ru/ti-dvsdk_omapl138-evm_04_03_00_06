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
  @version  0.1 - Sep 21,2006    Changes for FC 1.2 integration
********************************************************************************
*/


#ifndef _TESTAPP_ENCODER_
#define _TESTAPP_ENCODER_

#include <ti/bios/include/std.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/xdm.h >
#include <ti/xdais/dm/ivideo.h>
//#include <ividenc.h>
#include <ti/xdais/dm/ividenc1.h>

/* Macro definitions for constants and Buffer sizes                           */
#ifdef HD_SUPPORT
#define IMAGE_WIDTH                 3840      //!< Width of the Display Buffer
#define IMAGE_HEIGHT                2176      //!< Height of the Display Buffer
//#define OUTPUT_BUFFER_SIZE          2048*1024 //!< Upto 2048*4 KBytes
#define OUTPUT_BUFFER_SIZE          3840*2176 //!< Upto 2048*4 KBytes
#else
#define IMAGE_WIDTH                 720      //!< Width of the Display Buffer      
#define IMAGE_HEIGHT                480      //!< Height of the Display Buffer 
#define OUTPUT_BUFFER_SIZE          512*1024 //!< Upto 512 KBytes
#endif
#define INPUT_BUFFER_SIZE           IMAGE_WIDTH*IMAGE_HEIGHT*2 //!< Input Buf Size
#define STRING_SIZE                 256      //!< Size of strings used for filenames
#define NUM_ALGS                    1        //!< Number of Algorithms
#define DMAN3_PARAMBASE_H264        78       //!< Base index in parameter RAM
#define DMAN3_TOTALCHANNELS_H264    6        //!< Number of channels used by h264
#define DMAN3_NUMPARAMENTRIES_H264  48       //!< Number of param enteries requested by algortihm
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

/* Function Declarations                                                      */
/*!
********************************************************************************
  @fn                 XDAS_Void  TestApp_EnableCache(void)
  @brief              Enable cache settings for system
  @param   void       None
  @return             None
********************************************************************************
*/

XDAS_Void  TestApp_EnableCache(void);

/*!
********************************************************************************
  @fn                       XDAS_Int32 TestApp_ReadInputData(FILE *finFile,
                                   XDM_BufDesc* inputBufDesc);
  @brief                    Reads Input Data from FILE
  @param  finFile[IN]       Input File Handle
  @param  inputBufDesc[IN]  Buffer Descriptor for input buffers
  @return                   Status of the operation ( PASS / FAIL )
********************************************************************************
*/
XDAS_Int32 TestApp_ReadInputData(FILE *finFile,
                                 IVIDEO1_BufDescIn* inputBufDesc);

/*!
********************************************************************************
  @fn                       XDAS_Void TestApp_WriteOutputData(FILE *fOutFile, 
                                  XDM_BufDesc * outputBufDesc,  
                                  IVIDENC_OutArgs *outArgs);
  @brief                    Writes Output Data into FILE
  @param  fOutFile[IN]      Output File Handle
  @param  outputBufDesc[IN] Buffer Descriptor for output buffers
  @param  outArgs[IN]       Info for args related to output buf(like numBytes)
  @return                   NONE
********************************************************************************
*/

XDAS_Void TestApp_WriteOutputData(FILE *fOutFile, 
                                  XDM_BufDesc * outputBufDesc,  
                                  IVIDENC1_OutArgs *outArgs);

/*!
********************************************************************************
  @fn                       XDAS_Int32 TestApp_CompareOutputData(FILE *fRefFile, 
                                     XDM_BufDesc * outputBufDesc, 
                                     IVIDENC_OutArgs *outArgs, 
                                     XDAS_Int32 offset);
  @brief                    Compares Output Data with Reference
  @param  fRefFile[IN]      Reference File Handle
  @param  outputBufDesc[IN] Buffer Descriptor for output buffers
  @param  outArgs[IN]       Info for args related to output buf(like numBytes)
  @return                   NONE
********************************************************************************
*/
XDAS_Int32 TestApp_CompareOutputData(FILE *fRefFile, 
                                     XDM_BufDesc * outputBufDesc, 
                                     IVIDENC1_OutArgs *outArgs);
/*!
********************************************************************************
  @fn                       XDAS_Void TestApp_SetInitParams(
                                IVIDENC_Params *params);
  @brief                    Set creation time parameters
  @param  params[IN]        Creation parameters
  @return                   NONE
********************************************************************************
*/

XDAS_Void TestApp_SetInitParams(IVIDENC1_Params *params);

/*!
********************************************************************************
  @fn                       XDAS_Void TestApp_SetDynamicParams(
                                IVIDENC_DynamicParams *dynamicParams);
  @brief                    Set run time parameters
  @param  dynamicParams[IN] run time parameters
  @return                   NONE
********************************************************************************
*/

XDAS_Void TestApp_SetDynamicParams(IVIDENC1_DynamicParams *dynamicParams);

/*!
********************************************************************************
  @fn                       XDAS_Int32 readparamfile(FILE * fname) ;
  @brief                    Parses codec specific parameter file and populates
                            configurable parameters
  @param  fname[IN]         parameter FILE handle
  @return                   NONE
********************************************************************************
*/
XDAS_Int32 readparamfile(FILE * fname) ;

#endif //_TESTAPP_ENCODER_

/******************************************************************************/
/*    Copyright (c) 2006 Texas Instruments, Incorporated                      */
/*    All Rights Reserved                                                     */
/******************************************************************************/
