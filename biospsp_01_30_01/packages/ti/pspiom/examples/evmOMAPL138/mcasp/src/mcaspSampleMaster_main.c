/******************************************************************************
**+-------------------------------------------------------------------------+**
**|                            ****                                         |**
**|                            ****                                         |**
**|                            ******o***                                   |**
**|                      ********_///_****                                  |**
**|                      ***** /_//_/ ****                                  |**
**|                       ** ** (__/ ****                                   |**
**|                           *********                                     |**
**|                            ****                                         |**
**|                            ***                                          |**
**|                                                                         |**
**|         Copyright (c) 2008-2010 Texas Instruments Incorporated          |**
**|                        ALL RIGHTS RESERVED                              |**
**|                                                                         |**
**| Permission is herbiy granted to licensees of Texas Instruments          |**
**| Incorporated (TI) products to use this computer program for the sole    |**
**| purpose of implementing a licensee product based on TI products.        |**
**| No other rights to reproduce, use, or disseminate this computer         |**
**| program, whether in part or in whole, are granted.                      |**
**|                                                                         |**
**| TI makes no representation or warranties with respect to the            |**
**| performance of this computer program, and specifically disclaims        |**
**| any responsibility for any damages, special or consequential,           |**
**| connected with the use of this program.                                 |**
**|                                                                         |**
**+-------------------------------------------------------------------------+**
******************************************************************************/

/**
 * \file     mcaspSampleMaster_main.c
 *
 * \brief    sample application for demostrating Mcasp evm to EVM communication
 *
 *           This file contains the initialisation routines for the mcasp
 *
 *           (C) Copyright 2008, Texas Instruments, Inc
 */

/* ========================================================================== */
/*                            INCLUDE FILES                                   */
/* ========================================================================== */

#include <stdio.h>
#include <std.h>
#include <log.h>
#include <pwrm.h>
#include <pwrmL138.h>
#include "ti/pspiom/mcasp/Mcasp.h"
#include "ti/pspiom/cslr/soc_OMAPL138.h"
#include "ti/pspiom/cslr/cslr_syscfg0_OMAPL138.h"

/* ========================================================================== */
/*                           MACRO DEFINTIONS                                 */
/* ========================================================================== */

/* KICK0 and KICK1 registers for lockin unlocking the pinmux register write   */
#define KICK0_REGISTER      (*(volatile Uint32 *)0x01C14038u)
#define KICK1_REGISTER      (*(volatile Uint32 *)0x01C1403Cu)

#define KICK0_ENABLE_KEY    0x83E70B13u
#define KICK1_ENABLE_KEY    0x95a4f1e0u

/**
 * Mcasp device params. To be filled in userMcaspInit function which
 * is called before driver creation
 */
Mcasp_Params mcaspParams;


extern LOG_Obj trace;

/* ========================================================================== */
/*                           FUNCTION DEFINITIONS                             */
/* ========================================================================== */

/**
 * \brief  Void main(Void)
 *
 *         Main function of the sample application. This function enables
 *         the mcasp instance in the power sleep controller and also
 *         enables the pinmux for the mcasp 1 instance.
 *
 * \param  None
 *
 * \return None
 */
Void main(Void)
{
    LOG_printf(&trace,"Mcasp EVM-EVM Sample application.");

    return;
}


/**
 * \brief   Mcasp init function called when creating the driver.
 *
 * \param  None
 *
 * \return None
 */
Void mcaspUserInit(Void)
{
    CSL_SyscfgRegsOvly syscfgRegs = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;

    /*Enable write access to PINMUX and CFG registers in KICK0R and KICK1R    */
    KICK0_REGISTER = KICK0_ENABLE_KEY;
    KICK1_REGISTER = KICK1_ENABLE_KEY;

    /* write to the pinmux registers to enable the mcasp0                     */
    syscfgRegs->PINMUX0  = 0x01111111;
    syscfgRegs->PINMUX1  = 0x11111111;
    syscfgRegs->PINMUX2  = 0x11111111;
    
    Mcasp_init();
    
    mcaspParams = Mcasp_PARAMS;
    mcaspParams.hwiNumber = 8;

}

/* ========================================================================== */
/*                                END OF FILE                                 */
/* ========================================================================== */
