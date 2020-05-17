/*
 *  Copyright 1999 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */

/*
 * (C) Copyright 2000	 Texas Instruments Inc.  All rights reserved.
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
 *  ======== alg_create.c ========
 *  This file contains several implementations of the ALG_create API operation.
 *  The various implementations make differing assumptions about the runtime
 *  environment:
 *
 *      1. assume that a constant fixed maximum number of IALG_MemRecs is
 *         sufficient.
 *      2. assume the runtime supports the alloca() operation.  alloca()
 *         allocates space by simply "pushing" space on the C stack, in
 *         effect dynamically allocating automatic storage.
 *      3. assume malloc() and free() are supported.
 *      4. assume no alloca() and no malloc() but setjmp() and longjmp()
 *         are supported.
 */
#ifdef C6000
#pragma CODE_SECTION(ALG_create, ".text:mpeg2vdec_ti_create")
#pragma CODE_SECTION(ALG_delete, ".text:mpeg2vdec_ti_delete")
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

/*
 *  The following definitions determine which implementation of ALG_create()
 *  you want to use.  Modify the following definitions to select the
 *  implementation desired.
 */
#define FIXEDMEM    1       /* assume fix max number of mem recs */
#define HASALLOCA   0       /* runtime alloca() function */
#define HASMALLOC   0       /* runtime includes malloc() and free() */

#define MAXMEMRECS  16      /* adjust as necessary for fixed model */

#if FIXEDMEM
/*
 *  ======== ALG_create ========
 */
ALG_Handle ALG_create(IALG_Fxns *fxns, IALG_Handle p, IALG_Params *params)
{
  //  IALG_MemRec memTab[MAXMEMRECS];
	 IALG_MemRec *memTab;

    Int n;
    ALG_Handle alg;
    IALG_Fxns *fxnsPtr = NULL;
    if (fxns != NULL) {
        n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc() : MAXMEMRECS;

        if ((memTab = (IALG_MemRec *)malloc(n * sizeof (IALG_MemRec)))) {

            n = fxns->algAlloc(params, &fxnsPtr, memTab);
            if (n <= 0) {
                return (NULL);
            }

            if (_ALG_allocMemory(memTab, n)) {
                alg = (IALG_Handle)memTab[0].base;
                alg->fxns = fxns;
                if (fxns->algInit(alg, memTab, p, params) == IALG_EOK) {
                    free(memTab);
                    return (alg);
                }
                fxns->algFree(alg, memTab);
                _ALG_freeMemory(memTab, n);
            }

            free(memTab);
        }
    }
#if 0
    if (fxns != NULL)
    {
        if (fxns->algNumAlloc == NULL || fxns->algNumAlloc() <= MAXMEMRECS)
        {
            n = fxns->algAlloc(params, &fxnsPtr, memTab);
            if ((p != NULL && fxnsPtr != p->fxns)
                || (p == NULL && fxnsPtr != NULL)) {
                return (NULL);
            }
            if (_ALG_allocMemory(memTab, n))
            {
                alg = (IALG_Handle)memTab[0].base;
                alg->fxns = fxns;
                if (fxns->algInit(alg, memTab, p, params) == IALG_EOK) {
                    return (alg);
                }
                fxns->algFree(alg, memTab);
                _ALG_freeMemory(memTab, n);
            }
        }
    }
#endif
    return (NULL);
}

/*
 *  ======== ALG_delete ========
 */
Void ALG_delete(ALG_Handle alg)
{
    IALG_MemRec memTab[MAXMEMRECS];
    Int n;
    IALG_Fxns *fxns;

    if (alg != NULL && alg->fxns != NULL) {
        fxns = alg->fxns;
        n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc() : IALG_DEFMEMRECS;
        if (n <= MAXMEMRECS) {
            memTab[0].base = alg;
            n = fxns->algFree(alg, memTab);
            _ALG_freeMemory(memTab, n);
        }
    }
}

#else   /* allow arbitrary IALG_MemRec's */

#if HASALLOCA

#include <stdlib.h>
#include <alloca.h>     /* extern void *alloca(size_t); */

/*
 *  ======== ALG_create ========
 */
