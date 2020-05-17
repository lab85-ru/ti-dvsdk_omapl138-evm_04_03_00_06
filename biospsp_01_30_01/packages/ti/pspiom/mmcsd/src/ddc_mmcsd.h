/*
 * ddc_mmcsd.h
 *
 * This file contains MMCSD core functionality macros and interface definitions.
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

/**
 * \file ddc_mmcsd.h
 *
 * \brief DDC defined for a typical MMCSD controller
 *
 * This file incorporates all features defined by a generic storage.
 * This file contains definitions for MMCSD DDC layer.
 *
 * (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#ifndef _DDC_MMCSD_H_
#define _DDC_MMCSD_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include <sem.h>
#include <hwi.h>
#include <tsk.h>

#include "llc_mmcsd.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                             MACRO DEFINITIONS                              */
/* ========================================================================== */
#define MMCSD_SD_STATUS_SIZE_BYTE               64U
/**< SD status size in bytes                                                  */
#define MMCSD_SWITCH_CMD_DATA_SIZE_BYTE         64U
/**< Switch Command data size                                                 */

#define DDC_MMCSD_HWEVENT_ERROR_STATUS          1
/**< hardware event to notify error in status                                 */
#define DDC_MMCSD_HWEVENT_ERROR_RESPONSE        2
/**< hardware event to notify error in command response                       */
#define DDC_MMCSD_HWEVENT_SYNC_DONE             3
/**< hardware event to notify that synchronization is done                    */
#define DDC_MMCSD_HWEVENT_CANCEL_DONE           4
/**< hardware event to notify that cancellation is done                       */
#define DDC_MMCSD_READ_DATA_CRC_ERROR           (1 << 6)
/**< CRC error while data read*/
#define DDC_MMCSD_WRITE_DATA_CRC_ERROR          (1 << 5)
/**< CRC error while data write*/

#define DDC_MMCSD_IOCMD_READ                    1U
/**< Read from the MMCSD                                                      */
#define DDC_MMCSD_IOCMD_WRITE                   2U
/**< Write to the MMCSD                                                       */

#define DDC_MMCSD_FLAG_SYNCPENDING              (1U << 1U)
/**< Set if a sync of async IO packets was requested and the operation has
 * not yet completed                                                          */
#define DDC_MMCSD_FLAG_CANCELPENDING            (1U << 2U)
/**< Set if a cancel of async IO packets was requested and the operation has
 * not yet completed                                                          */
#define DDC_MMCSD_FLAG_DEVICE_STOPPED           (1U << 3U)
/**< Set if the device was requested to be stopped by the application
 * through the ioctl request                                                  */
#define DDC_MMCSD_FLAG_DEVICE_ERROR             (1U << 4U)
/**< Set if the device was requested to be stopped by the application
 * through the ioctl request                                                  */

#define DDC_MMCSD_MAX_PWRM_EVENTS               (PWRM_INVALIDEVENT)
/**< Maximum PWRM events supported by MMCSD                                   */
/* ========================================================================== */
/*                      MMCSD DDC DATA TYPES                                  */
/* ========================================================================== */
/* Forward Declarations */
struct DDC_MmcsdDeviceObj;
struct DDC_MmcsdChannelObj;
struct DDC_MmcsdObj;

/**\brief DDC Device object Handle*/
typedef struct DDC_MmcsdDeviceObj* DDC_MmcsdDeviceHandle;

/**\brief DDC object Handle*/
typedef struct DDC_MmcsdObj* DDC_MmcsdHandle;

/** DeviceState represents the state in which the driver is wrt the current
 * card (device).
 */
typedef enum
{
    DDC_MMCSD_DEVICESTATE_START = 0,
    DDC_MMCSD_DEVICESTATE_TRAN,
    DDC_MMCSD_DEVICESTATE_READ,
    DDC_MMCSD_DEVICESTATE_WRITE,
    DDC_MMCSD_DEVICESTATE_READ_STOP,
    DDC_MMCSD_DEVICESTATE_WRITE_STOP,
    DDC_MMCSD_DEVICESTATE_STATUS,
    DDC_MMCSD_DEVICESTATE_FINISH_BUSY,
    DDC_MMCSD_DEVICESTATE_FINISH_CHECK
}DDC_MmcsdDeviceState;

/** Enumeration for voltages ranges for MMC/SD
 */
