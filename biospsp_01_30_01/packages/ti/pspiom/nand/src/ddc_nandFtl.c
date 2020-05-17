/*
 * ddc_nandFtl.c
 *
 * This file contains NAND flash translation functionality implementation. 
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

/** \file      ddc_nandFtl.c
 *
 *  \brief     NAND device FTL module implementation.
 *
 *  This file contains all the interfaces required to operate on the NAND
 *  device. It provides function for erase, write, read and initialize.
 *
 *  (C)      Copyright 2008,      Texas Instruments, Inc
 *
 *
 */

/* ========================================================================== */
/*                            INCLUDE FILES                                   */
/* ========================================================================== */
#include <std.h>
#include <iom.h>
#include <assert.h>

#include "llc_nand.h"
#include "ddc_nandftl.h"
#include "llc_nandAbstract.h"

#ifdef NAND_INSTRUMENTATION_ENABLED
#include <ti/sdo/pspdrivers/pal_os/bios/psp_log.h>
#endif /* NAND_INSTRUMENTATION_ENABLED */
/* ========================================================================== */
/*                            LOCAL VARIABLES                                 */
/* ========================================================================== */

/**\brief Logical to Physical Block mapping table instances                   */
static Uint32 DDC_nandFtlMapTable[PSP_NAND_INSTANCES]
                                 [PSP_NAND_FTL_MAX_LOG_BLOCKS];

/**\brief WLA Bit mapping table instances                                     */
static Uint32 DDC_nandFtlBitMap[PSP_NAND_INSTANCES]
                               [PSP_NAND_FTL_MAX_PHY_BLOCKS / 32u];

/**\brief NAND FTL instances                                                  */
static DDC_nandFtlObj ftlObjInstances[PSP_NAND_INSTANCES];

/**\brief Temporary storage of page data                                      */
static Uint8 pageData[PSP_NAND_MAX_PAGE_SIZE + MEM_CACHE_LINE_SIZE];


/* ========================================================================== */
/*                       LOCAL FUNCTION PROTOTYPES                            */
/* ========================================================================== */
static Int32 nandFtlInitFromSpare(DDC_nandFtlObj *ftlObj);

static Int32 nandFtlInitErase(DDC_nandFtlObj  *ftlObj);

static Void nandWlaInit(DDC_nandWlaData  *wlaData,
                        Uint32           protectedBlocks);

static Void nandWlaUpdateTable(DDC_nandWlaData  *wlaData,
                               Uint32           blockNum,
                               Bool             isUsed);

static Int32 nandWlaFindFreeBlock(DDC_nandWlaData  *wlaData,
                                  Uint32           *blockNum);

static Uint32 nandWlaGetFreeBlockCount(DDC_nandWlaData  *wlaData);

static Void nandFtlCacheInit(DDC_nandCacheObj   *cacheObj,
                             Uint32             pagesPerBlock,
                             Ptr                hLlcObj,
                             LLC_nandFxns       *llcFxns);

static Uint16 nandFtlCacheGetIndex(DDC_nandCacheObj  *cacheObj,
                                   Uint32            phyBlockNum);

static Void nandFtlCacheInvalidate(DDC_nandCacheObj  *cacheObj,
                                   Uint32            phyBlockNum);

static Uint16 nandFtlCacheFindFreeLine(DDC_nandCacheObj  *cacheObj);

static Int32 nandFtlCachePrefetch(DDC_nandCacheObj  *cacheObj,
                                  Uint32            phyBlock,
                                  Uint16            *lineIdx);

static Int32 nandFtlCacheIsLogPageFree(DDC_nandCacheObj  *cacheObj,
                                       Uint32            phyBlock,
                                       Uint8             logPage,
                                       Bool              *status,
                                       Uint8             *phyPage);

static Int32 nandFtlCacheMapLogPage(DDC_nandCacheObj  *cacheObj,
                                    Uint32            phyBlock,
                                    Uint8             logPage,
                                    Uint8             phyPage);


static Int32 nandFtlCacheGetPhyFreePage(DDC_nandCacheObj  *cacheObj,
                                        Uint32            phyBlock,
                                        Uint8             *phyPage);


static Void nandFtlCacheLineSetPhyBlock(DDC_nandCacheObj  *cacheObj,
                                        Uint32            oldPhyBlock,
                                        Uint32            newPhyBlock);

static Int32 nandFtlCopyPage(DDC_nandFtlObj  *ftlObj,
                             Uint32          srcPhyBlk,
                             Uint8           srcPhyPage,
                             Uint32          dstPhyBlk,
                             Uint8           dstPhyPage);

static Int32 nandFtlCopyBlock(DDC_nandFtlObj  *ftlObj,
                              Uint32          srcPhyBlk,
                              Uint32          dstPhyBlk,
                              Uint8           numPages);

static Int32 nandFtlMoveBlock(DDC_nandFtlObj *ftlObj, Uint32 *srcPhyBlk);

static Int32 nandMergeBlock(DDC_nandFtlObj  *ftlObj,
                            Uint32          *dstBlk,
                            Uint32          srcBlk);

static Int32 nandFtlRecycleBlock(DDC_nandFtlObj *ftlObj, Uint32 eraseBlock);

/* ========================================================================== */
/*                        PUBLIC FUNCTION DEFINITIONS                         */
/* ========================================================================== */

/**
 * \brief Initializes the NAND FTL module.
 *
 *        This function initializes the NAND FTL module. It also initializes
 *        the WLA and logical to physical block mapping table as well as the
 *        FTL cache module.
 */
DDC_nandFtlObj* DDC_nandFtlInit(const DDC_nandFtlInitObj *ftlInitObj,
                                Uint32                   *maxSectors,
                                Int32                    *rtnValue)
{
    Int32               result;   /* Result of FTL module initialization */
    DDC_nandFtlObj      *ftlObj;  /* FTL module instance object          */
    Uint32              idx;      /* Counter variable                    */
    Bool                flag = TRUE;
    do
    {
        /* Validate the input parameters */
        assert((NULL != ftlInitObj) && (NULL != maxSectors));

        /* Get the reference pointer to the FTL object instance               */
        ftlObj = (DDC_nandFtlObj*)&ftlObjInstances[ftlInitObj->instanceId];

        /* Keep a copy of the required initialization parameters              */
        flag                    = FALSE;
        ftlObj->instanceId      = ftlInitObj->instanceId;
        ftlObj->eraseAtInit     = ftlInitObj->eraseAtInit;
        ftlObj->hLlcObj         = ftlInitObj->hLlcObj;
        ftlObj->llcFxns         = ftlInitObj->llcFxns;
        ftlObj->devInfo         = ftlInitObj->devInfo;
        ftlObj->protectedBlocks = ftlInitObj->protectedBlocks;

        /* Set the pointer to the logical to physical mapping table */
        ftlObj->log2PhyTable =
            (Uint32*)&(DDC_nandFtlMapTable[ftlInitObj->instanceId]);

        /* Initialize the WLA module                                          */
        ftlObj->wlaData.wlaBitMap =
            (Uint32*)(&DDC_nandFtlBitMap[ftlInitObj->instanceId]);
        ftlObj->wlaData.bitMapLength = (ftlObj->devInfo->numBlocks) >> 5u;
        ftlObj->wlaData.freeBlocks = ftlObj->devInfo->numBlocks;
        nandWlaInit((DDC_nandWlaData*)&ftlObj->wlaData,
            ftlObj->protectedBlocks);

        /* Initialize the logical to physical mapping table.                  *
         * Note: memset was not used here because the value of                *
         * FTL_NAND_INVAL_BLOCK is 0x00FFFFFF. Because of the 0x00 in the MSB *
           8-bits, memset was cannot be used.                                 */
        for (idx = 0; idx < PSP_NAND_FTL_MAX_LOG_BLOCKS; idx++)
        {
            ftlObj->log2PhyTable[idx] = FTL_NAND_INVAL_BLOCK;
        }

        /* Initialize the cache module of FTL                                 */
        nandFtlCacheInit((DDC_nandCacheObj*)&ftlObj->cacheObj,
            ftlObj->devInfo->pagesPerBlock,
            ftlObj->hLlcObj,
            ftlObj->llcFxns);

        /* Initialize the FTL                                                 */
        if (FALSE == ftlObj->eraseAtInit)
        {
            /* Initialize FTL by reading the spare area                       */
            result = nandFtlInitFromSpare(ftlObj);
            if (result != IOM_COMPLETED)
            {
                /* FTL initialization by reading the spare area failed        */
                break;
            }
        }
        else
        {
            /* Initialize FTL by erasing the NAND blocks                      */
            result = nandFtlInitErase(ftlObj);
            if (result != IOM_COMPLETED)
            {
                /* FTL initialization by erasing blocks failed                */
                break;
            }
        }

        /* Update the number of usable block information                      */
        *maxSectors = ftlObj->usableBlocks * ftlObj->devInfo->pagesPerBlock;

        /* Set the merge block to invalid block                               */
        ftlObj->mergeBlock = FTL_NAND_INVAL_BLOCK;
    }while (flag);

    /* If the FTL initialization is complete, set the FTL handle to return    */
    if (result != IOM_COMPLETED)
    {
        ftlObj = NULL;
    }

    if (rtnValue != NULL)
    {
        /* Let the caller know the status                                     */
        *rtnValue = result;
    }
    /* Otherwise the caller never gave us an opportunity to inform the reason */

    /* Return the result of initializing the FTL                              */
    return (ftlObj);
}

/**
 * \brief Writes single or multiple logical sectors to NAND flash.
 *
 *        This function writes single or multiple logical sectors to the NAND
 *        flash. The logical sectors have be consecutive.
 */
