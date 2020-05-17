/* 
 * Copyright (c) 2010, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/*
 *  ======== universal_copy.c ========
 *  Universal "copy" algorithm.
 *
 *  This file contains an implementation of the IUNIVERSAL interface
 *  defined by XDM.
 */
#include <xdc/std.h>
#include <string.h>

#include <ti/xdais/dm/iuniversal.h>

#ifdef __TI_COMPILER_VERSION__
/* XDAIS Rule 13 - this #pragma should only apply to TI codegen */
#pragma CODE_SECTION(UNIVERSALCOPY_TI_control, ".text:algControl")
#pragma CODE_SECTION(UNIVERSALCOPY_TI_process, ".text:algProcess")
#pragma CODE_SECTION(UNIVERSALCOPY_TI_initObj, ".text:algInit")
#pragma CODE_SECTION(UNIVERSALCOPY_TI_free,    ".text:algFree")
#pragma CODE_SECTION(UNIVERSALCOPY_TI_alloc,   ".text:algAlloc")
#endif

#include "universal_copy_ti.h"
#include "universal_copy_ti_priv.h"

/* TODO, need to autogenerate this */
#define VERSIONSTRING "1.00.00.00"

/* Helper definitions */
#define BITSPERBYTE     8       /* number of bits in a byte */

/* buffer definitions */
#define MININBUFS       1
#define MINOUTBUFS      1
#define MININBUFSIZE    1
#define MINOUTBUFSIZE   1

#define IALGFXNS  \
    &UNIVERSALCOPY_TI_IALG,  /* module ID */                         \
    NULL,                       /* activate */                          \
    UNIVERSALCOPY_TI_alloc,  /* alloc */                             \
    NULL,                       /* control (NULL => no control ops) */  \
    NULL,                       /* deactivate */                        \
    UNIVERSALCOPY_TI_free,   /* free */                              \
    UNIVERSALCOPY_TI_initObj,/* init */                              \
    NULL,                       /* moved */                             \
    NULL                        /* numAlloc (NULL => IALG_MAXMEMRECS) */

/*
 *  ======== UNIVERSALCOPY_TI_IUNIVERSALCOPY ========
 *  This structure defines TI's implementation of the IUNIVERSAL interface
 *  for the UNIVERSALCOPY_TI module.
 */
IUNIVERSAL_Fxns UNIVERSALCOPY_TI_IUNIVERSALCOPY = {
    {IALGFXNS},
    UNIVERSALCOPY_TI_process,
    UNIVERSALCOPY_TI_control,
};

/*
 *  ======== UNIVERSALCOPY_TI_IALG ========
 *  This structure defines TI's implementation of the IALG interface
 *  for the UNIVERSALCOPY_TI module.
 */
#ifdef __TI_COMPILER_VERSION__
/* satisfy XDAIS symbol requirement without any overhead */
asm("_UNIVERSALCOPY_TI_IALG .set _UNIVERSALCOPY_TI_IUNIVERSALCOPY");

#else

/*
 *  We duplicate the structure here to allow this code to be compiled and
 *  run using non-TI toolchains at the expense of unnecessary data space
 *  consumed by the definition below.
 */
IALG_Fxns UNIVERSALCOPY_TI_IALG = {      /* module_vendor_interface */
    IALGFXNS
};

#endif

IUNIVERSAL_Params UNIVERSALCOPY_TI_PARAMS = {
    sizeof(IUNIVERSAL_Params),       /* size */
};

/*
 *  ======== UNIVERSALCOPY_TI_alloc ========
 *  Return a table of memory descriptors that describe the memory needed
 *  to construct our object.
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int UNIVERSALCOPY_TI_alloc(const IALG_Params *algParams,
    IALG_Fxns **pf, IALG_MemRec memTab[])
{
    /* Request memory for my object */
    memTab[0].size = sizeof(UNIVERSALCOPY_TI_Obj);
    memTab[0].alignment = 0;
    memTab[0].space = IALG_EXTERNAL;
    memTab[0].attrs = IALG_PERSIST;

    return (1);
}


/*
 *  ======== UNIVERSALCOPY_TI_free ========
 *  Return a table of memory pointers that should be freed.  Note
 *  that this should include *all* memory requested in the
 *  alloc operation above.
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int UNIVERSALCOPY_TI_free(IALG_Handle handle, IALG_MemRec memTab[])
{
    /*
     * Because our internal object size doesn't depend on create
     * params, we can just leverage the algAlloc() call with default
     * (NULL) create params.
     */
    return (UNIVERSALCOPY_TI_alloc(NULL, NULL, memTab));
}


