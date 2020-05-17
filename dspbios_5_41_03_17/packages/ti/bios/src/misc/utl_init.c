/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== utl_init.c ========
 *
 */

#pragma CODE_SECTION(UTL_init, ".sysinit")

#include <std.h>
#include <sys.h>
#include <utl.h>
#include <string.h>

/*
 *  ======== UTL_init ========
 */
Void UTL_init(Void)
{
    /* clear circular buffer */
    memset((Ptr)&SYS_PUTCBEG, 0, &SYS_PUTCEND - &SYS_PUTCBEG + 1);
}


