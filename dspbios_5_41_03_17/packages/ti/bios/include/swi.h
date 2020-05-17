/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== swi.h ========
 *
 */

#ifndef SWI_
#define SWI_

#include <fxn.h>
#include <sts.h>
#include <hwi.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SWI_MINPRI      1
#define SWI_MAXPRI      14

#define SWI_getpri(swi) ( (swi)->mask )

typedef Void (*SWI_Fxn) (Arg,Arg); 

/*
 * Structure for SWI global data.  This definition is used by
 * assembly code (using .cdecls support) as well as C.
 */
typedef struct SWI_DataObj {
    Fxn         runfxn;
    Fxn         execfxn;
    Uns         curmask;
    Uns         curset;
    Int         lock;
    Uns         curmbox;
    Ptr         curfxn;
    Ptr         rdybeg;
    Uns         inswi;
} SWI_DataObj;

#ifdef _6x_
extern far SWI_DataObj SWI_data;
#else
extern SWI_DataObj SWI_data;
#endif

/*
 * Had to rename the structure name in order to use
 * the C structure template from assembly code.
 */
typedef struct SWI_Obj {
    Int         lock;
    Ptr         ready;
    Uns         mask;
    Ptr         link;
    Uns         initkey;
    Uns         mailbox;
    FXN_Obj     fxnobj;
    Int         stslock;
    STS_Obj     *sts;           /* pointer to STS_Obj */
} SWI_Obj;

typedef struct SWI_Obj *SWI_Handle;

typedef struct SWI_Attrs {
    SWI_Fxn     fxn;
    Arg         arg0;
    Arg         arg1;
#if defined(_54_)
    Bool        iscfxn;
#endif
    Int         priority;
    Uns         mailbox;
} SWI_Attrs;

extern SWI_Attrs        SWI_ATTRS;      /* declared in swi_recr.c */

extern Uns      SWI_D_curmask;
extern Uns      SWI_D_inswi;
extern Int      SWI_D_lock;

/*
 *  ======== SWI_isSWI ========
 *  Returns true if current thread is a SWI.
 */
#define SWI_isSWI()             ((HWI_D_inhwi == 0) && (SWI_D_inswi != 0))

extern Void     SWI_andn(SWI_Handle swi, Uns key);
extern Void     SWI_dec(SWI_Handle swi);
extern Void     SWI_disable(Void);
extern Void     SWI_enable(Void);
extern Void     SWI_inc(SWI_Handle swi);
extern Void     SWI_or(SWI_Handle swi, Uns key);
extern Void     SWI_post(SWI_Handle swi);
extern Uns      SWI_getmbox(Void);
extern SWI_Handle SWI_self(Void);
extern Uns      SWI_raisepri(Uns mask);
extern Void     SWI_restorepri(Uns key);

extern SWI_Handle SWI_create(SWI_Attrs *attrs);
extern Void     SWI_new(SWI_Handle swihandle, SWI_Attrs *attrs);
extern Void     SWI_delete(SWI_Handle swihandle);
extern Void     SWI_getattrs(SWI_Handle swihandle, SWI_Attrs *attrs);
extern Void     SWI_setattrs(SWI_Handle swihandle, SWI_Attrs *attrs);

/*
 * Below two SWI hook functions are primarily needed to hide
 * the Argument/register type nuances arising from C55xx codegen
 * conventions (Pointers use [X]ARs while Integers go on T0/T1).
 * However, to keep the generality, we have added these two
 * distinct function prototypes for use in calling user specified
 * C functions in association w/ Bios objects - like say the
 * PIP notifier functions.
 *
 * For C54x and C6xx, the below two prototypes simply alias with
 * their corresponding vanilla version viz., SWI_andn() and SWI_or()
 *
 */
extern Void     SWI_andnHook(Arg swi, Arg key);
extern Void     SWI_orHook(Arg swi, Arg key);

#ifdef _BIOSROM_

extern Void (*SWI_andnPtr)(SWI_Handle swi, Uns key);
extern Void (*SWI_decPtr)(SWI_Handle swi);
extern Void (*SWI_disablePtr)(Void);
extern Void (*SWI_enablePtr)(Void);
extern Uns  (*SWI_getmboxPtr)(Void);
extern Void (*SWI_incPtr)(SWI_Handle swi);
extern Void (*SWI_orPtr)(SWI_Handle swi, Uns key);
extern Void (*SWI_postPtr)(SWI_Handle swi);
extern Uns  (*SWI_raisepriPtr)(Uns mask);
extern Void (*SWI_restorepriPtr)(Uns key);
extern SWI_Handle (*SWI_selfPtr)(Void);

extern SWI_Handle (*SWI_createPtr)(SWI_Attrs *attrs);
extern Void (*SWI_deletePtr)(SWI_Handle swihandle);
extern Void (*SWI_getattrsPtr)(SWI_Handle swihandle, SWI_Attrs *attrs);
extern Void (*SWI_setattrsPtr)(SWI_Handle swihandle, SWI_Attrs *attrs);

#define SWI_andnJ(swi, key)     (SWI_andnPtr)(swi, key)
#define SWI_decJ(swi)           (SWI_decPtr)(swi)
#define SWI_disableJ()          (SWI_disablePtr)()
#define SWI_enableJ()           (SWI_enablePtr)()
#define SWI_getmboxJ()          (SWI_getmboxPtr)()
#define SWI_incJ(swi)           (SWI_incPtr)(swi)
#define SWI_orJ(swi, key)       (SWI_orPtr)(swi, key)
#define SWI_postJ(swi)          (SWI_postPtr)(swi)
#define SWI_raisepriJ(mask)     (SWI_raisepriPtr)(mask)
#define SWI_restorepriJ(key)    (SWI_restorepriPtr)(key)
#define SWI_selfJ()             (SWI_selfPtr)()

#define SWI_createJ(attrs)      (SWI_createPtr)(attrs)
#define SWI_deleteJ(swihandle)  (SWI_deletePtr)(swihandle)
#define SWI_getattrsJ(swihandle, attrs) (SWI_getattrsPtr)(swihandle, attrs)
#define SWI_setattrsJ(swihandle, attrs) (SWI_setattrsPtr)(swihandle, attrs)

#else

#define SWI_andnJ(swi, key)     SWI_andn(swi, key)
#define SWI_decJ(swi)           SWI_dec(swi)
#define SWI_disableJ()          SWI_disable()
#define SWI_enableJ()           SWI_enable()
#define SWI_getmboxJ()          SWI_getmbox()
#define SWI_incJ(swi)           SWI_inc(swi)
#define SWI_orJ(swi, key)       SWI_or(swi, key)
#define SWI_postJ(swi)          SWI_post(swi)
#define SWI_raisepriJ(mask)     SWI_raisepri(mask)
#define SWI_restorepriJ(key)    SWI_restorepri(key)
#define SWI_selfJ()             SWI_self()

#define SWI_createJ(attrs)      SWI_create(attrs)
#define SWI_deleteJ(swihandle)  SWI_delete(swihandle)
#define SWI_getattrsJ(swihandle, attrs) SWI_getattrs(swihandle, attrs)
#define SWI_setattrsJ(swihandle, attrs) SWI_setattrs(swihandle, attrs)

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* SWI_ */



