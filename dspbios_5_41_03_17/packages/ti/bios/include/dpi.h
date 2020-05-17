/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dpi.h ========
 *  New Pipe Module Interface
 *
 */

#ifndef DPI_
#define DPI_

#include <dev.h>

#ifdef __cplusplus
extern "C" {
#endif

extern  Void        DPI_init();

extern  DEV_Fxns    DPI_FXNS;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* DPI_ */
