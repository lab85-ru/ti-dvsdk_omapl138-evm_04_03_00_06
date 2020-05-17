/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/*!
********************************************************************************
  @file     TestAppEncoder.c
  @brief    This is the top level client file that drives the MPEG4 Encoder
            (Baseline Profile) Video Encoder Call using XDM Interface
  @author   Multimedia Codecs TI India
  @version  0.0 - Jan 24,2006    initial version
********************************************************************************
*/

/* Standard C header files                                                    */
#include <stdio.h>
#include <stdlib.h>

/* MPEG4 Encoder Interface header files                                                */
#ifdef SETUP_VC
#include <std.h>
#else
#include <ti/bios/include/std.h> // Take from Bios
#endif

#ifdef SETUP_VC
#include <ialg.h>
#else
#include <ti/xdais/ialg.h>
#endif

#include "mp4venc_ti.h"
#include "imp4venc.h"

/* Client header file                                                         */
#include "TestAppEncoder.h"

/* CSL and DMAN3 header files                                                 */
#ifdef C6000
  #include "bcache.h"
  #include "dman3.h"
#else
  #include <memory.h>
#endif

#ifdef DAVINCI_SPECIFIC
#define  QDMA_BASE   0x01C00000
#define  NUM_QDMA_CHANNELS    6
#endif
/* Input buffer allocation                                                    */
XDAS_Int8  inputData[INPUT_BUFFER_SIZE];
/* Output buffer allocation */
XDAS_Int8  outputData[OUTPUT_BUFFER_SIZE + MV_DATA_SIZE];
/* Reference buffer allocation */
XDAS_Int8  refData[OUTPUT_BUFFER_SIZE];

/* Reference buffer allocation */
XDAS_Int8  mvBuffer[MV_DATA_SIZE];

#ifdef C6000
#pragma    DATA_SECTION(inputData,  ".input_buffer")
#pragma    DATA_SECTION(outputData, ".output_buffer")
#pragma    DATA_SECTION(refData,    ".output_buffer")
#endif //C6000

//#define WRITE_MV_DATA
/* Base Class Structures                                                      */
IVIDENC1_Params          params;
IVIDENC1_DynamicParams   dynamicParams;
XDAS_UInt32             framesToEncode, headerFlag ;

/*Extended Class structures*/
IMP4VENC_Params          extn_params;
IMP4VENC_DynamicParams   extn_dynamicParams;

XDAS_UInt32 DMAN3_QDMA_CHANNELS_MP4[6] = {0,1,2,3,4,5};

