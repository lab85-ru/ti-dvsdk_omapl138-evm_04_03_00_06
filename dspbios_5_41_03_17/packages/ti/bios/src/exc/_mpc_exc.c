/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== _mpc_exc.c ========
 *
 */

#include <std.h>

#include <exc.h>
#include <fxn.h>
#include <log.h>
#include <_mpc.h>

Bool noPrint = FALSE;
Uns _MPC_mpFar[3];
Uns _MPC_mpFsr[3];
EXC_Status _MPC_excStatus;
Void (*_MPC_userHook)(Void) = FXN_F_nop;

extern far LOG_Obj trace;

#pragma CODE_SECTION(_MPC_init, ".bios")
Void _MPC_init(Void)
{
    EXC_exceptionHook = _MPC_exceptionHandler;
    EXC_internalHook = _MPC_internalHandler;
    EXC_externalHook = _MPC_externalHandler;

    /* clear any exceptions that might be left over from the last run */
    noPrint = TRUE;
    _MPC_externalHandler();
    noPrint = FALSE;

    EXC_evtExpEnable(EXC_EVTPMCCMPA);
    EXC_evtExpEnable(EXC_EVTDMCCMPA);
    EXC_evtExpEnable(EXC_EVTUMCCMPA);

    EXC_init();
}

#pragma CODE_SECTION(_MPC_exceptionHandler, ".bios")
Void _MPC_exceptionHandler(Void)
{
    _MPC_excStatus = EXC_getLastStatus();

    (*_MPC_userHook)();
}

#pragma CODE_SECTION(_MPC_internalHandler, ".bios")
Void _MPC_internalHandler(Void)
{
    _MPC_excStatus = EXC_getLastStatus();
}

#pragma CODE_SECTION(_MPC_externalHandler, ".bios")
Void _MPC_externalHandler(Void)
{
    Int i;

    for (i = 0; i < 3; i++) {
        _MPC_mpFar[i] = 0;
        _MPC_mpFsr[i] = 0;
    }

    _MPC_decodeException(_MPC_PMC);
    _MPC_decodeException(_MPC_DMC);
    _MPC_decodeException(_MPC_UMAP0);
}

#pragma CODE_SECTION(_MPC_decodeException, ".bios")
Void _MPC_decodeException(Uns id)
{
    _MPC_Controller *ctrl;
    _MPC_Fault *mpfault;
    Uns mpfsr;
    Uns mpfar;
    Uns mask;
    Uns shift;
    Char *idStr;

    ctrl = _MPC_controllers[id - 1];

    mpfault = ctrl->mpFault;

    mpfsr = mpfault->mpFsr;
    mpfar = mpfault->mpFar;

    _MPC_mpFar[id] = mpfar;
    _MPC_mpFsr[id] = mpfsr;

    if (mpfsr != NULL) {
        if (!noPrint) {
            mask = _MPC_MPFSR_FIDMASK;
            shift = _MPC_MPFSR_FIDSHIFT;
            idStr = (id == _MPC_DMC ? "DMC" : (id == _MPC_PMC ? "PMC" : (id == _MPC_UMAP0 ? "UMC" : (id == _MPC_UMAP1 ? "UMC" : ""))));
            LOG_error("%s:", idStr);
            LOG_error("MPC exception @0x%x", mpfar);
            LOG_error("  MPFSR=0x%x", mpfsr);
            LOG_error("  Faulted ID=0x%x", (mpfsr & mask) >> shift);

            if (mpfsr & _MPC_MPFSR_SECE) {
                LOG_error("  Security violation", 0);
            }

            if (mpfsr & _MPC_MPFSR_UXE) {
                LOG_error("  User Execute violation", 0);
            }
            if (mpfsr & _MPC_MPFSR_UWE) {
                LOG_error("  User Write violation", 0);
            }
            if (mpfsr & _MPC_MPFSR_URE) {
                LOG_error("  User Read violation", 0);
            }
            if (mpfsr & _MPC_MPFSR_SXE) {
                LOG_error("  Supervisor Execute violation", 0);
            }
            if (mpfsr & _MPC_MPFSR_SWE) {
                LOG_error("  Supervisor Write violation", 0);
            }
            if (mpfsr & _MPC_MPFSR_SRE) {
                LOG_error("  Supervisor Read violation", 0);
            }
        }

        /* write 1 to MPFCR to clear MPFSR & MPFAR registers */
        mpfault->mpFcr = 1;
    }

    /*
     * Need to clear event so CPU can see a 0->1 transition for the
     * next one
     */
    EXC_evtEvtClear(ctrl->evtNum);
}


#pragma CODE_SECTION(_MPC_getLastMPFAR, ".bios")
Uint32 _MPC_getLastMPFAR(Uns id)
{
    return _MPC_mpFar[id];
}

#pragma CODE_SECTION(_MPC_getLastMPFSR, ".bios")
Uint32 _MPC_getLastMPFSR(Uns id)
{
    return _MPC_mpFsr[id];
}

