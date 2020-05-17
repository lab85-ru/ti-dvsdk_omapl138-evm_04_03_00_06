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
/*
static const char Copyright[] = "Copyright (C) 2003 Texas Instruments "
                                "Incorporated. All rights Reserved."; 
*/


#include <ti/bios/include/std.h>
#include <ti/sdo/fc/utils/api/alg.h>
#include <ti/xdais/ialg.h>

#include <stdlib.h>     /* malloc/free declarations */
#include <string.h>     /* memset declaration */
#include "ih264vdec.h"

#define INTERNAL_DATA_MEM_SIZE   0x10000 /* Internal Scratch Buffer */

#ifdef _TMS320C6400
#define myMemalign  memalign
#else  /* ~_TMS320C6400 */
void *memAlignVc(size_t alignment, size_t size);
#define myMemalign  memAlignVc
#endif /* ~_TMS320C6400 */

#define myFree      free

#ifndef TEST_VC_APP
#pragma DATA_SECTION(internalDataMemory, "UDRAM_INTMEM")
#endif

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
    /* external memory request >-> do the normal way */
    memTab->base = (void *)myMemalign(memTab->alignment, memTab->size);
  } else {
    /* internal memory request */
    unsigned int  alignBytes;
    alignBytes = (((unsigned int) pInternalDataMemory + (memTab->alignment - 1)) & ~ (memTab->alignment - 1));
    alignBytes -= (unsigned int) pInternalDataMemory;
    pInternalDataMemory += alignBytes;
    internalDataMemorySize -= alignBytes;
    if(internalDataMemorySize >= memTab->size) {
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
    /* external memory request >-> do the normal way */
    myFree( memTab->base);
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
        /* memTab[i].base = (void *)myMemalign(memTab[i].alignment, memTab[i].size); */
     
	    //printf("\n memtab %d \t Size = %d \t Attrs %d",i,memTab[i].size,memTab[i].attrs);

        if (memTab[i].base == NULL) {
            _ALG_freeMemory(memTab, i);
            return (FALSE);
        }
        //memset(memTab[i].base, 0, memTab[i].size);
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
            /* myFree(memTab[i].base); */
        }
    }
}

/* ========================================================================== */
/* End of file : alg_malloc.c                                                 */
/* -------------------------------------------------------------------------- */
/*            Copyright (c) 2003 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/* ========================================================================== */

