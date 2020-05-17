/** ============================================================================
 *  @file   omap2530_hal.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/OMAP2530/
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


#if !defined (OMAP2530_HAL_H)
#define OMAP2530_HAL_H


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


/** ============================================================================
 *  @name   OMAP2530_HalObj
 *
 *  @desc   Hardware Abstraction object.
 *
 *  @field  interface
 *              Pointer to HAL interface table.
 *  @field  isrObject
 *              MMU fault isr object.
 *  @field  generalCtrlBase
 *              base address of the control module.
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
typedef struct OMAP2530_HalObj_tag {
    HAL_Interface *  interface       ;
    IsrObject *      isrObject       ;
    Uint32           generalCtrlBase ;
    Uint32           mailboxBase     ;
    Uint32           mmuBase         ;
    Uint32           prcmBase        ;
    Uint32           tlbIndex        ;
    Uint32           procId          ;
    Bool             type            ;
} OMAP2530_HalObj ;


/** ============================================================================
 *  @func   OMAP2530_halInitialize
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
OMAP2530_halInit (IN     Pvoid * halObj,
                  IN     Pvoid   initParams) ;


/** ============================================================================
 *  @func   OMAP2530_halExit
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
OMAP2530_halExit (IN     Pvoid * halObj) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (OMAP2530_HAL_H) */
