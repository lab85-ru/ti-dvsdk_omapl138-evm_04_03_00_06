/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/*!
********************************************************************************
@file     TestAppDecoder.c
@brief    This is the top level client file that drives the MPEG-4
(Advanced Simple Profile) Video Decoder Call using XDM Interface
@author   Multimedia Codecs TI India
@version  0.1 - Jan 24,2006    initial version
********************************************************************************
*/

/* Standard C header files */
#include <std.h>
#include <stdio.h>
#include <stdlib.h>
#include <std.h>

/* M4H3 Interface header files */
#include "m4h3dec.h"
#include "m4h3dec_ti.h"

/*---------------------------------------------------------------------------*/
/*  HDVICP and buffer management operations' header files.                   */
/*---------------------------------------------------------------------------*/
#include "buffermanager.h"

#ifndef MSVC
/* CSL and DMAN3 header files */
#include "bcache.h"
#include "dman3.h"
#include "bios_rts.h"
#endif

/* Client header file */
#include "TestAppDecoder.h"

/* Enable to profile the code */
//#define PROFILING	

//#define SCRATCH_TEST /* Enable this to test scratch memory */
//#define INTR_TEST 1 /* Enable this to test decoder with interrupts enables (not for OMAP 3430) */
//#define TEST_ODD_RESOLUTION_BITEXACT /* Enable this flag to check bit exactness for odd resolution */

/* Definitions for PROFILFING */
#ifdef PROFILING
unsigned int cycles[2000];
unsigned int cnt;
#endif /* PROFILING */

/* Input buffer allocation */
XDAS_Int8 inputData[INPUT_BUFFER_SIZE];
XDAS_UInt8  appMBErrStatBuf[(FRAME_WIDTH*FRAME_HEIGHT) >> 8];

#ifdef QUANT_VERSION
/* Allocation for quant value & version info buffer*/
XDAS_UInt8  quant[3600*2];
#endif

#ifdef SCRATCH_TEST
int scratch_flag = 1;
int num_memtab;
#else
int scratch_flag = 0;
int num_memtab = 0;
#endif
IALG_MemRec *memTab_scratch;
int create_multi = 0; 


IVIDDEC2_Params             params;
IM4H3DEC_DynamicParams		dynamicParams;

XDAS_UInt32                framesToDecode;
XDAS_Int32				   dbFlag;
XDAS_Int32				   drFlag;
XDAS_Int32				   cnFlag;
XDAS_Int32				   frameorder;
XDAS_Int32				   displaywidth;
XDAS_Int32				   FrameLevelByteSwapFlag;
int traceFp;

#ifdef TEST_ODD_RESOLUTION_BITEXACT
int actualHeight,actualWidth ;
#endif

sTokenMapping sTokenMap[] =
{
  {"FramesToDecode", &framesToDecode},
  {"ImageHeight",    &params.maxHeight},
  {"ImageWidth",     &params.maxWidth},
  {"ChromaFormat",   &params.forceChromaFormat},
  {"DeblockFlag",   &dbFlag},
  {"DeringFlag",    &drFlag},
  {"ConcealFlag",   &cnFlag},
  {"FrameLevelByteSwapFlag",   &FrameLevelByteSwapFlag},
  {NULL,             NULL}     
};

#ifdef INTR_TEST
#include "intc.h"
XDAS_UInt32 Interrupt_Enable =  TRUE ; //FALSE;
XDAS_UInt32 Interrupt_Delay  = 80;
int interrupt_counter;

#ifdef DM648
void Interrupt_start()
{
	*((unsigned int *)0x02044820) = 0x0;
	*((unsigned int *)0x02044824) = 0x0;
	*((unsigned int *)0x02044824) = 0x0007;
        *((unsigned int *)0x0204481C) = Interrupt_Delay;//count
        *((unsigned int *)0x02044814) = 0x0;
	*((unsigned int *)0x02044804) = 0x1;
	*((unsigned int *)0x02044820) = 0x800000;
}
#else
void Interrupt_start(void)
{
	*((unsigned int *)0x01C21420) = 0x0;
	*((unsigned int *)0x01C21424) = 0x0;
	*((unsigned int *)0x01C21424) = 0x0007;
    	*((unsigned int *)0x01C2141C) = 100;//count
    	*((unsigned int *)0x01C21414) = 0x0;
	*((unsigned int *)0x01C21404) = 0x1;
	*((unsigned int *)0x01C21420) = 0x800000;
}
#endif
#endif


