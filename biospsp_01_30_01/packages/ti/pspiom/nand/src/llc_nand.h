/*
 * llc_nand.h
 *
 * This file contains NAND low level controller functionality macros and inter-
 * face definitions.
 * NAND Driver provides Driver Layer Interface to do operations on the NAND
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

/** \file      llc_nand.h
 *
 *  \brief     Data types and interfaces of the NAND LLC module.
 *
 *  This file lists the data types and interfaces of the NAND LLC module.
 *  The implementation of the NAND LLC module is according to the interfaces
 *  specified in the LLC module abstraction.
 *
 *  (C)      Copyright 2008,      Texas Instruments, Inc
 *
 */


#ifndef _LLC_NAND_H
#define _LLC_NAND_H

#if !defined(CHIP_C6747) && !defined(CHIP_OMAPL137) && !defined(CHIP_C6748) && !defined(CHIP_OMAPL138)
#error "No chip type defined! (Must use -DCHIP_C6747 or -DCHIP_OMAPL137 or -DCHIP_C6748 or -DCHIP_OMAPL138)"
#endif

#if (defined(CHIP_C6748) || defined(CHIP_OMAPL138)) && !defined(BIOS_PWRM_ENABLE)
#error "Wrong Chip ID defined for BIOS_PWRM_ENABLE."
#endif

#if (defined(CHIP_C6747) || defined(CHIP_OMAPL137)) && defined(BIOS_PWRM_ENABLE)
#error "Do not use Bios PWRM module for this SoC"
#endif

/* ========================================================================== */
/*                              INCLUDE FILES                                 */
/* ========================================================================== */
#include <std.h>
#include <sem.h>
#include <string.h>

#include <ti/sdo/edma3/drv/edma3_drv.h>

#include <ti/pspiom/cslr/cslr_emifa2.h>

#include <ti/pspiom/nand/psp_nand.h>

#include "llc_nandAbstract.h"

#ifdef BIOS_PWRM_ENABLE
    #include <pwrm.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                             MACRO DEFINITIONS                              */
/* ========================================================================== */

#define LLC_NAND_CMD_READ             (0x00u) /**< NAND Read command          */
#define LLC_NAND_CMD_READ_CYC2        (0x30u) /**< NAND Read cycle2 command   */
#define LLC_NAND_CMD_READ2            (0x50u) /**< NAND Read spare command    */
#define LLC_NAND_CMD_READID           (0x90u) /**< NAND read ID command       */
#define LLC_NAND_CMD_RESET            (0xFFu) /**< NAND Reset command         */
#define LLC_NAND_CMD_PROGRAM          (0x80u) /**< NAND page program command  */
#define LLC_NAND_CMD_PROGRAM_CYC2     (0x10u) /**< NAND program cycle2 command*/
#define LLC_NAND_CMD_ERASE            (0x60u) /**< NAND block erase command   */
#define LLC_NAND_CMD_ERASE_CYC2       (0xD0u) /**< NAND erase cycle2 command  */
#define LLC_NAND_CMD_READ_STATUS      (0x70u) /**< NAND read status command   */
#define LLC_NAND_CMD_READ_RANDOM      (0x05u) /**< NAND read random command   */
#define LLC_NAND_CMD_READ_RANDOM_CYC2 (0xE0u) /**< NAND randome cycle2 command*/

#define LLC_NAND_BW_8BIT            (0u)      /**< NAND 8-Bit data bus width  */
#define LLC_NAND_BW_16BIT           (1u)      /**< NAND 16-Bit data bus width */
#define LLC_NAND_PS_SMALL           (512u)    /**< Small Page NAND device     */
#define LLC_NAND_PS_BIG             (2048u)   /**< Big Page NAND device       */

/**\brief Maximum length of the spare area */
#define LLC_NAND_MAX_SPARE_AREA_LENGTH  (0x40u)

/**\brief Maximum length of the ECC parity data */
#define LLC_NAND_MAX_ECC_PARITY_LENGTH  (40u)

