/*
 * Timer_example.c
 *
 * This file contains the test / demo code to demonstrate basic TIMER 
 * operations using the Regsiter CSL macros.
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
 *   @brief A simple example to demonstrate CSL TIMER register layer usage
 *   and interrupts in a CSLR application.
 *
 *   @file  Timer_example.c
 *
 *   <b> Example Description </b>
 *   @verbatim
 *   This example demonstrates using timer register layer csl to configure the
 *   timers using CSLR macros.  The example sets up TIMER0 and TIMER1 in 32
 *   bit unchained mode.  The TIMER0 1:2 and the 3:4
 *   side is used by the application.  For TIMER1 also, both sides are used by the
 *   application.  All the timer period values have been set to the default
 *   timer period value.  The example calls the test function
 *   which enables the interrupts and start all 4 timers running.  Once the
 *   timers have been started, the main function will wait for some  timer periods to
 *   ensure that all 4 timers used by the application have had time to run.
 *   When the task wakes,  it calls myExit() which  will check that all 4 interrupts
 *   where generated by the timers used by the application and indicate a pass/fail
 *   status in the
 *   message log
 *   @endverbatim
 *
 *
 *   <b> Procedure to run the example </b>
 *   @verbatim
 *   1. Configure the CCS setup to work with the emulator being used
 *   2. Please refer CCS manual for setup configuration and loading
 *      proper GEL file
 *   3. Launch CCS window
 *   4. Open project Timer_example.pjt
 *   5. Build the project and load the .out file of the project.
 *   6. Run the program
 *   @endverbatim
 *
 * ============================================================================
 **/

#include <stdio.h>
#include <c6x.h>
#include <ti/pspiom/cslr/csl_types.h>
#include <ti/pspiom/cslr/cslr_dspintc.h>
#include <ti/pspiom/cslr/soc_OMAPL137.h>
#include <ti/pspiom/cslr/cslr_tmr.h>
#include <ti/pspiom/cslr/cslr_syscfg_OMAPL137.h>



const unsigned int TIMER_PERIOD = 0x00001000;

static void device_init(void);
static void setup_TIMER0(void);
static void setup_TIMER1(void);
static void delay(Uint32 count);
static void test_TIMERS(void);

void myExit(int);
void test_TIMERS(void);

void TIMER0_TINT12_isr();
void TIMER0_TINT34_isr();
void TIMER1_TINT12_isr();
void TIMER1_TINT34_isr();

volatile int TIMER0_TINT12_status = 1;          // Pass/Fail flag
volatile int TIMER0_TINT34_status = 1;          // Pass/Fail flag
volatile int TIMER1_TINT12_status = 1;          // Pass/Fail flag
volatile int TIMER1_TINT34_status = 1;          // Pass/Fail flag
volatile int test_status = 1;                   // Test status

CSL_TmrRegsOvly tmr0Regs = (CSL_TmrRegsOvly)CSL_TMR_0_REGS;
CSL_TmrRegsOvly tmr1Regs = (CSL_TmrRegsOvly)CSL_TMR_1_REGS;

extern const unsigned int TIMER_PERIOD;

/* DSP interrupt events for the Timers  */
#define TIMER_EVENT0   04
#define TIMER_EVENT1   64
#define TIMER_EVENT2   40
#define TIMER_EVENT3   48


/*============================================================================*/
/*                        EXTERNAL FUNCTION PROTOTYPES                        */
/*============================================================================*/

extern void intcVectorTable(void);

/*============================================================================*/
/*                            FUNCTION DEFINTIONS                             */
/*============================================================================*/

void main (void)
{
    /* Enable TIMER0 and TIMER1 in mux and psc                                */
    device_init();

    /* Configure TIMER0, both halves to be used by application                */
    setup_TIMER0();

    /* Configure TIMER1 as dual 32 bit unchained timers for use by application*/
    setup_TIMER1();

    /* function to test the timer interrupts                                  */
    test_TIMERS();
}