ALG_Handle ALG_create(IALG_Fxns *fxns, IALG_Handle p, IALG_Params *params)
{
    IALG_MemRec *memTab;
    Int n;
    ALG_Handle alg;
    IALG_Fxns *fxnsPtr;

    if (fxns != NULL) {
        n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc() : IALG_DEFMEMRECS;

        memTab = (IALG_MemRec *)alloca(n * sizeof (IALG_MemRec));

        n = fxns->algAlloc(params, &fxnsPtr, memTab);
        if ((p != NULL && fxnsPtr != p->fxns)
            || (p == NULL && fxnsPtr != NULL)) {
            return (NULL);
        }

        if (_ALG_allocMemory(memTab, n)) {
            alg = (IALG_Handle)memTab[0].base;
            alg->fxns = fxns;
            if (fxns->algInit(alg, memTab, p, params) == IALG_EOK) {
                return (alg);
            }
            fxns->algFree(alg, memTab);
            _ALG_freeMemory(memTab, n);
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

        memTab = (IALG_MemRec *)alloca(n * sizeof (IALG_MemRec));

        memTab[0].base = alg;
        n = fxns->algFree(alg, memTab);
        _ALG_freeMemory(memTab, n);
    }
}

#else   /* arbitrary number of IALG_MemRecs and no stack allocation */

#if HASMALLOC

#include <stdlib.h>

/*
 *  ======== ALG_create ========
 */
ALG_Handle ALG_create(IALG_Fxns *fxns, IALG_Handle p, IALG_Params *params)
{
    IALG_MemRec *memTab;
    Int n;
    ALG_Handle alg;
    IALG_Fxns *fxnsPtr;

    if (fxns != NULL) {
        n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc() : IALG_DEFMEMRECS;

        if ((memTab = (IALG_MemRec *)malloc(n * sizeof (IALG_MemRec)))) {

            n = fxns->algAlloc(params, &fxnsPtr, memTab);
            if ((p != NULL && fxnsPtr != p->fxns)
                || (p == NULL && fxnsPtr != NULL)) {
                return (NULL);
            }

            if (_ALG_allocMemory(memTab, n)) {
                alg = (IALG_Handle)memTab[0].base;
                alg->fxns = fxns;
                if (fxns->algInit(alg, memTab, p, params) == IALG_EOK) {
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

    if (alg != NULL && alg->fxns != NULL)
    {
        fxns = alg->fxns;
        n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc() : IALG_DEFMEMRECS;

        if ((memTab = (IALG_MemRec *)malloc(n * sizeof (IALG_MemRec))))
        {
            memTab[0].base = alg;
            n = fxns->algFree(alg, memTab);
            _ALG_freeMemory(memTab, n);

            free(memTab);
        }
    }
}

#else   /* arbitrary number of IALG_MemRecs no alloca() and no malloc() */

#include <setjmp.h>

typedef struct CreateArgs {
    Int         n;
    IALG_Handle handle;
    IALG_Handle p;
    IALG_Fxns   *fxns;
    IALG_Params *params;
    jmp_buf     *envp;
} CreateArgs;

static Void createObj(IALG_Fxns *, IALG_Params *, IALG_MemRec *, IALG_Handle, IALG_Handle *);
static Void callCreate(Int n, CreateArgs *args);

#define stackGrowsDown(ip, op)  ( (void *)(ip) < (void *)(op) )

/*
 *  ======== ALG_create ========
 */
ALG_Handle ALG_create(IALG_Fxns *fxns, IALG_Handle p, IALG_Params *params)
{
    CreateArgs args;
    jmp_buf env;

    if (fxns != NULL) {
        args.n = fxns->algNumAlloc == NULL ? IALG_DEFMEMRECS : fxns->algNumAlloc();
        args.fxns = fxns;
        args.params = params;
        args.handle = NULL;
        args.p = p;
        args.envp = &env;

        if (setjmp(env) != 0) {
            return (args.handle);   /* return *after* call to longjmp */
        }

        callCreate(args.n, &args);  /* always calls longjmp */
    }

    return (NULL);                  /* never reached! */
}

/*
 *  ======== callCreate ========
 */
static Void callCreate(Int n, CreateArgs *args)
{
    IALG_MemRec scratch[IALG_DEFMEMRECS];
    IALG_MemRec *memTab = scratch;
    jmp_buf *envp = args->envp;

//    printf("callCreate(0x%lx, 0x%lx)\n", n, args);

    if (n <= IALG_DEFMEMRECS) {
        if (!stackGrowsDown(scratch, args)) {
            memTab += IALG_DEFMEMRECS - args->n;
        }

        createObj(args->fxns, args->params, memTab, args->p, &args->handle);

        longjmp(*envp, 1);
    }
    else {
        callCreate(n - IALG_DEFMEMRECS, args);
    }
}

/*
 *  ======== createObj ========
 */
static Void createObj(IALG_Fxns *fxns, IALG_Params *params,
                        IALG_MemRec *memTab, IALG_Handle p, IALG_Handle *out)
{
    Int n;
    ALG_Handle alg;
    IALG_Fxns *fxnsPtr = NULL;

//    printf("createObj(0x%lx, 0x%lx, 0x%lx)\n", fxns, params, memTab);

    *out = NULL;

    n = fxns->algAlloc(params, &fxnsPtr, memTab);
    if ((p != NULL && fxnsPtr != p->fxns)
        || (p == NULL && fxnsPtr != NULL)) {
        return;
    }

    if (_ALG_allocMemory(memTab, n)) {
        alg = (IALG_Handle)memTab[0].base;
        alg->fxns = fxns;
        if (fxns->algInit(alg, memTab, p, params) == IALG_EOK) {
            *out = alg;
            return;
        }
        fxns->algFree(alg, memTab);
        _ALG_freeMemory(memTab, n);
    }
}

#endif  /* not HASMALLOC */

#endif  /* not HASALLOCA */

#endif  /* not FIXEDMEM */
