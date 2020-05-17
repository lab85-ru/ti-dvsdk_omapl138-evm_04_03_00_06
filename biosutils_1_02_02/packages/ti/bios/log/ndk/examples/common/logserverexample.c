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

#include <ti/bios/include/std.h>
#include <ti/bios/include/clk.h>
#include <ti/bios/include/log.h>

#include <ti/bios/log/ndk/LogServerCgi.h>
#include <ti/bios/log/support/LogAux.h>
#include <ti/bios/log/support/LogTrack.h>

extern LOG_Obj logTrace;
extern LOG_Obj logTest;
extern LOG_Obj trace;


/* set user configurable values */
LogServerCgi_Config LogServerCgi_config =
        {2,   /* set each CGI thread priority to 2 */
        0,    /* set the memory segment ID for MEM_alloc calls to seg 0 */
        1024, /* set the size for the buffer that will hold log data */
        };

Void logTracePrdfxn(Void); 
Void logTestPrdfxn(Void);

/* Function for tasks created with Config Tool */ 
Void task(Arg id_arg, Arg time_arg);

/*
 *  Main Entry Point
 */
int main()
{
}

/*
 *  logTracePrd
 *
 *  This fxn runs periodically and continuously updates the logTrace log.
 *
 */
Void logTracePrdfxn()
{
    LOG_printf4(&logTrace, "logTrace data: %c %c %c %c\n", 'p', 'q', 'r', 's');
}

/*
 *  logTestPrd
 *
 *  This fxn runs periodically and continuously updates the logTest log.
 *
 */
Void logTestPrdfxn()
{
    LOG_printf4(&logTest, "logTest data: %c %c %c %c\n", 'h', 'i', 'j', 'k');
}

/*
 *  ======== task ========
 */
Void task(Arg id_arg, Arg time_arg)
{
    Int     id = ArgToInt (id_arg);
    Int     time = ArgToInt (time_arg);
    LgUns   currTime;
    LgUns   startTime;

    /* this outer while loop runs once per task run */
    while (1) {
        LOG_printf(&trace, "Task %d  starting!\n", id);
        startTime = CLK_getltime(); /* time when task begins */

        do {
            currTime = CLK_getltime();
        } while (currTime - startTime < time);
 
        LOG_printf(&trace, "Task %d going to sleep ...\n", id);
        TSK_sleep(300);
    }
}

/*
 *  ======== logserverexample.c ========
 *
 */
/*
 *  @(#) ti.bios.log.ndk.examples; 1, 0, 2, 02,34; 5-23-2009 17:29:16; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

