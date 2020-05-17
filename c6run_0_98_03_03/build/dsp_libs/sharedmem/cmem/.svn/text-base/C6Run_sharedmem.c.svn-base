/*
 * C6Run_sharedmem.c
 */

/*
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 */

/* 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*  ----------------------------------- Standard C Headers            */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/*  ----------------------------------- Application headers           */
#include "c6run.h"
#include "debug_common.h"
#include "C6Run_common.h"

/*  ----------------------------------- This module's header          */
#include "C6Run_sharedmem.h"

/************************************************************
* Explicit External Declarations                            *
************************************************************/


/************************************************************
* Local Macro Declarations                                  *
************************************************************/


/************************************************************
* Local Typedef Declarations                                *
************************************************************/


/************************************************************
* Local Function Declarations                               *
************************************************************/


/************************************************************
* Local Variable Definitions                                *
************************************************************/


/************************************************************
* Global Variable Definitions                               *
************************************************************/

volatile bool     C6RUN_SHAREDMEM_initialized         = false;

void             *C6RUN_SHAREDMEM_localBaseAddr       = NULL;
void             *C6RUN_SHAREDMEM_remoteBaseAddr      = NULL;

void             *C6RUN_SHAREDMEM_localCmemBaseAddr   = NULL;
void             *C6RUN_SHAREDMEM_localCmemEndAddr    = NULL;
void             *C6RUN_SHAREDMEM_localCodeBaseAddr   = NULL;
void             *C6RUN_SHAREDMEM_localCodeEndAddr    = NULL;

int32_t           C6RUN_SHAREDMEM_binBitShift         = 0;
uint32_t          C6RUN_SHAREDMEM_binSize             = 0;

void             *C6RUN_SHAREDMEM_bufferTable[NUM_TABLE_BINS];


/************************************************************
* Global Function Definitions                               *
************************************************************/

// Memory subsystem initialization routines
int C6RUN_SHAREDMEM_init  ( void )
{
  int i;

  if (C6RUN_SHAREDMEM_initialized)
  {
    return C6RUN_SUCCESS;
  }
  
  VERBOSE_PRINT0("Entered C6RUN_SHAREDMEM_init()\n");
 
  // Init the C6RUN_MEM_bufferTable to NULL values
  
  for (i=0; i<NUM_TABLE_BINS; i++)
  {
    C6RUN_SHAREDMEM_bufferTable[i] = NULL;
  }

  C6RUN_SHAREDMEM_initialized = true;
  
  VERBOSE_PRINT0("Leaving C6RUN_SHAREDMEM_init()\n");
  
  return C6RUN_SUCCESS;
}

void C6RUN_SHAREDMEM_exit ( void )
{
  int i;
  
  if (!C6RUN_SHAREDMEM_initialized)
  {
    return;
  }
  
  VERBOSE_PRINT0("Entered C6RUN_SHAREDMEM_exit()\n");

  // Check for orphaned memory(leaks) and print it out
  for (i=0; i<NUM_TABLE_BINS; i++)
  {
    void *ptr = C6RUN_SHAREDMEM_bufferTable[i];
    while (ptr != NULL)
    {
      VERBOSE_PRINT2("Orphaned memory at bin %d, 0x%08X.\n",i,(intptr_t)ptr);
      ptr = (C6RUN_SHAREDMEM_getBuf(ptr))->next;
    }
  }

  C6RUN_SHAREDMEM_initialized = false;
  
  VERBOSE_PRINT0("Leaving C6RUN_SHAREDMEM_exit()\n");
}