Int32 DDC_nandFtlWrite(DDC_nandFtlObj  *ftlObj,
                       Uint32          logSector,
                       Uint32          numSectors,
                       Uint8           *data)
{
    Int32       result;    /* Result of writting pages to NAND device      */
    Int32       mvResult;  /* Result of moving contents of  physical block */
    Uint32      sectorCnt; /* Sector counter                               */
    Uint32      logBlk;    /* Logical block number                         */
    Uint32      phyBlk;    /* Physical block number                        */
    Uint8       logPage;   /* Logical page number                          */
    Uint8       phyPage;   /* Physical page number                         */
    Bool        status;    /* Status of availablity of logical page        */
    Uint16      lineIdx;   /* Cache line index number                      */
    Int32       mrResult;  /* Result of merging two blocks                 */
    Uint32      freeBlocks; /* Number of free blocks in the system          */
    static LLC_nandSpareArea spareAreaData;
                          /* Place holder for spare area, during a page w */
    Bool        flag = TRUE;
#ifdef NAND_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
        "NAND",
        PSP_DVT_DESC(PSP_DVT_eDATA_SND_START,
                     PSP_DVT_dCHANNEL_ID_INST,
                     PSP_DVT_dPACKET_ID,
                     PSP_DVT_dNONE),
        CHANNELID_INSTANCEID(
            1,
            0),
        data);
#endif /* NAND_INSTRUMENTATION_ENABLED */
    do
    {
        /* Initialize the local variables                                     */
        result = IOM_EBADARGS;
        flag = FALSE;

        /* Validate the input parameters                                      */
        assert((NULL != ftlObj) && (NULL != data));

       /* Validate the LLC Handle */
       if (NULL == ftlObj->hLlcObj)
       {
           break;
       }

       /* Check if the media is write protected */
       result = ftlObj->llcFxns->isWriteProtected(ftlObj->hLlcObj,
                    FTL_NAND_INVAL_BLOCK);

       /* Exit if the media is write protected */
       if (result != IOM_COMPLETED)
       {
           break;
       }
        /* Invalidate the merge status                                        */
        ftlObj->mergeBlock = FTL_NAND_INVAL_BLOCK;

        /* Determine the logical block and logical page number                */
        logBlk  = (Uint32)(logSector / ftlObj->devInfo->pagesPerBlock);
        logPage = (Uint8) (logSector % ftlObj->devInfo->pagesPerBlock);

        /* Write the requested number of sectors                              */
        for (sectorCnt = 0; sectorCnt < numSectors; sectorCnt++)
        {
            /* Determine the physical block number of logical block number    */
            phyBlk = ftlObj->log2PhyTable[logBlk];

            /* If the logical block number is not mapped to any physical      *
             * block number, then allocate a new physical block for the       *
             * logical block number.                                          */
            if (FTL_NAND_INVAL_BLOCK == phyBlk)
            {
                /* Determine if there are more than one free block available  *
                 * in the system. Only if there atleast one free block,       *
                 * allocate it for use. NOTE: Always maintain one free        *
                 * block for block/page overwrite cases.                      */
                freeBlocks = nandWlaGetFreeBlockCount(
                                 (DDC_nandWlaData*)&ftlObj->wlaData);
                if (1u == freeBlocks)
                {
                    /* A new block has to be allocated to map to a logical    *
                     * block (that is not already mapped). Since there is only*
                     * one free block available in the system at this point,  *
                     * that free block will be required for block/page        *
                     * overwrite cases. So do not allocate the free block and *
                     * return out of free blocks error. This case could occur *
                     * if there are more bad blocks than reserved blocks.     */
                    result = PSP_NAND_E_NO_FREE_BLOCK;
                    break;
                }

                /* Get a new physical block for the logical block             */
                result = nandWlaFindFreeBlock(
                             (DDC_nandWlaData*)&ftlObj->wlaData,
                             (Uint32*)&phyBlk);
                if (result != IOM_COMPLETED)
                {
                    /* A free block was not available. So return.             */
                    ftlObj->mergeBlock = FTL_NAND_INVAL_BLOCK;
                    break;
                }

                /* Update the logical to physical mapping table               */
                ftlObj->log2PhyTable[logBlk] = phyBlk;
            }

            /* Check if this logical page is already written in the block     */
            result = nandFtlCacheIsLogPageFree(
                         (DDC_nandCacheObj*)&ftlObj->cacheObj,
                         phyBlk,
                         logPage,
                         &status,
                         (Uint8*)&phyPage);
            if (FALSE == status)
            {
                /* The logical page is already written                        */
                ftlObj->mergeBlock = phyBlk;

                /* Get a new physical block for the logical block             */
                result = nandWlaFindFreeBlock(
                             (DDC_nandWlaData*)&ftlObj->wlaData,
                             &phyBlk);
                if (result != IOM_COMPLETED)
                {
                    /* A free block was not available. So return.             */
                    /* Ensure that we do not trigger a merge, as we are out of
                       free blocks now */
                    /* Only timeout could occur here - this will leave driver
                      in in-valid state i.e. we might have marked block used
                      in WLA but physically not used. On re-initialization
                      WLA is populated with no of blocks avialable from device
                       hence WLA will be correct on re-initialization */
                    ftlObj->mergeBlock = FTL_NAND_INVAL_BLOCK;
                    break;
                }

                /* Update the logical to physical mapping table               */
                ftlObj->log2PhyTable[logBlk] = phyBlk;

                /* Prefetch the new block into the cache                      */
                result = nandFtlCachePrefetch(
                             (DDC_nandCacheObj*)&ftlObj->cacheObj,
                             phyBlk,
                             (Uint16*)&lineIdx);
                if (result != IOM_COMPLETED)
                {
                    /* Prefetch of logical to physical page mapping failed    */
                    /* Prefetch fails - due to timeouts, invalid args, others
                       clear merge op */
                    /* KW says "Code is unreachable". This is valid error but
                       the function inside nandFtlCachePrefetch, i.e.
                       readSpareFxn, currently does not take care of timeout
                       errors. Once it takes care of that, this function will
                       have this check and will not require a modification at
                       upper layer */
                    ftlObj->mergeBlock = FTL_NAND_INVAL_BLOCK;
                    break;
                }
            }

            /* Do until the page is written successfully to a physical page   */
            do
            {
                /* Get a physical page number for the logical page            */
                result = nandFtlCacheGetPhyFreePage(
                             (DDC_nandCacheObj*)&ftlObj->cacheObj,
                             phyBlk,
                             (Uint8*)&phyPage);

               /* Populate Spare area details */
               spareAreaData.logicalBlock  = logBlk;
               spareAreaData.logicalPage   = logPage;
               spareAreaData.eccData       = NULL;
               spareAreaData.badBlockMark  = FALSE;
               /* Let the page write function compute the ECC Parity */
                if (result != IOM_COMPLETED)
                {
                    /* Error in finding a free page for the logical page      */
                    break;
                }

                /* Write the page                                             */
                result = ftlObj->llcFxns->writePageFxn(ftlObj->hLlcObj,
                             phyBlk,
                             phyPage,
                             data,
                             &spareAreaData,
                             TRUE);
                if (result != IOM_COMPLETED)
                {
                   /* Require to move only on write failure, otherwise
                      return the error code */
                   if (PSP_NAND_E_WRITE_FAIL == result)
                   {
                    /* The write to page has failed. So the current physical  *
                     * block contents have to be moved to a new physical      *
                     * block. Note: The physical block to which the move was  *
                     * successfully performed is returned after the move is   *
                     * complete.                                              */
                    mvResult = nandFtlMoveBlock(ftlObj, &phyBlk);
                    if (mvResult != IOM_COMPLETED)
                    {
                        /* If the merge block is valid,then change the logical*
                         * to physical mapping table and invalidate the merge *
                         * status.                                            */

                        if (ftlObj->mergeBlock != FTL_NAND_INVAL_BLOCK)
                        {
                            ftlObj->log2PhyTable[logBlk] = ftlObj->mergeBlock;
                            ftlObj->mergeBlock = FTL_NAND_INVAL_BLOCK;
                        }

                        /* Error in moving the physical block to a new block  */
                        result = mvResult;
                        break;
                    }
                    /* Update the logical to physical mapping table.          */
                    ftlObj->log2PhyTable[logBlk] = phyBlk;
                   }
                   else
                   {
                       /* On PSP_NAND_E_WRITE_PROTECTED, PSP_NAND_E_TIMEOUT
                          let the application know. */
                       /* Ensure that we do not trigger merge op */
                       ftlObj->mergeBlock = FTL_NAND_INVAL_BLOCK;
                       break;
                   }
                }
            }while (result != IOM_COMPLETED);

            /* Determine if the page write was successful.                    */
            if (result != IOM_COMPLETED)
            {
                /* Page write failed. So cannot continue.                     */
                break;
            }

            /* Map the logical page number to physical page number in the     *
             * FTL cache module.                                              */
            result = nandFtlCacheMapLogPage(
                         (DDC_nandCacheObj*)&ftlObj->cacheObj,
                         phyBlk,
                         logPage,
                         phyPage);
            if (result != IOM_COMPLETED)
            {
                /* Error in mapping the logical page number to the physical   *
                 * page number                                                */
                break;
            }

            /* Maintain the data buffer address and the logical page number   */
            data = data + ftlObj->devInfo->pageSize;
            logPage = logPage + 1u;

            /* If the logical page number last written was the last page of a *
             * logical block, then move to the next consecutive logical       *
             * block number.                                                  */
            if (logPage == ftlObj->devInfo->pagesPerBlock)
            {
                /* At this point, check if any blocks have to be merged       */
                if (ftlObj->mergeBlock != FTL_NAND_INVAL_BLOCK)
                {
                    /* A block is currently open. So merge operation has to be*
                     * performed now.                                         */
                    result = nandMergeBlock(ftlObj,
                                 &phyBlk,
                                 ftlObj->mergeBlock);
                    if (result != IOM_COMPLETED)
                    {
                        /* Error in merging two physical blocks               */
                        ftlObj->log2PhyTable[logBlk] = ftlObj->mergeBlock;
                        ftlObj->mergeBlock = FTL_NAND_INVAL_BLOCK;
                        break;
                    }

                    /* Update the logical to physical block mapping           */
                    ftlObj->log2PhyTable[logBlk] = phyBlk;

                    /* Invalidate the merge status                            */
                    ftlObj->mergeBlock = FTL_NAND_INVAL_BLOCK;
                }

                /* Move to the next logical block                             */
                logPage = 0u;
                logBlk  = logBlk + 1u;
            }
        }

        /* The requested number of logical sectors have been written. Check if*
         * any blocks have to be merged.                                      */
        if (ftlObj->mergeBlock != FTL_NAND_INVAL_BLOCK)
        {
            /* Two physical blocks have to be merged                          */
            mrResult = nandMergeBlock(ftlObj, &phyBlk, ftlObj->mergeBlock);
            if (mrResult != IOM_COMPLETED)
            {
                /* Error in merging two physical blocks                       */
                ftlObj->log2PhyTable[logBlk] = ftlObj->mergeBlock;
                ftlObj->mergeBlock = FTL_NAND_INVAL_BLOCK;
                result = mrResult;
                break;
            }

            /* Update the logical to physical block mapping                   */
            ftlObj->log2PhyTable[logBlk] = phyBlk;
        }
    }while (flag);