/* Mapping of Prameters from config file string to the place holder           */
#ifndef XDM_BASE_CLASS_ONLY
sTokenMapping sTokenMap[] =
{
  {"ImageWidth",     &params.maxWidth          },
  {"ImageHeight",    &params.maxHeight         },
  {"FrameRate",       &params.maxFrameRate      },
  {"Bitrate",         &params.maxBitRate        },
  {"ChromaFormat",    &params.inputChromaFormat },
  {"IntraPeriod",     &dynamicParams.intraFrameInterval},
  {"FramesToEncode",  &framesToEncode           },

  /*Codec specific extended parameters*/
  {"EncodingPreset",     &params.encodingPreset  },
  {"RateControlPreset",  &params.rateControlPreset  },
  {"EncodeMode",         &extn_params.encodeMode    }, 
  {"LevelIdc",           &extn_params.levelIdc      }, 
  {"RateControlMethod",  &extn_params.rcAlgo        },
  {"VBVBufSize",         &extn_params.vbvBufferSize },
  {"MaxDelay",           &extn_params.maxDelay      },
  {"UseVOS",             &extn_params.useVOS     },
  {"UseGOV",             &extn_params.useGOV    },
  {"DPEnable",           &extn_params.useDataPartition },
  {"RVLCEnable",         &extn_params.useRVLC    },

  {"ResyncInterval",     &extn_dynamicParams.resyncInterval },
  {"HECInterval",        &extn_dynamicParams.hecInterval  },
  {"AIRRate",            &extn_dynamicParams.airRate      },
  {"MIRRate",            &extn_dynamicParams.mirRate      },  
  {"QpIntra",            &extn_dynamicParams.qpIntra      },
  {"QpInter",            &extn_dynamicParams.qpInter      },
  {"Fcode",              &extn_dynamicParams.fCode        },
  {"UseHpi",             &extn_dynamicParams.useHpi       },
  {"UseAcPred",          &extn_dynamicParams.useAcPred },  
  {"LatFrameFlag",       &extn_dynamicParams.lastFrame },
  {"EncodeHeaderOnly",   &headerFlag },
  {"EnableMVAccess",     &extn_dynamicParams.MVDataEnable },
  {"EnableUMV",          &extn_dynamicParams.useUMV },
  {"EnableSCD",           &extn_params.enableSCD},
  {"QpMax", &extn_dynamicParams.qpMax },
  {"QpMin", &extn_dynamicParams.qpMin },
  {NULL,                 NULL                             }
};
#else
sTokenMapping sTokenMap[] =
{
  {"ImageWidth",     &params.maxWidth          },
  {"ImageHeight",    &params.maxHeight         },
  {"FrameRate",       &params.maxFrameRate      },
  {"Bitrate",         &params.maxBitRate        },
  {"ChromaFormat",    &params.inputChromaFormat },
  {"IntraPeriod",     &dynamicParams.intraFrameInterval},
  {"FramesToEncode",  &framesToEncode           },
  {"EncodeHeaderOnly",   &headerFlag },
  {NULL,                 NULL                             }
};

#endif

/* Main Function acting as a client for Video Encode Call                     */
XDAS_Int32 main ()
{
  /* File I/O variables */
  FILE *fConfigFile, *ftestFile, *finFile, *fparamsFile;
#ifdef WRITE_MV_DATA
  FILE *mvfd;
   XDAS_Int32  jj ;
   motion_mbdata  *mbMV_data;
#endif
  XDAS_Int8 line[STRING_SIZE], inFile[STRING_SIZE], testFile[STRING_SIZE],
    paramsFile[STRING_SIZE], testCompliance, ii;
  XDAS_Int8 *fname = "..\\..\\Test\\TestVecs\\Config\\Testvecs.cfg";

  /* Input/Output  buffers  and their sizes */
 // XDAS_Int8*  pInputBuf   [XDM_MAX_IO_BUFFERS];xdm1.0
  XDAS_Int8*  pOutputBuf  [XDM_MAX_IO_BUFFERS];
 // XDAS_Int32  inBufSize   [XDM_MAX_IO_BUFFERS];
  XDAS_Int32  outBufSize  [XDM_MAX_IO_BUFFERS];


  IVIDENC1_Fxns            *ividEncfxns;
 
#ifdef XDM_BASE_CLASS_ONLY
  IVIDENC1_Status          status;
#else
IMP4VENC_Status				status;
#endif

  IVIDENC1_InArgs          inArgs;
  IVIDENC1_OutArgs         outArgs;
  
  IMP4VENC_OutArgs      outArgs_ext;
  /* Algorithm specific handle */
  IALG_Handle handle;

  /* Input/Output Buffer Descriptor variables */
  IVIDEO1_BufDescIn inputBufDesc; //xdm1.0
  XDM_BufDesc outputBufDesc;

  /* DMAN3 specific variables */
  XDAS_Int32  groupId, numAlgs;
  IALG_Handle handleArray[NUM_ALGS];
  IDMA3_Fxns* dmaFxns[NUM_ALGS];

  /* Output Buffer Management specific variables */
  XDAS_UInt32 bytesGenerated;

  /* Other variables */
  XDAS_UInt32  frameCount, countConfigSet;
  XDAS_Int32   retVal, testVal;

   
   XDAS_Int32  RetVal=0;
#ifdef DAVINCI_SPECIFIC    
    unsigned int  chan;
    unsigned int physical_chan[NUM_QDMA_CHANNELS];
	int i;
#endif

#ifdef DAVINCI_SPECIFIC    
     unsigned int *QDMAQNUM = (unsigned int *)(QDMA_BASE   + 0x0260);
     unsigned int *QUETCMAP = (unsigned int *)(QDMA_BASE   + 0x0280);
     unsigned int *QUEPRI   = (unsigned int *)(QDMA_BASE   + 0x0284);
#endif


  /* Enable Cache Settings       */
  TestApp_EnableCache();

#ifdef WRITE_MV_DATA
	mvfd = fopen( "..\\..\\Test\\TestVecs\\Output\\mvdata.txt","wt");
#endif

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
      break ;
    }
    /* Read Parameters file name */
    if(fgets(line,254,fConfigFile))
    {
      sscanf(line,"%s",paramsFile);
    }
    else
    {
      break ;
    }
    /* Read Input file name */
    if(fgets(line,254,fConfigFile))
    {
      sscanf(line,"%s",inFile);
    }
    else
    {
      break ;
    }

    /* Read Output/Reference file name */
    if(fgets(line,254,fConfigFile))
    {
      sscanf(line,"%s",testFile);
    }
    else
    {
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
      printf("Couldn't open Test File... %s",ftestFile);
      printf("\n Exiting for this configuration..." );
	  continue;
    }

    /* Initialization of parameters needed for Algorithm Instance create */
    TestApp_SetInitParams(&params);
    //TestApp_SetInitExtParams(&params);

    if (params.inputChromaFormat == 1)
	    inputBufDesc.framePitch = params.maxWidth;
    else
	    inputBufDesc.framePitch = params.maxWidth << 1;

#ifdef C6000
    /* Initializing DMAN3 parameters */
    /* Base Index Value from where DMA params are written */
    DMAN3_PARAMS.paRamBaseIndex  = 78; 
    /* Number of QDMA channels used by the Algorithm */
    DMAN3_PARAMS.numQdmaChannels = 6;  
	DMAN3_PARAMS.qdmaChannels    = DMAN3_QDMA_CHANNELS_MP4 ;
    /* Number of Algorithms: System Parameter */
    numAlgs                      = NUM_ALGS;  
    /* Group Id needed by DMAN3 library: System Parameter */
	  DMAN3_PARAMS.numPaRamEntries = 48;
    DMAN3_PARAMS.maxQdmaChannels = 6;
    groupId                      = 0;  
#endif  //C6000

#ifdef DAVINCI_SPECIFIC    

    for (i = 0; i < NUM_QDMA_CHANNELS; i++)
    {
        chan      = physical_chan[i];
        *QDMAQNUM = ((*QDMAQNUM) | (0x1 << (4 * chan)));
    }
    
    /*--------------------------------------------------------*/
    /* Event queue to TC channel mapping, Event queue 0 maps  */
    /* to TC channel 0. Event Queue 1 maps to TC channel 1.   */
    /*--------------------------------------------------------*/
    
    *QUETCMAP = (0x1 << 4) | (0x0);
    
    /*--------------------------------------------------------*/
    /* Make QDMA lower priority than other EDMA traffic by    */
    /* setting priority of queue 1 to be 7 and that of        */
    /* queue 0 to be 0x3.                                     */
    /*--------------------------------------------------------*/
    /* KC : SHould ntoe that '3' is an arbitrary decision     */
    /* that should be friendly  to VPSS and other critical    */
    /* DaVinci traffic.                                       */
    /*--------------------------------------------------------*/
    
    *QUEPRI   = (0x7 << 4) | (0x3);
#endif         

