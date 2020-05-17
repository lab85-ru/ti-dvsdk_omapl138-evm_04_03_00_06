/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *  @(#) PSL 5,1,0 09-24-2007 (psl-c03)
 */
//-----------------------------------------------------------------------------
// PSLvolt_cfg.h
// 
// Header file containing type and variable declarations for PSL
// voltage related configuration data.
//-----------------------------------------------------------------------------

#ifndef _PSLVOLT_CFG_H
#define _PSLVOLT_CFG_H

#include "PSLclk_cfg.h"

//-----------------------------------------------------------------------------
// Voltage Scaling Supported indicator
//   1 - Voltage Scaling is supported
//   0 - Voltage Scaling is not supported.
//
//   NOTE:  The PSL_VoltTable still needs to be filled so that the initial
//          voltage can be verified correct.
//-----------------------------------------------------------------------------
extern const unsigned PSL_voltScalingSupported;

//-----------------------------------------------------------------------------
// Number of voltage points supported by the voltage regulator.  The number of
// entries in each of the data tables listed below is equal to this number.
//-----------------------------------------------------------------------------
extern const unsigned PSL_voltCnt;

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
typedef struct {
   float volt;  // voltage 
   float freq;  // frequency for PSL_CPU_CLK
} PSL_VoltTable;


//-----------------------------------------------------------------------------
// The voltage table contains the various voltage settings that may be used to
// power the core.  Associated with each voltage are the maximum clock 
// frequencies that can operate at the specified voltage.
//
// The voltage settings and associated frequencies can be obtained from the
// device's data sheet.
//-----------------------------------------------------------------------------
extern PSL_VoltTable PSL_voltTable[];


//-----------------------------------------------------------------------------
// Maximum latencies incurred during voltage scaling operations.   The latency
// is given in microseconds.  Following the initiation of a voltage scaling
// operation, the latency is the time required before the new voltage has been
// reached.
//-----------------------------------------------------------------------------
extern const unsigned PSL_voltDecreaseLatency;
extern const unsigned PSL_voltIncreaseLatency;


//-----------------------------------------------------------------------------
// Type definitions for voltage regulator functions.  Users may supply functions
// of these types to override the default voltage regulator functionality that
// is provided in PSL.lib, and which uses the GPIO pins.
//-----------------------------------------------------------------------------
typedef void (* PSL_VoltRegInitFunc)(void);

typedef void (* PSL_VoltRegScaleFunc)(float currVoltage, 
                                      float newVoltage,
                                      float currFrequency, 
                                      int   wait);
                                      
//-----------------------------------------------------------------------------
// Pointer to function that performs any one time initialization that may
// be required before the voltage regulator can perform scaling operations.
//-----------------------------------------------------------------------------
extern PSL_VoltRegInitFunc  PSL_voltRegInitFunc;


//-----------------------------------------------------------------------------
// Pointer to function that performs voltage scaling.
//-----------------------------------------------------------------------------
extern PSL_VoltRegScaleFunc PSL_voltRegScaleFunc;


//-----------------------------------------------------------------------------
// Comment out the following #define if the default voltage regulator control
// implementation, which uses GPIO pins to control the voltage regulator,  is
// NOT being used.
//
//-----------------------------------------------------------------------------
#ifndef _PSL_NULL_VOLT_REG_
#define USING_DEFAULT_VOLT_REG_CONTROL 1
#endif // _PSL_NULL_VOLT_REG_



#ifdef USING_DEFAULT_VOLT_REG_CONTROL

//-----------------------------------------------------------------------------
// PSL_GpioVoltRegTable structure.
//
// 
// Each entry in the PSL_GpioVoltRegTable structure contains both a voltage and
// a corresponding bit mask that specifies the setting of the GPIO bits for that
// voltage.
//
// The voltage is specified in Volt units.
//-----------------------------------------------------------------------------
typedef struct {
   float    volt;       // voltage 
   unsigned gpioMask;   // setting of GPIO bits for corresponding voltage
} PSL_GpioVoltRegTable;


//-----------------------------------------------------------------------------
// Addresses of GPIO IODIR and IODATA registers in I/O space.
//-----------------------------------------------------------------------------
extern const unsigned PSL_gpioIodirAddr;
extern const unsigned PSL_gpioIodataAddr;


//-----------------------------------------------------------------------------
// Mask that specifies which GPIO pin(s) is/are used to control the voltage
// regulator.
//
// The mask is used by the default voltage regulator control implementation,
// which uses the GPIO pins to control the regulator.  
//-----------------------------------------------------------------------------
extern const unsigned PSL_gpioPinsMask;


//-----------------------------------------------------------------------------
// The GPIO voltage regulator table contains the GPIO bit settings for each
// voltage that is supported by the regulator.
//
// This table is used by the default voltage regulator control implementation,
// which uses the GPIO pins to control the regulator. 
//-----------------------------------------------------------------------------
extern PSL_GpioVoltRegTable PSL_gpioVoltRegTable[];

#endif   // USING_DEFAULT_VOLT_REG_CONTROL


#endif   // #ifndef _PSLVOLT_CFG_H
