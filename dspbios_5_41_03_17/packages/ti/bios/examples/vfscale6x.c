/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */
/*
 *  ======== vfscale6x.c ========
 *  This program is built for testing PWRM on 6748.
 *
 */

#include <std.h>
#include <log.h>
#include <clk.h>
#include <pwrm.h>

extern far LOG_Obj trace;

PWRM_NotifyResponse myNotifyFxn(PWRM_Event eventType, Arg eventArg1,
    Arg eventArg2, Arg clientArg);
Void task();
Uns getSpeedCPU();

/*
 *  ======== main ========
 */
Void main(int argc, char *argv[])
{
    return;		/* fall into BIOS idle loop (enable tasks to run) */
}

/*
 *  ======== task ========
 */
Void task()
{
    PWRM_NotifyHandle notifyHandle1;
    PWRM_NotifyHandle notifyHandle2;
    Fxn delayedCompletionFxn;
    PWRM_Status status;
    Uns numSP;
    Uns freq;
    Uns volt;
    Uns SP;
    Uns i;

    /* register for setpoint change notifications */
    status = PWRM_registerNotify(PWRM_PENDING_CPU_SETPOINTCHANGE, 0, 
        myNotifyFxn, 0, &notifyHandle1, &delayedCompletionFxn);
    if (status == PWRM_SOK) {
        LOG_printf(&trace, "registered for PENDING_CPU_SETPOINTCHANGE event");
    }

    status = PWRM_registerNotify(PWRM_DONE_CPU_SETPOINTCHANGE, 0, 
        myNotifyFxn, 0, &notifyHandle2, &delayedCompletionFxn);
    if (status == PWRM_SOK) {
        LOG_printf(&trace, "registered for DONE_CPU_SETPOINTCHANGE event");
    }

    /* get number of setpoints supported */
    status = PWRM_getNumSetpoints(PWRM_CPU, &numSP);
    if (status == PWRM_SOK) {
        LOG_printf(&trace, "number CPU setpoints = %d", numSP);
    }

    /* display setpoint information */
    LOG_printf(&trace, "CPU setpoint info:");
    for (i = 0; i < numSP; i++) {
        status = PWRM_getSetpointInfo(PWRM_CPU, i, &freq, &volt);
        if (status == PWRM_SOK) {
            LOG_printf(&trace, "  setpoint %d:", i);
            LOG_printf(&trace, "     freq = %dMHz, voltage = %dmV", 
                (freq / 1000), volt);
        }
    }

    /* get current setpoint */
    status = PWRM_getCurrentSetpoint(PWRM_CPU, &SP);
    if (status == PWRM_SOK) {
        LOG_printf(&trace, "current CPU setpoint = %d", SP);
    }

    /* move to each supported setpoint */
    for (i = 0; i < numSP; i++) {
        LOG_printf(&trace, "changing to CPU setpoint %d ...", i);
        status = PWRM_changeSetpoint(PWRM_CPU, i, 0);
        if (status == PWRM_SOK) {
            LOG_printf(&trace, "new setpoint = %d, measured CPU speed = %dMHz",
                i, getSpeedCPU());
        }
    }

    /* un-register for setpoint change notifications */
    PWRM_unregisterNotify(notifyHandle1);
    PWRM_unregisterNotify(notifyHandle2);
}


/*
 *  ======== getSpeedCPU ========
 */
Uns getSpeedCPU()
{
    Uns cycles;
    Uns round;
    Uns tick1;
    Uns tick2;
    Uns tsc1;
    Uns tsc2;

    /* wait for next tick before counting cycles */
    tick1 = tick2 = CLK_getltime();
    while (tick1 == tick2) {
        tick2 = CLK_getltime();
    }

    /* now wait for the next full CLK tick for cycle count */
    tick1 = tick2 = CLK_getltime();
    tsc1 = CLK_gethtime();
    while (tick1 == tick2) {
        tick2 = CLK_getltime();
        tsc2 = CLK_gethtime();
    }

    /* compute elapsed CPU cycles */
    cycles = tsc2 - tsc1;

    /* check if should round up to next MHz (assumes CLK ticks at 1 msec) */
    round = cycles % 1000;
    if (round >= 500) {
        round = 1;
    }
    else {
        round = 0;
    }

    /* return measured MHz */
    return((cycles / 1000) + round);
}


/*
 *  ======== myNotifyFxn ========
 */
/* ARGSUSED */
PWRM_NotifyResponse myNotifyFxn(PWRM_Event eventType, Arg eventArg1,
    Arg eventArg2, Arg clientArg)
{
    switch (eventType)
    {
        case PWRM_PENDING_CPU_SETPOINTCHANGE  :
            LOG_printf(&trace, "    myNotifyFxn - PENDING_CPU_SETPOINTCHANGE");
            LOG_printf(&trace, "      current = %d, next = %d", eventArg1, 
                eventArg2);
            break;

        case PWRM_DONE_CPU_SETPOINTCHANGE  :
            LOG_printf(&trace, "    myNotifyFxn - DONE_CPU_SETPOINTCHANGE");
            LOG_printf(&trace, "      previous = %d, current = %d", eventArg1, 
                eventArg2);
            break;

    }

    return(PWRM_NOTIFYDONE);
}
