/*
 * Psc.h
 *
 * This file contains Application programming interface for the PSC driver and 
 * command/macro definitions used by the PSC driver.
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
 *  ======== ti/psp/psc/Psc.h ========
 */
#ifndef _PSC_H_
#define _PSC_H_

#include <std.h>
#ifdef __cplusplus
extern "C" 
{
#endif

/**
 *  \brief Psc device Id
 */
typedef enum Psc_DevId
{
    Psc_DevId_0,
    /**< Address Psc 0                                                        */
    Psc_DevId_1
    /**< Address Psc 1                                                        */
}Psc_DevId;

/* -------- constants -------- */
/**
 *  \brief  Wait time out for the device clock state transition
 */
#define Psc_TIMEOUT_COUNT (10240u)


/**
 *  \brief controls the clock domain for a given device
 *
 *  Use this function to control the clock domain for a particular device.
 *  The moduleId specifies the device that needs to have its clock
 *  controlled.The clkCtrl is either {@link  #moduleClkCtrl_DISABLE} or
 *  {@link #moduleClkCtrl_ENABLE}.
 *  The function raises an error in case that the state transition is not
 *  sucessful or else if any wrong parameters are passed.
 *
 *  \param pscDevId         Psc_DevId_0 or Psc_DevId_1
 *  \param moduleId         LPSC number of the module to control
 *  \param clkCtrl          if true clock is enabled; false if disabled
 *  \returns                void
 */
Int32 Psc_ModuleClkCtrl(Psc_DevId              pscDevId,
                        Uint32                 moduleId,
                        Bool                   clkCtrl);

/**
 *  \brief Returns the count of application instances that 
 *         are still depend on the particular peripheral (no of
 *         applications that are yet to call lpsc power off for
 *         particular peripheral.
 *
 *
 *  \param pscDevId         Psc_DevId_0 or Psc_DevId_1
 *  \param moduleId         LPSC number of the module to control
 *  \returns                no of pending lpsc closures (dependencies)
 *                          for a peripheral
 */

Int32 Psc_getDependencyCount(Psc_DevId       pscDevId,
                             Uint32          moduleId);

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
Void Psc_init(Void);


#ifdef __cplusplus
}
#endif
#endif /*_PSC_H_ */
