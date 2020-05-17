/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dpi.c ========
 *  New Pipe Driver
 *
 *  The pipe device is a software device used to stream data between
 *  SPOX tasks.  The old DPI driver accomplished this by creating a
 *  shared pair of queues distinct from the original queues created by
 *  SIO.  This allowed efficient I/O plus the ability to have multiple
 *  readers and writers. This method has some limitations, the most
 *  significant being that stacking drivers won't stack on top of the
 *  old DPI (idling is the other).
 *
 *  The new DPI is a pipe implementation which can have stacking drivers
 *  on top. Instead of criss-crossing device pointers to the shared
 *  queues, the orignal SIO queues are left intact and the input/output
 *  function handles the transferring between the device queues.  This
 *  method has the limitation of only one reader and one writer allowed.
 *  It does correctly handle device idling, with one exception: if the
 *  input side of the pipe is closed before the output side, the output
 *  side will throw away remaining output frames instead of waiting for
 *  them to drain if it's been created with attrs.flush TRUE.
 *
 *  The new DPI also fixes the simultaneous SIO_select problem per MR 2172
 *
 *  As usual, SEM semaphores are used to keep track of the number of
 *  available buffers on each of the fromdevice queues.
 *
 */

#pragma CODE_SECTION(DPI_init, ".sysinit")

#include <string.h>

#include <std.h>

#include <dev.h>
#include <mem.h>
#include <que.h>
#include <sem.h>
#include <sys.h>
#include <tsk.h>

#include <dpi.h>

#define COPYBUFS        /* Comment this line for a more efficient version
                           of the DPI driver when using SIO_ISSUERECLAIM. */

#include <stdlib.h>     /* for atoi() */

/*
 *  ======== SPipeObj ========
 * *ONE* SPipeObj is allocated for each pipe (e.g. "/pipe0").  readers
 * and writers adjust their semaphore pointers (in DPI_open()) to use
 * the common pair in SPipeObj.
 */
typedef struct SPipeObj {
    QUE_Elem        link;       /* MUST be first element of this structure */
    SEM_Handle      dataSem;
    SEM_Handle      freeSem;
    SEM_Handle      readySem[2];
    DEV_Handle      device[2];
    Int             id;
} SPipeObj;

/*
 * One PipeObj is allocated for each open device.
 */
typedef struct PipeObj {
    SEM_Handle      toSem;
    SEM_Handle      fromSem;
    SPipeObj        *sPipe;
} PipeObj;

#define Static static

/*
 *  Driver function table.
 */
Static Int      DPI_close(DEV_Handle dev);
Static Int      DPI_issue(DEV_Handle dev);
Static Int      DPI_open(DEV_Handle dev, String name);
Static Bool     DPI_ready(DEV_Handle dev, SEM_Handle sem);
Static Int      DPI_reclaim(DEV_Handle dev);

DEV_Fxns DPI_FXNS = {
    DPI_close,          /* close */
    DEV_CTRL,           /* ctrl */
    DEV_IDLE,           /* idle */
    DPI_issue,          /* issue */
    DPI_open,           /* open */
    DPI_ready,          /* ready */
    DPI_reclaim,        /* reclaim */
};

Static SEM_Handle mutex;        /* for mutual exclusion in open and close */
Static QUE_Handle sPipeList;    /* list of all shared pipe objects */

Static SPipeObj *mkSPipe(DEV_Handle dev);
Static Void rmSPipe(SPipeObj *sPipe);


/*
 *  ======== DPI_close ========
 */
Static Int DPI_close(DEV_Handle dev)
{
    PipeObj         *pipe = (PipeObj *)dev->object;
    SPipeObj        *sPipe = pipe->sPipe;

    MEM_freeJ(0, pipe, sizeof (PipeObj));
    
    SEM_pendJ(mutex, SYS_FOREVER);

    sPipe->device[dev->mode] = NULL;
    sPipe->readySem[dev->mode] = NULL;
             
    if (sPipe->device[DEV_INPUT] == NULL &&
                sPipe->device[DEV_OUTPUT] == NULL) {
        /* delete all shared pipe sub-objects */
        SEM_deleteJ(sPipe->dataSem);
        SEM_deleteJ(sPipe->freeSem);

        /* remove sPipe obj from sPipeList */
        QUE_remove(&sPipe->link);
        
        /* delete sPipe object itself */
        MEM_freeJ(0, sPipe, sizeof (SPipeObj));
    }

    SEM_postJ(mutex);

    return (SYS_OK);
}

