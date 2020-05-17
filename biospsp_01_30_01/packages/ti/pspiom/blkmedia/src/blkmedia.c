/*
 * blkmedia.c
 *
 * This file contains Block Media layer implementation.
 * Block Media driver layer interface provides abstraction over lower level
 * storage/media device drivers like MMCSD/NAND
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

/** \file   blkmedia.c
 *
 *  \brief  Block Device Driver file
 *
 *  This file implements the Block device driver functions.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 */


/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include <iom.h>
#include <hwi.h>
#include <tsk.h>
#include <sys.h>
#include <bcache.h>
#include <string.h>
#include <assert.h>

#include <ti/sdo/edma3/drv/edma3_drv.h>

#include "ti/pspiom/blkmedia/psp_blkdev.h"
#include "blkmedia.h"

#ifdef BLKMEDIA_INSTRUMENTATION_ENABLED
#include <ti/sdo/pspdrivers/pal_os/bios/psp_log.h>
#endif /* BLKMEDIA_INSTRUMENTATION_ENABLED */

/**< make this define equal to printf x for debugging. Also it may be required
 to include stdio.h, if prints are not coming properly.                       */
#define BLKMED_ERR_DEBUG(x)

#define BLK_CACHE_LINE_SIZE_IN_BYTES (128u)    /**< Data cache line length */
#define EMEMCOPY_SEM_TAKE_TIMEOUT  (0x2000u)   /**< Edma mem-copy timeout  */

#if (PSP_FILE_SYSTEM)
#define NAND_HEADS                 (16u)       /**< NAND Head size         */
#define NAND_SPT                   (32u)       /**< NAND sector per track  */
/* ertfs */
#include <ti/rtfs/ebs/include/rtfs.h>
#include <ti/rtfs/bfs.h>
#include "ti/pspiom/mmcsd/psp_mmcsd.h"
#include "ti/pspiom/nand/psp_nand.h"

/* Local function that maps device parameters to an rtfs_media_insert_args
 structure and calls pc_rtfs_media_insert() */
static Int32 blkmediaRtfsMountDevice(BLK_DEV_Info *pDevice, Uint32 devNum);

/* Configuration functions: Pointers to these funtions are attached to the
 rtfs_media_insert_args structure that is passed to pc_rtfs_media_insert() */
int FSADAPTMEM_device_configure_media(
        struct rtfs_media_insert_args *pmedia_parms,
        struct rtfs_media_resource_reply *pmedia_config_block,
        int sector_buffer_required);

int FSADAPTMEM_device_configure_volume(
        struct rtfs_volume_resource_request *prequest_block,
        struct rtfs_volume_resource_reply *pvolume_config_block);

/* IO functions: Pointers to these funtions are attached to the
 rtfs_media_insert_args structure that is passed to pc_rtfs_media_insert().
 Entry points passed to Rtfs. These functions are called by Rtfs to acquire
 blk dev services. */
static Int32 blkmediaIo(Void *devHandle,
                        Void *pdrive,
                        Uint32 sector,
                        Void *pBuf,
                        Uint32 numOfSector,
                        Int32 reading);

static int blkmediaEraseSectors(Void *devHandle,
                                Void *pdrive,
                                Uint32 start_sector,
                                Uint32 n_sectors);

static int blkmediaIoctl(Void *devHandle,
                         Void *pdrive,
                         int opcode,
                         int iArgs,
                         Void *vargs);

static Int32 blkmediaGetDeviceParams(Uint32 DevNo, BLK_DEV_Info *pDevice);
#endif

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Int32 blkmediaReadIo(Ptr handle,
                            Ptr mediaHandle,
                            Ptr buffer,
                            Uint32 sector,
                            Uint32 numSect);

static Int32 blkmediaReadAsync(Ptr handle,
                               Ptr mediaHandle,
                               Ptr buffer,
                               Uint32 sector,
                               Uint32 numSect);

static Int32 blkmediaWriteIo(Ptr handle,
                             Ptr mediaHandle,
                             Ptr buffer,
                             Uint32 sector,
                             Uint32 numSect);

static Int32 blkmediaWriteAsync(Ptr handle,
                                Ptr mediaHandle,
                                Ptr buffer,
                                Uint32 sector,
                                Uint32 numSect);

static Int32 blkmediaRegisterMedia(PSP_BlkDrvId_t driverId);

static Void blkmediaTask(Void);

static Int32 blkmediaIntIoctl(Ptr handle,
                              Ptr mediaHandle,
                              PSP_BlkDevIoctlInfo_t const *pIoctlInfo,
                              Bool *pIsComplete);

static Bool blkmediaCallback(Ptr handle, PSP_BlkDevRes_t const *pRes);

static Int32 blkmediaVoidIo(Ptr handle,
                            Ptr mediaHandle,
                            Ptr buffer,
                            Uint32 sector,
                            Uint32 numSect);

static Int32 blkmediaIoctlVoid(Ptr handle,
                               Ptr mediaHandle,
                               const PSP_BlkDevIoctlInfo_t *pIoctlInfo,
                               Bool *pIsComplete);

static BLK_DEV_Req* blkmediaAllocReq(BLK_DEV_Info *pDrive);

static Void blkmediaRelReq(BLK_DEV_Info *pDrive, BLK_DEV_Req *pReq);

static Int32 blkmediaEdmaMemcpy(Ptr hEdma,
                                Uint8 *dst,
                                Uint8 *src,
                                Uint32 len,
                                BLK_DEV_DataDir dir,
                                Uint32 sinch1Id,
                                Uint32 sinch1Tcc);

static Int32 blkmediaEdmaMemcpyConfig(Ptr hEdma,
                                      Uint8 *dst,
                                      Uint8 *src,
                                      Uint32 len,
                                      Uint32 sinch1Id,
                                      Uint32 sinch1Tcc);

static Ptr              gBlkmediaSyncEdmaHandle = NULL;
/**< hEdma handle stores here for Sync IO operation                           */
static Ptr              gBlkmediaAsyncEdmaHandle = NULL;
/**< hEdma handle stores here for RAW IO operation                            */
static Uint32           gBlkmediaIoChId = EDMA3_DRV_DMA_CHANNEL_ANY;
/**< ChannelId  for Sync IO operation                                         */
static Uint32           gBlkmediaIoTcc = EDMA3_DRV_TCC_ANY;
/**< TCC for Sync IO operation                                                */
static Uint32           gBlkmediaAsyncChId = EDMA3_DRV_DMA_CHANNEL_ANY;
/**< ChannelId for RAW IO operation                                           */
static Uint32           gBlkmediaAsyncTcc = EDMA3_DRV_TCC_ANY;
/**< TCC for RAW IO operation                                                 */
static BLK_DEV_Info   gBlkmediaInfo[PSP_BLK_DEV_MAXDEV];
/**< Block Device Information                                                 */
static PSP_BlkDrvReg_t  gBlkmediaReg[PSP_BLK_DRV_MAX];
/**< Driver Registration Info                                                 */
static SEM_Handle       gBlkmediaEvtSem;
/**< Block Driver Event semaphore                                             */
static SEM_Handle       gBlkmediaMountSem;
/**< Block Driver Mount semaphore                                             */
static BLK_DEV_Req    gBlkmediaReq[BLK_DEV_MAXREQ];
/**< Request completion queue cache                                           */
BLK_DEV_RAW_Dev         gBlkmediaRawDev = {PSP_BLK_DRV_MAX};
/**< RAW Device information                                                   */
static TSK_Handle       gBlkmediaTaskHandle = NULL;
/**< Blk-Dev Task handle                                                      */
static Bool             gBlkmediaOpen = FALSE;
/**< Block media driver is opened or not                                      */

#ifdef PSP_BUFF_ALIGNMENT/* Added for Buffer alignment */
    #ifdef PSP_MULTI_TASK_BUFFERS
        /**< DATA_ALIGN pragma for gBlkmediaIoBuffer buffer alignment*/
        #pragma DATA_ALIGN(gBlkmediaIoBuffer, BLK_CACHE_LINE_SIZE_IN_BYTES);
        Uint8 gBlkmediaIoBuffer[PSP_BUFFER_IO_SIZE * PSP_BLK_DRV_MAX];
        /**< Sync IO buffer                                                   */

        /**< DATA_ALIGN pragma for gBlkmediaAsyncBuffer buffer alignment*/
        #pragma DATA_ALIGN(gBlkmediaAsyncBuffer, BLK_CACHE_LINE_SIZE_IN_BYTES);
        Uint8 gBlkmediaAsyncBuffer[PSP_BUFFER_ASYNC_SIZE];
        /**< Async IO buffer. Not made multiple buffers here, as at any point
        of time only one device can be registered for RAW access to block media.
        */
    #else
        /**< DATA_ALIGN pragma for gBlkmediaIoBuffer buffer alignment*/
        #pragma DATA_ALIGN(gBlkmediaIoBuffer, BLK_CACHE_LINE_SIZE_IN_BYTES);
        Uint8 gBlkmediaIoBuffer[PSP_BUFFER_IO_SIZE];
        /**< Sync IO buffer                                                   */

        /**< DATA_ALIGN pragma for gBlkmediaAsyncBuffer buffer alignment*/
        #pragma DATA_ALIGN(gBlkmediaAsyncBuffer, BLK_CACHE_LINE_SIZE_IN_BYTES);
        Uint8 gBlkmediaAsyncBuffer[PSP_BUFFER_ASYNC_SIZE];
        /**< Async IO buffer                                                  */
    #endif
#endif

#if defined(PSP_BLK_EDMA_MEMCPY_IO)
static SEM_Handle gBlkmediaIoEdmaSem;   /**< IO Semaphore for edma copy */
static Void blkmediaIoEdmaCallback(Uint32 tcc,
                                   EDMA3_RM_TccStatus status,
                                   Ptr appData);
#endif

#if defined(PSP_BLK_EDMA_MEMCPY_ASYNC)
static SEM_Handle gBlkmediaAsyncEdmaSem; /**< Async Semaphore for edma copy */
static Void blkmediaAsyncEdmaCallback(Uint32 tcc,
                                      EDMA3_RM_TccStatus status,
                                      Ptr appData);
#endif
/* ========================================================================== */
/*                          FUNCTION DEFINTIONS                               */
/* ========================================================================== */


/* ========================================================================== */
/*                         EXTERNAL FUNCTIONS                                 */
/* ========================================================================== */


/**
 *  \brief Block Driver Callback interface. Used for propagating events from the
 *         underlying storage drivers to the block driver independent of the
 *         device context (Ex. Device insertion/removal, media write protected).
 *
 *  \param driverId   [IN]  : Id of the Storage Driver
 *  \param pEventInfo [IN]  : Storage Driver Device Event information.
 *
 *  \return Void             : None
 ******************************************************************************/
Void PSP_blkmediaCallback(PSP_BlkDrvId_t driverId,
                          PSP_BlkDevEvent_t const *pEventInfo)
{
    /*Assumption being that we support only one one device per storage driver.*/
    BLK_DEV_Info      *pDevice = &gBlkmediaInfo[driverId];
    Uint32              devNum = 0;
    Uint32              cookie;
    PSP_BlkDevRes_t     res;
    Int32 retVal = IOM_COMPLETED;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (((NULL == pEventInfo) || (driverId >= PSP_BLK_DRV_MAX)) ||
        (NULL == pDevice))
    {
        retVal = IOM_EBADARGS;
    }
#endif
/* End parameter checking                                                     */

    if(IOM_COMPLETED == retVal)
    {
        cookie = HWI_disable();
        switch (pEventInfo->EventId)
        {
            case PSP_BLK_EVT_MEDINS :
                /* Wake up the Block Driver task to do the device register for this
                storage device*/
                pDevice->DevState = BLK_DEV_AVAIL;
                SEM_post(gBlkmediaEvtSem);
                break;

            case PSP_BLK_EVT_MEDREM :
                while ((pEventInfo->EvtData != gBlkmediaInfo[devNum].DevHandle) &&
                       (++devNum < PSP_BLK_DEV_MAXDEV))
                {
                    ;
                }

                if (devNum < PSP_BLK_DEV_MAXDEV)
                {
                    if (BLK_DEV_REG == gBlkmediaInfo[devNum].DevState)
                    {
#if (PSP_FILE_SYSTEM)
                        /* Tell Rtfs that this device is removed */
                        pc_rtfs_media_alert((Void *)&gBlkmediaInfo[devNum],
                                            RTFS_ALERT_EJECT,
                                            0);
#endif
                        gBlkmediaInfo[devNum].pDevOps = NULL;
                        gBlkmediaInfo[devNum].isMediaWriteProtect = FALSE;
                        gBlkmediaInfo[devNum].DevHandle = NULL;
                        gBlkmediaInfo[devNum].maxUserSect = 0;
                        gBlkmediaInfo[devNum].sectorSize = 0;
                        gBlkmediaInfo[devNum].Cyl = 0;
                        gBlkmediaInfo[devNum].Head = 0;
                        gBlkmediaInfo[devNum].sectPerTrack = 0;
                        gBlkmediaInfo[devNum].BlkOps.Blk_Read  = &blkmediaVoidIo;
                        gBlkmediaInfo[devNum].BlkOps.Blk_Write = &blkmediaVoidIo;
                        gBlkmediaInfo[devNum].BlkOps.Blk_Ioctl = &blkmediaIoctlVoid;
                    }
                    gBlkmediaInfo[devNum].DevState = BLK_DEV_INIT;
                }
                break;

            default :
                break;
        }
        HWI_restore(cookie);

        /* Propogate the events to RAW application if remote client connected */
        if (TRUE == pDevice->isRawRegistered)
        {
            res.IsEvent = TRUE;
            res.ReqInfo.Event.EventId = pEventInfo->EventId;
            pDevice->appCb(NULL, &res);
        }
    }
}


