/*
 * mmcsdSample_io.c
 *
 * This file contains the test / demo code to demonstrate the MMCSD driver func-
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

/** \file   mmcsdSample_io.c
 *
 *  \brief  Demo application for the MMCSD driver
 *
 *  This sample application, is a representative test program.
 *  This application test MMCSD driver on Bios.
 *
 *
 */

#include <std.h>
#include <hwi.h>
#include <tsk.h>
#include <iom.h>
#include <log.h>
#include <string.h>
#include <stdio.h>

#include "ti/pspiom/mmcsd/psp_mmcsd.h"
#include "ti/pspiom/blkmedia/psp_blkdev.h"

#define MMC_INST_ID                     0U
/*Instance no of mmcsd                                                        */

#define NUM_SECTORS                     6144U
/*No of sectors read or written                                               */

#define BYTE_COUNT                      (NUM_SECTORS * 512U)
/* No of bytes to write or read                                               */

#define INCLUDE_MEDIA_WP_TEST           0
/*This will enable the WP test on the media.                                  */

#define INCLUDE_ACTUAL_CARD_FREQ_IOCTL  0
/*This will actually alter the freq of the card.
Please note that the further operation of the card takes place in this freq*/

#define BUFFER_ALINED_CACHE_LENGTH
/* Enable this if user want to pass cache length aligned buffer, In polled mode
   buffer must be 4 byte aligned */

/*#define CARD_INS_REM_TEST*/
/* Enable this if user want to test card insertion removal test */

#define DATA_ALIGN                      128

/*
 * External references
 */
extern LOG_Obj    trace;

/*
* Global Static references
*/
static SEM_Handle mmcsdCbSem;
PSP_BlkDevRes_t mmcsdRespInfo;
Uint32 sectorCount = NUM_SECTORS;
/*
 * Note: Buffer alignement is required only when working in DMA Mode.
 */
static Uint8 srcmmcsdBufOrg[BYTE_COUNT + DATA_ALIGN];
/*+128 if need to make unalign for testing purpose*/
static Uint8 dstmmcsdBufOrg[BYTE_COUNT + DATA_ALIGN];
/*+128 if need to make unalign for testing purpose*/
static Uint8 *srcmmcsdBuf = NULL;
static Uint8 *dstmmcsdBuf = NULL;
Uint32 loop = 0;
Uint32 addr = 0;

Bool blkMmcsdTestCallBack(Ptr handle, PSP_BlkDevRes_t const *pErrInfo);
static Void blkMmcsdTest(Void);


#ifndef Mmcsd_GPIO_CDWP_ENABLE
static Void checkMediaCard(Uint32 instanceId);
#endif


#ifdef Mmcsd_GPIO_CDWP_ENABLE

#define MMCSD_CARD_DETECT_TASK_PRIO     8
#define MMCSD_CARD_DETECT_TASK_STK_SIZE 0x1000
volatile Int32 statuspin3 = 0;

SEM_Handle  mmcsdGpioSem;
SEM_Handle  mmcsdGpioAppSem;

Void mmcsdGpioInputIsr(Void)
{
    SEM_postBinary (mmcsdGpioSem);
}

static Void mmcsdGpioCardDetectMedia(Void)
{
    Int32 result = IOM_COMPLETED, flag = 1;
    PSP_MmcsdCardType   cardType;
    while (flag)
    {
        LOG_printf(&trace, " MMCSD_GPIO : MMC/SD Card has been either removed"
            " or inserted");
        TSK_sleep(1000u);
        result = PSP_mmcsdCheckCard(&cardType,MMC_INST_ID);
        TSK_sleep(1000u);
        if (IOM_COMPLETED == result)
        {
            if ((PSP_MMCSD_CARDTYPE_MMC == cardType)||
                (PSP_MMCSD_CARDTYPE_SD == cardType))
            {
                /*At GPIO2_1_PIN bit value of 1 at the input reg represents that
                the card is WP and 0 means it is not WP*/
                statuspin3 = *(volatile Uint32*)0x01E26048 & 0x00000002;
                if (statuspin3)
                {
                    LOG_printf(&trace, " MMCSD_GPIO : SD Card is WP by looking"
                        " at the notch on the card");
                }
                else
                {
                    LOG_printf(&trace, " MMCSD_GPIO : SD Card is not WP by"
                        " looking at the notch on the card");
                }
                SEM_post (mmcsdGpioAppSem);
            }
            else
            {
                LOG_printf(&trace, " MMCSD_GPIO : Please insert the card");
            }
        }
        else
        {
            LOG_printf(&trace, " MMCSD_GPIO : Please re-insert the card");
        }
        SEM_pendBinary(mmcsdGpioSem, SYS_FOREVER);
    }
}

