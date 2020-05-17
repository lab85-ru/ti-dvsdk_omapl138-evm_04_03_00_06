/*
 * Gpio.c
 *
 * This file contains Application Interface implementation for the GPIO Driver. 
 * GPIO Driver provides Application interface to do operations on the GPIO pins 
 * and or/banks of pins like settings pin directions, pin/banks values, pin/bank 
 * interrupt enabling/disabling, interrupt handler registration etc
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

/** \file   Gpio.c
    \brief  Gpio module for GPIO specific operations.

    (C) Copyright 2008, Texas Instruments, Inc

    \author     Steve Connell, Madhvapathi Sriram
    \version    0.1 - Created                  Steve Connell, Madhvapathi Sriram
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include "std.h"
#include "string.h"
#include "assert.h"
#include "log.h"
#include "iom.h"
#include "ecm.h"
#include "hwi.h"
#include "c64.h"


#include "ti/pspiom/gpio/Gpio.h"
#include "ti/pspiom/gpio/src/GpioLocal.h"

#ifdef CHIP_C6747
    #include "ti/pspiom/cslr/soc_C6747.h"
#endif

#ifdef CHIP_OMAPL137
    #include "ti/pspiom/cslr/soc_OMAPL137.h"
#endif

#ifdef CHIP_C6748
    #include "ti/pspiom/cslr/soc_C6748.h"
#endif

#ifdef CHIP_OMAPL138
    #include "ti/pspiom/cslr/soc_OMAPL138.h"
#endif
/*
 *  Create macros for making default Gpio_PARAMS struct
 *
 *  (an example format of the PARAMS struct is shown in Gpio.h.  This could
 *   be used for reference when trying to understand these macros)
 */
#define Gpio_HWINUM(x) (x)
#define Gpio_INUSE(x) (x)

/* macro for Gpio_pinConf array elements in the Gpio_Params struct (this
 * represents one of the pins that's found in a bank)
 */
#define Gpio_PINCONF_ELEM \
    {\
        Gpio_INUSE(Gpio_InUse_Yes),\
        Gpio_HWINUM(-1)\
    }

/* macro for Gpio_BankConfig array element (which represents a bank) in the
 * Gpio_Params struct
 */
#define Gpio_BANKCONF_ELEM \
    {\
        {\
            /* there are 16 pins per bank */\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM,\
            Gpio_PINCONF_ELEM\
        },\
        Gpio_HWINUM(-1),\
        Gpio_INUSE(Gpio_InUse_Yes)\
    }

/* -------- constants -------- */

/* use above macros to ease the creation of the large, multi dimensional
 * default PARAMS object:
 */
const struct Gpio_Params Gpio_PARAMS = {
    0,                      /* instNum                                        */
    {
        /* there are currently 8 banks available, each w/ 16 pins */
        Gpio_BANKCONF_ELEM,     /* BankParams[0]                              */
        Gpio_BANKCONF_ELEM,
        Gpio_BANKCONF_ELEM,
        Gpio_BANKCONF_ELEM,
        Gpio_BANKCONF_ELEM,     /* ... */
        Gpio_BANKCONF_ELEM,
        Gpio_BANKCONF_ELEM,
        Gpio_BANKCONF_ELEM      /* BankParams[7]                              */
    }
};

/* ========================================================================== */
/*                       GLOBAL MODULE STATE                                  */
/* ========================================================================== */

static Bool inUse[CSL_GPIO_PER_CNT];

static Gpio_Module_State Gpio_module = {&inUse[0]};

static Gpio_Object Gpio_Instances[CSL_GPIO_PER_CNT];

static Gpio_HwInfo Gpio_deviceInstInfo[CSL_GPIO_PER_CNT];

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */

/*
 * Default Int Handler
 */
static void Gpio_DefaultIntHandler(Ptr arg)
{
    if(arg == arg)
    {
        arg =arg;
    }

    return;
}

/**
 *  \brief GPIO Initialization
 *  This is the first call the module make locally, before any other GPIO
 *  function usage.This function is to initialize the GPIO data structures.
 *  Here is where the list of banks/pins details is populated and later used.
 *
 *  \param  instHandle       [IN] Gpio_Handle
 *  \param  gpioBankConfInfo [IN] User Supplied Gpio Configuration
 *
 *  \enter  instanHandle        should be valid and not null
 *          gpioBankConfInfo    should be valid and not null
 *  \leave  not implemented
 *
 *  \return None.
 */

static Void gpioInit(Gpio_Handle instHandle,
                     Gpio_BankConfig* gpioBankConfInfo)
{
    Int             i,j;
    Gpio_BankConfig *bankConfInfo;
    Gpio_PinConfig  *pinConfInfo;
    Gpio_Bank       *bankInfo;
    Gpio_Pin        *pinInfo;
    Gpio_HwInfo     *hwInfo;

    assert(NULL != gpioBankConfInfo);
    assert(NULL != instHandle);

    /* Get the hwInfo for this bank                                   */
    hwInfo = &Gpio_deviceInstInfo[instHandle->instNum];

    /* Cycle through the banks                                            */
    for (i = 0; i < Gpio_NUM_BANKS; i++)
    {
        /* Get the Bank Config info for this bank                         */
        bankConfInfo = (gpioBankConfInfo + i);
        assert(NULL != bankConfInfo);
        
        /* Get the Bank for this bank                                     */
        bankInfo = &instHandle->BankInfo[i];

        /* Initialize the device Info for this instance                   */
        instHandle->deviceInfo.baseAddress = hwInfo->baseAddress;
        instHandle->deviceInfo.numBanks = hwInfo->numBanks;
        instHandle->deviceInfo.numPinsPerBank = hwInfo->numPinsPerBank;
        instHandle->deviceInfo.numPins = hwInfo->numPins;

        /* Update the bank numbers                                        */
        bankInfo->bankNum = (Uint8)i;

        /* Mark a bank is in use/not in use based on user configuration   */
        bankInfo->inUse = bankConfInfo->inUse;

        /* Mark by default that all bank are interrupt disabled           */
        bankInfo->intEnabled = 0;

        /* Update the bank DSP event nums from the predefined database    */
        bankInfo->evtNum  = hwInfo->bankEventNumber[i];

        /* If the bank event numbers are valid then they can be routed to an
         * HWI.
         */
        if(0  <= bankInfo->evtNum )
        {
            bankInfo->hwiNum = bankConfInfo->hwiNum;
        }
        else
        {
            bankInfo->hwiNum = -1;
        }

        /*Register a default handler for each bank interrupt              */
        bankInfo->isrHandler = (&Gpio_DefaultIntHandler);

        /* Cycle through each pin in this bank                            */
        for (j = 0; j < Gpio_NUM_PINS_PER_BANK; j++)
        {
            /* Get the Pin conf info for this bank                        */
            pinConfInfo = &bankConfInfo->PinConfInfo[j];

            /* Get the Pin info for this bank                             */
            pinInfo = &(bankInfo->PinInfo[j]);

            /* Mark a pin is in use/not in use based on user configuration*/
            pinInfo->inUse = pinConfInfo->inUse;

            /* Mark by default as the pin interrupt is disabled           */
            pinInfo->intEnabled = 0;

            /* Update the pin event nums from pre-defined database        */
            pinInfo->evtNum = hwInfo->pinEventNumber[j+(16*i)];

            /* If the pin event numbers are valid then they can be routed to
             * an HWI
             */
            if( 0 <= pinInfo->evtNum)
            {
                pinInfo->hwiNum = pinConfInfo->hwiNum;
            }
            else
            {
                pinInfo->hwiNum = -1;
            }

            /*Register a default handler for each pin interrupt           */
            pinInfo->isrHandler = (&Gpio_DefaultIntHandler);
        }
    }

    instHandle->deviceInfo.baseAddress = hwInfo->baseAddress;

    return;
}

