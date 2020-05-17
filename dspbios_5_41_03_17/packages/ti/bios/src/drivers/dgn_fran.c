/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dgn_fran.c ========
 *  Spectron-supplied "user" function for DGN.
 *
 */

#include <std.h>
#include <stddef.h>
#include <dgn.h>

/*
 *  ======== DGN_frand ========
 *  Fill the buffer whose address is "addr" with random values generated
 *  "size" times.  The random values are generated using a recursive
 *  equation.
 */
Void DGN_frand(DGN_GenObj *gen, Float *addr, size_t size)
{
    size_t                 i;
    register Long       k, seed;
    Float               scale;

    /*
     *  Set local variables needed to generate the random sequence.
     *
     *  seed is set to an initial value if this is the first call
     *  for this generator.  Else, it is set to the last seed generated
     *  from the previous call.  
     */
    seed = gen->seed;
    scale = 
        (gen->dparams->rand.upper - gen->dparams->rand.lower) * 4.656613e-10;

    /*
     *  This loop generates the random sequence.
     */
    for (i = (size / sizeof(Float)); i > 0; i--) {
        k = seed / 53688;
        seed = 40014 * (seed - k * 53688) - k * 12211;
        if ( seed < 0 ) {
            seed += 2147483563;
        }
        *addr++ = seed * scale + gen->dparams->rand.lower;
    }

    gen->seed = seed;  /* save the seed for the next invocation */
}