static Void createCardDetectTask(Void)
{
    TSK_Handle testTaskId;
    SEM_Attrs semAttr, semAttr1;
    TSK_Attrs attrs0 = {MMCSD_CARD_DETECT_TASK_PRIO, NULL, 1024, NULL, NULL,
                        NULL, ' ', (1u)};

    semAttr.name = "Mmcsd_GpioCd_Sema";
    mmcsdGpioSem = SEM_create(0U, &semAttr);
    if (NULL == mmcsdGpioSem)
    {
        LOG_printf(&trace, " MMCSD_GPIO : sem create failed");
    }

    semAttr1.name = "Mmcsd_GpioApp_Sema";
    mmcsdGpioAppSem = SEM_create(0U, &semAttr1);
    if (NULL == mmcsdGpioAppSem)
    {
        LOG_printf(&trace, " MMCSD-GPIO : sem create failed");
    }

    attrs0.name = "Mmcsd_CD_Task";
    /*Create Card detect task*/
    testTaskId = TSK_create((Fxn)mmcsdGpioCardDetectMedia, &attrs0, NULL);
    if (NULL == testTaskId)
    {
        LOG_printf(&trace, " MMCSD-GPIO : Error creating task for gpio");
    }

}
#endif


#ifndef Mmcsd_GPIO_CDWP_ENABLE
static Void checkMediaCard(Uint32 instanceId)
{
    Int32               result = IOM_COMPLETED, flag = 1;
    PSP_MmcsdCardType   cardType = PSP_MMCSD_CARDTYPE_NONE;

    while (flag)
    {
        result = PSP_mmcsdCheckCard(&cardType, instanceId);
        TSK_sleep(1000u);
        if (IOM_COMPLETED == result)
        {
            if ((PSP_MMCSD_CARDTYPE_MMC == cardType)||
                (PSP_MMCSD_CARDTYPE_SD == cardType))
            {
                break;
            }
            else
            {
                LOG_printf(&trace, " MMCSD_SAMPLE: Please insert the card");
                TSK_sleep(1000U);
            }
        }
        else
        {
            LOG_printf(&trace, " MMCSD_SAMPLE: checkCard failed");
        }
    }
}
#endif