/**
 *  \brief The Media Driver clients like devices registered for RAW access
 *         shall use this function to register a storage driver for RAW access
 *         for a Block media device.
 *
 *  \param appCb   [IN]  : Address of the callback function of application which
 *                         will be called after every read and write.
 *  \param pIntOps [OUT] : Block Interface driver DevOps having read write and
 *                         ioctl function pointers. PSP_BlkDevOps_t structure
 *                         will contain address of a read write and ioctl
 *                         function after returning from this function. This
 *                         will be use by application for read , write and ioctl
 *                         functions of storage device.
 *  \param pHandle [OUT] : Block Driver Device Handle for the storage device.
 *                         This will be the first arg of read, write and ioctl
 *                         functions called by the application.
 *
 *  \return Int32        : IOM_EBADARGS/IOM_COMPLETED.
 ******************************************************************************/
Int32 PSP_blkmediaAppRegister(PSP_blkDevCallback_t  appCb,
                              PSP_BlkDevOps_t **pIntOps,
                              Ptr *pHandle)
{
    Int32             device = 0;
    Int32             err = IOM_COMPLETED;
    Uint32            cookie;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (((NULL == appCb) || (NULL == pIntOps)) || (NULL == pHandle))
    {
        err = IOM_EBADARGS;
    }
#endif
/* End parameter checking                                                     */

    if(IOM_COMPLETED == err)
    {
        *pIntOps = NULL;
        *pHandle = NULL;

        cookie = HWI_disable();

        while (device < PSP_BLK_DEV_MAXDEV)
        {
            if ((gBlkmediaInfo[device].DevBus == gBlkmediaRawDev.BLKAPP_DEV) &&
                (FALSE == gBlkmediaInfo[device].isRawRegistered) &&
                (gBlkmediaInfo[device].DevHandle != NULL) &&
                (BLK_DEV_MAXQUEUE == gBlkmediaInfo[device].AvailReq) &&
                (BLK_DEV_REG == gBlkmediaInfo[device].DevState))
            {
#if (PSP_FILE_SYSTEM)
                /* Eject all media during RAW usage by telling the File system
                that this device is removed */
                pc_rtfs_media_alert((Void *)&gBlkmediaInfo[device],
                                    RTFS_ALERT_EJECT,
                                    0);
#endif
                gBlkmediaInfo[device].isRawRegistered = TRUE;
                *pHandle = (Ptr)&gBlkmediaInfo[device];
                gBlkmediaInfo[device].BlkOps.Blk_Read  = &blkmediaReadAsync;
                if (TRUE == gBlkmediaInfo[device].isMediaWriteProtect)
                {
                    gBlkmediaInfo[device].BlkOps.Blk_Write = &blkmediaVoidIo;
                }
                else
                {
                    gBlkmediaInfo[device].BlkOps.Blk_Write = &blkmediaWriteAsync;
                }
                *pIntOps = &gBlkmediaInfo[device].BlkOps;
                gBlkmediaInfo[device].appCb = appCb;
                err = IOM_COMPLETED;
                break;
            }
            device++;
        }

        if (PSP_BLK_DEV_MAXDEV == device)
        {
            err = IOM_EBADARGS;
        }

        HWI_restore(cookie);
    }
    return err;
}


/**
 *  \brief The Media Driver clients like Mass Storage drivers shall use this
 *         function to un-register from a Block device. Should be called only in
 *         task context.
 *
 *  \param handle [IN]  : Block Device Device handle.
 *
 *  \return Int32       : IOM_EBADARGS/IOM_COMPLETED
 ******************************************************************************/
Int32 PSP_blkmediaAppUnRegister(Ptr handle)
{
    BLK_DEV_Info    *pDevice;
    Int32           err;
    Bool            flag = TRUE;

    do
    {
        /* Initialize the local variables as required                         */
        err = IOM_EBADARGS;
        flag = FALSE;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if (NULL == handle)
        {
            break;
        }
#endif
/* End parameter checking                                                     */
        pDevice = (BLK_DEV_Info *)handle;

        if (((pDevice != NULL) && (TRUE == pDevice->isRawRegistered)) &&
            (BLK_DEV_MAXQUEUE == pDevice->AvailReq))
        {
            pDevice->isRawRegistered = FALSE;
            if (pDevice->pDevOps != NULL)
            {
                pDevice->BlkOps.Blk_Read  = &blkmediaReadIo;
                pDevice->BlkOps.Blk_Write = &blkmediaWriteIo;
#if (PSP_FILE_SYSTEM)
                /* Call this API to notify the file system that the device is
                available now for use*/
                err = blkmediaRtfsMountDevice(pDevice, (Uint32)pDevice->devNum);
                if (err != IOM_COMPLETED)
                {
                    BLKMED_ERR_DEBUG(("\r\nBLK_MED: Rtfs_Mount_Device failed"));
                }
#endif
            }
            pDevice->appCb = NULL;
            err = IOM_COMPLETED;
        }
    }while (flag);

    return err;
}


/**
 *  \brief This function intialize the block media driver, take the resources,
 *         initialze the data structure and create a block media task for
 *         storage driver registration. Block media needs to be intialized
 *         before any intialization to storage driver(if block media is used to
 *         access the storage driver). This function also intialize the file
 *         system(if supported). This function needs to be called only once
 *         from the application and not to be called for every storage media
 *         init.
 *
 *  \param hEdma      [IN] EDMA driver handle
 *  \param edmaEventQ [IN] EDMA Event Queue number to be used for Blk-Media
 *  \param taskPrio   [IN] Block media task prority. The priority should be
 *                         greater than any other storage task priority. The
 *                         value should be in supported rage of OS
 *  \param taskSize   [IN] Stack size for Blk-Media task. Minimum 4Kbytes
 *
 *  \return Int32      : Success/Error
 ******************************************************************************/
Int32 PSP_blkmediaDrvInit(Ptr hEdma,
                          Uint32 edmaEventQ,
                          Uint32 taskPrio,
                          Uint32 taskSize)
{
    SEM_Attrs       semAttr;
    Uint8           devNum = 0;
    BLK_DEV_Req     *pReq  = gBlkmediaReq;
    Uint8           cnt = 0;
    Int32           result;
    TSK_Attrs       attrs;
    Bool            flag = TRUE;

    do
    {
        /* Initialize the local variables as required                         */
        result = IOM_EBADARGS;
        flag = FALSE;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((NULL == hEdma) || (NULL == pReq))
        {
            break;
        }
#endif
/* End parameter checking                                                     */

        if (gBlkmediaOpen == TRUE)
        {
            result = IOM_COMPLETED;
            break;
        }

        gBlkmediaSyncEdmaHandle = gBlkmediaAsyncEdmaHandle = NULL;
#if defined(PSP_BLK_EDMA_MEMCPY_IO)
        gBlkmediaSyncEdmaHandle = hEdma;
#endif
#if defined(PSP_BLK_EDMA_MEMCPY_ASYNC)
        gBlkmediaAsyncEdmaHandle = hEdma;
#endif

        memset(gBlkmediaInfo, 0, sizeof(BLK_DEV_Info) * PSP_BLK_DEV_MAXDEV);
        memset(gBlkmediaReg, 0, sizeof(PSP_BlkDrvReg_t) * PSP_BLK_DRV_MAX);
        memset(gBlkmediaReq, 0, sizeof(BLK_DEV_Req) * BLK_DEV_MAXREQ);

        /* Blk Driver event semaphore */
        semAttr.name = (String)"Blkmedia_Event_Sema";
        gBlkmediaEvtSem = SEM_create(0U, &semAttr);
        if (NULL == gBlkmediaEvtSem)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED: Sem gBlkmediaEvtSem  Create failed"));
            break;
        }

        semAttr.name = (String)"Blkmedia_Mount_Sema";
        gBlkmediaMountSem = SEM_create(1U, &semAttr);
        if (NULL == gBlkmediaMountSem)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED: Sem gBlkmediaMountSem  Create failed"));
            break;
        }

#ifdef PSP_BLK_EDMA_MEMCPY_IO
        if (gBlkmediaSyncEdmaHandle != NULL)
        {
            /* Blk Driver event semaphore */
            semAttr.name = (String)"Blkmedia_Edma_Io_Sema";
            gBlkmediaIoEdmaSem = SEM_create(0U, &semAttr);
            if (NULL == gBlkmediaIoEdmaSem)
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: Edma IO_sem create failed"));
                break;
            }

            /* Blk Driver EDMA Request channel */
            result = EDMA3_DRV_requestChannel(gBlkmediaSyncEdmaHandle,
                         &gBlkmediaIoChId,
                         &gBlkmediaIoTcc,
                         edmaEventQ,
                         &blkmediaIoEdmaCallback,
                         NULL);
            if (IOM_COMPLETED != result)
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: Edma IO_Channel create failed"));
                break;
            }
        }
#endif /* PSP_BLK_EDMA_MEMCPY_IO */

#ifdef PSP_BLK_EDMA_MEMCPY_ASYNC
        if (gBlkmediaAsyncEdmaHandle != NULL)
        {
            /* Blk Driver event semaphore */
            semAttr.name = (String)"Blkmedia_Edma_Async_Sema";
            gBlkmediaAsyncEdmaSem = SEM_create(0U, &semAttr);
            if (NULL == gBlkmediaAsyncEdmaSem)
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: Edma Async_sem create failed"));
                break;
            }

            /* Blk Driver EDMA Request channel */
            result = EDMA3_DRV_requestChannel(gBlkmediaAsyncEdmaHandle,
                         &gBlkmediaAsyncChId,
                         &gBlkmediaAsyncTcc,
                         edmaEventQ,
                         &blkmediaAsyncEdmaCallback,
                         NULL);
            if (IOM_COMPLETED != result)
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: Edma Async_Channel create failed"));
                break;
            }
        }
#endif /* PSP_BLK_EDMA_MEMCPY_ASYNC */

        /* Initialize attributes */
        attrs.stack = NULL;
        attrs.stackseg = 0;
        attrs.environ = NULL;
        attrs.initstackflag = 1U;
        attrs.exitflag = 1U;
        attrs.name = "Blkmedia_Task";
        attrs.priority = (Int)taskPrio;
        attrs.stacksize = taskSize;
        /* Create Block Driver task */
        gBlkmediaTaskHandle = TSK_create((Fxn)&blkmediaTask, &attrs, NULL);
        if (gBlkmediaTaskHandle == NULL)
        {
            BLKMED_ERR_DEBUG(("\r\n Error creating blkmediaTask()"));
            break;
        }

        for (devNum = 0; devNum < PSP_BLK_DEV_MAXDEV; devNum++)
        {
            /* Create the device IO access completion semaphore */
            semAttr.name = (String)"Blkmedia_Io_Sema";
            gBlkmediaInfo[devNum].ioSema = SEM_create(0U, &semAttr);
            if (NULL == gBlkmediaInfo[devNum].ioSema)
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: ioSema Create failed"));
                break;
            }

            /* Create the device Async access completion semaphore */
            semAttr.name = (String)"Blkmedia_Async_Sema";
            gBlkmediaInfo[devNum].asyncSema = SEM_create(0U, &semAttr);
            if (NULL == gBlkmediaInfo[devNum].asyncSema)
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: asyncSema Create failed"));
                break;
            }

            /* Create IOCTL owner sema to protect from multiple IOCTL's from getting
             * submitted on the same device*/
            semAttr.name = (String)"Blkmedia_Ctl_Sema";
            gBlkmediaInfo[devNum].ctlSema = SEM_create(1U, &semAttr);
            if (NULL == gBlkmediaInfo[devNum].ctlSema)
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: ctlSema Create failed"));
                break;
            }

            gBlkmediaInfo[devNum].pReqCache = pReq;
            pReq->pDevice = &gBlkmediaInfo[devNum];

            for (cnt = 1U; cnt < BLK_DEV_MAXQUEUE; cnt++)
            {
                pReq++;
                QUE_insert(&(gBlkmediaInfo[devNum].pReqCache->Link), &(pReq->Link));
                pReq->pDevice = &gBlkmediaInfo[devNum];
            }

            gBlkmediaInfo[devNum].AvailReq = BLK_DEV_MAXQUEUE;
            cnt = 0;

            /* Start application task or demo task here */
            gBlkmediaInfo[devNum].devNum = devNum;
            gBlkmediaInfo[devNum].BlkOps.Blk_Read = &blkmediaVoidIo;
            gBlkmediaInfo[devNum].BlkOps.Blk_Write = &blkmediaVoidIo;
            gBlkmediaInfo[devNum].BlkOps.Blk_Ioctl = &blkmediaIntIoctl;
            /* We assume that each bus has only one block device and each device has
             * only one partition. */
            gBlkmediaInfo[devNum].DevBus = (PSP_BlkDrvId_t)devNum;
            gBlkmediaInfo[devNum].devCb = &blkmediaCallback;
            gBlkmediaInfo[devNum].DevState = BLK_DEV_CREATED;
        }

#if (PSP_FILE_SYSTEM)
        /* Initialize ertfs  */
        if(IOM_COMPLETED == result)
        {
            if (IOM_COMPLETED != bfs_startup())
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: Rtfs resource init failed"));
                result = IOM_EBADARGS;
            }
            else
            {
                result = IOM_COMPLETED;
            }
        }
#endif
        gBlkmediaOpen = TRUE; /*Block media is initialized*/
    }while (flag);

    return (result);
}


/**
 *  \brief This function deinitialize the Block Media Driver. This fuction
 *         deallocates any resources taken during init and also deletes the
 *         task. The fuction also frees the EDMA channel allocated during init.
 *         This function also deinit the file system(if supported).
 *
 *  \return Int32      : Success/Error
 ******************************************************************************/
Int32 PSP_blkmediaDrvDeInit(Void)
{
    Int32   err = IOM_COMPLETED;
    Uint8   devNum;

#ifdef PSP_BLK_EDMA_MEMCPY_IO
    if (gBlkmediaSyncEdmaHandle != NULL)
    {
        /* Delete semaphore created for EMEMCOPY */
        SEM_delete(gBlkmediaIoEdmaSem);
        /* Delete Channel created for EMEMCOPY */
        err = EDMA3_DRV_freeChannel(gBlkmediaSyncEdmaHandle, gBlkmediaIoChId);
        if (IOM_COMPLETED != err)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED:Error in IO Edma channel Free"));
        }
    }
