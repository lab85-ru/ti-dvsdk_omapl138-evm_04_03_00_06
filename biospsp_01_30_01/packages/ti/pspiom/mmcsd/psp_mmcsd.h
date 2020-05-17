/*
 * psp_mmcsd.h
 *
 * This file contains Application programming interface for the MMCSD driver and
 * command/macro definitions used by the MMCSD driver.
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


/** \mainpage    PSP MMCSD Driver on BIOS
 *
 *  \section    Introduction
 *
 *  \subsection    xxx    Purpose and Scope
 *  The purpose of this document is to explain the MMCSD driver on the PSP
 *  Framework architecture written in DSP/BIOS for platform.
 *
 *  \subsection    bbb    MMCSD Controller
 *  The multimedia card (MMC)/secure digital (SD) card is used to provide
 *  removable data storage. The MMC/SD card controller provides an interface
 *  to external MMC and SD cards. The communication between the MMC/SD card
 *  controller and MMC/SD card(s) is performed according to the MMC/SD protocol.
 *  The MMCSD controller supports interface to multimedia cards (MMC), secure
 *  digital (SD) memory cards. The controller uses the MMC/SD protocol and
 *  Secure Digital Input Output (SDIO) protocol. The controller has 512-bit
 *  read/write FIFO to lower system overhead.
 *
 *  \subsection    ccc    DSP/BIOS System
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
 *  \file   ti/pspiom/mmcsd/psp_mmcsd.h
 *
 *  \brief  MMCSD interface definition
 *
 *  This file contains the interfaces, data types and symbolic definitions
 *  that are needed by the application to utilize the services of the MMCSD
 *  device driver. The functions named as PSP_xxx are also here. The reason
 *  being if there is a need to use the MMCSD driver directly without
 *  interfacing with the block media then these functions can be used. The APP
 *  layer should not use any other function.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#ifndef _PSP_MMCSD_H_
#define _PSP_MMCSD_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                             MACRO DEFINITIONS                              */
/* ========================================================================== */
#define MMCSD_CSD_SIZE_BYTE         16U
/**< CSD register size in bytes                                               */
#define MMCSD_CID_SIZE_BYTE         16U
/**< SD status size in bytes                                                  */
#define MMCSD_SCR_SIZE_BYTE         8U
/**< SCR register size in bytes                                               */
#define PSP_MMCSD_WAIT_FOREVER      0xFFFFFFFF
/**< Wait forever                                                             */

typedef Uint32 PSP_CardFrequency;
/**< MMCSD card frequency                                                     */

typedef Ptr PSP_MmcsdHandle;
/**< A handle to the MMCSD DDC                                                */

typedef Ptr PSP_MmcsdChannelHandle;
/**< A handle to the MMCSD channel                                            */

/* ========================================================================== */
/*                             MMCSD ENUMS                                    */
/* ========================================================================== */
/**\brief Following IOCTL commands will be used to perform control operation on
 *   MMCSD. The IOCTL command is passes as second argument in PSP_mmcsdStatus()
 *   function when the driver is used directly with the application. If the
 *   driver is used with block media please see the user guide on how to use
 *   these ioctls.                                                            */