#ifdef XDM_BASE_CLASS_ONLY
    /* Setting the sizes of Base Class Objects */
    params.size                  = sizeof(IVIDENC1_Params);
    status.size                  = sizeof(IVIDENC1_Status);
    dynamicParams.size           = sizeof(IVIDENC1_DynamicParams);
    inArgs.size                  = sizeof(IVIDENC1_InArgs);
    outArgs.size                 = sizeof(IVIDENC1_OutArgs);
#else
    params.size                  = sizeof(IMP4VENC_Params);
    status.videncStatus.size     = sizeof(IMP4VENC_Status);
    dynamicParams.size           = sizeof(IMP4VENC_DynamicParams);
    inArgs.size                  = sizeof(IMP4VENC_InArgs);
    outArgs.size                 = sizeof(IMP4VENC_OutArgs);
#endif
    /* Initializing Variables */
    frameCount                   = 0; /* Tracks the number of frames decoded */

    /* Create the Algorithm object (instance) */
    printf("\nCreating Algorithm Instance...");

#ifdef XDM_BASE_CLASS_ONLY
    if ((handle =  (IALG_Handle)ALG_create (
      (IALG_Fxns *) &MP4VENC_TI_IMP4VENC, 
      (IALG_Handle) NULL, 
      (IALG_Params *) &params)) == NULL)
    {
      printf("\nFailed to Create Instance... Exiting for this configuration..");
	    continue;
    }
#else
    extn_params.numFrames = framesToEncode;
    extn_params.videncParams =params; 
    
	if(headerFlag)
	{
		dynamicParams.generateHeader  = XDM_GENERATE_HEADER; 
	}
    
	extn_params.videncParams = params;
    if ((handle =  (IALG_Handle)ALG_create (
      (IALG_Fxns *) &MP4VENC_TI_IMP4VENC, 
      (IALG_Handle) NULL, 
      (IALG_Params *) &extn_params)) == NULL)
    {
      printf("\nFailed to Create Instance... Exiting for this configuration..");
	    continue;
    }
#endif
   //     CACHE_wbInvL2(outputBufDesc.bufs[ii],
     //     outputBufDesc.bufSizes[ii],
     //     CACHE_WAIT);

    /* Assigning Algorithm handle fxns field to ividdecfxns */
    ividEncfxns = (IVIDENC1_Fxns *)handle->fxns ;

    printf("\nAlgorithm Instance Creation Done...\n");

#ifdef C6000
    /* DMA allocation using DMAN3 API's */
    DMAN3_init();

    /* Provide the Algorithm handle to DMAN3 Interface for granting channels */
    handleArray[0] = (IALG_Handle) handle;
    dmaFxns[0]     = &MP4VENC_TI_IDMA3;

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
//    inputBufDesc.bufs = (XDAS_Int8 **)pInputBuf; //XDM1.0
    outputBufDesc.bufs = (XDAS_Int8 **)pOutputBuf;
//    inputBufDesc.bufSizes = (XDAS_Int32 *)inBufSize ;
    outputBufDesc.bufSizes = (XDAS_Int32 *)outBufSize ;

  // Get the version number //XDM1.0
#ifdef XDM_BASE_CLASS_ONLY
      /* Optional: Set Run time parameters in the Algorithm via control()     */
      RetVal = ividEncfxns->control((IVIDENC1_Handle)handle, XDM_GETVERSION, 
        (IVIDENC1_DynamicParams *)&dynamicParams, (IVIDENC1_Status *)&status);
#else
      /* Optional: Set Run time parameters in the Algorithm via control()     */
      extn_dynamicParams.videncDynamicParams = dynamicParams;
      RetVal = ividEncfxns->control((IVIDENC1_Handle)handle, XDM_GETVERSION, 
        (IVIDENC1_DynamicParams *)&extn_dynamicParams, (IVIDENC1_Status *)&status);