#endif /* PSP_BLK_EDMA_MEMCPY_IO */

#ifdef PSP_BLK_EDMA_MEMCPY_ASYNC
    if (gBlkmediaAsyncEdmaHandle != NULL)
    {
        /* Delete semaphore created for EMEMCOPY */
        SEM_delete(gBlkmediaAsyncEdmaSem);
        /* Delete Channel created for EMEMCOPY */
        err = EDMA3_DRV_freeChannel(gBlkmediaAsyncEdmaHandle,
                  gBlkmediaAsyncChId);
        if (IOM_COMPLETED != err)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED:Error in Async Edma channel Free"));
        }
    }
#endif /* PSP_BLK_EDMA_MEMCPY_ASYNC */
    memset(gBlkmediaReq, 0, sizeof(BLK_DEV_Req) * BLK_DEV_MAXREQ);
    TSK_delete(gBlkmediaTaskHandle);
    SEM_delete(gBlkmediaEvtSem);
    SEM_delete(gBlkmediaMountSem);

#if (PSP_FILE_SYSTEM)
    /* Tell Rtfs to release all drive structures and buffers associated with
    dev  */
    bfs_shutdown();
#endif

    for (devNum = 0; devNum < PSP_BLK_DEV_MAXDEV; devNum++)
    {
        SEM_delete(gBlkmediaInfo[devNum].ioSema);
        SEM_delete(gBlkmediaInfo[devNum].ctlSema);
        SEM_delete(gBlkmediaInfo[devNum].asyncSema);
        gBlkmediaInfo[devNum].devNum = 0;
        gBlkmediaInfo[devNum].BlkOps.Blk_Read = NULL;
        gBlkmediaInfo[devNum].BlkOps.Blk_Write = NULL;
        gBlkmediaInfo[devNum].BlkOps.Blk_Ioctl = NULL;
        gBlkmediaInfo[devNum].DevBus = (PSP_BlkDrvId_t) 0;
        gBlkmediaInfo[devNum].devCb  = NULL;
        gBlkmediaInfo[devNum].DevState = BLK_DEV_DELETED;
    }
    gBlkmediaOpen = FALSE; /*Block media is un-initialized*/
    return err;
}



/*
 *  \brief This function registers the storage driver with Block Media Driver.
 *         Storage driver will call this function during initialization of the
 *         device with a fuction pointer which can be called as soon as device
 *         is detected to get the read write and ioctl pointers of the device.
 *         The same parameter is set to NULL during deinit of a storage device.
 *
 *  \param driverId [IN]      : Id of the Storage Driver
 *  \param pRegInfo [IN]      : Structure containing the device
 *                              register/un-register function. The function
 *                              passed here will be used later to get the read
 *                              write and ioctl pointers of the storage device.
 *
 *  \return Int32             : Success/Error
 ******************************************************************************/
Int32 PSP_blkmediaDrvRegister(PSP_BlkDrvId_t driverId,
                              PSP_BlkDrvReg_t const *pRegInfo)
{
    Uint32      cookie;
    Int32       retVal = IOM_COMPLETED;

    cookie = HWI_disable();
/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == pRegInfo) || (driverId >= PSP_BLK_DRV_MAX))
    {
        retVal = IOM_EBADARGS;
    }
#endif
/* End parameter checking                                                     */

    if(IOM_COMPLETED == retVal)
    {
        gBlkmediaReg[driverId] = *pRegInfo;

        if ((NULL == pRegInfo->DeviceReg) &&
            (BLK_DEV_MAXQUEUE == gBlkmediaInfo[driverId].AvailReq))
        {
            if (BLK_DEV_REG == gBlkmediaInfo[driverId].DevState)
            {
                /* unregister all the drives owned by this storage driver and
                unmount any mounted drives from the file system.  These should
                have already been done before just doing it to clean up
                correctly.*/
                gBlkmediaInfo[driverId].pDevOps = NULL;
                gBlkmediaInfo[driverId].isMediaWriteProtect = FALSE;
                gBlkmediaInfo[driverId].DevHandle = NULL;
                gBlkmediaInfo[driverId].maxUserSect = 0;
                gBlkmediaInfo[driverId].sectorSize = 0;
                gBlkmediaInfo[driverId].Cyl = 0;
                gBlkmediaInfo[driverId].Head = 0;
                gBlkmediaInfo[driverId].sectPerTrack = 0;
                gBlkmediaInfo[driverId].DevState = BLK_DEV_INIT;

#if (PSP_FILE_SYSTEM)
                /* Mark the drive as been ejected as the driver has
                unregistered. Tell the FS that device is ejected. Tell Rtfs that
                this device is removed */
                pc_rtfs_media_alert((Void *)&gBlkmediaInfo[driverId],
                    RTFS_ALERT_EJECT,
                    0);
#endif
            }
        }
        HWI_restore(cookie);
    }
    return retVal;
}



/**
 *  \brief Handle the BLK IOCTL commands when device is active. This IOCTL can
 *         be used to set a device to be used for RAW access, get which device
 *         is currently set to be used for RAW access, set init completion
 *         callback for the storage device etc. See supported IOCTL commands in
 *         PSP_BlkDrvIoctl_t.
 *
 *  \param  pDevName [IN]     : Address of variable which contains Device Name
 *  \param  pIoctl   [IN/OUT] : IOCTL info structure
 *
 *  \return Int32             : Success (IOM_COMPLETED) / Failure (IOM_EBADARGS)
 ******************************************************************************/
Int32 PSP_blkmediaDrvIoctl(Ptr *pDevName, PSP_BlkDrvIoctlInfo_t *pIoctl)
{
    Int32           media;
    PSP_BlkDrvId_t  *device;
    Int32           err;
    Bool            flag = TRUE;

    do
    {
        /* Initialize the local variables as required                         */
        err = IOM_EBADARGS;
        flag = FALSE;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((NULL == pIoctl) || (NULL == pDevName))
        {
            break;
        }
#endif
/* End parameter checking                                                     */

        switch (pIoctl->Cmd)
        {
            case PSP_BLK_DRV_SETRAWDEV:
                device = (PSP_BlkDrvId_t*)pIoctl->pData;
                /* Enforce the rule that we cannot have more than one media driver
            registered for RAW access, But will not fail if same device is send again*/
            if ((NULL == device) || ((gBlkmediaRawDev.BLKAPP_DEV != PSP_BLK_DRV_MAX) && (gBlkmediaRawDev.BLKAPP_DEV != (*device))))
                {
                    err = IOM_EBADARGS;
                }
                else
                {
                    switch (*device)
                    {
                        case PSP_BLK_DRV_MMC0:
                            gBlkmediaRawDev.BLKAPP_DEV = PSP_BLK_DRV_MMC0;
                            err = IOM_COMPLETED;
                            break;
#if defined(CHIP_C6748) || defined(CHIP_OMAPL138)
                        case PSP_BLK_DRV_MMC1:
                            gBlkmediaRawDev.BLKAPP_DEV = PSP_BLK_DRV_MMC1;
                            err = IOM_COMPLETED;
                            break;
#endif
                        case PSP_BLK_DRV_NAND:
                            gBlkmediaRawDev.BLKAPP_DEV = PSP_BLK_DRV_NAND;
                            err = IOM_COMPLETED;
                            break;
                        case PSP_BLK_DRV_USB0:
                            gBlkmediaRawDev.BLKAPP_DEV = PSP_BLK_DRV_USB0;
                            err = IOM_COMPLETED;
                            break;
                        case PSP_BLK_DRV_USB1:
                            gBlkmediaRawDev.BLKAPP_DEV = PSP_BLK_DRV_USB1;
                            err = IOM_COMPLETED;
                            break;
#if defined(CHIP_C6748) || defined(CHIP_OMAPL138)
                        case PSP_BLK_DRV_ATA0:
                            gBlkmediaRawDev.BLKAPP_DEV = PSP_BLK_DRV_ATA0;
                            err = IOM_COMPLETED;
                            break;
                        case PSP_BLK_DRV_ATA1:
                            gBlkmediaRawDev.BLKAPP_DEV = PSP_BLK_DRV_ATA1;
                            err = IOM_COMPLETED;
                            break;
#endif
                        default:
                            err = IOM_EBADARGS;
                    }
                }
                break;

            case PSP_BLK_DRV_GETRAWDEV:
                *((PSP_BlkDrvId_t*)pIoctl->pData) = gBlkmediaRawDev.BLKAPP_DEV;
                err = IOM_COMPLETED;
                break;

            case PSP_BLK_DRV_SET_INIT_COMP_CALLBACK:
                media = *((PSP_BlkDrvId_t*)pDevName);
                gBlkmediaInfo[media].InitCallback = \
                    (BLK_DRV_InitCallback_t)(Uint32)pIoctl->pData;
                err = IOM_COMPLETED;
                break;

            default:
                err = IOM_EBADARGS;
        }
    }while (flag);

    return err;
}


/**
 *  \brief Handle the BLK IOCTL commands when device is active. This IOCTL can
 *         be used to set device operation mode, get device sector size, get
 *         size of storage device etc. See supported IOCTL commands in
 *         PSP_BlkDevIoctl_t.
 *
 *  \param driverId   [IN]     : Id of the Storage Driver
 *  \param pIoctl     [IN/OUT] : IOCTL info structure
 *
 *  \return Int32              : Success(IOM_COMPLETED) / Failure (IOM_EBADARGS)
 ******************************************************************************/
Int32 PSP_blkmediaDevIoctl(PSP_BlkDrvId_t driverId,
                           PSP_BlkDevIoctlInfo_t *pIoctl)
{
    Int32           err;
    BLK_DEV_Io_t    ioRead = NULL;
    BLK_DEV_Io_t    ioWrite = NULL;
    BLK_DEV_Info    *pBlkDev = &gBlkmediaInfo[driverId];
    Bool            flag = TRUE;

    do
    {
        /* Initialize the local variables as required                         */
        err = IOM_EBADARGS;
        flag = FALSE;
/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((NULL == pIoctl) || (driverId >= PSP_BLK_DRV_MAX))
        {
            break;
        }
#endif
/* End parameter checking                                                     */

        TSK_disable();
        /*Check if this device is registered or not*/
        if (((pBlkDev != NULL) && (pBlkDev->DevState != BLK_DEV_REG)) ||
            (pBlkDev->AvailReq != BLK_DEV_MAXQUEUE))
        {
            TSK_enable();
            break;
        }

        ioRead = pBlkDev->BlkOps.Blk_Read;
        ioWrite = pBlkDev->BlkOps.Blk_Write;
        if ((NULL == ioRead) || (NULL == ioWrite))
        {
            break;
        }
        pBlkDev->BlkOps.Blk_Read  = &blkmediaVoidIo;
        pBlkDev->BlkOps.Blk_Write = &blkmediaVoidIo;
        TSK_enable();

        /* Take the IOCTL ctl sema for exclusive IOCTL processing on this device */
        if (SEM_pend(pBlkDev->ctlSema, SYS_FOREVER) != TRUE)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED: ctlSema :Sem take fail"));
            break;
        }
        /*call the internal ioctl function*/
        err = pBlkDev->BlkOps.Blk_Ioctl(pBlkDev, NULL, pIoctl, NULL);
        SEM_post(pBlkDev->ctlSema);
        TSK_disable();
        pBlkDev->BlkOps.Blk_Read  = ioRead;
        pBlkDev->BlkOps.Blk_Write = ioWrite;
        TSK_enable();
    }while (flag);

    return err;
}


/* ========================================================================== */
/*                            LOCAL  FUNCTIONS                                */
/* ========================================================================== */


/**
 *  \brief Callback for the BLK Device layer
 *
 *  \param handle [IN] : BLK Device Handle
 *  \param pRes   [IN] : Request response info.
 *
 *  \return Bool       : TRUE
 ******************************************************************************/
static Bool blkmediaCallback(Ptr handle, PSP_BlkDevRes_t const *pRes)
{
    BLK_DEV_Req  *pReq;

    assert((NULL != handle) && (NULL != pRes));
    pReq = (BLK_DEV_Req *)handle;
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
    /* Check whether this is a last transfer  */
    if (pReq->pDevice->alignInfo.split > 0)
    {
        /* One transfer is completed, Release semaphore to start next transfer*/
        pReq->pDevice->alignInfo.split--;
        /* Check whether it is a io call or Async call. If it is io call then
        release semaphore*/
        if (FALSE == pReq->pDevice->isRawRegistered)
        {
            SEM_post(pReq->pDevice->ioSema);
        }
        else
        {
            SEM_post(pReq->pDevice->asyncSema);
        }
    }
    else
    {
#endif  /* #endif PSP_BUFF_ALIGNMENT    */
        if ((TRUE == pReq->pDevice->isRawRegistered) &&
            (FALSE == (((BLK_DEV_Req *)handle)->isIoctl)))
        {
            blkmediaRelReq(pReq->pDevice, ((BLK_DEV_Req *)handle));
            SEM_post(pReq->pDevice->asyncSema);
            pReq->pDevice->appCb(((BLK_DEV_Req *)handle)->AppHandle, pRes);
        }
        else
        {
            if (TRUE == pRes->IsEvent)
            {
                switch (pRes->ReqInfo.Event.EventId)
                {
                    case PSP_BLK_EVT_MEDWP:
                        pReq->pDevice->isMediaWriteProtect = TRUE;
                        if (TRUE == pReq->pDevice->isRawRegistered)
                        {
                            pReq->pDevice->BlkOps.Blk_Write = &blkmediaVoidIo;
                        }
                        break;

                    case PSP_BLK_EVT_MEDRW:
                        pReq->pDevice->isMediaWriteProtect = FALSE;
                        if (TRUE == pReq->pDevice->isRawRegistered)
                        {
                            pReq->pDevice->BlkOps.Blk_Write = \
                                &blkmediaWriteAsync;
                        }
                        break;

                    default:
                        break;
                }

                /* Propogate the events to RAW application if we have a remote
                client connected */
                if (TRUE == pReq->pDevice->isRawRegistered)
                {
                    pReq->pDevice->appCb(NULL, pRes);
                }
            }
            else
            {
                pReq->Status = *pRes;
#if (PSP_FILE_SYSTEM)
                /*Added to propogate WP event to FS*/
                switch (pRes->ReqInfo.Event.EventId)
                {
                    case PSP_BLK_EVT_MEDWP:
                        {
                            pReq->pDevice->isMediaWriteProtect = TRUE;
                            pc_rtfs_media_alert(pReq->pDevice,
                                RTFS_ALERT_WPSET,
                                0);
                        }
                        break;
                    case PSP_BLK_EVT_MEDRW:
                        {
                            pReq->pDevice->isMediaWriteProtect = FALSE;
                            pc_rtfs_media_alert(pReq->pDevice,
                                RTFS_ALERT_WPCLEAR,
                                0);
                        }
                        break;
                    default:
                        break;
                }
#endif
                SEM_post(pReq->pDevice->ioSema);
            }
        }
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
    }
#endif  /* #endif PSP_BUFF_ALIGNMENT    */

    return TRUE;
}




