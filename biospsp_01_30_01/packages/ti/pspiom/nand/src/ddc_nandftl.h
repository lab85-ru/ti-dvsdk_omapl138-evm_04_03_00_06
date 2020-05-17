/*
 * ddc_nandftl.h
 *
 * This file contains NAND flash translation functionality interface and macro 
 * definitions. 
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

/** \file      ddc_nandftl.h
 *
 *  \brief     NAND device FTL module implementation.
 *
 *  This file contains all the interfaces required to operate on the NAND
 *  device. It provides function for erase, write, read and initialize.
 *
 *  (C)      Copyright 2008,      Texas Instruments, Inc
 *
 */


#ifndef _DDC_NANDFTL_H
#define _DDC_NANDFTL_H

/* ========================================================================== */
/*                            INCLUDE FILES                                   */
/* ========================================================================== */

#include "llc_nand.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */

#define FTL_NAND_INVAL_BLOCK         (0xFFFFFFu)
/**< Invalid logical block number. Do not change the value of this constant. */

#define FTL_NAND_INVAL_PAGE          (0xFF)
/**< Invalid logical page number. Do not change the value of this constant. */

#define FTL_NAND_E_INVAL_CACHE_INDEX (0xFFFFu)
/**< Invalid FTL cache line index. Do not change the value of this constant. */

/* ========================================================================== */
/*                           NAND DATA TYPES                                  */
/* ========================================================================== */


/**
 *  \brief  Cache line representation
 *
 *  This structure defines the cache line organization in the cache module.
 */
typedef struct DDC_nandCacheLine_t
{
    Uint32  phyBlock;                             /**< Physical block cached */
    Uint8   nextPhyFreePage;                      /**< Next phy free page    */
    Uint8   log2Phy[PSP_NAND_MAX_PAGES_IN_BLOCK]; /**< Log2Phy page mapping  */
}DDC_nandCacheLine;
/**< Cache line representation */

/**
 *  \brief  NAND FTL cache module data representation
 *
 *  This structure defines the cache module data mainatained by the cache
 *  module in the NAND FTL.
 */
typedef struct DDC_nandCacheObj_t
{
    Uint32              nextDealloc;        /**< Next cache line to dealloc */
    Uint16              pagesPerBlock;      /**< Pages per block            */
    DDC_nandCacheLine   cacheLine[PSP_NAND_MAX_CACHE_LINES]; /**< cache line */
    Ptr                 hLlcObj;           /**< LLC module handle          */
    LLC_nandFxns        *llcFxns;           /**< LLC module function table  */
}DDC_nandCacheObj;
/**< NAND FTL cache module data representation */


/**
 *  \brief  NAND FTL WLA data representation
 *
 *  This structure defines the NAND FTL WLA data maintained by the FTL
 *  module.
 */
typedef struct DDC_nandWlaData_t
{
    Uint32  *wlaBitMap;    /**< WLA bit map table                   */
    Uint32  bitMapLength;  /**< Number of 32-bit words in the table */
    Uint32  nextBlock;     /**< Next block to be allocated          */
    Uint32  freeBlocks;    /**< Number of blocks available for allocation */
}DDC_nandWlaData;
/**< NAND FTL WLA data representation */


/**
 *  \brief  NAND FTL instance internal data object
 *
 *  This structure defines the private data held by the NAND FTL module
 *  instance.
 */
typedef struct DDC_nandFtlObj_t
{
    Uint32              instanceId;         /**< Instance ID of FTL          */
    Bool                eraseAtInit;        /**< Erase At Initilization      */
    PSP_nandDeviceInfo  *devInfo;           /**< Device organization info    */
    LLC_nandFxns        *llcFxns;           /**< LLC function pointer table  */
    Ptr                 hLlcObj;            /**< LLC module handle           */
    Uint32              *log2PhyTable;      /**< Logical to Physical mapping */
    DDC_nandWlaData     wlaData;            /**< FTL WLA object              */
    DDC_nandCacheObj    cacheObj;           /**< FTL cache object            */
    Uint32              protectedBlocks;    /**< Number of protected blocks  */
    Uint32              availableBlocks;    /**< Available number of blocks  */
    Uint32              usableBlocks;       /**< Usuable number of blovks    */
    Uint32              mergeBlock;         /**< Block to be merged          */
}DDC_nandFtlObj;
/**< NAND FTL instance internal data object */


