/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== rta.h ========
 * 
 * Global definitions for the RTA SDK
 *
 */

#ifndef __RTADEFS_H_
#define __RTADEFS_H_

/*
 *  RTA States
 *  These are the possible states of the target.
 *
 *  RTA_NOT_LOADED:
 *      A program is not loaded on the target, or a program is loaded,
 *      but no RTA data will be available due to some error (for example,
 *      the target program is not a BIOS program).
 *  RTA_HALTED:
 *      The target is halted, and the target program supports the gathering
 *      of RTA data.
 *  RTA_RUNNING:
 *      The target is running, and the target program supports the gathering
 *      of RTA data.
 *  RTA_STATE_UNKNOWN:
 *      The target is in an unknown state.
 */
#define RTA_STATE_COUNT 4       /* Total number of target states */
typedef enum tagRTA_STATE {
    RTA_NOT_LOADED,
    RTA_HALTED,
    RTA_RUNNING,
    RTA_STATE_UNKNOWN
} RTA_STATE;


/*
 *  Log Enumerations
 *
 *  LogExecutionState values represent the execution state
 *  of a thread of execution.  The values have the following meanings:
 *
 *  LOG_WAITING:
 *      The thread is in a waiting state.  For example, a task that is
 *      blocked on a semaphore, or an SWI that has not been posted.
 *
 *  LOG_READY:
 *      The thread is ready to running.  For example, an SWI that has been
 *      posted but has not yet run, or a task that has been preempted.
 *
 *  LOG_UNKNOWN_STATE:
 *      Not enough LOG data has been obtained to determine the state of the
 *      thread.
 *
 *  LOG_RUNNING:
 *      The thread is running. Only one thread will be in this state at a
 *      time.
 *
 *  LOG_DONE:
 *      This state only applies to tasks that have terminated.
 *
 *
 *  The following two states only apply to the CLK tick, PRD tick, and SEM
 *  post threads of execution. These "threads" can be in one of two states:
 *  either an event occurred, or an event did not occur.
 *
 *  LOG_EVENT:
 *      An event occurred for this thread.  For example, if the thread
 *      represents CLK ticks, this state indicates that a CLK tick occurred.
 *
 *  LOG_NOEVENT:
 *      An event did not occur for this thread.
 */
/*
   If modified, also check:
        "Color definitions" section in gch.h (vbd)
        _VSM_stateInfoTab in vsm_tab.c (vbd)
        graph state values in _vsm.h (vbd)
        gcrColor  in stateplot.cpp (gui)
        NUM_COLORS in statechartapp.h (gui)
*/
#define LOG_EXECUTION_STATE_COUNT   9
typedef enum tagLOG_EXECUTION_STATE {
    LOG_NOT_READY,
    LOG_READY,
    LOG_RUNNING,
    LOG_DONE,
    LOG_UNKNOWN_STATE,
    LOG_MESSAGE,
    LOG_ERROR_STATE,
    LOG_EVENT,
    LOG_NO_EVENT
} LOG_EXECUTION_STATE;

/*
 *  RTAEnable flag types
 *
 *  The RTAEnable flag types have the following meanings:
 *
 *  RTAENABLE_SWILOGGING:
 *      The trace flag refers to SWI logging.  If the flag is set (non-zero),
 *      SWI logging is enabled, otherwise, SWI logging is disabled.
 *
 *  RTAENABLE_PRDLOGGING:       The flag refers to PRD logging.
 *  RTAENABLE_CLKLOGGING:       The flag refers to CLK logging.
 *  RTAENABLE_TSKLOGGING:       The flag refers to TSK logging.
 *
 *  RTAENABLE_SWIACCUMULATION:
 *      The flag refers to SWI accumulation. If the flag is set, data
 *      will be accumulated in SWI STS objects.
 *
 *  RTAENABLE_PRDACCUMULATION:  The flag refers to PRD accumulation
 *  RTAENABLE_PIPACCUMULATION:  The flag refers to PIP accumulation
 *  RTAENABLE_HWIACCUMULATION:  The flag refers to HWI accumulation
 *  RTAENABLE_TSKACCUMULATION:  The flag refers to TSK accumulation
 *
 *  RTAENABLE_GLOBALTARGET:
 *      The flag refers to the global target enable bit (TRC_GBLTARG).
 *      
 *  RTAENABLE_GLOBALHOST:
 *      The flag refers to the global host enable bit (TRC_GBLHOST).
 */
