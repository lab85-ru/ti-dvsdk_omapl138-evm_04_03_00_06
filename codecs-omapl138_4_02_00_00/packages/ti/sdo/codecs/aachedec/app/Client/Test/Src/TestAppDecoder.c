/** ==================================================================
* @file TestAppDecoder.c
*
* @desc This file shows how to use the generic DECODER decoder library
*       directly in an application.
* =====================================================================
* Copyright (c) Texas Instruments Inc 2004-2006
*
* Use of this software is controlled by the terms and conditions found
*in the license agreement under which this software has been supplied *
=====================================================================*/

/* AAC Decoder Interface header files */
#include "TestAppDecoder.h"
#include "iaacdec.h"
#include "bcache.h"
XDAS_Void TestApp_EnableCache(void);

/* Input buffer allocation */
XDAS_UInt8 inputData[INPUT_BUFFER_SIZE];

/* Output buffer allocation */
XDAS_UInt16  outputData[OUTPUT_BUFFER_SIZE];

/* Main Function acting as a client for Audio Decode Call */
XDAS_Int32 main (void)
{
    IAUDDEC1_Fxns *iaudDecFxns;

    /* Base Class Structures*/
    IAUDDEC1_Status status;
    IAUDDEC1_Params decParams;
    IAUDDEC1_DynamicParams dynamicparams;
    IAUDDEC1_InArgs inargs;
    IAUDDEC1_OutArgs outargs;

    XDM1_BufDesc inBufDesc;
    XDM1_BufDesc outBufDesc;
    /* Algorithm specific handle*/
    IALG_Handle handle;
    XDAS_Int32 error = 0;
    XDAS_Int32 frameCnt=0,desiredChannelMode,testCompliance=0;
    XDAS_Int16 testVal;
    XDAS_UInt32  ii;

	    /* File I/O variables*/
    FILE   *cfgFp = NULL,*inputFile= NULL, *outputFile= NULL;

    char cfgFileName   [FILE_STRING_SIZE] = "..\\..\\Test\\TestVecs\\Config\\Testvecs.cfg";
    char outputFileName[FILE_STRING_SIZE];
    char inputFileName [FILE_STRING_SIZE];

    TestApp_EnableCache();

    cfgFp = fopen (cfgFileName, "r");
    if (cfgFp == NULL)
    {
        printf ("Error opening config file %s \n", cfgFileName);
        return XDM_EFAIL;
    }

    while (!feof (cfgFp))
    {
        fscanf(cfgFp,"%d %s %s %d ",&testCompliance,&inputFileName, &outputFileName, &desiredChannelMode);

        if(testCompliance)
        printf("\nRunning in Compliance Mode");
        else
        printf("\nRunning in Output Dump Mode");

        /* check for commented lines */
        if ((inputFileName[0] == ':') || (inputFileName[0] == ';')
            || (inputFileName[0] == '#'))
            continue;

        /* Open input file */
        inputFile = fopen (inputFileName, "rb");
        if (inputFile == NULL)
        {
            printf
                ("Error opening input file %s.....proceeding to next file.\n",
                 inputFileName);
            continue;
        }

        /* Open output/Reference file */
        if(testCompliance)
        outputFile = fopen (outputFileName, "rb");
        else
        outputFile = fopen (outputFileName, "wb");

        if (outputFile == NULL)
        {
            printf
                ("Error opening output/reference file %s.....proceeding to next file.\n",
                 outputFileName);
            continue;
        }

        printf ("\nDecoding %s file. Please wait..................\n",
                inputFileName);

        /* Setting the sizes of Base Class Objects*/
        decParams.size               = sizeof(IAUDDEC1_Params);
        status.size                  = sizeof(IAUDDEC1_Status);
        dynamicparams.size           = sizeof(IAUDDEC1_DynamicParams);
        inargs.size                  = sizeof(IAUDDEC1_InArgs);
        outargs.size                 = sizeof(IAUDDEC1_OutArgs);

		decParams.outputPCMWidth = 16;
		decParams.pcmFormat = IAUDIO_INTERLEAVED;
        decParams.dataEndianness = XDM_LE_16;

        inargs.numBytes = 0;
		inargs.lfeFlag = 0;
		inargs.desiredChannelMode = desiredChannelMode;

        outargs.bytesConsumed = 0;
        outargs.extendedError = XDM_EOK;


        /* Create an instance of the algorithm */
        if ((handle = (IALG_Handle )ALG_create((IALG_Fxns *)&AACDEC_TII_IAACDEC,NULL, (IALG_Params *)&decParams)) != NULL)
        {

        /* Assigning Algorithm handle fxns field to iaudDecfxns*/
        iaudDecFxns = (IAUDDEC1_Fxns *)handle->fxns;

        /* Get Buffer information */
        error = iaudDecFxns->control((IAUDDEC1_Handle)handle, XDM_GETBUFINFO,&dynamicparams, &status);

		/* Input and output buffer information for process API*/
        inBufDesc.numBufs = status.bufInfo.minNumInBufs;
		inBufDesc.descs[0].bufSize = status.bufInfo.minInBufSize[0];
        outBufDesc.numBufs = status.bufInfo.minNumOutBufs;
		outBufDesc.descs[0].bufSize = status.bufInfo.minOutBufSize[0];

        inBufDesc.descs[0].buf = (XDAS_Int8*)&inputData;
		outBufDesc.descs[0].buf = (XDAS_Int8*)&outputData;


		error = iaudDecFxns->control((IAUDDEC1_Handle)handle, XDM_SETDEFAULT,&dynamicparams, &status);

		dynamicparams.downSampleSbrFlag = 0;
		error = iaudDecFxns->control((IAUDDEC1_Handle)handle, XDM_SETPARAMS,&dynamicparams, &status);

       if(error == IALG_EFAIL)
	   {
			printf("SetParams command Failed\n");
	   }

        while (1)
        {
          inargs.numBytes  = inargs.numBytes +fread(inputData + inargs.numBytes, 1,
                                   sizeof(inputData) - inargs.numBytes, inputFile);
          /* size of input data in bytes in the input buffer*/
          if ( inargs.numBytes < 8)
                    break;

      /* Cache Write Back Invalidate after doing fread*/
          BCACHE_wbInv(inputData, INPUT_BUFFER_SIZE*2, TRUE);


          /* Cache Invalidate for Input Buffer*/
          for(ii=0 ; ii < inBufDesc.numBufs ; ii++ )
          {
          /* Cache Invalidate for Input Buffer*/
          BCACHE_inv(inBufDesc.descs[0].buf, inBufDesc.descs[0].bufSize, TRUE);
          }

         BCACHE_wbInvAll();

        error = iaudDecFxns->process((IAUDDEC1_Handle)handle, &inBufDesc, &outBufDesc,&inargs, &outargs);

		/* Cache Writeback Invalidate for Output Buffers*/
        if(inBufDesc.descs[0].buf != NULL)
         {
           for(ii=0 ; ii < outBufDesc.numBufs ; ii++ )
           {
            BCACHE_wbInv(outBufDesc.descs[0].buf,outBufDesc.descs[0].bufSize, TRUE);
           }
         }


        if (outargs.extendedError)
          {
           printf("AAC decoder failed error %d to decode frame %d \n", outargs.extendedError, frameCnt);
           if(XDM_ISFATALERROR(outargs.extendedError))
             iaudDecFxns->control((IAUDDEC1_Handle)handle, XDM_RESET,&dynamicparams, &status);
                   // break;
          }


         memmove(inputData, inputData + outargs.bytesConsumed,
         inargs.numBytes - outargs.bytesConsumed);
         inargs.numBytes -= outargs.bytesConsumed;

         error = iaudDecFxns->control((IAUDDEC1_Handle)handle, XDM_GETSTATUS,&dynamicparams, &status);


         if(testCompliance)
         {
                if((frameCnt > 1) && (outargs.extendedError == XDM_EOK))
                    testVal = CompareOutputData(outputFile,outputData, (XDAS_Int16)status.channelMode,status.numSamples);
                    else
                    testVal=XDM_EOK;

                 if(testVal == XDM_EOK)
                  {
                  printf("\n Decoder compliance test passed for the frame No=%d.  ",frameCnt);
                  }
                 else
                  {
                  printf("\n Decoder compliance test Failed for the frame No=%d.  ",frameCnt);
                  break;
                  }
         }
         else
         {
         /* Write output to File */
         /* Skip first two blocks so output is time aligned with input */
         if((frameCnt > 1) && (status.validFlag == 1))
         {

			 WriteToFile(outputFile,outBufDesc.descs[0].buf,  &status);

          }
         }
         /* Increment Frame Count */
         frameCnt++;
            }

         printf ("\nCompleted Decoding %d frames of %s file.\n",frameCnt,
                inputFileName);
         frameCnt=0;

            ALG_delete((IALG_Handle)handle);/* delete the algorithm */
         }

         if (outputFile)
            fclose (outputFile);
         if (inputFile)
            fclose (inputFile);
    }
    fclose (cfgFp);
    return XDM_EOK;

}


