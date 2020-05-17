/*
 * GpioLocal.h
 *
 * GPIO Driver internal header file
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

/*
 *  ======== ti/pspiom/gpio/src/GpioLocal.h ========
 *
 *! Revision History
 *! ================
 *! 13-Nov-2008 connell created.
 */

/** \file   GpioLocal.h
    \brief  Non-public header for internal GPIO specific operations.

    (C) Copyright 2008, Texas Instruments, Inc

    \author     Steve Connell
    \version    0.1 - Created                       Steve Connell
 */

#ifndef _GpioLocal_H_
#define _GpioLocal_H_

#include <std.h>
#include <ti/pspiom/cslr/cslr_gpio.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(CHIP_C6747) && !defined(CHIP_OMAPL137) && !defined(CHIP_C6748) && !defined(CHIP_OMAPL138)
#error "No chip type defined! (Must use -DCHIP_C6747 or -DCHIP_OMAPL137 or -DCHIP_C6748 or -DCHIP_OMAPL138)"
#endif

/* -------- structs --------                                                  */
 
typedef struct Gpio_Pin {
    Gpio_InUse inUse;
    Uint8 intEnabled;
    Int8 evtNum;
    Int32 hwiNum;
    Gpio_Isr isrHandler;
}Gpio_Pin;

typedef struct Gpio_Bank {
    Uint8 bankNum;
    Gpio_InUse inUse;
    Uint8 intEnabled;
    Gpio_Pin PinInfo[Gpio_NUM_PINS_PER_BANK];
    Int8 evtNum;
    Int32 hwiNum;
    Gpio_Isr isrHandler;
}Gpio_Bank;

typedef struct Gpio_HwInfo {
    CSL_GpioRegsOvly baseAddress;
    Uint8 numBanks;
    Uint8 numPinsPerBank;
    Uint16 numPins;
    Int8 bankEventNumber[Gpio_NUM_BANKS];
    Int8 pinEventNumber[Gpio_NUM_PINS];
}Gpio_HwInfo;

/* -------- per-instance state --------                                       */
typedef struct Gpio_Object {
    Uint32 instNum;
    Gpio_Bank BankInfo[Gpio_NUM_BANKS];
    Gpio_HwInfo deviceInfo;
} Gpio_Object;

/* -------- module-wide state -------- 
 *
 * inUse in module state points to an array whose size is soc specific 
 * Pointer used to match IDriver codebase.
 */
typedef struct Gpio_Module_State {
    Bool *inUse;
} Gpio_Module_State;

static void Gpio_DefaultIntHandler(Ptr arg);

static Void gpioInit(Gpio_Handle instHandle,
                     Gpio_BankConfig* gpioBankConfInfo);
                     
#ifdef __cplusplus
}
#endif /* extern "C"                                                          */

#endif /* _GpioLocal_H_                                                        */

