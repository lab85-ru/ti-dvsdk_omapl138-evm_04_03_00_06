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
 * ======== Load.h ========
 *
 */

/*
 * The Load module provides an API (Load_getcpuload()) that returns the
 * current CPU load.  The CPU load is averaged over a period of time as
 * specified by the 'window' variable within load.c.
 * 
 *  It is necessary to do the following in order to use Load_getcpuload()
 * 
 * (1) A BIOS IDL object which has its function set to '_Load_idlefxn' must
 *     exist.  The code to do this has already been written for the user, and
 *     exists in 2 different files:
 *     
 *     a. ti/bios/utils/Load.tci
 *         If the user's app is built using Tconf, then this file must be
 *         included into the build by adding the following statement into the
 *         *.tcf file:
 *     
 *         utils.importFile('Load.tci');
 *
 *     b. ti/bios/utils/package.xs
 *         If the user's app is built using RTSC, then one must use the Load
 *         module. This is done by adding the following line to the *.cfg file:
 *     
 *         var Load = xdc.useModule('ti.bios.utils.Load');
 *     
 *     NOTE: The two above mentioned methods are mutually exclusive.  One can't
 *           use both method 'a' and method 'b' in the same program build, the
 *           user must choose the appropriate one for your build.
 *
 * (2) call 'Load_init()' from your application's main() program
 * 
 * (3) call 'Load_getcpuload()' to return the CPU load as averaged over
 *     the pre-compiled window.  This window can be changed by modifying
 *     the Load_WINDOW value in this file and recompiling load.c.
 * 
 * (4) compile load.c and link it with your application.
 * 
 * 
 * CAVEATS:
 * 
 * (1) This implementation self-calibrates the shortest path through the
 *     IDL loop.  It does this by keeping track of the shortest time between 
 *     invocations of 'Load_idlefxn'.  This relies on the fact that the
 *     IDL loop will execute at least 2 times in a row w/o being interrupted!
 *     In the unlikely event that is a problem for your application, you can
 *     measure the shortest path through the IDL loop using the profiler and
 *     set 'minloop' to this time.  Note that the units of minloop are
 *     CLK_gethtime() units which are processor dependent; e.g., CPU/4 for
 *     'C6211, CPU/(TDDR+1) for 'C54xx, CPU for C64P, etc..
 */ 
#ifndef Load_
#define Load_

#ifdef __cplusplus
extern "C" {
#endif

#define Load_WINDOW     128     /* default sampling period in milliseconds */

/*
 *  ======== Load_getcpuload ========
 *  Load_getcpuload() returns the estimated CPU load as a (truncated) integer.
 */
extern Int Load_getcpuload(void);

/*
 *  ======== Load_idlefxn ========
 *  Load_idlefxn() must be configured and called from a DSP/BIOS IDL object.
 *  Make sure that 'Include in CPU load calibration' is checked (TRUE).
 */
extern Void Load_idlefxn(void);

/*
 *  ======== Load_init ========
 *  Load_init() must be called from application's main() function.
 */
extern Void Load_init(void);

/*
 *  ======== Load_setPeriod ========
 */
extern Void Load_setPeriod(Int new);

/*
 *  ======== Load_setWindow ========
 */
extern Void Load_setWindow(Uint32 new);

/*
 *  ======== Load_getWindow ========
 */
extern Uint32 Load_getWindow(Void);

/*
 *  ======== Load_reset ========
 */
extern Void Load_reset(Void);

#ifdef __cplusplus
}
#endif

#endif
/*
 *  @(#) ti.bios.utils; 2, 0, 2, 02,31; 5-23-2009 17:29:34; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */
