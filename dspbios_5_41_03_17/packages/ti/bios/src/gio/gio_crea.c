/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== gio_crea.c ========
 *
 */

#include <std.h>
#include <dev.h>
#include <mem.h>
#include <que.h>
#include <sys.h>

#include <gio.h>
#include <iom.h>

#include "_gio.h"

#pragma CODE_SECTION(GIO_create, ".bios")

#ifdef _BIOSROM_
#pragma DATA_SECTION(GIO_createPtr, ".biosjmp")
GIO_Handle (*GIO_createPtr)(String name, Int mode, Int *status,
        Ptr optArgs, GIO_Attrs *attrs) = GIO_create;
#endif

/*
 *  ======== GIO_create ========
 */
GIO_Handle GIO_create(String name, Int mode, Int *status, Ptr optArgs, \
        GIO_Attrs *attrs)
{
    GIO_Handle  gioChan;
    IOM_Packet  *packet;
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

    /*  allocate and 0-fill IOM object */
    gioChan = MEM_calloc(0, sizeof(GIO_Obj), 0);
    if (gioChan == NULL) {
        *status = IOM_EALLOC;  
       
        return (NULL);
    }

    /* initialize queue structures */
    QUE_new(&gioChan->freeList);

    /*
     * Allocate packets for asynch I/O.
     */
    for (i=0; i < attrs->nPackets; i++) {

        packet = _GIO_mkPacket();

        if (packet == NULL) {
           
            *status = IOM_EALLOC;

            GIO_deleteJ(gioChan);
            return (NULL);
        }

        QUE_putJ(&gioChan->freeList, packet);
    }

    /*
     * Create semaphore or other synchronization object.  'gioChan->syncObj' is
     * used to wait for I/O to complete when GIO_submit() is called with
     * NULL *appCallback parameter. 
     */
    gioChan->syncObj = GIO->SEMCREATE(0, NULL);

    if (gioChan->syncObj == NULL) {

        *status = IOM_EALLOC;
 
        GIO_deleteJ(gioChan);
        return (NULL);
    }

    gioChan->fxns = (IOM_Fxns *)entry->fxns;
    gioChan->mode = mode;
    gioChan->timeout = attrs->timeout;

    *status = gioChan->fxns->mdCreateChan(&gioChan->mdChan, entry->devp,
            name, mode, optArgs, _GIO_iomCallback, gioChan);

    if (gioChan->mdChan == NULL) {
        
        GIO_deleteJ(gioChan);
        return (NULL);
    }

    return (gioChan);
}
