/** ==================================================================
* @file aac_alg_create.c
*
* @path codecs_dev\AAC_Decoder\AlgTest\TestSource\C
*
* @desc This file contains the implementation of ALG_create API operation.
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
 *  ======== alg_create.c ========
 *  This file contains a simple implementation of the ALG_create API operation.
 */

#include "std.h"
#include "alg.h"
#include "ialg.h"
#include "stdlib.h"

#include "_alg.h"

/**********************************************************************
 *
 *  FUNCTION NAME: ALG_create
 *
 *  DESCRIPTION  : This function creates an instance of algorithm.. 
 *
 *  ARGUMENT LIST:
 *
 *  Argument        Type            IO     Description
 *  -------------   --------        --     -------------------------------------
 *  *fxns           IALG_Fxns       I      pointer to  IALG_Fxns structure                                        
 *  p               IALG_Handle     I      IALG_Handle                                                   
 *  *params         IALG_Params     I      pointer to  IALG_Params structure                                                      
 *                                                                         
 * RETURN VALUE:    none.
 *
 **********************************************************************/

ALG_Handle
ALG_create (IALG_Fxns * fxns, IALG_Handle p, IALG_Params * params)
{
    IALG_MemRec *memTab;
    Int n;
    ALG_Handle alg;
    IALG_Fxns *fxnsPtr;


    if (fxns != NULL)
    {
        n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc () : IALG_DEFMEMRECS;

        if ((memTab = (IALG_MemRec *) malloc (n * sizeof (IALG_MemRec))))
        {

            n = fxns->algAlloc (params, &fxnsPtr, memTab);
            if (n <= 0)
            {
                return (NULL);
            }
/* dynamic buffer alloction for memtab */

            if (_ALG_allocMemory (memTab, n))
            {
                alg = (IALG_Handle) memTab[0].base;
                alg->fxns = fxns;
                if (fxns->algInit (alg, memTab, p, params) == IALG_EOK)
                {
                    free (memTab);
                    return (alg);
                }
                fxns->algFree (alg, memTab);
                _ALG_freeMemory (memTab, n);
            }

            free (memTab);
        }
    }

    return (NULL);
}

/**********************************************************************
 *
 *  FUNCTION NAME: ALG_delete
 *
 *  DESCRIPTION  : This function deletes instance of algorithm.. 
 *
 *  ARGUMENT LIST:
 *
 *  Argument        Type            IO     Description
 *  -------------   --------        --     -------------------------------------
 *  alg             ALG_Handle       I      pointer to alg                                        
 *                                                                         
 * RETURN VALUE:    none.
 *
 **********************************************************************/

Void
ALG_delete (ALG_Handle alg)
{
    IALG_MemRec *memTab;
    Int n;
    IALG_Fxns *fxns;

    if (alg != NULL && alg->fxns != NULL)
    {
        fxns = alg->fxns;
        n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc () : IALG_DEFMEMRECS;

        if ((memTab = (IALG_MemRec *) malloc (n * sizeof (IALG_MemRec))))
        {
            memTab[0].base = alg;
            n = fxns->algFree (alg, memTab);
            _ALG_freeMemory (memTab, n);

            free (memTab);
        }
    }
}
