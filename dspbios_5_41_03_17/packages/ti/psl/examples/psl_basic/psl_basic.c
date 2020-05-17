/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *  @(#) PSL 5,1,0 09-24-2007 (psl-c03)
 */

//----------------------------------------------------------------------------
// psl_basic
//
// This example shows how to use some of the basic functionality that
// is provided by the PSL.  It shows:
//
//   - how to initialize the PSL
//   - how to change the frequency and voltage using the PSL.
//   - how to determine the current frequency and voltage settings.
//
//----------------------------------------------------------------------------

#include <stdio.h>
#include "PSL.h"

extern void func1();
extern void func2();
extern void func3();

// Variable specifying PSL_ClkID that will be used in all PSL calls.
PSL_ClkID clk = PSL_CPU_CLK;
   

// Print Error status
void printStatus(PSL_Status status) {
   printf("Error is ");
   
   switch (status) {
   case PSL_INVALID_CLK:
      printf("PSL_INVALID_CLK");
      break;
      
   case PSL_INVALID_FREQ:
      printf("PSL_INVALID_FREQ");
      break;
      
   case PSL_INVALID_INITIAL_FREQ:
      printf("PSL_INVALID_INITIAL_FREQ");
      break;
      
   case PSL_INVALID_INITIAL_VOLTAGE:
      printf("PSL_INVALID_INITIAL_VOLTAGE");
      break;
      
   case PSL_INVALID_SETPOINT:
      printf("PSL_INVALID_SETPOINT");
      break;
      
   case PSL_MAX_FREQ_EXCEEDED:
      printf("PSL_MAX_FREQ_EXCEEDED");
      break;
      
   case PSL_MAX_VOLTAGE_EXCEEDED:
      printf("PSL_MAX_VOLTAGE_EXCEEDED");
      break;
      
   case PSL_INCOMPATIBLE_VOLTAGE:
      printf("PSL_INCOMPATIBLE_VOLTAGE");
      break;
      
   case PSL_INCOMPLETE_INITIALIZATION:
      printf("PSL_INCOMPLETE_INITIALIZATION");
      break;
      
   case PSL_CANNOT_CHANGE_SETPOINT:
      printf("PSL_CANNOT_CHANGE_SETPOINT");
      break;
      
   default:
      printf("Unknown error");
      break;
   }
   
   printf("\n");
}


// Print current frequency and voltage settings.
void printCurrSettings() {
   float currFreq;

#ifndef _PSL_NULL_VOLT_
   float currVolt;
#endif // _PSL_NULL_VOLT_
   
   PSL_getFrequencies(1, &clk, &currFreq);

#ifndef _PSL_NULL_VOLT_
   currVolt = PSL_getVoltage();
   printf("Current frequency: %f MHz, Current voltage: %f volts\n\n",
           currFreq,
           currVolt);
#else
   printf("Current frequency: %f MHz\n\n",
           currFreq);
#endif // _PSL_NULL_VOLT_
}
 
           
void main (void)

{
   PSL_Status status;
    
   // Index into PSL_cpuFreqTable[] that specifies initial freq of 200 MHz.
   unsigned initFreqIndex = 15; 
   
   // PSL_cpuFreqTable[5] represents 72 MHz
   // PSL_cpuFreqTable[15] represent 200 MHz.
   PSL_Setpoint _72MHzSetpoint = 5;
   PSL_Setpoint _200MHzSetpoint = 15;  
   
   // Initialize the PSL.  The frequency following reset is
   // specified by PSL_cpuFreqTable[15].  The voltage following
   // reset is 1.6v.
   status = PSL_initialize(1, &clk, &initFreqIndex, 1.6f);

   if (status != PSL_OK)
   {
      // handle error;
      printStatus(status);
      return;
   }
   
   
   // Execute func1 at 200 MHz (i.e. the initial frequency);
   printf("Executing func1.\n");
   printCurrSettings(); 
   func1();

   //Change frequency to 72 MHz.
   status = PSL_changeSetpoints(1, 
                                &clk, 
                                &_72MHzSetpoint, 
                                TRUE,     // change voltage also
                                FALSE, 
                                NULL, NULL);
   if (status != PSL_OK) {
      // handle error
      printStatus(status);
      return;
   }

   // Execute func2 at 72 MHz;
   printf("Executing func2.\n");
   printCurrSettings();
   func2();

   // Change frequency back to 200 MHz.
   status = PSL_changeSetpoints(1, 
                                &clk, 
                                &_200MHzSetpoint, 
                                TRUE,     // change voltage also
                                FALSE, 
                                NULL, NULL);

   if (status != PSL_OK) {
      // handle error
      printStatus(status);
      return;
   }

   // Execute func3 at 200 MHz;
   printf("Executing func3.\n");
   printCurrSettings();
   func3();
}
