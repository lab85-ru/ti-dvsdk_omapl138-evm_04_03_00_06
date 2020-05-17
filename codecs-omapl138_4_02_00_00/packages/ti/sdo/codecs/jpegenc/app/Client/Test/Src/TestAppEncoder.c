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
********************************************************************************
*/

/* Standard C header files                                                    */
#include <stdio.h>
#include <stdlib.h>

/* H264 Interface header files                                                */


/* Client header file                                                         */
#include "TestAppEncoder.h"

/* CSL and DMAN3 header files                                                 */
#ifdef C6000
 /* CSL and DMAN3 header files */
#include "bcache.h"

#else
//  #include <memory.h>
#endif

unsigned int   t0, t1, EncodeTime;
double cycles;
/* Input buffer allocation                                                    */
XDAS_Int8  inputData[INPUT_BUFFER_SIZE];
/* Output buffer allocation */
XDAS_Int8  outputData[OUTPUT_BUFFER_SIZE];
/* Reference buffer allocation */
XDAS_Int8  refData[OUTPUT_BUFFER_SIZE];
int total_size = 0;
#ifdef C6000
#pragma    DATA_SECTION(inputData,  ".ccdData")
#pragma    DATA_SECTION(outputData, ".bitsData")
#pragma    DATA_SECTION(refData,    ".ref_buffer")
#endif //C6000

#include <idmjpge.h>
#include <dmjpge_tigem.h>
/* Base Class Structures                                                      */
IIMGENC1_Params          params;
IIMGENC1_DynamicParams   dynamicParams;
IIMGENC1_InArgs        inArgs;
IIMGENC1_OutArgs       outArgs;
IIMGENC1_Status        status;
XDM1_BufDesc                  InBuffers;
XDM1_BufDesc                  OutBuffers;

XDAS_UInt32             ImageToEncode = 50 ;
XDAS_UInt32				TotalBytes;
extern long               timerJPEG;
 Uint32                      Total_BytesRead;
Uint32	captureHeight;
Uint32	DRI_Interval;

/* Mapping of Prameters from config file string to the place holder           */
sTokenMapping sTokenMap[] =
{
  {"MaxImageWidth",     &params.maxWidth          },
  {"MaxImageHeight",    &params.maxHeight         },
  {"Scan",    &params.maxScans         },
  {"IPImageWidth",     &dynamicParams.inputWidth          },
  {"IPImageHeight",    &dynamicParams.inputHeight         },
  {"ForceChromaFormat",    &params.forceChromaFormat         },
  {"inputChromaFormat",    &dynamicParams.inputChromaFormat  },
  {"QP",             &dynamicParams.qValue         },
  {"numMCU",             &dynamicParams.numAU         },
  {"captureWidth",             &dynamicParams.captureWidth         },
  {"captureHeight",             &captureHeight         },
  {"DRI_Interval",             &DRI_Interval         },

  {NULL,              NULL                      }
};
#ifdef  DAVINCI_INTERRUPT

#include "c6x.h"
#include "intc.h"

void Interrupt_start(void);

void Interrupt_start(void)
{
	*((unsigned int *)0x01C21420) = 0x0;
	*((unsigned int *)0x01C21424) = 0x0;
	*((unsigned int *)0x01C21424) = 0x0007;
    *((unsigned int *)0x01C2141C) = 300;//count
    *((unsigned int *)0x01C21414) = 0x0;
	*((unsigned int *)0x01C21404) = 0x1;
	*((unsigned int *)0x01C21420) = 0x800000;
}
#endif


