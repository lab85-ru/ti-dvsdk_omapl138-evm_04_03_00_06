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
/*   Copyright (c) 2006 Texas Instruments Inc.  All rights reserved.     */
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
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/* ========================================================================== */
/* "@(#) XDAS 2.12 05-21-01 (__imports)" */
//static const char Copyright[] = "Copyright (C) 2003 Texas Instruments "
//                                "Incorporated. All rights Reserved."; 

/* -------------------------------------------------------------------------- */
/* Assigning text section to allow better control on placing function in      */
/* memory of our choice and our alignment. The details about placement of     */
/* these section can be obtained from the linker command file "mpeg2enc.cmd". */
/* -------------------------------------------------------------------------- */
#ifdef _TMS320C6400

#pragma CODE_SECTION(ALG_init, ".text:init")
#pragma CODE_SECTION(ALG_exit, ".text:exit")
#pragma CODE_SECTION(_ALG_allocMemory, ".text:create")
#pragma CODE_SECTION(_ALG_freeMemory, ".text:create")
#pragma DATA_SECTION( internalDataMemory, ".intDataMem");
#define myMemalign  memalign
#else
void *memAlignVc(int alignment, unsigned int size);
#define myMemalign  memAlignVc
#endif

#define myFree      free
#ifdef OMAP2430_SPECIFIC
#define INTERNAL_DATA_MEM_SIZE    (0xE600)
#elif CHIP_DAVINCI
#define INTERNAL_DATA_MEM_SIZE    (0x8000)
#elif SETUP_DM642
#define INTERNAL_DATA_MEM_SIZE    (0x10000)
#else
#define INTERNAL_DATA_MEM_SIZE    (0x5000)
#endif

unsigned char internalDataMemory[ INTERNAL_DATA_MEM_SIZE];
unsigned char *pInternalDataMemory = internalDataMemory;
unsigned int  internalDataMemorySize = INTERNAL_DATA_MEM_SIZE;


#include <ti/bios/include/std.h>
#include <alg.h>
#include <ti/xdais/ialg.h>

#include <stdlib.h>     /* malloc/free declarations */
#include <string.h>     /* memset declaration */
#include <stdio.h>

Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n);
Void _ALG_freeMemory(IALG_MemRec memTab[], Int n);
/*Specify prototype for TI JPEG decoder   */
#include "jpegdec.h"
#include "jpegdec_ti.h" 
FILE *Memory;
Int Size_external_persist,Size_Internal_persist;
Int Size_external_Scratch,Size_Internal_Scratch;
extern JPEGDEC_Params    params;
void * memAlignVc(int alignment, unsigned int size)
{
     return malloc( size);
} /* memAlignVc */


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
    Int i,j;
	Int Size_external_persist1;
	Int temp;
	int stack =8;
	int program_mem = 102; // RGB : 63; cropp : 70 , resizing : 77, org : 56
	int temp_per,temp_scr;
    char * base_ptr;
	if(n >= 9)
		Memory = fopen("Memory_Ext_prog.txt","w");
	else
		Memory = fopen("Memory_Ext_Seql.txt","w");


    Size_external_persist = 0;
    Size_Internal_persist = 0;
    Size_external_Scratch = 0;
    Size_Internal_Scratch  = 0;

	fprintf(Memory," \n For resolution : %d * %d \n\n",params.imgdecParams.maxWidth,params.imgdecParams.maxHeight);

    for (i = 0; i < n; i++) {
      /* This is to take care of change in memory allocator for / 
      /		different requirements. */
	  fprintf(Memory," \t Memtab No. %d ",i);

      allocateMemTabRequest( &memTab[i]);

        if (memTab[i].base == NULL) {
            _ALG_freeMemory(memTab, i);
            return (FALSE);
        }
		else
		{
			base_ptr = memTab[i].base;
			base_ptr++;
//			for(j=1;j<memTab[i].size;j++)
//				*base_ptr++ = 0;
		}
    }

	temp_per = ((double)((Size_external_persist )/1024) +1 ) / 2 * 2;
	temp_scr = ((double)((Size_external_Scratch )/1024) +1 ) / 2 * 2;

	fprintf(Memory," \n \n \t Size_external_persist %d in KByte %f  i.e.  %d \n ",Size_external_persist,(double)Size_external_persist/1024,temp_per);
    fprintf(Memory," \t Size_external_Scratch %d  in KByte %f i.e.   %d \n ",Size_external_Scratch,(double)Size_external_Scratch/1024,temp_scr);
