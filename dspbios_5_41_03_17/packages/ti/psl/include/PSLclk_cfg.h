/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *  @(#) PSL 5,1,0 09-24-2007 (psl-c03)
 */
//-----------------------------------------------------------------------------
// PSLclk_cfg.h
// 
// Header file containing type and variable declarations for PSL
// clock related configuration data.
//-----------------------------------------------------------------------------

#ifndef _PSLCLK_CFG_H
#define _PSLCLK_CFG_H

#define CLK_CNT 1    // one clock is controlled by PSL


//-----------------------------------------------------------------------------
// Address of clock mode register in I/O space.
//-----------------------------------------------------------------------------
extern const unsigned PSL_clkmdRegAddr;


//-----------------------------------------------------------------------------
// Enumeration type defining the clocks that are controlled by the PSL.
//-----------------------------------------------------------------------------
typedef enum {
   PSL_CPU_CLK = 0
} PSL_ClkID;


//-----------------------------------------------------------------------------
// Input Clock (CLKIN) frequencies in MHz for PSL_CPU_CLK.
//-----------------------------------------------------------------------------
extern const float PSL_cpuInputFreq;


//-----------------------------------------------------------------------------
// Max operating frequency for device in MHz that is being controlled
// by PSL_CPU_CLK (see device's data sheet)
//-----------------------------------------------------------------------------
extern const float PSL_cpuMaxFreq;


//-----------------------------------------------------------------------------
// Enumeration type defining possible clock modes.
//-----------------------------------------------------------------------------
typedef enum {    // clock's operating mode
   PSL_BYPASS, 
   PSL_LOCK
} PSL_ClkMode;


//-----------------------------------------------------------------------------
// Clock frequency structure for PSL_CPU_CLK.
//
// For bypass mode frequencies, the output frequency is calculated as:
//
//    output frequency = input frequency / (bypass_div + 1)
//
// The values of PLL_mult and PLL_div should be 0 for bypass mode frequencies.
//
// For lock mode frequencies, the output frequency is calculated as:
//
//    output frequency = input frequency * (PLL_mult / (PLL_div + 1))
//
// The value of bypass_div should be 0 for lock mode frequencies.
//
// PLL Lock Latency is calculated as follows:
//     latency =  4 * ( PLL DIV + 1 ) * [10D + 30 (PLL MULT + 1 ) + 2]
//     where D = 1 + log2(200 / (PLL MULT * Output frequency)) rounded to the 
//                                                             nearest integer.
//-----------------------------------------------------------------------------
typedef struct {
    unsigned    PLL_mult;
    unsigned    PLL_div;
    unsigned    bypass_div;
    PSL_ClkMode mode;
    unsigned    lockLatency;
} PSL_CPUFreq;


//-----------------------------------------------------------------------------
// PSL_CPU_CLK clock frequencies.
//-----------------------------------------------------------------------------
extern const unsigned PSL_cpuFreqCnt;  // number of frequencies in PSL_cpuFreqTable[], 
                                       // must be <= 16.

extern PSL_CPUFreq PSL_cpuFreqTable[]; // table of CPU clock frequencies


#endif   // #ifndef _PSLCLK_CFG_H
