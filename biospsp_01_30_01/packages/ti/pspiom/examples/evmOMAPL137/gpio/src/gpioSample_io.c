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
 *   @brief A simple example to demonstrate CSL 3.x GPIO register layer usage
     and interrupts in a BIOS application.
 *
 *   @file  gpioSample_io.c
 *
 *   <b> Example Description </b>
 *   @verbatim
 *   This example demonstrates the use of GPIO driver/module. The sample does
 *   this by taking input on GPIO pin GPIO0_8(configured as input pin), via user
 *   switch SW3-1. This GPIO pin is configured to trigger an interrupt at the
 *   rising edge. The ISR handler for this interrupt, GPIO_input_isr is called,
 *   which sets the status variable to 1. Due to this the while loop in the task
 *   breaks and blinks led DS1 which is connected to the GPIO0_12
 *   (configured as output pin).
 *   @endverbatim
 *
 *
 *   <b> Procedure to run the example </b>
 *   @verbatim
 *   1. Open gpioSample project in CCS
 *   2. Build the project
 *   3. Connect to target and load the gpioSample.x674
 *   4. Run the program
 *   5. On the console message "Waiting for user to configure SW3-1" is printed
 *      in a loop
 *      a. This message occurs until the user intervenes for generating interrupt
 *   6. Toggle the user swith SW3-1
 *   8. "GPIO Interrupt occured !" message is printed on the console
 *       a. the movement of switch position causes the GPIO interrupt
 *   9. The User LED DS1 now starts blinking (turning on and off at very low freq)
 *  10. This is the end of demonstration
 *   @endverbatim
 *
 * ============================================================================
 **/

#include "std.h"
#include "iom.h"
         
#include "log.h"
#include "tsk.h"
         
#include "ti/pspiom/cslr/cslr_gpio.h"
#include "ti/pspiom/cslr/cslr_syscfg_OMAPL137.h"
#include "ti/pspiom/cslr/soc_OMAPL137.h"
#include "ti/pspiom/gpio/Gpio.h"
#include "ti/pspiom/platforms/evmOMAPL137/gpio_evmInit.h"

extern Gpio_Handle gpio0;
extern LOG_Obj trace;
volatile Int32 flag = 0;

#define LOW 0x0
#define HIGH 0x1
#define GPIO_BANK_0 0x00

#define GPIO0_8_PIN 9
#define GPIO0_12_PIN 13

Void GPIO_input_isr(Ptr);


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
    Gpio_PinCmdArg UserPinCmdArg;
    Gpio_IntrCmdArg UserIntrCmdArg;
    volatile Int32 ledBlinkCount=4;

    /* Call the EVM specific initialization function                          */
    configureGpio();

    /* Configure GPIO0_12 as an output */
    UserPinCmdArg.pin = GPIO0_12_PIN;
    UserPinCmdArg.value = Gpio_Direction_Output;
    Gpio_setPinDir(gpio0,&UserPinCmdArg);

    /* Configure GPIO(GPIO0_8_PIN) as an input */
    UserPinCmdArg.pin = GPIO0_8_PIN;
    UserPinCmdArg.value = Gpio_Direction_Input;
    Gpio_setPinDir(gpio0,&UserPinCmdArg);


    /* Set Data high in SET_DATA register for GPIO(GPIO0_12_PIN).            *
     * This turns the LED off -see schematic                                 */
    UserPinCmdArg.pin = GPIO0_12_PIN;
    UserPinCmdArg.value = HIGH;
    Gpio_setPinVal(gpio0,&UserPinCmdArg);

    /* Enable GPIO Bank interrupt for bank GPIO_BANK_0                        */
    Gpio_bankInterruptEnable(gpio0,GPIO_BANK_0);

    /* Configure GPIO(GPIO0_8_PIN) to generate interrupt on rising edge       */
    Gpio_setRisingEdgeTrigger(gpio0,GPIO0_8_PIN);

    /* Set the interrupt handler for GPIO0_8_PIN. However we cannot register
     * interrupts for individual pins in OMAPL137, therefore  register interrupt
     * for the associated bank(BANK0) as a whole
     */
    UserIntrCmdArg.value = GPIO_BANK_0;
    UserIntrCmdArg.bankOrPin = Gpio_BankOrPin_isBank;
    UserIntrCmdArg.isrHandler = (Gpio_Isr)&GPIO_input_isr;
    Gpio_regIntHandler(gpio0,&UserIntrCmdArg);

    LOG_printf(&trace, "Waiting for GPIO Interrupt\n");
    while( 0 == flag)
    {
        TSK_sleep(2000);
        LOG_printf(&trace, "Waiting for user to toggle SW3-1\n");
    }

    LOG_printf(&trace, "GPIO Interrupt occured !\n");

    while(ledBlinkCount > 0)
    {
        /* Make the GPIO pin conected to the LED to low. *
         * This turns on the LED - see schematic         */
        UserPinCmdArg.pin = GPIO0_12_PIN;
        UserPinCmdArg.value = LOW;
        Gpio_setPinVal(gpio0,&UserPinCmdArg);

        delay();

        /* Make the GPIO pin conected to the LED to high *
         * This turns the off the LED - see schematic    */
        UserPinCmdArg.pin = GPIO0_12_PIN;
        UserPinCmdArg.value = HIGH;
        Gpio_setPinVal(gpio0,&UserPinCmdArg);

        delay();

        ledBlinkCount--;

    }

    /* close the GPIO driver handle */
    Gpio_close(gpio0);

    LOG_printf(&trace, "End of GPIO sample application!\n");
}


Void GPIO_input_isr(Ptr ignore)
{
    Gpio_PinCmdArg UserPinCmdArg;
    
    /* To avoid compiler warnings                                             */
    ignore = ignore; 
       
    /* 
    There is significant signal bounce at the GPIO pin connected to the User 
    Switches (SW3). These bounces can generate spurious interrupts (at either
    falling or rising). Hence check the steady state (after some delay).
    However, in real applications this delay-check may be done outside ISR
    */
    delay();
    
    UserPinCmdArg.pin = GPIO0_8_PIN;
    Gpio_getPinVal(gpio0,&UserPinCmdArg);

    /* Let this be here now. I want to see the Heart beat :-)                 */
    /* Want interrupts only at rising edge - Check pin in steady state high   */
    if(1u == UserPinCmdArg.value)
    {
        flag=1;
    }
}