/**
 *  \brief GPIO Set the Pin Use status
 *  This function is used to set the Pin inUse status
 *  If the pin is marked as in use, then it shall not be a GPIO and hence GPIO
 *  operations shall not be permitted.
 *
 *  \param   instHandle      [IN]  Gpio_Handle
 *  \param   cmdArg          [IN]  User Setting for this pin
 *
 *  \enter  instHandle          should be valid and not null
 *          cmdArg              should be valid and not null. Should represent
 *                              valid pin numbmer (<numPins). status to be set
 *                              should be Gpio_InUse type only
 *  \leave  not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_setPinUseStatus(Gpio_Object* instHandle,
                          Gpio_PinCmdArg *cmdArg)
{
    Uint32              gpioPin;
    Uint32              index;
    Gpio_Bank          *bankInfo ;
    Int32               status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if((NULL == instHandle) || (NULL == cmdArg))
    {
        status = IOM_EBADARGS;
    }
    /*The pin number given should not exceed nummber of pins for this inst*/
    else if(instHandle->deviceInfo.numPins <= cmdArg->pin)
    {
        status = IOM_EBADARGS;
    }
    /* The pin status to be set should either be inUse_Yes or inUse_No */
    else if(((Gpio_InUse_Yes != cmdArg->value) &&
             (Gpio_InUse_No != cmdArg->value)))
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = IOM_COMPLETED;
    }
#endif

    if(IOM_COMPLETED == status)
    {
        /* The gpioPin number in the driver starts from zero. While in User's
           guide it starts from 1. Hence this operation                       */
        gpioPin = (cmdArg->pin - 1u);

        /* The index of the bank info structure to which the pin belongs is
         * the pin number divided by 16, since, each bank structure has
         * info for 16 pins
         */
        index = (gpioPin >> 4u);
        /* The index for the pin info structure inside the bank structure
         * after we obtain the bank index, is stored in the 4 lsbs.
         */
        gpioPin = (gpioPin & 0xfu);
        bankInfo = &instHandle->BankInfo[index];

        bankInfo->PinInfo[gpioPin].inUse = (Gpio_InUse)cmdArg->value;
    }
    return (status);
}



/**
 *  \brief GPIO get the Pin Use status
 *  This function is used to get the Pin inUse status
 *  This function shall return the if the given pin is in Use r is available as
 *  a GPIO. If it is available as a GPIO (inUse_No) then GPIO operation are
 *  permitted
 *
 *  \param   instHandle      [IN]  Gpio_Handle
 *  \param   cmdArg          [IN]  User Setting for this pin
 *  \enter  instHandle          should be valid and not null
 *          cmdArg              should be valid and not null. Should represent
 *                              valid pin numbmer (<numPins).
 *  \leave  not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_getPinUseStatus(Gpio_Object* instHandle,
                          Gpio_PinCmdArg *cmdArg)
{
    Uint32              gpioPin;
    Uint32              index;
    Gpio_Bank          *bankInfo ;
    Int32               status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if((NULL == instHandle) || (NULL == cmdArg))
    {
        status = IOM_EBADARGS;
    }
    /*The pin number given should not exceed nummber of pins for this inst*/
    else if(instHandle->deviceInfo.numPins < cmdArg->pin)
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(IOM_COMPLETED == status)
    {
        /* The gpioPin number in the driver starts from zero. While in User's
           guide it starts from 1. Hence this operation                       */
        gpioPin = (cmdArg->pin - 1u);
        /* The index of the bank info structure to which the pin belongs is
         * the pin number divided by 16, since, each bank structure has
         * info for 16 pins
         */
        index = (gpioPin >> 4u);
        /* The index for the pin info structure inside the bank structure
         * after we obtain the bank index, is stored in the 4 lsbs.
         */
        gpioPin = (gpioPin & 0xfu);
        bankInfo = &instHandle->BankInfo[index];

        cmdArg->value = (Uint32)bankInfo->PinInfo[gpioPin].inUse;
    }
    return (status);
}

/**
 *  \brief GPIO Set the bank Use status
 *  This function is used to set the bank inUse status
 *  If the bank is marked as in use, then it shall not be a GPIO and hence GPIO
 *  operations shall not be permitted.
 *
 *  \param   instHandle      [IN]  Gpio_Handle
 *  \param   cmdArg          [IN]  User Setting for this bank
 *  \enter   instHandle          should be valid and not null
 *           cmdArg              should be valid and not null. Should represent
 *                               valid bank number (<numBanks). status to be set
 *                               should be Gpio_InUse type only
 *  \leave  not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_setBankUseStatus(Gpio_Object* instHandle,
                           Gpio_BankCmdArg *cmdArg)
{
    Gpio_Bank          *bankInfo ;
    Int32               status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if((NULL == instHandle) || (NULL == cmdArg))
    {
        status = IOM_EBADARGS;
    }
    /* The bank number given should not exceed nummber of banks for this inst*/
    else if(instHandle->deviceInfo.numBanks <= cmdArg->bankIndex)
    {
        status = IOM_EBADARGS;
    }
    /* The bank status to be set should either be inUse_Yes or inUse_No */
    else if(((Gpio_InUse_Yes != cmdArg->value) &&
             (Gpio_InUse_No != cmdArg->value)))
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(IOM_COMPLETED == status)
    {
        bankInfo = &instHandle->BankInfo[cmdArg->bankIndex];
        bankInfo->inUse = (Gpio_InUse)cmdArg->value;
    }

    return (status);
}

