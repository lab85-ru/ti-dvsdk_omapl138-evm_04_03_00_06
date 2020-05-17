/*
 * dda_mmcsdBios.c
 *
 * This file contains MMCSD media layer implementation. This file implements the
 * Device Driver Adaptation layer functionality
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

/** \file   dda_mmcsdBios.c
 *
 *  \brief  MMCSD media layer implementation
 *
 *  This file implements the DDA functionality
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include <assert.h>
#include <iom.h>
#include <string.h>

#include "dda_mmcsdBios.h"
#include "ddc_mmcsd.h"

#ifdef MMCSD_INSTRUMENTATION_ENABLED
#include    <ti/sdo/pspdrivers/pal_os/bios/psp_log.h>
#endif /* MMCSD_INSTRUMENTATION_ENABLED */
/* ========================================================================== */
/*                            STATIC GLOBAL VARIABLES                         */
/* ========================================================================== */
/**\brief PSP object Handle*/
static PSP_MmcsdHandle      gMmcsdHandle;
/**\brief block media callback Semaphore*/
static SEM_Handle           gMmcsdCallbackSem;
/**\brief error is there or not*/
static Int                  gMmcsdDetectError;
/**\brief DDA bios object Handle*/
static DDA_MmcsdObj         gMmcsdDdaObj[PSP_MMCSD_NUM_INSTANCES];

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
static Int32 mmcsdMediaHwEventNotification0(PSP_MmcsdHwEvent hwEvent, Ptr arg);

Int32 mmcsdMediaCallback0(Ptr handle, Int32 count);

Int32 mmcsdDrvRegMedia0(PSP_BlkDevIdent_t    *pIdent,
                        PSP_blkDevCallback_t blockDevCb,
                        PSP_BlkDevOps_t      **pDevOps,
                        Ptr                  *pHandle);
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
static Int32 mmcsdMediaHwEventNotification1(PSP_MmcsdHwEvent hwEvent, Ptr arg);

Int32 mmcsdMediaCallback1(Ptr handle, Int32 count);

Int32 mmcsdDrvRegMedia1(PSP_BlkDevIdent_t    *pIdent,
                        PSP_blkDevCallback_t blockDevCb,
                        PSP_BlkDevOps_t      **pDevOps,
                        Ptr                  *pHandle);
#endif

static Int32 mmcsdDrvRegMedia(PSP_BlkDevIdent_t *        pIdent,
                              PSP_blkDevCallback_t       blockDevCb,
                              PSP_BlkDevOps_t            **pDevOps,
                              Ptr * const                pHandle,
                              Uint32                     instanceId);

Int32 mmcsdDrvIoctl(Ptr handle,
                    Ptr mediaHandle,
                    PSP_BlkDevIoctlInfo_t const * pIoctlInfo,
                    Bool *pIsComplete);

Int32 mmcsdRead(Ptr handle,
                Ptr mediaHandle,
                Ptr buffer,
                Uint32 sectNum,
                Uint32 numSect);

Int32 mmcsdWrite(Ptr handle,
                 Ptr mediaHandle,
                 Ptr buffer,
                 Uint32 sectNum,
                 Uint32 numSect);

Void mmcsdDetectCallback(Ptr const param);

/* ========================================================================== */
/*                         LOCAL FUNCTION DEFINTIONS                          */
/* ========================================================================== */
#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
/**\brief event notification for any H/W error*/
static Int32 mmcsdMediaHwEventNotification0(PSP_MmcsdHwEvent hwEvent, Ptr arg)
{
    PSP_BlkDevEvent_t blkEventInfo;
    Uint32 intState;
    arg = arg;      /* to remove compilation warning */
    switch (hwEvent)
    {
        case PSP_MMCSD_HWEVENT_CARD_REMOVED:
            MMCSD_DRV_DEBUG(("\r\nMMCSD : mmcsdMediaHwEventNotification0 -"
                             " CardRemoved"));
            intState = HWI_disable();
            gMmcsdDdaObj[MMCSD_INST_ID0].cardmediaType = \
                PSP_MMCSD_CARDTYPE_NONE;
            HWI_restore(intState);
            blkEventInfo.EventId = PSP_BLK_EVT_MEDREM;
            blkEventInfo.EvtData = &gMmcsdDdaObj[MMCSD_INST_ID0].blkDrvInfo;
            /*Used for propagating events from the MMCSD0 driver to the block
            driver independent of the device context (Ex. Device insertion).*/
            PSP_blkmediaCallback(PSP_BLK_DRV_MMC0, &blkEventInfo);
            break;

        case PSP_MMCSD_HWEVENT_DATA_CORRUPTED:
            MMCSD_DRV_DEBUG(("\r\nMMCSD : mmcsdMediaHwEventNotification0 -"
                             " DATA CRC Error"));
            break;

        case PSP_MMCSD_HWEVENT_COMMAND_CORRUPTED:
            MMCSD_DRV_DEBUG(("\r\nMMCSD : mmcsdMediaHwEventNotification0 -"
                             " Command CRC Error"));
            break;

        case PSP_MMCSD_HWEVENT_ERROR_CARD_STATUS:
            MMCSD_DRV_DEBUG(("\r\nMMCSD : mmcsdMediaHwEventNotification0 -"
                             " Error"));
            break;

        default:
            /*print error*/
            MMCSD_ERR_DEBUG(("\r\n Invalid err in MediaHwEventNotification0"));
    }
    return IOM_COMPLETED;
}