#ifdef NAND_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
        "NAND",
        PSP_DVT_DESC(PSP_DVT_eDATA_SND_END,
                     PSP_DVT_dCHANNEL_ID_INST,
                     PSP_DVT_dPACKET_ID,
                     PSP_DVT_dNONE),
        CHANNELID_INSTANCEID(
            1,
            0),
        data);
#endif /* NAND_INSTRUMENTATION_ENABLED */
    /* Return the status of writting multiple sectors from NAND               */
    return (result);
}



/**
 * \brief Reads single or multiple logical sectors from NAND flash.
 *
 * This function reads single or multiple logical sectors from the NAND
 * flash. The logical sectors have be consecutive.
 */
Int32 DDC_nandFtlRead(DDC_nandFtlObj  *ftlObj,
                      Uint32          logSector,
                      Uint32          numSectors,
                      Uint8           *data)
{
    Int32              result;    /* Result of reading pages from NAND device */
    Uint32             sectorCnt; /* Sector counter                           */
    Uint32             logBlk;    /* Logical block number                     */
    Uint32             phyBlk;    /* Physical block number                    */
    Uint8              logPage;   /* Logical page number                      */
    Uint8              phyPage;   /* Physical page number                     */
    Bool               status;    /* Status of availablity of logical page    */
    LLC_nandSpareArea  spareData; /* Spare data area                          */
    Bool               flag = TRUE;
#ifdef NAND_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
        "NAND",
        PSP_DVT_DESC(PSP_DVT_eRCV_START,
                     PSP_DVT_dCHANNEL_ID_INST,
                     PSP_DVT_dPACKET_ID,
                     PSP_DVT_dNONE),
        CHANNELID_INSTANCEID(
            0,
            0),
        data
        );
#endif /* NAND_INSTRUMENTATION_ENABLED */
    do
    {
        /* Initialize the local variables as required                         */
        result = IOM_EBADARGS;
        flag = FALSE;

        /* Validate the input parameters                                      */
        assert((NULL != ftlObj) && (NULL != data));

        /* Determine the logical block and logical page number                */
        logBlk  = (Uint32) (logSector / ftlObj->devInfo->pagesPerBlock);
        logPage = (Uint8) (logSector % ftlObj->devInfo->pagesPerBlock);

        /* Determine the physical block number of logical block number        */
        phyBlk = ftlObj->log2PhyTable[logBlk];

        /* Read the requested number of sectors                               */
        for (sectorCnt = 0; sectorCnt < numSectors; sectorCnt++)
        {
           /* Specify that we are not giving data space for ecc */
           spareData.eccData = NULL;

            /* Set the status of sector as unavailable                        */
            status = TRUE;

            /* Determine if the logical block is mapped to a physical block   */
            if (phyBlk != FTL_NAND_INVAL_BLOCK)
            {
                /* The logical block is mapped to a physical block. Determine *
                 * if the logical page is mapped to a physical page.          */
                result = nandFtlCacheIsLogPageFree(
                             (DDC_nandCacheObj*)&ftlObj->cacheObj,
                             phyBlk,
                             logPage,
                             &status,
                             (Uint8*)&phyPage);
                if (FALSE == status)
                {
                    /* The physical block number and the physical page number *
                     * is found. Read the physical page.                      */
                    result = ftlObj->llcFxns->readPageFxn(ftlObj->hLlcObj,
                                 phyBlk,
                                 phyPage,
                                 data,
                                 &spareData,
                                 TRUE);
                    if (result != IOM_COMPLETED)
                    {
                        /* Page read has failed. So stop reading and return   *
                         * back.                                              */
                        break;
                    }
                }
            }

            /* If the logical page is not mapped to any physical page, then   *
             * fill up the next pageSize portion of the buffer with default   *
             * value of NAND page.                                            */
            if (TRUE == status)
            {
                /* Fill default page values as data                           */
                memset((Ptr)data, 0xFFu, ftlObj->devInfo->pageSize);

                /* Set the status of read as success                          */
                result = IOM_COMPLETED;
            }

            /* Update the data buffer pointer address and the next logical    *
             * logical sector number to read                                  */
            data = data + ftlObj->devInfo->pageSize;
            logPage = logPage + 1u;

            /* Determine if the last page read was the last page of the       *
             * logical block. If yes, the cross over to the next logical      *
             * block number.                                                  */
            if (logPage == ftlObj->devInfo->pagesPerBlock)
            {
                /* Moving to the next consecutive logical block number        */
                logPage = 0;
                logBlk  = logBlk + 1u;

                /* Determine the physical block number of logical block number*/
                phyBlk = ftlObj->log2PhyTable[logBlk];
            }
        }
    }while (flag);
#ifdef NAND_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
        "NAND",
        PSP_DVT_DESC(PSP_DVT_eRCV_END,
                     PSP_DVT_dCHANNEL_ID_INST,
                     PSP_DVT_dPACKET_ID,
                     PSP_DVT_dNONE),
        CHANNELID_INSTANCEID(
            0,
            0),
        data
        );
#endif /* NAND_INSTRUMENTATION_ENABLED */
    /* Return the status of reading multiple sectors from NAND                */
    return (result);
}


/**
 * \brief  Erases a consecutive group of logical pages
 *
 *         This function erases a consecutive group of logical pages. The
 *         physical blocks are only erased of all the logical pages mapped to
 *         the physical block can be erased. No partial erase of blocks are
 *         performed.
 */
Int32 DDC_nandFtlErase(DDC_nandFtlObj  *ftlObj,
                       Uint32          *rtnLogPage,
                       Uint32          *rtnNumPages)
{
    Int32       result = IOM_COMPLETED; /* Result of erasing a logical pages  */
    Uint32      phyBlkNum;     /* Physical block number to be erased */
    Uint32      logBlkNum;     /* Logical block number to be erased  */
    Uint32      pagesPerBlock; /* Number of pages in a block         */
    Uint32      skipPages;     /* Number of pages to be skipped      */
    Uint32      logPage;
    Uint32      numPages;
    Bool        flag = TRUE;
    do
    {
        result = IOM_EBADARGS;
        flag = FALSE;

        /* Validate the input parameters                                      */
        assert(NULL != ftlObj);
        /* Validate the LLC Handle                                            */
        assert(NULL != ftlObj->hLlcObj);
        assert((NULL != rtnLogPage) && (NULL != rtnNumPages));

        logPage = *rtnLogPage;
        numPages = *rtnNumPages;

        /* Check if the media is write protected                              */
        result = ftlObj->llcFxns->isWriteProtected(ftlObj->hLlcObj,
                     FTL_NAND_INVAL_BLOCK);
        *rtnLogPage     = 0x0;
        *rtnNumPages    = 0x0;

        /* Exit if the media is write protected                               */
        if (result != IOM_COMPLETED)
        {
            break;
        }

        /* Initialize the local variables                                     */
        pagesPerBlock = ftlObj->devInfo->pagesPerBlock;

        /* Determine the starting logical block number                        */
        logBlkNum = (Uint32)(logPage / ftlObj->devInfo->pagesPerBlock);

        /* Determine if the starting logical page number specified as         *
         * parameter is at a block boundary                                   */
        if ((logPage % pagesPerBlock) != 0)
        {
            /* The starting logical page number is not at a block boundary.   *
             * So calculate the number of logical pages to be skipped to reach*
             * the logical page at the next block boundary                    */
            skipPages = pagesPerBlock - (logPage % pagesPerBlock);

            /* If the number of pages to be skipped is more than the number   *
             * of pages to be erased, then the erase operation is considered  *
             * as complete.                                                   */
            if (skipPages >= numPages)
            {
                /* Nothing to erase. Return success.                          */
                result = IOM_COMPLETED;
                break;
            }

            /* Decrement the number of logical pages to be erased and  move   *
             * the logical block number by one                                */
            numPages = numPages - skipPages;
            logBlkNum = logBlkNum + 1U;
        }

        /* Update the starting page number                                    */
        *rtnLogPage = logBlkNum * pagesPerBlock;
        /* Keep erasing logical blocks until there are logical pages to be    *
         * erased. Note: Only blocks with all logical pages in the block      *
         * marked for erase is erased.                                        */
        while (numPages >= pagesPerBlock)
        {
            /* From the logical to physical block mapping table, get the      *
             * physical block number of the block specified                   */
            phyBlkNum = ftlObj->log2PhyTable[logBlkNum];

            /* If the logical block is not mapped to any physical block, then *
             * there is no need to erase                                      */
            if (phyBlkNum != FTL_NAND_INVAL_BLOCK)
            {
                /* Invalidate the cache line that is caching this physical    *
                 * block                                                      */
                nandFtlCacheInvalidate((DDC_nandCacheObj*)&ftlObj->cacheObj,
                    phyBlkNum);

                /* Erase the physical block                                   */
                ftlObj->log2PhyTable[logBlkNum] = FTL_NAND_INVAL_BLOCK;
                result = ftlObj->llcFxns->eraseBlockFxn(ftlObj->hLlcObj,
                             phyBlkNum);
                if (IOM_COMPLETED == result)
                {
                    /* Erase is successful. Mark the physical block           *
                     * as available.                                          */
                    nandWlaUpdateTable((DDC_nandWlaData*)&ftlObj->wlaData,
                         phyBlkNum,
                         FALSE);
                }
                else if (PSP_NAND_E_ERASE_FAIL == result)
                {
                    /* Erase failed - Mark this block as BAD and continue with
                        erase for other blocks, if any                        */
                    ftlObj->llcFxns->markBadBlockFxn(ftlObj->hLlcObj,
                        phyBlkNum);

                    /* Mark this block as unsuable in the WLA and decrement   *
                     * the number of usable blocks in the device.             */
                    nandWlaUpdateTable((DDC_nandWlaData*)&ftlObj->wlaData,
                        phyBlkNum,
                        TRUE);
                    ftlObj->usableBlocks--;
                    result = IOM_COMPLETED;
                }
                else
                {
                    /* Error conditions that requires to notified apps
                       Command Timeouts - No gurantee that next command would
                                           complete - exit now.
                       Write Protection - Should not occur, as we checked for
                                         this when we enter this function..
                       Invalid Arguments - Could be stack overflow, or system
                                            wide issue - exit now.
                       LLC_NAND_E_BLOCK_BAD - Cannot occur here, the logical
                       block was already mapped to physical block and the driver
                       ensures that logical block will not be mapped to BAD
                       block */
                       /* In case there were more than one block to be erased
                       and we could erase only the first block, let the user
                       know that request was not completed. */
                    break;

                }

            }

            /* Decrement the number of pages yet to be erased and move to the *
             * next logical block number                                      */
            numPages  = numPages - pagesPerBlock;
            logBlkNum = logBlkNum + 1U;
            *rtnNumPages    += pagesPerBlock;
        }

    }while (flag);

    /* Return the result of erasing a logical block                           */
    return (result);
}

