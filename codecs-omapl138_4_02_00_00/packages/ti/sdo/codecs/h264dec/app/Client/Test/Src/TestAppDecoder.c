/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/*!
********************************************************************************
@file     TestAppDecoder.c
@brief    This is the top level client file that drives the H264
(Baseline Profile) Video Decoder Call using XDM Interface
@author   Multimedia Codecs TI India
@version  0.1 - Jan 24,2006    initial version
********************************************************************************
*/

//#define FRAMING_MODE
//#define COMPUTE_FRAME_LENGTH
//#define SUPPORT_PIC_REORDERING
//#define RUNTIME_RESOL_CHANGE

//For NAL type streams (equivalant to H264D_TI_NUM_BYTES_IN_NAL_ARR)
#define NUM_BYTES_IN_NAL_ARR  1200

/* Standard C header files */
#include <stdio.h>
#include <stdlib.h>

/* H264 Interface header files */
#include "h264vdec_ti.h"

/* Client header file */
#include "TestAppDecoder.h"

#include "buffermanager.h"
#ifndef TEST_VC_APP
/* CSL and DMAN3 header files */
#include <ti/bios/include/bcache.h>
#include <dman3.h>
#include "c6x.h" 

/* Input buffer allocation */
#pragma    DATA_SECTION(inputData, ".input_buffer")
#pragma    DATA_SECTION(inputData_NAL_buf, ".input_buffer")

/* Output buffer allocation */
/* Output buffer allocation */
#pragma    DATA_SECTION(refData, ".ref_buffer")
#endif

int H264VDEC_getFrameNALs(XDM1_BufDesc *Input,
						  XDAS_Int8 *inputData_NAL,
						  unsigned int bufferLen,
						  IVIDDEC2_DynamicParams *params);

int H264VDEC_getOneNAL(XDM1_BufDesc *Input,
					   XDAS_Int8 *inputData_NAL,
					   unsigned int bufferLen,
					   IVIDDEC2_DynamicParams *params);


XDAS_Int8   inputData[INPUT_BUFFER_SIZE];
XDAS_Int32  app_numBytesInNALarr[NUM_BYTES_IN_NAL_ARR];
XDAS_Int8   inputData_NAL_buf[INPUT_BUFFER_SIZE];
XDAS_Int8   getVersionBuff[64]; 

XDAS_Int8  refData[OUTPUT_BUFFER_SIZE];

#ifdef SUPPORT_PIC_REORDERING
unsigned int frm_out = 0;
#endif //SUPPORT_PIC_REORDERING

IH264VDEC_Params           params;
IH264VDEC_DynamicParams   dynamicParams;
XDAS_UInt32                framesToDecode;

sTokenMapping sTokenMap[] =
{
  {"FramesToDecode", &framesToDecode},
  {"ImageHeight",    &(params.viddecParams.maxHeight)},
  {"ImageWidth",     &(params.viddecParams.maxWidth)},
  {"ChromaFormat",   &(params.viddecParams.forceChromaFormat)},
  {"InputStreamFormat", &params.inputStreamFormat}, 
  {NULL,             NULL}     
};


XDAS_UInt32 frameCount;

#ifdef FRAMING_MODE

// for FM1_BT_B
XDAS_Int32 AU_sizes_array[] = { 4758,874,1018,846,958,888,965,795,692,809,864,722};

// for FM2_SVA_A
//XDAS_Int32 AU_sizes_array[] = {2600,500,666,488,622,540,532,550,519,467,397,535,471,434,263,505,633 };

#endif //FRAMING_MODE

#ifdef ENABLE_TRACE_BITS
FILE * bittrace_fp;
#endif /* ENABLE_TRACE_BITS */

/* Main Function acting as a client for Video Decode Call */
XDAS_Int32 main ()
{
  /* File I/O variables                                                       */
  FILE *fConfigFile, *ftestFile, *finFile, *fparamsFile;
  
  XDAS_Int8 line[STRING_SIZE], inFile[STRING_SIZE], testFile[STRING_SIZE];
  XDAS_Int8 paramsFile[STRING_SIZE], testCompliance;
  XDAS_Int8 *fname = "../../Test/TestVecs/Config/Testvecs.cfg";
  
  /* Input/Output  buffers  and their sizes                                   */
  XDAS_UInt8*  pOutputBuf [XDM_MAX_IO_BUFFERS];
  XDAS_Int32   outBufSize [XDM_MAX_IO_BUFFERS];
  XDAS_Int32   ii;
  XDAS_Int8    ret_val, first_time;
  
  IVIDDEC2_Fxns *ividDecFxns;
  
  //Extended class Structure
  IH264VDEC_Status           h264_status;

  /* Base Class Structures                                                    */
  IVIDDEC2_Status             *status = (IVIDDEC2_Status*)&h264_status;
  IH264VDEC_InArgs             inArgs;
  IH264VDEC_OutArgs            outArgs;
  
  /* Algorithm specific handle                                                */
  IALG_Handle handle;
  
  /* Input/Output Buffer Descriptors                                          */
  XDM1_BufDesc inputBufDesc;
  XDM_BufDesc  outputBufDesc;
    
  /* Input Buffer Management specific variables                               */
  XDAS_Int32 validBytes, bytesInAppInputBuffer, bytesConsumed; 
  
  /* Other variables                                                          */
  XDAS_UInt32 accessLength;
  XDAS_Int32  retVal, testVal, countConfigSet;  
#ifdef COMPUTE_FRAME_LENGTH
  XDAS_UInt32 accessLength;
#endif

//#define SKIP_SUPPORT_TEST
#ifdef SKIP_SUPPORT_TEST
    unsigned int temp_skip_ctr = 0;
    unsigned int temp_skip_flag = 0;
#endif

//#ifdef SEI_SUPPORT
    sSeiVuiParams_t sei_vui_buffer;
//#endif /*SEI_SUPPORT*/
    


#ifndef TEST_VC_APP
  /* DMAN3 specific variables                                                 */
  XDAS_Int32  groupId, numAlgs;
  IALG_Handle handleArray[NUM_ALGS];
  IDMA3_Fxns  *dmaFxns[NUM_ALGS];

  /* Timer Variables                                                          */
  XDAS_Int32 decodeTime, worstTime;
  XDAS_Int32 Time_start, Time_stop;
#else
  XDAS_Int32 Time_start, Time_stop;
#endif
  //Sreni 25 Apr 2006.
  XDAS_Int32 i;
#ifdef FRAMING_MODE
  XDAS_Int32 frame_num;
#endif //FRAMING_MODE
  /*-------------------------------------------------------------------------*/
  XDAS_Int8 *temp_input_buf_ptr;
  /*  Handle to a buffer element allocated by the buffer manager module      */
  /*-------------------------------------------------------------------------*/
  BUFFMGR_buffEleHandle buffEle;
#ifndef TEST_VC_APP
  /* Timer Enable */
  TSC_enable();

  /* Enable Cache Settings                                                    */
  TestApp_EnableCache();
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
    TestApp_SetInitParams(&params.viddecParams);
    
#ifndef TEST_VC_APP
    /* Initializing DMAN3 parameters                                          */
    /* Base Index Value from where DMA params are written                     */
    DMAN3_PARAMS.paRamBaseIndex  = 78; 
    /* Number of QDMA channels used by the Algorithm                          */
    DMAN3_PARAMS.numQdmaChannels = 4;  
    /* Number of Algorithms: System Parameter                                 */
    numAlgs                      = NUM_ALGS;  
    /* Group Id needed by DMAN3 library: System Parameter                     */
    groupId                      = 0;
    /* No of parameter ram entries   8+8+8+24                                 */
    DMAN3_PARAMS.numPaRamEntries = 48;
    DMAN3_PARAMS.numPaRamGroup[groupId] = 48;

#endif
    
    /* Setting the sizes of Base Class Objects                                */
    params.viddecParams.size               = sizeof(IH264VDEC_Params);
    status->size                           = sizeof(IH264VDEC_Status);
    dynamicParams.viddecDynamicParams.size = sizeof(IH264VDEC_DynamicParams);
    inArgs.viddecInArgs.size               = sizeof(IVIDDEC2_InArgs);
    outArgs.viddecOutArgs.size             = sizeof(IVIDDEC2_OutArgs);
    
    /* Initializing Variables                                                */
    frameCount                   = 0;
    /*-----------------------------------------------------------------------*/
    /*  The outBufsInUseFlag tells us whether the previous input buffer given*/
    /*  by the application to the algorithm is still in use or not. Since    */
    /*  this is before the first decode call, assign this flag to 0. The     */
    /*  algorithm will take care to initialize this flag appropriately from  */
    /*  hereon for the current configuration.                                */
    /*-----------------------------------------------------------------------*/
    outArgs.viddecOutArgs.outBufsInUseFlag = 0;

#ifdef SUPPORT_PIC_REORDERING
    frm_out = 0;
#endif /* SUPPORT_PIC_REORDERING */
    
    /* Read the bitstream in the Application Input Buffer                    */
    validBytes = bytesInAppInputBuffer = TestApp_ReadByteStream(finFile);
    fclose(finFile);

//Sreni 25Apr2006.
	for (i=0; i<20; i++)
	  inputData[validBytes+i] = 0;
    
    /* Return if there is an error in reading the file                       */
    if (validBytes < 0) /* Error condition                                   */
    {
      printf("Valid Bytes incorrect... Exiting for this configuration...\n");
      continue;
    }
    
    /* Create the Algorithm object (instance)                                */
    printf("\nCreating Algorithm Instance...");
    
    if ((handle =  (IALG_Handle)ALG_create ((const IALG_Fxns *) &H264VDEC_TI_IH264VDEC, 
                                            (IALG_Handle) NULL,
											(IALG_Params *) &params.viddecParams)) == NULL)
    {
      printf( "\nFailed to Create Instance... Exiting for this configuration..");
      continue;
    }
    printf("\nAlgorithm Instance Creation Done...\n");
    
#ifndef TEST_VC_APP
    /* DMA allocation using DMAN3 API's                                      */
    DMAN3_init();
    
    /* Provide the Algorithm handle to DMAN3 Interface for granting channels */
    handleArray[0] = (IALG_Handle) handle;
    dmaFxns[0]     = &H264VDEC_TI_IDMA3;
    
    if (DMAN3_grantDmaChannels(groupId, handleArray, dmaFxns, numAlgs) != DMAN3_SOK)
	{
      printf("\nProblem adding algorithm's dma resources");
      printf("... Exiting for this configuration... \n");
      continue;
    }
#endif
    
	/*-----------------------------------------------------------------------*/
    /*  The following function is used to initialize the dynamic params of   */
    /*  the IVIDDEC2 handle.                                                 */
    /*-----------------------------------------------------------------------*/
    TestApp_SetDynamicParams(&dynamicParams.viddecDynamicParams);
    
    /* set space for buffer descriptors                                      */
	//inputBufDesc.descs[0].buf   = inputData;  //XDM1.0
	temp_input_buf_ptr     = inputData;
    outputBufDesc.bufs     = (XDAS_Int8 **)pOutputBuf;
    outputBufDesc.bufSizes = (XDAS_Int32 *)outBufSize ;    

#ifdef SUPPORT_PIC_REORDERING
    /* Setting of input arguments                                          */
    inArgs.viddecInArgs.inputID         = 0;
#endif

	/*-----------------------------------------------------------------------*/
    /* Assigning Algorithm handle fxns field to ividDecfxns                  */
	/*-----------------------------------------------------------------------*/
    ividDecFxns = (IVIDDEC2_Fxns *)handle->fxns;
	/*------------------------------------------------------------------------ 
     * Optional: Getting lib version via control(),
	 * Application has to provide the buffer for getting the version info 
	 *-------------------------------------------------------------------------
	 */
	 status->data.buf = (XDAS_Int8 *)getVersionBuff;
     ividDecFxns->control((IVIDDEC2_Handle)handle,
		                   XDM_GETVERSION,
                           (IVIDDEC2_DynamicParams *)&dynamicParams,
                           (IVIDDEC2_Status *)status);


	/*------------------------------------------------------------------------ 
     Optional: Set Run time parameters in the Algorithm via control()   
	 -------------------------------------------------------------------------
	 */
      ividDecFxns->control((IVIDDEC2_Handle)handle,
		                   XDM_SETPARAMS,
                           (IVIDDEC2_DynamicParams *)&dynamicParams,
                           (IVIDDEC2_Status *)status);

	/*-----------------------------------------------------------------------*/
    /*  The GETBUFINFO call gives information for number of input and        */
    /*  output buffers. For the first call however, since the buffmgr_init() */
    /*  has already initialized the luma and chroma buffer handles, the      */
    /*  GETBUFINFO call will not return any sizes for outbufs.               */
    /*-----------------------------------------------------------------------*/
      ividDecFxns->control((IVIDDEC2_Handle)handle,
		                   XDM_GETBUFINFO,
                           (IVIDDEC2_DynamicParams *)&dynamicParams,
                           (IVIDDEC2_Status *)status);

	/*-----------------------------------------------------------------------*/
    /*  The buffermanager init function call allocates the memory for the    */
    /*  number of buffers of the corresponding sizes as passed in arguments  */
    /*  to it. This function initializes the output/reference bufs based on  */
    /*  the outBuf parameters returned from the GETBUFINFO call above. This  */
    /*  call will allocate each of the numOutbufs memory equal to (specified */
    /*  bufsize * MAX_BUFF_ELEMENTS). MAX_BUFF_ELEMENTS refers to the max    */
    /*  num of ref bufs required by algorithm. The initialization of buff    */
    /*  ptr will take place only for the first ref buffer but allocation     */
    /*  will happen for MAX_BUFF_ELEMENTS so that after the first decode     */
    /*  call, when actual height and width of picture are known, remaining   */
    /*  ref bufs can be initialised in the buffmgr_reinit function.          */
    /*  Note that the allocation of output/ref buffers will happen only once */
    /*  for the entire bunch of configuration sets provided in the testvecs  */
    /*  config file.                                                         */
    /*-----------------------------------------------------------------------*/
    retVal = BUFFMGR_Init(status->bufInfo.minNumOutBufs,
                           status->bufInfo.minOutBufSize);
    if(retVal)
    {
        fprintf (stdout,"\nMemory could not get allocated for output buffers\n");
        break;
    }


	/*---------------------------------------------------------------------*/
    /* Buffer pointers for the following are passed through inputBufDesc   */
	/* and are used inside the algorithm for storing respective data       */
	/* inputBufDesc.descs[0].buf = inputData                               */
	/* inputBufDesc.descs[1].buf = appMBErrStatBuf                         */
	/* inputBufDesc.descs[2].buf = app_numBytesInNALarr                    */
	/* inputBufDesc.descs[3].buf = &sei_vui_buffer;                        */
	/*---------------------------------------------------------------------*/

	/*---------------------------------------------------------------------*/
	/* Following pointers from algorithm are pased to the app through      */
	/* the outputBufDesc for its use                                       */
	/* inputBufDesc.descs[1].buf = appMBErrStatBuf                         */
    /* inputBufDesc.descs[3].buf = &sei_vui_buffer                         */
	/* Application can use the data using below pointers thru outputBufDesc*/
    /* outputBufDesc.bufs[4]     = inputBufDesc.descs[1].buf               */
    /* outputBufDesc.bufs[5]     = inputBufDesc.descs[3].buf               */
	/*---------------------------------------------------------------------*/
    inputBufDesc.numBufs     = status->bufInfo.minNumInBufs ;

	/*---------------------------------------------------------------------*/
    /*  For the num of input bufs, initialize the buffer pointer addresses */
    /*  and buffer sizes.                                                  */
    /*---------------------------------------------------------------------*/
    for(ii=0; ii<(status->bufInfo.minNumInBufs);ii++ )
    {
	   inputBufDesc.descs[ii].buf     = temp_input_buf_ptr;
	   temp_input_buf_ptr            += status->bufInfo.minInBufSize[ii];
	   inputBufDesc.descs[ii].bufSize = status->bufInfo.minInBufSize[ii];       
    }

	/* Initializing actual buffer addr to the inputBufDesc.descs[1].buf */
	if((params.inputStreamFormat == 1) && (status->bufInfo.minNumInBufs > 1))
	{
		inputBufDesc.descs[ii-1].buf = (XDAS_Int8*)app_numBytesInNALarr;
	}


    /*-----------------------------------------------------------------------*/
	/* Get the minimum number of output buffers                              */
	/*-----------------------------------------------------------------------*/
	outputBufDesc.numBufs     = status->bufInfo.minNumOutBufs;



    /*-----------------------------------------------------------------------*/
    /*  Resetting bytesConsumed variable to 0. This variable will contain    */
    /*  the total number of bytes consumed by the algorithm till now for the */
    /*  current configuration.                                               */
    /*-----------------------------------------------------------------------*/
    bytesConsumed = 0;

#ifndef TEST_VC_APP
    decodeTime = 0; worstTime = 0;
#endif

#ifdef ENABLE_TRACE_BITS
  if( (bittrace_fp = fopen ("bittrace.trs", "w")) == NULL)
  {
     fprintf(stderr, "Unable to open \"bittrace.trs\" file.\n"); exit(0);
  }
#endif /* ENABLE_TRACE_BITS */ 

#ifdef ENABLE_TRACE_RECON
    fclose (fopen ("recMBs_Deblock", "w"));
    fclose (fopen ("recMBs_Recon", "w"));
    fclose (fopen ("recMBs_PreIDCT", "w"));
    fclose (fopen ("recMBs_PostIDCT", "w"));
#endif

    /* The application must allocate buffer for holding 
	NAL lengths information and make the following pointer
	point to it */
  

	/*-----------------------------------------------------------------------*/
    /* Initailizing frame_num variable to 0                                  */
	/*-----------------------------------------------------------------------*/
#ifdef FRAMING_MODE
	frame_num = 0;
#endif //FRAMING_MODE


	first_time = 1;

	/*-----------------------------------------------------------------------*/
	/* Do-While Loop for Decode Call                                         */
	/*-----------------------------------------------------------------------*/
    do
    {
      /* Activate the Algorithm                                              */
      handle->fxns->algActivate(handle);
      

      inputBufDesc.descs[0].buf   = (XDAS_Int8 *)((XDAS_Int32) inputData + bytesConsumed);
/*
#ifdef COMPUTE_FRAME_LENGTH
	  // Find the Frame  length                                              
      accessLength = H264VDEC_getFrameSize(inputBufDesc.bufs[0], validBytes);

      // Assign the number of bytes available                                
      inArgs.numBytes        = accessLength; 
#else
      // Assign the number of bytes available                                
      inArgs.numBytes        = validBytes; 
#endif
	  
*/
  
#ifdef FRAMING_MODE

    accessLength =  AU_sizes_array[frame_num++];

	memcpy(inputData_NAL_buf, inputBufDesc.descs[0].buf, accessLength);

	inputBufDesc.descs[0].buf = inputData_NAL_buf;

	/* Assign the number of bytes available   */
    inArgs.viddecInArgs.numBytes        = accessLength; 

	  
#else
   if(params.inputStreamFormat == 1) //NALUnit Format
   {
	  /* Find the Frame  length                                              */
      /*accessLength = H264VDEC_getFrameNALs(inputBufDesc,
											 inputData_NAL_buf,
											 validBytes,
											 (IVIDDEC2_DynamicParams *)&dynamicParams);
      */
      accessLength = H264VDEC_getOneNAL((XDM1_BufDesc *)&inputBufDesc,
										inputData_NAL_buf,
										validBytes,
										(IVIDDEC2_DynamicParams *)&dynamicParams);

      if(dynamicParams.numNALunits == 0)
	     break;

	  inputBufDesc.descs[0].buf   = inputData_NAL_buf;

	  /* Assign the number of bytes available                                */
      inArgs.viddecInArgs.numBytes        = accessLength; 

   }
   else
   {
      /* Assign the number of bytes available                                */
      inArgs.viddecInArgs.numBytes        = validBytes; 
   }
#endif

      BCACHE_wbInvAll();

      /*---------------------------------------------------------------------*/
      /*  Check for the value of the outBufsInUseFlag. If the previous       */
      /*  decode call was for the first field of an interlaced frame, the    */
      /*  previous outbuf supplied by algorithm may still be required for    */
      /*  decoding second field. In that case, the algorithm will not need   */
      /*  a new buffer from the application. If above is not the case, call  */
      /*  the getfreebuffer() function of buffermanager to get the buffer    */
      /*  element to be supplied to algorithm for the next decode call.      */
      /*---------------------------------------------------------------------*/
      if (!outArgs.viddecOutArgs.outBufsInUseFlag)
      {
          buffEle = BUFFMGR_GetFreeBuffer();
          if (buffEle == NULL)
          {
              fprintf (stdout,"\nNo free buffer available\n");
              break;
          }
      }

      /*---------------------------------------------------------------------*/
      /*  Copy the output buffer size values and pointers from the buffer    */
      /*  element returned by getfreebuffer() function. Initialize the       */
      /*  numBufs to 3 as numoutbufs is always 3 for luma and chroma buffers.*/
      /*---------------------------------------------------------------------*/

	  /*------------------------------------------------------------------------
	  * Following outputBufDesc locations are used to dump MB error status
	  * data and SEI_0VUI info from the algorithm to the application. For 420
      * outputBufDesc.bufs[3] = mbErrorBufPtr;
	  * outputBufDesc.bufs[4] = SeiVui_buffer_ptr;
	  * For YUV422 format it should be passed as below
	  * outputBufDesc.bufs[1] = mbErrorBufPtr;
	  * outputBufDesc.bufs[2] = SeiVui_buffer_ptr;
      *------------------------------------------------------------------------
	  */
      //outputBufDesc.bufSizes    = (XDAS_Int32 *)buffEle->bufSize;
      //outputBufDesc.bufs        = (XDAS_Int8 **)buffEle->buf;    

	  for(ii=0; ii<(status->bufInfo.minNumOutBufs);ii++ )
	  {
	     outputBufDesc.bufSizes[ii] = buffEle->bufSize[ii];
		 outputBufDesc.bufs[ii]     = (XDAS_Int8 *)buffEle->buf[ii];
	  }
	  /* Initialing with SEI_VUI str pointer */
	  if((status->bufInfo.minNumOutBufs > 1) && (dynamicParams.Sei_Vui_parse_flag == TRUE))
	  {
		  /* Store the str pointer */
	      outputBufDesc.bufs[ii-1] = (XDAS_Int8 *)&sei_vui_buffer;
	  }



	  /*---------------------------------------------------------------------*/
      /*  Initialize the input ID in input arguments to the bufferid of the  */
      /*  buffer element returned from getfreebuffer() function.             */
      /*---------------------------------------------------------------------*/
      inArgs.viddecInArgs.inputID  = buffEle->bufId;
      

      /* Optional: Set Run time parameters in the Algorithm via control()    */
      ividDecFxns->control((IVIDDEC2_Handle)handle,
		                   XDM_SETPARAMS, 
                           (IVIDDEC2_DynamicParams *)&dynamicParams,
                           (IVIDDEC2_Status *)status);
    
#ifndef TEST_VC_APP

	  if(params.inputStreamFormat == 0)
	  {
		  /* Cache Invalidate for Input Buffer                                   */
		  for(ii=0 ; ii < inputBufDesc.numBufs ; ii++ )
		  {
			/* Cache Invalidate for Input Buffer                                 */
			//BCACHE_inv(inputBufDesc.bufs[ii], validBytes, TRUE);
		  }  
	  }

      /* Timer value read                                                    */
      Time_start  = TSC_read();

	  /* Dummy function for supersim profiling: start address                */
	  beginFrame(); 

#endif



#ifdef SKIP_SUPPORT_TEST

      /*Disabling and enabling skipping every 32 frames once for testing*/
      if(!(temp_skip_ctr & 0x01F))
      {
          if(temp_skip_flag)
                dynamicParams.viddecDynamicParams.frameSkipMode = IVIDEO_NO_SKIP;
          else
                dynamicParams.viddecDynamicParams.frameSkipMode = IVIDEO_SKIP_B;
          
          H264MPVDEC_control (h264vdec, XDM_SETPARAMS,&dynamicParams, &h264vdecstatus);     

          temp_skip_flag ^= 0x01;

      }

      temp_skip_ctr ++;

#endif /* SKIP_SUPPORT_TEST */


      /* Basic Algorithm process() call                                      */
      /*---------------------------------------------------------------------*/
      /*  Start the decode process for one frame/field by calling process    */
      /*  function.                                                          */
      /*---------------------------------------------------------------------*/
      retVal = ividDecFxns->process((IVIDDEC2_Handle)handle,
                                    (XDM1_BufDesc *)&inputBufDesc,
                                    (XDM_BufDesc *)&outputBufDesc,
                                    (IVIDDEC2_InArgs *)&inArgs.viddecInArgs,
                                    (IVIDDEC2_OutArgs *)&outArgs);

      
#ifndef TEST_VC_APP
	  /* Dummy function for supersim profiling: start address                */
	  endFrame(); 

      /* Timer value read                                                    */
      Time_stop  = TSC_read();
      decodeTime += (Time_stop-Time_start);
	  if(worstTime<(Time_stop-Time_start)) worstTime = (Time_stop-Time_start);

      /* Cache Writeback Invalidate for Output Buffers                       */
      if(outArgs.viddecOutArgs.displayBufs->bufDesc[0].buf != NULL)
	  {
        for(ii=0 ; ii < outArgs.viddecOutArgs.displayBufs->numBufs; ii++ )
        {
		  /*
          BCACHE_wbInv(outArgs.viddecOutArgs.displayBufs.bufs[ii], 
          outArgs.viddecOutArgs.displayBufs.bufSizes[ii], TRUE);
		  */
        }
	  }
#endif /* TEST_VC_APP */


	  /* This check is required in this testApp becos
	  we are trying to run NAL input format flow using a 
	  byte Stream format input. So "accessLength" and
	  "outArgs.bytesConsumed" are different. But this is
	  always "outArgs.bytesConsumed" in the actual application.*/

#ifdef FRAMING_MODE
	  bytesConsumed          += accessLength;
      validBytes             -= accessLength;
#else
      if(params.inputStreamFormat == 1) //NALUnit Format
	  {
		  bytesConsumed          += accessLength;
          validBytes             -= accessLength;
	  }
	  else
	  {
		  bytesConsumed          += outArgs.viddecOutArgs.bytesConsumed;
          validBytes             -= outArgs.viddecOutArgs.bytesConsumed;
	  }
#endif

      ///* Write the output frames in the display order                    */
     // TestApp_WriteOutputData(ftestFile, &outputBufDesc, &outArgs); //HARMEET
      //exit(-1);

      if(retVal == XDM_EFAIL)
      {
        printf("\n Process function returned an Error...  ");
        //break; /* Error Condition: Application may want to break off       */
      }
      
      /* Optional: Read status via control()                                 */
      ividDecFxns->control((IVIDDEC2_Handle)handle,
                           XDM_GETSTATUS, 
                           (IVIDDEC2_DynamicParams *)&dynamicParams,
                           (IVIDDEC2_Status *)status);
      
      /* DeActivate the Algorithm                                            */
      handle->fxns->algDeactivate(handle);

	  /*---------------------------------------------------------------------*/
      /*  If this is the first time in the stream, reinitialize all the      */
      /*  buffers based on the picture width and height.                     */
      /*---------------------------------------------------------------------*/
	  if(first_time)
	  {

		  /*-----------------------------------------------------------------*/
          /*  The GETBUFINFO call will give information for number of        */
          /*  output buffers. This call is after the first decode call hence */
          /*  the outputbuffer sizes returned by this call is calculated     */
          /*  based on the actual pic width and height. Hence, there is need */
          /*  for buffer reinitializations which happens immediately after   */
          /*  this call.                                                     */
          /*-----------------------------------------------------------------*/
		  ividDecFxns->control((IVIDDEC2_Handle)handle,
		                       XDM_GETBUFINFO,
                               (IVIDDEC2_DynamicParams *)&dynamicParams,
                               (IVIDDEC2_Status *)status);

          /*-----------------------------------------------------------------*/
          /*  Reinitialize the buffers using the actual buffer sizes for     */
          /*  luma and chroma. This call will initialize the buf ptrs for    */
          /*  all MAX_BUFF_ELEMENTS (max ref buffers required) unlike the    */
          /*  first call which had initialized only buffer ptrs for first    */
          /*  ref buffers.                                                   */
          /*-----------------------------------------------------------------*/
          ret_val = BUFFMGR_ReInit(status->maxNumDisplayBufs,
                                   status->bufInfo.minNumOutBufs,
                                   status->bufInfo.minOutBufSize);

          if (ret_val)
          {
              fprintf (stdout,"\nMemory could not get allocated for output buffers\n");
              break;
          }

          /*-----------------------------------------------------------------*/
          /*  Reset the flag first_time to 0 since the buffers are correctly */
          /*  reinitialized.                                                 */
          /*-----------------------------------------------------------------*/
          first_time = 0;

	  }

#ifndef SUPPORT_PIC_REORDERING      
      /* Check for frame ready via display buffer pointers                   */
	  if(outArgs.viddecOutArgs.displayBufs->bufDesc[0].buf != NULL)
      {

        printf("\n Decoded Frame # %d  ",frameCount);
        printf("\tDecode Time = %d",(XDAS_UInt32)(Time_stop-Time_start));
        
		if(testCompliance)
        {
          /* Compare the output frames with the Reference File               */
          testVal = TestApp_CompareOutputData(ftestFile, 
                                              &outputBufDesc, 
                                              &outArgs.viddecOutArgs);
          
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
           TestApp_WriteOutputData(ftestFile, &outputBufDesc, &outArgs.viddecOutArgs);
        }

#ifndef TEST_VC_APP
        /* Cache Writeback Invalidate for Output Buffers                       */
        for(ii=0 ; ii < outArgs.viddecOutArgs.displayBufs->numBufs ; ii++ )
        {
		  /*
          BCACHE_inv(outArgs.viddecOutArgs.displayBufs.bufs[ii], 
          outArgs.viddecOutArgs.displayBufs.bufSizes[ii], TRUE);
		  */
        }
#endif /* TEST_VC_APP */


		//if(params.inputStreamFormat == 0)
			frameCount++;
      }
#endif /* SUPPORT_PIC_REORDERING */

#ifdef SUPPORT_PIC_REORDERING     

      printf("\n Decoded Frame # %d  ",frameCount);
      printf("\tDecode Time = %d",(XDAS_UInt32)(Time_stop-Time_start));
      frameCount++;

      if(outArgs.viddecOutArgs.displayBufs->bufDesc[0].buf != NULL)
      {
        printf("\n Displaying Frame # %d  ",frm_out);
        frm_out++;

        if(testCompliance)
        {
          /* Compare the output frames with the Reference File               */
          testVal = TestApp_CompareOutputData(ftestFile, 
            &outputBufDesc, 
            &outArgs.viddecOutArgs);
          
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
           TestApp_WriteOutputData(ftestFile, &outputBufDesc, &outArgs.viddecOutArgs);
        }

#ifndef TEST_VC_APP
        /* Cache Writeback Invalidate for Output Buffers                       */
        for(ii=0 ; ii < outArgs.viddecOutArgs.displayBufs->numBufs ; ii++ )
        {
		  /*
          BCACHE_inv(outArgs.viddecOutArgs.displayBufs.bufs[ii], 
          outArgs.viddecOutArgs.displayBufs.bufSizes[ii], TRUE);
		  */
        }
#endif /* TEST_VC_APP */

      
      }

#endif /* SUPPORT_PIC_REORDERING */
      

      /* Check for Break off condition                                       */
      if (bytesConsumed >= bytesInAppInputBuffer)
      {
		/*---------------------------------------------------------------------*/
		/*  Always release buffers - which are released from the algorithm     */
		/*  side   -back to the buffer manager. The freebufID array of outargs */
		/*  contains the sequence of bufferIds which need to be freed. This    */
		/*  gets populated by the algorithm. The following function will do    */
		/*  the job of freeing up the buffers.                                 */
		/*---------------------------------------------------------------------*/
        BUFFMGR_ReleaseBuffer((XDAS_UInt32 *)outArgs.viddecOutArgs.freeBufID);
        printf("\n Bitstream Ended...  ");

#ifdef SUPPORT_PIC_REORDERING
        ividDecFxns->control((IVIDDEC2_Handle)handle,
                             IH264VDEC_FLUSH,
                             (IVIDDEC2_DynamicParams *)&dynamicParams,
                             (IVIDDEC2_Status *)status);

        ividDecFxns->process((IVIDDEC2_Handle)handle,
                             (XDM1_BufDesc *)&inputBufDesc,
                             (XDM_BufDesc *)&outputBufDesc,
                             (IVIDDEC2_InArgs *)&inArgs.viddecInArgs,
                             (IVIDDEC2_OutArgs *)&outArgs);

        while(outArgs.viddecOutArgs.displayBufs->bufDesc[0].buf != NULL)
        {

#ifndef TEST_VC_APP
          /* Cache Writeback Invalidate for Output Buffers                       */
          for(ii=0 ; ii < outArgs.viddecOutArgs.displayBufs->numBufs; ii++ )
          {
		    /*
            BCACHE_wbInv(outArgs.viddecOutArgs.displayBufs.bufs[ii], 
            outArgs.viddecOutArgs.displayBufs.bufSizes[ii], TRUE);
			*/
          }
#endif /* TEST_VC_APP */

          printf("\n Displaying Frame # %d  ",frm_out);
          frm_out++;

          if(testCompliance)
          {
            /* Compare the output frames with the Reference File               */
            testVal = TestApp_CompareOutputData(ftestFile, 
              &outputBufDesc, 
              &outArgs.viddecOutArgs);
          
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
            TestApp_WriteOutputData(ftestFile, &outputBufDesc, &outArgs.viddecOutArgs);
          }

		  /*---------------------------------------------------------------------*/
		  /*  Always release buffers - which are released from the algorithm     */
		  /*  side   -back to the buffer manager. The freebufID array of outargs */
		  /*  contains the sequence of bufferIds which need to be freed. This    */
		  /*  gets populated by the algorithm. The following function will do    */
		  /*  the job of freeing up the buffers.                                 */
		  /*---------------------------------------------------------------------*/
          BUFFMGR_ReleaseBuffer((XDAS_UInt32 *)outArgs.viddecOutArgs.freeBufID);
#ifndef TEST_VC_APP
          /* Cache Writeback Invalidate for Output Buffers                       */
          for(ii=0 ; ii < outArgs.viddecOutArgs.displayBufs->numBufs; ii++ )
          {
		    /*
            BCACHE_inv(outArgs.viddecOutArgs.displayBufs.bufs[ii], 
            outArgs.viddecOutArgs.displayBufs.bufSizes[ii], TRUE);
			*/
          }
#endif /* TEST_VC_APP */


          ividDecFxns->control((IVIDDEC2_Handle)handle,
                               IH264VDEC_FLUSH,
                               (IVIDDEC2_DynamicParams *)&dynamicParams,
                               (IVIDDEC2_Status *)status);

          ividDecFxns->process((IVIDDEC2_Handle)handle,
                               (XDM1_BufDesc *)&inputBufDesc,
                               (XDM_BufDesc *)&outputBufDesc,
                               (IVIDDEC2_InArgs *)&inArgs.viddecInArgs,
                               (IVIDDEC2_OutArgs *)&outArgs);
        }

#endif /* SUPPORT_PIC_REORDERING */

        break; /* Bitstream ended: Break off the while loop                  */
      }

      if (frameCount >= framesToDecode)
      {
        printf("\n Specified number of Frames Decoded...  ");
		/*---------------------------------------------------------------------*/
		/*  Always release buffers - which are released from the algorithm     */
		/*  side   -back to the buffer manager. The freebufID array of outargs */
		/*  contains the sequence of bufferIds which need to be freed. This    */
		/*  gets populated by the algorithm. The following function will do    */
		/*  the job of freeing up the buffers.                                 */
		/*---------------------------------------------------------------------*/
        BUFFMGR_ReleaseBuffer((XDAS_UInt32 *)outArgs.viddecOutArgs.freeBufID);

#ifdef SUPPORT_PIC_REORDERING
        ividDecFxns->control((IVIDDEC2_Handle)handle,
                             IH264VDEC_FLUSH,
                             (IVIDDEC2_DynamicParams *)&dynamicParams,
                             (IVIDDEC2_Status *)status);

        ividDecFxns->process((IVIDDEC2_Handle)handle,
                             (XDM1_BufDesc *)&inputBufDesc,
                             (XDM_BufDesc *)&outputBufDesc,
                             (IVIDDEC2_InArgs *)&inArgs.viddecInArgs,
                             (IVIDDEC2_OutArgs *)&outArgs);

        while(outArgs.viddecOutArgs.displayBufs->bufDesc[0].buf != NULL)
        {

#ifndef TEST_VC_APP
          /* Cache Writeback Invalidate for Output Buffers                       */
          for(ii=0 ; ii < outArgs.viddecOutArgs.displayBufs->numBufs; ii++ )
          {
		    /*
            BCACHE_wbInv(outArgs.viddecOutArgs.displayBufs.bufs[ii], 
            outArgs.viddecOutArgs.displayBufs.bufSizes[ii], TRUE);
			*/
          }
#endif /* TEST_VC_APP */

          printf("\n Displaying Frame # %d  ",frm_out);
          frm_out++;

          if(testCompliance)
          {
            /* Compare the output frames with the Reference File               */
            testVal = TestApp_CompareOutputData(ftestFile, 
              &outputBufDesc, 
              &outArgs.viddecOutArgs);
          
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
            TestApp_WriteOutputData(ftestFile, &outputBufDesc, &outArgs.viddecOutArgs);
          }

		  /*---------------------------------------------------------------------*/
		  /*  Always release buffers - which are released from the algorithm     */
		  /*  side   -back to the buffer manager. The freebufID array of outargs */
		  /*  contains the sequence of bufferIds which need to be freed. This    */
		  /*  gets populated by the algorithm. The following function will do    */
		  /*  the job of freeing up the buffers.                                 */
		  /*---------------------------------------------------------------------*/
          BUFFMGR_ReleaseBuffer((XDAS_UInt32 *)outArgs.viddecOutArgs.freeBufID);
#ifndef TEST_VC_APP
          /* Cache Writeback Invalidate for Output Buffers                       */
          for(ii=0 ; ii < outArgs.viddecOutArgs.displayBufs->numBufs; ii++ )
          {
		    /*
            BCACHE_inv(outArgs.viddecOutArgs.displayBufs.bufs[ii], 
            outArgs.viddecOutArgs.displayBufs.bufSizes[ii], TRUE);
			*/
          }
#endif /* TEST_VC_APP */


          ividDecFxns->control((IVIDDEC2_Handle)handle,
                               IH264VDEC_FLUSH,
                               (IVIDDEC2_DynamicParams *)&dynamicParams,
                               (IVIDDEC2_Status *)status);

          ividDecFxns->process((IVIDDEC2_Handle)handle,
                                (XDM1_BufDesc *)&inputBufDesc,
                                (XDM_BufDesc *)&outputBufDesc,
                                (IVIDDEC2_InArgs *)&inArgs.viddecInArgs,
                                (IVIDDEC2_OutArgs *)&outArgs);
        }

#endif /* SUPPORT_PIC_REORDERING */

        break; /* Bitstream ended: Break off the while loop                  */
      }  
	  
	  /*---------------------------------------------------------------------*/
      /*  Always release buffers - which are released from the algorithm     */
      /*  side   -back to the buffer manager. The freebufID array of outargs */
      /*  contains the sequence of bufferIds which need to be freed. This    */
      /*  gets populated by the algorithm. The following function will do    */
      /*  the job of freeing up the buffers.                                 */
      /*---------------------------------------------------------------------*/
      BUFFMGR_ReleaseBuffer((XDAS_UInt32 *)outArgs.viddecOutArgs.freeBufID);

    }  while(1);  /* end of Do-While loop                                    */
    
    /* Output file close                                                     */
    fclose(ftestFile);

#ifdef ENABLE_TRACE_BITS
    fprintf(bittrace_fp, "*** Frame End ***\n\n");
    fflush(bittrace_fp);
#endif /* ENABLE_TRACE_BITS */

#ifdef ENABLE_TRACE_BITS
    fclose(bittrace_fp);
#endif /* ENABLE_TRACE_BITS */      
    
#ifndef TEST_VC_APP
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
#endif
    
	/*-----------------------------------------------------------------------*/
    /*  The algorithm currently does not guarantee to set freebufid of       */
    /*  all buffers in use when the framecount exceeds framestoDecode.       */
    /*  This is managed for now by an application function. Note that        */
    /*  doing this has significance only when running multiple streams.      */
    /*-----------------------------------------------------------------------*/
    BUFFMGR_ReleaseAllBuffers();

	/*-------------------------------------------------------------------------*/
    /*  Free the buffers allocated for output/reference for all the streams    */
    /*  that have been executed.                                               */
    /*-------------------------------------------------------------------------*/
    BUFFMGR_DeInit(status->maxNumDisplayBufs,outputBufDesc.numBufs);
    /* Delete the Algorithm instance object specified by handle */
    ALG_delete (handle);
    
    if(frameCount == 0) 
    { 
      frameCount = 1; /* To avoid division with zero */
    }
    
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
      (XDAS_UInt32)status->outputWidth, (XDAS_UInt32)status->outputHeight);
    
