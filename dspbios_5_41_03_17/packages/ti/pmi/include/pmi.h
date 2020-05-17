/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== pmi.h ========
 *
 *  Power Manager Interface (PMI) definitions for C6748.
 *
 */

#ifndef PMI_
#define PMI_

#include <pmi_defs6748.h>

#ifdef __cplusplus
extern "C" {
#endif

#define REG(x) (*((volatile unsigned *)(x)))

#define PMI_BYPASS      0x0
#define PMI_LOCK        0x1

typedef enum {
    PMI_OK,
    PMI_BUSY,
    PMI_FAIL
} PMI_Status;

/*
 *  ======== PMI_bypassPLL ========
 *  Bypass PLL.
 *
 */
PMI_Status PMI_bypassPLL(PMI_Pll pll);

/*
 *  ======== PMI_calcWaitBypass ========
 *  Calculate cycles to wait for PLL bypass.
 */
unsigned PMI_calcWaitBypass(PMI_Pll pll);

/*
 *  ======== PMI_configurePLL ========
 *  Configure PLL.
 *
 */
PMI_Status PMI_configurePLL(PMI_Pll pll, PMI_PllCfg * cfg);

/*
 *  ======== PMI_configureSources ========
 *  Configure clock sources for PLL.
 *
 */
PMI_Status PMI_configureSources(PMI_Pll pll, PMI_ClkSourceCfg * clkCfg);

/*
 *  ======== PMI_enablePLL ========
 *  Enable the PLL.
 *
 */
PMI_Status PMI_enablePLL(PMI_Pll pll);

/*
 *  ======== PMI_getVoltage ========
 *  Get the current core voltage from the PMIC.
 *
 */
PMI_Status PMI_getVoltage(unsigned * voltage);

/*
 *  ======== PMI_getFrequencyLatency ========
 *  Get estimate of latency to scale between two frequency configurations.
 *
 */
PMI_Status PMI_getFrequencyLatency(PMI_PllCfg * fromPllCfg,
    PMI_PllCfg * toPllCfg, PMI_ClkDivCfg * fromDivCfg,
    PMI_ClkDivCfg * toDivCfg, unsigned * latency);

/*
 *  ======== PMI_getModuleState ========
 *  Get a module's state.
 *
 */
PMI_Status PMI_getModuleState(unsigned pd, unsigned psc, unsigned lpsc, unsigned
    *state);

/*
 *  ======== PMI_getStatePLL ========
 *  Get a PLL's state info.
 *
 */
PMI_Status PMI_getStatePLL(PMI_Pll pll, PMI_PllState * state);

/*
 *  ======== PMI_getVoltageLatency ========
 *  Get estimate of latency to scale from one voltage to another.
 *
 */
PMI_Status PMI_getVoltageLatency(unsigned fromVolt, unsigned toVolt, 
    unsigned * latency);

/*
 *  ======== PMI_init ========
 *  Initializes PMI module.
 *
 */
PMI_Status PMI_init(void);

/*
 *  ======== PMI_initPLL ========
 *  Initializes a PLL for control.
 *
 */
PMI_Status PMI_initPLL(PMI_Pll pll);

/*
 *  ======== PMI_initVoltageControl ========
 *  Initializes PMI voltage control functionality.
 *
 */
PMI_Status PMI_initVoltageControl(void);

/*
 *  ======== PMI_lockPLL ========
 *  Lock PLL.
 *
 */
PMI_Status PMI_lockPLL(PMI_Pll pll, unsigned lockCycles);

/*
 *  ======== PMI_powerDownPLL ========
 *  Power down a PLL.
 *
 */
PMI_Status PMI_powerDownPLL(PMI_Pll pll);

/*
 *  ======== PMI_powerUpPLL ========
 *  Power up a PLL.
 *
 */
PMI_Status PMI_powerUpPLL(PMI_Pll pll, PMI_ClkSourceCfg * clkCfg);

/*
 *  ======== PMI_scaleFrequency ========
 *  Scale the frequency and/or dividers of a PLL.
 *
 */
PMI_Status PMI_scaleFrequency(PMI_Pll pll, PMI_PllCfg * pllCfg, 
    PMI_ClkDivCfg * divCfg, PMI_ClkSourceCfg * clkCfg);

/*
 *  ======== PMI_setVoltage ========
 *  Scale the core voltage.
 *
 */
PMI_Status PMI_setVoltage(unsigned newVoltage, unsigned waitForScale);

/*
 *  ======== PMI_setDividersPLL ========
 *  Set PLL clock divider registers.
 *
 */
PMI_Status PMI_setDividersPLL(PMI_Pll pll, PMI_ClkDivCfg * cfg);

/*
 *  ======== PMI_setModuleState ========
 *  Set a module's state.
 *
 */
PMI_Status PMI_setModuleState(unsigned pd, unsigned psc, unsigned lpsc,
    unsigned state);

/*
 *  ======== PMI_sleepCPU ========
 *  Put the CPU in a sleep state.
 *
 */
PMI_Status PMI_sleepCPU(PMI_Sleep state, unsigned scaleVoltage, 
    unsigned sleepArg);

/*
 *  ======== PMI_waitCpuCycles ========
 *  Spin wait for a number of CPU cycles.
 */
void PMI_waitCpuCycles(unsigned cycles);


#ifdef __cplusplus
}
#endif

#endif /* PMI_ */
