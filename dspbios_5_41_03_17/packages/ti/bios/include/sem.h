/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== sem.h ========
 *
 */

#ifndef SEM_
#define SEM_

#include <knl.h>
#include <obj.h>
#include <que.h>
#include <sys.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SEM_Obj ========
 *!
 *! Warning!  If any fields are added here, be sure to change COUNT in
 *! sem_dec.s##.
 */

typedef struct SEM_Obj  *SEM_Handle;
typedef Void (*SEM_Fxn)(SEM_Handle);

typedef struct SEM_Obj {
    KNL_Job     job;            /* handles interrupt-level posting */
    Int         count;          /* semaphore value */
    QUE_Obj     pendQ;          /* tasks wait here */
    String      name;           /* printable name */
#ifdef PRIORITY
    Uns         type;       /* bit mask SEM_PRIORITY (0x1) or SEM_FIFO (0x0) */
#endif
} SEM_Obj;

typedef struct SEM_Attrs {
    Uns         type;
    String      name;       /* printable name */
} SEM_Attrs;

#define SEM_OBJECT(obj, count)  SEM_Obj obj = { \
    {                           /* job */ \
        { NULL, NULL },         /* wListElem */ \
        0,                      /* wCount */ \
        (KNL_Fxn)_SEM_dopost,   /* fxn */ \
    }, \
    count,                      /* count */ \
    { &obj.pendQ, &obj.pendQ }, /* pendQ */ \
    NULL                        /* name  */ \
}

#ifdef PRIORITY
/*
 * bit mask for SEM_Attrs 'type' field -- default is (0x0)
 */
#define SEM_FIFO        0x0
#define SEM_PRIORITY    0x1
#endif

extern SEM_Attrs        SEM_ATTRS;      /* declared in sem_crea.c */

#define SEM_count(sem)          ((sem)->count + (sem)->job.wCount)

#define SEM_reset(sem, cnt)     ((sem)->count = (cnt))

extern SEM_Handle SEM_create(Int count, SEM_Attrs *attrs);
extern Void SEM_delete(SEM_Handle sem);
extern Void SEM_ipost(SEM_Handle sem);
extern Bool SEM_pend(SEM_Handle sem, Uns timeout);
extern Void SEM_post(SEM_Handle sem);
extern Void SEM_print(SEM_Handle sem);

/*
extern Bool _SEM_dec(SEM_Handle sem);
*/
extern Void _SEM_dopost(SEM_Handle sem);

static inline void SEM_new(SEM_Handle sem, Int count) 
{
    sem->count = count;
    QUE_new(&(sem->pendQ));
    sem->job.wCount = 0;
    sem->job.fxn = (KNL_Fxn)_SEM_dopost;
}

/*
 *  Binary semaphore APIs.
 *
 *  Binary semphores use same SEM_Obj as standard SEM_pend/post() functions,
 *  but require user to use SEM_pendBinary/postBinary() APIs as a pair.  Mixing
 *  APIs (e.g. SEM_post/SEM_pendBinary()) is not supported.
 */
extern Bool SEM_pendBinary(SEM_Handle sem, Uns timeout);
extern Void SEM_postBinary(SEM_Handle sem);

/*
 *  The definitions below are needed as these are used by
 *  the expanded version of KNL_exit in SEM_postEnterKnl and SEM_pendEnterKnl
 */

#ifdef _6x_
extern far SWI_Obj KNL_swi;
extern far FXN_Obj *SWI_D_curfxn;
#else
extern SWI_Obj KNL_swi;
extern FXN_Obj *SWI_D_curfxn;
#endif

extern Uns SWI_D_curmask;

#ifdef _BIOSROM_

extern Bool (*SEM_pendPtr)(SEM_Handle sem, Uns timeout);
extern Void (*SEM_postPtr)(SEM_Handle sem);
extern SEM_Handle (*SEM_createPtr)(Int count, SEM_Attrs *attrs);
extern Void (*SEM_deletePtr)(SEM_Handle sem);
extern Void (*SEM_ipostPtr)(SEM_Handle sem);

#define SEM_pendJ(sem, timeout)         (SEM_pendPtr)(sem, timeout)
#define SEM_postJ(sem)                  (SEM_postPtr)(sem)
#define SEM_createJ(count, attrs)       (SEM_createPtr)(count, attrs)
#define SEM_deleteJ(sem)                (SEM_deletePtr)(sem)
#define SEM_ipostJ(sem)                 (SEM_ipostPtr)(sem)

#else

#define SEM_pendJ(sem, timeout)         SEM_pend(sem, timeout)
#define SEM_postJ(sem)                  SEM_post(sem)
#define SEM_createJ(count, attrs)       SEM_create(count, attrs)
#define SEM_deleteJ(sem)                SEM_delete(sem)
#define SEM_ipostJ(sem)                 SEM_ipost(sem)

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* SEM_ */