// Memory helper routines
pC6RUN_SHAREDMEM_Buffer C6RUN_SHAREDMEM_getBuf(void * ptr)
{
  pC6RUN_SHAREDMEM_Buffer pBuffer = PTR2BUF(ptr);
  
  if (!IS_MALLOC_MAGIC(pBuffer->magic,ptr))
  {
    if ( IS_MEMALIGN_MAGIC((((uint32_t *)ptr)[-1]),ptr) )
    {
      // It appears to be a memaligned buffer
      pBuffer = ((void**)ptr)[-2];
      
      ptr = BUF2PTR(pBuffer);
      if (!IS_MALLOC_MAGIC(pBuffer->magic, ptr))
      {
        VERBOSE_PRINT0("C6RUN_SHAREDMEM_getBuf() failed because memalign magic number found, but no malloc magic number found: \n");
        VERBOSE_PRINT1("  pBuffer        = 0x%08X \n",(unsigned int) pBuffer);
        VERBOSE_PRINT1("  ptr            = 0x%08X \n",(unsigned int) ptr);
        VERBOSE_PRINT1("  pBuffer->magic = 0x%08X \n",(unsigned int) pBuffer->magic ^ ((unsigned int)ptr));
        pBuffer = NULL;
      }
    }
    else
    {
      VERBOSE_PRINT0("C6RUN_SHAREDMEM_getBuf() failed because no magic number found: \n");
      VERBOSE_PRINT1("  pBuffer        = 0x%08X \n",(unsigned int) pBuffer);      
      VERBOSE_PRINT1("  ptr            = 0x%08X \n",(unsigned int) ptr);
      VERBOSE_PRINT1("  ptr[-1]        = 0x%08X \n",(((uint32_t *)ptr)[-1]));
      VERBOSE_PRINT1("  ptr[-2]        = 0x%08X \n",(((uint32_t *)ptr)[-2]));
      pBuffer = NULL;
    }
  }
  
  // Do some additional checks to make sure the above checks are not returning
  // a false positive
#if defined(FAST_BUFFER_LOOKUP)
  if (pBuffer)
  {
    if (pBuffer->next)
    {
      // Make sure the pBuffer->next is in the valid range
      if ( ( ((uint32_t)pBuffer->next) < ((uint32_t)C6RUN_SHAREDMEM_localCmemBaseAddr) ) ||
           ( ((uint32_t)pBuffer->next) > ((uint32_t)C6RUN_SHAREDMEM_localCmemEndAddr) ) )
      {
        return NULL;
      }
    }
    
    if (GET_BUF_SIZE(pBuffer) >= (DSP_REGION_CMEM_SIZE + DSP_REGION_CODE_SIZE))
    {
      return NULL;
    }
  }
#endif
  
  return pBuffer;
}

void C6RUN_SHAREDMEM_insertInBin(pC6RUN_SHAREDMEM_Buffer buf, void *ptr, size_t size)
{
  uint32_t binNum, binCount, totalBinCount;
  
  if (buf == NULL)
  {
    return;
  }
  
  // Get the binNumber
  binNum = GET_BIN_NUMBER(ptr);
  
  VERBOSE_PRINT1("Insert - Bin: %d\n",binNum);
  VERBOSE_PRINT1("  Buffer:  0x%08X\n",(uint32_t)buf);
  VERBOSE_PRINT1("  Pointer: 0x%08X\n",(uint32_t)ptr);

  // Round the size variable up to the next multiple of the binSize
  size = (size + (C6RUN_SHAREDMEM_binSize - 1)) & ~(C6RUN_SHAREDMEM_binSize - 1);
  totalBinCount = size >> C6RUN_SHAREDMEM_binBitShift;
  binCount = 0;
  
  // For every bin but the last, just set the bin to the buffer.
  // We can do this since we know each bin is full with this one buffer.
  while ((++binCount) < totalBinCount)
  {
    C6RUN_SHAREDMEM_bufferTable[binNum] = ptr;
    binNum++;
  }

  // For the last buffer, we might have other ones already in that bin
  // so we need to handle this appropriatley
  buf->next = C6RUN_SHAREDMEM_bufferTable[binNum];
  C6RUN_SHAREDMEM_bufferTable[binNum] = ptr;
}