/**
 *  \brief Register for Block Devices with the storage drivers. This is done in
 *         a task and gets activated by block device insertion/removal activity
 *         or during Init.
 *
 *
 *  \return Void
 ******************************************************************************/
static Void blkmediaTask(Void)
{
    Uint32                 devNum = 0;
    BLK_DEV_Info           *pDevice;
    PSP_BlkDevIoctlInfo_t  ioctlInfo;
    Int32                  err = IOM_EBADARGS;
    Uint32                 cookie;
    Bool                   isData = FALSE;
    PSP_BlkOpMode          opMode;
    Bool                   taskFlag = FALSE;
    Bool                   deviceFlag = FALSE;

    taskFlag = TRUE;
    while (taskFlag)
    {
        taskFlag = TRUE;
        /*This sema is released whenever there is a device inserted or
        available*/
        if (TRUE != SEM_pend(gBlkmediaEvtSem, SYS_FOREVER))
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED: gBlkmediaEvtSem Take failed"));
        }

        devNum = 0;
        do
        {
            deviceFlag = TRUE;
            /*Is any device inserted or available*/
            while ((gBlkmediaInfo[devNum].DevState != BLK_DEV_AVAIL) &&
                 (++devNum < PSP_BLK_DRV_MAX))
            {
              ;
            }

            if (devNum >= PSP_BLK_DRV_MAX)
            {
                break;
            }
            /*Take semaphore as mounting is in progress and it should not enter
            for other Storage Driver till the current mounting is over*/
            if (TRUE != SEM_pend(gBlkmediaMountSem, SYS_FOREVER))
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: MountSem Take failed"));
            }
            pDevice = &gBlkmediaInfo[devNum];
            /* Register for the device with the storage driver */
            err = blkmediaRegisterMedia(pDevice->DevBus);
            if (err == IOM_COMPLETED)
            {
                /*Use the ioctl function pointer to get/set different
                functionalities of driver*/

                /* Set the best Operation mode for this device. The opmode will
                be changed for only those device which supports it otherwise it
                should be left untouched*/
                ioctlInfo.Cmd = PSP_BLK_SETOPMODE;
                opMode = PSP_OPMODE_AUTO;
                ioctlInfo.pData = (Ptr)(&opMode);
                pDevice->BlkOps.Blk_Ioctl((Ptr)pDevice, NULL, &ioctlInfo, NULL);

                /* Get the Operation mode for this device */
                ioctlInfo.Cmd = PSP_BLK_GETOPMODE;
                ioctlInfo.pData = (Ptr)(&opMode);
                pDevice->BlkOps.Blk_Ioctl((Ptr)pDevice, NULL, &ioctlInfo, NULL);

                /* Set operating mode of device in block media, to be used
                during read/write*/
                pDevice->opMode = (PSP_BlkOpMode)(opMode);

                /* Get Disk Capacity */
                ioctlInfo.Cmd = PSP_BLK_GETSECTMAX;
                ioctlInfo.pData = (Ptr)&pDevice->maxUserSect;
                if ((pDevice->BlkOps.Blk_Ioctl((Ptr)pDevice,
                         NULL, &ioctlInfo, NULL)) != IOM_COMPLETED)
                {
                    pDevice->DevState = BLK_DEV_INIT;
                    err = IOM_EBADARGS;
                    SEM_post(gBlkmediaMountSem);
                    break;
                }
                if (err == IOM_COMPLETED)
                {
                    /* Get Sector Size */
                    ioctlInfo.Cmd = PSP_BLK_GETBLKSIZE;
                    ioctlInfo.pData = (Ptr)&pDevice->sectorSize;
                    if ((pDevice->BlkOps.Blk_Ioctl((Ptr)pDevice,
                             NULL, &ioctlInfo, NULL)) != IOM_COMPLETED)
                    {
                        pDevice->DevState = BLK_DEV_INIT;
                        err = IOM_EBADARGS;
                        SEM_post(gBlkmediaMountSem);
                        break;
                    }
                }
                if (err == IOM_COMPLETED)
                {
                    ioctlInfo.Cmd = PSP_BLK_GETWPSTAT;
                    ioctlInfo.pData = (Ptr)&isData;
                    if ((pDevice->BlkOps.Blk_Ioctl((Ptr)pDevice,
                             NULL, &ioctlInfo, NULL)) != IOM_COMPLETED)
                    {
                        pDevice->DevState = BLK_DEV_INIT;
                        err = IOM_EBADARGS;
                        SEM_post(gBlkmediaMountSem);
                        break;
                    }
                }
                if (TRUE == isData)
                {
                    pDevice->isMediaWriteProtect = TRUE;
                }

                if (TRUE == pDevice->isRawRegistered)
                {
                    pDevice->BlkOps.Blk_Read  = &blkmediaReadAsync;
                    pDevice->BlkOps.Blk_Write = &blkmediaWriteAsync;
                }
                else
                {
                    /*Call this API to notify the OS that the device is
                    available now for use*/
                }

                /* Update the device state with protection */
                cookie = HWI_disable();
                if ((err == IOM_COMPLETED) &&
                    (pDevice->DevState == BLK_DEV_AVAIL))
                {
                    pDevice->DevState = BLK_DEV_REG;
                    if (NULL == pDevice->InitCallback)
                    {
                        BLKMED_ERR_DEBUG(("\r\nBLK_MED: InitCB not called"));
                    }
                    else
                    {
                        /*Call the Storage driver function to tell that mount is
                        over*/
                        pDevice->InitCallback((Ptr)&err);
                    }
                }
                else
                {
                    pDevice->DevState = BLK_DEV_INIT;
                }
                HWI_restore(cookie);
            }
            else
            {
                pDevice->DevState = BLK_DEV_INIT;
                if (NULL == pDevice->InitCallback)
                {
                    BLKMED_ERR_DEBUG(("\r\nBLK_MED: BLK_DEV_REG Failed"));
                }
                else
                {
                    pDevice->InitCallback((Ptr)&err);
                }
            }
#if (PSP_FILE_SYSTEM)
            if (pDevice->DevState == BLK_DEV_REG)
            {
                /* If its not a RAW device access then only mount the device  */
                if (FALSE == pDevice->isRawRegistered)
                {
                    /* The device is initialized, now call Local function that
                    maps device parameters to an rtfs_media_insert_args
                    structure and calls pc_rtfs_media_insert() */
                    err = blkmediaRtfsMountDevice(pDevice, devNum);
                    if (err != IOM_COMPLETED)
                    {
                        BLKMED_ERR_DEBUG(("\r\nBLKMED: Rtfs_Mount failed"));
                    }
                }
            }
#endif
            SEM_post(gBlkmediaMountSem);
        }while (deviceFlag);
    }
}



/**
 *  \brief Register for a Block device with Storage Driver.
 *
 *  \param driverId [IN] : Device Bus ID
 *
 *  \return Int32        : Error/Success
 ******************************************************************************/
static Int32 blkmediaRegisterMedia(PSP_BlkDrvId_t driverId)
{
    Int32               err = IOM_EBADARGS;
    Ptr                 handle = NULL;
    PSP_BlkDevOps_t     *pIntOps = NULL;
    PSP_BlkDrvReg_t     *pDriver = &gBlkmediaReg[driverId];
    BLK_DEV_Info        *pDevice = &gBlkmediaInfo[driverId];

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == pDriver) || (NULL == pDevice))
    {
        err = IOM_EBADARGS;
    }
#endif
/* End parameter checking                                                     */

    /*Call the driver function to get write , read and ioctl function pointer*/
    pDriver->DeviceReg(NULL, pDevice->devCb, &pIntOps, &handle);

    if (handle != NULL)
    {
        if (FALSE == pDevice->isRawRegistered)
        {
            pDevice->BlkOps.Blk_Read  = &blkmediaReadIo;
            pDevice->BlkOps.Blk_Write = &blkmediaWriteIo;
        }
        pDevice->BlkOps.Blk_Ioctl = &blkmediaIntIoctl;
        /*Copy the read, write and ioctl function pointers of driver to the
        block media driver table*/
        pDevice->pDevOps   = pIntOps;
        pDevice->DevHandle = handle;
        err = IOM_COMPLETED;
    }

    return err;
}


/**
 *  \brief Do a Block read operation and return status under normal conditions.
 *
 *  \param handle  [IN] : Application Handle.
 *  \param mediaHandle   [IN] : Request Handle. Can be NULL
 *  \param buffer  [IN] : Buffer Pointer
 *  \param sector  [IN] : Sector Number (start sector number)
 *  \param numSect [IN] : Count of Sectors
 *
 *  \return Int32       : Success (0)/ Failure (!0)
 ******************************************************************************/
static Int32 blkmediaReadIo(Ptr handle,
                            Ptr mediaHandle,
                            Ptr buffer,
                            Uint32 sector,
                            Uint32 numSect)
{
    Int32           err = IOM_COMPLETED;
    BLK_DEV_Info    *pDevice;
    BLK_DEV_Req     *pReq = NULL;
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
    Uint32          sectsize, sectorperchunk = 0;
    Int32           count;
    Uint32          chunk = 0, extrasect = 0;
    Bool            alignedflag = FALSE;
#endif

    assert((NULL != handle) && (NULL != buffer));
    pDevice = (BLK_DEV_Info *)handle;
    mediaHandle = mediaHandle; /* to remove compilation warning */

#ifdef BLKMEDIA_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
                  "BLKMEDIA",
                  PSP_DVT_DESC(PSP_DVT_eFUNC_START,
                               PSP_DVT_dCHANNEL_ID_INST,
                               PSP_DVT_dPACKET_ID,
                               PSP_DVT_dSIZE_BYTES),
                  CHANNELID_INSTANCEID(
                      0,
                      0),
                  buffer,
                  numSect);
#endif /* BLKMEDIA_INSTRUMENTATION_ENABLED */

    /* Added for cache alignment    */
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
    sectsize = (pDevice->sectorSize);
    /* Check if Buffer need to be cache aligned */
    if (((Uint32)buffer % BLK_CACHE_LINE_SIZE_IN_BYTES) != 0)
    {
        /*if opmode is polled then there is no need to align the buffers*/
        if (PSP_OPMODE_DMAINTERRUPT == pDevice->opMode)
        {
            alignedflag = TRUE;
        }
        if (TRUE == alignedflag)
        {
            /* Find total no. of transfer and no. of bytes to be transmitted
            last */
            sectorperchunk = PSP_BUFFER_IO_SIZE / sectsize;
            extrasect = numSect % sectorperchunk;
            chunk = numSect / sectorperchunk;

            /* get a cache aligned buffer Align buffer pointer to
            BLK_CACHE_LINE_SIZE_IN_BYTES bit align address */
#ifdef PSP_MULTI_TASK_BUFFERS
            pDevice->alignInfo.pAlignedData = \
                &gBlkmediaIoBuffer[pDevice->DevBus * PSP_BUFFER_IO_SIZE];
#else
            pDevice->alignInfo.pAlignedData = gBlkmediaIoBuffer;
#endif
        }
    }

    pDevice->alignInfo.split = chunk;
    if ((0 == extrasect) && (pDevice->alignInfo.split > 0))
    {
        (pDevice->alignInfo.split)--;
    }
