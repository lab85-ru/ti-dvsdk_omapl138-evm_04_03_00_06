/*
 * psp_ata_med.h
 *
 * ATA Media interface header file
 * This file defines the various ATA interfaces for Media Drivers using ATA
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

#ifndef ATA_MED_H
#define ATA_MED_H

#include <tistdtypes.h>

/**
 * \defgroup ATAMedia ATA/ATAPI Media Interface Definitions
 *
 * All ATA/ATAPI Media Drivers use these interface definitions for interacting
 * with the ATA/ATAPI Driver.
 *
 */
/*@{*/

/**
 * \name ATA/ATAPI Media Error Definitions
 *
 * Error return definitions for ATA/ATAPI Media Driver Requests.
 */
/*@{*/

#define ATA_NO_ERR       (0x0)      /**< No Error                 */
#define ATA_ERR_OK       (ATA_NO_ERR) /**< No Error               */
#define ATA_ERR_GEN      (-1)       /**< Generic Error            */
#define ATA_ERR_WP       (-2)      /**< Media is write protected */
#define ATA_ERR_DF       (-3)      /**< Generic Device Fault     */
#define ATA_ERR_UADDR    (-4)      /**< Inaccessible address     */
#define ATA_ERR_TO       (-5)      /**< Command Timeout          */
#define ATA_ERR_RES      (-6)      /**< Media is Reset           */
/*@}*/


/* ATA Buffer Defines */
#define ATA_DEV_BPS                   (0x200u) /**< Device Block Size */

/* ATA Extented Timeout Command Flag */
#define ATA_CMD_EXTTO                 (0x80u) /**< Command to be executed with
                                                a timeout of 40 Secs. */

/**
 * \name ATA/ATAPI Media Event Definitions
 *
 * Event definitions for ATA/ATAPI Media Driver Requests.
 */
/*@{*/
#define ATA_DEV_EVT_PWR_SLEEP    (0x1u)    /**< Device Sleep event    */
#define ATA_DEV_EVT_PWR_STDBY    (0x2u)    /**< Device Standby event  */
#define ATA_DEV_EVT_PWR_IDLE     (0x3u)    /**< Device Idle event     */
#define ATA_DEV_EVT_PWR_ACTIVE   (0x4u)    /**< Device Active event   */
#define ATA_DEV_EVT_RES_BEGIN    (0x5u)    /**< Reset begin event     */
#define ATA_DEV_EVT_RES_END      (0x6u)    /**< Reset end event       */
#define ATA_DEV_EVT_MED_WP       (0x7u)    /**< Media Write Protected */
#define ATA_DEV_EVT_MED_CHANGE   (0x8u)    /**< Media Changed         */
#define ATA_DEV_EVT_MED_CHREQ    (0x9u)    /**< Eject Request         */
#define ATA_DEV_EVT_MED_EJECT    (0xAu)    /**< Media Ejected         */
#define ATA_DEV_EVT_MED_NOMED    (0xBu)    /**< No Media in the Device */
#define ATA_DEV_EVT_MED_WRITE    (0xCu)    /**< Media is Writable      */
#define ATA_DEV_EVT_MED_AVAIL    (0xDu)    /**< Media is Available     */
#define ATA_DEV_EVT_MAX_EVENT    (0xFu)    /**< Holds max number of events
                                             supported */
/*@}*/

/**
 * \name ATA/ATAPI device Opmode Definitions
 *
 * Device Operating mode definitions.
 */
/*@{*/
/* Mode of Device operation */
typedef enum {
  PIO = 0,    /**< Device is in PIO Mode */
  DMA,        /**< Device is in DMA Mode */
  UDMA,       /**< Device is in UDMA Mode */
  AUTO        /**< Device is in AUTO Mode selection */
} ATA_DEV_Mode_t;
/*@}*/

/*@{*/
/* Driver mode */
typedef enum {
  ATA_DMA_INT_MODE = 0,      /**< Device is operating in DMA mode */
  ATA_INTERRUPT_MODE        /**< Device is operating in interrupt mode */
} ATA_DriverMode_t;
/*@}*/

/**
 * \name ATA/ATAPI device Power Mode Definitions
 *
 * Device operating Power Mode definitions.
 */
/*@{*/
typedef enum {
  ATA_DEV_ACTIVE = 0,    /**< Device is in Active Power State */
  ATA_DEV_IDLE,          /**< Device is in Idle Power State */
  ATA_DEV_STDBY,         /**< Device is in Standby Power State */
  ATA_DEV_SLEEP,         /**< Device is in Sleep Power State */
  ATA_DEV_DEVOFF,		 /**< Device power is cut off  */
  ATA_DEV_PWRMAX         /**< Max Number of Power State */
} ATA_DEV_PwrMode_t;
/*@}*/

/**
 * \name ATA/ATAPI device Raw Command mode Definitions
 *
 * ATA/ATAPI device Raw command execution modes.
 */
