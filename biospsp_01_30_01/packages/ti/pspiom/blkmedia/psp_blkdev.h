/*
 * psp_blkdev.h
 *
 * This file contains Application programming interface for the Block Media 
 * driver and command/macro definitions used by the MMCSD driver.
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


/** \mainpage    PSP Block media Driver on BIOS 5.x
 *
 *  \section    Introduction
 *
 *  \subsection    xxx    Purpose and Scope
 *  The purpose of this document is to explain the Block media driver written
 *  for DSP/BIOS 5.x.
 *
 *  \subsection    aaa    Block Media Driver
 *  The Block Media Driveris a generic interface for all underlying storage
 *  drivers. The underlying storage drivers can be used with or without file
 *  system. The block media takes care for the accessing the storage drivers.
 *  The Block Media Driver is also written for working with File system(ERTFS
 *  in this case). Block media provides following key features:
 *  -# Block media provides Sync access for both File system as well as for
 *     Raw/Sector level access.
 *  -# Provides interfaces for RAW access for USB, NAND applications to talk to
 *     Storage Block devices in a uniform way.
 *  -# Provides support for big block sector sizes.
 *  -# Supports cache alignment on unaligned buffers from application.
 *  -# Provides Write Protect support, Removable media support.
 *
 *  \subsection    bbb    DSP/BIOS System
 *  DSP/BIOS is a real time kernel provided by TI to use with its
 *  DSP processors.
 *
 *  \subsection    ddd   Compiler defines to add/remove features
 *
 *  Define to turn OFF parameter checking.
 *  Use -DPSP_DISABLE_INPUT_PARAMETER_CHECK when building library
 *
 *  Define to turn OFF asserts. Use -DNDEBUG when building library.
 *
 *  \subsection    References
 *    -# DSP/BIOS driver development <b>SPRU616.pdf</b>
 *
 *  \file   ti/pspiom/blkmedia/psp_blkdev.h
 *
 *  \brief  Block media interface definition
 *
 *  This file contains the interfaces, data types and symbolic definitions
 *  that are needed by the application to utilize the serivces of the Block
 *  media device driver.
 */

#ifndef _PSP_BLKDEV_H_
#define _PSP_BLKDEV_H_

#include <std.h>