#endif  /* #endif PSP_BUFF_ALIGNMENT    */

    pReq = blkmediaAllocReq(pDevice);

    if (pReq != NULL)
    {
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
        if (TRUE == alignedflag)
        {
            for (count = 0; count < chunk; count++)
            {
                if (NULL == pDevice->pDevOps)
                {
                    err = IOM_EBADARGS;
                    break;
                }

                /* Call driver Read routine of storage driver to read in to
                buffer */
                err = pDevice->pDevOps->Blk_Read(pDevice->DevHandle,
                          (Ptr)pReq,
                          pDevice->alignInfo.pAlignedData,
                          sector + (count * sectorperchunk),
                          sectorperchunk);
                if ((err != IOM_COMPLETED) || (pDevice->pDevOps == NULL))
                {
                    break;
                }
                /* Wait for transfer to be completed */
                if (SEM_pend(pDevice->ioSema, SYS_FOREVER) != TRUE)
                {
                    BLKMED_ERR_DEBUG(("\r\nBLK_MED: Read_IO ioSema take fail"));
                    break;
                }
                /* Copy Data to application buffer  */
                if (gBlkmediaSyncEdmaHandle != NULL)
                {
                    err = blkmediaEdmaMemcpy(gBlkmediaSyncEdmaHandle,
                              (Uint8*)buffer +
                                  (count * sectorperchunk * sectsize),
                              pDevice->alignInfo.pAlignedData,
                              sectorperchunk * sectsize,
                              BLK_DEV_Read,
                              gBlkmediaIoChId,
                              gBlkmediaIoTcc);
                }
                else
                {
                    memcpy(((Uint8*)buffer +
                                (count * sectorperchunk * sectsize)),
                        pDevice->alignInfo.pAlignedData,
                        (sectorperchunk * sectsize));
                }
            }
            if ((extrasect != 0) && (err == IOM_COMPLETED))
            {
                if (NULL == pDevice->pDevOps)
                {
                    err = IOM_EBADARGS;
                }
                else
                {
                    /* Transmit extra sectors */
                    err = pDevice->pDevOps->Blk_Read(pDevice->DevHandle,
                              (Ptr)pReq,
                              pDevice->alignInfo.pAlignedData,
                              sector + (count * sectorperchunk),
                              extrasect);
                    if (err == IOM_COMPLETED)
                    {
                        /* Wait for transfer to be completed */
                        if (SEM_pend(pDevice->ioSema, SYS_FOREVER) != TRUE)
                        {
                            BLKMED_ERR_DEBUG(("\r\nBLK_MED: RIOSem take fail"));
                        }
                        else if (gBlkmediaSyncEdmaHandle != NULL)
                        {
                            /* Copy Data to application buffer  */
                            err = blkmediaEdmaMemcpy(gBlkmediaSyncEdmaHandle,
                                      (Uint8 *) buffer +
                                          (count * sectorperchunk * sectsize),
                                      pDevice->alignInfo.pAlignedData,
                                      extrasect * sectsize,
                                      BLK_DEV_Read,
                                      gBlkmediaIoChId,
                                      gBlkmediaIoTcc);
                        }
                        else
                        {
                            memcpy(((Uint8*) buffer +
                                       (count * sectorperchunk * sectsize)),
                                pDevice->alignInfo.pAlignedData,
                                (extrasect * sectsize));
                        }
                    }
                }
            }
        }
        else
        {
#endif /* #endif PSP_BUFF_ALIGNMENT */

            err = pDevice->pDevOps->Blk_Read(pDevice->DevHandle,
                      (Ptr)pReq,
                      buffer,
                      sector,
                      numSect);
            if (err == IOM_COMPLETED)
            {
                if (SEM_pend(pDevice->ioSema, SYS_FOREVER) != TRUE)
                {
                    BLKMED_ERR_DEBUG(("\r\nBLK_MED: Read :ioSema take fail"));
                }
            }
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
        }
#endif  /* #endif PSP_BUFF_ALIGNMENT    */
        if (err == IOM_COMPLETED)
        {
            err = pReq->Status.ReqInfo.Status.Error;
        }
        blkmediaRelReq(pDevice, pReq);
    }
#ifdef BLKMEDIA_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
                  "BLKMEDIA",
                  PSP_DVT_DESC(PSP_DVT_eFUNC_END,
                               PSP_DVT_dCHANNEL_ID_INST,
                               PSP_DVT_dPACKET_ID,
                               PSP_DVT_dSIZE_BYTES),
                  CHANNELID_INSTANCEID(
                      0,
                      0),
                  buffer,
                  0);
#endif /* BLKMEDIA_INSTRUMENTATION_ENABLED */
    return err;
}



/**
 *  \brief Do a Block read operation and return status under Asynch conditions.
 *
 *  \param handle       [IN] : Handle returned during registration with the
 *                             blkmedia.
 *  \param mediaHandle  [IN] : Address of variable of type PSP_BlkDevRes_t to
 *                             get the error value.
 *  \param buffer       [IN] : Buffer Pointer
 *  \param sector       [IN] : Sector Number (start sector number) from where
 *                             the data needs to be read
 *  \param numSect      [IN] : Count of Sectors to be read
 *
 *  \return Int32       : Success (0)/ Failure (!0)
 ******************************************************************************/
static Int32 blkmediaReadAsync(Ptr handle,
                               Ptr mediaHandle,
                               Ptr buffer,
                               Uint32 sector,
                               Uint32 numSect)
{
    Int32           err  = IOM_COMPLETED;
    BLK_DEV_Req     *pReq = NULL;
    BLK_DEV_Info    *pDevice;
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
    Uint32          sectsize, sectorperchunk = 0;
    Int32           count;
    Uint32          chunk = 0, extrasect = 0;
    Bool            alignedflag = FALSE;
#endif

    assert(((NULL != handle) && (NULL != mediaHandle))&& (NULL != buffer));
    pDevice = (BLK_DEV_Info *)handle;

#ifdef BLKMEDIA_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
                  "BLKMEDIA",
                  PSP_DVT_DESC(PSP_DVT_eFUNC_START,
                               PSP_DVT_dCHANNEL_ID_INST,
                               PSP_DVT_dPACKET_ID,
                               PSP_DVT_dSIZE_BYTES),
                  CHANNELID_INSTANCEID(
                      0,
                      0),
                  buffer,
                  numSect);
#endif /* BLKMEDIA_INSTRUMENTATION_ENABLED */

    /* Added for cache alignment  */
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
    sectsize = (pDevice->sectorSize);

    /* Check if Buffer need to be cache aligned */
    if (((Uint32)buffer % BLK_CACHE_LINE_SIZE_IN_BYTES) != 0)
    {
        /*if opmode is polled then there is no need to align the buffers*/
        if (PSP_OPMODE_DMAINTERRUPT == pDevice->opMode)
        {
            alignedflag = TRUE;
        }
        if (TRUE == alignedflag)
        {
            /* Find total no. of transfer and no. of bytes to be transmitted
            last */
            sectorperchunk = PSP_BUFFER_ASYNC_SIZE / sectsize;
            extrasect = numSect % sectorperchunk;
            chunk = numSect / sectorperchunk;

            /* get a cache aligned buffer pointer to
            BLK_CACHE_LINE_SIZE_IN_BYTES bit align address */
            pDevice->alignInfo.pAlignedData = gBlkmediaAsyncBuffer;
        }
    }
    pDevice->alignInfo.split = chunk;
    if ((0 == extrasect) && (pDevice->alignInfo.split > 0))
    {
        (pDevice->alignInfo.split)--;
    }
#endif  /* #endif PSP_BUFF_ALIGNMENT    */

    pReq = blkmediaAllocReq(pDevice);
    if (pReq != NULL)
    {
        pReq->AppHandle = mediaHandle;
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
        if (TRUE == alignedflag)
        {
            for (count = 0; count < chunk; count++)
            {
                if (pDevice->pDevOps == NULL)
                {
                    err = IOM_EBADARGS;
                    break;
                }

                /* Call driver Read routine to read in to buffer */
                err = pDevice->pDevOps->Blk_Read(pDevice->DevHandle,
                          (Ptr)pReq,
                          pDevice->alignInfo.pAlignedData,
                          sector + (count * sectorperchunk),
                          sectorperchunk);
                /* Wait for transfer to be completed */
                if (SEM_pend(pDevice->asyncSema, SYS_FOREVER) != TRUE)
                {
                    BLKMED_ERR_DEBUG(("\r\nBLK_MED: RAsyncSema take fail"));
                    break;
                }
                if ((err != IOM_COMPLETED) || (pDevice->pDevOps == NULL))
                {
                    blkmediaRelReq(pDevice, pReq);
                    break;
                }

                /* Copy Data to application buffer  */
                if (gBlkmediaAsyncEdmaHandle != NULL)
                {
                    err = blkmediaEdmaMemcpy(gBlkmediaAsyncEdmaHandle,
                              (Uint8 *)buffer +
                                  (count * sectorperchunk * sectsize),
                              pDevice->alignInfo.pAlignedData,
                              sectorperchunk * sectsize,
                              BLK_DEV_Read,
                              gBlkmediaAsyncChId,
                              gBlkmediaAsyncTcc);
                }
                else
                {
                    memcpy((Char*)buffer +
                               (count * sectorperchunk * sectsize),
                        pDevice->alignInfo.pAlignedData,
                        sectorperchunk * sectsize);
                }
            }
            if ((extrasect != 0) && (err == IOM_COMPLETED))
            {
                /* Transmit extra sectors */
                err = pDevice->pDevOps->Blk_Read(pDevice->DevHandle,
                          (Ptr)pReq,
                          pDevice->alignInfo.pAlignedData,
                          sector + (count * sectorperchunk),
                          extrasect);
                /* Wait for transfer to be completed */
                if (SEM_pend(pDevice->asyncSema, SYS_FOREVER) != TRUE)
                {
                    BLKMED_ERR_DEBUG(("\r\nBLK_MED: RAsyncSema take fail"));
                }
                if ((err == IOM_COMPLETED) &&
                    (gBlkmediaAsyncEdmaHandle != NULL))
                {
                    /* Copy Data to application buffer  */
                    err = blkmediaEdmaMemcpy(gBlkmediaAsyncEdmaHandle,
                              (Uint8 *) buffer +
                                  (count * sectorperchunk * sectsize),
                              pDevice->alignInfo.pAlignedData,
                              extrasect * sectsize,
                              BLK_DEV_Read,
                              gBlkmediaAsyncChId,
                              gBlkmediaAsyncTcc);
                }
                else
                {
                    memcpy((Char*) buffer + (count * sectorperchunk * sectsize),
                        pDevice->alignInfo.pAlignedData,
                        extrasect * sectsize);
                }
            }
        }
        else
        {
#endif /* #endif PSP_BUFF_ALIGNMENT */
            err = pDevice->pDevOps->Blk_Read(pDevice->DevHandle,
                      (Ptr)pReq,
                      buffer,
                      sector,
                      numSect);
            /* Wait for transfer to be completed */
            if (SEM_pend(pDevice->asyncSema, SYS_FOREVER) != TRUE)
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: RAsyncSema take fail"));
            }
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
        }
#endif  /* #endif PSP_BUFF_ALIGNMENT    */
    }
    else
    {
        err = IOM_EBADARGS;
    }
#ifdef BLKMEDIA_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
                  "BLKMEDIA",
                  PSP_DVT_DESC(PSP_DVT_eFUNC_END,
                               PSP_DVT_dCHANNEL_ID_INST,
                               PSP_DVT_dPACKET_ID,
                               PSP_DVT_dSIZE_BYTES),
                  CHANNELID_INSTANCEID(
                      0,
                      0),
                  buffer,
                  0);
#endif /* BLKMEDIA_INSTRUMENTATION_ENABLED */
    return err;
}




/**
 *  \brief Do a Block Write operation and return status under normal conditions
 *
 *  \param handle  [IN] : Device Handle.
 *  \param mediaHandle   [IN] : Request Handle. Can be NULL
 *  \param buffer  [IN] : Buffer Pointer
 *  \param sector  [IN] : Sector Number (start sector number)
 *  \param numSect [IN] : Count of Sectors
 *
 *  \return Int32       : Success (0)/ Failure (!0)
 ******************************************************************************/
static Int32 blkmediaWriteIo(Ptr handle,
                             Ptr mediaHandle,
                             Ptr buffer,
                             Uint32 sector,
                             Uint32 numSect)
{
    Int32           err = IOM_COMPLETED;
    BLK_DEV_Info    *pDevice;
    BLK_DEV_Req     *pReq = NULL;
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
    Uint32 sectsize, sectorperchunk = 0;
    Int32 count;
    Uint32 chunk = 0, extrasect = 0;
    Bool alignedflag = FALSE;
#endif

    assert((NULL != handle) && (NULL != buffer));
    pDevice = (BLK_DEV_Info *)handle;
    mediaHandle = mediaHandle; /* to remove compilation warning */

#ifdef BLKMEDIA_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
        "BLKMEDIA",
        PSP_DVT_DESC(PSP_DVT_eFUNC_START,
                     PSP_DVT_dCHANNEL_ID_INST,
                     PSP_DVT_dPACKET_ID,
                     PSP_DVT_dSIZE_BYTES),
        CHANNELID_INSTANCEID(
            1,
            0),
        buffer,
        numSect);
#endif /* BLKMEDIA_INSTRUMENTATION_ENABLED */

    /* Added for cache alignment  */
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
    sectsize = (pDevice->sectorSize);

    /* Check if Buffer need to be aligned */
    if (((Uint32)buffer % BLK_CACHE_LINE_SIZE_IN_BYTES) != 0)
    {
        if (PSP_OPMODE_DMAINTERRUPT == pDevice->opMode)
        {
            alignedflag = TRUE;
        }

        if (TRUE == alignedflag)
        {
            /* Find total no. of transfer and no. of bytes to be transmitted
            last */
            sectorperchunk = PSP_BUFFER_IO_SIZE / sectsize;
            extrasect = numSect % sectorperchunk;
            chunk = numSect / sectorperchunk;

            /* Set Align buffer pointer to BLK_CACHE_LINE_SIZE_IN_BYTES bit
            align address */
#ifdef PSP_MULTI_TASK_BUFFERS
            pDevice->alignInfo.pAlignedData = \
                &gBlkmediaIoBuffer[pDevice->DevBus * PSP_BUFFER_IO_SIZE];
#else
            pDevice->alignInfo.pAlignedData = gBlkmediaIoBuffer;
#endif
        }
    }

    pDevice->alignInfo.split = chunk;
    if ((extrasect == 0) && (pDevice->alignInfo.split > 0))
    {
        (pDevice->alignInfo.split)--;
    }