typedef enum
{
    DDC_MMCSD_CARDOCR_1_65_TO_1_95  = (1 << 7),
    /**< 1.65V - 1.95V                                                        */
    DDC_MMCSD_CARDOCR_2_0_TO_2_1    = (1 << 8),
    /**< 2.0V - 2.1V                                                          */
    DDC_MMCSD_CARDOCR_2_1_TO_2_2    = (1 << 9),
    /**< 2.1V - 2.2V                                                          */
    DDC_MMCSD_CARDOCR_2_2_TO_2_3    = (1 << 10),
    /**< 2.2V - 2.3V                                                          */
    DDC_MMCSD_CARDOCR_2_3_TO_2_4    = (1 << 11),
    /**< 2.3V - 2.4V                                                          */
    DDC_MMCSD_CARDOCR_2_4_TO_2_5    = (1 << 12),
    /**< 2.4V - 2.5V                                                          */
    DDC_MMCSD_CARDOCR_2_5_TO_2_6    = (1 << 13),
    /**< 2.5V - 2.6V                                                          */
    DDC_MMCSD_CARDOCR_2_6_TO_2_7    = (1 << 14),
    /**< 2.6V - 2.7V                                                          */
    DDC_MMCSD_CARDOCR_2_7_TO_2_8    = (1 << 15),
    /**< 2.7V - 2.8V                                                          */
    DDC_MMCSD_CARDOCR_2_8_TO_2_9    = (1 << 16),
    /**< 2.8V - 2.9V                                                          */
    DDC_MMCSD_CARDOCR_2_9_TO_3_0    = (1 << 17),
    /**< 2.9V - 3.0V                                                          */
    DDC_MMCSD_CARDOCR_3_0_TO_3_1    = (1 << 18),
    /**< 3.0V - 3.1V                                                          */
    DDC_MMCSD_CARDOCR_3_1_TO_3_2    = (1 << 19),
    /**< 3.1V - 3.2V                                                          */
    DDC_MMCSD_CARDOCR_3_2_TO_3_3    = (1 << 20),
    /**< 3.2V - 3.3V                                                          */
    DDC_MMCSD_CARDOCR_3_3_TO_3_4    = (1 << 21),
    /**< 3.3V - 3.4V                                                          */
    DDC_MMCSD_CARDOCR_3_4_TO_3_5    = (1 << 22),
    /**< 3.4V - 3.5V                                                          */
    DDC_MMCSD_CARDOCR_3_5_TO_3_6    = (1 << 23)
    /**< 3.5V - 3.6V                                                          */
}DDC_MmcsdCardOcr;

/**
 *  \brief  DDC State
 *
 *  DDC state is maintained/updated by DDC implementation
 */
typedef enum
{
    DDC_DELETED,
    DDC_CREATED,
    DDC_INITIALIZED,
    DDC_OPENED,
    DDC_CLOSED,
    DDC_DEINITIALIZED,
    DDC_PWRM_SUSPEND
    /* ... more generic DDC states here */
}DDC_DriverState;

/**\brief Function pointer for different FSM states of MMCSD*/
typedef Int32 (*DDC_MmcsdStFxn) (DDC_MmcsdDeviceHandle devDDC, Uint16 status);

/**
 * \struct DDC_MmcsdDrvrHwCfg
 *
 * \brief DDC Driver hardware configurations
 *
 * The following object defines hardware specific voltage range suported and
 * the threshold value.
 */
typedef struct
{
    Uint32                      ocr;
    /**< MMCSD OCR-Voltage range for Card.Operating conditions (voltage levels)
     * to be provided through configuration; it is a bitvector obtained by
     * logically ORing DDC_MmcsdCardOcr elements                              */
    Uint16                      rwThreshold;
    /**< minimum data requirement in bytes for a read or
     * write request to be sent                                               */
}DDC_MmcsdDrvrHwCfg;


/**
 * \struct DDC_MmcsdClockInfo
 *
 * \brief DDC Driver Clock configurations
 *
 * The following object defines various clock divider values.
 */