/*@{*/
typedef enum {
  ATA_MED_RAW_READ = 0,  /**< Read Data in ATA Raw command */
  ATA_MED_RAW_WRITE,     /**< Write Data in ATA Raw command */
  ATA_MED_RAW_NDATA      /**< No Data returned in ATA Raw command */
} ATA_RawCmdMode_t;
/*@}*/

/**
 * \name ATA/ATAPI device IOCTL Command Definitions
 *
 * ATA/ATAPI device IOCTL command enum's.
 */
/*@{*/
typedef enum {
  ATA_IOCTL_RETRY = 0,    /**< Set the retry count for read/write Ops */
  ATA_IOCTL_GETMODE,      /**< Get driver Opmode */
  ATA_IOCTL_GETIDENT,     /**< Get identify info */
  ATA_IOCTL_SETBLKSZ,     /**< Set Sector Size   */
  ATA_IOCTL_REQABRT,      /**< Abort Requests on the device */
  ATA_IOCTL_GETPWRMODE,   /**< Get power mode */
  ATA_IOCTL_GETMEDSTAT,   /**< Get the Media status */
  ATA_IOCTL_SETDEVOFF,    /**< Set the device to power off state */
  ATA_IOCTL_GETPWRSTAT,   /**< Get the device power status by querying the device */
  ATA_IOCTL_HARDRESET,    /**< Initiate a Hard Reset on ATA Device/Bus */
  ATA_IOCTL_SUSPEND_IO,	  /**< Suspend IO operations on the ATA device */
  ATA_IOCTL_RESUME_IO,	  /**< Suspend IO operations on the ATA device */
  ATA_IOCTL_MAX
} ATA_DEV_IoctlCmd_t;
/*@}*/

/**
 * \brief ATA/ATAPI device IO function Definition
 *
 * ATA/ATAPI device IO function type definitions.
 *
 * \param Handle [IN]  ATA Handle for the device
 * \param MHndl  [IN]  Media Driver Handle for this device
 * \param Buffer [IN]  Buffer Pointer
 * \param SectNum [IN] Sector Number to start
 * \param NumSect [IN] Number of sectors to transfer   */
typedef   int  (*ATA_Io_t) (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 SectNum,
                            Uint32 NumSect);
/**
 * \brief ATA device Identify information
 *
 */
typedef struct
{
  Uint8    SerialNo [20];  /**< Device Serial Number */
  Uint16   Cyl;            /**< Number of Cyl */
  Uint16   Head;           /**< Number of Heads */
  Uint16   SectperTrack;   /**< Sectors Per Track */
  Uint8    FirmwareRev[8]; /**< Firmware revision */
  Uint8    ModelNo[40];    /**< Model number */
  Uint8     IsAta;          /**< Is identified device an ATA device or ATAPI */
  Uint8     IsMaster;       /**< Discover just the Master device */
} ATA_DEV_Ident_t;

/**
 * \brief ATA/ATAPI request status return information
 *
 */
typedef struct
{
  int       Error;    /**< Error type */
  Uint32    ErrSect;  /**< Error Sector Number */
  Uint32    ErrByte;  /**< Byte Count at which error occured */
  Ptr       Buffer;   /**< Buffer Pointer */
} ATA_DEV_ReqStatus_t;

/**
 * \brief ATA/ATAPI device Event information
 *
 */
typedef struct
{
  Uint32    EventId;  /**< Event Type */
} ATA_DEV_Event_t;

/**
 * \brief ATA/ATAPI Request response type information
 *
 */
typedef struct
{
  ATA_DEV_ReqStatus_t    Status;  /**<  Status of Request completion */
  ATA_DEV_Event_t        Event;   /**<  Event infor on request completion */
} ATA_DEV_ReqInfo_t;

/**
 * \brief ATA/ATAPI Request response information
 *
 */
typedef struct
{
  Bool               IsEvent;  /**< Is the response a event or status */
  ATA_DEV_ReqInfo_t  ReqInfo;  /**< Response Information */
} ATA_DEV_Res_t;

/**
 * \brief ATA/ATAPI Raw command structure.
 *
 */
typedef struct {
  Uint8  Features;                    /**< ATA Features         */
  Uint8  SectorCnt;                   /**< Number of Sectors    */
  Uint8  LbaLow;                      /**< LBA Low bits         */
  Uint8  LbaMid;                      /**< LBA Mid bits         */
  Uint8  LbaHigh;                     /**< LBA High bits        */
  Uint8  Device;                      /**< Device Selection     */
  Uint8  Cmd;                         /**< ATA Command          */
  Uint8  Dummy [5];                   /**< Needed for ATAPI command */
} ATA_Dev_Cmd_t;

/**
 * \brief ATA/ATAPI Innterface driver Device operation table.
 *
 */