/* ========================================================================== */
/*                      LOCAL FUNCTION DEFINITIONS                            */
/* ========================================================================== */
/* ========================================================================== */
/*                  Private Functions of NAND FTL Module                      */
/* ========================================================================== */
/**
 * \brief   Initializes the FTL module by reading the spare area
 *
 *          This function initializes the FTL data structures by reading the
 *          information from the spare area of the pages during initialization.
 *
 * \param   ftlObj  [IN] Handle of the NAND FTL module instance
 *
 * \return   IOM_COMPLETED  - On success
 */
static Int32 nandFtlInitFromSpare(DDC_nandFtlObj *ftlObj)
{
    Uint32            idx;         /* General purpose counter                */
    Uint32            totalBlocks; /* Total number of blocks in the device   */
    Uint32            badBlocks;   /* Bad blocks detected during init        */
    Ptr               hLlcObj;     /* Handle of LLC module                   */
    Int32             result;      /* Result of initializing from spare area */
    LLC_nandSpareArea spareData;   /* Spare data information                 */
    Bool              flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert(NULL != ftlObj);

        if (NULL == ftlObj->hLlcObj)
        {
            result = IOM_EBADARGS;
            break;
        }

        /* Initialize the local variables as required                         */
        flag              = FALSE;
        totalBlocks       = ftlObj->devInfo->numBlocks;
        badBlocks         = 0;
        hLlcObj           = (LLC_nandObj*)ftlObj->hLlcObj;
        spareData.eccData = NULL;
        result            = IOM_COMPLETED;

        /* Read the spare area of all the usable blocks and initialize the    *
         * logical to physical mapping table and WLA                          */
        for (idx = (ftlObj->protectedBlocks + 1u); idx < totalBlocks; idx++)
        {
           /* Determing if the block is BAD */
           assert(NULL != hLlcObj);
           result = ftlObj->llcFxns->isBlockBad(hLlcObj, idx);
           if (IOM_COMPLETED == result)
           {
               /* Block is not BAD, check if in use. */
               /* Read the spare area of the first page of the block */
               result = ftlObj->llcFxns->readSpareFxn(hLlcObj,
                            idx,
                            0,
                            &spareData);
               if (IOM_COMPLETED == result)
               {
                    /* The block is not bad. Update the logical to physical   *
                     * block mapping if the block is used and update the WLA. */
                    if (spareData.logicalBlock != 0xFFFFFFu)
                    {
                        /* The block is a used block                          */
                        nandWlaUpdateTable(
                            (DDC_nandWlaData*)&ftlObj->wlaData,
                            idx,
                            TRUE);

                        ftlObj->log2PhyTable[spareData.logicalBlock] = idx;
                    }
               }
               else
                {
                    /* Timeout should not occur here, as the previous commands
                       have succeded. */
                    /* Reading the spare area failed. The block is bad block.
                       So mark this block as used in WLA */
                    nandWlaUpdateTable((DDC_nandWlaData*)&ftlObj->wlaData,
                        idx,
                        TRUE);
                    badBlocks++;
                   result = IOM_COMPLETED;
                }
           }
           else if ((LLC_NAND_E_BLOCK_BAD == result) ||
                    (PSP_NAND_E_READ_FAIL == result))
           {
                /* The block is bad block. So mark this block as used in
                   WLA */
                nandWlaUpdateTable((DDC_nandWlaData*)&ftlObj->wlaData,
                    idx,
                    TRUE);
                badBlocks++;
                result = IOM_COMPLETED;
           }
           else
           {
               /* Other errors timeouts, invalid args, etc... */
               /* Nothing much could be done now, exit */
               break;

           }
        }

       if (IOM_COMPLETED == result)
       {
            /* Set the number of available and usable blocks to zero          */
            ftlObj->availableBlocks = 0;
            ftlObj->usableBlocks    = 0;

            /* Compute the number of available blocks                         */
            if ((totalBlocks - badBlocks) > (ftlObj->protectedBlocks + 1u))
            {
                ftlObj->availableBlocks = (totalBlocks - badBlocks) -
                                          (ftlObj->protectedBlocks + 1u);
            }

            /* Compute the number of usable blocks                            */
            if (ftlObj->availableBlocks > PSP_NAND_RESERVED_BLOCKS)
            {
                ftlObj->usableBlocks = (ftlObj->availableBlocks) -
                                       PSP_NAND_RESERVED_BLOCKS;
            }
       }
       /* If the operation has failed, let the FTL init know the status       */
    }while (flag);

    /* Return the result of initializing FTL by reading the spare area        */
    return (result);
}


/**
 * \brief   Initializes the FTL module by erasing the blocks
 *
 *          This function initializes the FTL data structures by erasing the
 *          blocks of the device during initialization.
 *
 * \param   ftlObj  [IN] Handle of the NAND FTL module instance
 *
 * \return   IOM_COMPLETED  - On success
 */
static Int32 nandFtlInitErase(DDC_nandFtlObj  *ftlObj)
{
    Uint32      idx;         /* General purpose counter                */
    Uint32      totalBlocks; /* Total number of blocks in the device   */
    Uint32      badBlocks;   /* Bad blocks detected during init        */
    Ptr         hLlcObj;     /* Handle of LLC module                   */
    Int32       result = IOM_COMPLETED; /* Result of initializing by erasing  */
    Bool        flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert(NULL != ftlObj);
        flag = FALSE;

        if (NULL == ftlObj->hLlcObj)
        {
            result = IOM_EBADARGS;
            break;
        }

        /* Check if the media is write protected                              */
        result = ftlObj->llcFxns->isWriteProtected(ftlObj->hLlcObj,
                     FTL_NAND_INVAL_BLOCK);

        /* Exit if the media is write protected                               */
        if (result != IOM_COMPLETED)
        {
            break;
        }

        /* Initialize the local variables as required                         */
        totalBlocks = ftlObj->devInfo->numBlocks;
        badBlocks   = 0u;
        hLlcObj     = ftlObj->hLlcObj;

        /* Except the first block and the protected number of blocks, erase   *
         * all the other blocks in the device.                                */
        for (idx = (ftlObj->protectedBlocks + 1u); idx < totalBlocks; idx++)
        {
            /* Erase the block                                                */
            assert(NULL != hLlcObj);
            result = ftlObj->llcFxns->eraseBlockFxn(hLlcObj, idx);

            /* Determine if there are any errors during the block erase       *
             * operation                                                      */
            if (result != IOM_COMPLETED)
            {
                /* Determine if the block became bad during this erase        */
            if (PSP_NAND_E_ERASE_FAIL == result)
            {
                    /* The block became bad during this erase. Mark this block*
                     * as bad block in the spare area of the block.           */
                    ftlObj->llcFxns->markBadBlockFxn(ftlObj->hLlcObj, idx);

                    /* Mark this block as unsuable in the WLA and increment
                       the number of bad blocks in the device. */
                    result = IOM_COMPLETED;
                    nandWlaUpdateTable((DDC_nandWlaData*)&ftlObj->wlaData,
                        idx,
                        TRUE);
                    badBlocks++;
                }
                else if (LLC_NAND_E_BLOCK_BAD == result)
                {

                    /* This block is marked BAD alredy, update the our data
                      structures accordingly. App/file sys need not be informed
                      of this. */
                    result = IOM_COMPLETED;
                    /* Mark this block as unsuable in the WLA and increment   *
                     * the number of bad blocks in the device.                */
                    nandWlaUpdateTable((DDC_nandWlaData*)&ftlObj->wlaData,
                        idx,
                        TRUE);
                    badBlocks++;
                }
                else
                {
                    /* Error conditions that requires to notified apps
                       Command Timeouts - No gurante that next command would
                                            complete - exit now.
                       Write Protection - Should occur, as we checked for this
                                          when we enter this function.
                       Invalid Arguments - Could be stack overflow, or system
                                             wide issue - exit now. */
                    /* In case there were more than one block to be erased and
                        we could erase only the first block, let the user
                        know that request was not completed. */
                    break;
                }
            }
        }

        if (IOM_COMPLETED == result)
        {
            /* Set the number of available and usable blocks to zero          */
            ftlObj->availableBlocks = 0;
            ftlObj->usableBlocks    = 0;

            /* Compute the number of available blocks                         */
            if ((totalBlocks - badBlocks) > (ftlObj->protectedBlocks + 1u))
            {
                ftlObj->availableBlocks = (totalBlocks - badBlocks) -
                                          (ftlObj->protectedBlocks + 1u);
            }

            /* Compute the number of usable blocks                            */
            if (ftlObj->availableBlocks > PSP_NAND_RESERVED_BLOCKS)
            {
                ftlObj->usableBlocks    = ftlObj->availableBlocks -
                                          PSP_NAND_RESERVED_BLOCKS;
            }
        }
        else
        {
            /* Exit, let the caller know that erase failed                    */
            break;
        }
    }while (flag);

    /* Return the result of initializing FTL with erase                       */
    return (result);
}


/* ========================================================================== */
/*                         Wear Levelling Functions                           */
/* ========================================================================== */

/**
 * \brief   Initializes the FTL WLA module.
 *
 *          This function initializes the FTL WLA modue with default values.
 *          All the blocks represented by WLA are marked as avaialable except
 *          the protected blocks.
 *
 * \param   wlaData         [IN] Handle of the NAND WLA module instance
 * \param   protectedBlocks [IN] Number of blocks to protect
 *
 * \return   IOM_COMPLETED  - On success
 */
