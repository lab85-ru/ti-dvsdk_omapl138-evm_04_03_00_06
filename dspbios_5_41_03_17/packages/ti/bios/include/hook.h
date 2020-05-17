/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 * ======== hook.h ========
 *
 */

#ifndef HOOK_
#define HOOK_

#include <std.h>

#include <tsk.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef Int HOOK_Id;

typedef Void (*HOOK_InitFxn)(HOOK_Id id);
typedef Void (*HOOK_CreateFxn)(TSK_Handle task);
typedef Void (*HOOK_DeleteFxn)(TSK_Handle task);
typedef Void (*HOOK_ExitFxn)(Void);
typedef Void (*HOOK_ReadyFxn)(TSK_Handle task);
typedef Void (*HOOK_SwitchFxn)(TSK_Handle prev, TSK_Handle next);

extern Int _HOOK_knlId;

extern Int _HOOK_NUMHOOKS;
extern HOOK_InitFxn _HOOK_initFxnTable[];
extern HOOK_CreateFxn _HOOK_createFxnTable[];
extern HOOK_DeleteFxn _HOOK_deleteFxnTable[];
extern HOOK_ExitFxn _HOOK_exitFxnTable[];
extern HOOK_ReadyFxn _HOOK_readyFxnTable[];
extern HOOK_SwitchFxn _HOOK_switchFxnTable[];

Void HOOK_setenv(TSK_Handle task, HOOK_Id id, Ptr env);
Ptr HOOK_getenv(TSK_Handle task, HOOK_Id id);

#ifdef _BIOSROM_

extern Void (*HOOK_setenvPtr)(TSK_Handle task, HOOK_Id id, Ptr env);
extern Ptr (*HOOK_getenvPtr)(TSK_Handle task, HOOK_Id id);

#define HOOK_setenvJ(task, id, env) (HOOK_setenvPtr)(task, id, env)
#define HOOK_getenvJ(task, id)  (HOOK_getenvPtr)(task, id)

#else

#define HOOK_setenvJ(task, id, env) HOOK_setenv(task, id, env)
#define HOOK_getenvJ(task, id)  HOOK_getenv(task, id)

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* HOOK_ */