typedef enum
{
    PSP_MMCSD_IOCTL_START = 0,
    /**< start the MMCSD channel
    *   This will start MMCSD channel for read/write. Only this ioctl command
    *   needs to be passed as second argument in PSP_mmcsdStatus() function.  */
    PSP_MMCSD_IOCTL_GET_CARDREGS,
    /**< Read the card registers
    *   This IOCTL will be used to read card registers of storage MMC/SD device
    *   Application has to pass pointer to PSP_MmcsdCardRegs structure variable
    *   as a third argument in PSP_mmcsdStatus() function. For example
    *   PSP_MmcsdCardRegs      cardRegs;
    *   cmdArg = (&cardRegs);                                                 */
    PSP_MMCSD_IOCTL_GET_BLOCKSIZE,
    /**< Get the blocksize of the card
    *   This IOCTL will be used to get number of bytes per sector of MMC/SD
    *   device. Application has to pass pointer to Uint32 variable as a
    *   third argument in PSP_mmcsdStatus() function. For example
    *   Uint32 Sect_Size;
    *   cmdArg = (&Sect_Size);                                                */
    PSP_MMCSD_IOCTL_CHECK_CARD,
    /**< Check if card is present.
    *   This IOCTL will be used to check which card is present (MMC or SD)
    *   Application has to pass pointer to PSP_MmcsdCardType structure variable
    *   as a third argument in PSP_mmcsdStatus() function. For example
    *   PSP_MmcsdCardType card_type;
    *   cmdArg = (&card_type);                                                */
    PSP_MMCSD_IOCTL_GET_OPMODE,
    /**< Get the opmode of the driver
    *   This IOCTL will be used to get operating mode of the MMCSD device.
    *   Application has to pass pointer to PSP_MmcsdOpMode enum variable
    *   as a third argument in PSP_mmcsdStatus() function. For example
    *   PSP_MmcsdOpMode opmode;
    *   cmdArg = (&opmode);                                                   */
    PSP_MMCSD_IOCTL_SET_CALLBACK,
    /**< Set the callback function for I/O completion
    *   This IOCTL will be used to set callback function which will be called
    *   after every read/write. Application has to pass PSP_MmcsdAppCallback
    *   type function pointer as a third argument in PSP_mmcsdStatus() function.
    *   This will be already used by BLK_MED so application should not use this,
    *   unless it is used for RAW mode of operation without using block media
    *   and file system
    *   PSP_MmcsdAppCallback cb;
    *   cmdArg = cb;                                                          */
    PSP_MMCSD_IOCTL_SET_HWEVENT_NOTIFICATION,
    /**< Set the hardware event notification callback
    *   This IOCTL will be used to set callback function which will be called
    *   for media insertion or removal, to notify upper layer about hardware
    *   events. Application has to pass PSP_MmcsdHwEventNotification type
    *   function pointer as a third argument in PSP_mmcsdStatus() function.
    *   This will be already used by BLK_MED so application should not use this,
    *   unless it is used for RAW mode of operation without using block media
    *   and file system
    *   For example
    *   PSP_MmcsdHwEventNotification callback;
    *   cmdArg = callback;                                                    */
    PSP_MMCSD_IOCTL_GET_HWEVENT_NOTIFICATION,
    /**< Get the hardware event notification callback
    *   This IOCTL will be used to get callback function which will be called
    *   for media insertion or removal, to notify upper layer about hardware
    *   events. Application has to pass address of PSP_MmcsdHwEventNotification
    *   type function pointer as a third argument in PSP_mmcsdStatus() function.
    *   For example
    *   PSP_MmcsdHwEventNotification callback;
    *   cmdArg = (&callback);                                                 */
    PSP_MMCSD_IOCTL_GET_CARD_SIZE,
    /**< Get the card storage capacity.
    *   This IOCTL returns size of MMC/SD card in bytes for all cards except for
    *   High capacity card. In the case of High capacity SD card, it is returned
    *   in KBytes and using IOCTL PSP_MMCSD_IOCTL_CHECK_HIGH_CAPACITY_CARD, it
    *   could be found whether it is high capacity or not. Application has to
    *   pass pointer to Uint32 variable as a third argument. For example
    *   Uint32 size_of_card;
    *   cmdArg = (&size_of_card);                                             */
    PSP_MMCSD_IOCTL_SET_TEMPORARY_WP,
    /**< Set the temporary write protection
    *   This IOCTL can be use to set temporary write protect state of MMC/SD
    *   card. Application has to pass true to Bool variable in argument.
    *   For example
    *   Bool writeprotect_state;
    *   writeprotect_state = 1;
    *   cmdArg = (&writeprotect_state);                                       */
    PSP_MMCSD_IOCTL_GET_TEMPORARY_WP,
    /**< Set the temporary write protection
    *   This IOCTL can be use to get temporary write protect state of MMC/SD
    *   card.Application has pass pointer to Bool variable in argument.
    *   For example
    *   Bool writeprotect_state;
    *   cmdArg = (&writeprotect_state);                                       */
    PSP_MMCSD_IOCTL_SET_PERMANENT_WP,
    /**< Set the permanent write protection
    *   This IOCTL can be use to set permanent write protect state of MMC/SD
    *   card. Application has to pass true to Bool variable in argument.
    *   CAUTION : This ioctl will make the card permanently write protected
    *   For example
    *   Bool writeprotect_state;
    *   writeprotect_state = 1;
    *   cmdArg = (&writeprotect_state);                                       */
    PSP_MMCSD_IOCTL_GET_PERMANENT_WP,
    /**< Set the permanent write protection
    *   This IOCTL can be use to get permanent write protect state of MMC/SD
    *   card. Application has pass pointer to Bool variable in argument.
    *   For example
    *   Bool writeprotect_state;
    *   cmdArg = (&writeprotect_state);                                       */
    PSP_MMCSD_IOCTL_CHECK_HIGH_CAPACITY_CARD,
    /**< Check this is high capacity card or not.This IOCTL will return true in
    *   cmdArg if it is high capacity card else false. Application has to pass
    *   pointer to Bool variable
    *   Bool highCapacity;
    *   cmdArg = (&highCapacity);                                             */
    PSP_MMCSD_IOCTL_GET_TOTAL_SECTORS,
    /**< This IOCTL returns size of MMC/SD card in sectors. Application has to
    *   pass address of Uint32 variable in cmdArg. For example
    *   Uint32 No_of_Sector;
    *   cmdArg = (&No_of_Sector);                                             */
    PSP_MMCSD_IOCTL_SET_EVENTQ,
    /**< Setting the TC value of the EDMA channel. This IOCTL can be used to set
    *   event queue of EDMA channel. Application has to pass
    *   PSP_MmcsdEdmaEventQueue type enum in cmdArg. For example
    *    cmdArg = (Ptr)PSP_MMCSD_EDMA3_EVENTQ_0;                              */
    PSP_MMCSD_IOCTL_SET_CARD_FREQUENCY,
    /**< Setting the frequency of card at which it is supposed to operate.
    *   If this IOCTL is not called then driver operates at maximum possible
    *   frequency. Please note that frequency set by user will be valid till
    *   card is not removed; it means that card frequency reverts back to max.
    *   possible once card is removed and re inserted. Frequency is supposed to
    *   be passed in Hz for this call. Driver will set user passed frequency or
    *   whatever is possible but lower than user passed frequency. Please note
    *   that even if this IOCTL is used for testing purpose the frequency is
    *   altered for all the read and write operations.
    *   Alplication has pass frequency in Hz. For example
    *   PSP_CardFrequency mmcFreq = 16000000
    *   cmdArg = (Ptr)(&mmcFreq);                                             */
    PSP_MMCSD_IOCTL_GET_CARD_VENDOR,
    /**< This IOCTL returns vendor id of MMCSD. Application has to
    *   pass address of Uint32 variable in cmdArg. For example
    *   Uint32 vendorId;
    *   cmdArg = (&vendorId);                                                 */
    PSP_MMCSD_IOCTL_GET_CONTROLLER_REG,
    /**< Read the controller register
    *   This IOCTL will be used to read a particular register offset of hw
    *   controller register of MMC/SD. Caution: The mmcsd driver may not work
    *   correctly if this ioctl is used during any read write or any comand
    *   transfer is going on as some of the registers in the controller are
    *   readonly and their value resets, once they are read.  Application has to
    *   pass register address offset and the place holder for value as a
    *   argument in PSP_mmcsdStatus() function. For example
    *   Uint32 offset;
    *   Uint32 value;
    *   cmdArg = (&offset);
    *   param  = (&value);                                                    */
    PSP_MMCSD_IOCTL_SET_CONTROLLER_REG,
    /**< Write the controller register
    *   This IOCTL will be used to write value to a particular register offset
    *   of hw controller register of MMC/SD.  Application has to pass register
    *   address offset and the place holder for value as a argument in
    *   PSP_mmcsdStatus() function. For example
    *   Uint32 offset;
    *   Uint32 value;
    *   cmdArg = (&offset);
    *   param  = (&value);                                                    */
    PSP_MMCSD_IOCTL_MAX_IOCTL
    /**< Book-keep - Max ioctl's                                              */
}PSP_MmcsdIoctl;
/**< The IOCTLS for MMCSD.                                                    */