#ifndef TEST_VC_APP
	printf("\t Total Decode Cycles                 = %d\n",
	  decodeTime);  
	printf("\t Average Decode Cycles per Frame     = %d\n",
	  (XDAS_UInt32)(decodeTime/frameCount));
	printf("\t Worst Case Decode Cycles per Frame  = %d\n",
	  (XDAS_UInt32)worstTime);
	printf("\t Average MCPS for 30 Frames/Sec      = %d\n",
	  (XDAS_UInt32)(1+((30*(decodeTime/frameCount))/1000000)));
	printf("\t Worst MCPS for 30 Frames/Sec        = %d\n",
	  (XDAS_UInt32)(1+(30*worstTime)/1000000));
#endif    

    printf (" --------------    END   --------------------\n");    
  } /* Read the Config File until it reaches the end of file */
  
  printf("\n End of execution\n");
  /* Close the config files */
  fclose(fConfigFile);
  
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

#ifndef TEST_VC_APP
    /* Cache Invalidate for Input Buffer */
    //BCACHE_wbInv(inputData, BufferSize, TRUE);
#endif
    
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
                                       XDM_BufDesc * outputBufDesc, 
                                       IVIDDEC2_OutArgs *outArgs)
  {
    XDAS_Int32 i, bufferSize, retVal;
    XDAS_UInt8 *outputData;
    
    retVal = XDM_EOK;
    
    for(i = 0; i < outArgs->displayBufs->numBufs; i++)
    {      
      outputData = (XDAS_UInt8 *)outArgs->displayBufs->bufDesc[i].buf;
	  bufferSize = outArgs->displayBufs->bufDesc[i].bufSize;
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
                                    XDM_BufDesc * outputBufDesc, 
                                    IVIDDEC2_OutArgs *outArgs)
  {
    
    XDAS_UInt8 *s;
    XDAS_Int32 i;    
    
    for(i = 0; i < outArgs->displayBufs->numBufs; i++)
    {      
	  s = (XDAS_UInt8 *)outArgs->displayBufs->bufDesc[i].buf;
      fwrite (s, sizeof (Byte), outArgs->displayBufs->bufDesc[i].bufSize, fOutFile);
    }
    
    fflush (fOutFile);
    return;
  }
  
  
  /*
  //==============================================================================
  // TestApp_SetInitParams
  //  setting of creation time parameters
  */
  
  XDAS_Void TestApp_SetInitParams(IVIDDEC2_Params *params)  
  {
	  IH264VDEC_Params *extParams = (IH264VDEC_Params *)params;
    /* Set IVIDDEC parameters                                                   */
    
    /* Max Frame Rate: Not currently used in the algorithm                      */
      params->maxFrameRate    = 30000;

    /* Max Bit Rate: Not currently used in the algorithm                        */
      params->maxBitRate      = 10485760;

    /* Data Endianness (0: Little, 1: Big) :                                    */
    params->dataEndianness  = XDM_BYTE;	

#ifdef SUPPORT_PIC_REORDERING
	  /* Must be set between 0 and 16 for successful decoding */
	  extParams->maxDisplayDelay = 0;
#else
	  extParams->maxDisplayDelay = 0;
#endif //SUPPORT_PIC_REORDERING
    return;
  }
  
  /*
  //==============================================================================
  // TestApp_SetDynamicParams
  //  setting of run time parameters
  */
  
  XDAS_Void TestApp_SetDynamicParams(IVIDDEC2_DynamicParams *dynamicParams)
  {
	  IH264VDEC_DynamicParams *extParams = (IH264VDEC_DynamicParams *)dynamicParams;
    /* Set IVIDDEC Run time parameters */
    dynamicParams->decodeHeader  = XDM_DECODE_AU; 
	//dynamicParams->decodeHeader  = XDM_PARSE_HEADER;  


    /*-----------------------------------------------------
    * If Display width is set to 0, decoder takes value 
    * of image width as Display width. Any non-zero value
    * will be honoured by the decoder. 
    * Note: Displaywidth needs to be an even number.
    * 
    -----------------------------------------------------*/

#ifdef DISPLAY_WIDTH_CHANGES
    dynamicParams->displayWidth  = 0;             //Supported now.
#else
    dynamicParams->displayWidth  = 0;             //Not Supported: Set to default value
#endif
    dynamicParams->frameSkipMode = IVIDEO_NO_SKIP;//Not Supported: Set to default value
    
	  dynamicParams->frameOrder    = IVIDDEC2_DISPLAY_ORDER;

	  dynamicParams->newFrameFlag  = XDAS_FALSE;

	  dynamicParams->mbDataFlag    = XDAS_FALSE;

	  /*---------------------------------------------------------------------*/
      /* MB_ERROR_STAT                                                       */
	  /* The global buffer size is passed to algorithm,                      */
	  /* The error status for each MB of the frame is copied to the buffer,  */
	  /* returned as output bufDesc[4]    .                                  */
	  /* Init the flag with TRUE to get these datas                          */
	  /*---------------------------------------------------------------------*/
      extParams->mbErrorBufFlag  = FALSE; //TRUE;
      extParams->mbErrorBufSize  = ((IMAGE_WIDTH * IMAGE_HEIGHT)/256);

	  /*---------------------------------------------------------------------*/
	  /* Collecting the status of SEI and VUI structure and returned the     */
	  /* pointer through output bufDesc[5]                                   */
	  /* Init the flag with TRUE to get these datas                          */
	  /*---------------------------------------------------------------------*/
      extParams->Sei_Vui_parse_flag = FALSE; //TRUE;

	  /* Should be set to zero for byte stream format */
	  extParams->numNALunits = 0;
    return;
  }
  
  
#ifndef TEST_VC_APP
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
    BCACHE_setMar((Ptr *)0x80000000, 0x10000000, BCACHE_MAR_ENABLE);      
    
    BCACHE_wbInvAll(); /* No need to call BCACHE_wait() */
  } /* TestApp_EnableCache */
  