typedef struct
{
    Bool                        isOpenDrainDivBy4Reqd;
    /**< clock frequency to be divided by 4 instead of 2                      */
    Uint32                      openDrain;
    /**< clock frequency (typically supplied as a multiple of
     * functional clock) to be used for open drain mode in MMC/SD             */
    Uint32                      mmcPushPull;
    /**< clock frequency (typically supplied as a multiple of
     * functional clock) to be used for push pull mode in MMC                 */
    Uint32                      sdPushPull;
    /**< clock frequency (typically supplied as a multiple of
     * functional clock) to be used for push pull mode in SD                  */
    Uint32                      sdPushPullHighSpeed;
    /**< clock frequency (typically supplied as a multiple of
     * functional clock) to be used for push pull mode in High Speed SD       */
}DDC_MmcsdClockInfo;

/**
 * \struct DDC_MmcsdIoPacket
 *
 * \brief DDC Device Packet/ Buffer Objects
 *
 * All DDC Devices use the following buffer/ packet objects to request
 * asynchronous IO transaction
 */
typedef struct
{
    QUE_Elem                iopLink;
    /**< List of IO requests, tracked at the device                           */
    Uint8                   cmd;
    /**< Typically READ or WRITE command                                      */
    Ptr                     buf;
    /**< Points to the application supplied IO buffer for the IO
     * transaction. This may point to the actual buffer, an array of
     * scatter/ gather buffers or some other driver specific payload format   */
    Uint32                  xferRequest;
    /**< Number of bytes being requested in this transaction                  */
    Uint32                  timeout;
    /**< Amount of time given to this iop to complete                         */
    Int32                   result;
    /**< Status of IO transaction                                             */
    Uint32                  xferActual;
    /**< Number of bytes actually transferred                                 */
    DDC_MmcsdHandle         hDDC;
    /**< Handle to the context object                                         */
    Uint32                  blockAddress;
    /**< The address of the first block on the mmcsd device, from where
     * the IO request or modification request is addressed                    */
    Ptr                     mediaHandle;
    /**< Handle passed from application                                       */
}DDC_MmcsdIoPacket;

#ifdef BIOS_PWRM_ENABLE
/**
 * \struct DDC_MmcsdPwrmInfo
 *
 * \brief Structure holding PWRM related information
 */
typedef struct DDC_MmcsdPwrmInfo_t
{
    PWRM_Event                  pwrmEvent;
    /**< Current pwrm event being processed                                   */

    PWRM_NotifyHandle           notifyHandle[DDC_MMCSD_MAX_PWRM_EVENTS];
    /**< Handles required for unregistering of the events with PWRM           */

    Fxn                         delayedCompletionFxn[DDC_MMCSD_MAX_PWRM_EVENTS];
    /**< delayed completion callback function pointer                         */
}DDC_MmcsdPwrmInfo;
#endif

/**
 * \struct DDC_MmcsdDeviceObj
 *
 * \brief MMCSD DDC persistent context object
 *
 * A singleton of this type per MMCSD controller instance in the system
 */
