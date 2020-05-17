/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== _log.h ========
 *
 */
#ifndef _LOG_
#define _LOG_

#include <std.h>

/*
 *  System Log event ids
 *  WARNING: These ids *must* match those in log.h50, log.h54, ...
 */
#define _LOG_PRDBEGIN   0   /* handle = prd, arg0 = 0 */
#define _LOG_PRDEND     1   /* handle = 0, arg0 = 0 */
#define _LOG_SWIPOST    2   /* handle = swi, arg0 = 0 */
#define _LOG_SWIBEGIN   3   /* handle = swi, arg0 = 0 */
#define _LOG_SWIEND     4   /* handle = swi, arg0 = swi->lock */
#define _LOG_PRDTICK    5   /* tick.high = PRD_D_timh, tick.low = PRD_D_timl */
#define _LOG_CLKINT     6   /* tick.high = CLK_R_timh, tick.low = CLK_R_timl */
#define _LOG_HWIBEGIN   7   /* handle = hwi id, arg0 = 0 */
#define _LOG_USRMSG     8   /* user message (format arg0) */
#define _LOG_USRERR     9   /* user error (format arg0) */

/* SPOX specific system event ids */
#define _LOG_TSKBLOCKED         10      /* handle = tsk, arg0 = 0 */
#define _LOG_TSKDONE            11      /* handle = tsk, arg0 = 0 */
#define _LOG_TSKRUNNING         12      /* handle = tsk, arg0 = 0 */
#define _LOG_TSKREADY           13      /* handle = tsk, arg0 = 0 */
#define _LOG_SEMPOST            14      /* handle = sem, arg0 = sem count */
#define _LOG_TSKYIELD           15      /* handle = tsk, arg0 = 0 */
#define _LOG_PWRMRSRCCHNG       16      /* arg0 = id, arg1 = 0(off) or 1(on) */

#define _LOG_NUMIDS             17

/*
 *  Bit masks for enabling and disabling LOG object on the target.  Must
 *  be the same as in log.hti.
 *
 *  _LOG_ENABLETARG: This bit should only be set and cleared by the target.
 *  _LOG_ENABLEHOST: This bit should only be set and cleared by the host.
 *
 *  Both _LOG_ENABLETARG and _LOG_ENABLEHOST bits must be cleared for the
 *  log to be enabled.  When either bit is set, the log is disabled.
 */
#define _LOG_ENABLETARG 0x1
#define _LOG_ENABLEHOST 0x2

/*
 * Comment out this this line since _log.h used by both the host
 * and target sides.  The '#ifdef DSP_' symbol below is used
 * to determine if the DSP_DMem-dependant structures should be exposed.
 *
 * This means that <dsp.h> must be included before <_log.h> in some cases!
#include <dsp.h>
*/

#ifdef DSP_     /* only expand rest of file if <dsp.h> is included first! */

/*
 *  ======== _LOG_MAXPRINTFARGS ========
 *  The max number of arguments that can be referenced within a 
 *  LOG_printf() format string.
 */
#define _LOG_MAXPRINTFARGS  2

/*
 *  ======== _LOG_Event ========
 *  This structure defines the layout of the various LOG event
 *  records that are written by the target.
 */
typedef struct _LOG_Event {
    LgUns               seqNum;         /* sequence number */
    union {
        struct {
            union {
                struct {
                    LgUns       arg0;   /* optional argument */
                    LgUns       format; /* "printf" format string */
                } user;
                struct {
                    LgUns    arg0;      /* optional additional argument */
                    LgUns       handle; /* thread handle */
                } thread;
                
                struct {
                    LgUns       high;   /* high 16-bits of tick */
                    LgUns       low;    /* low 16-bits of tick */
                } tick;
            } asa;
            
            LgUns       id;             /* event id */
        } system;

        struct {
            LgUns       arg0;           /* argument 0 for printf */
            LgUns       arg1;           /* argument 1 for printf */
            LgUns       format;         /* printf format */
        } printf;
        
        struct {
            LgUns       arg0;           /* raw data argument 1 */
            LgUns       arg1;           /* raw data argument 2 */
            LgUns       arg2;           /* raw data argument 3 */
        } raw;
    } asa;
} _LOG_Event;

#define _LOG_NULL    ((LgUns)(~0))
#define _LOG_NULL16  0xFFFF
#define _LOG_NULL55L 0xFFFF0000

/*
 *  ======== _LOG_ELEMSIZE ========
 *  Size of a LOG record on target (words)
 */
#define _LOG_ELEMSIZE   (sizeof(_LOG_Event) / sizeof(LgUns))

/*
 *  ======== _LOG_Type ========
 *  Each log is classified according to the type of data it contains
 */
typedef enum _LOG_Type {
    _LOG_SYSTEM,                /* log contains system LOG events */
    _LOG_PRINTF,                /* log contains printf records */
    _LOG_RAWDATA                /* log contains uninterpreted data */
} _LOG_Type;

/*
 *  ======== _LOG_Log ========
 */
typedef struct _LOG_Log {
    String      name;           /* name of log */
    _LOG_Type   type;           /* _LOG_SYSTEM, _LOG_PRINTF, _LOG_RAWDATA,.. */
    String      format;         /* printf format for _LOG_RAWDATA logs */
    Int         size;           /* size of log on target (in target words) */
    LgUns       addr;           /* address of log buffer on target */
    Int         bufPageNum;     /* page number of log buffer on target */
    Int         logPageNum;     /* page number of log object on target */
    LgUns       seqAddr;        /* address of log sequence number */
    LgUns       flagAddr;       /* address of log flag */
    LgUns       seqNum;         /* current sequence number */
    LgUns       *buf;           /* local buffer to hold target data */
} _LOG_Log;

/*
 *  ======== _LOG_SWIFXNOFF ========
 *  fxn offset within SWI object in DSP words
 *
 *  WARNING: This offset must match that in swi.h50, swi.h54, ...
 */
#define _LOG_SWIFXNOFF  6

/*
 *  ======== _LOG_SEQOFFSET ========
 *  Sequence number offset in LOG object
 *
 *  WARNING: This offset must match that in log.h50, log.h54 ...
 */
#define _LOG_SEQOFFSET  2

/*
 *  ======== _LOG_FLAGOFFSET ========
 *  Sequence number offset in LOG object
 *
 *  WARNING: This offset must match that in log.h50, log.h54 ...
 */
#define _LOG_FLAGOFFSET  1

/*
 *  ======== _LOG_hasHandle ========
 *  Note: _LOG_PRDEND is defined as having a handle, but the handle is
 *  not used.
 */
#define _LOG_hasHandle(id)  ((id) >= 0 && (id) <= _LOG_SWIEND \
    || (id) >= _LOG_TSKBLOCKED && (id) < _LOG_NUMIDS)

#define _LOG_isTSKevent(id) ((id) >= _LOG_TSKBLOCKED \
    && (id) <= _LOG_TSKYIELD && (id) != _LOG_SEMPOST)
    
#define _LOG_isSWIevent(id) ((id) >= _LOG_SWIPOST \
    && (id) <= _LOG_SWIEND)

/*
 *  ======== _LOG_isUser ========
 */
#define _LOG_isUser(id) ( ((id) == _LOG_USRMSG) || ((id) == _LOG_USRERR) )

#endif  /* DSP_ */

#endif  /* _LOG_ */