/***************Rajashekara**********************************/
int InputBuffSize[4];
int num_buf,Capture_Height;
Int32 Initialize_original_buffer(IDMJPGE_TIGEM_DynamicParams *params)
{
	int temp;
	if(params->params.size == sizeof(IIMGENC1_DynamicParams))
         Capture_Height = 0;
	else
		Capture_Height = params->captureHeight;

	if (params->params.captureWidth == 0 && Capture_Height==0)
		temp = params->params.inputHeight * params->params.inputWidth;

	else if (params->params.captureWidth != 0 && Capture_Height==0)
		temp = params->params.inputHeight * params->params.captureWidth;

	 else if (params->params.captureWidth == 0 && Capture_Height!=0)
		temp = Capture_Height * params->params.inputWidth;

	else
		temp = Capture_Height * params->params.captureWidth;  //REM



	switch(params->params.inputChromaFormat)
	{
	case XDM_YUV_422ILE:
		{
			num_buf  = 1;
			InputBuffSize[0] = temp<<1 ;
		}; break;

    case XDM_GRAY:
		{
			num_buf  = 1;
			InputBuffSize[0] = temp ;
		}; break;

    case XDM_YUV_444P:
		{
			num_buf  = 3;
			InputBuffSize[0] = temp ;
			InputBuffSize[1] = temp ;
			InputBuffSize[2] = temp ;
		}; break;
    case XDM_YUV_420P:
		{
			num_buf  = 3;
			InputBuffSize[0] = temp ;
			InputBuffSize[1] = temp >> 2 ;
			InputBuffSize[2] = temp >> 2;
		}; break;

    case XDM_YUV_422P:
		{
			num_buf = 3;
			InputBuffSize[0] = temp ;
			InputBuffSize[1] = temp >> 1;
			InputBuffSize[2] = temp >> 1;
		}; break;

    case XDM_YUV_411P:
		{
			num_buf = 3;
			InputBuffSize[0] = temp ;
			InputBuffSize[1] = temp >> 2;
			InputBuffSize[2] = temp >> 2;
		}; break;

    case XDM_DEFAULT:
		{
			num_buf = 3;
			InputBuffSize[0] = temp ;
			InputBuffSize[1] = temp ;
			InputBuffSize[2] = temp ;
		}; break;

	}
}

	/****************Rajashekara************************************/



/* Main Function acting as a client for Video Encode Call                     */
XDAS_Int32 main ()
{
  /* File I/O variables */
  FILE *fConfigFile, *ftestFile, *finFile, *fparamsFile,*fReportFile;
  XDAS_Int8 line[STRING_SIZE],inFile[STRING_SIZE], testFile[STRING_SIZE],
    paramsFile[STRING_SIZE], testCompliance, ii;

#ifdef C6000
  XDAS_Int8 *fname = "..\\..\\Test\\TestVecs\\Config\\testvecs.cfg ";
  XDAS_Int8 *freport = "..\\..\\Test\\TestVecs\\Config\\Report.txt";
#else
  XDAS_Int8 *fname = "..\\Test\\TestVecs\\Config\\testvecs_vc.cfg";
  XDAS_Int8 *freport = "..\\Test\\TestVecs\\Config\\Report.txt";
#endif

  /* Input/Output  buffers  and their sizes                                   */
  XDAS_Int8*  pInputBuf   [XDM_MAX_IO_BUFFERS];
  XDAS_Int8*  pOutputBuf  [XDM_MAX_IO_BUFFERS];
  XDAS_Int32  inBufSize   [XDM_MAX_IO_BUFFERS];
  XDAS_Int32  outBufSize  [XDM_MAX_IO_BUFFERS];


  IIMGENC1_Fxns            *IIMGENC1fxns;

  IIMGENC1_Status          status;
  IIMGENC1_InArgs          inArgs;
  IIMGENC1_OutArgs         outArgs;

  /* Algorithm specific handle                                                */
  IALG_Handle handle;

  /* Input/Output Buffer Descriptor variables */
  XDM1_BufDesc inputBufDesc, outputBufDesc;

  /* DMAN3 specific variables                                                 */
  XDAS_Int32  groupId, numAlgs;
  IALG_Handle handleArray[NUM_ALGS];


  /* Output Buffer Management specific variables                              */
  XDAS_UInt32 bytesGenerated;

  /* Other variables                                                          */
  XDAS_UInt32  ImageCount, countConfigSet;
  XDAS_Int32   retVal, testVal ;

  /* Enable Cache Settings                                                    */
  TestApp_EnableCache();

#ifdef DAVINCI_INTERRUPT
   INTC_init();
#endif
#ifdef C6000
  /* Enables the Time Stamp Counter */
    TSC_enable();
  #endif

   /* Setting the sizes of Base Class Objects */
	params.size                    = sizeof(IIMGENC1_Params);
	params.maxHeight               = 3172;
	params.maxWidth                = 2048;
	params.maxScans                = 3;
	params.dataEndianness          = XDM_BYTE;
	dynamicParams.size              = sizeof(IIMGENC1_DynamicParams);
	dynamicParams.numAU             = 0;
	dynamicParams.generateHeader    = XDM_ENCODE_AU;

	status.size                     = sizeof(IIMGENC1_Status);

	inArgs.size                     = sizeof(IIMGENC1_InArgs);

	outArgs.size                    = sizeof(IIMGENC1_OutArgs);
	outArgs.extendedError           = 0;
	outArgs.bytesGenerated          = 0;
	outArgs.currentAU               = 0;

  /* Open Test Config File                                                    */
  fReportFile = fopen(freport,"w");

  if (!fReportFile)
  {
      printf("Couldn't openReport file %s",freport);
      return XDM_EFAIL;
  }

  /* Open Test Config File                                                    */
  fConfigFile = fopen(fname,"r");

  if (!fConfigFile)
  {
      printf("Couldn't open parameter file %s",fname);
      return XDM_EFAIL;
  }

  countConfigSet = 1; /* Reset countConfigSet value to 1 */

    /* Initializing Variables */
    ImageCount                   = 0; /* Tracks the number of frames decoded */

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
    if(fgets(line,STRING_SIZE,fConfigFile))
    {
       sscanf(line,"%s",paramsFile);

    }
    else
    {
      break ;
    }
    /* Read Input file name */
    if(fgets(line,STRING_SIZE,fConfigFile))
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
    printf("\n Input file...  %s  ",inFile);

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
      printf("\n Couldn't open Test File... %s",testFile);
      printf("\n Exiting for this configuration..." );
	  continue;
    }
    printf("\nTest File... %s",testFile);

    /* Initialization of parameters needed for Algorithm Instance create */
    TestApp_SetInitParams(&params);



    /* Create the Algorithm object (instance) */
    printf("\nCreating Algorithm Instance...");


    if ((handle =  (IALG_Handle)ALG_create (
      (IALG_Fxns *) &DMJPGE_TIGEM_IDMJPGE,
      (IALG_Handle) NULL,
      (IALG_Params *) &params)) == NULL)
    {
      printf("\nFailed to Create Instance... Exiting for this configuration..");
	    continue;
    }

#ifdef C6000
     BCACHE_wbInvAll(); /* No need to call BCACHE_wait() */
#endif

    /* Assigning Algorithm handle fxns field to IIMGdecfxns */
    IIMGENC1fxns = (IIMGENC1_Fxns *)handle->fxns ;

    printf("\nAlgorithm Instance Creation Done...\n");

    /* Set run time dynamic parameters */
    TestApp_SetDynamicParams(&dynamicParams);

    /* set space for buffer descriptors                                       */
    /*inputBufDesc.bufs = (XDAS_Int8 **)pInputBuf;
    outputBufDesc.bufs = (XDAS_Int8 **)pOutputBuf;
    inputBufDesc.bufSizes = (XDAS_Int32 *)inBufSize;
    outputBufDesc.bufSizes = (XDAS_Int32 *)outBufSize;*/

    /* Activate the Algorithm                                                 */
    handle->fxns->algActivate(handle);
     /* Optional: Set Run time parameters in the Algorithm via control()     */
    IIMGENC1fxns->control((IIMGENC1_Handle)handle, XDM_SETPARAMS,
        (IIMGENC1_DynamicParams *)&dynamicParams, (IIMGENC1_Status *)&status);

    /* Get Buffer information                                                 */
    IIMGENC1fxns->control((IIMGENC1_Handle)handle,
                         XDM_GETBUFINFO,
                         (IIMGENC1_DynamicParams *)&dynamicParams,
                         (IIMGENC1_Status *)&status);