/*
 *  ======== DPI_init ========
 */
Void DPI_init(Void)
{
    /* Make sure the initialization happens only once for the DPI driver.*/
    static Bool curInit = FALSE;

    if ( curInit ) {
        return;
    }
    curInit = TRUE;

    mutex = SEM_createJ(1, NULL);
    sPipeList = QUE_create(NULL);

    if (mutex == NULL || sPipeList == NULL) {
        SYS_abort("DPI");
    }
}
    
/*
 *  ======== DPI_issue ========
 */
Static Int DPI_issue(DEV_Handle dev)
{
    PipeObj         *pipe = (PipeObj *)dev->object;
    SPipeObj        *sPipe = pipe->sPipe;
    DEV_Handle      otherdev = sPipe->device[dev->mode ^ 0x1];
    SEM_Handle      otherReady = sPipe->readySem[dev->mode ^ 0x1];
    DEV_Frame       *otherframe;
    DEV_Frame       *frame;
#ifdef COPYBUFS
    DEV_Frame       *srcframe;
    DEV_Frame       *dstframe;
#endif
    
    /*
     * Atomically check that each side has a frame so we can do an
     * exchange.  We can't be sure that a frame is on the
     * dev->todevice queue (just put there by SIO) since a task
     * switch to the task on the other side might intervene and
     * take the frame from this side.
     */
    TSK_disable();
    if (otherdev != NULL &&
        !QUE_empty(dev->todevice) && !QUE_empty(otherdev->todevice)) {

        otherframe = QUE_get(otherdev->todevice);
        frame = QUE_get(dev->todevice);

        /* done with atomic stuff */
        TSK_enable();

/*
 * #define COPYBUFS to cause buffers to be copied through the pipe
 * instead of being exchanged.  Doing so retains the semantics of
 * the ISSUERECLAIM model, but is slow.  If COPYBUFS is *not* defined,
 * then one side reclaims buffers issued by the other side, thereby
 * not strictly retaining buffer ordering.
 */
#ifdef COPYBUFS
        if (dev->mode == DEV_INPUT) {
            dstframe = frame;
            srcframe = otherframe;
        }
        else {
            dstframe = otherframe;
            srcframe = frame;
        }
        memcpy(dstframe->addr, srcframe->addr, srcframe->size);
        dstframe->size = srcframe->size;
        dstframe->arg = srcframe->arg;

        QUE_putJ(dev->fromdevice, frame);
        QUE_putJ(otherdev->fromdevice, otherframe);

        /*
         * frames reclaimed from an output device must have size 0.
         */
        if (dev->mode != DEV_INPUT) {
            frame->size = 0;
        }
        else {
            otherframe->size = 0;
        }
#else
        QUE_putJ(dev->fromdevice, otherframe);
        QUE_putJ(otherdev->fromdevice, frame);

        /*
         * frames reclaimed from an output device must have size 0.
         */
        if (dev->mode != DEV_INPUT) {
            otherframe->size = 0;
        }
        else {
            frame->size = 0;
        }
#endif
    }
    else {
        /* done with atomic stuff */
        TSK_enable();
    }

    SEM_postJ(pipe->toSem);

    if (otherReady != NULL) {
        SEM_postJ(otherReady);
    }

    return SYS_OK;
}

/*
 *  ======== DPI_open ========
 */
