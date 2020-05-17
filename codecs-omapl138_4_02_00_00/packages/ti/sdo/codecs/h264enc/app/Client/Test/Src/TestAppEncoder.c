/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/*!
********************************************************************************
  @file     TestAppEncoder.c
  @brief    This is the top level client file that drives the H264
            (Baseline Profile) Video Encoder Call using XDM Interface
  @author   Multimedia Codecs TI India
  @version  0.0 - Jan 24,2006    initial version
  @version  0.1 - Sep 21,2006    Changes for FC 1.2 integration
********************************************************************************
*/

/* Standard C header files                                                    */
#include <stdio.h>
#include <stdlib.h>

/* H264 Interface header files                                                */
#include "ih264venc.h"
#include "h264venc.h"
#include "h264venc_ti.h"
#include "h264venc.h"

/* Client header file                                                         */
#include "TestAppEncoder.h"

/* CSL and DMAN3 header files                                                 */
#ifdef C6000
  #include "ti/bios/include/bcache.h"
  #include "dman3.h"
#else
  #include <memory.h>
#endif

//#define KAILASH

/* Input buffer allocation                                                    */
XDAS_Int8  inputData[INPUT_BUFFER_SIZE];
/* Output buffer allocation */
XDAS_Int8  outputData[OUTPUT_BUFFER_SIZE];
/* Reference buffer allocation */
XDAS_Int8  refData[OUTPUT_BUFFER_SIZE];

#ifdef C6000
#pragma    DATA_SECTION(inputData,  ".input_buffer")
#pragma    DATA_SECTION(outputData, ".output_buffer")
#pragma    DATA_SECTION(refData,    ".ref_buffer")
#endif //C6000
#ifdef KAILASH
#define QDMA_BASE  0x02A04000
#endif


/* Base Class Structures                                                      */
IVIDENC1_Params          params;
IVIDENC1_DynamicParams   dynamicParams;
XDAS_UInt32             framesToEncode ;

XDAS_UInt32 DMAN3_QDMA_CHANNELS_H264[DMAN3_TOTALCHANNELS_H264] = {0,1,2,3,4,5};

/* Mapping of Prameters from config file string to the place holder           */
sTokenMapping sTokenMap[] =
{
  {"ImageWidth",     &params.maxWidth          },
  {"ImageHeight",    &params.maxHeight         },
  {"FrameRate",       &params.maxFrameRate      },
  {"Bitrate",         &params.maxBitRate        },
  {"ChromaFormat",    &params.inputChromaFormat },
  {"IntraPeriod",     &dynamicParams.intraFrameInterval},
  {"FramesToEncode",  &framesToEncode           },
  {NULL,              NULL                      }
};

