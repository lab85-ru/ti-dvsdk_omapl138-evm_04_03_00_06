/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== u8tou32.c ========
 *
 *  These functions are used with the DGS stacking driver to provide
 *  conversion to/from packed unsigned 8-bit integers to unsigned
 *  32-bit integers.
 *
 */

#include <std.h>
#include <stddef.h>

/*
 *  ======== u32tou8 ========
 *
 *  Convert an array of 32-bit unsigned values to packed 8-bit
 *  unsigned values.
 *
 *  The buffer must contain a multiple of 4 amount of 32-bit unsigned values.
 *
 *  Paramaters:
 *      arg - unused
 *
 *      src - pointer to array of Uns values (input)
 *
 *      dst - pointer to array of packed values (output)
 *
 *      size - number of bytes in the src array
 *
 *  Returns:
 *      number of bytes in destination buffer
 */
size_t u32tou8(arg, src, dst, size)
    Arg         arg;
    Uns         *src;
    Uns         *dst;
    size_t      size;
{
    size_t      i;
    size_t      nUns;

    nUns = size / sizeof (Uns);

    for (i = nUns / 4; i > 0; i--) {
        *dst = (0xff & *src++) << 24;
        *dst |= (0xff & *src++) << 16;
        *dst |= (0xff & *src++) << 8;
        *dst++ |= 0xff & *src++;
    }

    return (size / 4);
}

/*
 *  ======== u8tou32 ========
 *
 *  Convert an array of packed 8-bit unsigned values to 32-bit
 *  unsigned values.
 *
 *  The buffer must contain a multiple of 4 amount of 8-bit unsigned values.
 *
 *  Paramaters:
 *      arg - unused
 *
 *      src - pointer to array of packed values (input)
 *
 *      dst - pointer to array of Uns values (output)
 *
 *      size - number of bytes in the src array
 *
 *  Returns:
 *      number of bytes in destination buffer
 */
size_t u8tou32(arg, src, dst, size)
    Arg         arg;
    Uns         *src;
    Uns         *dst;
    size_t      size;
{
    size_t      i;
    size_t      nUns;

    nUns = size / sizeof (Uns);

    for (i = nUns; i > 0; i--) {
        *dst++ = 0xff & (*src >> 24);
        *dst++ = 0xff & (*src >> 16);
        *dst++ = 0xff & (*src >> 8);
        *dst++ = 0xff & *src++;
    }

    return (size * 4);
}
