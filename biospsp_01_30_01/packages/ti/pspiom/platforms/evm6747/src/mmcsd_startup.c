/*
 * mmcsd_startup.c
 *
 * This file contains MMCSD driver initialization routines. These routines inti-
 * alize the MMCSD "driver" which are necessary to set the driver in a sane state
 * before the applications interact witht the driver. Usually, these are the first
 * routines to be called before starting any operation on MMCSD device/driver.
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
 *  \file   mmcsd_startup.c
 *
 *  \brief  This file contains the startup code for mmcsd driver.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#include <std.h>
#include <iom.h>

#include "ti/pspiom/mmcsd/psp_mmcsd.h"
#include "ti/pspiom/blkmedia/psp_blkdev.h"

#include <ti/sdo/edma3/drv/edma3_drv.h>      /* EDMA3 defines */

#define MMCSD_CLK_FREQ              (150u * 1000u * 1000u)
/*Clock frequency for mmcsd                                                   */

#define MMC_INST_ID                 0U
/*Instance no of mmcsd                                                        */

/**
 * EDMA3 device handle created by EDMA3 initialization function.
 * The handle is initialized in function edma3init () implemented in file
 * bios_edma3_drv_sample_init.c
 */
extern EDMA3_DRV_Handle hEdma;

/**
 * External Functions
 *
 * Functions defined in sample applications specific for the driver.
 */
extern EDMA3_DRV_Result edma3init(Void); /* EDMA initialization example */
extern EDMA3_DRV_Result edma3deinit(Void);

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Int32 deinitMmcsd(Void);

static Int32 initMmcsd(Ptr edmaHandle);



/**
 *  \brief Deinitialize MMCSD device.
 *
 *  This API Deinitialize the MMCSD driver.
 *
 *  \return Int32
 */
static Int32 deinitMmcsd(Void)
{
    Int32 result = IOM_EBADARGS;

    result = PSP_mmcsdDrvDeInit(MMC_INST_ID);

    return result;
}

/**
 *  \brief Initialize MMCSD driver.
 *
 *  This function will Initialize driver and wait for device to be registered
 *  if device is detected.
 *
 *  \return 0 if success, non zero otherwise.
 */
static Int32 initMmcsd(Ptr edmaHandle)
{
    Int32 result = IOM_EBADARGS;
    PSP_MmcsdConfig mmcsdConfig;
    if (NULL != edmaHandle)
    {
        mmcsdConfig.opMode = PSP_MMCSD_OPMODE_DMAINTERRUPT;
        mmcsdConfig.hEdma = edmaHandle;
        mmcsdConfig.eventQ = PSP_MMCSD_EDMA3_EVENTQ_0;
        mmcsdConfig.hwiNumber = 7u;
        mmcsdConfig.pscPwrmEnable = FALSE;/* default no power management reqd */
        mmcsdConfig.pllDomain = PSP_MMCSD_PLL_DOMAIN_0;
        result = PSP_mmcsdDrvInit(MMCSD_CLK_FREQ, MMC_INST_ID, &mmcsdConfig);
        if (IOM_COMPLETED != result)
        {
            /* Driver init failed */
        }
    }
    return result;
}

Int32 mmcsdStorageInit()
{
    Int32 result = IOM_COMPLETED;
    EDMA3_DRV_Result edmaResult;

    /* Initialize the EDMA */
    edmaResult = edma3init();
    if (EDMA3_DRV_SOK != edmaResult)
    {
        /* EDMA Initialization Failed */
        result = edmaResult;
    }
    else
    {
        result = PSP_blkmediaDrvInit(hEdma, 0x00, 0x02u, 4096u);
        if (IOM_COMPLETED == result)
        {
            result = initMmcsd(hEdma);
            if (IOM_COMPLETED != result)
            {
                /* Init MMCSD fail */
            }
        }
        else
        {
            /* Block media Init fail */
        }
    }
    return result;
}


Int32 mmcsdStorageDeInit()
{
    Int32 result = IOM_COMPLETED;
    EDMA3_DRV_Result edmaResult;

    result = deinitMmcsd();
    if (IOM_COMPLETED == result)
    {
        result = PSP_blkmediaDrvDeInit();
        if (IOM_COMPLETED != result)
        {
            /* BLKMED Deinit fails */
        }
        else
        {
            edmaResult = edma3deinit();
            if (EDMA3_DRV_SOK != edmaResult)
            {
                /* EDMA Driver deletion failed */
                result = edmaResult;
            }
        }
    }
    else
    {
        /* MMCSD Driver de-init. failed */
    }

    return result;
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