void Interrupt_start(void)
{
	*((unsigned int *)0x01C21420) = 0x0;
	*((unsigned int *)0x01C21424) = 0x0;
	*((unsigned int *)0x01C21424) = 0x0007;
    *((unsigned int *)0x01C2141C) = 8000000;//count
    *((unsigned int *)0x01C21414) = 0x0;
	*((unsigned int *)0x01C21404) = 0x1;
	*((unsigned int *)0x01C21420) = 0x800000;
} 
void beginFrame (void)
{
  asm ("*profiling begin");
}

void endFrame (void)
{
  asm ("*profiling end");
}

#endif






/*
//============================================================================
// H264VDEC_getFrameSize 
*/
/*!  
 ************************************************************************************* 
 * \brief 
 *  Find the frame length
 * \return 
 * number of bytes
 ************************************************************************************* 
 */

#define     NALU_TYPE_SEI   6      //!<  6: SEI
#define 	NALU_TYPE_SPS   7      //!<  7: SPS
#define 	NALU_TYPE_PPS   8      //!<  8: PPS
#define     NALU_TYPE_AUD   9      //!<  9: AUD
#define     NALU_TYPE_FILL 12      //!< 12: filler data

#ifdef TEST_VC_APP
#define _mem4(ptr)        ( *(unsigned int *)(ptr))
#endif

