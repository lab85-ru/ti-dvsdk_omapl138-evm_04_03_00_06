/** ==================================================================
* @file aac_alg_malloc.c
*
* @path codecs_dev\AAC_Decoder\AlgTest\TestSource\C
*
* @desc This module implements an algorithm memory management "policy" in which
*       no memory is shared among algorithm objects.  Memory is, however
*       reclaimed when objects are deleted.
* =====================================================================
* Copyright (c) Texas Instruments Inc 2004
*
* Use of this software is controlled by the terms and conditions found
in the license agreement under which this software has been supplied *
=====================================================================*/

/*
 *  Copyright 2001 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */
/* "@(#) XDAS 2.12 04-24-01 (__imports)" */
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
 *  Note 2: Memory alignment is supported for the c54x targets with the
 *  memalign54() function which allocates 'size + align' memory.  This
 *  is necessary since the 3.50 C54x compiler's run-time library does not
 *  support memalign().  This wastes 'align' extra memory.
 *
 */

#include <stdio.h>
#include "std.h"
#include "alg.h"
#include "ialg.h"

#include <stdlib.h>             /* malloc/free declarations */
#include <string.h>             /* memset declaration */
#if  defined (_54_) || (_55_)
void *memalign54 (size_t alignment, size_t size);
void free54 (void *ptr);
#define myMemalign  memalign54
#define myFree      free54
#else
#define myMemalign  malloc
#define myFree      free
#endif


Bool _ALG_allocMemory (IALG_MemRec memTab[], Int n);
Void _ALG_freeMemory (IALG_MemRec memTab[], Int n);

/**********************************************************************
 *
 *  FUNCTION NAME: ALG_activate
 *
 *  DESCRIPTION  : This function activates algorithm by initializing memory. 
 *
 *  ARGUMENT LIST:
 *
 *  Argument        Type            IO     Description
 *  -------------   --------        --     -------------------------------------
 *  alg             ALG_Handle      I      pointer to ALG_Handle                                      
 *                                                                         
 * RETURN VALUE:    none.
 *
 **********************************************************************/

Void
ALG_activate (ALG_Handle alg)
{
    /* restore all persistant shared memory */
    ;                           /* nothing to do since memory allocation never
                                   shares any data */

    /* do app specific initialization of scratch memory */
    if (alg->fxns->algActivate != NULL)
    {
        alg->fxns->algActivate (alg);
    }
}

/**********************************************************************
 *
 *  FUNCTION NAME: ALG_deactivate
 *
 *  DESCRIPTION  : This function deactivates algorithm. 
 *
 *  ARGUMENT LIST:
 *
 *  Argument        Type            IO     Description
 *  -------------   --------        --     ------------------------------
 *  alg             ALG_Handle      I      pointer to ALG_Handle                                      
 *                                                                         
 * RETURN VALUE:    none.
 *
 **********************************************************************/

Void
ALG_deactivate (ALG_Handle alg)
{
    /* do app specific store of persistent data */
    if (alg->fxns->algDeactivate != NULL)
    {
        alg->fxns->algDeactivate (alg);
    }

    /* save all persistant shared memory */
    ;                           /* nothing to do since memory allocation never
                                   shares any data */

}


/**********************************************************************
 *
 *  FUNCTION NAME: ALG_init
 *
 *  DESCRIPTION  : ALG_init. 
 *
 *  ARGUMENT LIST:
 *
 *  Argument        Type            IO     Description
 *  -------------   --------        --     -------------------------------------
 *                                                                         
 * RETURN VALUE:    none.
 *
 **********************************************************************/
Void
ALG_init (Void)
{
}

/**********************************************************************
 *
 *  FUNCTION NAME: ALG_exit
 *
 *  DESCRIPTION  : ALG_exit. 
 *
 *  ARGUMENT LIST:
 *
 *  Argument        Type            IO     Description
 *  -------------   --------        --     -------------------------------------
 *                                                                         
 * RETURN VALUE:    none.
 *
 **********************************************************************/

Void
ALG_exit (Void)
{
}

