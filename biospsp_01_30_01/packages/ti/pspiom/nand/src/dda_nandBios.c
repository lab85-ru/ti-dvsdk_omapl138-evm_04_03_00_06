/*
 * dda_nandBios.c
 *
 * This file contains NAND media layer implementation. This file implements the
 * Device Driver Adaptation layer functionality
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

/** \file   dda_nandBios.c
 *
 *  \brief  DDA wrapper for interfacing NAND driver with block-media driver.
 *
 *  This file implements the functionality to interface the NAND driver with
 *  the block-media driver. The contents of this driver is targeted for the
 *  BIOS driver implementation.
 *
 *
 *  (C) Copyright 2006, Texas Instruments, Inc
 */

/* ========================================================================== */
/*                            INCLUDE FILES                                   */
/* ========================================================================== */
#include <std.h>
#include <iom.h>
#include <assert.h>

#include <ti/pspiom/blkmedia/psp_blkdev.h>

#include <ti/pspiom/nand/psp_nand.h>
#include "dda_nandBios.h"
#include "ddc_nand.h"
#include "llc_nand.h"

#ifdef NAND_INSTRUMENTATION_ENABLED
#include <ti/sdo/pspdrivers/pal_os/bios/psp_log.h>
#endif /* NAND_INSTRUMENTATION_ENABLED */
/* ========================================================================== */
/*                           LOCAL VARIABLES                                  */
/* ========================================================================== */

/**\brief Handle to the NAND device      */
static Ptr gNandHandle;

/**\brief NAND driver information table  */
static DDA_nandObj gDdaNandObj[DDC_NAND_MAXDEVS];

/**\brief NAND driver busy semaphore     */
static SEM_Handle  gSyncSem;

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */

static Void nandMountCallback(Void);

static Int32 nandRegisterMedia(PSP_BlkDevIdent_t      *pIdent,
                               PSP_blkDevCallback_t    blockDevCb,
                               PSP_BlkDevOps_t       **pDevOps,
                               Ptr                    *pHandle);

static Int32 nandMediaCallback(Ptr handle, Int32 count);

/* ========================================================================== */
/*                        GLOBAL FUNCTION DEFINTIONS                          */
/* ========================================================================== */

/**
 * \brief Initialize the NAND drvier block-media interface module.
 *
 *  This API initializes the NAND driver. Called by the application, this API
 *  initializes the device through the corresponding PSP initialization call.
 *  On a successful initialization, the NAND driver is registered with the
 *  block media and proceeds to check the NAND device(s) on board.
 */
