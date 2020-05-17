/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== exc.c ========
 *
 */

#include <std.h>

#include <exc.h>
#include <fxn.h>
#include <log.h>
#include <sys.h>

Void EXC_internal(Void);
Void EXC_external(Void);
Void EXC_nmi(Void);

Void (*EXC_exceptionHook)(Void) = FXN_F_nop;
Void (*EXC_internalHook)(Void) = FXN_F_nop;
Void (*EXC_externalHook)(Void) = FXN_F_nop;
Void (*EXC_nmiHook)(Void) = FXN_F_nop;

EXC_Status EXC_status;


#pragma CODE_SECTION(EXC_init, ".bios")
/*
 *  ======== EXC_init ========
 *
 * EXC_init must be called to enable events to cause an exception.
 *
 * This is a default implementation and can be expanded/changed to suit
 * your needs.
 */
Void EXC_init(Void)
{
    extern volatile cregister unsigned TSR;

    /* enable EXCEP input to generate an NMI interrupt */
    TSR |= EXC_TSRXEN | EXC_TSRGEE;

    EXC_status.efr = 0;
    EXC_status.nrp = 0;
    EXC_status.ntsr = 0;
    EXC_status.ierr = 0;
}

#pragma CODE_SECTION(EXC_exceptionHandler, ".bios")
/*
 *  ======== EXC_exceptionHandler ========
 *
 * EXC_exceptionHandler is an example exception handler.
 *
 * Since this is a default implementation, it can be expanded/changed to suit
 * your needs.
 */
Void EXC_exceptionHandler(Bool abortFlag)
{
    extern volatile cregister unsigned EFR;
    extern volatile cregister unsigned ECR;
    extern volatile cregister unsigned NRP;
    extern volatile cregister unsigned NTSR;
    unsigned efr;

    efr = EFR;
    EXC_status.efr = efr;               /* record EFR in EXC_Status */
    EXC_status.nrp = NRP;               /* record NRP in EXC_Status */
    EXC_status.ntsr = NTSR;             /* record NTSR in EXC_Status */

    LOG_error("EXC_exceptionHandler: EFR=0x%x", EXC_status.efr);
    LOG_error("  NRP=0x%x", EXC_status.nrp);
    LOG_error("  mode=%s", EXC_status.ntsr & 0x40 ? "user" : "supervisor");

    (*EXC_exceptionHook)();

    /* clear flags in EFR */
    ECR = efr;

    /* process all possible causes of exception */
    if (efr & EXC_EFRIXF) {
        /* internal exception */
        efr ^= EXC_EFRIXF;
        EXC_internal();
    }
    if (efr & EXC_EFREXF) {
        /* external exception */
        efr ^= EXC_EFREXF;
        EXC_external();
    }
    if (efr & EXC_EFRNXF) {
        /* legacy NMI exception */
        efr ^= EXC_EFRNXF;
        EXC_nmi();
    }

    if (abortFlag) {
        SYS_abort("Run-time exception detected, aborting ...");
    }
}

#pragma CODE_SECTION(EXC_internal, ".bios")
Void EXC_internal(Void)
{
    extern volatile cregister unsigned IERR;
    Uint32 ierr;

    ierr = IERR;

    /* record IERR in EXC_status field */
    EXC_status.ierr = ierr;

    LOG_error("Internal exception: IERR=0x%x", ierr);

    if (ierr & EXC_IERRIFX) {
        LOG_error("  Instruction fetch exception", 0);
    }
    if (ierr & EXC_IERRFPX) {
        LOG_error("  Fetch packet exception", 0);
    }
    if (ierr & EXC_IERREPX) {
        LOG_error("  Execute patcket exception", 0);
    }
    if (ierr & EXC_IERROPX) {
        LOG_error("  Opcode exception", 0);
    }
    if (ierr & EXC_IERRRCX) {
        LOG_error("  Resource conflict exception", 0);
    }
    if (ierr & EXC_IERRRAX) {
        LOG_error("  Resource access exception", 0);
    }
    if (ierr & EXC_IERRPRX) {
        LOG_error("  Privilege exception", 0);
    }
    if (ierr & EXC_IERRLBX) {
        LOG_error("  Loop buffer exception", 0);
    }
    if (ierr & EXC_IERRMS) {
        LOG_error("  Missed stall", 0);
    }

    (*EXC_internalHook)();

    /* clear internal exceptions to allow them to be recognized again */
    IERR = 0;
}

#pragma CODE_SECTION(EXC_external, ".bios")
Void EXC_external(Void)
{
    LOG_error("External exception:", 0);

    (*EXC_externalHook)();
}

#pragma CODE_SECTION(EXC_nmi, ".bios")
Void EXC_nmi(Void)
{
    LOG_error("Legacy NMI exception:", 0);

    (*EXC_nmiHook)();
}

#pragma CODE_SECTION(EXC_getLastStatus, ".bios")
EXC_Status EXC_getLastStatus(Void)
{
    return EXC_status;
}

#pragma CODE_SECTION(EXC_clearLastStatus, ".bios")
Void EXC_clearLastStatus(Void)
{
    EXC_status.efr = 0;
    EXC_status.nrp = 0;
    EXC_status.ntsr = 0;
    EXC_status.ierr = 0;
}

