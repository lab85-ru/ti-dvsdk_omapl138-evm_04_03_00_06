/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dhl.h ========
 *  Host-Link driver header.
 *
 *  This driver is used for streaming data between the host and the DSP,
 *  using an underlying HST object.
 *
 */

#ifndef DHL_
#define DHL_

#include <dev.h>
#include <sem.h>
#include <hst.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  DHL Object and Handle.  This device uses an HST object with an underlying
 *  PIP object, for streaming data between the DSP and the host.
 *  The "notify" function for this HST object is DHL_interrupt.
 *
 *  The fields of the DHL_Obj have the following meanings:
 *
 *  SEM_Obj     done    - The count of this semaphore = number of buffers
 *                        on fromdevice queue.
 *  Uns         nested  - This flag prevents reentrancy of DHL_copy when
 *                        called from threads of different priority.
 *  HST_Obj     *hst    - Underlying HST object
 *  SEM_Handle  ready   - Temporary semaphore used by DHL_ready()
 *  DEV_Handle  device  - Device handle to get at todevice and fromdevice
 *                        queues.
 *  Uns      *curptrBuf - Used to keep track of current position in device
 *                        buffer from where to copy data to or from the PIP
 *                        frame.
 *  Uns        countBuf - Number of words left to copy to or from the
 *                        device buffer to or from PIP frames.
 *  Uns      *curptrFrm - Used to keep track of current position in PIP frame
 *                        from where to copy data to/from the device buffer.
 *  Uns        countFrm - Number of words left to copy to or from the
 *                        PIP frame to the device buffer.
 *  Uns         started - Set to TRUE on the first call to DHL_issue(). This is
 *                        ensure that input buffers are not filled until
 *                        DHL_issue() is called.  (The input PIP
 *                        may be filled as soon as the channel is started on
 *                        the host.) Set to FALSE from DHL_idle().
 *  Int         devid   - Set to the device id. It allows to discern what
 *                        device to use when DHL_notify is triggered. The
 *                        device id is set to the channel id of it HST channel.
 *  Int         mode    - Set to the mode of the underlying HST object. Needs
 *                        to match the mode of the stream that opens the device.
 *
 */
typedef struct DHL_Obj {
    SEM_Obj     done;           /* Count = # of buffers on fromdevice queue. */
    Uns         nested;         /* Reentrancy lock */
    HST_Obj     *hst;           /* Underlying HST object */
    SEM_Handle  ready;          /* Temporary semaphore used by DHL_ready() */
    DEV_Handle  device;         /* Device handle - to get at queues */
    Uns         *curptrBuf;     /* Position in buffer to copy to pip frame */
    Uns         countBuf;       /* Number left in buffer to copy to pip */
    Uns         *curptrFrm;     /* Position in frame to copy to device buffer */
    Uns         countFrm;       /* Number left in pip frame to copy to buffer */
    Uns         started;        /* Set to TRUE when input is started */
    Int         devid;          /* Set to the device id. */
    Int         mode;           /* SIO_INPUT or SIO_OUTPUT */
} DHL_Obj, *DHL_Handle;

extern DEV_Fxns DHL_FXNS;

extern Void DHL_notify(Arg arg0);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* DHL_ */
