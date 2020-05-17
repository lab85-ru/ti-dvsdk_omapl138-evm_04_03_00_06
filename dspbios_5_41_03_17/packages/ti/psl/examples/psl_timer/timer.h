/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *  @(#) PSL 5,1,0 09-24-2007 (psl-c03)
 */
#include "PSL.h"    // Power Scaling Library API

extern unsigned timer0Cnt;

extern void StopTimer0(unsigned     count,
                       PSL_ClkID    *clks,
                       PSL_Setpoint *currentSetpoints, 
                       PSL_Setpoint *newSetpoints);
                

extern void RestartTimer0(unsigned     count,
                          PSL_ClkID    *clks,
                          PSL_Setpoint *oldSetpoints, 
                          PSL_Setpoint *currentSetpoints);
                          
extern void SetupTimer0();

extern void CloseTimer0();

