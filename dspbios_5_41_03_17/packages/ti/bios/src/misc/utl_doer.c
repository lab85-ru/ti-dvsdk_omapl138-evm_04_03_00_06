/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== utl_doer.c ========
 *
 *  An error function, typically configured in the SYS module.
 *
 */

#pragma CODE_SECTION(UTL_doError, ".bios")

#include <std.h>
#include <sys.h>
#include <log.h>
#include <stdarg.h>
#include <utl.h>

/* 
 *  ======== UTL_doError ========
 */
Void UTL_doError(String s, Int errno, va_list ap)
{
#if UTL_USESYS
    SYS_printf("*** %s ERROR 0x%x ", s, errno);

    if (errno < SYS_EUSER) {
        SYS_vprintf(SYS_errors[errno], ap);
    }

    SYS_putchar('\n');
#else
    LOG_error("SYS_error called: error id = 0x%x", (Arg)errno);
#endif

}
