/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dgs.c ========
 *  Stackable Streaming Gather/Scatter Driver
 *
 *  This driver can be used for packing and unpacking various data
 *  formats that involve different word sizes; for example, packing
 *  4 32-bit words (with only 8 significant bits) into one 32-bit word.
 *  The user supplies a function that packs or unpacks buffers.  A
 *  ratio of two integers is used to specify the compression/expansion
 *  factor.  All gathering/scattering must be completed on frame boundaries
 *  as this driver (for efficiency) does not maintain remainders across
 *  I/O operations.
 *
 *  Buffers processed by this device are expanded/compressed by the
 *  factor num/den.  Buffers are allocated and handed to the underlying
 *  device.  For INPUT devices the underlying device's buffer size is
 *  (bufsize * den) / num.  For OUTPUT devices the underlying device's
 *  buffer size is (bufsize * num) / den.
 *
 *  This driver does not support any control calls.
 *
 *  Note that the driver needs a user supplied transform function to do
 *  anything useful.  Configuring the driver involves supplying a transform
 *  function that will do the actual packing or unpacking.  A numerator
 *  and denominator must also be supplied where num / den specifies the
 *  size of the transformed buffer.
 *
 *  For example, a transformation that compresses two 32 bit words into
 *  a single 32 bit word would have num = 1 and den = 2 since the buffer
 *  resulting from the transformation is 1/2 the size of the original 
 *  buffer.
 *
 */

#include <std.h>
#include <stddef.h>

#include <dev.h>
#include <mem.h>
#include <que.h>
#include <sem.h>
#include <sys.h>

#include <dgs.h>

#include <string.h>

typedef struct DGS_CopyObj {
    DEV_Obj     dobj;           /* MUST be first field */
    QUE_Elem    downFrame;      /* frames for underlying device */
    QUE_Elem    upFrame;        /* frames from overlying layer (SIO) */
    Fxn         transFxn;       /* this is the only conf param that changes */
    Arg         arg;            /* argument for transFxn() */
} DGS_CopyObj;

typedef DGS_CopyObj *DGS_Copy;  /* copy device object handle */

static Void objClean(DGS_Copy copy);

static Int      DGS_close(DEV_Handle device);
static Int      DGS_ctrl(DEV_Handle device, Uns cmd, Arg arg);
static Int      DGS_idle(DEV_Handle device, Bool flush);
static Int      DGS_issue(DEV_Handle device);
static Int      DGS_open(DEV_Handle device, String name);
static Bool     DGS_ready(DEV_Handle device, SEM_Handle sem);
static Int      DGS_reclaim(DEV_Handle device);

/*
 *  Driver function table.
 */
DEV_Fxns DGS_FXNS = {
    DGS_close,          /* close */
    DGS_ctrl,           /* ctrl */
    DGS_idle,           /* idle */
    DGS_issue,          /* issue */
    DGS_open,           /* open */
    DGS_ready,          /* ready */
    DGS_reclaim,        /* relaim */
};

/*
 *  ======== localcopy ========
 */
static Int localcopy(Arg arg, void *src, void *dst, size_t size)
{
    memcpy(dst, src, size);

    return (size);
}

/*
 *  ======== emptyQ ========
 */
static Void emptyQ(QUE_Handle queue, size_t size)
{
    DEV_Frame   *frame;

    while (!QUE_empty(queue)) {
        frame = QUE_get(queue);
        DEV_rmframe(frame, 0, size);
    }
}

/*
 *  ======== objClean ========
 *  Clean up a DGS_CopyObj.
 *  Can only be called after both dobj.fromdevice & dobj.todevice
 *  have been attempted to be created and after downFrame has been
 *  QUE_new()ed.
 */
static Void objClean(DGS_Copy copy)
{
    QUE_Handle  queue;
    size_t      size;

    size = copy->dobj.bufsize;

    queue = copy->dobj.fromdevice;
    if (queue != NULL) {
        emptyQ(queue, size);
        QUE_delete(queue);
    }
    queue = copy->dobj.todevice;
    if (queue != NULL) {
        emptyQ(queue, size);
        QUE_delete(queue);
    }
    emptyQ(&copy->downFrame, size);

    /* recycle copy object */ 
    MEM_freeJ(0, copy, sizeof(DGS_CopyObj));
}

/*
 *  ======== DGS_close ========
 */
static Int DGS_close(DEV_Handle device)
{
    DGS_Params  *params = (DGS_Params *)device->params;
    DGS_Copy    copy = (DGS_Copy)device->object;
   
    if (params->deleteFxn) {
        (*params->deleteFxn)(params->arg);
    }

    /* close underlying device(s) */
    DEV_close((DEV_Handle)copy);

    objClean(copy);
    
    return (SYS_OK);
}

/*
 *  ======== DGS_ctrl ========
 */
static Int DGS_ctrl(DEV_Handle  device, Uns cmd, Arg arg)
{
    DGS_Copy    copy = (DGS_Copy)device->object;

    return (DEV_ctrl((DEV_Handle)copy, cmd, arg));
}

/*
 *  ======== DGS_idle ========
 */
static Int DGS_idle(DEV_Handle device, Bool flush)
{
    DGS_Copy    copy = (DGS_Copy)device->object;

    return (DEV_idle((DEV_Handle)copy, flush));
}

/*
 *  ======== DGS_issue ========
 */
