/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *  @(#) PSL 5,1,0 09-24-2007 (psl-c03)
 */
//-----------------------------------------------------------------------------
// PSLvolt_reg_null.c
// 
// Source file containing PSL implementation for NULL voltage
// regulator control functions.
//-----------------------------------------------------------------------------

#include "PSLvolt_cfg.h"

//-----------------------------------------------------------------------------
// PSL_voltRegInit_null().
//
// Description:
//    Function that does nothing, but serves as a place holder for boards
//    that have no voltage regulator to configure.  This is used to define
//    the implementation in PSLvolt_cfg.h
//
// Parameters: None
//
// Return Value: None
//   
//-----------------------------------------------------------------------------
void PSL_voltRegInit_null(void) {

}


//-----------------------------------------------------------------------------
// PSL_voltRegScale_null().
//
// Description:
//    Function that does nothing, but serves as a place holder for boards
//    that have no voltage regulator to configure.  This is used to define
//    the implementation in PSLvolt_cfg.h
//
//    Note:  None of the parameters matter, but must be there to be consistant
//           with the declaration in the configuration file.
//
// Parameters:
//    currVoltage - The current voltage.
//
//    newVoltage - The new voltage.
//
//    currFrequency - The current clock frequency of the device that
//                    is executing this routine.  The frequency is 
//                    needed to implement a delay loop in cases where
//                    wait is TRUE and the voltage regulator provides no
//                    notification as to when the new voltage has been
//                    reached.
//
//    wait - TRUE or FALSE.
//
// Return Value: None
//   
//-----------------------------------------------------------------------------
void PSL_voltRegScale_null(float currVoltage,
                           float newVoltage,
                           float currFrequency,
                           int   wait) {
   
}