#define LLC_NAND_RBWAIT_TIMEOUT  (0xFFFFFFu) /**< Maximum timeout loop counts */
#define LLC_NAND_SMALL_PAGE_SIZE (512u)      /**< Small Page size             */
#define LLC_NAND_BIG_PAGE_SIZE   (2048u)     /**< Big Page size               */

#define LLC_NAND_E_STATUS_FAIL     (-1) /**< The write/erase operation failed */
#define MEM_CACHE_LINE_SIZE (128u)/**< Cache line alignment size*/

#define LLC_NAND_MAX_PWRM_EVENTS               (PWRM_INVALIDEVENT)
/**< Maximum PWRM events supported by NAND                                    */

/* ========================================================================== */
/*                             NAND DATA TYPES                                */
/* ========================================================================== */
#ifdef BIOS_PWRM_ENABLE
/**
 * \struct LLC_nandPwrmInfo
 *
 * \brief Structure holding PWRM related information
 */
typedef struct LLC_nandPwrmInfo_t
{
    PWRM_Event                  pwrmEvent;
    /**< Current pwrm event being processed                                   */

    PWRM_NotifyHandle           notifyHandle[LLC_NAND_MAX_PWRM_EVENTS];
    /**< Handles required for unregistering of the events with PWRM           */

    Fxn                         delayedCompletionFxn[LLC_NAND_MAX_PWRM_EVENTS];
    /**< delayed completion callback function pointer                         */
}LLC_nandPwrmInfo;
#endif

/**
 *  \brief  NAND driver module instance private data
 *
 *  This structure defines the private data held by the NAND driver
 *  instance. This structure is initialized by the LLC_nandInit function and so
 *  it need not be initialized to zero.
 */
typedef struct LLC_nandObj_t
{
    Uint32                  instanceId;       /**< Instance ID                */
    PSP_nandDeviceInfo      *nandDevInfo;     /**< NAND device Info           */
    CSL_EmifaRegsOvly       regs;             /**< EMIF register base address */
    Ptr                     hEdma;            /**< EDMA driver handle         */
    Uint32                  clkFreq;          /**< EMIF clock frequency       */
    Uint32                  CExDATA;          /**< NAND data address          */
    Uint32                  CExALE;           /**< NAND address latch enable  */
    Uint32                  CExCLE;           /**< NAND command latch enable  */
    Uint32                  addressCycles;    /**< Number of Address cycles   */
    SEM_Handle              syncSem;          /**< Synchronization Semaphore  */
    Uint32                  edmaChannel;      /**< EDMA Channel Number        */
    Uint32                  edmaTcc;          /**< EDMA TCC Number            */
    Uint32                  edmaEvtQ;         /**< EDMA Event queue Number    */
    Uint32                  pwrmLpscID;       /**< LPSC Id                    */
    Uint32                  pscInstance;      /**< PSC instance               */
    Bool                    isPwrAware;       /**< Power aware or not         */
    Uint32                  submitCount;      /**< IO submit count            */
    PSP_nandPllDomain       pllDomain;        /**< Pll domain to be used      */
#ifdef BIOS_PWRM_ENABLE
    LLC_nandPwrmInfo        pwrmInfo;         /**< PWRM related information   */
    Bool                    isSuspended;      /**< suspended for IO operations*/
#endif
}LLC_nandObj;

/**< NAND LLC module function pointer table */
extern LLC_nandFxns llcNandFxns;


/* ========================================================================== */
/*                          NAND PUBLIC FUNCTIONS                             */
/* ========================================================================== */

/**
 * \brief   Initializes the LLC NAND device layer
 *
 *          This function initializes the NAND LLC layer. It detects the NAND
 *          device ID or uses a supplied ID and configures the EMIF registers.
 *          It also sets up the NAND device timing values in the EMIF
 *          registers.
 *
 * \param   initCfg   [IN]  NAND LLC module initialization data
 * \param   arg       [IN]  Not used and ignored
 *
 * \return  NULL     - If the NAND LLC module init fails.
 *          Non NULL - If the NAND LLC module init passed.
 */
