/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/*!
********************************************************************************
@file     TestAppDecoder.c
@brief    This is the top level client file that drives the JPEG
(Baseline Profile) Image Decoder Call using XDM Interface
@author   Multimedia Codecs TI India
@version  0.1 - Jan 24,2006    initial version
********************************************************************************
*/

/* Standard C header files */
#include <stdio.h>
#include <stdlib.h>

/* JPEG Interface header files */
#include "jpegdec_ti.h"
#include "jpegdec.h"
/* CSL and DMAN3 header files */

 /* CSL and DMAN3 header files */
#include "bcache.h"
 

/* Client header file */
#include "TestAppDecoder.h"

/* Input buffer allocation */
#pragma    DATA_SECTION(inputData, ".input_buffer")
XDAS_Int8 inputData[INPUT_BUFFER_SIZE];

/* Output buffer allocation */
#pragma    DATA_SECTION(outputData, ".display_buffer")
XDAS_Int8  outputData[OUTPUT_BUFFER_SIZE];

/* Output buffer allocation */
#pragma    DATA_SECTION(refData, ".ref_buffer")
XDAS_Int8  refData[OUTPUT_BUFFER_SIZE];

JPEGDEC_Handle    handle;
JPEGDEC_Params    params;
JPEGDEC_DynamicParams    dynamicParams;
JPEGDEC_Status    status;
JPEGDEC_InArgs	   inArgs; 
IJPEGDEC_OutArgs    outArgs; 

XDAS_UInt32 resizeOption,progressiveDecFlag,RGB_Format,alpha_rgb;
XDAS_UInt32 outImgRes,numMCU_row,x_org,y_org,x_length,y_length;

 
sTokenMapping sTokenMap[] =
{
  {"ImageHeight",    &params.imgdecParams.maxHeight},
  {"ImageWidth",     &params.imgdecParams.maxWidth},
  {"Scan",			 &params.imgdecParams.maxScans},
  {"ChromaFormat",   &params.imgdecParams.forceChromaFormat},
  {"InputFormat",    &params.progressiveDecFlag},
  {"ResizeOption",   &resizeOption},
  {"displayWidth",   &dynamicParams.imgdecDynamicParams.displayWidth},
  {"RGB_Format",     &RGB_Format}, 
  {"outImgRes",      &outImgRes}, 
  {"numAU",          &dynamicParams.imgdecDynamicParams.numAU},
  {"numMCU_row",     &dynamicParams.numMCU_row},
  {"x_org",     &dynamicParams.x_org},
  {"y_org",     &dynamicParams.y_org},
  {"x_length",     &dynamicParams.x_length},
  {"y_length",     &dynamicParams.y_length},  
  {"alpha_rgb",     &dynamicParams.alpha_rgb},
  {NULL,             NULL}     
};
unsigned int   t0, t1, decodetime,total_decodetime,tslice;
float Megapixel;
/* Main Function acting as a client for Image Decode Call */
XDAS_Int32 main ()
{
  /* File I/O variables                                                       */
  FILE *fConfigFile, *ftestFile, *finFile, *fparamsFile,*freportFile;
  unsigned char *store_outptr1,*store_outptr2,*store_outptr3;
  
  XDAS_Int8 line[STRING_SIZE], inFile[STRING_SIZE], testFile[STRING_SIZE];
  XDAS_Int8 paramsFile[STRING_SIZE], testCompliance;
  XDAS_Int8 *fname = "..\\..\\Test\\TestVecs\\Config\\Testvecs.cfg";
  XDAS_Int8 *freport = "..\\..\\Test\\TestVecs\\Config\\Report.txt";
  
  /* Input/Output  buffers  and their sizes                                   */
  XDAS_UInt8*  pInputBuf  [XDM_MAX_IO_BUFFERS];
  XDAS_UInt8*  pOutputBuf [XDM_MAX_IO_BUFFERS];
  XDAS_UInt8   inBufSize  [XDM_MAX_IO_BUFFERS];
  XDAS_UInt8   outBufSize [XDM_MAX_IO_BUFFERS];
  XDAS_UInt32  ii,multi_create;
  
  IIMGDEC1_Fxns *IIMGDEC1Fxns;
  
 
  /* Algorithm specific handle                                                */
  IALG_Handle handle;
  
  /* Input/Output Buffer Descriptors                                          */
  XDM1_BufDesc inputBufDesc, outputBufDesc;
  
  /* DMAN3 specific variables                                                 */
  XDAS_Int32  numAlgs;
 
  /* Input Buffer Management specific variables                               */
  XDAS_Int32 validBytes, bytesInAppInputBuffer; 
  XDAS_UInt32 bytesConsumed;
  
  /* Other variables                                                          */
  XDAS_Int32  ScanCount, retVal, testVal, countConfigSet;
  
  /* Enable Cache Settings                                                    */
  TestApp_EnableCache();

#ifdef TIMER
    /* Enables the Time Stamp Counter */
    TSC_enable();
#endif /*TIMER*/  

  /* Open Test Config File                                                    */
  fConfigFile = fopen(fname,"r");
  
  if (!fConfigFile)
  {
    printf("Couldn't open parameter file %s",fname);
    return XDM_EFAIL;
  }
  
  freportFile = fopen(freport,"w");
  
  if (!freportFile)
  {
    printf("Couldn't open report file %s",freport);
    return XDM_EFAIL;
  }

  countConfigSet = 1; /* Reset countConfigSet value to 1 */
  multi_create   = 1; 
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
      printf("Couldn't open Test File... %s",ftestFile);
      printf("\n Exiting for this configuration..." );
      continue;
    }   
    
    /* Initialization of parameters needed for Algorithm Instance create      */
    TestApp_SetInitParams((IIMGDEC1_Params *)&params);
    
      /* Number of Algorithms: System Parameter                                 */
    numAlgs                      = NUM_ALGS;  
     
    /* Setting the sizes of Base Class Objects                                */

 	  params.imgdecParams.size               = sizeof(IJPEGDEC_Params);
	  status.imgdecStatus.size               = sizeof(IJPEGDEC_Status);
	  dynamicParams.imgdecDynamicParams.size  = sizeof(IJPEGDEC_DynamicParams);
	  inArgs.imgdecInArgs.size               = sizeof(IJPEGDEC_InArgs);
	  outArgs.imgdecOutArgs.size 			 = sizeof(IJPEGDEC_OutArgs);
   
    /* Initializing Variables                                                */
    ScanCount                   = 1; 

    
    /* Read the bitstream in the Application Input Buffer                    */
    validBytes = bytesInAppInputBuffer = TestApp_ReadByteStream(finFile);
    fclose(finFile);