/**
 * \brief Enumerations for driver operating modes
 *
 *  Driver operation modes enumberation
 */
typedef enum
{
    PSP_MMCSD_OPMODE_POLLED       = 0,
    /** Polled operation mode                                                 */
    PSP_MMCSD_OPMODE_INTERRUPT    = 1,
    /** Interrupt mode of operation. Not supported by the driver              */
    PSP_MMCSD_OPMODE_DMAINTERRUPT = 2
    /** DMA Mode of operation, DMA interrupts will be used for DMA completion */
}PSP_MmcsdOpMode;


/**\brief MMCSD error type*/
typedef enum
{
    PSP_MMCSD_RESULT_ERROR_FAILURE = -384,
    /**< activity resulted in a failure                                       */
    PSP_MMCSD_RESULT_ERROR_CANCELLED,
    /**< activity was accepted for execution & terminated at a
     *  later point of time                                                   */
    PSP_MMCSD_RESULT_ERROR_VOLTAGE_MISMATCH
    /**< card inserted does not match the supported voltage
     *  range                                                                 */
}PSP_MmcsdResult;

/**\brief MMCSD Card state*/
typedef enum
{
    PSP_MMCSD_HWEVENT_CARD_REMOVED = 1,
    /**< the existing card has been removed                                   */
    PSP_MMCSD_HWEVENT_DATA_CORRUPTED,
    /**< corruption detected in the card data transaction                     */
    PSP_MMCSD_HWEVENT_COMMAND_CORRUPTED,
    /**< corruption detected in the card command transaction                  */
    PSP_MMCSD_HWEVENT_ERROR_CARD_STATUS
    /**< error detected in card status                                        */
}PSP_MmcsdHwEvent;

