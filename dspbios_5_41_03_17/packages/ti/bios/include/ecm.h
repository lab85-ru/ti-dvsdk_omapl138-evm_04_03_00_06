/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== ecm.h ========
 *
 */

#ifndef ECM_
#define ECM_

#include <std.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef Void (*ECM_Fxn) (Arg);

typedef struct ECM_Attrs {
    Arg arg;                    /* function argument */
    Bool unmask;                /* unmask == 1 means enable the event */
} ECM_Attrs;

extern far ECM_Attrs ECM_ATTRS;

extern Void ECM_disableEvent(Uns eventId);
extern Void ECM_dispatchPlug(Uns eventId, ECM_Fxn fxn, ECM_Attrs *attrs);
extern Void ECM_dispatch(Uns eventId);
extern Void ECM_enableEvent(Uns eventId);

#ifdef __cplusplus
}
#endif  /* extern "C" */

#endif
