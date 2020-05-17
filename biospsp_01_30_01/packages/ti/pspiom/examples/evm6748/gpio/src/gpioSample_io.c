/*
 * gpioSample_io.c
 *
* This file contains the test / demo code to demonstrate the GPIO driver funct-
 * ionality on DSP/BIOS 5.
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
 *   @brief A simple example to demonstrate gpio driver usage
 *
 *   @file  gpioSample_io.c
 *
 *   <b> Example Description </b>
 *   @verbatim
 *   This example demonstrates the use of GPIO driver/module. The sample does
 *   this by taking input on GPIO pin GPIO0_4(configured as input pin), via an
 *   MMCSD card inserted in the MMCSD slot. This GPIO pin is configured to trigger 
 *   an interrupt. The ISR handler for this interrupt, gpioInputIsr is called,
 *   which sets the status variable to 1. Due to this the while loop in the task
 *   breaks.
 *   @endverbatim
 *
 *
 *   <b> Procedure to run the example </b>
 *   @verbatim
 *   1. Open gpioSample project in CCS
 *   2. Build the project
 *   3. Connect to target and load the gpioSample.x674
 *   4. Run the program
 *   5. On the console message "Waiting for GPIO Interrupt" is printed in a loop
 *      a. This message occurs until the user intervenes for generating interrupt
 *   6. Insert the MMCSD card in the MMCSD slot
 *   8. "GPIO Interrupt occured !" message is printed on the console
 *  10. This is the end of demonstration
 *   @endverbatim
 *
 * ============================================================================
 **/

#include <std.h>
#include <iom.h>
         
#include <log.h>
#include <tsk.h>
         
#include "ti/pspiom/cslr/cslr_gpio.h"
#include "ti/pspiom/cslr/cslr_syscfg0_C6748.h"
#include "ti/pspiom/cslr/soc_C6748.h"
#include "ti/pspiom/gpio/Gpio.h"
#include "ti/pspiom/platforms/evm6748/gpio_evmInit.h"

extern LOG_Obj trace;
volatile Int32 flag = 0;
extern Gpio_Handle     gpio0;

#define LOW 0x0
#define HIGH 0x1
#define GPIO_BANK_0 0x00
#define GPIO_BANK_4                 4u
#define GPIO4_0_PIN                 65u

Void gpioInputIsr(Ptr);


Void delay(Void)
{
    volatile Int32 i,j;
      for(i=0; i<5000; i++)
      {
          for(j=0; j<1000; j++)
          {
              ;
          }
      }
}

Void gpioExampleTask(Void)
{
    Gpio_PinCmdArg  pinCmdArg;
    Gpio_IntrCmdArg intrCmdArg;

    /* Configure GPIO(GPIO4_0_PIN) as an input                                */
    pinCmdArg.pin   = GPIO4_0_PIN;
    pinCmdArg.value = Gpio_Direction_Input;
    Gpio_setPinDir(gpio0, &pinCmdArg);

    /* Enable GPIO Bank interrupt for bank GPIO_BANK_4                        */
    Gpio_bankInterruptEnable(gpio0, GPIO_BANK_4);

    /* Configure GPIO(GPIO4_0_PIN) to generate interrupt on rising edge       */
    Gpio_setRisingEdgeTrigger(gpio0, GPIO4_0_PIN);

    /* Configure GPIO(GPIO4_0_PIN) to generate interrupt on falling edge      */
    Gpio_setFallingEdgeTrigger(gpio0, GPIO4_0_PIN);

    /* Set the interrupt handler for GPIO4_0_PIN. However we cannot register
     * interrupts for individual pins in C6748, therefore  register interrupt
     * for the associated bank(BANK4) as a whole
     */
    intrCmdArg.value = GPIO_BANK_4;
    intrCmdArg.bankOrPin = Gpio_BankOrPin_isBank;
    intrCmdArg.isrHandler = (Gpio_Isr)gpioInputIsr;
    Gpio_regIntHandler(gpio0, &intrCmdArg);

    /*At GPIO4_0_PIN bit value of 1 at the input reg represents that the card
    is removed and 0 means it is inserted*/

    LOG_printf(&trace, "Waiting for GPIO Interrupt\n");
    while( 0 == flag)
    {
        TSK_sleep(2000);
        LOG_printf(&trace, "Waiting for user to Insert MMCSD card\n");
    }

    LOG_printf(&trace, "GPIO Interrupt occured !\n");

    /* close the GPIO driver handle */
    Gpio_close(gpio0);

    LOG_printf(&trace, "End of GPIO sample application!\n");
}


Void gpioInputIsr(Ptr ignore)
{
    Gpio_PinCmdArg  pinCmdArg;

    /* To avoid compiler warnings                                             */
    ignore = ignore; 
    /* Clear the interrupt status                                             */
    pinCmdArg.pin   = GPIO4_0_PIN;
    Gpio_clearInterruptStatus(gpio0,&pinCmdArg);

    flag=1;           
}

