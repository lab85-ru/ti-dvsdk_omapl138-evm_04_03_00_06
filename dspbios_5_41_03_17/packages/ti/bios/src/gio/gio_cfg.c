/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== gio_cfg.c ========
 *  General Input/Output Module's public header file.
 *
 */

#include <std.h>

#include <gio.h>
#include <sem.h>

/*
 *  A pointer named 'GIO' and a global GIO_CONFIG structure will be initialized
 *  by the configuration tool to point to an GIO_Config structure.  This 
 *  structure will be referenced at run-time so that IOM will not have any
 *  hard-coded reference to SEM_pend, SEM_post, etc. This will allow IOM to
 *  be used in TSK and non-TSK based applications.
 */
GIO_Config GIO_CONFIG = {
    (GIO_TsemCreate)SEM_create,
    (GIO_TsemDelete)SEM_delete,
    (GIO_TsemPend)SEM_pend,
    (GIO_TsemPost)SEM_post
};

GIO_Config *GIO = &GIO_CONFIG;
