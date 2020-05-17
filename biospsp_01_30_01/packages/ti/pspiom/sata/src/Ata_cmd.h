/*
 * ata.h
 *
 * ATA command and Error defines file
 * This file defines the various commands and error information.
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
 *  \author     Swaminathan S.
 *  \version    10.0
 */

#ifndef _ATA_CMD_H
#define _ATA_CMD_H

#include "tistdtypes.h"

/**
 * \defgroup ATACommand  ATA Command Definitions
 *
 * The ATA/ATAPI command/Timeout definitions are used by the ATA/ATAPI Interface
 * driver.
 */
/*@{*/

/**
 * \name ATA Set Feature command Sub Commmand defines
 *
 */
/*@{*/
#define ATA_SET_MULT			(0xC6u)  /**< Enable ATA Multiple Read/Write  */
#define ATA_SET_SPEED			(0x03u)  /**< Enable support for DMA/UDMA Modes */
#define ATA_SET_ENBREMSTATUS	(0x95u)  /**< Enable Removable Media Status notification */
#define ATA_SET_DISREMSTATUS	(0x31u)  /**< Disable Removable Media Status notification */
/*@}*/

/**
 * \name ATA READ command Flavours
 *
 */
/*@{*/
#define ATA_READ                      (0x20u)  /**< Read PIO Sectors              */
#define ATA_READ_MUL                  (0xC4u)  /**< Read PIO Multiple Sectors     */
#define ATA_READ_DMA                  (0xC8u)  /**< Read using DMA (Ultra/MW)     */
#define ATA_READ_DMA_QED              (0xC7u)  /**< Read DMA in Queued Mode       */
#define ATA_READ_EXT                  (0x24u)  /**< Read PIO Sectors 48 Bit mode  */
#define ATA_READ_MUL_EXT              (0x29u)  /**< Read PIO Multiple 48 Bit mode */
#define ATA_READ_DMA_EXT              (0x25u)  /**< Read DMA 48 Bit Mode mode     */
#define ATA_READ_DMA_QED_EXT          (0x26u)  /**< Read DMA Queued 48 Bit mode   */
/*@}*/

/**
 * \name ATA WRITE command Flavours
 *
 */
/*@{*/
#define ATA_WRITE                     (0x30u)  /**< Write PIO Sectors              */
#define ATA_WRITE_MUL                 (0xC5u)  /**< Write PIO Multiple Sectors     */
#define ATA_WRITE_DMA                 (0xCAu)  /**< Write using DMA (Ultra/MW)     */
#define ATA_WRITE_DMA_QED             (0xCCu)  /**< Write DMA in Queued Mode       */
#define ATA_WRITE_EXT                 (0x34u)  /**< Write PIO Sectors 48 Bit mode  */
#define ATA_WRITE_MUL_EXT             (0x39u)  /**< Write PIO Multiple 48 Bit mode */
#define ATA_WRITE_DMA_EXT             (0x35u)  /**< Write DMA 48 Bit Mode mode     */
#define ATA_WRITE_DMA_QUEUED_EXT      (0x36u)  /**< Write DMA Queued 48 Bit mode   */
#define ATA_CFA_WRITE_WO_ERASE        (0x38u)  /**< CFA Write Sectors without erase */
#define ATA_CFA_WRITE_MUL_WO_ERASE    (0xCDu)  /**< CFA Write Mult Sectors without Erase   */
#define ATA_CFA_ERASE                 (0xC0u)  /**< CFA Sector Erase               */
/*@}*/

/**
 * \name ATA Generic command Defines
 *
 */
/*@{*/
#define ATA_IDENTIFY                  (0xECu)  /**< Identify ATA Device            */
#define ATA_MEDIA_STATUS              (0xDAu)  /**< Removable Media Status         */
#define ATA_MEDIA_EJECT               (0xEDu)  /**< Eject the Media in the device  */
#define ATA_MEDIA_LOCK                (0xDEu)  /**< Lock the Media in the device   */
#define ATA_MEDIA_UNLOCK              (0xDFu)  /**< Un-Lock the Media in the device  */
#define ATA_SET_FEATURES              (0xEFu)  /**< Set Device Features            */
#define ATA_SET_MUL_MODE              (0xC6u)  /**< Set the device in multiple mode */
#define ATA_READ_MAX_SECT             (0xF8u)  /**< Get the max user addressable sect */
#define ATA_GET_MEDSTAT               (0xDAu)  /**< Get the Medis Status */
/*@}*/

/**
 * \name ATA Power Save command Defines
 *
 */
/*@{*/
#define ATA_PWR_SLEEP                 (0xE6u)  /**< Set the device to Sleep mode  */
#define ATA_PWR_STDBY                 (0xE2u)  /**< Set the device to Standby mode  */
#define ATA_PWR_IDLE                  (0xE3u)  /**< Set the device to Idle mode  */
#define ATA_PWR_CHECK                 (0xE5u)  /**< Get the device power  */
/*@}*/

/**
 * \name ATAPI device specific ATA Command Defines
 *
 */
/*@{*/
#define ATAPI_IDENTIFY                (0xA1u)  /**< Identify a ATAPI Device        */
#define ATAPI_RESET                   (0x08u)  /**< Reset a ATAPI device           */
#define ATAPI_PACKET                  (0xA0u)  /**< ATAPI Command init command     */
/*@}*/

/**
 * \name ATA Set features sub command SET SPEED param definitions
 *
 */