extern Ptr LLC_nandInit(LLC_nandInitConfig  const   *initCfg,
                        PSP_nandDeviceInfo         **devInfo);

/**
 * \brief    Writes a page to the NAND device
 *
 *           This function writes the data specified to a NAND page. The
 *           physical block number and the physical page number in that block
 *           should be specified. The logical block number of the logical page
 *           number should also be supplied since this information is written
 *           the spare area of the page.
 *
 * \param    handle    [IN]  Handle of the NAND LLC module instance.
 * \param    phyBlock  [IN]  Physical block number to write
 * \param    phyPage   [IN]  Physical page number to write
 * \param    data      [IN]  Pointer to the data buffer to write
 * \param    spareArea [IN]  Pointer to spare area structure, logical block
 *                           logical page and ECC data will be used.
 * \param    computeEcc[IN]  Flag to idicate if ECC has to be computed or use
 *                           ecc passed in the arg spareArea.
 *
 * \return   IOM_COMPLETED                       - On success
 *           PSP_NAND_E_WRITE_PROTECTED  - If NAND device is write protected
 *           PSP_NAND_E_WRITE_FAIL       - If page write has failed
 *           PSP_NAND_E_TIMEOUT          - If page write timed-out
 */
extern Int32 LLC_nandWritePage(Ptr  const  handle,
                               Uint32             phyBlock,
                               Uint8              phyPage,
                               Uint8              *data,
                               LLC_nandSpareArea  *spareArea,
                               Bool               computeEcc);


/**
 * \brief    Reads a page to the NAND device
 *
 *           This function reads the data specified from a NAND page. The
 *           physical block number and the physical page number in that block
 *           should be specified.
 *
 * \param    handle    [IN]   Handle of the NAND LLC module instance.
 * \param    phyBlock  [IN]   Physical block number to read
 * \param    phyPage   [IN]   Physical page number to read
 * \param    data      [OUT]  Pointer to the data buffer to read
 * \param    spareData [OUT]  Used to return back ECC parity data for the page
 * \param    useEcc    [IN]   Flag to configure use of ECC module.
 *
 * \return   IOM_COMPLETED                - On success
 *           PSP_NAND_E_ECC_FAIL   - Uncorrectable ECC error detected in page
 *           PSP_NAND_E_TIMEOUT    - If page read timed-out
 */
extern Int32 LLC_nandReadPage(Ptr const   handle,
                              Uint32             phyBlock,
                              Uint8              phyPage,
                              Uint8              *data,
                              LLC_nandSpareArea  *spareData,
                              Bool               useEcc);


/**
 * \brief    Erase a block in NAND device
 *
 *           This function erases a block in the NAND device. The block number
 *           to be erased has to be specified as parameter.
 *
 * \param    handle    [IN]  Handle of the NAND LLC module instance.
 * \param    blockNum  [IN]  Physical block number to erase
 *
 * \return   IOM_COMPLETED                 - On success
 *           PSP_NAND_E_ERASE_FAIL - NAND block erase failed
 *           PSP_NAND_E_TIMEOUT    - NAND block erase block timed-out
 *           PSP_NAND_E_BLOCK_BAD  - Block is bad, cannot erase
 *           PSP_NAND_E_READ_FAIL  - Read of spare area failed.
 *           PSP_NAND_E_WRITE_PROTECTED - Device is write protected.
 */
extern Int32 LLC_nandEraseBlock(Ptr const  handle,
                                Uint32 blockNum);

/**
 * \brief    Marks the block as bad block
 *
 *           This function marks the block as bad. It writes a bad block
 *           indicator value (non 0xFF value) to the location of the bad block
 *           indicator location. It writes the bad block indicator values to
 *           1st, 2nd and last page of the device.
 *
 * \param    handle    [IN]  Handle of the NAND LLC module instance.
 * \param    blockNum  [IN]  Physical block number to read
 *
 * \return   IOM_COMPLETED           - On success
 *           PSP_E_TIMEOUT         - If write of bad block value did not succeed
 */
