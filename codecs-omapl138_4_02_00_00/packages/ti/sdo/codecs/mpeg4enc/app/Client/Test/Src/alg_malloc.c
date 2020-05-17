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

/* -------------------------------------------------------------------------- */
/* Assigning text section to allow better control on placing function in      */
/* memory of our choice and our alignment. The details about placement of     */
/* these section can be obtained from the linker command file "mpeg4enc.cmd". */
/* -------------------------------------------------------------------------- */

#ifdef SETUP_VC
#include <std.h>
#else
#include <ti/bios/include/std.h> /* Take from Bios*/
#endif
#include <alg.h>

#ifdef SETUP_VC
#include <ialg.h>
#else
#include <ti/xdais/ialg.h>
#endif


#include <stdlib.h>     /* malloc/free declarations */
#include <string.h>     /* memset declaration */

#if (defined SETUP_DAVINCI || defined SETUP_DM642 || defined OMAP2430_SPECIFIC )
/*The CODE_SECTION pragma is used to link code into an area separate from the.text section.*/
#pragma CODE_SECTION(ALG_init, ".text:init")
/*The CODE_SECTION pragma is used to link code into an area separate from the.text section.*/
#pragma CODE_SECTION(ALG_exit, ".text:exit")
/*The CODE_SECTION pragma is used to link code into an area separate from the.text section.*/
#pragma CODE_SECTION(_ALG_allocMemory, ".text:create")
/*The CODE_SECTION pragma is used to link code into an area separate from the.text section.*/
#pragma CODE_SECTION(_ALG_freeMemory, ".text:create")
/*The CODE_SECTION pragma is used to link code into an area separate from the.text section.*/
#pragma DATA_SECTION( internalDataMemory, ".intDataMem");
/*The CODE_SECTION pragma is used to link code into an area separate from the.text section.*/
#pragma DATA_SECTION( ExternalGlobalMem, ".extDataMem");
#define myMemalign  memalign

#ifdef DAVINCI_DSP_ONLY
#define L1D_MEM_SIZE		(64*1024)
#define INTERNAL_DATA_MEM_SIZE    (L1D_MEM_SIZE)
#else
#define L1D_MEM_SIZE		(52*1024)
#define INTERNAL_DATA_MEM_SIZE    (L1D_MEM_SIZE)
#endif

#else
void *memAlignVc(size_t alignment, size_t size);
#define myMemalign  memAlignVc
#define INTERNAL_DATA_MEM_SIZE    (0x100000)
#endif

#define myFree      free

/* list of some parameters macros for worst case memory allocation in external memory */
#define EXTERNAL_DATA_MEM_SIZE    (16*1024*1024)


unsigned char  ExternalGlobalMem[EXTERNAL_DATA_MEM_SIZE];             
unsigned char  *ExternalGlobalMemPool = ExternalGlobalMem;   
unsigned int   externalDataMemorySize = EXTERNAL_DATA_MEM_SIZE;

unsigned char internalDataMemory[ INTERNAL_DATA_MEM_SIZE];
unsigned char *pInternalDataMemory = internalDataMemory;
unsigned int  internalDataMemorySize = INTERNAL_DATA_MEM_SIZE;


Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n);
Void _ALG_freeMemory(IALG_MemRec memTab[], Int n);

void * memAlignVc(size_t alignment, size_t size)
{
  return malloc( size);
} /* memAlignVc */


int allocateMemTabRequest( IALG_MemRec *memTab)
{
  if( memTab->space == IALG_EXTERNAL ) {
    unsigned int  alignBytes;
    alignBytes = (((unsigned int) ExternalGlobalMemPool + (memTab->alignment - 1)) & (~ (memTab->alignment - 1)));
    alignBytes -= (unsigned int) ExternalGlobalMemPool;
    ExternalGlobalMemPool += alignBytes;
    externalDataMemorySize -= alignBytes;
    if(externalDataMemorySize >= memTab->size) {
      /* allocate memory */
      memTab->base = ExternalGlobalMemPool;
      ExternalGlobalMemPool += memTab->size;
      externalDataMemorySize -= memTab->size;
    } else {
      memTab->base = 0;
    }
  } else {
    /* internal memory request */
    unsigned int  alignBytes;
    alignBytes = (((unsigned int) pInternalDataMemory + (memTab->alignment - 1)) & (~ (memTab->alignment - 1)));
	if(alignBytes!=0)
	{
	    alignBytes -= (unsigned int) pInternalDataMemory;
	    pInternalDataMemory += alignBytes;
	    internalDataMemorySize -= alignBytes;
	}
    if(internalDataMemorySize >= memTab->size) 
    {
		/* allocate memory */
		memTab->base = pInternalDataMemory;
		pInternalDataMemory += memTab->size;
		internalDataMemorySize -= memTab->size;
    } else {
		memTab->base = 0;
    }
  }
  return 0;
} /* allocateMemTabRequest */

int freeMemTabRequest( IALG_MemRec  *memTab)
{
  if( memTab->space == IALG_EXTERNAL ) {
    memTab->base = 0;
    ExternalGlobalMemPool = ExternalGlobalMem;
    externalDataMemorySize = EXTERNAL_DATA_MEM_SIZE ;
  
  } else {
    /* internal memory  free request. XXX see the code  below 
     * for the dangers of calling them as normal mallocs. Free is faked!!!  */
    memTab->base = 0;
    pInternalDataMemory = internalDataMemory;
    internalDataMemorySize = INTERNAL_DATA_MEM_SIZE;
  }
  return 0;
} /* freeMemTabRequest */



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
    
    for (i = 0; i < n; i++) {
      /* XXX changing the code here, to change the memory allocator for
       * different requirements. */
      allocateMemTabRequest( &memTab[i]);

        if (memTab[i].base == NULL) {
            _ALG_freeMemory(memTab, i);
            return (FALSE);
        }

    }

    return (TRUE);
}


/*
 *  ======== _ALG_freeMemory ========
 */
Void _ALG_freeMemory(IALG_MemRec memTab[], Int n)
{
    Int i;
    
    for (i = 0; i < n; i++) {
        if (memTab[i].base != NULL) {
          /* XXX changing code here too. to take care of internal memory
           * allocatiuons */
            freeMemTabRequest( &memTab[i]);

        }
    }
}


/* ========================================================================== */
/* End of file : alg_malloc.c                                                 */
/* -------------------------------------------------------------------------- */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/* ========================================================================== */

