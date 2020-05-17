/* ========================================================================== */
/*                                                                            */
/*  TEXAS INSTRUMENTS, INC.                                                   */
/*                                                                            */
/*  NAME                                                                      */
/*     alg_malloc.c                                                           */
/*                                                                            */
/*  DESCRIPTION                                                               */
/*    This module implements an algorithm memory management "policy" in which */
/*  no memory is shared among algorithm objects.  Memory is, however          */
/*  reclaimed when objects are deleted.                                       */
/*                                                                            */
/*  preemption      sharing             object deletion                       */
/*  ----------      -------             ---------------                       */
/*  yes(*)          none                yes                                   */
/*                                                                            */
/*  Note 1: this module uses run-time support functions malloc() and free()   */
/*  to allocate and free memory.  Since these functions are *not* reentrant,  */
/*  either all object creation and deletion must be performed by a single     */
/*  thread or reentrant versions or these functions must be created.          */
/*                                                                            */
/*  COPYRIGHT NOTICES                                                         */
/*   Copyright (C) 1996, MPEG Software Simulation Group. All Rights           */
/*   Reserved.                                                                */
/*                                                                            */
/*   Copyright (c) 2001-2003 Texas Instruments Inc.  All rights reserved.     */
/*   Exclusive property of the Video & Imaging Products, Emerging End         */
/*   Equipment group of Texas Instruments India Limited. Any handling,        */
/*   use, disclosure, reproduction, duplication, transmission, or storage     */
/*   of any part of this work by any means is subject to restrictions and     */
/*   prior written permission set forth in TI's program license agreements    */
/*   and associated software documentation.                                   */
/*                                                                            */
/*   This copyright notice, restricted rights legend, or other proprietary    */
/*   markings must be reproduced without modification in any authorized       */
/*   copies of any part of this work.  Removal or modification of any part    */
/*   of this notice is prohibited.                                            */
/*                                                                            */
/*   U.S. Patent Nos. 5,283,900  5,392,448                                    */
/* -------------------------------------------------------------------------- */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/* ========================================================================== */
/* "@(#) XDAS 2.12 05-21-01 (__imports)" */
static const char Copyright[] = "Copyright (C) 2003 Texas Instruments "
                                "Incorporated. All rights Reserved."; 

/* -------------------------------------------------------------------------- */
/* Assigning text section to allow better control on placing function in      */
/* memory of our choice and our alignment. The details about placement of     */
/* these section can be obtained from the linker command file "mpeg2enc.cmd". */
/* -------------------------------------------------------------------------- */
/* Standard C header files                                                    */
#include <stdio.h>
#include <std.h>
#include <alg.h>
//#include <ialg.h>

#include <stdlib.h>     /* malloc/free declarations */
#include <string.h>     /* memset declaration */
extern int total_size;

#if defined (_54_) || (_55_) || (_28_)
void *mem_align(size_t alignment, size_t size);
void mem_free(void *ptr);
#define myMemalign  mem_align
#define myFree      mem_free
#else
#define myMemalign  memalign
#define myFree      free
#endif
#define CACHE_ALIGN_SIZE  128

/*-------------------------------------------*/
/* Create a 4k buffer to hold an aligned     */
/* heap for the input buffer.                */
/*-------------------------------------------*/
                      
#ifdef ENABLE_QDMA
#pragma DATA_SECTION( internalDataMemory, ".intDataMem");
#endif
#define INTERNAL_DATA_MEM_SIZE    (0x2000)


unsigned char internalDataMemory[ INTERNAL_DATA_MEM_SIZE];
unsigned char *pInternalDataMemory = internalDataMemory;
unsigned int  internalDataMemorySize = INTERNAL_DATA_MEM_SIZE;


Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n);
Void _ALG_freeMemory(IALG_MemRec memTab[], Int n);

/*
 *  ======== ALG_activate ========
 */
Void ALG_activate(ALG_Handle alg)
{
    /* restore all persistant shared memory */
        ;   /* nothing to do since memory allocation never shares any data */
    
    /* do app specific initialization of scratch memory */
    if (alg->fxns->algActivate != NULL) {
        alg->fxns->algActivate(alg);
    }
}


/*
 *  ======== ALG_deactivate ========
 */
Void ALG_deactivate(ALG_Handle alg)
{
    /* do app specific store of persistent data */
    if (alg->fxns->algDeactivate != NULL) {
        alg->fxns->algDeactivate(alg);
    }

    /* save all persistant shared memory */
        ;   /* nothing to do since memory allocation never shares any data */
    
}

/*
 *  ======== ALG_exit ========
 */
Void ALG_exit(Void)
{
}

/*
 *  ======== ALG_init ========
 */
Void ALG_init(Void)
{
}


/*
 *  ======== _ALG_allocMemory ========
 */
Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n)
{
    Int i;
	Int Size_external_persist,Size_Internal_persist;
	Int Size_external_Scratch,Size_Internal_Scratch;
 
     FILE *Memory;
    
#ifdef _TMS320C6400
  extern far Int __cacheAlignHeap__;
#endif 
    
    Memory = fopen("Memory.txt","w");

    Size_external_persist = 0;
    Size_Internal_persist = 0;
    Size_external_Scratch = 0;
    Size_Internal_Scratch  = 0;

    for (i = 0; i < n; i++) {
#ifndef _TMS320C6400
        memTab[i].base = malloc(memTab[i].size);
		total_size += memTab[i].size;
		if (memTab[i].space != IALG_EXTERNAL)
		{
			if( memTab[i].attrs == IALG_PERSIST)
			{
				fprintf(Memory," \t Memtab No. %d Internal_persist ",i);
				fprintf(Memory," \t size %d \n ",memTab[i].size);
				Size_Internal_persist = Size_Internal_persist + memTab[i].size;
			}
			else
			{
				fprintf(Memory," \t Memtab No. %d Internal_Scratch ",i);
				fprintf(Memory," \t size %d \n ",memTab[i].size);
				Size_Internal_Scratch = Size_Internal_Scratch + memTab[i].size;
				
			}
			
		}
		else
		{
			if( memTab[i].attrs == IALG_PERSIST)
			{
				fprintf(Memory," \t Memtab No. %d External_persist ",i);
				fprintf(Memory," \t size %d \n ",memTab[i].size);
				Size_external_persist = Size_external_persist + memTab[i].size;
			}
			else
			{
				fprintf(Memory," \t Memtab No. %d External_Scratch ",i);
				fprintf(Memory," \t size %d \n ",memTab[i].size);
				Size_external_Scratch = Size_external_Scratch + memTab[i].size;
				
			}
		}
#else        

        /* Hacking in something to be able to have a buffer
           aligned on a cache boundary. */
        if (memTab[i].space != IALG_EXTERNAL) {
//          memTab[i].base = alignedHeap;  
	   /* internal memory request */
	    unsigned int  alignBytes;
 		  if( memTab[i].attrs == IALG_PERSIST)
		  {
	      fprintf(Memory," \t Memtab No. %d Internal_persist ",i);
          fprintf(Memory," \t size %d \n ",memTab[i].size);
          Size_Internal_persist = Size_Internal_persist + memTab[i].size;
		  }
		  else
		  {
	      fprintf(Memory," \t Memtab No. %d Internal_Scratch ",i);
          fprintf(Memory," \t size %d \n ",memTab[i].size);
          Size_Internal_Scratch = Size_Internal_Scratch + memTab[i].size;

		  }

	    alignBytes = (((unsigned int) pInternalDataMemory + (memTab[i].alignment - 1)) & ~ (memTab[i].alignment - 1));
	    alignBytes -= (unsigned int) pInternalDataMemory;
	    pInternalDataMemory += alignBytes;
	    internalDataMemorySize -= alignBytes;
	    if(internalDataMemorySize >= memTab[i].size) {
	      /* allocate memory */
	      memTab[i].base = pInternalDataMemory;
	      pInternalDataMemory += memTab[i].size;
	      internalDataMemorySize -= memTab[i].size;
	    } else {
	      memTab[i].base = 0;
	    }
 
        }
        else {
		  memTab[i].base = (void *)myMemalign(memTab[i].alignment, memTab[i].size);
		  if( memTab[i].attrs == IALG_PERSIST)
		  {
	      fprintf(Memory," \t Memtab No. %d External_persist ",i);
          fprintf(Memory," \t size %d \n ",memTab[i].size);
          Size_external_persist = Size_external_persist + memTab[i].size;
		  }
		  else
		  {
	      fprintf(Memory," \t Memtab No. %d External_Scratch ",i);
          fprintf(Memory," \t size %d \n ",memTab[i].size);
          Size_external_Scratch = Size_external_Scratch + memTab[i].size;

		  }

		}
          
          

#endif        

        if (memTab[i].base == NULL) {
            _ALG_freeMemory(memTab, i);
            return (FALSE);
        }
        memset(memTab[i].base, 0, memTab[i].size);
    }
	
	fprintf(Memory," \n \n \t Size_external_persist %d in KByte %f \n ",Size_external_persist,(double)Size_external_persist/1024);
    fprintf(Memory," \t Size_external_Scratch %d  in KByte %f\n ",Size_external_Scratch,(double)Size_external_Scratch/1024);
	fprintf(Memory," \t Size_Internal_persist %d  in KByte %f\n ",Size_Internal_persist,(double)Size_Internal_persist/1024);
    fprintf(Memory," \t Size_Internal_Scratch %d  in KByte %f\n ",Size_Internal_Scratch,(double)Size_Internal_Scratch/1024);
	fprintf(Memory," \n \n \t Total External		 %d  in KByte %f\n ",Size_external_persist + Size_external_Scratch,(double)(Size_external_persist + Size_external_Scratch)/1024);
	fprintf(Memory," \t Total Internal		 %d  in KByte %f\n ",Size_Internal_persist + Size_Internal_Scratch,(double)(Size_Internal_persist + Size_Internal_Scratch)/1024);

    fclose(Memory);
    return (TRUE);
}

/*
 *  ======== _ALG_freeMemory ========
 */
Void _ALG_freeMemory(IALG_MemRec memTab[], Int n)
{
    Int i;
    
    for (i = 0; i < n; i++) {
	   if( memTab[i].space == IALG_EXTERNAL ) {
	    /* external memory request >-> do the normal way */
	    free( memTab[i].base);
	  } else {
	    /* internal memory  free request. XXX see the code  below 
	     * for the dangers of calling them as normal mallocs. Free is faked!!!  */
	    memTab[i].base = 0;
	    pInternalDataMemory = internalDataMemory;
	    internalDataMemorySize = INTERNAL_DATA_MEM_SIZE;
    }
}
}

/* ========================================================================== */
/* End of file : alg_malloc.c                                                 */
/* -------------------------------------------------------------------------- */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/* ========================================================================== */

