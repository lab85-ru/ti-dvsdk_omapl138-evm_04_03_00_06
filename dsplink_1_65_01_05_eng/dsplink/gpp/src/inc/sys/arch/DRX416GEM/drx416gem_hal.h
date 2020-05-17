/** ============================================================================
 *  @file   drx416gem_hal.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DRX416GEM/
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


#if !defined (DRX416GEM_HAL_H)
#define DRX416GEM_HAL_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <hal.h>


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
 *  @name   DRX416GEM_HalObj
 *
 *  @desc   Hardware Abstraction object.
 *
 *  @field  interface
 *              Pointer to HAL interface table.
 *  @field  baseCfgBus
 *              base address of the configuration bus peripherals memory range.
 *  @field  offsetSysModule
 *              Offset of the system module from CFG base.
 *  ============================================================================
 */
typedef struct DRX416GEM_HalObj_tag {
    HAL_Interface * interface       ;
    Uint32          baseCfgBus      ;
    Uint32          offsetSysModule ;
} DRX416GEM_HalObj ;


/** ============================================================================
 *  @func   DRX416GEM_halInitialize
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
DRX416GEM_halInit (IN     Pvoid * halObj,
                   IN     Pvoid   initParams) ;


/** ============================================================================
 *  @func   DRX416GEM_halExit
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
DRX416GEM_halExit (IN     Pvoid * halObj) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (DRX416GEM_HAL_H) */
