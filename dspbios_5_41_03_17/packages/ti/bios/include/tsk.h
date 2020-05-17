/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== tsk.h ========
 *
 */

#ifndef TSK_
#define TSK_

#include <stdarg.h>
#include <stddef.h>

#include <knl.h>
#include <prd.h>
#include <sem.h>
#include <sts.h>
#include <sys.h>
#include <trg.h>
#include <hwi.h>
#include <swi.h>
#include <_hook.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TSK_IDLEPRI     KNL_IDLEPRI
#define TSK_MINPRI      KNL_MINPRI
#define TSK_MAXPRI      KNL_MAXPRI
#define TSK_MAXARGS     KNL_MAXARGS

#define TSK_RUNNING     KNL_RUNNING
#define TSK_READY       KNL_READY
#define TSK_BLOCKED     KNL_BLOCKED
#define TSK_TERMINATED  KNL_TERMINATED
#define TSK_STACKSTAMP  TRG_STACKSTAMP  

typedef struct TSK_Config {
    Int     STACKSEG;
    Int     PRIORITY;
    size_t  STACKSIZE;
#ifdef _55_
    size_t  SYSSTACKSIZE;
#endif
    Fxn     CREATEFXN;
    Fxn     DELETEFXN;
    Fxn     EXITFXN;
    Fxn     SWITCHFXN;
    Fxn     READYFXN;
} TSK_Config;

extern TSK_Config *TSK;
                /* For C55x, TSK is defined in tsk.c as TSK_Config *TSK */
                /* For C54x, TSK is defined in TSK_end as
                        .bss _TSK, 1 * STD_TARGWORDMAUS, 0, STD_TARGWORDMAUS */
                /* For C6x, TSK is defined in TSK_end as
                        .bss _TSK, 1 * STD_TARGWORDMAUS, STD_TARGWORDMAUS */

typedef Int TSK_Mode;

typedef struct TSK_Attrs {
    Int         priority;       /* task priority */
    Ptr         stack;          /* stack supplied  */
    size_t      stacksize;      /* size of stack */
#ifdef _55_
    size_t      sysstacksize;  /* size of system stack */
#endif
    Int         stackseg;       /* segment to allocate stack from */
    Ptr         environ;        /* environment pointer */
    String      name;           /* printable name */
    Bool        exitflag;       /* FALSE for server tasks */
    Bool        initstackflag;  /* FALSE disables stack initialization */
} TSK_Attrs;

typedef struct TSK_Obj {
    KNL_Obj     kobj;           /* kernel object */

    Ptr         stack;          /* used in TSK_delete() */
#ifdef _55_
    Ptr         sysstack;       /* system stack for 55x */
                                /* Unused in _55Pb_ */
#endif
    size_t      stacksize;      /* ditto */
#ifdef _55_
    size_t      sysstacksize;   /* system stackszie for 55x */
                                /* Unused in _55Pb_ */
#endif
    Int         stackseg;       /* segment to allocate stack from */
    String      name;           /* printable name */
    Ptr         environ;        /* environment pointer */
    Int         errno;          /* TSK_seterr()/TSK_geterr() */
    Bool        exitflag;       /* FALSE for server tasks */
} TSK_Obj, *TSK_Handle;

typedef struct TSK_Stat {
    TSK_Attrs   attrs;          /* task attributes */
    TSK_Mode    mode;           /* TSK_RUNNING, TSK_READY, ... */
    Ptr         sp;             /*stack pointer(not valid when TSK_RUNNING!)*/
   #ifdef _55_
    Ptr         ssp;            /* System stack pointer only forr 55x*/
                                /* Unused in _55Pb_ */
   #endif
    size_t      used;           /* stack used */
   #ifdef _55_
    size_t      sysUsed;        /* System stack used             */
   #endif
} TSK_Stat;

extern TSK_Attrs TSK_ATTRS;     /* default attributes */
                                /* defined in tsk.c */