XDAS_Int16 CompareOutputData(FILE *outputFile,UWord16 *out, Word16 nch,Word32 nSamples)
{
    Word16 i,retVal;
    UWord16 c,temp[1];
    retVal = XDM_EOK;

    if(nch == IAUDIO_1_0)
    {
        for(i=0;i<nSamples;i=i+1)
        {
            /* Left Channel */
            c    = out[i];
            fread(&temp, 1,sizeof(Word16) , outputFile);
            if(c!=temp[0])
            {
            retVal = XDM_EFAIL;
            break;
            }
        }
    }
    else
    {
        for(i=0;i<nSamples*2;i=i+2)
        {
            /* Left Channel */
            c    = out[i];
            fread(&temp, 1,sizeof(Word16) , outputFile);
            if(c!=temp[0])
            {
            retVal = XDM_EFAIL;
            break;
            }

            /* Right Channel  */
            c    = out[i+1];
            fread(&temp, 1,sizeof(Word16) , outputFile);
            if(c!=temp[0])
            {
            retVal = XDM_EFAIL;
            break;
            }
        }
    }

     return retVal;
} // end function WriteToFileAudioInterleaved



void WriteToFile(FILE *outputFile,XDAS_Int8 *out, IAUDDEC1_Status *status)
{

   XDAS_Int16 bytesPerSample = (status->outputBitsPerSample >> 3);

   if(status->channelMode == IAUDIO_1_0)
   {
	   fwrite(out, bytesPerSample, status->numSamples, outputFile);
   }

   else/*Stereo*/
   {
    	if(status->pcmFormat == IAUDIO_INTERLEAVED)
   	    {

   	       fwrite(out, bytesPerSample, status->numSamples*2, outputFile);
   	    }

   	else/*Block Mode*/
   	{
	   XDAS_Int32 i;
   	   XDAS_Int8 * outL = (XDAS_Int8*)((XDAS_Int8 *) out);
       XDAS_Int8 * outR = (XDAS_Int8*)((XDAS_Int8 *) out + (status->numSamples * bytesPerSample)) ;

   		for(i=0;i<(status->numSamples * bytesPerSample); i+= bytesPerSample)
	    {
	            /* Left Channel */
	        fwrite(&outL[i], bytesPerSample, 1, outputFile);

	            /* Right Channel  */
	        fwrite(&outR[i], bytesPerSample, 1, outputFile);
        }

   	}

   }

}

#define EXT_MEM_BASE (0x80000000)
#define EXT_MEM_SIZE (0x10000000)

/* Cache Settings */
XDAS_Void TestApp_EnableCache(void)
{
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
} /* TestApp_EnableCache */


/* Nothing beyond this point. */
