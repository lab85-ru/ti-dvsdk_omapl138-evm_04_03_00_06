/*
 *  ======== sleep6x.c ========
 *  This program is built for testing PWRM on 6748.
 *
 */

#include <std.h>
#include <log.h>
#include <tsk.h>
#include <clk.h>
#include <pwrm.h>

extern far LOG_Obj trace;

#define REG(x) (*((volatile unsigned *)(x)))

/* RTC register and bit definitions */
#define RTC_SECOND_REG  0x01C23000
#define RTC_ALMSEC_REG  0x01C23020
#define RTC_CTRL_REG    0x01C23040
#define RTC_STATUS_REG  0x01C23044
#define RTC_INTR_REG    0x01C23048
#define RTC_OSC_REG     0x01C23054
#define RTC_KICK0R_REG  0x01C2306C
#define RTC_KICK1R_REG  0x01C23070
#define KICK0_UNLOCK    0x83E70B13
#define KICK1_UNLOCK    0x95A4F1E0
#define RTC_OSC_SWRESET 0x00000020
#define RTC_START       0x00000081
#define RTC_ALARM_INTR  0x00000008
#define RTC_ALARM_BIT   0x00000040

/* PINMUX register and bit definitions */
#define PINMUX0_REG     0x01C14120
#define MUX_MASK        0xF0000000
#define MUX_RTC_ALARM   0x20000000

/* RTC alarm delay for DEEPSLEEP */
#define ALARM_DELAY     0x15

PWRM_NotifyResponse myNotifyFxn(PWRM_Event eventType, Arg eventArg1,
    Arg eventArg2, Arg clientArg);
Void task();

/*
 *  ======== main ========
 */
Void main(int argc, char *argv[])
{
    return;             /* fall into BIOS idle loop (enable tasks to run) */
}

/*
 *  ======== task ========
 */
Void task()
{
    PWRM_NotifyHandle notifyHandle1;
    PWRM_NotifyHandle notifyHandle2;
    PWRM_NotifyHandle notifyHandle3;
    PWRM_NotifyHandle notifyHandle4;
    Fxn delayedCompletionFxn;
    PWRM_Status status;
    Uns current;
    Uns new;

    /* register for sleep notifications */
    status = PWRM_registerNotify(PWRM_GOINGTOSLEEP, 0, myNotifyFxn, 0, 
        &notifyHandle1, &delayedCompletionFxn);
    if (status == PWRM_SOK) {
        LOG_printf(&trace, "registered for GOINGTOSLEEP event");
    }

    status = PWRM_registerNotify(PWRM_AWAKEFROMSLEEP, 0, myNotifyFxn, 0, 
        &notifyHandle2, &delayedCompletionFxn);
    if (status == PWRM_SOK) {
        LOG_printf(&trace, "registered for AWAKEFROMSLEEP event");
    }

    status = PWRM_registerNotify(PWRM_GOINGTODEEPSLEEP, 0, myNotifyFxn, 0, 
        &notifyHandle3, &delayedCompletionFxn);
    if (status == PWRM_SOK) {
        LOG_printf(&trace, "registered for GOINGTODEEPSLEEP event");
    }

    status = PWRM_registerNotify(PWRM_AWAKEFROMDEEPSLEEP, 0, myNotifyFxn, 0, 
        &notifyHandle4, &delayedCompletionFxn);
    if (status == PWRM_SOK) {
        LOG_printf(&trace, "registered for AWAKEFROMDEEPSLEEP event");
    }

    LOG_printf(&trace, "current CLK ticks = %d", CLK_getltime());

    LOG_printf(&trace, "going to STANDBY ...");

    /* go to STANDBY until next CLK tick ... */
    status = PWRM_sleepDSP(PWRM_STANDBY, 0, 0);

    LOG_printf(&trace, "awake from STANDBY, status = 0x%x", status);

    LOG_printf(&trace, "current CLK ticks = %d", CLK_getltime());

    LOG_printf(&trace, "going to SLEEP ...");

    /* go to SLEEP until next CLK tick ... */
    status = PWRM_sleepDSP(PWRM_SLEEP, 0, 0);

    LOG_printf(&trace, "awake from SLEEP, status = 0x%x", status);

    LOG_printf(&trace, "current CLK ticks = %d", CLK_getltime());

    /* unlock RTC registers */
    REG(RTC_KICK0R_REG) = KICK0_UNLOCK;  /* unlock RTC registers */
    REG(RTC_KICK1R_REG) = KICK1_UNLOCK;
  
    /* wait for next CLK tick to start RTC configuration */
    current = new = CLK_getltime();
    while (new == current) {
        new = CLK_getltime();
    }

    /* initiate SW reset of RTC */
    REG(RTC_OSC_REG) |= RTC_OSC_SWRESET;

    /* configure PINMUX for RTC_ALARM functionality */
    REG(PINMUX0_REG) &= ~MUX_MASK;
    REG(PINMUX0_REG) |= MUX_RTC_ALARM;

    /* wait for RTC reset completion */
    current = new = CLK_getltime();
    while (new == current) {
        new = CLK_getltime();
    }

    /* set alarm time (in seconds) */
    REG(RTC_ALMSEC_REG) = ALARM_DELAY;

    /* enable alarm interrupt to signal DEEPSLEEP */
    REG(RTC_INTR_REG) = RTC_ALARM_INTR;

    /* enable isolation and start the RTC */ 
    REG(RTC_CTRL_REG) |= RTC_START;

    LOG_printf(&trace, "current RTC time = %x sec", REG(RTC_SECOND_REG));

    LOG_printf(&trace, "going to DEEPSLEEP ...");

    /* go to DEEPSLEEP until RTC alarm signal ... */
    status = PWRM_sleepDSP(PWRM_DEEPSLEEP, PWRM_RTC_ALARM, 0);

    LOG_printf(&trace, "awake from DEEPSLEEP, status = 0x%x", status);

    LOG_printf(&trace, "current RTC time = %x sec", REG(RTC_SECOND_REG));

    /* un-register for sleep notifications */
    PWRM_unregisterNotify(notifyHandle1);
    PWRM_unregisterNotify(notifyHandle2);
    PWRM_unregisterNotify(notifyHandle3);
    PWRM_unregisterNotify(notifyHandle4);
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
        case PWRM_GOINGTOSLEEP :  
            LOG_printf(&trace, "    myNotifyFxn - GOINGTOSLEEP");
            break;

        case PWRM_AWAKEFROMSLEEP  :  
            LOG_printf(&trace, "    myNotifyFxn - AWAKEFROMSLEEP");
            break;

        case PWRM_GOINGTODEEPSLEEP :  
            LOG_printf(&trace, "    myNotifyFxn - GOINGTODEEPSLEEP");
            break;

        case PWRM_AWAKEFROMDEEPSLEEP  :  
            LOG_printf(&trace, "    myNotifyFxn - AWAKEFROMDEEPSLEEP");
            break;
    }

    return(PWRM_NOTIFYDONE);
}
