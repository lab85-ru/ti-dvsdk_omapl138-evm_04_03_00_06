/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/*
********************************************************************************
@file     TestAppDecoder.c
@brief    This is the top level client file that drives the MPEG2
(Main Profile) Video Decoder Call using XDM Interface
@author   Multimedia Codecs TI India
@version  0.0 - Jan 24,2006    initial version
********************************************************************************
*/

/* Standard C header files */
#include <stdio.h>
#include <stdlib.h>
#include <ti/bios/include/std.h>

/* MPEG2 Interface header files */
#include "mpeg2vdec_ti.h"
#include "mpeg2vdec.h"

/*---------------------------------------------------------------------------*/
/*  HDVICP and buffer management operations' header files.                   */
/*---------------------------------------------------------------------------*/
#include "buffermanager.h"

/* CSL and DMAN3 header files */
#include <ti/bios/include/bcache.h>
#include "dman3.h"
#include "bios_rts.h"

/* Client header file */
#include "TestAppDecoder.h"

/* Input buffer allocation */
#pragma    DATA_SECTION(inputData, ".input_buffer")
XDAS_Int8 inputData[INPUT_BUFFER_SIZE];

/* Output buffer allocation */
#pragma    DATA_SECTION(outputData, ".display_buffer")
XDAS_Int8  outputData[2][OUTPUT_BUFFER_SIZE];

/* Output buffer allocation */
#pragma    DATA_SECTION(refData, ".ref_buffer")
XDAS_Int8  refData[OUTPUT_BUFFER_SIZE];

IVIDDEC2_Params             params;
IVIDDEC2_DynamicParams      dynamicParams;
XDAS_UInt32                framesToDecode;