void C6RUN_SHAREDMEM_removeFromBin(pC6RUN_SHAREDMEM_Buffer buf, void *ptr, size_t size)
{
  uint32_t binNum, binCount, totalBinCount;
  pC6RUN_SHAREDMEM_Buffer pCurrBuffer, pNextBuffer;
  
  if ((ptr == NULL) || (buf == NULL) || (size == 0))
  {
    return;
  }
  
  // Get the binNumber
  binNum = GET_BIN_NUMBER(ptr);
  
  VERBOSE_PRINT2("Remove - Bin: %d,Pointer: 0x%08X\n",binNum,(uint32_t)ptr);
  
  // Round the size variable up to the next multiple of the binSize
  size = (size + (C6RUN_SHAREDMEM_binSize - 1)) & ~(C6RUN_SHAREDMEM_binSize - 1);
  totalBinCount = size >> C6RUN_SHAREDMEM_binBitShift;
  binCount = 1;
  
  // For every bin but the last, just set the bin to the NULL.
  // We can do this since we know each bin is used by this one buffer.
  while (binCount < totalBinCount)
  {
    C6RUN_SHAREDMEM_bufferTable[binNum] = NULL;
    binNum++;
    binCount++;
  }

  // For the last buffer, we might have other ones still in the bin
  // so we need to handle this case appropriately 
  
  // Simply go through the bin list and remove this one
  pCurrBuffer = C6RUN_SHAREDMEM_getBuf(C6RUN_SHAREDMEM_bufferTable[binNum]);
  if (ptr == C6RUN_SHAREDMEM_bufferTable[binNum])
  {
    if (pCurrBuffer)
    {
      C6RUN_SHAREDMEM_bufferTable[binNum] = pCurrBuffer->next;
      VERBOSE_PRINT1("  Found Buffer : 0x%08X\n",(uint32_t)pCurrBuffer);
    }
    else
    {
      VERBOSE_PRINT0("  Bin buffer mismatch: \n");
      VERBOSE_PRINT1("    Buffer : 0x%08X\n",(uint32_t)pCurrBuffer);
    }
  }
  else
  {
    while (pCurrBuffer->next != NULL)
    {
      pNextBuffer = C6RUN_SHAREDMEM_getBuf(pCurrBuffer->next);
      if (pNextBuffer)
      {
        if (ptr == pCurrBuffer->next)
        {
          pCurrBuffer->next = pNextBuffer->next;
          VERBOSE_PRINT1("  Skip Buffer : 0x%08X\n",(uint32_t)pCurrBuffer);
          VERBOSE_PRINT1("  Found Buffer: 0x%08X\n",(uint32_t)pNextBuffer);
          break;
        }
        VERBOSE_PRINT1("  Skip Buffer : 0x%08X\n",(uint32_t)pCurrBuffer);
        pCurrBuffer = pNextBuffer;
      }
      else
      {
        VERBOSE_PRINT0("  Bin buffer mismatch: \n");
        VERBOSE_PRINT1("    Buffer : 0x%08X\n",(uint32_t)pNextBuffer);
        break;
      }
    }
  }
}