#endif /* #endif PSP_BUFF_ALIGNMENT */

    pReq = blkmediaAllocReq(pDevice);

    if (pReq != NULL)
    {
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
        if (TRUE == alignedflag)
        {
            for (count = 0; count < chunk; count++)
            {
                /* Copy application buffer to block media buffer */
                if (gBlkmediaSyncEdmaHandle != NULL)
                {
                    err = blkmediaEdmaMemcpy(gBlkmediaSyncEdmaHandle,
                              pDevice->alignInfo.pAlignedData,
                              (Uint8 *)buffer +
                                  (count * sectorperchunk * sectsize),
                              sectorperchunk*sectsize,
                              BLK_DEV_Write,
                              gBlkmediaIoChId,
                              gBlkmediaIoTcc);
                }
                else
                {
                    memcpy(pDevice->alignInfo.pAlignedData,
                        ((Uint8*)buffer + (count * sectorperchunk * sectsize)),
                        (sectorperchunk * sectsize));
                }

                /* Call driver write routine to transmit buffer */
                if (NULL == pDevice->pDevOps)
                {
                    err = IOM_EBADARGS;
                    break;
                }

                if (err == IOM_COMPLETED)
                {
                    err = pDevice->pDevOps->Blk_Write(pDevice->DevHandle,
                              (Ptr)pReq,
                              pDevice->alignInfo.pAlignedData,
                              sector + (count * sectorperchunk),
                              sectorperchunk);
                }
                if ((err != IOM_COMPLETED) || (pDevice->pDevOps == NULL))
                {
                    break;
                }
                /* Wait for transfer to be completed */
                if (SEM_pend(pDevice->ioSema, SYS_FOREVER) != TRUE)
                {
                    BLKMED_ERR_DEBUG(("\r\nBLK_MED: WIO :ioSema take fail"));
                    break;
                }
            }
            if ((extrasect != 0) && (err == IOM_COMPLETED))
            {
                /* Transmit extra sectors */
                if (gBlkmediaSyncEdmaHandle != NULL)
                {
                    err = blkmediaEdmaMemcpy(gBlkmediaSyncEdmaHandle,
                              pDevice->alignInfo.pAlignedData,
                              (Uint8*) buffer +
                                  (count * sectorperchunk * sectsize),
                              extrasect * sectsize,
                              BLK_DEV_Write,
                              gBlkmediaIoChId,
                              gBlkmediaIoTcc);
                }
                else
                {
                    memcpy(pDevice->alignInfo.pAlignedData,
                        ((Uint8*) buffer +
                            (count * sectorperchunk * sectsize)),
                        extrasect * sectsize);
                }

                if (pDevice->pDevOps == NULL)
                {
                    err = IOM_EBADARGS;
                }
                if (err == IOM_COMPLETED)
                {
                    err = pDevice->pDevOps->Blk_Write(pDevice->DevHandle,
                              (Ptr)pReq,
                              pDevice->alignInfo.pAlignedData,
                              sector + (count * sectorperchunk),
                              extrasect);
                    if (err == IOM_COMPLETED)
                    {
                        if (SEM_pend(pDevice->ioSema, SYS_FOREVER) != TRUE)
                        {
                            BLKMED_ERR_DEBUG(("\r\nBLK_MED: WIoSem take fail"));
                        }
                    }
                }
            }
        }
        else
        {
#endif /* #endif PSP_BUFF_ALIGNMENT */
            err = pDevice->pDevOps->Blk_Write(pDevice->DevHandle,
                      (Ptr)pReq,
                      buffer,
                      sector,
                      numSect);
            if (err == IOM_COMPLETED)
            {
                if (SEM_pend(pDevice->ioSema, SYS_FOREVER) != TRUE)
                {
                    BLKMED_ERR_DEBUG(("\r\nBLK_MED: WIoSem take fail"));
                }
            }
#ifdef PSP_BUFF_ALIGNMENT  /*If buffer alignment is enabled.    */
        }
#endif  /* #endif PSP_BUFF_ALIGNMENT    */
        if (err == IOM_COMPLETED)
        {
            err = pReq->Status.ReqInfo.Status.Error;
        }
        blkmediaRelReq(pDevice, pReq);
    }
#ifdef BLKMEDIA_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
        "BLKMEDIA",
        PSP_DVT_DESC(PSP_DVT_eFUNC_END,
                     PSP_DVT_dCHANNEL_ID_INST,
                     PSP_DVT_dPACKET_ID,
                     PSP_DVT_dSIZE_BYTES),
        CHANNELID_INSTANCEID(
            1,
            0),
        buffer,
        0);
#endif /* BLKMEDIA_INSTRUMENTATION_ENABLED */
    return err;
}



/**
 *  \brief Do a Block Write operation and return status under Acynch conditions
 *
 *  \param handle  [IN] : Device Handle.
 *  \param mediaHandle   [IN] : Application Handle.
 *  \param buffer  [IN] : Buffer Pointer
 *  \param sector  [IN] : Sector Number (start sector number)
 *  \param numSect [IN] : Count of Sectors
 *
 *  \return Int32       : Success (0)/ Failure (!0)
 ******************************************************************************/
static Int32 blkmediaWriteAsync(Ptr handle,
                                Ptr mediaHandle,
                                Ptr buffer,
                                Uint32 sector,
                                Uint32 numSect)
{
    Int32           err  = IOM_COMPLETED;
    BLK_DEV_Req     *pReq = NULL;
    BLK_DEV_Info    *pDevice;
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
    Uint32 sectsize, sectorperchunk = 0;
    Int32 count;
    Uint32 chunk = 0, extrasect = 0;
    Bool alignedflag = FALSE;
#endif

    assert(((NULL != handle) && (NULL != mediaHandle)) && (NULL != buffer));
    pDevice = (BLK_DEV_Info *)handle;

#ifdef BLKMEDIA_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
        "BLKMEDIA",
        PSP_DVT_DESC(PSP_DVT_eFUNC_START,
                     PSP_DVT_dCHANNEL_ID_INST,
                     PSP_DVT_dPACKET_ID,
                     PSP_DVT_dSIZE_BYTES),
        CHANNELID_INSTANCEID(
            1,
            0),
        buffer,
        numSect);
#endif /* BLKMEDIA_INSTRUMENTATION_ENABLED */

    /* Added for cache alignment    */
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
    sectsize = (pDevice->sectorSize);

    /* Check if Buffer need to be aligned */
    if (((Uint32)buffer % BLK_CACHE_LINE_SIZE_IN_BYTES) != 0)
    {
        if (PSP_OPMODE_DMAINTERRUPT == pDevice->opMode)
        {
            alignedflag = TRUE;
        }

        if (TRUE == alignedflag)
        {
            /* Find total no. of transfer and no. of bytes to be transmitted
            last */
            sectorperchunk = PSP_BUFFER_ASYNC_SIZE / sectsize;
            extrasect = numSect % sectorperchunk;
            chunk = numSect / sectorperchunk;

            /* Set Align buffer pointer to BLK_CACHE_LINE_SIZE_IN_BYTES bit
            align address */
            pDevice->alignInfo.pAlignedData = gBlkmediaAsyncBuffer;
        }
    }
    pDevice->alignInfo.split = chunk;
    if ((0 == extrasect) && (pDevice->alignInfo.split > 0))
    {
        (pDevice->alignInfo.split)--;
    }
#endif  /* #endif PSP_BUFF_ALIGNMENT    */

    pReq = blkmediaAllocReq(pDevice);
    if (pReq != NULL)
    {
        pReq->AppHandle = mediaHandle;

#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
        if (TRUE == alignedflag)
        {
            for (count = 0; count < chunk; count++)
            {
                /* Copy application buffer to block media buffer */
                if (gBlkmediaAsyncEdmaHandle != NULL)
                {
                    err = blkmediaEdmaMemcpy(gBlkmediaAsyncEdmaHandle,
                              pDevice->alignInfo.pAlignedData,
                              (Uint8*)buffer +
                                  (count * sectorperchunk * sectsize),
                              sectorperchunk * sectsize,
                              BLK_DEV_Write,
                              gBlkmediaAsyncChId,
                              gBlkmediaAsyncTcc);
                }
                else
                {
                    memcpy(pDevice->alignInfo.pAlignedData,
                        (Char*)buffer + (count * sectorperchunk * sectsize),
                        sectorperchunk * sectsize);
                }
                if (pDevice->pDevOps == NULL)
                {
                    err = IOM_EBADARGS;
                    break;
                }

                /* Call driver write routine to transmit buffer */
                err = pDevice->pDevOps->Blk_Write(pDevice->DevHandle,
                          (Ptr)pReq,
                          pDevice->alignInfo.pAlignedData,
                          sector + (count * sectorperchunk),
                          sectorperchunk);
                /* Wait for transfer to be completed */
                if (SEM_pend(pDevice->asyncSema, SYS_FOREVER) != TRUE)
                {
                    BLKMED_ERR_DEBUG(("\r\nBLK_MED: WAsyncSema take fail"));
                    break;
                }
                if ((err != IOM_COMPLETED) || (pDevice->pDevOps == NULL))
                {
                    blkmediaRelReq(pDevice, pReq);
                    break;
                }
            }
            if ((extrasect != 0) && (err == IOM_COMPLETED))
            {
                if (NULL == pDevice->pDevOps)
                {
                    err = IOM_EBADARGS;
                }
                else
                {
                    /* Transmit extra sectors */
                    if (gBlkmediaAsyncEdmaHandle != NULL)
                    {
                        err = blkmediaEdmaMemcpy(gBlkmediaAsyncEdmaHandle,
                                  pDevice->alignInfo.pAlignedData,
                                  (Uint8 *)buffer +
                                      (count * sectorperchunk * sectsize),
                                  extrasect * sectsize,
                                  BLK_DEV_Write,
                                  gBlkmediaAsyncChId,
                                  gBlkmediaAsyncTcc);
                    }
                    else
                    {
                        memcpy(pDevice->alignInfo.pAlignedData,
                            (Char*)buffer + (count * sectorperchunk * sectsize),
                            extrasect * sectsize);
                    }

                    err = pDevice->pDevOps->Blk_Write(pDevice->DevHandle,
                              (Ptr)pReq,
                              pDevice->alignInfo.pAlignedData,
                              sector + (count * sectorperchunk),
                              extrasect);
                    /* Wait for transfer to be completed */
                    if (SEM_pend(pDevice->asyncSema, SYS_FOREVER) != TRUE)
                    {
                        BLKMED_ERR_DEBUG(("\r\nBLK_MED: WAsyncSema take fail"));
                    }
                }
            }
        }
        else
        {
#endif  /* #endif PSP_BUFF_ALIGNMENT    */
            err = pDevice->pDevOps->Blk_Write(pDevice->DevHandle,
                      (Ptr)pReq,
                      buffer,
                      sector,
                      numSect);
            /* Wait for transfer to be completed */
            if (SEM_pend(pDevice->asyncSema, SYS_FOREVER) != TRUE)
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: WAsyncSema take fail"));
            }
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
        }
#endif  /* #endif PSP_BUFF_ALIGNMENT    */
    }
    else
    {
        err = IOM_EBADARGS;
    }
#ifdef BLKMEDIA_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
        "BLKMEDIA",
        PSP_DVT_DESC(PSP_DVT_eFUNC_END,
                     PSP_DVT_dCHANNEL_ID_INST,
                     PSP_DVT_dPACKET_ID,
                     PSP_DVT_dSIZE_BYTES),
        CHANNELID_INSTANCEID(
            1,
            0),
        buffer,
        0);
#endif /* BLKMEDIA_INSTRUMENTATION_ENABLED */
    return err;
}


/**
 *  \brief Dummy IOCTL function
 *
 *   \param handle      [IN]  : Handle of the Block Device
 *   \param mediaHandle       [IN]  : Job Handle from this module
 *   \param pIoctlInfo  [IN]  : IOCTL information
 *   \param pIsComplete [OUT] : Is the IOCTL complete
 *
 *  \return Result : IOM_EBADARGS
 ******************************************************************************/
static Int32 blkmediaIoctlVoid(Ptr handle,
                               Ptr mediaHandle,
                               const PSP_BlkDevIoctlInfo_t *pIoctlInfo,
                               Bool *pIsComplete)
{
    handle = handle; /* to remove compilation warning */
    mediaHandle = mediaHandle; /* to remove compilation warning */
    pIoctlInfo = pIoctlInfo; /* to remove compilation warning */
    pIsComplete = pIsComplete; /* to remove compilation warning */
    return IOM_EBADARGS;
}



/**
 *  \brief interface to Internal IOCTL
 *
 *  \param handle      [IN]  Handle of the Block Device
 *  \param mediaHandle [IN]  Job Handle from this module. Can be NULL
 *  \param pIoctlInfo  [IN]  IOCTL information
 *  \param pIsComplete [OUT] Is the IOCTL complete. Can be NULL
 *
 *  \return Result - Success (0)/ Failure (!0)
 ******************************************************************************/
static Int32 blkmediaIntIoctl(Ptr handle,
                              Ptr mediaHandle,
                              PSP_BlkDevIoctlInfo_t const *pIoctlInfo,
                              Bool *pIsComplete)
{
    BLK_DEV_Info  *pDevice;
    Int32           err = IOM_EBADARGS;
    Bool            isComplete = FALSE;
    BLK_DEV_Req   *pReq = NULL;

    assert((NULL != handle) && (NULL != pIoctlInfo));

    pDevice = (BLK_DEV_Info *)handle;
    mediaHandle = mediaHandle; /* to remove compilation warning */

    if ((pDevice != NULL) && (pDevice->pDevOps != NULL))
    {
        /*Allocate the request*/
        pReq = blkmediaAllocReq(pDevice);

        if (pReq != NULL)
        {
            /*set the ioctl flag of the request token*/
            pReq->isIoctl = TRUE;
            /*Call the appropriate storage ioctl*/
            err = pDevice->pDevOps->Blk_Ioctl(pDevice->DevHandle,
                      pReq,
                      pIoctlInfo,
                      &isComplete);
            if (err == IOM_COMPLETED)
            {
                if (FALSE == isComplete)
                {
                    SEM_pend(pDevice->ioSema, SYS_FOREVER);
                    err = pReq->Status.ReqInfo.Status.Error;
                }
                /*if it is a request to change the opmode then update the same
                in block media structure*/
                if (PSP_BLK_SETOPMODE == pIoctlInfo->Cmd)
                {
                    pDevice->opMode = (PSP_BlkOpMode)(*(pIoctlInfo)->pData);
                }

                if (NULL != pIsComplete)
                {
                    *pIsComplete = TRUE;
                }
            }
            /*clear the ioctl flag*/
            pReq->isIoctl = FALSE;
            blkmediaRelReq(pDevice, pReq);
        }
    }
  return err;
}


/**
 *  \brief Void IO function
 *
 *  \param handle  [IN] : Application Handle.
 *  \param mediaHandle   [IN] : Request Handle
 *  \param buffer  [IN] : Buffer Pointer
 *  \param sector  [IN] : Sector Number (start sector number)
 *  \param numSect [IN] : Count of Sectors
 *
 *  \return Int32    : IOM_EBADARGS
 ******************************************************************************/
