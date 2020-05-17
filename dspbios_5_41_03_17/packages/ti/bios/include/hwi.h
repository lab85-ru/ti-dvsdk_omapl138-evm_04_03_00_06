/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== hwi.h ========
 *
 */
#ifndef HWI_
#define HWI_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HWI_DataObj {
#ifdef _55_
    LgUns       spsave;
    LgUns       sysspsave;
    LgUns       stkBottom;
    LgUns       sysstkBottom;
    LgUns       stkTop;
    LgUns       sysstkTop;
#else
    Uns         spsave;
    Uns         stkBottom;
    Uns         stkTop;
#endif
    Uns         inhwi;
} HWI_DataObj;

#ifdef _6x_
extern far HWI_DataObj HWI_data;
#else
extern HWI_DataObj HWI_data;
#endif

extern Uns       HWI_D_inhwi;

#if defined(_6x_)

typedef struct HWI_Attrs {
    Uns         intrMask;       /* IER bitmask, 1="self" (default) */
    Uns         ccMask;         /* CSR CC bitmask, 1="leave alone" (default) */
    Arg         arg;            /* fxn arg (default=0)*/
} HWI_Attrs;

Void HWI_dispatchPlug(Int vecid, Fxn fxn, Int dmachan, HWI_Attrs *attrs);
extern HWI_Attrs HWI_ATTRS;

/*
 *  ======== HWI_eventMap ========
 *  binds the specified event to the specified vector.
 */
extern Void HWI_eventMap(Int vectid, Int eventid);

#if defined(_2430_) || defined(_3430_)
extern Void HWI_applyWugenMasks(Uint32 mask[]);
extern Void HWI_disableWugen(Int eventid);
extern Void HWI_enableWugen(Int eventid);
extern Void HWI_getWugenMasks(Uint32 mask[]);
extern Void HWI_ierToWugenMasks(Uint32 mask[]);
#endif

#endif

#if defined(_54_)

typedef struct HWI_Attrs {
    Uns         intrMask;       /* IMR bitmask, 1 = "self" (default) */
    Arg         arg;            /* fxn arg (default = 0)*/
} HWI_Attrs;

Void HWI_dispatchPlug(Int vecid, Fxn fxn, HWI_Attrs *attrs);

extern HWI_Attrs HWI_ATTRS;

#endif
#if defined(_28_)

typedef struct HWI_Attrs {
    Uns         iermask;        /* IMR bitmask, 1 = "self" (default) */
    Arg         arg;            /* fxn arg (default = 0)*/
} HWI_Attrs;

Void HWI_dispatchPlug(Int vecid, Fxn fxn, HWI_Attrs *attrs);
extern HWI_Attrs HWI_ATTRS;

#endif

#if defined(_55_)

typedef struct HWI_Attrs {
    Uns         ier0mask;       /* IER0 bitmask */
    Uns         ier1mask;       /* IER1 bitmask */
    Arg         arg;            /* fxn arg (default = 0) */
    LgUns       mirmask;        /* Level 2 interrupt mask */
                                /* only used in 2320/2420 applications */
    LgUns       mir1mask;       /* Level 2 interrupt mask1 */
                                /* only used in 2320 applications */
} HWI_Attrs;

Void HWI_dispatchPlug(Int vecid, Fxn fxn, HWI_Attrs *attrs);
extern HWI_Attrs HWI_ATTRS;

#endif

/*
 *  ======== HWI_isHWI ========
 *  Returns true if current thread is a HWI.
 */
#define HWI_isHWI()             (HWI_D_inhwi != 0)

/*
 *  ======== HWI_disable ========
 *  Disable interrupts
 */
extern Uns HWI_disable(Void);

/*
 *  ======== HWI_enable ========
 *  Enable interrupts
 */
extern Void HWI_enable(Void);

/*
 *  ======== HWI_restore ========
 *  Restore interrupts state
 */
extern Void HWI_restore(Uns old);

/*
 *  ======== HWI_disableJ ========
 *  ======== HWI_enableJ ========
 *  ======== HWI_restoreJ ========
 *  Re-vectorable (for ROM) Disable/Enable/Restore interrupt functions
 */


#ifdef _BIOSROM_

extern Uns (*HWI_disablePtr)(Void);
extern Void (*HWI_enablePtr)(Void);
extern Void (*HWI_restorePtr)(Uns old);

#define HWI_disableJ()          (HWI_disablePtr)()
#define HWI_enableJ()           (HWI_enablePtr)()
#define HWI_restoreJ(old)       (HWI_restorePtr)(old)

#else

#define HWI_disableJ()          HWI_disable()
#define HWI_enableJ()           HWI_enable()
#define HWI_restoreJ(old)       HWI_restore(old)

#endif

/*
 *  ======== HWI_disableI ========
 *  ======== HWI_enableI ========
 *  ======== HWI_restoreI ========
 *  Internally used Disable/Enable/Restore interrupt functions
 *  that use intrinsics when supported by codegen tools.
 */

#if defined(_55_) || defined(_6x_)

#define HWI_disableI()          _disable_interrupts()
#define HWI_enableI()           _enable_interrupts()
#define HWI_restoreI(old)       _restore_interrupts(old)

#else

#define HWI_disableI()          HWI_disableJ()
#define HWI_enableI()           HWI_enableJ()
#define HWI_restoreI(old)       HWI_restoreJ(old)

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* HWI_ */