/** Enumeration for card type */
typedef enum
{
    PSP_MMCSD_CARDTYPE_NONE = 0,
    /**< card not inserted                                                    */
    PSP_MMCSD_CARDTYPE_MMC,
    /**< card inserted is of type MMC                                         */
    PSP_MMCSD_CARDTYPE_SD
    /**< card inserted is of type SD                                          */
}PSP_MmcsdCardType;

/** EDMA event queue configuration params                                     */
typedef enum
{
    PSP_MMCSD_EDMA3_EVENTQ_0 = 0,
    /**< Select EDMA Event Queue 0                                            */
    PSP_MMCSD_EDMA3_EVENTQ_1
    /**< Select EDMA Event Queue 1                                            */
}PSP_MmcsdEdmaEventQueue;


/**
 * \brief  PLL domain to be used by the device
 */
typedef enum PSP_MmcsdPllDomain_t
{
    PSP_MMCSD_PLL_DOMAIN_0 = 0,
    /**< PLL domain 0       */

    PSP_MMCSD_PLL_DOMAIN_1 = 1
    /**< PLL domain 1       */
}PSP_MmcsdPllDomain;

/* ========================================================================== */
/*                             FUNCTION POINTERS                              */
/* ========================================================================== */
/** @brief Typedef for application callback
           MMCSD driver will give callback to application via Block
    media to notify that read/write has been completed.                       */
typedef Int32 (*PSP_MmcsdAppCallback)(Ptr buf, Int32 count);

/** @brief Typedef for Hardware event notification callback
           MMCSD driver will give call back to upper layer to notify
    about storage media inserted or removed                                   */
typedef Int32 (*PSP_MmcsdHwEventNotification) (PSP_MmcsdHwEvent hwEvent,
                                               Ptr arg);

/* ========================================================================== */
/*                             MMCSD STRUCTURES                               */
/* ========================================================================== */
/**
 * \struct PSP_MmcsdCardMmc
 *
 * \brief standard registers on MMC card
*/
typedef struct
{
    Uint32              ocr;
    /**< operating conditions register                                        */
    Uint8               cid[MMCSD_CID_SIZE_BYTE];
    /**< card identification register                                         */
    Uint8               csd[MMCSD_CSD_SIZE_BYTE];
    /**< card specific data register                                          */
    Uint16              rca;
    /**< relative card address register                                       */
    Uint16              dsr;
    /**< driver stage register                                                */
}PSP_MmcsdCardMmc;
/**< standard registers on MMC card                                           */