#ifdef INTERNAL_MEM
	fprintf(Memory," \t Size_Internal_persist %d  in KByte %f\n ",Size_Internal_persist,(double)Size_Internal_persist/1024);
    fprintf(Memory," \t Size_Internal_Scratch %d  in KByte %f\n ",Size_Internal_Scratch,(double)Size_Internal_Scratch/1024);
	fprintf(Memory," \t Total Internal		 %d  in KByte %f\n ",Size_Internal_persist + Size_Internal_Scratch,(double)(Size_Internal_persist + Size_Internal_Scratch)/1024);
#endif

	fprintf(Memory," \n \t Total External including contant		 %d.18  in KByte \n ",temp_per + temp_scr);
	fprintf(Memory," \t Total External	including stack & program	 %d.18  in KByte \n ",temp_per + temp_scr + program_mem + stack);

	if(params.progressiveDecFlag == 1)
	{
/* For Data Sheet */
		temp = ( 6 * params.imgdecParams.maxHeight * params.imgdecParams.maxWidth );
		Size_external_persist1 = Size_external_persist - temp;
		
		temp = 6 * 720 * 480; // D1
		Size_external_persist =  Size_external_persist1 + temp;
		temp_per = ((double)((Size_external_persist )/1024) +1 ) / 2 * 2;

		fprintf(Memory," \n For D1 : 720 * 480 ");
		fprintf(Memory," \n \t Size_external_persist %d in KByte \n ",temp_per);

#ifdef INTERNAL_MEM
		fprintf(Memory," \t Size_Internal_persist %d  in KByte %f\n ",Size_Internal_persist,(double)Size_Internal_persist/1024);
		fprintf(Memory," \t Size_Internal_Scratch %d  in KByte %f\n ",Size_Internal_Scratch,(double)Size_Internal_Scratch/1024);
		fprintf(Memory," \t Total Internal		 %d  in KByte %f\n ",Size_Internal_persist + Size_Internal_Scratch,(double)(Size_Internal_persist + Size_Internal_Scratch)/1024);

#endif

		fprintf(Memory," \n \t Total External including contant		 %d.18  in KByte \n ",temp_per + temp_scr);
		fprintf(Memory," \t Total External	including stack & program	 %d.18  in KByte \n ",temp_per + temp_scr + program_mem + stack);

		temp = 6 * 1280 * 1024; // 1.3 MP
		Size_external_persist =  Size_external_persist1 + temp;
		temp_per = ((double)((Size_external_persist )/1024) +1 ) / 2 * 2;

		fprintf(Memory," \n For 1.3 MP : 1280 * 1024 ");
		fprintf(Memory," \n \t Size_external_persist %d in KByte \n ",temp_per);
#ifdef INTERNAL_MEM
		fprintf(Memory," \t Size_Internal_persist %d  in KByte %f\n ",Size_Internal_persist,(double)Size_Internal_persist/1024);
		fprintf(Memory," \t Size_Internal_Scratch %d  in KByte %f\n ",Size_Internal_Scratch,(double)Size_Internal_Scratch/1024);
		fprintf(Memory," \t Total Internal		 %d  in KByte %f\n ",Size_Internal_persist + Size_Internal_Scratch,(double)(Size_Internal_persist + Size_Internal_Scratch)/1024);
#endif		
		fprintf(Memory," \n \t Total External including contant		 %d.18  in KByte \n ",temp_per + temp_scr);
		fprintf(Memory," \t Total External	including stack & program	 %d.18  in KByte \n ",temp_per + temp_scr + program_mem + stack);

		temp = 6 * 1600 * 1200; // 1.9 MP
		Size_external_persist =  Size_external_persist1 + temp;
		temp_per = ((double)((Size_external_persist )/1024) +1 ) / 2 * 2;

		fprintf(Memory," \n For 1.9 MP : 1600x1200  ");
		fprintf(Memory," \n \t Size_external_persist %d in KByte \n ",temp_per);
#ifdef INTERNAL_MEM
		fprintf(Memory," \t Size_Internal_persist %d  in KByte %f\n ",Size_Internal_persist,(double)Size_Internal_persist/1024);
		fprintf(Memory," \t Size_Internal_Scratch %d  in KByte %f\n ",Size_Internal_Scratch,(double)Size_Internal_Scratch/1024);
		fprintf(Memory," \t Total Internal		 %d  in KByte %f\n ",Size_Internal_persist + Size_Internal_Scratch,(double)(Size_Internal_persist + Size_Internal_Scratch)/1024);
#endif
		fprintf(Memory," \n \t Total External including contant		 %d.18  in KByte \n ",temp_per + temp_scr);
		fprintf(Memory," \t Total External	including stack & program	 %d.18  in KByte \n ",temp_per + temp_scr + program_mem + stack);

		temp = 6 * 2048 * 1536; // 3.1 MP
		Size_external_persist =  Size_external_persist1 + temp;
		temp_per = ((double)((Size_external_persist )/1024) +1 ) / 2 * 2;

		fprintf(Memory," \n For 3.1 MP : 2048x1536  ");
		fprintf(Memory," \n \t Size_external_persist %d in KByte \n ",temp_per);
#ifdef INTERNAL_MEM
		fprintf(Memory," \t Size_Internal_persist %d  in KByte %f\n ",Size_Internal_persist,(double)Size_Internal_persist/1024);
		fprintf(Memory," \t Size_Internal_Scratch %d  in KByte %f\n ",Size_Internal_Scratch,(double)Size_Internal_Scratch/1024);
		fprintf(Memory," \t Total Internal		 %d  in KByte %f\n ",Size_Internal_persist + Size_Internal_Scratch,(double)(Size_Internal_persist + Size_Internal_Scratch)/1024);
#endif
		fprintf(Memory," \n \t Total External including contant		 %d.18  in KByte \n ",temp_per + temp_scr);
		fprintf(Memory," \t Total External	including stack & program	 %d.18  in KByte \n ",temp_per + temp_scr + program_mem + stack);

		temp = 6 * 2560 * 2048; // 5.2 MP
		Size_external_persist =  Size_external_persist1 + temp;
		temp_per = ((double)((Size_external_persist )/1024) +1 ) / 2 * 2;

		fprintf(Memory," \n For 5.2 MP : 2560×2048  ");
		fprintf(Memory," \n \t Size_external_persist %d in KByte \n ",temp_per);
#ifdef INTERNAL_MEM
		fprintf(Memory," \t Size_Internal_persist %d  in KByte %f\n ",Size_Internal_persist,(double)Size_Internal_persist/1024);
		fprintf(Memory," \t Size_Internal_Scratch %d  in KByte %f\n ",Size_Internal_Scratch,(double)Size_Internal_Scratch/1024);
		fprintf(Memory," \t Total Internal		 %d  in KByte %f\n ",Size_Internal_persist + Size_Internal_Scratch,(double)(Size_Internal_persist + Size_Internal_Scratch)/1024);
#endif
		fprintf(Memory," \n \t Total External including contant		 %d.18  in KByte \n ",temp_per + temp_scr);
		fprintf(Memory," \t Total External	including stack & program	 %d.18  in KByte \n ",temp_per + temp_scr + program_mem + stack);

    }

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
        if (memTab[i].base != NULL) {
            freeMemTabRequest( &memTab[i]);
        }
    }
}



