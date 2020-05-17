/** ============================================================================
 *  @file   omap2530_hal_intgen.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/OMAP2530/
 *
 *  @desc   Hardware Abstraction Layer for Omap2530.
 *          Defines necessary functions for Interrupt Handling.
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

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <omap2530_hal.h>
#include <omap2530_hal_intgen.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_HAL_INTGEN


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_ARCH_HAL_INTGEN, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/*  ============================================================================
 *  @const  MAILBOX_SYSCONFIG_OFFSET,
 *          MAILBOX_SYSSTATUS_OFFSET,
 *          MAILBOX_MESSAGE_0_OFFSET,
 *          MAILBOX_MESSAGE_1_OFFSET,
 *          MAILBOX_MSGSTATUS_0_OFFSET,
 *          MAILBOX_MSGSTATUS_1_OFFSET,
 *          MAILBOX_IRQSTATUS_0_OFFSET,
 *          MAILBOX_IRQENABLE_0_OFFSET,
 *          MAILBOX_IRQSTATUS_1_OFFSET
 *          MAILBOX_IRQENABLE_1_OFFSET
 *
 *  @desc   Offsets from the Mailbox base address
 *  ============================================================================
 */
#define MAILBOX_SYSCONFIG_OFFSET      0x10
#define MAILBOX_SYSSTATUS_OFFSET      0x14
#define MAILBOX_MESSAGE_0_OFFSET      0x40
#define MAILBOX_MESSAGE_1_OFFSET      0x44
#define MAILBOX_MSGSTATUS_0_OFFSET    0xC0
#define MAILBOX_MSGSTATUS_1_OFFSET    0xC4
#define MAILBOX_IRQSTATUS_0_OFFSET    0x100
#define MAILBOX_IRQENABLE_0_OFFSET    0x104
#define MAILBOX_IRQSTATUS_1_OFFSET    0x108
#define MAILBOX_IRQENABLE_1_OFFSET    0x10C



/*  ============================================================================
 *  @const  CM_ICLKEN1_CORE_OFFSET
 *
 *  @desc   Offsets from the CORE_CM base address
 *  ============================================================================
 */
#define CM_ICLKEN1_CORE_OFFSET      0x210



/** ============================================================================
 *  @func   OMAP2530_halIntCtrl
 *
 *  @desc   Interrupt Controller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP2530_halIntCtrl (IN         Pvoid          halObj,
                      IN         DSP_IntCtrlCmd cmd,
                      IN         Uint32         intId,
                      IN OUT     Pvoid          arg)
{
    DSP_STATUS         status    = DSP_SOK ;
    OMAP2530_HalObj * halObject = NULL    ;

    TRC_3ENTER ("OMAP2530_halIntCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObj) ;

    halObject = (OMAP2530_HalObj *) halObj ;

    switch (cmd) {
        case DSP_IntCtrlCmd_Enable:
        {

            /* Enable Mailboxes interface clock control. Set bit 30 */
            SET_BIT (REG (halObject->prcmBase + CM_ICLKEN1_CORE_OFFSET), 30) ;

            /* Perform software reset of the Mailbox module, set bit 1 */
            SET_BIT (REG (halObject->mailboxBase + MAILBOX_SYSCONFIG_OFFSET), 1) ;

            /* Wait for reset complete. If Bit 0 == 0, it's still resetting */
            while( (REG (halObject->mailboxBase + MAILBOX_SYSSTATUS_OFFSET) & 1 ) == 0 )
            {
                /* Wait for reset to complete */
            }

            /* Enable autoidle, set bit 0 */
            SET_BIT (REG (halObject->mailboxBase + MAILBOX_SYSCONFIG_OFFSET), 0) ;

            /*
             *  Configure the Mail BOX IRQENABLE register for DSP.
             *  DSP receives on Mailbox 1.
             */
            REG (halObject->mailboxBase + MAILBOX_IRQENABLE_1_OFFSET) = 0x4 ;

            /*
             *  Configure the Mail BOX IRQENABLE register for GPP.
             *  GPP receives on Mailbox 0.
             */
            REG (halObject->mailboxBase + MAILBOX_IRQENABLE_0_OFFSET) = 0x1 ;

        }
        break ;

        case DSP_IntCtrlCmd_Disable:
        {
            /*
             *  Disable the Mail BOX IRQENABLE register for DSP.
             *  DSP receives on Mailbox 1.
             */
            REG (halObject->mailboxBase + MAILBOX_IRQENABLE_1_OFFSET) = 0x0 ;

            /*
             *  Disable the Mail BOX IRQENABLE register for GPP.
             *  GPP receives on Mailbox 0.
             */
            REG (halObject->mailboxBase + MAILBOX_IRQENABLE_0_OFFSET) = 0x0 ;
        }
        break ;

        case DSP_IntCtrlCmd_Send:
        {
            /* Put into the DSP's mailbox to generate the interrupt.
              * Sends a specified interrupt to the DSP
              */
            REG32(halObject->mailboxBase + MAILBOX_MESSAGE_1_OFFSET) = intId ;

        }
        break ;

        case DSP_IntCtrlCmd_Clear:
        {
            /* Read the register to get the entry from the mailbox FIFO */
            REG32(halObject->mailboxBase + MAILBOX_MESSAGE_0_OFFSET);

            /* Clear the IRQ status.
             * If there are more in the mailbox FIFO, it will re-assert.
             */
            REG32(halObject->mailboxBase + MAILBOX_IRQSTATUS_0_OFFSET) = 0x1;
        }
        break ;

        case DSP_IntCtrlCmd_Check:
        {
            /* Do nothing here for Omap2530 Gem */
            *((Bool *) arg) = TRUE ;
        }
        break ;

        default:
        {
            /* Unsupported interrupt control command */
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        break ;
    }

    TRC_1LEAVE ("OMAP2530_halIntCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
