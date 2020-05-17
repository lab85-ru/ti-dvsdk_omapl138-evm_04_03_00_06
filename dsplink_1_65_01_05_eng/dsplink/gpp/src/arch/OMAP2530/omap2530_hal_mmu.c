/** ============================================================================
 *  @file   omap2530_hal_mmu.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/OMAP2530/
 *
 *  @desc   Memory management module.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation version 2.
 *  
 *  This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 *  whether express or implied; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *  ============================================================================
 */


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>
#include <linkcfgdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <omap2530_hal.h>
#include <omap2530_hal_mmu.h>
#include <omap2530_phy_shmem.h>

#if defined (__cplusplus)
extern "C" {
#endif

/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_HAL_MMU


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_ARCH_HAL_MMU, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/*  ============================================================================
 *  @const  MMU_SYSCONFIG_OFFSET,
 *          MMU_SYSSTATUS_OFFSET,
 *          MMU_IRQSTATUS_OFFSET,
 *          MMU_CNTL_OFFSET,
 *          MMU_FAULT_AD_OFFSET,
 *          MMU_LOCK_OFFSET,
 *          MMU_LD_TBL_OFFSET,
 *          MMU_CAM_OFFSET,
 *          MMU_RAM_OFFSET
 *          MMU_FLUSH_ENTRY
 *
 *  @desc   Offsets from the MMU base address
 *  ============================================================================
 */
#define MMU_SYSCONFIG_OFFSET      0x10
#define MMU_SYSSTATUS_OFFSET      0x14
#define MMU_IRQSTATUS_OFFSET      0x18
#define MMU_IRQENABLE_OFFSET      0x1C
#define MMU_CNTL_OFFSET           0x44
#define MMU_FAULT_AD_OFFSET       0x48
#define MMU_LOCK_OFFSET           0x50
#define MMU_LD_TBL_OFFSET         0x54
#define MMU_CAM_OFFSET            0x58
#define MMU_RAM_OFFSET            0x5C
#define MMU_FLUSH_ENTRY           0x64

/*  ============================================================================
 *  @const  MMU_CAM_PRESERVE,
 *          MMU_CAM_VALID
 *
 *  @desc   CAM register field values
 *  ============================================================================
 */
#define MMU_CAM_PRESERVE          (1 << 3)
#define MMU_CAM_VALID             (1 << 2)

/*  ============================================================================
 *  @const  MMU_GLOBAL_MEMORY
 *
 *  @desc   Address lower than this do not go through the DSP's MMU
 *  ============================================================================
 */
#define MMU_GLOBAL_MEMORY         0x11000000

/*  ============================================================================
 *  @const  MMU_IRQENABLE_FAULTS
 *
 *  @desc   Faults to enable (MULTIHITFAULT and TLBMISS)
 *  ============================================================================
 */
#define MMU_IRQENABLE_FAULTS          0x11

/*  ============================================================================
 *  @const  MMU_FAULT_INTERRUPT
 *
 *  @desc   Interrupt Id for DSP MMU faults
 *  ============================================================================
 */
#define MMU_FAULT_INTERRUPT          28

/*  ============================================================================
 *  @const  MMU_NUM_TLB_ENTRIES
 *
 *  @desc   Number of TLB entries on the DSP's MMU
 *  ============================================================================
 */
#define MMU_NUM_TLB_ENTRIES       32

/*  ============================================================================
 *  @const  SIZE_4KB,
 *          SIZE_64KB,
 *          SIZE_1MB,
 *          SIZE_16MB
 *
 *  @desc   Size constants
 *  ============================================================================
 */
#define SIZE_4KB                 0x1000
#define SIZE_64KB               0x10000
#define SIZE_1MB               0x100000
#define SIZE_16MB             0x1000000

/*  ============================================================================
 *  @const  NUM_SIZE_ENTRIES_TLB
 *
 *  @desc   size of the entriesCount
 *  ============================================================================
 */
#define  NUM_SIZE_ENTRIES_TLB    4

/*  ============================================================================
 *  @const  OMAP2530_HalMmuEntriesCount,
 *
 *  @desc   TLB entry counter in MMU.Global
 *  ============================================================================
 */
STATIC Uint32 OMAP2530_HalMmuEntriesCount [4] ;

/** ----------------------------------------------------------------------------
 *  @name   OMAP2530_HalMmuEventList
 *
 *  @desc   List of Event.
 *  ----------------------------------------------------------------------------
 */
STATIC List * OMAP2530_HalMmuEventList ;

/** ============================================================================
 *  @name  OMAP2530_HalMmuEntryInfo
 *
 *  @desc  Maintain all entries list.
 *
 *  ============================================================================
 */
STATIC MMU_EntryInfo * OMAP2530_HalMmuEntryInfo ;

/** ============================================================================
 *  @name  OMAP3530_HalMmuIrqStatus, OMAP3530_HalMmuFaultAddr
 *
 *  @desc  Information about MMU fault that has occurred.
 *  ============================================================================
 */
STATIC Uint32 OMAP2530_HalMmuIrqStatus = 0u ;
STATIC Uint32 OMAP2530_HalMmuFaultAddr = 0u ;

/** ============================================================================
 *  @func   OMAP2530_halMmuISR
 *
 *  @desc   Used to print out if there is an MMU fault on the DSP.
 *
 *  @modif  All MMU faults are disabled after call.
 *..============================================================================
 */
STATIC
NORMAL_API
DSP_STATUS
OMAP2530_halMmuISR (IN Pvoid refData)
{
    DSP_STATUS status    = DSP_SOK ;

    TRC_0ENTER ("OMAP2530_halMmuISR") ;

    DBC_Require (refData != NULL) ;

    PRINT_Printf ("DSP MMU Error Fault! "
                  " MMU_IRQSTATUS = [0x%x]. Virtual DSP addr reference"
                  " that generated the interrupt = [0x%x].\n",
                  OMAP2530_HalMmuIrqStatus,
                  OMAP2530_HalMmuFaultAddr) ;

    OMAP2530_HalMmuIrqStatus = 0x0u ;
    OMAP2530_HalMmuFaultAddr = 0x0u ;

    TRC_1LEAVE ("OMAP2530_halMmuISR", status) ;

    return status ;
}


/** ============================================================================
 *  @func   OMAP2530_halMmuCheckIntGen
 *
 *  @desc   This function implements check routine to check the interrupt
 *          generation. In case of shared interrupts, other devices can also
 *          generate same ints.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
Bool
OMAP2530_halMmuCheckIntGen (IN Pvoid refData)
{
    Bool              isAsserted = TRUE ;
    OMAP2530_HalObj * halObject  = NULL ;

    DBC_Require (refData != NULL) ;

    TRC_1ENTER ("OMAP2530_halMmuCheckIntGen", refData) ;

    halObject = (OMAP2530_HalObj *) refData ;

    /* Store the values for displaying later */
    OMAP2530_HalMmuIrqStatus = REG (halObject->mmuBase + MMU_IRQSTATUS_OFFSET) ;
    OMAP2530_HalMmuFaultAddr = REG (halObject->mmuBase + MMU_FAULT_AD_OFFSET) ;

    /* De-assert the interrupt */
    REG (halObject->mmuBase + MMU_IRQSTATUS_OFFSET) = OMAP2530_HalMmuIrqStatus ;

    /* Disable anymore from being generated. */
    REG (halObject->mmuBase + MMU_IRQENABLE_OFFSET) = 0u ;

    TRC_1LEAVE ("OMAP2530_halMmuCheckIntGen", isAsserted) ;

    return isAsserted ;
}


