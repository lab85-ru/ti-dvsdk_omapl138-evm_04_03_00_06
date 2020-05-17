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
 *  ======== audenc1_copy.c ========
 *  Audio encoder "copy" algorithm.
 *
 *  This file contains an implementation of the IAUDENC1 interface
 *  defined by XDM.
 */
#include <xdc/std.h>
#include <string.h>

#include <ti/xdais/dm/iaudenc1.h>

#ifdef __TI_COMPILER_VERSION__
/* XDAIS Rule 13 - this #pragma should only apply to TI codegen */
#pragma CODE_SECTION(AUDENC1COPY_TI_control, ".text:algControl")
#pragma CODE_SECTION(AUDENC1COPY_TI_process, ".text:algProcess")
#pragma CODE_SECTION(AUDENC1COPY_TI_initObj, ".text:algInit")
#pragma CODE_SECTION(AUDENC1COPY_TI_free,    ".text:algFree")
#pragma CODE_SECTION(AUDENC1COPY_TI_alloc,   ".text:algAlloc")
#endif

#include "audenc1_copy_ti.h"
#include "audenc1_copy_ti_priv.h"

/* TODO, need to autogenerate this */
#define VERSIONSTRING "1.00.00.00"

/* buffer definitions */
#define MININBUFS       1
#define MINOUTBUFS      1
#define MININBUFSIZE    1
#define MINOUTBUFSIZE   1

/* utility macro */
#define BITSPERBYTE     8

#define IALGFXNS  \
    &AUDENC1COPY_TI_IALG,/* module ID */                        \
    NULL,               /* activate */                          \
    AUDENC1COPY_TI_alloc,/* alloc */                            \
    NULL,               /* control (NULL => no control ops) */  \
    NULL,               /* deactivate */                        \
    AUDENC1COPY_TI_free, /* free */                             \
    AUDENC1COPY_TI_initObj, /* init */                          \
    NULL,               /* moved */                             \
    NULL                /* numAlloc (NULL => IALG_MAXMEMRECS) */

/*
 *  ======== AUDENC1COPY_TI_IAUDENCCOPY ========
 *  This structure defines TI's implementation of the IAUDENC1 interface
 *  for the AUDENC1COPY_TI module.
 */
IAUDENC1_Fxns AUDENC1COPY_TI_IAUDENC1COPY = {    /* module_vendor_interface */
    {IALGFXNS},
    AUDENC1COPY_TI_process,
    AUDENC1COPY_TI_control,
};

/*
 *  ======== AUDENC1COPY_TI_IALG ========
 *  This structure defines TI's implementation of the IALG interface
 *  for the AUDENC1COPY_TI module.
 */
#ifdef __TI_COMPILER_VERSION__
/* satisfy XDAIS symbol requirement without any overhead */
asm("_AUDENC1COPY_TI_IALG .set _AUDENC1COPY_TI_IAUDENC1COPY");

#else

/*
 *  We duplicate the structure here to allow this code to be compiled and
 *  run using non-TI toolchains at the expense of unnecessary data space
 *  consumed by the definition below.
 */
IALG_Fxns AUDENC1COPY_TI_IALG = {      /* module_vendor_interface */
    IALGFXNS
};

#endif

IAUDENC1_Params AUDENC1COPY_TI_PARAMS = {
    sizeof(IAUDENC1_Params),
    44100,                          /* sampleRate */
    128000,                         /* bitRate */
    IAUDIO_2_0,                     /* channelMode */
    XDM_LE_16,                      /* dataEndianness */
    IAUDIO_CBR,                     /* encMode */
    IAUDIO_INTERLEAVED,             /* inputFormat */
    16,                             /* inputBitsPerSample */
    0,                              /* maxBitRate - only for VBR */
    0,                              /* dualMonoMode - only for dualMono */
    XDAS_FALSE,                     /* crcFlag */
    XDAS_FALSE,                     /* ancFlag */
    XDAS_FALSE                      /* lfeFlag */
};

/*
 *  ======== AUDENC1COPY_TI_alloc ========
 *  Return a table of memory descriptors that describe the memory needed
 *  to construct our object.
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int AUDENC1COPY_TI_alloc(const IALG_Params *algParams,
    IALG_Fxns **pf, IALG_MemRec memTab[])
{
    /* Request memory for my object */
    memTab[0].size = sizeof(AUDENC1COPY_TI_Obj);
    memTab[0].alignment = 0;
    memTab[0].space = IALG_EXTERNAL;
    memTab[0].attrs = IALG_PERSIST;

    return (1);
}