static Void nandWlaInit(DDC_nandWlaData  *wlaData,
                            Uint32           protectedBlocks)
{
    Uint32  idx;   /* Counter variable */

    /* Validate the input parameters                                      */
    assert(NULL != wlaData);

    /* Initialize the WLA bit map table with default values               */
    memset((Ptr)(wlaData->wlaBitMap), 0xFFu, (wlaData->bitMapLength * 4u));

    /* Mark all the protected blocks as used                              */
    for (idx = 1u; idx <= protectedBlocks; idx++)
    {
        nandWlaUpdateTable(wlaData, idx, TRUE);
    }

    /* Set the next block from which a search for the next free block     */
    wlaData->nextBlock = 1u;

    /* The first block is always assumed to be used                       */
    wlaData->wlaBitMap[0] &= ~(1u);
    wlaData->freeBlocks = wlaData->freeBlocks - 1U;
}

/**
 * \brief   Updates the status of block in the WLA table
 *
 *          This function updates the status of the block in thw WLA table as
 *          either available or used.
 *
 * \param   wlaData   [IN] Handle of the NAND WLA module instance
 * \param   blockNum  [IN] Number of physical block
 * \param   isUsed    [IN] Status of the block
 *
 * \return   IOM_COMPLETED  - On success
 */
static Void nandWlaUpdateTable(DDC_nandWlaData  *wlaData,
                               Uint32           blockNum,
                               Bool             isUsed)
{
    Uint32       wordNum;  /* Word number in the WLA bitmap table */
    Uint32       bitNum;   /* Bit number in the WLA bitmap table  */

    /* Validate the input parameters                                      */
    assert(NULL != wlaData);

    /* Find out the word number and bit number that should be marked      */
    wordNum = blockNum >> 5u;
    bitNum  = blockNum & 0x1Fu;

    /* Mark the block as used or unused. Used block is marked as "zero".  */
    if (TRUE == isUsed)
    {
        /* Mark the block as used                                         */
        wlaData->wlaBitMap[wordNum] &= (Uint32)(~(1u << bitNum));
        wlaData->freeBlocks = wlaData->freeBlocks - 1U;
    }
    else
    {
        /* Mark the block as available                                    */
        wlaData->wlaBitMap[wordNum] |= (1u << bitNum);
        wlaData->freeBlocks = wlaData->freeBlocks + 1U;
    }
}


/**
 * \brief   Finds a free block from the WLA table
 *
 *          This function finds a free block from the WLA table. Any available
 *          block found from the table is marked as used.
 *
 * \param   wlaData  [IN]  Handle of the NAND FTL module instance
 * \param   blockNum [OUT] Block number allocated from WLA.
 *
 * \return   IOM_COMPLETED  - On success
 */
static Int32 nandWlaFindFreeBlock(DDC_nandWlaData *wlaData, Uint32 *blockNum)
{
    Uint32      *bitMap;      /* Pointer to the WLA bitmap table             */
    Uint32      mask;         /* Mask for extracting data from WLA bitmap    */
    Uint32      wordNum;      /* Word number in the WLA bitmap table         */
    Uint32      bitNum;       /* Bit number in the WLA bitmap table          */
    Uint32      wrapWordNum;  /* WLA word number on which wrap will occur    */
    Bool        wrap;         /* Wrap status flag                            */
    Int32       result;       /* Result of searching for a free block in WLA */
    Bool        flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert((NULL != wlaData) && (NULL != blockNum));

        /* Initialize the local variables as required                         */
        flag        = FALSE;
        bitMap      = wlaData->wlaBitMap;
        wordNum     = wlaData->nextBlock >> 5u;
        bitNum      = wlaData->nextBlock & 0x1Fu;
        wrapWordNum = wordNum;
        wrap        = FALSE;
        result      = IOM_COMPLETED;
        if (NULL == bitMap)
        {
            break;
        }

        /* Search for the next free block                                     */
        mask = ~((1u << bitNum) - 1u);
        while (0x00 == (bitMap[wordNum] & mask))
        {
            /* If all the bits have been searchedthen there are no free blocks*/
            if (TRUE == wrap)
            {
                /* There are no free blocks available                         */
                result = PSP_NAND_E_NO_FREE_BLOCK;
                break;
            }

            /* If the search hits the bottom of the bit-map table, then wrap  *
             * back to the start of the table.                                */
            if ((wordNum + 1u) == wlaData->bitMapLength)
            {
                /* Wrap to the top of the table                               */
                wordNum = 0;
            }
            else
            {
                /* Move to the next word in the table                         */
                wordNum++;
            }

            /* Check if search is wrapping, that is, has search reached the   *
             * same word number from where the search had started.            */
            if (wordNum == wrapWordNum)
            {
                /* Searching is now wrapping                                  */
                wrap = TRUE;
            }

            /* Reset the next bit number and the mask for next word search    */
            bitNum = 0x0u;
            mask   = 0xFFFFFFFFu;
        }

        /* Search of bit map is complete. Check if a free block is found.     */
        if (IOM_COMPLETED == result)
        {
            /* A word with free block is found. Search through and find the   *
             * number of the free block.                                      */
            mask = 1u << bitNum;
            while (0u == (bitMap[wordNum] & mask))
            {
                /* Move to the next bit and keep searching                    */
                bitNum++;
                mask = 1u << bitNum;
            }

            /* A free block number has been found                             */
            *blockNum = (wordNum << 5u) + bitNum;

            /* Note down the starting position for the next search. The search*
             * loops through all the bits in the WLA in forward direction,    *
             * that is, block 1, block 2, block 3 .... etc. So note down the  *
             * next bit in WLA table from where search should begin.          */
            wlaData->nextBlock = (*blockNum) + 1u;

            /* If the search pointer moves beyond the number of physical
               blocks available, then wrap the search in the WLA */
            if (wlaData->nextBlock == (wlaData->bitMapLength << 5u))
            {
                wlaData->nextBlock = 1u;
            }

            /* Mark the block as used in the bitmap table                     */
            nandWlaUpdateTable(wlaData, *blockNum, TRUE);
        }
    }while (flag);

    /* Return the result of finding the next free block                       */
    return (result);
}

/**
 * \brief   Returns the currently available number of free blocks
 *
 *          This function returns the currently available number of free blocks
 *          in the system.
 *
 * \param   wlaData  [IN]  Handle of the NAND WLA module instance
 *
 * \return   IOM_COMPLETED  - On success
 */
static Uint32 nandWlaGetFreeBlockCount(DDC_nandWlaData  *wlaData)
{
    Uint32  freeBlocks = 0;   /* Number of free blocks available */

    /* Validate the input parameters                                      */
    assert(NULL != wlaData);

    /* Determine the number of free blocks                                */
    freeBlocks = wlaData->freeBlocks;

    /* Return the number of free blocks in the system                         */
    return (freeBlocks);
}


/* ========================================================================== */
/*                           NAND CACHE FUNCTIONS                             */
/* ========================================================================== */
/**
 * \brief   Initializes the FTL cache module
 *
 *          This function initializes the FTL cache module. It resets the values
 *          of the cache lines to default.
 *
 * \param   cacheObj      [IN] Handle of the NAND FTL cache module instance
 * \param   pagesPerBlock [IN] Number of pages per block to support
 * \param   hLlcObj       [IN] Handle of the LLC NAND module instance
 * \param   llcFxns       [IN] Function pointer table of LLC functions
 */
static Void nandFtlCacheInit(DDC_nandCacheObj   *cacheObj,
                             Uint32             pagesPerBlock,
                             Ptr                hLlcObj,
                             LLC_nandFxns       *llcFxns)
{
    Uint16  idx;  /* Temporary index variable   */
    Bool        flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert(((NULL != cacheObj) && (NULL != llcFxns)) && (NULL != hLlcObj));

        /* Initialize the cache object                                        */
        flag = FALSE;
        cacheObj->nextDealloc   = 0u;
        cacheObj->pagesPerBlock = (Uint16)pagesPerBlock;
        cacheObj->hLlcObj       = hLlcObj;
        cacheObj->llcFxns       = llcFxns;

        /* Initialize the cache lines in the cache object with invalid        *
         * page numbers and invalid physical block numbers                    */
        for (idx = 0u; idx < PSP_NAND_MAX_CACHE_LINES; idx++)
        {
            cacheObj->cacheLine[idx].phyBlock = FTL_NAND_INVAL_BLOCK;
            cacheObj->cacheLine[idx].nextPhyFreePage = 0u;
            memset((Ptr)&(cacheObj->cacheLine[idx].log2Phy),
                FTL_NAND_INVAL_PAGE,
                pagesPerBlock);
        }
    }while (flag);
}

/**
 * \brief   Updates the status of block in the WLA table
 *
 *          This function updates the status of the block in thw WLA table as
 *          either available or used.
 */
static Uint16 nandFtlCacheGetIndex(DDC_nandCacheObj  *cacheObj,
                                   Uint32            phyBlockNum)
{
    Uint16  cacheLineIndex;  /* Index of the cache line */
    Uint16  idx;             /* Cache line counter      */
    Bool        flag = TRUE;
    do
    {
        /* Initialize the local variables as required                         */
        cacheLineIndex = FTL_NAND_E_INVAL_CACHE_INDEX;
        flag = FALSE;

        /* Validate the input parameters                                      */
        assert(NULL != cacheObj);

        /* Search through all the cache lines to find the physical block      */
        for (idx = 0; idx < PSP_NAND_MAX_CACHE_LINES; idx++)
        {
            if (cacheObj->cacheLine[idx].phyBlock == phyBlockNum)
            {
                /* Cache line with the specfied physical block number found   */
                cacheLineIndex = idx;
                break;
            }
        }
    }while (flag);

    /* Return the index of the cache line                                     */
    return (cacheLineIndex);
}


/**
 * \brief  Invalidates a cache line
 *
 *         This function invalidates a cache line that is caching the info
 *         of the physical blocks specfied.
 *
 * \param  cacheObj     [IN] Handle of the NAND FTL cache module instance
 * \param  phyBlockNum  [IN] Physical block number
 *
 * \return   IOM_COMPLETED  - On success
 */