/**
 *  \brief GPIO get the bank Use status
 *  This function is used to get the bank inUse status
 *  This function shall return the if the given bank is in Use or is available as
 *  a GPIO. If it is available as a GPIO (inUse_No) then GPIO operation are
 *  permitted
 *
 *  \param   instHandle      [IN]  Gpio_Handle
 *  \param   cmdArg          [IN]  User Setting for this bank
 *  \enter  instHandle          should be valid and not null
 *          cmdArg              should be valid and not null. Should represent
 *                              valid bank number (<numBanks).
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_getBankUseStatus(Gpio_Object* instHandle,
                           Gpio_BankCmdArg *cmdArg)
{
    Gpio_Bank          *bankInfo ;
    Int32               status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if((NULL == instHandle) || (NULL == cmdArg))
    {
        status = IOM_EBADARGS;
    }
    /* The bank number given should not exceed nummber of banks for this inst*/
    else if(instHandle->deviceInfo.numBanks <= cmdArg->bankIndex)
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(IOM_COMPLETED == status)
    {
        bankInfo = &instHandle->BankInfo[cmdArg->bankIndex];
        cmdArg->value = (Uint32)bankInfo->inUse;
    }
    return (status);
}

/**
 *  \brief GPIO Set the Pin direction
 *  This function is used to set the Pin Direction to Output/Input
 *
 *
 *  \param   instHandle      [IN]  Gpio_Handle
 *  \param   cmdArg          [IN]  User Setting for this pin
 *  \enter  instHandle          should be valid and not null
 *          cmdArg              should be valid and not null. Should represent
 *                              valid pin number (<numPins). Direction to be set
 *                              should be Gpio_Direction type only
 * \leave   not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_setPinDir(Gpio_Object* instHandle,
                    Gpio_PinCmdArg *cmdArg)
{
    Uint32               index;
    CSL_GpioRegsOvly     gpioBaseAddress;
    Uint32               gpioPin;
    Gpio_Bank           *bankInfo = NULL;
    Gpio_Pin            *pinInfo  = NULL;
    Int32                status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if((NULL == instHandle) || (NULL == cmdArg))
    {
        status = IOM_EBADARGS;
    }
    /* The pin number given should not exceed nummber of pins for this inst*/
    else if(instHandle->deviceInfo.numPins <= cmdArg->pin)
    {
        status = IOM_EBADARGS;
    }
    /* The direction should be either input or output */
    else if((Gpio_Direction_Input != cmdArg->value)
              && (Gpio_Direction_Output != cmdArg->value))
    {
        status = IOM_EBADARGS;
    }
    else if(NULL == instHandle->deviceInfo.baseAddress)
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(IOM_COMPLETED == status)
    {
        gpioBaseAddress = instHandle->deviceInfo.baseAddress;
        
        if(NULL != gpioBaseAddress)
        {
            /* The gpioPin number in the driver starts from zero. While in User's
               guide it starts from 1. Hence this operation                   */
            gpioPin = (cmdArg->pin - 1u);
            
            /*
             * Get the bank info for the bank of this pin
             * The index is gpioPin divided by 16
             */
            bankInfo = &instHandle->BankInfo[(gpioPin >> 4u)];
            
            /*
             * Get the pin info for the pin in this bank
             * The index is 4 lsb's of the gpioPin in the bank info
             */
            pinInfo = &bankInfo->PinInfo[(gpioPin & 0xfu)];
            
            /* The index of the bank register to which the pin belongs to is
             * the pin number divided by 32, since, each register (32-bit) has
             * control/data bit fields for 32 pins
             */
            index = (gpioPin >> 5u);
            
            /* In a bank register pin bit field can't exceed 31 since there are
             * only 32 bits. Hence we mask of the higher bits
             */
            gpioPin = (gpioPin & 0x1fu);
            
            if((Gpio_InUse_Yes != bankInfo->inUse) &&
               (Gpio_InUse_Yes != pinInfo->inUse))
            {
                if (Gpio_Direction_Input == cmdArg->value )
                {
                    gpioBaseAddress->BANK[index].DIR |= (1u<<gpioPin);
                }
                else
                {
                    gpioBaseAddress->BANK[index].DIR = \
                        ~((~(gpioBaseAddress->BANK[index].DIR)) |(1u<<gpioPin));
                }
            }
            else {
                status = IOM_EBADARGS;
            }
        }
        else
        {
            status = IOM_EBADARGS;    
        }
    
    }
    return (status);
}



/**
 *  \brief GPIO Set the Pin value
 *  This function is used to set the Pin Value
 *
 *
 *  \param   instHandle    [IN]  Gpio_Handle
 *  \param   cmdArg        [IN]  User Setting for this pin
 *  \enter   instHandle         should be valid and not null
 *           cmdArg             should be valid and not null. Should represent
 *                              valid pin number (<numPins). Value to be set
 *                              should be either 0 or 1 only
 *  \leave   not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_setPinVal(Gpio_Object* instHandle,
                    Gpio_PinCmdArg *cmdArg)
{
    Uint32              index;
    CSL_GpioRegsOvly    gpioBaseAddress;
    Uint32              gpioPin;
    Uint32              value;
    Gpio_Bank          *bankInfo = NULL;
    Gpio_Pin           *pinInfo  = NULL;
    Int32               status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if((NULL == instHandle)||(NULL == cmdArg))
    {
        status = IOM_EBADARGS;
    }
    /* The pin number given should not exceed nummber of pins for this inst*/
    else if(instHandle->deviceInfo.numPins <= cmdArg->pin)
    {
        status = IOM_EBADARGS;
    }
    /* The pin value can only be high(1) or low (0) */
    else if((1u !=cmdArg->value) && (0 != cmdArg->value))
    {
        status = IOM_EBADARGS;
    }
    else if(NULL == instHandle->deviceInfo.baseAddress)
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(IOM_COMPLETED == status)
    {
        gpioBaseAddress = instHandle->deviceInfo.baseAddress;
        
        if(NULL != gpioBaseAddress)
        {
            /* The gpioPin number in the driver starts from zero. While in User's
               guide it starts from 1. Hence this operation                       */
            gpioPin = (cmdArg->pin - 1u);
            value   = cmdArg->value;
            
            /*
             * Get the bank info for the bank of this pin
             * The index is gpioPin divided by 16
             */
            bankInfo = &instHandle->BankInfo[(gpioPin >> 4u)];
            
            /*
             * Get the pin info for the pin in this bank
             * The index is 4 lsb's of the gpioPin in the bank info
             */
            pinInfo = &bankInfo->PinInfo[(gpioPin & 0xf)];
            
            if((Gpio_InUse_Yes != bankInfo->inUse) &&
               (Gpio_InUse_Yes != pinInfo->inUse))
            {
                /* The index of the bank register to which the pin belongs to is
                 * the pin number divided by 32, since, each register (32-bit) has
                 * control/data bit fields for 32 pins
                 */
                index = (gpioPin >> 5u);
            
                /* In a bank register pin bit field can't exceed 31 since there are
                 * only 32 bits. Hence we mask of the higher bits
                 */
                gpioPin = (gpioPin & 0x1fu);
            
                if (1u == value)
                {
                    gpioBaseAddress->BANK[index].SET_DATA = (1u<<gpioPin);
                }
                else
                {
                    gpioBaseAddress->BANK[index].CLR_DATA = (1u<<gpioPin);
                }
            }
            else {
                status = IOM_EBADARGS;
            }
        }
        else
        {
            status = IOM_EBADARGS;
        }
    }
    return (status);
}


