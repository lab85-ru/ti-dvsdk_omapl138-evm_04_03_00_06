/*
 * Vpif_evmInit.h
 *
 * This file contains Application programming interface for the VPIF application
 * related EVM (platform) specific initialization routines
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 *  \file   Vpif_evmInit.h
 *
 *  \brief  This file contains the board level details of vpif.
 *
 */

#ifndef _VPIF_EVMINIT_H_
#define _VPIF_EVMINIT_H_

#ifdef __cplusplus
extern "C"
{
#endif /* extern "C" */

typedef enum EvmInit_VpifChannel_t
{
    EvmInit_VpifChannel_0,
    EvmInit_VpifChannel_1,
    EvmInit_VpifChannel_2,
    EvmInit_VpifChannel_3,
    EvmInit_VpifChannel_BOTHCAPCH,/* For RAW Capture use both capture channel */
    EvmInit_VpifChannel_BOTHDISPCH/* Not Supported */
}EvmInit_VpifChannel;


/**
 *  \brief Set clock settings and also the PINMUX for VPIF device
 *
 *  \param channelNo [In]  VPIF channel no of type EvmInit_VpifChannel
 *  \param isHd      [In]  TRUE if format is SD else FALSE.
 *                         Value of TRUE is not supported.
 *
 *  \return  NONE
 */
Void configureVpif0(EvmInit_VpifChannel channelNo, Bool isHd);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _VPIF_EVMINIT_H_ */
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