/*
 *  ======== AUDENC1COPY_TI_free ========
 *  Return a table of memory pointers that should be freed.  Note
 *  that this should include *all* memory requested in the
 *  alloc operation above.
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int AUDENC1COPY_TI_free(IALG_Handle handle, IALG_MemRec memTab[])
{
    /*
     * Because our internal object size doesn't depend on create
     * params, we can just leverage the algAlloc() call with default
     * (NULL) create params.
     */
    return (AUDENC1COPY_TI_alloc(NULL, NULL, memTab));
}


/*
 *  ======== AUDENC1COPY_TI_initObj ========
 *  Initialize the memory allocated on our behalf (including our object).
 */
/* ARGSUSED - this line tells the TI compiler not to warn about unused args. */
Int AUDENC1COPY_TI_initObj(IALG_Handle handle, const IALG_MemRec memTab[],
    IALG_Handle p, const IALG_Params *algParams)
{
    AUDENC1COPY_TI_Obj *obj = (AUDENC1COPY_TI_Obj *)handle;
    const IAUDENC1_Params *params = (IAUDENC1_Params *)algParams;

    /*
     * In lieu of XDM defined default params, use our codec-specific ones.
     * Note that these default values _should_ be documented in your algorithm
     * documentation so users know what to expect.
     */
    if (params == NULL) {
        params = &AUDENC1COPY_TI_PARAMS;
    }

    obj->bitRate = params->bitRate;
    obj->lfeFlag = params->lfeFlag;
    obj->sampleRate = params->sampleRate;
    obj->channelMode = params->channelMode;
    obj->dualMonoMode = params->dualMonoMode;
    obj->encMode = params->encMode;
    obj->inputBitsPerSample = params->inputBitsPerSample;

    return (IALG_EOK);
}


/*
 *  ======== AUDENC1COPY_TI_process ========
 */
XDAS_Int32 AUDENC1COPY_TI_process(IAUDENC1_Handle handle, XDM1_BufDesc *inBufs,
    XDM1_BufDesc *outBufs, IAUDENC1_InArgs *inArgs, IAUDENC1_OutArgs *outArgs)
{
    XDAS_Int32 numInBytes;
    AUDENC1COPY_TI_Obj *obj = (AUDENC1COPY_TI_Obj *)handle;

    /* validate arguments - this codec only supports "base" xDM. */
    if ((inArgs->size != sizeof(*inArgs)) ||
            (outArgs->size != sizeof(*outArgs))) {
        outArgs->extendedError = XDM_UNSUPPORTEDPARAM;

        return (IAUDENC1_EUNSUPPORTED);
    }

    /* validate that there's at least a single inBuf and outBuf */
    if ((inBufs->numBufs < 1) || (outBufs->numBufs < 1)) {
            outArgs->extendedError = XDM_UNSUPPORTEDPARAM;
        return (IAUDENC1_EFAIL);
    }

    /* convert samples to bytes */
    numInBytes = inArgs->numInSamples * (obj->inputBitsPerSample / BITSPERBYTE);

    /*
     * Sanity check that inBufs->descs[0].bufSize is really capable of holding
     * numInBytes bytes of data, and that outBufs->descs[0].bufSize
     * can hold the 'encoded' data.
     */
    if ((numInBytes > inBufs->descs[0].bufSize) ||
        (numInBytes > outBufs->descs[0].bufSize)) {

        /* inBuf is smaller than provided data(!) or "too small" outBuf */
        outArgs->extendedError = XDM_UNSUPPORTEDPARAM;

        return (IAUDENC1_EFAIL);
    }

    /* everything looks good, do the 'encode', set outArgs and return */
    memcpy(outBufs->descs[0].buf, inBufs->descs[0].buf, numInBytes);

    /* report _how_ we accessed the 2 data buffers */
    XDM_CLEARACCESSMODE_WRITE(inBufs->descs[0].accessMask);
    XDM_SETACCESSMODE_READ(inBufs->descs[0].accessMask);

    XDM_CLEARACCESSMODE_READ(outBufs->descs[0].accessMask);
    XDM_SETACCESSMODE_WRITE(outBufs->descs[0].accessMask);

    /* outArgs->bytesGenerated reports the total number of bytes encoded */
    outArgs->bytesGenerated = numInBytes;

    /* Fill out the rest of the outArgs struct */
    outArgs->extendedError = 0;
    outArgs->numInSamples = inArgs->numInSamples;
    outArgs->numZeroesPadded = 0;      /* TODO */

    return (IAUDENC1_EOK);
}