/**
 *  \brief GPIO Get the value on Pin
 *  This function is used to get the Pin value
 *
 *
 *  \param   instHandle     [IN]  Gpio_Handle
 *  \param   cmdArg         [IN]  User Data holder for this pin
 *  \enter  instHandle          should be valid and not null
 *          cmdArg              should be valid and not null. Should represent
 *                              valid pin number (<numPins).
 * \leave   not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_getPinVal(Gpio_Object* instHandle,
                    Gpio_PinCmdArg *cmdArg)
{
    Uint32           index;
    CSL_GpioRegsOvly gpioBaseAddress;
    Uint32           gpioPin ;
    Int32            status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if((NULL == instHandle) || (NULL == cmdArg))
    {
        status = IOM_EBADARGS;
    }
    /*The pin number given should not exceed nummber of pins for this inst*/
    else if(instHandle->deviceInfo.numPins <= cmdArg->pin)
    {
        status = IOM_EBADARGS;
    }
    else if(NULL == instHandle->deviceInfo.baseAddress)
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(IOM_COMPLETED == status)
    {
        gpioBaseAddress = instHandle->deviceInfo.baseAddress;
        
        if(NULL != gpioBaseAddress)
        {
            /* The gpioPin number in the driver starts from zero. While in User's
               guide it starts from 1. Hence this operation                   */
            gpioPin = (cmdArg->pin - 1u);
            
            /* The index of the bank register to which the pin belongs to is
             * the pin number divided by 32, since, each register (32-bit) has
             * control/data bit fields for 32 pins
             */
            index = (gpioPin >> 5u);
            
            /* In a bank register pin bit field can't exceed 31 since there are
             * only 32 bits. Hence we mask of the higher bits
             */
            gpioPin = (gpioPin & 0x1fu);
            
            cmdArg->value = ( gpioBaseAddress->BANK[index].IN_DATA
                                    << (31u - gpioPin) ) >> 31u ;
        }
        else
        {
            status = IOM_EBADARGS;
        }
    }
    return (status);
}

/**
 *  \brief GPIO Set value to a group of pins in a bank
 *
 *
 *  \param   instHandle   [IN]  Gpio_Handle
 *  \param   cmdArg       [IN]  User data for the pin group
 *  \enter  instHandle          should be valid and not null
 *          cmdArg              should be valid and not null. Should represent
 *                              valid bank number (<numBanks).
 * \leave   not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_setGroupVal(Gpio_Object* instHandle,
                      Gpio_BankCmdArg *cmdArg)
{
    CSL_GpioRegsOvly    gpioBaseAddress;
    Uint32              outVal ;
    Uint32              bankRegIndex;
    Uint32              outMask = 0;
    Int32               status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if((NULL == instHandle) || (NULL == cmdArg))
    {
        status = IOM_EBADARGS;
    }
    else if(instHandle->deviceInfo.numBanks <= cmdArg->bankIndex)
    {
        status = IOM_EBADARGS;
    }
    else if(NULL == instHandle->deviceInfo.baseAddress)
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(status == IOM_COMPLETED)
    {
        gpioBaseAddress = instHandle->deviceInfo.baseAddress;

        if(NULL != gpioBaseAddress)
        {
            /* This is because the regIndex here is for bank setting registers .
             * Each bank setting registers contain setting for two banks */
            bankRegIndex = cmdArg->bankIndex/2u;
            
            /* If the banknum is an even, then the position if the out bits in the
             * bank register for its pins is in the lower 16 bits */
            if(0 == (cmdArg->bankIndex%2u))
            {
                outMask = (cmdArg->mask)&0x0000FFFFu;
                outVal = (cmdArg->value)&0x0000FFFFu;
            }
            /* If the banknum is an odd, then the position if the out bits in the
             * bank register for its pins is in the higher 16 bits */
            else
            {
                outMask = ((cmdArg->mask)&0x0000FFFFu) << 16u;
                outVal = ((cmdArg->value)&0x0000FFFFu) << 16u;
            }
            
            /* Set GPIO pins corresponding to 1's*/
            gpioBaseAddress->BANK[bankRegIndex].SET_DATA=(outMask & outVal);
            
            /* Reset GPIO pins corresponding to 0's*/
            gpioBaseAddress->BANK[bankRegIndex].CLR_DATA=
                (outMask & (~outVal));
        }
        else
        {
            status = IOM_EBADARGS;
        }
    }

    return (status);
}

/**
 *  \brief GPIO get value from a group of pins in a bank
 *
 *
 *  \param   instHandle   [IN]  Gpio_Handle
 *  \param   cmdArg       [IN]  User data holder for the pin group
 *  \enter   instHandle         should be valid and not null
 *           cmdArg             should be valid and not null. Should represent
 *                              valid bank number (<numBanks).
 * \leave    not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_getGroupVal(Gpio_Object* instHandle,
                      Gpio_BankCmdArg *cmdArg)
{
    CSL_GpioRegsOvly gpioBaseAddress;
    Uint32        regIndex ;
    Uint32        readVal = 0;
    Int32         status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if((NULL == instHandle)||(NULL == cmdArg))
    {
        status = IOM_EBADARGS;
    }
    else if(instHandle->deviceInfo.numBanks <= cmdArg->bankIndex)
    {
        status = IOM_EBADARGS;
    }
    else if(NULL == instHandle->deviceInfo.baseAddress)
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(IOM_COMPLETED == status)
    {
        gpioBaseAddress = instHandle->deviceInfo.baseAddress;
        
        if(NULL != gpioBaseAddress)
        {
            regIndex = cmdArg->bankIndex/2u;
            readVal = gpioBaseAddress->BANK[regIndex].IN_DATA;
            
            /* If the banknum is even then the position if the in bits in the
             * bank register for its pins is in the lower 16 bits */
            if(0 == (cmdArg->bankIndex%2u))
            {
                cmdArg->value = (readVal)&0x0000FFFFu;
            }
            /* If the banknum is an odd, then the position if the in bits in the
             * bank register for its pins is in the higher 16 bits */
            else
            {
                cmdArg->value = ((readVal)&0xFFFF0000)>>16;
            }
        }
        else
        {
            status = IOM_EBADARGS;
        }
    }
    return (status);
}

