/*
 *  Copyright 2007 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 *  @(#) PSL 5,1,0 09-24-2007 (psl-c03)
 */
#include "timer.h"

// Include CSL support
#include "csl.h"
#include "csl_timer.h"
#include "csl_irq.h"

extern PSL_ClkID clk;

TIMER_Handle timer0Handle;

unsigned timer0Cnt = 0;


//------------------------------------------------------------------------------
// Function to stop the timer.  Called immediately before a scaling operation 
// is initiated.
//------------------------------------------------------------------------------
void StopTimer0(unsigned     count,
                PSL_ClkID    *clks,
                PSL_Setpoint *currentSetpoints, 
                PSL_Setpoint *newSetpoints) {
                
   TIMER_stop(timer0Handle);

}


//------------------------------------------------------------------------------
// Function to reprogram and restart the Timer0. Called immediately after a 
// scaling operation completes.
//
// Assumes that Timer0 is alreadly stopped
//------------------------------------------------------------------------------
void RestartTimer0(unsigned     count,
                   PSL_ClkID    *clks,
                   PSL_Setpoint *oldSetpoints, 
                   PSL_Setpoint *currentSetpoints) {
   
   float currFreq;
   unsigned long cycles;
   
   Uint16 timer0TCR;
   
   // Set timer loading (TLB) bit prior to initializing the period
   // and prescale registers.
   timer0TCR = TIMER_RGETH(timer0Handle, TCR);
   timer0TCR |= TIMER_FMK(TCR,TLB,1);           // TLB = 1;
   TIMER_RSETH(timer0Handle, TCR, timer0TCR);
   
   // Reprogram the period and prescale register such that the
   // interrupt period is 10 microseconds.  The actual number of
   // CPU cycles is determined based on the current CPU frequency.
   
   PSL_querySetpointFrequencies(1, clks, currentSetpoints, &currFreq);

   cycles = (unsigned long)(10.0f * currFreq);
   TIMER_FSETH(timer0Handle, PRD, PRD, cycles & 0xFFFF);        // Write PRD register
   TIMER_FSETH(timer0Handle, PRSC, TDDR, (cycles >> 16) & 0xF); // Write TDDR field of PRSC register
   
   // Restart the timer
   TIMER_start(timer0Handle);
}


//------------------------------------------------------------------------------
// Timer0 interrupt handler.
//------------------------------------------------------------------------------
interrupt void Timer0Isr(void)
{
    ++timer0Cnt;   // count number of interrupts
}


//------------------------------------------------------------------------------
// Function to configure and start Timer0.  Also configures and enables
// Timer0 interrupt.
//------------------------------------------------------------------------------
void SetupTimer0() {
   Uint16 eventId0;
   
   Uint16 timer0TCR;
   Uint16 timer0PRD;
   Uint16 timer0PRSC;
   
   float currFreq;
   unsigned long cycles;

   // Set interrupt vector pointers to internal memory
   asm("        MOV #2, mmap(IVPD)");
   asm("        MOV #2, mmap(IVPH)");
   
   // Initialize the CSL Library.  A one-time only initialization of the 
   // CSL library must be done before calling any CSL module API.
   CSL_init();
   
   // Open Timer 0, set registers to power on defaults
   timer0Handle = TIMER_open(TIMER_DEV0, TIMER_OPEN_RESET);

   // Get Event Id associated with Timer 0, for use with CSL interrupt 
   // enable functions.                    */         
   eventId0 = TIMER_getEventId(timer0Handle);

   /* Clear any pending Timer interrupts */
   IRQ_clear(eventId0);             

   // Configure the Timer0 registers
    
   // Timer0 Control register
   timer0TCR = TIMER_TCR_RMK(\
                      TIMER_TCR_IDLEEN_DEFAULT,    /* IDLEEN == 0 */ \
                      TIMER_TCR_FUNC_OF(0),        /* FUNC   == 0 */ \
                      TIMER_TCR_TLB_RESET,         /* TLB    == 1 */ \
                      TIMER_TCR_SOFT_BRKPTNOW,     /* SOFT   == 0 */ \
                      TIMER_TCR_FREE_WITHSOFT,     /* FREE   == 0 */ \
                      TIMER_TCR_PWID_OF(0),        /* PWID   == 0 */ \
                      TIMER_TCR_ARB_RESET,         /* ARB    == 1 */ \
                      TIMER_TCR_TSS_STOP ,         /* TSS    == 1 */ \
                      TIMER_TCR_CP_PULSE,          /* CP     == 0 */ \
                      TIMER_TCR_POLAR_LOW,         /* POLAR  == 0 */ \
                      TIMER_TCR_DATOUT_0           /* DATOUT == 0 */ \
                 );
                      
   // Configure the period and prescale register such that the
   // interrupt period is 10 microseconds.  The actual number of
   // CPU cycles is determined based on the current CPU frequency.
   PSL_getFrequencies(1, &clk, &currFreq);
   cycles = (unsigned long)(10.0f * currFreq);
   
   timer0PRD  = TIMER_PRD_RMK(cycles & 0xFFFF);
                
   timer0PRSC = TIMER_PRSC_RMK(0, (cycles >> 16) & 0xF);  /* PSC, TDDR */
    
   TIMER_configArgs(timer0Handle,       
                    timer0TCR,  
                    timer0PRD,  
                    timer0PRSC);
 
   // Enable Timer interrupt
   IRQ_enable(eventId0);             
   
   // Enable all maskable interrupts
   IRQ_globalEnable();      
 
   //  Start Timer
   TIMER_start(timer0Handle);
        
}


void CloseTimer0 () {
   TIMER_stop(timer0Handle);
   TIMER_close(timer0Handle);
}

