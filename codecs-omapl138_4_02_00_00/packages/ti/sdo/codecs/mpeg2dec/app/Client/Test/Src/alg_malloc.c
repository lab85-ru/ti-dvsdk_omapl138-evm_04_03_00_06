/*
 *  Copyright 1999 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */

/*
 * (C) Copyright 2000     Texas Instruments Inc.  All rights reserved.
 * Exclusive property of the Multimedia DSP Branch, Tsukuba Research &
 * Development Center of Texas Instruments Japan Limited.  Any handling,
 * use, disclosure, reproduction, duplication, transmission, or storage
 * of any part of this work by any means is subject to restrictions and
 * prior written permission set forth in TI's program license agreements
 * and associated software documentation.
 *
 * This copyright notice, restricted rights legend, or other proprietary
 * markings must be reproduced without modification in any authorized
 * copies of any part of this work.  Removal or modification of any part
 * of this notice is prohibited.
 *
 */

/* "@(#) DAIS 1.00 07-29-99 (dais-a09)" */
/*
 *  ======== alg_malloc.c ========
 *  This module implements an algorithm memory management "policy" in which
 *  no memory is shared among algorithm objects.  Memory is, however
 *  reclaimed when objects are deleted.
 *
 *  preemption      sharing             object deletion
 *  ----------      -------             ---------------
 *  yes(*)          none                yes
 *
 *  Note 1: this module uses run-time support functions malloc() and free()
 *  to allocate and free memory.  Since these functions are *not* reentrant,
 *  either all object creation and deletion must be performed by a single
 *  thread or reentrant versions or these functions must be created.
 *
 */
#ifdef C6000
#pragma CODE_SECTION(ALG_init, ".text:mpeg2vdec_ti_init")
#pragma CODE_SECTION(ALG_exit, ".text:mpeg2vdec_ti_exit")
#pragma CODE_SECTION(_ALG_allocMemory, ".externaltext")
#pragma CODE_SECTION(_ALG_freeMemory, ".externaltext")

#endif

#ifdef ENABLE_VC
#include <std.h>
#include <alg.h>
#include <ialg.h>
#include "_alg.h"
#else
#include <ti/bios/include/std.h>
#include <ti/sdo/fc/utils/api/alg.h>
#include <ti/xdais/ialg.h>
#include <ti/sdo/fc/utils/api/_alg.h>
#endif

#include <stdio.h>
#include <stdlib.h>     /* malloc/free declarations */
#include <string.h>     /* memset declaration */

int freeMemTabRequest( IALG_MemRec  *memTab);

/*
    Should remove this if the memory allocation routines support alignment
    and memory space.
*/
#define NO_STATIC_MEM

#ifndef NO_STATIC_MEM
#include "memtab.h"
#endif

/***************/


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
#ifdef NO_STATIC_MEM
#ifdef C6000
#pragma DATA_SECTION( internalDataMemory1, ".intmem_dma");
#pragma DATA_SECTION( internalDataMemory2, ".intmem");
#endif

#define INTERNAL_DATA_MEM_SIZE1    (35*1024)

unsigned char internalDataMemory1[ INTERNAL_DATA_MEM_SIZE1];
unsigned char *pInternalDataMemory1;
unsigned int  internalDataMemorySize1;


#define INTERNAL_DATA_MEM_SIZE2    (10*1024)

unsigned char internalDataMemory2[ INTERNAL_DATA_MEM_SIZE2];
unsigned char *pInternalDataMemory2;
unsigned int  internalDataMemorySize2;


int allocateMemTabRequest1( IALG_MemRec *memTab)
{
  if( memTab->space == IALG_EXTERNAL ) {
    /* external memory request >-> do the normal way */
    // Special allocation for frame staorage as it is needed on offchip mem and hence
    // we can use malloc for this.
    if( (memTab->base = (Void*) malloc(memTab->size)) == NULL )
    {
        printf("malloc failed:increase heap\n");
        return (FALSE) ;
    }

  } else {
    /* internal memory request */
    unsigned int  alignBytes;
    alignBytes = (((unsigned int) pInternalDataMemory1 + (memTab->alignment - 1)) & ~ (memTab->alignment - 1));
    alignBytes -= (unsigned int) pInternalDataMemory1;
    pInternalDataMemory1 += alignBytes;
    internalDataMemorySize1 -= alignBytes;
    if(internalDataMemorySize1 >= memTab->size) {
      /* allocate memory */
      memTab->base = pInternalDataMemory1;
      pInternalDataMemory1 += memTab->size;
      internalDataMemorySize1 -= memTab->size;
    } else {
      memTab->base = 0;
    }
  }
  return 0;
} /* allocateMemTabRequest1 */


