/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== i16toi32.c ========
 *
 *  These functions are used with the DGS stacking driver to provide
 *  conversion to/from packed signed 16-bit integers to signed 32-bit
 *  integers.
 *
 */

#include <std.h>
#include <stddef.h>

/*
 *  ======== i16toi32 ========
 *
 *  Convert an array of packed 16-bit integer values to 32-bit
 *  integer values.
 *
 *  The buffer must contain an even amount of 16-bit integers.
 *
 *  Paramaters:
 *      arg - unused
 *
 *      src - pointer to array of packed values (input)
 *
 *      dst - pointer to array of Int values (output)
 *
 *      size - number of bytes in the src array
 */
size_t i16toi32(Arg arg, Uns *src, Int *dst, size_t size)
{
    Int         tmp;
    size_t      i;
    size_t      nUns;

    nUns = size / sizeof (Uns);

    for (i = nUns; i > 0; i--) {
        tmp = ((Int)*src) >> 16;
        *dst++ = tmp;
        tmp = *src++ & (Uns) 0xffff;
        tmp |= ((tmp & (Uns) 0x8000) ? (Uns) 0xffff0000 : 0x0);
        *dst++ = tmp;
    }

    return (size * 2);
}

/*
 *  ======== i32toi16 ========
 *  Convert an array of 32-bit
 *  integer values to packed 16-bit integer values.
 *
 *  The buffer must contain an even amount of 32-bit integers.
 *
 *  Paramaters:
 *      arg - unused
 *
 *      src - pointer to array of Int values (input)
 *
 *      dst - pointer to array of packed values (output)
 *
 *      size - number of bytes in the src array
 */
size_t i32toi16(Arg arg, Int *src, Uns *dst, size_t size)
{
    Int         i;
    size_t      tmp;
    size_t      nInts;

    nInts = size / sizeof (Int);

    for (i = nInts / 2; i > 0; i--) {
        tmp = (Uns)(*src++) << 16;
        tmp |= (Uns)*src++ & (Uns)0xffff;
        *dst++ = tmp;
    }

    return (size / 2);
}
