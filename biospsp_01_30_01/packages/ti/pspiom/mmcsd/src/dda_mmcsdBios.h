/*
 * dda_mmcsdBios.h
 *
 * This file contains MMCSD media layer macros and interface definitions. 
 * MMCSD Driver provides Driver Layer Interface to do operations on the MMCSD  
 * peripheral like device initialization, channel creation, control commands for 
 * peripheral specific operations etc
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

/** \file   dda_mmcsdBios.h
    \brief  BIOS Specific MMCSD media header file

    This file contains the declaration for media layer interface.

    (C) Copyright 2008, Texas Instruments, Inc

 */

#ifndef _DDA_MMCSDBIOS_H_
#define _DDA_MMCSDBIOS_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <ti/pspiom/blkmedia/psp_blkdev.h>
#include <ti/pspiom/mmcsd/psp_mmcsd.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                      MMCSD DDA DATA TYPES                                  */
/* ========================================================================== */
/**
 * \brief MMCSD driver info structure
 *
 */
typedef struct
{
    PSP_BlkDevOps_t       devOps;
    /**< dev ops                                                              */
    PSP_blkDevCallback_t  devCb;
    /**< device callback                                                      */
    Ptr                   handle;
    /**< Void pointer handle                                                  */
}DDA_MmcsdBlkDrvInfo;


/**
 * \brief MMCSD DDA bios structure
 *
 */
typedef struct
{
    PSP_MmcsdCardType           cardmediaType;
    /**< Card Type MMC or SD                                                  */
    PSP_MmcsdChannelHandle      ddaChannelHandle;
    /**< Channel Handle                                                       */
    DDA_MmcsdBlkDrvInfo         blkDrvInfo;
    /**< MMCSD driver info                                                    */
}DDA_MmcsdObj;

#ifdef __cplusplus
}
#endif

#endif/* _DDA_MMCSDBIOS_H_ */
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
