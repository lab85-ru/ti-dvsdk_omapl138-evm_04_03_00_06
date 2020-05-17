/** ============================================================================
 *  @file   omap3530_hal.h
 *
 *  @path   $(DSPLINK)/gpp/inc/sys/arch/OMAP3530/
 *
 *  @desc   Hardware Abstraction Layer.
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
