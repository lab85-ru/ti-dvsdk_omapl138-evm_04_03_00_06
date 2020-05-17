/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== pscl.h ========
 *
 *  Power Scaling Library (PSCL) definitions for C6748.
 *
 */

#ifndef PSCL_
#define PSCL_

#include <pscl_clkcfg.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TRUE 
#define FALSE 0 
#define TRUE  1 
#endif

typedef enum {
   PSCL_OK,
   PSCL_INVALID_CLK,
   PSCL_INVALID_FREQ,
   PSCL_INVALID_INITIAL_FREQ,
   PSCL_INVALID_INITIAL_VOLTAGE,
   PSCL_INVALID_SETPOINT,
   PSCL_INCOMPATIBLE_VOLTAGE,
   PSCL_INCOMPLETE_INITIALIZATION,
   PSCL_CANNOT_CHANGE_SETPOINT,
   PSCL_CANNOT_CHANGE_VOLTAGE,
   PSCL_NOT_INITIALIZED
} PSCL_Status;

typedef unsigned PSCL_Setpoint;

typedef void (* PSCL_PrologFunc)(PSCL_ClkID clk, 
    PSCL_Setpoint currentSetpoint, PSCL_Setpoint newSetpoint);

typedef void (* PSCL_EpilogFunc)(PSCL_ClkID clk, 
    PSCL_Setpoint oldSetpoint, PSCL_Setpoint newSetpoint);


/*
 *  ======== PSCL_initialize ========
 *
 *
 */
extern PSCL_Status PSCL_initialize(unsigned count, PSCL_ClkID *clks,
    unsigned  *initSetpoint, unsigned goToSetpoints);


/*
 *  ======== PSCL_getNumSetpoints ========
 *
 *
 */
extern PSCL_Status PSCL_getNumSetpoints(PSCL_ClkID clk, 
    unsigned * numSetpoints);


/*
 *  ======== PSCL_getSetpoint ========
 *
 *
 */
extern PSCL_Status PSCL_getSetpoint(PSCL_ClkID clk, PSCL_Setpoint *setpoint);


/*
 *  ======== PSCL_changeSetpoint ========
 *
 *
 */
extern PSCL_Status PSCL_changeSetpoint(PSCL_ClkID clk, 
    PSCL_Setpoint newSetpoint, int scaleVoltage, int waitForVoltScale,
    PSCL_PrologFunc prologFunc, PSCL_EpilogFunc epilogFunc);


/*
 *  ======== PSCL_querySetpoint ========
 *
 *
 */
extern PSCL_Status PSCL_querySetpoint(PSCL_ClkID clk, PSCL_Setpoint setpoint,
     unsigned *frequency, unsigned *voltage, unsigned *mode);


/*
 *  ======== PSCL_querySetpointFrequency ========
 *
 *
 */
extern PSCL_Status PSCL_querySetpointFrequency(PSCL_ClkID clk, 
    PSCL_Setpoint setpoint, unsigned * frequency);


/*
 *  ======== PSCL_querySetpointVoltage ========
 *
 *
 */
extern PSCL_Status PSCL_querySetpointVoltage(PSCL_ClkID clk, 
    PSCL_Setpoint setpoint, unsigned * voltage);
                                            
                                               
/*
 *  ======== PSCL_querySetpointMode ========
 *
 *
 */
extern PSCL_Status PSCL_querySetpointMode(PSCL_ClkID clk, 
    PSCL_Setpoint setpoint, unsigned * mode);
                                            

/*
 *  ======== PSCL_querySetpointTransition ========
 *
 *
 */
extern PSCL_Status PSCL_querySetpointTransition(PSCL_ClkID clk, 
    PSCL_Setpoint fromSetpoint, PSCL_Setpoint toSetpoint,
    unsigned * freqScalingLatency, unsigned * voltageScalingLatency);   
                               

/*
 *  ======== PSCL_getFrequency ========
 *
 *
 */
extern PSCL_Status PSCL_getFrequency(PSCL_ClkID clk, unsigned * frequency);


/*
 *  ======== PSCL_getMode ========
 *
 *
 */
extern PSCL_Status PSCL_getMode(PSCL_ClkID clk, unsigned * mode);

/*
 *  ======== PSCL_getVoltage ========
 *
 *
 */
extern PSCL_Status PSCL_getVoltage(unsigned * voltage);

/*
 *  ======== PSCL_willChangeVoltage ========
 *
 *
 */
extern PSCL_Status PSCL_queryWillChangeVoltage(PSCL_ClkID clk, PSCL_Setpoint
    setpoint, unsigned * change);

#ifdef __cplusplus
}
#endif

#endif /* PSCL_ */