/* Main Function acting as a client for Video Decode Call */
XDAS_Int32 main ()
{
  /* File I/O variables                                                       */
  FILE *fConfigFile, *ftestFile, *finFile, *fparamsFile;
  
  XDAS_Int8 line[STRING_SIZE], inFile[STRING_SIZE], testFile[STRING_SIZE];
  XDAS_Int8 paramsFile[STRING_SIZE], testCompliance;

#ifndef MSVC
  XDAS_Int8 *fname = "..\\..\\..\\Client\\Test\\TestVecs\\Config\\Testvecs.cfg";
#else
  XDAS_Int8 *fname = "..\\client\\test\\TestVecs\\Config\\Testvecs_vc.cfg";

#endif
  
  /* Input/Output  buffers  and their sizes                                   */
  XDAS_UInt32  ii;
 
  IVIDDEC2_Fxns *ividDecFxns;
  
  /* Extended Class Structures                                                    */
  IM4H3DEC_Status             status;
  IM4H3DEC_InArgs            inArgs;
  IM4H3DEC_OutArgs            outArgs;
  
  /* Algorithm specific handle                                                */
  IALG_Handle handle;
  
  /* Input/Output Buffer Descriptors                                          */
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
  
  /* Input Buffer Management specific variables                               */
  XDAS_Int32 validBytes; 
  XDAS_UInt32 bytesConsumed,ret_val,first_time;
  XDAS_UInt32 BytesRead, BufferSize;

  /* Other variables                                                          */
  XDAS_Int32  frameCount, retVal, testVal, countConfigSet,displayFrame;

#ifdef PROFILING        
  unsigned long long t0, t1;
  FILE *fp_profiling, *fp_status;
#endif /* PROFILING */

#ifdef PROFILING        
  fp_profiling = fopen("../../../frame_cycles_mpeg4_dec.csv", "w");
  if(fp_profiling == NULL)
  {
  printf("Could not open file for profile dump");
  return(0);
  }
  fclose(fp_profiling);

  fp_profiling = fopen("../../../frame_cycles_mpeg4_dec.csv", "a+b");

  fp_status = fopen("../../../status_mpeg4_dec.csv", "w");
  if(fp_status == NULL)
  {
  printf("Could not open file for status dump");
  return(0);
  }
  fclose(fp_status);

  fp_status = fopen("../../../status_mpeg4_dec.csv", "a+b");

  TSC_enable();
  
  {
  	FILE *fp;
		
	fp = fopen("profile.xls", "w");
	if(fp == NULL)
	{
		printf("File Opening Error !");
		exit(0);
	}
	
	fprintf(fp,"\n\tTest Vector Name");
	fprintf(fp,"\tNo of frames");
	fprintf(fp,"\tAverage Cycles");
	fprintf(fp,"\tPeak Cycles\n");
	fclose(fp);
  }
#endif /* PROFILING */


#ifdef INTR_TEST
  /* Initialize Interrupt Settings                                            */
  INTC_init();
#endif

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
    printf("\n Input file...  %s  ",inFile);
 
    if (!finFile)
    {
      printf("\n Couldn't open Input file...  %s  ",inFile);
      printf("\n Exiting for this configuration...");
      continue;
    }
#ifdef PROFILING
#ifndef SELF_TEST
 	fprintf(fp_profiling, "\n%s", inFile);
    fprintf(fp_status, "\n%s", inFile);
#endif
#endif /* PROFILING */
    
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
    TestApp_SetInitParams(&params);
    
    /* Initializing DMAN3 parameters                                          */
#ifdef DM648
    /*---------------KAILASH_PVS-------------*/
    DMAN3_PARAMS.qdmaPaRamBase  = (unsigned int *)0x02A04000 ;
#endif

#ifndef MSVC
    /* Base Index Value from where DMA params are written                     */
    DMAN3_PARAMS.paRamBaseIndex  = 78; 
    /* Number of QDMA channels used by the Algorithm                          */
    DMAN3_PARAMS.numQdmaChannels = 4;  
    /* Number of Algorithms: System Parameter                                 */
    numAlgs                      = NUM_ALGS;  
    /* Group Id needed by DMAN3 library: System Parameter                     */
    groupId                      = 0;
    
    DMAN3_PARAMS.numPaRamGroup[groupId] = 32 ;
	DMAN3_PARAMS.numPaRamEntries = 32 ;
#endif
    /* Setting the sizes of Base Class Objects                                */
    params.size                  = sizeof(IVIDDEC2_Params);
    status.viddecStatus.size   = sizeof(IM4H3DEC_Status);
    dynamicParams.viddecDynamicParams.size           = sizeof(IM4H3DEC_DynamicParams);
    inArgs.viddecInArgs.size                  = sizeof(IM4H3DEC_InArgs);
    outArgs.viddecOutArgs.size                 = sizeof(IM4H3DEC_OutArgs);
    
    /* Initializing Variables                                                */
    frameCount                   = 0; 
    
    /* Read the bitstream in the Application Input Buffer                    */
    fseek (finFile, 0, SEEK_END);
    BufferSize = ftell (finFile);
    printf("\nFileSize = %d", BufferSize);
    fseek (finFile, 0, SEEK_SET);
	validBytes = BufferSize;
    
    /* Return if there is an error in reading the file                       */
    if (validBytes < 0) /* Error condition                                   */
    {
      printf("Valid Bytes incorrect... Exiting for this configuration...\n");
      continue;
    }
    
    /* Create the Algorithm object (instance)                                */
    printf("\nCreating Algorithm Instance...");
    
    if ((handle =  (IALG_Handle)ALG_create (
      (IALG_Fxns *) &M4H3DEC_TI_IM4H3DEC, 
      (IALG_Handle) NULL, (IALG_Params *) &params)) == NULL)
    {
      printf( "\nFailed to Create Instance... Exiting for this configuration..");
      continue;
    }
    printf("\nAlgorithm Instance Creation Done...\n");

#ifndef MSVC    
    /* DMA allocation using DMAN3 API's                                      */
    DMAN3_init();
    
    /* Provide the Algorithm handle to DMAN3 Interface for granting channels */
    handleArray[0] = (IALG_Handle) handle;
    dmaFxns[0]     = &M4H3DEC_TI_IDMA3;
    
    if (DMAN3_grantDmaChannels(groupId, handleArray, dmaFxns, numAlgs)
      != DMAN3_SOK) {
      printf("\nProblem adding algorithm's dma resources");
      printf("... Exiting for this configuration... \n");
      continue;
    }
#endif

	frameorder = 0;
    
	dynamicParams.postDeblock = dbFlag;
	dynamicParams.postDering =  drFlag; 
	dynamicParams.errorConceal = cnFlag;
	dynamicParams.FrameLevelByteSwap = 1; 
	dynamicParams.viddecDynamicParams.mbDataFlag = 0;
    dynamicParams.viddecDynamicParams.frameOrder = frameorder;//IVIDDEC2_DISPLAY_ORDER; //IVIDDEC2_DECODE_ORDER;
    TestApp_SetDynamicParams((IVIDDEC2_DynamicParams *)&dynamicParams);

    /*-----------------------------------------------------------------------*/
    /*  The outBufsInUseFlag tells us whether the previous input buffer given*/
    /*  by the application to the algorithm is still in use or not. Since    */
    /*  this is before the first decode call, assign this flag to 0. The     */
    /*  algorithm will take care to initialize this flag appropriately from  */
    /*  hereon for the current configuration.                                */
    /*-----------------------------------------------------------------------*/
    outArgs.viddecOutArgs.outBufsInUseFlag     = 0;

    TestApp_SetDynamicParams((IVIDDEC2_DynamicParams *)&dynamicParams);
    /* Assigning Algorithm handle fxns field to ividEncfxns                  */
    ividDecFxns = (IVIDDEC2_Fxns *)handle->fxns;

    /* Initialize dynamic params */
    dynamicParams.useHighPrecIdctQp1 = 0;
    dynamicParams.viddecDynamicParams.displayWidth = 0;
    dynamicParams.viddecDynamicParams.newFrameFlag = 0;

    /*-----------------------------------------------------------------------*/
    /*  The XDM_RESET call gives resets all the internal data structures and */
    /*  context for the new stream that is to be decoded.                    */
    /*-----------------------------------------------------------------------*/
    ividDecFxns->control((IVIDDEC2_Handle)handle, XDM_RESET,
                        (IVIDDEC2_DynamicParams *)&dynamicParams,
                        (IVIDDEC2_Status *)&status);
    ividDecFxns->control((IVIDDEC2_Handle)handle, XDM_SETPARAMS,
                        (IVIDDEC2_DynamicParams *)&dynamicParams,
                        (IVIDDEC2_Status *)&status);
    /*-----------------------------------------------------------------------*/
    /*  The GETBUFINFO call gives information for number and size of input   */
    /*  and output buffers. For this first call, the actual width and height */
    /*  of the picture is not known hence, this control call returns the out */
    /*  buffer sizes based on the maxHeight and maxWidth provided as part    */
    /*  of the input params from testparams file.                            */
    /*-----------------------------------------------------------------------*/
    ividDecFxns->control((IVIDDEC2_Handle)handle, XDM_GETBUFINFO,
                        (IVIDDEC2_DynamicParams *)&dynamicParams,
                        (IVIDDEC2_Status *)&status);
    /*-----------------------------------------------------------------------*/
    /*  The buffermanager init function call allocates the memory for the    */
    /*  number of buffers of the corresponding sizes as passed in arguments  */
    /*  to it. This function initializes the output/reference bufs based on  */
    /*  the outBuf parameters returned from the GETBUFINFO call above. This  */
    /*  call will allocate each of the numOutbufs memory equal to (specified */
    /*  bufsize). The initialization and allocation of buff ptr will take    */
    /*  place only for the first ref buffer. For the remaining ref buffers,  */
    /*  allocation will happen after the first decode call, when actual      */
    /*  height and width of picture are known.                               */
    /*  Note that the allocation of output/ref buffers will happen each time */
    /*  for the entire bunch of configuration sets provided in the testvecs  */
    /*  config file.                                                         */
    /*-----------------------------------------------------------------------*/
    ret_val = BUFFMGR_Init(status.viddecStatus.bufInfo.minNumOutBufs,
                                                 status.viddecStatus.bufInfo.minOutBufSize);

    if (ret_val)
    {
        printf ("\nMemory could not get allocated for output buffers\n");
        break;
    }
    

#ifdef PROFILING	
	cnt = 0;
#endif /* PROFILING */

    /*-----------------------------------------------------------------------*/
    /*  Initialize the input buffer properties as required by algorithm      */
    /*  based on info received by preceding GETBUFINFO call. First init the  */
    /*  number of input bufs.                                                */
    /*-----------------------------------------------------------------------*/

    inputBufDesc.numBufs = status.viddecStatus.bufInfo.minNumInBufs ;
    inputBufDesc.descs[0].bufSize = status.viddecStatus.bufInfo.minInBufSize[0];

      for(ii=0; ii<(status.viddecStatus.bufInfo.minNumInBufs-1);ii++ )
      {
        inputBufDesc.descs[ii+1].buf = inputBufDesc.descs[ii].buf +
          status.viddecStatus.bufInfo.minInBufSize[ii];
        inputBufDesc.descs[ii+1].bufSize =
          status.viddecStatus.bufInfo.minInBufSize[ii +1];
      }

    /*-----------------------------------------------------------------------*/
    /*  Initialize the output buffer properties as required by algorithm     */
    /*  based on info received by preceding GETBUFINFO call.                 */
    /*-----------------------------------------------------------------------*/
    outputBufDesc.numBufs = status.viddecStatus.bufInfo.minNumOutBufs;
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
    outArgs.viddecOutArgs.bytesConsumed = 0;
	inArgs.viddecInArgs.numBytes = 0;
    displayFrame = 0;
    /* Do-While Loop for Decode Call                                         */
    do
    {
#ifdef DM648
#ifdef SCRATCH_TEST      
      memset(0x00A01000, 'c',  0x00010000);
	  printf("\n Corrupted scratch");
#endif
#endif
      
	  /* Assign the number of bytes available                                */
      inputBufDesc.descs[0].buf   = (XDAS_Int8 *)((XDAS_Int32) inputData);
	  bytesConsumed = outArgs.viddecOutArgs.bytesConsumed;
	  BytesRead = TestApp_ReadByteStream(finFile,inputBufDesc.descs[0].bufSize, bytesConsumed, inArgs.viddecInArgs.numBytes);      
	  inArgs.viddecInArgs.numBytes =  BytesRead;
	  validBytes = BytesRead;
    

       /* Check for Break off condition                                       */
      if (BytesRead <=0)
      {
        printf("\n Bitstream Ended...  ");
        break; /* Bitstream ended: Break off the while loop                  */
      }

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
      inArgs.viddecInArgs.inputID            = buffEle->bufId;

      /* Optional: Set Run time parameters in the Algorithm via control()    */
      ividDecFxns->control((IVIDDEC2_Handle)handle, XDM_SETPARAMS, 
        (IVIDDEC2_DynamicParams *)&dynamicParams, 
        (IVIDDEC2_Status *)&status);  
	  
      /* Cache Invalidate for Input Buffer                                   */
      for(ii=0 ; ii < inputBufDesc.numBufs ; ii++ )
      {
#ifndef MSVC
        /* Cache Invalidate for Input Buffer                                 */

        BCACHE_inv(inputBufDesc.descs[ii].buf, INPUT_BUFFER_SIZE, TRUE);
#endif
      }  

#ifdef INTR_TEST
      if (Interrupt_Enable)
      {
  	    Interrupt_start();
	  
	    INTC_attachIsr(INT_EVT_TMR1,NULL); 
        /* Enable Interrupts */                                                   
        INTC_enable(INT_EVT_TMR1);

        printf("\nEnabling interrupts..\n");
		interrupt_counter = 0;
        	
      }
#endif /* INTR_TEST */

    ALG_activate((IALG_Handle)handle);

    outArgs.viddecOutArgs.displayBufs->frameWidth =0;
    outArgs.viddecOutArgs.displayBufs->frameHeight =0;

#ifdef PROFILING      
      t0  = TSC_read();
#endif /* PROFILING */
 
      /* Basic Algorithm process() call                                      */
      retVal = ividDecFxns->process((IVIDDEC2_Handle)handle, 
        (XDM1_BufDesc *)&inputBufDesc,
        (XDM_BufDesc *)&outputBufDesc,
        (IVIDDEC2_InArgs *)&inArgs,
        (IVIDDEC2_OutArgs *)&outArgs);

#ifdef PROFILING      
      t1  = TSC_read();
	  fprintf(fp_profiling, "\n%d", (t1-t0));
#endif /* PROFILING */

	  ALG_deactivate((IALG_Handle)handle);

#ifdef PROFILING
	  cycles[cnt] = (t1-t0);
	  cnt++;
#endif /* PROFILING */

#ifdef INTR_TEST

      if (Interrupt_Enable)
      {
        /* Disable Interrupts                                 */
        INTC_disable(INT_EVT_TMR1);
        printf("Interrupts disabled\n");
      }
#endif /* INTR_TEST */

#ifndef MSVC      
      /* Cache Writeback Invalidate for Output Buffers                       */
      if(outArgs.viddecOutArgs.outputID[0] > 0)
	  {
        for(ii=0 ; ii < outputBufDesc.numBufs ; ii++ )
        {
          BCACHE_wbInv(outputBufDesc.bufs[ii],
            outputBufDesc.bufSizes[ii], TRUE);
        }
	  }
#endif

      if(retVal == XDM_EFAIL)
      {
        printf("\n Process function returned an Error for stream %s...",inFile);
       // break; /* Error Condition: Application may want to break off         */
      }
      
      /* Optional: Read status via control()                                 */
      ividDecFxns->control((IVIDDEC2_Handle)handle, 
        XDM_GETSTATUS, 
        (IVIDDEC2_DynamicParams *)&dynamicParams, 
        (IVIDDEC2_Status *)&status);
#ifdef QUANT_VERSION
	  {
		  /* Optional: Read Quant Data via control()   */
		  status.viddecStatus.data.buf = quant;
		  status.viddecStatus.data. bufSize = 3600*2;
		  ividDecFxns->control((IVIDDEC2_Handle)handle, XDM_GETSTATUS,
			  (IVIDDEC2_DynamicParams *)&dynamicParams,
			  (IVIDDEC2_Status *)&status);

		  /* Optional: Get Version Data via control()   */		  
		  status.viddecStatus.data.bufSize = 10;
		  ividDecFxns->control((IVIDDEC2_Handle)handle, XDM_GETVERSION,
			  (IVIDDEC2_DynamicParams *)&dynamicParams,
			  (IVIDDEC2_Status *)&status);
		  
	  }
#endif
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
          BUFFMGR_ReInit (status.viddecStatus.maxNumDisplayBufs,
                  status.viddecStatus.bufInfo.minNumOutBufs, status.viddecStatus.bufInfo.minOutBufSize);

          /*-----------------------------------------------------------------*/
          /*  Reset the flag first_time to 0 since the buffers are correctly */
          /*  reinitialized.                                                 */
          /*-----------------------------------------------------------------*/
          first_time = 0;

      }


      /* Check for frame ready via display buffer pointers                   */
      if(outArgs.viddecOutArgs.outputID[0] > 0)
      {
        printf("\n Decoded Frame # %d  ",frameCount);

#ifdef TEST_ODD_RESOLUTION_BITEXACT
        actualHeight = status.actualHeight;
		actualWidth = status.actualWidth;
#endif
        if(testCompliance)
        {
          /* Compare the output frames with the Reference File               */
          testVal = TestApp_CompareOutputData(ftestFile,
            &outputBufDesc,
            (IVIDDEC2_OutArgs *)&outArgs);

          if(testVal == XDM_EOK)
		  {
			  printf("\n Decoder compliance test passed ");
	      }
	      else
	      {
			  /*Test Compliance Failed... Breaking...*/
			  printf("\n Decoder compliance test failed");
			  break;
          }
        }
        else
        {
          /* Write the output frames                     */
     	      TestApp_WriteOutputData(ftestFile, &outputBufDesc, (IVIDDEC2_OutArgs *) &outArgs);
	      printf("\n Decoder output dump completed” ");
        }
        displayFrame++;
       }

	  frameCount++;
 
      /*---------------------------------------------------------------------*/
      /*  Always release buffers - which are released from the algorithm     */
      /*  side   -back to the buffer manager. The freebufID array of outargs */
      /*  contains the sequence of bufferIds which need to be freed. This    */
      /*  gets populated by the algorithm. The following function will do    */
      /*  the job of freeing up the buffers.                                 */
      /*---------------------------------------------------------------------*/
      BUFFMGR_ReleaseBuffer((XDAS_UInt32 *)outArgs.viddecOutArgs.freeBufID);

      if (frameCount >= framesToDecode)
      {
        printf("\n Specified number of Frames Decoded...  ");
        break; /* Bitstream ended: Break off the while loop                  */
      }
      bytesConsumed          += outArgs.viddecOutArgs.bytesConsumed;
      validBytes             -= outArgs.viddecOutArgs.bytesConsumed;

    }  while(1);  /* end of Do-While loop                                    */


  /* Add logic for flushing display buffers */
    ividDecFxns->control((IVIDDEC2_Handle)handle,XDM_FLUSH,
                         (IVIDDEC2_DynamicParams *)&dynamicParams,
                         (IVIDDEC2_Status *)&status);
    
    
    while(displayFrame < framesToDecode)
    {
		inArgs.viddecInArgs.numBytes        = 0; 

		ALG_activate((IALG_Handle)handle);
		
		/* Basic Algorithm process() call                                      */
		retVal = ividDecFxns->process((IVIDDEC2_Handle)handle, 
			(XDM1_BufDesc *)&inputBufDesc,
			(XDM_BufDesc *)&outputBufDesc,
			(IVIDDEC2_InArgs *)&inArgs,
			(IVIDDEC2_OutArgs *)&outArgs);
		ALG_deactivate((IALG_Handle)handle);
		
		ividDecFxns->control((IVIDDEC2_Handle)handle,XDM_GETSTATUS,
			(IVIDDEC2_DynamicParams *)&dynamicParams,
			(IVIDDEC2_Status *)&status);
		
		if(retVal)
			break;
	
		/* Write the output frames in the display order                    */
		if(outArgs.viddecOutArgs.outputID[0] > 0)
			TestApp_WriteOutputData(ftestFile, &outputBufDesc, (IVIDDEC2_OutArgs *) &outArgs);
		
		displayFrame++;
    }
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
    BUFFMGR_DeInit(status.viddecStatus.maxNumDisplayBufs, outputBufDesc.numBufs);
    /* Output file close                                                     */
    fclose(ftestFile);
