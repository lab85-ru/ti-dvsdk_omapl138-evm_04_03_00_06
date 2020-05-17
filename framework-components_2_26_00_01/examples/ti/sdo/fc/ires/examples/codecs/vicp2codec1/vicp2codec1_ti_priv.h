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
 * */
/*
 *  ======== vicp2codec1_ti_priv.h ========
 *  Internal vendor specific (TI) interface header for VICP2CODEC1
 *  algorithm. Only the implementation source files include
 *  this header; this header is not shipped as part of the
 *  algorithm.
 *
 *  This header contains declarations that are specific to
 *  this implementation and which do not need to be exposed
 *  in order for an application to use the VICP2CODEC1 algorithm.
 */
#ifndef VICP2CODEC1_TI_PRIV_
#define VICP2CODEC1_TI_PRIV_


#include <ti/xdais/ires.h>
#include <ti/sdo/fc/ires/vicp/ires_vicp2.h>
#include <ti/sdo/fc/ires/grouputils/ires_grouputils.h>



#define ALGNUM_VICPRESOURCES 2
#define ALGNUM_RESOURCES   (ALGNUM_VICPRESOURCES +1 )

typedef struct VICP2CODEC1_TI_CtxtArgs {
    Int size;           
} VICP2CODEC1_TI_CtxtArgs;

typedef struct VICP2CODEC1_TI_Obj {
    IALG_Obj       alg;            /* MUST be first field of all XDAS algs */

    /* IRES stuff */
    IRES_VICP2_Handle           resourceHandles[ALGNUM_VICPRESOURCES]; 
    IRES_GROUPUTILS_Handle      grouputils;
    short                       resourceState[ALGNUM_VICPRESOURCES]; 
    IRES_VICP2_ProtocolArgs     protocolArgs[ALGNUM_VICPRESOURCES];
    IRES_GROUPUTILS_ProtocolArgs grouputilsArgs;
    VICP2CODEC1_TI_CtxtArgs      contextArgs;
} VICP2CODEC1_TI_Obj;

extern Int VICP2CODEC1_TI_alloc(const IALG_Params *algParams, IALG_Fxns **pf,
    IALG_MemRec memTab[]);

extern Int VICP2CODEC1_TI_free(IALG_Handle handle, IALG_MemRec memTab[]);

extern Int VICP2CODEC1_TI_initObj(IALG_Handle handle, const IALG_MemRec memTab[],
        IALG_Handle parent, const IALG_Params *algParams);

#endif /* VICP2CODEC1_TI_PRIV_ */
/*
 *  @(#) ti.sdo.fc.ires.examples.codecs.vicp2codec1; 1, 0, 0,32; 12-1-2010 17:25:11; /db/atree/library/trees/fc/fc-p01x/src/ xlibrary

 */