typedef struct {
  ATA_Io_t  Ata_Read;
  /**< Do a Block Read operation */
  ATA_Io_t  Ata_Write;
  /**< Do a Block write operation */
  int  (*Ata_SetDevMode) (Ptr AtaHandle, ATA_DEV_Mode_t Mode, Uint32 Level);
  /**< Set device operation mode
   *   \param AtaHandle  [IN]  Handle of the ATA/ATAPI interface
   *   \param Mode       [IN]  Opmode
   *   \param Level      [IN]  Level in the Opmode  */
  int  (*Ata_DevReset)   (Ptr AtaHandle);
  /**< Reset the Device
   *   \param AtaHandle  [IN]  Handle of the ATA/ATAPI interface
   */
  int  (*Ata_SetPwrMode) (Ptr AtaHandle, Ptr MedHndl, ATA_DEV_PwrMode_t Mode);
  /**< Set device power mode
   *   \param AtaHandle  [IN]  Handle of the ATA/ATAPI interface
   *   \param Mode       [IN]  Power mode
   *   \param MedHndl    [IN]  Media Handle  */
  int  (*Ata_SubAtapiReq)  (Ptr AtaHandle, Ptr MedHndl, Ptr pCmd);
  /**< Submit at Raw ATAPI command
   *   \param AtaHandle  [IN]  Handle of the ATA/ATAPI interface
   *   \param MedHndl    [IN]  Media Handle
   *   \param pCmd       [IN]  Packet Command  */
  int  (*Ata_GetMaxSect) (Ptr AtaHandle);
  /**< Get the maximum user accssible sector number
   *   \param AtaHandle  [IN]  Handle of the ATA/ATAPI interface */
  int  (*Ata_Ioctl) (Ptr AtaHandle, ATA_DEV_IoctlCmd_t Cmd, Ptr *pData);
  /**< ATA Device IOCTL
   *   \param AtaHandle  [IN]  Handle of the ATA/ATAPI interface
   *   \param Cmd        [IN]  IOCTL Command
   *   \param pData      [IN]  Data Buffer  */
}ATA_INT_Ops_t;



/**
 *  \brief ATA_MED_Callback_t () : Media Driver Callback function prototype.
 *
 *  \param Handle [IN]           : Error status.
 *  \param pRes [IN]             : Buffer pointer.
 *  \return Bool                 : TRUE/FALSE
 ******************************************************************************/
typedef Bool  (*ATA_MED_Callback_t)   (Ptr Handle, ATA_DEV_Res_t *pRes);

/**
 *  \brief ATA_MED_Register ()  : Identify the requested device on the ATA Bus and
 *                                register for that device (Media) with ATA Interface.
 *  \param pDeviceIdent [IN]    : Device Indentification Information.
 *  \param MediaCallback [IN]   : Callback routine for the registered Media Driver.
 *  \param pAtaIntOps [OUT]     : ATA Interface API's.
 *  \param pHandle    [OUT]     : Handle to device (to be used for future
 *                                transactions).
 *  \return int                 : Success/Error
 ******************************************************************************/
int ATA_MED_Register (ATA_DEV_Ident_t *pDeviceIdent, ATA_MED_Callback_t MediaCallback,
                        ATA_INT_Ops_t **pAtaIntOps, Ptr *pHandle);

/**
 *  \brief ATA_MED_UnRegister ()  : Free the requested device on the ATA Bus.
 * If the device has a active request we return ATA_ERR_GEN.  The caller
 * can either wait for this active request to complete or do a "DeInit"
 * (under critical power conditions) inspite of this error.  Care should be
 * taken by the caller not to free the outstanding request information as it
 * might complete at any point in time.  In this case if the caller is not
 * willing to wait then the caller can call "DeInit" of ATA and "Init of ATA"
 * subsystem.  Alternately if this command does not complete then the normal Timeout
 * mechanism kicks in and resets the bus to bring back the system to its
 * normal operating self.
 *
 *
 *  \param Handle [IN]            : Handle to device.
 *
 *  \return int                   : Error/Success
 ******************************************************************************/
int ATA_MED_UnRegister (Ptr Handle);

/**
 *  \brief ATA_Init ()          : Init ATA Interface Driver.
 ******************************************************************************/
void ATA_Init (void);

/**
 *  \brief ATA_DeInit ()          : DeInit ATA Interface Driver.
 ******************************************************************************/
void ATA_DeInit (void);

/**
 *  \brief ATA_DrvInit ()          : Init ATA Driver Interface Driver.
 ******************************************************************************/
void ATA_DrvInit (void);
/**
 *  \brief ATA_DrvInit () : Init the ATA driver.
 *
 *  \return void
 ******************************************************************************/
void ATA_DrvDeInit (void);

/**
 * \brief ATA_Suspend_IO : Suspend the IO on the device
 *
 * \param Handle [IN]  Device Handle for the device
 *
 * \return 0 on success error upon failure
 ******************************************************************************/
Int32 ATA_Suspend_IO(Ptr Handle);

/**
 * \brief ATA_Resume_IO : Resume the IO on the device
 *
 * \param Handle [IN]  Device Handle for the device
 *
 * \return 0 on success error upon failure
 ******************************************************************************/
Int32 ATA_Resume_IO(Ptr Handle);

#endif /* ATA_MED_H */

