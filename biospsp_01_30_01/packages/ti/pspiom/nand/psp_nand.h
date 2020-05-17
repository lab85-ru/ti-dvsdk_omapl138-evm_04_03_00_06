/*
 * psp_nand.h
 *
 * This file contains Application programming interface for the NAND driver and
 * command/macro definitions used by the NAND driver.
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

/** \mainpage    PSP NAND Driver on BIOS
 *
 *  \section    Introduction
 *
 *  \subsection    xxx    Purpose and Scope
 *  The purpose of this document is to explain the NAND driver on the PSP
 *  Framework architecture written in DSP/BIOS for platform.
 *
 *
 *  \subsection    bbb    EMIFB Controller
 *  EMIFA memory controller is complaint with the JESD21-C SDR SDRAM memories
 *  utilizing 16-bit data bus of EMIFA memory controller.The purpose of the
 *  EMIFA is to provide a means for the CPU to connect to a variety of external
 *  devices including:
 *  -# Single data rate (SDR) SDRAM
 *  -# Asynchronous devices including NOR Flash, NAND Flash, and SRAM
 *
 *  The most common use for the EMIFA is to interface with NAND flash.
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
 *  \file   ti/pspiom/nand/psp_nand.h
 *
 *  \brief  NAND device driver application interface.
 *
 *  This file contains all the interfaces required to operate on the NAND
 *  device. It provides function for erase, write, read and initialize.
 *
 *  (C) Copyright 2008,Texas Instruments, Inc
 *
 */

#ifndef _PSP_NAND_H
#define _PSP_NAND_H

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>