/* This function will pass only if card is already registered with media layer*/
static Int32 checkBlockMediaIoctl(PSP_BlkDevOps_t* pDevOps, Ptr handle)
{
    PSP_BlkDevIoctlInfo_t   ioctlInfo;
    Int32                   result;
    Uint32                  blockSize;
    Uint32                  sectCnt;
    PSP_BlkOpMode           opmode;
    Bool                    writeProtect;
    Bool                    isRemovable;
    Bool                    isComplete;
    PSP_MmcsdEdmaEventQueue eventQueue;

    ioctlInfo.Cmd = PSP_BLK_GETOPMODE;
    ioctlInfo.pData = (Ptr)&opmode;
    result = pDevOps->Blk_Ioctl(handle,
                 (Ptr)&mmcsdRespInfo,
                 &ioctlInfo,
                 &isComplete);
    if (IOM_COMPLETED != result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: GET OPMODE - Fails");
    }

    ioctlInfo.Cmd = PSP_BLK_GETBLKSIZE;
    ioctlInfo.pData = (Ptr)&blockSize;
    result = pDevOps->Blk_Ioctl(handle,
                 (Ptr)&mmcsdRespInfo,
                 &ioctlInfo,
                 &isComplete);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Block size is %d", blockSize);
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: GET BLK SIZE - Fails");
    }

    ioctlInfo.Cmd = PSP_BLK_GETSECTMAX;
    ioctlInfo.pData = (Ptr)&sectCnt;
    result = pDevOps->Blk_Ioctl(handle,
                 (Ptr)&mmcsdRespInfo,
                 &ioctlInfo,
                 &isComplete);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: No. of sectors is %d", sectCnt);
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: GET SECT MAX - Fails");
    }

    ioctlInfo.Cmd = PSP_BLK_GETWPSTAT;
    ioctlInfo.pData = (Ptr)&writeProtect;
    result = pDevOps->Blk_Ioctl(handle,
                 (Ptr)&mmcsdRespInfo,
                 &ioctlInfo,
                 &isComplete);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: WP status of card? %d",writeProtect);
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: GET WP STAT - Fails");
    }

    ioctlInfo.Cmd = PSP_BLK_GETREMSTAT;
    ioctlInfo.pData = (Ptr)&isRemovable;
    result = pDevOps->Blk_Ioctl(handle,
                 (Ptr)&mmcsdRespInfo,
                 &ioctlInfo,
                 &isComplete);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Device removable? %d", isRemovable);
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: GET REM STAT - Fails");
    }

    ioctlInfo.Cmd = PSP_BLK_SETEVENTQ;
    eventQueue = PSP_MMCSD_EDMA3_EVENTQ_0;
    ioctlInfo.pData = (Void *)&eventQueue;
    ioctlInfo.pData1 = (Void *)NULL;
    result = pDevOps->Blk_Ioctl(handle,
                 (Ptr)&mmcsdRespInfo,
                 &ioctlInfo,
                 &isComplete);
    if (IOM_COMPLETED != result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: SET EVENT Q  - Fails");
    }
    return result;

}


/* This function will pass only if card is already registered with media layer*/
static Void checkMmcsdIoctl(Void)
{
    PSP_BlkDevIoctlInfo_t   ioctlInfo;
    Int32                   result;
    Uint32                  blockSize;
    Uint32                  sectCnt;
    PSP_MmcsdCardType       cardType;
    PSP_MmcsdCardRegs       cardRegs;
    Bool                    isHighcapacityCard;
    Bool                    tempWriteProtect = TRUE;
    PSP_MmcsdEdmaEventQueue eventQueue;
    Uint32                  cardFreq;
    Uint32                  vendorId;
    PSP_BlkOpMode           opmode;
    Uint32                  offset;
    Uint32                  value;

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_CHECK_CARD);
    ioctlInfo.pData = (Ptr)&cardType;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: CardType is %d",cardType);
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE : Check card - Fails");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_GET_BLOCKSIZE);
    ioctlInfo.pData = (Ptr)&blockSize;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Block size is %d",blockSize);
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE : Get Block size - Fails");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_GET_CARDREGS);
    ioctlInfo.pData = (Ptr)&cardRegs;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED != result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Read Card reg - Fails");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_GET_CARD_SIZE);
    ioctlInfo.pData = (Ptr)&sectCnt;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (result != IOM_COMPLETED)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Get Card Size - Fails");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                      PSP_MMCSD_IOCTL_CHECK_HIGH_CAPACITY_CARD);
    ioctlInfo.pData = (Ptr)&isHighcapacityCard;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED == result)
    {
        if (0 == isHighcapacityCard)
        {
            LOG_printf(&trace, " MMCSD_SAMPLE: Size of card in bytes is"
                                " %d", sectCnt);
            LOG_printf(&trace, " MMCSD_SAMPLE: This is not High capacity card");
        }
        else
        {
            LOG_printf(&trace, " MMCSD_SAMPLE: Size of card in Kbytes is"
                                " %d", sectCnt);
            LOG_printf(&trace, " MMCSD_SAMPLE: This is High capacity card");
        }
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Check High Capacity - Fails");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                      PSP_MMCSD_IOCTL_GET_TOTAL_SECTORS);
    ioctlInfo.pData = (Ptr)&sectCnt;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: No of sectors in card is"
                            " %d", sectCnt);
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Get total Sectors - Fails");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_GET_TEMPORARY_WP);
    ioctlInfo.pData = (Ptr)&tempWriteProtect;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Temporary write protect of card"
                            " is %d", tempWriteProtect);
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Get Temporary write protect - Fail");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_GET_PERMANENT_WP);
    ioctlInfo.pData = (Ptr)&tempWriteProtect;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Permanent write protect of card"
                            " is %d", tempWriteProtect);
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Get Permanent write protect - Fail");
    }

