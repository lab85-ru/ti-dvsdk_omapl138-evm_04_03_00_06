/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *  @(#) PSL 5,1,0 09-24-2007 (psl-c03)
 */
//-----------------------------------------------------------------------------
// PSLvolt_reg_evm5509a.c
// 
// Source file containing PSL implementation for evm5509a PSL voltage
// regulator control functions.
//-----------------------------------------------------------------------------

#include "PSLvolt_cfg.h"

extern void PSL_gpioVoltScaleDelay(unsigned long);

//-----------------------------------------------------------------------------
// PSL_gpioVoltRegInit_evm5509a().
//
// Description:
//    Function that performs any one time initialization that may be
//    required before the voltage regulator can perform scaling operations.
//    This function is used with a voltage regulator that is controlled
//    by the GPIO pins. 
//
// Parameters: None
//
// Return Value: None
//   
//-----------------------------------------------------------------------------
void PSL_gpioVoltRegInit_evm5509a(void) {

   volatile unsigned *iodir_reg;
   unsigned iodir_val;
   
   volatile unsigned *iodata_reg;
   unsigned iodata_val;
   
   //--------------------------------------------------------------------------
   // Setting a bit in the IODIR register causes the corresponding bit in the
   // IODATA register to be cleared.  This will only occur if the IODATA
   // register is NOT written prior to writing the IODIR register after
   // reset.  Therefore, read the value of IODATA and write it back prior
   // to setting up the IODIR register. 
   //--------------------------------------------------------------------------
   iodata_reg = (unsigned *)PSL_gpioIodataAddr;
   iodata_val = *((ioport unsigned*)(iodata_reg));
   *((ioport unsigned*)(iodata_reg)) = iodata_val;
   
   //--------------------------------------------------------------------------
   // Configure the GPIO IODIR register so that the bit(s) which will be used
   // to control the voltage regulator are configured as output bits.  Outputs
   // bits are specified by setting the corresponding bit to a 1. 
   //--------------------------------------------------------------------------
   iodir_reg = (unsigned *)PSL_gpioIodirAddr;
   iodir_val = *((ioport unsigned*)(iodir_reg));
   *((ioport unsigned*)(iodir_reg)) = iodir_val | PSL_gpioPinsMask;
}


//-----------------------------------------------------------------------------
// PSL_gpioVoltRegScale_evm5509a().
//
// Description:
//    Function that scales the voltage to the specified voltage.  If 
//    wait is TRUE then wait until the new voltage has been reached.
//    This function is used with a voltage regulator that is controlled
//    by the GPIO pins.
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
//    wait - TRUE if this routine should wait for the new voltage to reach
//           the regulation point.  FALSE otherwise.
//
// Return Value: None
//   
//-----------------------------------------------------------------------------
void PSL_gpioVoltRegScale_evm5509a(float currVoltage,
                                  float newVoltage,
                                  float currFrequency,
                                  int   wait) {
   
   int i;
   unsigned long delay;
   
   volatile unsigned *iodata_reg;
   unsigned iodata_val;
   
   unsigned gpioSetting = 0;
   
   //--------------------------------------------------------------------------
   // If this routine should wait for the new voltage to be reached, then
   // determine the number of cycles that the delay loop should execute.
   // A delay loop must be implemented here since the voltage regulator
   // provides no notification mechanism in this case.
   //--------------------------------------------------------------------------
   if (wait) {
      delay = (newVoltage < currVoltage) ? PSL_voltDecreaseLatency : PSL_voltIncreaseLatency;
      delay = delay * currFrequency;  // convert from microseconds to cycles
   }
                          
   //--------------------------------------------------------------------------
   // Initiate the voltage scaling operation by setting the correct bits in the
   // GPIO IODATA register.  The correct setting of the GPIO bits depends on
   // the newVoltage.  
   //--------------------------------------------------------------------------
   iodata_reg = (unsigned *)PSL_gpioIodataAddr;
   iodata_val = *((ioport unsigned*)(iodata_reg));
   
   //--------------------------------------------------------------------------
   // Determine the correct GPIO setting for the new voltage.
   //--------------------------------------------------------------------------
   for (i = 0; i < PSL_voltCnt; i++) {
      if (PSL_gpioVoltRegTable[i].volt == newVoltage) {
         gpioSetting = PSL_gpioVoltRegTable[i].gpioMask;
         break;
      }
   }
      
   //--------------------------------------------------------------------------
   // Initiate the voltage scaling.
   //--------------------------------------------------------------------------
   *((ioport unsigned*)(iodata_reg)) = (iodata_val & ~PSL_gpioPinsMask) | gpioSetting;
   
   //--------------------------------------------------------------------------
   // If necessary, wait for the new voltage to be reached.
   //--------------------------------------------------------------------------                                  
   if (wait) {
     PSL_gpioVoltScaleDelay(delay);
   }

}

