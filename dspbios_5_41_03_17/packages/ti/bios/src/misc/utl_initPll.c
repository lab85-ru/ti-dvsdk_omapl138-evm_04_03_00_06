/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== utl_initPll ========
 *  This file contains the function that sets up the PLL for 64LC parts
 *  The function needs to be used as an initialization function which
 *  will be called before main(). Use following tconf options.
 *
 *  bios.GBL.CALLUSERINITFXN = true;
 *  bios.GBL.USERINITFXN = prog.extern("UTL_initPll");
 *
 */

#pragma CODE_SECTION(UTL_initPll, ".sysinit");
#pragma CODE_SECTION(pllWait, ".sysinit");

#include <std.h>

/* LC specific PLL0 registers */
#define PLL0_PLLCTL             (volatile Uint32 *)0x01C40900
#define PLL0_PLLM               (volatile Uint32 *)0x01C40910
#define PLL0_PLLDIV1            (volatile Uint32 *)0x01C40918
#define PLL0_PLLDIV2            (volatile Uint32 *)0x01C4091C
#define PLL0_PLLDIV3            (volatile Uint32 *)0x01C40920

/*
 *  Default CPU speed is set for 594.0 Mhz.  This is assuming an input
 *  frequency clock of 27.0 Mhz and a multiplier factor of 22.
 *  (NOTE: set pllm to 21 to get a multiplier value of 22.  Set div values
 *  to 0 to get a divide factor of 1.)
 */
#define PLL_MULT        21
#define PLL_DIV1        0
#define PLL_DIV2        0
#define PLL_DIV3        0

Void pllWait(Int count);

/*
 *  ======== UTL_initPll ========
 *  This function sets up the PLL multiplier and divider factors to the
 *  specified values above.
 */
Void UTL_initPll()
{
    volatile Uint32 *pllctl = PLL0_PLLCTL;
    volatile Uint32 *pllm = PLL0_PLLM;
    volatile Uint32 *plldiv1 = PLL0_PLLDIV1;
    volatile Uint32 *plldiv2 = PLL0_PLLDIV2;
    volatile Uint32 *plldiv3 = PLL0_PLLDIV3;

    *pllctl &= 0xffffffde;      /* set PLL to bypass mode */

    pllWait(0x10);              /* wait at least 4 cycles of slowest clock   */
                                /* for PLL to be in bypass mode              */

    *pllctl &= 0xfffffff7;      /* reset PLL (PLLRST = 1) */

    *pllctl |= 0x00000010;      /* disable PLL */

    *pllctl &= 0xFFFFFFFD;      /* power up PLL */
    
    *pllctl &= 0xFFFFFFEF;      /* enable PLL */

    *pllm = PLL_MULT;           /* set PLL multipler */

    *plldiv1 = PLL_DIV1;                /* set PLL divider 1 */
    *plldiv1 |= 0x00008000;     /* enable PLL divider 1 */
    *plldiv2 = PLL_DIV2;                /* set PLL divider 2 */
    *plldiv2 |= 0x00008000;     /* enable PLL divider 2 */
    *plldiv3 = PLL_DIV3;                /* set PLL divider 3 */
    *plldiv3 |= 0x00008000;     /* enable PLL divider 3 */

    pllWait(0x10);              /* wait for PLL to reset */

    *pllctl |= 0x00000008;      /* set PLLRST = 1 to deassert PLL reset */

    pllWait(0x1000);            /* wait at least 2000 clock cycles for  */
                                /* PLL to lock                          */

    *pllctl |= 0x00000001;      /* set PLL to PLL mode */
}

/*
 *  ======== pllWait ========
 *  This function is a spin loop for the specified count parameter.
 *  Its used to stall the CPU when setting up the PLL.
 */
Void pllWait(Int count)
{
    volatile Int dummy;
    Int i;

    for (i=0; i < count; i++) {
        dummy += 1;
    }
}
