/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== fxn.h ========
 *
 */


#ifndef FXN_
#define FXN_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== FXN_Obj ========
 */

typedef struct FXN_Obj {
    Fxn fxn;
    Arg arg1;
    Arg arg0;
#if defined(_54_) 
    Bool cfxn;          /* extra glue is needed for 54x C functions */
#endif
} FXN_Obj;

typedef struct FXN_Obj *FXN_Handle;

extern Void FXN_F_nop();

#ifdef _BIOSROM_

extern Void (*FXN_F_nopPtr)(Void);

#define FXN_F_nopJ()            (FXN_F_nopPtr)()

#else

#define FXN_F_nopJ()            FXN_F_nop()

#endif

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* FXN_ */