/**
 *  \brief GPIO Clear rising trigger level value for a pin interrupt
 *
 *
 *  \param   instHandle       [IN]  Gpio_Handle
 *  \param   gpioPin          [IN]  Pin for which trigger value is set
 *  \enter   instHandle             should be valid and not null
 *           gpioPin                should be valid pin number (<numPins).
 * \leave    not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_clearRisingEdgeTrigger(Gpio_Handle instHandle,
                                 Uint8 gpioPin)
{
    CSL_GpioRegsOvly gpioBaseAddress;
    Uint32           index;
    Gpio_Bank       *bankInfo = NULL;
    Gpio_Pin        *pinInfo = NULL;
    Int32            status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if(NULL == instHandle)
    {
        status = IOM_EBADARGS;
    }/* The pin num given should not exceed the num pins for this instance*/
    else if(instHandle->deviceInfo.numPins < gpioPin)
    {
        status = IOM_EBADARGS;
    }
    else if(NULL == instHandle->deviceInfo.baseAddress)
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(status == IOM_COMPLETED)
    {
        gpioBaseAddress = instHandle->deviceInfo.baseAddress;
        
        if(NULL != gpioBaseAddress)
        {
            /* The gpioPin number in the driver starts from zero. While in User's
               guide it starts from 1. Hence this operation                   */
               gpioPin = (gpioPin - 1u);
            
            /*
             * Get the bank info for the bank of this pin
             * The index is gpioPin divided by 16
             */
            bankInfo = &instHandle->BankInfo[(gpioPin >> 4u)];
            
            /*
             * Get the pin info for the pin in this bank
             * The index is 4 lsb's of the gpioPin in the bank info
             */
            pinInfo = &bankInfo->PinInfo[(gpioPin & 0xfu)];
            
            if((Gpio_InUse_Yes != bankInfo->inUse) &&
               (Gpio_InUse_Yes != pinInfo->inUse))
            {
               /* The index of the bank register to which the pin belongs to is
                * the pin number divided by 32, since, each register (32-bit) has
                * control/data bit fields for 32 pins
                */
                index = (gpioPin >> 5u);
            
                /* In a bank register pin bit field can't exceed 31 since there are
                 * only 32 bits. Hence we mask of the higher bits
                 */
                gpioPin = (gpioPin & 0x1fu);
            
                gpioBaseAddress->BANK[index].CLR_RIS_TRIG = (1u<<gpioPin);
            }
            else
            {
                status = IOM_EBADARGS;
            }
        }
        else
        {
            status = IOM_EBADARGS;
        }
    }
    return (status);
}

/**
 *  \brief GPIO Set rising trigger level value for a pin interrupt
 *
 *
 *  \param   instHandle       [IN]  Gpio_Handle
 *  \param   gpioPin          [IN]  Pin for which trigger value is set
 *  \enter   instHandle          should be valid and not null
 *           gpioPin             should be valid pin number (<numPins).
 * \leave    not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_setRisingEdgeTrigger(Gpio_Handle instHandle,
                               Uint8 gpioPin)
{
    CSL_GpioRegsOvly gpioBaseAddress;
    Uint32           index;
    Gpio_Bank       *bankInfo = NULL;
    Gpio_Pin        *pinInfo = NULL;
    Int32            status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if(NULL == instHandle)
    {
        status = IOM_EBADARGS;
    }/* The pin num given should not exceed the num pins for this instance*/
    else if(instHandle->deviceInfo.numPins < gpioPin)
    {
        status = IOM_EBADARGS;
    }
    else if(NULL == instHandle->deviceInfo.baseAddress)
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(IOM_COMPLETED == status)
    {
        gpioBaseAddress = instHandle->deviceInfo.baseAddress;
        
        if(NULL != gpioBaseAddress)
        {
            /* The gpioPin number in the driver starts from zero. While in User's
               guide it starts from 1. Hence this operation                   */
            gpioPin = (gpioPin - 1u);
            /*
             * Get the bank info for the bank of this pin
             * The index is gpioPin divided by 16
             */
            bankInfo = &instHandle->BankInfo[(gpioPin >> 4u)];
            
            /*
             * Get the pin info for the pin in this bank
             * The index is 4 lsb's of the gpioPin in the bank info
             */
            pinInfo = &bankInfo->PinInfo[(gpioPin & 0xf)];
            
            if((Gpio_InUse_Yes != bankInfo->inUse) &&
               (Gpio_InUse_Yes != pinInfo->inUse))
            {
                /* The index of the bank register to which the pin belongs to is
                * the pin number divided by 32, since, each register (32-bit) has
                * control/data bit fields for 32 pins
                */
                index = (gpioPin >> 5u);
            
                /* In a bank register pin bit field can't exceed 31 since there are
                 * only 32 bits. Hence we mask of the higher bits
                 */
                gpioPin = (gpioPin & 0x1fu);
            
                gpioBaseAddress->BANK[index].SET_RIS_TRIG = (1u<<gpioPin);
            }
            else
            {
                status = IOM_EBADARGS;
            }
        }
        else
        {
            status = IOM_EBADARGS;    
        }
    }
    return (status);
}


/**
 *  \brief GPIO Clear falling trigger level value for a pin interrupt
 *
 *
 *  \param   instHandle       [IN]  Gpio_Handle
 *  \param   gpioPin          [IN]  Pin for which trigger value is set
 *  \enter   instHandle       should be valid and not null
 *           gpioPin          should be a valid pin number (<numPins).
 * \leave   not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_clearFallingEdgeTrigger(Gpio_Handle instHandle,
                                  Uint8 gpioPin)
{
    CSL_GpioRegsOvly gpioBaseAddress;
    Uint32           index;
    Gpio_Bank       *bankInfo = NULL;
    Gpio_Pin        *pinInfo = NULL;
    Int32            status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if(NULL == instHandle)
    {
        status = IOM_EBADARGS;
    }
    /* The pin num given should not exceed the num pins for this instance*/
    else if(instHandle->deviceInfo.numPins < gpioPin)
    {
        status = IOM_EBADARGS;
    }
    else if(NULL == instHandle->deviceInfo.baseAddress)
    {
        status = IOM_EBADARGS;
    }    
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if( IOM_COMPLETED == status)
    {
        gpioBaseAddress = instHandle->deviceInfo.baseAddress;
        
        if(NULL != gpioBaseAddress)
        {
            /* The gpioPin number in the driver starts from zero. While in User's
               guide it starts from 1. Hence this operation                   */
               gpioPin = (gpioPin - 1u);
            
            /*
             * Get the bank info for the bank of this pin
             * The index is gpioPin divided by 16
             */
            bankInfo = &instHandle->BankInfo[(gpioPin >> 4u)];
            
            /*
             * Get the pin info for the pin in this bank
             * The index is 4 lsb's of the gpioPin in the bank info
             */
            pinInfo = &bankInfo->PinInfo[(gpioPin & 0xf)];
            
            if((Gpio_InUse_Yes != bankInfo->inUse) &&
               (Gpio_InUse_Yes != pinInfo->inUse))
            {
                /* The index of the bank register to which the pin belongs to is
                * the pin number divided by 32, since, each register (32-bit) has
                * control/data bit fields for 32 pins
                */
                index = (gpioPin >> 5);
                /* In a bank register pin bit field can't exceed 31 since there are
                * only 32 bits. Hence we mask of the higher bits
                */
                gpioPin = (gpioPin & 0x1f);
            
                gpioBaseAddress->BANK[index].CLR_FAL_TRIG = (1u<<gpioPin);
            }
            else
            {
                status = IOM_EBADARGS;
            }
        }
        else
        {
            status = IOM_EBADARGS;
        }
    }
    return (status);
}


