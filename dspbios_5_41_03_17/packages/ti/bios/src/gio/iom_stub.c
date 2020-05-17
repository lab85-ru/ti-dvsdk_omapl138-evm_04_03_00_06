/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== iom_stub.c ========
 *   IOM mini-driver fxn stub for functions not implemented.
 *
 */
 
#include <std.h>

#include <iom.h>

#pragma CODE_SECTION(IOM_mdNotImpl, ".bios")

/*
 *  ======== IOM_mdNotImpl ========
 * 
 *  Convenient mini-driver not implemented stub function.
 *  See iom.h file for #define names IOM_[xxx]NOTIMPL used in 
 *  mini-driver fxn table to stub out a fxn.
 */
Int IOM_mdNotImpl(Void)
{
    return (IOM_ENOTIMPL);
}
