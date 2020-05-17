/*
 * ddc_nandAbstract.h
 *
 * This file contains NAND LLC module abstraction.
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

/** \file      llc_nandAbstract.h
 *
 *  \brief     NAND LLC module abstraction
 *
 *  This file defines the interfaces the NAND LLC module should implement.
 *  There could be seperate implementations of these interfaces, for example,
 *  NAND and OneNAND. For any new type of the NAND driver to be supported by
 *  the driver, the LLC module for that device should implement the
 *  interface defined in this file.
 *
 *  (C)      Copyright 2008,      Texas Instruments, Inc
 *
 */

#ifndef _LLC_NANDABSTRACT_H
#define _LLC_NANDABSTRACT_H

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include <ti/pspiom/nand/psp_nand.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                          MACRO DEFINTIONS                                  */
/* ========================================================================== */

/**\brief Number of times to retry read operations in case of ECC errors during
 *        page read operations.
 */
#define LLC_NAND_READ_RETRY   (3u)

/**\brief LLC Abstraction Layer Error Codes */
#define LLC_NAND_E_BLOCK_BAD  (-1)  /**< The block is bad block           */

/* ========================================================================== */
/*                           NAND DATA TYPES                                  */
/* ========================================================================== */

/**
 *  \brief  LLC module layer initialization information
 *
 *  This structure defines the data required to initialize the LLC module
 *  of the NAND driver.
 */
typedef enum LLC_nandIoctlCmd_t
{
    LLC_NAND_IOCTL_SET_EDMA_EVTQ,
    LLC_NAND_IOCTL_MAX
}
LLC_nandIoctlCmd;
/**< LLC NAND Ioctl commands */


/**
 *  \brief  LLC module layer initialization information
 *
 *  This structure defines the data required to initialize the LLC module
 *  of the NAND driver.
 */
typedef struct LLC_nandInitConfig_t
{
    Uint32                 instanceId;    /**< Instance ID of chip select  */
    Uint32                 clkFreq;       /**< EMIF clock frequency        */
    PSP_nandDeviceInfo    *deviceInfo;    /**< Device Info, if any         */
    PSP_nandDeviceTiming  *deviceTiming;  /**< Device timing, if any       */
    Ptr                    hEdma;         /**< EDMA driver handle          */
    Uint32                 edmaEvtQ;      /**< EDMA event queue            */
    Uint32                 edmaChannel;   /**< EDMA channel number to use  */
    Bool                   isPwrAware;    /**< Power aware or not          */
    PSP_nandPllDomain      pllDomain;     /**< Pll domain where the device is */
}
LLC_nandInitConfig;
/**< NAND LLC module intialization information */


/**
 *  \brief  LLC module spare area information representation
 *
 *  This structure is used to hold the interpreted version of the data in the
 *  spare area of a page.
 */
typedef struct LLC_nandSpareArea_t
{
    Uint32   logicalBlock; /**< Logical block number stored in the spare area */
    Bool     badBlockMark; /**< Bad block mark stored in the spare area       */
    Uint8    logicalPage;  /**< Logical page number stored in the spare area  */
    Uint8    *eccData;     /**< ECC data, if any, in the spare area           */
}
LLC_nandSpareArea;
/**< LLC module spare area information representation */



/* ========================================================================== */
/*                           NAND PUBLIC FUNCTIONS                            */
/* ========================================================================== */

/**
 * \brief    Initializes the LLC device layer
 *
 *           This function initializes the LLC layer. It detects the NAND device
 *           ID or uses a supplied ID and configures the EMIF registers. It also
 *           sets up the NAND device timing values in the EMIF registers.
 *
 * \param    initCfg   [IN]     NAND LLC module initialization data
 * \param    devInfo   [IN/OUT] Pointer to device organization information
 *
 * \return   NULL     - If the NAND LLC module init fails.
 *           Non NULL - If the NAND LLC module init passed.
 */
typedef Ptr (*LLC_nandInitFxn)(LLC_nandInitConfig const *initCfg,
                               PSP_nandDeviceInfo      **devInfo);

