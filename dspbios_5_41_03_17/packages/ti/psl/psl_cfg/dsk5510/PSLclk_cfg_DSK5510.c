/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *  @(#) PSL 5,1,0 09-24-2007 (psl-c03)
 */
//-----------------------------------------------------------------------------
// PSLclk_cfg_DSK5510.c
// 
// Source file containing 5510DSK specific variable definitions for PSL
// clock related configuration data.  
//-----------------------------------------------------------------------------


#include "PSLclk_cfg.h"

//-----------------------------------------------------------------------------
// Address of clock mode register in I/O space.
//-----------------------------------------------------------------------------
const unsigned PSL_clkmdRegAddr = 0x1C00;


//-----------------------------------------------------------------------------
// Input Clock (CLKIN) frequencies in MHz for PSL_CPU_CLK.
//-----------------------------------------------------------------------------
const float PSL_cpuInputFreq =  24.0f;  // 24 MHz input clock (CLKIN) frequency


//-----------------------------------------------------------------------------
// Max operating frequency for device in MHz that is being controlled
// by PSL_CPU_CLK (see device's data sheet)
//-----------------------------------------------------------------------------
const float PSL_cpuMaxFreq   = 200.0f;    // 200 MHz max operating frequency


//-----------------------------------------------------------------------------
// PSL_CPU_CLK clock frequencies.
//
// PLL Lock Latency is in microseconds.
//-----------------------------------------------------------------------------
const unsigned PSL_cpuFreqCnt = 16;  // number of frequencies must be <= 16.


PSL_CPUFreq PSL_cpuFreqTable[] = {
   {0, 0, 3, PSL_BYPASS, 0}, //  6 MHz (input freq / 4), bypass mode, latency
   {0, 0, 1, PSL_BYPASS, 0}, // 12 MHz (input freq / 2), bypass mode, latency
   {0, 0, 0, PSL_BYPASS, 0}, // 24 MHz (input freq / 1), bypass mode, latency
   
   { 2, 0, 0, PSL_LOCK, 18}, //  48 MHz (input freq * ( 2 / 1)), lock mode, latency
   { 5, 1, 0, PSL_LOCK, 60}, //  60 MHz (input freq * ( 5 / 2)), lock mode, latency
   { 3, 0, 0, PSL_LOCK, 22}, //  72 MHz (input freq * ( 3 / 1)), lock mode, latency
   { 7, 1, 0, PSL_LOCK, 80}, //  84 MHz (input freq * ( 7 / 2)), lock mode, latency
   { 4, 0, 0, PSL_LOCK, 25}, //  96 MHz (input freq * ( 4 / 1)), lock mode, latency
   { 9, 1, 0, PSL_LOCK, 100},// 108 MHz (input freq * ( 9 / 2)), lock mode, latency
   { 5, 0, 0, PSL_LOCK, 30}, // 120 MHz (input freq * ( 5 / 1)), lock mode, latency
   {11, 1, 0, PSL_LOCK, 117},// 132 MHz (input freq * (11 / 2)), lock mode, latency
   { 6, 0, 0, PSL_LOCK, 35}, // 144 MHz (input_freq * ( 6 / 1)), lock mode, latency
   {13, 1, 0, PSL_LOCK, 137},// 156 MHz (input freq * (13 / 2)), lock mode, latency
   { 7, 0, 0, PSL_LOCK, 38}, // 168 MHz (input_freq * ( 7 / 1)), lock mode, latency
   {15, 1, 0, PSL_LOCK, 154},// 180 MHz (input freq * (15 / 2)), lock mode, latency
   {25, 2, 0, PSL_LOCK, 376},// 200 MHz (input_freq * (25 / 3)), lock mode, latency
};


