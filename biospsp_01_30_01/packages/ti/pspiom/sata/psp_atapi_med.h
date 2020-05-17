/*
 * psp_atapi_med.h
 *
 * ATAPI Media interface header file
 * This file defines the various ATAPI interfaces for Media Drivers using ATA
 * interface.
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
 *	\author     Swaminathan S.
 *	\version    10.0
 */

#ifndef ATAPI_MED_H
#define ATAPI_MED_H

#include <tistdtypes.h>
#include <psp_ata_med.h>

/**
 * \defgroup ATAPIMedia ATAPI Interface Definitions
 *
 * All ATAPI Media Drivers use these interface definitions for interacting
 * with the ATA/ATAPI Driver.
 *
 */
#define ATAPI_REQSENSE_SIZE      	(0x12u)      /**< Request Sense buffer size (INQUIRY) */
#define ATAPI_READCAP_SIZE       	(0x08u)      /**< ReadCap buffer size       */

#define ATAPI_MODESENSE_PG_CD_CAP   (0x2Au)   /**< Mode Sense Params */
#define ATAPI_MODESENSE_CURRENT_VAL (0x00)   /**< Mode Sense Params */
#define ATAPI_READSUB_PG_CUR        (0x01u)   /**< Mode Sense Params */
#define ATAPI_TOC_PG_TEXT           (0x05u)   /**< Mode Sense Params */
#define ATAPI_READ_FUA_BIT          (0x08u)   /**< Mode Sense Params */

/**
 * \name ATAPI command Flavours
 *
 */
#define ATAPI_TESTUNITREADY           (0x00)  /**< Test if unit is ready         */
#define ATAPI_REQSENSE                (0x03)  /**< Request Sense                 */
#define ATAPI_INQUIRY                 (0x12)  /**< INQUIRY                       */
#define ATAPI_STARTSTOP               (0x1B)  /**< Start Stop Unit               */
#define ATAPI_READCAPACITY            (0x25)  /**< Read Capacity Info            */
#define ATAPI_SEEK                    (0x2B)  /**< Seek Command                  */
#define ATAPI_READSUBCHNL             (0x42)  /**< Read Sub Channel Data         */
#define ATAPI_READTOC                 (0x43)  /**< Read TOC                      */
#define ATAPI_PLAYAUDIO               (0x45)  /**< Play Audio output             */
#define ATAPI_GETEVENTSTATUS          (0x4A)  /**< Get event/status notification */
#define ATAPI_PAUSEPLAYSCAN           (0x4B)  /**< Pause Play/Scan               */
#define ATAPI_STOPPLAYSCAN            (0x4E)  /**< Stop Play/Scan                */
#define ATAPI_MODESELECT              (0x55)  /**< Mode Select                   */
#define ATAPI_MODESENSE               (0x5A)  /**< Mode Sense                    */
#define ATAPI_READ                    (0xA8)  /**< Read Data (12)                */
#define ATAPI_WRITE                   (0xAA)  /**< Write Data (12)               */
#define ATAPI_READDVDSTR              (0xAD)  /**< Read DVD Structure            */
#define ATAPI_WRITEANDVERIFY          (0xAE)  /**< Write Data (12) & Verify      */
#define ATAPI_SCAN                    (0xBA)  /**< Scan                          */
#define ATAPI_GETLOADINFO             (0xD1)  /**< Gets load status information  */
#define ATAPI_GETDRIVEINFO            (0xDA)  /**< Gets Kenwood drive Information*/
#define ATAPI_EXECUTEDRIVEDIAG        (0xDE)  /**< Execute driver diagnosis      */
/*@}*/

#define ATAPI_CMDSZ                   (0xC) /**< ATAPI Command Size */

#define ATAPI_TOC_MSF_FORMAT          (0x02)  /**< TOC MSF format */
#define ATAPI_TOC_LOGICAL_FORMAT      (0x00)  /**< TOC Logical format */
#define ATAPI_TOC_SUB_CHANNEL_MODE    (0x80)  /**< TOC subchannel mode info. */
#define ATAPI_TOC_TRACK_MODE          (0x00)  /**< TOC Track Mode info */
#define ATAPI_MEDIA_CLASS             (0x04)  /**< Class of Media      */

/* Response size for get_drive_info request */
#define GET_DRIVE_INFO_SIZE           (122u)   /**< Drive Info buffer size */
/**
 * \brief Raw ATAPI command structure.
 *
 */
typedef struct
{
  Ptr               Cmd;       /**<  ATAPI command */
  Ptr               Buffer;    /**< Buffer Pointer */
  Uint32            TransCnt;  /**< Transfer count in bytes */
  ATA_RawCmdMode_t  Mode;      /**< Command mode */
} ATAPI_Dev_Req_t;

#endif /* ATAPI_MED_H */

