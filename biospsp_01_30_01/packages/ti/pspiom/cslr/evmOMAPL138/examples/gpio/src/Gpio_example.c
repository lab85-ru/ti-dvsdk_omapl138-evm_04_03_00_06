/*
 * Gpio_example.c
 *
 * This file contains the test / demo code to demonstrate basic GPIO operations 
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
 *   @brief A simple example to demonstrate CSL GPIO register layer usage
     and interrupts in a BIOS application.
 *
 *   @file  Gpio_example.c
 *
 *   <b> Example Description </b>
 *   @verbatim
 *   This example demonstrates the use of GPIO module. The sample does
 *   this by configuring the GPIO pin GPIO0_7(configured as output pin) as
 *   an interrupt pin. Then it sets the status of the pin to high to
 *   trigger an interrupt which is then serviced by an ISR function.
 *   @endverbatim
 *
 *
 *   <b> Procedure to run the example </b>
 *   @verbatim
 *   1. Open Gpio_example.pjt in CCS
 *   2. Build the project
 *   3. Connect to target and load the gpio_example.out
 *   4. Run the program
 *   5. The messages will be printed on to the console as the interrupts are
 *      generated and ISR function is called.
 *   6. on successful completion "GPIO sample application completed" is
 *      logged to the console.
 *   @endverbatim
 *
 * ============================================================================
 **/

/*============================================================================*/
/*                              INLCUDE FILES                                 */
/*============================================================================*/

#include <stdio.h>
#include <c6x.h>
#include <ti/pspiom/cslr/cslr_dspintc.h>
#include <ti/pspiom/cslr/soc_OMAPL138.h>
#include <ti/pspiom/cslr/cslr_gpio.h>
#include <ti/pspiom/cslr/cslr_syscfg0_OMAPL138.h>
#include <ti/pspiom/cslr/cslr_psc_OMAPL138.h>

/*============================================================================*/
/*                        EXTERNAL FUNCTION PROTOTYPES                        */
/*============================================================================*/

extern void intcVectorTable(void);

/*============================================================================*/
/*                         LOCAL FUNCTION PROTOTYPES                          */
/*============================================================================*/

static void gpioPowerOn(void);
static void GpioStartTest(void);
static void delay(Uint32 count);

volatile Bool intStatus = 0;
static Uint8 count    = 0;


/*============================================================================*/
/*                             GLOBAL VARIABLES                               */
/*============================================================================*/

/* sys config registers overlay                                               */
CSL_SyscfgRegsOvly   sysRegs  = (CSL_SyscfgRegsOvly)(CSL_SYSCFG_0_REGS);
/* Psc register overlay                                                       */
CSL_PscRegsOvly      psc1Regs = (CSL_PscRegsOvly)(CSL_PSC_1_REGS);
/* Gpio register overlay                                                      */
CSL_GpioRegsOvly     gpioRegs = (CSL_GpioRegsOvly)(CSL_GPIO_0_REGS);
/* Interrupt Controller Register Overlay                                      */
CSL_DspintcRegsOvly intcRegs = (CSL_DspintcRegsOvly)CSL_INTC_0_REGS;
/*============================================================================*/
/*                             MACRO DEFINITIONS                              */
/*============================================================================*/


#define INT_GENERATED_FALSE 0x00
#define INT_GENERATED_TRUE  0x01

/*============================================================================*/
/*                          FUNCTION DEFINITIONS                              */
/*============================================================================*/

void main (void)
{
    /* This function will configure an GPIO pin as an interrupt pin           */

    /* Key to be written to enable the pin mux registers for write            */
    sysRegs->KICK0R = 0x83e70b13;
    sysRegs->KICK1R = 0x95A4F1E0;

    /* enable the pinmux for the GPIO bank 0 pin 7                            */
    sysRegs->PINMUX1 = ((CSL_SYSCFG_PINMUX1_PINMUX1_3_0_GPIO0_7)
                        << (CSL_SYSCFG_PINMUX1_PINMUX1_3_0_SHIFT));

    /* lock the pinmux registers                                              */
    sysRegs->KICK0R = 0x00000000;
    sysRegs->KICK1R = 0x00000000;

    /* first enable the GPIO in the PSC                                       */
    gpioPowerOn();

    /* Configure GPIO0_7 (GPIO0_7_PIN) as an output                           */
    CSL_FINS(gpioRegs->BANK[0].DIR,GPIO_DIR_DIR7,0);

    /* set the GIPO0_7 value to 0                                             */
    CSL_FINS(gpioRegs->BANK[0].OUT_DATA,GPIO_OUT_DATA_OUT7,0);

    /* Enable GPIO Bank interrupt for bank 0                                  */
    CSL_FINST(gpioRegs->BINTEN,GPIO_BINTEN_EN0,ENABLE);

    /* Configure GPIO(GPIO0_7_PIN) to generate interrupt on rising edge       */
    CSL_FINS(gpioRegs->BANK[0].SET_RIS_TRIG,
             GPIO_SET_RIS_TRIG_SETRIS7,
             CSL_GPIO_SET_RIS_TRIG_SETRIS_ENABLE);

    /* map GPIO0 event to cpu int4                                            */
    CSL_FINS(intcRegs->INTMUX1,
             DSPINTC_INTMUX1_INTSEL4,
             CSL_INTC_EVENTID_GPIO_BNK0_INT);

    /* set ISTP to point to the vector table address                          */
    ISTP = (unsigned int)intcVectorTable;

    /* clear all interrupts, bits 4 thru 15                                   */
    ICR = 0xFFF0;

    /* enable the bits for non maskable interrupt and CPUINT4                 */
    IER = 0x12;

    /* enable interrupts, set GIE bit                                         */
    _enable_interrupts();

    /* set interrupt generated status to false                                */
    intStatus = INT_GENERATED_FALSE;

    GpioStartTest();
}

