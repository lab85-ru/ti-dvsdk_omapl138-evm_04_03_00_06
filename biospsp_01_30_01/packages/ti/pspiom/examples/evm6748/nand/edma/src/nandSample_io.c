/*
 * nandSample_io.c
 *
 * This file contains the test / demo code to demonstrate the NAND driver func-
 * tionality in DMA mode of operation on DSP/BIOS 5.
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

/** \file   nandSample_io.c
 *
 *  \brief  Demo application for the nand driver. The following are the steps
 *          followed:
 *          1. Detect the NAND
 *          2. Open a file for write and fill it up with data
 *          3. Read the data back from the file and compare
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#include <std.h>
#include <sem.h>
#include <iom.h>
#include <tsk.h>
#include <log.h>
#include <string.h>
#include <stdio.h>

#include "ti/pspiom/blkmedia/psp_blkdev.h"
#include "ti/pspiom/nand/psp_nand.h"

#define BYTE_COUNT                  ((3u)*(1024u)*(1024u))

#define BUFFER_ALINED_CACHE_LENGTH
/* Enable this if user want to pass cache length aligned buffer, In polled mode
   buffer must be 4 byte aligned */

#define DATA_ALIGN                  128

/*
 * External references
 */
extern LOG_Obj    trace;

/*
* Global Static references
*/
static SEM_Handle nandCbSem;
Uint32 nandRespInfo;
/*
 * Note: Buffer alignement is required only when working in DMA Mode.
 */
static Uint8 srcNandBufOrg[BYTE_COUNT + DATA_ALIGN];
/*+128 if need to make unalign for testing purpose*/
static Uint8 dstNandBufOrg[BYTE_COUNT + DATA_ALIGN];
/*+128 if need to make unalign for testing purpose*/
static Uint8 *writeBuf = NULL;
static Uint8 *readBuf = NULL;
Bool blkNandTestCallBack(Ptr handle, PSP_BlkDevRes_t const *pErrInfo);
static Void blkNandTest(Void);


/* This function will pass only if nand is already registered with media layer*/
static Int32 checkBlockMediaIoctl(PSP_BlkDevOps_t* pDevOps, Ptr handle)
{
    PSP_BlkDevIoctlInfo_t   ioctlInfo;
    Int32                   result;
    Uint32                  blockSize;
    Uint32                  sectCnt;
    PSP_NandOpMode          opmode;
    Bool                    isRemovable;
    Bool                    isComplete;
    Uint32                  eventQueue;

    ioctlInfo.Cmd = PSP_BLK_GETOPMODE;
    ioctlInfo.pData = (Ptr)&opmode;
    result = pDevOps->Blk_Ioctl(handle,
                 (Ptr)&nandRespInfo,
                 &ioctlInfo,
                 &isComplete);
    if (IOM_COMPLETED != result)
    {
        LOG_printf(&trace, " NAND_SAMPLE: GET OPMODE - Fails");
    }

    ioctlInfo.Cmd = PSP_BLK_GETBLKSIZE;
    ioctlInfo.pData = (Ptr)&blockSize;
    result = pDevOps->Blk_Ioctl(handle,
                 (Ptr)&nandRespInfo,
                 &ioctlInfo,
                 &isComplete);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " NAND_SAMPLE: Block size is %d", blockSize);
    }
    else
    {
        LOG_printf(&trace, " NAND_SAMPLE: GET BLK SIZE - Fails");
    }

    ioctlInfo.Cmd = PSP_BLK_GETSECTMAX;
    ioctlInfo.pData = (Ptr)&sectCnt;
    result = pDevOps->Blk_Ioctl(handle,
                 (Ptr)&nandRespInfo,
                 &ioctlInfo,
                 &isComplete);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " NAND_SAMPLE: Number of sectors in nand is"
                            " %d", sectCnt);
    }
    else
    {
        LOG_printf(&trace, " NAND_SAMPLE: GET SECT MAX - Fails");
    }

    ioctlInfo.Cmd = PSP_BLK_GETREMSTAT;
    ioctlInfo.pData = (Ptr)&isRemovable;
    result = pDevOps->Blk_Ioctl(handle,
                 (Ptr)&nandRespInfo,
                 &ioctlInfo,
                 &isComplete);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " NAND_SAMPLE: Is Device removable? %d",isRemovable);
    }
    else
    {
        LOG_printf(&trace, " NAND_SAMPLE: GET REM STAT - Fails");
    }

    ioctlInfo.Cmd = PSP_BLK_SETEVENTQ;
    eventQueue = 0;
    ioctlInfo.pData = (Void *)&eventQueue;
    ioctlInfo.pData1 = (Void *)NULL;
    result = pDevOps->Blk_Ioctl(handle,
                 (Ptr)&nandRespInfo,
                 &ioctlInfo,
                 &isComplete);
    if (IOM_COMPLETED != result)
    {
        LOG_printf(&trace, " NAND_SAMPLE: SET EVENT Q - Fails");
    }
    return result;

}