/**
 *  \brief  NAND FTL initialization parameters
 *
 *  This structure defines the initialization parameters of the NAND FTL
 *  module.
 */
typedef struct DDC_nandFtlInitObj_t
{
    Uint32              instanceId;         /**< Instance ID of FTL         */
    Bool                eraseAtInit;        /**< Erase At Initilization     */
    PSP_nandDeviceInfo  *devInfo;           /**< Device organization info   */
    LLC_nandFxns        *llcFxns;           /**< LLC function pointer table */
    Ptr                 hLlcObj;            /**< LLC module handle          */
    Uint32              protectedBlocks;    /**< Number of protected blocks */
}DDC_nandFtlInitObj;
/**< Cache line representation */

/* ========================================================================== */
/*                             NAND DATA TYPES                                */
/* ========================================================================== */

/**
 * \brief  Initializes the NAND FTL module.
 *
 *         This function initializes the NAND FTL module. It also initializes
 *         the WLA and logical to physical block mapping table as well as the
 *         FTL cache module.
 *
 * \param  ftlInitObj [IN]   FTL initialization parameters
 * \param  maxSectors [OUT]  Maximum number of logical sectors available
 * \param  rtnValue   [OUT]  return value
 *
 * \return On success, return the handle of the FTL instances.
 */
DDC_nandFtlObj* DDC_nandFtlInit(const DDC_nandFtlInitObj *ftlInitObj,
                                Uint32                   *maxSectors,
                                Int32                    *rtnValue);


/**
 * \brief  Writes single or multiple logical sectors to NAND flash.
 *
 *         This function writes single or multiple logical sectors to the NAND
 *         flash. The logical sectors have be consecutive.
 *
 * \param  ftlObj     [IN]   FTL instance handle
 * \param  logSector  [OUT]  Starting logical sector number
 * \param  numSectors [IN]   Number of sectors to write
 * \param  data       [OUT]  Data buffer pointer
 *
 * \return IOM_COMPLETED    - If write completes.
 *         Error code - if write does not complete.
 */
Int32 DDC_nandFtlWrite(DDC_nandFtlObj  *ftlObj,
                       Uint32          logSector,
                       Uint32          numSectors,
                       Uint8           *data);

/**
 * \brief  Reads single or multiple logical sectors from NAND flash.
 *
 *         This function reads single or multiple logical sectors from the NAND
 *         flash. The logical sectors have be consecutive.
 *
 * \param  ftlObj     [IN]   FTL instance handle
 * \param  logSector  [OUT]  Starting logical sector number
 * \param  numSectors [IN]   Number of sectors to read
 * \param  data       [OUT]  Data buffer pointer
 *
 * \return IOM_COMPLETED    - If write completes.
 *         Error code            - if write does not complete.
 */
Int32 DDC_nandFtlRead(DDC_nandFtlObj  *ftlObj,
                      Uint32          logSector,
                      Uint32          numSectors,
                      Uint8           *data);

/**
 * \brief  Erases a logical block
 *
 *         This function erases a logical block number. It recycles the physical
 *         block mapped to the logical block and updates the logical to physical
 *         mapping table.
 *
 * \param  ftlObj     [IN]  FTL instance handle
 * \param  logPage    [IN/OUT]     Logical page number to start with
 * \param  numPages   [IN/OUT]     Number of pages to erase
 *
 * \return IOM_COMPLETED    - If erase completes.
 *         Error code            - if erases does not complete.
 */
Int32 DDC_nandFtlErase(DDC_nandFtlObj  *ftlObj,
                       Uint32          *logPage,
                       Uint32          *numPages);



#ifdef __cplusplus
}
#endif


#endif /* _DDC_NANDFTL_H */
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */

