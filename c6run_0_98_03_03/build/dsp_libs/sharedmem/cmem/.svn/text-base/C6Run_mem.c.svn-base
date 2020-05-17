/*
 * C6Run_mem.c
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

/*  ----------------------------------- Application headers           */
#include "c6run.h"
#include "debug_common.h"
#include "C6Run_common.h"
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


/************************************************************
* Global Function Definitions                               *
************************************************************/

// C6Run memory buffer management routines
void *C6RUN_MEM_malloc(size_t size)
{
  void *tmpPtr;
  
  VERBOSE_PRINT1("C6RUN_MEM_malloc(%d)\n",size);
  
  tmpPtr = C6RUN_MEM_memalign(MIN_ALIGNMENT,size);

  VERBOSE_PRINT1("C6RUN_MEM_malloc returns = 0x%08X\n",(uint32_t)tmpPtr);
  
  return tmpPtr;
}

void *C6RUN_MEM_realloc(void *oldPtr,size_t size)
{
  size_t totalSize;
  void *tmpPtr = NULL;
  pC6RUN_SHAREDMEM_Buffer pBuffer;
  
  VERBOSE_PRINT2("C6RUN_MEM_realloc(0x%08X,%d)\n",(uint32_t)oldPtr,size);
  
  if (!C6RUN_SHAREDMEM_initialized)
  {
    C6RUN_SHAREDMEM_init();
  }
  
  if (oldPtr == NULL)
  {
    return C6RUN_MEM_malloc(size);
  }
  
  // Calculate total size
  totalSize = (size + sizeof(C6RUN_SHAREDMEM_Buffer));
  totalSize = C6RUN_SHAREDMEM_ALIGN(totalSize);
  
  // Get the buffer
  pBuffer = C6RUN_SHAREDMEM_getBuf(oldPtr);
  if (pBuffer == NULL)
  {
    return NULL;
  }
  
  // Realloc the buffer
  tmpPtr = realloc((void *)pBuffer,totalSize);
  
  if (tmpPtr)
  {
    pBuffer = (pC6RUN_SHAREDMEM_Buffer) tmpPtr;
    tmpPtr = BUF2PTR(pBuffer);
    // If the new pointer is not the same as the old one, we have
    // some updates we need to do in the buffer table
    if (tmpPtr != oldPtr)
    {
      C6RUN_MEM_free(oldPtr);
      
      pBuffer->next = NULL;
      pBuffer->magic = SET_MALLOC_MAGIC(tmpPtr);
      
      C6RUN_SHAREDMEM_MALLOC_LOCK();
      
      // Add new pointer/buffer to table
      C6RUN_SHAREDMEM_insertInBin(pBuffer, tmpPtr, size);
      
      C6RUN_SHAREDMEM_MALLOC_UNLOCK();
    }
  }
  
  VERBOSE_PRINT1("C6RUN_MEM_realloc returns = 0x%08X\n",(uint32_t)tmpPtr);
  
  return tmpPtr;
}