/**
 *  \brief GPIO Set falling trigger level value for a pin interrupt
 *
 *
 *  \param   instHandle       [IN]  Gpio_Handle
 *  \param   gpioPin          [IN]  Pin for which trigger value is set
 *  \enter   instHandle       should be valid and not null
 *           gpioPin          should be a valid pin number (<numPins).
 * \leave   not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_setFallingEdgeTrigger(Gpio_Handle instHandle,
                                Uint8 gpioPin)
{
    CSL_GpioRegsOvly gpioBaseAddress;
    Uint32           index;
    Gpio_Bank       *bankInfo = NULL;
    Gpio_Pin        *pinInfo = NULL;
    Int32            status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if(NULL == instHandle)
    {
        status = IOM_EBADARGS;
    }
    /* The pin num given should not exceed the num pins for this instance*/
    else if(instHandle->deviceInfo.numPins < gpioPin)
    {
        status = IOM_EBADARGS;
    }
    else if(NULL == instHandle->deviceInfo.baseAddress)
    {
        status = IOM_EBADARGS;
    }      
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(IOM_COMPLETED == status)
    {
        gpioBaseAddress = instHandle->deviceInfo.baseAddress;
        
        if(NULL != gpioBaseAddress)
        {
            /* The gpioPin number in the driver starts from zero. While in User's
               guide it starts from 1. Hence this operation                       */
               gpioPin = (gpioPin - 1u);
            
            /*
             * Get the bank info for the bank of this pin
             * The index is gpioPin divided by 16
             */
            bankInfo = &instHandle->BankInfo[(gpioPin >> 4u)];
            
            /*
             * Get the pin info for the pin in this bank
             * The index is 4 lsb's of the gpioPin in the bank info
             */
            pinInfo = &bankInfo->PinInfo[(gpioPin & 0xf)];
            
            if((Gpio_InUse_Yes != bankInfo->inUse) &&
               (Gpio_InUse_Yes != pinInfo->inUse))
            {
                /* The index of the bank register to which the pin belongs to is
                * the pin number divided by 32, since, each register (32-bit) has
                * control/data bit fields for 32 pins
                */
            
                index = (gpioPin >> 5u);
                /* In a bank register pin bit field can't exceed 31 since there are
                * only 32 bits. Hence we mask of the higher bits
                */
                gpioPin = (gpioPin & 0x1fu);
            
                gpioBaseAddress->BANK[index].SET_FAL_TRIG = (1u<<gpioPin);
            }
            else
            {
                status = IOM_EBADARGS;
            }
        }
        else
        {
        status = IOM_EBADARGS;
        }
    }
    return (status);
}



/**
 *  \brief GPIO Bank interrupt  enable
 *
 *
 *  \param   instHandle   [IN]  Gpio_Handle
 *  \param   bankNum      [IN]  The bank number
 *  \enter   instHandle        should be valid and not null
 *           bankNum           should be a valid bank number (<numBanks).
 * \leave   not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_bankInterruptEnable(Gpio_Handle instHandle,
                              Uint8  bankNum)
{
    CSL_GpioRegsOvly        gpioBaseAddress;
    Int32                   status = IOM_COMPLETED;
    Gpio_Bank              *bankInfo;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if(NULL == instHandle)
    {
        status = IOM_EBADARGS;
    }
    /* The bank num given should not exceed the num banks for this instance*/
    else if(instHandle->deviceInfo.numBanks <= bankNum)
    {
        status = IOM_EBADARGS;
    }
    else if(NULL == instHandle->deviceInfo.baseAddress)
    {
        status = IOM_EBADARGS;
    }      
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(IOM_COMPLETED == status)
    {
        bankInfo = &instHandle->BankInfo[bankNum];

        if(!(Gpio_InUse_Yes == bankInfo->inUse))
        {
            gpioBaseAddress = instHandle->deviceInfo.baseAddress;
            if(NULL != gpioBaseAddress)
            {
                gpioBaseAddress->BINTEN |= (1u<<bankNum);
                instHandle->BankInfo[bankNum].intEnabled = 1u;
            }
            else
            {
                status = IOM_EBADARGS;
            }
        }
        else
        {
            status = IOM_EBADARGS;
        }
    }
    return (status);
}


/**
 *  \brief GPIO Bank interrupt disable
 *
 *
 *  \param   instHandle   [IN]  Gpio_Handle
 *  \param   bankNum      [IN]  The bank number
 *  \enter   instHandle        should be valid and not null
 *           bankNum           should be a valid bank number (<numBanks).
 * \leave   not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_bankInterruptDisable(Gpio_Handle instHandle,
                               Uint8    bankNum)
{
    CSL_GpioRegsOvly    gpioBaseAddress;
    Gpio_Bank          *bankInfo;
    Int32               status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if(NULL == instHandle)
    {
        status = IOM_EBADARGS;
    }
    /* The bank num given should not exceed the num banks for this instance*/
    else if(instHandle->deviceInfo.numBanks <= bankNum)
    {
        status = IOM_EBADARGS;
    }
    else if(NULL == instHandle->deviceInfo.baseAddress)
    {
        status = IOM_EBADARGS;
    } 
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(IOM_COMPLETED == status)
    {
        bankInfo = &instHandle->BankInfo[bankNum];
        gpioBaseAddress = instHandle->deviceInfo.baseAddress;

        if(Gpio_InUse_Yes != bankInfo->inUse)
        {
            if(NULL != gpioBaseAddress)
            {
                gpioBaseAddress->BINTEN &= (~(1u<<bankNum));
                instHandle->BankInfo[bankNum].intEnabled = 0;
            }
            else
            {
                status = IOM_EBADARGS;
            }
        }
        else
        {
            status = IOM_EBADARGS;
        }
    }
    return (status);
}

/**
 *  \brief GPIO clear interrupt status
 *  This function is used to clear GPIO pin interrupt status
 *
 *
 *  \param   instHandle      [IN]  Gpio_Handle
 *  \param   cmdArg          [IN]  Pin for which the status is to be cleared
 *  \enter   instHandle            should be valid and not null
 *           cmdArg                should be valid and not null. Should represent
 *                                 valid pin number (<numPins).
 * \leave   not implemented
 *
 *  \return status              IOM error code status
 */