/*
 * \brief    Function to test the GPIO functionality.
 *
 *           Will generate a GPIO interrupt by writing to the GPIO outdata
 *           register
 *
 * \param    None
 * \return   None
 *
 */

static void GpioStartTest(void)
{
    printf("Starting the GPIO testing\n");
    /* This function will set a GPIO pin to 1 (which is configured earlier) so*
     * an interrupt handler registered previously for that event is invoked   */
    while (count <= 5)
    {
        /* set the Bank 0 pin 7 to 1 to generate an interrupt                 */
        CSL_FINS(gpioRegs->BANK[0].OUT_DATA,GPIO_OUT_DATA_OUT7,1);

        while (INT_GENERATED_TRUE != intStatus)
        {
            delay(1000);
        }

        printf("Interrupt generated by Gpio module, Int count %d \n",count);
        intStatus = INT_GENERATED_FALSE;
        count++;
    }
    printf("GPIO sample application completed\n");
}

/*
 * \brief    interrupt Handler routine for the GPIO interrupt
 *
 * \param    None
 * \return   None
 *
 */
interrupt void gpioInputIsr(void)
{
    /* The interrupt handler for the GPIO interrupts                          */

    /* the interrupt could have been because of any one of the pins in the    *
     * bank 0. Hence we will only check if the pin 7 is generating the        *
     * interrupt and then reset it and exit.                                  */
    if (gpioRegs->BANK[0].INTSTAT & CSL_GPIO_INTSTAT_STAT7_MASK)
    {
        /* reset the interrupt source (so that multiple interrupts dont ccur  */
        CSL_FINS(gpioRegs->BANK[0].OUT_DATA,GPIO_OUT_DATA_OUT7,0);

        /* reset the interrupt status register                                */
        CSL_FINS(gpioRegs->BANK[0].INTSTAT,GPIO_INTSTAT_STAT7,0);

        /* cannot print here hence set the status variable so that that task  *
         * can print the message                                              */
        intStatus = INT_GENERATED_TRUE;
    }
}


/*
 * \brief    Function to power on the GPIO module in the power sleep controller.
 *
 * \param    None
 * \return   None
 *
 *  Note: This function causes the program to abort in case it is unable to   *
 *        enable the GPIO module.
 */
static void gpioPowerOn(void)
{
    volatile Uint32 pscTimeoutCount = 10240u;
    Uint32          temp            = 0;

    /* we will now power on the GPIO module in the PSC.                       *
     * Configure the GPIO Module to Enable state                              */
    psc1Regs->MDCTL[CSL_PSC_GPIO] = ((psc1Regs->MDCTL[CSL_PSC_GPIO]
                                        & 0xFFFFFFE0)
                                     | CSL_PSC_MDSTAT_STATE_ENABLE);

    /* Kick start the Enable Command                                          */
    temp = psc1Regs->PTCMD;
    temp = ((temp & CSL_PSC_PTCMD_GO0_MASK)
            | (CSL_PSC_PTCMD_GO0_SET << CSL_PSC_PTCMD_GO0_SHIFT));

    psc1Regs->PTCMD |= temp;

    /* Wait for the power state transition to occur                           */
    while (((psc1Regs->PTSTAT & (CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION)) != 0)
        && (pscTimeoutCount>0))
    {
        pscTimeoutCount--;
    }

    /* Check if PSC state transition timed out                                */
    if (0 == pscTimeoutCount)
    {
        printf("GPIO PSC transition to ON state timed out\n");
    }
    else
    {
        printf("Gpio enabled in PSC\n");
    }
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
/*============================================================================*/
/*                               END OF FILE                                  */
/*============================================================================*/