Initialize_original_buffer((IDMJPGE_TIGEM_DynamicParams*) &dynamicParams)	;

    /* DeActivate the Algorithm                                               */
    handle->fxns->algDeactivate(handle);

    /*Fill up the buffers as required by algorithm                            */
    inputBufDesc.numBufs  = status.bufInfo.minNumInBufs ;
    inputBufDesc.descs[0].buf     = inputData;
    inputBufDesc.descs[0].bufSize = status.bufInfo.minInBufSize[0];

    for(ii=0; ii< (status.bufInfo.minNumInBufs - 1);ii++ )
    {
      inputBufDesc.descs[ii+1].buf = inputBufDesc.descs[ii].buf +
        status.bufInfo.minInBufSize[ii];
      inputBufDesc.descs[ii +1].bufSize =
        status.bufInfo.minInBufSize[ii +1];
    }

    outputBufDesc.numBufs     = status.bufInfo.minNumOutBufs ;
    outputBufDesc.descs[0].buf     = outputData;
    outputBufDesc.descs[0].bufSize = status.bufInfo.minOutBufSize[0];
	if(outputBufDesc.descs[0].bufSize > OUTPUT_BUFFER_SIZE)
	{
		outputBufDesc.descs[0].bufSize = OUTPUT_BUFFER_SIZE;
	}

    for(ii=0; ii< (status.bufInfo.minNumOutBufs-1); ii++ )
    {
		if(outputBufDesc.descs[0].bufSize > OUTPUT_BUFFER_SIZE)
		{
			exit(0);
		}

     outputBufDesc.descs[ii+1].buf = outputBufDesc.descs[ii].buf +
      status.bufInfo.minOutBufSize[ii];
      outputBufDesc.descs[ii+1].bufSize =
        status.bufInfo.minOutBufSize[ii+1];
    }

    /* Assigning Algorithm handle fxns field to IIMGENC1fxns                   */
    IIMGENC1fxns = (IIMGENC1_Fxns *)handle->fxns;

    /* Resetting bytesGenerated variable                                      */
    bytesGenerated = 0;

	/* Read from Input File                                                 */
      TestApp_ReadInputData(finFile,
        &inputBufDesc);


#ifdef C6000
    /* Cache Invalidate for Input Buffer                                   */
      for(ii=0 ; ii < inputBufDesc.numBufs ; ii++ )
      {
        /* Cache Invalidate for Input Buffer                                 */
		BCACHE_inv(inputBufDesc.descs[ii].buf, inputBufDesc.descs[ii].bufSize, TRUE);

      }
#endif  //C6000

#ifdef DAVINCI_INTERRUPT
    INTC_enable(INT_EVT_TMR1);
	INTC_attachIsr(INT_EVT_TMR1,NULL);
    Interrupt_start();
#endif

#ifdef C6000
      /* Timer value before start of decoding */
      t0  = TSC_read();
 #endif  //C6000
      /* Activate the Algorithm                                               */
      handle->fxns->algActivate(handle);
     /* Optional: Set Run time parameters in the Algorithm via control()     */

      IIMGENC1fxns->control((IIMGENC1_Handle)handle, XDM_SETPARAMS,
        (IIMGENC1_DynamicParams *)&dynamicParams, (IIMGENC1_Status *)&status);

#ifdef C6000
     BCACHE_wbInvAll(); /* No need to call BCACHE_wait() */
