/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dnl.c ========
 *  NULL device ala /dev/null
 *
 */

#include <std.h>

#include <dev.h>
#include <que.h>
#include <sys.h>

#include <dnl.h>

Int DNL_exchange();

/*
 *  Driver function table.
 */
DEV_Fxns DNL_FXNS = {
    DEV_CLOSE,          /* close */
    DEV_CTRL,           /* ctrl */
    DEV_IDLE,           /* idle */
    DNL_exchange,       /* issue */
    DEV_OPEN,           /* open */
    DEV_READY,          /* ready */
    (DEV_Treclaim)SYS_zero      /* reclaim */
};

/*
 *  ======== DNL_exchange ========
 */
Int DNL_exchange(DEV_Handle device)
{
    QUE_putJ(device->fromdevice, QUE_get(device->todevice));

    return (SYS_OK);
}