/* Main Function acting as a client for Video Encode Call                     */
int main ()
{
  /* File I/O variables */
  FILE *fConfigFile, *ftestFile, *finFile, *fparamsFile;
  XDAS_Int8 line[STRING_SIZE], inFile[STRING_SIZE], testFile[STRING_SIZE],
    paramsFile[STRING_SIZE], testCompliance, ii;
  XDAS_Int8 *fname = "..\\..\\Test\\TestVecs\\Config\\Testvecs.cfg";

  /* Input/Output  buffers  and their sizes                                   */
  XDAS_Int8*  pInputBuf   [XDM_MAX_IO_BUFFERS];
  XDAS_Int8*  pOutputBuf  [XDM_MAX_IO_BUFFERS];
//  XDAS_Int32  inBufSize   [XDM_MAX_IO_BUFFERS];
//  XDAS_Int32  outBufSize  [XDM_MAX_IO_BUFFERS];


  IVIDENC1_Fxns            *ividEncfxns;

  IVIDENC1_Status          status;
  IVIDENC1_InArgs          inArgs;
  IVIDENC1_OutArgs         outArgs;

  /* Algorithm specific handle                                                */
  IALG_Handle handle;

  /* Input/Output Buffer Descriptor variables */
//  XDM_BufDesc inputBufDesc, outputBufDesc;
  IVIDEO1_BufDescIn              inputBufDesc; //XDM1.0 migration
  XDM_BufDesc outputBufDesc;
  /* DMAN3 specific variables                                                 */
  XDAS_Int32  groupId, numAlgs;
  IALG_Handle handleArray[NUM_ALGS];
  IDMA3_Fxns* dmaFxns[NUM_ALGS];


  /* Output Buffer Management specific variables                              */
  XDAS_UInt32 bytesGenerated;

  /* Other variables                                                          */
  XDAS_UInt32  frameCount, countConfigSet;
  XDAS_Int32   retVal, testVal ;

  /* Enable Cache Settings                                                    */
  TestApp_EnableCache();

  /* Open Test Config File                                                    */
  fConfigFile = fopen(fname,"r");

  if (!fConfigFile)
  {
      printf("Couldn't open parameter file %s",fname);
      return XDM_EFAIL;
  }

  countConfigSet = 1; /* Reset countConfigSet value to 1 */

  /* Read the Config File until it reaches the end of file                    */
  while(!feof(fConfigFile))
  {
    /* Read Compliance Checking parameter */
    if(fgets(line,254,fConfigFile))
    {
      sscanf(line,"%d",&testCompliance);
    }
    else
    {
      /* File doesn't have suffcient data */
      break ;
    }
    /* Read Parameters file name */
    if(fgets(line,254,fConfigFile))
    {
      sscanf(line,"%s",paramsFile);
    }
    else
    {
      /* File doesn't have suffcient data */
      break ;
    }
    /* Read Input file name */
    if(fgets(line,254,fConfigFile))
    {
      sscanf(line,"%s",inFile);
    }
    else
    {
      /* File doesn't have suffcient data */
      break ;
    }

    /* Read Output/Reference file name */
    if(fgets(line,254,fConfigFile))
    {
      sscanf(line,"%s",testFile);
    }
    else
    {
      /* File doesn't have suffcient data */
      break ;
    }

    printf("\n*******************************************");
    printf("\nRead Configuration Set %d",countConfigSet);
    printf("\n*******************************************");
	  countConfigSet++;

    /* Open Parameters file */
    fparamsFile = fopen (paramsFile, "rb");
    if (!fparamsFile)
    {
      printf("\nCouldn't open Parameters file...   %s\n ",paramsFile);
      printf("Exiting for this configuration...\n");
	  continue;
    }

    if(readparamfile(fparamsFile) < 0 )
    {
      printf("\nSyntax Error in %s\n ",paramsFile);
      printf("Exiting for this configuration...\n");
	  continue;
    }

    /* Close Parameters File */
    fclose(fparamsFile);
    if(testCompliance)
    {
      printf("\nRunning in Compliance Mode");
    }
    else
    {
      printf("\nRunning in Output Dump Mode");
    }

    /* Open input file */
    finFile = fopen (inFile, "rb");
    if (!finFile)
    {
      printf("\n Couldn't open Input file...  %s  ",inFile);
      printf("\n Exiting for this configuration...");
	  continue;
    }

    /* Open output/reference file */
    if(testCompliance)
    {
        ftestFile = fopen (testFile, "rb");
    }
    else
    {
        ftestFile = fopen (testFile, "wb");
    }
    if( !ftestFile)
    {
      printf("\n Couldn't open Test File... %s",ftestFile);
      printf("\n Exiting for this configuration..." );
	  continue;
    }

    /* Initialization of parameters needed for Algorithm Instance create */
    TestApp_SetInitParams(&params);

    /* Setting the sizes of Base Class Objects */
    params.size                  = sizeof(IVIDENC1_Params);
    status.size                  = sizeof(IVIDENC1_Status);
    dynamicParams.size           = sizeof(IVIDENC1_DynamicParams);
    inArgs.size                  = sizeof(IVIDENC1_InArgs);
    outArgs.size                 = sizeof(IVIDENC1_OutArgs);

    /* Initializing Variables */
    frameCount                   = 0; /* Tracks the number of frames decoded */

    /* Create the Algorithm object (instance) */
    printf("\nCreating Algorithm Instance...");

    if ((handle =  (IALG_Handle)ALG_create (
      (IALG_Fxns *) &H264VENC_TI_IH264VENC,
      (IALG_Handle) NULL,
      (IALG_Params *) &params)) == NULL)
    {
      printf("\nFailed to Create Instance... Exiting for this configuration..");
	    continue;
    }
    /* Assigning Algorithm handle fxns field to ividdecfxns */
    ividEncfxns = (IVIDENC1_Fxns *)handle->fxns ;

    printf("\nAlgorithm Instance Creation Done...\n");

#ifdef C6000
    /* Initializing DMAN3 parameters */
    /* Base Index Value from where DMA params are written */
    DMAN3_PARAMS.paRamBaseIndex  = DMAN3_PARAMBASE_H264 ;
    /* Number of QDMA channels used by the Algorithm */
    DMAN3_PARAMS.numQdmaChannels = DMAN3_TOTALCHANNELS_H264;
 	  DMAN3_PARAMS.qdmaChannels    = DMAN3_QDMA_CHANNELS_H264 ;
    DMAN3_PARAMS.numPaRamEntries = DMAN3_NUMPARAMENTRIES_H264 ;

    /* Number of Algorithms: System Parameter */
    numAlgs                      = NUM_ALGS;
    /* Group Id needed by DMAN3 library: System Parameter */
    groupId                      = 0;

    DMAN3_PARAMS.numPaRamGroup[groupId] = DMAN3_NUMPARAMENTRIES_H264 ;
#ifdef KAILASH
        /*--------KAILASH_PVS------------*/
    DMAN3_PARAMS.qdmaPaRamBase  = (unsigned int *)QDMA_BASE ;

    /* Base Index Value from where DMA params are written */
    DMAN3_PARAMS.paRamBaseIndex  = 78;
    /* Number of QDMA channels used by the Algorithm */
    DMAN3_PARAMS.numQdmaChannels = 6;
	DMAN3_PARAMS.qdmaChannels    = DMAN3_QDMA_CHANNELS_H264 ;
    /* Number of Algorithms: System Parameter */
    /* Group Id needed by DMAN3 library: System Parameter */
    groupId                      = 0;

    DMAN3_PARAMS.numPaRamEntries = 48;
    DMAN3_PARAMS.maxQdmaChannels = 6;

    /* Sample Configuration of  QDMA properties                               */
    /* Associating the four QDMA channels (0,1,2,3) to Queue 3                */

#endif

    /* DMA allocation using DMAN3 API's */
    DMAN3_init();

    /* Provide the Algorithm handle to DMAN3 Interface for granting channels */
    handleArray[0] = (IALG_Handle) handle;
    dmaFxns[0]     = &H264VENC_TI_IDMA3;

    if (DMAN3_grantDmaChannels(groupId, handleArray, dmaFxns, numAlgs)
         != DMAN3_SOK)
    {
      printf("\nProblem adding algorithm's dma resources");
      printf("... Exiting for this configuration... \n");
	  continue;
    }
#endif  //C6000



    /* Set run time dynamic parameters */
    TestApp_SetDynamicParams(&dynamicParams);

    /* set space for buffer descriptors                                       */
 /*   inputBufDesc.bufs = (XDAS_Int8 **)pInputBuf;
    outputBufDesc.bufs = (XDAS_Int8 **)pOutputBuf;
    inputBufDesc.bufSizes = (XDAS_Int32 *)inBufSize ;
    outputBufDesc.bufSizes = (XDAS_Int32 *)outBufSize ;*/

    inputBufDesc.bufDesc[0].buf = (XDAS_Int8 *)pInputBuf; //using the single buffer descriptor//XDM1.0 migration
    outputBufDesc.bufs= (XDAS_Int8 **)pOutputBuf;

    /* Activate the Algorithm                                                 */
    handle->fxns->algActivate(handle);
    /* Get Buffer information                                                 */
    ividEncfxns->control((IVIDENC1_Handle)handle,
                         XDM_GETBUFINFO,
                         (IVIDENC1_DynamicParams *)&dynamicParams,
                         (IVIDENC1_Status *)&status);

    /* DeActivate the Algorithm                                               */
    handle->fxns->algDeactivate(handle);

    /*Fill up the buffers as required by algorithm                            */
    inputBufDesc.numBufs  = status.bufInfo.minNumInBufs ;
    inputBufDesc.bufDesc[0].buf     = inputData;
    inputBufDesc.bufDesc[0].bufSize = status.bufInfo.minInBufSize[0];

    /* Mark Input Buffer pointers in the contiguous memory allocated by static*/
    /* array inputData                                                        */
    for(ii=0; ii< (status.bufInfo.minNumInBufs - 1);ii++ )
    {
   //   inputBufDesc.bufs[ii+1] = inputBufDesc.bufs[ii] +
        inputBufDesc.bufDesc[ii+1].buf = inputBufDesc.bufDesc[ii].buf +
        status.bufInfo.minInBufSize[ii];

      inputBufDesc.bufDesc[ii+1].bufSize =
        status.bufInfo.minInBufSize[ii +1];
    }

    outputBufDesc.numBufs     = status.bufInfo.minNumOutBufs ;
    outputBufDesc.bufs[0]     = outputData;
    outputBufDesc.bufSizes[0] = status.bufInfo.minOutBufSize[0];

    /* Mark Input Buffer pointers in the contiguous memory allocated by static*/
    /* array outputData                                                       */
    for(ii=0; ii< (status.bufInfo.minNumOutBufs-1); ii++ )
    {
      outputBufDesc.bufs[ii+1] = outputBufDesc.bufs[ii] +
      status.bufInfo.minOutBufSize[ii];

      outputBufDesc.bufSizes[ii+1] =
        status.bufInfo.minOutBufSize[ii+1];
    }

    /* Assigning Algorithm handle fxns field to ividEncfxns                   */
    ividEncfxns = (IVIDENC1_Fxns *)handle->fxns;

    /* Resetting bytesGenerated variable                                      */
    bytesGenerated = 0;

    /* Do-While Loop for Encode Call                                          */
    do
    {
      /* Read from Input File                                                 */
      TestApp_ReadInputData(finFile,
        &inputBufDesc);

#ifdef C6000
      for(ii=0 ; ii < inputBufDesc.numBufs ; ii++ )
      {
        /* Cache Invalidate for Input Buffer */
        BCACHE_wbInv(inputBufDesc.bufDesc[ii].buf,
			inputBufDesc.bufDesc[ii].bufSize,
			TRUE);
      }
#endif  //C6000

      /* Activate the Algorithm                                               */
      handle->fxns->algActivate(handle);

      /* Optional: Set Run time parameters in the Algorithm via control()     */
      ividEncfxns->control((IVIDENC1_Handle)handle, XDM_SETPARAMS,
        (IVIDENC1_DynamicParams *)&dynamicParams, (IVIDENC1_Status *)&status);

      /* Basic Algorithm process() call */
      retVal = ividEncfxns->process((IVIDENC1_Handle)handle,
        (IVIDEO1_BufDescIn *)&inputBufDesc,
        (XDM_BufDesc *)&outputBufDesc,
        (IVIDENC1_InArgs *)&inArgs,
        (IVIDENC1_OutArgs *)&outArgs);

      if(retVal == XDM_EFAIL)
      {
        printf("\n Process function returned an Error...  ");
        printf("\n Exiting with error code %0x...  ",
          outArgs.extendedError);
        break; /* Error Condition: Application may want to break off          */
      }

      /* Optional: Read status via control()                                  */
      ividEncfxns->control((IVIDENC1_Handle)handle,
        XDM_GETSTATUS,
        (IVIDENC1_DynamicParams *)&dynamicParams,
        (IVIDENC1_Status *)&status);

      /* DeActivate the Algorithm                                             */
      handle->fxns->algDeactivate(handle);

#ifdef C6000
      /* Cache Writeback Invalidate for Output Buffers                        */
      for(ii=0 ; ii < outputBufDesc.numBufs ; ii++ )
      {
        BCACHE_wbInv(outputBufDesc.bufs[ii],
          outputBufDesc.bufSizes[ii],
          TRUE);
      }
#endif //C6000

      /* Check for frame ready via recon buffer information                   */
      if(outArgs.reconBufs.numBufs)
      {
          printf("\n Encoded Frame # %d  ",frameCount);
		  fflush(stdout);

          if(testCompliance)
          {
            /* Compare the output frames with the Reference File */
            testVal = TestApp_CompareOutputData(ftestFile,
                                                (XDM_BufDesc *)&outputBufDesc,
                                                &outArgs);
            if(testVal != XDM_EOK)
            {
              /* Test Compliance Failed... Breaking...*/
              printf( "\n Encoder compliance test failed for this frame. ");
              break;
            }
          }
          else
          {
            /* Write the output frames in the display order */
            TestApp_WriteOutputData(ftestFile, &outputBufDesc, &outArgs);
          }
          frameCount++;
      }
      bytesGenerated += outArgs.bytesGenerated ;

      /* Check for Break off condition if requsated number of frames encoded */
      if (frameCount >= framesToEncode)
      {
        printf("\n Specified number of Frames Encoded...  ");
        break;
      }

    }  while(1);  /* end of Do-While loop of encoding each frame */

    if(testVal == XDM_EOK && testCompliance )
    {
      printf("\n Encoder compliance test passed");
    }

    if(testVal == XDM_EOK && (!testCompliance) )
    {
      printf("\n Encoder output dump completed");
    }
    /* Output file close */
    fclose(ftestFile);

#ifdef C6000

    /* Reclaim DMA resources from algorithm and deinitialize the DMA manager  */
    if (DMAN3_releaseDmaChannels( handleArray, dmaFxns, numAlgs) != DMAN3_SOK)
    {
      printf("\nProblem removing algorithm's dma resources..." );
      printf("Exiting for this configuration... ");
      continue;
    }
    DMAN3_exit();
#endif  //C6000

    /* Delete the Algorithm instance object specified by handle */
    ALG_delete (handle);

    if(frameCount == 0)
    {
      frameCount = 1; /* To avoid division with zero */
    }
    printf("\n --------------  SUMMARY --------------------\n");
    if(!testCompliance)
    {
      printf(" Encoder output dump completed\n");
    }
    printf("  Total number of Frames          = %d\n",
      (XDAS_UInt32)frameCount);
    printf("  Bit Rate at %d frames/Sec       = %d Kbps\n",params.maxFrameRate,(
      XDAS_UInt32)(((bytesGenerated*8*params.maxFrameRate)/frameCount)/1000));
    printf("  Width and Height                = %d, %d \n",
      dynamicParams.inputWidth, dynamicParams.inputHeight);
    printf (" --------------    END   --------------------\n");

  } /* Read the Config File until it reaches the end of file */

  printf("\n End of execution\n");
  /* Close the config files */
  fclose(fConfigFile);

  return XDM_EOK;
} /* main() */