void *C6RUN_MEM_memalign(size_t alignment, size_t size)
{
  size_t totalSize;
  void *buf = NULL, *ptr = NULL;
  uint32_t startMemPtr, endMemPtr;
  
  VERBOSE_PRINT2("C6RUN_MEM_memalign(%d,%d)\n",alignment,size);

  if (!C6RUN_SHAREDMEM_initialized)
  {
    C6RUN_SHAREDMEM_init();
  }
  
  // Check for non power-of-two alignment
  if ((alignment & (alignment - 1)))
  {
    VERBOSE_PRINT0("C6RUN_MEM_memalign: Error, invalid alignment!\n");
    return NULL;
  }
  
  // Aligment is at least MIN_ALIGNMENT<<1 bytes
  totalSize   = (alignment-MIN_ALIGNMENT) + size + sizeof(C6RUN_SHAREDMEM_Buffer);
  totalSize   = C6RUN_SHAREDMEM_ALIGN(totalSize);
  
  // Get the allocated data from the dlmalloc implementation
  buf = malloc(totalSize);
  if (buf == NULL)
  {
    return NULL;
  }
    
  startMemPtr = (uint32_t) BUF2PTR(buf);
  endMemPtr   = (uint32_t) (startMemPtr + size - 1);  
  
  if (GET_BIN_NUMBER(startMemPtr) != GET_BIN_NUMBER(endMemPtr))
  {
    // If the buffer spans multiple bins then we need to align
    // our buffer so that the startMemPtr sits at the start of the 
    // next bin
    VERBOSE_PRINT1("Bin spanning buffer at 0x%08X\n",(uint32_t)buf);
  
    // Now resize the buffer previously malloced
    free(buf);
    buf = memalign(C6RUN_SHAREDMEM_binSize, totalSize);
    if (buf == NULL)
    {
      return NULL;
    }
    VERBOSE_PRINT1("  realigning to start of next bin at 0x%08X\n",(uint32_t)buf);
  }

  if (buf)
  {
    pC6RUN_SHAREDMEM_Buffer pBuffer = (pC6RUN_SHAREDMEM_Buffer) buf;
    intptr_t bufPtr;

    pBuffer->next = NULL;
    ptr = BUF2PTR(buf);
    pBuffer->magic = SET_MALLOC_MAGIC(ptr);

    bufPtr = (intptr_t)ptr;
    if (bufPtr & (alignment - 1))
    {
      // Align the pointer
      bufPtr += ((-bufPtr) & (alignment - 1));
      
      ptr = (void *)bufPtr;
    }
  
    if (BUF2PTR(buf) != ptr)
    {
      // There is always enough space for the base pointer and the memalign magic word
      ((uint32_t*)ptr)[-1] = SET_MEMALIGN_MAGIC(ptr);
      ((void**)ptr)[-2] = buf;
    }
  }
  
  if (ptr)
  {
    // Insert the pBuffer into the buffer table
    C6RUN_SHAREDMEM_MALLOC_LOCK();
    
    C6RUN_SHAREDMEM_insertInBin(buf, ptr, size);
    
    C6RUN_SHAREDMEM_MALLOC_UNLOCK();
  }
  
  VERBOSE_PRINT1("C6RUN_MEM_memalign returns = 0x%08X\n",(uint32_t)ptr);

  return ptr;
}

void *C6RUN_MEM_calloc(size_t nelem, size_t elsize)
{
  size_t totalSize;
  void*  tmpPtr;
  
  VERBOSE_PRINT2("C6RUN_MEM_calloc(%d,%d)\n",nelem,elsize);
  
  totalSize = nelem * elsize;
  tmpPtr  = C6RUN_MEM_memalign(MIN_ALIGNMENT,totalSize);
  if (tmpPtr != NULL)
  {
    memset(tmpPtr, 0x00, totalSize);
  }
  
  VERBOSE_PRINT1("C6RUN_MEM_calloc returns = 0x%08X\n",(uint32_t)tmpPtr);
  
  return tmpPtr;
}

void C6RUN_MEM_free(void* ptr)
{
  VERBOSE_PRINT1("C6RUN_MEM_free(0x%08X)\n",(uint32_t)ptr);

  if (!C6RUN_SHAREDMEM_initialized)
  {
    C6RUN_SHAREDMEM_init();
  }
  
  if (ptr)
  {
    pC6RUN_SHAREDMEM_Buffer pBuffer = C6RUN_SHAREDMEM_getBuf(ptr);
    
    if (pBuffer)
    {
      size_t size = GET_BUF_SIZE(pBuffer) - (((intptr_t)ptr) - ((intptr_t)pBuffer));

      C6RUN_SHAREDMEM_MALLOC_LOCK();
    
      C6RUN_SHAREDMEM_removeFromBin(pBuffer, ptr, size);
    #if defined(FAST_BUFFER_LOOKUP)
      pBuffer->next   = NULL;
      pBuffer->magic  = NULL;
    #endif
      free((void *)pBuffer);
    
      C6RUN_SHAREDMEM_MALLOC_UNLOCK(); 
    }
  }
  
  VERBOSE_PRINT0("Leaving C6RUN_MEM_free.\n");
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
