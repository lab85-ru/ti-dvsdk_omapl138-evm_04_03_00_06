/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== gio_sbmt.c ========
 *
 */

#include <std.h>
#include <que.h>

#include <gio.h>
#include <iom.h>

#include "_gio.h"

#pragma CODE_SECTION(GIO_submit, ".bios")

#ifdef _BIOSROM_
#pragma DATA_SECTION(GIO_submitPtr, ".biosjmp")
Int (*GIO_submitPtr)(GIO_Handle gioChan, Uns cmd, Ptr bufp, \
         size_t *psize, GIO_AppCallback *appCallback) = GIO_submit;
#endif

/*
 *  ======== GIO_submit ========
 */
Int GIO_submit(GIO_Handle gioChan, Uns cmd, Ptr bufp, \
         size_t *psize, GIO_AppCallback *appCallback)
{
    Int         status;
    Bool        semStat;
    IOM_Packet  *packet;

    if (appCallback == NULL) {
        /* synchronous operation, use dedicated packet */
        packet = &gioChan->syncPacket;
    }
    else {
        /* asynchronous operation, get packet from freelist */
        packet = QUE_get(&gioChan->freeList);
        if (packet == (IOM_Packet *)(&gioChan->freeList)) {
            return (IOM_ENOPACKETS);
        }
    }

    /* initialize size variable if psize == NULL */
    if (psize == NULL) {
        packet->size = 0;
        psize = &packet->size;
    }

    packet->cmd = cmd;
    packet->addr = bufp;
    packet->size = *psize;
    packet->status = IOM_COMPLETED;
    /* 
     * 'appCallback' will be NULL for synchronous calls. 
     * 'packet->misc' is used in callback function to call callback (async)
     * or post semaphore (sync).
     */
    packet->misc = (Arg)appCallback;

    /* call down into mini-driver */
    status = gioChan->fxns->mdSubmitChan(gioChan->mdChan, packet);


    if ((status == IOM_COMPLETED) || (status < 0)) {
        if (status == IOM_COMPLETED) {
            *psize = packet->size;
            status = packet->status;
        }

        /* If async then place packet back on free list */    
        if (appCallback != NULL) {
            
            QUE_putJ(&gioChan->freeList, packet);
        }

        return (status);
    }

    /*
     * Call SEMPEND Fxn only if synchronous i/o and no error returned
     *   from mdSubmitChan().
     */
    if (appCallback == NULL) {

        if (status < 0) {    /* error occured */
            *psize = 0;
            return (status);
        }

        /* synchronous I/O -- call global blocking function */
        semStat = GIO->SEMPEND(gioChan->syncObj, gioChan->timeout);

        if (semStat) {
            *psize = packet->size;
            status = packet->status;
        }
        else {    /* timeout occurred */
            *psize = 0;
            
            /* 
             * NOTE: A channel timeout needs special handling. Timeouts are
             * usually due to some serious underlying device or system state
             * and may require the channel, or possibly the device,to be reset.
             * Because the mini-driver may still own the IOM_Packet here
             * driver's will need to perform timeout processing. We will call
             * the mini-driver's control fxn with the IOM_CHAN_TIMEDOUT command
             * code.
             */
             if ((status = gioChan->fxns->mdControlChan(gioChan->mdChan,
                     IOM_CHAN_TIMEDOUT, NULL)) != IOM_COMPLETED) { 
                 
                 return (IOM_ETIMEOUTUNREC); /* Fatal: may have lost IOP */
             }
             
             return (IOM_ETIMEOUT);
        }
    }

    return (status);
}


