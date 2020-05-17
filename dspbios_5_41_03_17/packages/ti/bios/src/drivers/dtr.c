/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dtr.c ========
 *  Stacking Transformer Driver
 *
 *  This driver may be stacked on top of any streaming device and will
 *  transform data to or from the device from fix point to floating point
 *  or vice versa.  A user supplied function (devid = DTR_USER) may be 
 *  specified in params to allow arbitrary "in place" transformation/
 *  conditioning of streaming data.
 *
 */

#include <std.h>

#include <dev.h>
#include <fxn.h>
#include <mem.h>
#include <que.h>
#include <sem.h>
#include <sys.h>

#include <dtr.h>

typedef struct DTR_TransObj {
    DEV_Obj     dobj;           /* MUST be first field */

    Fxn         fxn;            /* fix2flt, flt2fix, multiply, or user */
    Arg         arg;            /* arg for fxn (specified in params) */
} DTR_TransObj;

/*
 *  Default values for this transformer.
 */
DTR_Params DTR_PARAMS = {
    { 1 },              /* scale.value */
    { (Arg)NULL,        /* user.arg */
      (Fxn)NULL },      /* user.fxn */
};

static Int      DTR_close(DEV_Handle device);
static Int      DTR_ctrl(DEV_Handle device, Uns cmd, Arg arg);
static Int      DTR_idle(DEV_Handle device, Bool flush);
static Int      DTR_issue(DEV_Handle device);
static Int      DTR_open(DEV_Handle device, String name);
static Bool     DTR_ready(DEV_Handle device, SEM_Handle sem);
static Int      DTR_reclaim(DEV_Handle device);

/*
 *  Driver function table.
 */
DEV_Fxns DTR_FXNS = {
    DTR_close,          /* close */
    DTR_ctrl,           /* ctrl */
    DTR_idle,           /* idle */
    DTR_issue,          /* issue */
    DTR_open,           /* open */
    DTR_ready,          /* ready */
    DTR_reclaim,        /* reclaim */
};

/*
 *  ======== DTR_close ========
 */
Int DTR_close(DEV_Handle device)
{
    DTR_TransObj        *trans = (DTR_TransObj *)device->object;
    QUE_Handle          queue;

    /* close underlying device(s) */
    DEV_close(&trans->dobj);

    if (device->mode == DEV_INPUT) {
        queue = trans->dobj.fromdevice;
    }
    else {
        queue = trans->dobj.todevice;
    }
    QUE_delete(queue);

    /* recycle trans object */ 
    MEM_freeJ(0, trans, sizeof(DTR_TransObj));  /* free object */
    
    return (SYS_OK);
}

/*
 *  ======== DTR_ctrl ========
 */
Int DTR_ctrl(DEV_Handle device, Uns cmd, Arg arg)
{
    DTR_TransObj        *trans = (DTR_TransObj *)device->object;

    return (DEV_ctrl(&trans->dobj, cmd, arg));
}

/*
 *  ======== DTR_idle ========
 */
Int DTR_idle(DEV_Handle device, Bool flush)
{
    DTR_TransObj        *trans = (DTR_TransObj *)device->object;

    return (DEV_idle(&trans->dobj, flush));
}

/*
 *  ======== DTR_issue ========
 */
Int DTR_issue(DEV_Handle device)
{
    DTR_TransObj        *trans = (DTR_TransObj *)device->object;
    DEV_Frame           *frame;
    Int                 status;
    
    if (device->mode != DEV_INPUT) {    /* DEV_OUTPUT */
        frame = QUE_get(device->todevice);

        (*trans->fxn)(trans->arg, frame->addr, frame->size);

        QUE_putJ(trans->dobj.todevice, frame);
    }

    status = DEV_issue(&trans->dobj);

    return (status);
}

/*
 *  ======== DTR_open ========
 */
Int DTR_open(DEV_Handle device, String name)
{
    DTR_TransObj        *trans;
    DEV_Device          *entry;
    DTR_Params          *params;
    QUE_Handle          queue;
    Int                 status;

    name = DEV_matchJ(name, &entry);
    if (entry == NULL) {
        SYS_error("DTR", SYS_ENODEV);
        return (SYS_ENODEV);
    }

    /* allocate transformer object */
    if ((trans = MEM_allocJ(0, sizeof(DTR_TransObj), 0)) == MEM_ILLEGAL) {
        SYS_error("DTR MEM_alloc", SYS_EALLOC);
        return (SYS_EALLOC);
    }

    trans->dobj = *device;      /* copy descriptor fields */
    trans->dobj.fxns = *(DEV_Fxns *)(entry->fxns);

    /*
     * create 'data' queues.  the 'free' queues (i.e. todevice for
     * input and fromdevice for output) are shared since there
     * is no reason to copy an empty frame from one queue to the
     * next.
     *
     * INPUT:
     *    create new 'fromdevice' in downstream device object.
     * OUTPUT:
     *    create new 'todevice' in downstream device object.
     */
    queue = QUE_create(NULL);
    if (queue == NULL) {
        MEM_freeJ(0, trans, sizeof(DTR_TransObj));      /* free object */
        SYS_error("DTR QUE_create", SYS_EALLOC);

        return (SYS_EALLOC);
    }

    if (device->mode == DEV_INPUT) {
        trans->dobj.fromdevice = queue;
    }
    else {
        trans->dobj.todevice = queue;
    }

    trans->dobj.devid = entry->devid;
    trans->dobj.params = entry->params;

    /* open underlying device */
    if ((status = DEV_open(&trans->dobj, name)) != SYS_OK) {
        QUE_delete(queue);
        MEM_freeJ(0, trans, sizeof(DTR_TransObj));

        return (status);
    }

    params = (device->params) ? (DTR_Params *)device->params : &DTR_PARAMS;

    if (device->devid == NULL) {
        /* DTR_USER */
        if ((trans->fxn = params->user.fxn) == NULL) {
            trans->fxn = (Fxn)FXN_F_nop;
        }
        trans->arg = params->user.arg;
    }
    else {
        trans->fxn = (Fxn)device->devid;
        trans->arg = (Arg)&params->scale.value;
    }

    device->object = (Ptr)trans;

    return (SYS_OK);
}

/*
 *  ======== DTR_ready ========
 */
Bool DTR_ready(DEV_Handle device, SEM_Handle sem)
{
    DTR_TransObj        *trans = (DTR_TransObj *)device->object;

    return (DEV_ready(&trans->dobj, sem));
}

/*
 *  ======== DTR_reclaim ========
 */
Int DTR_reclaim(DEV_Handle device)
{
    DTR_TransObj        *trans = (DTR_TransObj *)device->object;
    DEV_Frame           *frame;
    Int                 status;

    status = DEV_reclaim(&trans->dobj);

    if (device->mode == DEV_INPUT && status == SYS_OK) {
        frame = QUE_get(trans->dobj.fromdevice);

        (*trans->fxn)(trans->arg, frame->addr, frame->size);

        QUE_putJ(device->fromdevice, frame);
    }

    return (status);
}
