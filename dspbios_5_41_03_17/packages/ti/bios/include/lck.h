/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== lck.h ========
 *  resource lock module - (was IR in 1.4)
 *
 */

#ifndef LCK_
#define LCK_

#include <sem.h>
#include <sys.h>
#include <tsk.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LCK_Obj {
    SEM_Obj     sem;            /* use SEM_new() to initialize */
    Int         value;          /* current non-negative lock value */
    TSK_Handle  owner;          /* task handle of owner */
} LCK_Obj, *LCK_Handle;

typedef struct LCK_Attrs {
    Int dummy;
} LCK_Attrs;

extern LCK_Attrs        LCK_ATTRS;

extern LCK_Handle       LCK_SYSTEM;     /* global system lock */

/*
 *  ======== LCK_create ========
 */
extern LCK_Handle LCK_create(LCK_Attrs *attrs);

/*
 *  ======== LCK_delete ========
 */
extern Void LCK_delete(LCK_Handle lock);

/*
 *  ======== LCK_init ========
 */
extern Void LCK_init();

/*
 *  ======== LCK_pend ========
 */
extern Bool LCK_pend(LCK_Handle lock, Uns timeout);

/*
 *  ======== LCK_post ========
 */
extern Void LCK_post(LCK_Handle lock);

/*
 *  ======== LCK_release ========
 *  macro for backwards compatibility -- LCK_post() is recommended
 */
#define LCK_release(lock)       LCK_post((lock))

/*
 *  ======== LCK_seize ========
 *  macro for backwards compatibility -- LCK_pend() is recommended
 */
#define LCK_seize(lock)         LCK_pend((lock), SYS_FOREVER)

#ifdef _BIOSROM_

extern LCK_Handle (*LCK_createPtr)(LCK_Attrs *attrs);
extern Void (*LCK_deletePtr)(LCK_Handle lock);
extern Bool (*LCK_pendPtr)(LCK_Handle lock, Uns timeout);
extern Void (*LCK_postPtr)(LCK_Handle lock);

#define LCK_createJ(attrs)              (LCK_createPtr)(attrs)
#define LCK_deleteJ(lock)               (LCK_deletePtr)(lock)
#define LCK_pendJ(lock, timeout)        (LCK_pendPtr)(lock, timeout)
#define LCK_postJ(lock)                 (LCK_postPtr)(lock)

#else

#define LCK_createJ(attrs)              LCK_create(attrs)
#define LCK_deleteJ(lock)               LCK_delete(lock)
#define LCK_pendJ(lock, timeout)        LCK_pend(lock, timeout)
#define LCK_postJ(lock)                 LCK_post(lock)

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* LCK_ */
