/** ============================================================================
 *  @file   hal_interrupt.c
 *
 *  @path   $(DSPLINK)/dsp/src/base/hal/DspBios/OMAP2530/
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

/*  ============================================================================
 *  @macro  REG32
 *
 *  @desc   Register access method.
 *  ============================================================================
 */
 #define REG32(A)        (*(volatile Uint32 *) (A))

/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define  FILEID                 FID_HAL_C

/** ============================================================================
 *  @const  NUM_ARM_TO_DSP_INT
 *
 *  @desc   Defines the number of ARM-DSP interrupts supported by the
 *          hal_interrupt module.
 *  ============================================================================
 */
#define  NUM_ARM_TO_DSP_INT     1

/*  ============================================================================
 *  @const  MAILBOX_BASE,
 *          MAILBOX_MESSAGE_0,
 *          MAILBOX_MESSAGE_1,
 *          MAILBOX_MSGSTATUS_0,
 *          MAILBOX_MSGSTATUS_1,
 *          MAILBOX_IRQSTATUS_0,
 *          MAILBOX_IRQSTATUS_1
 *
 *  @desc   Mailbox management values
 *  ============================================================================
 */
#define MAILBOX_BASE           0x48094000
#define MAILBOX_MESSAGE_0      MAILBOX_BASE + 0x40
#define MAILBOX_MESSAGE_1      MAILBOX_BASE + 0x44
#define MAILBOX_MSGSTATUS_0    MAILBOX_BASE + 0xC0
#define MAILBOX_MSGSTATUS_1    MAILBOX_BASE + 0xC4
#define MAILBOX_IRQSTATUS_0    MAILBOX_BASE + 0x100
#define MAILBOX_IRQSTATUS_1    MAILBOX_BASE + 0x108

/*  ============================================================================
 *  @const  WUGEN_MEVTCLR0
 *
 *  @desc   Wake-up Generator register needed to enable mailbox interrupts.
 *  ============================================================================
 */
#define WUGEN_MEVTCLR0         0x01C21070

/*  ============================================================================
 *  @const  BASE_ARM2DSP_INTID
 *
 *  @desc   Interrupt number of the first ARM2DSP interrupt.
 *  ============================================================================
 */
#define  BASE_ARM2DSP_INTID     55

/*  ============================================================================
 *  @const  BASE_DSP2ARM_INTID
 *
 *  @desc   Interrupt number of the first DSP2ARM interrupt.
 *  ============================================================================
 */
#define  BASE_DSP2ARM_INTID     26


/*  ============================================================================
 *  @macro  ARM2DSP_INT_INDEX
 *
 *  @desc   Index of the ARM2DSP interrupt (0/1/2/3) based on the interrupt
 *          number.
 *  ============================================================================
 */
#define  ARM2DSP_INT_INDEX(intId)  (intId - BASE_ARM2DSP_INTID)


/** ============================================================================
 *  @name   HAL_IntIsrFxn
 *
 *  @desc   ISR function pointer.
 *  ============================================================================
 */
static Fxn  HAL_IntIsrFxn [NUM_ARM_TO_DSP_INT] ;

/** ============================================================================
 *  @name   HAL_IntIsrArg
 *
 *  @desc   ISR function argument.
 *  ============================================================================
 */
static Ptr  HAL_IntIsrArg [NUM_ARM_TO_DSP_INT] ;

/** ============================================================================
 *  @name   HAL_IntVectorId
 *
 *  @desc   Interrupt vector number.
 *  ============================================================================
 */
static Uint32 HAL_IntVectorId [NUM_ARM_TO_DSP_INT] ;


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
    Uint32 i ;

    /* Initialize global variable */
    for (i = 0 ; i < NUM_ARM_TO_DSP_INT ; i++) {
        HAL_IntIsrFxn   [i] = NULL ;
        HAL_IntIsrArg   [i] = NULL ;
        HAL_IntVectorId [i] = 0 ;
    }
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
    C64_enableIER (1 << (HAL_IntVectorId [ARM2DSP_INT_INDEX (intId)])) ;
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
    C64_disableIER (1 << (HAL_IntVectorId [ARM2DSP_INT_INDEX (intId)])) ;
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
    Uint32    index ;

    /* Disable global interrupts */
    hold = HWI_disable () ;

    index = ARM2DSP_INT_INDEX (intId) ;
    HAL_IntIsrFxn   [index] = func ;
    HAL_IntIsrArg   [index] = arg ;
    HAL_IntVectorId [index] = intVectorId ;

    /* Map the interrupt number to HWI vector */
    HWI_eventMap (intVectorId, intId) ;

    /* Register interrupt for communication between ARM and DSP */
    hwiAttrs.intrMask = (1 << intVectorId) ;
    hwiAttrs.ccMask   = 1 ;
    hwiAttrs.arg      = (Arg) index ;
    HWI_dispatchPlug (intVectorId,
                      (Fxn) HAL_intShmStub,
                      -1,
                      &hwiAttrs) ;

    BCACHE_wbInvAll () ;

    /* Restore global interrupts */
    HWI_restore (hold) ;

    /* Unmask IVA_2_IRQ[10] to allow interrupt to come into DSP */
    REG32(WUGEN_MEVTCLR0) = (1<<10);

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
    /* To avoid compiler warning. */
    (void) intId ;

    /* Wait for DSP to clear the previous interrupt */
    while( REG32(MAILBOX_MSGSTATUS_1) & 7 );
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
    /* To avoid compiler warning. */
    (void) intId ;

    /* Put into the GPP's mailbox to generate the interrupt. */
    REG32(MAILBOX_MESSAGE_0) = arg ;
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
    Uns arg ;

    /* To avoid compiler warning. */
    (void) intId ;

    /* Read into val. */
    arg = REG32(MAILBOX_MESSAGE_1);

    /* Clear the IRQ status.
     * If there are more in the mailbox FIFO, it will re-assert.
     */
    REG32(MAILBOX_IRQSTATUS_1) = 0x4;

    return (arg) ;
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
    /* arg contains the index number for the interrupt. */
    if (HAL_IntIsrFxn [(Uint32) arg] != NULL) {
        (*HAL_IntIsrFxn [(Uint32) arg]) (HAL_IntIsrArg [(Uint32) arg]) ;
    }
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