#ifdef __cplusplus
extern "C"
{
#endif


/* ========================================================================== */
/*                             MACRO DEFINITIONS                              */
/* ========================================================================== */

/**< This switch can be used to enable buffer alignment in block media driver
   If application passes unaligned buffer to read/write from storage media
   than block media aligns this buffer to cache line length and than passes
   it to storage driver.
 */
#define PSP_BUFF_ALIGNMENT

/**< Added for cache alignment, Keep this buffer Size as a Multiple of no of
bytes per sector. */
#ifdef PSP_BUFF_ALIGNMENT

/**< Buffer for IO Cache alignment                                            */
#define PSP_BUFFER_IO_SIZE          0x100000u

/**< Buffer for ASYNC Cache alignment                                         */
#define PSP_BUFFER_ASYNC_SIZE       0x7D000u

/**< multi tasking buffers are required or not for alignment                  */
/*#define PSP_MULTI_TASK_BUFFERS*/
#endif /* PSP_BUFF_ALIGNMENT */

/**< EDMA memcopy for IO read/write                                           */
#define PSP_BLK_EDMA_MEMCPY_IO

/**< EDMA memcopy for Async read/write                                        */
/*#define PSP_BLK_EDMA_MEMCPY_ASYNC */

/**< Max Block Devices supported                                              */
#define PSP_BLK_DEV_MAXDEV          PSP_BLK_DRV_MAX

/* ========================================================================== */
/*                      BLOCK MEDIA DATA TYPES                                */
/* ========================================================================== */
/**
 * \brief Block Interface Driver ID Definitions
 */
typedef enum
{
    PSP_BLK_DRV_MMC0  = 0,
    /**< MMC0 Driver Id                                                       */
#if defined(CHIP_C6748) || defined(CHIP_OMAPL138)
    PSP_BLK_DRV_MMC1,
    /**< MMC1 Driver Id                                                       */
#endif
    PSP_BLK_DRV_NAND,
    /**< NAND Driver Id                                                       */
	PSP_BLK_DRV_USB0,
	PSP_BLK_DRV_USB1,
#if defined(CHIP_C6748) || defined(CHIP_OMAPL138)
    PSP_BLK_DRV_ATA0,
    /**< ATA0 Driver Id 													  */
    PSP_BLK_DRV_ATA1,
    /**< ATA1 Driver Id                                                       */
#endif
    PSP_BLK_DRV_MAX
}PSP_BlkDrvId_t;



/**
 * \brief Enumerations for driver operating modes
 *
 *  Driver operation modes enumberation
 */
typedef enum
{
    PSP_OPMODE_POLLED       = 0,
    /**< Polled operation mode                                                */
    PSP_OPMODE_INTERRUPT,
    /**< Interrupt mode of operation                                          */
    PSP_OPMODE_DMAINTERRUPT,
    /**< DMA Mode of operation. DMA interrupts will be used for DMA completion*/
    PSP_OPMODE_UDMA,
    /**< Device in UDMA Mode                                                  */
    PSP_OPMODE_AUTO,
    /**< Device in AUTO Mode                                                  */
	PSP_OPMODE_DMA,
    /**< Device in DMA Mode                                                  */
	PSP_OPMODE_PIO,
    /**< Device in PIO Mode                                                  */
    PSP_OPMODE_OPMAX
    /**< Max opmode possible                                                  */
}PSP_BlkOpMode;

/**
 * \brief Block Interface Driver Event Definitions
 *  This table will be used in PSP_blkmediaCallback() function to define
 *  hardware event.
 */
typedef enum
{
    PSP_BLK_EVT_MEDREM = 0,
    /**< Media Removed                                                        */
    PSP_BLK_EVT_MEDINS,
    /**< Media Inserted                                                       */
    PSP_BLK_EVT_MEDWP,
    /**< Media Write Protected                                                */
    PSP_BLK_EVT_MEDRW,
    /**< Media is Writable                                                    */
    PSP_BLK_EVT_MAX
}PSP_BlkDevEvt_t;

/**
 * \brief Block Device request status information.
 */
typedef struct
{
    Int32     Error;
    /**< Error type                                                           */
    Uint32    ErrSect;
    /**< Error Sector Number                                                  */
    Uint32    ErrByte;
    /**< Byte Count at which error occurred                                   */
    Ptr       Buffer;
    /**< Buffer Pointer                                                       */
}PSP_BlkDevReqStatus_t;

/**
 * \brief Block device Event information
 *          This structure is used in call back function which prapogates
 *  hardware event form lower layer to upper layer. EventId can be either
 *  PSP_BLK_EVT_MEDINS or PSP_BLK_EVT_MEDREM of type PSP_BlkDevEvt_t. EvtData
 *  will be used only if event is PSP_BLK_EVT_MEDREM. caller has to set EvtData
 *  by device handle which was returned in driver open call.
 *
 */
typedef struct
{
    PSP_BlkDevEvt_t     EventId;
    /**< Event Type                                                           */
    Ptr                 EvtData;
    /**< Event Data                                                           */
}PSP_BlkDevEvent_t;

/**
 * \brief Block Request response type information
 *
 */
typedef struct
{
    PSP_BlkDevReqStatus_t       Status;
    /**<  Status of Request completion                                        */
    PSP_BlkDevEvent_t           Event;
    /**<  Event info on request completion                                    */
}PSP_BlkDevReqInfo_t;

/**
 * \brief Block Request response information
 * This atructure will be used in BLK_DEV_CallBack() function.
 *
 */
typedef struct
{
    Bool                    IsEvent;
    /**< Is the response a event or status                                    */
    PSP_BlkDevReqInfo_t     ReqInfo;
    /**< Response Information                                                 */
}PSP_BlkDevRes_t;

/**
 * \brief Block device Identify information
 *
 */
typedef struct
{
    Uint8    SerialNo[20];
    /**< Device Serial Number                                                 */
    Uint16   Cyl;
    /**< Number of Cyl                                                        */
    Uint16   Head;
    /**< Number of Heads                                                      */
    Uint16   sectPerTrack;
    /**< Sectors Per Track                                                    */
}PSP_BlkDevIdent_t;

/**
 * \brief Block Interface Driver IOCTL Definitions
 *  Application has to pass pointer to PSP_BlkDevIoctlInfo_t structure variable
 *  in PSP_blkmediaDevIoctl() function. For all IOCTL, Cmd element of this
 *  structure will be IOCTL command.
 *
 */
typedef enum
{
    PSP_BLK_GETSECTMAX = 0,
    /**< Get the Max Sector information
       Application has to pass address of Uint32 variable in pData element of
       PSP_BlkDevIoctlInfo_t structure. PSP_blkmediaDevIoctl() function will
       fill this variable by total number of available sector on the storage
       device. For Example
       pData = (Ptr)(&sectCnt)                                                */
    PSP_BLK_GETBLKSIZE,
    /**< Get the Block Size of one Sector
       Application has to pass address of Uint32 variable in pData element of
       PSP_BlkDevIoctlInfo_t structure. PSP_blkmediaDevIoctl() function will
       fill size of a sector in this element (Number of bytes per sector).
       For Example
       pData = (Ptr)(&blockSize)                                              */
    PSP_BLK_SETPWRMODE,
    /**< Set the Power mode for the device
       Currently this IOCTL is  not supported in any driver.                  */
    PSP_BLK_SETOPMODE,
    /**< Set the Operating Mode for the device
       Application has to pass operational mode to be set in pData element of
       PSP_BlkDevIoctlInfo_t structure. For Example
       PSP_BlkOpMode operationMode = PSP_OPMODE_DMAINTERRUPT;
       pData = (Ptr)(&operationMode);
       And to set in DMA mode, Device driver(e.g. NAND driver) should have
       EDMA device handle, channel, etc in some other method, say at init time*/
    PSP_BLK_GETOPMODE,
    /**< Get the Operating Mode of the device
       Application has to pass address of PSP_BlkOpMode type variable in
       pData element of PSP_BlkDevIoctlInfo_t structure. For Example
       PSP_BlkOpMode operationMode;
       pData = (Ptr)(&operationMode);                                         */
    PSP_BLK_DEVRESET,
    /**< Reset the block device
       Currently this IOCTL is  not supported in any driver.                  */
    PSP_BLK_GETWPSTAT,
    /**< Get the media write protect status
       This IOCTL is only supported in MMCSD driver. It returns write protect
       status of media in Bool variable. For Example
       Bool writeProtect;
       pData = (Ptr)(&writeProtect);                                          */
    PSP_BLK_GETREMSTAT,
    /**< Is device Removable or not
       This IOCTL is only supported in MMCSD driver. This IOCTL can be used to
       check whether device is removeable or not? MMCSD will always returm
       TRUE in Bool variable. For Example
       Bool isRemovable;
       pData = (Ptr)(&isRemovable);                                           */
    PSP_BLK_SETEVENTQ,
    /**< Set Event queue of EDMA channel
       This IOCTL can be used to set Event queue of EDMA channel if device is
       operating in EDMA mode of operation For Example
       PSP_Nand_Edma_EventQueue eventQueue;
       eventQueue = PSP_NAND_EDMA3_EVENTQ_1;
       pData = (Ptr)(&eventQueue);                                            */
    PSP_BLK_IOCTL_MAX
}PSP_BlkDevIoctl_t;

/**
 * \brief Block driver Interface IOCTL Definitions
 *  Application has to pass pointer to PSP_BlkDrvIoctlInfo_t structure variable
 *  in PSP_blkmediaDrvIoctl() function. For all IOCTL, Cmd element of this
 *  structure will be IOCTL command. Application has to pass address of variable
 *  which contains Device name on which control operation has to be performed.
 *
 */
typedef enum
{
    PSP_BLK_DRV_SETRAWDEV = 0,
    /**< Set device for raw access
    This IOCTL can be used to set a device to be used for RAW acces to the
    media driver. For Example
    PSP_BlkDrvId_t driverDev = PSP_BLK_DRV_MMC0;
    drvIoctlInfo.pData = (Ptr)(&driverDev);                                   */
    PSP_BLK_DRV_GETRAWDEV,
    /**< Get the current device set for raw access
    This IOCTL can be used to get the device which is currently set for RAW
    acces of the storage media. For Example
    PSP_BlkDrvId_t device;
    pData = (Ptr)(&device);                                                   */
    PSP_BLK_DRV_SET_INIT_COMP_CALLBACK,
    /**< Set init completion call back function for device
    This IOCTL can be used to set call back function of driver which will
    be called by block media when device is registered with file system.
    User has to pass function pointer ss a argument. The function should be of
    type BLK_DRV_InitCallback_t. For Example
    pData = (Ptr)(&mmcsdDetectCallback);                                      */
    PSP_BLK_DRV_IOCTL_MAX
}PSP_BlkDrvIoctl_t;

/**
 * \brief Block Device IOCTL structure
 *        This structure contains control command and data if required to
 * perform control operation. Cmd variable is used to pass command, pData
 * will be used to pass data which is required to perform control operation
 * and pData1 can be used to pass secondary data if required, currently this
 * variable is not used in any ioctl but it is there for feture use.
 */
typedef struct
{
    PSP_BlkDevIoctl_t   Cmd;
    /**< IOCTL command                                                        */
    Ptr                 *pData;
    /**< IOCTL Data                                                           */
    Ptr                 *pData1;
    /**< IOCTL Data                                                           */
}PSP_BlkDevIoctlInfo_t;

/**
 * \brief Block Driver IOCTL structure
 *        This structure contains control command and data if required to
 * perform control operation. Cmd variable is used to pass command, pData
 * will be used to pass data which is required to perform control operation
 * and pData1 can be used to pass secondary data if required, currently this
 * variable is not used in any ioctl but it is there for feture use.
 *
 */
typedef struct
{
    PSP_BlkDrvIoctl_t   Cmd;
    /**< IOCTL command                                                        */
    Ptr                 *pData;
    /**< IOCTL Data                                                           */
    Ptr                 *pData1;
    /**< IOCTL Data                                                           */
}PSP_BlkDrvIoctlInfo_t;

/* ========================================================================== */
/*                          GLOBAL FUNCTIONS                                  */
/* ========================================================================== */
/**
 * \brief  Block  Driver interface Device Callback function Prototype
 *         This function will be called after every read/write operation.
 *
 * \param  handle   :  Handle for the request
 * \param  pErrInfo :  Pointer to Error Info
 * \return Bool     :  TRUE
 ******************************************************************************/
typedef Bool (*PSP_blkDevCallback_t) (Ptr handle,
                                      PSP_BlkDevRes_t const *pErrInfo);

/**
 * \brief Block device IO function Definition
 *
 *  Block device IO function type definitions.
 *
 * \param handle        [IN]  Device Handle for the device
 * \param mediaHandle   [IN]  Media Driver Handle for this device
 * \param buffer        [IN]  Buffer Pointer
 * \param sectNum       [IN]  Sector Number to start
 * \param numSect       [IN]  Number of sectors to transfer
 ******************************************************************************/
typedef Int32 (*BLK_DEV_Io_t) (Ptr handle,
                               Ptr mediaHandle,
                               Ptr buffer,
                               Uint32 sectNum,
                               Uint32 numSect);

/**
 * \brief Block Device Interface driver Device operation table. Variable of this
 *        structure has to be passed in PSP_blkmediaAppRegister function to get
 *        address of read/write and ioctl function.
 *
 */
typedef struct
{
    BLK_DEV_Io_t  Blk_Read;
    /**< Do a Block Read operation */
    BLK_DEV_Io_t  Blk_Write;
    /**< Do a Block write operation */
    Int32  (*Blk_Ioctl) (Ptr handle,
                         Ptr mediaHandle,
                         PSP_BlkDevIoctlInfo_t const *pIoctlInfo,
                         Bool *pIsComplete);
    /**< Block Device IOCTL - It's a Sync call.
    *   \param handle        [IN]  Handle of the Block Device
    *   \param mediaHandle   [IN]  Job Handle from this module
    *   \param pIoctlInfo    [IN]  IOCTL information
    *   \param pIsComplete   [OUT] Optional; It is the IOCTL complete status
    */
}PSP_BlkDevOps_t;

/** \name Blk-Dev Register Information
 */
/*
 *  \brief BLK_DEV_Register ()  : Storage device driver register function for
 *                                the block device. Storage device has to pass
 *                                address of this function in
 *                                PSP_blkmediaDrvRegister function. When media
 *                                is detected and storage driver gives callback
 *                                to block media with event
 *                                of media inserted, block media will call this
 *                                function to get address of read and write
 *                                function of storage device.
 *  \param pIdent     [IN]      : Identification information.
 *  \param blockDevCb [IN]      : Callback routine for the registered block
                                  Device. For handling events specific to a
                                  storage driver. After comlpetion of every
                                  read/write operation this function will be
                                  called.
 *  \param pDevOps    [OUT]     : Block Device Interface Operations.
 *  \param pHandle    [OUT]     : Handle to device (to be used for future
 *                                transactions).
 *  \return Int32               : Success/Error
 ******************************************************************************/
typedef Int32 (*PSP_blkDevRegisterFunc_t) (PSP_BlkDevIdent_t *pIdent,
                                           PSP_blkDevCallback_t blockDevCb,
                                           PSP_BlkDevOps_t **pDevOps,
                                           Ptr *pHandle);

/**
 *  Signature of call back function of media which will be called by
 *  block media after device has been registered with filesystem.
 */
typedef Void (*BLK_DRV_InitCallback_t) (Ptr);

/**
 * \brief Block Driver Register Information
 *
 */
typedef struct
{
    PSP_blkDevRegisterFunc_t    DeviceReg;
    /**< Storage Driver Device Register Function. This function will be called
    by block media driver when media will be detected at hardware layer       */
}PSP_BlkDrvReg_t;


/* ========================================================================== */
/*                   BLOCK MEDIA PUBLIC FUNCTIONS                             */
/* ========================================================================== */

/**
 *  \brief This function initialize the block media driver, take the resources,
 *         initialize the data structure and create a block media task for
 *         storage driver registration. Block media needs to be intialized
 *         before any intialization to storage driver(if block media is used to
 *         access the storage driver). This function also intialize the file
 *         system(if supported). This function needs to be called only once
 *         from the application and not to be called for every storage media
 *         init.
 *
 *  \param hEdma      [IN] EDMA driver handle
 *  \param edmaEventQ [IN] EDMA Event Queue number to be used for Blk-Media
 *  \param taskPrio   [IN] Block media task priority. The priority should be
 *                         greater than any other storage task priority. The
 *                         value should be in supported rage of OS
 *  \param taskSize   [IN] Stack size for Blk-Media task. Minimum 4Kbytes
 *
 *  \return Int32      : Success/Error
 ******************************************************************************/
Int32 PSP_blkmediaDrvInit(Ptr hEdma,
                          Uint32 edmaEventQ,
                          Uint32 taskPrio,
                          Uint32 taskSize);


/**
 *  \brief This function de-initialize the Block Media Driver. This function
 *         de-allocates any resources taken during init and deletes the task
 *         created during init. The function also frees the EDMA channel
 *         allocated during init. This function also de-init the file system
 *         (if supported).
 *
 *  \return Int32      : Success/Error
 ******************************************************************************/
Int32 PSP_blkmediaDrvDeInit(Void);


/**
 *  \brief This function registers the storage driver with Block Media Driver.
 *         Storage driver will call this function during initialization of the
 *         device with a function pointer which can be called as soon as device
 *         is detected to get the read write and ioctl pointers of the device.
 *         The same parameter is set to NULL during de-init of a storage device.
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
                              PSP_BlkDrvReg_t const *pRegInfo);

/**
 *  \brief Block Driver Callback interface. This function is used for
 *         propagating events from the underlying storage drivers to the block
 *         driver independent of the device context (Ex. Device
 *         insertion/removal, media write protected).
 *
 *  \param driverId   [IN]  : Id of the Storage Driver
 *  \param pEventInfo [IN]  : Storage Driver Device Event information.
 *
 *  \return Void             : None
 ******************************************************************************/
Void PSP_blkmediaCallback(PSP_BlkDrvId_t driverId,
                          PSP_BlkDevEvent_t const *pEventInfo);

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
                           PSP_BlkDevIoctlInfo_t *pIoctl);

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
Int32 PSP_blkmediaDrvIoctl(Ptr *pDevName, PSP_BlkDrvIoctlInfo_t *pIoctl);


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
 *                         will be use by application for read, write and ioctl
 *                         functions of storage device.
 *  \param pHandle [OUT] : Block Driver Device Handle for the storage device.
 *                         This will be the first arg of read, write and ioctl
 *                         functions called by the application.
 *
 *  \return Int32        : IOM_EBADARGS/IOM_COMPLETED.
 ******************************************************************************/
Int32 PSP_blkmediaAppRegister(PSP_blkDevCallback_t  appCb,
                              PSP_BlkDevOps_t **pIntOps,
                              Ptr *pHandle);


/**
 *  \brief The Media Driver clients like Mass Storage drivers shall use this
 *         function to un-register from a Block device. Should be called only in
 *         task context.
 *
 *  \param handle [IN]  : Block Media Device handle.
 *
 *  \return Int32       : IOM_EBADARGS/IOM_COMPLETED
 ******************************************************************************/
Int32 PSP_blkmediaAppUnRegister(Ptr handle);

#ifdef __cplusplus
}
#endif

#endif /* _PSP_BLKDEV_H_ */
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
