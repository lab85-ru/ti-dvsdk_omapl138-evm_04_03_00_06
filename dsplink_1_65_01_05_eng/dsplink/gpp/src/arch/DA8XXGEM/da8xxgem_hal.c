/** ============================================================================
 *  @file   da8xxgem_hal.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DA8XXGEM/
 *
 *  @desc   Power management module.
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
#include <mem.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <da8xxgem_hal.h>
#if (DA8XXGEM_PHYINTERFACE == SHMEM_INTERFACE)
#include <da8xxgem_phy_shmem.h>
#endif /* if (DA8XXGEM_PHYINTERFACE == SHMEM_INTERFACE) */


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
 *  @func   DA8XXGEM_halInitialize
 *
 *  @desc   Initializes the HAL object.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_halInit (IN     Pvoid * halObj,
                   IN     Pvoid   initParams)
{
    DSP_STATUS         status   = DSP_SOK ;
    DA8XXGEM_InitParams * params = (DA8XXGEM_InitParams *) initParams ;
    DA8XXGEM_HalObj * halObject ;
    LINKCFG_Dsp *        dspObj ;
    LINKCFG_DspConfig *  dspCfg ;

    TRC_2ENTER ("DA8XXGEM_halInit", halObj, initParams) ;

    DBC_Require (halObj != NULL) ;

    (void) initParams ;

    status = MEM_Alloc ((Pvoid) &halObject,
                        sizeof (DA8XXGEM_HalObj),
                        MEM_DEFAULT) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }
    else {
        *halObj = (Pvoid) halObject ;
        dspCfg    = LDRV_LinkCfgPtr->dspConfigs [params->dspId] ;
        dspObj    = dspCfg->dspObject ;

#if (DA8XXGEM_PHYINTERFACE == SHMEM_INTERFACE)
        halObject->interface = &DA8XXGEM_shmemInterface ;
#endif /* if (DA8XXGEM_PHYINTERFACE == SHMEM_INTERFACE) */
        status = halObject->interface->phyInit ((Pvoid) halObject) ;
        if (DSP_FAILED (status)) {
            FREE_PTR (halObject) ;
            SET_FAILURE_REASON ;
        }
        else {
            if(dspObj->arg4 == 0) {
                /* Unlocking the KICK registers once
                 * DSPLINK does not do lock and Unlock sequence
                 * as writing to kick registers  simultaneousely
                 * by both GPP and DSP causes failurs in sending interrupts.
                 */
                /* Unlock the kick0 area */
                *((volatile Uint32 *) (  halObject->baseCfgBus
                              + halObject->offsetSysModule
                              + OFFSET_KICK0 )) = KICK0_UNLOCKVAL ;

                /* Unlock the kick1 area */
                *((volatile Uint32 *) (  halObject->baseCfgBus
                              + halObject->offsetSysModule
                              + OFFSET_KICK1)) = KICK1_UNLOCKVAL ;
            }
        }
    }

    TRC_1LEAVE ("DA8XXGEM_halInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DA8XXGEM_halExit
 *
 *  @desc   Finalizes the HAL object.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_halExit (IN     Pvoid * halObj)
{
    DSP_STATUS status   = DSP_SOK ;

    TRC_1ENTER ("DA8XXGEM_halExit", halObj) ;

    DBC_Require (halObj != NULL) ;

    FREE_PTR ((*halObj)) ;

    TRC_1LEAVE ("DA8XXGEM_halInit", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
