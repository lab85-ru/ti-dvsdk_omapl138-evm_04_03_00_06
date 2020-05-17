/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dio_cb.c ========
 *  Callback based functions of DIO.
 *
 */

#include <std.h>

#include <dev.h>
#include <que.h>
#include <sem.h>
#include <sys.h>

#include <iom.h>
#include <dio.h>

Int     DIO_cbIdle(DEV_Handle device, Bool flush);
Int     DIO_cbIssue(DEV_Handle device);
Bool    DIO_cbReady(DEV_Handle device, SEM_Handle sem);
Int     DIO_cbReclaim(DEV_Handle device);
Void    DIO_cbCallback(Ptr devp, DEV_Frame *frame);

/*
 *  ======== DIO_cbCallback ========
 */
Void DIO_cbCallback(Ptr devp, DEV_Frame *frame)
{
    DEV_Handle  device = (DEV_Handle)devp;
    DIO_Handle  dio = (DIO_Handle)device->object;

    if (frame->cmd == IOM_READ || frame->cmd == IOM_WRITE) {
        QUE_putJ(device->fromdevice, frame); 
        dio->context.cb.fxn(dio->context.cb.arg0,
            dio->context.cb.arg1);
    }
}

/*
 *  ======== DIO_cbIdle ========
 *  DIO_cbIdle() simply return SYS_OK
 *  which means it really is a NOP.
 */
Int DIO_cbIdle(DEV_Handle device, Bool flush)
{
    return (SYS_OK);
}

/*
 *  ======== DIO_cbIssue ========
 */
Int DIO_cbIssue(DEV_Handle device)
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
            DIO_cbCallback(device, frame);
        }
    
        return (SYS_OK);
    }

}

/*
 *  ======== DIO_cbReady ========
 */
Bool DIO_cbReady(DEV_Handle device, SEM_Handle sem)
{
    return (!(QUE_empty(device->fromdevice)));
}

/*
 *  ======== DIO_cbReclaim ========
 *  This function is expecting at least one buffer ready to be
 *  processed from the fromdevice queue.
 *  If there are no buffers ready then it will return an error.
 *  If more than one buffer is ready, it will call the callback function.
 */
Int DIO_cbReclaim(DEV_Handle device)
{
    DIO_Handle  dio = (DIO_Handle)device->object;
    QUE_Handle  queElem;

    queElem = device->fromdevice->next;

    if (queElem == device->fromdevice) {
        return (SYS_EBADIO);
    }

    if (queElem->next != device->fromdevice) {
        dio->context.cb.fxn(dio->context.cb.arg0,
            dio->context.cb.arg1);
    }

    return (SYS_OK);
}
