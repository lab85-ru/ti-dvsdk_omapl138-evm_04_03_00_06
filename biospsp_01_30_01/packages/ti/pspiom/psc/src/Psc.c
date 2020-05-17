/*
 * Psc.c
 *
 * This file contains PSC driver Module Interface implementation for the PSC
 * Driver. PSC Driver provides APIs directly that can be directly called to do
 * operations like transitioning power states for a particular peripheral (module)
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

/**
 *  \file    Psc.c
 *
 *  \brief   This file implements the power sleep controller's sleep and wakeup
 *           capablity of different devices
 *
 *           (C) Copyright 2008, Texas Instruments, Inc
 *
 *  \author  PSG
 *
 *  \version
 *           0.2 - Imtiaz    Power management related changes.
 *           0.1 - M.Sriram  Created newly for the OMAPL138 platform
 *
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include <std.h>
#include <assert.h>
#include <string.h>
#include <iom.h>
#include <ti/pspiom/psc/Psc.h>

#ifdef CHIP_C6747
    #include <ti/pspiom/cslr/soc_C6747.h>
    #include <ti/pspiom/cslr/cslr_psc_C6747.h>
#endif

#ifdef CHIP_OMAPL137
    #include <ti/pspiom/cslr/soc_OMAPL137.h>
    #include <ti/pspiom/cslr/cslr_psc_OMAPL137.h>
#endif

#ifdef CHIP_C6748
    #include <ti/pspiom/cslr/soc_C6748.h>
    #include <ti/pspiom/cslr/cslr_psc_C6748.h>
#endif

#ifdef CHIP_OMAPL138
    #include <ti/pspiom/cslr/soc_OMAPL138.h>
    #include <ti/pspiom/cslr/cslr_psc_OMAPL138.h>
#endif


/**
 * \brief    Variable to hold the power state of all the modules controlled by
 *           the PSC.
 *
 * \note     The "CSL_LPSC_INSTANCE_MAX" indicates the number of PSC instances
 *           and "CSL_LPSC_NUMBER_MAX" indicates the max number of devices
 *           supported per instance.
 */
static Uint32 Psc_dependency[CSL_LPSC_INSTANCE_MAX][CSL_LPSC_NUMBER_MAX];

/**
 *  \brief  Function which enables or disable the clock power domain.
 *
 *   The function enables/disables the clock power domain of a particular module
 *   depending on the moduleID passed.
 *
 *  \param  pscDevId    [IN]  devId
 *  \param  moduleId    [IN]  LPSC number of the module to be controlled
 *  \param  clkCtrl     [IN]  clock enable or disable)
 *
 *  \return DriverTypes_COMPLETED or ERROR
 */
