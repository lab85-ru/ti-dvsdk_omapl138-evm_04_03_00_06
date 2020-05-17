/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== hst.h ========
 *
 */

#ifndef HST_
#define HST_

#include <std.h>

#include <pip.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HST_Obj {
    Uns         mask;
    PIP_Obj     *pipe;
} HST_Obj;

typedef struct HST_Obj *HST_Handle;

extern PIP_Handle HST_getpipe(HST_Handle hst);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* HST_ */
