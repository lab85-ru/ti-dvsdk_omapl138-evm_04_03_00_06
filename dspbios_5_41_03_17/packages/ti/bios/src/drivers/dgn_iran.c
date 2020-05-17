/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dgn_iran.c ========
 *  Spectron-supplied "user" function for DGN.
 *
 */

#include <std.h>
#include <stddef.h>

#include <dgn.h>

/*
 *  ======== DGN_irand ========
 *  Fill the buffer whose address is "addr" with random values generated
 *  "size" times.  The random values are generated using a recursive
 *  equation.
 */
Void DGN_irand(DGN_GenObj *gen, Int16 *addr, size_t size)
{
    size_t         i;
    Uns         range;
    Uns         rnd;
    LgUns       next;

    range = gen->dparams->rand.upper - gen->dparams->rand.lower + 1;
    next = gen->seed;

    for (i = (size / sizeof(Int16)); i > 0; i--) {
        /*
         *      Random numbers are generated using a linear congruential
         *  psuedo random generator using the equation:
         *      f(n+1) = (a * f(n) + c) mod M,  for (n >= 0)
         *  to insure the cycle length to be M = 2^N for an N-bit
         *  binary two's complement machine:
         *      a mod 8 == 5
         *      c mod 2 == 1
         *  See Knuth Vol II Seminumerical Algorithms for theory.
         */
        next = next * 1103515245 + 12345;

        /*
         *  Note the top bits are "more random" than the bottom bits
         *  so to scale the number treat the top 16 bits as a binary
         *  fraction (from 0.0 - < 1.0) and multiply by the desired
         *  range and truncate the result.
         */
        rnd = (next >> 16);
        if (range) {
            rnd = (Uint16) (((LgUns) rnd * (LgUns) range) >> 16);
        }
        *addr++ = rnd + gen->dparams->rand.lower;
    }

    gen->seed = next;
}

