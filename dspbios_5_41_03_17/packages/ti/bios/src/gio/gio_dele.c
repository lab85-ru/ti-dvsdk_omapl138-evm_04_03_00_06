/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== gio_dele.c ========
 *
 */

#include <std.h>
#include <mem.h>
#include <que.h>

#include <gio.h>
#include <iom.h>

#include "_gio.h"

#pragma CODE_SECTION(GIO_delete, ".bios")

#ifdef _BIOSROM_
#pragma DATA_SECTION(GIO_deletePtr, ".biosjmp")
Int (*GIO_deletePtr)(GIO_Handle gioChan) = GIO_delete;
#endif

/*
 *  ======== GIO_delete ========
 */
Int GIO_delete(GIO_Handle gioChan)
{
    IOM_Packet *packet;

    /* flush and delete low-level device ... */
    if (gioChan->fxns != NULL && gioChan->mdChan != NULL) {
        GIO_flush(gioChan);
        gioChan->fxns->mdDeleteChan(gioChan->mdChan);
    }

    /* delete semaphore or alternate sync object ... */
    if (gioChan->syncObj != NULL) {
        GIO->SEMDELETE(gioChan->syncObj);
    }

    /* free frames ... */
    packet = QUE_get(&gioChan->freeList);
    while (packet != (IOM_Packet *)(&gioChan->freeList)) {
        _GIO_rmPacket(packet);
        packet = QUE_get(&gioChan->freeList);
    }

    /* free GIO object. */
    (Void)MEM_freeJ(0, gioChan, sizeof(GIO_Obj));

    return (IOM_COMPLETED);
}
