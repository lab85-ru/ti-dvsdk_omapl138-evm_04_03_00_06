/* 
 * Copyright (c) 2009, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Use of this software is controlled by the terms and conditions found in
 * the New BSD Software License Agreement, which is reproduced in its
 * entirity below.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/*
 *  ======== Load.c ========
 */

#include <ti/bios/include/std.h>
#include <ti/bios/include/clk.h>
#include <ti/bios/include/sys.h>

#include "Load.h"

#define TSHIFT  0   /* timer shift */
#define QSHIFT  4   /* fractional arithmetic shift */

#define QFACTOR (1 << QSHIFT)

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
static Int aveload = 100 * QFACTOR;

/*
 *  ======== Load_getcpuload ========
 */
Int Load_getcpuload(void)
{
    return ((aveload + QFACTOR / 2) / QFACTOR);
}

/*
 *  ======== Load_idlefxn ========
 */
Void Load_idlefxn(Void)
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
        

        /* assert(idle < total) */
        if (total > (1 << (24 - QSHIFT))) {
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
        aveload = (3 * aveload + cpuload) / 4;
        count = total = 0;
    }
}

/*
 *  ======== Load_init ========
 */
Void Load_init(void)
{
    Load_setPeriod(Load_WINDOW);
}

/*
 *  ======== Load_setPeriod ========
 *  Set the CPU load sampling period in ms
 *
 *  This period determines the rate that we compute a new "instaneous" cpu
 *  load value.  The average CPU load is obtained by averaging this time
 *  series using a first-order low pass filter.
 */
Void Load_setPeriod(Int new)
{
    window = (CLK_countspms() * new) >> TSHIFT;
}

/*
 *  ======== Load_setWindow ========
 */
Void Load_setWindow(Uint32 new)
{
    window = new;
}

/*
 *  ======== Load_getWindow ========
 */
Uint32 Load_getWindow(Void)
{
    return (window);
}
/*
 *  ======== Load_getMinLoop ========
 */
Uint32 Load_getMinLoop(Void)
{
    return (minloop);
}

/*
 *  ======== Load_getMaxLoop ========
 */
Uint32 Load_getMaxLoop(Void)
{
    return (maxloop);
}

/*
 *  ======== Load_reset ========
 */
Void Load_reset(Void)
{
    minloop = 0xffffffff;
    maxloop = 0x0;
}
/*
 *  @(#) ti.bios.utils; 2, 0, 2, 02,31; 5-23-2009 17:29:34; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

