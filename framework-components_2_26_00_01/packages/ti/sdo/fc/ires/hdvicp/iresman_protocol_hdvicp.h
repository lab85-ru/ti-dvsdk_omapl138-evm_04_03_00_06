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
/**
 *  @file       iresman_protocol_hdvicp.h 
 *
 *  @brief      IRESMAN PROTOCOL HDVICP Interface Definitions (C64P) - 
 *              Interface of the protocol implementation to the device  
 *              specific resman, for the construction/deletion of 
 *              HDVICP resource handles
 */

#ifndef IRESMAN_PROTOCOL_HDVICP_
#define IRESMAN_PROTOCOL_HDVICP_

/** @ingroup    ti_sdo_fc_ires_hdvicp_IRESMAN_HDVICP */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif

#include <ti/sdo/fc/ires/iresman_protocol.h>

/*
 * @brief           Arguments that are passed from the IRES Resource manager
 *                  to intialize the handle
 */
typedef struct IRESMAN_HDVICP_ConstructHandleArgs {

    IRESMAN_ConstructArgs args;

    Int id;                     /* ID of the resource returned */

    Void *base0;                 /* Base address */ 

    Void *base1;                 /* Base address */ 

    Void *info;                  /* Info  */  
                                //TODO: Find out what this is
    Bool persistent;

} IRESMAN_HDVICP_ConstructHandleArgs;


/*
 * Interface to expose to the resource manager to be able to construct handles
 */
extern IRESMAN_ConstructFxns IRESMAN_HDVICP_CONSTRUCTFXNS;

/*@}*/ /** @ingroup    DSPIRESPROTHDVICP */


#ifdef __cplusplus
}
#endif /* extern "C" */


#endif  /* IRESMAN_PROTOCOL_HDVICP_ */

/*
 *  @(#) ti.sdo.fc.ires.hdvicp; 1, 0, 1,133; 12-1-2010 17:25:16; /db/atree/library/trees/fc/fc-p01x/src/ xlibrary

 */

