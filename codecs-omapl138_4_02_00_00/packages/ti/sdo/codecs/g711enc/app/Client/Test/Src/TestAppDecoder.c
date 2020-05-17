/******************************************************************************/
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/******************************************************************************/

/*!
********************************************************************************
@file     TestAppDecoder.c
@brief    This is the top level client file that drives the G711
          Speech Decoder Call using XDM Interface
@author   Multimedia Codecs TI India
@version  0.1 - Jan 30,2006    initial version
********************************************************************************
*/

/* Standard C header files */
#include <stdio.h>
#include <std.h>

/* Client header file */
#include "TestApp.h"
#include "TestAppDecoder.h"
#include "bcache.h"

/* Input buffer allocation */
XDAS_Int8   inputData[INPUT_BUFFER_SIZE];

/* Output buffer allocation */
XDAS_Int8   outputData[OUTPUT_BUFFER_SIZE];

/* Output Reference buffer allocation */
XDAS_Int8   refData[OUTPUT_BUFFER_SIZE];

/* Algorithm specific parameters */
XDAS_Int32  CompandingLaw;

/* Other variables */
XDAS_Int16 frame_size = NUMBER_OF_SAMPLES;

sTokenMapping sTokenMap[] =
{
  {"CompandingLaw", &CompandingLaw},
  {NULL,            NULL}
};

/* Main Function acting as a client for Speech Encode Call */
XDAS_Int32 main( )
{
  /* File I/O variables                                                       */
  FILE *fConfigFile, *ftestFile, *finFile, *fparamsFile;

  XDAS_Int8 line[STRING_SIZE], inFile[STRING_SIZE], testFile[STRING_SIZE];
  XDAS_Int8 paramsFile[STRING_SIZE], testCompliance;
  XDAS_Int8 *fname = "..\\..\\Test\\TestVecs\\Config\\Testvecs_dec.cfg";

  /* Base Class Structures                                                    */
  ISPHDEC1_Params         params;
  ISPHDEC1_InArgs         inargs;
  ISPHDEC1_OutArgs        outargs;

  /* Algorithm specific handle                                                */
  ISPHDEC1_Handle         handle;

  /* Input/Output Buffer Descriptors                                          */
  XDM1_SingleBufDesc      inBufs;
  XDM1_SingleBufDesc      outBufs;

  /* Other variables                                                          */
  XDAS_Int32 inBytesRead, frameCount, retVal, testVal, countConfigSet;

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

    /* Set the Instance Creation Parameters in params */
    params.size                 = sizeof (ISPHDEC1_Params);
    params.compandingLaw        = CompandingLaw;

    /* Set the Input Arguments structure */
    inargs.size                 = sizeof (ISPHDEC1_InArgs);
    inargs.frameType            = ISPHDEC1_FTYPE_SPEECHGOOD;

    /* Set the Output Arguments structure */
    outargs.size                = sizeof (ISPHDEC1_OutArgs);

    /* Initializing Variables                                                */
    frameCount                  = 0;
    
    /* Create the Algorithm object (instance)                                */
    printf("\nCreating Algorithm Instance...");
    
      if ((handle =  (ISPHDEC1_Handle) ALG_create (
                                      (IALG_Fxns *) &G711DEC_TII_IG711DEC,
                                      (IALG_Handle) NULL,
                                      (IALG_Params *) &params)) == NULL)
      {
        printf("\nFailed to Create Instance... Exiting for this configuration..");
        continue;
      }
    printf("\nAlgorithm Instance Creation Done...\n");

    /* Set operating frame size                                               */
    frame_size        = NUMBER_OF_SAMPLES;

    /*Fill up the buffers as required by algorithm                            */
    inBufs.buf      = inputData;
    inBufs.bufSize  = frame_size;

    outBufs.buf     = outputData;

    /* Do-While Loop for Encode Call                                          */
    do
    {
      /* Update the frame counter                                             */
      frameCount++;

      /* Read from Input File                                                 */
      inBytesRead = TestApp_ReadInputData(finFile, 
                                          &inBufs);

      /* Cache Invalidate for Input Buffer */
      BCACHE_inv(inBufs.buf, inBufs.bufSize, TRUE);

      if(inBytesRead != inBufs.bufSize)
      {
        inBufs.bufSize  = inBytesRead;
      }

      if(inBytesRead < 0)
        break;

      asm(" nop ");
      asm(" nop ");
      asm(" nop ");

      /* basic G711 decoding call */
      retVal = handle->fxns->process (handle,
                                      &inBufs,
                                      &outBufs,
                                      &inargs,
                                      &outargs);

      asm(" nop ");
      asm(" nop ");
      asm(" nop ");

      if(retVal == XDM_EFAIL)
      {
        printf("\n Process function returned an Error...  ");
        break; /* Error Condition: Application may want to break off         */
      }

      /* Cache Writeback Invalidate for Output Buffers                        */
      BCACHE_wbInv(outBufs.buf, outBufs.bufSize, TRUE);

      if(testCompliance)
      {
        /* Compare the output frames with the Reference File               */
        testVal = TestApp_CompareOutputData(ftestFile, &outBufs, &outargs);
        
        if(testVal == XDM_EOK)
        {
          /* Test Compliance Passed... Continuing...                       */
          printf("\n Decoder compliance test passed for frame # %d",frameCount);
        }
        else 
        {
          /* Test Compliance Failed... Breaking...                         */
          printf("\n Decoder compliance test failed for frame # %d",frameCount);
          break;
        }
      }
      else
      {
        /* Write the output frames in the display order                    */
        TestApp_WriteOutputData(ftestFile, &outBufs, &outargs);
        printf("\n Decoder output dump completed for frame # %d",frameCount);
      }
    } while(inBytesRead > 0);   /* end of Do-While loop                          */

    /* Destroy the handle instance */
    ALG_delete ((ISPHDEC1_Handle)handle);

    printf("\n --------------  SUMMARY --------------------\n");
    printf("\t Total number of Frames     = %d\n",frameCount);
    printf (" --------------    END   --------------------\n");    

    fclose(finFile);
    fclose(ftestFile);
  }  /* Read the Config File until it reaches the end of file */

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
                                  XDM1_SingleBufDesc *inputBufDesc)
{
    XDAS_Int32 bytesRead;

    /*Read the input buffers from FILE                                      */
    bytesRead = fread(inputBufDesc->buf,
                      1, 
                      inputBufDesc->bufSize,
                      finFile);

    /* Make sure that external memory contain correct copy of input data    */
    BCACHE_wbInv(inputBufDesc->buf,
                  inputBufDesc->bufSize,
                  TRUE);

    if(bytesRead != inputBufDesc->bufSize)
    {
      bytesRead = -1 ;
    }

    return (bytesRead);
}