Static Int DPI_open(DEV_Handle dev, String name)
{
    PipeObj         *pipe;
    SPipeObj        *sPipe, *tmpPipe;

    /* decode and validate devid */
    if (dev->devid < 0) {
        dev->devid = atoi(name);
    }

    SEM_pendJ(mutex, SYS_FOREVER);

    /* search pipe list for previously opened pipe with same id */
    sPipe = MEM_ILLEGAL;
    if (!QUE_empty(sPipeList)) {
        tmpPipe = (SPipeObj *)QUE_head(sPipeList);
        do {
            if (tmpPipe->id == dev->devid) {
                sPipe = tmpPipe;
                break;
            }
            tmpPipe = (SPipeObj *)QUE_next((&tmpPipe->link));
        } while (tmpPipe != (SPipeObj *)sPipeList);
    }

    if (sPipe == MEM_ILLEGAL) {
        /*
         * Allocate and initialize SPipeObj on first open.
         */
        sPipe = mkSPipe(dev);
        if (sPipe == MEM_ILLEGAL) {
            SEM_postJ(mutex);
            return SYS_EALLOC;
        }
        QUE_putJ(sPipeList, &sPipe->link);
    }
    else {      /* sPipe found on list */
        if (sPipe->device[dev->mode] != NULL) {
            /*
             * Only one input and one output allowed
             */
            SEM_postJ(mutex);
            return SYS_EBUSY;
        }
    }
    sPipe->device[dev->mode] = dev;
    SEM_postJ(mutex);

    pipe = MEM_allocJ(0, sizeof (PipeObj), 0);
    if (pipe == MEM_ILLEGAL) {
        /*
         * We need to undo work done by mkSPipe() if first open.
         * Also need to undo changes to sPipeList queue.
         */
        QUE_remove(&sPipe->link);
        rmSPipe(sPipe);
        return SYS_EALLOC;
    }

    /*
     * Criss-cross SEM handles so both sides are referencing
     * the same physical objects.
     */
    if (dev->mode == DEV_INPUT) {
        pipe->fromSem = sPipe->dataSem;
        pipe->toSem = sPipe->freeSem;
    }
    else {
        pipe->toSem = sPipe->dataSem;
        pipe->fromSem = sPipe->freeSem;
    }

    /*
     * Point things around.
     */
    pipe->sPipe = sPipe;
    dev->object = (Ptr)pipe;

    return (SYS_OK);
}

/*
 *  ======== DPI_ready ========
 *  called by SIO_select(), returns TRUE if device is ready.
 */
Static Bool DPI_ready(DEV_Handle dev, SEM_Handle sem)
{
    PipeObj         *pipe = (PipeObj *)dev->object;

    pipe->sPipe->readySem[dev->mode] = sem;
    
    return !(QUE_empty(dev->fromdevice));
}

/*
 *  ======== DPI_reclaim ========
 */
Static Int DPI_reclaim(DEV_Handle dev)
{
    PipeObj         *pipe = (PipeObj *)dev->object;

    if (SEM_pendJ(pipe->fromSem, dev->timeout)) {
        return (SYS_OK);
    }
    else {
        return (SYS_ETIMEOUT);
    }
}

/*
 *  ======== mkSPipe ========
 */
Static SPipeObj *mkSPipe(DEV_Handle dev)
{
    SPipeObj *sPipe = MEM_allocJ(0, sizeof (SPipeObj), 0);
    
    if (sPipe != MEM_ILLEGAL) {
        sPipe->dataSem = SEM_createJ(0, NULL);
        if (sPipe->dataSem == NULL) {
            rmSPipe(sPipe);
            return (MEM_ILLEGAL);
        }

        sPipe->freeSem = SEM_createJ(0, NULL);
        if (sPipe->freeSem == NULL) {
            rmSPipe(sPipe);
            return(MEM_ILLEGAL);
        }

        sPipe->readySem[DEV_INPUT] = NULL;
        sPipe->readySem[DEV_OUTPUT] = NULL;
        sPipe->device[DEV_INPUT] = NULL;
        sPipe->device[DEV_OUTPUT] = NULL;
        sPipe->id = dev->devid;

        return (sPipe);
    }
    else {
        return (MEM_ILLEGAL);
    }
}

/*
 *  ======== rmSPipe ========
 */
Static Void rmSPipe(SPipeObj *sPipe)
{

    if (sPipe == MEM_ILLEGAL) {
        return;
    }
    else {
        if (sPipe->dataSem != NULL) {
            SEM_deleteJ(sPipe->dataSem);
        }
        if (sPipe->freeSem != NULL) {
            SEM_deleteJ(sPipe->freeSem);
        }
        MEM_freeJ(0, sPipe, sizeof(SPipeObj));
    }
}
