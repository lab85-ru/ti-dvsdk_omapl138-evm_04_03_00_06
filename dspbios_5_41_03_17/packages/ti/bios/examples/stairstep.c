/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */
/*
 *  ======== stairstep.c ========
 *
 *  This testcase tests the cpuload graph. In cpuloadInit it gets the
 *  CPU frequency using an API and fills the loadValue array with load
 *  values corresponding to 25, 50, 75 and 98 percent cpuload.
 *  The loadPrd which has a small period calls AUDIO_load function 
 *  with loadValue. The stepPrd which has a very long period changes the 
 *  loadValue in a circular fashion. The expected output on the cpuload graph
 *  is a staircase with steps of 25 percent.
 */

#include <std.h>

#include <log.h>
#include <clk.h>
#include <sts.h>
#include <prd.h>
#include <gbl.h>
#include <sys.h>

#include "stairstepcfg.h"

#define LOAD_WINDOW     500     /* window size in milliseconds */
#define TSHIFT  0   /* timer shift */
#define QSHIFT  4   /* fractional arithmetic shift */

#define QFACTOR (1 << QSHIFT)


#if defined(_54_) || defined(_28_) || defined(_55_)
/* to avoid left shift too large warning on 28, 54, 55 platforms */
#define MAX 0x100000
#else
#define MAX (1 << (24 - QSHIFT))
#endif

#define LOAD_STAGE              5

/*
 * LOAD_getcpuload() returns the estimated CPU load as a (truncated) integer.
 */
Int LOAD_getcpuload(void);

/*
 * LOAD_idlefxn() must be configured and called from a DSP/BIOS IDL object.
 * Make sure that 'Include in CPU load calibration' is checked (TRUE).
 */
Void LOAD_idlefxn(void);

/*
 * LOAD_init() must be called from application's main() function.
 */
Void LOAD_init(void);

/*
 *  ======== LOAD_setPeriod ========
 */
Void LOAD_setPeriod(Int new);

Void cpuloadInit();
Void AUDIO_load(Uns load);

Int loadPercent[LOAD_STAGE] = {25, 50, 75, 98, 0};
LgUns loadValue[LOAD_STAGE] = {0, 0, 0, 0, 0};
static LgUns loadVal = 0;
Uns loadPeriod = 0;

/*
 *  ======== main ========
 */
Void main()
{
    cpuloadInit();
    
    LOG_printf(&trace, "stairstep example started!!\n");    

    /* Fall into BIOS idle loop */
    return;                 
}


/*
 *  ======== cpuloadInit ========
 */
Void cpuloadInit() 
{
    PRD_Handle loadHandle = &loadPrd;
    LgUns maxLoad;
    Int i;
        
    if (loadHandle->period > 0) {
        loadPeriod = (Uns)loadHandle->period;
    }
        
    /* maxLoad is maximum instructions per second (100% cpuload) */ 
    maxLoad = loadPeriod * (GBL_getFrequency()/1000);
        
    if (loadValue[0] == 0) {
        /* calculate cpuload increase value */
        for (i = 0; i< LOAD_STAGE; i++) {
            loadValue[i] = loadPercent[i] * maxLoad / 100;
        }
    }
}

/*
 *  ======== load ========
 */
Void load(Void)
{
    static LgUns oldLoad = 0;
    LgUns t;

    /* display confirmation of load changes */
    if (oldLoad != loadVal ) {
        oldLoad = loadVal;
        LOG_printf(&trace, "load: new load = %d000 instructions every %d ms",
                (Uns)loadVal, loadPeriod);
        if (loadVal == loadValue[3]) {  //maxload
            LOG_printf(&trace, "sts0 average should be around 10 millisecond");
        }
    }

    if (loadVal) {
        if (loadVal == loadValue[3]) {  //maxload
            t = CLK_gethtime();
            STS_set(&sts0,t);
            AUDIO_load((Uns)loadVal);
            t = CLK_gethtime();
            STS_delta(&sts0,t);
        }
        else
        {
            AUDIO_load((Uns)loadVal);
        }
    }
    
}

/*
 *  ======== step ========
 */
Void step(void)
{
    static Int loadIndex = 0;
    Uns cpuloadReal;
       
    loadVal = loadValue[loadIndex];
        
    cpuloadReal = LOAD_getcpuload();
    LOG_printf(&trace, "real cpuload change was: %d %%", cpuloadReal); 
            
    LOG_printf(&trace, "calculated cpuload change will be: %d %%", 
            loadPercent[loadIndex]);
    loadIndex++;
    if (loadIndex == LOAD_STAGE) {
        loadIndex = 0;
    }
}

/*
 * 'minloop' is the shortest time thru the IDL loop.  minloop is initialized
 * to the max long unsigned value, and modified by the Load_idlefxn() when
 * it finds a shorter path.
 */
static Uint32 minloop = 0xffffffff;
static Uint32 maxloop = 0x0;

/*
 * 'window' is time window in CLK_gethtime() units.  window is initialzed
 * by Load_init().
 */
static Uint32 window = 0xffffffff;

/*
 * 'cpuload' is the CPU load for the last measured window.
 */
static Int cpuload = 100 * QFACTOR;

/*
 *  ======== Load_getcpuload ========
 */
Int LOAD_getcpuload(void)
{
    return ((cpuload + QFACTOR / 2) / QFACTOR);
}

/*
 *  ======== Load_idlefxn ========
 */
Void LOAD_idlefxn(Void)
{
    static Uint32    count = 0;
    static Uint32    total = 0;
    static Uint32    t0 = 0;
    Uint32           t1;
    Uint32           delta;

    count++;        /* bump number of times thru IDL loop */

    /*
     * Compute the time difference since the last call to Load_idlefxn.
     */
    t1 = CLK_gethtime() >> TSHIFT;
    delta = t1 - t0;
    t0 = t1;

    total += delta; /* accumulate time */

    /*
     * 'minloop' keeps track of the shortest time thru the IDL loop.
     * Note that we check to make sure delta is not zero (delta
     * will be zero during IDL loop calibration)
     */
    if (delta != 0 && delta < minloop) {
        minloop = delta;
    }
    if (delta > maxloop) {
        maxloop = delta;
    }

    /*
     * Compute the CPU load if the time window has expired.
     */
    if (total >= window) {
        Uint32 idle = count * minloop;

        if (total > MAX) {
            /* if total's too big, we scale it down to avoid overflow */
            cpuload = (
                    (100 * QFACTOR * ((total - idle) >> (7 + QSHIFT)))
                    + (total >> (8 + QSHIFT))
                ) / (total >> (7 + QSHIFT));
        }
        else {
            cpuload = 100 * QFACTOR - (
                (100 * QFACTOR * idle + (total >> 1)) / total
            );
        }
        count = total = 0;
    }
}

/*
 *  ======== Load_init ========
 */
Void LOAD_init(void)
{
    LOAD_setPeriod(LOAD_WINDOW);
}

/*
 *  ======== Load_setPeriod ========
 *  Set the CPU load sampling period in ms
 *
 *  This period determines the rate that we compute a new "instaneous" cpu
 *  load value.  
 *
 */
Void LOAD_setPeriod(Int new)
{
    window = (CLK_countspms() * new) >> TSHIFT;
}

/*
 *  There must be a 1 to 1 ratio between CLK_gethtime() and clock cycles here.
 */
Void AUDIO_load( Uns count )
{
    LgInt start, end;

    start = CLK_gethtime();
    end = start + (((LgInt)count * 1000)/CLK_cpuCyclesPerHtime());
    while (CLK_gethtime() < end)
        ;
}