static Int32 blkmediaVoidIo(Ptr handle,
                            Ptr mediaHandle,
                            Ptr buffer,
                            Uint32 sector,
                            Uint32 numSect)
{
    handle = handle; /* to remove compilation warning */
    mediaHandle = mediaHandle; /* to remove compilation warning */
    buffer = buffer; /* to remove compilation warning */
    sector = sector; /* to remove compilation warning */
    numSect = numSect; /* to remove compilation warning */
    return IOM_EBADARGS;
}


/**
 *  \brief Allocate a Request token from ReqCache of the device.
 *
 *  \param pDrive [IN] : Pointer to the Drive.
 *
 *  \return BLK_DEV_Req*  : Pointer to allocated request
 ******************************************************************************/
static BLK_DEV_Req* blkmediaAllocReq(BLK_DEV_Info *pDrive)
{
    BLK_DEV_Req  *pReq;
    Uint32         cookie;

    assert(NULL != pDrive);

    cookie = HWI_disable();
    if (pDrive->AvailReq != 0)
    {
        pReq = pDrive->pReqCache;
        QUE_remove(&(pDrive->pReqCache->Link));
        pDrive->AvailReq--;
    }
    else
    {
        pDrive->pReqCache = NULL;
        pReq = NULL;
    }

    HWI_restore(cookie);
    return pReq;
}


/**
 *  \brief Release a Request token to ReqCache of the device.
 *
 *  \param pDrive [IN] : Pointer to the Drive.
 *  \param pReq   [IN] : Pointer to allocated request
 *
 *  \return Void
 ******************************************************************************/
static Void blkmediaRelReq(BLK_DEV_Info *pDrive, BLK_DEV_Req *pReq)
{
    Uint32          cookie;

    assert((NULL != pDrive) && (NULL != pReq));

    cookie = HWI_disable();
    if (pDrive->AvailReq != 0)
    {
        QUE_insert(&(pDrive->pReqCache->Link), &(pReq->Link));
    }
    else
    {
        pDrive->pReqCache = pReq;
    }
    pDrive->AvailReq++;
    HWI_restore(cookie);
}



/**
 *  \brief EDMA copy function
 *
 *  \param hEdma     [IN] : Edma Handle
 *  \param dst       [IN] : Destination Buffer
 *  \param src       [IN] : Destination Buffer.
 *  \param len       [IN] : Length of buffer
 *  \param dir       [IN] : Read/Write
 *  \param sinch1Id  [IN] : Channel ID of EDMA
 *  \param sinch1Tcc [IN] : Interrupt to which EDMA to be linked
 *
 *  \return Int32  : Success (IOM_COMPLETED) / Failure (IOM_EBADARGS)
 ******************************************************************************/
static Int32 blkmediaEdmaMemcpy(Ptr hEdma,
                                Uint8 *dst,
                                Uint8 *src,
                                Uint32 len,
                                BLK_DEV_DataDir dir,
                                Uint32 sinch1Id,
                                Uint32 sinch1Tcc)
{
    Uint32 unalignedLen, alignedSize, remBytes;
    Int32  err = IOM_COMPLETED;

    assert(((NULL != hEdma) && (NULL != dst)) && (NULL != src));

    if (dir == BLK_DEV_Write)
    {
        /* In case of write src buffer will be unaligned*/
        /* Find size of first unaligned bytes */
        unalignedLen = BLK_CACHE_LINE_SIZE_IN_BYTES -
                           ((Uint32)src % BLK_CACHE_LINE_SIZE_IN_BYTES);

        /* Get aligned data size */
        alignedSize = len - BLK_CACHE_LINE_SIZE_IN_BYTES;

        /* Get last unaligned data size */
        remBytes = BLK_CACHE_LINE_SIZE_IN_BYTES - unalignedLen;

        /* Copy first unaligned data*/
        memcpy(dst, src, unalignedLen);

        /* Move src and dst buffer to copy last unaligned data*/
        src += (unalignedLen + alignedSize);
        dst += (unalignedLen + alignedSize);

        /* Copy last unaligned data*/
        memcpy(dst, src, remBytes);

        /* Move src and dst buffer to aligned address*/
        src -= (alignedSize);
        dst -= (unalignedLen + alignedSize);

        /*Flush src and flush,invalidate dst buffer*/
        BCACHE_wb(src, alignedSize, TRUE);
        BCACHE_wbInv(dst, len, TRUE);

        /*Increment Destination */
        dst += unalignedLen;

        /* Copy aligned size of the bytes using EDMA*/
        if (IOM_COMPLETED != blkmediaEdmaMemcpyConfig(hEdma,
                                 dst,
                                 src,
                                 alignedSize,
                                 sinch1Id,
                                 sinch1Tcc))
        {
            err = IOM_EBADARGS;
        }
    }
    else
    {
        /* In case of read dst buffer will be unaligned*/
        /* Find size of first unaligned bytes */
        unalignedLen = BLK_CACHE_LINE_SIZE_IN_BYTES -
                           ((Uint32)dst % BLK_CACHE_LINE_SIZE_IN_BYTES);

        /* Get aligned data size */
        alignedSize = len - BLK_CACHE_LINE_SIZE_IN_BYTES;

        /* Get last unaligned data size */
        remBytes = BLK_CACHE_LINE_SIZE_IN_BYTES - unalignedLen;

        /* Copy first unaligned data*/
        memcpy(dst, src, unalignedLen);

        /* Move src and dst buffer to copy last unaligned data*/
        src += (unalignedLen + alignedSize);
        dst += (unalignedLen + alignedSize);

        /* Copy last unaligned data*/
        memcpy(dst, src, remBytes);

        /* Move src and dst buffer to aligned address*/
        dst -= (alignedSize);
        src -= (unalignedLen + alignedSize);

        /*Flush src and flush,invalidate destination buffer*/
        BCACHE_wb(src, len, TRUE);
        BCACHE_wbInv(dst, alignedSize, TRUE);
        /*Increment Destination */
        src += unalignedLen;

        /* Copy aligned size of the bytes using EDMA*/
        if (IOM_COMPLETED != blkmediaEdmaMemcpyConfig(hEdma,
                                 dst,
                                 src,
                                 alignedSize,
                                 sinch1Id,
                                 sinch1Tcc))
        {
            err = IOM_EBADARGS;
        }
    }
    return err;
}


/**
 *  \brief EDMA copy function
 *
 *  \param hEdma        [IN] : EDMA handle
 *  \param dst          [IN] : Destination Buffer
 *  \param src          [IN] : Destination Buffer.
 *  \param len          [IN] : Length of buffer
 *  \param sinch1Id     [IN] : Channel ID of EDMA
 *  \param sinch1Tcc    [IN] : Interrupt to which EDMA to be linked
 *
 *  \return Int32            : Success (IOM_COMPLETED) / Failure (IOM_EBADARGS)
 ******************************************************************************/
static Int32 blkmediaEdmaMemcpyConfig(Ptr hEdma,
                                      Uint8 *dst,
                                      Uint8 *src,
                                      Uint32 len,
                                      Uint32 sinch1Id,
                                      Uint32 sinch1Tcc)
{
    Int32   result;
    Uint32  aCount, bCount, cCount, bIndex, cIndex, temp;
    Bool    flag = TRUE;

    assert(((NULL != hEdma) && (NULL != dst)) && (NULL != src));

    do
    {
        /* Initialize the local variables as required                         */
        result = IOM_EBADARGS;
        flag = FALSE;

        aCount = bCount = cCount = 0;

        if (len <= 65024)
        {
            aCount = len;
            bCount = 1u;
            cCount = 1u;
            bIndex = 0u;
            cIndex = 0u;
        }
        else
        {
            /* There is a limitation in edma driver for bIndex value to be max
            32766*/
            temp = 1u;
            do
            {
                temp++;
                bCount = temp;
                aCount = len/temp;
            }while (((bCount * aCount) != len) || (aCount > 32764));

            cCount = 1u;
            bIndex = aCount;
            cIndex = 0;
        }

        /* Write src address to EDMA controller*/
        result = EDMA3_DRV_setSrcParams(hEdma,
                     sinch1Id,
                     (Uint32)(src),
                     EDMA3_DRV_ADDR_MODE_INCR,
                     EDMA3_DRV_W8BIT);
        if (IOM_COMPLETED != result)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED: EDMA3SetSrcParams Failed"));
            break;
        }

        /* Write Dest address to EDMA controller*/
        result = EDMA3_DRV_setDestParams(hEdma,
                     sinch1Id,
                     (Uint32)(dst),
                     EDMA3_DRV_ADDR_MODE_INCR,
                     EDMA3_DRV_W32BIT);
        if (IOM_COMPLETED != result)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED: Edma3SetDestParams Failed"));
            break;
        }

        /* Set Src index */
        result = EDMA3_DRV_setSrcIndex(hEdma, sinch1Id, bIndex, cIndex);
        if (IOM_COMPLETED != result)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED: Edma3SetSrcIndex Failed"));
            break;
        }

        /* Set Dest index */
        result = EDMA3_DRV_setDestIndex(hEdma, sinch1Id, bIndex, cIndex);
        if (IOM_COMPLETED != result)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED: Edma3SetDestIndex Failed"));
            break;
        }

        /* Set A, B and C count and mode of transfer*/
        result = EDMA3_DRV_setTransferParams(hEdma,
                     sinch1Id,
                     aCount,
                     bCount,
                     cCount,
                     0,
                     EDMA3_DRV_SYNC_AB);
        if (IOM_COMPLETED != result)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED: Edma3SetTransferParams Failed"));
            break;
        }

        /* Enable interrupt*/
        result = EDMA3_DRV_setOptField(hEdma,
                     sinch1Id,
                     EDMA3_DRV_OPT_FIELD_TCINTEN,
                     1u);
        if (IOM_COMPLETED != result)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED: EdmaSetOptField Failed"));
            break;
        }

        result = EDMA3_DRV_setOptField(hEdma,
                     sinch1Id,
                     EDMA3_DRV_OPT_FIELD_TCC,
                     sinch1Tcc);
        if (IOM_COMPLETED != result)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED: EdmaSetOptField Failed"));
            break;
        }

        /* Start transfer */
        result = EDMA3_DRV_enableTransfer(hEdma,
                     sinch1Id,
                     EDMA3_DRV_TRIG_MODE_MANUAL);
        if (IOM_COMPLETED != result)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED: EdmaEdma3EnableTransfer Failed"));
            break;
        }

#ifdef PSP_BLK_EDMA_MEMCPY_IO
        if (sinch1Id == gBlkmediaIoChId)
        {
            if (TRUE != SEM_pend(gBlkmediaIoEdmaSem, EMEMCOPY_SEM_TAKE_TIMEOUT))
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: SEM_pend IoEdmaSem Failed"));
                result = IOM_EBADMODE;
                break;
            }
        }

#endif /* PSP_BLK_EDMA_MEMCPY_IO */
#ifdef PSP_BLK_EDMA_MEMCPY_ASYNC
        if (sinch1Id == gBlkmediaAsyncChId)
        {
            if (TRUE != SEM_pend(gBlkmediaAsyncEdmaSem, EMEMCOPY_SEM_TAKE_TIMEOUT))
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: SEM_pend AsyncEdmaSem Failed"));
                result = IOM_EBADMODE;
                break;
            }
        }
#endif  /* PSP_BLK_EDMA_MEMCPY_ASYNC */

    }while (flag);

    return (result);
}


#if defined(PSP_BLK_EDMA_MEMCPY_IO) || defined(PSP_BLK_EDMA_MEMCPY_ASYNC)
#ifdef PSP_BLK_EDMA_MEMCPY_IO
static Void blkmediaIoEdmaCallback(Uint32 tcc,
                                   EDMA3_RM_TccStatus status,
                                   Ptr appData)
{
    Int32 result;
    tcc = tcc; /* to remove compilation warning */
    appData = appData; /* to remove compilation warning */
    switch (status)
    {
        case EDMA3_RM_XFER_COMPLETE:
            result = EDMA3_DRV_disableTransfer(gBlkmediaSyncEdmaHandle,
                         gBlkmediaIoChId,
                         EDMA3_DRV_TRIG_MODE_MANUAL);
            if (IOM_COMPLETED != result)
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: EDMA3DisableTransfer Failed"));
                return ;
            }
            result = EDMA3_DRV_setOptField(gBlkmediaSyncEdmaHandle,
                         gBlkmediaIoChId,
                         EDMA3_DRV_OPT_FIELD_TCINTEN,
                         0);
            if (IOM_COMPLETED != result)
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: EDMA3_DRV_setOptField Failed"));
            }
            SEM_post(gBlkmediaIoEdmaSem);
            break;

        case EDMA3_RM_E_CC_DMA_EVT_MISS:
            /*Error!!! DMA Event Miss Interrupt received*/
            break;

        case EDMA3_RM_E_CC_QDMA_EVT_MISS:
            /*Error!!! QDMA Event Miss Interrupt received*/
            break;

        default:
            /*Invalid TCC Status received in the Interrupt handler*/
            break;
    }
}
#endif /* PSP_BLK_EDMA_MEMCPY_IO */


#ifdef PSP_BLK_EDMA_MEMCPY_ASYNC
Void blkmediaAsyncEdmaCallback(Uint32 tcc,
                               EDMA3_RM_TccStatus status,
                               Ptr appData)
{
    Int32 result;
    tcc = tcc; /* to remove compilation warning */
    appData = appData; /* to remove compilation warning */
    switch (status)
    {
        case EDMA3_RM_XFER_COMPLETE:
            result = EDMA3_DRV_disableTransfer(gBlkmediaAsyncEdmaHandle,
                         gBlkmediaAsyncChId,
                         EDMA3_DRV_TRIG_MODE_MANUAL);
            if (IOM_COMPLETED != result)
            {
                BLKMED_ERR_DEBUG(("\r\n BLK_MED : EDMA Disable Tx Failed"));
                return ;
            }
            result = EDMA3_DRV_setOptField(gBlkmediaAsyncEdmaHandle,
                         gBlkmediaAsyncChId,
                         EDMA3_DRV_OPT_FIELD_TCINTEN,
                         0);
            if (IOM_COMPLETED != result)
            {
                BLKMED_ERR_DEBUG(("\r\n BLK_MED: EDMA3 setOptField Failed"));
            }
            SEM_post(gBlkmediaAsyncEdmaSem);
            break;

        case EDMA3_RM_E_CC_DMA_EVT_MISS:
            /*Error!!! DMA Event Miss Interrupt received*/
            break;

        case EDMA3_RM_E_CC_QDMA_EVT_MISS:
            /*Error!!! QDMA Event Miss Interrupt received*/
            break;

        default:
            /*Invalid TCC Status received in the Interrupt handler*/
            break;
    }
}
#endif /* PSP_BLK_EDMA_MEMCPY_ASYNC */
#endif /*defined(PSP_BLK_EDMA_MEMCPY_IO) || defined(PSP_BLK_EDMA_MEMCPY_ASYNC)*/



#if (PSP_FILE_SYSTEM)
static Int32 blkmediaGetDeviceParams(Uint32 DevNo, BLK_DEV_Info *pDevice)
{
    Int32                   isSdhcCard = 0;
    Uint32                  cardSize, totalSectors, bytesPerSector;
    PSP_BlkDevIoctlInfo_t   ioctlInfo;
    Int32 result = IOM_COMPLETED;
    Bool    flag = TRUE;

    assert(NULL != pDevice);
    do
    {
        flag = FALSE;
        if (pDevice->DevState != BLK_DEV_REG)
        {
            result = IOM_EBADARGS;
            break;
        }

        totalSectors = pDevice->maxUserSect;
        bytesPerSector = pDevice->sectorSize;

        if (PSP_BLK_DRV_NAND == DevNo)
        { /* NAND device */
            if (2048 == bytesPerSector)
            {
                pDevice->Head = 16u;
                pDevice->sectPerTrack = NAND_SPT;
            }
            if (512 == bytesPerSector)
            {
                pDevice->Head = 4u;
                pDevice->sectPerTrack = NAND_SPT;
            }
        }

        if ((PSP_BLK_DRV_MMC0 == DevNo)
#if defined(CHIP_C6748) || defined(CHIP_OMAPL138)
            || (PSP_BLK_DRV_MMC1 == DevNo)
#endif
            )
        {
            ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                PSP_MMCSD_IOCTL_CHECK_HIGH_CAPACITY_CARD);
            ioctlInfo.pData = (Ptr)&isSdhcCard;
            if ((pDevice->BlkOps.Blk_Ioctl((Ptr)pDevice,
                     NULL, &ioctlInfo, NULL)) != IOM_COMPLETED)
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: Could not find SDHC"));
                result = IOM_EBADARGS;
                break;
            }

            ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                PSP_MMCSD_IOCTL_GET_CARD_SIZE);
            ioctlInfo.pData = (Ptr)&cardSize;
            if ((pDevice->BlkOps.Blk_Ioctl((Ptr)pDevice,
                     NULL, &ioctlInfo, NULL)) != IOM_COMPLETED)
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: Could not find card size"));
                result = IOM_EBADARGS;
                break;
            }

            if (FALSE == isSdhcCard)
            {
                if (cardSize < 0x200000)
                {   /* ~ 2MB*/
                    pDevice->Head           = 2u;
                    pDevice->sectPerTrack   = 16u;
                }
                else if (cardSize < 0x1000000)
                {   /* ~ 16MB*/
                    pDevice->Head           = 2u;
                    pDevice->sectPerTrack   = 32u;
                }
                else if (cardSize < 0x2000000)
                {   /* ~ 32MB*/
                    pDevice->Head           = 4u;
                    pDevice->sectPerTrack   = 32u;
                }
                else if (cardSize < 0x8000000)
                {   /* ~ 128MB*/
                    pDevice->Head           = 8u;
                    pDevice->sectPerTrack   = 32u;
                }
                else if (cardSize < 0x10000000)
                {   /* ~ 256MB*/
                    pDevice->Head           = 16u;
                    pDevice->sectPerTrack   = 32u;
                }
                else if (cardSize < 0x1F800000)
                {   /* ~ 504MB*/
                    pDevice->Head           = 16u;
                    pDevice->sectPerTrack   = 63u;
                }
                else if (cardSize < 0x3F000000)
                {   /* ~ 1008MB*/
                    pDevice->Head           = 32u;
                    pDevice->sectPerTrack   = 63u;
                }
                else if (cardSize < 0x7E00C000)
                {   /* ~ 2016MB*/
                    pDevice->Head           = 64u;
                    pDevice->sectPerTrack   = 63u;
                }
                else
                {   /* ~ 2048MB*/
                    pDevice->Head           = 128u;
                    pDevice->sectPerTrack   = 63u;
                }
            }
            else
            {
                if (cardSize < 0x3f0000)
                {   /* ~ 4032MB*/
                    pDevice->Head           = 128u;
                    pDevice->sectPerTrack   = 63u;
                }
                else
                {   /* ~ 32768MB*/
                    pDevice->Head           = 255u;
                    pDevice->sectPerTrack   = 63u;
                }
            }
        }

        if ((PSP_BLK_DRV_USB0 == DevNo) || (PSP_BLK_DRV_USB1 == DevNo))
        {
            pDevice->Head           = 16u;
            pDevice->sectPerTrack   = 32u;
        }
#if defined(CHIP_C6748) || defined(CHIP_OMAPL138)
        if ((PSP_BLK_DRV_ATA0 == DevNo) || (PSP_BLK_DRV_ATA1 == DevNo))
        {
            pDevice->Head           = 16u;
            pDevice->sectPerTrack   = 32u;
        }
#endif
        pDevice->Cyl = (Uint16)(totalSectors /
                        (pDevice->Head * pDevice->sectPerTrack));
        }while (flag);

    return (result);
}


static Int32 blkmediaRtfsMountDevice(BLK_DEV_Info *pDevice, Uint32 devNum)
{
    Int32                           err = IOM_EBADARGS;
    PSP_BlkDevIoctlInfo_t           ioctlInfo;
    PSP_nandDeviceInfo              devInfo;
    Int32                           result;
    struct rtfs_media_insert_args   rtfsInsertParms;
    /* Media parameter structure passed to RTFS*/

    assert(NULL != pDevice);

    result = blkmediaGetDeviceParams(devNum, pDevice);
    if (IOM_EBADARGS == result)
    {
        BLKMED_ERR_DEBUG(("\r\nBLK_MED: blkmediaGetDeviceParams fail"));
        return (err);
    }

    /* Initialize Rtfs insert parms structure */
    switch (devNum)
    {
        case PSP_BLK_DRV_USB0:
        case PSP_BLK_DRV_USB1:
            /*For non erasable media set the erasse block size as 0*/
            pDevice->eraseBlockSize = 0;
            rtfsInsertParms.device_type = BFS_DEVICE_TYPE_USB;
            rtfsInsertParms.unit_number = (Int32)(devNum - PSP_BLK_DRV_USB0);
            break;

        case PSP_BLK_DRV_MMC0:
#if defined(CHIP_C6748) || defined(CHIP_OMAPL138)
        case PSP_BLK_DRV_MMC1:
#endif
            /*For non erasable media set the erasse block size as 0*/
            pDevice->eraseBlockSize = 0;
            rtfsInsertParms.device_type = BFS_DEVICE_TYPE_MMC;
            rtfsInsertParms.unit_number = (Int32)(devNum - PSP_BLK_DRV_MMC0);
            break;

        case PSP_BLK_DRV_NAND:
            /*Reterive the nand erase block size. Erase blk size is equal to no
            of pages per block i.e. no of sectors per block.*/
            ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                PSP_NAND_IOCTL_GET_DEVICE_INFO);
            ioctlInfo.pData = (Ptr)&devInfo;
            if ((pDevice->BlkOps.Blk_Ioctl((Ptr)pDevice,
                     NULL, &ioctlInfo, NULL)) != IOM_COMPLETED)
            {
                BLKMED_ERR_DEBUG(("\r\nBLK_MED: Could not find erase blk size"
                    " using default of 32"));
                pDevice->eraseBlockSize = 32u;/*default erase block size*/
            }
            else
            {
                pDevice->eraseBlockSize = devInfo.pagesPerBlock;
            }
            rtfsInsertParms.device_type = BFS_DEVICE_TYPE_NAND;
            rtfsInsertParms.unit_number = 0;
            break;
#if defined(CHIP_C6748) || defined(CHIP_OMAPL138)
        case PSP_BLK_DRV_ATA0:
        case PSP_BLK_DRV_ATA1:
            /*For non erasable media set the erasse block size as 0*/
            pDevice->eraseBlockSize = 0;
            rtfsInsertParms.device_type = BFS_DEVICE_TYPE_ATA;
            rtfsInsertParms.unit_number = (Int32)(devNum - PSP_BLK_DRV_ATA0);
            break;
#endif
        default:
            err = IOM_EBADARGS;
            return (err);
    }

    /* Set up mount parameters and call Rtfs to mount the device*/
    rtfsInsertParms.devhandle = (Void *)pDevice;
    rtfsInsertParms.media_size_sectors = (dword)pDevice->maxUserSect;
    rtfsInsertParms.numheads = (dword)pDevice->Head;
    rtfsInsertParms.secptrk  = (dword)pDevice->sectPerTrack;
    rtfsInsertParms.numcyl   = (dword)pDevice->Cyl;
    if (0 == rtfsInsertParms.numcyl)
    {
        rtfsInsertParms.numcyl = 1u;
    }
    if (rtfsInsertParms.numcyl > 1023)
    {
        rtfsInsertParms.numcyl = 1023u;
    }

    rtfsInsertParms.sector_size_bytes        = (dword)pDevice->sectorSize;
    rtfsInsertParms.eraseblock_size_sectors  = (dword)pDevice->eraseBlockSize;
    rtfsInsertParms.write_protect            = \
        (int) pDevice->isMediaWriteProtect;

    /* Attach IO functions in this file */
    rtfsInsertParms.device_io                = &blkmediaIo;
    rtfsInsertParms.device_erase             = &blkmediaEraseSectors;
    rtfsInsertParms.device_ioctl             = &blkmediaIoctl;

    /* Attach Device mount and volume mount configuration functions in the
    adaptation layer */
    rtfsInsertParms.device_configure_media   = \
        &FSADAPTMEM_device_configure_media;
    rtfsInsertParms.device_configure_volume  = \
        &FSADAPTMEM_device_configure_volume;

    /* Provide a sector buffer for system operations like reading partition
    table, also used for multisector IO */
    if (pc_rtfs_media_insert(&rtfsInsertParms) < 0)
    {
        BLKMED_ERR_DEBUG(("\r\nBLKMED: pc_rtfs_media_insert fails"));
        return (err);
    }
    err = IOM_COMPLETED;
    return (err);
}

static int blkmediaEraseSectors(Void *devHandle,
                                Void *pdrive,
                                Uint32 start_sector,
                                Uint32 n_sectors)
{
    BLK_DEV_Info            *pDevice;
    PSP_BlkDevIoctlInfo_t   ioctlInfo;
    Int32                   eraseStatus = 1;

    assert(NULL != devHandle);
    pDevice = (BLK_DEV_Info *)devHandle;
    assert(NULL != pDevice);
    pdrive = pdrive; /* to remove compilation warning */
    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                        PSP_NAND_IOCTL_ERASE_BLOCK);
    ioctlInfo.pData = (Ptr)&start_sector;/*should be erase block aligned*/
    ioctlInfo.pData1 = (Ptr)&n_sectors;
    if ((pDevice->BlkOps.Blk_Ioctl((Ptr)pDevice,
             NULL, &ioctlInfo, NULL)) != IOM_COMPLETED)
    {
        eraseStatus = 0;
    }
    return eraseStatus;
}


static Int32 blkmediaIo(Void *devHandle,
                        Void *pdrive,
                        Uint32 sector,
                        Void *pBuf,
                        Uint32 numOfSector,
                        Int32 reading)
{
    BLK_DEV_Info      *pDevice;
    Int32             err = 0;

    assert(NULL != devHandle);
    pDevice = (BLK_DEV_Info *)devHandle;

    /* parameter checking */
    assert((NULL != pBuf) && (0 != numOfSector));
    pdrive = pdrive; /* to remove compilation warning */
    if (TRUE == reading)
    {
        err = blkmediaReadIo((Ptr)pDevice, NULL, pBuf, sector, numOfSector);
        if (err != IOM_COMPLETED)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED: Read fail at %d with %d sectors", \
                sector, numOfSector));
        }
    }
    else    /*it's a write*/
    {
        err = blkmediaWriteIo((Ptr)pDevice, NULL, pBuf, sector, numOfSector);
        if (err != IOM_COMPLETED)
        {
            BLKMED_ERR_DEBUG(("\r\nBLK_MED: Write fail at %d with %d sector", \
                sector, numOfSector));
        }
    }

    if (IOM_COMPLETED == err)
    {
        err = 1;
    }
    else
    {
        err = 0;
    }

    return err;
}


static int blkmediaIoctl(Void *devHandle,
                         Void *pdrive,
                         int opcode,
                         int iArgs,
                         Void *vargs)
{
    BLK_DEV_Info    *pDevice;
    Int32 ioctlStatus = 0;

    assert(NULL != devHandle);
    pDevice = (BLK_DEV_Info *)devHandle;
    assert(NULL != pDevice);
    iArgs = iArgs; /* to remove compilation warning */
    vargs = vargs; /* to remove compilation warning */
    switch (opcode)
    {
        case RTFS_IOCTL_FORMAT:
            if (0 != pDevice->eraseBlockSize)
            {
                if (0 == blkmediaEraseSectors(devHandle,
                         pdrive,
                         0u,
                         pDevice->maxUserSect))
                {
                    ioctlStatus = -1;
                }
            }
            break;

        case RTFS_IOCTL_SHUTDOWN:
            break;

        default:
            ioctlStatus = -1;
            break;
    }
    return ioctlStatus;
}
#endif /* #if (PSP_FILE_SYSTEM) */
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
