/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dio_cbStatic.c ========
 *  Static Callback based implementation of DIO
 *
 */

#include <std.h>

#include <dev.h>
#include <sys.h>

#include <iom.h>
#include <dio.h>

static Int      DIO_cbStaticClose(DEV_Handle device);
static Int      DIO_cbStaticOpen(DEV_Handle device, String name);

/*
 *  Static Callback based version of function table.
 */
DEV_Fxns DIO_cbStaticFxns = {
    DIO_cbStaticClose,          /* close */
    DIO_ctrl,                   /* ctrl */
    DIO_cbIdle,                 /* idle */
    DIO_cbIssue,                /* issue */
    DIO_cbStaticOpen,           /* open */
    DIO_cbReady,                /* ready */
    DIO_cbReclaim               /* reclaim */
};

static DIO_Handle mkPort(DEV_Handle device, String name);

extern far Int DIO_NUMCBSTATIC;         /* Number of Callback based DIO */
extern far DIO_Obj DIO_CBTABLE[];       /* Table of Callback based DIO */

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
    static Int  numUsed = 0;

    /* supports only the number of statically created callback based DIO */
    if (numUsed >= DIO_NUMCBSTATIC) {
        SYS_error("DIO", SYS_EBADIO);
        return (NULL);
    }

    /* callback must not be NULL if using this version of DIO */
    if (callback == NULL) {
        return (NULL);
    }
    
    /* must be connected to some driver of type DEV_IOMTYPE */
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

    /* allocate dio object */
    dio = &DIO_CBTABLE[numUsed++];

    /* initialize the DIO callback structure */
    dio->context.cb = *callback;

    dio->fxns = (IOM_Fxns *)entry->fxns;

    mode = (device->mode == DEV_INPUT) ? IOM_INPUT : IOM_OUTPUT;

    /* create a channel from the mini-driver */
    status = dio->fxns->mdCreateChan(&dio->chanp, entry->devp, name, mode,
                params->chanParams, DIO_cbCallback, device); 

    if (status != IOM_COMPLETED) {
        return (NULL);
    }

    return (dio);
}

/*
 *  ======== DIO_cbStaticClose ========
 *  DIO_cbStaticClose() should never be called.
 */
static Int DIO_cbStaticClose(DEV_Handle device)
{
    return (SYS_EBADIO);
}

/*
 *  ======== DIO_cbStaticOpen ========
 */
static Int DIO_cbStaticOpen(DEV_Handle device, String name)
{
    /* allocates DIO_Obj and creates mini-driver channel */
    if ((device->object = (Ptr)mkPort(device, name)) != NULL) {
        return (SYS_OK);
    }
    else {
        return (SYS_EBADIO);
    }
}
