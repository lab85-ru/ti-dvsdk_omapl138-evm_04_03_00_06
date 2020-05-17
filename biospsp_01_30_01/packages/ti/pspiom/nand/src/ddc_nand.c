/*
 * ddc_nand.c
 *
 * This file contains NAND core functionality implementation.
 * NAND Driver provides Driver Layer Interface to do operations on the NAND
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

/** \file      ddc_nand.c
 *
 *  \brief     Implementation of DDC layer of NAND of NAND driver.
 *
 *  This file contains the implementation of the DDC layer of the NAND
 *  driver. It implements all the API's defined in the psp_nand.h file.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 */

/* ========================================================================== */
/*                            INCLUDE FILES                                   */
/* ========================================================================== */
#include <std.h>
#include <iom.h>
#include <assert.h>

#include "llc_nand.h"
#include "ddc_nandftl.h"
#include "ddc_nand.h"
#include "llc_nandAbstract.h"

#include <ti/pspiom/nand/psp_nand.h>
/* ========================================================================== */
/*                            NAND DATA TYPES                                 */
/* ========================================================================== */

/**
 *  \brief  NAND driver object instances
 *
 *  This is the instantiation of the NAND driver device objects
 */
static DDC_nandObj  nandObj[PSP_NAND_INSTANCES] =
{
    {
        DDC_DRIVER_STATE_DELETED
    }
};

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK

static Int32 nandValidateMacros(DDC_nandObj           *hNandObj,
                                const PSP_nandConfig  *config);
#endif
/* End parameter checking                                                     */

/* ========================================================================== */
/*                        GLOBAL FUNCTION DEFINITIONS                         */
/* ========================================================================== */


/**
 * \brief Initializes the NAND driver.
 *
 *        This function initializes the NAND driver instance. It detects the
 *        NAND device and prepares the FTL and LLC modules.
 */