static Void nandFtlCacheInvalidate(DDC_nandCacheObj  *cacheObj,
                                   Uint32            phyBlockNum)
{
    Uint8     idx;            /* General purpose counter                      */
    Uint16    lineIdx;        /* Line index on which physical block is cached */
    Uint16    pagesPerBlock;  /* Number of pages per block                    */
    Uint8     *table;         /* Pointer to table of physical page numbers    */
    Bool      flag = TRUE;
    
    do
    {
        /* Validate the input parameters                                      */
        assert(NULL != cacheObj);
        flag = FALSE;

        /* Get the index of the cache line                                    */
        lineIdx = nandFtlCacheGetIndex(cacheObj, phyBlockNum);
        if (FTL_NAND_E_INVAL_CACHE_INDEX == lineIdx)
        {
            /* The physical block is not cached                               */
            break;
        }

        /* The line index on which the physical block is cached has been      *
         * found. Initialize the local variables as required                  */
        table = (Uint8*)&(cacheObj->cacheLine[lineIdx].log2Phy[0]);
        pagesPerBlock = cacheObj->pagesPerBlock;

        /* Invalidate the logical page to physical page mapping information   */
        for (idx = 0; idx < pagesPerBlock; idx++)
        {
            table[idx] = (Uint8)FTL_NAND_INVAL_PAGE;
        }

        /* Unmap the cache line from the physical block number                */
        cacheObj->cacheLine[lineIdx].nextPhyFreePage = 0;
        cacheObj->cacheLine[lineIdx].phyBlock = FTL_NAND_INVAL_BLOCK;
    }while (flag);
}


/**
 * \brief  Finds a free cache line for caching the physical block.
 *
 *         This function finds a free cache line for caching a physical block.
 *         If there are no cache lines available, then one of the cache lines is
 *         evicted and that cache line index is returned. This function is used
 *         within the cache module functions only.
 *
 * \param  cacheObj     [IN] Handle of the NAND FTL cache module instance
 *
 * \return   IOM_COMPLETED  - On success
 */
static Uint16 nandFtlCacheFindFreeLine(DDC_nandCacheObj  *cacheObj)
{
    Uint32  idx;  /* General purpose counter */
    Bool    flag = TRUE;
    do
    {
        /* Initialize the local variables                                     */
        idx = 0;
        flag = FALSE;

        /* Validate the input parameters                                      */
        assert(NULL != cacheObj);


        /* Check if there is any free cache line                              */
        for (idx = 0u; idx < PSP_NAND_MAX_CACHE_LINES; idx++)
        {
            if (FTL_NAND_INVAL_BLOCK == cacheObj->cacheLine[idx].phyBlock)
            {
                break;
            }
        }

        /* If no free lines are found, then invalidate a line and use it      */
        if (PSP_NAND_MAX_CACHE_LINES == idx)
        {
            /* Get the cache line number that has to be selected for          *
             * invalidation (evict)                                           */
            idx = cacheObj->nextDealloc;

            /* Invalidate the selected cache line                             */
            nandFtlCacheInvalidate(cacheObj, cacheObj->cacheLine[idx].phyBlock);

            /* Note down the next cache line that would get evicted if need   *
             * be. Cache lines will be evicted in round-robin method          */
            cacheObj->nextDealloc = (cacheObj->nextDealloc + 1u) &
                                     (PSP_NAND_MAX_CACHE_LINES - 1u);
        }
    }while (flag);

    /* Return the cache line index to use                                     */
    return ((Uint16)(idx & (PSP_NAND_MAX_CACHE_LINES - 1u)));
}


/**
 * \brief  Prefeteches a block into a cache line
 *
 *         This function prefetechs the logical to physical page mapping of a
 *         block into a cache line.
 *
 * \param  cacheObj  [IN]  Handle of the NAND FTL cache module instance
 * \param  phyBlock  [IN]  Physical block number
 * \param  lineIdx   [OUT] Cache line index
 *
 * \return   IOM_COMPLETED  - On success
 */
static Int32 nandFtlCachePrefetch(DDC_nandCacheObj  *cacheObj,
                                  Uint32            phyBlock,
                                  Uint16            *lineIdx)
{
    Int32              result;        /* Result of prefetching cache info   */
    Uint8              idx;           /* General purpose counter            */
    Uint8              *table;        /* Logical to physical page map table */
    LLC_nandSpareArea  spareData;     /* Spare data information             */
    Uint16             pagesPerBlock; /* Number of pager per block          */
    Bool               flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert((NULL != cacheObj) && (NULL != lineIdx));

        /* Initialize the local variables                                     */
        result = IOM_COMPLETED;
        flag = FALSE;

        /* Get the index of the cache line                                    */
        *lineIdx = nandFtlCacheGetIndex(cacheObj, phyBlock);
        if ((*lineIdx) != FTL_NAND_E_INVAL_CACHE_INDEX)
        {
            /* The physical block is already cached                           */
            break;
        }

        /* Get a next cache line to use                                       */
        *lineIdx = nandFtlCacheFindFreeLine(cacheObj);

        /* Initialize the local variables as required                         */
        table = (Uint8*)&(cacheObj->cacheLine[*lineIdx].log2Phy[0u]);
        pagesPerBlock = cacheObj->pagesPerBlock;
        spareData.eccData = NULL;

        /* For all the physical pages, determibe the logical page number from *
         * the spare area of the page                                         */
        for (idx = 0u; idx < pagesPerBlock; idx++)
        {
            /* Read the spare area of the page. Note: The result of the       *
             * read spare area function is ignored because this implementation*
             * of the driver ignores timeout conditions when reading from     *
             * spare area for filling up cache lines.                         */
            cacheObj->llcFxns->readSpareFxn(cacheObj->hLlcObj,
                phyBlock,
                idx,
                &spareData);

            /* If this physical page is not written, then stop because there  *
             * will be no further physical page written in this block.        *
             * Remember: The physical blocks are always written in sequential *
             * order in a block starting from first page.                     */
            if (FTL_NAND_INVAL_PAGE == spareData.logicalPage)
            {
                break;
            }

            /* Note down the logical page number for the physical page        */
            table[spareData.logicalPage] = idx;
        }

        /* The cache line prefetch is complete. So map the cache line to      *
         * a physical block number.                                           */
        cacheObj->cacheLine[*lineIdx].nextPhyFreePage = idx;
        cacheObj->cacheLine[*lineIdx].phyBlock = phyBlock;
    }while (flag);

    /* Return the result of prefetching a cache line                          */
    return (result);
}


/**
 * \brief  Determines if the logical page is free
 *
 *         This function determines if the logical page number is free in the
 *         specified physical block number.
 *
 * \param  cacheObj  [IN]  Handle of the NAND FTL module instance
 * \param  phyBlock  [IN]  Physical block number
 * \param  logPage   [IN]  Logical page number
 * \param  status    [OUT] Status of the logical page
 * \param  phyPage   [OUT] Physical page number of logical page
 *
 * \return   IOM_COMPLETED  - On success
 */
static Int32 nandFtlCacheIsLogPageFree(DDC_nandCacheObj  *cacheObj,
                                       Uint32            phyBlock,
                                       Uint8             logPage,
                                       Bool              *status,
                                       Uint8             *phyPage)
{
    Int32       result;   /* Result of determing if the logical page is free */
    Uint16      lineIdx;  /* Cache line index                                */
    Bool        flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert(((NULL != cacheObj) && (NULL != phyPage)) && (NULL != status));

        /* Initialize the local variables as required                         */
        *status = FALSE;
        result  = IOM_COMPLETED;
        flag = FALSE;

        /* Get the index of the cache line                                    */
        lineIdx = nandFtlCacheGetIndex(cacheObj, phyBlock);
        if (FTL_NAND_E_INVAL_CACHE_INDEX == lineIdx)
        {
            /* The specified block is not in the cache. So cache it first.    */
            result = nandFtlCachePrefetch(cacheObj,
                         phyBlock,
                         (Uint16*)&lineIdx);

            if (result != IOM_COMPLETED)
            {
                /* Error in prefetching a page information into cache         */
                break;
            }
        }

        /* Find out if the logical page is free                               */
        *phyPage = cacheObj->cacheLine[lineIdx].log2Phy[logPage];
        if (FTL_NAND_INVAL_PAGE == (*phyPage))
        {
            /* The logical page is not mapped to a physical page              */
            *status = TRUE;
        }
    }while (flag);

    /* Return the result of determing if the logical page is free             */
    return (result);
}


/**
 * \brief  Maps a logical page to physical page
 *
 *         This function maps a logical page to the physical page in the cache.
 *
 * \param  cacheObj  [IN] Handle of the NAND FTL cache module instance
 * \param  phyBlock  [IN] Physical block number
 * \param  logPage   [IN] Logical page number
 * \param  phyPage   [IN] Physical page number
 *
 * \return   IOM_COMPLETED  - On success
 */
static Int32 nandFtlCacheMapLogPage(DDC_nandCacheObj  *cacheObj,
                                    Uint32            phyBlock,
                                    Uint8             logPage,
                                    Uint8             phyPage)
{
    Uint16      lineIdx;  /* Cache line index                                */
    Int32       result;   /* Result of mapping logical page to physical page */
    Bool        flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert(NULL != cacheObj);

        /* Initialize the local variables as required                         */
        result = IOM_COMPLETED;
        flag = FALSE;

        /* Get the index of the cache line                                    */
        lineIdx = nandFtlCacheGetIndex(cacheObj, phyBlock);
        if (FTL_NAND_E_INVAL_CACHE_INDEX == lineIdx)
        {
            /* The specified block is not in the cache. So cache it first.    */
            result = nandFtlCachePrefetch(cacheObj,
                         phyBlock,
                         (Uint16*)&lineIdx);
            if (result != IOM_COMPLETED)
            {
                /* Error in prefetching cache line contents                   */
                break;
            }
        }

        /* Map the logical page                                               */
        cacheObj->cacheLine[lineIdx].log2Phy[logPage] = phyPage;
        cacheObj->cacheLine[lineIdx].nextPhyFreePage++;
    }while (flag);

    /* Return the result of mapping the logical page                          */
    return (result);
}


