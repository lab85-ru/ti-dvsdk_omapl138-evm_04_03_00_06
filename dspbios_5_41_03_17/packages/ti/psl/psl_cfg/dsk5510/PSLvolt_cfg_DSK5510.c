/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *  @(#) PSL 5,1,0 09-24-2007 (psl-c03)
 */
//-----------------------------------------------------------------------------
// PSLvolt_cfg_DSK5510.c
// 
// Source file containing 5510DSK specific variable definitions for PSL
// voltage related configuration data.
//-----------------------------------------------------------------------------

#include "PSLvolt_cfg.h"

extern void PSL_gpioVoltRegInit_DSK5510(void);
extern void PSL_gpioVoltRegScale_DSK5510(float currVoltage,
                                         float newVoltage,
                                         float currFrequency,
                                         int   wait);

//-----------------------------------------------------------------------------
// Voltage Scaling Supported indicator
//   1 - Voltage Scaling is supported
//   0 - Voltage Scaling is not supported.
//
//   NOTE:  The PSL_VoltTable still needs to be filled so that the initial
//          voltage can be verified correct.
//-----------------------------------------------------------------------------
const unsigned PSL_voltScalingSupported = 1;

//-----------------------------------------------------------------------------
// Number of voltage points supported by the voltage regulator.  The number of
// entries in each of the data tables listed below is equal to this number.
//-----------------------------------------------------------------------------
const unsigned PSL_voltCnt = 2;


//-----------------------------------------------------------------------------
// PSL_VoltTable structure.
//
// 
// Each entry in the PSL_VoltTable structure contains both a voltage and
// a set of clock frequencies.   The clock frequencies specify the maximum
// frequencies that can be sustained by the specified voltage.  The entries
// must be ordered such that the voltages are listed in increasing order.
//
// The voltage is specified in Volt units.  The frequencies are specified in
// MHz units.
//-----------------------------------------------------------------------------
PSL_VoltTable PSL_voltTable[] = {
  {1.1f, 72.0f},   // 0 MHz up to, and including 72 MHz, require a 
                   // minimum voltage of 1.1v

 
  {1.6f, 200.0f},  // frequencies > 72 MHz up to the max frequency 
                   // require a minimum voltage of 1.6v
}; 


//-----------------------------------------------------------------------------
// Maximum latencies incurred during voltage scaling operations.   The latency
// is given in microseconds.  Following the initiation of a voltage scaling
// operation, the latency is the time required before the new voltage has been
// reached.
//-----------------------------------------------------------------------------
const unsigned PSL_voltDecreaseLatency = 3300;   // 3.3 millisecond latency on voltage drop
const unsigned PSL_voltIncreaseLatency = 300;    // 300 microsecond latency on voltage increase


//-----------------------------------------------------------------------------
// Pointer to function that performs any one time initialization that may
// be required before the voltage regulator can perform scaling operations.
//
// If the default implementation, which uses the GPIO pins to control the
// regulator, is NOT being used, then the initialization routine should
// be changed to the correct user supplied initialization function.
//
// Note that the GPIO implementation for the C5510 DSK contains some
// functionality that is specific to that board.  Use the initialization
// routine PSL_gpioVoltRegInit, whose source code is delivered with the PSL,
// for a board generic GPIO implementation that will work with custom C5510
// boards.
//-----------------------------------------------------------------------------
PSL_VoltRegInitFunc  PSL_voltRegInitFunc = PSL_gpioVoltRegInit_DSK5510;


//-----------------------------------------------------------------------------
// Pointer to function that performs voltage scaling.
//
// If the default implementation, which uses the GPIO pins to control the
// regulator, is NOT being used, then the scaling routine should be changed
// to the correct user supplied scaling function.
//
// Note that the GPIO implementation for the C5510 DSK contains some
// functionality that is specific to that board.  Use the voltage scaling
// routine PSL_gpioVoltRegScale, whose source code is delivered with the PSL,
// for a board generic GPIO implementation that will work with custom C5510
// boards.
//-----------------------------------------------------------------------------
PSL_VoltRegScaleFunc PSL_voltRegScaleFunc = PSL_gpioVoltRegScale_DSK5510;


#ifdef USING_DEFAULT_VOLT_REG_CONTROL

//-----------------------------------------------------------------------------
// Addresses of GPIO IODIR and IODATA registers in I/O space.
//-----------------------------------------------------------------------------
const unsigned PSL_gpioIodirAddr  = 0x3400;
const unsigned PSL_gpioIodataAddr = 0x3401;


//-----------------------------------------------------------------------------
// Mask that specifies which GPIO pin(s) is/are used to control the voltage
// regulator.
//
// The mask is used by the default voltage regulator control implementation,
// which uses the GPIO pins to control the regulator.  
//-----------------------------------------------------------------------------
const unsigned PSL_gpioPinsMask = 0x1;   // GPIO bit 0


//-----------------------------------------------------------------------------
// The GPIO voltage regulator table contains the GPIO bit settings for each
// voltage that is supported by the regulator.
//
// This table is used by the default voltage regulator control implementation,
// which uses the GPIO pins to control the regulator. 
//-----------------------------------------------------------------------------
PSL_GpioVoltRegTable PSL_gpioVoltRegTable[] = {
  {1.1f, 0x0},  // set GPIO bit 0 to a 0 for 1.1v

 
  {1.6f, 0x1},  // set GPIO bit 0 to a 1 for 1.6v
}; 

#endif   // USING_DEFAULT_VOLT_REG_CONTROL