Int32 PSP_nandDrvInit(PSP_nandConfig  *config)
{
    PSP_BlkDrvReg_t        blkDrvRegInfo;
    PSP_BlkDevEvent_t      eventInfo;
    PSP_BlkDrvId_t         device;
    PSP_BlkDrvIoctlInfo_t  drvIoctlInfo;
    Int32                  numDevices;
    Int32                  result;
    SEM_Attrs              semAttr;
    Bool                   flag = TRUE;

    do
    {
        /* Initialize the local variables as required                         */
        result = IOM_EBADARGS;
        flag = FALSE;
        device = PSP_BLK_DRV_NAND;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        /* Validate the input parameters                                      */
        if (NULL == config)
        {
            break;
        }
#endif
/* End parameter checking                                                     */

        /* Initialize the block media registeration information               */
        blkDrvRegInfo.DeviceReg = &nandRegisterMedia;

        for (numDevices = 0u; numDevices < DDC_NAND_MAXDEVS; numDevices++)
        {
            gDdaNandObj[numDevices].devOps.Blk_Ioctl = &DDA_nandIoctl;
            gDdaNandObj[numDevices].devOps.Blk_Read  = &DDA_nandRead;
            gDdaNandObj[numDevices].devOps.Blk_Write = &DDA_nandWrite;
        }

        /* Initialize the NAND driver                                         */
        result = PSP_nandInit(0u, config);

        if (result != IOM_COMPLETED)
        {
            /* NAND initialization failed                                     */
            break;
        }

        /* Open the NAND driver                                               */
        gNandHandle = PSP_nandOpen(0u);

        if (NULL == gNandHandle)
        {
            /* NAND open failed. So cannot continue with the initialization.  */
            result = IOM_EALLOC;
            break;
        }

        /* Create the DDA module synchornization sempahore                    */
        semAttr.name = (String)"Nand_Sync_Sema";
        gSyncSem = SEM_create(0U, &semAttr);
        if (NULL == gSyncSem)
        {
            /* Semaphore create failed. So close the NAND driver and terminate*
             * the NAND initialization.                                       */
            PSP_nandClose(gNandHandle);
            result = IOM_EALLOC;
            break;
        }


        /* Register the DDA module's mount complete callback function with    *
         * block media driver                                                 */
        drvIoctlInfo.Cmd = PSP_BLK_DRV_SET_INIT_COMP_CALLBACK;
        drvIoctlInfo.pData = (Ptr)&nandMountCallback;

        result = PSP_blkmediaDrvIoctl((Ptr)(&device), (Ptr)&drvIoctlInfo);

        if (result != IOM_COMPLETED)
        {
            /* Registering callback with block-media failed                   */
            PSP_nandClose(gNandHandle);
            SEM_delete(gSyncSem);
            break;
        }

        /* Register the NAND driver with the block media driver               */
        result = PSP_blkmediaDrvRegister(PSP_BLK_DRV_NAND, &blkDrvRegInfo);

        if (result != IOM_COMPLETED)
        {
            /* Registeration with block media failed. So close the NAND       *
             * driver and delete the semaphore.                               */
            PSP_nandClose(gNandHandle);
            SEM_delete(gSyncSem);
            break;
        }

        /* Inform the block-media to mount the NAND device                    */
        eventInfo.EventId  = PSP_BLK_EVT_MEDINS;
        eventInfo.EvtData  = &gDdaNandObj[0];
        PSP_blkmediaCallback(PSP_BLK_DRV_NAND, &eventInfo);

        /* Wait for the block-media to ack the mount                          */
        SEM_pend(gSyncSem, SYS_FOREVER);
        SEM_post(gSyncSem);

    }while (flag);

    /* Return the result of initializing the DDC NAND module                  */
    return (result);
}

/**
 * \brief Deinitialize the interface between block-meida and NAND driver.
 *
 *  API that De-initialize the NAND media on the board/in the system,
 *  through the PSP's Deinit call. After the NAND device(s) is (are)
 *  de-initialized read,write and ioctl will not work .
 */
Int32 PSP_nandDrvDeInit(Void)
{
    PSP_BlkDevEvent_t  eventInfo;
    Int32              numDevices;
    Int32              result;
    PSP_BlkDrvReg_t    blkDrvRegInfo;
    Bool               flag = TRUE;

    do
    {
        /* Initialize the local variables as required                         */
        result = IOM_EBADARGS;
        flag = FALSE;

        if (NULL != gNandHandle)
        {
            /* Set all the device operations to NULL                          */
            for (numDevices = 0u; numDevices < DDC_NAND_MAXDEVS; numDevices++)
            {
                gDdaNandObj[numDevices].devOps.Blk_Ioctl = NULL;
                gDdaNandObj[numDevices].devOps.Blk_Read  = NULL;
                gDdaNandObj[numDevices].devOps.Blk_Write = NULL;
            }

            /* Delete the DDC sync semaphore                                  */
            SEM_delete(gSyncSem);

            /* Close the NAND driver                                          */
            result = PSP_nandClose(gNandHandle);

            if (result != IOM_COMPLETED)
            {
                /* Closing NAND driver failed. So do not continue with        *
                 * the deinitialization.                                      */
                result = IOM_EBADMODE;
                break;
            }

            /* Ask the block-media to remove the media                        */
            eventInfo.EventId = PSP_BLK_EVT_MEDREM;
            eventInfo.EvtData = &gDdaNandObj[0];
            PSP_blkmediaCallback(PSP_BLK_DRV_NAND, &eventInfo);

            /* Unregister the NAND driver with the block-media                */
            blkDrvRegInfo.DeviceReg = NULL;
            result = PSP_blkmediaDrvRegister((PSP_BlkDrvId_t)PSP_BLK_DRV_NAND,
                         &blkDrvRegInfo);
        }
    }while (flag);

    /* Return the result of deinitializing the NAND DDA module                */
    return (result);
}