Int32 Gpio_clearInterruptStatus(Gpio_Object* instHandle,
                                Gpio_PinCmdArg *cmdArg)
{
    Uint32               index;
    CSL_GpioRegsOvly     gpioBaseAddress;
    Uint32               gpioPin;
    Gpio_Bank           *bankInfo = NULL;
    Gpio_Pin            *pinInfo  = NULL;
    Int32                status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if((NULL == instHandle) || (NULL == cmdArg))
    {
        status = IOM_EBADARGS;
    }
    /* The pin number given should not exceed nummber of pins for this inst*/
    else if(instHandle->deviceInfo.numPins <= cmdArg->pin)
    {
        status = IOM_EBADARGS;
    }
    else if(NULL == instHandle->deviceInfo.baseAddress)
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(IOM_COMPLETED == status)
    {
        gpioBaseAddress = instHandle->deviceInfo.baseAddress;
        
        if(NULL != gpioBaseAddress)
        {
            /* The gpioPin number in the driver starts from zero. While in User's
               guide it starts from 1. Hence this operation                   */
            gpioPin = (cmdArg->pin - 1u);
            
            /*
             * Get the bank info for the bank of this pin
             * The index is gpioPin divided by 16
             */
            bankInfo = &instHandle->BankInfo[(gpioPin >> 4u)];
            
            /*
             * Get the pin info for the pin in this bank
             * The index is 4 lsb's of the gpioPin in the bank info
             */
            pinInfo = &bankInfo->PinInfo[(gpioPin & 0xfu)];
            
            /* The index of the bank register to which the pin belongs to is
             * the pin number divided by 32, since, each register (32-bit) has
             * control/data bit fields for 32 pins
             */
            index = (gpioPin >> 5u);
            
            /* In a bank register pin bit field can't exceed 31 since there are
             * only 32 bits. Hence we mask of the higher bits
             */
            gpioPin = (gpioPin & 0x1fu);
            
            if((Gpio_InUse_Yes != bankInfo->inUse) &&
               (Gpio_InUse_Yes != pinInfo->inUse))
            {
                    gpioBaseAddress->BANK[index].INTSTAT |= (1u<<gpioPin);
            }
        }
        else
        {
            status = IOM_EBADARGS;    
        }
    
    }
    return (status);
}


/**
 *  \brief GPIO register interrupt handler
 *  This funtion register an interrupt handler for a pin/bank interrupt as
 *  specified in the user command argument
 *
 *  \param   instHandle   [IN]  Gpio_Handle
 *  \param   cmdArg       [IN]  User Interrupt data this bank/pin
 *  \enter   instHandle         should be valid and not null
 *           cmdArg             should be not null. Should represent a valid pin
 *                              or bank event which can generate event. Should
 *                              provide valid function for interrupt registration
 * \leave   not implemented
 *
 *  \return status              IOM error code status
 */

Int32 Gpio_regIntHandler(Gpio_Object* instHandle,
                        Gpio_IntrCmdArg *cmdArg)
{
    Uint32          bankNum;
    Uint32          pinNum;
    Gpio_Bank       *bankInfo;
    Gpio_Pin        *pinInfo;
    Int32           status = IOM_COMPLETED;
    ECM_Attrs       ecmattrs   = ECM_ATTRS;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if((NULL == instHandle)||(NULL == cmdArg))
    {
        status = IOM_EBADARGS;
    }
    else if (NULL == cmdArg->isrHandler)
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = IOM_COMPLETED;
    }    
#endif

    if(IOM_COMPLETED == status)
    {
        switch (cmdArg->bankOrPin)
        {
            case Gpio_BankOrPin_isBank:

                 if(instHandle->deviceInfo.numBanks <= cmdArg->value)
                 {
                    status = IOM_EBADARGS;
                 }
                 else
                 {
                     bankNum = cmdArg->value;
                     bankInfo = &instHandle->BankInfo[bankNum];

                     if(bankInfo->evtNum <= 0)
                     {
                         status = IOM_EBADARGS;
                     }
                     else
                     {
                         if(Gpio_InUse_No == bankInfo->inUse)
                         {
                             bankInfo->isrHandler = cmdArg->isrHandler;

                             /*
                              *  Map the eventid to the ECM Dispatch of the
                              *  DSP/BIOS
                              */
                             ecmattrs.unmask = 1u;
                             ecmattrs.arg = (Arg)instHandle;
                             ECM_dispatchPlug(
                                 bankInfo->evtNum,
                                 (ECM_Fxn)(bankInfo->isrHandler),
                                 &ecmattrs);

                             ECM_enableEvent(bankInfo->evtNum);

                             /* enable HWI number 'hwiNum'                    */
                             C64_enableIER(1 << bankInfo->hwiNum);
                         }
                         else
                         {
                             status = IOM_EBADARGS;
                         }
                     }
                 }
            break;

            case Gpio_BankOrPin_isPin:

                 if((instHandle->deviceInfo.numPins <= cmdArg->value))
                 {
                    status = IOM_EBADARGS;
                 }
                 else
                 {
                     /*Calculate the bank in which it falls. */
                     /* The gpioPin number in the driver starts from zero. While
                        in User's guide it starts from 1.Hence this operation */
                     pinNum = (cmdArg->value - 1u);
                     bankNum = pinNum/(instHandle->deviceInfo.numPinsPerBank);
                     bankInfo = &instHandle->BankInfo[bankNum];
                     pinInfo = &instHandle->BankInfo[bankNum].PinInfo[pinNum];

                     if(pinInfo->evtNum <= 0)
                     {
                         status = IOM_EBADARGS;
                     }

                     /* Verify if the GpioPin asked for is available for use
                      * Only then register the interrupt, and markit as busy
                      */
                     if((Gpio_InUse_No == pinInfo->inUse) &&
                        (Gpio_InUse_No == bankInfo->inUse))
                     {
                         pinInfo->isrHandler = cmdArg->isrHandler;

                         /*
                          *  Map the eventid to the ECM Dispatch of the
                          *  DSP/BIOS
                          */
                         ecmattrs.unmask = 1u;
                         ecmattrs.arg = (Arg)instHandle;
                         ECM_dispatchPlug(
                             pinInfo->evtNum,
                             (ECM_Fxn)(pinInfo->isrHandler),
                             &ecmattrs);

                         ECM_enableEvent(pinInfo->evtNum);

                         /* enable HWI number 'hwiNum'                    */
                         C64_enableIER(1 << pinInfo->hwiNum);
                     }
                     else
                     {
                         status = IOM_EBADARGS;
                     }
                 }
            break;

            default:
                status = IOM_EBADARGS;
            break;
        }
    }

    return (status);
}



/**
 *  \brief GPIO unregister interrupt handler
 *  This funtion unregister an interrupt handler for a pin/bank interrupt as
 *  specified in the user command argument.
 *
 *  \param   instHandle   [IN]  Gpio_Handle
 *  \param   cmdArg       [IN]  User Interrupt data this bank/pin
 *
 *  \return status              IOM error code status
 */

Int32 Gpio_unregIntHandler(Gpio_Object* instHandle,
                          Gpio_IntrCmdArg *cmdArg)
{
    Uint32     bankNum;
    Uint32     pinNum;
    Gpio_Bank *bankInfo;
    Gpio_Pin  *pinInfo;
    Int32      status = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if((NULL == instHandle)||(NULL == cmdArg))
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = IOM_COMPLETED;
    }    
