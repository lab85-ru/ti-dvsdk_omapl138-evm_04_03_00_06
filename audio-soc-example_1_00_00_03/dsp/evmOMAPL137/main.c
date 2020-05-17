/* --COPYRIGHT--,BSD
 * Copyright (c) 2010, Texas Instruments Incorporated
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
 * --/COPYRIGHT--*/

/*
 *  ======== main.c ========
 */

#include <std.h>

/* DSP/BIOS interface files */
#include <tsk.h>
#include <log.h>

/* DSP/BIOS LINK interface files */
#include <dsplink.h>
#include <failure.h>

/* DSP Platform Support Package (PSP) interface files */
#include <ti/pspiom/platforms/evmOMAPL137/Audio_evmInit.h>

extern Int edma3init();

extern Void processThreadInit();
extern Void inputThreadInit();
extern Void outputThreadInit();

extern Void createOutputStream (Void);

extern Void inputThread(Void);
extern Void processThread(Void);
extern Void outputThread(Void);

/* Statically defined LOG object */
extern LOG_Obj trace;

static Void ThreadInit(Void);

/*  
 *  Define to allow DSP to spin in main() for
 *  debugging DSP code when using DSP/BIOS Link
 */
#define DEBUG_DSP 0

#if DEBUG_DSP
volatile int flag = 1;
#endif

/*
 *  ======== main ========
 *  Set-up the message queues for all TSKs and initialize
 *  DSP/BIOS Link and the PSP audio driver.
 */
void main() 
{
    TSK_Attrs  attrs = TSK_ATTRS;

#if DEBUG_DSP
    while(flag);
#endif

    /* Enable the pinmux for the mcasp device */
    configureAudio();
    LOG_printf(&trace,"Audio config called \n");

    /* Initialize DSP/BIOS LINK */
    DSPLINK_init () ;
    LOG_printf(&trace,"DSPLINK init called \n");
   
    /* Initialize the threads that create message queues */
    LOG_printf(&trace,"Calling Thread init functions\n");
    inputThreadInit();
    outputThreadInit();
    processThreadInit();

    /* Create initialization thread to finalize driver initialization */
    attrs.name = "ThreadInit";
    attrs.stacksize = 0x1000;
    if (TSK_create((Fxn)ThreadInit, &attrs) == NULL) {
        SYS_abort("Failed create echo thread");
    }
  
    return;
}



void ThreadInit ( Void )
{
    TSK_Attrs  attrs = TSK_ATTRS;
    
    /* Initialize EDMA driver */
    LOG_printf(&trace,"Calling edma3init \n");
    edma3init();

    /* Create output SIO stream which must be done within a TSK function */
    LOG_printf(&trace,"Creating outputstream\n");
    createOutputStream();

    /* Create all remaining threads */
    attrs.name = "input";
    attrs.stacksize = 0x1000;
    if (TSK_create((Fxn)inputThread, &attrs) == NULL) {
        SYS_abort("Failed create input thread");
    }

    attrs.name = "process";
    attrs.stacksize = 0x1000;
    if (TSK_create((Fxn)processThread, &attrs) == NULL) {
        SYS_abort("Failed create process thread");
    }

    attrs.name = "output";
    attrs.stacksize = 0x1000;
    if (TSK_create((Fxn)outputThread, &attrs) == NULL) {
        SYS_abort("Failed create output thread");
    }
}