/**
 * \brief Execute a IOCTL command.
 *
 *  This function executes a IOCTL command specified by the block-meida
 *  driver.
 */
Int32 DDA_nandIoctl(Ptr                           handle,
                    Ptr                           mHandle,
                    const PSP_BlkDevIoctlInfo_t  *pIoctlInfo,
                    Bool                         *pIsComplete)
{
    DDA_nandObj  *pDevice;
    Uint32       *pData;
    Uint32       *pData1;
    Int32         result;
    Uint32        ioctlCmd;
    Bool          flag = TRUE;

    do
    {
        /* Validate the input parameters                                      */
        assert(((NULL != handle) && (NULL != mHandle)) &&
               ((NULL != pIoctlInfo) && (NULL != pIsComplete)));
        mHandle = mHandle; /* To remove compilation warning */
        /* Initialization the local variables as required                     */
        flag = FALSE;
        if (NULL == pIoctlInfo->pData)
        {
            result = IOM_EBADARGS;
            break;
        }
        ioctlCmd = (Uint32)pIoctlInfo->Cmd;
        if ((PSP_BLK_IOCTL_MAX + PSP_NAND_IOCTL_ERASE_BLOCK) == ioctlCmd)
        {
            /* For the erase command, we would require the number of pages    *
             * ensure that its supplied                                       */
            if (NULL == pIoctlInfo->pData1)
            {
                result = IOM_EBADARGS;
                break;
            }
        }

        pDevice = (DDA_nandObj*)handle;
        pData   = (Uint32*)pIoctlInfo->pData;
        pData1  = (Uint32*)pIoctlInfo->pData1;

        /* Execute the command specified */
        *pIsComplete = TRUE;

        switch (ioctlCmd)
        {
            case PSP_BLK_GETSECTMAX:
                /* Get the total number of sectors in the NAND device         */
                result = PSP_nandIoctl(
                             pDevice->handle,
                             PSP_NAND_IOCTL_GET_NAND_SIZE,
                             pData,
                             NULL);

                if (result != IOM_COMPLETED)
                {
                    result = IOM_EBADARGS;
                }
                break;

            case PSP_BLK_GETBLKSIZE:
                /* Get the page size of the NAND device                       */
                result = PSP_nandIoctl(
                             pDevice->handle,
                             PSP_NAND_IOCTL_GET_SECTOR_SIZE,
                             pData,
                             NULL);

                if (result != IOM_COMPLETED)
                {
                    result = IOM_EBADARGS;
                }
                break;

            case PSP_BLK_GETOPMODE:
                /* Determine the current operating mode of the driver         */
                result = PSP_nandIoctl(
                             pDevice->handle,
                             PSP_NAND_IOCTL_GET_OPMODE,
                             pData,
                             NULL);

                if (result != IOM_COMPLETED)
                {
                    result = IOM_EBADARGS;
                }
                break;

            case PSP_BLK_SETOPMODE:
            case PSP_BLK_DEVRESET:
            case PSP_BLK_SETPWRMODE:
            case PSP_BLK_GETWPSTAT:
                /* These IOCTLS are not suported                              */
                result = IOM_COMPLETED;
                break;

            case PSP_BLK_GETREMSTAT:
                /* Non Removable Media                                    */
                *(Bool *)pData = FALSE;
                result = IOM_COMPLETED;
                break;

            case PSP_BLK_SETEVENTQ:
                /* Set the EDMA event queue                                   */
                result =  PSP_nandIoctl(
                              pDevice->handle,
                              PSP_NAND_IOCTL_SET_EVENTQ,
                              (Ptr)pData,
                              NULL);

                if (result != IOM_COMPLETED)
                {
                    result = IOM_EBADARGS;
                }
                break;

            /* The command could be a NAND driver specific IOCTL              */
            case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_NAND_IOCTL_GET_NAND_SIZE):
                /* Get the total number of sectors in the NAND device         */
                result = PSP_nandIoctl(
                             pDevice->handle,
                             PSP_NAND_IOCTL_GET_NAND_SIZE,
                             pData,
                             NULL);
                if (result != IOM_COMPLETED)
                {
                    result = IOM_EBADARGS;
                }
                break;

            case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_NAND_IOCTL_GET_SECTOR_SIZE):
                /* Get the size of the page of NAND device                    */
                result = PSP_nandIoctl(
                             pDevice->handle,
                             PSP_NAND_IOCTL_GET_SECTOR_SIZE,
                             pData,
                             NULL);

                if (result != IOM_COMPLETED)
                {
                    result = IOM_EBADARGS;
                }
                break;


            case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_NAND_IOCTL_SET_EVENTQ):
                /* Set the EDMA event queue                                   */
                result = PSP_nandIoctl(
                             pDevice->handle,
                             PSP_NAND_IOCTL_SET_EVENTQ,
                             (Ptr)pData,
                             NULL);

                if (result != IOM_COMPLETED)
                {
                    result = IOM_EBADARGS;
                }
                break;

            case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_NAND_IOCTL_ERASE_BLOCK):
                /* Erase a NAND block                                         */
                if (TRUE != SEM_pend(gSyncSem, SYS_FOREVER))
                {
                    result = IOM_EBADARGS;
                    break;
                }

                /* Issue NAND Ioctl to erase a block                          */
                result = PSP_nandIoctl(
                             pDevice->handle,
                             PSP_NAND_IOCTL_ERASE_BLOCK,
                             pData,
                             pData1);

                SEM_post(gSyncSem);
                break;

            case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_NAND_IOCTL_GET_OPMODE):
                /* Determine the current operating mode of the driver         */
                result = PSP_nandIoctl(
                             pDevice->handle,
                             PSP_NAND_IOCTL_GET_OPMODE,
                             pData,
                             NULL);

                if (result != IOM_COMPLETED)
                {
                    result = IOM_EBADARGS;
                }
                break;

            case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_NAND_IOCTL_GET_DEVICE_INFO):
                /* Get the device info of the NAND device                     */
                result = PSP_nandIoctl(
                             pDevice->handle,
                             PSP_NAND_IOCTL_GET_DEVICE_INFO,
                             pData,
                             NULL);

                if (result != IOM_COMPLETED)
                {
                    result = IOM_EBADARGS;
                }
                break;

            default:
                /* Unknown IOCTL command is specified                         */
                result = IOM_EBADARGS;
                break;
        }
    }while (flag);

    /* Return the result of executing the IOCTL command                       */
    return (result);
}

