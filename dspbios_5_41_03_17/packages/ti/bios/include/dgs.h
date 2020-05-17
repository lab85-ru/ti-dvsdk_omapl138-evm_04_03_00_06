/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dgs.h ========
 *  Stacking Gather/Scatter Device Module Interface
 *
 */

#ifndef DGS_
#define DGS_


#include <dev.h>
#include <fxn.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DGS_init        ((Fxn)FXN_F_nop)

extern DEV_Fxns DGS_FXNS;

/*
 *  ======== DGS_Params ========
 */
typedef struct DGS_Params {     /* device parameters */
    Fxn         createFxn;
    Fxn         deleteFxn;
    Fxn         transFxn;
    Arg         arg;
    Int         num;
    Int         den;
} DGS_Params;

extern Int u32tou8();
extern Int u8tou32();

extern Int u16tou32();
extern Int u32tou16();

extern Int i16toi32();
extern Int i32toi16();

extern Int i16tof32();
extern Int f32toi16();

extern Int u8toi16();
extern Int i16tou8();

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* DGS_ */