#endif



 	  TotalBytes = 0;
       /* Basic Algorithm process() call */
      retVal = IIMGENC1fxns->process((IIMGENC1_Handle)handle,
        (XDM1_BufDesc *)&inputBufDesc,
        (XDM1_BufDesc *)&outputBufDesc,
        (IIMGENC1_InArgs *)&inArgs,
        (IIMGENC1_OutArgs *)&outArgs);

	  TotalBytes +=outArgs.bytesGenerated;
      if(retVal == XDM_EFAIL)
      {
	        printf("\n Process function returned an Error...  ");
			continue;
	  }

      /* Optional: Set Run time parameters in the Algorithm via control()     */
      IIMGENC1fxns->control((IIMGENC1_Handle)handle, XDM_GETSTATUS,
        (IIMGENC1_DynamicParams *)&dynamicParams, (IIMGENC1_Status *)&status);
		if((!testCompliance) && (dynamicParams.numAU))
	TestApp_WriteOutputData(ftestFile, &outputBufDesc, &outArgs);
      while (outArgs.currentAU < status.totalAU)
      {

	      /* Basic Algorithm process() call */
	      retVal = IIMGENC1fxns->process((IIMGENC1_Handle)handle,
	        (XDM1_BufDesc *)&inputBufDesc,
	        (XDM1_BufDesc *)&outputBufDesc,
	        (IIMGENC1_InArgs *)&inArgs,
	        (IIMGENC1_OutArgs *)&outArgs);
	      TotalBytes +=outArgs.bytesGenerated;
	      if(retVal == XDM_EFAIL)
	      {
	        printf("\n Process function returned an Error...  ");
	        break; /* Error Condition: Application may want to break off          */
	      }
		 if((!testCompliance) && (dynamicParams.numAU))
		 TestApp_WriteOutputData(ftestFile, &outputBufDesc, &outArgs);
	  }
      /* Optional: Read status via control()                                  */
      IIMGENC1fxns->control((IIMGENC1_Handle)handle,
        XDM_GETSTATUS,
        (IIMGENC1_DynamicParams *)&dynamicParams,
        (IIMGENC1_Status *)&status);


    /* DeActivate the Algorithm                                             */
    handle->fxns->algDeactivate(handle);
#ifdef C6000
	/* Timer value before start of decoding */
    t1  = TSC_read();
#endif

	EncodeTime =  t1-t0;

#ifdef DAVINCI_INTERRUPT
    INTC_disable(INT_EVT_TMR1);
#endif

#ifdef C6000
      /* Cache Writeback Invalidate for Output Buffers                        */
      for(ii=0 ; ii < outputBufDesc.numBufs ; ii++ )
      {
	   BCACHE_wbInv(outputBufDesc.descs[ii].buf,
          outputBufDesc.descs[ii].bufSize, TRUE);
      }
