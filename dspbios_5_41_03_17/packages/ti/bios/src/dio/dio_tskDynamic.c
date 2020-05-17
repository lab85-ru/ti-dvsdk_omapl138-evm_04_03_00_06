/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dio_tskDynamic.c ========
 *  Dynamic SEM based implementation of DIO.
 * 
 */

#include <std.h>

#include <dev.h>
#include <mem.h>
#include <sem.h>
#include <sys.h>

#include <iom.h>
#include <dio.h>

static Int      DIO_tskDynamicClose(DEV_Handle device);
static Int      DIO_tskDynamicOpen(DEV_Handle device, String name);

/*
 *  Dynamic SEM based version of function table.
 */
DEV_Fxns DIO_tskDynamicFxns = {
    DIO_tskDynamicClose,        /* close */
    DIO_ctrl,                   /* ctrl */
    DIO_tskIdle,                /* idle */
    DIO_tskIssue,               /* issue */
    DIO_tskDynamicOpen,         /* open */
    DIO_tskReady,               /* ready */
    DIO_tskReclaim              /* reclaim */
};

static DIO_Handle mkPort(DEV_Handle device, String name);
static Void rmPort(DIO_Handle dio);

/*
 *  ======== mkPort ========
 *  Creates a DIO object and binds the controller.
 */
static DIO_Handle mkPort(DEV_Handle device, String name)
{
    DIO_Params *params = (DIO_Params *)device->params;
    DIO_Handle dio;
    DEV_Device  *entry;
    Uns         mode;
    Int         status;

    /* params should contain name of mini-driver */
    if (params == NULL) {
        return (NULL);
    }
    
    /*
     * check to see that name of mini-driver matches one in the device table
     * and its type is of DEV_IOMTYPE.
     */
    (void)DEV_matchJ(params->name, &entry);
    if (entry == NULL || entry->type != DEV_IOMTYPE) {
        return (NULL);
    }

    /* allocate 0-initialized dio object */
    if ((dio = MEM_calloc(0, sizeof(DIO_Obj), 0)) == MEM_ILLEGAL) {
        return (NULL);
    }

    /*
     * Tasks will pend on dio->complete if there are no available frames on
     * the fromdevice queue.
     */
    dio->context.sems.complete = SEM_createJ(0, NULL);

    /* make sure SEM_create() succeeded ... */
    if (dio->context.sems.complete == NULL) {
        MEM_freeJ(0, dio, sizeof(DIO_Obj));     /* free dio object */
        return (NULL);
    }

    dio->fxns = (IOM_Fxns *)entry->fxns;

    mode = (device->mode == DEV_INPUT) ? IOM_INPUT : IOM_OUTPUT;

    /* create a channel from the mini-driver */
    status = dio->fxns->mdCreateChan(&dio->chanp, entry->devp, name, mode,
                params->chanParams, DIO_tskCallback, device); 

    if (status != IOM_COMPLETED) {
        rmPort(dio);
        return (NULL);
    }

    return (dio);
}

/*
 *  ======== rmPort ========
 *  Removes a DIO object and cleans up
 */
static Void rmPort(DIO_Handle dio)
{
    /* if chanp not NULL, must delete mini-driver channel */
    if (dio->chanp != NULL) {
        dio->fxns->mdDeleteChan(dio->chanp);
    }

    /* remove the semaphore ... */
    SEM_deleteJ(dio->context.sems.complete);

    /* and finally the object */
    MEM_freeJ(0, dio, sizeof(DIO_Obj));
}

/*
 *  ======== DIO_tskDynamicClose ========
 *  DIO_tskIdle() should be called before DIO_tskDynamicClose().
 */
static Int DIO_tskDynamicClose(DEV_Handle device)
{
    DIO_Handle  dio = (DIO_Handle)device->object;

    /*
     * All frames should have been returned by mini-driver since SIO should
     * have already called DIO_idle().
     */
    rmPort(dio);

    return (SYS_OK);
}

/*
 *  ======== DIO_tskDynamicOpen ========
 */
static Int DIO_tskDynamicOpen(DEV_Handle device, String name)
{
    /* allocates DIO_Obj and creates mini-driver channel */
    if ((device->object = (Ptr)mkPort(device, name)) != NULL) {
        return (SYS_OK);
    }
    else {
        return (SYS_EBADIO);
    }
}
