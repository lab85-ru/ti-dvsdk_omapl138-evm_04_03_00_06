/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== knl.h ========
 *
 */


#ifndef KNL_
#define KNL_

#include <atm.h>
#include <fxn.h>
#include <que.h>
#include <stdarg.h>
#include <sts.h>
#include <swi.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KNL_IDLEPRI (0)     /* idle task priority (should not block) */
#define KNL_MINPRI  (1)     /* minimum task priority */
#define KNL_MAXPRI  (15)    /* maximum task priority */

/*
 * Warning! KNL_SWIMASK and KNL_CURMASK must match their counterparts in knl.hti
 */
#define KNL_SWIMASK (0x1)   /* priority mask of KNL_swi */
#define KNL_CURMASK (0x2)   /* SWI_D_curmask value when inside the kernel */

#define KNL_RUNASYNC    (0)
#define KNL_RUNSYNC     (1)

#define KNL_RUNNING     (0)
#define KNL_READY       (1)
#define KNL_BLOCKED     (2)
#define KNL_TERMINATED  (3)

#define KNL_MAXARGS (8)

typedef struct KNL_Obj KNL_Obj;
typedef struct KNL_Obj *KNL_Handle;

struct KNL_Obj {
    QUE_Elem    ready;          /* ready/semaphore queue elem */
    QUE_Elem    alarm;          /* alarm queue elem */
    QUE_Elem    setpri;         /* set priority queue elem */
    QUE_Handle  queue;          /* task's ready queue */
    volatile Int priority;      /* task priority */
    Uns         mask;           /* 1 << priority */
    Ptr         sp;             /* current stack pointer */
    Uns         timeout;        /* timeout value */
    Int         mode;           /* KNL_BLOCKED, KNL_READY,... */
    /*
     *  This STS Object is used to measure the time from when the task is
     *  made ready, until the function TSK_deltatime() is called.  When the
     *  task is made ready, the STS object is "set" to the current time.
     */  
    STS_Obj     *sts;
    Bool        signalled;      /* pending task waken up by sem or timeout */
};

/*
 *  This struct overlays the fields in a KNL_Obj struct.
 *  It is used by KNL_tick() and SEM_pend() to find the timeout
 *  values for tasks on the alarm queue.
 */
typedef struct KNL_AObj KNL_AObj;
struct KNL_AObj {
    QUE_Elem    alarm;
    QUE_Elem    setpri;
    QUE_Handle  queue;
    volatile Int priority;
    Uns         mask;
    Ptr         sp;
    Uns         timeout;
    Int         mode;
    STS_Obj     *sts;
    Bool        signalled;
};

typedef struct KNL_Job *KNL_JobHandle;
typedef Void (*KNL_Fxn)(KNL_JobHandle);

typedef struct KNL_Job {
    QUE_Obj     wListElem;      /* KNL_wList queue */
    Int         wCount;         /* number of posts when on KNL_wList */
    KNL_Fxn     fxn;            /* function that services the job */
} KNL_Job;

/* Can be modified from an ISR and must be declared as volatile */
extern volatile Bool KNL_wFlag;         /* defined in knl.c */

#define KNL_NUMQUEUES ((KNL_MAXPRI + 5))

extern QUE_Obj KNL_queues[KNL_NUMQUEUES];

/* Can be modified from an ISR and must be declared as volatile */
extern volatile QUE_Handle KNL_wList;   /* defined in knl.c */
extern QUE_Handle KNL_alarm;
extern QUE_Handle KNL_setpri;
extern QUE_Handle KNL_inactive;
extern QUE_Handle KNL_priority;

extern Uns KNL_set;

extern KNL_Obj KNL_dummy;
extern KNL_Handle KNL_curtask;
extern KNL_Handle KNL_prevtask;

extern volatile Uns KNL_curtime;

extern SWI_Obj *KNL_swiptr;

extern QUE_Handle KNL_curqueue;
extern QUE_Handle KNL_maxqueue;

extern Fxn KNL_tswitchfxn;
extern Fxn KNL_tcreatefxn;
extern Fxn KNL_tdeletefxn;
extern Fxn KNL_treadyfxn;
extern Fxn KNL_idlefxn;

/*
 *  ======== KNL_block ========
 */
#define KNL_block()\
    QUE_dequeue(KNL_curqueue);\
    KNL_curtask->mode = KNL_BLOCKED;\
    if (QUE_empty(KNL_curqueue)) {\
        KNL_set &= ~(KNL_curtask->mask);\
    }\
    KNL_curqueue = NULL;\
    KNL_maxqueue = &KNL_priority[KNL_max()]

/*
 *  ======== KNL_busy ========
 *  The KNL_busy() macro returns TRUE if the SWI scheduler is blocked or if
 *  SWI_D_curmask > KNL_SWIMASK, when an SWI is the current process or we're
 *  already inside the scheduler. Thus, KNL_busy prevents us from entering the
 *  scheduler when inside an ISR, an SWI, or the kernel itself.
 */
#define KNL_busy()      ((SWI_data.lock > -1) || (SWI_data.curmask > KNL_SWIMASK))

/*
 *  ======== KNL_init ========
 */
#define KNL_init        FXN_F_nop

/*
 *  ======== KNL_yield ========
 */
#define KNL_yield()\
    QUE_enqueue(KNL_curqueue, QUE_dequeue(KNL_curqueue));

extern Void KNL_enter(void);
extern Void KNL_exit(void);
extern Void KNL_idle(void);
extern Int KNL_max(void);
extern Void KNL_ready(KNL_Handle task);
#ifdef _55_
extern Ptr KNL_start(Ptr userStack, Ptr systemStack, Fxn fxn, va_list argv, Fxn exit, Fxn entry);
#else
extern Ptr KNL_start(Ptr stack, Fxn fxn, va_list argv, Fxn exit, Fxn entry);
#endif
extern Void KNL_switch(Ptr *oldsp, Ptr *newsp);
extern Void KNL_post(KNL_JobHandle job);
extern Void KNL_ipost(KNL_JobHandle job);
extern Void KNL_tick(void);
extern Void KNL_saveIsrFrame(void);

#ifdef _BIOSROM_

extern Void (*KNL_enterPtr)(Void);
extern Void (*KNL_exitPtr)(Void);
extern Void (*KNL_ipostPtr)(KNL_JobHandle job);
extern Void (*KNL_postPtr)(KNL_JobHandle job);
extern Void (*KNL_readyPtr)(KNL_Handle task);
extern Void (*KNL_runPtr)(Arg mode_arg);
extern Void (*KNL_switchPtr)(Ptr *oldsp, Ptr *newsp);

#define KNL_enterJ()                    (KNL_enterPtr)()
#define KNL_exitJ()                     (KNL_exitPtr)()
#define KNL_ipostJ(job)                 (KNL_ipostPtr)(job)
#define KNL_postJ(job)                  (KNL_postPtr)(job)
#define KNL_readyJ(task)                (KNL_readyPtr)(task)
#define KNL_runJ(mode)                  (KNL_runPtr)(mode)
#define KNL_switchJ(oldsp, newsp)       (KNL_switchPtr)(oldsp, newsp)

#else

#define KNL_enterJ()                    KNL_enter()
#define KNL_exitJ()                     KNL_exit()
#define KNL_ipostJ(job)                 KNL_ipost(job)
#define KNL_postJ(job)                  KNL_post(job)
#define KNL_readyJ(task)                KNL_ready(task)
#define KNL_runJ(mode)                  KNL_run(mode)
#define KNL_switchJ(oldsp, newsp)       KNL_switch(oldsp, newsp)

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /*KNL_*/