/**\brief Callback from dda for instance 0*/
Int32 mmcsdMediaCallback0(Ptr handle, Int32 count)
{
    PSP_BlkDevRes_t pRes = {0, };
    Int32 result = IOM_COMPLETED;

    assert(NULL != handle);

    if (count != 0)
    {
        pRes.ReqInfo.Status.Error = IOM_COMPLETED;
        pRes.IsEvent = FALSE;
    }
    else
    {
        pRes.ReqInfo.Status.Error = IOM_EBADARGS;
        pRes.IsEvent = FALSE;
    }
    /*After completion of every read/write call driver calls
    blkmediaCallback() function to return error value and to notify block
    media driver and in turn file system that read/write has been completed.
    Storage driver has to pass Media Driver Handle of device, which was
    received in read/write call from Block media driver as a first parameter
    and pointer to error information structure (PSP_BlkDevRes_t *) as a
    second parameter by setting error value which was generated during
    read/write function call.*/
    gMmcsdDdaObj[MMCSD_INST_ID0].blkDrvInfo.devCb(handle, &pRes);

    return result;
}

/**\brief The function mmcsdDrvRegMedia0 will be called from the block
media once mmcsd instance 0 is registered with the file system*/
Int32 mmcsdDrvRegMedia0(PSP_BlkDevIdent_t    *pIdent,
                        PSP_blkDevCallback_t blockDevCb,
                        PSP_BlkDevOps_t      **pDevOps,
                        Ptr                  *pHandle)
{
    return mmcsdDrvRegMedia(pIdent,
               blockDevCb,
               pDevOps,
               pHandle,
               (Uint32)MMCSD_INST_ID0);
}
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
/**\brief event notification for any H/W error*/
static Int32 mmcsdMediaHwEventNotification1(PSP_MmcsdHwEvent hwEvent, Ptr arg)
{
    PSP_BlkDevEvent_t blkEventInfo;
    Uint32 intState;
    arg = arg;      /* to remove compilation warning */
    switch (hwEvent)
    {
        case PSP_MMCSD_HWEVENT_CARD_REMOVED:
            MMCSD_DRV_DEBUG(("\r\nMMCSD : mmcsdMediaHwEventNotification0 -"
                             " CardRemoved"));
            intState = HWI_disable();
            gMmcsdDdaObj[MMCSD_INST_ID1].cardmediaType = \
                PSP_MMCSD_CARDTYPE_NONE;
            HWI_restore(intState);
            blkEventInfo.EventId = PSP_BLK_EVT_MEDREM;
            blkEventInfo.EvtData = &gMmcsdDdaObj[MMCSD_INST_ID1].blkDrvInfo;
            /*Used for propagating events from the MMCSD0 driver to the block
            driver independent of the device context (Ex. Device insertion).*/
            PSP_blkmediaCallback(PSP_BLK_DRV_MMC1, &blkEventInfo);
            break;

        case PSP_MMCSD_HWEVENT_DATA_CORRUPTED:
            MMCSD_DRV_DEBUG(("\r\nMMCSD : mmcsdMediaHwEventNotification0 -"
                             " DATA CRC Error"));
            break;

        case PSP_MMCSD_HWEVENT_COMMAND_CORRUPTED:
            MMCSD_DRV_DEBUG(("\r\nMMCSD : mmcsdMediaHwEventNotification0 -"
                             " Command CRC Error"));
            break;

        case PSP_MMCSD_HWEVENT_ERROR_CARD_STATUS:
            MMCSD_DRV_DEBUG(("\r\nMMCSD : mmcsdMediaHwEventNotification0 -"
                             " Error"));
            break;

        default:
            /*print error*/
            MMCSD_ERR_DEBUG(("\r\n Invalid err in MediaHwEventNotification0"));
    }
    return IOM_COMPLETED;
}

/**\brief Callback from dda for instance 1*/
Int32 mmcsdMediaCallback1(Ptr handle, Int32 count)
{
    PSP_BlkDevRes_t pRes = {0, };
    Int32 result = IOM_COMPLETED;

    assert(NULL != handle);

    if (count != 0)
    {
        pRes.ReqInfo.Status.Error = IOM_COMPLETED;
        pRes.IsEvent = FALSE;
    }
    else
    {
        pRes.ReqInfo.Status.Error = IOM_EBADARGS;
        pRes.IsEvent = FALSE;
    }
    /*After completion of every read/write call driver calls
    blkmediaCallback() function to return error value and to notify block
    media driver and in turn file system that read/write has been completed.
    Storage driver has to pass Media Driver Handle of device, which was
    received in read/write call from Block media driver as a first parameter
    and pointer to error information structure (PSP_BlkDevRes_t *) as a
    second parameter by setting error value which was generated during
    read/write function call.*/
    gMmcsdDdaObj[MMCSD_INST_ID1].blkDrvInfo.devCb(handle, &pRes);

    return result;
}

/**\brief The function mmcsdDrvRegMedia1 will be called from the block
media once mmcsd instance 1 is registered with the file system*/
Int32 mmcsdDrvRegMedia1(PSP_BlkDevIdent_t    *pIdent,
                        PSP_blkDevCallback_t blockDevCb,
                        PSP_BlkDevOps_t      **pDevOps,
                        Ptr                  *pHandle)
{
    return mmcsdDrvRegMedia(pIdent,
               blockDevCb,
               pDevOps,
               pHandle,
               (Uint32)MMCSD_INST_ID1);
}
#endif