#endif //C6000

      /* Check for frame ready via recon buffer information                   */
     // if(outArgs.reconBufs.numBufs)
      {
          fflush(stdout);


          if((testCompliance) && (!dynamicParams.numAU))
          {
            /* Compare the output frames with the Reference File */
            testVal = TestApp_CompareOutputData(ftestFile,
                                                (XDM1_BufDesc *)&outputBufDesc,
                                                &outArgs);
            if(testVal != XDM_EOK)
            {
              /* Test Compliance Failed... Breaking...*/
              printf( "\n Encoder compliance test failed for this Image. ");
			  break;

            }
          }
          else
          {
            /* Write the output frames in the display order */
            TestApp_WriteOutputData(ftestFile, &outputBufDesc, &outArgs);
          }
          ImageCount++;
      }
      bytesGenerated += outArgs.bytesGenerated ;
	  		fclose(ftestFile);

      /* Check for Break off condition */
      if (ImageCount >= ImageToEncode)
      {
        printf("\n Specified number of Images Encoded...  ");
      	break;
      }


    if(testVal == XDM_EOK && testCompliance )
    {
      printf("\n Encoder compliance test passed \n");
    }

    if(testVal == XDM_EOK && (!testCompliance) )
    {
      printf("\n Encoder output dump completed \n");
    }
    /* Output file close */
    fclose(ftestFile);


    /* Delete the Algorithm instance object specified by handle */
    ALG_delete (handle);

    if(ImageCount == 0)
    {
      ImageCount = 1; /* To avoid division with zero */
    }
    if(!testCompliance)
    {
      printf(" Encoder output dump completed\n");
    }
  	cycles = (double)EncodeTime / ((double)(dynamicParams.inputHeight * dynamicParams.inputWidth ) ) ;
	fprintf(fReportFile,"  EncodeTime	\t 		= %d \n",
      EncodeTime);


    printf("   Image Encoded          = %d\n",
      (XDAS_UInt32)ImageCount);
    printf("  Width and Height       = %d, %d \n",
      dynamicParams.inputWidth, dynamicParams.inputHeight);
    printf("  QP	\t 		= %d  \n",
      dynamicParams.qValue);
    printf("  Ratio	\t 		= %f   \n",
      (double)Total_BytesRead / (double)outArgs.bytesGenerated);

	printf("\t MegaPixel/sec 330 :  %f \t Cycles Taken :  %ld \t Cycles per pixel : %f \t  Total_BytesRead : %d \t TotalBytes: %d \t Ratio : %f\n",
                    (double)330 / cycles,
                    EncodeTime,
					cycles,
                      Total_BytesRead,
                    TotalBytes,
                    (double)Total_BytesRead / (double)outArgs.bytesGenerated);
	printf("\t MegaPixel/sec 450 :  %f \t Cycles Taken :  %12ld \t Cycles per pixel : %f \t  Total_BytesRead : %d \t TotalBytes: %d \t Ratio : %f\n",
                    (double)450 / cycles,
                    EncodeTime,
					cycles,
                      Total_BytesRead,
                    TotalBytes,
                    (double)Total_BytesRead / (double)outArgs.bytesGenerated);

	printf("\t MegaPixel/sec 594 :  %f \t Cycles Taken :  %12ld \t Cycles per pixel : %f \t  Total_BytesRead : %d \t TotalBytes: %d \t Ratio : %f\n",
					(double)594 / cycles,
					EncodeTime,
					cycles,
					  Total_BytesRead,
					TotalBytes,
					(double)Total_BytesRead / (double)outArgs.bytesGenerated);

    fprintf(fReportFile," \t \t -------------- %d No of Image -----------\n",
      (XDAS_UInt32)ImageCount);

    fprintf(fReportFile," I/P Image Encoded			= %s\n",
      inFile);
    fprintf(fReportFile," O/P or reff Image Encoded \t \t	= %s\n",
      testFile);
    fprintf(fReportFile,"Parameter File				= %s\n",
      paramsFile);

    fprintf(fReportFile," inputChromaFormat	\t 		= %d\n",
      (XDAS_UInt32)dynamicParams.inputChromaFormat);
    fprintf(fReportFile," forceChromaFormat	\t 		= %d\n",
      (XDAS_UInt32)params.forceChromaFormat);

    fprintf(fReportFile,"  Width and Height	\t 		= %d, %d \n",
      dynamicParams.inputWidth, dynamicParams.inputHeight);
	fprintf(fReportFile,"MegaPixel/sec :  %f \t  Cycles per pixel : %f \t  Total_BytesRead : %d \t TotalBytes: %d \t Ratio : %f \t Cycles Taken :  %12ld \n",
                    (double)330 / cycles,
                    cycles,
                    Total_BytesRead,
                    TotalBytes,
                    (double)Total_BytesRead / (double)outArgs.bytesGenerated,
                    EncodeTime);
	fprintf(fReportFile,"MegaPixel/sec :  %f \t  Cycles per pixel : %f \t  Total_BytesRead : %d \t TotalBytes: %d \t Ratio : %f \t Cycles Taken :  %f \n",
                    (double)450 / cycles,
                    cycles,
                    Total_BytesRead,
                    TotalBytes,
                    (double)Total_BytesRead / (double)outArgs.bytesGenerated,
                    EncodeTime);
	fprintf(fReportFile,"MegaPixel/sec :  %f \t  Cycles per pixel : %f \t  Total_BytesRead : %d \t TotalBytes: %d \t Ratio : %f \t Cycles Taken :  %12ld \n",
                    (double)594 / cycles,
                    cycles,
                    Total_BytesRead,
                    TotalBytes,
                    (double)Total_BytesRead / (double)outArgs.bytesGenerated,
                    EncodeTime);

    fprintf(fReportFile,"\n\n");
	fflush(fReportFile);
	fflush(stdout);

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
                                  XDM1_BufDesc* inputBufDesc)
{
    XDAS_Int32 i, bytesRead ;
    Total_BytesRead = 0;
    for(i=0; i < num_buf ; i++)
    {
      /*Read the input buffers from FILE                                      */
      bytesRead = fread(inputBufDesc->descs[i].buf,
                        1,
                        InputBuffSize[i],
                        finFile);
	 Total_BytesRead += bytesRead;
	 if (Total_BytesRead > INPUT_BUFFER_SIZE)
    {
      printf( "\nWarning : File size exceeds the application input buffer size %d ",
        INPUT_BUFFER_SIZE);
      printf( "\nContinuing decoding for %d bytes.\n", INPUT_BUFFER_SIZE);
    }

#ifdef C6000
      /* Make sure that external memory contain correct copy of input data    */

       /* Cache Invalidate for Input Buffer */
    BCACHE_wbInv(inputBufDesc->descs[i].buf, inputBufDesc->descs[i].bufSize, TRUE);


#endif //C6000

      if(bytesRead != InputBuffSize[i])
      {
        bytesRead = -1 ;
		printf("Input Buffer Size is less than the Image Size");//REM
//		exit(0);
        break ;
      }
    }
	fclose(finFile);
    return (bytesRead);
}

