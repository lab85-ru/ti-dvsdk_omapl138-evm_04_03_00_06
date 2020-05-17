/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== gio.c ========
 *
 */

#pragma CODE_SECTION(GIO_init, ".sysinit")

#include <std.h>

#include <gio.h>
#include <sys.h>

GIO_Attrs GIO_ATTRS = {
    2,              /* nPackets */
    SYS_FOREVER,    /* timeout */
};


/*
 *  ======== GIO_init ========
 */
Void GIO_init()
{
}