#ifndef MSVC
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
    /* Delete the Algorithm instance object specified by handle */
    ALG_delete (handle);

#ifdef PROFILING
	{
		unsigned int i;
		unsigned long long avg=0;
		unsigned int max = 0;
		FILE *fp;
		
		fp = fopen("profile.xls", "a");
		if(fp == NULL)
		{
			printf("File Opening Error !");
			exit(0);
		}

		/* Calculate average and peak cycles */
		for(i=0; i<cnt; i++)
		{
			if(cycles[i] > max)
			{
				max = cycles[i];
			}
			avg += cycles[i];			
		}
		avg /= cnt;

		fprintf(fp, "\n\t%s", inFile);
		fprintf(fp,"\t%d\t%d\t%d\n", cnt, (unsigned int) avg, max);
		fclose(fp);
	  }
#endif /* PROFILING */

    
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
      (XDAS_UInt32)status.viddecStatus.outputWidth, (XDAS_UInt32)status.viddecStatus.outputHeight);
    
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
  
//DAS_Int32 TestApp_ReadByteStream(FILE *finFile)
  XDAS_Int32 TestApp_ReadByteStream(FILE *finFile, XDAS_UInt32 bufSizes, XDAS_UInt32 bytesConsumed, XDAS_UInt32 numBytes)

