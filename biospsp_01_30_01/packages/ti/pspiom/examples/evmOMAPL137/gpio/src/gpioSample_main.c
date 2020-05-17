/*
 * gpioSample_main.c
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
 *   @file  gpioSample_main.c
 *  @verbatim
 *  Here, example task, whose function is defined in gpioSample_io.c, is created
 *  @endverbatim
 *
 *   <b> Example Description </b>
 *   @verbatim
 *   This example demonstrates the use of GPIO drver/module. The sample does
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
 *   6. Toggle the user switch SW3-1
 *   8. "GPIO Interrupt occured !" message is printed on the console
 *       a. the movement of switch position causes the GPIO interrupt
 *   9. The User LED DS1 now starts blinking (turning on and off at very low freq)
 *  10. This is the end of demonstration
 *   @endverbatim
 *
 * ============================================================================
 **/

#include <std.h>
#include <sys.h>

#include "ti/pspiom/gpio/Gpio.h"
#include "ti/pspiom/cslr/cslr_psc_OMAPL137.h"
#include "ti/pspiom/cslr/soc_OMAPL137.h"
#include "ti/pspiom/psc/Psc.h"

Gpio_Handle gpio0;

extern Void GPIO_input_isr(Ptr ignore);

Void main (Void)
{
    /* start with the default params */
    Gpio_Params gpioParams = Gpio_PARAMS;

    /* enable the gpio instance in the PSC module  */
    Psc_ModuleClkCtrl(Psc_DevId_1, CSL_PSC_GPIO, TRUE);

    /* update the gpio parameters to our needs */
    gpioParams.instNum = 0;
    gpioParams.BankParams[0].inUse = Gpio_InUse_No;
    gpioParams.BankParams[0].hwiNum  = 9;

    /*
    It is to be noted here that the pin numbers in GPIO peripheral user guide
    starts from 1 and end at N. However the GPIO params uses arrays to maintain
    the pin and bank configuration info. Hence, respective position for this
    pin in the array will be (pinNumber-1).
    */
    gpioParams.BankParams[0].PinConfInfo[8].inUse = Gpio_InUse_No;
    gpioParams.BankParams[0].PinConfInfo[12].inUse = Gpio_InUse_No;

    /* open the GPIO driver to get a handle to it */
    gpio0 = Gpio_open(&gpioParams);

    /* make sure our driver instance is ok */
    if (NULL == gpio0)
    {
        SYS_abort("Gpio_open() failed to create Gpio driver instance!\n");
    }
}