#if (INCLUDE_MEDIA_WP_TEST)
    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_SET_TEMPORARY_WP);
    tempWriteProtect = 1;
    ioctlInfo.pData = (Ptr)&tempWriteProtect;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Temporary write protect of card"
                            " is set");
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Set Temporary write protect - Fail");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_GET_TEMPORARY_WP);
    ioctlInfo.pData = (Ptr)&tempWriteProtect;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Temporary write protect of card"
                            " is %d", tempWriteProtect);
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Get Temporary write protect - Fail");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_SET_TEMPORARY_WP);
    tempWriteProtect = 0;
    ioctlInfo.pData = (Ptr)&tempWriteProtect;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Temporary write protect of card"
                            " is removed");
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Set Temporary WP removal - Fails");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_GET_TEMPORARY_WP);
    ioctlInfo.pData = (Ptr)&tempWriteProtect;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Temporary write protect of card"
                            " is %d", tempWriteProtect);
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Get Temporary WP - Fails");
    }
#endif

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_GET_OPMODE);
    ioctlInfo.pData = (Ptr)&opmode;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Operation mode is  %d",opmode);
    }
    else
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Get Opmode - Fails");
    }

    if (PSP_OPMODE_DMAINTERRUPT == opmode)
    {

        ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                            PSP_MMCSD_IOCTL_SET_EVENTQ);
        eventQueue=PSP_MMCSD_EDMA3_EVENTQ_1;
        ioctlInfo.pData = (Void *)&eventQueue;
        ioctlInfo.pData1 = (Void *)NULL;
        result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
        if (IOM_COMPLETED != result)
        {
            LOG_printf(&trace, " MMCSD_SAMPLE: Error while setting the eventQ");
        }
    }
    cardFreq = 55000000;
    /*This is invalid freq value. This ioctl alters the freq at which the card
    is currently communicating. So this ioctl is tested with invalid value
    For valid case see the #if INCLUDE_ACTUAL_CARD_FREQ_IOCTL macro below.
    But this will alter the freq of
    card and it is left up to the user to use this ioctl with caution*/
    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_SET_CARD_FREQUENCY);
    ioctlInfo.pData = (Ptr)&cardFreq;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED == result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Set Card Freq - Fails");
    }

#if (INCLUDE_ACTUAL_CARD_FREQ_IOCTL)
    if (PSP_MMCSD_CARDTYPE_MMC == cardType)
    {
        cardFreq = 20000000;
    }
    if (PSP_MMCSD_CARDTYPE_SD == cardType)
    {
        cardFreq = 25000000;
    }
    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_SET_CARD_FREQUENCY);
    ioctlInfo.pData = (Ptr)&cardFreq;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED != result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Set Card Freq - Fails");
    }
#endif

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_GET_CARD_VENDOR);
    ioctlInfo.pData = (Ptr)&vendorId;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED != result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Get Vendor Id - Fails");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_GET_CONTROLLER_REG);
    offset = 0x14;/*14h - MMCTOR - MMC Response Time-Out Register*/
    ioctlInfo.pData = (Ptr)&offset;
    ioctlInfo.pData1 = (Ptr)&value;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED != result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Get controller regs - Fails");
    }

    ioctlInfo.Cmd = (PSP_BlkDevIoctl_t)(PSP_BLK_IOCTL_MAX +
                                        PSP_MMCSD_IOCTL_SET_CONTROLLER_REG);
    offset = 0x14;/*14h - MMCTOR - MMC Response Time-Out Register*/
    ioctlInfo.pData = (Ptr)&offset;
    ioctlInfo.pData1 = (Ptr)&value;
    result = PSP_blkmediaDevIoctl(PSP_BLK_DRV_MMC0, &ioctlInfo);
    if (IOM_COMPLETED != result)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Set controller regs - Fails");
    }
}


Bool blkMmcsdTestCallBack(Ptr handle, PSP_BlkDevRes_t const *pErrInfo)
{
    if ((handle == NULL) || (pErrInfo == NULL))
    {
        return FALSE;
    }
    SEM_post(mmcsdCbSem);
    return TRUE;
}

