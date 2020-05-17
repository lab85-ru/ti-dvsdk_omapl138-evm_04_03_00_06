/*
 * Gpio.h
 *
 * This file contains Application Interface for the GPIO Driver. GPIO Driver
 * provides Application interface to do operations on the GPIO pins and or/banks
 * of pins like settings pin directions, pin/banks values, pin/bank interrupt 
 * enabling/disabling, interrupt handler registration etc
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
 *  ======== ti/psp/gpio/Gpio.h ========
 *
 *! Revision History
 *! ================
 *! 10-Nov-2008 connell created from PSP Team's BIOS 6 Gpio module.
 */

/** \mainpage    PSP Gpio Driver on BIOS 5.x
 *
 *  \section    Introduction
 *
 *  \subsection    xxx    Purpose and Scope
 *  The purpose of this document is to explain the Gpio driver written for
 *  DSP/BIOS 5.x.
 *
 *  \subsection    aaa    Gpio Hardware
 *  This module offers methods to interact with the Gpio hardware to set and
 *  get settings on the Gpio pins or banks.
 *
 *  The driver supports all the functions, like setting the Gpio pin directions,
 *  mark a Gpio pin/bank available for use, enable and disable bank interrupts,
 *  register interrupt handlers for a pin/bank interrupt, etc.
 *
 *  The Gpio module does not support any data transfer protocol or does not
 *  represent the data transfer layer. It gives trivial APIs which applications
 *  can use to perform basic Gpio operations.
 *
 *  Sample usage in an application:  
 *  
 *      Gpio_Handle gpio0;
 *
 *      Gpio_Params gpioParams;
 *
 *      gpioParams.instNum = 0;
 *
 *      gpioParams.BankParams[2].inUse = Gpio_InUse_No;
 *
 *      gpioParams.BankParams[2].hwiNum  = 8;
 *
 *      gpioParams.BankParams[2].PinConfInfo[3].inUse = Gpio_InUse_No;
 *
 *      gpio0 = Gpio_open(&gpioParams);
 *
 *      // call Gpio APIs now
 *
 *      Gpio_setRisingEdgeTrigger(gpio0, GPIO0_8_PIN);
 *
 *      // ... call other Gpio APIs, etc. ... 
 *  
 *      Gpio_close(gpio0);
 *  
 *  \subsection    bbb    DSP/BIOS System
 *  DSP/BIOS is a real time kernel provided by TI to use with its
 *  DSP processors.
 *
 *  \subsection    References
 *    -# DSP/BIOS driver development <b>SPRU616.pdf</b>
 *
 *  \file   ti/pspiom/gpio/Gpio.h
 *
 *  \brief  Gpio interface definition
 *
 *  This file contains the interfaces, data types and symbolic definitions
 *  that are needed by the application to utilize the serivces of the Gpio
 *  device driver.
 */

/** \file   Gpio.h
    \brief  Public header file for Gpio module.

    (C) Copyright 2008, Texas Instruments, Inc

    \author     Steve Connell, Madhvapathi Sriram
    \version    0.1 - Created                  Steve Connell, Madhvapathi Sriram
 */

#ifndef Gpio_H_
#define Gpio_H_

#include <std.h>
#include <ti/pspiom/cslr/cslr_gpio.h>

#ifdef CHIP_C6747
#include <ti/pspiom/cslr/soc_C6747.h>
#endif

#ifdef CHIP_OMAPL137
#include <ti/pspiom/cslr/soc_OMAPL137.h>
#endif

#ifdef CHIP_OMAPL138
#include <ti/pspiom/cslr/soc_OMAPL138.h>
#endif

#ifdef CHIP_C6748
#include <ti/pspiom/cslr/soc_C6748.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* -------- constants -------- */
/*! Total number of Gpio banks on the chip                                    */
#define Gpio_NUM_BANKS         CSL_GPIO_NUM_BANKS

/*! Total number of Gpio pins on the chip                                     */
#define Gpio_NUM_PINS          CSL_GPIO_NUM_PINS

/*! Number of Gpio pins contained in each bank                                */
#define Gpio_NUM_PINS_PER_BANK 16

/*! Total number of Gpios supported                                           */
#define Gpio_NUM_INSTANCES      1

/* -------- typedefs --------                                                 */

typedef Void (*Gpio_Isr)(Ptr);

/*!
 * Pointer to a Gpio instance.
 */
typedef struct Gpio_Object *Gpio_Handle;

/* -------- enums -------- */

/*!
 * Enumeration if the value/operation specified is for a bank or pin
 */
typedef enum Gpio_BankOrPin {
    Gpio_BankOrPin_isBank = 0,
    Gpio_BankOrPin_isPin
}Gpio_BankOrPin;