/* This function will pass only if nand is already registered with media layer*/
static Void checkNandIoctl(Void)
{
    PSP_BlkDevIoctlInfo_t   ioctlInfo;
    Int32                   result;
    Uint32                  blockSize;
    Uint32                  sectCnt;
    Uint32                  eventQueue;
    PSP_NandOpMode              opmode;
    PSP_nandDeviceInfo      devInfo;

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_NAND_IOCTL_GET_NAND_SIZE);
    ioctlInfo.pData = (Ptr)&sectCnt;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_NAND, &ioctlInfo);
    if (result == IOM_COMPLETED)
    {
        LOG_printf(&trace, " NAND_SAMPLE: No of sectors in NAND is %d",sectCnt);
    }
    else
    {
        LOG_printf(&trace, " NAND_SAMPLE : Get No of sectors - Fails");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_NAND_IOCTL_GET_SECTOR_SIZE);
    ioctlInfo.pData = (Ptr)&blockSize;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_NAND, &ioctlInfo);
    if (result == IOM_COMPLETED)
    {
        LOG_printf(&trace, " NAND_SAMPLE: Blk/Sector size is %d",blockSize);
    }
    else
    {
        LOG_printf(&trace, " NAND_SAMPLE : Get Block/Sector size -  Fails");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_NAND_IOCTL_SET_EVENTQ);
    eventQueue = 1;
    ioctlInfo.pData = (Void *)&eventQueue;
    ioctlInfo.pData1 = (Void *)NULL;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_NAND, &ioctlInfo);
    if (result != IOM_COMPLETED)
    {
        LOG_printf(&trace, " NAND_SAMPLE : Error while setting the eventQ");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_NAND_IOCTL_GET_OPMODE);
    ioctlInfo.pData = (Ptr)&opmode;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_NAND, &ioctlInfo);
    if (result == IOM_COMPLETED)
    {
        LOG_printf(&trace, " NAND_SAMPLE: Opmode of nand is %d",opmode);
    }
    else
    {
        LOG_printf(&trace, " NAND_SAMPLE: Get Opmode - Fails");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_NAND_IOCTL_GET_DEVICE_INFO);
    ioctlInfo.pData = (Ptr)&devInfo;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_NAND, &ioctlInfo);
    if (result == IOM_COMPLETED)
    {
        LOG_printf(&trace, " NAND_SAMPLE: Vendor/Manufacturer/Maker ID of"
                            " NAND device is 0x%x", devInfo.vendorId);
        LOG_printf(&trace, " NAND_SAMPLE: Device ID of the NAND device is"
                            "  0x%x", devInfo.deviceId);
        LOG_printf(&trace, " NAND_SAMPLE: Size of one page is"
                            " %d", devInfo.pageSize);
        LOG_printf(&trace, " NAND_SAMPLE: Number of pages per block is"
                            " %d", devInfo.pagesPerBlock);
        LOG_printf(&trace, " NAND_SAMPLE: Number of blocks in the NAND device"
                            " is %d", devInfo.numBlocks);
        LOG_printf(&trace, " NAND_SAMPLE: Size of spare area of each page is"
                            "  %d", devInfo.spareAreaSize);
        LOG_printf(&trace, " NAND_SAMPLE: Data bus width of the NAND device is"
                            " %d", devInfo.dataBusWidth);
    }
    else
    {
        LOG_printf(&trace, " NAND_SAMPLE: Get Device info - Fails");
    }
}