/**
 *  \brief mmcsdDrvRegMedia ()  : Register for the block device.
 *
 *  \param pIdent       : Identification information. Not used can be NULL.
 *  \param blockDevCb   : Callback routine for the registered block Device.
 *  \param pDevOps      : Block Device Interface Operations.
 *  \param pHandle      : Handle to device (to be used for future transactions).
 *  \param instanceId   : Instance number of the MMCSD controller
 *
 *  \return Int32       : Success/Error
 ******************************************************************************/
Int32 mmcsdDrvRegMedia(PSP_BlkDevIdent_t * pIdent,
                       PSP_blkDevCallback_t blockDevCb,
                       PSP_BlkDevOps_t **pDevOps,
                       Ptr * const pHandle,
                       Uint32 instanceId)
{
    Int32 result = IOM_COMPLETED;

    assert(((NULL != pDevOps) && (NULL != blockDevCb)) && (NULL != pHandle));
    pIdent = pIdent;      /* to remove compilation warning */

    gMmcsdDdaObj[instanceId].blkDrvInfo.handle =\
        gMmcsdDdaObj[instanceId].ddaChannelHandle;
    if (gMmcsdDdaObj[instanceId].cardmediaType != PSP_MMCSD_CARDTYPE_NONE)
    {
        result = PSP_mmcsdStatus(gMmcsdDdaObj[instanceId].ddaChannelHandle,
                     PSP_MMCSD_IOCTL_START,
                     NULL,
                     NULL);
    }
    else
    {
        MMCSD_DRV_DEBUG(("\r\n MMCSD : Please insert the card"));
    }

    if (IOM_COMPLETED != result)
    {
        MMCSD_ERR_DEBUG(("\r\n MMCSD : Device Initialization failed"));
    }
    else
    {
        if (gMmcsdDdaObj[instanceId].cardmediaType != PSP_MMCSD_CARDTYPE_NONE)
        {
            MMCSD_DRV_DEBUG(("\r\nMMCSD : Device Initialized successfully"));
        }
    }

    *pDevOps = &(gMmcsdDdaObj[instanceId].blkDrvInfo.devOps);
    *pHandle = &gMmcsdDdaObj[instanceId].blkDrvInfo;

    /*Save the CB function which should be called after completion of every
    read/write call to notify block media driver and in turn file system that
    read/write has been completed.*/
    gMmcsdDdaObj[instanceId].blkDrvInfo.devCb = blockDevCb;

    return result;
}


/**
 *   \brief Block Device IOCTL
 *
 *   \param handle       Handle of the Block Device
 *   \param mediaHandle        Job Handle from this module
 *   \param pIoctlInfo   IOCTL information
 *   \param pIsComplete  Is the IOCTL complete
 *
 *  \return Int32       : Success/Error
 ******************************************************************************/