#endif


    if(IOM_COMPLETED == status)
    {

        switch (cmdArg->bankOrPin)
        {
            case Gpio_BankOrPin_isBank:
                if(instHandle->deviceInfo.numBanks <= cmdArg->value)
                {
                    status = IOM_EBADARGS;
                }
                else
                {
                    bankNum = cmdArg->value;
                    bankInfo = &instHandle->BankInfo[bankNum];

                    if(bankInfo->evtNum <= 0)
                    {
                        status = IOM_EBADARGS;
                    }

                    if(bankInfo->isrHandler != (&Gpio_DefaultIntHandler))
                    {
                        bankInfo->isrHandler = (&Gpio_DefaultIntHandler);
                        ECM_disableEvent(bankInfo->evtNum);
                    }
                    else
                    {
                        /* No app handler is registered ; So raise error */
                    }
                }
                break;

            case Gpio_BankOrPin_isPin:
                if(instHandle->deviceInfo.numPins <= cmdArg->value)
                {
                    status = IOM_EBADARGS;
                }
                else
                {
                    /*Calculate the bank in which it falls.
                     * There are 16 pins in each bank
                     */
                    /* The gpioPin number in the driver starts from zero. While
                       in User's guide it starts from 1.Hence this operation  */
                    pinNum = (cmdArg->value - 1u);
                    bankNum = pinNum/(instHandle->deviceInfo.numPinsPerBank);
                    bankInfo = &instHandle->BankInfo[bankNum];
                    pinInfo = &instHandle->BankInfo[bankNum].PinInfo[pinNum];

                    if(pinInfo->evtNum < 0)
                    {
                        status = IOM_EBADARGS;
                    }

                    if(pinInfo->isrHandler != (&Gpio_DefaultIntHandler))
                    {
                        pinInfo->isrHandler = (&Gpio_DefaultIntHandler);
                        ECM_disableEvent(pinInfo->evtNum);
                    }
                    else
                    {
                        status = IOM_EBADARGS;
                    }
                }
            break;

            default:
                status = IOM_EBADARGS;
            break;
        }
    }
    return (status);
}

/*!
 *  \brief Open a GPIO handle.
 *
 * This function is used to open a Gpio_Handle for use as a Gpio instance.
 *  @param(params)         Gpio_Params handle containing Gpio settings.
 *  @b(returns)            handle to Gpio instance
 */
Gpio_Handle Gpio_open(Gpio_Params *params)
{
    Int     i;
    Int32   status = IOM_COMPLETED;
    static Bool visited = FALSE;
    Gpio_Handle retHandle;
    Uint32 hwiKey = 0;
    /*  only initialize array of instances and module state on first run of
     *  this function (this function will be called for each Gpio instance,
     *  once multiple instances are supported.
     */
    hwiKey = _disable_interrupts();
    if (FALSE == visited)
    {
        _restore_interrupts(hwiKey);
        visited = TRUE;

        for (i = 0; i < CSL_GPIO_PER_CNT; i++)
        {
            /* all instances are available initially */
            Gpio_module.inUse[i] = FALSE;
            memset((Void *)&Gpio_Instances[i], 0x0, sizeof(Gpio_Object));
        }
#if defined(CHIP_C6747) || defined(CHIP_OMAPL137) || defined(CHIP_C6748) || defined(CHIP_OMAPL138)
        /* Gpio0 instance  */
        Gpio_deviceInstInfo[0].baseAddress    = (CSL_GpioRegsOvly)CSL_GPIO_0_REGS;
        Gpio_deviceInstInfo[0].numBanks       = (Uint8)Gpio_NUM_BANKS;
        Gpio_deviceInstInfo[0].numPinsPerBank = (Uint8)Gpio_NUM_PINS_PER_BANK;
        Gpio_deviceInstInfo[0].numPins        = (Uint16)Gpio_NUM_PINS;

        Gpio_deviceInstInfo[0].bankEventNumber[0] = (Int8)CSL_INTC_EVENTID_GPIO_BNK0_INT;
        Gpio_deviceInstInfo[0].bankEventNumber[1] = (Int8)CSL_INTC_EVENTID_GPIO_BNK1_INT;
        Gpio_deviceInstInfo[0].bankEventNumber[2] = (Int8)CSL_INTC_EVENTID_GPIO_BNK2_INT;
        Gpio_deviceInstInfo[0].bankEventNumber[3] = (Int8)CSL_INTC_EVENTID_GPIO_BNK3_INT;
        Gpio_deviceInstInfo[0].bankEventNumber[4] = (Int8)CSL_INTC_EVENTID_GPIO_BNK4_INT;
        Gpio_deviceInstInfo[0].bankEventNumber[5] = (Int8)CSL_INTC_EVENTID_GPIO_BNK5_INT;
        Gpio_deviceInstInfo[0].bankEventNumber[6] = (Int8)CSL_INTC_EVENTID_GPIO_BNK6_INT; 
        Gpio_deviceInstInfo[0].bankEventNumber[7] = (Int8)CSL_INTC_EVENTID_GPIO_BNK7_INT;

        for (i = 0; i < Gpio_NUM_PINS; i++)
        {
            Gpio_deviceInstInfo[0].pinEventNumber[i] = -1;
        }
#endif

#if defined(CHIP_C6748) || defined(CHIP_OMAPL138)
        Gpio_deviceInstInfo[0].bankEventNumber[8] = (Int8)CSL_INTC_EVENTID_GPIO_BNK8_INT;
#endif
    }
    else
    {
        status = IOM_EINUSE;
        _restore_interrupts(hwiKey);
    }

/* verify parameters */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if(IOM_COMPLETED == status)
    {
        if ((NULL == params))
        {
             status = IOM_EBADARGS;
        }
        else if((Gpio_NUM_INSTANCES <= params->instNum) ||
                (TRUE == Gpio_module.inUse[params->instNum]))
        {
             status = IOM_EBADARGS;
        }
        else
        {
            status = IOM_COMPLETED;
        }
    }
#endif

    if(IOM_COMPLETED == status)
    {
        Gpio_Instances[params->instNum].instNum = params->instNum;
        gpioInit(&Gpio_Instances[params->instNum], \
            (Gpio_BankConfig*)&params->BankParams[0]);
        retHandle = &Gpio_Instances[params->instNum];
    }
    else
    {
        retHandle = NULL;
    }

    return (retHandle);
}

/*!
 *  \brief Close a GPIO handle
 *
 * This function is used to close a Gpio_Handle that was opened previously.
 *  @param(instHandle)     pointer to the Gpio module instance handle
 *  @b(returns)            none.
 */
Void Gpio_close(Gpio_Handle instHandle)
{
    if(NULL != instHandle)
    {
        /* This instance is no more in use       */
        Gpio_module.inUse[instHandle->instNum] = FALSE;
        memset((Void *)&Gpio_Instances[instHandle->instNum], 0x0, sizeof(Gpio_Object));
    }
}

