/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== log.h ========
 *
 */

#ifndef LOG_
#define LOG_

#include <_log.h>
#include <trc.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== LOG_Obj structure ========
 */
typedef struct LOG_Event {
#if (defined(_55_) && defined(__LARGE_MODEL__)) 
    Uns pad;
#endif
#if defined(_28_)
    Long seqnum;
#else
    Uns seqnum;
#endif
    Arg val1;
    Arg val2;
    Arg val3;
}  LOG_Event;

/* The trace buffer is limited to 64K on _55H_. We want to keep
 * the LOG event time the same by making use of the lenmask 
 */
typedef struct LOG_Obj {
    Arg* bufend;
    Uns flag;
    Uns seqnum;
    LOG_Event* curptr;
    Uns lenmask;
    LOG_Event* bufbeg;
} LOG_Obj;

typedef struct LOG_Obj *LOG_Handle; 

#ifdef _6x_
extern far LOG_Handle LOG_D_system;  /* distinguished system log */
#else
extern LOG_Handle LOG_D_system;  /* distinguished system log */
#endif


/*
 *  ======== LOG_disable ========
 *  set the flag field of the LOG object to 1 to disable the LOG
 */
#define LOG_disable(log) ((log)->flag = 1)

/*
 *  ======== LOG_enable ========
 *  set the flag field of the LOG object to 0 to enable the LOG
 */
#define LOG_enable(log) ((log)->flag = 0)

/*
 *  ======== LOG_error ========
 *  Write a "user" error event into the system log.  This operation is
 *  not affected by any TRC trace bits; an error event is *always* written
 *  to the system log.
 */
#define LOG_error(format, arg0) \
    LOG_eventJ(LOG_D_system, (Arg)(arg0), (Arg)(format), (Arg)_LOG_USRERR) 

/*
 *  ======== LOG_event ========
 *  Write an uninterpreted 3-argument event into the specified log.
 */
extern Void LOG_event(LOG_Handle log, Arg arg0, Arg arg1, Arg arg2);

/*
 *  ======== LOG_event5 ========
 *  Write an uninterpreted 5-argument event into the specified log.
 */
extern Void LOG_event5(LOG_Handle log, Arg arg0, Arg arg1, Arg arg2, Arg arg3, Arg arg4);

/*
 *  ======== LOG_copy ========
 *  Copy an uninterpreted 8-word record into log's buffer.  No sequence
 *  number management is performed, it is left to the caller to manage
 *  the sequence number field as needed.
 */
extern Void LOG_copy(LOG_Handle log, Void *record);

/*
 *  ======== LOG_message ========
 *  Write a "user" message event into the system log provided that both
 *  TRC_GBLHOST and TRC_GBLTARG trace is enabled.
 */
#define LOG_message(format, arg0)                \
     if (!TRC_query(TRC_GBLHOST | TRC_GBLTARG))  \
        LOG_eventJ(LOG_D_system,(Arg)(arg0),(Arg)(format),(Arg)_LOG_USRMSG) 

/*
 *  ======== LOG_printf ========
 *  Write a up-to-2-format-arguments "printf" event into the specified log.
 */
extern Void LOG_printf(LOG_Handle log, String format, ...);

/*
 *  ======== LOG_printf4 ========
 *  Write a up-to-4-format-arguments "printf" event into the specified log.
 */
extern Void LOG_printf4(LOG_Handle log, String format, ...);

/*
 *  ======== LOG_reset ========
 *  reset the LOG by doing the following:
 *  log->flag = 0;
 *  log->seqnum = 0;
 *  log->curptr = log->bufbeg;
 *  set each word in the log buf to 0xffffffff;
 */
extern Void LOG_reset(LOG_Handle log);

#ifdef _BIOSROM_

extern Void (*LOG_eventPtr)(LOG_Handle log, Arg arg0, Arg arg1, Arg arg2);

#define LOG_eventJ(log, arg0, arg1, arg2)       (LOG_eventPtr)(log, arg0, arg1, arg2)

#else

#define LOG_eventJ(log, arg0, arg1, arg2)       LOG_event(log, arg0, arg1, arg2)

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* LOG_ */