void setup_TIMER0 (void)
{
    /* Clear TIM12 register                        */
    CSL_FINST(tmr0Regs->TIM12,TMR_TIM12_TIM12,RESETVAL);

    /* Clear TIM34 register                        */
    CSL_FINST(tmr0Regs->TIM34,TMR_TIM34_TIM34,RESETVAL);

    /* Select 32 bit unchained mode                */
    /* Take the timer out of reset and set the pre-scalar count for 3:4       */
    tmr0Regs->TGCR = CSL_FMKT(TMR_TGCR_TIMMODE,32BIT_UNCHAIN)
                     | CSL_FMKT(TMR_TGCR_TIM12RS,NO_RESET)
                     | CSL_FMKT(TMR_TGCR_TIM34RS,NO_RESET)
                     | CSL_FMK(TMR_TGCR_PSC34,0x1);

    /* Set timer0 PRD1:2                           */
    CSL_FINS(tmr0Regs->PRD12,TMR_PRD12_PRD12,TIMER_PERIOD);

    /* Set timer0 PRD3:4                           */
    CSL_FINS(tmr0Regs->PRD34,TMR_PRD34_PRD34,TIMER_PERIOD);
}


void setup_TIMER1 (void)
{
    /* Clear TIM12 register                        */
    CSL_FINST(tmr1Regs->TIM12,TMR_TIM12_TIM12,RESETVAL);

    /* Clear TIM34 register                        */
    CSL_FINST(tmr1Regs->TIM34,TMR_TIM34_TIM34,RESETVAL);

    /* Select 32 bit unchained mode                                           *
     * Take the timer out of reset and set the pre-scalar count for 3:4       */
    tmr1Regs->TGCR = CSL_FMKT(TMR_TGCR_TIMMODE,32BIT_UNCHAIN)
                     | CSL_FMKT(TMR_TGCR_TIM12RS,NO_RESET)
                     | CSL_FMKT(TMR_TGCR_TIM34RS,NO_RESET)
                     | CSL_FMK(TMR_TGCR_PSC34,0x1);

    /* Set timer1 PRD1:2                           */
    CSL_FINS(tmr1Regs->PRD12,TMR_PRD12_PRD12,TIMER_PERIOD);

    /* Set timer1 PRD3:4                           */
    CSL_FINS(tmr1Regs->PRD34,TMR_PRD34_PRD34,TIMER_PERIOD);
}

static void test_TIMERS(void)
{
    /* Interrupt Controller Register Overlay                                  */
    CSL_DspintcRegsOvly intcRegs = (CSL_DspintcRegsOvly)CSL_INTC_0_REGS;

    printf("Testing TIMER0 and TIMER1 in unchained 32 bit mode.\n");
    printf("Starting TIMER0 3:4 side and TIMER1 1:2 and 3:4 sides.\n");

    /* connect the event to the interrupt 4,5,6 and 7                         */
    CSL_FINS(intcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL4,TIMER_EVENT0);
    CSL_FINS(intcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL5,TIMER_EVENT1);
    CSL_FINS(intcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL6,TIMER_EVENT2);
    CSL_FINS(intcRegs->INTMUX1, DSPINTC_INTMUX1_INTSEL7,TIMER_EVENT3);

    /* set ISTP to point to the vector table address                          */
    ISTP = (unsigned int)intcVectorTable;

    /* clear all interrupts, bits 4 thru 15                                   */
    ICR = 0xFFF0;

    /* enable the bits for non maskable interrupt and                         */
    IER = 0xF2;

    /* enable interrupts, set GIE bit                                         */
    _enable_interrupts();

    delay(100);

    /* Enable TIMER0 1:2 side, one shot mode                                  */
    CSL_FINST(tmr0Regs->TCR,TMR_TCR_ENAMODE12,EN_ONCE);

    /* Enable TIMER0 3:4 side, one shot mode                                  */
    CSL_FINST(tmr0Regs->TCR,TMR_TCR_ENAMODE34,EN_ONCE);

    /* Enable TIMER1 1:2 side, one shot mode                                  */
    CSL_FINST(tmr1Regs->TCR,TMR_TCR_ENAMODE12,EN_ONCE);

    /* Enable TIMER1 3:4 side, one shot mode                                  */
    CSL_FINST(tmr1Regs->TCR,TMR_TCR_ENAMODE34,EN_ONCE);

    /* Sleep for 3 system clock PRD's to ensure other timers have ran.        *
     * Since 3:4 sides are running at a divide by 2 PRD, we need to           *
     * sleep at least 2 PRDs.                                                 */
    delay(1000);

    do 
    {
        /* Check if interrupts occurred       */
        test_status = TIMER0_TINT12_status
                        | TIMER0_TINT34_status
                        | TIMER1_TINT12_status
                        | TIMER1_TINT34_status;

        delay(1000);
    }while (test_status != 0);

    /* Call exit function                     */
    myExit(test_status);
}


