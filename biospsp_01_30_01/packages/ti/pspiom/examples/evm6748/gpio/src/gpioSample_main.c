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
#include <sys.h>
#include <pwrm.h>
#include <pwrm6748.h>

#include "ti/pspiom/gpio/Gpio.h"

Gpio_Handle     gpio0;

#define GPIO_BANK_0 0x00
#define GPIO_BANK_4                 4u
#define GPIO4_0_PIN                 65u

Void main (Void)
{
    Gpio_Params     gpioParams = Gpio_PARAMS;

    /* update the gpio parameters to our needs */
    gpioParams.instNum = 0;
    gpioParams.BankParams[GPIO_BANK_4].inUse = Gpio_InUse_No;
    gpioParams.BankParams[GPIO_BANK_4].hwiNum  = 8u;
    gpioParams.BankParams[GPIO_BANK_4].PinConfInfo[0].inUse = Gpio_InUse_No;

    /* open the GPIO driver to get a handle to it */
    gpio0 = Gpio_open(&gpioParams);
    /* power on the GPIO device in the Power sleep controller                 */
    /* Power on using Bios PWRM API                                           */
    PWRM_setDependency(PWRM_RSRC_GPIO);
    
    return;
}