//The following ensures that atleast one-complete frame is present in the buffer
int H264VDEC_getFrameNALs(XDM1_BufDesc *Input,
						  XDAS_Int8 *inputData_NAL,
						  unsigned int bufferLen,
						  IVIDDEC2_DynamicParams *params)
{
	XDAS_Int8 *stream;
    XDAS_UInt32 byteCount,nalLength,frame_length,NALidx = 0;

    XDAS_UInt8 currNalType,firstMbInSliceFlag,nextNalType;
    XDAS_UInt8 vcl_data_present = 0;
	XDAS_UInt32 byte_pos = 0;
    XDAS_Int32 *numNalArr;

	IH264VDEC_DynamicParams *dynamicParams = (IH264VDEC_DynamicParams *)params;
	numNalArr = (XDAS_Int32*)Input->descs[1].buf;    
	stream = Input->descs[0].buf;
    nalLength    = 0;
	frame_length = 0;
	currNalType  = stream[4] & 0x1f;
	nextNalType  = currNalType;

    for (byteCount = 4; byteCount < bufferLen; byteCount++)
	{
        if (stream[byteCount] == 0 && stream[byteCount+1] == 0 && stream[byteCount+2] == 0 && stream[byteCount+3] == 0x01)
		{			
			//Input->descs[1].buf[NALidx++] = nalLength;
			numNalArr[NALidx++] = nalLength;
			currNalType = nextNalType;
			
            if ((currNalType !=  NALU_TYPE_SPS) && 
				(currNalType !=  NALU_TYPE_PPS)&&
				(currNalType !=  NALU_TYPE_AUD)&&
				(currNalType !=  NALU_TYPE_SEI)&&
				(currNalType !=  NALU_TYPE_FILL)
				) 
			{
				//vcl-data
      			vcl_data_present = 1;				
            }		

			frame_length += 4;
			stream += 3;	
			nalLength = 0;

			//for the next NAL
			nextNalType  = stream[byteCount + 1] & 0x1f;

			if(((nextNalType ==  NALU_TYPE_SPS)|| 
				(nextNalType ==  NALU_TYPE_PPS)||
				(nextNalType ==  NALU_TYPE_AUD)||
				(nextNalType ==  NALU_TYPE_SEI)||
				(nextNalType ==  NALU_TYPE_FILL)) && (vcl_data_present)) 
			{
				//non-vcl-data
				//frameCount++;
                break;
			}
			else
			{
				//vcl-data
	            firstMbInSliceFlag = stream[byteCount+2] & 0x80;
				if (firstMbInSliceFlag && vcl_data_present)
				{
					//frameCount++;
                    break;
				}
			}			
        }
		else
		{
           inputData_NAL[byte_pos++] = stream[byteCount];
		   nalLength++;
		   frame_length++;
		}    		
    }

	if(byteCount == bufferLen)
	{
		//Input->descs[1].buf[NALidx++] = nalLength;
		numNalArr[NALidx++] = nalLength;
		frame_length++;
		//frameCount++;
	}

	dynamicParams->numNALunits = NALidx;    
	return frame_length; 
}



