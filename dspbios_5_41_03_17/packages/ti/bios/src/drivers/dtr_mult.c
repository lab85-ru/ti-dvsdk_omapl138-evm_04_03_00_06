/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== dtr_mult.c ========
 *  Spectron-supplied "user" function for DTR.
 *
 */

#include <std.h>
#include <stddef.h>

#include <dtr.h>

/*
 *  ======== DTR_multiply ========
 *
 *  This transformation simply scales the data by an amount
 *  specified when the device was opened.
 *
 *  *addr = *addr * scale
 */
Void DTR_multiply(Arg arg, DTR_Elem *addr, size_t size)
{
    register size_t     i;
    register DTR_Scale  scale;

    scale = *(DTR_Scale *)arg;

    for (i = (size / sizeof(DTR_Elem)); i > 0; i--, addr++) {
        *addr = *addr * scale;
    }
}

#ifndef _FLOAT_
/*
 *  ======== DTR_multiplyInt16 ========
 *
 *  This transformation simply scales the data by an amount
 *  specified when the device was opened.
 *
 *  *addr = *addr * scale
 */
Void DTR_multiplyInt16(Arg arg, DTR_ElemInt16 *addr, size_t size)
{
    register size_t     i;
    register DTR_Scale  scale;

    scale = *(DTR_Scale *)arg;

    for (i = (size / sizeof(DTR_ElemInt16)); i > 0; i--, addr++) {
        *addr = *addr * scale;
    }
}

#endif