/*
//============================================================================
// TestApp_CompareOutputData
//  Comparing Output Data with Reference File data
*/

XDAS_Int32 TestApp_CompareOutputData(FILE *fRefFile, 
                                      XDM1_SingleBufDesc *outputBufDesc, 
                                      ISPHDEC1_OutArgs *outArgs)
{
  XDAS_Int32 bufferSize, retVal;
  XDAS_UInt8 *outputData;
  
  retVal = XDM_EOK;
  
  outputData = (XDAS_UInt8 *)outputBufDesc->buf;
  bufferSize = outputBufDesc->bufSize;
  fread (refData, 1, bufferSize, fRefFile);
  if(memcmp(outputData, refData, bufferSize)) {
    retVal = XDM_EFAIL;
  }

  return retVal;
}

/*
//============================================================================
// TestApp_WriteOutputData
//  Writing Output Data in a File
*/

XDAS_Void TestApp_WriteOutputData(FILE *fOutFile, 
                                  XDM1_SingleBufDesc *outputBufDesc, 
                                  ISPHDEC1_OutArgs *outArgs)
{
  
  XDAS_UInt8 *s;
  
  s = (XDAS_UInt8 *)outputBufDesc->buf;
  fwrite (s, sizeof (Byte), outputBufDesc->bufSize, fOutFile);
  
  fflush (fOutFile);
  return;
}

/*
//============================================================================
// byteSwapBitStream
//  Swap the bytes of the input buffer
*/

XDAS_Void byteSwapBitStream (XDAS_Int16 *p, XDAS_Int16 size)
{
 XDAS_Int16 i;
 for(i=0; i<size; i++)
    p[i] = ((p[i] & 0x00FF) << 8) | ((p[i] & 0xFF00) >> 8);
}

/*
//============================================================================
// TestApp_EnableCache
//  Initialization of the Cache
*/

#define EXT_MEM_BASE (0x80000000)
#define EXT_MEM_SIZE (0x10000000)

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
} /* DoCacheSettings */

/* Nothing past this point */
