/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dio_tskStatic.c ========
 *  Static SEM based implementation of DIO.
 *
 */

#include <std.h>

#include <dev.h>
#include <sys.h>

#include <iom.h>
#include <dio.h>

static Int      DIO_tskStaticClose(DEV_Handle device);
static Int      DIO_tskStaticOpen(DEV_Handle device, String name);

/*
 *  Static SEM based version of function table.
 */
DEV_Fxns DIO_tskStaticFxns = {
    DIO_tskStaticClose,         /* close */
    DIO_ctrl,                   /* ctrl */
    DIO_tskIdle,                /* idle */
    DIO_tskIssue,               /* issue */
    DIO_tskStaticOpen,          /* open */
    DIO_tskReady,               /* ready */
    DIO_tskReclaim              /* reclaim */
};

static DIO_Handle mkPort(DEV_Handle device, String name);

extern far Int DIO_NUMTSKSTATIC;        /* Number of SEM based DIO */
extern far DIO_Obj DIO_TSKTABLE[];      /* Table of SEM based DIO */

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
    static Int  numUsed = 0;

    /* supports only the number of statically created SEM based DIO */
    if (numUsed >= DIO_NUMTSKSTATIC) {
        return (NULL);
    }

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

    /* allocate dio object */
    dio = &DIO_TSKTABLE[numUsed++];

    dio->fxns = (IOM_Fxns *)entry->fxns;

    mode = (device->mode == DEV_INPUT) ? IOM_INPUT : IOM_OUTPUT;

    /* create a channel from the mini-driver */
    status = dio->fxns->mdCreateChan(&dio->chanp, entry->devp, name, mode,
                params->chanParams, DIO_tskCallback, device); 

    if (status != IOM_COMPLETED) {
        return (NULL);
    }

    return (dio);
}

/*
 *  ======== DIO_tskStaticClose ========
 *  DIO_tskStaticClose() should never be called.
 */
static Int DIO_tskStaticClose(DEV_Handle device)
{
    return (SYS_EBADIO);
}

/*
 *  ======== DIO_tskStaticOpen ========
 */
static Int DIO_tskStaticOpen(DEV_Handle device, String name)
{
    /* allocates DIO_Obj and creates mini-driver channel */
    if ((device->object = (Ptr)mkPort(device, name)) != NULL) {
        return (SYS_OK);
    }
    else {
        return (SYS_EBADIO);
    }
}
