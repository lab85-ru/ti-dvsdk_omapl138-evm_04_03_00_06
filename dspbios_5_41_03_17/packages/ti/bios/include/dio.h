/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dio.h ========
 *
 */

#ifndef DIO_
#define DIO_

#include <std.h>

#include <dev.h>
#include <sem.h>

#include <iom.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Function table to be use with dynamically created TSK
 */
extern DEV_Fxns DIO_tskDynamicFxns;

/*
 * Function table to be use with statically created TSK
 */
extern DEV_Fxns DIO_tskStaticFxns;

/*
 * Function table to be use with dynamically created callback thread
 */
extern DEV_Fxns DIO_cbDynamicFxns;

/*
 * Function table to be use with statically created callback thread
 */
extern DEV_Fxns DIO_cbStaticFxns;

/*
 *  ======== DIO_Params ========
 */
typedef struct DIO_Params {
    String      name;           /* name of mini driver */
    Ptr         chanParams;     /* optional mini driver create argument */
} DIO_Params;

/*
 * ======== DIO_Obj ========
 * In TSK version, there will contain a complete and ready semaphore.
 * In callback version, there will be a DEV_Callback object which
 * contains a function and two arguments
 */
typedef struct DIO_Obj {        
    IOM_Fxns    *fxns;                  /* mini-driver function table */
    Ptr         chanp;                  /* mini-driver handle */

    union context {
        struct sems {
            SEM_Handle  complete;       /* tasks wait for I/O here */
            SEM_Handle  ready;          /* used by DIO_ready */
        } sems; 
        DEV_Callback    cb;             /* use by callback version */
    } context;
} DIO_Obj;

typedef DIO_Obj *DIO_Handle;            /* device object handle */

extern Void DIO_init(Void);
extern Int  DIO_ctrl(DEV_Handle device, Uns cmd, Arg arg);
extern Int  DIO_cbIdle(DEV_Handle device, Bool flush);
extern Int  DIO_cbIssue(DEV_Handle device);
extern Bool DIO_cbReady(DEV_Handle device, SEM_Handle sem);
extern Int  DIO_cbReclaim(DEV_Handle device);
extern Void DIO_cbCallback(Ptr devp, DEV_Frame *frame);
extern Int  DIO_tskIdle(DEV_Handle device, Bool flush);
extern Int  DIO_tskIssue(DEV_Handle device);
extern Bool DIO_tskReady(DEV_Handle device, SEM_Handle sem);
extern Int  DIO_tskReclaim(DEV_Handle device);
extern Void DIO_tskCallback(Ptr devp, DEV_Frame *frame);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* DIO_ */

