/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dnl.h ========
 *  Null Device Module Interface
 *
 */

#ifndef DNL_
#define DNL_


#include <dev.h>
#include <fxn.h>

#ifdef __cplusplus
extern "C" {
#endif

extern DEV_Fxns DNL_FXNS;

#define DNL_init    ((Fxn)FXN_F_nop)

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* DNL_ */
