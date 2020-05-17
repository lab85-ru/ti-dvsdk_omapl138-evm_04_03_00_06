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
#include <ti/pspiom/psc/Psc.h>
#include <ti/pspiom/mcasp/Mcasp.h>

/* ========================================================================== */
/*                           MACRO DEFINTIONS                                 */
/* ========================================================================== */

/* KICK0 and KICK1 registers for lockin unlocking the pinmux register write   */
#define KICK0_REGISTER      (*(volatile Uint32 *)0x01C14038u)
#define KICK1_REGISTER      (*(volatile Uint32 *)0x01C1403Cu)

/* pin mux registers used by the mcasp and i2c devices                        */
#define PINMUX8             (*((volatile Uint32 *)0x01C14140u))
#define PINMUX11            (*((volatile Uint32 *)0x01C1414Cu))
#define PINMUX12            (*((volatile Uint32 *)0x01C14150u))
#define PINMUX13            (*((volatile Uint32 *)0x01C14154u))

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
    /* Configure the pinmux settings                                          */
    /*Enable write access to PINMUX and CFG registers in KICK0R and KICK1R    */
    KICK0_REGISTER = KICK0_ENABLE_KEY;
    KICK1_REGISTER = KICK1_ENABLE_KEY;
    
    /* write to the pinmux registers to enable the mcasp1 and i2c0            */
    PINMUX8  = 0x01122000u;  /* Mcasp1 and I2c0 */
    PINMUX11 = 0x11100000u;  /* Mcasp1          */
    PINMUX12 = 0x11111111u;  /* Mcasp1          */
    PINMUX13 = 0x00111111u;  /* Mcasp1          */
    
    Mcasp_init();
    
    mcaspParams = Mcasp_PARAMS;
    mcaspParams.hwiNumber = 8;

}

/* ========================================================================== */
/*                                END OF FILE                                 */
/* ========================================================================== */
