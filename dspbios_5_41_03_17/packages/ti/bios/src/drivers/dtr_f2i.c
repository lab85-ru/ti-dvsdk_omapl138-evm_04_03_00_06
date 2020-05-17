/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dtr_f2i.c ========
 *  Spectron-supplied "user" function for DTR.
 *
 *  Note: For the purpose of transforming fixed point data to floating point,
 *  this function assumes that sizeof(Float) == sizeof(Int)!  Of course,
 *  for the C30/C40 this is ok, but ...
 *
 */

#include <std.h>
#include <stddef.h>

#include <dtr.h>

/*
 *  ======== DTR_flt2fix ========
 *
 *  This transformation converts floating point data to integer data.
 *
 *  *addr = (Int)(*addr * scale)
 */
Void DTR_flt2fix(Arg arg, DTR_Elem *addr, size_t size)
{
    register size_t     i;
    register Int        *iptr;
    register Float      *fptr;
    register DTR_Scale  scale;

    scale = *(DTR_Scale *)arg;
    fptr = (Float *)addr;
    iptr = (Int *)addr;

    for (i = (size / sizeof(Float)); i > 0; i--) {
        *iptr++ = (Int)(*fptr++ * scale);
    }
}

