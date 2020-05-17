/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dio.c ========
 *
 *  DSP/BIOS SIO device driver (DEV) which interfaces to IOM mini-
 *  drivers. See the DSP/BIOS Users Guide for more information on SIO
 *
 */

#pragma CODE_SECTION(DIO_init, ".sysinit")

#include <std.h>

#include <dev.h>
#include <sys.h>

#include <dio.h>

Int     DIO_ctrl(DEV_Handle device, Uns cmd, Arg arg);
Int     DIO_issue(DEV_Handle device);

/*
 *  ======== DIO_ctrl ========
 *  Simply call down to the mini-driver.
 */
Int DIO_ctrl(DEV_Handle device, Uns cmd, Arg arg)
{
    Int status;

    DIO_Handle dio = (DIO_Handle)device->object;

    status = dio->fxns->mdControlChan(dio->chanp, cmd, (Ptr)arg);

    if (status < 0 ) {
        return (SYS_EBADIO);
    }
    else {
        return (SYS_OK);
    }
}

/*  ======== DIO_init ========
 *
 */
Void DIO_init(Void)
{
    /*
     *  Module initialization
     */
}
