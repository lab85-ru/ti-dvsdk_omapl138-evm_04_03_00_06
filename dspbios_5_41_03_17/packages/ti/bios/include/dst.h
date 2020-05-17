/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dst.h ========
 *  Stacking "Split" Device
 *
 */

#ifndef DST_
#define DST_

#include <dev.h>
#include <fxn.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DST_init        ((Fxn)FXN_F_nop)

extern DEV_Fxns DST_FXNS;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* DST_ */