/*
//============================================================================
// TestApp_ReadInputData
//  Reading Byte Stream from a File
*/

XDAS_Int32 TestApp_ReadInputData(FILE *finFile,
                                  IVIDEO1_BufDescIn* inputBufDesc)
{
    XDAS_Int32 i, bytesRead ;

    for(i=0; i < inputBufDesc->numBufs ; i++)
    {
      /*Read the input buffers from FILE                                      */
      bytesRead = fread(inputBufDesc->bufDesc[i].buf,
                        1,
                        inputBufDesc->bufDesc[i].bufSize,
                        finFile);
#ifdef C6000
      /* Make sure that external memory contain correct copy of input data    */
      BCACHE_wbInv(inputBufDesc->bufDesc[i].buf,
                 inputBufDesc->bufDesc[i].bufSize,
                 TRUE);

#endif //C6000

      if(bytesRead != inputBufDesc->bufDesc[i].bufSize)
      {
        bytesRead = -1 ;
        /* File doesn't have suffcient data */
        break ;
      }
    }
    return (bytesRead);
}

/*
//============================================================================
// TestApp_CompareOutputData
//  Comparing Output Data with Reference File data
*/

XDAS_Int32 TestApp_CompareOutputData(FILE *fRefFile,
                                     XDM_BufDesc * outputBufDesc,
                                     IVIDENC1_OutArgs *outArgs)
{
  XDAS_Int32 i, retVal ;

  retVal = XDM_EOK;

  /* Compare all the output Buffers with the ref File                         */
  for(i=0; i < outputBufDesc->numBufs ; i++)
  {
    fread(refData, 1, outArgs->bytesGenerated, fRefFile);
    if(memcmp(refData, outputBufDesc->bufs[i], outArgs->bytesGenerated))
    {
      retVal = XDM_EFAIL;
      /* Mismatch occured so no point to continue further */
      break ;
    }
  }

  return retVal;
}

