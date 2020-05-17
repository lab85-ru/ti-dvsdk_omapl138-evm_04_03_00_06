/** ============================================================================
 *  @file   omap3530_hal.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/OMAP3530/
 *
 *  @desc   Hardware Abstraction Layer.
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


#if !defined (OMAP3530_HAL_H)
#define OMAP3530_HAL_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <hal.h>

/*  ----------------------------------- OSAL Headers                */
#include <osal.h>

#if defined (__cplusplus)
extern "C" {
#endif


/*  ============================================================================
 *  @macro  REG
 *
 *  @desc   Regsiter access method.
 *  ============================================================================
 */
#define REG(x)              *((volatile Uint32 *) (x))


/** ============================================================================
 *  @name   OMAP3530_HalObj
 *
 *  @desc   Hardware Abstraction object.
 *
 *  @field  interface
 *              Pointer to HAL interface table.
 *  @field  isrObject
 *              Pointer to IsrObject.
 *  @field  generalCtrlBase
 *              Virtual base address of the General Control module.
 *  @field  iva2CmBase
 *              Virtual base address of the IVA2 CM module.
 *  @field  coreCmBase
 *              Virtual base address of the CORE CM module.
 *  @field  perCmBase
 *              Virtual base address of the PER CM module.
 *  @field  iva2PrmBase
 *              Virtual base address of the IVA2 PRM module.
 *  @field  mailboxBase
 *              base address of the mailbox module.
 *  @field  mmuBase
 *              base address of the MMU module.
 *  @field  prcmBase
 *              base address of the PRCM module.
 *  @field  tlbIndex
 *              TLB entry of the MMU module.
 *  @field  procId 
 *              Processor ID.
 *  @field  type
 *              Flag to verify the static or dynamic entry.
 *  ============================================================================
 */
typedef struct OMAP3530_HalObj_tag {
    HAL_Interface * interface       ;
    IsrObject     * isrObject       ;
    Uint32          generalCtrlBase ;
    Uint32          iva2CmBase      ;
    Uint32          coreCmBase      ;
    Uint32          perCmBase       ;
    Uint32          iva2PrmBase     ;
    Uint32          mailboxBase     ;
    Uint32          mmuBase         ;
    Uint32          tlbIndex        ;
    Uint32          procId          ;
    Bool            type            ;

} OMAP3530_HalObj ;


/** ============================================================================
 *  @func   OMAP3530_halInitialize
 *
 *  @desc   Initializes the HAL object.
 *
 *  @arg    halObj.
 *              Pointer to HAL object.
 *  @arg    initParams.
 *              Parameters for initialize (optional).
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              All other error conditions.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP3530_halInit (IN     Pvoid * halObj,
                   IN     Pvoid   initParams) ;


/** ============================================================================
 *  @func   OMAP3530_halExit
 *
 *  @desc   Finalizes the HAL object.
 *
 *  @arg    halObj.
 *              Pointer to HAL object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              All other error conditions.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP3530_halExit (IN Pvoid * halObj) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (OMAP3530_HAL_H) */