/**
 * \brief  Gets a free physical page from a physical block
 *
 *         This function gets a free physical page from the specified physical
 *         block. This function will not update this mapping in the cache.
 *
 * \param  cacheObj  [IN] Handle of the NAND FTL cache module instance
 * \param  phyBlock  [IN] Physical block number
 * \param  phyPage   [IN] Physical page number
 *
 * \return   IOM_COMPLETED  - On success
 */
static Int32 nandFtlCacheGetPhyFreePage(DDC_nandCacheObj  *cacheObj,
                                        Uint32            phyBlock,
                                        Uint8             *phyPage)
{
    Uint16      lineIdx; /* Index of the cache line                  */
    Int32       result;  /* Result of finding the free physical page */
    Bool        flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert((NULL != cacheObj) && (NULL != phyPage));

        /* Initialize the local variables as required                         */
        result = IOM_COMPLETED;
        flag = FALSE;

        /* Get the index of the cache line                                    */
        lineIdx = nandFtlCacheGetIndex(cacheObj, phyBlock);
        if (FTL_NAND_E_INVAL_CACHE_INDEX == lineIdx)
        {
            /* The specified block is not in the cache. So cache it first.    */
            result = nandFtlCachePrefetch(cacheObj,
                         phyBlock,
                         (Uint16*)&lineIdx);
            if (result != IOM_COMPLETED)
            {
                /* Error in prefetching cache line contents                   */
                break;
            }
        }

        /* Map the logical page                                               */
        *phyPage = cacheObj->cacheLine[lineIdx].nextPhyFreePage;
    }while (flag);

    /* Return the result of mapping the logical page                          */
    return (result);
}



 /**
 * \brief  Sets the physical block number in the cache line
 *
 *         This function sets a new physical block number for a specified cache
 *         line.This function is used in cases of block move operations where
 *         the physical block number moved to is set as the new physical block
 *         number in the cache line.
 *
 * \param  cacheObj     [IN] Handle of the NAND FTL cache module instance
 * \param  oldPhyBlock  [IN] Current physical block number
 * \param  newPhyBlock  [IN] New physical block number to be assigned.
 *
 * \return   IOM_COMPLETED  - On success
 */
static Void nandFtlCacheLineSetPhyBlock(DDC_nandCacheObj  *cacheObj,
                                        Uint32            oldPhyBlock,
                                        Uint32            newPhyBlock)
{
    Uint32  lineIdx;   /* Cache line index counter */
    Bool    flag = TRUE;
    
    do
    {
        /* Validate the input parameters */
        assert(NULL != cacheObj);
        flag = FALSE;

        /* Get the index of the cache line                                    */
        lineIdx = nandFtlCacheGetIndex(cacheObj, oldPhyBlock);

        /* If a valid cache line is found, then update the cache line with    *
         * the new physical block number                                      */
        if (lineIdx != FTL_NAND_E_INVAL_CACHE_INDEX)
        {
            cacheObj->cacheLine[lineIdx].phyBlock = newPhyBlock;
        }
    }while (flag);
}



/**
 * \brief  Copy a physical page to another physical page.
 *
 *         This function copies a physical page to another physical page. The
 *         pages could be seperate physical blocks.
 *
 * \param  ftlObj      [IN] Handle of the NAND FTL module instance
 * \param  srcPhyBlk   [IN] Source physical block
 * \param  srcPhyPage  [IN] Source physical page
 * \param  dstPhyBlk   [IN] Destination physical block
 * \param  dstPhyPage  [IN] Destination physical page
 *
 * \return   IOM_COMPLETED  - On success
 */
static Int32 nandFtlCopyPage(DDC_nandFtlObj  *ftlObj,
                             Uint32          srcPhyBlk,
                             Uint8           srcPhyPage,
                             Uint32          dstPhyBlk,
                             Uint8           dstPhyPage)
{
    Int32              result;    /* Result of copying a physical page */
    LLC_nandSpareArea  spareData; /* Spare area data                   */
    Uint8              *data;     /* Temporary data buffer pointer     */
    static Uint8       eccParity[LLC_NAND_MAX_ECC_PARITY_LENGTH];
                                  /* Place holder for parity data      */
    Bool        flag = TRUE;
    
    do
    {
        /* Validate the input parameters */
        assert(NULL != ftlObj);

        /* Initialize the local variables */
        result = IOM_EBADARGS;
        flag = FALSE;
        spareData.eccData = NULL;

        data = (Uint8*)(((Uint32)(pageData+((MEM_CACHE_LINE_SIZE)-(1U))))
                            & (~((MEM_CACHE_LINE_SIZE)-(1U))));
        if (NULL == data)
        {
            break;
        }

        /* Read the source page contents */
        result = ftlObj->llcFxns->readPageFxn(ftlObj->hLlcObj,
                     srcPhyBlk,
                     srcPhyPage,
                     data,
                     &spareData,
                     TRUE);
        if (result != IOM_COMPLETED)
        {
            if (PSP_NAND_E_ECC_FAIL == result)
            {
                /* Initialize parity data - which requires to remain
                   without any changes for this page.
                   On subsequent read the ECC error would be reported */
                /* KW says "Buffer overflow, array index of 'eccParity' may be
                   outside the bounds. Array 'eccParity' of size 40, may use
                   index values 0..2147483646" which is a problem with KW*/
                memset(eccParity, 0x0, sizeof(eccParity));

                /* Re-Read the page along with parity */
                spareData.eccData = eccParity;

                /* Re-Read the source page contents with spare contents */
                result = ftlObj->llcFxns->readPageFxn(ftlObj->hLlcObj,
                             srcPhyBlk,
                             srcPhyPage,
                             data,
                             &spareData,
                             FALSE);
                /* Ignore Errors - Right now error on block move are not
                   going to be handled */
                /* Write the page */
                result = ftlObj->llcFxns->writePageFxn(ftlObj->hLlcObj,
                             dstPhyBlk,
                             dstPhyPage,
                             data,
                             &spareData,
                             FALSE);
                /* On error - treat as write failure and return */
                if (result != IOM_COMPLETED)
                {
                    result = PSP_NAND_E_WRITE_FAIL;
                    break;
                }
                /* Upon subsequent read, ECC error would be notified. Apps
                   could decide the next course of action */
           }
           else
           {
                result = PSP_NAND_E_READ_FAIL;
                break;
           }
        }
       else
       {
           /* Ensure to clear ecc parity */
           spareData.eccData = NULL;
            /* Write the destination page contents */
            result = ftlObj->llcFxns->writePageFxn(ftlObj->hLlcObj,
                         dstPhyBlk,
                         dstPhyPage,
                         data,
                         &spareData,
                         TRUE);
            if (result != IOM_COMPLETED)
            {
                result = PSP_NAND_E_WRITE_FAIL;
                break;
            }
       }
    }while (flag);

    /* Return the result of copying the page                                  */
    return (result);
}


/**
 * \brief  Copies the contents of one physical block to another physical block
 *
 *         This function copies the pages of a source physical block to a
 *         destination physical block. If there are any page write errors, then
 *         the block copy is terminated.
 *
 * \param  ftlObj     [IN] Handle of the NAND FTL module instance
 * \param  srcPhyBlk  [IN] Source physical block
 * \param  dstPhyBlk  [IN] Destination physical block
 * \param  numPages   [IN] Number of pages to be copied
 *
 * \return   IOM_COMPLETED  - On success
 */
static Int32 nandFtlCopyBlock(DDC_nandFtlObj  *ftlObj,
                              Uint32          srcPhyBlk,
                              Uint32          dstPhyBlk,
                              Uint8           numPages)
{
    Int32       result; /* Result of the block copy operation */
    Uint8       idx;    /* General purpose counter            */
    Bool        flag = TRUE;
    
    do
    {
        /* Initialize the local variables                                     */
        result = IOM_EBADARGS;
        flag = FALSE;

        /* Validate the input parameters                                      */
        assert(NULL != ftlObj);

        /* Copy all the valid pages from the source physical block to the     *
         * destination physical block                                         */
        for (idx = 0u; idx < numPages; idx++)
        {
            /* Copy a page                                                    */
            result = nandFtlCopyPage(ftlObj,
                         srcPhyBlk,
                         idx,
                         dstPhyBlk,
                         idx);
            /* If there is error in writting the page, terminate the  block   *
             * copy process                                                   */
            if (PSP_NAND_E_WRITE_FAIL == result)
            {
                break;
            }
        }
    }while (flag);

    /* Ignore all other errors                                                */
    if (result != PSP_NAND_E_WRITE_FAIL)
    {
        result = IOM_COMPLETED;
    }

    /* Return the result of copying a block                                   */
    return (result);
}


/**
 * \brief  Move contents of a block to a new block
 *
 *         This function moves a contents of a block to a new block. This
 *         function is called to move contents to a new block when a write
 *         failure occurs on one of the pages of the block.This function retries
 *         the move block operation until the move operation is complete. The
 *         physical block number to which the block has been moved is returned
 *         in the "srcPhyBlk" parameter.
 *
 * \param  ftlObj     [IN]  Handle of the NAND FTL module instance
 * \param  srcPhyBlk  [OUT] Physical block number moved to.
 *
 * \return   IOM_COMPLETED  - On success
 */
