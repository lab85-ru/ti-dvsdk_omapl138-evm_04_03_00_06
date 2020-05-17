/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dio_cbDynamic.c ========
 *  Dynamic Callback based implementation of DIO.
 *
 */

#include <std.h>

#include <dev.h>
#include <mem.h>
#include <sys.h>

#include <iom.h>
#include <dio.h>

static Int      DIO_cbDynamicClose(DEV_Handle device);
static Int      DIO_cbDynamicOpen(DEV_Handle device, String name);

/*
 *  Dynamic Callback based version of function table.
 */
DEV_Fxns DIO_cbDynamicFxns = {
    DIO_cbDynamicClose,         /* close */
    DIO_ctrl,                   /* ctrl */
    DIO_cbIdle,                 /* idle */
    DIO_cbIssue,                /* issue */
    DIO_cbDynamicOpen,          /* open */
    DIO_cbReady,                /* ready */
    DIO_cbReclaim               /* reclaim */
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
    DEV_Callback  *callback = (DEV_Callback *)device->callback;
    DIO_Handle dio;
    DEV_Device  *entry;
    Uns         mode;
    Int         status;

    /* callback must not be NULL if using this version of DIO */
    if (callback == NULL) {
        return (NULL);
    }
    
    /* params must contain name of mini-driver */
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

    /* initialize the DIO callback values */
    dio->context.cb = *callback;

    dio->fxns = (IOM_Fxns *)entry->fxns;

    mode = (device->mode == DEV_INPUT) ? IOM_INPUT : IOM_OUTPUT;

    /* create a channel from the mini-driver */
    status = dio->fxns->mdCreateChan(&dio->chanp, entry->devp, name, mode,
                params->chanParams, DIO_cbCallback, device); 

    if (status != IOM_COMPLETED) {
        rmPort(dio);
        return (NULL);
    }

    return (dio);
}

/*
 *  ======== rmPort ========
 *  Remove a DIO object and cleans up
 */
static Void rmPort(DIO_Handle dio)
{
    /* if chanp not NULL, must delete mini-driver channel */
    if (dio->chanp != NULL) {
        dio->fxns->mdDeleteChan(dio->chanp);
    }

    MEM_freeJ(0, dio, sizeof(DIO_Obj));
}

/*
 *  ======== DIO_cbDynamicClose ========
 *  DIO_cbDynamicClose() can only be called if the user knows that 
 *  all frames have been reclaimed and there are no pending frames.
 */
static Int DIO_cbDynamicClose(DEV_Handle device)
{
    DIO_Handle dio = (DIO_Handle)device->object;

    rmPort(dio);

    return (SYS_OK);
}

/*
 *  ======== DIO_cbDynamicOpen ========
 */
static Int DIO_cbDynamicOpen(DEV_Handle device, String name)
{
    /* allocates DIO_Obj and creates mini-driver channel */
    if ((device->object = (Ptr)mkPort(device, name)) != NULL) {
        return (SYS_OK);
    }
    else {
        return (SYS_EBADIO);
    }
}