sTokenMapping sTokenMap[] =
{
  {"FramesToDecode", &framesToDecode},
  {"ImageHeight",    &params.maxHeight},
  {"ImageWidth",     &params.maxWidth},
  {"ChromaFormat",   &params.forceChromaFormat},
  {NULL,             NULL}
};
/* Main Function acting as a client for Video Decode Call */
XDAS_Int32 main ()
{
    /* File I/O variables */
  FILE *fConfigFile, *ftestFile, *finFile, *fparamsFile;

  XDAS_Int8 line[STRING_SIZE], inFile[STRING_SIZE], testFile[STRING_SIZE];
  XDAS_Int8 paramsFile[STRING_SIZE], testCompliance;
  XDAS_Int8 *fname = "..\\..\\Test\\TestVecs\\Config\\Testvecs.cfg";


  XDAS_UInt32  ii;

  IVIDDEC2_Fxns *ividDecFxns;

  /* Base Class Structures                                                    */
  IVIDDEC2_Status             status;
  IVIDDEC2_InArgs             inArgs;
  IVIDDEC2_OutArgs 		      outArgs;


  /* Algorithm specific handle */
  IALG_Handle handle;

  /* Input/Output Buffer Descriptor variables */
  XDM1_BufDesc inputBufDesc;
  XDM_BufDesc outputBufDesc;

  /*-------------------------------------------------------------------------*/
  /*  Handle to a buffer element allocated by the buffer manager module      */
  /*-------------------------------------------------------------------------*/
  BUFFMGR_buffEleHandle buffEle;


  /* DMAN3 specific variables                                                 */
  XDAS_Int32  groupId, numAlgs;
  IALG_Handle handleArray[NUM_ALGS];
  IDMA3_Fxns  *dmaFxns[NUM_ALGS];
  XDAS_UInt32 qdmaChannels[6]={0,1,2,3,4,5};

  /* Input Buffer Management specific variables                               */
  XDAS_Int32 validBytes, bytesInAppInputBuffer;
  XDAS_UInt32 bytesConsumed;

  /* Other variables                                                          */
  XDAS_Int32  frameCount, retVal, testVal = XDM_EOK, countConfigSet;

  XDAS_Int8   ret_val,first_time;

    /* Enable Cache Settings                                                  */
    TestApp_EnableCache();

    fConfigFile = fopen(fname,"r");
    if (!fConfigFile)
    {
        printf("Couldn't open config file %s",fname);
        return XDM_EFAIL;
    }
  /* Reset countConfigSet value to 1                                          */
  countConfigSet = 1;
  /* Read the Config File until it reaches the end of file                    */
  while(!feof(fConfigFile))
  {
    /* Read Compliance Checking parameter                                     */

    if(fgets(line,254,fConfigFile))
    {
      sscanf(line,"%d",&testCompliance);
    }
    else
    {
      break ;
    }
    /* Read Parameters file name                                              */
    if(fgets(line,254,fConfigFile))
    {
      sscanf(line,"%s",paramsFile);
    }
    else
    {
      break ;
    }
    /* Read Input file name                                                   */
    if(fgets(line,254,fConfigFile))
    {
      sscanf(line,"%s",inFile);
    }
    else
    {
      break ;
    }

    /* Read Output/Reference file name                                       */
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

    /* Open Parameters file                                                   */
    fparamsFile = fopen (paramsFile, "rb");
    if (!fparamsFile)
    {
      printf("\nCouldn't open Parameters file...   %s\n ",paramsFile);
      printf("Exiting for this configuration...\n");
      continue;
    }

    if(readparamfile(fparamsFile) < 0)
    {
      printf("\nSyntax Error in %s\n ",paramsFile);
      printf("Exiting for this configuration...\n");
      continue;
    }

    /* Close Parameters File                                                  */
    fclose(fparamsFile);

    if(testCompliance)
    {
      printf("\nRunning in Compliance Mode");
    }
    else
    {
      printf("\nRunning in Output Dump Mode");
    }

    /* Open input file                                                        */
    finFile = fopen (inFile, "rb");
    if (!finFile)
    {
      printf("\n Couldn't open Input file...  %s  ",inFile);
      printf("\n Exiting for this configuration...");
      continue;
    }

    /* Open output/reference file                                             */
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
      printf("\nCouldn't open Test File... %s",testFile);
      printf("\n Exiting for this configuration..." );
      continue;
    }

    /* Initialization of parameters needed for Algorithm Instance create      */
    TestApp_SetInitParams(&params);

    /* Initializing DMAN3 parameters */
    /* Base Index Value from where DMA params are written                     */
    DMAN3_PARAMS.paRamBaseIndex  = 78;
    /* Number of QDMA channels used by the Algorithm                          */
    DMAN3_PARAMS.numQdmaChannels = 4;
    /* Number of Algorithms: System Parameter                                 */
    numAlgs                      = NUM_ALGS;
    /* Group Id needed by DMAN3 library: System Parameter                     */
    groupId                      = 0;
	/* DMAN3_NUM_CONTIGUOUS_PARAM_ENTRIES                                     */
	DMAN3_PARAMS.numPaRamEntries = 48;
	/* DMAN3_NUMPARAM_GROUP                                                   */
	DMAN3_PARAMS.numPaRamGroup[0]= 0;
	/* DMAN3_QDMA_CHANNELS[]                                                  */
	DMAN3_PARAMS.qdmaChannels	 = qdmaChannels;

    /* Setting the sizes of Base Class Objects                                */
    params.size                  = sizeof(IVIDDEC2_Params);
    status.size                  = sizeof(IVIDDEC2_Status);
    dynamicParams.size           = sizeof(IVIDDEC2_DynamicParams);
    inArgs.size                  = sizeof(IVIDDEC2_InArgs);
    outArgs.size                 = sizeof(IVIDDEC2_OutArgs);

     /* Initializing Variables                                                */
    frameCount                   = 0;

    /* Read the bitstream in the Application Input Buffer                     */
    validBytes = bytesInAppInputBuffer = TestApp_ReadByteStream(finFile);
    fclose(finFile);

    /* Return if there is an error in reading the file                        */
    if (validBytes < 0) /* Error condition                                    */
	{
        printf("Valid Bytes incorrect... Exiting for this configuration...\n");
	    continue;
	}

    /* Create the Algorithm object (instance)                                 */
    printf("\nCreating Algorithm Instance...");

    if ((handle =  (IALG_Handle)ALG_create (
      (IALG_Fxns *) &MPEG2VDEC_TI_IMPEG2VDEC,
      (IALG_Handle) NULL, (IALG_Params *) &params)) == NULL)
    {
      printf( "\nFailed to Create Instance... Exiting for this configuration..");
	    continue;
    }
    printf("\nAlgorithm Instance Creation Done...\n");

    /* DMA allocation using DMAN3 API's                                        */
    DMAN3_init();

    /* Provide the Algorithm handle to DMAN3 Interface for granting channels   */
    handleArray[0] = (IALG_Handle) handle;
    dmaFxns[0]     = &MPEG2VDEC_TI_IDMA3;

    if (DMAN3_grantDmaChannels(groupId, handleArray, dmaFxns, numAlgs)
      != DMAN3_SOK) {
      printf("\nProblem adding algorithm's dma resources");
      printf("... Exiting for this configuration... \n");
      continue;
    }
    /*--------------------------------------------------------------------------*/
    /*  The outBufsInUseFlag tells us whether the previous input buffer given   */
    /*  by the application to the algorithm is still in use or not. Since       */
    /*  this is before the first decode call, assign this flag to 0. The        */
    /*  algorithm will take care to initialize this flag appropriately from     */
    /*  hereon for the current configuration.                                   */
    /*--------------------------------------------------------------------------*/
    outArgs.outBufsInUseFlag     = 0;

    TestApp_SetDynamicParams(&dynamicParams);
    /* Assigning Algorithm handle fxns field to ividEncfxns                     */
    ividDecFxns = (IVIDDEC2_Fxns *)handle->fxns;

    /*--------------------------------------------------------------------------*/
    /*  The XDM_RESET call gives resets all the internal data structures and    */
    /*  context for the new stream that is to be decoded.                       */
    /*--------------------------------------------------------------------------*/
    ividDecFxns->control((IVIDDEC2_Handle)handle, XDM_RESET,
                        (IVIDDEC2_DynamicParams *)&dynamicParams,
                        (IVIDDEC2_Status *)&status);
    ividDecFxns->control((IVIDDEC2_Handle)handle, XDM_SETPARAMS,
                        (IVIDDEC2_DynamicParams *)&dynamicParams,
                        (IVIDDEC2_Status *)&status);
    /*--------------------------------------------------------------------------*/
    /*  The GETBUFINFO call gives information for number and size of input      */
    /*  and output buffers. For this first call, the actual width and height    */
    /*  of the picture is not known hence, this control call returns the out    */
    /*  buffer sizes based on the maxHeight and maxWidth provided as part       */
    /*  of the input params from testparams file.                               */
    /*--------------------------------------------------------------------------*/
    ividDecFxns->control((IVIDDEC2_Handle)handle, XDM_GETBUFINFO,
                        (IVIDDEC2_DynamicParams *)&dynamicParams,
                        (IVIDDEC2_Status *)&status);
    /*--------------------------------------------------------------------------*/
    /*  The buffermanager init function call allocates the memory for the       */
    /*  number of buffers of the corresponding sizes as passed in arguments     */
    /*  to it. This function initializes the output/reference bufs based on     */
    /*  the outBuf parameters returned from the GETBUFINFO call above. This     */
    /*  call will allocate each of the numOutbufs memory equal to (specified    */
    /*  bufsize). The initialization and allocation of buff ptr will take       */
    /*  place only for the first ref buffer. For the remaining ref buffers,     */
    /*  allocation will happen after the first decode call, when actual         */
    /*  height and width of picture are known.                                  */
    /*  Note that the allocation of output/ref buffers will happen each time    */
    /*  for the entire bunch of configuration sets provided in the testvecs     */
    /*  config file.                                                            */
    /*--------------------------------------------------------------------------*/
    ret_val = BUFFMGR_Init(status.bufInfo.minNumOutBufs,
                                                 status.bufInfo.minOutBufSize);

    if (ret_val)
    {
        printf ("\nMemory could not get allocated for output buffers\n");
        break;
    }

    /*--------------------------------------------------------------------------*/
    /*  Initialize the input buffer properties as required by algorithm         */
    /*  based on info received by preceding GETBUFINFO call. First init the     */
    /*  number of input bufs.                                                   */
    /*--------------------------------------------------------------------------*/

    inputBufDesc.numBufs = status.bufInfo.minNumInBufs ;
    inputBufDesc.descs[0].bufSize = status.bufInfo.minInBufSize[0];

      for(ii=0; ii<(status.bufInfo.minNumInBufs-1);ii++ )
      {
        inputBufDesc.descs[ii+1].buf = inputBufDesc.descs[ii].buf +
          status.bufInfo.minInBufSize[ii];
        inputBufDesc.descs[ii+1].bufSize =
          status.bufInfo.minInBufSize[ii +1];
      }

    /*-----------------------------------------------------------------------*/
    /*  Initialize the output buffer properties as required by algorithm     */
    /*  based on info received by preceding GETBUFINFO call.                 */
    /*-----------------------------------------------------------------------*/
    outputBufDesc.numBufs = status.bufInfo.minNumOutBufs;
    /*-----------------------------------------------------------------------*/
    /*  Initialize the first time flag to 1 which will get used for the      */
    /*  condition check for reinitializing the buffers after the first       */
    /*  decode call.                                                         */
    /*-----------------------------------------------------------------------*/
    first_time = 1;

    /* set space for buffer descriptors                                      */

    inputBufDesc.descs[0].buf     = inputData;


    /* Resetting bytesGenerated variable                                     */
    bytesConsumed = 0;

    /* Do-While Loop for Decoder Call                                        */
    do
    {


      /* Assign the number of bytes available                                */
      inArgs.numBytes        = validBytes;
      inputBufDesc.descs[0].buf   = (XDAS_Int8 *)((XDAS_Int32) inputData +
        bytesConsumed);

      /*---------------------------------------------------------------------*/
      /*  Check for the value of the outBufsInUseFlag. If the previous       */
      /*  decode call was for the first field of an interlaced frame, the    */
      /*  previous outbuf supplied by algorithm may still be required for    */
      /*  decoding second field. In that case, the algorithm will not need   */
      /*  a new buffer from the application. If above is not the case, call  */
      /*  the getfreebuffer() function of buffermanager to get the buffer    */
      /*  element to be supplied to algorithm for the next decode call.      */
      /*---------------------------------------------------------------------*/
      if (!outArgs.outBufsInUseFlag)
      {
          buffEle = BUFFMGR_GetFreeBuffer();
      }

      /*---------------------------------------------------------------------*/
      /*  Copy the output buffer size values and pointers from the buffer    */
      /*  element returned by getfreebuffer() function. Initialize the       */
      /*  numBufs to 2 as numoutbufs is always 2 for luma and chroma buffers.*/
      /*---------------------------------------------------------------------*/
      outputBufDesc.bufSizes    = (XDAS_Int32 *) buffEle->bufSize;
      outputBufDesc.bufs        = (XDAS_Int8 **) buffEle->buf;

      /*---------------------------------------------------------------------*/
      /*  Initialize the input ID in input arguments to the bufferid of the  */
      /*  buffer element returned from getfreebuffer() function.             */
      /*---------------------------------------------------------------------*/
      inArgs.inputID            = buffEle->bufId;

     /* Optional: Set Run time parameters in the Algorithm via control()    */
      ividDecFxns->control((IVIDDEC2_Handle)handle, XDM_SETPARAMS,
        (IVIDDEC2_DynamicParams *)&dynamicParams,
        (IVIDDEC2_Status *)&status);

      /* Cache Invalidate for Input Buffer                                   */
      for(ii=0 ; ii < inputBufDesc.numBufs ; ii++ )
      {
        /* Cache Invalidate for Input Buffer                                 */
        BCACHE_inv(inputBufDesc.descs[ii].buf, validBytes, TRUE);
      }

      /* Basic Algorithm process() call                                      */
      retVal = ividDecFxns->process((IVIDDEC2_Handle)handle,
        (XDM1_BufDesc *)&inputBufDesc,
        (XDM_BufDesc *)&outputBufDesc,
        (IVIDDEC2_InArgs *)&inArgs,
        (IVIDDEC2_OutArgs *)&outArgs);

      /* Cache Writeback Invalidate for Output Buffers                       */
      if(outArgs.outputID[0] > 0)
	  {
        for(ii=0 ; ii < outputBufDesc.numBufs ; ii++ )
        {
          BCACHE_wbInv(outputBufDesc.bufs[ii],
            outputBufDesc.bufSizes[ii], TRUE);
        }
	  }

      bytesConsumed          += outArgs.bytesConsumed;
      validBytes             -= outArgs.bytesConsumed;

      if(retVal == XDM_EFAIL)
      {
        printf("\n Process function returned an Error for stream %s...",inFile);
        break; /* Error Condition: Application may want to break off         */
      }

      /* Optional: Read status via control()                                 */
      ividDecFxns->control((IVIDDEC2_Handle)handle,
        XDM_GETSTATUS,
        (IVIDDEC2_DynamicParams *)&dynamicParams,
        (IVIDDEC2_Status *)&status);

	 /*---------------------------------------------------------------------*/
      /*  If this is the first time in the stream, reinitialize all the      */
      /*  buffers based on the picture width and height.                     */
      /*---------------------------------------------------------------------*/
      if (first_time)
      {
          /*-----------------------------------------------------------------*/
          /*  The GETBUFINFO call will give information for number of        */
          /*  output buffers. This call is after the first decode call hence */
          /*  the outputbuffer sizes returned by this call is calculated     */
          /*  based on the actual pic width and height. Hence, there is need */
          /*  for buffer allocation and initializations which happens        */
          /*  immediately after this call.                                   */
          /*-----------------------------------------------------------------*/
         ividDecFxns->control((IVIDDEC2_Handle)handle,XDM_GETBUFINFO,
                             (IVIDDEC2_DynamicParams *)&dynamicParams,
                             (IVIDDEC2_Status *)&status);

          /*-----------------------------------------------------------------*/
          /*  Reinitialize the buffers using the actual buffer sizes for     */
          /*  luma and chroma. This call will allocate and initialize the    */
          /*  remaining buf ptrs for the max num of ref frames supplied by   */
          /*  the alg in maxNumDisplayBufs (max ref buffers required). The   */
          /*  first call to buffmgr_init() had allocated the first ref       */
          /*  buffers hence this reinit call will allocate and init the      */
          /*  remaining ref buffers' pointers. The 1st ref buf ptr will      */
          /*  continue to be the one which was allocated by the buffmgr_init */
          /*  call.                                                          */
          /*-----------------------------------------------------------------*/
          BUFFMGR_ReInit (status.maxNumDisplayBufs,
                  status.bufInfo.minNumOutBufs, status.bufInfo.minOutBufSize);

          /*-----------------------------------------------------------------*/
          /*  Reset the flag first_time to 0 since the buffers are correctly */
          /*  reinitialized.                                                 */
          /*-----------------------------------------------------------------*/
          first_time = 0;

      }


      /* Check for frame ready via display buffer pointers                   */
      if(outArgs.outputID[0] > 0)
      {
        printf("\n Decoded Frame # %d  ",frameCount);

        if(testCompliance)
        {
          /* Compare the output frames with the Reference File               */
          testVal = TestApp_CompareOutputData(ftestFile,
            &outputBufDesc,
            &outArgs);

          if(testVal == XDM_EOK)
		  {
			printf("\nPASS: Pass criteria that more than 99 percent decoded samples should have at \n \
			 the most one-bit difference with reference output is satisfied for this frame");
	      }
	      else
	      {
			/* Test Compliance Failed... Breaking...*/
            break;
          }
        }
        else
        {
          /* Write the output frames in the display order                    */
          TestApp_WriteOutputData(ftestFile, &outputBufDesc, &outArgs);
        }
        frameCount++;
      }

      /*---------------------------------------------------------------------*/
      /*  Always release buffers - which are released from the algorithm     */
      /*  side   -back to the buffer manager. The freebufID array of outargs */
      /*  contains the sequence of bufferIds which need to be freed. This    */
      /*  gets populated by the algorithm. The following function will do    */
      /*  the job of freeing up the buffers.                                 */
      /*---------------------------------------------------------------------*/
      BUFFMGR_ReleaseBuffer((XDAS_UInt32 *)outArgs.freeBufID);

      /* Check for Break off condition                                       */
      if (bytesConsumed >= bytesInAppInputBuffer)
      {
        printf("\n Bitstream Ended...  ");
        break; /* Bitstream ended: Break off the while loop                  */
      }
      if (frameCount >= framesToDecode)
      {
        printf("\n Specified number of Frames Decoded...  ");
        break; /* Bitstream ended: Break off the while loop                  */
      }
    }  while(1);  /* end of Do-While loop                                    */

    /*-----------------------------------------------------------------------*/
    /*  The algorithm currently does not guarantee to set freebufid of       */
    /*  all buffers in use when the framecount exceeds framestoDecode.       */
    /*  This is managed for now by an application function. Note that        */
    /*  doing this has significance only when running multiple streams.      */
    /*-----------------------------------------------------------------------*/
    BUFFMGR_ReleaseAllBuffers();

    /*-----------------------------------------------------------------------*/
    /*  Free the buffers allocated for output/reference for all the streams  */
    /*  that have been executed.                                             */
    /*-----------------------------------------------------------------------*/
    BUFFMGR_DeInit(status.maxNumDisplayBufs, outputBufDesc.numBufs);
    /* Output file close                                                     */
    fclose(ftestFile);
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

    /* Delete the Algorithm instance object specified by handle */
    ALG_delete (handle);

    if(frameCount == 0)
    {
      frameCount = 1; /* To avoid division with zero */
    }

	if(testVal == XDM_EOK)
	{
      printf("\n --------------  SUMMARY --------------------\n");

      if(!testCompliance)
      {
        printf(" Decoder output dump completed\n");
      }
      printf("\t Total number of Frames              = %d\n",
        (XDAS_UInt32)frameCount);
      printf("\t Bit Rate at 30 frames/Sec           = %d Kbps\n",
        (XDAS_UInt32)(((bytesConsumed*8*30)/frameCount)/1000));
      printf("\t Width and Height                    = %d, %d \n",
        (XDAS_UInt32)status.outputWidth, (XDAS_UInt32)status.outputHeight);
    }

      printf ("\n--------------    END   --------------------\n");
  } /* Read the Config File until it reaches the end of file */

  printf("\n End of execution\n");
  fclose(fConfigFile);

  return XDM_EOK;
} /* main() */