Int32 mmcsdDrvIoctl(Ptr handle,
                    Ptr mediaHandle,
                    PSP_BlkDevIoctlInfo_t const * pIoctlInfo,
                    Bool *pIsComplete)
{
    DDA_MmcsdBlkDrvInfo *pDevice;
    Uint32              *ptrData;
    Int32               retVal = IOM_COMPLETED;
    Uint32              *pData1;
    Uint32              ioctlCmd;

    assert(((NULL != handle) && (NULL != mediaHandle)) &&
           ((NULL != pIoctlInfo) && (NULL != pIsComplete)));

    assert(NULL != pIoctlInfo->pData);
    ioctlCmd = (Uint32)pIoctlInfo->Cmd;

    if (((PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_SET_CONTROLLER_REG) == \
            ioctlCmd) ||
        (PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_GET_CONTROLLER_REG == \
            ioctlCmd))
    {
        /* For the get and set controller registers, we would require the data
        value ensure that its supplied                                       */
        assert(NULL != pIoctlInfo->pData1);
    }

    pDevice = (DDA_MmcsdBlkDrvInfo *)handle;
    ptrData = (Uint32 *)(Uint32)pIoctlInfo->pData;

    *pIsComplete = TRUE;
    switch (ioctlCmd)
    {
        /*Get total number of sectors.*/
        case PSP_BLK_GETSECTMAX:
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_GET_TOTAL_SECTORS,
                         ptrData,
                         NULL);
            if (retVal != IOM_COMPLETED)
            {
                MMCSD_ERR_DEBUG(("\r\nMMCSD :Error while finding total number"
                                 " of sectors"));
            }
            break;

        case PSP_BLK_GETBLKSIZE:
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_GET_BLOCKSIZE,
                         ptrData,
                         NULL);
            if (retVal != IOM_COMPLETED)
            {
                MMCSD_ERR_DEBUG(("\r\nMMCSD :Error while finding sector size"));
            }
            break;

        case PSP_BLK_GETWPSTAT:
            {
                Bool tempWP = 0;
                Bool perWP = 0;
                retVal = PSP_mmcsdStatus(pDevice->handle,
                             PSP_MMCSD_IOCTL_GET_TEMPORARY_WP,
                             &tempWP,
                             NULL);
                if (IOM_COMPLETED == retVal)
                {
                    retVal = PSP_mmcsdStatus(pDevice->handle,
                                 PSP_MMCSD_IOCTL_GET_PERMANENT_WP,
                                 &perWP,
                                 NULL);
                }

                if ((TRUE == tempWP) || (TRUE == perWP))
                {
                    *ptrData = 1U;
                }
                else
                {
                    *ptrData = 0;
                }
            }
            break;

        case PSP_BLK_SETOPMODE:
            /*Since MMCSD does not provide the capability to change the
            operating mode the driver returns success assuming whatever set
            during init as fine*/
            retVal = IOM_COMPLETED;
            break;

        case PSP_BLK_GETOPMODE:
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_GET_OPMODE,
                         ptrData,
                         NULL);
            break;

        case PSP_BLK_GETREMSTAT:
            /* Removable Media */
            *(Bool *)ptrData = TRUE;
            retVal = IOM_COMPLETED;
            break;

        case PSP_BLK_SETEVENTQ:
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_SET_EVENTQ,
                         ptrData,
                         NULL);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_GET_BLOCKSIZE):
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_GET_BLOCKSIZE,
                         ptrData,
                         NULL);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_GET_CARD_SIZE):
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_GET_CARD_SIZE,
                         ptrData,
                         NULL);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_CHECK_CARD):
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_CHECK_CARD,
                         ptrData,
                         NULL);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_START):
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_START,
                         NULL,
                         NULL);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_GET_OPMODE):
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_GET_OPMODE,
                         ptrData,
                         NULL);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_SET_TEMPORARY_WP):
            {
                PSP_BlkDevRes_t pRes = {0, };
                /*Should be false. Block media works as ASYNC if set TRUE which
                is the case for MSC class*/
                pRes.IsEvent = FALSE;
                /*This tells the blk media to wait for cb */
                *pIsComplete = FALSE;
                retVal = PSP_mmcsdStatus(pDevice->handle,
                             PSP_MMCSD_IOCTL_SET_TEMPORARY_WP,
                             ptrData,
                             NULL);
                if (IOM_COMPLETED == retVal)
                {
                    if (1U == (*ptrData))
                    {
                        pRes.ReqInfo.Event.EventId = PSP_BLK_EVT_MEDWP;
                    }
                    else
                    {
                        pRes.ReqInfo.Event.EventId = PSP_BLK_EVT_MEDRW;
                    }
                    /*TRICKY: Should always be after the setting of event id
                      variable as ReqInfo is a union*/
                    pRes.ReqInfo.Status.Error = IOM_COMPLETED;
                }
                else
                {
                    pRes.ReqInfo.Status.Error = IOM_EBADARGS;
                }
                /*Tells the blk media about the CURRENT WP/RW status of the
                device*/
                gMmcsdDdaObj[((DDC_MmcsdChannelHandle) \
                    (pDevice->handle))->hDDC->instanceId].blkDrvInfo.devCb(
                                                              mediaHandle,
                                                              &pRes);
            }
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_GET_TEMPORARY_WP):
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_GET_TEMPORARY_WP,
                         ptrData,
                         NULL);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_SET_PERMANENT_WP):
            {
                PSP_BlkDevRes_t pRes = {0, };
                /*Should be false. Block media works as ASYNC if set TRUE which
                is the case for MSC class*/
                pRes.IsEvent = FALSE;
                /*This tells the blk media to wait for cb */
                *pIsComplete = FALSE;
                retVal = PSP_mmcsdStatus(pDevice->handle,
                             PSP_MMCSD_IOCTL_SET_PERMANENT_WP,
                             ptrData,
                             NULL);
                if (IOM_COMPLETED == retVal)
                {
                    if (1U == (*ptrData))
                    {
                        pRes.ReqInfo.Event.EventId = PSP_BLK_EVT_MEDWP;
                    }
                    else
                    {
                        pRes.ReqInfo.Event.EventId = PSP_BLK_EVT_MEDRW;
                    }
                    /*TRICKY: Should always be after the setting of event id
                     variable as ReqInfo is a union*/
                    pRes.ReqInfo.Status.Error = IOM_COMPLETED;
                }
                else
                {
                    pRes.ReqInfo.Status.Error = IOM_EBADARGS;
                }
                /*Tells the blk media about the CURRENT WP/RW status of the
                device*/
                gMmcsdDdaObj[((DDC_MmcsdChannelHandle) \
                    (pDevice->handle))->hDDC->instanceId].blkDrvInfo.devCb(
                                                              mediaHandle,
                                                              &pRes);
            }
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_GET_PERMANENT_WP):
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_GET_PERMANENT_WP,
                         ptrData,
                         NULL);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX +
                                    PSP_MMCSD_IOCTL_CHECK_HIGH_CAPACITY_CARD):
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_CHECK_HIGH_CAPACITY_CARD,
                         ptrData,
                         NULL);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_GET_TOTAL_SECTORS):
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_GET_TOTAL_SECTORS,
                         ptrData,
                         NULL);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_SET_EVENTQ):
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_SET_EVENTQ,
                         ptrData,
                         NULL);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_GET_CARDREGS):
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_GET_CARDREGS,
                         ptrData,
                         NULL);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_SET_CARD_FREQUENCY):
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_SET_CARD_FREQUENCY,
                         ptrData,
                         NULL);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_GET_CARD_VENDOR):
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_GET_CARD_VENDOR,
                         ptrData,
                         NULL);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_GET_CONTROLLER_REG):
            pData1  = (Uint32*)pIoctlInfo->pData1;
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_GET_CONTROLLER_REG,
                         ptrData,
                         pData1);
            break;

        case (Uint32)(PSP_BLK_IOCTL_MAX + PSP_MMCSD_IOCTL_SET_CONTROLLER_REG):
            pData1  = (Uint32*)pIoctlInfo->pData1;
            retVal = PSP_mmcsdStatus(pDevice->handle,
                         PSP_MMCSD_IOCTL_SET_CONTROLLER_REG,
                         ptrData,
                         pData1);
            break;

        default:
            retVal = IOM_EBADARGS;
            break;
    }

    return retVal;
}