#endif
      if(RetVal == XDM_EFAIL)
      {
        printf("\n Control function returned an Error...  ");
        break; /* Error Condition: Application may want to break off          */
      }


    /* Assigning Algorithm handle fxns field to ividEncfxns                   */
    ividEncfxns = (IVIDENC1_Fxns *)handle->fxns;

    /* Resetting bytesGenerated variable                                      */
    bytesGenerated = 0;

    /* Do-While Loop for Encode Call                                          */
    do
    {
      /* Activate the Algorithm                                               */
      handle->fxns->algActivate(handle);

#ifdef XDM_BASE_CLASS_ONLY
      /* Optional: Set Run time parameters in the Algorithm via control()     */
	  inputBufDesc.frameWidth = dynamicParams.inputWidth;
	  inputBufDesc.frameHeight = dynamicParams.inputHeight;
      RetVal = ividEncfxns->control((IVIDENC1_Handle)handle, XDM_SETPARAMS, 
        (IVIDENC1_DynamicParams *)&dynamicParams, (IVIDENC1_Status *)&status);


#else
      /* Optional: Set Run time parameters in the Algorithm via control()     */
      extn_dynamicParams.videncDynamicParams = dynamicParams;
	  inputBufDesc.frameWidth = extn_dynamicParams.videncDynamicParams.inputWidth;
	  inputBufDesc.frameHeight= extn_dynamicParams.videncDynamicParams.inputHeight;

      RetVal = ividEncfxns->control((IVIDENC1_Handle)handle, XDM_SETPARAMS, 
        (IVIDENC1_DynamicParams *)&extn_dynamicParams, (IVIDENC1_Status *)&status);

#endif

      if(RetVal == XDM_EFAIL)
      {
        printf("\n Control function returned an Error...  ");
        break; /* Error Condition: Application may want to break off          */
      }

#ifdef XDM_BASE_CLASS_ONLY
    /* Get Buffer information    */                                             
    RetVal = ividEncfxns->control((IVIDENC1_Handle)handle, 
                         XDM_GETBUFINFO, 
                         (IVIDENC1_DynamicParams *)&dynamicParams, 
                         (IVIDENC1_Status *)&status);
#else
   // extn_dynamicParams.videncDynamicParams = dynamicParams;   
    RetVal= ividEncfxns->control((IVIDENC1_Handle)handle, 
                         XDM_GETBUFINFO, 
                         (IVIDENC1_DynamicParams *)&extn_dynamicParams, 
                         (IVIDENC1_Status *)&status);

#endif
      if(RetVal == XDM_EFAIL)
      {
        printf("\n Control function returned an Error...  ");
        break; /* Error Condition: Application may want to break off          */
      }

    handle->fxns->algDeactivate(handle);

    /*Fill up the buffers as required by algorithm                            */

//xdm1.0    
#ifdef XDM_BASE_CLASS_ONLY
	inputBufDesc.numBufs  = status.bufInfo.minNumInBufs ;
    inputBufDesc.bufDesc[0].buf     = inputData;
    inputBufDesc.bufDesc[0].bufSize = status.bufInfo.minInBufSize[0];

    for(ii=0; ii< (status.bufInfo.minNumInBufs - 1);ii++ )
    {
   //xdm1.0
	  inputBufDesc.bufDesc[ii+1].buf = inputBufDesc.bufDesc[ii].buf + 
        status.bufInfo.minInBufSize[ii];
      inputBufDesc.bufDesc[ii +1].bufSize = 
        status.bufInfo.minInBufSize[ii +1];
  }
    
    outputBufDesc.numBufs     = status.bufInfo.minNumOutBufs ;
    outputBufDesc.bufs[0]     = outputData;
    outputBufDesc.bufSizes[0] = status.bufInfo.minOutBufSize[0];
    for(ii=0; ii< (status.bufInfo.minNumOutBufs-1); ii++ )
    {
      outputBufDesc.bufs[ii+1] = outputBufDesc.bufs[ii] + 
      status.bufInfo.minOutBufSize[ii];
      outputBufDesc.bufSizes[ii+1] = 
        status.bufInfo.minOutBufSize[ii+1];
    }
#else
	inputBufDesc.numBufs  = status.videncStatus.bufInfo.minNumInBufs ;
    inputBufDesc.bufDesc[0].buf     = inputData;
    inputBufDesc.bufDesc[0].bufSize = status.videncStatus.bufInfo.minInBufSize[0];

    for(ii=0; ii< (status.videncStatus.bufInfo.minNumInBufs - 1);ii++ )
    {
   //xdm1.0
	  inputBufDesc.bufDesc[ii+1].buf = inputBufDesc.bufDesc[ii].buf + 
        status.videncStatus.bufInfo.minInBufSize[ii];
      inputBufDesc.bufDesc[ii +1].bufSize = 
        status.videncStatus.bufInfo.minInBufSize[ii +1];
  }
    
    outputBufDesc.numBufs     = status.videncStatus.bufInfo.minNumOutBufs ;
    outputBufDesc.bufs[0]     = outputData;
    outputBufDesc.bufSizes[0] = status.videncStatus.bufInfo.minOutBufSize[0];
    for(ii=0; ii< (status.videncStatus.bufInfo.minNumOutBufs-1); ii++ )
    {
      outputBufDesc.bufs[ii+1] = outputBufDesc.bufs[ii] + 
      status.videncStatus.bufInfo.minOutBufSize[ii];
      outputBufDesc.bufSizes[ii+1] = 
        status.videncStatus.bufInfo.minOutBufSize[ii+1];
    }
#endif
      /* Read from Input File                                                 */
      TestApp_ReadInputData(finFile, 
        &inputBufDesc);

#ifdef C6000
    //  for(ii=0 ; ii < inputBufDesc.numBufs ; ii++ )
    //  {
    //    /* Cache Invalidate for Input Buffer */
     //   BCACHE_inv(inputBufDesc.bufDesc[ii].buf,
//			inputBufDesc.bufDesc[ii].bufSize,
//			TRUE);
     // }
#endif  //C6000
    BCACHE_wbInvAll(); /* No need to call BCACHE_wait() */

      handle->fxns->algActivate(handle);
      outArgs_ext.videncOutArgs = outArgs; 
      /* Basic Algorithm process() call */
      retVal = ividEncfxns->process((IVIDENC1_Handle)handle, 
        (IVIDEO1_BufDescIn *)&inputBufDesc,
        (XDM_BufDesc *)&outputBufDesc,
        (IVIDENC1_InArgs *)&inArgs,
        (IVIDENC1_OutArgs *)&outArgs_ext);
      
      if(retVal == XDM_EFAIL)
      {
        printf("\n Process function returned an Error...  ");
        break; /* Error Condition: Application may want to break off          */
      }
     // outArgs = outArgs_ext.videncOutArgs;
#ifdef XDM_BASE_CLASS_ONLY  
	  /* Optional: Read status via control()                                  */
      ividEncfxns->control((IVIDENC1_Handle)handle, 
        XDM_GETSTATUS, 
        (IVIDENC1_DynamicParams *)&dynamicParams, 
        (IVIDENC1_Status *)&status);
#else
	  /* Optional: Read status via control()                                  */
      ividEncfxns->control((IVIDENC1_Handle)handle, 
        XDM_GETSTATUS, 
        (IVIDENC1_DynamicParams *)&extn_dynamicParams, 
        (IVIDENC1_Status *)&status);
#endif
      /* DeActivate the Algorithm                                             */
      handle->fxns->algDeactivate(handle);

#ifdef C6000
      /* Cache Writeback Invalidate for Output Buffers                        */
      //for(ii=0 ; ii < outputBufDesc.numBufs ; ii++ )
      //{
      //  BCACHE_wbInv(outputBufDesc.bufs[ii],
      //    outputBufDesc.bufSizes[ii],
      //    TRUE);
     // }
#endif //C6000
    BCACHE_wbInvAll(); /* No need to call BCACHE_wait() */

      /* Check for frame ready via recon buffer information                   */
      if(outArgs_ext.videncOutArgs.reconBufs.numBufs)
      {
          printf("\n Encoded Frame # %d  ",frameCount);
		  fflush(stdout);

          if(testCompliance)
          {
            /* Compare the output frames with the Reference File */
            testVal = TestApp_CompareOutputData(ftestFile, 
                                                (XDM_BufDesc *)&outputBufDesc, 
                                                &outArgs_ext.videncOutArgs);
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
            TestApp_WriteOutputData(ftestFile, &outputBufDesc, &outArgs_ext.videncOutArgs);
          }
#ifdef WRITE_MV_DATA
            if(extn_dynamicParams.MVDataEnable == 1)
			{
                mbMV_data =  (motion_mbdata  *)outputBufDesc.bufs[1];			    
				for(jj=0;jj< (status.mvDataSize >> 3);jj++)
				{
					fprintf(mvfd,"MB Number: %d\n",jj);
					fprintf(mvfd,"MVx: %d\n",mbMV_data->MVx);
					fprintf(mvfd,"MVy: %d\n",mbMV_data->MVy);
					fprintf(mvfd,"SSE: %d\n",mbMV_data->SSE);
					mbMV_data++;

				}
				fprintf(mvfd,"*****Frame : %d done******\n",frameCount);
				
			}

#endif
          frameCount++;
      }
      bytesGenerated += outArgs_ext.videncOutArgs.bytesGenerated ;

      /* Check for Break off condition */
      if (frameCount >= framesToEncode)
      {
        printf("\n Specified number of Frames Encoded...  ");
        break; 
      }

    }  while(1);  /* end of Do-While loop */
  
    if(testVal == XDM_EOK && testCompliance )
    {
      printf("\n Encoder compliance test passed for this configuration..  ");
    }

    if(testVal == XDM_EOK && (!testCompliance) )
    {
      printf("\n Encoder output dump completed");
    }
    /* Output file close */
    fclose(ftestFile);
   