Int32 Psc_ModuleClkCtrl(Psc_DevId       pscDevId,
                        Uint32          moduleId,
                        Bool            clkCtrl)
{
    Uint32            lpscNum   = moduleId;
    Uint16            state     = 0;
    volatile Uint32   count     = 0;
    CSL_PscRegsOvly   pscRegs   = NULL;
    Uint32            hwiKey    = 0x00;
    Int32             result    = IOM_COMPLETED;

    /* check the module for which the operation is requested                  */
    if (pscDevId == Psc_DevId_0)
    {
        pscRegs = (CSL_PscRegsOvly)CSL_PSC_0_REGS;
    }
    else
    {
        pscRegs = (CSL_PscRegsOvly)CSL_PSC_1_REGS;
    }

    assert(NULL != pscRegs);

    hwiKey = (Uint32)_disable_interrupts();

    /* The request is for the module to be switched on in the PSC             */
    if (TRUE == clkCtrl)
    {
        /* if the module is already switched on in the PSC(i.e. count >=1)then*
         * we will just increment the dependency count to keep track of the   *
         * dependent modules rather than trying to switch on the module again */
        if (1u <= Psc_dependency[pscDevId][lpscNum])
        {
            ++Psc_dependency[pscDevId][lpscNum];
            _restore_interrupts(hwiKey);
        }
        else
        {
            /* if the control has come here then the module is currently      *
             * switched OFF and the caller is requesting for the switching ON *
             * of the module. Hence we need to switch on the module in the    *
             * PSC physically and then increment the dependency count         */
            state = CSL_PSC_MDCTL_NEXT_ENABLE;
        }
    }
    else
    {
        /* The caller has requested for switching OFF of the module in the    *
         * PSC.Here if the dependency count is > 1 , it means that some other *
         * module(may be an IO request or another channel) is still           *
         * dependent on this device.Hence, we cannot disable the device at    *
         * this time and only will reduce the dependecy count.when the        *
         * dependency count has reached 1, the module can then be physically  *
         * switched OFF in the PSC                                            */
        if (1u < Psc_dependency[pscDevId][lpscNum])
        {
            --Psc_dependency[pscDevId][lpscNum];
            _restore_interrupts(hwiKey);
        }
        else
        {
            /* This checks if the caller is requesting for an already OFF     *
             * device to be switched ON. In that case we will flag an error   */
            if (Psc_dependency[pscDevId][lpscNum] < 1u)
            {
                result = IOM_EBADMODE;
                _restore_interrupts(hwiKey);
            }
            else
            {
                /* The device is already ON. Hence we will try to switch OFF  *
                 * the device in the PSC now                                  */
                state = CSL_PSC_MDCTL_NEXT_DISABLE;
            }
        }
    }

    /* depending on the "state" variable. we will now perform the switch      *
     * ON/OFF of the module                                                   */
    if ((IOM_COMPLETED == result) &&
        ((CSL_PSC_MDCTL_NEXT_DISABLE == state)
            ||(CSL_PSC_MDCTL_NEXT_ENABLE == state)))
    {
#if defined(CHIP_C6748) || defined(CHIP_OMAPL138)
        if (CSL_PSC_SATA == lpscNum)
        {
            pscRegs->MDCTL[lpscNum] =
                ((pscRegs->MDCTL[lpscNum] & ~(CSL_PSC_MDCTL_NEXT_MASK))
                    | state | CSL_PSC_MDCTL_FORCE_MASK);
        }
        else
        {
#endif
            pscRegs->MDCTL[lpscNum] =
                ((pscRegs->MDCTL[lpscNum] & ~(CSL_PSC_MDCTL_NEXT_MASK))| state);
#if defined(CHIP_C6748) || defined(CHIP_OMAPL138)
        }
#endif

        /* Set the GO[0]/GO[1] bit in PTCMD to 1 to initiate the transition(s)*/
        (pscRegs)->PTCMD = (CSL_PSC_PTCMD_GO0_SET << CSL_PSC_PTCMD_GO0_SHIFT);

        /* Wait for power state transition to finish                          */
        count = Psc_TIMEOUT_COUNT;
        while (((((pscRegs)->PTSTAT
            & (CSL_PSC_PTSTAT_GOSTAT0_MASK)) >> CSL_PSC_PTSTAT_GOSTAT0_SHIFT)
            == CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION)
            && (count > 0))
        {
            count --;
        }

        if (0 == count)
        {
            /* Timeout has occured. hence indicate error                      */
            result = IOM_EBADIO;
            _restore_interrupts(hwiKey);
        }
        else
        {
            /* Wait for MODSTAT = ENABLE/DISABLE from LPSC                    */
            count = Psc_TIMEOUT_COUNT;
            while (((((pscRegs->MDSTAT[lpscNum]
                    & CSL_PSC_MDSTAT_STATE_MASK) >> CSL_PSC_MDSTAT_STATE_SHIFT))
                    != state) && (count > 0))
            {
                count --;
            }

            if (0 == count)
            {
                /* timeout has occured hence indicate an error                */
                result = IOM_EBADIO;
                _restore_interrupts(hwiKey);
            }
            else
            {
                if (TRUE == clkCtrl)
                {
                    /* increment the dependency count                         */
                    ++Psc_dependency[pscDevId][lpscNum];
                }
                else
                {
                    /* decrement the dependency count                         */
                    --Psc_dependency[pscDevId][lpscNum];
                }
                _restore_interrupts(hwiKey);
            }
        }
    }
    return (result);
}

/**
 * \brief    Returns the count of application instances that are still depend on
 *           the particular peripheral (no of applications that are yet to call
 *           lpsc power off for particular peripheral.
 *
 * \param    pscDevId  [IN]  PSC instance number
 * \param    moduleId  [IN]  LPSC number of the module to control
 *
 * \return   no of pending lpsc closures (dependencies) for a peripheral
 *
 */
Int32 Psc_getDependencyCount(Psc_DevId       pscDevId,
                             Uint32          moduleId)
{
    Int32 retVal = IOM_EBADIO;

    if ((pscDevId < CSL_LPSC_INSTANCE_MAX) && (moduleId < CSL_LPSC_NUMBER_MAX))
    {
        retVal = (Int32)Psc_dependency[pscDevId][moduleId];
    }
    return(retVal);
}


/**
 * \brief    Function to initialize the PSC data structures.
 *
 * \note     This is a one time initialisation function to be called only once
 *           during the lifetime of the application.
 *
 * \param    None
 * 
 * \return   None
 */
Void Psc_init(Void)
{
    /* we will perform an one time initialisation of the PSC data structures  *
     * clear the data structure to the default value of 0                     */
    memset(Psc_dependency,0x00,sizeof(Psc_dependency));
}

/* ========================================================================== */
/*                                END OF FILE                                 */
/* ========================================================================== */