/**
 * \brief mmcsdRead :Block device Read function Definition
 *
 *  Block device IO function type definitions.
 *
 * \param handle   Device Handle for the device
 * \param mediaHandle    Media Driver Handle for this device
 * \param buffer   Buffer Pointer
 * \param sectNum  Sector Number to start
 * \param numSect  Number of sectors to transfer
 ******************************************************************************/
Int32 mmcsdRead(Ptr handle,
                Ptr mediaHandle,
                Ptr buffer,
                Uint32 sectNum,
                Uint32 numSect)
{
    Int32 retVal = IOM_EBADARGS;
    assert(((NULL != handle) && (NULL != buffer)) &&
        ((NULL != mediaHandle) && (0 != numSect)));

#ifdef MMCSD_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(
                &DVTEvent_Log,
                "MMCSD",
                PSP_DVT_DESC(
                            PSP_DVT_eFUNC_START,
                            PSP_DVT_dCHANNEL_ID_INST,
                            PSP_DVT_dPACKET_ID,
                            PSP_DVT_dSIZE_BYTES
                            ),
                CHANNELID_INSTANCEID(
                    DDC_MMCSD_IOCMD_READ,
                    0),
                (Uint16 *)buffer,
                numSect);
#endif /* MMCSD_INSTRUMENTATION_ENABLED */
    retVal = PSP_mmcsdInput(((DDA_MmcsdBlkDrvInfo *)handle)->handle,
                 mediaHandle,
                 sectNum,
                 (Uint16 *)buffer,
                 numSect,
                 PSP_MMCSD_WAIT_FOREVER);

    if (IOM_PENDING == retVal)
    {
        retVal = IOM_COMPLETED;
    }
#ifdef MMCSD_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(
                &DVTEvent_Log,
                "MMCSD",
                PSP_DVT_DESC(
                            PSP_DVT_eFUNC_END,
                            PSP_DVT_dCHANNEL_ID_INST,
                            PSP_DVT_dPACKET_ID,
                            PSP_DVT_dSIZE_BYTES
                            ),
                CHANNELID_INSTANCEID(
                    DDC_MMCSD_IOCMD_READ,
                    0),
                (Uint16 *)buffer,
                0);
#endif /* MMCSD_INSTRUMENTATION_ENABLED */
    return retVal;
}

/**
 * \brief mmcsdWrite : Block device Write function Definition
 *
 *  Block device IO function type definitions.
 *
 * \param handle   Device Handle for the device
 * \param mediaHandle    Media Driver Handle for this device
 * \param buffer   Buffer Pointer
 * \param sectNum  Sector Number to start
 * \param numSect  Number of sectors to transfer
 ******************************************************************************/
Int32 mmcsdWrite(Ptr handle,
                 Ptr mediaHandle,
                 Ptr buffer,
                 Uint32 sectNum,
                 Uint32 numSect)
{
    Int32 retVal = IOM_EBADARGS;

    assert(((NULL != handle) && (NULL != buffer)) &&
        ((NULL != mediaHandle) && (0 != numSect)));
#ifdef MMCSD_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(
                &DVTEvent_Log,
                "MMCSD",
                PSP_DVT_DESC(
                            PSP_DVT_eFUNC_START,
                            PSP_DVT_dCHANNEL_ID_INST,
                            PSP_DVT_dPACKET_ID,
                            PSP_DVT_dSIZE_BYTES
                            ),
                CHANNELID_INSTANCEID(
                    DDC_MMCSD_IOCMD_WRITE,
                    0),
                (Uint16 *)buffer,
                numSect);
#endif /* MMCSD_INSTRUMENTATION_ENABLED */
    retVal = PSP_mmcsdOutput(((DDA_MmcsdBlkDrvInfo *)handle)->handle,
                 mediaHandle,
                 sectNum,
                 (Uint16 *)buffer,
                 numSect,
                 PSP_MMCSD_WAIT_FOREVER);
    if (IOM_PENDING == retVal)
    {
        retVal = IOM_COMPLETED;
    }
#ifdef MMCSD_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(
                &DVTEvent_Log,
                "MMCSD",
                PSP_DVT_DESC(
                            PSP_DVT_eFUNC_END,
                            PSP_DVT_dCHANNEL_ID_INST,
                            PSP_DVT_dPACKET_ID,
                            PSP_DVT_dSIZE_BYTES
                            ),
                CHANNELID_INSTANCEID(
                    DDC_MMCSD_IOCMD_WRITE,
                    0),
                (Uint16 *)buffer,
                0);
#endif /* MMCSD_INSTRUMENTATION_ENABLED */
    return retVal;
}