static Int32 nandFtlMoveBlock(DDC_nandFtlObj *ftlObj, Uint32 *srcPhyBlk)
{
    Int32       result;    /* Result of moving the block                     */
    Uint32      dstPhyBlk; /* Destination physical block number              */
    Uint8       numPages;  /* Number of pages to be copied during block move */
    Bool        flag = TRUE;
    do
    {
        /* Validate the input parameters */
        assert((NULL != ftlObj) && (NULL != srcPhyBlk));
        flag = FALSE;

        do
        {
            /* Allocate a new block                                           */
            result = nandWlaFindFreeBlock(
                         (DDC_nandWlaData*)&ftlObj->wlaData,
                         &dstPhyBlk);
            if (result != IOM_COMPLETED)
            {
                /* There are no free physical blocks available                */
                break;
            }

            /* Find out how many pages from source block have to be moved.    *
             * This function will return the next free page in source block.  *
             * That information can be used to know how many pages are already*
             * available in the source physical block                         */
            result = nandFtlCacheGetPhyFreePage(
                         (DDC_nandCacheObj*)&ftlObj->cacheObj,
                         *srcPhyBlk,
                         (Uint8*)&numPages);
            if (result != IOM_COMPLETED)
            {
                /* The number of pages to be copied could not be determined   */
                break;
            }

            /* If there are any pages in the source block to copy, then copy  *
             * those pages to the destination block                           */
            if (numPages != 0u)
            {
                /* Perform a block copy from source to destination block      */
                result = nandFtlCopyBlock(ftlObj,
                             *srcPhyBlk,
                             dstPhyBlk,
                             numPages);
                if (PSP_NAND_E_WRITE_FAIL == result)
                {
                    /* Write to the destination block has failed. So mark     *
                     * the destination block as bad.                          */
                    ftlObj->llcFxns->markBadBlockFxn(
                        ftlObj->hLlcObj,
                        dstPhyBlk);

                   /* The only error that could occur are invalid-arguments
                      and command timeout - we are not going to handle these
                      for now. */

                    /* Block copy has failed. So go back again to get a new   *
                     * physical block and try again with the same process     */
                }
            }
        }while (result != IOM_COMPLETED);

        /* Check if block copy is complete                                    */
        if (result != IOM_COMPLETED)
        {
            /* Block copy could not be completed                              */
            break;
        }

        /* Block copy is complete. Update FTL statistics.                     */

        /* Update the physical block number if the cache                      */
        nandFtlCacheLineSetPhyBlock((DDC_nandCacheObj*)&ftlObj->cacheObj,
            *srcPhyBlk,
            dstPhyBlk);

        /* Mark the source block as bad                                       */
        ftlObj->llcFxns->markBadBlockFxn(ftlObj->hLlcObj, *srcPhyBlk);

        /* WLA need not be updated since srcBlk is already in used state      */
        /* The only error that could occur are invalid-arguments and command
          timeout - we are not going to handle these for now. */

        /* The block has been moved to a new destination block. Set the       *
         * destination block number as the new block.                         */
        *srcPhyBlk = dstPhyBlk;
    }while (flag);

    /* Return the result of moving the block */
    return (result);
}

/**
 * \brief  Merge the pages of two physical blocks into one block
 *
 *         This function merges information in two seperate physical blocks into
 *         a single physical block. This functionality is required when a page
 *         which is already written has to be written again. If write fails
 *         during the merge operation, a the merge is done onto a new block.
 *
 * \param  ftlObj  [IN]  Handle of the NAND FTL module instance
 * \param  dstBlk  [OUT] Destination physical block number.
 * \param  srcBlk  [IN]  Source block number.
 *
 * \return   IOM_COMPLETED  - On success
 */
static Int32 nandMergeBlock(DDC_nandFtlObj  *ftlObj,
                            Uint32          *dstBlk,
                            Uint32          srcBlk)
{
    Int32       result;         /* Result of merging two physical blocks */
    Int32       mvResult;       /* Result of moving a block to new block */
    Uint32      pagesPerBlock;  /* Number of pages per block             */
    Uint8       pageNum;        /* Page number counter                   */
    Uint8       srcPage;        /* Source physical page number           */
    Uint8       dstPage;        /* Destination physical page number      */
    Bool        status;         /* Status of physical page availability  */
    Bool        flag = TRUE;
    /* Thumb rules followed when merging two physical blocks                  *
     *                                                                        *
     * 1. If a logical page is already available in the destination block,    *
     *    then that copy of the page is considered to be the latest.          *
     *                                                                        *
     * 2. If a logical page is not in the destination block, but available in *
     *    the source block, then that physical page is copied to the          *
     *    destination block.                                                  *
     *                                                                        *
     * 3. On a page write failure, a new physical block is allocated and a    *
     *    block copy of the destination block to the newly allocated physical *
     *    block is performed.                                                 */

    do
    {
        /* Initialize the local variables                                     */
        result = IOM_EBADARGS;
        flag = FALSE;

        /* Validate the input parameters                                      */
        assert(NULL != ftlObj);

        /* Initialize the local variables as required                         */
        pagesPerBlock = ftlObj->devInfo->pagesPerBlock;
        mvResult      = IOM_COMPLETED;

        /* Copy all the logical pages from the source block to the            *
         * destination block                                                  */
        for (pageNum = 0u; pageNum < pagesPerBlock; pageNum++)
        {
            /* Check if logical page "pageNum" is already written in the      *
             * destination block.                                             */
            result = nandFtlCacheIsLogPageFree(
                         (DDC_nandCacheObj*)&ftlObj->cacheObj,
                         *dstBlk,
                         pageNum,
                         &status,
                         (Uint8*)&dstPage);
            if (result != IOM_COMPLETED)
            {
                /* Status check of logical page in destination block failed   */
                break;
            }

            if (TRUE == status) /* Logical page in destination block is free  */
            {
                /* Check if the logical page "pageNum" is written in the      *
                 * source block                                               */
                result = nandFtlCacheIsLogPageFree(
                             (DDC_nandCacheObj*)&ftlObj->cacheObj,
                             srcBlk,
                             pageNum,
                             &status,
                             (Uint8*)&srcPage);
                if (result != IOM_COMPLETED)
                {
                    /* Status check of logical page in source block failed    */
                    break;
                }

                /* The logical page in the source block has to be copied to   *
                 * the destination block                                      */
                if (FALSE == status)
                {
                    /* Do until the physical page is copied                   */
                    do
                    {
                        /* This logical page has to be copied from the source *
                         * block to the destination block. So get a free      *
                         * physical page number in the destination block.     */
                        result = nandFtlCacheGetPhyFreePage(
                                     (DDC_nandCacheObj*)&ftlObj->cacheObj,
                                     *dstBlk,
                                     (Uint8*)&dstPage);
                        if (result != IOM_COMPLETED)
                        {
                            /* Could not get a free physical page number      */
                            break;
                        }

                        /* Copy this logical page from source block to the    *
                         * destination block                                  */
                        result = nandFtlCopyPage(ftlObj,
                                     srcBlk,
                                     srcPage,
                                     *dstBlk,
                                     dstPage);
                        /* Check if the page copy is complete. If there is a  *
                         * page write error, then the contents of the current *
                         * destination block has to be moved to a new         *
                         * destination block                                  */
                        if (PSP_NAND_E_WRITE_FAIL == result)
                        {
                            /* Page copy resulted in a write error. So move   *
                             * the destination block to a new block.          */
                            mvResult = nandFtlMoveBlock(ftlObj, dstBlk);
                            if (mvResult != IOM_COMPLETED)
                            {
                                /* Error in moving the destination block. so  *
                                 * the merge process cannot continue          */
                                result = mvResult;
                                break;
                            }
                        }
                    }
                    while (result != IOM_COMPLETED);

                    /* Check if the page has been copied                      */
                    if (result != IOM_COMPLETED)
                    {
                        /* The page copy has failed. So stop the block merge  *
                         * procedure.                                         */
                        break;
                    }

                    /* Map the logical page number to physical page number in *
                     * the FTL cache module.                                  */
                    result = nandFtlCacheMapLogPage(
                                 (DDC_nandCacheObj*)&ftlObj->cacheObj,
                                 *dstBlk,
                                 pageNum,
                                 dstPage);
                    if (result != IOM_COMPLETED)
                    {
                        /* Mapping of logical page to physical page number in *
                         * in the cache module failed.                        */
                        break;
                    }
                }
            }
        }

        /* Recycle the source block if the merge operation is complete        */
        if (IOM_COMPLETED == mvResult)
        {
            result = nandFtlRecycleBlock(ftlObj, srcBlk);
            if (result != IOM_COMPLETED)
            {
                /* Ignore any error in recycling the block. Well this could
                   happen if the erase failed and erase could fail for
                   command timeout, invalid args nothing much could be done now
                   */
                result = IOM_COMPLETED;
            }
        }
    }while (flag);

    /* Return the result of merging the blocks                                */
    return (result);
}

/**
 * \brief  Recycle a used block.
 *
 *         This function recycles a used block. It erases the physical block
 *         and updates the logical to physical block mapping and the WLA table.
 *
 * \param  ftlObj      [IN] Handle of the NAND FTL module instance
 * \param  eraseBlock  [IN] Physical block number to be erased.
 *
 * \return   IOM_COMPLETED  - On success
 */
static Int32 nandFtlRecycleBlock(DDC_nandFtlObj *ftlObj, Uint32 eraseBlock)
{
    Int32   result;   /* Result of recycling a physical block */
    Ptr     hLlcObj;  /* Handle of the LLC module layer       */
    Bool    flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert(NULL != ftlObj);
        assert(NULL != ftlObj->hLlcObj);
        /* Initialize the local variables as reqired                          */
        flag = FALSE;
        hLlcObj = ftlObj->hLlcObj;
        assert(NULL != hLlcObj);

        /* Erase the specified block                                          */
        result = ftlObj->llcFxns->eraseBlockFxn(hLlcObj, eraseBlock);
        if (IOM_COMPLETED == result)
        {
            /* Erase is complete. So this block is available for reuse. So    *
             * mark this block as available in the WLA table.                 */
            nandWlaUpdateTable((DDC_nandWlaData*)&ftlObj->wlaData,
                eraseBlock,
                FALSE);
        }
        else  /* Block erase has failed */
        {
            /* Check if erase failed because NAND was write protected. If so  *
             * this is critical error.                                        */
            if (PSP_NAND_E_ERASE_FAIL == result)
            {
                /* Block erase has failed. Mark the block as bad.             */
                ftlObj->llcFxns->markBadBlockFxn(ftlObj->hLlcObj, eraseBlock);
                /* The only error that could occur are invalid-arguments
                   and command timeout - we are not going to handle these
                   for now. */
                /* Update the WLA table as used block since this block cannot *
                 * be used again                                              */
                nandWlaUpdateTable((DDC_nandWlaData*)&ftlObj->wlaData,
                    eraseBlock,
                    TRUE);
            }
           else
           {
               /* Erase failed - let the WLA NOT be updated and chache lines
                  remain in-tact.
                  Let the caller know that we could not erase                 */
               break;
           }
        }

        /* Invalidate the cache if this physical block is cached in any of the*
         * cache lines                                                        */
        nandFtlCacheInvalidate((DDC_nandCacheObj*)&ftlObj->cacheObj,
            eraseBlock);
    }while (flag);

    /* Return the result of erasing the physical block                        */
    return (result);
}
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