/*@{*/
#define ATA_DEV_PIOZ                  (0x08u)  /**< PIO Mode 0 */
#define ATA_DEV_PIO3                  (0x0Bu)  /**< PIO Mode 3 */
#define ATA_DEV_PIO4                  (0x0Cu)  /**< PIO Mode 4 */
#define ATA_DEV_DMA0                  (0x20u)  /**< DMA Mode 0 */
#define ATA_DEV_DMA1                  (0x21u)  /**< DMA Mode 1 */
#define ATA_DEV_DMA2                  (0x22u)  /**< DMA Mode 0 */
#define ATA_DEV_UDMA0                 (0x40u)  /**< UDMA Mode 0 */
#define ATA_DEV_UDMA1                 (0x41u)  /**< UDMA Mode 1 */
#define ATA_DEV_UDMA2                 (0x42u)  /**< UDMA Mode 2 */
#define ATA_DEV_UDMA3                 (0x43u)  /**< UDMA Mode 3 */
#define ATA_DEV_UDMA4                 (0x44u)  /**< UDMA Mode 4 */
#define ATA_DEV_UDMA5                 (0x45u)  /**< UDMA Mode 5 */
#define ATA_DEV_UDMA6                 (0x46u)  /**< UDMA Mode 6 */
/*@}*/

/**
 * \name ATA command status definitions
 *
 */
/*@{*/
#define ATA_DEV_BUSY			(0x80u)  /**< Device is Busy */
#define ATA_DEV_RDY				(0x40u)  /**< Device is Ready */
#define ATA_DEV_D				(0x20u) /**< Device Fault */
#define ATA_DEV_REQ				(0x08u)  /**< Device is Requesting transfer */
#define ATA_DEV_ERR				(0x01u)  /**< Device Error */
#define ATA_DEV_STAT_MASK		(0xE9u)	/*< mask for NA bits*/
/*@}*/

/**
 * \name ATA Device capabilities identification
 *
 */
/*@{*/
#define ATA_DEV_CYCLE_TIME		(0x02u)  /**< Device cycle time data is valid */
#define ATA_DEV_UDMA			(0x04u)  /**< Device UDMA data is valid */
#define ATA_DEV_CHS			(0x01u)  /**< Device CHS data is valid */
/*@}*/

/**
 * \name ATA Device Error Definitions
 *
 */
/*@{*/
#define ATA_DEV_ERR_WP				(0x40u)  /**< Write Protect     */
#define ATA_DEV_ERR_UADDR			(0x10u)  /**< Undefined Address */
#define ATA_DEV_ERR_ABRT			(0x04u)  /**< Command abort     */
#define ATA_DEV_ERR_MCR			(0x08u)  /**< Eject request     */
#define ATA_DEV_ERR_NM				(0x02u)  /**< No Media present  */
#define ATA_DEV_ERR_MC				(0x20u)  /**< Media changed     */
/*@}*/

/* ATA enable LBA */
#define ATA_DEV_LBA					(0x4Fu)  /**< Enable LBA Mode on a ATA device */

#define ATA_PIO_4					(4u)  /**< PIO Mode 4 */
#define ATA_PIO_3					(3u)  /**< PIO Mode 3 */
#define ATA_DMA_0					(0u)  /**< DMA Mode 0 */
#define ATA_DMA_1					(1u)  /**< DMA Mode 1 */
#define ATA_DMA_2					(2u)  /**< DMA Mode 2 */
#define ATA_UDMA_0					(0u)  /**< UDMA Mode 0 */
#define ATA_UDMA_1					(1u)  /**< UDMA Mode 1 */
#define ATA_UDMA_2					(2u)  /**< UDMA Mode 2 */
#define ATA_UDMA_3					(3u)  /**< UDMA Mode 3 */
#define ATA_UDMA_4					(4u)  /**< UDMA Mode 4 */
#define ATA_UDMA_5					(5u)  /**< UDMA Mode 5 */
#define ATA_UDMA_6					(6u)  /**< UDMA Mode 6 */

/**
 * \name ATA Device Timeout Definitions
 *
 */
/*@{*/
#define ATA_TIMER_GRAN			(10u)     /**< Timer granularity in MSec */

#define ATA_IDENT_TO			(10000u / ATA_TIMER_GRAN)  /**< Device Identify timeout (10 Sec)*/
#define ATA_RESET_TO			(40000u / ATA_TIMER_GRAN) /**< Device Reset timeout (40 Sec)*/
#define ATA_ONESEC_TO			(1000u  / ATA_TIMER_GRAN) /**< One Second Timeout */
#define ATA_HMSEC_TO			(100u / ATA_TIMER_GRAN)    /**< 100 Milli Second Timeout */
#define ATA_REMMED_TO			(1000u / ATA_TIMER_GRAN)   /**< Media Status Probe timeout */
#define ATA_REMMEDPWR_TO		(5000u / ATA_TIMER_GRAN)  /**< Media Status Probe timeout under powe save */
#define ATA_CMD_TO				(10000u / ATA_TIMER_GRAN) /**< Command Timeout 10 Sec */
#define ATA_TMSEC_TO			(10u / ATA_TIMER_GRAN)    /**< Ten MSec Timeout */
#define ATA_MAX_SECT			(256u)           /**< Maximum number of Sector trnsferred/transfer */
#define ATA_PIO_MAX_SECT		(20u)            /**< System maximum for PIO transactions.  Ideally
	                                                    this should be 256 but in certain systems this
														leads to stack overvlow due to interrupt nesting.
														So define this to appropriate value for PIO based
														data transfers for a given platform.
													*/
#endif /* _ATA_CMD_H */