#define RTAENABLE_FLAG_COUNT    13
typedef enum tagRTAENABLE_FLAG_TYPE {
    RTAENABLE_SWILOGGING,
    RTAENABLE_PRDLOGGING,
    RTAENABLE_CLKLOGGING,
    RTAENABLE_TSKLOGGING,
    RTAENABLE_SWIACCUMULATION,
    RTAENABLE_PRDACCUMULATION,
    RTAENABLE_PIPACCUMULATION,
    RTAENABLE_HWIACCUMULATION,
    RTAENABLE_TSKACCUMULATION,
    RTAENABLE_ENABLEUSER0,
    RTAENABLE_ENABLEUSER1,
    RTAENABLE_GLOBALTARGET,
    RTAENABLE_GLOBALHOST,
} RTAENABLE_FLAG_TYPE;

/*
 *  Host Channel Enumerations
 *
 *  HSTState values represent the state of the host channel
 */ 
#define HST_STATE_COUNT 3       /* Number of host channel states. */
typedef enum tagHST_STATE {
    HST_UNBOUND,  /* The host channel is not bound */
    HST_RUNNING,  /* The host channel is bound and running */
    HST_STOPPED   /* The host channel is bound and not running */
} HST_STATE;
              

/*
 *  HSTMode values represent the mode of the host channel
 */
#define HST_MODE_COUNT  2       /* Number of host channel modes. */
typedef enum tagHST_MODE {
    HST_INPUT,    /* The host channel is configured for input to the target. */
    HST_OUTPUT    /* The host channel is configured for output from the target. */
} HST_MODE;

/*
 *  STS Statisitic Fields
 *  Can be used as indices into variant array of STS data. Not needed by
 *  users of the C++ wrapper classes.
 */
#define STS_STATISTIC_FIELD_COUNT  4
typedef enum tagSTS_STATISTIC_FIELD
{
    STS_COUNT,
    STS_TOTAL,
    STS_MAX,
    STS_AVERAGE
} STS_STATISTIC_FIELD;

/*
 *  ======== STS_UNITS_TYPE ========
 *
 *  An STS object will have one of the following three types of units,
 *  depending on how it is updated on the target.
 *
 *  STS_HI_TIME_BASED_UNITS:  The high resolution time is used to update
 *  the STS object. For example, if it is updated in the following manner:
 *
 *      STS_set(&stsObj, CLK_gethtime());
 *      'do algorithm'
 *      STS_delta(&stsObj, CLK_gethtime());
 *    
 *  STS_LO_TIME_BASED_UNITS:  The low resolution time is used to update
 *  the STS object. For example, if it is updated in the following manner:
 *
 *      STS_set(&stsObj, CLK_getltime());
 *      'do algorithm'
 *      STS_delta(&stsObj, CLK_getltime());
 *
 *  STS_USER_UNITS:  The STS object is not time-based.
 *
 *  All STS data uploaded from the target will be in "raw" units. For
 *  time-based STS objects, raw units translate to the following.
 *
 *  STS_LO_TIME_BASED_UNITS     -  Timer interrupts
 *  STS_HI_TIME_BASED_UNITS     -  Instruction cycles
 * 
 *  Each time-based STS object will have a conversion factor to convert
 *  the raw units to microseconds. This conversion factor can be divided by
 *  1000 to get a factor to convert from raw units to milliseconds.
 */
typedef enum tagSTS_UNITS_TYPE
{
    STS_HI_TIME_BASED_UNITS,
    STS_LO_TIME_BASED_UNITS,
    STS_USER_UNITS
} STS_UNITS_TYPE;


/*
 *  ======== CFG_STREAM_TYPE ========
 *
 *  Calls to CCBiosConfig::GetOuptut use this enum
 *  to specify which stream to retrieve output from.
 *
 *      CFG_STD_OUTPUT  - Standard output
 *      CFG_STD_ERROR   - Standard error
 *
 */
typedef enum tagCFG_STREAM_TYPE
{
    CFG_STD_OUTPUT = 1,
    CFG_STD_ERROR = 2
} CFG_STREAM_TYPE;

#endif

