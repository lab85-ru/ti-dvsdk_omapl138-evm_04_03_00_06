/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *  @(#) PSL 5,1,0 09-24-2007 (psl-c03)
 */
//-----------------------------------------------------------------------------
// PSLvolt_cfg_null.c
// 
// Source file containing variable definitions for null PSL
// voltage related configuration data.
//-----------------------------------------------------------------------------

#include "PSLvolt_cfg.h"

extern void PSL_voltRegInit_null(void);
extern void PSL_voltRegScale_null(float currVoltage,
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
const unsigned PSL_voltScalingSupported = 0;

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
const unsigned PSL_voltDecreaseLatency = 0;   // 0 millisecond latency on voltage drop
const unsigned PSL_voltIncreaseLatency = 0;   // 0 microsecond latency on voltage increase


//-----------------------------------------------------------------------------
// Pointer to function that performs any one time initialization that may
// be required before the voltage regulator can perform scaling operations.
//-----------------------------------------------------------------------------
PSL_VoltRegInitFunc  PSL_voltRegInitFunc = PSL_voltRegInit_null;


//-----------------------------------------------------------------------------
// Pointer to function that performs voltage scaling.
//-----------------------------------------------------------------------------
PSL_VoltRegScaleFunc PSL_voltRegScaleFunc = PSL_voltRegScale_null;
