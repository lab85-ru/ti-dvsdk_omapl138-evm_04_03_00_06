/** ============================================================================
 *  @file   ldrv_smm.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/SMM/
 *
 *  @desc   Implements the interface exposed by the LDRV_SMM (Shared Memory
 *          Manager) sub-component.
 *          The SMM component provides services for management of shared memory
 *          within the system.
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
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <mem.h>
#include <mem_os.h>
#include <print.h>

/*  ----------------------------------- Link Driver                 */
#include <cfgmap.h>
#include <ldrv.h>
#include <ldrv_smm.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_SMM

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  TRC_SetReason (status, FID_C_LDRV_SMM, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_SMM_MemObject
 *
 *  @desc   This structure defines the memory object managed by the Shared
 *          Memory Manager (SMM). This object contains all information about the
 *          shared memory region required by the SMM.
 *
 *  @field  memEntry
 *              Pointer to memory entry in the configuration.
 *  @field  curFreeOffset
 *              Current free offset within the shared memory region. The memory
 *              from this offset onwards within this region is free to be
 *              allocated.
 *  ----------------------------------------------------------------------------
 */
typedef struct LDRV_SMM_MemObject_tag {
    LINKCFG_MemEntry * memEntry ;
    Uint32             curFreeOffset ;
} LDRV_SMM_MemObject ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_SMM_Object
 *
 *  @desc   This structure defines the Shared Memory Manager (SMM) object, which
 *          contains all state information required by the Shared Memory
 *          Manager.
 *
 *  @field  numMemEntries
 *              Number of memory entries within the memory table.
 *  @field  memTable
 *              Array of SMM memory objects.
 *  ----------------------------------------------------------------------------
 */
typedef struct LDRV_SMM_Object_tag {
    Uint32                numMemEntries ;
    LDRV_SMM_MemObject *  memTable ;
} LDRV_SMM_Object ;


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_SMM_State
 *
 *  @desc   State object for the LDRV_SMM component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC LDRV_SMM_Object LDRV_SMM_State [MAX_DSPS] ;

/*  ----------------------------------------------------------------------------
 *  @name   LDRV_SMM_IsInitialized
 *
 *  @desc   Flags for tracking initialization of the component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool LDRV_SMM_IsInitialized [MAX_DSPS] ;


/** ============================================================================
 *  @func   LDRV_SMM_moduleInit
 *
 *  @desc   This function initializes the LDRV_SMM module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_SMM_moduleInit (Void)
{
    DSP_STATUS status = DSP_SOK ;
    Uint32     dspId ;

    TRC_0ENTER ("LDRV_SMM_moduleInit") ;

    for (dspId = 0 ; dspId < MAX_DSPS ; dspId++) {
        /* Initialize the IsInitialized flag for the DSP. */
        LDRV_SMM_IsInitialized [dspId] = FALSE ;

        /* Initialize the state object. */
        LDRV_SMM_State [dspId].numMemEntries = 0 ;
        LDRV_SMM_State [dspId].memTable      = NULL ;
    }

    TRC_1LEAVE ("LDRV_SMM_moduleInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_SMM_moduleExit
 *
 *  @desc   This function finalizes the LDRV_SMM module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_SMM_moduleExit (Void)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    Uint32     dspId ;

    TRC_0ENTER ("LDRV_SMM_moduleExit") ;

    /* Finalize the IsInitialized flags for all DSPs. */
    for (dspId = 0 ; dspId < MAX_DSPS ; dspId++) {
        if (LDRV_SMM_IsInitialized [dspId] == TRUE) {
            tmpStatus = LDRV_SMM_exit (dspId) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("LDRV_SMM_moduleExit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_SMM_init
 *
 *  @desc   This function initializes the Shared Memory Manger (SMM) component.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_SMM_init (IN ProcessorId dspId)
{
    DSP_STATUS          status = DSP_SOK ;
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_MemEntry *  memTable ;
    LDRV_SMM_Object *   smmState ;
    Uint32              i ;
    LINKCFG_DspConfig * dspCfg ;
#if (  (defined (DM6437_PHYINTERFACE) && (DM6437_PHYINTERFACE == PCI_INTERFACE))                                 \
     ||(defined (DM642_PHYINTERFACE)  && (DM642_PHYINTERFACE  == PCI_INTERFACE))                                 \
     ||(defined (DM648_PHYINTERFACE)  && (DM648_PHYINTERFACE  == PCI_INTERFACE)))
    MemAllocAttrs       memAllocAttrs ;
    Uint32              gppAddr      ;
#endif /* if (DM6437_PHYINTERFACE == PCI_INTERFACE)... */
#if ((defined (DM6437_PHYINTERFACE))  && (DM6437_PHYINTERFACE == VLYNQ_INTERFACE))
    MemMapInfo         mapInfo ;
#endif /* if (DM6437_PHYINTERFACE == VLYNQ_INTERFACE) */


    TRC_1ENTER ("LDRV_SMM_init", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj   = dspCfg->dspObject ;
    memTable = dspCfg->memTables [dspObj->memTableId] ;

    if ((LDRV_SMM_IsInitialized [dspId] == FALSE) && (memTable != NULL)) {
        smmState = &(LDRV_SMM_State [dspId]) ;

        smmState->numMemEntries = dspObj->memEntries ;

        /* Allocate memory for SMM memory objects. */
        status = MEM_Calloc ((Void *) &(smmState->memTable),
                             (sizeof (LDRV_SMM_MemObject) * dspObj->memEntries),
                             MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            /* Check if the memory region is shared. If yes, initialize the
             * pointer to the memory entry.
             * Pointers to Non-shared memory regions remain NULL.
             */
            for (i = 0 ;
                 (i < smmState->numMemEntries) && (DSP_SUCCEEDED (status)) ;
                 i++) {
                smmState->memTable [i].curFreeOffset = 0 ;
                if (   (memTable [i].shared == TRUE)
                    && (memTable [i].syncd  == FALSE)) {
                    smmState->memTable [i].memEntry = &(memTable [i]) ;
                }
                else if (   (memTable [i].shared == TRUE)
                         && (memTable [i].syncd  == TRUE)) {
                    smmState->memTable [i].memEntry = &(memTable [i]) ;

#if (   (defined (DM6437_PHYINTERFACE) && (DM6437_PHYINTERFACE == PCI_INTERFACE))                                 \
     || (defined (DM642_PHYINTERFACE)  && (DM642_PHYINTERFACE  == PCI_INTERFACE))                                 \
     || (defined (DM648_PHYINTERFACE)  && (DM648_PHYINTERFACE  == PCI_INTERFACE)))
                    memAllocAttrs.bigArea = TRUE ;
                    status = MEM_Alloc ((Void **) &gppAddr,
                                        memTable [i].size,
                                        &memAllocAttrs) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                    else {
                        /* Update the physical address, since this is a big
                         * phys allocated memory
                         */
                        memTable [i].physAddr  =
                                        (Uint32) memAllocAttrs.physicalAddress ;
                        memTable [i].gppVirtAddr = gppAddr ;
                    }
#endif /* if (DM6437_PHYINTERFACE == PCI_INTERFACE)... */
#if ((defined (DM6437_PHYINTERFACE)) && (DM6437_PHYINTERFACE == VLYNQ_INTERFACE))
                    mapInfo.src  = memTable [i].physAddr ;
                    mapInfo.size = memTable [i].size ;

                    status = MEM_Map (&mapInfo) ;
                    if (DSP_SUCCEEDED (status)) {
                        memTable [i].gppVirtAddr = mapInfo.dst ;
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
#endif
                }
                else {
                    smmState->memTable [i].memEntry = NULL ;
                }
            }
        }
        else {
            SET_FAILURE_REASON ;
        }

        LDRV_SMM_IsInitialized [dspId] = TRUE ;
        if (DSP_FAILED (status)) {
            /* LDRV_SMM_exit executes cleanup only if the initialized flag is
             * TRUE.
             */
            LDRV_SMM_exit (dspId) ;
            LDRV_SMM_IsInitialized [dspId] = FALSE ;
        }
    }

    TRC_1LEAVE ("LDRV_SMM_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_SMM_exit
 *
 *  @desc   This function finalizes the Shared Memory Manger (SMM) component.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_SMM_exit (IN ProcessorId dspId)
{
    DSP_STATUS          status = DSP_SOK ;
    LDRV_SMM_Object *   smmState ;
#if (  (defined (DM6437_PHYINTERFACE) && (DM6437_PHYINTERFACE == PCI_INTERFACE))                                 \
     ||(defined (DM642_PHYINTERFACE)  && (DM642_PHYINTERFACE  == PCI_INTERFACE))                                 \
     ||(defined (DM648_PHYINTERFACE)  && (DM648_PHYINTERFACE  == PCI_INTERFACE))                                 \
     )
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_DspConfig * dspCfg ;
    LINKCFG_MemEntry *  memTable ;
    Uint32              i ;
    MemFreeAttrs        memFreeAttrs ;
#endif /* if (DM6437_PHYINTERFACE == PCI_INTERFACE)... */
#if ((defined (DM6437_PHYINTERFACE)) && (DM6437_PHYINTERFACE == VLYNQ_INTERFACE))
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_DspConfig * dspCfg ;
    LINKCFG_MemEntry *  memTable ;
    Uint32              i ;
    MemUnmapInfo     unmapInfo        ;
#endif

    TRC_1ENTER ("LDRV_SMM_exit", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    if (LDRV_SMM_IsInitialized [dspId] == TRUE) {
        smmState = &(LDRV_SMM_State [dspId]) ;

#if (  (defined (DM6437_PHYINTERFACE) && (DM6437_PHYINTERFACE == PCI_INTERFACE))                                 \
     ||(defined (DM642_PHYINTERFACE)  && (DM642_PHYINTERFACE  == PCI_INTERFACE))                                 \
     ||(defined (DM648_PHYINTERFACE)  && (DM648_PHYINTERFACE  == PCI_INTERFACE))                                 \
     ||(defined (DM6437_PHYINTERFACE)) && (DM6437_PHYINTERFACE == VLYNQ_INTERFACE))
    dspCfg   = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
    dspObj   = dspCfg->dspObject ;
    memTable = dspCfg->memTables [dspObj->memTableId] ;
    for (i = 0 ;
         (i < smmState->numMemEntries) && (DSP_SUCCEEDED (status)) ;
         i++) {
            if (   (memTable [i].shared == TRUE)
                && (memTable [i].syncd  == TRUE)) {
#if (  (defined (DM6437_PHYINTERFACE) && (DM6437_PHYINTERFACE == PCI_INTERFACE))                                 \
     ||(defined (DM642_PHYINTERFACE)  && (DM642_PHYINTERFACE  == PCI_INTERFACE))                                 \
     ||(defined (DM648_PHYINTERFACE)  && (DM648_PHYINTERFACE  == PCI_INTERFACE)))                                 \

                memFreeAttrs.bigArea = TRUE ;
                memFreeAttrs.size = memTable [i].size ;
                status = MEM_Free ((Void **) &memTable [i].gppVirtAddr,
                                   &memFreeAttrs) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
#endif
#if ((defined (DM6437_PHYINTERFACE)) && (DM6437_PHYINTERFACE == VLYNQ_INTERFACE))
                unmapInfo.addr = memTable [i].gppVirtAddr ;
                unmapInfo.size = memTable [i].size ;
                if (unmapInfo.addr != 0) {
                    status = MEM_Unmap (&unmapInfo) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
                memTable [i].gppVirtAddr = (Uint32) -1 ;
#endif
            }
        }
#endif /* if (DM6437_PHYINTERFACE == PCI_INTERFACE)... */

        if (smmState->memTable != NULL) {
            status = FREE_PTR (smmState->memTable) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        smmState->numMemEntries = 0 ;
        LDRV_SMM_IsInitialized [dspId] = FALSE ;
    }

    TRC_1LEAVE ("LDRV_SMM_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_SMM_alloc
 *
 *  @desc   This function allocates a chunk of memory of the requested size from
 *          the specified shared memory region.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_SMM_alloc (IN  ProcessorId dspId,
                IN  Uint32      memEntryId,
                OUT Uint32 *    physAddr,
                OUT Uint32 *    dspVirtAddr,
                OUT Uint32 *    gppVirtAddr,
                IN  Uint32      size)

{
    DSP_STATUS           status = DSP_SOK ;
    LDRV_SMM_Object *    smmState ;
    LDRV_SMM_MemObject * memPtr ;
    LINKCFG_MemEntry *   memEntry ;

    TRC_6ENTER ("LDRV_SMM_alloc",
                dspId,
                memEntryId,
                physAddr,
                dspVirtAddr,
                gppVirtAddr,
                size) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Assert (LDRV_SMM_IsInitialized [dspId] == TRUE) ;

    smmState = &(LDRV_SMM_State [dspId]) ;

    memPtr   = &(smmState->memTable [memEntryId]) ;

    DBC_Assert (memPtr->memEntry != NULL) ;
    memEntry = memPtr->memEntry ;

    /* Initialize all return values to NULL. */
    if (physAddr    != NULL) {
        *physAddr    = (Uint32) NULL ;
    }
    if (dspVirtAddr != NULL) {
        *dspVirtAddr = (Uint32) NULL ;
    }
    if (gppVirtAddr != NULL) {
        *gppVirtAddr = (Uint32) NULL ;
    }

    /* Check if the memory entry corresponds to a shared region. */
    if (memEntry == NULL) {
        status = DSP_EVALUE ;
        SET_FAILURE_REASON ;
    }
    else {
        /* Check if sufficient memory is available. */
        if ((memPtr->curFreeOffset + size) <= memEntry->size) {
            /* Return the address if the pointer is not NULL. This allows caller
             * to decide which addresses are to be returned.
             */
            if (physAddr    != NULL) {
                *physAddr    = memEntry->physAddr    + memPtr->curFreeOffset ;
            }

            if (dspVirtAddr != NULL) {
                *dspVirtAddr = memEntry->dspVirtAddr + memPtr->curFreeOffset ;
            }

            if (gppVirtAddr != NULL) {
                *gppVirtAddr = memEntry->gppVirtAddr + memPtr->curFreeOffset ;
            }

            memPtr->curFreeOffset+= size ;
        }
        else {
            status = DSP_EMEMORY ;
            PRINT_Printf (
                  "Error: Configured shared memory region is insufficient "
                  "to contain control regions and pool sizes.\n"
                  "Check /dsplink/config/all/CFG_<PLATFORM>.c "
                  "Memory entry: %d", memEntryId) ;
        }
    }

    TRC_1LEAVE ("LDRV_SMM_alloc", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_SMM_free
 *
 *  @desc   This function frees the chunk of memory of the requested size into
 *          the specified shared memory region.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_SMM_free (IN  ProcessorId dspId,
               IN  Uint32      memEntryId,
               IN  Uint32      size)

{
    DSP_STATUS           status = DSP_SOK ;
    LDRV_SMM_Object *    smmState ;
    LDRV_SMM_MemObject * memPtr ;

    TRC_3ENTER ("LDRV_SMM_free", dspId, memEntryId, size) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Assert (LDRV_SMM_IsInitialized [dspId] == TRUE) ;

    smmState = &(LDRV_SMM_State [dspId]) ;

    memPtr   = &(smmState->memTable [memEntryId]) ;

    /* Check if the memory entry corresponds to a shared region. */
    if (memPtr->memEntry == NULL) {
        status = DSP_EVALUE ;
        SET_FAILURE_REASON ;
    }
    else {
        /* Free the specified memory size. */
        memPtr->curFreeOffset-= size ;
    }

    TRC_1LEAVE ("LDRV_SMM_free", status) ;

    return status ;
}


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_SMM_debug
 *
 *  @desc   Prints the current status of this subcomponent.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_SMM_debug (IN  ProcessorId dspId)
{
    LDRV_SMM_Object * smmState ;
    Uint32            i ;

    TRC_1ENTER ("LDRV_SMM_debug", dspId) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;

    smmState = &(LDRV_SMM_State [dspId]) ;

    if (LDRV_SMM_IsInitialized [dspId] == TRUE) {
        /*  --------------------------------------------------------------------
         *  Print the LDRV SMM component state information.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_1PRINT (TRC_LEVEL4,
                    "SMM component state information for DSP ID %d:\n",
                    dspId) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    numMemEntries      = [0x%x].\n",
                    smmState->numMemEntries) ;
        TRC_1PRINT (TRC_LEVEL4,
                    "    memTable           = [0x%x].\n",
                    smmState->memTable) ;
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;

        /*  --------------------------------------------------------------------
         *  Print the information for the shared memory regions.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "Shared memory region information:\n") ;
        for (i = 0 ; i < smmState->numMemEntries ; i++) {
            if (smmState->memTable [i].memEntry != NULL) {
                TRC_1PRINT (TRC_LEVEL4,
                            "entryId            = [%d]\n",
                            i) ;
                TRC_1PRINT (TRC_LEVEL4,
                            "    physAddr       = [0x%x]\n",
                            smmState->memTable [i].memEntry->physAddr) ;
                TRC_1PRINT (TRC_LEVEL4,
                            "    dspVirtAddr    = [0x%x]\n",
                            smmState->memTable [i].memEntry->dspVirtAddr) ;
                TRC_1PRINT (TRC_LEVEL4,
                            "    gppVirtAddr    = [0x%x]\n",
                            smmState->memTable [i].memEntry->gppVirtAddr) ;
                TRC_1PRINT (TRC_LEVEL4,
                            "    size           = [0x%x]\n",
                            smmState->memTable [i].memEntry->size) ;
                TRC_1PRINT (TRC_LEVEL4,
                            "    curFreeOffset  = [0x%x]\n",
                            smmState->memTable [i].curFreeOffset) ;
            }
        }
    }
    else {
        TRC_1PRINT (TRC_LEVEL4,
                    "SMM component for DSP ID %d is not initialized.\n",
                    dspId) ;
    }

    TRC_0LEAVE ("LDRV_SMM_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif
