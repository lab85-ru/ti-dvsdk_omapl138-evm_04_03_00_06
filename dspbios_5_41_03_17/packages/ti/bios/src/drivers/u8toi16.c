/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== u8toi16.c ========
 *
 *  These functions are used with the DGS stacking driver to provide
 *  conversion to/from a packed 8-bit format (two 8-bit words in one
 *  16-bit word) to a one word per 16 bit format. 
 *
 */

#include <std.h>
#include <stddef.h>

/*
 *  ======== u8toi16 ========
 *
 *  Convert an array of packed 8-bit unsigned values to 16-bit
 *  integer values.
 *
 *  Parameters:
 *      arg - unused
 *
 *      src - pointer to array of packed values (input)
 *
 *      dst - pointer to array of Int values (output)
 *
 *      size - number of bytes in the src array
 */
size_t u8toi16(Arg arg, Uns *src, Int *dst, size_t size)
{
    Int         tmp;
    size_t      i;

    for (i = size; i > 0; i--) {

        /* copy high order 8-bit src value to dst */
        tmp = ((Int)*src) >> 8;  
        *dst++ = tmp;

        /* copy second 8-bit src value to high order 8 bits of dst */
        tmp = *src++ & (Uns) 0xff;
        /* sign extension */
        tmp |= ((tmp & (Uns) 0x80) ? (Uns) 0xff00 : 0x0);
        *dst++ = tmp;
    }

    return (size * 2);
}

/*
 *  ======== i16tou8 ========
 *
 *  Convert an array of 16-bit
 *  integer values to packed 8-bit integer values.
 *
 *  The buffer must contain an even amount of 16-bit integers.
 *
 *  Parameters:
 *      arg - unused
 *
 *      src - pointer to array of Int values (input)
 *
 *      dst - pointer to array of packed values (output)
 *
 *      size - number of bytes in the src array
 */
size_t i16tou8(Arg arg, Int *src, Uns *dst, size_t size)
{
    size_t      i;
    size_t      nUns;

    nUns = size / 2;

    for (i = nUns; i > 0; i--) {

        /* copy low-order eight bits of src to high-order bits of dst word */
        *dst = (Uns)(*src++) << 8;

        /* copy low-order eight bits of src to low-order bits of dst word */
        *dst |= (Uns)*src++ & (Uns)0xff;
        dst++;
    }

    return (size / 2);
}