/**
 *  \brief Call back function for mount of MMCSD.
 *
 *  This API will be called by block media to notify that device has been
 *  mounted with filesystem
 *
 *  \return Void.
 */
Void mmcsdDetectCallback(Ptr const param)
{
    assert(NULL != param);
    gMmcsdDetectError = *((Int*) param);
    SEM_post(gMmcsdCallbackSem);
}

/* ========================================================================== */
/*                        GLOBAL FUNCTION DEFINTIONS                          */
/* ========================================================================== */
Int32 PSP_mmcsdCheckCard(PSP_MmcsdCardType* cardType, Uint32 instanceId)
{
    Int32               result = IOM_COMPLETED;
    PSP_BlkDevEvent_t   blkEventInfo;
    Uint32              intState;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == cardType) || (instanceId >= PSP_MMCSD_NUM_INSTANCES))
    {
        result = IOM_EBADARGS;
    }
#endif
/* End parameter checking                                                     */
    if (IOM_COMPLETED == result)
    {
        /*Check whether device is present or not*/
        result = PSP_mmcsdStatus(gMmcsdDdaObj[instanceId].ddaChannelHandle,
                     PSP_MMCSD_IOCTL_CHECK_CARD,
                     cardType,
                     NULL);
        if (IOM_COMPLETED == result)
        {
            if (gMmcsdDdaObj[instanceId].cardmediaType != (*cardType))
            {
                intState = HWI_disable();
                gMmcsdDdaObj[instanceId].cardmediaType = *cardType;
                HWI_restore(intState);
                if ((PSP_MMCSD_CARDTYPE_MMC == \
                                gMmcsdDdaObj[instanceId].cardmediaType) ||
                    (PSP_MMCSD_CARDTYPE_SD == \
                                gMmcsdDdaObj[instanceId].cardmediaType))
                {
                    blkEventInfo.EventId = PSP_BLK_EVT_MEDINS;
                    blkEventInfo.EvtData = &gMmcsdDdaObj[instanceId].blkDrvInfo;
#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
                    if (MMCSD_INST_ID0 == instanceId)
                    {
                        /*Used for propagating events from the MMCSD0 driver
                          to the block driver if the card is present or inserted
                         (Ex. Device insertion).*/
                        PSP_blkmediaCallback(PSP_BLK_DRV_MMC0, &blkEventInfo);
                    }
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
                    if (MMCSD_INST_ID1 == instanceId)
                    {
                        /*Used for propagating events from the MMCSD1 driver
                          to the block driver if the card is present or inserted
                         (Ex. Device insertion).*/
                        PSP_blkmediaCallback(PSP_BLK_DRV_MMC1, &blkEventInfo);
                    }
#endif

                    if (TRUE != SEM_pend(gMmcsdCallbackSem, SYS_FOREVER))
                    {
                        MMCSD_ERR_DEBUG(("\r\n MMCSD: Sem gMmcsdCallbackSem "
                                         "Take failed"));
                    }

                    if (gMmcsdDetectError != 0)
                    {
                        MMCSD_DRV_DEBUG(("\r\n MMCSD: Card init. failed"));
                        result = gMmcsdDetectError;

                        blkEventInfo.EventId = PSP_BLK_EVT_MEDREM;
                        blkEventInfo.EvtData = \
                            &gMmcsdDdaObj[instanceId].blkDrvInfo;

#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
                        if (MMCSD_INST_ID0 == instanceId)
                        {
                            /*Used for propagating events from the MMCSD0 driver
                             to the block driver independent of the device
                             context (Ex. Device insertion).*/
                            PSP_blkmediaCallback(PSP_BLK_DRV_MMC0,
                                &blkEventInfo);
                        }
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
                        if (MMCSD_INST_ID1 == instanceId)
                        {
                            /*Used for propagating events from the MMCSD1 driver
                             to the block driver independent of the device
                             context (Ex. Device insertion).*/
                            PSP_blkmediaCallback(PSP_BLK_DRV_MMC1,
                                &blkEventInfo);
                        }
#endif
                        *cardType = PSP_MMCSD_CARDTYPE_NONE;
                        intState = HWI_disable();
                        gMmcsdDdaObj[instanceId].cardmediaType = *cardType;
                        HWI_restore(intState);
                    }
                    else
                    {
                        if (PSP_MMCSD_CARDTYPE_MMC == \
                                gMmcsdDdaObj[instanceId].cardmediaType)
                        {
                            MMCSD_DRV_DEBUG(("\r\nMMCSD: MMC Card is present"));
                        }
                        else if (PSP_MMCSD_CARDTYPE_SD == \
                                 gMmcsdDdaObj[instanceId].cardmediaType)
                        {
                            MMCSD_DRV_DEBUG(("\r\nMMCSD: SD Card is present"));
                        }
                        else
                        {
                            MMCSD_DRV_DEBUG(("\r\nMMCSD: No Card is present"));
                        }
                    }
                }
                else
                {
                    blkEventInfo.EventId = PSP_BLK_EVT_MEDREM;
                    blkEventInfo.EvtData = &gMmcsdDdaObj[instanceId].blkDrvInfo;
#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
                    if (MMCSD_INST_ID0 == instanceId)
                    {
                        /*Used for propagating events from the MMCSD0 driver to
                         the block driver independent of the device context
                         (Ex. Device insertion).*/
                        PSP_blkmediaCallback(PSP_BLK_DRV_MMC0, &blkEventInfo);
                    }
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
                    if (MMCSD_INST_ID1 == instanceId)
                    {
                        /*Used for propagating events from the MMCSD1 driver to
                         the block driver independent of the device context
                         (Ex. Device insertion).*/
                        PSP_blkmediaCallback(PSP_BLK_DRV_MMC1, &blkEventInfo);
                    }
#endif
                }
            }
        }
        else
        {
            if (PSP_MMCSD_RESULT_ERROR_FAILURE == result)
            {
                if (PSP_MMCSD_CARDTYPE_NONE == \
                                        gMmcsdDdaObj[instanceId].cardmediaType)
                {
                    /* Controller is intialized but card is not present */
                    result = IOM_COMPLETED;
                }
            }
            else
            {
                /* Card is present but actual error with happens with card being
                inserted */
            }

        }
    }
    return result;
}


