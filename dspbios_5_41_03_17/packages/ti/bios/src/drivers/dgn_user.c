/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dgn_user.c ========
 *  User function entry point for DGN
 *
 */

#include <std.h>
#include <stddef.h>

#include <dgn.h>

/*
 *  ======== DGN_user ========
 *  Fill the buffer whose address is "addr" with random values generated
 *  "size" times.  The random values are generated using a recursive
 *  equation.
 */
Void DGN_user(DGN_GenObj *gen, Ptr addr, size_t size)
{
    Arg         arg;
    Fxn         fxn;

    arg = gen->dparams->user.arg;
    fxn = gen->dparams->user.fxn;

    (*fxn)(arg, addr, size);
}