/*---------------------------------------------------------------------------*/
/*  TestApp_ReadByteStream                                                   */
/*  Reading Byte Stream from a File                                          */
/*---------------------------------------------------------------------------*/

  XDAS_Int32 TestApp_ReadByteStream(FILE *finFile)
  {
    XDAS_UInt32 BytesRead, BufferSize;

    fseek (finFile, 0, SEEK_END);
    BufferSize = ftell (finFile);
    printf("\nFileSize = %d", BufferSize);
    fseek (finFile, 0, SEEK_SET);

    if (BufferSize > INPUT_BUFFER_SIZE)
    {
      printf( "\nWarning : File size exceeds the application input buffer size %d ",
        INPUT_BUFFER_SIZE);
      printf( "\nContinuing decoding for %d bytes.\n", INPUT_BUFFER_SIZE);
      BufferSize = INPUT_BUFFER_SIZE;
    }

    /*Read the "BufferSize" number of bytes in the input buffer*/
    BytesRead = fread (inputData, 1, BufferSize, finFile);
    /* Cache Invalidate for Input Buffer */
    BCACHE_wbInv(inputData, BufferSize, TRUE);

    if (BytesRead != BufferSize)
    {
      printf("\nFailure in reading the input bitstream! \n");
      return(-1);
    }
    printf ("\nInput File read successfully...");
    return (BytesRead);
  }

  /*---------------------------------------------------------------------------*/
  /*  TestApp_CompareOutputData                                                */
  /*  Comparing Output Data with Reference File data                           */
  /*---------------------------------------------------------------------------*/
  XDAS_Int32 TestApp_CompareOutputData(FILE *fRefFile,
    XDM_BufDesc * outputBufDesc,
    IVIDDEC2_OutArgs *outArgs)
  {
    XDAS_Int32 i, bufferSize, retVal, k;
    XDAS_UInt8 *outputData, *refDataBuf;
    XDAS_Int32 noDiff, onebitDiff, gbitDiff, diff;
    XDAS_Int32 result, totPixels;
    float percent;

    retVal = XDM_EOK;
    refDataBuf = (XDAS_UInt8 *)refData;

	noDiff = onebitDiff = gbitDiff = 0;

    for(i = 0; i < outArgs->displayBufs[0].numBufs; i++)
    {
      outputData = (XDAS_UInt8 *)outArgs->displayBufs[0].bufDesc[i].buf;
      bufferSize = outArgs->displayBufs[0].bufDesc[i].bufSize;
      fread (refDataBuf, 1, bufferSize, fRefFile);

      for(k = 0;k < bufferSize; k++)
      {
        if(outputData[k] != refDataBuf[k])
        {
		   diff = abs(outputData[k] - refDataBuf[k]);
		   if(diff == 1)
		      onebitDiff++;
		   else
			  gbitDiff++;
	    }
	    else
	       noDiff++;
      }
    }

	result = (noDiff + onebitDiff) * 100;
	totPixels = noDiff + onebitDiff + gbitDiff;

	percent = (float)result/(float)totPixels;

	if(percent < 99)
	{
	  printf("\nFAIL: Pass criteria that more than 99 percent decoded samples should have at the most \n \
	   one-bit difference with reference output is NOT satisfied for this frame");
	  retVal = XDM_EFAIL;
	}

    return retVal;
}

  /*----------------------------------------------------------------------------*/
  /*  TestApp_WriteOutputData                                                   */
  /*  Writing Output Data in a File                                             */
  /*----------------------------------------------------------------------------*/

  XDAS_Void TestApp_WriteOutputData(FILE *fOutFile,
    XDM_BufDesc * outputBufDesc,
    IVIDDEC2_OutArgs *outArgs)
  {

    XDAS_UInt8 *s;
    XDAS_Int32 i;

    for(i = 0; i < outArgs->displayBufs[0].numBufs; i++)
    {
      s = (XDAS_UInt8 *)outArgs->displayBufs[0].bufDesc[i].buf;
      fwrite (s, sizeof (Byte), outArgs->displayBufs[0].bufDesc[i].bufSize, fOutFile);
    }

    fflush (fOutFile);
    return;
  }


