/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dgn_prhx.c ========
 *
 *  "User" function for DGN /printHex device.
 *!
 */


#include <std.h>
#include <stddef.h>
#include <sys.h>

/*
 *  ======== DGN_printHex ========
 */
Void DGN_printHex(Arg arg, Ptr addr, size_t size)
{
    Int16       *buf = (Int16 *)addr;
        
    size = size / sizeof(Int16);

    for (; size > 0; size--, buf++) {
        SYS_printf("0x%x\n", *buf);
    }
}