static Int DGS_issue(DEV_Handle device)
{
    DGS_Copy    copy = (DGS_Copy)device->object;
    DEV_Frame   *dstFrame;
    DEV_Frame   *srcFrame;
    Int         status;

    srcFrame = QUE_get(device->todevice);
    dstFrame = QUE_get(&copy->downFrame);

    if (device->mode != DEV_INPUT) {
        dstFrame->size = (*copy->transFxn)(
          copy->arg, srcFrame->addr, dstFrame->addr, srcFrame->size);
    }
    dstFrame->arg = srcFrame->arg;

    QUE_putJ(copy->dobj.todevice, dstFrame);

    status = DEV_issue(&copy->dobj);

    if (status == SYS_OK) {
        QUE_putJ(&copy->upFrame, srcFrame);
    }
    else {
        dstFrame = QUE_get(copy->dobj.todevice);
        QUE_putJ(&copy->downFrame, dstFrame);
        QUE_putJ(device->todevice, srcFrame);
    }

    return (status);
}

/*
 *  ======== DGS_open ========
 */
static Int DGS_open(DEV_Handle device, String name)
{
    DGS_Params  *params = (DGS_Params *)device->params;
    DGS_Copy    copy;
    DEV_Device  *entry;
    QUE_Handle  queue;
    DEV_Frame   *frame;
    Int         status;
    size_t      size;
    Int         i;

    name = DEV_matchJ(name, &entry);
    if (entry == NULL) {
        SYS_error("DGS", SYS_ENODEV);
        return (SYS_ENODEV);
    }

    /* device params *MUST* be supplied */
    if (params == NULL) {
        SYS_error("DGS", SYS_EINVAL);
        return (SYS_EINVAL);
    }

    /* allocate copy object */
    if ((copy = MEM_allocJ(0, sizeof(DGS_CopyObj), 0)) == MEM_ILLEGAL) {
        SYS_error("DGS MEM_alloc", SYS_EALLOC);
        return (SYS_EALLOC);
    }

    copy->dobj = *device;       /* copy descriptor fields */
    copy->dobj.fxns = *(DEV_Fxns *)(entry->fxns);

    /*
     * create queues and frames for underlying device.
     */
    QUE_new(&copy->downFrame);
    QUE_new(&copy->upFrame);

    copy->dobj.fromdevice = QUE_create(NULL);
    copy->dobj.todevice = QUE_create(NULL);

    if (copy->dobj.fromdevice == NULL || copy->dobj.todevice == NULL) {
        objClean(copy);
        SYS_error("DGS QUE_create", SYS_EALLOC);

        return (SYS_EALLOC);
    }

    if (params->transFxn == NULL) {
        copy->transFxn = (Fxn)localcopy;
    }
    else {
        copy->transFxn = params->transFxn;
    }

    /* if this instance needs to initialize, call its create function */
    if (params->createFxn) {

        copy->arg = (Arg)(*params->createFxn)(params->arg);
        if (copy->arg == NULL) {
            objClean(copy);
            SYS_error("DGS", SYS_EBADIO);

            return (SYS_EBADIO);
        }
    }
    else {
        copy->arg = params->arg;
    }

    /*
     * The temporary 'queue' variable will get overwritten below for
     * the DEV_ISSUERECLAIM model.  'size' is used for all models.
     */
    if (device->mode == DEV_INPUT) {
        queue = copy->dobj.todevice;
        size = (device->bufsize * params->den) / params->num;
    }
    else {
        queue = copy->dobj.fromdevice;
        size = (device->bufsize * params->num) / params->den;
    }

    /*
     * Put all frames on an internal DGS queue.
     */
    queue = &copy->downFrame;

    /*
     * Create frames and put them on the correct device queue.
     */
    for (i=0; i < device->nbufs; i++) {
        frame = DEV_mkframe(0, size, 0);
        QUE_putJ(queue, frame);
    }

    copy->dobj.bufsize = size;
    copy->dobj.devid = entry->devid;
    copy->dobj.params = entry->params;

    /* open underlying device */
    if ((status = DEV_open((DEV_Handle)copy, name)) != SYS_OK) {
        objClean(copy);

        return (status);
    }

    device->object = (Ptr)copy;

    return (SYS_OK);
}

/*
 *  ======== DGS_ready ========
 */
static Bool DGS_ready(DEV_Handle device, SEM_Handle sem)
{
    DGS_Copy    copy = (DGS_Copy)device->object;

    return (DEV_ready((DEV_Handle)copy, sem));
}

/*
 *  ======== DGS_reclaim ========
 */
static Int DGS_reclaim(DEV_Handle device)
{
    DGS_Copy    copy = (DGS_Copy)device->object;
    DEV_Frame   *dstFrame;
    DEV_Frame   *srcFrame;
    Int         status;

    if ((status = DEV_reclaim(&copy->dobj)) != SYS_OK) {
        return (status);
    }

    srcFrame = QUE_get(copy->dobj.fromdevice);
    dstFrame = QUE_get(&copy->upFrame);

    if (device->mode == DEV_INPUT) {
        dstFrame->size = (*copy->transFxn)(
          copy->arg, srcFrame->addr, dstFrame->addr, srcFrame->size);
    }
    else {
        dstFrame->size = 0;
    }
    dstFrame->arg = srcFrame->arg;

    QUE_putJ(&copy->downFrame, srcFrame);
    QUE_putJ(device->fromdevice, dstFrame);

    return (SYS_OK);
}
