/** ============================================================================
 *  @file   da8xxgem_hal.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DA8XXGEM/
 *
 *  @desc   Hardware Abstraction Layer for Power Controller (PWR)
 *          module in Primus. Declares necessary functions for
 *          PSC request handling.
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


#if !defined (DA8XXGEM_HAL_H)
#define DA8XXGEM_HAL_H


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
 *  @desc   Register access method.
 *  ============================================================================
 */
#define REG(x)              *((volatile Uint32 *) (x))

/*  ============================================================================
 *  @const  OFFSET_KICK0
 *
 *  @desc   Offset of the kick0 register.
 *  ============================================================================
 */
#define  OFFSET_KICK0            0x038u

/*  ============================================================================
 *  @const  OFFSET_KICK1
 *
 *  @desc   Offset of the kick1 register.
 *  ============================================================================
 */
#define  OFFSET_KICK1            0x03Cu

/*  ============================================================================
 *  @const  KICK0_UNLOCKVAL
 *
 *  @desc   VALUE to be written to unlock kick0 area.
 *  ============================================================================
 */
#define  KICK0_UNLOCKVAL         0x83E70B13u

/*  ============================================================================
 *  @const  KICK1_UNLOCKVAL
 *
 *  @desc   VALUE to be written to unlock kick1 area.
 *  ============================================================================
 */
#define  KICK1_UNLOCKVAL         0x95A4F1E0u

/** ============================================================================
 *  @name   DA8XXGEM_HalObj
 *
 *  @desc   Hardware Abstraction object.
 *
 *  @field  interface
 *              Pointer to HAL interface table.
 *  @field  baseCfgBus
 *              base address of the configuration bus peripherals memory range.
 *  @field  offsetSysModule
 *              Offset of the system module from CFG base.
 *  @field  offsetPsc0
 *              Offset of the PSC0 from CFG base.
 *  ============================================================================
 */
typedef struct DA8XXGEM_HalObj_tag {
    HAL_Interface * interface       ;
    Uint32          baseCfgBus      ;
    Uint32          offsetSysModule ;
    Uint32          offsetPsc0      ;
} DA8XXGEM_HalObj ;

/** ============================================================================
 *  @name   DA8XX_InitParams
 *
 *  @desc   Initialize parameters for HAL init call.
 *
 *  @field  dspId
 *              Processor Identifier.
 *  ============================================================================
 */
typedef struct DA8XXGEM_InitParams_tag {
    ProcessorId dspId   ;
} DA8XXGEM_InitParams ;

/** ============================================================================
 *  @func   DA8XXGEM_halInitialize
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
 *          DSP_EMEMORY
 *              A memory allocation failure occurred.
 *          DSP_EINVALIDARG
 *              An invalid argument was specified.
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
DA8XXGEM_halInit (IN     Pvoid * halObj,
                  IN     Pvoid   initParams) ;


/** ============================================================================
 *  @func   DA8XXGEM_halExit
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
DA8XXGEM_halExit (IN     Pvoid * halObj) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (DA8XXGEM_HAL_H) */
