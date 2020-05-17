/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dhs.h ========
 *  Host-Link driver header.
 *
 *  This driver is used for streaming data between the host and the DSP,
 *  using an underlying HST object.
 *
 */

#ifndef DHS_
#define DHS_

#include <dev.h>
#include <sem.h>
#include <hst.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  DHS Object and Handle.  This device uses an HST object with an underlying
 *  PIP object, for streaming data between the DSP and the host.
 *  The "notify" function for this HST object is DHS_interrupt.
 *
 */
typedef struct DHS_Obj {
    /*
     *  Flag set to TRUE after first DHS_io operation for input device.
     *  This is to ensure we don't call PIP_free() before the first PIP_get().
     */
    Bool        started;        /* Make PIP_free() happen before PIP_get() */

    SEM_Obj     pipreadySem;    /* Count > 0 if frame available in pipe. */
    Int         fromdevCnt;     /* Number of frames on fromdevice queue */
    HST_Obj     *hst;           /* Underlying HST object */
    SEM_Handle  ready;          /* Temporary semaphore used by DHS_ready() */
    DEV_Handle  device;         /* Device handle - to get at queues */
} DHS_Obj, *DHS_Handle;


extern DEV_Fxns DHS_FXNS;

extern Void DHS_interrupt(DHS_Obj *dhs);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* DHS_ */
