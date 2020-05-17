/*
 * Intc_example.c
 *
 * This file contains the test / demo code to demonstrate basic INTC operations 
 * using the Regsiter CSL macros.
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/** ============================================================================
 *  @brief  A simple example to demonstrate CSL register layer (for DSP INTC)
 *   usage and interrupts.
 *
 *  @file  interrupt_example.c
 *
 *  <b> Example description </b>
 *  @verbatim
    The interrupt example is a non-bios example that uses TIMER0 to generate cpu
    INT4.  TIMER0 is configured in 32bit unchained once mode and its event is
    mapped CPU interrupt 4.  The test enables TIMER0 and verifies that the
    interrupt service routine is executed when the period value is reached.
    A pass/fail status is returned to main.
    @endverbatim
 *
 *
 *  @n <b> Procedure to run the example </b>
 *  @verbatim
    1. Configure the CCS setup to work with the emulator being used.
    2. Please refer CCS manual for setup configuration and loading
       proper GEL file.
    3. Launch CCS.
    4. Open project Intc_example.pjt.
    5. Build the project and load the .out file for the project.
    @endverbatim
 *
 *  ============================================================================
 **/

#include <stdio.h>
#include <c6x.h>
#include <ti/pspiom/cslr/soc_OMAPL138.h>
#include <ti/pspiom/cslr/cslr_dspintc.h>
#include <ti/pspiom/cslr/cslr_tmr.h>
#include <ti/pspiom/cslr/cslr_syscfg0_OMAPL138.h>


static void setup(void);
static int test_interrupt(void);
extern void intcVectorTable(void);
static void device_init(void);

/* timer period setting                                                       */
const unsigned int TIMER_PERIOD = 0x00006978;
/* interrupt flag                                                             */
int intFlag=1;

/* TIMER0 Register Overlay                                                    */
CSL_TmrRegsOvly tmr0Regs = (CSL_TmrRegsOvly)CSL_TMR_0_REGS;
/* Interrupt Controller Register Overlay                                      */
CSL_DspintcRegsOvly intcRegs = (CSL_DspintcRegsOvly)CSL_INTC_0_REGS;

int main (void)
{
    /* enable TIMER0 in MUX and PSC                                           */
    device_init();
    
    /* setup TIMER0 and interrupt                                             */
    setup();
    
    /* test interrupt, returns 0 for pass and 1 for fail                      */
    return(test_interrupt());
}

void setup(void)
{
    /* clear TIM12 register                                                   */
    CSL_FINST(tmr0Regs->TIM12,TMR_TIM12_TIM12,RESETVAL);
    
    /* select 32 bit unchained mode and take the timer out of reset           */
    tmr0Regs->TGCR = CSL_FMKT(TMR_TGCR_TIMMODE,32BIT_UNCHAIN)
                   | CSL_FMKT(TMR_TGCR_TIM12RS,NO_RESET);
    
    /* set TIMER0 PRD1:2                                                      */
    CSL_FINS(tmr0Regs->PRD12,TMR_PRD12_PRD12,TIMER_PERIOD);
    
    /* map TIMER0 event to cpu int4                                           */
    CSL_FINS(intcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL4,TIMER0_EVENT);
    
    /* set ISTP to point to the vector table address                          */
    ISTP = (unsigned int)intcVectorTable;
    
    /* clear all interrupts, bits 4 thru 15                                   */
    ICR = 0xFFF0;
    
    /* enable the bits for non maskable interrupt and CPUINT4                 */
    IER |= 0x12;
    
    /* enable interrupts, set GIE bit                                         */
    _enable_interrupts();
}

int test_interrupt(void)
{
    printf("\nTesting CPU Interrupt 4 using TIMER0\n");
    
    /* enable TIMER0 1:2 side, once mode                                      */
    CSL_FINST(tmr0Regs->TCR,TMR_TCR_ENAMODE12,EN_ONCE);
    
    printf("\nStarting TIMER0\n");
    
    /* wait for TIMER0 to run to the period value and reset count             */
    while(CSL_FEXT(tmr0Regs->TIM12,TMR_TIM12_TIM12)!=0x0);
    
    if(intFlag!=0)
    {
      printf("Error\n");
      printf("\nInterrupt test:  FAILED\n");
    }
    else
      printf("\nInterrupt test:  PASSED\n");
    
    return(intFlag);
}

interrupt void interrupt4(void)
{
    /* disable TIMER0 1:2 side                                                */
    CSL_FINST(tmr0Regs->TCR,TMR_TCR_ENAMODE12,DISABLE);
    
    /* set the interrupt flag                                                 */
    intFlag=0;
}


void device_init(void)
{
    volatile Uint32 temp = 0;

    CSL_SyscfgRegsOvly sysRegs = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;


    /* Key to be written to enable the pin mux registers to be written        */
    sysRegs->KICK0R = 0x83e70b13;
    sysRegs->KICK1R = 0x95A4F1E0;
    
    /* mux between Timer 0/1 and GPIO[8:11], enable Timer0/1                  */
    temp  = (sysRegs->PINMUX4 & CSL_SYSCFG_PINMUX4_PINMUX4_3_0_MASK);
    temp |= ( CSL_SYSCFG_PINMUX4_PINMUX4_3_0_TM64P0_OUT12 <<
                CSL_SYSCFG_PINMUX4_PINMUX4_3_0_SHIFT );
    sysRegs->PINMUX4 |= temp;

}