/*
//============================================================================
// TestApp_CompareOutputData
//  Comparing Output Data with Reference File data
*/

XDAS_Int32 TestApp_CompareOutputData(FILE *fRefFile,
                                     XDM1_BufDesc * outputBufDesc,
                                     IIMGENC1_OutArgs *outArgs)
{
  XDAS_Int32 i, retVal ;

  retVal = XDM_EOK;

  /* Compare all the output Buffers with the ref File                         */
  for(i=0; i < outputBufDesc->numBufs ; i++)
  {
    fread(refData, 1, outArgs->bytesGenerated, fRefFile);
    if(memcmp(refData, outputBufDesc->descs[i].buf, outArgs->bytesGenerated))
    {
      retVal = XDM_EFAIL;
    }
    break ;
  }
  fclose(fRefFile);
  return retVal;
}

/*
//============================================================================
// TestApp_WriteOutputData
//  Writing Output Data in a File
*/


XDAS_Void TestApp_WriteOutputData(FILE *fOutFile,
                                  XDM1_BufDesc * outputBufDesc,
                                  IIMGENC1_OutArgs *outArgs)
{

  XDAS_Int32 i = 0;

  if(TotalBytes > OUTPUT_BUFFER_SIZE)
  {
    printf( "\nWarning : Output size exceeds output  buffer size %d OUTPUT_BUFFER_SIZE %d",
        TotalBytes,OUTPUT_BUFFER_SIZE);

  }
  /****************************************************************************/
  /* Write the bitstream                                                      */
  /****************************************************************************/
  for(i=0; i < outputBufDesc->numBufs ; i++)
  {
    if(outArgs->bytesGenerated != 0 )
    {
      fwrite( outputBufDesc->descs[i].buf,
        1,
        TotalBytes,
        fOutFile);
			TotalBytes=0;
    }
  }

}

/*
//============================================================================
// TestApp_SetInitParams
//  setting of creation time parameters
*/

XDAS_Void TestApp_SetInitParams(IIMGENC1_Params *params)
{

  return;
}

/*
//============================================================================
// TestApp_SetDynamicParams
//  setting of run time parameters
*/

XDAS_Void TestApp_SetDynamicParams(IIMGENC1_DynamicParams *dynamicParams)
{

  dynamicParams->generateHeader  = XDM_ENCODE_AU       ;

  return;
}


  /* Cache Settings */
XDAS_Void TestApp_EnableCache(void)
{
#ifdef C6000

    BCACHE_Size size;

	size.l1psize  = BCACHE_L1_32K; /* L1P cache size */
	size.l1dsize  = BCACHE_L1_32K; /* L1D cache size */
	size.l2size   = BCACHE_L2_64K; /* L2  cache size */
 	size.l2size   = BCACHE_L2_128K; /* L2  cache size */

	/* Set L1P, L1D and L2 cache sizes */
    BCACHE_setSize(&size);

    /* Cache Enable External Memory Space */
	/* BaseAddr, length, MAR enable/disable */
    /* Cache 0x80000000 --- 0x8FFFFFFF   */
    BCACHE_setMar((Ptr *)0x80000000, 0x10000000, BCACHE_MAR_ENABLE);


   /* Cache Enable External Memory Space */



#ifdef C6000
     BCACHE_wbInvAll(); /* No need to call BCACHE_wait() */
#endif

#endif  //C6000

} /* DoCacheSettings */

/******************************************************************************/
/*    Copyright (c) 2006 Texas Instruments, Incorporated                      */
/*    All Rights Reserved                                                     */
/******************************************************************************/