/**
 *  \brief PSP_mmcsdDrvDeInit () : DeInit the MMCSD driver.
 *
 *  \return Int32 IOM_COMPLETED or error
 ******************************************************************************/
Int32 PSP_mmcsdDrvDeInit(Uint32 instanceId)
{
    Int32               result = IOM_COMPLETED;
    PSP_BlkDevEvent_t   blkEventInfo;
    PSP_BlkDrvReg_t     blkDrvRegInfo;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (instanceId >= PSP_MMCSD_NUM_INSTANCES)
    {
        result = IOM_EBADARGS;
    }
#endif
/* End parameter checking                                                     */

    if (IOM_COMPLETED == result)
    {
        gMmcsdDdaObj[instanceId].blkDrvInfo.devOps.Blk_Ioctl = NULL;
        gMmcsdDdaObj[instanceId].blkDrvInfo.devOps.Blk_Read = NULL;
        gMmcsdDdaObj[instanceId].blkDrvInfo.devOps.Blk_Write = NULL;

        SEM_delete(gMmcsdCallbackSem);
        gMmcsdDdaObj[instanceId].cardmediaType = PSP_MMCSD_CARDTYPE_NONE;

        result = PSP_mmcsdCloseChannel(
                     gMmcsdDdaObj[instanceId].ddaChannelHandle);
        if (IOM_COMPLETED != result)
        {
            MMCSD_ERR_DEBUG(("\r\n MMCSD : PSP_mmcsdCloseChannel failed"));
        }
        else
        {
            result = PSP_mmcsdTerminate(gMmcsdHandle);
        }
        if (IOM_COMPLETED != result)
        {
            MMCSD_ERR_DEBUG(("\r\n MMCSD : PSP_mmcsdTerminate failed"));
        }

        blkEventInfo.EventId = PSP_BLK_EVT_MEDREM;
        blkEventInfo.EvtData = &gMmcsdDdaObj[instanceId].blkDrvInfo;

#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
        if (MMCSD_INST_ID0 == instanceId)
        {
            /*Used for propagating events from the MMCSD0 driver to the block
            driver independent of the device context (Ex. Device insertion).*/
            PSP_blkmediaCallback(PSP_BLK_DRV_MMC0, &blkEventInfo);
        }
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
        if (MMCSD_INST_ID1 == instanceId)
        {
            /*Used for propagating events from the MMCSD1 driver to the block
            driver independent of the device context (Ex. Device insertion).*/
            PSP_blkmediaCallback(PSP_BLK_DRV_MMC1, &blkEventInfo);
        }
#endif
        blkDrvRegInfo.DeviceReg = NULL;
        /*Register the MMCSD Driver with the Block Device Driver.*/
#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
        result = PSP_blkmediaDrvRegister(PSP_BLK_DRV_MMC0, &blkDrvRegInfo);
#endif
#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
        result = PSP_blkmediaDrvRegister(PSP_BLK_DRV_MMC1, &blkDrvRegInfo);
#endif
        if (IOM_COMPLETED != result)
        {
            MMCSD_ERR_DEBUG(("\r\n MMCSD : PSP_blkmediaDrvRegister failed"));
        }
        else
        {
            MMCSD_DRV_DEBUG(("\r\n MMCSD: Driver De-initialization ... Done"));
        }
    }
    return result;

}


/**
 *  \brief PSP_mmcsdDrvInit () : Init the MMCSD driver.
 *
 *  \return Int32 IOM_COMPLETED or error
 ******************************************************************************/