/*------------------------------------------------------------------------------*/
/*  TestApp_SetInitParams                                                       */
/*  setting of creation time parameters                                         */
/*------------------------------------------------------------------------------*/

  XDAS_Void TestApp_SetInitParams(IVIDDEC2_Params *params)
  {
    /* Set IVIDDEC parameters                                                   */

    /* Max Frame Rate: Not currently used in the algorithm                      */
    params->maxFrameRate        = 30;

    /* Max Bit Rate: Not currently used in the algorithm                        */
    params->maxBitRate          = 10000000;

    /* Data Endianness (0: Little, 1: Big) : Big Endian not supported           */
    params->dataEndianness      = XDM_BYTE;
    return;
  }

/*------------------------------------------------------------------------------*/
/*  TestApp_SetDynamicParams                                                    */
/*  setting of run time parameters                                              */
/*------------------------------------------------------------------------------*/

  XDAS_Void TestApp_SetDynamicParams(IVIDDEC2_DynamicParams *dynamicParams)
  {
    /* Set IVIDDEC Run time parameters */
    dynamicParams->decodeHeader  = XDM_DECODE_AU; //Supported
    dynamicParams->displayWidth  = 0;             //Supported
    dynamicParams->frameSkipMode = IVIDEO_NO_SKIP;//Not Supported: Set to default value

    return;
  }

#define EXT_MEM_BASE (0x80000000)
#define EXT_MEM_SIZE (0x05000000)

/*  Cache Settings                                                               */
XDAS_Void TestApp_EnableCache(void)
{
    BCACHE_Size size;

	size.l1psize  = BCACHE_L1_32K; /* L1P cache size                             */
	size.l1dsize  = BCACHE_L1_16K; /* L1D cache size                             */
	size.l2size   = BCACHE_L2_64K; /* L2  cache size                             */

	/* Set L1P, L1D and L2 cache sizes                                           */
    BCACHE_setSize(&size);

    /* Cache Enable External Memory Space                                        */
	/* BaseAddr, length, MAR enable/disable                                      */
    /* Cache 0x80000000 --- 0x8FFFFFFF                                           */
    BCACHE_setMar((Ptr *)EXT_MEM_BASE, EXT_MEM_SIZE, BCACHE_MAR_ENABLE);
    BCACHE_wbInvAll(); /* No need to call BCACHE_wait() */
} /* DoCacheSettings                                                             */


/*
//==============================================================================
//    Copyright (c) 2006 Texas Instruments, Incorporated
//    All Rights Reserved
//==============================================================================
*/