/*
//============================================================================
// TestApp_WriteOutputData
//  Writing Output Data in a File
*/


XDAS_Void TestApp_WriteOutputData(FILE *fOutFile,
                                  XDM_BufDesc * outputBufDesc,
                                  IVIDENC1_OutArgs *outArgs)
{

  XDAS_Int32 i = 0;

  /****************************************************************************/
  /* Write the bitstream                                                      */
  /****************************************************************************/
  for(i=0; i < outputBufDesc->numBufs ; i++)
  {
    if(outArgs->bytesGenerated != 0 )
    {
      fwrite( outputBufDesc->bufs[i],
        1,
        outArgs->bytesGenerated,
        fOutFile);
    }
  }

}

/*
//============================================================================
// TestApp_SetInitParams
//  setting of creation time parameters
*/

XDAS_Void TestApp_SetInitParams(IVIDENC1_Params *params)
{
  params->encodingPreset         = XDM_DEFAULT        ;
  params->rateControlPreset      = IVIDEO_LOW_DELAY   ;
  params->dataEndianness         = XDM_BYTE           ;
  params->maxInterFrameInterval  = 1                  ;
  params->inputContentType       = IVIDEO_PROGRESSIVE ;
  params->reconChromaFormat      = XDM_CHROMA_NA;

  return;
}

/*
//============================================================================
// TestApp_SetDynamicParams
//  setting of run time parameters
*/

XDAS_Void TestApp_SetDynamicParams(IVIDENC1_DynamicParams *dynamicParams)
{

  dynamicParams->inputHeight     = params.maxHeight    ;
  dynamicParams->inputWidth      = params.maxWidth     ;
  dynamicParams->refFrameRate    = params.maxFrameRate ;
  dynamicParams->targetFrameRate = params.maxFrameRate ;
  dynamicParams->targetBitRate   = params.maxBitRate   ;
  dynamicParams->generateHeader  = XDM_ENCODE_AU       ;
  dynamicParams->captureWidth    = 0                   ;
  dynamicParams->forceFrame     = IVIDEO_NA_FRAME;
  dynamicParams->mbDataFlag  = 0;
  dynamicParams->interFrameInterval = 1;

  return;
}


#define EXT_MEM_BASE (0x80000000)
#define EXT_MEM_SIZE (0x10000000)

  /* Cache Settings */
  XDAS_Void TestApp_EnableCache(void)
  {
#ifdef C6000
	BCACHE_Size size;

  size.l1psize  = BCACHE_L1_32K; /* L1P cache size */
  size.l1dsize  = BCACHE_L1_16K; /* L1D cache size */
  size.l2size   = BCACHE_L2_64K; /* L2  cache size */

	/* Set L1P, L1D and L2 cache sizes */
    BCACHE_setSize(&size);

    /* Cache Enable External Memory Space */
	/* BaseAddr, length, MAR enable/disable */
    /* Cache 0x80000000 --- 0x8FFFFFFF   */
    BCACHE_setMar((Ptr *)EXT_MEM_BASE, EXT_MEM_SIZE, BCACHE_MAR_ENABLE);
    BCACHE_wbInvAll(); /* No need to call BCACHE_wait() */
#endif /* C6000 */
  } /* TestApp_EnableCache */

/******************************************************************************/
/*    Copyright (c) 2006 Texas Instruments, Incorporated                      */
/*    All Rights Reserved                                                     */
/******************************************************************************/