Bool blkNandTestCallBack(Ptr handle, PSP_BlkDevRes_t const *pErrInfo)
{
    if ((handle == NULL) || (pErrInfo == NULL))
    {
        return FALSE;
    }

    SEM_post(nandCbSem);
    return TRUE;
}

static Void blkNandTest()
{
    PSP_BlkDevOps_t pDevOps1;
    PSP_BlkDevOps_t* pDevOps = &pDevOps1;
    Ptr handle;
    Int32 result;
    Uint32 cnt = 0,i = 0, loopCnt;
    PSP_BlkDrvIoctlInfo_t drvIoctlInfo;
    SEM_Attrs semAttr;
    PSP_BlkDrvId_t device = PSP_BLK_DRV_MMC0, driverDev;
    Uint32 sectorCount, sector_size;
    Uint32 loop = 0;
    Uint32 addr = 0;
    PSP_BlkDevIoctlInfo_t ioctlInfo;
    Bool isComplete;

    driverDev = (PSP_BlkDrvId_t)0;
    drvIoctlInfo.Cmd = PSP_BLK_DRV_GETRAWDEV;
    drvIoctlInfo.pData = (Ptr)&driverDev;
    result = PSP_blkmediaDrvIoctl((Ptr)&device, &drvIoctlInfo);
    LOG_printf(&trace, " NAND_SAMPLE: Currently %d device is set as a"
                        " Raw Device", driverDev);

    driverDev = PSP_BLK_DRV_NAND;
    /*To set NAND as a RAW device at runtime, call PSP_blkmediaDrvIoctl()
    function with PSP_BLK_DRV_SETRAWDEV as a command */
    drvIoctlInfo.Cmd = PSP_BLK_DRV_SETRAWDEV;
    drvIoctlInfo.pData = (Ptr)&driverDev;
    result = PSP_blkmediaDrvIoctl((Ptr)&device, &drvIoctlInfo);
    if (result == IOM_COMPLETED)
    {
        LOG_printf(&trace, " NAND_SAMPLE: Ioctl for setting nand device as"
                            " RAW device is successfull");
        driverDev = (PSP_BlkDrvId_t)0;
        drvIoctlInfo.Cmd = PSP_BLK_DRV_GETRAWDEV;
        drvIoctlInfo.pData = (Ptr)&driverDev;
        result = PSP_blkmediaDrvIoctl((Ptr)&device, &drvIoctlInfo);
        LOG_printf(&trace, " NAND_SAMPLE: Currently %d device is set as a"
                            " Raw Device", driverDev);
        /*The Media Driver clients like Mass Storage drivers shall use this
        function to register for a Block media device.*/
        result = PSP_blkmediaAppRegister(&blkNandTestCallBack,
                     &pDevOps,
                     &handle);
        if (result == IOM_COMPLETED)
        {
            LOG_printf(&trace, " NAND_SAMPLE: Media for remote client is"
                                " regsitered");
        }
        else
        {
            LOG_printf(&trace, " NAND_SAMPLE: Media for remote client"
                                " can not regsitered");
            return;
        }
        if ((NULL == handle) || (NULL == pDevOps))
        {
            LOG_printf(&trace, " NAND_SAMPLE: Invalid handle");
            return;
        }

        semAttr.name = (String)"Nand_Blk_Sema";
        nandCbSem = SEM_create(0U, &semAttr);
        if (nandCbSem == NULL)
        {
            LOG_printf(&trace, " NAND_SAMPLE: NAND_TestSem Create failed");
        }

        TSK_sleep(500U);

        ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                            PSP_NAND_IOCTL_GET_SECTOR_SIZE);
        ioctlInfo.pData = (Ptr)&sector_size;
        result = pDevOps->Blk_Ioctl(handle,
                     (Ptr)&nandRespInfo,
                     &ioctlInfo,
                     &isComplete);
        if (result != IOM_COMPLETED)
        {
            LOG_printf(&trace, "NAND_SAMPLE : Get Blk/Sector size - Fails");
        }

        checkBlockMediaIoctl(pDevOps, handle);
        checkNandIoctl();
        sectorCount = BYTE_COUNT/sector_size;

        loopCnt = 5U;

        while (loop < loopCnt)
        {
            cnt = BYTE_COUNT;
            for (i = 0; i < cnt; i++)
            {
                writeBuf[i] = loop + i + addr;
                readBuf[i] = 0;
            }

            result = pDevOps->Blk_Write(handle, (Ptr)&nandRespInfo,
                                        writeBuf, addr, sectorCount);
            if (result == IOM_PENDING)
            {
                SEM_pend(nandCbSem, SYS_FOREVER);
                LOG_printf(&trace, " NAND_SAMPLE: BLK write passed & loop"
                                    " count is %d ",loop);
            }
            else if (result != IOM_COMPLETED)
            {
                LOG_printf(&trace, " NAND_SAMPLE: Error returned from"
                                    " function Blk_Write while writing");
                addr = 0;
            }
            else
            {
                SEM_pend(nandCbSem, SYS_FOREVER);
                LOG_printf(&trace, " NAND_SAMPLE: BLK write passed & loop"
                                    " count is %d", loop);
            }

            result = pDevOps->Blk_Read(handle, (Ptr)&nandRespInfo,
                                       readBuf, addr, sectorCount);
            if (result == IOM_PENDING)
            {
                SEM_pend(nandCbSem, SYS_FOREVER);
                LOG_printf(&trace, " NAND_SAMPLE: BLK read passed & loop"
                                    " count is %d ",loop);
            }
            else if (result != IOM_COMPLETED)
            {
                LOG_printf(&trace, " NAND_SAMPLE:Error returned from"
                                    " function Blk_Read while reading");
                addr = 0;
            }
            else
            {
                SEM_pend(nandCbSem, SYS_FOREVER);
                LOG_printf(&trace, " NAND_SAMPLE: BLK read passed & loop"
                                    " count is %d", loop);
            }

            for (i = 0; i < (sectorCount * 512U); i++)
            {
                if (writeBuf[i] != readBuf[i])
                {
                    LOG_printf(&trace, " NAND_SAMPLE: BLK Data write-read"
                                        " matching failed at i = %d",i);
                    break;
                }
            }
            addr = addr + sectorCount;
            loop++;
        }
        /*The Media Driver clients like Mass Storage drivers shall use this
        function to un-register from a Block media device*/
        result = PSP_blkmediaAppUnRegister(handle);
        if (IOM_COMPLETED == result)
        {
            LOG_printf(&trace, " NAND_SAMPLE: Media for remote client"
                                " is unregsitered");
        }
        else
        {
            LOG_printf(&trace, " NAND_SAMPLE: Media for remote client"
                                " is not unregsitered");
        }
    }
}