/*!
 * Enumeration to check/mark if the pin direction is input or output
 */
typedef enum Gpio_Direction {
    Gpio_Direction_Output = 0,
    Gpio_Direction_Input
}Gpio_Direction;

/*!
 * Enumeration to check/mark if the bank is in use or not in use.
 */
typedef enum Gpio_InUse {
    Gpio_InUse_Yes = 0,
    Gpio_InUse_No
}Gpio_InUse;

/* -------- structs -------- */

/*!
 * Structure to pass the arguments for Bank settings
 */

typedef struct Gpio_BankCmdArg {
    Uint32 bankIndex;
    Uint32 value;
    Uint32 mask;
}Gpio_BankCmdArg;

/*!
 * Structure used for supplying the config for Gpio pins by the user
 * This configuration is passed by the user as part of bank configuration in the
 * Gpio_Params struct for Gpio_open().
 */
typedef struct Gpio_PinConfig {
    Gpio_InUse inUse;
    Int32 hwiNum;
}Gpio_PinConfig;

/*!
 * Structure used for supplying the config for Gpio banks by the user.
 * This information contains, the configuration for individual pins in
 * the bank, if it is in use or available for use, the hardware interrupt
 * number assigned for the event (if any) associated with this pin/bank
 * 
 * This structure should be part of the Gpio_Params struct passed to Gpio_open()
 */
typedef struct Gpio_BankConfig {
    Gpio_PinConfig PinConfInfo[Gpio_NUM_PINS_PER_BANK];
    Int32 hwiNum;
    Gpio_InUse inUse;
}Gpio_BankConfig;

/*!
 * Structure used to pass the Interrupt command arguments by the user
 */
typedef struct Gpio_IntrCmdArg {
    Uint32 value;
    Gpio_BankOrPin bankOrPin;
    Gpio_Isr isrHandler;
}Gpio_IntrCmdArg;

/*!
 * Structure to pass the arguments for Pin settings
 */
typedef struct Gpio_PinCmdArg {
    Uint32 pin;
    Uint32 value;
}Gpio_PinCmdArg;

/*!
 * Parameters to be passed to the Gpio_open() call. These
 * parameters should include all the details the user wishes to specify in the
 * Gpio_BankConfig struct. This is used to maintain a database within the GPIO
 * module and used further. For example, if the user requests to set a GPIO pin
 * direction to output or input, then the module checks if this GPIO pin is
 * already in use for someother purpose (or used as a functional pin ( as
 * indicated by the inUse)). 
 */
typedef struct Gpio_Params {
    Uint32 instNum;
    Gpio_BankConfig BankParams[Gpio_NUM_BANKS];
}Gpio_Params;

/**
 *  \brief Default Gpio_Params struct
 *
 *  Gpio_Params array elements will all be initialized as shown in the following
 *  psuedo code snippet:
 *  \code
 *  const struct Gpio_Params Gpio_PARAMS = {
 *
 *      0,                                 // instNum
 *
 *      {                                  // BankParams[0]
 *
 *          {               // PinConfInfo[0]
 *
 *              Gpio_InUse_Yes,   // inUse;
 *              -1,             // hwiNum;
 *
 *          },
 *
 *          {               // PinConfInfo[1]
 *
 *              Gpio_InUse_Yes,   // inUse;
 *              -1,             // hwiNum;
 *
 *          },
 *
 *          ...
 *
 *          {               // PinConfInfo[15]
 *
 *              Gpio_InUse_Yes,   // inUse;
 *              -1,             // hwiNum;
 *
 *          },
 *
 *          -1,             // hwiNum;
 *
 *          Gpio_InUse_Yes   // inUse;
 *
 *      },
 *
 *      ...
 *
 *      {                                  // BankParams[7]
 *
 *          {               // PinConfInfo[0]
 *
 *              Gpio_InUse_Yes,   // inUse;
 *              -1,             // hwiNum;
 *
 *          },
 *
 *          {               // PinConfInfo[1]
 *
 *              Gpio_InUse_Yes,   // inUse;
 *              -1,             // hwiNum;
 *
 *          },
 *
 *          ...
 *
 *          {               // PinConfInfo[15]
 *
 *              Gpio_InUse_Yes,   // inUse;
 *              -1,             // hwiNum;
 *
 *          },
 *
 *          -1,             // hwiNum;
 *
 *          Gpio_InUse_Yes   // inUse;
 *
 *      },
 *
 *  };
 * \endcode
 */
extern const struct Gpio_Params Gpio_PARAMS;

/* -------- module-wide functions -------- */

