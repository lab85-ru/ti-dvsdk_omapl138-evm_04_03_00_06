/** ============================================================================
 *  @file   tnetv107xgem_hal.h
 *
 *  @path   $(DSPLINK)/gpp/inc/sys/arch/TNETV107XGEM/
 *
 *  @desc   Hardware Abstraction Layer for Power Controller (PWR)
 *          module in Davinci. Declares necessary functions for
 *          PSC request handling.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


#if !defined (TNETV107XGEM_HAL_H)
#define TNETV107XGEM_HAL_H


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
 *  @name   TNETV107XGEM_HalObj
 *
 *  @desc   Hardware Abstraction object.
 *
 *  @field  interface
 *              Pointer to HAL interface table.
 *  @field  intdBase
 *              base address of the INTD regs.
 *  @field  cfgBase
 *              base address of the Chip Configuration regs.
 *  @field  clkCtrlBase
 *              base address of the Clock Control regs.
 *  ============================================================================
 */
typedef struct TNETV107XGEM_HalObj_tag {
    HAL_Interface * interface ;
    Uint32          intdBase  ;
    Uint32          cfgBase   ;
    Uint32          clkCtrlBase;
} TNETV107XGEM_HalObj ;


/** ============================================================================
 *  @func   TNETV107XGEM_halInitialize
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
TNETV107XGEM_halInit (IN     Pvoid * halObj,
                   IN     Pvoid   initParams) ;


/** ============================================================================
 *  @func   TNETV107XGEM_halExit
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
TNETV107XGEM_halExit (IN     Pvoid * halObj) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (TNETV107XGEM_HAL_H) */