#ifdef C6000

    /*
    * Reclaim DMA resources from algorithm and deinitialize the DMA
    * manager
    */
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
    printf("  Bit Rate at 30 frames/Sec       = %d Kbps\n",(
      XDAS_UInt32)(((bytesGenerated*8*30)/frameCount)/1000));
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
//xdm1.0
      bytesRead = fread(inputBufDesc->bufDesc[i].buf, 
                        1, 
                        inputBufDesc->bufDesc[i].bufSize,
                        finFile);


#ifdef C6000
   //   /* Make sure that external memory contain correct copy of input data    */
  //    BCACHE_wbInv(inputBufDesc->bufDesc[i].buf,
   //              inputBufDesc->bufDesc[i].bufSize,
   //              TRUE);

#endif //C6000
    BCACHE_wbInvAll(); /* No need to call BCACHE_wait() */

      if(bytesRead != inputBufDesc->bufDesc[i].bufSize)
      {
        bytesRead = -1 ;
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
    }
    break ;
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
  for(i=0; i < 1 ; i++)
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
#ifdef XDM_BASE_CLASS_ONLY
  params->encodingPreset         = XDM_DEFAULT; /*User defined*/      
  params->rateControlPreset      = IVIDEO_RATECONTROLPRESET_DEFAULT; /*User defined*/
#endif
  params->dataEndianness         = XDM_BYTE           ;       
  params->maxInterFrameInterval  = 1                  ;
  params->inputContentType       = IVIDEO_CONTENTTYPE_DEFAULT ;     
  params->reconChromaFormat       = XDM_CHROMA_NA ;     
  
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
  dynamicParams->forceFrame      = IVIDEO_NA_FRAME                   ;       
  dynamicParams->mbDataFlag      = 0                   ;       
  dynamicParams->interFrameInterval = params.maxInterFrameInterval;       
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