/*!
 * This function is used to open a Gpio_Handle for use as a Gpio instance.
 *  @param(params)         Gpio_Params handle containing Gpio settings.
 *  @b(returns)            handle to Gpio instance
 */
extern Gpio_Handle Gpio_open(Gpio_Params *params);

/*!
 * This function is used to close a Gpio_Handle that was opened previously.
 *  @param(instHandle)     pointer to the Gpio module instance handle
 *  @b(returns)            none.
 */
extern Void Gpio_close(Gpio_Handle instHandle);

/*!
 * This function is used to disable the interrupt from a bank.
 *  @param(instHandle)     pointer to the Gpio module instance
 *  @param(bankNum)        bank num from which interrupts are to be diabled
 *  @b(returns)            IOM error code.
 */
extern Int32 Gpio_bankInterruptDisable(Gpio_Handle instHandle, Uint8 bankNum);

/*!
 * This function is used to enable the interrupt from a bank.
 *  @param(instHandle)     pointer to the Gpio module instance
 *  @param(bankNum)        bank num from which interrupts are to be enabled
 *  @b(returns)            IOM error code.
 */
extern Int32 Gpio_bankInterruptEnable(Gpio_Handle instHandle, Uint8 bankNum);

/*!
 * This function is used to clear the falling edge trigger for the interrupt
 * generation from a pin. The argument should contain the pin number for
 * which the setting needs to be done.
 *  @param(instHandle)     pointer to the Gpio module instance
 *  @param(gpioPin)        pin number
 *  @b(returns)            IOM error code.
 */
extern Int32 Gpio_clearFallingEdgeTrigger(Gpio_Handle instHandle, Uint8 gpioPin);

/*!
 * This function is used to clear the rising edge trigger for the interrupt
 * generation from a pin. The argument should contain the pin number for
 * which the setting needs to be done.
 *  @param(instHandle)     pointer to the Gpio module instance
 *  @param(gpioPin)        pin number
 *  @b(returns)            IOM error code.
 */

extern Int32 Gpio_clearRisingEdgeTrigger(Gpio_Handle instHandle, Uint8 gpioPin);

/*!
 * This function is used to get the value at a group of gpio pins in a bank.
 * The bank command argument should contain the bank number to which the
 * pins belong. This function returns pin value at the pins of this bank (in
 * the value field of the bank command argument). The caller should mask out
 * the irrelevant pin values.
 *  @param(instHandle)     pointer to the Gpio module instance
 *  @param(cmdArg)         pointer to bank command argument
 *  @b(returns)            IOM error code.
 */
extern Int32 Gpio_getGroupVal(Gpio_Handle instHandle, Gpio_BankCmdArg* cmdArg);

/*!
 *  This function is used to get the Bank inUse status
 *  If the Bank is marked as in use, then it shall not be a GPIO and hence GPIO
 *  operations shall not be permitted.The bankIndex field should contain the
 *  bank number in cmdArg
 *  @param(instHandle)      [IN]  pointer to the Gpio module instance
 *  @param(cmdArg)          [IN]  pointer to bank command argument
 *  @b(returns)             IOM error code.
 */
Int32 Gpio_getBankUseStatus(Gpio_Handle instHandle, Gpio_BankCmdArg *cmdArg);

/*!
 *  This function is used to get the Pin inUse status
 *  If the pin is marked as in use, then it shall not be a GPIO and hence GPIO
 *  operations shall not be permitted.The pin field should contain the pin
 *  pin number in the cmdArg
 *  @param(instHandle)      [IN]  pointer to the Gpio module instance
 *  @param(cmdArg)          [IN]  pointer to pin command argument
 *  @b(returns)            IOM error code.
 */
Int32 Gpio_getPinUseStatus(Gpio_Handle instHandle, Gpio_PinCmdArg *cmdArg);

/*!
 * This function is used to get the value at a Gpio pin, if the pin is an
 * input pin. The pin command argument should contain the pin number and
 * the value that should is at the pin is returned in the value field of the
 * pin command argument.
 *  @param(instHandle)     pointer to the Gpio module instance
 *  @param(cmdArg)         pointer to pin command argument
 *  @b(returns)            IOM error code.
 */
extern Int32 Gpio_getPinVal(Gpio_Handle instHandle, Gpio_PinCmdArg* cmdArg);

/*!
 * This function is used to register an interrupt handler to a bank event
 * interrupt or a pin event interrupt. The interrupt command argument should
 * contain the pin or bank number and the interrupt handler function that is
 * to be registered for this pin ot bank event interrupt. The function makes
 * a check if the specified pin or bank has valid event number and proceeds
 * with registering the interrupt handler and also enabling the events.
 *  @param(instHandle)     pointer to the Gpio module instance
 *  @param(cmdArg)         pointer to interrupt command argument
 *  @b(returns)            IOM error code.
 */

extern Int32 Gpio_regIntHandler(Gpio_Handle instHandle, Gpio_IntrCmdArg* cmdArg);

/*!
 *  This function is used to set the Bank inUse status
 *  If the Bank is marked as in use, then it shall not be a GPIO and hence GPIO
 *  operations shall not be permitted.The bankIndex field should contain the
 *  bank number and value field should contain status to be set of type inUse,
 *  in cmdArg
 *  @param(instHandle)      [IN]  pointer to the Gpio module instance
 *  @param(cmdArg)          [IN]  pointer to bank command argument
 *  @b(returns)            IOM error code.
 */

Int32 Gpio_setBankUseStatus(Gpio_Handle instHandle, Gpio_BankCmdArg *cmdArg);

/*!
 * This function is used to set the falling edge trigger for the interrupt
 * generation from a pin. The argument should contain the pin number for
 * which the setting needs to be done.
 *  @param(instHandle)     pointer to the Gpio module instance
 *  @param(gpioPin)        pin number
 *  @b(returns)            IOM error code.
 */
extern Int32 Gpio_setFallingEdgeTrigger(Gpio_Handle instHandle, Uint8 gpioPin);

/*!
 * This function is used to set the value at a group of gpio pins in a bank.
 * The bank command argument should contain mask representing the pins and
 * the value field should contain the value that should be output at these
 * pins. The bank command argument should also specify the bank number to
 * which the group of pins belong.
 *  @param(instHandle)     pointer to the Gpio module instance
 *  @param(cmdArg)         pointer to bank command argument
 *  @b(returns)            IOM error code.
 */
extern Int32 Gpio_setGroupVal(Gpio_Handle instHandle, Gpio_BankCmdArg* cmdArg);

/*!
 * This function is used to set the pin direction of a Gpio pin as input pin
 * or output pin. The pin command argument should contain the pin number and
 * the pin direction to be set, in the value field
 *  @param(instHandle)     pointer to the Gpio module instance
 *  @param(cmdArg)         pointer to pin command argument
 *  @b(returns)            IOM error code.
 */
extern Int32 Gpio_setPinDir(Gpio_Handle instHandle, Gpio_PinCmdArg* cmdArg);

/*!
 *  This function is used to set the Pin inUse status
 *  If the pin is marked as in use, then it shall not be a GPIO and hence GPIO
 *  operations shall not be permitted. The pin field should contain the pin
 *  pin number and value field should contain status to be set of type InUse,
 *  in the cmdArg
 *  @param(instHandle)      [IN]  pointer to the Gpio module instance
 *  @param(cmdArg)          [IN]  pointer to pin command argument
 *  @b(returns)            IOM error code.
 */

Int32 Gpio_setPinUseStatus(Gpio_Handle instHandle, Gpio_PinCmdArg *cmdArg);

/*!
 * This function is used to set the value at a Gpio pin, if the pin is an
 * output pin. The pin command argument should contain the pin number and
 * the value that should be set to it, in the value field
 *  @param(instHandle)     pointer to the Gpio module instance
 *  @param(cmdArg)         pointer to pin command argument
 *  @b(returns)            IOM error code.
 */
extern Int32 Gpio_setPinVal(Gpio_Handle instHandle, Gpio_PinCmdArg* cmdArg);

/*!
 * This function is used to set the rising edge trigger for the interrupt
 * generation from a pin. The argument should contain the pin number for
 * which the setting needs to be done.
 *  @param(instHandle)     pointer to the Gpio module instance
 *  @param(gpioPin)        pin number
 *  @b(returns)            IOM error code.
 */
extern Int32 Gpio_setRisingEdgeTrigger(Gpio_Handle instHandle, Uint8 gpioPin);

/*!
 * This function is used to clear the interrupt status in a bank
 *  @param(instHandle)     pointer to the Gpio module instance
 *  @param(cmdArg)         pointer to pin command argument
 *  @b(returns)            IOM error code.
 */
extern Int32 Gpio_clearInterruptStatus(Gpio_Handle instHandle, Gpio_PinCmdArg* cmdArg);

/*!
 * This function is used to unregister the previously registered interrupt
 * handler, via {@link #regIntHandler}, for a pin/bank event. This function
 * also disables the event associated with the specified pin or bank event.
 *  @param(instHandle)     pointer to the Gpio module instance
 *  @param(cmdArg)         pointer to interrupt command argument
 *  @b(returns)            IOM error code.
 */
extern Int32 Gpio_unregIntHandler(Gpio_Handle instHandle, Gpio_IntrCmdArg* cmdArg);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* Gpio_H_ */

