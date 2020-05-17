/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== gio_new.c ========
 *
 */

#include <std.h>
#include <dev.h>
#include <que.h>
#include <sys.h>

#include <string.h>

#include <gio.h>
#include <iom.h>

#include "_gio.h"

#pragma CODE_SECTION(GIO_new, ".bios")

#ifdef _BIOSROM_
#pragma DATA_SECTION(GIO_newPtr, ".biosjmp")
GIO_Handle (*GIO_newPtr)(GIO_Handle gioChan, String name, Int mode, Int *status,
        Ptr optArgs, IOM_Packet packetBuf[], Ptr syncObject, 
        GIO_Attrs *attrs) = GIO_new;
#endif

/*
 *  ======== GIO_new ========
 */
GIO_Handle GIO_new(GIO_Handle gioChan, String name, Int mode, Int *status,
        Ptr optArgs, IOM_Packet *packetBuf, Ptr syncObject, GIO_Attrs *attrs)
{
    DEV_Device  *entry;
    Int         i;
    Int         tmpStat;

    if (attrs == NULL) {
        attrs = &GIO_ATTRS;
    }

    /*
     * status param is used to pass additional device status back to caller.
     */
    if (status == NULL) {
        status = &tmpStat;    /* no longer need to check if status valid ptr */
    }

    *status = IOM_COMPLETED;
    
    /*
     *  Find device structure in device table for device with name 'name'.
     *  DEV_match() returns the remaining name string for use by the
     *  mini-driver's create() function.
     */
    name = DEV_matchJ(name, &entry);
    if (entry == NULL) {
        SYS_error(name, SYS_ENODEV); /* sys error - no device found */
        return (NULL);
    }
    
    if (entry->type != DEV_IOMTYPE) {
        SYS_error("IOM", SYS_EINVAL); /* sys error - invalid device parameter */
        return (NULL);
    }

    /* initialize queue structures */
    QUE_new(&gioChan->freeList);

    /* zero out the packet buffers */
    memset(packetBuf, 0, attrs->nPackets * sizeof(IOM_Packet));

    /* Put packets into freeList. */
    for (i=0; i < attrs->nPackets; i++) {
        QUE_putJ(&gioChan->freeList, &packetBuf[i]);
    }

    /*
     * Plug semaphore or other synchronization object.  'gioChan->syncObj' is
     * used to wait for I/O to complete when GIO_submit() is called with
     * NULL *appCallback parameter. 
     */
    gioChan->syncObj = syncObject;

    gioChan->fxns = (IOM_Fxns *)entry->fxns;
    gioChan->mode = mode;
    gioChan->timeout = attrs->timeout;

    *status = gioChan->fxns->mdCreateChan(&gioChan->mdChan, entry->devp,
            name, mode, optArgs, _GIO_iomCallback, gioChan);

    if (gioChan->mdChan == NULL) {
        return (NULL);
    }

    return (gioChan);
}


