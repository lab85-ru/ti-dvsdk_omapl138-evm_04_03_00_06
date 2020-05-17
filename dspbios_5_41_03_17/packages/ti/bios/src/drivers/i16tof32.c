/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== i16tof32.c ========
 *
 *  These functions are used with the DGS stacking driver to provide
 *  conversion to/from packed signed 16-bit integers to 32-bit floating
 *  point values.
 *
 */

#include <std.h>
#include <stddef.h>

/*
 *  ======== i16tof32 ========
 *
 *  Convert an array of packed 16-bit integer values to 32-bit
 *  floating-point values.
 *
 *  The buffer must contain an even amount of 16-bit integers.
 *
 *  Paramaters:
 *      arg - unused
 *
 *      src - pointer to array of packed values (input)
 *
 *      dst - pointer to array of Float values (output)
 *
 *      size - number of bytes in the src array
 */
size_t i16tof32(Arg arg, Uns *src, Float *dst, size_t size)
{
    Int         tmp;
    size_t      i;
    size_t      nUns;

    nUns = size / sizeof (Uns);

    for (i = nUns; i > 0; i--) {
        tmp = ((Int) *src) >> 16;
        *dst++ = ((Float)tmp);
        tmp = *src++ & (Uns) 0xffff;
        tmp |= ((tmp & (Uns) 0x8000) ? (Uns) 0xffff0000 : 0x0);
        *dst++ = ((Float)tmp); 
    }

    return (size * 2);
}

/*
 *  ======== f32toi16 ========
 *  Convert an array of 32-bit
 *  floating-point values to packed 16-bit integer values.
 *
 *  The buffer must contain an even amount of 32-bit Floats.
 *
 *  Paramaters:
 *      arg - unused
 *
 *      src - pointer to array of Float values (input)
 *
 *      dst - pointer to array of packed values (output)
 *
 *      size - number of bytes in the src array
 */
size_t f32toi16(Arg arg, Float *src, Uns *dst, size_t size)
{
    size_t      i;
    Int         tmp;
    size_t      nFloats;

    nFloats = size / sizeof (Float);

    for (i = nFloats / 2; i > 0; i--) {
        tmp = ((Uns) (Int)(*src++)) << 16;
        tmp |= ((Uns)(Int)(*src++)) & (Uns) 0xffff;
        *dst++ = tmp;
    }

    return (size / 2);
}
