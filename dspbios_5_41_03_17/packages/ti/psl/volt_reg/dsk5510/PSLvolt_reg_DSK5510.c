/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *  @(#) PSL 5,1,0 09-24-2007 (psl-c03)
 */
//-----------------------------------------------------------------------------
// PSLvolt_reg_DSK5510.c
// 
// Source file containing PSL implementation for C5510 DSK PSL voltage
// regulator control functions.
//-----------------------------------------------------------------------------

#include "PSLvolt_cfg.h"

extern void PSL_gpioVoltScaleDelay(unsigned long);

extern void PSL_setDSKVcoreMonBit();      // C5510 DSK specific
extern void PSL_clearDSKVcoreMonBit();    // C5510 DSK specific

//-----------------------------------------------------------------------------
// PSL_gpioVoltRegInit_DSK5510().
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
void PSL_gpioVoltRegInit_DSK5510(void) {

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
// PSL_gpioVoltRegScale_DSK5510().
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
void PSL_gpioVoltRegScale_DSK5510(float currVoltage,
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
   // *** C5510 DSK SPECIFIC CODE ***
   //
   // On the C5510 DSK board, temporarily disable the power monitor's ability
   // to reset the DSP.  This will prevent the DSP from resetting if the 
   // DSK's "Power Good" signal goes low during transitions between different
   // voltages.  The monitoring of "Power Good" can be disabled by setting
   // bit 4 (VORE_MON bit) of the CPLD MISC register, which is at address 
   // 0x300006.
   //
   // Note: Since the address (0x300006) is greater than 16-bits, this
   // cannot be done in C code since the upper bits of a pointer are not
   // changed in the small memory model.  Therefore, this must be done
   // in assembly code.  For consistency, it will also be done in assembly
   // for the large memory model.
   //--------------------------------------------------------------------------
   PSL_setDSKVcoreMonBit();
   
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
   
   //--------------------------------------------------------------------------
   // *** C5510 DSK SPECIFIC CODE ***
   //
   // Enable the power monitor's ability to reset the DSP.  This will allow
   // the DSP to reset if the DSK's "Power Good" signal goes low during normal
   // operation. The monitoring of "Power Good" can be enabled by clearing
   // bit 4 (VCORE_MON bit) of the CPLD MISC register, which is at address 
   // 0x300006.
   //
   // Note: Since the address (0x300006) is greater than 16-bits, this
   // cannot be done in C code since the upper bits of a pointer are not
   // changed in the small memory model.  Therefore, this must be done
   // in assembly code.  For consistency, it will also be done in assembly
   // for the large memory model.
   //--------------------------------------------------------------------------
   PSL_clrDSKVcoreMonBit();
}