Int32 PSP_mmcsdDrvInit(Uint32 moduleFreq,
                       Uint32 instanceId,
                       PSP_MmcsdConfig* const config)
{
    PSP_BlkDrvReg_t         blkDrvRegInfo;
    Int32                   result;
    PSP_BlkDrvId_t          device;
    PSP_BlkDrvIoctlInfo_t   blkDrvIoctlInfo;
    SEM_Attrs               semAttr;
    PSP_MmcsdCardType       cardType;
    PSP_MmcsdChannelConfig  channelConfig;
    Bool                    flag = TRUE;

    do
    {
        /* Initialize the local variables as required                         */
        result = IOM_EBADARGS;
        flag = FALSE;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((instanceId >= PSP_MMCSD_NUM_INSTANCES) || (NULL == config))
        {
            break;
        }

        if ((PSP_MMCSD_OPMODE_POLLED != config->opMode) &&
            (PSP_MMCSD_OPMODE_DMAINTERRUPT != config->opMode))
        {
            break;
        }

        if (PSP_MMCSD_OPMODE_DMAINTERRUPT == config->opMode)
        {
            if (NULL == config->hEdma)
            {
                break;
            }

            if ((PSP_MMCSD_EDMA3_EVENTQ_0 != config->eventQ) &&
               (config->eventQ != PSP_MMCSD_EDMA3_EVENTQ_1))
            {
                break;
            }
        }
#endif
/* End parameter checking                                                     */

        memset(&gMmcsdDdaObj[instanceId], 0x00, sizeof(DDA_MmcsdObj));

        semAttr.name = (String)"Mmcsd_Callback_Sema";
        gMmcsdCallbackSem = SEM_create(0U, &semAttr);
        if (NULL == gMmcsdCallbackSem)
        {
            MMCSD_ERR_DEBUG(("\r\nMMCSD: gMmcsdCallbackSem Create failed"));
            result = IOM_EALLOC;
            break;
        }

#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
        if (MMCSD_INST_ID0 == instanceId)
        {
            device = PSP_BLK_DRV_MMC0;
        }
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
        if (MMCSD_INST_ID1 == instanceId)
        {
            device = PSP_BLK_DRV_MMC1;
        }
#endif

        blkDrvIoctlInfo.Cmd = PSP_BLK_DRV_SET_INIT_COMP_CALLBACK;
        blkDrvIoctlInfo.pData = (Ptr)&mmcsdDetectCallback;
        /* Register the module's mount complete callback function with block
           media driver                                                       */
        result = PSP_blkmediaDrvIoctl((Ptr)(&device), (Ptr)&blkDrvIoctlInfo);
        if (IOM_COMPLETED != result)
        {
            MMCSD_ERR_DEBUG(("\r\nMMCSD :PSP_mmcsdDrvInit:INIT COMP CB Fails"));
            result = IOM_EBADARGS;
            break;
        }

        gMmcsdDdaObj[instanceId].blkDrvInfo.devOps.Blk_Ioctl = &mmcsdDrvIoctl;
        gMmcsdDdaObj[instanceId].blkDrvInfo.devOps.Blk_Read = &mmcsdRead;
        gMmcsdDdaObj[instanceId].blkDrvInfo.devOps.Blk_Write = &mmcsdWrite;

#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
        if (MMCSD_INST_ID0 == instanceId)
        {
            channelConfig.appCallback = &mmcsdMediaCallback0;
        }
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
        if (MMCSD_INST_ID1 == instanceId)
        {
            channelConfig.appCallback = &mmcsdMediaCallback1;
        }
#endif

        result = PSP_mmcsdInitialize(instanceId,
                     &gMmcsdHandle,
                     config,
                     moduleFreq);

        if (IOM_COMPLETED == result)
        {
            result = PSP_mmcsdOpenChannel(gMmcsdHandle,
                         &gMmcsdDdaObj[instanceId].ddaChannelHandle,
                         &channelConfig);
        }
        if (IOM_COMPLETED != result)
        {
            MMCSD_ERR_DEBUG(("\r\n MMCSD : Intialization failed"));
        }
        /*Set hardware event notification for card insertion and removal.
        Timeout of a cmd is supposed to be card removal event*/
        if (IOM_COMPLETED == result)
        {
#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
            if (MMCSD_INST_ID0 == instanceId)
            {
                result = PSP_mmcsdStatus(
                             gMmcsdDdaObj[instanceId].ddaChannelHandle,
                             PSP_MMCSD_IOCTL_SET_HWEVENT_NOTIFICATION,
                             (Ptr)&mmcsdMediaHwEventNotification0,
                             NULL);
            }
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
            if (MMCSD_INST_ID1 == instanceId)
            {
                result = PSP_mmcsdStatus(
                             gMmcsdDdaObj[instanceId].ddaChannelHandle,
                             PSP_MMCSD_IOCTL_SET_HWEVENT_NOTIFICATION,
                             (Ptr)&mmcsdMediaHwEventNotification1,
                             NULL);
            }
#endif
        }

        if (IOM_COMPLETED == result)
        {
#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
            if (MMCSD_INST_ID0 == instanceId)
            {

                /*This function will be called by block media driver when MMCSD0
                will be detected at hardware layer */
                blkDrvRegInfo.DeviceReg = &mmcsdDrvRegMedia0;

                /*Register the MMCSD0 Driver with the Block Device Driver.*/
                /*blkDrvRegInfo contains address of a function which returns
                the read write and ioctl function*/
                PSP_blkmediaDrvRegister(PSP_BLK_DRV_MMC0, &blkDrvRegInfo);
            }
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
            if (MMCSD_INST_ID1 == instanceId)
            {

                /*This function will be called by block media driver when MMCSD1
                will be detected at hardware layer */
                blkDrvRegInfo.DeviceReg = &mmcsdDrvRegMedia1;

                /*Register the MMCSD1 Driver with the Block Device Driver.*/
                /*blkDrvRegInfo contains address of a function which returns
                the read write and ioctl function*/
                PSP_blkmediaDrvRegister(PSP_BLK_DRV_MMC1, &blkDrvRegInfo);
            }
#endif
            result = PSP_mmcsdCheckCard(&cardType, instanceId);
        }
    }while (flag);
    return result;
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