#if 0
//The following accumulates data for 1 SPS or 1PPS or
// an entire Frame in case of VCL NAL unit. But this 
// logic holds only for Non-ASO/Non-FMO streams 
int H264VDEC_getFrameNALs(XDM1_BufDesc *Input,
						  XDAS_Int8 *inputData_NAL,
						  unsigned int bufferLen,
						  IVIDDEC2_DynamicParams *params)
{

	XDAS_Int8 *stream;
    XDAS_UInt32 byteCount,nalLength,frame_length,NALidx = 0;

    XDAS_UInt8 currNalType,firstMbInSliceFlag;
	XDAS_Int32 *numNalArr;

	IH264VDEC_DynamicParams *dynamicParams = (IH264VDEC_DynamicParams *)params;
	numNalArr = (XDAS_Int32*)Input->descs[1].buf;
	stream = Input->descs[0].buf;
    currNalType  = stream[4] & 0x1f;
    nalLength    = 0;
	frame_length = 0;

    for(byteCount = 4; byteCount < bufferLen; byteCount++) 
	{
        if(stream[byteCount] == 0 && stream[byteCount+1] == 0 && stream[byteCount+2] == 0 && stream[byteCount+3] == 0x01)
		{		
            firstMbInSliceFlag = stream[byteCount+5] & 0x80;
            if ((currNalType !=  NALU_TYPE_SPS) && 
				(currNalType !=  NALU_TYPE_PPS)&&
				(currNalType !=  NALU_TYPE_AUD)&&
				(currNalType !=  NALU_TYPE_SEI)&&
				(currNalType !=  NALU_TYPE_FILL)
				) 
			{
				//Input->descs[1].buf[NALidx++] = nalLength;
				numNalArr[NALidx++] = nalLength;

				if (firstMbInSliceFlag)
				{
					//frameCount++;
                    break;
				}
				
				frame_length += 4;
				stream += 4;
				nalLength = 0;
            }
			else 
			{
				//Input->descs[1].buf[NALidx++] = nalLength;
				numNalArr[NALidx++] = nalLength;
                break;
            }
			
        }
		else
		{
           inputData_NAL[nalLength++] = stream[byteCount];
		   frame_length++;
		}        
		
    }

	if(byteCount == bufferLen)
	{
		//Input->descs[1].buf[NALidx++] = nalLength;
		numNalArr[NALidx++] = nalLength;
		frame_length++;
		//frameCount++;
	}

	dynamicParams->numNALunits = NALidx;    
	return frame_length+4; 
}
#endif



