/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dst.c ========
 *  Stackable "Split" Driver
 *
 *  This stacking driver can be used to input or output buffers that
 *  are larger than the physical device can actually handle.  For output,
 *  a single (large) buffer is split into multiple smaller buffers which
 *  are then output to the actual device.  Multiple (small) input buffers
 *  are input from the device and copied into a single (large) buffer.
 *
 *  CONFIGURATION:
 *
 *  #include <dst.h>
 *
 *  driver DST {
 *      "/split" :      devid = -1;
 *  };
 *
 *  USAGE:
 *
 *  An integer 'factor' is appended to the device name and decoded by
 *  DST_open().  It is used to determine the size of the buffers to
 *  be used with the underlying device.  Buffers for the underlying
 *  devices are the size of the application buffer divided by factor.
 *  The application buffer therefore has to be evenly divisible by
 *  factor.  For example:
 *
 *      stream = SIO_create("/split4/file:tmp.dat", SIO_INPUT, 1024, NULL);
 *
 *  will open a stacking split device which will do four 256 word inputs 
 *  from the "/file:tmp.dat" device and copy the data into 1024 word
 *  application buffers.
 *
 *  CONSTRAINTS:
 *  
 *  This driver assumes that the device buffers are smaller than the
 *  application buffers.
 *
 *  Application buffers must be evenly divisible by the 'factor' specified
 *  in the device name.
 *
 *  This driver does not support any control calls.
 *
 */

#include <std.h>
#include <stddef.h>

#include <dev.h>
#include <mem.h>
#include <que.h>
#include <sem.h>
#include <sys.h>

#include <dst.h>

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct DST_CopyObj {
    DEV_Obj     dobj;           /* MUST be first field */
    QUE_Elem    downFrame;      /* frames for underlying device */
    QUE_Elem    upFrame;        /* frames from overlying layer (SIO) */
} DST_CopyObj;

typedef DST_CopyObj *DST_Copy;  /* copy device object handle */

static Int      DST_close(DEV_Handle device);
static Int      DST_ctrl(DEV_Handle device, Uns cmd, Arg arg);
static Int      DST_idle(DEV_Handle device, Bool flush);
static Int      DST_issue(DEV_Handle device);
static Int      DST_open(DEV_Handle device, String name);
static Bool     DST_ready(DEV_Handle device, SEM_Handle sem);
static Int      DST_reclaim(DEV_Handle device);

/*
 *  Driver function table.
 */
DEV_Fxns DST_FXNS = {
    DST_close,          /* close */
    DST_ctrl,           /* ctrl */
    DST_idle,           /* idle */
    DST_issue,          /* issue */
    DST_open,           /* open */
    DST_ready,          /* ready */
    DST_reclaim,        /* reclaim */
};

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
 *  Clean up a DST_CopyObj.
 *  Can only be called after both dobj.fromdevice & dobj.todevice
 *  have been attempted to be created and after downFrame has been
 *  QUE_new()ed.
 */
static Void objClean(DST_Copy copy)
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
    MEM_freeJ(0, copy, sizeof(DST_CopyObj));
}

/*
 *  ======== DST_close ========
 */
static Int DST_close(DEV_Handle device)
{
    DST_Copy    copy = (DST_Copy)device->object;

    /* close underlying device(s) */
    DEV_close(&copy->dobj);

    objClean(copy);
    
    return (SYS_OK);
}

/*
 *  ======== DST_ctrl ========
 */
static Int DST_ctrl(DEV_Handle  device, Uns cmd, Arg arg)
{
    DST_Copy    copy = (DST_Copy)device->object;

    return (DEV_ctrl(&copy->dobj, cmd, arg));
}

/*
 *  ======== DST_idle ========
 */
static Int DST_idle(DEV_Handle device, Bool flush)
{
    DST_Copy    copy = (DST_Copy)device->object;

    return (DEV_idle(&copy->dobj, flush));
}

/*
 *  ======== DST_issue ========
 */
static Int DST_issue(DEV_Handle device)
{
    DST_Copy    copy = (DST_Copy)device->object;
    DEV_Frame   *upFrame;
    DEV_Frame   *downFrame;
    Char        *srcAddr;
    Int         status = SYS_OK;
    size_t      n, size;
    
    /* get frame from upstream queue */
    upFrame = QUE_get(device->todevice);

    n = upFrame->size;
    srcAddr = (Char *)upFrame->addr;

    while (n > 0) {
        /*
         * There's always a frame on the DST queue.
         */
        downFrame = QUE_get(&copy->downFrame);

        size = (copy->dobj.bufsize < n) ? copy->dobj.bufsize : n;

        if (device->mode != DEV_INPUT) {
            memcpy((Void *)downFrame->addr, srcAddr, size);
            srcAddr += size;
        }
        n -= size;
        downFrame->size = size;
        
        QUE_putJ(copy->dobj.todevice, downFrame);

        status = DEV_issue(&copy->dobj);
        if (status != SYS_OK) {
            break;
        }
    }
    if (status == SYS_OK) {
        QUE_putJ(&copy->upFrame, upFrame);
    }
    else {
        downFrame = QUE_get(copy->dobj.todevice);
        QUE_putJ(&copy->downFrame, downFrame);
        QUE_putJ(device->todevice, upFrame);
    }

    return (status);
}

