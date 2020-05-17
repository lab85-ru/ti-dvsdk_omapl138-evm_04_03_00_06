/*
 *  Copyright 2002 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) XDAS 2.51.00 02-20-03 (xdas-d20)" */
/*
 *  ======== alg_create.c ========
 *  This file contains a simple implementation of the ALG_create API operation.
 */
#ifdef _TMS320C6400
#pragma CODE_SECTION(ALG_create, ".text:create")
#pragma CODE_SECTION(ALG_delete, ".text:delete")
#endif /* _TMS320C6400 */

#include <std.h>
#include <alg.h>
#include <ialg.h>
#include <stdlib.h>
extern int scratch_flag;
extern IALG_MemRec *memTab_scratch;
extern int num_memtab;
#include <_alg.h>

/*
 *  ======== ALG_create ========
 */
#define TEST_ALGFREE

#ifdef TEST_ALGFREE
#define MAX_MEM_TAB (50)
IALG_MemRec gMemTab[MAX_MEM_TAB];
#endif
ALG_Handle ALG_create(IALG_Fxns *fxns, IALG_Handle p, IALG_Params *params)
{
    IALG_MemRec *memTab;
    Int n,i;
    ALG_Handle alg;
    IALG_Fxns *fxnsPtr;

    if (fxns != NULL) {
        n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc() : IALG_DEFMEMRECS;
        if ((memTab = (IALG_MemRec *)malloc(n * sizeof (IALG_MemRec)))) {

            n = fxns->algAlloc(params, &fxnsPtr, memTab);
            if (n <= 0) {
                return (NULL);
            }
            if (_ALG_allocMemory(memTab, n)) {
#ifdef TEST_ALGFREE
					memcpy(gMemTab, memTab, n*sizeof(IALG_MemRec));
#endif
                alg = (IALG_Handle)memTab[0].base;
                alg->fxns = fxns;

                if (fxns->algInit(alg, memTab, p, params) == IALG_EOK) 
				{
					
					if(scratch_flag)
					{
						num_memtab = n;
						
						if ((memTab_scratch = (IALG_MemRec *)malloc(n * sizeof (IALG_MemRec)))) 
						{
							for(i=0;i<n;i++)
							{
								memTab_scratch[i].base = memTab[i].base;
								memTab_scratch[i].size = memTab[i].size;
								memTab_scratch[i].attrs = memTab[i].attrs;
								memTab_scratch[i].alignment = n;
							}
						}
						
					}
					free(memTab);
                    return (alg);
                }
                fxns->algFree(alg, memTab);
                _ALG_freeMemory(memTab, n);
            }
            free(memTab);
        }
    }
    return (NULL);
}

/*
 *  ======== ALG_delete ========
 */
Void ALG_delete(ALG_Handle alg)
{
    IALG_MemRec *memTab;
    Int n;
    IALG_Fxns *fxns;

    if (alg != NULL && alg->fxns != NULL) {
        fxns = alg->fxns;
        n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc() : IALG_DEFMEMRECS;

        if ((memTab = (IALG_MemRec *)malloc(n * sizeof (IALG_MemRec)))) {
            memTab[0].base = alg;
            n = fxns->algFree(alg, memTab);
#ifdef TEST_ALGFREE
			{
				int i, j = 1;
				for(i = 0; i < n; i++){
					if(memcmp(&gMemTab[i], &memTab[i], sizeof(IALG_MemRec)) != 0){
						printf("Alg Free is not happening properly for index %d\n", i);
						j = 0;
					}
				}
				
				if(j){
					printf("\nALG FREE IS HAPPENING PROPERLY\n");
				}
			}
#endif //#ifdef TEST_ALGFREE
            _ALG_freeMemory(memTab, n);

            free(memTab);
        }
    }
}