Int32 PSP_nandInit(Uint32  instanceId, const PSP_nandConfig *config)
{
    Int32                result;     /* Result of initialization         */
    DDC_nandObj         *hNandObj;   /* Pointer to DDC instance object   */
    Ptr                  llcHandle;  /* LLC module handle                */
    DDC_nandFtlObj      *ftlObj;     /* FTL module handle                */
    LLC_nandInitConfig   llcInitCfg; /* LLC initialization configuration */
    DDC_nandFtlInitObj   ftlInitObj; /* FTL initialization configuration */
    Bool                 flag = TRUE;

    do
    {
        /* Initialize the local variables                                     */
        llcHandle = NULL;
        flag = FALSE;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        /* Validate the input parameter                                       */
        if ((instanceId >= PSP_NAND_INSTANCES) || (NULL == config))
        {
            result = IOM_EBADARGS;
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Get the instance object of the driver                              */
        hNandObj = &nandObj[instanceId];

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        /* Check the state of the driver                                      */
        if (DDC_DRIVER_STATE_DELETED != hNandObj->state)
        {
            /* Driver is not in the deleted state                             */
            result = IOM_EBADMODE;
            break;
        }

        /* Validate the operating mode input parameter                        */
        if ((PSP_NAND_OPMODE_DMAINTERRUPT != config->opMode)
             && (PSP_NAND_OPMODE_POLLED != config->opMode))
        {
            /* Invalid operating mode has been specified                      */
            result = IOM_EBADARGS;
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Keep a copy of the operating mode                                  */
        hNandObj->opMode = config->opMode;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        /* Sanity check on clock values */
        if (0x0 == config->inputClkFreq)
        {
            result = IOM_EBADARGS;
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Based on the NAND type, select the LLC module functions            */
        if (PSP_NT_NAND == config->nandType)
        {
            /* The NAND type is normal NAND                                   */
            hNandObj->llcFxns = &llcNandFxns;
        }
        else
        {
            /* The NAND type is not supported                                 */
            result = IOM_EBADARGS;
            break;
        }

        /* Initialize the LLC module                                          */
        llcInitCfg.instanceId   = instanceId;
        llcInitCfg.clkFreq      = config->inputClkFreq;
        llcInitCfg.deviceInfo   = config->nandDevInfo;
        llcInitCfg.deviceTiming = config->nandDevTiming;
        llcInitCfg.isPwrAware   = config->pscPwrmEnable;
        llcInitCfg.pllDomain = config->pllDomain;

        if (PSP_NAND_OPMODE_DMAINTERRUPT == config->opMode)
        {
            /* Operating mode is EDMA mode. Validate the hEdma parameter.     */
            if (NULL == config->hEdma)
            {
                result = IOM_EBADARGS;
                break;
            }

            /* Set the LLC DMA initialization parameters                      */
            llcInitCfg.hEdma        = config->hEdma;
            llcInitCfg.edmaEvtQ     = config->edmaEvtQ;
            llcInitCfg.edmaChannel  = config->edmaChannel;
        }
        else
        {
            /* Operating mode is polled mode                                  */
            llcInitCfg.hEdma        = NULL;
        }

        /* Initialize the LLC module                                          */
        llcHandle = hNandObj->llcFxns->initFxn(
                         &llcInitCfg,
                         &hNandObj->devInfo);

        if (NULL == llcHandle)
        {
            /* Initialization of the LLC module is not complete               */
            result = PSP_NAND_E_INIT_FAILED;
            break;
        }

        /* LLC module in successfully initialized. Keep a copy of the LLC     *
         * module handle                                                      */
        hNandObj->llcHandle = llcHandle;
/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        /* Depending on the NAND device organization information, validate the
           compile time defined data items. */
        result = nandValidateMacros(hNandObj,config);
        if (result != IOM_COMPLETED)
        {
            /* One of the user configurable macros has a value that cannot be *
             * supported by this driver. So cannot continue.                  */
            break;
        }

        /* Validate the number of blocks to be protected                      */
        if (((config->protectedBlocks) + (PSP_NAND_RESERVED_BLOCKS + 1u))
            > (hNandObj->devInfo->numBlocks))
        {
            /* Set the return value as invalid parameter                      */
            result = IOM_EBADARGS;
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Fill up the FTL initialization parameters                          */
        ftlInitObj.devInfo         = hNandObj->devInfo;
        ftlInitObj.instanceId      = instanceId;
        ftlInitObj.eraseAtInit     = config->eraseAtInit;
        ftlInitObj.hLlcObj         = llcHandle;
        ftlInitObj.protectedBlocks = config->protectedBlocks;

        if (NULL != hNandObj->llcFxns)
        {
            ftlInitObj.llcFxns = hNandObj->llcFxns;
        }

        /* Initialize the FTL module                                          */
        ftlObj = DDC_nandFtlInit((DDC_nandFtlInitObj*)&ftlInitObj,
                                  &hNandObj->maxSectors,
                                  &result);
        if (NULL == ftlObj)
        {
            /* The FTL initialization has failed. Exit. Deinitialize of the LLC
            module is done before we exit this func */
            break;
        }

        /* Keep a copy of the FTL module handle                               */
        hNandObj->ftlObj = ftlObj;

        /* The FTL has been initialized                                       */
        result = IOM_COMPLETED;
        hNandObj->state = DDC_DRIVER_STATE_CREATED;

    }while (flag);

    /* If there is an error in the initialization and if the LLC module       *
     * initialization is complete, then de-initialize LLC module              */

    if ((result != IOM_COMPLETED) && (llcHandle != NULL))
    {
        /* The LLC module is initialized but due to some other error the NAND *
         * driver initialization was not complete. So deinitialize the        *
         * LLC module.                                                        */
        hNandObj->llcFxns->deinitFxn(llcHandle);
    }

    /* Return the result of initializing the NAND flash                       */
    return (result);
}


/**
 * \brief   Open a NAND driver instance.
 *
 *          This function opens a NAND driver instance. It validates the current
 *          state of the driver first and then sets the state of the driver as
 *          opened.
 */
Ptr PSP_nandOpen(Uint32 instanceId)
{
    DDC_nandObj  *hNandObj;  /* Pointer to instance of the DDC nand module */
    Ptr           handle;    /* Handle of the NAND driver instance         */
    Bool          flag = TRUE;

    do
    {
        /* Initialize the local variables as required                         */
        handle = NULL;
        flag = FALSE;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK

        /* Validate the input parameters                                      */
        if (instanceId >= PSP_NAND_INSTANCES)
        {
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Get the reference pointer to nand object                           */
        hNandObj = &nandObj[instanceId];

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK

        /* Validate the current driver state                                  */
        if ((DDC_DRIVER_STATE_CREATED != hNandObj->state) &&
            (DDC_DRIVER_STATE_CLOSED != hNandObj->state))
        {
            /* The driver is in an invalidate state                           */
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Set the state of the driver as opened                              */
        hNandObj->state = DDC_DRIVER_STATE_OPENED;
        handle = (Ptr)hNandObj;
    }while (flag);

    /* Return the result of opening the NAND driver                           */
    return (handle);
}


/**
 * \brief  Write multiple sectors from the NAND device.
 *
 *         This function writes multiple sectors from the NAND device. The
 *         starting sector number and the number of sectors to write have to be
 *         specified.
 */
Int32 PSP_nandWrite(Ptr          handle,
                    Uint32       logSector,
                    Uint32       numSectors,
                    Uint8        *data)
{
    Int32        result;        /* Result of writing data to NAND device */
    DDC_nandObj  *hNandObj;     /* Handle of the DDC nand object         */
    Bool          flag = TRUE;

    do
    {

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        /* Validate the input parameters */
        if ((NULL == handle) || (NULL == data))
        {
            result = IOM_EBADARGS;
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Get the reference pointer to the DDC NAND instance object          */
        hNandObj = (DDC_nandObj*)handle;
        flag = FALSE;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        /* Validate the current state of the driver                           */
        if (DDC_DRIVER_STATE_OPENED != hNandObj->state)
        {
            /* Driver is in a invalid state                                   */
            result = IOM_EBADMODE;
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Determine if the logical sector specified as parameter is valid    */
        if ((logSector >= hNandObj->maxSectors)
            ||((logSector + numSectors) > hNandObj->maxSectors))
        {
            /* The requested sectors are not within valid logical sectors     */
            result = IOM_EBADARGS;
            break;
        }

        /* Write the data to the NAND device                                  */
        result = DDC_nandFtlWrite(
                      hNandObj->ftlObj,
                      logSector,
                      numSectors,
                      data);

    }while (flag);

    /* Return the result of writing multiple sectors to NAND device           */
    return (result);
}


/**
 * \brief  Read multiple sectors from the NAND device.
 *
 *         This function reads multiple sectors from the NAND device. The
 *         starting sector number and the number of sectors to read have to be
 *         specified.
 */
Int32 PSP_nandRead(Ptr          handle,
                   Uint32       logSector,
                   Uint32       numSectors,
                   Uint8        *data)
{
    Int32         result;     /* Result of writing data to NAND device */
    DDC_nandObj  *hNandObj;   /* Handle of the DDC nand object         */
    Bool          flag = TRUE;

    do
    {

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK

        /* Validate the input parameters                                      */
        if ((NULL == handle) || (NULL == data))
        {
            result = IOM_EBADARGS;
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Get the reference pointer to the DDC NAND instance object          */
        hNandObj = (DDC_nandObj*)handle;
        flag = FALSE;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK

        /* Validate the current state of the driver                           */
        if (DDC_DRIVER_STATE_OPENED != hNandObj->state)
        {
            /* Driver is in a invalid state                                   */
            result = IOM_EBADMODE;
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Determine if the logical sector specified as parameter is valid    */
        if ((logSector >= hNandObj->maxSectors) ||
            ((logSector+numSectors) > hNandObj->maxSectors))
        {
            /* The requested sectors are not within valid logical sectors     */
            result = IOM_EBADARGS;
            break;
        }

        /* Read the data to the NAND device                                   */
        result = DDC_nandFtlRead(
                     hNandObj->ftlObj,
                     logSector,
                     numSectors,
                     data);

        if (result != IOM_COMPLETED)
        {
            /* If the read did not complete, then return read fail error      */
            result = PSP_NAND_E_READ_FAIL;
        }
    }while (flag);

    /* Return the result of reading multiple sectors from NAND device         */
    return (result);
}


/**
 * \brief  Execute a NAND driver IOCTL.
 *
 *         This function executes a IOCTL supported by the NAND driver.
 */
Int32 PSP_nandIoctl(Ptr                handle,
                    PSP_nandIoctlCmd   cmd,
                    Ptr                cmdArg,
                    Ptr                param)
{
    Int32        result;     /* Result of writing data to NAND device */
    DDC_nandObj  *hNandObj;   /* Handle of the DDC nand object         */
    Uint32       logPage;    /* Starting logical page number          */
    Uint32       numPages;   /* Number of pages to be erased          */
    Bool          flag = TRUE;

    do
    {

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        /* Validate the input parameters */
        if ((NULL == handle) || (NULL == cmdArg))
        {
            result = IOM_EBADARGS;
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Initialize the local variables as required                         */
        result = IOM_COMPLETED;
        flag = FALSE;
        hNandObj = (DDC_nandObj*)handle;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK

        /* Validate the current driver state                                  */
        if (DDC_DRIVER_STATE_OPENED != hNandObj->state)
        {
            result = IOM_EBADMODE;
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Execute the requested control command                              */
        switch (cmd)
        {
            case PSP_NAND_IOCTL_ERASE_BLOCK:
                /* Erase the logical pages */
                if (NULL == param)
                {
                    /* Invalid param argument                                 */
                    result = IOM_EBADARGS;
                    break;
                }

                /* Validate the starting logical sector number and the        *
                 * number of sectors to be erased                             */
                logPage  = *(Uint32*)cmdArg;
                numPages = *(Uint32*)param;

                if ((logPage >= hNandObj->maxSectors) ||
                    ((logPage+numPages) > hNandObj->maxSectors))
                {
                    /* The starting logical page number and the number of     *
                     * logical pages to erase are not valid                   */
                    result = IOM_EBADARGS;
                    break;
                }

                /* Erase the logical pages                                    */
                result = DDC_nandFtlErase(hNandObj->ftlObj,
                             (Uint32*)cmdArg,
                             (Uint32*)param);
                break;

            case PSP_NAND_IOCTL_GET_NAND_SIZE:
                /* Return the number of sectors available in the NAND flash   */
                *(Uint32*)cmdArg = (Uint32)(hNandObj->maxSectors);
                break;

            case PSP_NAND_IOCTL_GET_SECTOR_SIZE:
                /* Return the page size                                       */
                *(Uint32*)cmdArg = (Uint32)(hNandObj->devInfo->pageSize);
                break;

            case PSP_NAND_IOCTL_SET_EVENTQ:
                /* Invoke the LLC module IOCTL function to set the EDMA       *
                 * event queue number                                         */
                result = hNandObj->llcFxns->ioctlFxn(
                             hNandObj->llcHandle,
                             (LLC_nandIoctlCmd)LLC_NAND_IOCTL_SET_EDMA_EVTQ,
                             cmdArg);
                break;

            case PSP_NAND_IOCTL_GET_OPMODE:
                /* Return the current operating mode of the NAND driver       */
                *(PSP_NandOpMode*)cmdArg = hNandObj->opMode;
                break;

            case PSP_NAND_IOCTL_GET_DEVICE_INFO:
                /* Copy the device information data                           */
                memcpy(
                    (Ptr)cmdArg,
                    (Ptr)hNandObj->devInfo,
                    sizeof(PSP_nandDeviceInfo));
                break;

            default:
                result = IOM_EBADARGS;
                break;
        }
    }while (flag);

    /* Return the result of executing the IOCTL command                       */
    return (result);
}


/**
 * \brief De-initializes the NAND driver.
 *
 *        This function de-initializes the NAND driver instance. It
 *        deinitializes the FTL layer and the LLC layer..
 */
Int32 PSP_nandClose(Ptr handle)
{
    Int32         result;     /* Result of writing data to NAND device */
    DDC_nandObj  *hNandObj;   /* Handle of the DDC nand object         */
    Bool          flag = TRUE;

    do
    {
        /* Initialize the local variables as required */
        result = IOM_EBADARGS;
        flag = FALSE;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        /* Validate the input parameters                                      */
        if (NULL ==  handle)
        {
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Initialize the local variables after parameter validation          */
        hNandObj = (DDC_nandObj*)handle;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        /* Validate the current driver state                                  */
        if (DDC_DRIVER_STATE_OPENED != hNandObj->state)
        {
            result = IOM_EBADMODE;
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Deinitialize the NAND LLC module                                   */
        result = hNandObj->llcFxns->deinitFxn(hNandObj->llcHandle);
        if (result != IOM_COMPLETED)
        {
            result = PSP_NAND_E_ERROR;
            break;
        }

        /* Set the state of the driver as closed                              */
        hNandObj->state = DDC_DRIVER_STATE_DELETED;
    }while (flag);

    /* Return the result of opening the NAND driver                           */
    return (result);
}

/* ========================================================================== */
/*                       LOCAL FUNCTION DEFINITIONS                           */
/* ========================================================================== */
/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
 /**
 * \brief  Validates the compile-time defined macros
 *
 *         This function validates the values of the compile-time defined macros.
 *         The values of these macros should be compatiable with the NAND device
 *         detected or as expected by the driver.
 *
 * \param  hNandObj  [IN]   Handle of the NAND DDC module instance
 * \param  config    [IN]   Pointer to NAND initialization configuration
 *
 * \return   IOM_COMPLETED              - All compile-time macros are fine.
 *           IOM_EBADARGS  - One of the compile-time macros is incorrect.
 */
static Int32 nandValidateMacros(DDC_nandObj           *hNandObj,
                                const PSP_nandConfig  *config)
{
    PSP_nandDeviceInfo  *devInfo; /* Pointer to NAND dev info                 */
    Int32               result;   /* Result of validating compile time macros */
    Bool                flag = TRUE;
    Uint32              rsvdBlocks = PSP_NAND_RESERVED_BLOCKS;
    Uint32              maxCacheLines = PSP_NAND_MAX_CACHE_LINES;

    do
    {
        /* Initialize the local variables */
        result = IOM_EBADARGS;
        flag = FALSE;
        /* Validate the input parameters                                      */
        assert((NULL != hNandObj) && (NULL != config));

        /* Initialize the local variables after parameter validation          */
        devInfo = hNandObj->devInfo;

        if (NULL == devInfo)
        {
            break;
        }

        /* Validate the number of reserved blocks specified. The number of    *
         * reserved blocks should be atleast one block.                       */
        if (rsvdBlocks == 0u)
        {
            /* There should be atleast one reserved block in the system       */
            break;
        }

        /* Validate the number of reserved blocks specified                   */
        if (devInfo->numBlocks <= PSP_NAND_RESERVED_BLOCKS)
        {
            /* The number of reserved blocks are more than the number of      *
             * available blocks in the NAND device. Modify the value of the   *
             * PSP_NAND_RESERVED_BLOCKS macro.                                */
            break;
        }

        /* Validate the number of pages per block specified                   */
        if (devInfo->pagesPerBlock > PSP_NAND_MAX_PAGES_IN_BLOCK)
        {
            /* The NAND device connected has more number of pages than that is*
             * defined at compile time. Modify the PSP_NAND_MAX_PAGES_IN_BLOCK*
             * to be atleast equal to the number of pages in the NAND device. */
            break;
        }

        /* Validate the maximum page size specified                           */
        if (devInfo->pageSize > PSP_NAND_MAX_PAGE_SIZE)
        {
            /* The NAND device connected has a bigger page size than that is  *
             * defined at compile time. Modify the PSP_NAND_MAX_PAGE_SIZE     *
             * to be atleast equal to the size of the page in the NAND device.*/
            break;
        }

        /* Validate the maximum number of cache lines specified               */
        if (maxCacheLines < 4u)
        {
            /* The number of cache lines specified is less than 4. It should  *
             * be  atleast 4 and it should be a power of 2.                   */
            break;
        }

        /* Validate the maximum number physical blocks specified              */
        if (devInfo->numBlocks > PSP_NAND_FTL_MAX_PHY_BLOCKS)
        {
            /* The number of physical blocks in the NAND device detected is   *
             * more than the compile time value specified. Modify the         *
             * PSP_NAND_FTL_MAX_PHY_BLOCKS macro value.                       */
            break;
        }

        /* Validate the maximum number of logical blocks specified */
        if (((devInfo->numBlocks - PSP_NAND_RESERVED_BLOCKS) -
            config->protectedBlocks) > PSP_NAND_FTL_MAX_LOG_BLOCKS)
        {
            /* The number of available logical blocks is greater than the     *
             * number of compile time specified number of maximum logical     *
             * blocks. Modify the value of the PSP_NAND_FTL_MAX_LOG_BLOCKS    *
             * macro.                                                         */
            break;
        }

        /* All the compile time macros are fine                               */
        result = IOM_COMPLETED;
    }while (flag);

    /* Return the result of validating the compile time macros                */
    return (result);
}
#endif
/* End parameter checking                                                     */

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