interrupt void TIMER0_TINT12_isr()
{
    /* Disable TIMER0 1:2 side                */
    CSL_FINST(tmr0Regs->TCR,TMR_TCR_ENAMODE12,DISABLE);

    /* Set flag to 0 indicating ISR occurred  */
    TIMER0_TINT12_status=0;
}

interrupt void TIMER0_TINT34_isr()
{
    /* Disable TIMER0 3:4 side                */
    CSL_FINST(tmr0Regs->TCR,TMR_TCR_ENAMODE34,DISABLE);

    /* Set flag to 0 indicating ISR occurred  */
    TIMER0_TINT34_status=0;
}


interrupt void TIMER1_TINT12_isr()
{
    /* Disable TIMER1 1:2 side                */
    CSL_FINST(tmr1Regs->TCR,TMR_TCR_ENAMODE12,DISABLE);

    /* Set flag to 0 indicating ISR occurred  */
    TIMER1_TINT12_status=0;
}

interrupt void TIMER1_TINT34_isr()
{
    /*( Disable TIMER1 3:4 side   */
    CSL_FINST(tmr1Regs->TCR,TMR_TCR_ENAMODE34,DISABLE);

    /* Set flag to 0 indicating ISR occurred  */
    TIMER1_TINT34_status=0;
}

void myExit(int test_result)
{
    /* Disable TIMER0 1:2 side  */
    CSL_FINST(tmr0Regs->TCR,TMR_TCR_ENAMODE12,DISABLE);

    /* Disable TIMER0 3:4 side  */
    CSL_FINST(tmr0Regs->TCR,TMR_TCR_ENAMODE34,DISABLE);

    /* Disable TIMER1 1:2 side  */
    CSL_FINST(tmr1Regs->TCR,TMR_TCR_ENAMODE12,DISABLE);

    /* Disable TIMER1 3:4 side  */
    CSL_FINST(tmr1Regs->TCR,TMR_TCR_ENAMODE34,DISABLE);

    /* Turn off all interrupts  */
    _disable_interrupts();

    /* Display test status      */
    if (test_result == 0)
    {
        printf("Timer0 and Timer1 interrupt test: PASSED.\n");
    }
    else
    {
        printf("Timer0 and Timer1 interrupt test: FAILED.\n");
    }
}



void device_init(void)
{
    CSL_SyscfgRegsOvly sysRegs = (CSL_SyscfgRegsOvly)CSL_SYSCFG_0_REGS;
    
    /* Key to be written to enable the pin mux registers to be written        */
    sysRegs->KICK0R = 0x83e70b13;
    sysRegs->KICK1R = 0x95A4F1E0;
    
    /* mux between Timer 0/1 and GPIO[8:11], enable Timer0/1                  */
    sysRegs->PINMUX8 = CSL_FMKT(SYSCFG_PINMUX8_PINMUX8_19_16,TM64P0_OUT12);
    
}


/*
 * \brief    Function to introduce a delay in to the program.
 *
 * \param    count [IN]  delay count to wait
 * \return   None
 *
 */
static void delay(Uint32 count)
{
    volatile Uint32 tempCount = 0;

    for (tempCount = 0; tempCount < count; tempCount++)
    {
        /* dummy loop to wait for some time  */
    }
}