#ifdef __cplusplus
extern "C"
{
#endif
/* ========================================================================== */
/*                          MACRO DEFINTIONS                                  */
/* ========================================================================== */
/**\brief Number of instances supported by the driver.                        */
#define PSP_NAND_INSTANCES           (1u)

/**\brief Number of blocks reserved for FTL operation. This macro defines the
 *        number of blocks that are reserved for replacing with bad blocks
 *        generated during runtime. This constant can be modified as per
 *        application requirement.
 *        Note: There should be atleast one reserved block.
 */
#define PSP_NAND_RESERVED_BLOCKS     (24u)

/**\brief Maximum number of pages in a block supported by the driver. The value
 *        of this constant can be changed as per the requirement. For example,
 *        if the driver will be used only with 32 pages per block device, then
 *        this constant can be set to 32.
 */
#define PSP_NAND_MAX_PAGES_IN_BLOCK  (128u)

/**\brief Maximum number of cache lines used by the cache module.
 *        Rule 1: The number of cache lines should be a power of 2. Note: The
 *                driver does not validate if the value of this macro is power
 *                of 2. So it is the responsibility of the driver user to
 *                specify a power of 2 value for this macro.
 *        Rule 2: The number of cache lines should not be less than 4.
 */
#define PSP_NAND_MAX_CACHE_LINES     (8u)

/**\brief Maximum page size supported by the driver. The value of this can be
 *        changed as per the requirement. For example, if the driver will be
 *        used only with 512 byte page devices, then this constant can be set
 *        to 512.
 */
#define PSP_NAND_MAX_PAGE_SIZE       (2048u)

/**\brief Base error code for NAND driver */
#define PSP_NAND_E_BASE              (-20)

/**\brief The following are the NAND error values */
#define PSP_NAND_E_WRITE_PROTECTED  (PSP_NAND_E_BASE - 1)
/**< A write protect error occured */
#define PSP_NAND_E_WRITE_FAIL       (PSP_NAND_E_BASE - 2)
/**< A write fail error occured */
#define PSP_NAND_E_READ_FAIL        (PSP_NAND_E_BASE - 3)
/**< A read fail error occured */
#define PSP_NAND_E_ERASE_FAIL       (PSP_NAND_E_BASE - 4)
/**< A erase fail error occured */
#define PSP_NAND_E_ERROR            (PSP_NAND_E_BASE - 5)
/**< General error occured */
#define PSP_NAND_E_NO_FREE_BLOCK    (PSP_NAND_E_BASE - 6)
/**< No free block error occured */
#define PSP_NAND_E_INIT_FAILED      (PSP_NAND_E_BASE - 7)
/**< Initialization fail error occured */
#define PSP_E_TIMEOUT               (PSP_NAND_E_BASE - 8)
/**< A timeout error occured */
#define PSP_NAND_E_ECC_FAIL         (PSP_NAND_E_BASE - 9)
/**< An un-correctable ECC error occured */

/**\brief Maximum number of logical blocks that can be managed by FTL module.
          The value of this constant can be changed as per the requirement. For
          example, if the driver is used with a NAND device that has only 2048
          blocks, then this constant can be set to 2048. */
#define PSP_NAND_FTL_MAX_LOG_BLOCKS   (4096u)

/**\brief Maximum number of physical blocks that can be managed by FTL module.
          The value of this constant can be changed as per the requirement.
          For example, if the driver is used with a NAND device that has only
          2048 blocks, then this constant can be set to 2048. */
#define PSP_NAND_FTL_MAX_PHY_BLOCKS   (4096u)


/* ========================================================================== */
/*                            NAND DATA TYPES                                 */
/* ========================================================================== */

/**
 * \brief Enumerations for driver operating modes
 *
 *  Driver operation modes enumberation
 */
typedef enum
{
    /** Polled operation mode       */
    PSP_NAND_OPMODE_POLLED       = 0,

    /** Interrupt mode of operation - Not Supported */
    PSP_NAND_OPMODE_INTERRUPT    = 1,

    /** DMA Mode of operation, DMA interrupts will be used for DMA completion */
    PSP_NAND_OPMODE_DMAINTERRUPT = 2

}PSP_NandOpMode;


/**
 *  \brief  Type of NAND device
 *
 *  This enum specfies the type of the NAND device.
 */
typedef enum PSP_nandType_t
{
    PSP_NT_NONE,    /**< No NAND Present                                   */
    PSP_NT_NAND,    /**< Normal NAND flash (512/2048-byte page SLC or MLC) */
    PSP_NT_ONENAND  /**< OneNAND flash type  - Not Supported               */
}
PSP_nandType;
/**< Type of NAND device */

/**
 *  \brief  PSP NAND Ioctl commands
 *
 *  Application has to pass IOCTL command(PSP_NandIoctlCmd) as a second
 *  parameter in PSP_nandIoctl() function.
 *
 *  NAND Ioctl commands
 */
typedef enum PSP_nandIoctlCmd_t
{
    PSP_NAND_IOCTL_GET_NAND_SIZE = 0u,
    /**< Get the size of NAND. Application has to pass address of Uint32
         variable in cmdArg parameter. For example:
         Uint32 numSector;
         cmdArg = (&numSector);                                               */

    PSP_NAND_IOCTL_GET_SECTOR_SIZE,
    /**< Get sector size of NAND. Application has to pass address of Uint32
         variable in cmdArg variable(third argument). For example:
         Uint32 bytesPerSector;
         cmdArg = (&bytesPerSector);                                          */

    PSP_NAND_IOCTL_SET_EVENTQ,
    /**< Set EDMA channel event queue. NOTE: This IOCTL should not be invoked
         when there is an active write or read from the NAND device is in
         progress. This IOCTL can be used to set event queue of EDMA channel.
         Application has to pass address of a Uint32 variable that contains
         the EDMA event queue number in the cmdArg parameter. For example:
         Uint32 edmaQueue;
         cmdArg = (&edmaQueue);                                               */

    PSP_NAND_IOCTL_ERASE_BLOCK,
    /**< Erase NAND flash logical pages. Application has to pass the starting
         logical page number and the number of consectuive logical pages to
         be erased. Note: The physical blocks in the NAND device are only
         erased if all the logical pages mapped into the physical block
         have to be erased. Partial erasing of blocks is not performed.
         For example,
         Uint32 startLogPage;
         Uint32 numLogPages;
         cmdArg = (&startLogPage);
         param  = (&numLogPage);                                              */

    PSP_NAND_IOCTL_GET_OPMODE,
    /**< Get the current operating mode of the NAND driver. Application has
         to pass address of the PSP_NandOpMode variable. The driver will return
         the current operating mode in that variable.                         */

    PSP_NAND_IOCTL_GET_DEVICE_INFO,
    /**< Get the current NAND device configuration. Application has to pass
         address of the PSP_nandDeviceInfo structure. For example,
         PSP_nandDeviceInfo  devInfo;
         cmdArg = (Ptr)(&devInfo);                                            */

    PSP_NAND_IOCTL_MAX_IOCTL
    /**< Book-keep - Max ioctl's                                              */
}
PSP_nandIoctlCmd;
/**< PSP NAND Ioctl commands */


/**
 * \brief  PLL domain to be used by the device
 */
typedef enum PSP_nandPllDomain_t
{
    PSP_NAND_PLL_DOMAIN_0 = 0,
    /**< PLL domain 0       */

    PSP_NAND_PLL_DOMAIN_1 = 1
    /**< PLL domain 1       */
}PSP_nandPllDomain;



/**
 *  \brief NAND device organization information
 *
 * This structure defines the organization of the NAND device. Caution: The
 * driver does not validate the contents of this structure supplied by the
 * application. So it should be ensured by the application that valid values
 * are supplied for this structure.
 */
typedef struct PSP_nandDeviceInfo_t
{
    Uint16   vendorId;       /**< Vendor/Manufacturer/Maker ID of NAND device */
    Uint16   deviceId;       /**< Device ID of the NAND device                */
    Uint32   pageSize;       /**< Size of one page. Note: Only 512 and 2048
                                  bytes per page is supported.                */
    Uint32   pagesPerBlock;  /**< Number of pages per block                   */
    Uint32   numBlocks;      /**< Number of blocks in the NAND device         */
    Uint8    spareAreaSize;  /**< Size of spare area of each page             */
    Uint8    dataBusWidth;   /**< Data bus width of the NAND device           */
}
PSP_nandDeviceInfo;
/**< NAND device organization information */

/**
 *  \brief NAND device timing information.
 *
 *  This structure defines the timing properties of the NAND device.
 *  The EMIF timing registers will be configured based on the timing values
 *  listed in this structure. Caution: The driver does not validate the
 *  contents of this structure supplied by the application. So it should be
 *  ensured by the application that valid values are supplied for this
 *  structure.
 */
typedef struct PSP_nandDeviceTiming_t
{
    Uint16   vendorId;     /**< Vendor/Manufacturer/Maker ID of NAND device */
    Uint16   deviceId;     /**< Device ID of the NAND device                */
    Uint8    writeSetup;   /**< Write setup time in ns                      */
    Uint8    writeStrobe;  /**< Write strobe time in ns                     */
    Uint8    writeHold;    /**< Write hold time in ns                       */
    Uint8    readSetup;    /**< Read setup time in ns                       */
    Uint8    readStrobe;   /**< Read strobe time in ns                      */
    Uint8    readHold;     /**< Read hold time in ns                        */
    Uint8    turnAround;   /**< Turnaround time in ns                       */
}
PSP_nandDeviceTiming;
/**< NAND device timing information */


/**
 * \brief NAND driver initialization configuration paramaters.
 *
 * This structure defines the NAND driver initializaion parameters. The
 * instance of this structure is supplied to the NAND driver initialization
 * interface.
 */
typedef struct PSP_nandConfig_t
{
    Uint32                 inputClkFreq;
    /**< EMIF Input clock frequency. Note: The driver does not validate the
         value of this parameter. It is upto the application to provide
         valid EMIF clock frequency.                                       */

    PSP_nandType           nandType;
    /**< Type of NAND flash. (NAND or OneNAND). Note: For J2, the value only
         of this parameter supported is PSP_NT_NAND. */

    PSP_NandOpMode         opMode;
    /**< Mode of operation of NAND controller. Note: Only polled mode and
         DMA mode are supported.                                           */

    Bool                   eraseAtInit;
    /**< If TRUE - Erase the complete NAND flash during initilization,
         otherwise, NAND is not erased during initialization.              */

    Uint32                 protectedBlocks;
    /**< Number for blocks to be reserved and not accessible by the driver.
         Note: It is advisible not to change for every initialization since
         the master boot record (partition table) need to be created every
         time this value is changed.                                       */

    Ptr                    hEdma;
    /**< EDMA driver handle. Required if opMode is DMA.                    */

    Uint32                 edmaEvtQ;
    /**< EDMA event queue number for DMA Mode of operation. Note: The driver
         does not validate the EDMA event queue number specified. So the
         application has to ensure that a valid EDMA event queue number is
         specified.                                                        */

    Uint32                 edmaChannel;
    /**< eDMA channel number to use. Note: The driver does not validate the
         number of the EDMA channel number specified. So application has to
         ensure that a valid EDMA channel number is supplied.              */

    PSP_nandDeviceInfo     *nandDevInfo;
    /**< Specify the NAND organization data if application would like to
         specify the NAND organization. This is an optional parmater. If not
         used, set this element to NULL. If set to NULL, the driver will
         read the device ID of the NAND and identify the device
         organization.                                                     */

    PSP_nandDeviceTiming   *nandDevTiming;
    /**< Specifiy the write/read hold/setup/strobe times. This is an optional
         parameter. If not used, set this element to NULL. If this element is
         NULL, the driver will use its internal values for hold/setup and
         strobe time.                                                      */

    Bool                   pscPwrmEnable;
    /**< Control for module power management enable & disable              */

    PSP_nandPllDomain      pllDomain;
    /**< Pll domain where the device is                                    */
}
PSP_nandConfig;
/**< NAND driver initialization configuration paramaters */

/* ========================================================================== */
/*                          GLOBAL FUNCTIONS                                  */
/* ========================================================================== */

/**
 * \brief  Initializes the NAND driver.
 *
 *         This function initializes the NAND driver instance. It detects the
 *         NAND device and prepares the FTL and LLC modules.
 *
 * \param  instanceId [IN]  Instance ID of the NAND driver.
 * \param  config     [IN]  Initialization parameter of the NAND driver.
 *
 * \return IOM_COMPLETED, on success.
 */
extern Int32 PSP_nandInit(Uint32 instanceId, const PSP_nandConfig  *config);

/**
 * \brief  Open a NAND driver instance.
 *
 *         This function opens a NAND driver instance. It validates the current
 *         state of the driver first and then sets the state of the driver as
 *         opened.
 *
 * \param  instanceId [IN]  Instance ID of the NAND driver.
 *
 * \return On success, return the handle of the NAND driver instance
 */
extern Ptr PSP_nandOpen(Uint32 instanceId);


/**
 * \brief  Write multiple sectors from the NAND device.
 *
 *         This function writes multiple sectors from the NAND device. The
 *         starting sector number and the number of sectors to write have to be
 *         specified.
 *
 * \param  handle      [IN]  Handle of the NAND drive instance
 * \param  logSector   [IN]  Logical sector number
 * \param  numSectors  [IN]  Number of sectors to write
 * \param  data        [IN]  Data buffer pointer.
 *
 * \return IOM_COMPLETED, on success
 */
extern Int32 PSP_nandWrite(Ptr          handle,
                           Uint32       logSector,
                           Uint32       numSectors,
                           Uint8        *data);


/**
 * \brief  Read multiple sectors from the NAND device.
 *
          This function reads multiple sectors from the NAND device.The starting
 *         sector number and the number of sectors to read have to be specified.
 *
 * \param  handle      [IN]  Handle of the NAND drive instance
 * \param  logSector   [IN]  Logical sector number
 * \param  numSectors  [IN]  Number of sectors to read
 * \param  data        [IN]  Data buffer pointer.
 *
 * \return IOM_COMPLETED, on success
 */
extern Int32 PSP_nandRead(Ptr          handle,
                          Uint32       logSector,
                          Uint32       numSectors,
                          Uint8        *data);

/**
 * \brief  Execute a NAND driver IOCTL.
 *
 *         This function executes a IOCTL supported by the NAND driver.
 *
 * \param  handle  [IN]  Handle of the NAND drive instance
 * \param  cmd     [IN]  IOCTL command
 * \param  cmdArg  [IN]  Optional command argument
 * \param  param   [IN]  Optional parameter
 *
 * \return IOM_COMPLETED, on success
 */
extern Int32 PSP_nandIoctl(Ptr               handle,
                           PSP_nandIoctlCmd  cmd,
                           Ptr               cmdArg,
                           Ptr               param);

/**
 * \brief  De-initializes the NAND driver.
 *
 *         This function de-initializes the NAND driver instance.
 *         It deinitializes the FTL layer and the LLC layer..
 *
 * \param  handle  [IN]  Handle of the NAND drive instance
 *
 * \return IOM_COMPLETED, on success
 */
extern Int32 PSP_nandClose(Ptr handle);


/**
 *  \brief Initialize the NAND driver block-media interface module.
 *
 *  This API initializes the NAND driver. Called by the application, this API
 *  initializes the device through the corresponding PSP initialization call.
 *  On a successful initialization, the NAND driver is registered with the
 *  block media and proceeds to check the NAND device(s) on board.
 */
Int32 PSP_nandDrvInit(PSP_nandConfig  *config);



/**
 * \brief Deinitialize the interface between block-meida and NAND driver.
 *
 *  API that De-initialize the NAND media on the board/in the system,
 *  through the PSP's Deinit call. After the NAND device(s) is (are)
 *  de-initialized read,write and ioctl will not work .
 */
Int32 PSP_nandDrvDeInit(Void);

#ifdef __cplusplus
}
#endif

#endif /* _PSP_NAND_H */

/* ========================================================================== */
/*                            END OF FILE                                     */
/* ========================================================================== */