/** ============================================================================
 *  @func   OMAP2530_halSearchMmuEntry
 *
 *  @desc   This function matches an event listener with a list element.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Bool
OMAP2530_halSearchMmuEntry (IN ListElement * elem, Pvoid data)
{
    MMU_EntryInfo       * info       = NULL  ;
    MMU_EntryInfo       * addInfo    = NULL  ;
    Bool                  retVal     = FALSE ;

    TRC_2ENTER ("OMAP2530_halSearchMmuEntry", elem, data) ;

    DBC_Require (elem != NULL) ;

    info   = (MMU_EntryInfo *) elem ;
    addInfo = (MMU_EntryInfo *) data ;

    /* Check for requested entry already exist in TLB */
    if (   (info->processAddr   == addInfo->processAddr)
        && (info->processSize   == addInfo->processSize)) {
        PRINT_Printf ("Entry exists for address: [0x%x] size: [0x%x]\n",
                      addInfo->processAddr,
                      addInfo->processSize) ;
        retVal = TRUE ;
    }
    /* Any existing entry's start address is covered by given entry's start */
    /* address and end address.                                             */
    /*(Given Start Address <= Entry's start address <= Given End address)   */
    else if (   (addInfo->processAddr <= info->processAddr )
             && (info->processAddr
             <= (addInfo->processAddr + addInfo->processSize - 1))) {
        PRINT_Printf ("Existing entry's end address is covered by given"
                      " entry's start & end address.,"
                      " can not create TLB entry for address:"
                      " [0x%x] size: [0x%x]\n",
                      addInfo->processAddr,
                      addInfo->processSize) ;
        retVal = TRUE ;
    }

    /* Any existing entry's end address is covered by given entry's start   */
    /* address and end address.                                             */
    /* (Given Start Address <= Entry's end address <= Given End address)    */
    else if (   (addInfo->processAddr
             <= (info->processAddr + info->processSize -1))
             && ((info->processAddr + info->processSize - 1)
             <= (addInfo->processAddr + addInfo->processSize -1))) {
        PRINT_Printf ("Existing entry's Start address is covered by given"
                      " entry's start & end address.,"
                      " can not create TLB entry for address:"
                      " [0x%x] size: [0x%x]\n",
                      addInfo->processAddr,
                      addInfo->processSize) ;
        retVal = TRUE ;
    }
    /* Given entry's start address is covered by any existing entry's start */
    /* address and end address.                                             */
    /*(Entry's Start Address <= Given start address <= Entry End address)   */
    else if (   (info->processAddr <= addInfo->processAddr)
             && (addInfo->processAddr
             <= (info->processAddr + info->processSize - 1))) {
        PRINT_Printf ("Start address is covered by existing entry,"
                      " can not create TLB entry for address:"
                      " [0x%x] size: [0x%x]\n",
                      addInfo->processAddr,
                      addInfo->processSize) ;
        retVal = TRUE ;
    }

    /* Given entry's end address is covered by any existing entry's start */
    /* address and end address.                                           */
    /* (Entry's Start Address <= given end address <= Entry End address)  */
    else if (   ((info->processAddr)
             <= (addInfo->processAddr + addInfo->processSize - 1))
             && ((addInfo->processAddr + addInfo->processSize - 1)
                     <= (info->processAddr + info->processSize - 1))) {
        PRINT_Printf ("End address is covered by existing entry,"
                      " can not create TLB entry for address:"
                      " [0x%x] size: [0x%x]\n",
                      addInfo->processAddr,
                      addInfo->processSize) ;
        retVal = TRUE ;
    }

    TRC_1LEAVE ("OMAP2530_halSearchMmuEntry", retVal) ;

    return retVal ;
}