static Void blkMmcsdTest()
{
    PSP_BlkDevOps_t pDevOps1;
    PSP_BlkDevOps_t* pDevOps = &pDevOps1;
    Ptr handle;
    Int32 result;
    Uint32 cnt = 0,i = 0, loopCnt;
    PSP_BlkDrvIoctlInfo_t drvIoctlInfo;
    SEM_Attrs semAttr;
    PSP_BlkDrvId_t device = PSP_BLK_DRV_MMC0, driverDev;

    driverDev = (PSP_BlkDrvId_t)0;
    drvIoctlInfo.Cmd = PSP_BLK_DRV_GETRAWDEV;
    drvIoctlInfo.pData = (Ptr)&driverDev;
    result = PSP_blkmediaDrvIoctl((Ptr)&device, &drvIoctlInfo);
    LOG_printf(&trace, " MMCSD_SAMPLE: Currently %d device is set as a"
                        " Raw Device", driverDev);

    driverDev = PSP_BLK_DRV_MMC0;
    /*To set MMCSD as a RAW device at runtime, call PSP_blkmediaDrvIoctl()
    function with PSP_BLK_DRV_SETRAWDEV as a command */
    drvIoctlInfo.Cmd = PSP_BLK_DRV_SETRAWDEV;
    drvIoctlInfo.pData = (Ptr)&driverDev;
    result = PSP_blkmediaDrvIoctl((Ptr)&device, &drvIoctlInfo);
    if (result == IOM_COMPLETED)
    {
        LOG_printf(&trace, " MMCSD_SAMPLE: Ioctl for setting mmcsd device"
                            " as RAW device is successfull");
        driverDev = (PSP_BlkDrvId_t)0;
        drvIoctlInfo.Cmd = PSP_BLK_DRV_GETRAWDEV;
        drvIoctlInfo.pData = (Ptr)&driverDev;
        result = PSP_blkmediaDrvIoctl((Ptr)&device, &drvIoctlInfo);
        LOG_printf(&trace, " MMCSD_SAMPLE: Currently %d device is set as"
                            " a Raw Device", driverDev);
        /*The Media Driver clients like Mass Storage drivers shall use this
        function to register for a Block media device.*/
        result = PSP_blkmediaAppRegister(&blkMmcsdTestCallBack,
                     &pDevOps,
                     &handle);
        if (IOM_COMPLETED == result)
        {
            LOG_printf(&trace, " MMCSD_SAMPLE: Media for remote client"
                                " is regsitered");
        }
        else
        {
            LOG_printf(&trace, " MMCSD_SAMPLE: Media for remote client"
                                " can not regsitered");
            return;
        }
        if ((NULL == handle) || (NULL == pDevOps))
        {
            LOG_printf(&trace, " MMCSD_SAMPLE: Invalid handle");
            return;
        }

        semAttr.name = (String)"Mmcsd_Blk_Sema";
        mmcsdCbSem = SEM_create(0U, &semAttr);
        if (NULL == mmcsdCbSem)
        {
            LOG_printf(&trace, " MMCSD_SAMPLE: Sem mmcsdCbSem  Create failed");
        }

        TSK_sleep(500U);
        checkBlockMediaIoctl(pDevOps, handle);
        checkMmcsdIoctl();
        cnt = sectorCount * 512U;

#ifdef CARD_INS_REM_TEST
        loopCnt = 200U;
#else
        loopCnt = 5U;
#endif

        while (loop < loopCnt)
        {
            for (i = 0; i < cnt; i++)
            {
                srcmmcsdBuf[i] = i + addr + (loop*1000);
                dstmmcsdBuf[i] = 0;
            }

            result = pDevOps->Blk_Write(handle, (Ptr)&mmcsdRespInfo,
                                        srcmmcsdBuf, addr, sectorCount);
            if (result == IOM_PENDING)
            {
                SEM_pend(mmcsdCbSem, SYS_FOREVER);
                LOG_printf(&trace, " MMCSD_SAMPLE: BLK write passed & loop"
                                    " count is %d ",loop);
            }
            else if (result != IOM_COMPLETED)
            {
                LOG_printf(&trace, " MMCSD_SAMPLE: Error returned from"
                                    " function Blk_Write while writing");
#ifdef Mmcsd_GPIO_CDWP_ENABLE
                SEM_pend(mmcsdGpioAppSem, SYS_FOREVER);
#else
                checkMediaCard(MMC_INST_ID);
#endif
                addr = 0;
            }
            else
            {
                SEM_pend(mmcsdCbSem, SYS_FOREVER);
                LOG_printf(&trace, " MMCSD_SAMPLE: BLK write passed & loop"
                                    " count is %d", loop);
            }

            result = pDevOps->Blk_Read(handle, (Ptr)&mmcsdRespInfo,
                                       dstmmcsdBuf, addr, sectorCount);
            if (result == IOM_PENDING)
            {
                SEM_pend(mmcsdCbSem, SYS_FOREVER);
                LOG_printf(&trace, " MMCSD_SAMPLE: BLK read passed & loop"
                                    " count is %d ",loop);
            }
            else if (result != IOM_COMPLETED)
            {
                LOG_printf(&trace, " MMCSD_SAMPLE:Error returned from"
                                    " function Blk_Read while reading");
#ifdef Mmcsd_GPIO_CDWP_ENABLE
                SEM_pend(mmcsdGpioAppSem, SYS_FOREVER);
#else
                checkMediaCard(MMC_INST_ID);
#endif
                addr = 0;
            }
            else
            {
                SEM_pend(mmcsdCbSem, SYS_FOREVER);
                LOG_printf(&trace, " MMCSD_SAMPLE: BLK read passed & loop"
                                    " count is %d",loop);
            }

            for (i = 0; i < (sectorCount * 512U); i++)
            {
                if (srcmmcsdBuf[i] != dstmmcsdBuf[i])
                {
                    LOG_printf(&trace, " MMCSD_SAMPLE: BLK Data write-read"
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
            LOG_printf(&trace, " MMCSD_SAMPLE: Media for remote client is"
                                " unregsitered");
        }
        else
        {
            LOG_printf(&trace, " MMCSD_SAMPLE: Media for remote client is"
                                " not unregsitered");
        }
    }
}


/**
 *  \brief Demonstrates use of PSP drivers.
 */
Void runMmcsdSample()
{
    Uint8 *tempSrcMmcsdBuf = NULL;
    Uint8 *tempDstMmcsdBuf = NULL;

    /* aligning srcmmcsdBuf & dstmmcsdBuf on CACHE line size boundary */
    memset(srcmmcsdBufOrg, 0xFF, sizeof(srcmmcsdBufOrg));
    memset(dstmmcsdBufOrg, 0xFF, sizeof(dstmmcsdBufOrg));

    tempSrcMmcsdBuf = srcmmcsdBufOrg;
    tempDstMmcsdBuf = dstmmcsdBufOrg;

    /* Aling the buffer on the cache line length */
    srcmmcsdBuf = (Ptr)((Uint32)(tempSrcMmcsdBuf + DATA_ALIGN - 1)
                                             & ~(DATA_ALIGN - 1));
    dstmmcsdBuf = (Ptr)((Uint32)(tempDstMmcsdBuf + DATA_ALIGN - 1)
                                             & ~(DATA_ALIGN - 1));
    if ((NULL == srcmmcsdBuf) || (NULL == dstmmcsdBuf))
    {
        LOG_printf(&trace, " MMCSD_SAMPLE:Buffer is NULL");
    }

	LOG_printf(&trace, " MMCSD_SAMPLE:Please insert the card");
#ifdef Mmcsd_GPIO_CDWP_ENABLE
    createCardDetectTask();
    SEM_pend(mmcsdGpioAppSem, SYS_FOREVER);
#else
    checkMediaCard(MMC_INST_ID);
#endif
	LOG_printf(&trace, " MMCSD_SAMPLE:Card is inserted");

#ifndef BUFFER_ALINED_CACHE_LENGTH
    srcmmcsdBuf += 4;
    dstmmcsdBuf += 4;
#endif /* BUFFER_ALINED_CACHE_LENGTH */

    blkMmcsdTest();
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
