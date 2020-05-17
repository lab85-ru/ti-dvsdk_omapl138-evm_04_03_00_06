/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dio_tsk.c ========
 *  SEM based functions of DIO.
 *
 */

#include <std.h>

#include <dev.h>
#include <que.h>
#include <sem.h>
#include <sys.h>

#include <iom.h>
#include <dio.h>

Int     DIO_tskIdle(DEV_Handle device, Bool flush);
Int     DIO_tskIssue(DEV_Handle device);
Bool    DIO_tskReady(DEV_Handle device, SEM_Handle sem);
Int     DIO_tskReclaim(DEV_Handle device);
Void    DIO_tskCallback(Ptr devp, DEV_Frame *frame);

/*
 *  ======== DIO_tskCallback ========
 */
Void DIO_tskCallback(Ptr devp, DEV_Frame *frame)
{
    DEV_Handle  device = (DEV_Handle)devp;
    DIO_Handle  dio = (DIO_Handle)device->object;

    if (frame->cmd == IOM_READ || frame->cmd == IOM_WRITE) {
        QUE_putJ(device->fromdevice, frame); 

        SEM_postJ(dio->context.sems.complete);

        /*
         * If semaphore was registered with DIO_ready(), dio->ready will
         * be non-NULL.  In this case, SIO_select() is probably waiting for
         * this semaphore to be posted by the first ready device.
         */
        if (dio->context.sems.ready) {
            SEM_postJ(dio->context.sems.ready);
        }
    }
    else {
        SEM_postJ(dio->context.sems.complete);
    }
}

/*
 *  ======== DIO_tskIdle ========
 *  DIO_tskIdle() puts the device back to the state it was in just after
 *  DIO_open() was called.
 */
Int DIO_tskIdle(DEV_Handle device, Bool flush)
{
    DIO_Handle dio = (DIO_Handle)device->object;
    Uns         pendCount = 0;
    DEV_Frame   localFrame;
    Int         status;

    localFrame.status = IOM_PENDING;

    if (device->mode == DEV_INPUT || flush) {
        localFrame.cmd = IOM_ABORT;
    }
    else {
        localFrame.cmd = IOM_FLUSH;
    }

    status = dio->fxns->mdSubmitChan(dio->chanp, &localFrame);

    if (status == IOM_PENDING) {
        for (;;) {
            if (SEM_pendJ(dio->context.sems.complete, device->timeout)) {
                /*
                 * Break out of the for loop when the local abort/flush
                 * frame status shows that it is complete.
                 */
                if (localFrame.status == IOM_COMPLETED) {
                    break;
                }
                else {
                    /* keep track of data frame completions */
                    pendCount++;
                }
            }
            else {
                return (SYS_ETIMEOUT);
            }
        }

        /* 
         * Update complete semaphore so it corresponds to number of frames
         * on the 'todevice' queue.  'for' loop above may have made the 
         * complete sem count inconsistent.
         */
        while (pendCount--) {
            SEM_postJ(dio->context.sems.complete);
        }
    }

    return (SYS_OK);
}

/*
 *  ======== DIO_tskIssue ========
 */
Int DIO_tskIssue(DEV_Handle device)
{
    DIO_Handle dio = (DIO_Handle)device->object;
    DEV_Frame   *frame;
    Int         status;

    frame = QUE_get(device->todevice);

    frame->cmd = (device->mode == DEV_INPUT) ? IOM_READ : IOM_WRITE;
    frame->status = IOM_PENDING;

    status = dio->fxns->mdSubmitChan(dio->chanp, frame);

    if (status < 0) {
        return (SYS_EBADIO);
    }
    else {
        if (status == IOM_COMPLETED) {
            QUE_putJ(device->fromdevice, frame);
            SEM_postJ(dio->context.sems.complete);
        }
        
        return (SYS_OK);
    }
}

/*
 *  ======== DIO_tskReady ========
 */
Bool DIO_tskReady(DEV_Handle device, SEM_Handle sem)
{
    DIO_Handle  dio = (DIO_Handle)device->object;

    dio->context.sems.ready = sem;

    return (!(QUE_empty(device->fromdevice)));
}

/*
 *  ======== DIO_tskReclaim ========
 */
Int DIO_tskReclaim(DEV_Handle device)
{
    DIO_Handle  dio = (DIO_Handle)device->object;

    /*
     * Wait here if there are no buffers on the device->fromdevice
     * queue.
     */
    if (SEM_pendJ(dio->context.sems.complete, device->timeout)) {
        return (SYS_OK);
    }
    else {
        return (SYS_ETIMEOUT);
    }
}
