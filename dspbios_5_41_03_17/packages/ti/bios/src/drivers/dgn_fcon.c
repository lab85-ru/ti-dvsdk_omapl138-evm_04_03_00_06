/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dgn_fcon.c ========
 *  Spectron-supplied "user" function for DGN.
 *
 */

#include <std.h>
#include <stddef.h>
#include <dgn.h>

/*
 *  ======== DGN_fconst ========
 *  Fill the buffer whose address is "addr" with constant values generated
 *  "size" times.
 */
Void DGN_fconst(DGN_GenObj *gen, Float *addr, size_t size)
{
    size_t                 i;

    for (i = (size / sizeof(Float)); i > 0; i--) {
        *addr++ = gen->dparams->constant.value;
    }
}