//The following accumulates data for 1 NAL unit 
int H264VDEC_getOneNAL(XDM1_BufDesc *Input,
					   XDAS_Int8 *inputData_NAL,
					   unsigned int bufferLen,
					   IVIDDEC2_DynamicParams *params)
{

	XDAS_Int8 *stream;
    XDAS_UInt32 byteCount,nalLength,frame_length,NALidx = 0;

   // XDAS_UInt8 firstMbInSliceFlag;

	XDAS_UInt8 next_start_code_bytes,start_pos;
	XDAS_Int32 *numNalArr;
	IH264VDEC_DynamicParams *dynamicParams = (IH264VDEC_DynamicParams *)params;
	numNalArr = (XDAS_Int32*)Input->descs[1].buf;
	stream = Input->descs[0].buf;

    nalLength    = 0;
	frame_length = 0;

	//locate the first start code
	for (byteCount = 0; byteCount < bufferLen; byteCount++)
	{

		if (stream[byteCount] == 0 && stream[byteCount+1] == 0 && stream[byteCount+2] == 0 && stream[byteCount+3] == 0x01)
		{
			start_pos = byteCount + 4;
			break;
		}
		else if (stream[byteCount] == 0 && stream[byteCount+1] == 0 && stream[byteCount+2] == 1)
		{
			start_pos = byteCount + 3;
			break;
		}
	}


    for (byteCount = start_pos; byteCount < bufferLen; byteCount++)
	{
		next_start_code_bytes = 0;
			
	    if(stream[byteCount] == 0 && stream[byteCount+1] == 0 && stream[byteCount+2] == 0 && stream[byteCount+3] == 0x01)
		{
			next_start_code_bytes = 4;
		}
	    else if(stream[byteCount] == 0 && stream[byteCount+1] == 0 && stream[byteCount+2] == 1)
		{
			next_start_code_bytes = 3;                    
		}
				
		if(next_start_code_bytes)
		{
			//Input->descs[1].buf[NALidx++] = nalLength;
			numNalArr[NALidx++] = nalLength;
            break;		
        }
		else
		{
           inputData_NAL[nalLength++] = stream[byteCount];
		   frame_length++;
		}		
    }

	if(byteCount == bufferLen)
	{
		//Input->descs[1].buf[NALidx++] = nalLength;
		numNalArr[NALidx++] = nalLength;
		frame_length++;
		//frameCount++;
	}

	dynamicParams->numNALunits = NALidx;    
	return frame_length+start_pos; 
}