/*
 *  ======== UNIVERSALCOPY_TI_initObj ========
 *  Initialize the memory allocated on our behalf (including our object).
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int UNIVERSALCOPY_TI_initObj(IALG_Handle handle, const IALG_MemRec memTab[],
    IALG_Handle p, const IALG_Params *algParams)
{
    const IUNIVERSAL_Params *params = (IUNIVERSAL_Params *)algParams;

    /*
     * In lieu of XDM defined default params, use our codec-specific ones.
     * Note that these default values _should_ be documented in your algorithm
     * documentation so users know what to expect.
     */
    if (params == NULL) {
        params = &UNIVERSALCOPY_TI_PARAMS;
    }

    return (IALG_EOK);
}


/*
 *  ======== UNIVERSALCOPY_TI_process ========
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
XDAS_Int32 UNIVERSALCOPY_TI_process(IUNIVERSAL_Handle h,
        XDM1_BufDesc *inBufs, XDM1_BufDesc *outBufs, XDM1_BufDesc *inOutBufs,
        IUNIVERSAL_InArgs *inArgs, IUNIVERSAL_OutArgs *outArgs)
{
    XDAS_Int32 numInBytes;

    /* validate arguments - this codec only supports "base" XDM. */
    if ((inArgs->size != sizeof(*inArgs)) ||
            (outArgs->size != sizeof(*outArgs))) {
        outArgs->extendedError = XDM_UNSUPPORTEDPARAM;

        return (IUNIVERSAL_EUNSUPPORTED);
    }

    /* validate that there's at least a single inBuf and outBuf */
    if ((inBufs->numBufs < 1) || (outBufs->numBufs < 1)) {
        outArgs->extendedError = XDM_UNSUPPORTEDPARAM;

        return (IUNIVERSAL_EFAIL);
    }

    numInBytes = inBufs->descs[0].bufSize < outBufs->descs[0].bufSize ?
        inBufs->descs[0].bufSize : outBufs->descs[0].bufSize;

    /* everything looks good, do the 'transcode', set outArgs and return */
    memcpy(outBufs->descs[0].buf, inBufs->descs[0].buf, numInBytes);

    /* report how we accessed the input buffer */
    inBufs->descs[0].accessMask = 0;
    XDM_SETACCESSMODE_READ(inBufs->descs[0].accessMask);

    /* report how we accessed the output buffer */
    outBufs->descs[0].accessMask = 0;
    XDM_SETACCESSMODE_WRITE(outBufs->descs[0].accessMask);

    return (IUNIVERSAL_EOK);
}


/*
 *  ======== UNIVERSALCOPY_TI_control ========
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
XDAS_Int32 UNIVERSALCOPY_TI_control(IUNIVERSAL_Handle handle,
    IUNIVERSAL_Cmd id, IUNIVERSAL_DynamicParams *dynParams,
    IUNIVERSAL_Status *status)
{
    XDAS_Int32 retVal;

    /* validate arguments - this codec only supports "base" XDM. */
    if ((dynParams->size != sizeof(*dynParams)) ||
            (status->size != sizeof(*status))) {
        return (IUNIVERSAL_EUNSUPPORTED);
    }

    /* initialize for the general case where we don't access the data buffer */
    XDM_CLEARACCESSMODE_READ(status->data.descs[0].accessMask);
    XDM_CLEARACCESSMODE_WRITE(status->data.descs[0].accessMask);

    switch (id) {
        case XDM_GETVERSION:
            if ((status->data.descs[0].buf != NULL) &&
                (status->data.descs[0].bufSize >=
                    strlen(VERSIONSTRING))) {

                strncpy((char *)status->data.descs[0].buf, VERSIONSTRING,
                        strlen(VERSIONSTRING));

                /* null terminate the string */
                status->data.descs[0].buf[strlen(VERSIONSTRING)] = '\0';

                /* strncpy wrote to the data buffer */
                XDM_SETACCESSMODE_WRITE(status->data.descs[0].accessMask);

                retVal = IUNIVERSAL_EOK;
            }
            else {
                retVal = IUNIVERSAL_EFAIL;
            }

            break;

        default:
            /* unsupported cmd */
            retVal = IUNIVERSAL_EFAIL;
            break;
    }

    return (retVal);
}
/*
 *  @(#) ti.xdais.dm.examples.universal_copy; 1, 0, 0,85; 9-18-2010 14:46:21; /db/wtree/library/trees/dais/dais-t03x/src/ xlibrary

 */

