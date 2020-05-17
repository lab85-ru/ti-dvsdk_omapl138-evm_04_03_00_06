/** ============================================================================
 *  @file   hal_interrupt.c
 *
 *  @path   $(DSPLINK)/dsp/src/base/hal/DspBios/DM648/
 *
 *  @desc   Implementation of interrupt-related functionality of the Hardware
 *          Abstraction Layer.
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


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <c64.h>
#include <hwi.h>
#include <bcache.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <failure.h>
#include <platform.h>
#include <_bitops.h>
#include <hal.h>
#include <hal_interrupt.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define  FILEID                         FID_HAL_C

/** ============================================================================
 *  @const   PCIINT_EVENT_ID
 *
 *  @desc    Event number for PCI event.
 *  ============================================================================
 */
#define PCIINT_EVENT_ID                 15

/** ============================================================================
 *  @const   DM648PCI_SOFTINT0_MASK
 *
 *  @desc    Mask for generating soft int0 (DSP->GPP)
 *  ============================================================================
 */
#define DM648PCI_SOFTINT0_MASK         0x01000000

/** ============================================================================
 *  @const   DM648PCI_SOFTINT1_MASK
 *
 *  @desc    Mask for generating soft int1 (GPP->DSP)
 *  ============================================================================
 */
#define DM648PCI_SOFTINT1_MASK         0x02000000

/** ============================================================================
 *  @const   DM648PCI_REG_STATSET
 *
 *  @desc    PCI int status register address used for generating INT to GPP.
 *  ============================================================================
 */
#define DM648PCI_REG_STATSET          0x02048410

/** ============================================================================
 *  @const   DM648PCI_REG_STATCLR
 *
 *  @desc    PCI int clear register address used for clearing INT from GPP.
 *  ============================================================================
 */
#define DM648PCI_REG_STATCLR          0x02048414

/** ============================================================================
 *  @const  REG32
 *
 *  @desc   Macro for register access.
 *  ============================================================================
 */
#define  REG32(addr)            *((Uint32 *)(addr))


/** ============================================================================
 *  @name   HAL_IntIsrFxn
 *
 *  @desc   ISR function pointer.
 *  ============================================================================
 */
static Fxn  HAL_IntIsrFxn ;

/** ============================================================================
 *  @name   HAL_IntIsrArg
 *
 *  @desc   ISR function argument.
 *  ============================================================================
 */
static Ptr  HAL_IntIsrArg ;

/** ============================================================================
 *  @name   HAL_IntVectorId
 *
 *  @desc   Interrupt vector number.
 *  ============================================================================
 */
static Uint32 HAL_IntVectorId ;


/** ----------------------------------------------------------------------------
 *  @func   HAL_intShmStub
 *
 *  @desc   ISR function for GPP interrupt.
 *
 *  @arg    arg
 *              Index number for the interrupt.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    HAL_intInit ()
 *  ----------------------------------------------------------------------------
 */
static
Void
HAL_intShmStub (Ptr arg) ;


/*  ============================================================================
 *  Create named sections for the functions to allow specific memory placement.
 *  ============================================================================
 */
#pragma CODE_SECTION (HAL_intInit,      ".text:DSPLINK_init")
#pragma CODE_SECTION (HAL_intRegister,  ".text:DSPLINK_init")

/** ============================================================================
 *  @func   HAL_intInit
 *
 *  @desc   Initializes the HAL Interrupt module.
 *
 *  @modif  None
 *  ============================================================================
 */
Void
HAL_intInit ()
{
    HAL_IntIsrFxn   = NULL ;
    HAL_IntIsrArg   = NULL ;
    HAL_IntVectorId = 0 ;
}


/** ============================================================================
 *  @func   HAL_intEnable
 *
 *  @desc   Enables the specified GPP interrupt.
 *
 *  @modif  None
 *  ============================================================================
 */
Void
HAL_intEnable (Uint32 intId)
{
    (Void) intId ;

    C64_enableIER (1 << HAL_IntVectorId) ;
}


/** ============================================================================
 *  @func   HAL_intDisable
 *
 *  @desc   Disables the specified GPP interrupt.
 *
 *  @modif  None
 *  ============================================================================
 */
Void
HAL_intDisable (Uint32 intId)
{
    (Void) intId ;

    C64_disableIER (1 << HAL_IntVectorId) ;
}


/** ============================================================================
 *  @func   HAL_intRegister
 *
 *  @desc   Register ISR for the specified GPP interrupt.
 *
 *  @modif  None
 *  ============================================================================
 */
Void
HAL_intRegister (Uint32 intId, Uint32 intVectorId, Fxn func, Ptr arg)
{
    HWI_Attrs hwiAttrs ;
    Uns       hold ;

    /* Disable global interrupts */
    hold = HWI_disable () ;

    HAL_IntIsrFxn   = func ;
    HAL_IntIsrArg   = arg  ;
    HAL_IntVectorId = intVectorId ;

    /* Map the interrupt number to HWI vector */
    HWI_eventMap (intVectorId, intId) ;

    /* Register interrupt for communication between ARM and DSP */
    hwiAttrs.intrMask = (1 << intVectorId) ;
    hwiAttrs.ccMask   = 1 ;
    hwiAttrs.arg      = (Arg) NULL ;
    HWI_dispatchPlug (intVectorId,
                      (Fxn) HAL_intShmStub,
                      -1,
                      &hwiAttrs) ;

    BCACHE_wbInvAll () ;

    /* Restore global interrupts */
    HWI_restore (hold) ;

    /* Enable the registered interrupt */
    HAL_intEnable (intId) ;
}


/** ============================================================================
 *  @func   HAL_intWaitClear
 *
 *  @desc   Wait for interrupt to be cleared.
 *
 *  @modif  None
 *  ============================================================================
 */
Void
HAL_intWaitClear (Uint32 intId)
{
    (Void) intId ;
}


/** ============================================================================
 *  @func   HAL_intSend
 *
 *  @desc   Send interrupt to GPP.
 *
 *  @modif  None
 *  ============================================================================
 */
Void
HAL_intSend (Uint32 intId, Uns arg)
{
    (void) intId ;
    (void) arg ;

    REG32 (DM648PCI_REG_STATSET) |= DM648PCI_SOFTINT0_MASK ;
}


/** ============================================================================
 *  @func   HAL_intClear
 *
 *  @desc   Clear interrupt.
 *
 *  @modif  None
 *  ============================================================================
 */
Uns
HAL_intClear (Uint32 intId)
{
    (void) intId ;

    REG32 (DM648PCI_REG_STATCLR) = DM648PCI_SOFTINT1_MASK ;

    return 0 ;
}


/** ----------------------------------------------------------------------------
 *  @func   HAL_intShmStub
 *
 *  @desc   Interrupt service routine stub for the GPP interrupt.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Void
HAL_intShmStub (Ptr arg)
{
    (void) arg ;

    /* arg contains the index number for the interrupt. */
    if (HAL_IntIsrFxn != NULL) {
        (*HAL_IntIsrFxn) (HAL_IntIsrArg) ;
    }
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