/**
 * \struct PSP_MmcsdCardSd
 *
 * \brief standard registers on SD card
*/
typedef struct
{
    Uint32              ocr;
    /**< operating conditions register                                        */
    Uint8               cid[MMCSD_CID_SIZE_BYTE];
    /**< card identification register                                         */
    Uint8               csd[MMCSD_CSD_SIZE_BYTE];
    /**< card specific data register                                          */
    Uint16              rca;
    /**< relative card address register                                       */
    Uint16              dsr;
    /**< driver stage register                                                */
    Uint8               scr[MMCSD_SCR_SIZE_BYTE];
    /**< SD configuration register                                            */
}PSP_MmcsdCardSd;

/** @brief MMCSD registers as per protocol definition                         */
typedef struct
{
    PSP_MmcsdCardType           cardType;
    /**< Type of card (protocol standard) inserted                            */
    PSP_MmcsdCardMmc            mmc;
    /**< standard registers on MMC card                                       */
    PSP_MmcsdCardSd             sd;
    /**< standard registers on SD card                                        */
}PSP_MmcsdCardRegs;
/**< Card register                                                            */

/** @brief MMCSD driver configuration                                         */
typedef struct
{
    PSP_MmcsdOpMode                     opMode;
    /**< Current OPMODE                                                       */
    Ptr                                 hEdma;
    /**< DMA Handle                                                           */
    PSP_MmcsdEdmaEventQueue             eventQ;
    /**< EDMA event Queue to be used for MMCSD                                */
    Uint16                              hwiNumber;
    /**< Hardware interrupt Number                                            */
    Bool                                pscPwrmEnable;
    /**< Control for module power management enable & disable                 */
    PSP_MmcsdPllDomain                  pllDomain;
    /**< Pll domain where the device is                                       */
}PSP_MmcsdConfig;

/** @brief MMCSD driver application callback function                         */
typedef struct
{
    PSP_MmcsdAppCallback appCallback;
    /**< app callback to be used for that channel                             */
}PSP_MmcsdChannelConfig;

/* ========================================================================== */
/*                          GLOBAL FUNCTIONS                                  */
/* ========================================================================== */
/**
 * \brief MMCSD Driver Initialize function
 *
 *  This function initializes the DDC and any other bound layers.
 *
 * \param   instNum         [IN]    instance number of the MMCSD controller
 * \param   hDDC            [OUT]   handle to the MMCSD controller
 * \param   pConfig         [IN]    EDMA configuration
 * \param   moduleInputClk  [IN]    Module Input frequency
 *
 * \return  IOM_COMPLETED if successful, else suitable error code.
 */
Int32 PSP_mmcsdInitialize(Uint32                 instNum,
                          PSP_MmcsdHandle* const hDDC,
                          PSP_MmcsdConfig* const pConfig,
                          Uint32                 moduleInputClk);


/**
 * \brief This function opens the driver and prepares for further transaction
 *
 *      This function Initializes all the required hardware and brings them
 *  to operational state.
 *
 * \param hDDC              [IN]    Handle to the DDC
 * \param hChannel          [OUT]   Handle to the channel
 * \param chnlConfig        [IN]    channel configuration
 *
 * \return IOM_COMPLETED if successful, else suitable error code
 */
Int32 PSP_mmcsdOpenChannel(PSP_MmcsdHandle               hDDC,
                           PSP_MmcsdChannelHandle* const hChannel,
                           PSP_MmcsdChannelConfig* const chnlConfig);

/**
 * \brief This function is called to close the MMCSD driver. This releases all
 *  the resources that were acquired during open.
 *
 * \param hChannel          [IN]    Handle to the channel
 *
 * \return IOM_COMPLETED if successful else suitable error code
 */
Int32 PSP_mmcsdCloseChannel(PSP_MmcsdChannelHandle hChannel);


/**
 * \brief This function is called by the application (through the driver entry)
 *  to read the data from the MMCSD.
 *
 * \param hChannel      [IN]    Handle to channel
 * \param mediaHandle   [IN]    Handle to MMC or SD media.
 * \param address       [IN]    start block address for the data read
 * \param bufPtr        [IN]    Pointer to the buffer, for the data read
 * \param bufSz         [IN]    Number of blocks to read from MMCSD
 * \param timeout       [IN]    Timeout value.
 *
 * \return the number of blocks read at the time of return if successful or
 * a suitable error code from Int32
 */
