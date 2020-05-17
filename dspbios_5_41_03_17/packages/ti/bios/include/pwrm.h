/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== pwrm.h ========
 *
 *  DSP/BIOS Power Manager definitions.
 *
 */

#ifndef PWRM_
#define PWRM_

#ifdef __cplusplus
extern "C" {
#endif

/* PWRM return codes: */
#define PWRM_SOK                0
#define PWRM_EFAIL              1
#define PWRM_EINVALIDEVENT      2
#define PWRM_EINVALIDHANDLE     3
#define PWRM_EINVALIDPOINTER    4
#define PWRM_EINVALIDVALUE      5
#define PWRM_ENOTIMPLEMENTED    6
#define PWRM_ENOTSUPPORTED      7
#define PWRM_EOUTOFRANGE        8
#define PWRM_ETIMEOUT           9
#define PWRM_EBUSY              10
#define PWRM_EINITFAILURE       11
#define PWRM_ETOOMANYCALLS      12

#if defined(_64P_) && !defined(_674_)
/* PWRM sleep modes: */
#define PWRM_HIBERNATE          0
#define PWRM_RETENTION          1
#define PWRM_RETENTIONALT       2
#define PWRM_STANDBY            3

/* PWRM sleep argument control word, sleepArg[0] */
#define PWRM_ARGSWUGENMASK      0x0001  /* sleepArg[1-4] = wugen masks */
#define PWRM_ARGSNOPLLCONFIG    0x0002  /* do not modify PLL configuration */

/* PWRM capabilities masks: */
#define PWRM_CVFSCALING         8
#define PWRM_CRESOURCETRACKING  0x200
#define PWRM_CLOADMONITORING    0x800

/* resource tracking callout defines */
#define PWRM_SET       1
#define PWRM_RELEASE   0

#elif defined(_674_)
/* PWRM sleep modes: */
#define PWRM_STANDBY            0x1
#define PWRM_SLEEP              0x2
#define PWRM_DEEPSLEEP          0x4

#define PWRM_EXTERNAL           0x8     /* PMI_EXTERNAL */
#define PWRM_RTC_ALARM          0x10    /* PMI_RTC_ALARM */

#define PWRM_PLL0               0x1
#define PWRM_PLL1               0x2

/* PWRM capabilities masks: */
#define PWRM_CVFSCALING         8
#define PWRM_CRESOURCETRACKING  0x200
#define PWRM_CLOADMONITORING    0x800

/* resource tracking special handler defines */
#define PWRM_RELEASE   0
#define PWRM_SET       1
#define PWRM_QUERY     2

#else
/* PWRM capabilities masks: */
#define PWRM_CDEEPSLEEP         1
#define PWRM_CSLEEPUNTILRESTART 2
#define PWRM_CSNOOZE            4
#define PWRM_CVFSCALING         8
#define PWRM_CRESOURCETRACKING  0x200

/* PWRM sleep modes: */
#define PWRM_DEEPSLEEP          0
#define PWRM_SLEEPUNTILRESTART  1
#define PWRM_SNOOZE             2

/* PWRM idle domain definitions: */
#define PWRM_IDLECPU            0x1
#define PWRM_IDLEDMA            0x2
#define PWRM_IDLECACHE          0x4
#define PWRM_IDLEPERIPH         0x8
#define PWRM_IDLECLKGEN         0x10
#define PWRM_IDLEEMIF           0x20
#define PWRM_IDLEXPORT          0x40
#define PWRM_IDLEMPORT          0x80
#define PWRM_IDLEIPORT          0x100
#define PWRM_IDLEHWA            0x200
#endif

typedef Void * PWRM_NotifyHandle;

typedef Uns PWRM_Status;

#if defined(_64P_) && !defined(_674_)
typedef enum {
    PWRM_GOINGTORETENTION = 0,
    PWRM_AWAKEFROMRETENTION,
    PWRM_GOINGTORETENTIONALT,
    PWRM_AWAKEFROMRETENTIONALT,
    PWRM_PENDINGSETPOINTCHANGE,
    PWRM_DONESETPOINTCHANGE,
    PWRM_GOINGTOHIBERNATE,
    PWRM_AWAKEFROMHIBERNATE,
    PWRM_GOINGTOSTANDBY,
    PWRM_AWAKEFROMSTANDBY,
    PWRM_INVALIDEVENT
} PWRM_Event;

#elif defined(_674_)
typedef enum {
    PWRM_GOINGTOSTANDBY = 0,
    PWRM_AWAKEFROMSTANDBY,
    PWRM_GOINGTOSLEEP,
    PWRM_AWAKEFROMSLEEP,
    PWRM_GOINGTODEEPSLEEP,
    PWRM_AWAKEFROMDEEPSLEEP,
    PWRM_PENDING_CPU_SETPOINTCHANGE,
    PWRM_PENDING_PER_SETPOINTCHANGE,
    PWRM_DONE_CPU_SETPOINTCHANGE,
    PWRM_DONE_PER_SETPOINTCHANGE,
    PWRM_INVALIDEVENT
} PWRM_Event;

#else
typedef enum {
    PWRM_PENDINGSETPOINTCHANGE = 0,
    PWRM_DONESETPOINTCHANGE,
    PWRM_GOINGTODEEPSLEEP,
    PWRM_AWAKEFROMDEEPSLEEP,
    PWRM_GOINGTOSNOOZE,
    PWRM_AWAKEFROMSNOOZE,
    PWRM_GOINGTOSLEEPUNTILRESTART,
    PWRM_INVALIDEVENT
} PWRM_Event;
#endif

typedef enum {
    PWRM_NOTIFYDONE = 0,
    PWRM_NOTIFYNOTDONE,
    PWRM_NOTIFYERROR
} PWRM_NotifyResponse;

#if defined(_674_)

typedef Void * PWRM_ConstraintHandle;

typedef enum {
    PWRM_DISALLOWED_CPU_SETPOINT_MASK = 1,
    PWRM_DISALLOWED_PER_SETPOINT_MASK = 2,
    PWRM_DISALLOWEDSLEEPSTATE_MASK = 3
} PWRM_Constraint;

typedef enum {
    PWRM_CPU = 0,
    PWRM_PER
} PWRM_Domain;

typedef struct PWRM_Attrs {
    Bool scaleVoltage;
    Bool waitForVoltageScale;
} PWRM_Attrs;

typedef struct PWRM_SleepOverride {
    Uns sleepVoltage;
    Uns bypassedPLLs;
} PWRM_SleepOverride;

#else

typedef struct PWRM_Attrs {
    Bool scaleVoltage;
    Bool waitForVoltageScale;
    Uns idleMask;
} PWRM_Attrs;

#endif

#if defined(_64P_) || defined(_674_)

typedef struct PWRM_CPULoadInfo {
    Uns busyCycles;
    Uns totalCycles;
    Uns timeStamp;
} PWRM_CPULoadInfo;

typedef struct PWRM_LoadMonitorInfo {
    Uns numSlots;
    Uns ticksPerSlot;
} PWRM_LoadMonitorInfo;

typedef struct PWRM_SetpointInfo {
    Uns frequency;
    Uns voltage;
} PWRM_SetpointInfo;

#endif

#if defined(_674_)
typedef struct PWRM_Config {
    Bool scaleVoltage;
    Bool waitForVoltageScale;
} PWRM_Config;
#else
typedef struct PWRM_Config {
    Bool scaleVoltage;
    Bool waitForVoltageScale;
    Uns idleMask;
} PWRM_Config;
#endif

extern PWRM_Config *PWRM;

extern PWRM_Attrs PWRM_ATTRS;

#if defined(_64P_)
#if defined(_674_)
/*
 *  ======== PWRM_changeSetpoint ========
 *  Initiate a change to the F/V setpoint of the CPU, or the peripheral domain.
 *
 */
extern PWRM_Status PWRM_changeSetpoint(PWRM_Domain domain, Uns newSetpoint,
    Uns notifyTimeout);

/*
 *  ======== PWRM_configure ========
 *  Specify new configuration parameters for PWRM.
 *
 */
extern PWRM_Status PWRM_configure(PWRM_Attrs attrs);
#else
#endif

#else
/*
 *  ======== PWRM_changeSetpoint ========
 *  Initiate a change to the CPU's F/V setpoint.
 *
 */
extern PWRM_Status PWRM_changeSetpoint(Uns newSetpoint, Uns notifyTimeout);


/*
 *  ======== PWRM_configure ========
 *  Specify new configuration parameters for PWRM.
 *
 */
extern PWRM_Status PWRM_configure(PWRM_Attrs attrs);
#endif


/*
 *  ======== PWRM_getCapabilities ========
 *  Get info on PWRM's capabilities on the current platform.
 *
 */
extern PWRM_Status PWRM_getCapabilities(Uns * capsMask);

#if defined(_674_)
/*
 *  ======== PWRM_getConstraintInfo ========
 *  Get info on aggregate of constraints that have been registered with PWRM.
 *
 */
extern PWRM_Status PWRM_getConstraintInfo(PWRM_Constraint type, Arg *value);
#endif

#if defined(_64P_) || defined(_674_)
/*
 *  ======== PWRM_getCPULoad ========
 *  Get CPU load history from PWRM.
 *
 */
extern PWRM_Status PWRM_getCPULoad(Uns numSlots, PWRM_CPULoadInfo * loadInfo);
#endif

#if !defined(_674_)
/*
 *  ======== PWRM_getCurrentSetpoint ========
 *  Get the current setpoint in effect.
 *
 */
extern PWRM_Status PWRM_getCurrentSetpoint(Uns * setpoint);
#else
/*
 *  ======== PWRM_getCurrentSetpoint ========
 *  Get the current setpoint in effect.
 *
 */
extern PWRM_Status PWRM_getCurrentSetpoint(PWRM_Domain domain, Uns * setpoint);
#endif

#if defined(_64P_) || defined(_674_)
/*
 *  ======== PWRM_getLoadMonitorInfo ========
 *  Get PWRM Load Monitor configuration info.
 *
 */
extern PWRM_Status PWRM_getLoadMonitorInfo(PWRM_LoadMonitorInfo * monitorInfo);
#endif

#if !defined(_674_)
/*
 *  ======== PWRM_getNumSetpoints ========
 *  Get the number of setpoints supported on the platform.
 *
 */
extern PWRM_Status PWRM_getNumSetpoints(Uns * numberSetpoints);
#else
/*
 *  ======== PWRM_getNumSetpoints ========
 *  Get the number of setpoints supported on the platform.
 *
 */
extern PWRM_Status PWRM_getNumSetpoints(PWRM_Domain domain, 
    Uns * numberSetpoints);
#endif


#if defined(_64P_)

#if !defined(_674_)
/*
 *  ======== PWRM_getSetpointInfo ========
 *  Get the frequency and core voltage for a setpoint.
 *
 */
extern PWRM_Status PWRM_getSetpointInfo(Uns setpoint, Uns *frequency,
    Uns *voltage);
#else
/*
 *  ======== PWRM_getSetpointInfo ========
 *  Get the frequency and core voltage for a setpoint.
 *
 */
extern PWRM_Status PWRM_getSetpointInfo(PWRM_Domain domain, Uns setpoint, 
    Uns *frequency, Uns *voltage);
#endif

#else

/*
 *  ======== PWRM_getSetpointInfo ========
 *  Get the frequency and core voltage for a setpoint.
 *
 */
extern PWRM_Status PWRM_getSetpointInfo(Uns setpoint, float *frequency,
    float *voltage);

#endif

#if defined(_64P_)
#if defined(_674_)
/*
 *  ======== PWRM_getTransitionLatency ========
 *  Get the latency to scale from one setpoint to another setpoint.
 *
 */
extern PWRM_Status PWRM_getTransitionLatency(PWRM_Domain domain, 
    Uns initialSetpoint, Uns finalSetpoint, Uns *frequencyLatency, 
    Uns *voltageLatency);
#else
#endif
#else
/*
 *  ======== PWRM_getTransitionLatency ========
 *  Get the latency to scale from one setpoint to another setpoint.
 *
 */
extern PWRM_Status PWRM_getTransitionLatency(Uns initialSetpoint,
    Uns finalSetpoint, Uns *frequencyLatency, Uns *voltageLatency);
#endif

#if !defined(_64P_) && !defined(_674_)
/*
 *  ======== PWRM_idleClocks ========
 *  Immediately idle select clock domains.
 *
 */
extern PWRM_Status PWRM_idleClocks(Uns domainMask, Uns * idleStatus);
#endif

#if defined(_64P_) && !defined(_674_)
/*
 *  ======== PWRM_initSetpointInfo ========
 *  Communicate platform setpoint info to PWRM at runtime.
 *
 */
extern PWRM_Status PWRM_initSetpointInfo(Uns numSetpoints, Uns currentSetpoint,
    PWRM_SetpointInfo * setpointInfo);
#endif

#if defined(_674_)
/*
 *  ======== PWRM_registerConstraint ========
 *  Register an operational constraint with PWRM.
 *
 */
extern PWRM_Status PWRM_registerConstraint(PWRM_Constraint type, Arg value,
PWRM_ConstraintHandle *handle);
#endif

/*
 *  ======== PWRM_registerNotify ========
 *  Register a function to be called on a specific power event.
 *
 */
extern PWRM_Status PWRM_registerNotify(PWRM_Event eventType, LgUns eventMask,
    Fxn notifyFxn, Arg clientArg, PWRM_NotifyHandle * notifyHandle,
    Fxn *delayedCompletionFxn);

#if defined(_64P_) || defined(_674_)
/*
 *  ======== PWRM_resetCPULoadHistory ========
 *  Reset PWRM's buffered CPU load history.
 *
 */
extern PWRM_Status PWRM_resetCPULoadHistory(Bool sync);

/*
 *  ======== PWRM_signalEvent ========
 *  Signal a PWRM event to registered clients.
 *
 */
extern PWRM_Status PWRM_signalEvent(PWRM_Event eventType, Arg eventArg1,
    Arg eventArg2, Uns notifyTimeout);
#endif

/*
 *  ======== PWRM_sleepDSP ========
 *  Change the sleep state of the DSP.
 *
 */
extern PWRM_Status PWRM_sleepDSP(Uns sleepCode, LgUns sleepArg,
    Uns notifyTimeout);

#if defined(_64P_) || defined(_674_)
/*
 *  ======== PWRM_startCPULoadMonitoring ========
 *  Start collecting cpu load statistics.
 *
 */
extern PWRM_Status PWRM_startCPULoadMonitoring(Void);

/*
 *  ======== PWRM_stopCPULoadMonitoring ========
 *  Stop collecting cpu load statistics.
 *
 */
extern PWRM_Status PWRM_stopCPULoadMonitoring(Void);
#endif

#if defined(_674_)
/*
 *  ======== PWRM_unregisterConstraint ========
 *  Unregister for a power notification.
 *
 */
extern PWRM_Status PWRM_unregisterConstraint(PWRM_ConstraintHandle handle);
#endif
 
/*
 *  ======== PWRM_unregisterNotify ========
 *  Unregister for a power notification.
 *
 */
extern PWRM_Status PWRM_unregisterNotify(PWRM_NotifyHandle notifyHandle);

#if defined(_64P_) && !defined(_674_)
/*
 *  ======== PWRM_validateSetpoint ========
 *  Check if a setpoint transition would be allowed, given currently
 *  registered constraints.
 *
 */
extern PWRM_Status PWRM_validateSetpoint(Uns setpoint);
#endif

#ifdef __cplusplus
}
#endif

#endif /* PWRM_ */