/**
 *  \brief Demonstrates use of PSP drivers.
 */
Void runNandSample()
{
    Uint8 *tempSrcNandBuf = NULL;
    Uint8 *tempDstNandBuf = NULL;

    /* aligning srcNandBuf & dstNandBuf on CACHE line size boundary */
    memset(srcNandBufOrg, 0xFF, sizeof(srcNandBufOrg));
    memset(dstNandBufOrg, 0xFF, sizeof(dstNandBufOrg));

    tempSrcNandBuf = srcNandBufOrg;
    tempDstNandBuf = dstNandBufOrg;

    /* Aling the buffer on the cache line length */
    writeBuf = (Ptr)((Uint32)(tempSrcNandBuf + DATA_ALIGN - 1)
                                             & ~(DATA_ALIGN - 1));
    readBuf = (Ptr)((Uint32)(tempDstNandBuf + DATA_ALIGN - 1)
                                             & ~(DATA_ALIGN - 1));
    if ((NULL == writeBuf) || (NULL == readBuf))
    {
        LOG_printf(&trace, " NAND_SAMPLE:Buffer is NULL");
    }

#ifndef BUFFER_ALINED_CACHE_LENGTH
    writeBuf += 4;
    readBuf += 4;
#endif /* BUFFER_ALINED_CACHE_LENGTH */

    blkNandTest();
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
