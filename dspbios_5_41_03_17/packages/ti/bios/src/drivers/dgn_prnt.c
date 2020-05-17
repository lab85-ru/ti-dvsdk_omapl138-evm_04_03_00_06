/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dgn_prnt.c ========
 *
 *  "User" function for DGN /printInt device.
 *!
 */
    

#include <std.h>
#include <stddef.h>
#include <sys.h>

/*
 *  ======== DGN_printInt ========
 */
Void DGN_printInt(Arg arg, Ptr addr, Uns size)
{
    Int16       *buf = (Int16 *)addr;
        
    size = size / sizeof(Int16);

    for (; size > 0; size--, buf++) {
        SYS_printf("%d\n", *buf);
    }
}