/**********************************************************************
 *
 *  FUNCTION NAME: _ALG_allocMemory
 *
 *  DESCRIPTION  : Allocates memory for alg. 
 *
 *  ARGUMENT LIST:
 *
 *  Argument        Type            IO     Description
 *  -------------   --------        --     -------------------------------------
 *  *memTab         IALG_MemRec     I      pointer to memtab 
 *  n               Int             I      number of Memtabs                                          
 *                                                                                                                                                 
 * RETURN VALUE:    TRUE.
 *
 **********************************************************************/

Bool
_ALG_allocMemory (IALG_MemRec memTab[], Int n)
{
    Int i;

    for (i = 0; i < n; i++)
    {
#if  defined (_54_) || (_55_)    
        memTab[i].base =
            myMemalign (memTab[i].alignment, memTab[i].size + 4);
#else
        memTab[i].base =
            myMemalign (memTab[i].size + 4);
#endif
        if (memTab[i].base == NULL)
        {
            _ALG_freeMemory (memTab, i);
            return (FALSE);
        }
        memset (memTab[i].base, 0, memTab[i].size);
    }

    return (TRUE);
}

/**********************************************************************
 *
 *  FUNCTION NAME: _ALG_freeMemory
 *
 *  DESCRIPTION  : Free memory allocated for alg. 
 *
 *  ARGUMENT LIST:
 *
 *  Argument        Type            IO     Description
 *  -------------   --------        --     -------------------------------------
 *  *memTab         IALG_MemRec     I      pointer to memtab 
 *  n               Int             I      number of Memtabs                                          
 *                                                                                                                                                 
 * RETURN VALUE:    TRUE.
 *
 **********************************************************************/

Void
_ALG_freeMemory (IALG_MemRec memTab[], Int n)
{
    Int i;

    for (i = 0; i < n; i++)
    {
        if (memTab[i].base != NULL)
        {
            myFree (memTab[i].base);
        }
    }
}

#if defined (_54_) || (_55_)

/**********************************************************************
 *
 *  FUNCTION NAME: memalign54
 *
 *  DESCRIPTION  : Align memory for C54x or C55x. 
 *
 *  ARGUMENT LIST:
 *
 *  Argument        Type            IO     Description
 *  -------------   --------        --     -------------------------------------
 *  alignment       size_t          I      block alignment 
 *  size            size_t          I      size of block                                         
 *                                                                                                                                                 
 * RETURN VALUE:    aligned memory ptr.
 *
 **********************************************************************/

void *
memalign54 (size_t alignment, size_t size)
{
    void **mallocPtr;
    void **retPtr;
    long tmp;
    /* return if invalid size value */
    if (size <= 0)
    {
        return (0);
    }

    /* 
     * If alignment is not a power of two, return what malloc returns. This is
     * how memalign behaves on the c6x.
     */
    if ((alignment & (alignment - 1)) || (alignment <= 1))
    {
        if ((mallocPtr = malloc (size + sizeof (mallocPtr))) != NULL)
        {
            *mallocPtr = mallocPtr;
            mallocPtr++;
        }
        return ((void *) mallocPtr);
    }

    /* allocate block of memory */
    if (!(mallocPtr = malloc (alignment + size)))
    {
        return (0);
    }

    /* Calculate aligned memory address */
    // retPtr = (void *) (((UWord32)mallocPtr + alignment)& ~(alignment - 1));
    tmp = (unsigned long) mallocPtr + alignment;
    tmp = tmp - (tmp & (alignment - 1));
    // tmp = tmp - (tmp%(alignment));
    retPtr = (void *) tmp;
    /* Set pointer to be used in the free54() fxn */
    retPtr[-1] = mallocPtr;

    /* return aligned memory */
    return ((void *) retPtr);
}

/**********************************************************************
 *
 *  FUNCTION NAME: free54
 *
 *  DESCRIPTION  : To free memory pointed by ptr.
 *
 *  ARGUMENT LIST:
 *
 *  Argument        Type            IO     Description
 *  -------------   --------        --     -------------------------------------
 *  *ptr            void            I      memory pointer to be freed
 *                                                                                                                                                 
 * RETURN VALUE:    modified memory ptr.
 *
 **********************************************************************/

Void
free54 (void *ptr)
{
    free ((void *) ((void **) ptr)[-1]);
}
#endif
