/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== pmi_voltnull.c ========
 *
 *  Null PMI voltage control functions.
 *
 */

#include <pmi.h>

/* estimated max latencies (in usec) for voltage up/down ramping */
const unsigned _PMI_maxVoltageUpLatency = 0;
const unsigned _PMI_maxVoltageDownLatency = 0;

/* max PLL lock latency: when PREDIV=1, MULT=17, w/24MHz input */
const unsigned _PMI_maxLockLatency = 21;

#define DEFAULT_VOLTAGE 1200    /* fixed voltage, in millivolts */

#pragma CODE_SECTION(PMI_getVoltage, ".pmonchip:slp");
#pragma CODE_SECTION(PMI_setVoltage, ".pmonchip:slp");


/*
 *  ======== PMI_getVoltage ========
 */
PMI_Status PMI_getVoltage(unsigned * voltage) 
{
    *voltage = 1200;
    return (PMI_OK);
}


/*
 *  ======== PMI_getVoltageLatency ========
 */
PMI_Status PMI_getVoltageLatency(unsigned fromVolt, unsigned toVolt,
    unsigned * latency) 
{
    *latency = 0;
    return (PMI_OK);
}


/*
 *  ======== PMI_initVoltageControl ========
 */
PMI_Status PMI_initVoltageControl(void)
{
    return (PMI_OK);
}


/*
 *  ======== PMI_setVoltage ========
 */
PMI_Status PMI_setVoltage(unsigned voltage, unsigned wait) 
{
    return (PMI_OK);
}