{
    XDAS_UInt32 BytesRead;
    
    if (bufSizes > INPUT_BUFFER_SIZE)
    {
      printf( "\nWarning : File size exceeds the application input buffer size %d ", 
        INPUT_BUFFER_SIZE);
      printf( "\nContinuing decoding for %d bytes.\n", INPUT_BUFFER_SIZE);
      bufSizes = INPUT_BUFFER_SIZE;
    }
    
    /*Read the "BufferSize" number of bytes in the input buffer*/
    fseek( finFile, -(numBytes - bytesConsumed), SEEK_CUR);

	memset (inputData, '0', bufSizes);
    BytesRead = fread (inputData, 1, bufSizes, finFile);
#ifndef MSVC
    /* Cache Invalidate for Input Buffer */
    BCACHE_wbInv(inputData, INPUT_BUFFER_SIZE, TRUE);
#endif   
    //printf ("\nInput File read successfully...");
    return (BytesRead);
  }

  /*
  //============================================================================
  // TestApp_CompareOutputData
  //  Comparing Output Data with Reference File data
  */
	XDAS_Int8 Refbuffer[720*480*3];

  XDAS_Int32 TestApp_CompareOutputData(FILE *fRefFile,
    XDM_BufDesc * outputBufDesc,
    IVIDDEC2_OutArgs *outArgs)
  {
    XDAS_Int32 retVal;
    XDAS_Int32 row,col;
	XDAS_Int32 YWidth,YHeight;
	XDAS_Int8 *output;
	XDAS_Int8 *ReferenceBuffer = Refbuffer;
	IM4H3DEC_OutArgs *outargs = (IM4H3DEC_OutArgs *)outArgs;

	retVal = XDM_EOK;


	YWidth	=	outargs->viddecOutArgs.displayBufs[0].frameWidth;
	YHeight	=	outargs->viddecOutArgs.displayBufs[0].frameHeight;



	output = &outargs->viddecOutArgs.displayBufs[0].bufDesc[0].buf[0];

    if(outargs->viddecOutArgs.displayBufs[0].chromaFormat == 4)
	{

		memset (ReferenceBuffer, 0, outargs->viddecOutArgs.displayBufs->bufDesc->bufSize);
		fread (ReferenceBuffer, 1, outargs->viddecOutArgs.displayBufs->bufDesc->bufSize, fRefFile);
		for(col=0; col<outargs->viddecOutArgs.displayBufs->bufDesc->bufSize; col++)
		{
			if(output[col] != ReferenceBuffer[col])
  			{
  				retVal = XDM_EFAIL;
  				break;
  			}
		}

		return retVal;
	}

	memset (ReferenceBuffer, 0, (YWidth  * YHeight * 3)>>1);
	fread (ReferenceBuffer, 1, (YWidth  * YHeight * 3)>>1, fRefFile);

	for ( row=0; row<YHeight; row++ )
	{
		for(col=0; col<YWidth; col++)
		{
		if(output[col] != ReferenceBuffer[col])
  		{
  			retVal = XDM_EFAIL;
  			break;
  		}
		}
		output +=	outargs->viddecOutArgs.displayBufs[0].framePitch;
		ReferenceBuffer += (YWidth);
	}

	output 	 = &outargs->viddecOutArgs.displayBufs[0].bufDesc[1].buf[0];

	for ( row=0; row < (YHeight>>1); row++ )
	{
		for(col=0; col<(YWidth>>1); col++)
		{
		if(output[col] != ReferenceBuffer[col])
  		{
  			retVal = XDM_EFAIL;
  			break;
  		}
		}
		output +=	(outargs->viddecOutArgs.displayBufs[0].framePitch >> 1);
		ReferenceBuffer += (YWidth>>1);
	}

	output 	= &outargs->viddecOutArgs.displayBufs[0].bufDesc[2].buf[0];
	for ( row=0; row < YHeight>>1; row++ )
	{
		for(col=0; col<(YWidth>>1); col++)
		{
		if(output[col] != ReferenceBuffer[col])
  		{
  			retVal = XDM_EFAIL;
  			break;
  		}
		}
		output +=	(outargs->viddecOutArgs.displayBufs[0].framePitch >> 1);
		ReferenceBuffer += (YWidth>>1);
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

	XDAS_Int32 YWidth, YHeight;
	XDAS_Int32 row,oddwidth=0,oddheight=0;
	XDAS_Int8 *output;
	IM4H3DEC_OutArgs *outargs = (IM4H3DEC_OutArgs *)outArgs;

	output = &outargs->viddecOutArgs.displayBufs[0].bufDesc[0].buf[0];
	YWidth	=	outargs->viddecOutArgs.displayBufs[0].frameWidth;
	YHeight	=	outargs->viddecOutArgs.displayBufs[0].frameHeight;

#ifdef TEST_ODD_RESOLUTION_BITEXACT
	/* Following code is added to test odd resolution bit exactness with Microsoft refference */
	if(actualHeight/2*2 !=   actualHeight)
	{
		YHeight	=	actualHeight;
		oddheight = 1;
	}
	if(actualWidth/2*2 !=   actualWidth)
	{
		YWidth	=	actualWidth;
		oddwidth = 1;
	}
#endif

    if(outargs->viddecOutArgs.displayBufs[0].chromaFormat == 4)
	{
		for ( row=0; row < YHeight; row++ )
		{
			fwrite(output, YWidth*2,   1, fOutFile);
			output +=	outargs->viddecOutArgs.displayBufs[0].framePitch;
		}
	   fflush(fOutFile);
	   return;
	}


	for ( row=0; row < YHeight; row++ )
	{
		fwrite(output, YWidth,   1, fOutFile);
		output +=	outargs->viddecOutArgs.displayBufs[0].framePitch;
		fflush(fOutFile);

	}

	output 	 = &outargs->viddecOutArgs.displayBufs[0].bufDesc[1].buf[0];

	for ( row=0; row < YHeight>>1; row++ )
	{
		fwrite(output, YWidth>>1,   1, fOutFile);
		if(oddwidth)
			fwrite(output+((YWidth>>1)-1), 1,  1, fOutFile);
		output +=	(outargs->viddecOutArgs.displayBufs[0].framePitch>> 1);
	}
    fflush(fOutFile);
	if(oddheight)
	{
		output -=	(outargs->viddecOutArgs.displayBufs[0].framePitch>> 1);
		fwrite(output, YWidth>>1,   1, fOutFile);
		if(oddwidth)
			fwrite(output+((YWidth>>1)-1), 1,  1, fOutFile);
	}

    fflush(fOutFile);
	output 	= &outargs->viddecOutArgs.displayBufs[0].bufDesc[2].buf[0];
	for ( row=0; row < YHeight>>1; row++ )
	{
		fwrite(output, YWidth>>1,   1, fOutFile);
		if(oddwidth)
			fwrite(output+((YWidth>>1)-1), 1,  1, fOutFile);
		output +=	(outargs->viddecOutArgs.displayBufs[0].framePitch >> 1);
	}
    fflush(fOutFile);
	if(oddheight)
	{
		output -=	(outargs->viddecOutArgs.displayBufs[0].framePitch>> 1);
		fwrite(output, YWidth>>1,   1, fOutFile);
		if(oddwidth)
			fwrite(output+((YWidth>>1)-1), 1,  1, fOutFile);
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
    /* Set IVIDDEC parameters                                                   */
    
    /* Max Frame Rate: Not currently used in the algorithm                      */
    params->maxFrameRate        = 30; 
    
    /* Max Bit Rate: Not currently used in the algorithm                        */
    params->maxBitRate          = 10000000; 
    
    /* Data Endianness (0: Little, 1: Big) : Big Endian not supported           */
    params->dataEndianness      = XDM_BYTE;
    return;
}
  
/*
//==============================================================================
// TestApp_SetDynamicParams
//  setting of run time parameters
*/
XDAS_Void TestApp_SetDynamicParams(IVIDDEC2_DynamicParams *dynamicParams)
{
    /* Set IVIDDEC Run time parameters */
    dynamicParams->decodeHeader  = XDM_DECODE_AU; // Supported
    dynamicParams->displayWidth  = displaywidth;             // Supported : Default value is zero
    dynamicParams->frameSkipMode = IVIDEO_NO_SKIP;// Not Supported: Set to default value
    return;
}
  
#ifdef DM648
#define EXT_MEM_BASE (0xE0000000)
#define EXT_MEM_SIZE (0x10000000)/*(0x02000000)*/
#else
#define EXT_MEM_BASE (0x80B0C000)
#define EXT_MEM_SIZE (0x01000000)/*(0x02000000)*/
#endif

 
/* Cache Settings */
XDAS_Void TestApp_EnableCache(void)
{
#ifndef  MSVC  
	BCACHE_Size size;
    
	size.l1psize  = BCACHE_L1_32K; /* L1P cache size */
#ifdef DM648
	size.l1dsize  = BCACHE_L1_32K; /* L1D cache size */
	size.l2size   = BCACHE_L2_256K; /* L2  cache size */
#else	
	size.l1dsize  = BCACHE_L1_16K; /* L1D cache size */
	size.l2size   = BCACHE_L2_64K; /* L2  cache size */
#endif
    
	/* Set L1P, L1D and L2 cache sizes */
    BCACHE_setSize(&size);
    
    /* Cache Enable External Memory Space */
	/* BaseAddr, length, MAR enable/disable */
    /* Cache 0x80000000 --- 0x8FFFFFFF   */
    BCACHE_setMar((Ptr *)EXT_MEM_BASE, EXT_MEM_SIZE, BCACHE_MAR_ENABLE);      
    BCACHE_wbInvAll(); /* No need to call BCACHE_wait() */
#endif
} /* TestApp_EnableCache */
  
  
/******************************************************************************/
/*    Copyright (c) 2006 Texas Instruments, Incorporated                      */
/*    All Rights Reserved                                                     */
/******************************************************************************/