extern Int32 LLC_nandMarkBlockAsBad(Ptr const  handle,
                                    Uint32            blockNum);

/**
 * \brief    Reads the spare area of the specified page
 *
 *           This function reads the spare area of the page number specified.
 *           The data area for storing the spare area has be specified.
 *
 * \param    handle    [IN]   Handle of the NAND LLC module instance.
 * \param    phyBlock  [IN]   Physical block number to read
 * \param    phyPage   [IN]   Physical page number to read
 * \param    spareData [OUT]  Spare area data read.
 *
 * \return   IOM_COMPLETED           - On success
 *           PSP_E_TIMEOUT         - If write of bad block value did not succeed
 */
extern Int32 LLC_nandReadSpareArea(Ptr const         handle,
                                   Uint32            phyBlock,
                                   Uint8             phyPage,
                                   LLC_nandSpareArea *spareData);


/**
 * \brief    Executes a IOCTL command
 *
 *           This function executes a IOCTL command specified for the NAND LLC
 *           module.
 *
 * \param    handle   [IN]  Handle of the NAND LLC module instance
 * \param    ioctlCmd [IN]  IOCTL command to execute
 * \param    param    [IN]  Optional parameter
 *
 *  \return  IOM_COMPLETED          - If IOCTL execution is complete.
 *           PSP_NAND_E_ERROR       - If IOCTL execution is not complete.
 */
extern Int32 LLC_nandIoctl(Ptr const   handle,
                           LLC_nandIoctlCmd   ioctlCmd,
                           Ptr                *param);

/**
 * \brief    Deinitializes the LLC NAND device layer
 *
 *           This function de-initializes the NAND LLC layer. It releases the
 *           EDMA channel and deletes the sync semaphore.
 *
 *  \param   handle [IN]   Handle of the NAND LLC module instance
 *
 *  \return  IOM_COMPLETED          - If deinitialization is complete.
 *           PSP_NAND_E_ERROR       - If deinitialization is not complete.
 */
extern Int32 LLC_nandDeinit(Ptr const handle);



/**
 * \brief Determines the write protection status of the media
 *
 *  This function determines the write protection status of the media. The
 *  write protection would be for the media, the function argument phyBlkNum
 *  should be 0xFFFFFFu.
 *
 *  \param  handle         [IN]   Handle of the NAND LLC module instance
 *  \param  phyBlkNum      [IN]   Should be 0xFFFFFFu.
 *
 *  \return  IOM_COMPLETED              - If NOT protected.
 *           PSP_NAND_E_WRITE_PROTECTED - If protected.
 *           IOM_EBADARGS               - If handle is NULL or phyBlkNum is NOT
 *                                         0xFFFFFFu
 */
extern Int32 LLC_nandIsWriteProtected(Ptr const handle, Uint32 phyBlkNum);

/**
 * \brief Determines if the give block is BAD
 *
 *  This function determines if the given physical block is BAD or not. This
 *  function would search for the BAD Block marker in Page 0, Page 1 and last
 *  page of the NAND device to determine if this is a bad block.
 *
 *  \param  handle         [IN]   Handle of the NAND LLC module instance
 *  \param  phyBlkNum      [IN]   Physical block number to check. Block number
 *                                should be between 0 and maximum number of
 *                                blocks in the device.
 *
 *  \return  IOM_COMPLETED              - If NOT BAD.
 *           PSP_NAND_E_READ_FAIL       - If spare area read fials, could be
 *                                         interpreated as BAD Block.
 *           LLC_NAND_E_BLOCK_BAD       - If the BLOCK is BAD.
 *           IOM_EBADARGS               - If handle is NULL or invalid physical
 *                                        block number.
 */
extern Int32 LLC_nandIsBlockBad(Ptr const handle, Uint32 phyBlkNum);
#ifdef __cplusplus
}
#endif

#endif /* _LLC_NAND_H */

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
