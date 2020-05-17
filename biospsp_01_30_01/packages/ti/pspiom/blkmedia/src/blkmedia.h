/*
 * blkmedia.h
 *
 * This file contains Block Media layer command and interface definitions used 
 * by Block Media driver layer. 
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

/** \file   blkmedia.h
    \brief  Block Device driver header file

    This file defines the various data structures to be used by Block Device
    Driver.

    (C) Copyright 2008, Texas Instruments, Inc

 */

#ifndef _BLKMEDIA_H_
#define _BLKMEDIA_H_

#include <sem.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define BLK_DEV_MAXQUEUE        5u
/**< Max queue depth per device                                               */

/**
 * \brief Block Device Driver config Definitions
 */
#define BLK_DEV_MAXREQ    (PSP_BLK_DEV_MAXDEV * BLK_DEV_MAXQUEUE)
/**< Blk-dev request queue size                                               */

/**
 * \brief Block Device Availability state definitions
 */
typedef enum BLK_DEV_Avail
{
    BLK_DEV_CREATED = -1,
    /**< Blk-dev created                                                      */
    BLK_DEV_INIT,
    /**< Blk-dev default state                                                */
    BLK_DEV_AVAIL,
    /**< Blk-dev available but not mounted                                    */
    BLK_DEV_REG,
    /**< Device mounted and registerd with blk-dev                            */
    BLK_DEV_DELETED
    /**< Blk-dev deleted                                                      */
}BLK_DEV_Avail;


/**
 * \brief Block Interface Driver: Direction for EDMA copy
 */
typedef enum BLK_DEV_DataDir
{
    BLK_DEV_Write = 0,
    /**< Media Write  */
    BLK_DEV_Read
    /**< Media Read   */
}BLK_DEV_DataDir;


/* Forward Declaration */
struct BLK_DEV_Info;

/**
 * \brief Block Device Request Status Completion information
 */
typedef struct BLK_DEV_Req
{
    QUE_Elem                Link;
    /**< links to other queued requests                                       */
    Ptr                     AppHandle;
    /**< Application Handle                                                   */
    Bool                    isIoctl;
    /**< Is this request a IOCTL                                              */
    struct BLK_DEV_Info  *pDevice;
    /**< Device Point                                                         */
    PSP_BlkDevRes_t         Status;
    /**< Status return for the IO's from this mount point                     */
}BLK_DEV_Req;

/**
 * \brief Block Device Mass storage device information
 */
typedef struct BLK_DEV_RAW_Dev
{
    PSP_BlkDrvId_t BLKAPP_DEV;
    /**< BLKAPP_DEV will expose the corresponding device to RAW applications. */
}BLK_DEV_RAW_Dev;

#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.                 */
/**
 *  \brief BLK Device Buffer alignment Structure Definition
 */
typedef struct BLK_DEV_BufferAlign
{
    Uint32    split;
    /**< Number of times we need to write                                     */
    Uint8     *pAlignedData;
    /**< Aligned Data                                                         */
}BLK_DEV_BufferAlign;
#endif /* #endif PSP_BUFF_ALIGNMENT */

/**
 * \brief Block Device Information structure Definition
 */
typedef struct BLK_DEV_Info {
    PSP_BlkDrvId_t          DevBus;
    /**< Type of Device bus                                                   */
    PSP_BlkDevOps_t         BlkOps;
    /**< Block Device operations                                              */
    PSP_BlkDevOps_t         *pDevOps;
    /**< Client Device operations                                             */
    Ptr                     DevHandle;
    /**< Block Device Handle                                                  */
    SEM_Handle              ioSema;
    /**< IO semaphore                                                         */
    SEM_Handle              asyncSema;
    /**< Async semaphore                                                      */
    SEM_Handle              ctlSema;
    /**< IOCTL semaphore                                                      */
    Uint8                   devNum;
    /**< Drive Number                                                         */
    PSP_blkDevCallback_t    appCb;
    /**< RAW device Callback Function                                         */
    PSP_blkDevCallback_t    devCb;
    /**< DEV Callback Function                                                */
    Uint32                  maxUserSect;
    /**< Maximum user accable sector                                          */
    Uint32                  sectorSize;
    /**< Size of one Sector                                                   */
    Uint32                  eraseBlockSize;
    /**< Erase block size for the media                                       */
    Uint16                  Cyl;
    /**< Max Cylinders                                                        */
    Uint16                  Head;
    /**< Max Heads                                                            */
    Uint16                  sectPerTrack;
    /**< Sectors per track                                                    */
    BLK_DEV_Avail         DevState;
    /**< Is the device available                                              */
    Bool                    isRawRegistered;
    /**< Is a RAW device is registered                                        */
    Bool                    IsIoPend;
    /**< Is IO in progress                                                    */
    BLK_DEV_Req           *pReqCache;
    /**< Available request Tokens                                             */
    Uint8                   AvailReq;
    /**< Avaibable request counts                                             */
    Bool                    isMediaWriteProtect;
    /**< Is Media Write Protect                                               */
    BLK_DRV_InitCallback_t  InitCallback;
    /**< Callback function address                                            */
    PSP_BlkOpMode           opMode;
    /**< Block device IO mode                                                 */
#ifdef PSP_BUFF_ALIGNMENT   /*If buffer alignment is enabled.   */
    BLK_DEV_BufferAlign     alignInfo;
    /**< Buffer Alignment information                                         */
#endif  /* #endif PSP_BUFF_ALIGNMENT    */
}BLK_DEV_Info;

#ifdef __cplusplus
}
#endif

#endif /* _BLKMEDIA_H_ */
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