int allocateMemTabRequest2( IALG_MemRec *memTab)
{
  if( memTab->space == IALG_EXTERNAL ) {
    /* external memory request >-> do the normal way */
    // Special allocation for frame staorage as it is needed on offchip mem and hence
    // we can use malloc for this.
    if( (memTab->base = (Void*) malloc(memTab->size)) == NULL )
    {
        printf("malloc failed:increase heap\n");
        return (FALSE) ;
    }

  } else {
    /* internal memory request */
    unsigned int  alignBytes;
    alignBytes = (((unsigned int) pInternalDataMemory2 + (memTab->alignment - 1)) & ~ (memTab->alignment - 1));
    alignBytes -= (unsigned int) pInternalDataMemory2;
    pInternalDataMemory2 += alignBytes;
    internalDataMemorySize2 -= alignBytes;
    if(internalDataMemorySize2 >= memTab->size) {
      /* allocate memory */
      memTab->base = pInternalDataMemory2;
      pInternalDataMemory2 += memTab->size;
      internalDataMemorySize2 -= memTab->size;
    } else {
      memTab->base = 0;
    }
  }
  return 0;
} /* allocateMemTabRequest2 */
#endif


#ifdef NO_STATIC_MEM
/*
 *  ======== _ALG_allocMemory ========
 */
Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n)
{
    Int i;
    pInternalDataMemory1 = internalDataMemory1;
    internalDataMemorySize1 = INTERNAL_DATA_MEM_SIZE1;
	pInternalDataMemory2 = internalDataMemory2;
    internalDataMemorySize2 = INTERNAL_DATA_MEM_SIZE2;

    for (i = 0; i < n; i++) {
      /* XXX changing the code here, to change the memory allocator for
       * different requirements. */
	   if((i== 2) || (i== 3) || (i== 4) || (i==7))
 			allocateMemTabRequest2( &memTab[i]);
       else
	        allocateMemTabRequest1( &memTab[i]);

        if (memTab[i].base == NULL) {
            _ALG_freeMemory(memTab, i);
            return (FALSE);
        }
    }

    return (TRUE);
}
#endif


#ifndef NO_STATIC_MEM
/*
 *  ======== _ALG_allocMemory ========
 */
Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n)
{

    memTab[0].base = (Void *) &(_mpeg2vdec_ti_obj);
    memTab[NEWFBUF].base = &_newframe_buf_all_storage[0];
    memTab[QMATRIX].base = &_qm_storage[0];
    memTab[MCPARA].base = &_mocom_para_storage[0];
    memTab[IDCTTMP].base = &_idct_tmp_storage[0];
    memTab[VLD_RECON_BUF].base = &g_err_recon_buffer[0];
    memTab[DUMMY_IDCT_BUF].base = &g_dummy_idct_buf[0];
    memTab[BUF_PP_422_INTERLEAVED].base =    &g_buf_pp_422_interleaved[0];

#ifdef _IP_BYTE_SWAP
	memTab[BS_BUF_BYTE_SWAP].base = &g_bs_buf_byte_swapped[0];
#endif //_IP_BYTE_SWAP

    // Special allocation for frame staorage as it is needed on offchip mem and hence
    // we can use malloc for this.
    if( (memTab[FRAME_STORAGE].base = (Void*) malloc(memTab[FRAME_STORAGE].size)) == NULL )
    {
        printf("malloc failed:increase heap\n");
        return (FALSE) ;
    }

    return (TRUE);
}
#endif


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


int freeMemTabRequest( IALG_MemRec  *memTab)
{
  if( memTab->space == IALG_EXTERNAL ) {
    /* external memory request >-> do the normal way */
    free( memTab->base);
  } else {
    /* internal memory  free request. XXX see the code  below
     * for the dangers of calling them as normal mallocs. Free is faked!!!  */
    memTab->base = 0;
  }
  return 0;
} /* freeMemTabRequest */
