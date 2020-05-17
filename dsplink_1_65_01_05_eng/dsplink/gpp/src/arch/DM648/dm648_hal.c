/** ============================================================================
 *  @file   dm648_hal.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM648/
 *
 *  @desc   Hardware Abstraction Layer for 6437_PCI.
 *          Creates and Deletes the platform specific HAL object.
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
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <osal.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm648_hal.h>
#include <dm648_phy_pci.h>

#if defined (__cplusplus)
extern "C" {
#endif

/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_HAL

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON      TRC_SetReason (status, FID_C_ARCH_HAL, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   LDRV_LinkCfgPtr
 *
 *  @desc   Global pointer containing DSPLINK configuration information for the
 *          driver.
 *  ============================================================================
 */
extern LINKCFG_Object * LDRV_LinkCfgPtr ;


/** ============================================================================
 *  @func   DM648_halInit
 *
 *  @desc   Initializes the HAL object and physical interface.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM648_halInit (IN Pvoid * halObj, IN Pvoid initParams)
{
    DSP_STATUS          status   = DSP_SOK ;
    DM648_InitParams *  params   = (DM648_InitParams *) initParams ;
    Uint32              tempMappedSize = 0 ;
    DM648_HalObj *      halObject          ;
    LINKCFG_MemEntry *  memTable           ;
    LINKCFG_DspConfig * dspCfg             ;
    LINKCFG_Dsp *       dspObj             ;

    TRC_2ENTER ("DM648_halInit", halObj, initParams) ;

    DBC_Require (halObj != NULL) ;
    DBC_Require (initParams != NULL) ;

    status = MEM_Calloc ((Pvoid *) &halObject,
                         sizeof (DM648_HalObj),
                         MEM_DEFAULT) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }
    else {
        dspCfg    = LDRV_LinkCfgPtr->dspConfigs [params->dspId] ;
        dspObj    = dspCfg->dspObject ;
        memTable  = dspCfg->memTables [dspObj->memTableId] ;

        /* is PCI phy required? */
        if (params->phyType == PCI_INTERFACE) {
            halObject->interface      = &DM648_pciInterface ;
            halObject->pci.busNo      = params->arg1 ;
            halObject->pci.slotNo     = params->arg2 ;
            halObject->pci.edmaChnlId = params->arg5 ;

            status = halObject->interface->phyInit (halObject) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            /* Map the shared memory region */
            else {
                status = halObject->interface->mapCtrl (
                                 (Pvoid) halObject,
                                 DSP_MapCtrlCmd_SetShared,
                                 memTable [params->shMemEntry].dspVirtAddr,
                                 memTable [params->shMemEntry].size,
                                 &memTable [params->shMemEntry].gppVirtAddr,
                                 &memTable [params->shMemEntry].physAddr,
                                 &tempMappedSize) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                else {

                }
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                else {
                    if (tempMappedSize != memTable [params->shMemEntry].size) {
                        /* Can not map the requested size */
                        status = DSP_ERANGE ;
                        SET_FAILURE_REASON ;
                    }
                    else {
                        halObject->pci.shMemVirtAddr =
                                 memTable [params->shMemEntry].gppVirtAddr ;
                        halObject->pci.shMemPhysAddr =
                                    memTable [params->shMemEntry].physAddr ;
                    }
                }
            }
        }
        else {
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }

        *halObj = halObject ;
    }

    TRC_1LEAVE ("DM648_halInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DM648_halExit
 *
 *  @desc   Finializes the HAL object and physical interface.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM648_halExit (IN Pvoid * halObj)
{
    DSP_STATUS     status    = DSP_SOK ;
    DM648_HalObj * halObject ;

    TRC_1ENTER ("DM648_halInit", halObj) ;

    DBC_Require (halObj != NULL) ;

    halObject = *halObj ;
    status = halObject->interface->phyExit (halObject) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    FREE_PTR (halObject) ;

    TRC_1LEAVE ("DM648_halExit", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
