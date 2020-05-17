/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dhl.c ========
 *  HST-LNK driver
 *
 *  This is the underlying driver for streaming data between the host and
 *  the DSP. Each DHL device has an underlying HST object, with LNK being
 *  used to transfer the data between the host and the DSP.
 *  Data is copied to or from the buffer at the head of the todevice
 *  queue, from or into the HST object's next available PIP frame.
 *  The HST notify function is DHL_interrupt.
 *
 */

#include <std.h>
#include <stddef.h>

#include <atm.h>
#include <dev.h>
#include <dhl.h>
#include <hst.h>
#include <pip.h>
#include <que.h>
#include <sem.h>
#include <sys.h>
#include <swi.h>
#include <tsk.h>

#include <stdlib.h>
#include <string.h>

#if DEBUG
#define myAssert(cond)  \
    if (!(cond)) { \
        SYS_abort(#cond ":  %s, line:  %d\n", __FILE__, __LINE__); \
    }
#define STATIC 
#else
#define myAssert(cond)
#define STATIC static
#endif

extern DHL_Handle       _DHL_tab[];     /* defined in dhl.h## */
extern Int              _DHL_numof;     /* defined in dhl.h## */

/*
 *  Driver functions
 */
STATIC Int      DHL_close(DEV_Handle device);
STATIC Int      DHL_idle(DEV_Handle device, Bool flush);
STATIC Int      DHL_open(DEV_Handle device, String name);
STATIC Bool     DHL_ready(DEV_Handle device, SEM_Handle sem);
STATIC Int      DHL_issue(DEV_Handle device);
STATIC Int      DHL_reclaim(DEV_Handle device);

/*
 *  DHL driver function table.
 */
DEV_Fxns DHL_FXNS = {
    DHL_close,          /* close */
    DEV_CTRL,           /* ctrl */
    DHL_idle,           /* idle */
    DHL_issue,          /* issue */
    DHL_open,           /* open */
    DHL_ready,          /* ready */
    DHL_reclaim,        /* reclaim */
};

/*
 *  Local static functions. 
 */
static Void copy(DHL_Handle dhl);
static Bool checkAndUnset(DHL_Handle dhl);

/*
 *  ======== DHL_close ========
 */
Int DHL_close(DEV_Handle device)
{
    device->object = NULL;

    return (SYS_OK);
}

/*
 *  ======== DHL_idle ========
 */
Int DHL_idle(DEV_Handle device, Bool flush)
{
    DHL_Obj     *dhl = (DHL_Obj *)device->object;
    Int         pendCount = 0;

    if (device->mode == DEV_INPUT) {
        /*
         *  This will stop input from the host (until we have
         *  HST_F_deletemask()).
         */
        dhl->started = FALSE;

        /* Put all frames on fromdevice queue */
        while (!QUE_empty(device->todevice)) {
            QUE_putJ(device->fromdevice, QUE_get(device->todevice));
            SEM_postJ(&dhl->done);
        }
    }
    else {
        /* Output */
        if (!flush) {
            /*
             *  Wait for all buffers to be transferred.  Keep track of the
             *  number of buffers transferred, to reset dhl->done.
             */
            while (!QUE_empty(device->todevice)) {
                SEM_pendJ(&dhl->done, SYS_FOREVER);
                pendCount++;
            }

            while (pendCount--) {
                SEM_postJ(&dhl->done);
            }
        }
        else {
            /* Discard the output */

            /* Stop output (until we get HST_F_deletemask() */
            dhl->started = FALSE;

            while (!QUE_empty(device->todevice)) {
                QUE_putJ(device->fromdevice, QUE_get(device->todevice));
                SEM_postJ(&dhl->done);
            }
        }
    }
    
    return (SYS_OK);
}

/*
 *  ======== DHL_notify ========
 *
 *  This is the "notify reader" function for input channels, and the
 *  "notify writer" function for output channels.
 */
Void DHL_notify(Arg arg0)
{
    Int         devid = (Uns)ArgToInt(arg0);
    Int         i;
    DHL_Handle  dhl;

    /* Search _DHL_tab for the right device. */
    for (i = 0; i < _DHL_numof; i++) {
        dhl = _DHL_tab[i];
        if (dhl->devid == devid) {
            break;
        }
    }

    /* Do not transfer data before first DHL_issue(), or if device is idle */
    if (!dhl->started) {
        return;
    }

    copy(dhl);

    return;
}

/*
 *  ======== DHL_issue ========
 */
Int DHL_issue(DEV_Handle device)
{
    DHL_Obj     *dhl = (DHL_Obj *)device->object;

    if (!dhl->started) {
        /* Input device is now started */
        dhl->started = TRUE;
    }

    copy(dhl);

    return (SYS_OK);
}

/*
 *  ======== DHL_open ========
 */
Int DHL_open(DEV_Handle device, String name)
{
    DHL_Handle  dhl;
    Int         i;

    /*
     *  Link to a statically created DHL object. devid is the id of the
     *  associated HST object, so it could be greater than _DHL_numof.
     */
    if (device->devid < 0 ) {
        return (SYS_EINVAL);
    }

    /* Search the _DHL_tab for the right devid. */
    for (i = 0; i < _DHL_numof; i++) {
        dhl = _DHL_tab[i];
        if (dhl->devid == device->devid) {
            break;
        }
    }

    /* Check if already used */
    if (dhl->device != NULL) {
        return (SYS_EBUSY);
    }
    else {
        dhl->device = device;
    }

    /* Check that device's mode (HST mode) matches the stream. */
    if (dhl->mode != device->mode) {
        return (SYS_EBADOBJ);
    }

    device->object = dhl;

    return(SYS_OK);
}

/*
 *  ======== DHL_ready ========
 */
Bool DHL_ready(DEV_Handle device, SEM_Handle sem)
{
    DHL_Handle  dhl = (DHL_Handle)device->object;

    dhl->ready = sem;

    return (!(QUE_empty(device->fromdevice)));
}

/*
 *  ======== DHL_reclaim ========
 */
Int DHL_reclaim(DEV_Handle device)
{
    DHL_Obj     *dhl = (DHL_Obj *)device->object;

    /* Make sure there's a buffer available. */
    if (!SEM_pendJ(&dhl->done, device->timeout)) {
        return (SYS_ETIMEOUT);
    }

    return (SYS_OK);
}

/*
 *  ======== copy ========
 *
 *  This routine is the heart of DHL, where data is transfered between the
 *  stream buffers and the pipe frames. It can be called at the task level by
 *  DHL_issue and at the HST_pump level by the pipe's callback. Since these
 *  threads can have different priorities, we must avoid reentrancy for this
 *  routine. In pseudo-code:
 *
 *  copy(dhl) {
 *      if (ATM{set(dhl.nested)}) {
 *              return;
 *      }
 *      while (checkAndUnSet(dhl)) {
 *              doCopy();
 *      }
 *      return;
 *  }
 */
static Void copy(DHL_Handle dhl)
{
    DEV_Handle  dev;
    DEV_Frame   *frame;
    PIP_Obj     *pip;
    size_t      size;
    size_t      i;
    Uns         *src, *dst;

    /* Avoid reentrancy. */
    if (ATM_setu(&(dhl->nested), (Uns)1)) {
        return;
    }

    dev = (DEV_Handle)dhl->device;
    pip = HST_getpipe(dhl->hst);

    while (checkAndUnset(dhl)) {

        /* Update dhl->curptrBuf to next available buffer, if necessary */
        if (dhl->curptrBuf == NULL) {
            frame = (DEV_Frame *)QUE_head(dev->todevice);
            dhl->curptrBuf = (Uns *)frame->addr;                        
            /* 
             *  SIO buf size is in Char, PIP frame size is in words.
             *  Convert contBuf to words to compare with countFrm later.
             */
            dhl->countBuf = frame->size / sizeof(Uns);
        }
        
        /*
         *  Update dhl->curptrFrm to next available PIP frame, if necessary.
         *  Set the src and dst for the copy.
         */
        if (dev->mode == DEV_OUTPUT) {
            if (dhl->curptrFrm == NULL) {
                PIP_alloc(pip);
                dhl->curptrFrm = PIP_getWriterAddr(pip);
                dhl->countFrm = PIP_getThreshold(pip);
            }

            src = dhl->curptrBuf;
            dst = dhl->curptrFrm;
        }
        else { /* DEV_INPUT */
            if (dhl->curptrFrm == NULL) {
                PIP_get(pip);
                dhl->curptrFrm = PIP_getReaderAddr(pip);
                dhl->countFrm = PIP_getReaderSize(pip);
            }
            
            src = dhl->curptrFrm;
            dst = dhl->curptrBuf;
        }

        /* Determine the size that can be copied and copy. */
        size = (dhl->countBuf < dhl->countFrm) ? 
                dhl->countBuf : dhl->countFrm;
        for (i = 0; i < size; i++) {
            *dst++ = *src++;
        }

        /* Update curptr's and count's */
        dhl->curptrBuf += size;
        dhl->curptrFrm += size;
        dhl->countBuf -= size;
        dhl->countFrm -= size;
        
        if (dev->mode == DEV_OUTPUT) {
            /*
             *  In output, the size of the device buffer drives when data is
             *  sent to the host. If the pipe frame or is full or we 
             *  finished copying a device buffer, send frame to the host.
             */
            if ((dhl->countFrm == 0) || (dhl->countBuf == 0)) {
                PIP_setWriterSize(pip, 
                                  (PIP_getThreshold(pip) - dhl->countFrm)
                                 );
                dhl->curptrFrm = NULL;
                PIP_put(pip);
            }

            /* If done with device buffer, transfer to fromdevice queue */
            if (dhl->countBuf == 0) {
                QUE_putJ(dev->fromdevice, QUE_get(dev->todevice));
                dhl->curptrBuf = NULL;
                SEM_postJ(&dhl->done);
            }
        }
        else { /* DEV_INPUT */

            /* If done with the pipe frame, return it */
            if (dhl->countFrm == 0) {
                dhl->curptrFrm = NULL;
                PIP_free(pip);
            }

            /*
             *  In input, the size of the pipe frame drives when data is put
             *  in the fromdevice queue and made available to the
             *  application. When the device buffer is full or we have
             *  finished reading a pipe frame, return the device buffer to
             *  the fromdevice queue.
             */ 
            if ((dhl->countBuf == 0) || (dhl->countFrm == 0)) {
                frame = QUE_get(dev->todevice);

                /* Update frame size to the number of elements copied */
                frame->size = frame->size - (dhl->countBuf * sizeof(Uns));

                QUE_putJ(dev->fromdevice, frame);
                dhl->curptrBuf = NULL;
                SEM_postJ(&dhl->done);
            }
        }

    } /* while (checkAndUnset(dhl)) */
}

/*
 *  ======== checkAndUnset ========
 *  This routine checks whether there's more data available to transfer
 *  between buffers and frames. More data could be available if copy() was
 *  preempted while "nested" was set.
 *
 *  It must disable SWI to make sure new data does not become available while it
 *  performs the check for data and clears "nested" (othewise the result of
 *  the check could be erroneous).
 *
 *  Note that we're making the implicit assumption that copy() will never be
 *  called from a hardware ISR.
 *
 *  If there's no more data available,  unset nested and return FALSE.
 *
 *  In pseudo-code:
 *
 *  bool checkAndUnset(dhl) {
 *      SWI_disable;
 *      if (bufferIsReady && frameIsReady) {
 *              status = TRUE;
 *      }
 *      else {
 *              status = FALSE;
 *              ATM(clear(dhl.nested));
 *      }
 *      SWI_enable;
 *      return (status);
 *  }
 */
static Bool checkAndUnset(DHL_Handle dhl)
{
    Bool buffers = FALSE;
    Bool frames = FALSE;
    DEV_Handle dev = dhl->device;
    PIP_Obj *pip;

    SWI_disable();

    /* Check for buffer availability */
    if (dhl->curptrBuf != NULL) {
                buffers = TRUE;
    }
    else if (!QUE_empty(dev->todevice)) {
                buffers = TRUE;
    }

    /* If there are buffer data available, check the frames. */
    if (buffers) {
        pip = HST_getpipe(dhl->hst);
        if (dev->mode == DEV_OUTPUT) {
            if (dhl->curptrFrm != NULL) {
                frames = TRUE;
            }
            else if (PIP_getWriterNumFrames(pip) > 0) {
                frames = TRUE;
            }
        }
        else { /* DEV_INPUT */
            if (dhl->curptrFrm != NULL) {
                frames = TRUE;
            }
            else if (PIP_getReaderNumFrames(pip) > 0) {
                frames = TRUE;
            }
        }
    }

    if (!(buffers && frames)) {
        ATM_clearu(&dhl->nested);
    }

    SWI_enable();

    return (buffers && frames);
}