/**
 * \brief    Writes a page to the NAND device
 *
 *           This function writes the data specified to a page. The physical
 *           block number and the physical page number in that block should be
 *           specified. The logical block number of the logical page number
 *           should also be supplied since this information is written the spare
 *           area of the page.
 *
 * \param    handle    [IN]  Handle of the LLC module instance.
 * \param    phyBlock  [IN]  Physical block number to write
 * \param    phyPage   [IN]  Physical page number to write
 * \param    data      [IN]  Pointer to the data buffer to write
 * \param    spareArea     [IN]  Pointer to spare area details, such as
 *                                 logical page, logical block number, ecc data.
 * \param    computeEcc    [IN]  Flag used to compute ECC or use the caller
 *                                 supplied ecc, supplied in spareArea
 *
 * \return   IOM_COMPLETED                       - On success
 *           PSP_NAND_E_WRITE_PROTECTED  - If NAND device is write protected
 *           PSP_NAND_E_WRITE_FAIL       - If page write has failed
 *           PSP_NAND_E_TIMEOUT          - If page write timed-out
 */
typedef Int32 (*LLC_nandWritePageFxn)(Ptr  const  handle,
                                      Uint32             phyBlock,
                                      Uint8              phyPage,
                                      Uint8              *data,
                                      LLC_nandSpareArea  *spareArea,
                                      Bool               computeEcc);

/**
 * \brief    Reads a page from the device
 *
 *           This function reads the data from the specified page. The physical
 *           block number and the physical page number in that block should be
 *           specified.
 *
 * \param    handle     [IN]  Handle of the LLC module instance.
 * \param    phyBlock   [IN]  Physical block number to read
 * \param    phyPage    [IN]  Physical page number to read
 * \param    data       [IN]  Pointer to the data buffer where data is stored.
 * \param    spareData  [IN]  Pointer to the spare data buffer.
 *
 * \return   IOM_COMPLETED                - On success
 *           PSP_NAND_E_ECC_FAIL  - Uncorrectable ECC error detected in page
 *           PSP_NAND_E_TIMEOUT   - If page read timed-out
 */
typedef Int32 (*LLC_nandReadPageFxn)(Ptr const   handle,
                                     Uint32             phyBlock,
                                     Uint8              phyPage,
                                     Uint8              *data,
                                     LLC_nandSpareArea  *spareData,
                                     Bool               useEcc);

/**
 * \brief    Erase a block of the device
 *
 *           This function erases a block in the device. The block number to be
 *           erased has to be specified as parameter.
 *
 * \param    handle    [IN]  Handle of the LLC module instance.
 * \param    blockNum  [IN]  Physical block number to be erase
 *
 * \return   IOM_COMPLETED                      - On success
 *           PSP_NAND_E_ERASE_FAIL      - NAND block erase failed
 *           PSP_NAND_E_TIMEOUT         - NAND block erase block timed-out
 *           PSP_NAND_E_BLOCK_BAD       - Block is bad, cannot erase
 *           PSP_NAND_E_READ_FAIL       - Read of spare area failed
 *           PSP_NAND_E_WRITE_PROTECTED - NAND is write-protected
 */
typedef Int32 (*LLC_nandEraseBlockFxn)(Ptr const handle,
                                       Uint32           blockNum);



/**
 * \brief    Marks the block as bad block
 *
 *           This function marks the block as bad. It writes a bad block
 *           indicator value (non 0xFF value) to the location of the bad block
 *           indicator location. Refer to device specific implementation to know
 *           more about the marking of bad block.
 *
 * \param    handle    [IN]  Handle of the LLC module instance.
 * \param    blockNum  [IN]  Physical block number to be marked as bad.
 *
 * \return   IOM_COMPLETED           - On success
 *           PSP_E_TIMEOUT         - If the marking of block as bad timed-out
 */
typedef Int32 (*LLC_nandMarkBlockAsBadFxn)(Ptr const handle,
                                           Uint32           blockNum);


/**
 * \brief    Reads the spare area of the specified page
 *
 *           This function reads the spare area of the physical block and page
 *           number specified. The information stored in the spare area is
 *           available in the spareData paramater for the caller.
 *
 * \param    handle    [IN]   Handle of the LLC module instance
 * \param    phyBlock  [IN]   Physical block number to read
 * \param    phyPage   [IN]   Physical page number to read
 * \param    spareData [OUT]  Spare Area data returned to the caller.
 *
 * \return   IOM_COMPLETED           - On success
 *           PSP_E_TIMEOUT          - If the read from spare area timed-out.
 */
typedef Int32 (*LLC_nandReadSpareAreaFxn)(Ptr const   handle,
                                          Uint32             phyBlock,
                                          Uint8              phyPage,
                                          LLC_nandSpareArea  *spareData);

/**
 * \brief   Executes a IOCTL command
 *
 *          This function executes a IOCTL command specified for the NAND LLC
 *          module.
 *
 * \param   handle   [IN]  Handle of the NAND LLC module instance
 * \param   ioctlCmd [IN]  IOCTL command to execute
 * \param   param    [IN]  Optional parameter
 *
 *  \return  IOM_COMPLETED          - If IOCTL execution is complete.
 *          PSP_NAND_E_ERROR       - If IOCTL execution is not complete.
 */
typedef Int32 (*LLC_nandIoctlFxn)(Ptr const  handle,
                                  LLC_nandIoctlCmd  ioctlCmd,
                                  Ptr               *param);


/**
 * \brief    Deinitializes the LLC NAND device layer
 *
 *           This function de-initializes the NAND LLC layer. It releases the
 *           EDMA channel and deletes the sync semaphore.
 *
 *  \param   handle [IN]   Handle of the NAND LLC module instance
 *
 *  \return  IOM_COMPLETED          - If deinitialization is complete.
 *           PSP_NAND_E_ERROR        - If deinitialization is not complete.
 */
typedef Int32 (*LLC_nandDeinitFxn)(Ptr const handle);


/**
 * \brief Determines if NAND media is write protected
 *
 *  This function determines if the media is write protected. The garnularity
 *  of protection could be in terms of BLOCK or whole media. BLOCK level
 *  protection shall only be supported for ONENAND, other LLC is expcted to
 *  return the status of whole media.
 *
 *  \param  handle         [IN]   Handle of the NAND LLC module instance
 *  \param  phyBlkNum      [IN]   Physical block number, in case of LLC for
 *                                ONE NAND, 0xFFFFFFu for others.
 *
 *  \return  IOM_COMPLETED                    - If NOT protected.
 *           PSP_NAND_E_WRITE_PROTECTED - If protected.
 */
typedef Int32 (*LLC_nandIsWriteProtectFxn)(Ptr const handle,
                                           Uint32           phyBlkNum);


/**
 * \brief Determines if the block is bad
 *
 * This function determines if the specified block is good or bad block. It
 * reads the spare area of the 1st, 2nd and last page of the NAND device to
 * determine if this is a bad block.
 *
 * \param    hNandObj   [IN]  Handle of the NAND LLC module instance
 * \param    blockNum   [IN]  Physical block number to check
 *
 * \return   IOM_COMPLETED                  - If block is not bad.
 *           PSP_NAND_E_BLOCK_BAD  - If block is bad.
 *           PSP_NAND_E_READ_FAIL  - If reading of spare area failed.
 */
typedef Int32 (*LLC_nandIsBlockBadFxn)(Ptr const  hNandObj,
                                       Uint32            blockNum);


/**
 *  \brief  NAND LLC module interfaces
 *
 *  This structure defines the table of NAND LLC module function pointers.
 *  All implementations of the NAND LLC module should provide this table.
 */
typedef struct  LLC_nandFxns_t
{
    LLC_nandInitFxn            initFxn;
    /**< Function pointer to NAND LLC module initialization function */

    LLC_nandWritePageFxn       writePageFxn;
    /**< Function pointer to NAND LLC write page function            */

    LLC_nandReadPageFxn        readPageFxn;
    /**< Function pointer to NAND LLC read page function             */

    LLC_nandEraseBlockFxn      eraseBlockFxn;
    /**< Function pointer to NAND LLC erase block function           */

    LLC_nandMarkBlockAsBadFxn  markBadBlockFxn;
    /**< Function pointer to NAND LLC mark bad block function        */

    LLC_nandReadSpareAreaFxn   readSpareFxn;
    /**< Function pointer to NAND LLC spare area read function       */

    LLC_nandIoctlFxn           ioctlFxn;
    /**< Function pointer to NAND LLC IOCTL function                 */

    LLC_nandDeinitFxn          deinitFxn;
    /**< Function pointer to NAND LLC spare initialization function  */

   LLC_nandIsWriteProtectFxn  isWriteProtected;
   /**< Function pointer to NAND LLC function to determine write
        protection status                                           */

   LLC_nandIsBlockBadFxn      isBlockBad;
   /**< Function poiter to NAND LLC function to determine if a given
        block is BAD                                                */
}
LLC_nandFxns;
/**< NAND LLC module function pointer table */

#ifdef __cplusplus
}
#endif

#endif  /* _LLC_NANDABSTRACT_H */
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