typedef struct DDC_MmcsdDeviceObj
{
    PSP_MmcsdHwEventNotification        hwEventNotification;
    /**< callback for MMCSD events                                            */
    Uint32                              versionId;
    /**< Version Id                                                           */
    Uint32                              instId;
    /**< Instance id/number                                                   */
    PSP_MmcsdOpMode                     opMode;
    /**< Operation mode of driver                                             */
    DDC_DriverState                     state;
    /**< DDC State                                                            */
    DDC_DriverState                     prevDevstate;
    /**< Previous DDC State                                                   */
    Uint32                              flags;
    /**< Configuration flags: see DDC_MMCSD_FLAG_xxx                          */
    LLC_MmcsdHandle                     hLlc;
    /**< the controller object handle                                         */
    DDC_MmcsdDrvrHwCfg*                 drvrHwCfg;
    /**< parameters that are needed by the controller & passed from above     */
    DDC_MmcsdClockInfo                  hDrvrClock;
    /**< MMCSD clock structure pointer                                        */
    QUE_Handle                          listHeadNode;
    /**< Pending IO requests, waiting for transfers to complete               */
    PSP_MmcsdCardRegs                   cardRegs;
    /**< card registers for the channel in question                           */
    DDC_MmcsdDeviceState                devState;
    /**< State in which the card currently is - this is used by the
     * driver in order to take further action at any point of time            */
    Uint16                              blkSize;
    /**< block size with which the controller interacts with this device      */
    Uint32                              chunkSize;
    /**< size of data transfer in one shot                                    */
    Uint32                              actualChunkXfered;
    /**< size of data transfer in one shot                                    */
    SEM_Handle                          semAlignDrvrThreads;
    /**< semaphore for managing ddc threads                                   */
    SEM_Handle                          syncSem;
    /**< semaphore for synchronization between task and ISR                   */
    DDC_MmcsdStFxn                    fsm[DDC_MMCSD_DEVICESTATE_FINISH_CHECK+1];
    /**< This is the fsm for the driver. One is added because klocwork gives
    error to have check every place for max                                   */
    Uint32                              sectorCnt;
    /**< No of sector on media                                                */
    Bool                                tempWP;
    /**< Temprary write protect                                               */
    Bool                                permanentWP;
    /**< Permanent write protect                                              */
    Bool                                isHighCapacitySDCard;
    /**< High capacity card flag                                              */
    Bool                                isHighSpeedSDCard;
    /**< High speed card flag                                                 */
    Bool                                isDmaError;
    /**< to check DMA error                                                   */
    Ptr                                 hEdma;
    /**< Handle to edma                                                       */
    PSP_MmcsdEdmaEventQueue             eventQ;
    /**< MMCSD EDMA eventqueue no.                                            */
    Bool                                taskRun;
    /**< MMCSD Task running or not.                                           */
    PSP_MmcsdPllDomain                  pllDomain;
    /**< Pll domain to be used for the device                                 */
#ifdef BIOS_PWRM_ENABLE
    DDC_MmcsdPwrmInfo                   pwrmInfo;
    /**< structure to hold the PWMR related information                       */
#endif
}DDC_MmcsdDeviceObj;
/**< MMCSD DDC persistent context object                                      */

/**\brief DDC app callback*/
typedef Int32 (*DDC_MmcsdAppCallback) (struct DDC_MmcsdChannelObj* pChannel,
                                       Int32                       count,
                                       Ptr                         mediaHandle);

/**\brief DDC App delay*/
typedef Int32 (*DDC_MmcsdAppDelay) (struct DDC_MmcsdChannelObj*   pChannel,
                                    DDC_MmcsdIoPacket*            iop,
                                    Uint32                        timeout);

/**
 * \struct DDC_MmcsdChannelObj
 *
 * \brief MMCSD DDC channel object structure
 *
 */
typedef struct DDC_MmcsdChannelObj
{
    PSP_MmcsdAppCallback        appCallback;
    /**< application callback                                                 */
    DDC_MmcsdAppCallback        ddcCallback;
    /**< DDC callback                                                         */
    DDC_MmcsdAppDelay           ddcDelay;
    /**< DDC delay                                                            */
    Bool                        isAssigned;
    /**< assisned or not                                                      */
    DDC_MmcsdHandle             hDDC;
    /**< DDC handle                                                           */
}DDC_MmcsdChannelObj, *DDC_MmcsdChannelHandle;
/**< MMCSD DDC channel object structure                                       */

/**
 * \struct DDC_MmcsdObj
 *
 * \brief MMCSD DDC object
 */
typedef struct DDC_MmcsdObj
{
    DDC_MmcsdIoPacket               iopPool[DDC_MMCSD_IO_DEPTH];
    /**< IO packet pool to be used to store pending requests                  */
    QUE_Handle                      iopFreeListHead;
    /**< Pointer to list of free packets                                      */
    QUE_Handle                      iopReclaimableListHead;
    /**< Pointer to list of completed packets                                 */
    DDC_MmcsdChannelObj             channelObj;
    /**< MMCSD channel object                                                 */
    Uint32                          instanceId;
    /**< MMCSD controller instance id                                         */
    Uint32                          blkSize;
    /**< Block size for transfers                                             */
    Uint16                          hwiNumber;
    /**< Hardware interrupt Number                                            */
    Uint32                          cpuArmClock;
    /**< ARM clock                                                            */
    LLC_MmcsdObj                    oLlc;
    /**< Low level controller object                                          */
    DDC_MmcsdDeviceObj              ddcDevObj;
    /**< MMCSD DDC object                                                     */
    TSK_Handle                      taskHandle;
    /**< Task Handle                                                          */
}DDC_MmcsdObj;
/**< MMCSD DDC object                                                         */

#ifdef __cplusplus
}
#endif

#endif /* _DDC_MMCSD_H_ */
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