extern Int      TSK_nTasks;     /* only modified while inside kernel */
extern Int      TSK_pid;        /* MP processor id */
extern Int      TSK_initPid;    /* TSK_pid initialization record */
extern Int      TSK_NUMPIDS;    /* number of processors in MP system */

extern SEM_Obj  TSK_timerSem;

#define TSK_geterr(task)        ((task)->errno)
#define TSK_getpri(task)        (((KNL_Handle)(task))->priority)
#define TSK_getname(task)       ((task)->name)
#define TSK_self()              ((TSK_Handle)KNL_curtask)
#define TSK_seterr(task, e)     ((task)->errno = (e))
#define TSK_sleep(nticks)       (SEM_pendJ(&TSK_timerSem, nticks))
#define TSK_time()              KNL_curtime
#define TSK_tick()              KNL_tick()
#define TSK_itick()             KNL_tick()

/*
 *  ======== TSK_isTSK ========
 *  Returns true if current thread is not a HWI or SWI.
 */
#define TSK_isTSK()             (!HWI_isHWI() && !SWI_isSWI() && (TSK_self() != (TSK_Handle)&KNL_dummy))

extern Void TSK_checkstacks(TSK_Handle oldtask, TSK_Handle newtask);
extern TSK_Handle TSK_create(Fxn fxn, TSK_Attrs *attrs, ...);
extern Void TSK_delete(TSK_Handle task);
extern Void TSK_disable(void);
extern Void TSK_enable(void);
extern Void TSK_exit(void);
extern Void TSK_init(void);
extern Void TSK_print(TSK_Handle task);
extern Void TSK_setenv(TSK_Handle task, Ptr env);
extern Int  TSK_setpri(TSK_Handle task, Int newpri);
extern Void TSK_stat(TSK_Handle task, TSK_Stat *statbuf);
extern Void TSK_start(TSK_Handle task, Fxn fxn, va_list argv);
extern Void TSK_switchFxn(KNL_Handle oldtask, KNL_Handle newtask);
extern Void TSK_yield(void);

/*
 *  Task hooks for instrumented kernel.
 */
extern Void TSK_createFxn(TSK_Handle task);
extern Void TSK_deleteFxn(TSK_Handle task);
extern Void TSK_exitFxn(Void);
extern Void TSK_readyFxn(TSK_Handle task);
extern Void TSK_settime(TSK_Handle task);
extern Void TSK_deltatime(TSK_Handle task);
extern STS_Handle TSK_getsts(TSK_Handle task);
extern Void TSK_resettime(TSK_Handle task);
extern Void TSK_switchFxn(KNL_Handle oldtask, KNL_Handle newtask);
extern Ptr  _TSK_getenv(TSK_Handle task);

/*
 * ======== TSK_getenv ========
 */
#ifdef  _INLINE

/*
 * Defined as a macro rather than an inline function because the 3.2.2 
 * 55x codegen tools generated better code using a macro than 
 * an inline of _TSK_getenv()
 */ 

#define TSK_getenv(x) \
    ( (_HOOK_NUMHOOKS == 0) ? (x)->environ : ((Ptr *)(x)->environ)[_HOOK_knlId] )
 
#else // _INLINE 

/*
 * The BIOS library version is used when optimizations are off
 * because the unoptimized code generated for the macro is much 
 * worse than the (optimized with -o2) BIOS library version.
 */

#define TSK_getenv(x) _TSK_getenv(x)

#endif // _INLINE

#ifdef _BIOSROM_

extern TSK_Handle (*TSK_createPtr)(Fxn fxn, TSK_Attrs *attrs, ...);
extern Void (*TSK_deletePtr)(TSK_Handle task);

/* can't include args for TSK_createJ since TSK_create is varargs */
#define TSK_createJ             (TSK_createPtr)
#define TSK_deleteJ(task)       (TSK_deletePtr)(task)

#else

/* can't include args for TSK_createJ since TSK_create is varargs */
#define TSK_createJ             TSK_create
#define TSK_deleteJ(task)       TSK_deletePtr(task)

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* TSK_ */
