/* --COPYRIGHT--,BSD
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
 * --/COPYRIGHT--*/

#include <ti/sdo/dmai/Dmai.h>
#include <ti/sdo/dmai/Buffer.h>
#include <ti/sdo/dmai/Blend.h>

#define MODULE_NAME     "Blend"

/******************************************************************************
 * Blend_create
 ******************************************************************************/
Blend_Handle Blend_create(Blend_Attrs *attrs)
{
    return NULL;
}

/******************************************************************************
 * Blend_config
 ******************************************************************************/
Int Blend_config(Blend_Handle hBlend, Ccv_Handle hCcv, Buffer_Handle hBmpBuf,
                 Buffer_Handle hSrcBuf, Buffer_Handle hDstBuf,
                 Blend_Config_Params *params)
{
    return Dmai_ENOTIMPL;
}

/******************************************************************************
 * Blend_execute
 ******************************************************************************/
Int Blend_execute(Blend_Handle hBlend, Buffer_Handle hBmpBuf,
                  Buffer_Handle hSrcBuf, Buffer_Handle hDstBuf)
{
    return Dmai_ENOTIMPL;
}

/******************************************************************************
 * Blend_delete
 ******************************************************************************/
Int Blend_delete(Blend_Handle hBlend)
{
    return Dmai_ENOTIMPL;
}