/*
 *  ======== AUDENC1COPY_TI_control ========
 */
XDAS_Int32 AUDENC1COPY_TI_control(IAUDENC1_Handle handle, IAUDENC1_Cmd id,
    IAUDENC1_DynamicParams *dynParams, IAUDENC1_Status *status)
{
    XDAS_Int32 retVal;
    AUDENC1COPY_TI_Obj *obj = (AUDENC1COPY_TI_Obj *)handle;

    /* validate arguments - this codec only supports "base" XDM. */
    if ((dynParams->size != sizeof(*dynParams)) ||
            (status->size != sizeof(*status))) {
        return (IAUDENC1_EUNSUPPORTED);
    }

    /* initialize for the general case where we don't access the data buffer */
    XDM_CLEARACCESSMODE_READ(status->data.accessMask);
    XDM_CLEARACCESSMODE_WRITE(status->data.accessMask);

    switch (id) {
        case XDM_GETSTATUS:
        case XDM_GETBUFINFO:
            status->extendedError = 0;
            status->validFlag = XDAS_TRUE;
            status->lfeFlag = obj->lfeFlag;
            status->bitRate = obj->bitRate;
            status->sampleRate = obj->sampleRate;
            status->channelMode = obj->channelMode;
            status->encMode = obj->encMode;

            status->bufInfo.minNumInBufs = MININBUFS;
            status->bufInfo.minNumOutBufs = MINOUTBUFS;
            status->bufInfo.minInBufSize[0] = MININBUFSIZE;
            status->bufInfo.minOutBufSize[0] = MINOUTBUFSIZE;

            retVal = IAUDENC1_EOK;
            break;

        case XDM_SETPARAMS:
            obj->lfeFlag = dynParams->lfeFlag;
            obj->bitRate = dynParams->bitRate;
            obj->sampleRate = dynParams->sampleRate;
            obj->channelMode = dynParams->channelMode;
            obj->dualMonoMode = dynParams->dualMonoMode;

            retVal = IAUDENC1_EOK;
            break;

        case XDM_SETDEFAULT:
            obj->bitRate = AUDENC1COPY_TI_PARAMS.bitRate;
            obj->lfeFlag = AUDENC1COPY_TI_PARAMS.lfeFlag;
            obj->sampleRate = AUDENC1COPY_TI_PARAMS.sampleRate;
            obj->channelMode = AUDENC1COPY_TI_PARAMS.channelMode;
            obj->dualMonoMode = AUDENC1COPY_TI_PARAMS.dualMonoMode;
            obj->encMode = AUDENC1COPY_TI_PARAMS.encMode;
            obj->inputBitsPerSample = AUDENC1COPY_TI_PARAMS.inputBitsPerSample;

            retVal = IAUDENC1_EOK;
            break;

        case XDM_RESET:
        case XDM_FLUSH:
            retVal = IAUDENC1_EOK;
            break;

        case XDM_GETVERSION:
            if ((status->data.buf != NULL) &&
                (status->data.bufSize >= strlen(VERSIONSTRING))) {
                strncpy((char *)status->data.buf, VERSIONSTRING,
                    strlen(VERSIONSTRING));

                /* null terminate the string */
                status->data.buf[strlen(VERSIONSTRING)] = '\0';

                /* strncpy wrote to the data buffer */
                XDM_SETACCESSMODE_WRITE(status->data.accessMask);

                retVal = IAUDENC1_EOK;
            }
            else {
                retVal = IAUDENC1_EFAIL;
            }

            break;

        default:
            /* unsupported cmd */
            retVal = IAUDENC1_EUNSUPPORTED;
            break;
    }

    return (retVal);
}
/*
 *  @(#) ti.xdais.dm.examples.audenc1_copy; 2, 0, 0,209; 9-18-2010 14:46:06; /db/wtree/library/trees/dais/dais-t03x/src/ xlibrary

 */