#ifdef COMPUTE_FRAME_LENGTH
int H264VDEC_getFrameSize(XDAS_Int8 *stream, unsigned int bufferLen)
{
    XDAS_UInt32 wordCount,nalLength;

    XDAS_UInt8 currNalType,firstMbInSliceFlag;

    currNalType = stream[4] & 0x1f;
    nalLength   = 0;

    for (wordCount = 4; wordCount < bufferLen; wordCount++)
	{
		if(stream[wordCount] == 0 && stream[wordCount+1] == 0 && stream[wordCount+2] == 0 && stream[wordCount+3] == 0x01)
		{
			firstMbInSliceFlag = stream[wordCount+5] & 0x80;

            if((currNalType != NALU_TYPE_SPS) && 
				(currNalType !=  NALU_TYPE_PPS)&&
				(currNalType !=  NALU_TYPE_AUD)&&
				(currNalType !=  NALU_TYPE_SEI)&&
				(currNalType !=  NALU_TYPE_FILL)
				) 
			{
				if(firstMbInSliceFlag)
                    break;
            }
			else
			{
                currNalType = stream[wordCount+4] & 0x1f;
            }
        }

        nalLength++;
    }

    return nalLength+4;
	
}
#endif  


  /******************************************************************************/
  /*    Copyright (c) 2006 Texas Instruments, Incorporated                      */
  /*    All Rights Reserved                                                     */
  /******************************************************************************/