Int32 PSP_mmcsdInput(PSP_MmcsdChannelHandle      hChannel,
                     Ptr                         mediaHandle,
                     Uint32                      address,
                     Uint16*                     bufPtr,
                     Uint32                      bufSz,
                     Uint32                      timeout);


/**
 * \brief This function is called by the application (through the driver entry)
 *  to write data to MMCSD.
 *
 * \param hChannel      [IN]    Handle to channel
 * \param mediaHandle   [IN]    Handle to MMC or SD media.
 * \param address       [IN]    start block address for the data write
 * \param bufPtr        [OUT]   Pointer to the buffer, for the data write
 * \param bufSz         [IN]    Number of blocks to write to MMCSD
 * \param timeout       [IN]    Timeout value.
 *
 * \return the number of blocks written at the time of return if successful or
 * a suitable error code from Int32
 */
Int32 PSP_mmcsdOutput(PSP_MmcsdChannelHandle      hChannel,
                      Ptr                         mediaHandle,
                      Uint32                      address,
                      Uint16*                     bufPtr,
                      Uint32                      bufSz,
                      Uint32                      timeout);


/**
 * \brief This function checks whether card is present or not and returns
 *  card type
 *
 * \param cardType   [OUT]    returns cardtype
 * \param instanceId [IN]     instance number of the MMCSD controller
 *
 * \return IOM_COMPLETED if successful else suitable error code
 */
Int32 PSP_mmcsdCheckCard(PSP_MmcsdCardType* cardType, Uint32 instanceId);

/**
 * \brief This function handles the IOCTLs for the MMCSD driver.
 *  Application has to pass IOCTL command in cmd varible and IOCTL data in
 *  cmdArg variable.
 *
 * \param hChannel  [IN]    Handle to channel
 * \param cmd       [IN]    IOCTL Command (one of the PSP_MMCSD_IOCTL* commands)
 * \param arg       [INOUT] Argument for the IOCTL
 * \param param     [INOUT] Optional parameter for the IOCTL. Can be NULL.
 *
 * \return IOM_COMPLETED if successful else suitable error code
 */
Int32 PSP_mmcsdStatus(PSP_MmcsdChannelHandle      hChannel,
                      PSP_MmcsdIoctl              cmd,
                      Ptr const arg,
                      Ptr param);

/**
 * \brief This function terminates the driver instance and releases all
 *  resources currently consumed by the driver.This deinitialises the driver
 *  and restore all resourses
 *
 * \param hDDC      [IN]    Handle to DDC
 *
 * \return IOM_COMPLETED if successful else suitable error code
 */
Int32 PSP_mmcsdTerminate(PSP_MmcsdHandle hDDC);

/**
 *  \brief Init the MMCSD driver.
 *
 *  This function initializes the MMCSD driver. This API inittailizes the
 *  device through the corresponding PSP initialization call. On a successful
 *  initialization, the MMCSD driver is registered with the block media and
 *  proceeds to check the MMCSD device(s) on board.
 *
 *  \param moduleFreq   [IN]    MMCSD device frequency.
 *  \param instanceId   [IN]    Instance of device
 *  \param config       [IN]    EDMA Config Pointer
 *
 *  \return Error code or IOM_COMPLETED.
 */
Int32 PSP_mmcsdDrvInit(Uint32 moduleFreq,
                       Uint32 instanceId,
                       PSP_MmcsdConfig* const config);


/**
 * \brief Deinitialize the MMCSD device.
 *
 *  This function De-initialize the MMCSD media on the board/in the system,
 *  through the PSP's Deinit call. After the MMCSD device is de-initialized
 *  read,write and ioctl function for MMCSD through ERTFS will not work .
 *
 * \param instanceId   [IN]    Instance of device
 *
 * \return Error code or IOM_COMPLETED.
 */
Int32 PSP_mmcsdDrvDeInit(Uint32 instanceId);

#ifdef __cplusplus
}
#endif

#endif  /* _PSP_MMCSD_H_ */
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
