/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *  @(#) PSL 5,1,0 09-24-2007 (psl-c03)
 */
//-----------------------------------------------------------------------------
// PSL.h
// 
// Header file containing type and function declarations for PSL API.
//-----------------------------------------------------------------------------

#ifndef _PSL_H
#define _PSL_H

#include "PSLclk_cfg.h"

//-----------------------------------------------------------------------------
// Macro definitions for boolean constants.
//-----------------------------------------------------------------------------
#ifndef TRUE 
#define FALSE 0 
#define TRUE  1 
#endif


//-----------------------------------------------------------------------------
// Enumeration type defining possible return values for PSL functions.
//-----------------------------------------------------------------------------
typedef enum {
   PSL_OK,
   PSL_INVALID_CLK,
   PSL_INVALID_FREQ,
   PSL_INVALID_INITIAL_FREQ,
   PSL_INVALID_INITIAL_VOLTAGE,
   PSL_INVALID_SETPOINT,
   PSL_MAX_FREQ_EXCEEDED,
   PSL_MAX_VOLTAGE_EXCEEDED,
   PSL_INCOMPATIBLE_VOLTAGE,
   PSL_INCOMPLETE_INITIALIZATION,
   PSL_CANNOT_CHANGE_SETPOINT,
   PSL_NOT_INITIALIZED
} PSL_Status;


//-----------------------------------------------------------------------------
// Type definition for PSL_Setpoint.  Users of the PSL will refer to specific
// setpoints via this type.
//-----------------------------------------------------------------------------
typedef unsigned PSL_Setpoint;


//-----------------------------------------------------------------------------
// Type definitions for callback functions.  Functions of these types may be
// called before (prologue) and after (epilogue) scaling operations performed
// by the function PSL_changeSetpoints.
//-----------------------------------------------------------------------------
typedef void (* PSL_PrologueFunc)(unsigned     count,
                                  PSL_ClkID    *clks,
                                  PSL_Setpoint *currentSetpoints, 
                                  PSL_Setpoint *newSetpoints);

typedef void (* PSL_EpilogueFunc)(unsigned     count,
                                  PSL_ClkID    *clks,
                                  PSL_Setpoint *oldSetpoints, 
                                  PSL_Setpoint *currentSetpoints);


//-----------------------------------------------------------------------------
// PSL_intialize().
//
// Description:
//    Perform any initialization required by the power scaling library.
//
//    The initial clock frequency and operating mode for each clock are
//    determined according to the values specified in the initFrequencies
//    array.  The values in this array are indexes into a clock's associated
//    frequency table, which is part of the user configurable data located
//    in PSLclk_cfg.c.  This routine does not actually change the frequency
//    of any clock.  Instead, it assumes that a clock's initial frequency
//    is also the clock's current frequency.  An initial frequency MUST be 
//    supplied for every clock that is defined by the enum type PSL_ClkID, 
//    which is located in PSLclk_cfg.h.
//
//    The initial voltage is specified by initVoltage.  This routine does
//    not actually change the voltage.  Instead, it assumes that the initial
//    voltage is also the current voltage.  The initial voltage must match
//    one of the voltages specified in the voltage table located in
//    PSLvolt_cfg.c
//
//    The initial setpoint for each clock will specify the clock's initial
//    frequency and the minimum voltage required for that frequency.  
//
//    This routine should be called once during target initialization.
//    If this routine is called multiple times, all calls after the first
//    successful call will return PSL_OK.  In this case, no re-initialization
//    occurs and the current setpoints are not changed. If none of the 
//    previous calls were successful, subsequent calls will attempt 
//    initialization as described above.
//
// Parameters:
//    count   [in]
//       Specifies the number of clocks pointed to by clks.  The count MUST
//       specify the number of clocks defined by the enum type PSL_ClkID, 
//       which is located in PSLclk_cfg.h.
//
//    *clks   [in]
//       Pointer to locations that specify the clocks.  The number of clocks
//       referred to by the pointer should match the count.  Every clock that
//       is defined by the enum type PSL_ClkID, which is located in PSLclk_cfg.h,
//       MUST be present in the array. 
//
//    *initFrequencies   [in]
//       Pointer to locations that specify the initial frequency of each clock.
//       The values in this array are indexes into a clock's associated frequency
//       table, which is part of the user configurable data located in PSLclk_cfg.c.
//       The initial frequency for clks[0] is specified by initFrequencies[0],
//       the initial frequency for clks[1] is specified by initFrequencies[1],
//       etc.
//
//    initVoltage   [in]
//       The initial voltage.
//
// Return Value:
//    PSL_OK
//       if all initialization required for the correct operation
//       of the scaling library succeeds.  If initialization does not
//       succeed, all setpoints for all clocks are considered invalid.
//
//    PSL_INVALID_CLK
//       if any of the specified clocks are invalid.
//
//    PSL_INVALID_FREQ
//       if any of the clock frequencies tables are empty, there are
//       more than 16 entries in any of the frequecies tables, or any of
//       the values (mult, div, mode) for a specific clock are invalid.
//       The clock frequency tables are part of the user configurable data
//       located in PSLclk_cfg.c.
//
//    PSL_INVALID_INITIAL_FREQ
//       if any of the values specified in the initFrequencies array are
//       invalid indexes into the corresponding clock's frequency table.
//       The clock frequency tables are part of the user configurable data
//       located in PSLclk_cfg.c. 
//
//    PSL_MAX_FREQ_EXCEEDED
//       if any of the frequencies specified in a clock's frequency table
//       exceed the maximum operating frequency of the device that the clock
//       is controlling. The maximum frequencies are part of the user 
//       configurable data located in PSLclk_cfg.c.
//
//    PSL_INCOMPATIBLE_VOLTAGE
//       if the initial voltage as specified by initVoltage is less
//       than the voltage required by any of the initial setpoints.
//  
//    PSL_INVALID_INITIAL_VOLTAGE
//               if the intial voltage as specified by initVoltage is not one of
//       the voltages specified in the voltage table located in PSLvolt_cfg.c
//
//        PSL_MAX_VOLTAGE_EXCEEDED
//               if any of the values in the user configurable data located in 
//       PSLvolt_cfg.c are beyond the maximum supported voltage.
//
//    PSL_INCOMPLETE_INITIALIZATION
//       if an initial frequency is not supplied for every clock that is
//       defined by the enum type PSL_ClkID, which is located in PSLclk_cfg.h.
//
//-----------------------------------------------------------------------------
extern PSL_Status PSL_initialize(unsigned  count,
                                 PSL_ClkID *clks,
                                 unsigned  *initFrequencies,
                                 float     initVoltage);


//-----------------------------------------------------------------------------
// PSL_getNumSetpoints().
//
// Description:
//    Return the number of valid setpoints for the specified clocks.  If a clock
//    has n valid setpoints, the valid setpoints for that clock are those in the
//    range 0 .. n - 1.
//
//    No setpoint for any clock is considered valid until the power scaling library
//    has been successfully initialized.
//
// Parameters:
//    count   [in]
//       Specifies the number of clocks pointed to by clks.
//
//    *clks   [in]
//       Pointer to locations that specify the clocks.  The number of clocks
//       referred to by the pointer should match the count.
//
//    *numSetpoints   [out]
//       Pointer to locations to store the setpoint count for each of the
//       clocks referred to by the clks pointer.  The number of valid setpoints
//       for clks[0] will be returned in numSetpoints[0], the valid number of
//       setpoints for clks[1] will be returned in numSetpoints[1], etc.  
// 
// Return Value:
//    PSL_OK
//       if all of the specified clocks are valid.
//
//    PSL_INVALID_CLK
//       if any of the specified clocks are invalid.
//
//-----------------------------------------------------------------------------
extern PSL_Status PSL_getNumSetpoints(unsigned  count,
                                      PSL_ClkID *clks,
                                      unsigned  *numSetpoints);


//-----------------------------------------------------------------------------
// PSL_getSetpoints().
//
// Description:
//    Return the current setpoint for each of the specified clocks.
//
// Parameters:
//    count   [in]
//       Specifies the number of clocks pointed to by clks.
//
//    *clks   [in]
//       Pointer to locations that specify the clocks.  The number of clocks
//       referred to by the pointer should match the count.
//
//    *setpoints   [out]
//       Pointer to locations to store the current setpoint for each of
//       the clocks referred to by the clks pointer.  The current setpoint
//       for clks[0] will be returned in setpoints[0], the current setpoint
//       for clks[1] will be returned in setpoints[1], etc.  
//
// Return Value:
//    PSL_OK
//       if all of the specified clocks are valid.
//
//    PSL_INVALID_CLK
//       if any of the specified clocks are invalid.
//
//    PSL_NOT_INITIALIZED
//       if PSL has not been initialized.
//
//-----------------------------------------------------------------------------
extern PSL_Status PSL_getSetpoints(unsigned     count,
                                   PSL_ClkID    *clks,
                                   PSL_Setpoint *setpoints);


//-----------------------------------------------------------------------------
// PSL_changeSetpoints().
//
// Description:
//    For each of the specified clocks, initiate a scaling operation to the
//    new setpoint.  This includes setting the CPU clock frequency and clock
//    mode to those specified by the clock's new setpoint.  
//
//    If scaleVoltage is TRUE and the current voltage is not sufficient
//    for any of the new setpoints, then the voltage will be increased to
//    the lowest level that will support all the new setpoints.  In this
//    case, the new voltage will also be sufficient for any current setpoint
//    that is not being changed.  If a lower voltage is sufficient for all 
//    new setpoints as well as all current setpoints that are not being changed,
//    the voltage will be decreased to the lowest level that will support
//    all of these setpoints. 
//
//    This routine will not return until the clocks are generating the new 
//    frequencies specified by the setpoints.  If waitForVoltScale is TRUE 
//    and the voltage was actually scaled, then this routine will also wait
//    until the new voltage is reached.  In addition, if a voltage increase
//    was required as part of the setpoint changes, or if the device is in
//    an unstable state until the new voltage is reached, then this routine
//    will also wait for the voltage scaling to complete, regardless of 
//    waitForVoltScale.
//    
//    Prior to initiating any scaling operations, this routine will call the
//    function referenced by prologueFunc.  If prologueFunc is NULL, no function
//    is called.  Similarly, following the scaling operations, this routine will
//    call the function referred to by epilogueFunc. The call to epilogueFunc 
//    will not occur until the clocks are generating the new frequencies.
//    If this routine must wait for the new voltage to be reached, then the call
//    to epilogueFunc will not occur until the voltage has been reached. If
//    epilogueFunc is NULL, no function is called.
//
//
// Parameters:
//    count   [in]
//       Specifies the number of clocks pointed to by clks.
//
//    *clks   [in]
//       Pointer to locations that specify the clocks.  The number of clocks
//       referred to by the pointer should match the count.
//
//    *newSetpoints   [in]
//       Pointer to locations that specify the new setpoint for each of
//       the clocks referred to by the clks pointer.  The new setpoint
//       for clks[0] is specified by newSetpoints[0], the new setpoint
//       for clks[1] is specified by newSetpoints[1], etc.
//
//    scaleVoltage   [in]
//       TRUE if the voltage should be scaled when necessary.  FALSE if the
//       voltage should not be scaled.
//
//    waitForVoltScale   [in]
//       TRUE if this routine should wait for the new voltage to be reached
//       after initiating the voltage scaling.  FALSE if waiting is not required.
//       Note that this parameter is ignored if a voltage increase is required or
//       if the device is in an unstable state until the new voltage is reached.
//       In these cases, this routine will always wait for the voltage scaling
//       to complete.
//
//    prologueFunc   [in]
//       Function called prior to scaling operations.  NULL if no function is to
//       be called.
//
//    epilogueFunc   [in]
//       Function called after the scaling operations have completed.  NULL if 
//       no function is to be called.
//
// Return Value:
//    PSL_OK
//       if the setpoint changes were successful.
//
//    PSL_INVALID_CLK
//       if any of the specified clocks are invalid.
//
//    PSL_INVALID_SETPOINT
//       if any of the new setpoints are invalid.  A clock's valid setpoints are
//       those in the range 0 .. n - 1, where n is the number of valid setpoints
//       returned by PSL_getNumSetpoints().  No scaling operations are performed
//       if any of the setpoints are invalid.
//
//    PSL_INCOMPATIBLE_VOLTAGE
//       if scaleVoltage is FALSE and the current voltage is less than the
//       voltage required by any of the new setpoints.  No scaling operations
//       are performed in this case.
// 
//    PSL_CANNOT_CHANGE_SETPOINT
//       if the setpoint could not be changed.
//
//    PSL_NOT_INITIALIZED
//       if PSL has not been initialized.
//  
//-----------------------------------------------------------------------------
extern PSL_Status PSL_changeSetpoints(unsigned         count,
                                      PSL_ClkID        *clks,
                                      PSL_Setpoint     *newSetpoints,
                                      int              scaleVoltage,
                                      int              waitForVoltScale,
                                      PSL_PrologueFunc prologueFunc,
                                      PSL_EpilogueFunc epilogueFunc);


//-----------------------------------------------------------------------------
// PSL_querySetpoints().
//
// Description:
//    Return the clock frequency, clock mode, and voltage that is associated
//    with each of the specified setpoints.
//
// Parameters:
//    count   [in]
//       Specifies the number of clocks pointed to by clks.
//
//    *clks   [in]
//       Pointer to locations that specify the clocks.  The number of clocks
//       referred to by the pointer should match the count.
//
//    *setpoints   [in]
//       Pointer to locations that specify the setpoints that are being
//       queried. The setpoint for clks[0] is specified by setpoints[0],
//       the setpoint for clks[1] is specified by setpoints[1], etc.
//
//    *frequencies   [out]
//       Pointer to locations to store the frequency associated with each
//       setpoint.  The frequency for setpoints[0] will be returned in 
//       frequencies[0], the frequency for setpoints[1] will be returned
//       in frequencies[1], etc.
//
//    *voltages   [out]
//       Pointer to locations to store the voltages associated with each
//       setpoint.  A setpoint's voltage is the minimum voltage required
//       for the setpoint's frequency.  Note that this voltage may not be
//       equal to the current voltage if voltage scaling was not performed
//       by PSL_changeSetpoint, or if the current setpoint for another clock
//       required a higher voltage.  The voltage for setpoints[0] will be 
//       returned in voltages[0], the voltage for setpoints[1] will be returned
//       in voltages[1], etc.
//
//    *modes   [out]
//       Pointer to locations to store the clock mode associated with each
//       setpoint (e.g. PSL_BYPASS or PSL_LOCK).  The clock mode for 
//       setpoints[0] will be returned in modes[0], the clock mode for
//       setpoints[1] will be returned in modes[1], etc.
//
// Return Value:
//    PSL_OK
//       If the specified clocks and setpoints are valid.
//
//    PSL_INVALID_CLK
//       if any of the specified clocks are invalid.
//
//    PSL_INVALID_SETPOINT
//       if any of the setpoints are invalid.  A clock's valid setpoints are
//       those in the range 0 .. n - 1, where n is the number of valid setpoints
//       returned by PSL_getNumSetpoints().  
//
//    PSL_NOT_INITIALIZED
//       if PSL has not been initialized.
//
//-----------------------------------------------------------------------------
extern PSL_Status PSL_querySetpoints(unsigned        count,
                                     PSL_ClkID       *clks,
                                     PSL_Setpoint    *setpoints,
                                     float           *frequencies,
                                     float           *voltages,
                                     PSL_ClkMode     *modes);


//-----------------------------------------------------------------------------
// PSL_querySetpointFrequencies()
//
// Description:
//    Return the clock frequency that is associated with each of the specified
//    setpoints.
//
// Parameters:
//    count   [in]
//       Specifies the number of clocks pointed to by clks.
//
//    *clks   [in]
//       Pointer to locations that specify the clocks.  The number of clocks
//       referred to by the pointer should match the count.
//
//    *setpoints   [in]
//       Pointer to locations that specify the setpoints that are being
//       queried. The setpoint for clks[0] is specified by setpoints[0],
//       the setpoint for clks[1] is specified by setpoints[1], etc.
//
//    *frequencies   [out]
//       Pointer to locations to store the frequency associated with each
//       setpoint.  The frequency for setpoints[0] will be returned in 
//       frequencies[0], the frequency for setpoints[1] will be returned
//       in frequencies[1], etc.
//
// Return Value:
//    PSL_OK
//       if all of the specified clocks are valid.
//
//    PSL_INVALID_CLK
//       if any of the specified clocks are invalid.
// 
//    PSL_INVALID_SETPOINT
//       if any of the setpoints are invalid.  A clock's valid setpoints are
//       those in the range 0 .. n - 1, where n is the number of valid setpoints
//       returned by PSL_getNumSetpoints(). 
//
//    PSL_NOT_INITIALIZED
//       if PSL has not been initialized.
//   
//-----------------------------------------------------------------------------
extern PSL_Status PSL_querySetpointFrequencies(unsigned     count,
                                               PSL_ClkID    *clks,
                                               PSL_Setpoint *setpoints,
                                               float        *frequencies);


//-----------------------------------------------------------------------------
// PSL_querySetpointVoltages()
//
// Description:
//    Return the voltage that is associated with each of the specified setpoints.
//
// Parameters:
//    count   [in]
//       Specifies the number of clocks pointed to by clks.
//
//    *clks   [in]
//       Pointer to locations that specify the clocks.  The number of clocks
//       referred to by the pointer should match the count.
//
//    *setpoints   [in]
//       Pointer to locations that specify the setpoints that are being
//       queried. The setpoint for clks[0] is specified by setpoints[0],
//       the setpoint for clks[1] is specified by setpoints[1], etc.
//
//    *voltages   [out]
//       Pointer to locations to store the voltages associated with each
//       setpoint.  A setpoint's voltage is the minimum voltage required
//       for the setpoint's frequency.  Note that this voltage may not be
//       equal to the current voltage if voltage scaling was not performed
//       by PSL_changeSetpoint, or if the current setpoint for another clock
//       required a higher voltage.  The voltage for setpoints[0] will be 
//       returned in voltages[0], the voltage for setpoints[1] will be returned
//       in voltages[1], etc.
//
// Return Value:
//    PSL_OK
//       if all of the specified clocks are valid.
//
//    PSL_INVALID_CLK
//       if any of the specified clocks are invalid.
//
//    PSL_INVALID_SETPOINT
//       if any of the setpoints are invalid.  A clock's valid setpoints are
//       those in the range 0 .. n - 1, where n is the number of valid setpoints
//       returned by PSL_getNumSetpoints(). 
//
//    PSL_NOT_INITIALIZED
//       if PSL has not been initialized.
//    
//-----------------------------------------------------------------------------
extern PSL_Status PSL_querySetpointVoltages(unsigned     count,
                                            PSL_ClkID    *clks,
                                            PSL_Setpoint *setpoints,
                                            float        *voltages);
                                            
                                               
                                                                                                                     
//-----------------------------------------------------------------------------
// PSL_querySetpointModes()
//
// Description:
//    Return the clock mode that is associated with each of the specified
//    setpoints.
//
// Parameters:
//    count   [in]
//       Specifies the number of clocks pointed to by clks.
//
//    *clks   [in]
//       Pointer to locations that specify the clocks.  The number of clocks
//       referred to by the pointer should match the count.
//
//    *setpoints   [in]
//       Pointer to locations that specify the setpoints that are being
//       queried. The setpoint for clks[0] is specified by setpoints[0],
//       the setpoint for clks[1] is specified by setpoints[1], etc.
//
//    *modes   [out]
//       Pointer to locations to store the clock mode associated with each
//       setpoint (e.g. PSL_BYPASS or PSL_LOCK).  The clock mode for 
//       setpoints[0] will be returned in modes[0], the clock mode for
//       setpoints[1] will be returned in modes[1], etc.
//
// Return Value:
//    PSL_OK
//       if all of the specified clocks are valid.
//
//    PSL_INVALID_CLK
//       if any of the specified clocks are invalid.
//
//    PSL_INVALID_SETPOINT
//       if any of the setpoints are invalid.  A clock's valid setpoints are
//       those in the range 0 .. n - 1, where n is the number of valid setpoints
//       returned by PSL_getNumSetpoints(). 
//
//    PSL_NOT_INITIALIZED
//       if PSL has not been initialized.
//    
//-----------------------------------------------------------------------------
extern PSL_Status PSL_querySetpointModes(unsigned     count,
                                         PSL_ClkID    *clks,
                                         PSL_Setpoint *setpoints,
                                         PSL_ClkMode  *modes);
                                            

//-----------------------------------------------------------------------------
// PSL_querySetpointTransitions().
//
// Description:
//    Return the maximum scaling latencies that are associated with each
//    of the specified setpoints changes.
//    
// Parameters:
//    count
//       Specifies the number of clocks pointed to by clks.
//
//    *clks   [in]
//       Pointer to locations that specify the clocks.  The number of clocks
//       referred to by the pointer should match the count.
//
//    *fromSetpoints   [in]
//       Pointer to locations that specify the source setpoints.  The source
//       setpoint for clks[0] is specified by fromSetpoints[0], the source
//       setpoint for clks[1] is specified by fromSetpoints[1], etc.
//
//    *toSetpoints   [in]
//       Pointer to locations that specify the destination setpoints.  The
//       destination setpoint for clks[0] is specified by toSetpoints[0], the 
//       destination setpoint for clks[1] is specified by toSetpoints[1], etc.
//
//    *freqScalingLatencies   [out]
//       Pointer to locations to store the maximum latencies associated with
//       each of the frequency scaling operations that will occur during the
//       specified setpoint changes.  The latencies are specified in 
//       microseconds.  Following the initiation of s frequency scaling
//       operation, the latency is the time required before the clock starts
//       generating the new frequency.  The latency for the setpoint change
//       associated with clks[0] is specified by freqScalingLatencies[0], the
//       latency for the setpoint change associated with clks[1] is specified
//       by freqScalingLatencies[1], etc.         
//
//    *voltageScalingLatency   [out]
//       Location to store the maximum latency associated with the
//       voltage scaling that may occur during the specified setpoint
//       changes.  The latency is given in microseconds.  Following
//       the initiation of the voltage scaling operation, the latency
//       is the time required before the new voltage has been reached.
//
// Return Value:
//    PSL_OK
//       If the specified clocks and setpoints are valid.
//
//    PSL_INVALID_CLK
//       if any of the specified clocks are invalid.
//
//    PSL_INVALID_SETPOINT
//       if any of the setpoints are invalid.  A clock's valid setpoints are
//       those in the range 0 .. n - 1, where n is the number of valid setpoints
//       returned by PSL_getNumSetpoints().   
//
//    PSL_NOT_INITIALIZED
//       if PSL has not been initialized.
//
//-----------------------------------------------------------------------------
extern PSL_Status PSL_querySetpointTransitions(unsigned     count,
                                               PSL_ClkID    *clks,
                                               PSL_Setpoint *fromSetpoints,
                                               PSL_Setpoint *toSetpoints,
                                               unsigned     *freqScalingLatencies,
                                               unsigned     *voltageScalingLatency);   
                               

//-----------------------------------------------------------------------------
// PSL_getFrequencies()
//
// Description:
//    Return the current clock frequency for each of the specified clocks.
//
// Parameters:
//    count   [in]
//       Specifies the number of clocks pointed to by clks.
//
//    *clks   [in]
//       Pointer to locations that specify the clocks.  The number of clocks
//       referred to by the pointer should match the count.
//
//    *frequencies   [out]
//       Pointer to locations to store the current frequency of each of
//       the specified clocks.  The current frequency of a clock is the same
//       as the frequency returned by PSL_querySetpointFrequencies when
//       that function is called with the clock's current setpoint (i.e. the
//       current frequency of a clock is always the same as the frequency of
//       the clock's current setpoint).  The current frequency for clks[0]
//       will be returned in frequencies[0], the current frequency for
//       clks[1] will be returned in frequencies[1], etc.
//
// Return Value:
//    PSL_OK
//       if all of the specified clocks are valid.
//
//    PSL_INVALID_CLK
//       if any of the specified clocks are invalid.
//
//    PSL_NOT_INITIALIZED
//       if PSL has not been initialized.
//    
//-----------------------------------------------------------------------------
extern PSL_Status PSL_getFrequencies(unsigned  count,
                                     PSL_ClkID *clks,
                                     float     *frequencies);


//-----------------------------------------------------------------------------
// PSL_getModes()
//
// Description:
//    Return the current clock mode (e.g. PSL_BYPASS or PSL_LOCK)
//    for each of the specified clocks.
//
// Parameters:
//    count   [in]
//       Specifies the number of clocks pointed to by clks.
//
//    *clks   [in]
//       Pointer to locations that specify the clocks.  The number of clocks
//       referred to by the pointer should match the count.
//
//    *modes   [out]
//       Pointer to locations to store the current mode of each of
//       the specified clocks.  The current operating mode of a clock is
//       the same as the mode returned by PSL_querySetpointModes when that
//       function is called with the clock's current setpoint (i.e. the
//       current mode of a clock is always the same as the mode of the
//       clock's current setpoint).  The current mode for clks[0] will be
//       returned in modes[0], the current mode for clks[1] will be returned
//       in modes[1], etc.
//
// Return Value:
//    PSL_OK
//       if all of the specified clocks are valid.
//
//    PSL_INVALID_CLK
//       if any of the specified clocks are invalid.
//
//    PSL_NOT_INITIALIZED
//       if PSL has not been initialized.
//    
//-----------------------------------------------------------------------------
extern PSL_Status PSL_getModes(unsigned    count,
                               PSL_ClkID   *clks, 
                               PSL_ClkMode *modes);

//-----------------------------------------------------------------------------
// PSL_getVoltage()
//
// Description:
//    Return the current voltage.
//
// Return Value:
//    The current voltage.  If voltage scaling was not performed in
//    any of the calls to PSL_changeSetpoint, the current voltage is
//    assumed to be the initial voltage as specified in the call to
//    PSL_initialize.  If voltage scaling is being done, the current
//    voltage will be the lowest voltage that is sufficient for all of
//    the current setpoints.
//    
//-----------------------------------------------------------------------------
extern float PSL_getVoltage();

              
#endif   // #ifndef _PSL_H