void *C6RUN_SHAREDMEM_lookupBuffer(void *ptr, size_t *buffSize)
{
  uint32_t binNum;
  void *currPtr;
  uint32_t base = 0, size = 0;
  bool bufferFound = false;
  pC6RUN_SHAREDMEM_Buffer pBuf = NULL;
  
  VERBOSE_PRINT1("C6RUN_SHAREDMEM_lookupBuffer(0x%08X,...)\n",(uint32_t)ptr);

  binNum = GET_BIN_NUMBER(ptr);
  currPtr = C6RUN_SHAREDMEM_bufferTable[binNum];

  VERBOSE_PRINT1("C6RUN_SHAREDMEM_lookupBuffer: binNum = %d \n",binNum);
  
  if (ptr)
  {
  #if defined(FAST_BUFFER_LOOKUP)
    pBuf = C6RUN_SHAREDMEM_getBuf(ptr);
    if (pBuf != NULL)
    {
      VERBOSE_PRINT0("C6RUN_SHAREDMEM_lookupBuffer: pBuf fast lookup succeeded.\n");
    
      *buffSize = GET_BUF_SIZE(pBuf) - (((uint32_t)ptr) - ((uint32_t)pBuf));
      return ((void *)ptr);  
    }
  #endif  
  
    // Check to make sure we have a populated bin
    if (currPtr)
    {
      // Search through buffer table bin for a match. A match is the
      // input pointer inside the range of an allocated buffer
      while ((currPtr != NULL) && (!bufferFound))
      {
        base = (uint32_t) currPtr;
        pBuf = C6RUN_SHAREDMEM_getBuf(currPtr);
        
        if (pBuf == NULL)
        {
          VERBOSE_PRINT0("C6RUN_SHAREDMEM_lookupBuffer: pBuf lookup failed.\n");
          exit(0);
        }
      
        // Get size from dlmalloc header data (chunk size minus dlmalloc 
        // header size minus C6RUN_SHAREDMEM_Buffer size minus any filler)
        size = GET_BUF_SIZE(pBuf) - (((uint32_t)currPtr) - ((uint32_t)pBuf));
      
        if ((base <= ((uint32_t) ptr)) && (((uint32_t) ptr) < (base + size)))
        {
          VERBOSE_PRINT1("  Found Buffer : 0x%08X\n",(uint32_t)pBuf);
          bufferFound = true;
          break;
        }
        
        VERBOSE_PRINT1("  Skip Buffer : 0x%08X\n",(uint32_t)pBuf);
        
        currPtr = pBuf->next;
      }
      
      if (!bufferFound)
      {
        size = 0;
        base = 0;
      }
    }
  }
  
  VERBOSE_PRINT1("C6RUN_SHAREDMEM_lookupBuffer returns base   = 0x%08X \n",base);
  VERBOSE_PRINT1("C6RUN_SHAREDMEM_lookupBuffer returns size   = 0x%08X \n",size);
  
  *buffSize = size;
  return ((void *)base);
}

void C6RUN_SHAREDMEM_printStats(void)
{
  uint32_t usedBinCount = 0, lastUsedBin=0;
  uint32_t totalUsedMemory = 0, binUsedMemory = 0;
  int i;
  
  // Make sure the memory system is initialized
  if (!C6RUN_SHAREDMEM_initialized)
  {
    C6RUN_SHAREDMEM_init();
  }
  
  printf("C6RUN_SHAREDMEM Statistics:\n");
  
  for (i = 0; i<NUM_TABLE_BINS; ++i)
  {
    void *ptr = C6RUN_SHAREDMEM_bufferTable[i];
    if (ptr)
    {
      ++usedBinCount;
      lastUsedBin = i;
      binUsedMemory = 0;
      
      while (ptr)
      {
        pC6RUN_SHAREDMEM_Buffer buf = C6RUN_SHAREDMEM_getBuf(ptr);
        binUsedMemory += GET_BUF_SIZE(buf) - (((intptr_t)ptr) - ((intptr_t)buf));
        ptr = buf->next;
      }
      totalUsedMemory += binUsedMemory;
      printf("\tBin %d: Memory used = 0x%08X\n",i,binUsedMemory);
    }
  }
 
  printf("\n\tLast Used Bin = %d\n",lastUsedBin);
  printf("\tTotal Bins Used = %d\n",usedBinCount);
  printf("\tTotal memory used = 0x%08X\n",totalUsedMemory);
}

void *C6RUN_SHAREDMEM_local2Remote(void *localPtr)
{
  return (void *) ((((intptr_t)localPtr) - (intptr_t)C6RUN_SHAREDMEM_localBaseAddr) + (intptr_t)C6RUN_SHAREDMEM_remoteBaseAddr);
}

void *C6RUN_SHAREDMEM_remote2Local(void *remotePtr)
{
  return (void *) ((((intptr_t)remotePtr) - (intptr_t)C6RUN_SHAREDMEM_remoteBaseAddr) + (intptr_t)C6RUN_SHAREDMEM_localBaseAddr);
}



/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