/**
 * \brief Read data from the NAND device.
 *
 *  This function reads data from the NAND device. The starting logical sector
 *  number and the number of sectors to be read have to be specified.
 */
Int32 DDA_nandRead(Ptr     handle,
                   Ptr     mHandle,
                   Ptr     buffer,
                   Uint32  sectNum,
                   Uint32  numSect)
{
    Int32   result;
    Bool    flag = TRUE;
#ifdef NAND_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
                  "NAND",
                  PSP_DVT_DESC(PSP_DVT_eFUNC_START,
                               PSP_DVT_dCHANNEL_ID_INST,
                               PSP_DVT_dPACKET_ID,
                               PSP_DVT_dSIZE_BYTES),
                  CHANNELID_INSTANCEID(
                      0,
                      0),
                  buffer,
                  numSect);
#endif /* NAND_INSTRUMENTATION_ENABLED */
    do
    {
        /* Initialize the local variables as required                         */
        result = IOM_EBADARGS;
        flag = FALSE;

        /* Validate the input paramaters                                      */
        assert(((NULL != handle) && (NULL != mHandle)) && (NULL != buffer));
#ifdef BIOS_PWRM_ENABLE
        if (TRUE == ((LLC_nandObj*)((DDC_nandObj*)((DDA_nandObj*)handle)->handle)->llcHandle)->isSuspended)
        {
            /* Driver is in a invalid state                                   */
            result = IOM_EBADMODE;
            break;
        }
#endif
        /* Lock the read path                                                 */
        if (SEM_pend(gSyncSem, SYS_FOREVER) != TRUE)
        {
            break;
        }
        ((LLC_nandObj*)((DDC_nandObj*)((DDA_nandObj*)handle)->handle)->llcHandle)->submitCount++;

        /* Read the data from the NAND device                                 */
        result = PSP_nandRead(
                     ((DDA_nandObj*)handle)->handle,
                     sectNum,
                     numSect,
                     (Uint8 *)buffer);
        ((LLC_nandObj*)((DDC_nandObj*)((DDA_nandObj*)handle)->handle)->llcHandle)->submitCount--;

        /* Unlock the read path                                               */
        SEM_post(gSyncSem);

        /* Inform the block-meda that the read is complete                    */
        nandMediaCallback(mHandle, result);
    }while (flag);
#ifdef NAND_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
                  "NAND",
                  PSP_DVT_DESC(PSP_DVT_eFUNC_END,
                               PSP_DVT_dCHANNEL_ID_INST,
                               PSP_DVT_dPACKET_ID,
                               PSP_DVT_dSIZE_BYTES),
                  CHANNELID_INSTANCEID(
                      0,
                      0),
                  buffer,
                  0);
#endif /* NAND_INSTRUMENTATION_ENABLED */
    /* Return the result of reading data from the NAND device                 */
    return (result);
}

/**
 * \brief Write data to the NAND device.
 *
 *  This function writes data to the NAND device. The starting logical sector
 *  number and the number of sectors to be read have to be specified.
 */
Int32 DDA_nandWrite(Ptr     handle,
                    Ptr     mHandle,
                    Ptr     buffer,
                    Uint32  sectNum,
                    Uint32  numSect)
{
    Int32   result;
    Bool    flag = TRUE;
#ifdef NAND_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
        "NAND",
        PSP_DVT_DESC(   PSP_DVT_eFUNC_START,
                        PSP_DVT_dCHANNEL_ID_INST,
                        PSP_DVT_dPACKET_ID,
                        PSP_DVT_dSIZE_BYTES),
        CHANNELID_INSTANCEID(
            1,
            0),
        buffer,
        numSect);
#endif /* NAND_INSTRUMENTATION_ENABLED */
    do
    {
        /* Initialize the local variables as required                         */
        result = IOM_EBADARGS;
        flag = FALSE;

        /* Validate the input paramaters                                      */
        assert(((NULL != handle) && (NULL != mHandle)) && (NULL != buffer));
#ifdef BIOS_PWRM_ENABLE
        if (TRUE == ((LLC_nandObj*)((DDC_nandObj*)((DDA_nandObj*)handle)->handle)->llcHandle)->isSuspended)
        {
            /* Driver is in a invalid state                                   */
            result = IOM_EBADMODE;
            break;
        }
#endif
        /* Lock the write path                                                */
        if (SEM_pend(gSyncSem, SYS_FOREVER) != TRUE)
        {
            break;
        }
        ((LLC_nandObj*)((DDC_nandObj*)((DDA_nandObj*)handle)->handle)->llcHandle)->submitCount++;
        /* Write the data to the NAND device                                  */
        result = PSP_nandWrite(
                     ((DDA_nandObj*)handle)->handle,
                     sectNum,
                     numSect,
                     (Uint8*)buffer);
        ((LLC_nandObj*)((DDC_nandObj*)((DDA_nandObj*)handle)->handle)->llcHandle)->submitCount--;

        /* Unlock the write path                                              */
        SEM_post(gSyncSem);

        /* Inform the block-meda that the write is complete                   */
        nandMediaCallback(mHandle, result);
    }while (flag);
#ifdef NAND_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
        "NAND",
        PSP_DVT_DESC(PSP_DVT_eFUNC_END,
                     PSP_DVT_dCHANNEL_ID_INST,
                     PSP_DVT_dPACKET_ID,
                     PSP_DVT_dSIZE_BYTES),
        CHANNELID_INSTANCEID(
            1,
            0),
        buffer,
        0);
#endif /* NAND_INSTRUMENTATION_ENABLED */
    /* Return the result of write to NAND device                              */
    return (result);
}


/* ========================================================================== */
/*                         LOCAL FUNCTION DEFINTIONS                          */
/* ========================================================================== */

/**
 * \brief  Block-media call back notification
 *
 *         This function is the interface to the block media. It is used to
 *         invoke the block-media callback function on success/error.
 *
 * \param  handle   [IN]    Media handle
 * \param  count    [IN]    Error count
 *
 * \return IOM_COMPLETED
 */
static Int32 nandMediaCallback(Ptr handle, Int32 count)
{
    Int32               result = IOM_COMPLETED;
    PSP_BlkDevRes_t   Res;

    assert(NULL != handle);

    Res.IsEvent = FALSE;

    if (0 == count)
    {
        Res.ReqInfo.Status.Error = IOM_COMPLETED;
    }
    else
    {
        Res.ReqInfo.Status.Error = IOM_EBADARGS;
    }

    /* NAND Driver interface Block Device Callback                            */
    gDdaNandObj[0].devCb(handle, (PSP_BlkDevRes_t *)&Res);

    return result;
}


/**
 * \brief  Register for the block device.
 *
 *         This function registers the (NAND) media with the block media driver.
 *
 * \param  pIdent       [IN]    Identification information.
 * \param  blockDevCb   [IN]    Callback routine for the registered block
 *                              Device.
 * \param  pDevOps      [OUT]   Block Device Interface Operations.
 * \param  pHandle      [OUT]   Handle to device (to be used for future
 *                              transactions).
 * \return Success/Error
 */
static Int32 nandRegisterMedia(PSP_BlkDevIdent_t      *pIdent,
                               PSP_blkDevCallback_t    blockDevCb,
                               PSP_BlkDevOps_t       **pDevOps,
                               Ptr                    *pHandle)
{
    Int32  devNum;
    Int32  result;
    Bool   flag = TRUE;

    pIdent = pIdent;      /* to remove compilation warning */

    do
    {
        /* Initialize the local variables as required                         */
        devNum = 0u;
        flag = FALSE;
        result = IOM_COMPLETED;

        /* Validate the input parameters                                      */
        assert(((NULL != pDevOps) && (NULL != pHandle)) &&(NULL != blockDevCb));

        /* Supply all the registation information                             */
        gDdaNandObj[devNum].handle = gNandHandle;
        *pDevOps = (PSP_BlkDevOps_t *)&(gDdaNandObj[devNum].devOps);
        *pHandle = &gDdaNandObj[devNum];
        gDdaNandObj[devNum].devCb = blockDevCb;
    }while (flag);

    /* Return the result of supplying the registeration information to block  *
     * media                                                                  */
    return (result);
}


/**
 *  \brief  Callback function called by block-media after mount of NAND.
 *
 *  The function is called by the block-media driver after the media has
 *  been mounted, to notify that device has been mounted with filesystem.
 *  This function should be registered with the block-media driver.
 *
 * \return None.
 */
static Void nandMountCallback(Void)
{
    SEM_post(gSyncSem);
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