/** ============================================================================
 *  @func   OMAP2530_halMmuAddEntry
 *
 *  @desc   Maintain a list of entries.
 *
 *  @modif  None.
 *  ============================================================================
*/
NORMAL_API
DSP_STATUS
OMAP2530_halMmuAddEntry (IN Uint32 tlbIndex,
                 IN Bool   type,
                 IN Uint32 dspVirtAddr,
                 IN Uint32 actualSize,
                 IN Uint32 processAddr,
                 IN Uint32 processSize)
{
    DSP_STATUS             status     = DSP_SOK ;
    MMU_EntryInfo        * info                 ;
    MMU_EntryInfo          addInfo              ;

    TRC_0ENTER ("OMAP2530_halMmuAddEntry") ;

    addInfo.dspVirtAddr = dspVirtAddr ;
    addInfo.actualSize = actualSize ;
    addInfo.processAddr = processAddr ;
    addInfo.processSize = processSize ;
    addInfo.type = type ;

    if (OMAP2530_HalMmuEventList == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    /* Search for the entry in the list */
    status = LIST_SearchElement (
               (List *) OMAP2530_HalMmuEventList,
               (Pvoid) &addInfo,
               (ListElement **) &info,
               (ListMatchFunc) (&OMAP2530_halSearchMmuEntry)) ;

    if ((status == DSP_ENOTFOUND) || (status == DSP_EINVALIDARG)) {
        status = MEM_Alloc ((Void **) &OMAP2530_HalMmuEntryInfo,
                            sizeof (MMU_EntryInfo),
                            MEM_DEFAULT) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            status = LIST_InitializeElement (
                                       &(OMAP2530_HalMmuEntryInfo->element)) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;

            if (DSP_SUCCEEDED (status)) {
                OMAP2530_HalMmuEntryInfo->dspVirtAddr = dspVirtAddr ;
                OMAP2530_HalMmuEntryInfo->actualSize = actualSize ;
                OMAP2530_HalMmuEntryInfo->processAddr = processAddr ;
                OMAP2530_HalMmuEntryInfo->processSize = processSize ;
                OMAP2530_HalMmuEntryInfo->tlbEntry = tlbIndex ;
                OMAP2530_HalMmuEntryInfo->type = type ;

                status = LIST_PutTail (OMAP2530_HalMmuEventList,
                                       &(OMAP2530_HalMmuEntryInfo->element)) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
        }
    }
    else {
        status = DSP_EALREADYEXISTS ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("OMAP2530_halMmuAddEntry", status) ;

    return status ;
}

/** ============================================================================
 *  @func   OMAP2530_halMmuRegion
 *
 *  @desc   This function configures the specified addr to be in the TLB
 *
 *  @modif  tlbIndex gets incremented.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
OMAP2530_halMmuRegion(IN OMAP2530_HalObj * halObject,
                    IN Uint32      dspVirtAddr,
                    IN Uint32      size,
                    IN Bool         type
                     )
{
    DSP_STATUS status            = DSP_SOK ;
    Uint32     sizeMask          = 0x0     ;
    Uint32     processSize       = 0x0     ;
    Uint32     actualSize        = 0x0     ;
    Uint32     processAddr       = 0x0     ;

    TRC_0ENTER ("OMAP2530_halMmuRegion") ;

    processAddr = dspVirtAddr ;
    actualSize = size ;

    /* Config checking */
    if ((size & (SIZE_4KB - 1)) != 0) {
        /* Not a multiple of 4KB */
        PRINT_Printf ("Configuration error:"
                      " Combined memory entries (starting with ADDRDSPVIRT"
                      " = 0x%x) does not have a size that is a multiple"
                      " of 4KB. Size is %d.\n",
                      dspVirtAddr,
                      size) ;
        status = DSP_ECONFIG ;
        SET_FAILURE_REASON ;
    }
    else if (halObject->tlbIndex >= (MMU_NUM_TLB_ENTRIES - 1)) {
        /* Not enough TLB entries left */
        PRINT_Printf ("Configuration error (A):"
                      " Exceeded maximum number [31] of Translation "
                      " Look-aside Buffers. ") ;
        if (   (OMAP2530_HalMmuEntriesCount [0] != 0)
            || (OMAP2530_HalMmuEntriesCount [1] != 0)
            || (OMAP2530_HalMmuEntriesCount [2] != 0)
            || (OMAP2530_HalMmuEntriesCount [3] != 0)) {
                PRINT_Printf ("The configured entries are:\n"
                              "    Number of entries of size 4KB:  %d\n"
                              "    Number of buffers of size 64KB: %d\n"
                              "    Number of buffers of size 1MB:  %d\n"
                              "    Number of buffers of size 16MB: %d\n",
                              OMAP2530_HalMmuEntriesCount [3],
                              OMAP2530_HalMmuEntriesCount [2],
                              OMAP2530_HalMmuEntriesCount [1],
                              OMAP2530_HalMmuEntriesCount [0]) ;
        }
        status = DSP_ECONFIG ;
        SET_FAILURE_REASON ;
    }

    /* Add the entry (or entries) */
    while ((size != 0) && DSP_SUCCEEDED (status)) {

        /* Determine the size of the entry */
        if (size >= SIZE_16MB && !(processAddr & (SIZE_16MB-1))) {
            sizeMask = 0x3 ;
            processSize = SIZE_16MB ;
            OMAP2530_HalMmuEntriesCount [0] ++ ;
        }
        else if (size >= SIZE_1MB && !(processAddr & (SIZE_1MB-1))) {
            sizeMask = 0x0 ;
            processSize = SIZE_1MB ;
            OMAP2530_HalMmuEntriesCount [1] ++ ;
        }
        else if (size >= SIZE_64KB && !(processAddr & (SIZE_64KB-1))) {
            sizeMask = 0x1 ;
            processSize = SIZE_64KB ;
            OMAP2530_HalMmuEntriesCount [2] ++ ;
        }
        else  if (!(processAddr & (SIZE_4KB-1))) {
            sizeMask = 0x2 ;
            processSize = SIZE_4KB ;
            OMAP2530_HalMmuEntriesCount [3] ++ ;
        }
        else {
            PRINT_Printf ("Configuration error: "
                          " MMU entries must be aligned to their page size(4KB,"
                          " 64KB, 1MB, or 16MB).\n");
            PRINT_Printf ("Since the addresses are not aligned the buffer of"
                          " size: %x at address: %x cannot be configured into"
                          " TLB entries\n",
                          size, processAddr) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
            break ;
        }
    /* Check and maintain the requested dynamic entries */
        if(DSP_SUCCEEDED (status)) {
            status = OMAP2530_halMmuAddEntry (halObject->tlbIndex,
                                           type,
                                           dspVirtAddr,
                                           actualSize,
                                           processAddr,
                                           processSize) ;

            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            /* Specify virtual address */
            REG (halObject->mmuBase + MMU_CAM_OFFSET) = (
                                          processAddr & 0xFFFFF000) |
                                          MMU_CAM_PRESERVE    |
                                          MMU_CAM_VALID       |
                                          sizeMask ;

            /* Specify physical address and element fields */
            REG (halObject->mmuBase + MMU_RAM_OFFSET) = (
                                          processAddr & 0xFFFFF000) |
                                          0x140;

            /* Specify the which TLB entry  */
            REG (halObject->mmuBase + MMU_LOCK_OFFSET) =
                                          (((halObject->tlbIndex) + 1) << 10) |
                                          ((halObject->tlbIndex)       << 4) ;

            /* Put the entry into the TLB */
            SET_BIT (REG (halObject->mmuBase + MMU_LD_TBL_OFFSET), 0) ;

            halObject->tlbIndex++ ;
            size -= processSize ;
            processAddr += processSize ;

            /* Ensure the halObject->tlbIndex does not exceed the number of
             * lockable entries
             */
            if (halObject->tlbIndex == MMU_NUM_TLB_ENTRIES) {
                PRINT_Printf ("Configuration error (B):"
                              " Exceeded maximum number [31] of Translation "
                              " Look-aside Buffers.\n") ;
                if (   (OMAP2530_HalMmuEntriesCount [0] != 0)
                    || (OMAP2530_HalMmuEntriesCount [1] != 0)
                    || (OMAP2530_HalMmuEntriesCount [2] != 0)
                    || (OMAP2530_HalMmuEntriesCount [3] != 0)) {
                    PRINT_Printf ("The configured entries are:\n"
                                  "    Number of entries of size 4KB:  %d\n"
                                  "    Number of buffers of size 64KB: %d\n"
                                  "    Number of buffers of size 1MB:  %d\n"
                                  "    Number of buffers of size 16MB: %d\n",
                                  OMAP2530_HalMmuEntriesCount [3],
                                  OMAP2530_HalMmuEntriesCount [2],
                                  OMAP2530_HalMmuEntriesCount [1],
                                  OMAP2530_HalMmuEntriesCount [0]) ;
                }
                status = DSP_ECONFIG ;
                SET_FAILURE_REASON ;
            }
        }
    }
    TRC_1LEAVE ("OMAP2530_halMmuRegion", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   OMAP2530_halCheckMmuEntry
 *
 *  @desc   This function matches an event listener with a list element.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Bool
OMAP2530_halCheckMmuEntry (IN ListElement * elem, Pvoid data)
{
    Bool            retVal        = FALSE   ;
    MMU_EntryInfo * info          = NULL    ;
    MMU_EntryInfo * delInfo       = NULL    ;

    TRC_2ENTER ("OMAP2530_halCheckMmuEntry", elem, data) ;

    DBC_Require (elem != NULL) ;

    info   = (MMU_EntryInfo *) elem ;
    delInfo = (MMU_EntryInfo *) data ;

    /* Check for requested entry exist in TLB */
    if (    (info->processAddr   == delInfo->processAddr)
        &&  (info->processSize   == delInfo->processSize)
        &&  (info->type          == delInfo->type       )) {
        /* Entry found */
        retVal = TRUE ;
    }
    else {
        /* Entry not exists */
        retVal = FALSE ;
    }
    TRC_1LEAVE ("OMAP2530_halCheckMmuEntry", retVal) ;

    return retVal ;
}


/** ============================================================================
 *  @func   OMAP2530_halMmuEnable
 *
 *  @desc   Enables the MMU for GEM Module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP2530_halMmuEnable (IN  Pvoid              halObj,
                       IN  LINKCFG_MemEntry * memTable,
                       IN  Uint32             memEntries)
{
    Uint32              i,j                       ;
    Bool                type                      ;
    DSP_STATUS          status     = DSP_SOK      ;
    DSP_STATUS          freeStatus = DSP_SOK      ;
    Uint32              sortedCnt  = 0            ;
    OMAP2530_HalObj   * halObject  = NULL         ;
    Uint32              tempSize, tempdspVirtAddr ;
    InterruptObject     intObj                    ;
    LINKCFG_MemEntry  * sorted                    ;

    TRC_3ENTER ("OMAP2530_halMmuEnable", halObj,memTable,memEntries) ;

    DBC_Require (halObj != NULL) ;

    halObject = (OMAP2530_HalObj *) halObj ;

    halObject->isrObject = NULL ;
    halObject->tlbIndex = 0 ;
    /* type is used to differentiate between dynamic and static entry */
    /* type is TRUE for dynamic */
    type = FALSE ;

    status = MEM_Alloc ((Void **) &sorted,
                        sizeof (LINKCFG_MemEntry) * memEntries,
                        MEM_DEFAULT) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }
    else {

        /* Initializing global entries  count */
        for (i = 0 ; i < NUM_SIZE_ENTRIES_TLB; i++) {
            OMAP2530_HalMmuEntriesCount[i] = 0 ;
        }

        /*  Make a copy of the memTable.  This copy will be sorted and merged.
         *  Only process regions above the global memory space. Regions
         *  below this do not go through the MMU.
         */
        for (i = 0 ; i < memEntries; i++) {

            if (memTable[i].dspVirtAddr >= MMU_GLOBAL_MEMORY) {
                sorted[sortedCnt].size        = memTable[i].size ;
                sorted[sortedCnt].dspVirtAddr = memTable[i].dspVirtAddr ;
                sortedCnt++ ;
            }
        }

        /* Sort the addresses into ascending order */
        for (i = 0 ; i < sortedCnt - 1; i++) {
            for (j = i + 1 ; j < sortedCnt; j++) {
                if (sorted[j].dspVirtAddr < sorted[i].dspVirtAddr) {
                    tempSize        = sorted[i].size ;
                    tempdspVirtAddr = sorted[i].dspVirtAddr ;

                    sorted[i].size        = sorted[j].size ;
                    sorted[i].dspVirtAddr = sorted[j].dspVirtAddr ;

                    sorted[j].size        = tempSize ;
                    sorted[j].dspVirtAddr = tempdspVirtAddr ;
                }
            }
        }

        /* Combine adjacent regions */
        for (i = 0 ; i < sortedCnt - 1; i++) {
           /* Ignore if the size is 0 */
            if (sorted[i].size != 0) {
                for (j = i + 1 ; j < sortedCnt; j++) {
                    if (sorted[i].dspVirtAddr + sorted[i].size ==
                        sorted[j].dspVirtAddr) {

                        /* Increase the size accordingly */
                        sorted[i].size += sorted[j].size ;

                        /* Mark this one as un-used by setting size = 0 */
                        sorted[j].size = 0 ;
                    }
                }
            }
        }


        /* Perform software reset of the MMU module, set bit 1 */
        SET_BIT (REG (halObject->mmuBase + MMU_SYSCONFIG_OFFSET), 1) ;

        /* Wait for reset complete. If bit 0 == 0, it's still resetting */
        while( (REG (halObject->mmuBase + MMU_SYSSTATUS_OFFSET) & 1 ) == 0 )
        {
            /* Wait for reset to complete */
        }

        /* Enable autoidle, set bit 0 */
        SET_BIT (REG (halObject->mmuBase + MMU_SYSCONFIG_OFFSET), 0) ;

        /* Creating a list for maintain the entries*/
        status = LIST_Create (&OMAP2530_HalMmuEventList) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        for (i = 0 ; i < sortedCnt && DSP_SUCCEEDED (status); i++) {
            /* Configure the TLB */
            if (sorted[i].size != 0) {
                status = OMAP2530_halMmuRegion(halObject,
                                             sorted[i].dspVirtAddr,
                                             sorted[i].size,
                                             type) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
        }

        if (DSP_SUCCEEDED (status)) {
            /* Enable DSP MMU faults to the GPP */
            REG (halObject->mmuBase + MMU_IRQENABLE_OFFSET) =
                                                         MMU_IRQENABLE_FAULTS ;

            /* Enable the MMU */
            SET_BIT (REG (halObject->mmuBase + MMU_CNTL_OFFSET), 1) ;
        }

        /* Free the temporary block of memory.
         * Do not return this status, but log the it if there is a failure.
         */
        freeStatus = MEM_Free ((Pvoid *)&sorted, MEM_DEFAULT) ;
        if (DSP_FAILED (freeStatus)) {
            SET_FAILURE_REASON ;
        }
    }

    /* If successful so far, plug ISR to handle MMU faults */
    if (DSP_SUCCEEDED (status)) {
        intObj.dspId = halObject->procId ;
        intObj.intId = MMU_FAULT_INTERRUPT ;
        /* Set shared to false as this interrrupt is not shared */
        intObj.shared = FALSE ;
        intObj.checkFunc = OMAP2530_halMmuCheckIntGen ;
        intObj.param     = halObject ;
        status = ISR_Create (OMAP2530_halMmuISR,
                             (Pvoid) halObject,
                             &intObj,
                             &(halObject->isrObject)) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            status = ISR_Install (NULL, halObject->isrObject) ;
            if (DSP_SUCCEEDED (status)) {
                /* DSP_intCtrl for DSP_IntCtrlCmd_Enable does nothing */
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("OMAP2530_halMmuEnable", status) ;

    return status ;
}


/** ============================================================================
 *  @func   OMAP2530_halMmuDisable
 *
 *  @desc   Disables the MMU for GEM Module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP2530_halMmuDisable (IN         Pvoid          halObj)
{
    DSP_STATUS         status    = DSP_SOK   ;
    DSP_STATUS         tmpStatus = DSP_SOK   ;
    OMAP2530_HalObj  * halObject = NULL      ;
    MMU_EntryInfo    * info                  ;
    MMU_EntryInfo    * nextInfo              ;

    TRC_1ENTER ("OMAP2530_halMmuDisable", halObj) ;

    DBC_Require (halObj != NULL) ;

    halObject = (OMAP2530_HalObj *) halObj ;

    /* Disable the MMU */
    CLEAR_BIT (REG (halObject->mmuBase + MMU_CNTL_OFFSET), 1) ;

    if (DSP_SUCCEEDED (tmpStatus)) {

        /* Make sure there is an object */
        if (halObject->isrObject != NULL) {
            tmpStatus = ISR_Uninstall (halObject->isrObject) ;

            tmpStatus = ISR_Delete (halObject->isrObject) ;

            halObject->isrObject = NULL ;
        }
    }
    if ((OMAP2530_HalMmuEventList!= NULL) && (DSP_SUCCEEDED (tmpStatus))) {
        status = LIST_First (OMAP2530_HalMmuEventList,
                                     (ListElement **) &info) ;
        while ((info != NULL) && (DSP_SUCCEEDED (status))) {
            if (DSP_SUCCEEDED (status)) {
                status = LIST_Next (OMAP2530_HalMmuEventList,
                                       (ListElement *)  info,
                                       (ListElement **) &nextInfo) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                status = LIST_RemoveElement (OMAP2530_HalMmuEventList,
                                              &(info->element)) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                else {
                    tmpStatus = FREE_PTR (info) ;
                    if (DSP_FAILED (tmpStatus)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
                info = nextInfo ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
        if (DSP_SUCCEEDED (status)) {
            status = LIST_Delete (OMAP2530_HalMmuEventList) ;;
            if (DSP_FAILED(status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("OMAP2530_halMmuDisable", status) ;

    return status ;
}


/** ============================================================================
 *  @func   OMAP2530_halMmuDeleteEntry
 *
 *  @desc   Delete entry from TLB.
 *
 *  @arg    mmuInfo
 *              Info of all entries in TLB.
 *  ============================================================================
*/
EXPORT_API
DSP_STATUS
OMAP2530_halMmuDeleteEntry(IN OMAP2530_HalObj * halObject,
                           IN ProcMemMapInfo *  mmuInfo,
                           IN Bool              type)
{
    Uint32     sizeMask          = 0x0     ;
    Uint32     processSize       = 0x0     ;
    DSP_STATUS status            = DSP_SOK ;
    DSP_STATUS tmpStatus         = DSP_SOK ;
    MMU_EntryInfo                  delInfo ;
    MMU_EntryInfo *                info    ;

    TRC_3ENTER ("OMAP2530_halMmuDeleteEntry", halObject, mmuInfo, type) ;

    while ((mmuInfo->size != 0) && DSP_SUCCEEDED (status)) {
        /* Determine the size of the entry */
        if (mmuInfo->size >= SIZE_16MB && !(mmuInfo->dspAddr &
           (SIZE_16MB-1))) {
            sizeMask = 0x3 ;
            processSize = SIZE_16MB ;
            OMAP2530_HalMmuEntriesCount [0] -- ;
        }
        else if (mmuInfo->size >= SIZE_1MB && !(mmuInfo->dspAddr &
                (SIZE_1MB-1))) {
            sizeMask = 0x0 ;
            processSize = SIZE_1MB ;
            OMAP2530_HalMmuEntriesCount [1] -- ;
        }
        else if (mmuInfo->size >= SIZE_64KB && !(mmuInfo->dspAddr &
                (SIZE_64KB-1))) {
            sizeMask = 0x1 ;
            processSize = SIZE_64KB ;
            OMAP2530_HalMmuEntriesCount [2] -- ;
        }
        else  if (!(mmuInfo->dspAddr & (SIZE_4KB-1))) {
            sizeMask = 0x2 ;
            processSize = SIZE_4KB ;
            OMAP2530_HalMmuEntriesCount [3] -- ;
        }
        else {
            PRINT_Printf ("Configuration error: "
                          "MMU entries must be aligned to their page size (4KB,"
                          " 64KB, 1MB, or 16MB).\n");
            PRINT_Printf ("Since the addresses are not aligned the buffer of"
                          " size: %x at address: %x cannot be configured into"
                          " TLB entries\n",
                          mmuInfo->size, mmuInfo->dspAddr) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
            break ;
        }

        if (OMAP2530_HalMmuEventList!= NULL) {
            delInfo.processAddr = mmuInfo->dspAddr ;
            delInfo.processSize = processSize ;
            delInfo.type = type ;
            /* Search for the entry in the list */
            status = LIST_SearchElement (
                       (List *) OMAP2530_HalMmuEventList,
                       (Pvoid) &delInfo,
                       (ListElement **) &info,
                       (ListMatchFunc) (&OMAP2530_halCheckMmuEntry)) ;
        }
        if (DSP_SUCCEEDED (status)) {
            /* Specifying its virtual address in the MMUn.MMU_CAM register */
            REG (halObject->mmuBase + MMU_CAM_OFFSET) = (mmuInfo->dspAddr &
                 0xFFFFF000) | sizeMask ;

            /* Flush the entries from TLB */
            SET_BIT (REG (halObject->mmuBase + MMU_FLUSH_ENTRY), 0) ;

            /*Decrement the mmu entry count*/
            halObject->tlbIndex-- ;

            if (DSP_SUCCEEDED (status)) {
                mmuInfo->size = mmuInfo->size - processSize ;
                mmuInfo->dspAddr = mmuInfo->dspAddr + processSize ;
            }

            tmpStatus = LIST_RemoveElement (OMAP2530_HalMmuEventList,
                            &(info->element)) ;
            DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;

            tmpStatus = FREE_PTR (info) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("OMAP2530_halMmuDeleteEntry", status) ;

    return status ;

}


/** ============================================================================
 *  @func   OMAP2530_halMmuCtrl
 *
 *  @desc   MMU conrtoller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP2530_halMmuCtrl (IN         Pvoid               halObj,
                     IN         DSP_MmuCtrlCmd      cmd,
                     IN         LINKCFG_MemEntry *  memTable,
                     IN         Uint32              memEntries,
                     IN         ProcMemMapInfo *    mmuInfo)
{
    DSP_STATUS        status    = DSP_SOK ;
    OMAP2530_HalObj * halObject = NULL    ;
    Bool              type                ;

    TRC_3ENTER ("OMAP2530_halMmuCtrl", halObj,memTable,memEntries) ;

    DBC_Require (NULL != halObj) ;

    switch (cmd) {
        case DSP_MmuCtrlCmd_Enable:
        {
            status = OMAP2530_halMmuEnable ((Pvoid)halObj,
                                            memTable,
                                            memEntries) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        break ;

        case DSP_MmuCtrlCmd_Disable:
        {
            status = OMAP2530_halMmuDisable ((Pvoid)halObj) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

        }
        break ;

        case DSP_MmuCtrlCmd_AddEntry:
        {
            halObject = (OMAP2530_HalObj *) halObj ;
            /* Disable the MMU */
            CLEAR_BIT (REG (halObject->mmuBase + MMU_CNTL_OFFSET), 1) ;
            /* type is used to differentiate between dynamic and static entry */
            type = TRUE ;
            /* Add the entry in TLB for new request */
            status = OMAP2530_halMmuRegion(halObject,
                                         mmuInfo->dspAddr,
                                         mmuInfo->size,
                                         type) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            /* Enable the MMU */
            SET_BIT (REG (halObject->mmuBase + MMU_CNTL_OFFSET), 1) ;

        }
        break ;

        case DSP_MmuCtrlCmd_DeleteEntry:
        {
            halObject = (OMAP2530_HalObj *) halObj ;
            /* Disable the MMU */
            CLEAR_BIT (REG (halObject->mmuBase + MMU_CNTL_OFFSET), 1) ;

            /* type is used to differentiate between dynamic and static entry */
            type = TRUE ;

            status = OMAP2530_halMmuDeleteEntry (halObject,
                                                 mmuInfo,
                                                 type) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            /* Enable the MMU */
            SET_BIT (REG (halObject->mmuBase + MMU_CNTL_OFFSET), 1) ;
        }
        break ;

        default:
        {
            /* Unsupported interrupt control command */
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        break ;
    }

    TRC_1LEAVE ("OMAP2530_halMmuCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
