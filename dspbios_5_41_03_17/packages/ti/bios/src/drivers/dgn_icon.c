/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dgn_icon.c ========
 *  Spectron-supplied "user" function for DGN.
 *
 */

#include <std.h>
#include <stddef.h>

#include <dgn.h>

/*
 *  ======== DGN_iconst ========
 *  Fill the buffer whose address is "addr" with constant values generated
 *  "size" times.
 */
Void DGN_iconst(DGN_GenObj *gen, Int16 *addr, size_t size)
{
    size_t        i;

    for (i = (size / sizeof(Int16)); i > 0; i--) {
        *addr++ = gen->dparams->constant.value;
    }
}

