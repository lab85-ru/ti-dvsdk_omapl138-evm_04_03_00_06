/** ============================================================================
 *  @file   omap3530_phy_shmem.h
 *
 *  @path   $(DSPLINK)/gpp/inc/sys/arch/OMAP3530/Linux/
 *
 *  @desc   Physical Interface Abstraction Layer for OMAP3530.
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


#if !defined (OMAP3530_PHY_SHMEM_H)
#define OMAP3530_PHY_SHMEM_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <hal.h>


#if defined (__cplusplus)
extern "C" {
#endif

/*  ============================================================================
 *  @const  GENERAL_CONTROL_BASE/IVA2_CM_BASE/CORE_CM_BASE/PER_CM_BASE
 *          MAILBOX_BASE/MMU_BASE
 *
 *  @desc   Base address of different Peripherals.
 *  ============================================================================
 */
#define GENERAL_CONTROL_BASE   0x48002270
#define IVA2_CM_BASE           0x48004000
#define CORE_CM_BASE           0x48004A00
#define PER_CM_BASE            0x48005000
#define IVA2_PRM_BASE          0x48306000
#define MAILBOX_BASE           0x48094000
#define MMU_BASE               0x5D000000
#define MMU_SIZE                   0x1000


/** ============================================================================
 *  @name   OMAP3530_shmemInterface
 *
 *  @desc   Interface functions exported by the Shared Driver subcomponent.
 *  ============================================================================
 */
extern HAL_Interface OMAP3530_shmemInterface ;


/* ============================================================================
 *  @func   OMAP3530_phyShmemInit
 *
 *  @desc   Initializes Shared Driver/device.
 *
 *  @arg    halObject.
 *              HAL object.
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
OMAP3530_phyShmemInit (IN Pvoid halObj) ;


/* ============================================================================
 *  @func   OMAP3530_phyShmemExit
 *
 *  @desc   Finalizes Shared Driver/device.
 *
 *  @arg    halObject.
 *              HAL object.
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
OMAP3530_phyShmemExit (IN Pvoid halObj) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (OMAP3530_PHY_SHMEM_H) */