int allocateMemTabRequest( IALG_MemRec *memTab)
{
  if( memTab->space == IALG_EXTERNAL ) {
    /* external memory request >-> do the normal way */
    memTab->base = (void *)myMemalign(memTab->alignment, memTab->size);
	  if( memTab->attrs == IALG_PERSIST)
  {
  fprintf(Memory,"  External_persist ");
  fprintf(Memory," \t size	%d \n ",memTab->size);
  Size_external_persist = Size_external_persist + memTab->size;
  }
  else
  {
  fprintf(Memory,"  External_Scratch ");
  fprintf(Memory," \t size	%d \n ",memTab->size);
  Size_external_Scratch = Size_external_Scratch + memTab->size;

  }

  } else {
    /* internal memory request */
    unsigned int  alignBytes;

	  if( memTab->attrs == IALG_PERSIST)
	  {
      fprintf(Memory," \t Internal_persist ");
      fprintf(Memory," \t size	%d \n ",memTab->size);
      Size_Internal_persist = Size_Internal_persist + memTab->size;
	  }
	  else
	  {
      fprintf(Memory,"   Internal_Scratch ");
      fprintf(Memory," \t size	%d \n ",memTab->size);
      Size_Internal_Scratch = Size_Internal_Scratch + memTab->size;

	  }


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




/* ========================================================================== */
/* End of file : alg_malloc.c                                                 */
/* -------------------------------------------------------------------------- */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.             */
/*                           All Rights Reserved.                             */
/* ========================================================================== */