#ifdef CHIP_DAVINCI


#ifdef ENABLE_CACHE
        /* Cache clean */
      BCACHE_wbInvAll(); /* No need to call BCACHE_wait() */

#endif
#endif /*CHIP_DAVINCI*/
    /* Return if there is an error in reading the file                       */
    if (validBytes < 0) /* Error condition                                   */
    {
      printf("Valid Bytes incorrect... Exiting for this configuration...\n");
      continue;
    }
	params.outImgRes  = outImgRes;
	if(multi_create)
	{
    /* Create the Algorithm object (instance)                                */
    printf("\nCreating Algorithm Instance...");
    
    if ((handle =  (IALG_Handle)ALG_create (
      ( IALG_Fxns *) &JPEGDEC_TI_IJPEGDEC, 
      (IALG_Handle) NULL, (IALG_Params *) &params)) == NULL)
    {
      printf( "\nFailed to Create Instance... Exiting for this configuration..");
      continue;
    }
    printf("\nAlgorithm Instance Creation Done...\n");
	multi_create = 1; /* Set to 
	                     1 : multiple create     
	                     0 : Not multiple create
						*/
    }
    dynamicParams.progDisplay = 0;
    dynamicParams.resizeOption = resizeOption;
    dynamicParams.RGB_Format = RGB_Format;
	/*dynamicParams.imgdecDynamicParams.numAU = 0;
	dynamicParams.numMCU_row = 1;*/

    TestApp_SetDynamicParams((IIMGDEC1_DynamicParams *)&dynamicParams);
    
    /* set space for buffer descriptors                                      */
    inputBufDesc.descs[0].buf = (XDAS_Int8 *)pInputBuf;
    outputBufDesc.descs[0].buf = (XDAS_Int8 *)pOutputBuf;
    inputBufDesc.descs[0].bufSize = (XDAS_Int32 )inBufSize ;
    outputBufDesc.descs[0].bufSize = (XDAS_Int32)outBufSize ;

    inputBufDesc.descs[0].buf     = inputData;
    outputBufDesc.descs[0].buf     = outputData;

    /* Assigning Algorithm handle fxns field to IIMGDECfxns                  */
    IIMGDEC1Fxns = (IIMGDEC1_Fxns *)handle->fxns;
    
    /* Resetting bytesGenerated variable                                     */
    bytesConsumed = 0;
    
    dynamicParams.imgdecDynamicParams.decodeHeader = XDM_PARSE_HEADER;       
       /* Activate the Algorithm                                              */
    handle->fxns->algActivate(handle);
    
	/* Assign the number of bytes available                                */
	inArgs.imgdecInArgs.numBytes        = validBytes; 
	dynamicParams.frame_numbytes = inArgs.imgdecInArgs.numBytes;
	inputBufDesc.descs[0].buf   = (XDAS_Int8 *)((XDAS_Int32) inputData +
	bytesConsumed);

	/* Get Buffer information                                              */
	IIMGDEC1Fxns->control((IIMGDEC1_Handle)handle, XDM_GETBUFINFO,
	(IIMGDEC1_DynamicParams *)&dynamicParams,
	(IIMGDEC1_Status *)&status);

	/* Fill up the buffers as required by algorithm                        */
	inputBufDesc.numBufs     = status.imgdecStatus.bufInfo.minNumInBufs ;
	inputBufDesc.descs[0].bufSize = status.imgdecStatus.bufInfo.minInBufSize[0];

	for(ii=0; ii<(status.imgdecStatus.bufInfo.minNumInBufs-1);ii++ )
	{
		inputBufDesc.descs[ii+1].buf = inputBufDesc.descs[ii].buf +
		  status.imgdecStatus.bufInfo.minInBufSize[ii];
		inputBufDesc.descs[ii+1].bufSize=
		  status.imgdecStatus.bufInfo.minInBufSize[ii +1];
	}

	outputBufDesc.numBufs     = status.imgdecStatus.bufInfo.minNumOutBufs ;
	outputBufDesc.descs[0].bufSize = status.imgdecStatus.bufInfo.minOutBufSize[0];
	for(ii=0; ii<(status.imgdecStatus.bufInfo.minNumOutBufs-1);ii++ )
	{
		outputBufDesc.descs[ii+1].buf = outputBufDesc.descs[ii].buf +
		  status.imgdecStatus.bufInfo.minOutBufSize[ii];
		outputBufDesc.descs[ii+1].bufSize =
		  status.imgdecStatus.bufInfo.minOutBufSize[ii+1];
	}
     
     
	/* Cache Invalidate for Input Buffer                                   */
	for(ii=0 ; ii < inputBufDesc.numBufs ; ii++ )
	{
		/* Cache Invalidate for Input Buffer                                 */
        BCACHE_inv(inputBufDesc.descs[ii].buf, validBytes, TRUE);

 	}  

    IIMGDEC1Fxns->control((IIMGDEC1_Handle)handle, XDM_SETPARAMS,
        (IIMGDEC1_DynamicParams *)&dynamicParams,
        (IIMGDEC1_Status *)&status);
  
	/* Basic Algorithm process() call                                      */
	retVal = IIMGDEC1Fxns->process((IIMGDEC1_Handle)handle,
	(XDM1_BufDesc *)&inputBufDesc,
	(XDM1_BufDesc *)&outputBufDesc,
	(IIMGDEC1_InArgs *)&inArgs,
	(IIMGDEC1_OutArgs *)&outArgs);

 


	IIMGDEC1Fxns->control((IIMGDEC1_Handle)handle, XDM_GETSTATUS,
	(IIMGDEC1_DynamicParams *)&dynamicParams,
	(IIMGDEC1_Status *)&status);

 
	IIMGDEC1Fxns->control((IIMGDEC1_Handle)handle, XDM_RESET,
	(IIMGDEC1_DynamicParams *)&dynamicParams,
	(IIMGDEC1_Status *)&status);
     /* DeActivate the Algorithm                                            */
    handle->fxns->algDeactivate(handle);
	
	dynamicParams.imgdecDynamicParams.decodeHeader = XDM_DECODE_AU;       
    
	total_decodetime = 0;
		store_outptr1 = outputBufDesc.descs[0].buf;
		store_outptr2 = outputBufDesc.descs[1].buf;
		store_outptr3 = outputBufDesc.descs[2].buf;
		tslice = 0;
    /* Do-While Loop for Decode Call                                         */
    do
    {
      /* Activate the Algorithm                                              */
      handle->fxns->algActivate(handle);
    
      /* Assign the number of bytes available                                */
      inArgs.imgdecInArgs.numBytes        = validBytes; 
      inputBufDesc.descs[0].buf   = (XDAS_Int8 *)((XDAS_Int32) inputData );

      /* Get Buffer information                                              */
      IIMGDEC1Fxns->control((IIMGDEC1_Handle)handle, XDM_GETBUFINFO,
        (IIMGDEC1_DynamicParams *)&dynamicParams,
        (IIMGDEC1_Status *)&status);
      
      /* Fill up the buffers as required by algorithm                        */
      inputBufDesc.numBufs     = status.imgdecStatus.bufInfo.minNumInBufs ;
      inputBufDesc.descs[0].bufSize = status.imgdecStatus.bufInfo.minInBufSize[0];
      
      for(ii=0; ii<(status.imgdecStatus.bufInfo.minNumInBufs-1);ii++ )
      {
          inputBufDesc.descs[ii+1].buf = inputBufDesc.descs[ii].buf +
          status.imgdecStatus.bufInfo.minInBufSize[ii];
		  		inputBufDesc.descs[ii+1].bufSize=
          status.imgdecStatus.bufInfo.minInBufSize[ii +1];
      }
      
      outputBufDesc.numBufs     = status.imgdecStatus.bufInfo.minNumOutBufs ;
      outputBufDesc.descs[0].bufSize = status.imgdecStatus.bufInfo.minOutBufSize[0];
      for(ii=0; ii<(status.imgdecStatus.bufInfo.minNumOutBufs-1);ii++ )
      {
          outputBufDesc.descs[ii+1].buf = outputBufDesc.descs[ii].buf +
          status.imgdecStatus.bufInfo.minOutBufSize[ii];
		  		outputBufDesc.descs[ii+1].bufSize =
          status.imgdecStatus.bufInfo.minOutBufSize[ii+1];
      }
     
      /* Optional: Set Run time parameters in the Algorithm via control()    */
      IIMGDEC1Fxns->control((IIMGDEC1_Handle)handle, XDM_SETPARAMS,
        (IIMGDEC1_DynamicParams *)&dynamicParams,
        (IIMGDEC1_Status *)&status);
      
      /* Cache Invalidate for Input Buffer                                   */
      for(ii=0 ; ii < inputBufDesc.numBufs ; ii++ )
      {
        /* Cache Invalidate for Input Buffer                                 */
        BCACHE_inv(inputBufDesc.descs[ii].buf, validBytes, TRUE);

      } 
	  
		

	  while(1)
	  {
#ifdef TIMER
		/* Timer value before start of decoding */
    	t0  = TSC_read();
#endif
		

      /* Basic Algorithm process() call                                      */
      retVal = IIMGDEC1Fxns->process((IIMGDEC1_Handle)handle,
        (XDM1_BufDesc *)&inputBufDesc,
        (XDM1_BufDesc *)&outputBufDesc,
        (IIMGDEC1_InArgs *)&inArgs,
        (IIMGDEC1_OutArgs *)&outArgs);

#ifdef TIMER
		/* Timer value at the end of encoding */
		t1  = TSC_read();   	   
#endif 

		if(dynamicParams.numMCU_row )
			{
				inputBufDesc.descs[0].buf += outArgs.imgdecOutArgs.bytesConsumed;
				inArgs.imgdecInArgs.numBytes -= outArgs.imgdecOutArgs.bytesConsumed;
			}

      /* Optional: Read status via control()                                 */
      IIMGDEC1Fxns->control((IIMGDEC1_Handle)handle, 
        XDM_GETSTATUS, 
        (IIMGDEC1_DynamicParams *)&dynamicParams, 
        (IIMGDEC1_Status *)&status);


		tslice +=(t1 - t0);
	   	decodetime = t1-t0;

		total_decodetime +=decodetime;

			 
			 if(status.end_of_seq  == 1)

	  {
		break;
	  }

		if(status.mode == 0) // sequential baseline
			{
				outputBufDesc.descs[0].buf += status.bytesgenerated[0];
				outputBufDesc.descs[1].buf += status.bytesgenerated[1];
				outputBufDesc.descs[2].buf += status.bytesgenerated[2];
			}
			else if((status.mode == 1)) // Non interlaeved sequential 
			{
				if(params.imgdecParams.forceChromaFormat!=8)
				{
					outputBufDesc.descs[0].buf += status.bytesgenerated[0];
					outputBufDesc.descs[1].buf += status.bytesgenerated[1];
					outputBufDesc.descs[2].buf += status.bytesgenerated[2];
				}
			}
			else if(status.mode != 2)
			{
				outputBufDesc.descs[0].buf += status.bytesgenerated[0];
				outputBufDesc.descs[1].buf += status.bytesgenerated[1];
				outputBufDesc.descs[2].buf += status.bytesgenerated[2];
			}

			if(status.end_of_scan)
			{

				outputBufDesc.descs[0].buf = store_outptr1; 
				outputBufDesc.descs[1].buf = store_outptr2; 
				outputBufDesc.descs[2].buf = store_outptr3;
				printf("\n Complete scan no. %d \n ", ScanCount);
				ScanCount++;			
				
			}
			

}
     
      /* Cache Writeback Invalidate for Output Buffers                       */
      if(outputBufDesc.descs[0].buf != NULL)
	  {
        for(ii=0 ; ii < outputBufDesc.numBufs ; ii++ )
        {
           BCACHE_wbInv(outputBufDesc.descs[ii].buf,
          outputBufDesc.descs[ii].bufSize, TRUE);

  
        }
	  }

      bytesConsumed          += outArgs.imgdecOutArgs.bytesConsumed;
 
      if(retVal == XDM_EFAIL)
      {
        printf("\n Process function returned an Error...  ");
        break; /* Error Condition: Application may want to break off         */
      }
      

#ifdef TIMER
   	   /* Time to decode*/
		Megapixel = (((float) (decodetime))/((float)(status.imgdecStatus.outputWidth*status.imgdecStatus.outputHeight)));
			
        printf("cycles taken                %d \n", decodetime);
		printf("cycles/pixel                : %f \n",
		(((float) (decodetime))/((float)(status.imgdecStatus.outputWidth*status.imgdecStatus.outputHeight))));
 		if(ScanCount == 0)
 		{       
			fprintf(freportFile,"\n I/P file ... %s \t ",inFile);
			fprintf(freportFile,"O/P file ... %s \n",testFile);
		}
        fprintf(freportFile,"cycles taken : %d \t", decodetime);
		fprintf(freportFile,"cycles/pixel : %f \t",Megapixel);
		fprintf(freportFile,"Megapixel 330: %f \t",(float)(330/Megapixel));
		fprintf(freportFile,"Megapixel 450: %f \n",(float)(450/Megapixel));
		fprintf(freportFile,"Megapixel 500: %f \t",(float)(500/Megapixel));
		

		fflush(freportFile);
		fflush(stdout);
#endif /*TIMER*/      
      /* DeActivate the Algorithm                                            */
      handle->fxns->algDeactivate(handle);

	  if(( outArgs.imgdecOutArgs.extendedError != JPEGDEC_SUCCESS))
	  {
			printf("\n Decoder ERROR %0x \n ",outArgs.imgdecOutArgs.extendedError);
			break;
	   }
         
      /* Check for frame ready via display buffer pointers                   */
      if(outputBufDesc.descs[0].buf != NULL)
      {
        printf("Decoded Frame # %d \n ",ScanCount);
		if(dynamicParams.progDisplay)
		{        
	        if(testCompliance)
	        {
	          /* Compare the output frames with the Reference File               */
	          testVal = TestApp_CompareOutputData(ftestFile, 
	            &outputBufDesc, 
	            (IIMGDEC1_OutArgs *)&outArgs);
	          
	          if(testVal == XDM_EOK)
	          {
	            printf("\n Decoder compliance test passed for this frame.  ");
	          }
	          else 
	          {
	            /* Test Compliance Failed... Breaking...                         */
	            break;
	          }
	        }
	        else
	        {
	          /* Write the output frames in the display order                    */
	          TestApp_WriteOutputData(ftestFile, &outputBufDesc, (IIMGDEC1_OutArgs *)&outArgs);
	        }
		} // dynamicParams.progDisplay
       
      }
      
 	  if(status.end_of_seq  == 1)
	  {
		break;
	  }

     
    }  while(1);  /* end of Do-While loop    
                                    */
		outputBufDesc.descs[0].buf = store_outptr1; 
		outputBufDesc.descs[1].buf = store_outptr2; 
		outputBufDesc.descs[2].buf = store_outptr3;

		IIMGDEC1Fxns->control((IIMGDEC1_Handle)handle,
        XDM_GETSTATUS,
        (IIMGDEC1_DynamicParams *)&dynamicParams,
        (IIMGDEC1_Status *)&status);
     
	 for(ii=0; ii<(status.imgdecStatus.bufInfo.minNumOutBufs);ii++ )
      {
        
        outputBufDesc.descs[ii].bufSize = 
          status.imgdecStatus.bufInfo.minOutBufSize[ii];
      }

		if(dynamicParams.numMCU_row )
	   total_decodetime = tslice;
	   tslice=0;

		Megapixel = (((float) (total_decodetime))/((float)(status.imgdecStatus.outputWidth*status.imgdecStatus.outputHeight)));
			
        printf("total cycles taken                %d \n", total_decodetime);
		printf("cycles/pixel                : %f \n\n",
		(((float) (total_decodetime))/((float)(status.imgdecStatus.outputWidth*status.imgdecStatus.outputHeight))));
        
        fprintf(freportFile,"\n total cycles taken : %d \t", total_decodetime);
		fprintf(freportFile,"total cycles/pixel : %f \t",Megapixel);
        
        total_decodetime = Megapixel/ScanCount;
        printf("Avg. cycles taken/pixel                %d \n", total_decodetime);
        fprintf(freportFile,"Avg. cycles taken/pixel                %d \n", total_decodetime);

		fprintf(freportFile,"Megapixel 330: %f \t",(float)(330/total_decodetime));
		fprintf(freportFile,"Megapixel 450: %f \n",(float)(450/total_decodetime));
		fprintf(freportFile,"Megapixel 500: %f \t",(float)(500/total_decodetime));
        fflush(freportFile);
		if(!dynamicParams.progDisplay)
		{        
	        if(testCompliance)
	        {
	          /* Compare the output frames with the Reference File               */
	          testVal = TestApp_CompareOutputData(ftestFile, 
	            &outputBufDesc, 
	            (IIMGDEC1_OutArgs *)&outArgs);
	          
	          if(testVal == XDM_EOK)
	          {
	            printf("\n Decoder compliance test passed for this frame.  ");
	          }
	          else 
	          {
	            /* Test Compliance Failed... Breaking...                         */
	            break;
	          }
	        }
	        else
	        {
	          /* Write the output frames in the display order                    */
	          TestApp_WriteOutputData(ftestFile, &outputBufDesc, (IIMGDEC1_OutArgs *)&outArgs);
	        }
		} // dynamicParams.progDisplay
		    
    /* Output file close                                                     */
    fclose(ftestFile); 
    if(multi_create)
	{
     /* Delete the Algorithm instance object specified by handle */
    ALG_delete (handle);
    }
    if(ScanCount == 0) 
    { 
      ScanCount = 1; /* To avoid division with zero */
    }
    
    printf("\n --------------  SUMMARY --------------------\n");
    if(!testCompliance) 
    {
      printf(" Decoder output dump completed\n");
    }
    printf("\t Total number of Scans              = %d\n",
      (XDAS_UInt32)ScanCount);
    printf("\t Width and Height                    = %d, %d \n",
      (XDAS_UInt32)status.imgdecStatus.outputWidth, (XDAS_UInt32)status.imgdecStatus.outputHeight);
    
    printf (" --------------    END   --------------------\n");    
  } /* Read the Config File until it reaches the end of file */

    if(multi_create == 0)
	{
     /* Delete the Algorithm instance object specified by handle */
    ALG_delete (handle);
    }  
  printf("\n End of execution\n");
  /* Close the config files */
  fclose(fConfigFile);
  fclose(freportFile);
 
  return XDM_EOK;
} /* main() */

  /*
  //============================================================================
  // TestApp_ReadByteStream
  //  Reading Byte Stream from a File
  */
  
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
  
  /*
  //============================================================================
  // TestApp_CompareOutputData
  //  Comparing Output Data with Reference File data
  */
  
  XDAS_Int32 TestApp_CompareOutputData(FILE *fRefFile,
    XDM1_BufDesc * outputBufDesc,
    IIMGDEC1_OutArgs *outArgs)
  {
    XDAS_Int32 i, bufferSize, retVal;
    XDAS_UInt8 *outputData;
    
    retVal = XDM_EOK;
    
    for(i = 0; i < outputBufDesc->numBufs; i++)
    {      
      outputData = (XDAS_UInt8 *)outputBufDesc->descs[i].buf;
      bufferSize = outputBufDesc->descs[i].bufSize;
      fread (refData, 1, bufferSize, fRefFile);
      if(memcmp(outputData, refData, bufferSize)) {
        printf( "\n Decoder compliance test failed for this frame. "); 
        retVal = XDM_EFAIL;
        break;
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
    XDM1_BufDesc * outputBufDesc,
    IIMGDEC1_OutArgs *outArgs)
  {
    
    XDAS_UInt8 *s;
    XDAS_Int32 i;    
    
    for(i = 0; i < outputBufDesc->numBufs; i++)
    {
      s = (XDAS_UInt8 *)outputBufDesc->descs[i].buf;
      fwrite (s, sizeof (Byte), outputBufDesc->descs[i].bufSize, fOutFile);
    }
    
    fflush (fOutFile);
    return;
  }
  
  
  /*
  //==============================================================================
  // TestApp_SetInitParams
  //  setting of creation time parameters
  */
  
  XDAS_Void TestApp_SetInitParams(IIMGDEC1_Params *params)
  {
    /* Set IIMGDEC parameters                                                   */
	
	/* Default Max Picture Width     */    
 //   params->maxWidth            = DEFAULT_WIDTH; 
    
    /* Default Max Picture Height      */
  //  params->maxHeight           = DEFAULT_HEIGHT;

    /* Default Max Scan */
//	params->maxScans			= DEFAULT_SCAN;  

     /* Data Endianness (0: Little, 1: Big) : Big Endian not supported */
    params->dataEndianness      = XDM_BYTE; 
    
     return;
  }
  
  /*
  //==============================================================================
  // TestApp_SetDynamicParams
  //  setting of run time parameters
  */
  
  XDAS_Void TestApp_SetDynamicParams(IIMGDEC1_DynamicParams *dynamicParams)
  {
    /* Set IIMGDEC Run time parameters */      
  

   /*Do header analysis first to get the decoded image size*/
	dynamicParams->decodeHeader = XDM_DECODE_AU;       
   
    return;
  }

#define EXT_MEM_BASE (0x80000000)
#define EXT_MEM_SIZE (0x10000000)
  
#ifdef DM644X  
#define EXT_MEM_BASE (0x80000000)
#define EXT_MEM_SIZE (0x10000000)
#endif

#ifdef OMAP3530  
#define EXT_MEM_BASE (0x80000000)
#define EXT_MEM_SIZE (0x10000000)
#endif

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


  
  /******************************************************************************/
  /*    Copyright (c) 2006 Texas Instruments, Incorporated                      */
  /*    All Rights Reserved                                                     */
  /******************************************************************************/
