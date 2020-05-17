/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== prd.h ========
 *
 */

#ifndef PRD_
#define PRD_

#include <fxn.h>
#include <sts.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PRD_Obj {
    Uns         status;
    Uns         kount;
    Uns         period;
    Uns         nticks;
    FXN_Obj     fxnobj;
    STS_Obj     *sts;
#if (defined(_54_) && defined(_FAR_MODE))
    Uns         pad;
#endif
} PRD_Obj;

typedef struct PRD_Obj *PRD_Handle;

extern Uns PRD_D_tick; /* low 16bits of PRD tick counter - used by SPOX */
                       /* SEM_pend() to record how long to pend for     */
                       /* acquiring semaphore before calling timeout    */

extern LgUns PRD_getticks(Void);
extern Void PRD_tick(Void);
extern Void PRD_start(PRD_Handle prd);
extern Void PRD_stop(PRD_Handle prd);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* PRD_ */