/*
 *  ======== DST_open ========
 */
static Int DST_open(DEV_Handle device, String name)
{
    DST_Copy    copy;
    DEV_Device  *entry;
    Int         status;
    QUE_Handle  queue;
    DEV_Frame   *frame;
    Int         factor;
    size_t      size;
    Int         i;

    /*
     * If devid is nonzero, it holds the 'factor' to divide buffer
     * sizes by.  Otherwise the factor is found in the device name.
     */
    if (device->devid > 0) {
        factor = device->devid;
    }
    else {
        factor = atoi(name);

        /*
         * Skip the numeric characters to get to the underlying
         * device's name.
         */
        while (isdigit(*name)) {
            name++;
        }
    }

    if (factor <= 0) {
        SYS_error("DST", SYS_EINVAL);
        return (SYS_EINVAL);
    }

    size = device->bufsize / factor;

    /*
     * Make sure device->bufsize is *EVENLY* divisible by 'factor'.
     * If it isn't, the integer division (above) would have truncated.
     */
    if (size * factor != device->bufsize) {
        SYS_error("DST", SYS_EINVAL);
        return (SYS_EINVAL);
    }

    /*
     * find underlying device in device table
     */
    name = DEV_matchJ(name, &entry);
    if (entry == NULL) {
        SYS_error("DST", SYS_ENODEV);
        return (SYS_ENODEV);
    }


    /* allocate copy object */
    if ((copy = MEM_allocJ(0, sizeof(DST_CopyObj), 0)) == MEM_ILLEGAL) {
        SYS_error("DST", SYS_EALLOC);

        return (SYS_EALLOC);
    }

    copy->dobj = *device;       /* copy descriptor fields */
    copy->dobj.fxns = *(DEV_Fxns *)(entry->fxns);
    copy->dobj.devid = entry->devid;
    copy->dobj.params = entry->params;

    copy->dobj.bufsize = size;  /* size of underlying buffers */

    /*
     * create queues and frames for underlying device.
     */
    QUE_new(&copy->downFrame);
    QUE_new(&copy->upFrame);

    copy->dobj.fromdevice = QUE_create(NULL);
    copy->dobj.todevice = QUE_create(NULL);

    if (copy->dobj.fromdevice == NULL || copy->dobj.todevice == NULL) {
        objClean(copy);
        SYS_error("DST QUE_create()", SYS_EALLOC);

        return (SYS_EALLOC);
    }

    /*
     * Put all frames on an internal DST queue.
     * Also, we need to have 'factor' buffers for every buffer
     * in the overlying stream.
     */
    queue = &copy->downFrame;
    copy->dobj.nbufs *= factor;

    /*
     * create frames and put them on the correct device queue.
     */
    for (i=0; i < copy->dobj.nbufs; i++) {
        frame = DEV_mkframe(0, size, 0);
        QUE_putJ(queue, frame);
    }

    /* open underlying device */
    if ((status = DEV_open(&copy->dobj, name)) != SYS_OK) {
        objClean(copy);

        return (status);
    }

    device->object = (Ptr)copy;

    return (SYS_OK);
}

/*
 *  ======== DST_ready ========
 */
static Bool DST_ready(DEV_Handle device, SEM_Handle sem)
{
    DST_Copy    copy = (DST_Copy)device->object;

    return (DEV_ready(&copy->dobj, sem));
}

/*
 *  ======== DST_reclaim ========
 */
static Int DST_reclaim(DEV_Handle device)
{
    DST_Copy    copy = (DST_Copy)device->object;
    DEV_Frame   *upFrame;
    DEV_Frame   *downFrame;
    Char        *dstAddr;
    Int         status;
    size_t      n, size;
    
    /* get empty frame from upstream queue */
    upFrame = QUE_get(&copy->upFrame);

    n = upFrame->size;
    upFrame->size = 0;
    dstAddr = (Char *)upFrame->addr;

    while (n > 0) {
        if ((status = DEV_reclaim(&copy->dobj)) != SYS_OK) {
            return (status);
        }

        /* get full frame from downstream queue */
        downFrame = QUE_get(copy->dobj.fromdevice);

        /*
         * For an INPUT stream, the size of the actual buffer returned
         * is contained in the frame->size field, but we shouldn't
         * go by that since we need to reclaim all buffers that we
         * issued and we issued them by the following formula in
         * DST_issue().
         */
        size = (copy->dobj.bufsize < n) ? copy->dobj.bufsize : n;

        if (device->mode == DEV_INPUT) {
            /*
             * copy data from one frame to the other
             */
            memcpy(dstAddr, (void *)downFrame->addr, size);

            dstAddr += size;

            upFrame->size += size;
        }
        n -= size;

        /* put empty frame on downstream queue */
        QUE_putJ(&copy->downFrame, downFrame);
    }

    /* put full frame on upstream queue */
    QUE_putJ(device->fromdevice, upFrame);

    return (SYS_OK);
}
