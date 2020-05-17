/*
 * malloc.c
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

/************************************************************************
*  Redefines the C standard library memory allocation routines so that  *
*  they now use the Shared Memory functionality underlying C6Run.       *
************************************************************************/

/*  ----------------------------------- Standard Headers              */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/*  ----------------------------------- Linux Posix Headers           */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

/*  ----------------------------------- Application headers             */
#include "c6run.h"


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

/* Begin standard C memory allocation routines */
#undef malloc
void *malloc(size_t size)
{
  return C6RUN_MEM_malloc(size);
}

#undef calloc
void *calloc(size_t nelem, size_t elsize)
{
  return C6RUN_MEM_calloc(nelem,elsize);
}

#undef realloc
void *realloc(void *oldPtr, size_t size)
{
  return C6RUN_MEM_realloc(oldPtr,size);
}

#undef posix_memalign
int posix_memalign(void **memptr, size_t alignment, size_t size)
{
  *memptr = C6RUN_MEM_memalign(alignment, size);
  return 0;
}

#undef memalign
void *memalign(size_t boundary, size_t size)
{
  return C6RUN_MEM_memalign(boundary, size);
}

#undef valloc
void *valloc(size_t size)
{
  return C6RUN_MEM_memalign(getpagesize(), size);
}

// String duplication routines
#undef strdup
char *strdup(const char *s)
{
  size_t len = strlen (s) + 1;
  void *new = C6RUN_MEM_malloc (len);  

  if (new == NULL)
    return NULL;

  return (char *) memcpy (new, s, len);
}

#undef strndup
char *strndup(const char *s, size_t size)
{
  void *new = C6RUN_MEM_malloc (size + sizeof(char));
  size_t len = strlen (s);

  if (new == NULL)
    return NULL;
  
  if (len > size)
  {
    memcpy (new, s, size);
    ((char *)new)[size] = 0x0;
  }
  else
  {
    memcpy (new, s, len + sizeof(char));
  }
  return new;
}

// Memory free routines
#undef free
void free(void *ptr)
{
  C6RUN_MEM_free(ptr);
}

#undef cfree
void cfree(void *ptr)
{
  C6RUN_MEM_free(ptr);
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
