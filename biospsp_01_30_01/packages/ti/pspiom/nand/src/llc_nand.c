/*
 * llc_nand.c
 *
 * This file contains NAND low level controller functionality implementation.
 * NAND Driver provides Driver Layer Interface to do operations on the NAND
 * peripheral like device initialization, channel creation, control commands for
 * peripheral specific operations etc
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

/** \file      llc_nand.c
 *
 *  \brief     NAND device LLC module implementation.
 *
 *  This file contains all the interfaces required to operate on the NAND
 *  device. It provides function for erase, write, read and initialize. This
 *  file implements all the interfaces in the llc_nandAbstract.c file.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include <std.h>
#include <iom.h>
#include <tsk.h>
#include <assert.h>
#include <bcache.h>

#include "llc_nandAbstract.h"
#include "llc_nand.h"

#ifdef BIOS_PWRM_ENABLE
    #include <pwrm.h>
#endif

#include <ti/pspiom/psc/Psc.h>

#ifdef CHIP_C6747
    #include <ti/pspiom/cslr/soc_C6747.h>
#endif

#ifdef CHIP_OMAPL137
    #include <ti/pspiom/cslr/soc_OMAPL137.h>
#endif
#ifdef CHIP_C6748
    #include <ti/pspiom/cslr/soc_C6748.h>
#endif

#ifdef CHIP_OMAPL138
    #include <ti/pspiom/cslr/soc_OMAPL138.h>
#endif
/* ========================================================================== */
/*                          MACRO DEFINTIONS                                  */
/* ========================================================================== */

/**\brief NAND Media mapped at - When porting or EVM changes - ensure that this
          macro is updated with the base address of the memory segment at which
          NAND media is mapped */
#if defined(CHIP_C6747) || defined(CHIP_OMAPL137) || defined(CHIP_C6748) || defined(CHIP_OMAPL138)
#define LLC_NAND_MEMORY_MAPPED_AT     (CSL_EMIFA_CS3_ADDR)
#define LLC_NAND_ADDR_LATCH_ENABLE    (0x62000008u) /**< Address enable mask */
#define LLC_NAND_CHIP_ENABLE          (0x62000010u) /**< Chip enable mask */
#endif

/* ========================================================================== */
/*                            LOCAL VARIABLES                                 */
/* ========================================================================== */

/**\brief The LLC NAND device object instances. The LLC NAND object data struct.
          need not be initialized because the all the elements of the object are
          initialized during the initialization of the LLC module. */
static LLC_nandObj  llcNandObj[PSP_NAND_INSTANCES];

/**\brief The NAND device organization information lookup table */
static PSP_nandDeviceInfo nandDeviceInfoLut[] =
{

    { 0x00ECu, 0x0076u,  512u,  32u,  4096u, 16u, 0u  }, /*K9F1208B0B  */
    { 0x00ECu, 0x00D3u, 2048u, 128u,  4096u, 64u, 0u  }, /*K9xxG08UxM  */
    { 0x002Cu, 0x00CCu, 2048u,  64u,  4096u, 64u, 1u  }, /*MT29F1GxxABB*/
    { 0x002Cu, 0x00DCu, 2048u,  64u,  4096u, 64u, 0u  }, /* MT29F4G08AAA */
    { 0x0020u, 0x0075u,  512u,  32u,  2048u, 16u, 0u  }, /*NAND512W3A  */
    { 0x002Cu, 0x00DAu, 2048u,  64u,  2048u, 64u, 0u  }, /*MT29F2G08AAC*/
    { 0x0000u, 0x0000u,    0u,   0u,     0u,  0u, 0u  }
};

/**\brief The NAND device timing information lookup table */
static PSP_nandDeviceTiming nandDeviceTimingLut[] =
{
    { 0x00ECu, 0x0076u,  0u, 25u, 30u, 15u, 40u,  0u,  0u },
    { 0x00ECu, 0x00D3u,  0u, 15u, 20u, 10u, 35u,  0u,  0u },
    { 0x002Cu, 0x00CCu, 40u, 40u, 40u, 40u, 40u, 40u, 40u },
    { 0x002Cu, 0x00DCu,  0u, 30u, 30u, 20u, 40u,  0u,  0u }, /* MT29F4G08AAA */
    { 0x0020u, 0x0075u, 40u, 40u, 40u, 40u, 40u, 40u, 40u },
    { 0x002Cu, 0x00DAu, 40u, 40u, 40u, 40u, 40u, 40u, 40u },
    { 0x0000u, 0x0000u,  0u,  0u,  0u,  0u,  0u,  0u,  0u }
};

/**\brief Application provided NAND device info. The driver keeps a copy of the*
  *       application specified NAND device info in this structure.           */
static PSP_nandDeviceInfo  appNandDevInfo;


/** \brief  LLC NAND module implementation funciton pointer table
  *
  * This data structure stores the function pointers of the LLC NAND
  * driver. It suports both small page (512 byte) and big page (2048 byte)
  * devices of both SLC and MLC cell types.
  */
LLC_nandFxns  llcNandFxns =
{
    &LLC_nandInit,                  /**< LLC NAND initialization function    */
    &LLC_nandWritePage,             /**< LLC NAND page write function        */
    &LLC_nandReadPage,              /**< LLC NAND page read function         */
    &LLC_nandEraseBlock,            /**< LLC NAND erase block function       */
    &LLC_nandMarkBlockAsBad,        /**< LLC NAND mark block as bad function */
    &LLC_nandReadSpareArea,         /**< LLC NAND read spare area function   */
    &LLC_nandIoctl,                 /**< LLC NAND IOCTL function             */
    &LLC_nandDeinit,                /**< LLC NAND deinitialization function  */
    &LLC_nandIsWriteProtected,      /**< LLC NAND write protection status    */
    &LLC_nandIsBlockBad             /**< LLC NAND bad block check function   */
};

/**\brief Temporary storage for 4-bit ECC parity. Data area for the spare area
          read/write from the page. Used if the caller is not interstead with
          ECC Data.*/
Uint8             eccParity[LLC_NAND_MAX_ECC_PARITY_LENGTH];
/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Int32 LLC_nandWriteDataArea(LLC_nandObj  *hNandObj,
                                   Uint8        *data,
                                   Uint8        *parity);

static Void LLC_nandWriteSpareArea(LLC_nandObj        *hNandObj,
                                   LLC_nandSpareArea  *spareData);

static Int32 LLC_nandReadDataArea(LLC_nandObj  *hNandObj,
                                  Uint8        *data,
                                  Uint8        *parity,
                                  Bool          useEcc);

static Void LLC_nandECC4BitStart(LLC_nandObj *hNandObj, Bool toRead);

static Void LLC_nandECC4BitStop(LLC_nandObj *hNandObj);

static Int32 LLC_nandECC4BitCorrect(LLC_nandObj   *hNandObj,
                                    Uint8         *data,
                                    Uint8         *parity);

static Void LLC_nandECC4BitParityRead(LLC_nandObj *hNandObj, Uint8 *parity);

static PSP_nandDeviceInfo* LLC_nandLookupDeviceInfo(
                               LLC_nandObj         *hNandObj,
                               PSP_nandDeviceInfo  *nandDevInfoLut,
                               Uint32              nandId);

static PSP_nandDeviceTiming* LLC_nandLookupDeviceTiming(
                                 LLC_nandObj           *hNandObj,
                                 PSP_nandDeviceTiming  *nandDevTimingLut,
                                 Uint32                nandId);

static Uint32 LLC_nandReadId(LLC_nandObj  *hNandObj);

static Int32 LLC_nandWaitUntilReady(LLC_nandObj *hNandObj);

static Int32 LLC_nandGetStatus(LLC_nandObj *hNandObj);

static Void LLC_nandSendPageAddress(LLC_nandObj  *hNandObj,
                                    Uint32       rowAddress,
                                    Uint32       columnAddress,
                                    Bool         rowOnly);

static Void LLC_nandSetWaitTime(LLC_nandObj           *hNandObj,
                                PSP_nandDeviceTiming  *devTiming);

static Int32 LLC_nandResetDevice(LLC_nandObj *hNandObj);

static Void LLC_nandSetDataWidth(LLC_nandObj *hNandObj, Uint8 dataWidth);

static Void LLC_nandDelay(volatile Uint32 delay);

static Void LLC_nandDmaCallback(Uint32              lch,
                                EDMA3_RM_TccStatus  status,
                                Ptr                 data);

static Int32 LLC_nandEdmaRead(LLC_nandObj *hNandObj, Uint8 *data);

static Int32 LLC_nandEdmaWrite(LLC_nandObj *hNandObj, Uint8 *data);

static inline Void LLC_nandWriteAddress(LLC_nandObj *handle, Uint8 address);

static inline Void LLC_nandWriteCommand(LLC_nandObj *handle, Uint8 command);

static inline Uint16 LLC_nandReadData(LLC_nandObj *handle);

static inline Uint8 LLC_nandReadData8Bit(LLC_nandObj *handle);

static inline Void LLC_nandWriteData(LLC_nandObj *handle, Uint16 data);

static Int32 nandLpscOn(LLC_nandObj* const hNandObj);

static Int32 nandLpscOff(LLC_nandObj* const hNandObj);

#ifdef BIOS_PWRM_ENABLE
static Int32 nandRegisterNotification(LLC_nandObj* const hNandObj);

static PWRM_NotifyResponse nandNotifyCallback(PWRM_Event    eventType,
                                              Arg           eventArg1,
                                              Arg           eventArg2,
                                              Arg           clientArg);

static PWRM_NotifyResponse nandConfigureNewVfSettings(LLC_nandObj* const hNandObj,
                                                      Uns           eventArg1,
                                                      Uns           eventArg2);

static Int32 nandUnregisterNotification(LLC_nandObj* const hNandObj);
#endif

/* ========================================================================== */
/*                        NAND GLOBAL FUNCTION DEFINTIONS                     */
/* ========================================================================== */


/*============================================================================*/
/*                             PWRM FUNCTIONS                                 */
/*============================================================================*/

/**
 *  \brief    nandLpscOn
 *
 *   This function is invoked to power on the peripheral. Note that this
 *   function uses different functions to power ON the module, depending on
 *   the PWRM is enabled in this driver or NOT
 *
 *  \param    hNandObj    [IN]    handle to the nand object
 *
 *  \return   IOM_COMPLETED in case of success else an error code is returned.
 *
 *  \enter  hNandObj    must be a valid nand object
 *          The driver should be in opened state.
 *
 *  \leave  Not Implemented.
 */
static Int32 nandLpscOn(LLC_nandObj* const hNandObj)
{
    Int32 lpscOnStatus;
    assert(NULL != hNandObj);

#ifdef BIOS_PWRM_ENABLE
    /* power on using PWRM API*/
    lpscOnStatus = PWRM_setDependency((PWRM_Resource)hNandObj->pwrmLpscID);
#else
    /*power on using PSC API*/
    lpscOnStatus = Psc_ModuleClkCtrl((Psc_DevId)hNandObj->pscInstance,
                       (Uint32)hNandObj->pwrmLpscID,
                       TRUE);
#endif
    return lpscOnStatus;
}



/**
 *  \brief    nandLpscOff
 *
 *   This function is invoked to power off the peripheral
 *   handler and interrupt cause respectively.
 *
 *  \param    hNandObj    [IN]    handle to the nand object
 *
 *  \return   IOM_COMPLETED in case of success else an error code is returned.
 *
 *  \enter  hNandObj    must be a valid nand object
 *          The driver should be in opened state.
 *
 *  \leave  Not Implemented.
 */
static Int32 nandLpscOff(LLC_nandObj* const hNandObj)
{
    Int32 lpscOffStatus;
    assert(NULL != hNandObj);

#ifdef BIOS_PWRM_ENABLE
    /* power off  using PWRM API*/
    lpscOffStatus = PWRM_releaseDependency((PWRM_Resource)hNandObj->pwrmLpscID);
#else
     /*power off using PSC API*/
    lpscOffStatus = Psc_ModuleClkCtrl((Psc_DevId)hNandObj->pscInstance,
                        (Uint32)hNandObj->pwrmLpscID,
                        FALSE);
#endif
    return lpscOffStatus;
}


#ifdef BIOS_PWRM_ENABLE
/**
 * \brief     This function registers with the BIOS PWRM module for all the
 *            possible power management events.
 *
 * \param     hNandObj [IN] Handle to the driver instance object
 *
 * \return    IOM_COMPLETED in case of success
 *            IOM_EBADARGS  in case of error
 *
 * \enter     hNandObj is a NON null valid pointer
 *
 * \leave     Not implemented
 */
static Int32 nandRegisterNotification(LLC_nandObj* const hNandObj)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != hNandObj);

    /* register for the events to be notified by the driver                   */
    for (eventCnt = 0; eventCnt < LLC_NAND_MAX_PWRM_EVENTS; eventCnt++)
    {
        /* register the notify function for the PWRM event                    */
        retVal = PWRM_registerNotify(
                    (PWRM_Event)eventCnt,
                    NULL,
                    nandNotifyCallback,
                    (Arg)hNandObj,
                    &(hNandObj->pwrmInfo.notifyHandle[eventCnt]),
                    &(hNandObj->pwrmInfo.delayedCompletionFxn[eventCnt]));

        if (PWRM_SOK != retVal)
        {
            status = IOM_EBADARGS;
            break;
        }
    }
    return (status);
}

/**
 * \brief     This function unregisters all the power management events
 *            which were previously registered by the driver instance.
 *
 * \param     hNandObj [IN] Handle to the driver instance object
 *
 * \return    IOM_COMPLETED in case of success
 *            IOM_EBADARGS  in case of error
 *
 * \enter     hNandObj is a NON null valid pointer
 *
 * \leave     Not implemented
 */
static Int32 nandUnregisterNotification(LLC_nandObj* const hNandObj)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != hNandObj);

    for (eventCnt = 0; eventCnt < LLC_NAND_MAX_PWRM_EVENTS; eventCnt++)
    {
        /* register the notify function for the PWRM event                    */
        if (NULL != (hNandObj->pwrmInfo.notifyHandle[eventCnt]))
        {
            retVal = PWRM_unregisterNotify(
                        (hNandObj->pwrmInfo.notifyHandle[eventCnt]));

            if (PWRM_SOK != retVal)
            {
                status = IOM_EBADARGS;
                break;
            }
            else
            {
                hNandObj->pwrmInfo.notifyHandle[eventCnt] = 0x00;
            }
        }
    }
    return (status);
}

/**
 * \brief     This function is the notify function called by the BIOS whenever
 *            a power event occurs in the system.This function handles the
 *            power events and calls the appropriate functions to process the
 *            same.
 *
 * \param     eventType [IN] power event type
 * \param     eventArg1 [IN] event-specific argument
 * \param     eventArg2 [IN] event-specific argument
 * \param     clientArg [IN] argument passed by the PWRM module(usually
 *                          (supplied during the registration of the notifyFxn)
 *
 * \return    IOM_COMPLETED if success else error code
 *
 * \enter     eventType is a valid event
 *
 * \leave     Not implemented
 */
static PWRM_NotifyResponse nandNotifyCallback(PWRM_Event    eventType,
                                              Arg           eventArg1,
                                              Arg           eventArg2,
                                              Arg           clientArg)
{
    LLC_nandObj          *hNandObj = NULL;
    PWRM_NotifyResponse  pwrmStatus = PWRM_NOTIFYDONE;

    assert(NULL != clientArg);
    hNandObj = (LLC_nandObj *)clientArg;
    assert(NULL != hNandObj);

    /* check the event notified by the PWRM module and process accordingly    */
    switch (eventType)
    {
        case PWRM_GOINGTOSLEEP:
        case PWRM_GOINGTOSTANDBY:
        case PWRM_GOINGTODEEPSLEEP:
        /* The nand driver shall support PWRM events only when channel is  *
         * disabled and no packets are pending in driver.                   *
         * Hence, there nothing much to be done here but to check submit    *
         * count and then returning the status                              */
            if (0 != hNandObj->submitCount)
            {
                pwrmStatus = PWRM_NOTIFYERROR;
            }
            else
            {
                hNandObj->isSuspended = TRUE;
            }
            break;

        case PWRM_PENDING_CPU_SETPOINTCHANGE:
        case PWRM_PENDING_PER_SETPOINTCHANGE:
        /* The nand driver shall support PWRM events only when channel is  *
         * disabled and no packets are pending in driver.                   *
         * Hence, there nothing much to be done here but to check submit    *
         * count and then returning the status                              */
            if (((PWRM_PENDING_CPU_SETPOINTCHANGE == eventType) &&
                 (PSP_NAND_PLL_DOMAIN_0 == hNandObj->pllDomain)) ||
                ((PWRM_PENDING_PER_SETPOINTCHANGE == eventType) &&
                 (PSP_NAND_PLL_DOMAIN_1 == hNandObj->pllDomain)))
            {
                if (0 != hNandObj->submitCount)
                {
                    pwrmStatus = PWRM_NOTIFYERROR;
                }
                else
                {
                    hNandObj->isSuspended = TRUE;
                }
            }
            break;

        case PWRM_DONE_CPU_SETPOINTCHANGE:
        case PWRM_DONE_PER_SETPOINTCHANGE:
             /* The driver shall now reconfigure the clock settings based     *
              * on the new set point of the system                            */
            if (((PWRM_DONE_CPU_SETPOINTCHANGE == eventType) &&
                 (PSP_NAND_PLL_DOMAIN_0 == hNandObj->pllDomain)) ||
               ((PWRM_DONE_PER_SETPOINTCHANGE == eventType) &&
                (PSP_NAND_PLL_DOMAIN_1 == hNandObj->pllDomain)))
            {
                pwrmStatus = nandConfigureNewVfSettings(hNandObj,
                                                        eventArg1,
                                                        eventArg2);
                hNandObj->isSuspended = FALSE;
            }
            break;

        case PWRM_AWAKEFROMSLEEP:
        case PWRM_AWAKEFROMDEEPSLEEP:
        case PWRM_AWAKEFROMSTANDBY:
            /* There is nothing for the nand driver to be done here. Just   *
             * check the channel status. The nand will be enabled, during   *
             * first IOP submit                                              */
            if (0 != hNandObj->submitCount)
            {
                pwrmStatus = PWRM_NOTIFYERROR;
            }
            else
            {
                hNandObj->isSuspended = FALSE;
            }
            break;

        default:
            break;
    }
    return (pwrmStatus);
}

/**
 * \brief      This function calculates clock settings for the new DVFS set
 *             point.
 *
 * \param      hNandObj [IN] Handle to the device instance object
 * \param      eventArg1  [IN] previous set point.
 * \param      eventArg2  [IN] current set point
 *
 * \return     PWRM_SOK incase that the prescaler values for the new set point
 *             are calculated successfully else an appropriate error code is
 *             returned
 *
 * \enter      hNandObj is a valid non NULL pointer
 *
 * \leave      Not imeplemented
 */
static PWRM_NotifyResponse nandConfigureNewVfSettings(LLC_nandObj* const hNandObj,
                                                      Uns           eventArg1,
                                                      Uns           eventArg2)
{
    PWRM_NotifyResponse  retVal = PWRM_NOTIFYDONE;
    Int32                status = IOM_COMPLETED;
    PWRM_Status          pwrmStat = PWRM_SOK;
    PWRM_Domain          domain = PWRM_CPU;
    Uns                  frequency = 0x00;
    Uns                  voltage = 0x00;
    Uint32               sysclkDiv = 0x00;
    PSP_nandDeviceTiming  *nandDevTiming;  /* NAND device timing          */
    Uint32                deviceId;        /* 4-byte device ID            */

    /* This function will calculate the settings for the new set point and    *
     * also indicate if this is a valid setpoint                              */
    assert(NULL != hNandObj);

    /* check if both the set points are not the same                          */
    if (eventArg1 != eventArg2)
    {
        /* The DVFS set point change has happened successfully.calculate the  *
         * new divider values                                                 */

        /* check what clock domain is supported by the device                 */
        if (PSP_NAND_PLL_DOMAIN_0 != hNandObj->pllDomain)
        {
            domain = PWRM_PER;
        }

        /* calculate the sysclk divisor value                                 */
        pwrmStat = PWRM_getSetpointInfo(domain,
                       eventArg1,
                       &frequency,
                       &voltage);

        if (PWRM_SOK == pwrmStat)
        {
            /* calculate the sysclk divisor value. This should not be constant */
            sysclkDiv = 3;

            /* get the information about the current set point                */
            pwrmStat = PWRM_getSetpointInfo(domain,
                           eventArg2,
                           &frequency,
                           &voltage);

            if (PWRM_SOK == pwrmStat)
            {
                hNandObj->clkFreq = (frequency * 1000) / sysclkDiv;
            }
        }

        /* If the driver is power aware enable the PSC */
        if ((IOM_COMPLETED == status) && (TRUE == hNandObj->isPwrAware))
        {
            status = nandLpscOn(hNandObj);
        }
        deviceId = ((hNandObj->nandDevInfo->deviceId << 8) |
                    hNandObj->nandDevInfo->vendorId);
        if ((IOM_COMPLETED == status) && (PWRM_SOK == pwrmStat))
        {
            /* Application has not specified timing. So do a lookup.      */
            nandDevTiming = LLC_nandLookupDeviceTiming(
                                (LLC_nandObj*)hNandObj,
                                &nandDeviceTimingLut[0],
                                deviceId);

            /* Determine if NAND device timing data is found              */
            if (NULL == nandDevTiming)
            {
                /* The NAND device timing data was not found in the       *
                 * lookup table. So terminate in the initialization.      *
                 * NOTE: If a NAND device timing data is not found in the *
                 * lookup table, then should the driver configure the     *
                 * worst-case timing data and continue?                   */
                status = IOM_EBADARGS;
            }

            LLC_nandSetWaitTime((LLC_nandObj*)hNandObj, nandDevTiming);
        }
        else
        {
            status = IOM_EBADARGS;
        }

        /* If the driver is power aware disable the PSC */
        if ((IOM_COMPLETED == status) && (TRUE == hNandObj->isPwrAware))
        {
            status = nandLpscOff(hNandObj);
        }
    }

    if (IOM_COMPLETED != status)
    {
        retVal = PWRM_NOTIFYERROR;
    }

    return (retVal);
}

#endif

/*============================================================================*/
/*                             PWRM FUNCTIONS END                             */
/*============================================================================*/



/**
 * \brief Initializes the LLC NAND device layer
 *
 *        This function initializes the NAND LLC layer. It detects the NAND
 *        device ID or uses a supplied ID and configures the EMIF registers.
 *        It also sets up the NAND device timing values in the EMIF registers.
 */
Ptr LLC_nandInit(LLC_nandInitConfig  const *initCfg,
                 PSP_nandDeviceInfo       **devInfo)
{
    LLC_nandObj           *hNandObj;       /* Handle of LLC_nandObj       */
    Uint32                maxPages;        /* Total number of pages       */
    Uint32                deviceId;        /* 4-byte device ID            */
    PSP_nandDeviceTiming  *nandDevTiming;  /* NAND device timing          */
    Int32                 result;          /* Result of initializaiton    */
    Int32                 pwrResult;       /* Result of power APIs        */
    Bool                  pscPwrOn = FALSE;/* Pwr off or not              */
    SEM_Attrs             semAttr;         /* Semaphore attributes        */
    EDMA3_DRV_Result      edmaResult;      /* EDMA result                 */
    volatile Uint32       dummyEMIFRead;   /* Temporary variable          */
    Bool                  flag = TRUE;

    /* Initialize the local variables */
    result   = IOM_EBADARGS;
    hNandObj = NULL;

    do
    {
        /* Validate the input parameters                                      */
        assert((NULL != initCfg) && (NULL != devInfo));
        flag = FALSE;

        /* Get the handle of the LLC NAND object                              */
        hNandObj = (LLC_nandObj*)&llcNandObj[initCfg->instanceId];

        /* Validate the instance ID suppied as parameter                      */
        if (initCfg->instanceId >= PSP_NAND_INSTANCES)
        {
            break;
        }

        /* Keep a copy of the input parameters                                */
        hNandObj->instanceId  = initCfg->instanceId;
        hNandObj->clkFreq     = initCfg->clkFreq;
        hNandObj->hEdma       = initCfg->hEdma;
        hNandObj->edmaEvtQ    = initCfg->edmaEvtQ;
        hNandObj->isPwrAware  = initCfg->isPwrAware;
		hNandObj->submitCount = 0;
        hNandObj->pllDomain   = initCfg->pllDomain;
        if (0 == initCfg->instanceId)
        {
            /* Initialize other elements                                      */
            hNandObj->regs = (CSL_EmifaRegsOvly)CSL_EMIFA_0_REGS;
            hNandObj->pscInstance = (Uint32)CSL_LPSC_INST_EMIFA_0;
            #ifndef BIOS_PWRM_ENABLE
                hNandObj->pwrmLpscID = (Uint32)CSL_PSC_EMIFA;
            #else
                hNandObj->pwrmLpscID = (Uint32)CSL_LPSC_NUMBER_EMIFA_0;
            #endif
        }

        /* If the driver is power aware enable the PSC */
        if (TRUE == hNandObj->isPwrAware)
        {
            pwrResult = nandLpscOn(hNandObj);
            if (IOM_COMPLETED != pwrResult)
            {
                result = pwrResult;
                break;
            }
            else
            {
                pscPwrOn = TRUE;
            }
        }
        else
        {
            /*
             * The first thing to do when power management is not desired is to
             * turn on the controller using the PSC or PWRM API
             */
            pwrResult = nandLpscOn(hNandObj);
            if (IOM_COMPLETED != pwrResult)
            {
                result = pwrResult;
                break;
            }
        }

        /* Initialize the NAND flash control register                         */
        hNandObj->regs->NANDFCR = CSL_EMIFA_NANDFCR_RESETVAL;

        switch (initCfg->instanceId)
        {
            /* For instance ID 0, the following are the NAND addresses        */
            case 0:  /* CS3 */
                hNandObj->CExDATA = (Uint32)LLC_NAND_MEMORY_MAPPED_AT;
                hNandObj->CExALE  = (Uint32)LLC_NAND_ADDR_LATCH_ENABLE;
                hNandObj->CExCLE  = (Uint32)LLC_NAND_CHIP_ENABLE;

                /* NAND Flash Control Register (NANDFCR)                      */
                /* Set Bits to 1 if using NAND Flash on CSxNAND; mapped to    *
                 * '/EM_CS[x]'.                                               */
                hNandObj->regs->NANDFCR = \
                    CSL_FMK(EMIFA_NANDFCR_CS3NAND,
                            CSL_EMIFA_NANDFCR_CS3NAND_NAND_ENABLE);
                break;

            default:
                break;
        }

        /* Asynchronous wait cycle Configuration Register                     */
        hNandObj->regs->AWCC |=
            (Uint32) (CSL_FMK(EMIFA_AWCC_WP0, CSL_EMIFA_AWCC_WP0_WAIT_ONHIGH));

        /* If the NAND device configuration is not provided, then device ID   *
         * has to be read to determine the device configuration.              */
        if (NULL == initCfg->deviceInfo)
        {
            /* NAND device configuration is not provided. Before reading the
               device ID, set the asynchronus timing to worst case, set the
               data width and reset the NAND device. */
            LLC_nandSetWaitTime((LLC_nandObj*)hNandObj, NULL);
            LLC_nandSetDataWidth((LLC_nandObj*)hNandObj, LLC_NAND_BW_8BIT);
            result = LLC_nandResetDevice((LLC_nandObj*)hNandObj);
            if (result != IOM_COMPLETED)
            {
                /* The reset of the NAND device did not complete */
                result = PSP_NAND_E_ERROR;
                break;
            }

            /* Read the NAND device ID                                        */
            deviceId = LLC_nandReadId((LLC_nandObj*)hNandObj);

            /* Determine if a NAND device has been found                      */
            if (0x00 ==  deviceId)
            {
                /* No NAND device has been found                              */
                result = PSP_NAND_E_ERROR;
                break;
            }

            /* Lookup the table and determibe the device configuration        */
            hNandObj->nandDevInfo = LLC_nandLookupDeviceInfo(
                                        (LLC_nandObj*)hNandObj,
                                        &nandDeviceInfoLut[0],
                                        deviceId);
            *devInfo = hNandObj->nandDevInfo;

            /* Determine if NAND device organization data is found            */
            if (NULL == hNandObj->nandDevInfo)
            {
                /* NAND device organization info not found in lookup table    */
                result = PSP_NAND_E_ERROR;
                break;
            }

            /* NAND device organization has been found. Configure the EMIF
               registers according to the data width of the device. */
            LLC_nandSetDataWidth((LLC_nandObj*)hNandObj,
                                  hNandObj->nandDevInfo->dataBusWidth);

            /* Determine if application has preference of timing              */
            if (NULL == initCfg->deviceTiming)
            {
                /* Application has not specified timing. So do a lookup.      */
                nandDevTiming = LLC_nandLookupDeviceTiming(
                                    (LLC_nandObj*)hNandObj,
                                    &nandDeviceTimingLut[0],
                                    deviceId);

                /* Determine if NAND device timing data is found              */
                if (NULL == nandDevTiming)
                {
                    /* The NAND device timing data was not found in the       *
                     * lookup table. So terminate in the initialization.      *
                     * NOTE: If a NAND device timing data is not found in the *
                     * lookup table, then should the driver configure the     *
                     * worst-case timing data and continue?                   */
                    result = PSP_NAND_E_ERROR;
                    break;
                }
            }
            else
            {
                /* Use the application specified timing                       */
                nandDevTiming = initCfg->deviceTiming;
            }

            LLC_nandSetWaitTime((LLC_nandObj*)hNandObj, nandDevTiming);
        }
        else
        {
            /* Application has provided the device configuration. Keep a      *
             * copy of this information.                                      */
            memcpy(
                (Ptr)&appNandDevInfo,
                (Ptr)initCfg->deviceInfo,
                sizeof(PSP_nandDeviceInfo));

            hNandObj->nandDevInfo = (PSP_nandDeviceInfo*)&appNandDevInfo;
            *devInfo = hNandObj->nandDevInfo;

            /* Configure timing using application specified values */
            if (NULL == initCfg->deviceTiming)
            {
                /* Application has not supplied the device timing info. So    *
                 * terminate the initialization.                              *
                 * NOTE: If a NAND device timing data is not supplied by the  *
                 * application, then should the driver configure the          *
                 * worst-case timing data and continue?                       */
                result = PSP_NAND_E_ERROR;
                break;
            }
            LLC_nandSetWaitTime((LLC_nandObj*)hNandObj, initCfg->deviceTiming);

            /* Initialize the data width                                      */
            LLC_nandSetDataWidth((LLC_nandObj*)hNandObj,
                hNandObj->nandDevInfo->dataBusWidth);

            /* Reset the NAND device */
            result = LLC_nandResetDevice((LLC_nandObj*)hNandObj);
            if (result != IOM_COMPLETED)
            {
                /* The reset of the NAND device did not complete              */
                result = PSP_NAND_E_ERROR;
                break;
            }

            /* Read the device ID of the NAND device */
            deviceId = LLC_nandReadId(hNandObj);

            /* Ensure that the device ID read from the NAND device is same
               as the application specified NAND device ID. */
            if (((Uint8)(deviceId & 0xFFu) !=
                     (Uint8)(initCfg->deviceInfo->vendorId)) ||
                 ((Uint8)((deviceId >> 8u) & 0xFFu) !=
                     (Uint8)(initCfg->deviceInfo->deviceId)))
            {
                result = IOM_EBADARGS;
                break;
            }
        }
        /* Compute the number of address cycles (consider only the row address*
         * portion of the address cycle. The last addressable page should be  *
         * considered when calculating the maximum number of pages.           */
        hNandObj->addressCycles = 0;
        maxPages = (hNandObj->nandDevInfo->numBlocks *
                    hNandObj->nandDevInfo->pagesPerBlock) - 1u;

        while (maxPages > 0)
        {
            hNandObj->addressCycles++;
            maxPages = maxPages >> 8u;
        }

        /* If hEdma is not null, it means EDMA mode of data transfer has been *
         * selected. In that case, acquire a EDMA channel.                    */
        if (NULL != hNandObj->hEdma)
        {
            /* Acquire a EDMA channel for the data transfer                   */
            hNandObj->edmaTcc     = EDMA3_DRV_TCC_ANY;
            hNandObj->edmaChannel = initCfg->edmaChannel;
            edmaResult = EDMA3_DRV_requestChannel(
                             (Ptr)hNandObj->hEdma,
                             (Uint32*)&hNandObj->edmaChannel,
                             &hNandObj->edmaTcc,
                             hNandObj->edmaEvtQ,
                             &LLC_nandDmaCallback,
                             (Ptr)hNandObj);

            result = (edmaResult == EDMA3_DRV_SOK) ? IOM_COMPLETED : IOM_EALLOC;
            if (result != IOM_COMPLETED)
            {
                break;
            }

            /* Acquire a semaphore for EDMA synchornization                   */
            semAttr.name = (String)"Nand_EdmaSync_Sema";
            hNandObj->syncSem = SEM_create(0U, &semAttr);
            result = (hNandObj->syncSem != NULL) ? IOM_COMPLETED : IOM_EALLOC;
            if (result != IOM_COMPLETED)
            {
                break;
            }
        }

        /* Initialize is complete                                             */
        result = IOM_COMPLETED;
    }while (flag);

    /* If the driver is power aware disable the PSC */
    if ((TRUE == hNandObj->isPwrAware) && (TRUE == pscPwrOn))
    {
        pwrResult = nandLpscOff(hNandObj);
        if (IOM_COMPLETED != pwrResult)
        {
            result = pwrResult;
        }
    }

#ifdef BIOS_PWRM_ENABLE
    memset(&hNandObj->pwrmInfo, 0x00, sizeof(LLC_nandPwrmInfo));
    hNandObj->isSuspended = FALSE;
    if ((IOM_COMPLETED == result) && (TRUE == hNandObj->isPwrAware))
    {
        /* register the notify function for the PWRM events               */
        result = nandRegisterNotification(hNandObj);
    }
#endif

    /* If NAND LLC initialization is not complete,then set the handle as NULL.*/
    if (result != IOM_COMPLETED)
    {
        /* NAND device LLC module initialization is not complete              */
        hNandObj = NULL;
    }

    /* Return the result of initializing                                      */
    return ((Ptr)hNandObj);
}


/**
 * \brief Writes a page to the NAND device
 *
 *        This function writes the data specified to a NAND page. The physical
 *        block number and the physical page number in that block should be
 *        specified. The logical block number of the logical page number should
 *        also be supplied since this information is written the spare area of
 *        the page.
 */
Int32 LLC_nandWritePage(Ptr  const         handle,
                        Uint32             phyBlock,
                        Uint8              phyPage,
                        Uint8              *data,
                        LLC_nandSpareArea  *spareArea,
                        Bool               computeEcc)

{
    LLC_nandObj       *hNandObj;   /* Handle of the LLC NAND object          */
    Uint32            temp;        /* Temorary linear page number            */
    Int32             result;      /* Result of writting the page            */
    Bool              flag = TRUE;
    Int32             pwrResult;       /* Result of power APIs        */
    Bool              pscPwrOn = FALSE;/* Pwr off or not              */

    do
    {
        /* Validate the input parameters                                      */
        assert(((NULL != handle) && (NULL != data)) && (NULL != spareArea));

        /* Initialize the required local variables                            */
        hNandObj = (LLC_nandObj*)handle;
        flag = FALSE;

        /* Check if the caller dosent was to compute ECC, in which case caller
            should have provided the ECC data */
        if (FALSE == computeEcc)
        {
            if (NULL == spareArea->eccData)
            {
                result = IOM_EBADARGS;
                break;
            }
        }

        /* If the driver is power aware enable the PSC */
        if (TRUE == hNandObj->isPwrAware)
        {
            pwrResult = nandLpscOn(hNandObj);
            if (IOM_COMPLETED != pwrResult)
            {
                result = pwrResult;
                break;
            }
            else
            {
                pscPwrOn = TRUE;
            }
        }

        /* Write the page program command. The LLC_NAND_CMD_READ command is
           issued first so that the "pointer" is moved to the start of the page.
           If the previous operation was to read the spare area, then the
           "pointer" would be pointing to the spare area. Note: This is not
           mandatory in big page device. */
        LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_READ);
        LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_PROGRAM);

        /* Write the physical address of the page                             */
        temp = (phyBlock * hNandObj->nandDevInfo->pagesPerBlock) + phyPage;
        LLC_nandSendPageAddress(hNandObj, temp, 0x00U, FALSE);

        /* Write the data area of the page                                    */
        result = LLC_nandWriteDataArea(hNandObj, data, &eccParity[0]);
        if (result != IOM_COMPLETED)
        {
            /* The write to data area has failed. So Reset the NAND device
               and return an error code. The reset is required becuase
               the NAND has not completed the write operation. */
            LLC_nandResetDevice(hNandObj);
            break;
        }

        /* Write the spare area of the page */
        spareArea->badBlockMark = FALSE;
        /* If ECC is to be computed, use the computed value */
        if (TRUE == computeEcc)
        {
            spareArea->eccData      = eccParity;
        }

        /* Otherwise use the suplied values */
        LLC_nandWriteSpareArea(hNandObj, spareArea);

        /* Issue the page program confirm command                             */
        LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_PROGRAM_CYC2);

        /* Wait until the NAND is ready                                       */
        result = LLC_nandWaitUntilReady(hNandObj);

        if (result != IOM_COMPLETED)
        {
            /* NAND device page write a timed-out                             */
            break;
        }

        /* Get the status of the page program operation                       */
        result = LLC_nandGetStatus(hNandObj);

        if (LLC_NAND_E_STATUS_FAIL == result)
        {
            /* Set the result of write failed                                 */
            result = PSP_NAND_E_WRITE_FAIL;
            break;
        }
    }while (flag);

    /* If the driver is power aware disable the PSC */
    if ((TRUE == hNandObj->isPwrAware) && (TRUE == pscPwrOn))
    {
        pwrResult = nandLpscOff(hNandObj);
        if (IOM_COMPLETED != pwrResult)
        {
            result = pwrResult;
        }
    }

    /* Return the result of writing the page                                  */
    return (result);
}


/**
 * \brief Reads a page to the NAND device
 *
 *        This function reads the data specified from a NAND page. The physical
 *        block number and the physical page number in that block should be
 *        specified.
 */
Int32 LLC_nandReadPage(Ptr const         handle,
                       Uint32            phyBlock,
                       Uint8             phyPage,
                       Uint8             *data,
                       LLC_nandSpareArea *spareData,
                       Bool              useEcc)
{
    Uint32            temp;             /* Temporary linear page address    */
    Int32             result;           /* Result of reading from the page  */
    LLC_nandObj       *hNandObj;        /* LLC nand device object pointer   */
    Uint8             readRetry;        /* Number of times read is retried  */
    volatile Uint32   dummyEMIFRead;    /* Temporary variable               */
    Bool              flag = TRUE;
    Int32             pwrResult;       /* Result of power APIs        */
    Bool              pscPwrOn = FALSE;/* Pwr off or not              */

    do
    {
        /* Validate the input parameters                                      */
        assert(((NULL != handle) && (NULL != data)) && (NULL != spareData));

        /* Initialize the required local variables                            */
        flag = FALSE;
        hNandObj = (LLC_nandObj*)handle;
        readRetry = 0u;

        /* Calculate the page address                                         */
        temp = (phyBlock * hNandObj->nandDevInfo->pagesPerBlock) + phyPage;

        /* If the driver is power aware enable the PSC */
        if (TRUE == hNandObj->isPwrAware)
        {
            pwrResult = nandLpscOn(hNandObj);
            if (IOM_COMPLETED != pwrResult)
            {
                result = pwrResult;
                break;
            }
            else
            {
                pscPwrOn = TRUE;
            }
        }

        do
        {
            /* First read the spare area of the page                          */
            /* Check if the caller is interstead with ECC Data, is so use
               the data space provided by the caller */
            if (NULL == spareData->eccData)
            {
                spareData->eccData = eccParity;
            }
            result = LLC_nandReadSpareArea((Ptr)hNandObj,
                         phyBlock,
                         phyPage,
                         spareData);
            if (result != IOM_COMPLETED)
            {
                /* Reading the spare area timed-out                           */
                break;
            }

            /* Write the page read command                                    */
            LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_READ);

            /* Write the physical address of the page                         */
            LLC_nandSendPageAddress(hNandObj, temp, 0x00U, FALSE);

            /* If this is big page device, send the second cycle read command */
            if (LLC_NAND_BIG_PAGE_SIZE == hNandObj->nandDevInfo->pageSize)
            {
                LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_READ_CYC2);
            }

            /* Wait until the NAND is ready                                   */
            result = LLC_nandWaitUntilReady(hNandObj);
            if (result != IOM_COMPLETED)
            {
                /* Read the page contents timed-out                           */
                break;
            }

            /* Read the data area of the page                                 */
            result = LLC_nandReadDataArea(hNandObj,
                         data,
                         spareData->eccData,
                         useEcc);

            /* Increment the read retry attempts                              */
            readRetry = readRetry + 1u;
        }
        while ((result != IOM_COMPLETED) && (readRetry < LLC_NAND_READ_RETRY));
    }while (flag);

    /* If the driver is power aware disable the PSC */
    if ((TRUE == hNandObj->isPwrAware) && (TRUE == pscPwrOn))
    {
        pwrResult = nandLpscOff(hNandObj);
        if (IOM_COMPLETED != pwrResult)
        {
            result = pwrResult;
        }
    }

    /* Return the result of reading the page                                  */
    return (result);
}


/**
 * \brief Erase a block in NAND device
 *
 *        This function erases a block in the NAND device. The block number to
 *        be erased has to be specified as parameter.
 */
Int32 LLC_nandEraseBlock(Ptr const handle, Uint32 blockNum)
{
    LLC_nandObj   *hNandObj;  /* Handle of the LLC nand object      */
    Int32         result;     /* Result of marking the block as bad */
    Uint32        pageNum;    /* First page number of the block     */
    Bool          flag = TRUE;
    Int32         pwrResult;       /* Result of power APIs        */
    Bool          pscPwrOn = FALSE;/* Pwr off or not              */

    do
    {
        /* Validate the input parameters */
        assert(NULL != handle);

        /* Initialize the local variables                                     */
        hNandObj = (LLC_nandObj*)handle;
        flag = FALSE;

        /* If the driver is power aware enable the PSC */
        if (TRUE == hNandObj->isPwrAware)
        {
            pwrResult = nandLpscOn(hNandObj);
            if (IOM_COMPLETED != pwrResult)
            {
                result = pwrResult;
                break;
            }
            else
            {
                pscPwrOn = TRUE;
            }
        }

        /* Determine if the block is bad. The block should not be erased if   *
         * the block is bad.                                                  */
        result = LLC_nandIsBlockBad((Ptr)hNandObj, blockNum);
        if (result != IOM_COMPLETED)
        {
            /* Block is bad or some other error has occured when checking if  *
             * the block is bad. Cannot continue so return.                   */
            break;
        }

        /* This is a good block and it can be erased. The following is the    *
         * procedure for erasing the block.                                   *
         *                                                                    *
         * A. Calculate linear page number of the first page of the block.    *
         * B. Send the block erase command.                                   *
         * C. Send the first page address of the block.                       *
         * D. Send the block erase confirm command.                           *
         * E. Wait until the erase is complete and then check the status.     */

        pageNum = blockNum * hNandObj->nandDevInfo->pagesPerBlock;
        LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_ERASE);
        LLC_nandSendPageAddress(hNandObj, pageNum, 0x00U, TRUE);
        LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_ERASE_CYC2);

        /* Wait until the NAND device is in ready state                       */
        result = LLC_nandWaitUntilReady(hNandObj);
        if (PSP_E_TIMEOUT == result)
        {
            /* NAND erase operation timed-out                                 */
            break;
        }

        /* Erase operation is complete. Read the status of the erase operation*
         * and determine if the erase succedded                               */
        result = LLC_nandGetStatus(hNandObj);

        if (PSP_NAND_E_WRITE_PROTECTED == result)
        {
            /* NAND device is write protected. So erase has failed.           */
            break;
        }

        if (LLC_NAND_E_STATUS_FAIL == result)
        {
            /* NAND erase operation has failed                                */
            result = PSP_NAND_E_ERASE_FAIL;
            break;
        }
    }while (flag);

    /* If the driver is power aware disable the PSC */
    if ((TRUE == hNandObj->isPwrAware) && (TRUE == pscPwrOn))
    {
        pwrResult = nandLpscOff(hNandObj);
        if (IOM_COMPLETED != pwrResult)
        {
            result = pwrResult;
        }
    }

    /* Return the status of erasing the block                                 */
    return (result);
}


/**
 * \brief Marks the block as bad block
 *
 *        This function marks the block as bad. It writes a bad block indicator
 *        value (non 0xFF value) to the location of the bad block indicator
 *        location. It writes the bad block indicator values to 1st, 2nd and
 *        last page of the device.
 */
Int32 LLC_nandMarkBlockAsBad(Ptr const handle, Uint32 blockNum)
{
    Uint32            pagesPerBlock;  /* Number of pages per block            */
    Uint8             count;          /* Count of number of pages to be marked*/
    Uint32            pageNum;        /* Page number to be marked             */
    Int32             result;         /* Result of marking the block as bad   */
    LLC_nandObj       *hNandObj;      /* LLC nand device object pointer       */
    Uint8             markCount;      /* Number of pages successfully marked  */
    LLC_nandSpareArea spareData;      /* Spare Area information               */
    Bool              flag = TRUE;
    Int32             pwrResult;       /* Result of power APIs        */
    Bool              pscPwrOn = FALSE;/* Pwr off or not              */

    do
    {
        /* Initialize the local variables */
        flag = FALSE;
        markCount = 0;
        result    = IOM_EBADARGS;
        pageNum   = 0u;

        /* Validate the input parameters */
        assert(NULL != handle);

        /* Initialize the local variables after validating parameters */
        hNandObj  = (LLC_nandObj*)handle;

        /* Keep a copy of the number of pages per block                       */
        pagesPerBlock = hNandObj->nandDevInfo->pagesPerBlock;

        /* If the driver is power aware enable the PSC */
        if (TRUE == hNandObj->isPwrAware)
        {
            pwrResult = nandLpscOn(hNandObj);
            if (IOM_COMPLETED != pwrResult)
            {
                result = pwrResult;
                break;
            }
            else
            {
                pscPwrOn = TRUE;
            }
        }

        /* First erase the block before marking it as bad. The result of      *
         * marking this as bad can be ignored.                                */
        LLC_nandEraseBlock(handle, blockNum);

        /* Write the bad block indicator value in the spare area of 1st, 2nd  *
         * and last page of the block                                         */
        for (count = 0; count < 3u; count++)
        {
            switch (count)
            {
                /* First page number of the block                             */
                case 0:
                    pageNum = blockNum * pagesPerBlock;
                    break;

                /* Second page number of the block                            */
                case 1:
                    pageNum = (blockNum * pagesPerBlock) + 1u;
                    break;

                /* Last page number of the block                              */
                case 2:
                    pageNum = (blockNum * pagesPerBlock) + (pagesPerBlock - 1u);
                    break;

                /* Nothing to do in default case                              */
                default:
                    break;
            }

            /* Issue the page write command                                   */
            if (LLC_NAND_SMALL_PAGE_SIZE == hNandObj->nandDevInfo->pageSize)
            {
                /* For small-page device, first set the read pointer to       *
                 * the spare area location                                    */
                LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_READ2);

                /* Issue the page write command and page address */
                LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_PROGRAM);
                LLC_nandSendPageAddress(hNandObj, pageNum, 0x00u, FALSE);
            }
            else
            {
                /* For big-page device, send the write page command and the   *
                 * page address                                               */
                LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_PROGRAM);
                LLC_nandSendPageAddress(hNandObj, pageNum, 2048u, FALSE);
            }

            /* Write the contents of the spare area                           */
            spareData.logicalBlock = 0xFFFFFFu;
            spareData.logicalPage  = 0xFFu;
            spareData.badBlockMark = TRUE;
            spareData.eccData      = NULL;
            LLC_nandWriteSpareArea(hNandObj, &spareData);

            /* Write the program confirm command                              */
            LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_PROGRAM_CYC2);

            /* Wait until NAND device is ready after the program operation    */
            result = LLC_nandWaitUntilReady(hNandObj);
            if (IOM_COMPLETED == result)
            {
                /* Keep a count of how many pages have been marked correctly  */
                markCount = markCount + 1u;
            }

            /* Note: There is no need to check the status of page write       *
             * operation because (1) three pages of the block are being marked*
             * as bad and atleast one would pass (2) even if write to all the *
             * three pages fails, there is nothing much that can be done.     */
            LLC_nandGetStatus(hNandObj);
        }
    }while (flag);

    /* Determine if atleast one page has been written with bad block value    */
    if (0 != markCount)
    {
        /* Atleast one page has been marked with bad block indicator value    */
        result = IOM_COMPLETED;
    }

    /* If the driver is power aware disable the PSC */
    if ((TRUE == hNandObj->isPwrAware) && (TRUE == pscPwrOn))
    {
        pwrResult = nandLpscOff(hNandObj);
        if (IOM_COMPLETED != pwrResult)
        {
            result = pwrResult;
        }
    }

    /* Return the result of marking the block as bad                          */
    return (result);
}

/**
 * \brief Reads the spare area of the page
 *
 *        This function reads the spare area of the page number specified. The
 *        data area for storing the spare area has be specified.
 */
Int32 LLC_nandReadSpareArea(Ptr const         handle,
                            Uint32            phyBlock,
                            Uint8             phyPage,
                            LLC_nandSpareArea *spareData)
{
    Uint16       *tempPtr;      /* Typecasted pointer                       */
    Int32        result;        /* Result of reading the spare area         */
    Uint16       count;         /* Counting number of spare area words read */
    Uint16       parityLength;  /* Number of word of parity data            */
    LLC_nandObj  *hNandObj;     /* LLC nand device object pointer           */
    Uint32       temp;          /* Temporary linear page address            */
    Uint16       data[3];       /* Temporary storage for spare area info    */
    Bool         flag = TRUE;
    Int32        pwrResult;       /* Result of power APIs        */
    Bool         pscPwrOn = FALSE;/* Pwr off or not              */

    do
    {
        /* Validate the input parameters                                      */
        assert((NULL != handle) && (NULL != spareData));

        /* Initialize the local variables as required                         */
        flag = FALSE;
        hNandObj = (LLC_nandObj*)handle;
        temp = (phyBlock * hNandObj->nandDevInfo->pagesPerBlock) + phyPage;

        /* If the driver is power aware enable the PSC */
        if (TRUE == hNandObj->isPwrAware)
        {
            pwrResult = nandLpscOn(hNandObj);
            if (IOM_COMPLETED != pwrResult)
            {
                result = pwrResult;
                break;
            }
            else
            {
                pscPwrOn = TRUE;
            }
        }

        /* Issue the approriate READ commands to the NAND device              */
        if (LLC_NAND_PS_SMALL == hNandObj->nandDevInfo->pageSize)
        {
            /* Issue the spare area read command and the page address         */
            LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_READ2);
            LLC_nandSendPageAddress(hNandObj, temp, 0x00u, FALSE);
        }
        else /* Big page */
        {
            /* Issue the page read command and the page address. For NAND     *
             * devices with 2048-byte page size, set the column address as    *
             * 2048 to the page  address so that the read starts from the     *
             * spare area directly                                            */
            LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_READ);
            LLC_nandSendPageAddress(hNandObj, temp, 2048u, FALSE);
            LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_READ_CYC2);
        }

        /* Wait until NAND device is ready                                    */
        result = LLC_nandWaitUntilReady(hNandObj);
        if (PSP_E_TIMEOUT == result)
        {
            /* NAND device is not ready. Return the timeout status.           */
            break;
        }

        /* Read the logical block number, logical page number and bad block   *
         * indicator.                                                         */
        data[0] = LLC_nandReadData(hNandObj);
        data[1] = LLC_nandReadData(hNandObj);
        data[2] = LLC_nandReadData(hNandObj);

        /* Determine of the ECC parity information is required                */
        if (NULL != spareData->eccData)
        {
            /* Determine the number of 16-bit words of parity information     */
            parityLength = (Uint16)(hNandObj->nandDevInfo->pageSize >> 9u) * 5u;

            /* Read the ECC parity information                                */
            tempPtr = (Uint16*)spareData->eccData;
            assert(NULL != tempPtr);
            for (count = 0; count < parityLength; count++)
            {
                *tempPtr = LLC_nandReadData(hNandObj);
                tempPtr++;
            }
        }

        /* Write the data into the spare data                                 */
        if ((LLC_NAND_SMALL_PAGE_SIZE == hNandObj->nandDevInfo->pageSize) &&
             (LLC_NAND_BW_8BIT == hNandObj->nandDevInfo->dataBusWidth))
        {
            spareData->logicalBlock = data[0];
            spareData->logicalBlock = ((data[1] & 0xFFu) << 16u) |
                                      spareData->logicalBlock;
            spareData->logicalPage  = (Uint8)((data[1] >> 8u) & 0xFFu);
        }
        else
        {
            spareData->logicalBlock = data[1];
            spareData->logicalBlock = ((data[2] & 0xFFu) << 16u) |
                                      spareData->logicalBlock;
            spareData->logicalPage  = (Uint8)((data[2] >> 8u) & 0xFFu);
        }

        /* Check the bad block indicator byte/word                            */
        spareData->badBlockMark = FALSE;
        if (LLC_NAND_PS_SMALL == hNandObj->nandDevInfo->pageSize)
        {
            if (LLC_NAND_BW_8BIT == hNandObj->nandDevInfo->dataBusWidth)
            {
                /* Check the 5th byte of the spare area for small-page        *
                 * 8-bit NAND device                                          */
                if ((Uint8)((data[2] >> 8u) & 0xFFu) != 0xFFu)
                {
                    /* Bad block found                                        */
                    spareData->badBlockMark = TRUE;
                    break;
                }
            }
            else  /* Small-page 16-bit NAND device */
            {
                /* Check the 0th word of the spare area for small-page        *
                 * 16-bit NAND device.                                        */
                if (data[0] != 0xFFFFu)
                {
                    /* Bad block found                                        */
                    spareData->badBlockMark = TRUE;
                    break;
                }
            }
        }
        else  /* Big Page NAND device */
        {
            if (LLC_NAND_BW_8BIT == hNandObj->nandDevInfo->dataBusWidth)
            {
                /* Check the 0th byte of the spare area for big-page          *
                 * 8-bit NAND device                                          */
                if ((data[0] & 0xFFu) != 0xFFu)
                {
                    /* Bad block found                                        */
                    spareData->badBlockMark = TRUE;
                    break;
                }
            }
            else  /* Big-page 16-bit NAND device */
            {
                /* Check the 0th word of the spare area for big-page          *
                 * 16-bit NAND device.                                        */
                if (data[0] != 0xFFFFu)
                {
                    /* Bad block found                                        */
                    spareData->badBlockMark = TRUE;
                    break;
                }
            }
        }

        /* Set the result of the spare area read as success                   */
        result = IOM_COMPLETED;
    }while (flag);


    /* If the driver is power aware disable the PSC */
    if ((TRUE == hNandObj->isPwrAware) && (TRUE == pscPwrOn))
    {
        pwrResult = nandLpscOff(hNandObj);
        if (IOM_COMPLETED != pwrResult)
        {
            result = pwrResult;
        }
    }

    /* Return the result of reading the spare area                            */
    return (result);
}

/**
 * \brief Executes a IOCTL command
 *
 *        This function executes a IOCTL command specified for the NAND LLC
 *        module.
 */
Int32 LLC_nandIoctl(Ptr const         handle,
                    LLC_nandIoctlCmd  ioctlCmd,
                    Ptr               *param)
{
    Int32             result;      /* Result of executing the IOCTL  */
    EDMA3_DRV_Result  edmaResult;  /* EDMA result                    */
    LLC_nandObj       *hNandObj;   /* LLC nand device object pointer */
    Bool              flag = TRUE;

    do
    {
        /* Validate the input parameters */
        assert((NULL != handle) && (NULL != param));

        /* Initialize the local variables as required                         */
        hNandObj = (LLC_nandObj*)handle;
        flag = FALSE;

        /* Execute the requested IOCTL command                                */
        switch (ioctlCmd)
        {
            case LLC_NAND_IOCTL_SET_EDMA_EVTQ:

                /* Change the EDMA channel event queue number                 */
                edmaResult = EDMA3_DRV_mapChToEvtQ(
                                 hNandObj->hEdma,
                                 hNandObj->edmaChannel,
                                 (EDMA3_RM_EventQueue)(*(Uint32*)param));
                result = (edmaResult == EDMA3_DRV_SOK) ?
                         IOM_COMPLETED : PSP_NAND_E_ERROR;
                if (IOM_COMPLETED == result)
                {
                    /* Keep a copy of the event queue number selected.        */
                    hNandObj->edmaEvtQ = *(Uint32*)param;
                }
                break;

            default:

                /* An invalid LLC NAND Ioctl command is specified             */
                result = IOM_EBADARGS;
                break;
        }
    }while (flag);

    /* Return the result of exeucting the LLC NAND IOCTL command              */
    return (result);
}


/**
 * \brief Deinitializes the LLC OneNAND device layer
 *
 *        This function de-initializes the OneNAND LLC layer. It releases the
 *        EDMA channel and deletes the sync semaphore.
 */
Int32 LLC_nandDeinit(Ptr const handle)
{
    LLC_nandObj           *hNandObj;        /* Handle of LLC_nandObj    */
    Int32                 result;           /* Result of initializaiton */
    EDMA3_DRV_Result      edmaResult;       /* EDMA result of freeing channel */
    Bool                  flag = TRUE;
    do
    {
        /* Initialize the local variables                                     */
        result = IOM_EBADARGS;
        flag = FALSE;

        /* Validate the input parameters                                      */
        assert(NULL != handle);

        /* Get the handle of the LLC NAND object                              */
        hNandObj = (LLC_nandObj*)handle;

        /* If hEdma is not null, it means EDMA mode of data transfer has been *
         * selected. In that case, release the EDMA channel.                  */
        if (hNandObj->hEdma != NULL)
        {
            /* Release the EDMA channel for the data transfer                 */
            edmaResult = EDMA3_DRV_freeChannel((Ptr)hNandObj->hEdma,
                             hNandObj->edmaChannel);

            /* Release the semaphore for EDMA synchornization                 */
            SEM_delete(hNandObj->syncSem);
            /* Ensure that hEdma handle is deinitialized */
            hNandObj->hEdma = NULL;
            /* Ensure that EDMA and Semaphore are released */
            if (edmaResult != EDMA3_DRV_SOK)
            {
                /* Either EDMA channel release or sempahore release has
                   failed. So return with error. */
                result = PSP_NAND_E_ERROR;
                break;
            }
        }
        /* If the driver is not power aware disable the PSC */
        if (FALSE == hNandObj->isPwrAware)
        {
            /*
             * The last thing to do when power management is not desired is to
             * turn off the controller using the PSC or PWRM API
             */
            result = nandLpscOff(hNandObj);
            if (IOM_COMPLETED != result)
            {
                break;
            }
        }

#ifdef BIOS_PWRM_ENABLE
        if ((IOM_COMPLETED == result) && (TRUE == hNandObj->isPwrAware))
        {
            /* unregister all the PWRM event notifications                    */
            result = nandUnregisterNotification(hNandObj);
        }
#endif

        /* De-Initialization is complete                                      */
        result = IOM_COMPLETED;
    }while (flag);

    /* Return the result of de-initializing LLC module                        */
    return (result);
}


/**
 * \brief Determines the write protection status of the media
 *
 * This function determines the write protection status of the media. The
 * write protection would be for the media, the function argument phyBlkNum
 * should be 0xFFFFFFu.
 */
Int32 LLC_nandIsWriteProtected(Ptr const handle, Uint32 phyBlkNum)
{
    LLC_nandObj           *hNandObj;       /* Handle of LLC_nandObj    */
    Int32                 result;          /* Result of initializaiton */
    Uint8                 nandStatus;      /* Holds the device status  */
    Int32                 pwrResult;       /* Result of power APIs     */
    Bool                  pscPwrOn = FALSE;

    /* Initialize the local variables */
    result = IOM_COMPLETED;

    /* Validate the input parameters */
    if ((handle != NULL) && (0xFFFFFFu == phyBlkNum))
    {
        /* Get the handle of the LLC NAND object */
        hNandObj = (LLC_nandObj*)handle;

        /* If the driver is power aware enable the PSC */
        if (TRUE == hNandObj->isPwrAware)
        {
            pwrResult = nandLpscOn(hNandObj);
            if (IOM_COMPLETED != pwrResult)
            {
                result = pwrResult;
            }
            else
            {
                pscPwrOn = TRUE;
            }
        }

        if (IOM_COMPLETED == result)
        {
            /* Issue the READ_STATUS command and read the NAND status */
            LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_READ_STATUS);
            LLC_nandDelay(5u);
            nandStatus = LLC_nandReadData8Bit(hNandObj);
            /* Determine the current status of the NAND flash */
            if ((Uint8)0x00 == (0x80u & nandStatus))
            {
                /* NAND is write protected */
                result = PSP_NAND_E_WRITE_PROTECTED;
            }
        }

        /* If the driver is power aware disable the PSC */
        if ((TRUE == hNandObj->isPwrAware) && (TRUE == pscPwrOn))
        {
            pwrResult = nandLpscOff(hNandObj);
            if (IOM_COMPLETED != pwrResult)
            {
                result = pwrResult;
            }
        }
   }

   return (result);
}



Int32 LLC_nandIsBlockBad(Ptr const handle, Uint32 phyBlkNum)
{
    LLC_nandObj        *hNandObj;     /* Handle to LLC Object                  */
    Int32             result;        /* Status of the block                   */
    LLC_nandSpareArea spareData;     /* Spare area data                       */
    Uint8             pageNum;       /* Page Number                           */
    Uint8             count;         /* Keeping count of number of pages read */
    Bool              flag = TRUE;
    Int32             pwrResult;       /* Result of power APIs        */
    Bool              pscPwrOn = FALSE;/* Pwr off or not              */

    do
    {
        /* Initialize the local variables                                     */
        result = IOM_EBADARGS;
        flag = FALSE;
        spareData.eccData = NULL;
        pageNum = 0u;

        /* Validate the input parameters                                      */
        assert(NULL != handle);

        hNandObj = (LLC_nandObj*)handle;

        /* Check if the block number is valid                                 */
        if (phyBlkNum >= hNandObj->nandDevInfo->numBlocks)
        {
            break;
        }

        /* If the driver is power aware enable the PSC */
        if (TRUE == hNandObj->isPwrAware)
        {
            pwrResult = nandLpscOn(hNandObj);
            if (IOM_COMPLETED != pwrResult)
            {
                result = pwrResult;
                break;
            }
            else
            {
                pscPwrOn = TRUE;
            }
        }

        /* Read the spare area of 1st, 2nd and last page of the block         */
        for (count = 0; count < 3u; count++)
        {
            switch (count)
            {
                /* First page number of the block                             */
                case 0:
                    pageNum = 0u;
                    break;

                /* Second page number of the block                            */
                case 1:
                    pageNum = 1u;
                    break;

                /* Last page number of the block                              */
                case 2:
                    pageNum = (Uint8)(hNandObj->nandDevInfo->pagesPerBlock - 1u);
                    break;

                /* Nothing to do in default case                              */
                default:
                    break;
            }

            /* Read the spare area of the page */
            result = LLC_nandReadSpareArea((Ptr)hNandObj,
                         phyBlkNum,
                         pageNum,
                         &spareData);
            if (result != IOM_COMPLETED)
            {
                /* Reading of spare area failed */
                result = PSP_NAND_E_READ_FAIL;
                break;
            }

            /* Check the bad block indicator byte/word */
            if (TRUE == spareData.badBlockMark)
            {
                /* Bad block found */
                result = LLC_NAND_E_BLOCK_BAD;
                break;
            }
        }
    }while (flag);

    /* If the driver is power aware disable the PSC */
    if ((TRUE == hNandObj->isPwrAware) && (TRUE == pscPwrOn))
    {
        pwrResult = nandLpscOff(hNandObj);
        if (IOM_COMPLETED != pwrResult)
        {
            result = pwrResult;
        }
    }

    /* Return the result for bad block check */
    return (result);
}
/* ========================================================================== */
/*                      NAND LOCAL FUNCTION DEFINITIONS                       */
/* ========================================================================== */

/**
 * \brief  Writes the data area of the page.
 *
 *         This function writes the data area of the page. After writing the
 *         data area of the page, it return the 4-bit ECC parity generated by
 *         the EMIF.
 *
 * \param  hNandObj  [IN]   Handle of the NAND LLC module instance
 * \param  data      [IN]   Data buffer pointer with data to be written
 * \param  parity    [OUT]  Parity values read from the EMIF registers
 *
 * \return   IOM_COMPLETED     - On success
 *           PSP_E_TIMEOUT     - If write of bad block value did not succeed.
 */
static Int32 LLC_nandWriteDataArea(LLC_nandObj  *hNandObj,
                                   Uint8        *data,
                                   Uint8        *parity)
{
    Uint16           wordCount;     /* Word number to be written to the NAND */
    Uint16           subPageCount;  /* Sub-page number to be written         */
    Uint16           *tempPtr;      /* Type-casted pointer to data pointer   */
    Uint32           subPages;      /* Number of sub-pages to write          */
    Int32            result;        /* Result if writting data to data area  */
    volatile Uint32  dummyEMIFRead; /* Temporary variable                    */
    Bool             flag = TRUE;
    do
    {
        /* Initialize the local variables */
        result = IOM_EBADARGS;
        flag = FALSE;

        /* Validate the input parameters */
        assert(((NULL != hNandObj) && (NULL != data)) && (NULL != parity));

        /* Since write is always in 16-bits, reduce the size by 2             */
        tempPtr = (Uint16*)data;
        subPages = hNandObj->nandDevInfo->pageSize >> 9u;

        /* Write all the subpages of the device                               */
        for (subPageCount = 0u; subPageCount < subPages; subPageCount++)
        {
            /* Start the ECC  */
            LLC_nandECC4BitStart(hNandObj, FALSE);

            /* If a EDMA driver handle is available, write data using EDMA    */
            if (hNandObj->hEdma != NULL)
            {
                result = LLC_nandEdmaWrite(hNandObj, (Uint8*)tempPtr);
                if (result != IOM_COMPLETED)
                {
                    break;
                }
            }
            else /* Polled mode */
            {
                /* Write the data in polled mode                              */
                for (wordCount = 0u; wordCount < 256u; wordCount++)
                {
                    LLC_nandWriteData(hNandObj, tempPtr[wordCount]);
                }
                /* In polled-mode, the write is always considered to be
                   complete without any error */
                result = IOM_COMPLETED;
            }

            /* 512-bytes have been written. So stop the ECC. */
            LLC_nandECC4BitStop(hNandObj);


            /* Read the parity generated by EMIF for this sub-page            */
            LLC_nandECC4BitParityRead(hNandObj, parity);

            /* Adjust the parity pointer and the data pointer                 */
            parity = parity + 10u;
            tempPtr = tempPtr + 256u;
        }
    }while (flag);

    /* Return the result of writting data to data area of the page            */
    return (result);
}


/**
 * \brief  Writes the spare area of the page
 *
 *         This function writes the contents of the spare area of a page. This
 *         function writes the logical block number, logical page number, bad
 *         block indicator and the compressed 4-bit ECC parity information in
 *         the spare area of the page.
 *
 * \param  hNandObj          [IN]  Handle of the NAND LLC module instance
 * \param  spareData         [IN]  Spare Area Data information.
 *
 * \return   IOM_COMPLETED       - On success
 *           PSP_E_TIMEOUT       - If write of bad block value did not succeed.
 */
static Void LLC_nandWriteSpareArea(LLC_nandObj        *hNandObj,
                                   LLC_nandSpareArea  *spareData)
{
    Uint16  data[3];      /* First six bytes of the spare area          */
    Uint16  badBlockVal;  /* Bad block indicator value                  */
    Uint8   parityLength; /* Number of bytes in the parity              */
    Uint32  idx;          /* Index of the spare area word to be written */
    Uint16  *tempPtr;     /* Type-casted pointer to parity              */
    Bool    flag = TRUE;
    do
    {
        assert((NULL != hNandObj) && (NULL != spareData));
        flag = FALSE;

        /* Prepare the bad block indicator value to be written                */
        badBlockVal = (spareData->badBlockMark == FALSE) ? 0xFFFFu : 0xAAAAu;

        /* Prepare the first six bytes of the spare area                      */
        if ((LLC_NAND_SMALL_PAGE_SIZE == hNandObj->nandDevInfo->pageSize) &&
             (LLC_NAND_BW_8BIT == hNandObj->nandDevInfo->dataBusWidth))
        {
            data[0] = spareData->logicalBlock & 0xFFFFu;
            data[1] = ((Uint16)((Uint16)(spareData->logicalPage) << 8u)) |
                      ((spareData->logicalBlock >> 16u) & 0xFFu);
            data[2] = badBlockVal;
        }
        else
        {
            data[0] = badBlockVal;
            data[1] = spareData->logicalBlock & 0xFFFFu;
            data[2] = ((Uint16)((Uint16)(spareData->logicalPage) << 8u)) |
                      ((spareData->logicalBlock >> 16u) & 0xFFu);
        }

        /* Write the first six bytes of the spare area                        */
        LLC_nandWriteData(hNandObj, data[0]);
        LLC_nandWriteData(hNandObj, data[1]);
        LLC_nandWriteData(hNandObj, data[2]);

        /* Determine if any parity has been provided. If not, there is no
           need to write the reamining words of the spare area.               */
        if (NULL == spareData->eccData)
        {
            /* No parity information to write to spare area                   */
            break;
        }

        /* Determine the number of 16-bit words of parity information         */
        parityLength = (hNandObj->nandDevInfo->pageSize >> 9u) * 5u;

        /* Write the parity words to the spare area                           */
        tempPtr = (Uint16*)spareData->eccData;
        assert(NULL != tempPtr);
        for (idx = 0; idx < parityLength; idx++)
        {
            LLC_nandWriteData(hNandObj, tempPtr[idx]);
        }
    }while (flag);
}


/**
 * \brief  Reads the data area of the page
 *
 *         This function reads the data area of the page. The parity should be
 *         read before calling this function and should be supplied to this
 *         function.
 *
 * \param  hNandObj     [IN]  Handle of the NAND LLC module instance
 * \param  data         [IN]  Data buffer pointer to which data is read
 * \param  parity       [IN]  4-bit ECC paritt values to write to spare area.
 * \param  useEcc       [IN]  Flag to use ECC module for error detection
 *                              correction..
 *
 * \return   IOM_COMPLETED      - On success
 *           PSP_E_TIMEOUT      - If write of bad block value did not succeed.
 */
static Int32 LLC_nandReadDataArea(LLC_nandObj  *hNandObj,
                                  Uint8        *data,
                                  Uint8        *parity,
                                  Bool         useEcc)
{
    Uint16          wordCount;     /* Word number to be read          */
    Uint16          subPageCount;  /* Sub-page number                 */
    Uint16          *tempPtr;      /* Type-casted pointer             */
    Uint8           subPages;      /* Number of sub-pages             */
    Int32           result;        /* Result of reading the data area */
    volatile Uint32 dummyEMIFRead; /* Temporary variable              */
    Bool            flag = TRUE;
    do
    {
        /* Initialize the local variables                                     */
        result = IOM_COMPLETED;
        flag = FALSE;

        /* Validate the input parameters                                      */
        assert(((NULL != hNandObj) && (NULL != data)) && (NULL != parity));

        /* Since read is always in 16-bits, reduce the size by 2              */
        tempPtr = (Uint16*)data;
        subPages = hNandObj->nandDevInfo->pageSize >> 9u;

        /* Read the page data area                                            */
        for (subPageCount = 0u; subPageCount < subPages; subPageCount++)
        {

            if (TRUE == useEcc)
            {
                /* Start the 4-bit ECC parity generation                      */
                LLC_nandECC4BitStart(hNandObj, TRUE);
            }

            /* If the EDMA driver handle is available, then do the data       *
             * read using the EDMA channel.                                   */
            if (hNandObj->hEdma != NULL)
            {
                result = LLC_nandEdmaRead(hNandObj, (Uint8*)tempPtr);
                if (result != IOM_COMPLETED)
                {
                    break;
                }
            }
            else /* Polled mode */
            {
                /* Read the sub-page from the data area                       */
                for (wordCount = 0u; wordCount < 256u; wordCount++)
                {
                    tempPtr[wordCount] = LLC_nandReadData(hNandObj);
                }
            }

            if (TRUE == useEcc)
            {
                /* Stop the 4-bit ECC parity generation                       */
                LLC_nandECC4BitStop(hNandObj);

                /* Perform ECC error detection and correction                 */
                result = LLC_nandECC4BitCorrect(hNandObj,
                             (Uint8*)tempPtr,
                             parity);

                /* If there is an ECC error, and ECC has been requested
                    return appropriate error code                             */
                if ((PSP_NAND_E_ECC_FAIL == result) && (TRUE == useEcc))
                {
                    break;
                }
           }

            /* Maintain the data pointers and the parity pointers             */
            tempPtr = tempPtr + 256u;
            parity = parity + 10u;
        }
    }while (flag);

    /* Return the result of reading the data area of the page                 */
    return (result);
}


/**
 * \brief  Starts the 4-Bit ECC hardware module
 *
 *         This function starts the 4-bit ECC hardware module. This function is
 *         invoked before read/write operations.
 *
 * \param  hNandObj     [IN]  Handle of the NAND LLC module instance
 * \param  toRead       [IN]  Identifies the operation as read or write
 *
 * \return None.
 */
static Void LLC_nandECC4BitStart(LLC_nandObj *hNandObj, Bool toRead)
{
    Uint32 val;

    toRead = toRead;      /* to remove compilation warning */
    /* Validate the input parameters */
    assert(NULL != hNandObj);

    /*
     * Start a new ECC calculation for reading or writing 512 bytes
     *  of data.
     */
    val = (hNandObj->regs->NANDFCR & ~(CSL_EMIFA_NANDFCR_4BITECCSEL_MASK));
    val |= (CSL_EMIFA_NANDFCR_4BITECCSEL_CS3ECC <<
            CSL_EMIFA_NANDFCR_4BITECCSEL_SHIFT) |
           (CSL_EMIFA_NANDFCR_CS3NAND_NAND_ENABLE <<
            CSL_EMIFA_NANDFCR_CS3NAND_SHIFT) |
           (CSL_EMIFA_NANDFCR_4BITECC_START_START <<
            CSL_EMIFA_NANDFCR_4BITECC_START_SHIFT);
    hNandObj->regs->NANDFCR = val;

}


/**
 * \brief  Stops the 4-Bit ECC hardware module
 *
 *         This function stops the 4-bit ECC hardware module. This function is
 *         invoked after read/write operations.
 *
 * \param  hNandObj     [IN]  Handle of the NAND LLC module instance
 *
 * \return None.
 */
static Void LLC_nandECC4BitStop(LLC_nandObj *hNandObj)
{
    /* Validate the input parameters */
    assert(NULL != hNandObj);

    hNandObj->regs->NANDFCR &= ~CSL_FMK(EMIFA_NANDFCR_4BITECC_START,
                                        CSL_EMIFA_NANDFCR_4BITECC_START_START);
}


/**
 * \brief  Reads the parity from the EMIF NAND registers.
 *
 *         This function reads the parity values from the EMIF nand registers.
 *         It then compresses the 10-bit 8-word parity values into 10-bytes.
 *
 * \param  hNandObj     [IN]  Handle of the NAND LLC module instance
 * \param  parity       [IN]  4-bit ECC paritt values to write to spare area.
 *
 * \return   IOM_COMPLETED       - On success
 *           PSP_E_TIMEOUT       - If write of bad block value did not succeed.
 */
static Void LLC_nandECC4BitParityRead(LLC_nandObj *hNandObj, Uint8 *parity)
{
    Uint16  eccParity1[8];/* as eccParity is already a global var */
    Bool    flag = TRUE;
    do
    {
        /* Validate the input parameters */
        assert((NULL != hNandObj) && (NULL != parity));
        flag = FALSE;

        /* 1. Read 10-bytes ECC syndrome value from syndrome regs             */
        /* bit 9:0*/
        eccParity1[0] = (hNandObj->regs->NAND4BITECC1 &
            (CSL_EMIFA_NAND4BITECC1_4BITECCVAL1_MASK));
        /* bit 25:16*/
        eccParity1[1] = (hNandObj->regs->NAND4BITECC1 &
            (CSL_EMIFA_NAND4BITECC1_4BITECCVAL2_MASK)) >>
            CSL_EMIFA_NAND4BITECC1_4BITECCVAL2_SHIFT;
        /* bit 9:0*/
        eccParity1[2] = (hNandObj->regs->NAND4BITECC2 &
            (CSL_EMIFA_NAND4BITECC2_4BITECCVAL3_MASK));
        /* bit 25:16*/
        eccParity1[3] = (hNandObj->regs->NAND4BITECC2 &
            (CSL_EMIFA_NAND4BITECC2_4BITECCVAL4_MASK)) >>
            CSL_EMIFA_NAND4BITECC2_4BITECCVAL4_SHIFT;
        /* bit 9:0*/
        eccParity1[4] = (hNandObj->regs->NAND4BITECC3 &
            (CSL_EMIFA_NAND4BITECC3_4BITECCVAL5_MASK));
        /* bit 25:16*/
        eccParity1[5] = (hNandObj->regs->NAND4BITECC3 &
            (CSL_EMIFA_NAND4BITECC3_4BITECCVAL6_MASK)) >>
            CSL_EMIFA_NAND4BITECC3_4BITECCVAL6_SHIFT;
        /* bit 9:0*/
        eccParity1[6] = (hNandObj->regs->NAND4BITECC4 &
            (CSL_EMIFA_NAND4BITECC4_4BITECCVAL7_MASK));
        /* bit 25:16*/
        eccParity1[7] = (hNandObj->regs->NAND4BITECC4 &
            (CSL_EMIFA_NAND4BITECC4_4BITECCVAL8_MASK)) >>
            CSL_EMIFA_NAND4BITECC4_4BITECCVAL8_SHIFT;

        /* 2. convert 10 * 8-bits of parity into 8 * 10-bits                  */
        parity[0] = eccParity1[0] & (0xFFu);
        parity[1] = ((eccParity1[1] & (0x3Fu)) << 2) |
                    ((eccParity1[0] & (0x300u)) >> 8);
        parity[2] = ((eccParity1[2] & (0x0Fu)) << 4) |
                    ((eccParity1[1] & (0x3C0u)) >> 6);
        parity[3] = ((eccParity1[3] & (0x03u)) << 6) |
                    ((eccParity1[2] & (0x3F0u)) >> 4);
        parity[4] = ((eccParity1[3] & (0x3FCu))>> 2);
        parity[5] = eccParity1[4] & (0xFFu);
        parity[6] = ((eccParity1[5] & (0x3Fu)) << 2) |
                    ((eccParity1[4] & (0x300u)) >> 8);
        parity[7] = ((eccParity1[6] & (0x0Fu)) << 4) |
                    ((eccParity1[5] & (0x3C0u)) >> 6);
        parity[8] = ((eccParity1[7] & (0x03u)) << 6) |
                    ((eccParity1[6] & (0x3F0u)) >> 4);
        parity[9] = ((eccParity1[7] & (0x3FCu))>> 2);

    }while (flag);
}


/**
 * \brief  Corrects any ECC errors in the data
 *
 *         This function corrects any ECC errors found in the data. The data
 *         pointer and the parity pointer have to be supplied.
 *
 * \param  hNandObj     [IN]  Handle of the NAND LLC module instance
 * \param  data         [IN]  Parity values read from the EMIF registers
 * \param  parity       [IN]  4-bit ECC parity values to write to spare area.
 *
 * \return   IOM_COMPLETED           - On success
 *         PSP_E_TIMEOUT         - If write of bad block value did not succeed.
 */
static Int32 LLC_nandECC4BitCorrect(LLC_nandObj   *hNandObj,
                                    Uint8         *data,
                                    Uint8         *parity)
{
    Uint16        eccParity1[8];/* as eccParity is already a global var */
    Int8          i;
    Int32         result;       /* Result of correcting any ECC errors   */
    Uint32        eccState;     /* Temporary variable to store ECC state */
    Uint16        errorAddress; /* Address of byte that has error        */
    Uint16        errorValue;   /* Error value to be xored               */
    Uint8         numErrors;    /* Number of ECC errors                  */
    volatile Uint32  dummyEMIFRead; /* Temporary variable                    */
    Bool          flag = TRUE;
    Uint32        ecc[4];

    do
    {
        /* Validate the input parameters                                      */
        assert(((NULL != hNandObj) && (NULL != data)) && (NULL != parity));
        flag = FALSE;


        /* step 1.
         * convert 8 * 10-bits of parityinto 10 * 8-bits i.e. this converts   *
         * the parity that is written in the spare area by the nand writer    *
         * as 10 bytes into eight 10 bit words                                */

        eccParity1[0] = (((parity[1] & (0x3u)) << 8) | (parity[0]));
        eccParity1[1] = (((parity[2] & (0xFu)) << 6) |
                        ((parity[1] & (0xFCu)) >> 2));
        eccParity1[2] = (((parity[3] & (0x3Fu)) << 4) |
                        ((parity[2] & (0xF0u)) >> 4));
        eccParity1[3] = (Uint16)((parity[4] << 2) |
                        ((parity[3] & (0xC0u)) >> 6));
        eccParity1[4] = (((parity[6] & (0x3u)) << 8) | (parity[5]));
        eccParity1[5] = (((parity[7] & (0xFu)) << 6) |
                        ((parity[6] & (0xFCu)) >> 2));
        eccParity1[6] = (((parity[8] & (0x3Fu)) << 4) |
                        ((parity[7] & (0xF0u)) >> 4));
        eccParity1[7] = (Uint16)((parity[9] << 2) |
                        ((parity[8] & (0xC0u)) >> 6));

        /*
         * Write the parity values in the NAND Flash 4-bit ECC Load register.
         * The parity data is written in reverse order to ECC Load reg.
         * Write each parity value one at a time starting from 4bit_ecc_val8
         * to 4bit_ecc_val1.
         */

        for (i = 7; i >= 0; i--)
        {
            hNandObj->regs->NAND4BITECCLOAD = eccParity1[i];
        }

        /*
         * Perform a dummy read to the EMIF Revision Code and Status register.
         * This is required to ensure time for syndrome calculation after
         * writing the ECC values in previous step.
         */
        dummyEMIFRead = hNandObj->regs->REVID;

         /*
          * Read the syndrome from the NAND Flash 4-Bit ECC 1-4 registers.
          * A syndrome value of 0 means no bit errors. If the syndrome is
          * non-zero then go further otherwise return.
          */
         ecc[0] = hNandObj->regs->NAND4BITECC1 & 0x03FF03FF;
         ecc[1] = hNandObj->regs->NAND4BITECC2 & 0x03FF03FF;
         ecc[2] = hNandObj->regs->NAND4BITECC3 & 0x03FF03FF;
         ecc[3] = hNandObj->regs->NAND4BITECC4 & 0x03FF03FF;

        if ((ecc[0] == 0x0) && (ecc[1] == 0x0) && (ecc[2] == 0x0) &&
            (ecc[3] == 0x0))
        {
            result = IOM_COMPLETED;
        }
        else
        {

             /*
              * Clear any previous address calculation by doing a dummy read
              * of an error address register.
              */
             dummyEMIFRead = hNandObj->regs->NANDERRADD1;

            /*
             * Set the addr_calc_st bit(bit no 13) in the NAND Flash Control
             * register to 1.
             */
            hNandObj->regs->NANDFCR |=
                (CSL_EMIFA_NANDFCR_4BITECC_ADD_CALC_START_START <<
                CSL_EMIFA_NANDFCR_4BITECC_ADD_CALC_START_SHIFT);

             /*
              * ECC_STATE field reads 0x3 (Error correction complete) immediately
              * after setting the 4BITECC_ADD_CALC_START bit. So if you immediately
              * begin trying to poll for the state, you may fall right out of your
              * loop without any of the correction calculations having taken place.
              * The recommendation from the hardware team is to wait till ECC_STATE
              * reads less than 4, which means ECC HW has entered correction state.
              */
             do
            {
                eccState = (hNandObj->regs->NANDFSR >> 8) & 0x0f;
            }while (eccState < 4);

            /*  Wait for the corr_state(bit 8:11) field in the NAND Flash
             *  Status register to be equal to 0x1, 0x2, or 0x3.
             *  0x0: no error;
             *  0x1: cannot be corrected(five or more errors).The number of
             *       errors calculated (err_num field) differs from the number
             *       of errors searched.
             *  0x2: error correction complete (errors on bit 8 or 9).
             *  0x3: error correction complete (error exists).
             *  0x5: calculating number of errors.
             *  0x6 to 0xF: calculating/searching error value */
            do
            {
                eccState = hNandObj->regs->NANDFSR;
                eccState &= CSL_EMIFA_NANDFSR_ECC_STATE_MASK;
                eccState = eccState >> CSL_EMIFA_NANDFSR_ECC_STATE_SHIFT;
            }while (eccState >= 0x04);

            if (0u == eccState) /* no error */
            {
                result = IOM_COMPLETED;
            }
            else if ((2u == eccState) || (3u == eccState))
            {
                /*  The number of errors in err_num(bit 16:17) field in the*
                 *  NAND Flash Status register.                            *
                 *  0x0: 1 error;                                          *
                 *  0x1: 2 errors;                                         *
                 *  0x2: 3 errors;                                         *
                 *  0x3: 4 errors found.                                   */
                numErrors = (hNandObj->regs->NANDFSR &
                             CSL_EMIFA_NANDFSR_ECC_ERRNUM_MASK) >>
                             CSL_EMIFA_NANDFSR_ECC_ERRNUM_SHIFT;

                /* Read the error address, error value and correct            */
                for (i = 0; i <= numErrors; i++)
                {
                    switch (i)
                    {
                        case 0:
                            errorAddress = hNandObj->regs->NANDERRADD1 &
                                CSL_EMIFA_NANDERRADD1_4BITECCERRADD1_MASK;
                            errorValue = hNandObj->regs->NANDERRVAL1 &
                                CSL_EMIFA_NANDERRVAL1_4BITECCERRVAL1_MASK;
                            break;
                        case 1:
                            errorAddress = (hNandObj->regs->NANDERRADD1 >>
                                CSL_EMIFA_NANDERRADD1_4BITECCERRADD2_SHIFT) &
                                CSL_EMIFA_NANDERRADD1_4BITECCERRADD1_MASK;
                            errorValue = (hNandObj->regs->NANDERRVAL1 >>
                                CSL_EMIFA_NANDERRVAL1_4BITECCERRVAL2_SHIFT) &
                                CSL_EMIFA_NANDERRVAL1_4BITECCERRVAL1_MASK;
                            break;
                        case 2:
                            errorAddress = hNandObj->regs->NANDERRADD2 &
                                CSL_EMIFA_NANDERRADD2_4BITECCERRADD3_MASK;
                            errorValue = hNandObj->regs->NANDERRVAL2 &
                                CSL_EMIFA_NANDERRVAL2_4BITECCERRVAL3_MASK;
                            break;
                        case 3:
                            errorAddress = (hNandObj->regs->NANDERRADD2 >>
                                CSL_EMIFA_NANDERRADD2_4BITECCERRADD4_SHIFT) &
                                CSL_EMIFA_NANDERRADD2_4BITECCERRADD3_MASK;
                            errorValue = (hNandObj->regs->NANDERRVAL2 >>
                                CSL_EMIFA_NANDERRVAL2_4BITECCERRVAL4_SHIFT) &
                                CSL_EMIFA_NANDERRVAL2_4BITECCERRVAL3_MASK;
                            break;
                        default:
                            /* print error */
                            break;
                    }
                    errorAddress = (512u + 7u) - errorAddress;
                    /* xor the corrupt data with error value   */
                    data[errorAddress] ^= (Uint8)errorValue;
                }
                result = IOM_COMPLETED;
            }
            else
            {
                result = PSP_NAND_E_ECC_FAIL;
            }
        }
    }while (flag);

    /* Return the result of correcting the ECC errors */
    return (result);
}


/**
 * \brief Lookup the device organization based on the nand device ID
 *
 *         This function looks up the NAND device organization based on the NAND
 *         device ID specified as parameter. The lookup table to be used has to
 *         be specified. If no matching entry is found, then this function
 *         returns a NULL, otherwise it returns a pointer to the device
 *         organization structure.
 *
 * \param  hNandObj       [IN]  Handle of the NAND LLC module instance
 * \param  nandDevInfoLut [IN]  NAND device organization lookup table
 * \param  nandId         [IN]  NAND device ID.
 *
 * \return Pointer to device info structure, on success
 *         NULL, on failure.
 */
static PSP_nandDeviceInfo* LLC_nandLookupDeviceInfo(
                               LLC_nandObj         *hNandObj,
                               PSP_nandDeviceInfo  *nandDevInfoLut,
                               Uint32              nandId)
{
    PSP_nandDeviceInfo  *devInfo = NULL;/* Ptr to NAND device organization */
    Uint16              vendorId; /* 1st byte of the device ID           */
    Uint16              deviceId;  /* 2nd byte of the device ID          */
    Bool                flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert((NULL != hNandObj) && (NULL != nandDevInfoLut));
        hNandObj = hNandObj; /* To remove compilation warning */
        /* Initialize the local variables                                     */
        devInfo = (PSP_nandDeviceInfo*)nandDevInfoLut;
        flag = FALSE;

        vendorId = (Uint8)(nandId & 0xFFu);
        deviceId = (Uint8)((nandId >> 8u) & 0xFFu);

        /* Lookup the table until all the entries of the table are checked    */
        while (devInfo->vendorId != 0u)
        {
            /* Check if the vendor ID and device ID match                     */
            if ((devInfo->vendorId == vendorId) &&
                 (devInfo->deviceId == deviceId))
            {
                /* Device organization values found                           */
                break;
            }

            /* Device ID values did not match. Check next entry.              */
            devInfo++;
        }

        /* If no matching devices are found, set return address as NULL.      */
        if (0u == devInfo->vendorId)
        {
            devInfo = NULL;
        }

    }while (flag);

    /* Return the address of the device organization                          */
    return (devInfo);
}


/**
 * \brief  Lookup the device timing values based on the nand device ID
 *
 *         This function looks up the NAND device timing based on the NAND
 *         device ID specified as parameter. The lookup table to be used has to
 *         be specified.If no matching entry is found, then this function
 *         returns a NULL, otherwise it returns a pointer to the device timing
 *         structure.
 *
 * \param  hNandObj         [IN]  Handle of the NAND LLC module instance
 * \param  nandDevTimingLut [IN]  Parity values read from the EMIF registers
 * \param  nandId          [IN]  4-bit ECC paritt values to write to spare area.
 *
 * \return   IOM_COMPLETED   - On success
 *           PSP_E_TIMEOUT   - If write of bad block value did not succeed.
 */
static PSP_nandDeviceTiming* LLC_nandLookupDeviceTiming(
                                 LLC_nandObj           *hNandObj,
                                 PSP_nandDeviceTiming  *nandDevTimingLut,
                                 Uint32                nandId)
{
    PSP_nandDeviceTiming  *devTiming = NULL;  /* Pointer to NAND device timing*/
    Uint16                vendorId;    /* 1st byte of the device ID     */
    Uint16                deviceId;    /* 2nd byte of the device ID     */
    Bool                  flag = TRUE;
    do
    {
        /* Valiate the input parameters                                       */
        assert((NULL != hNandObj) && (NULL != nandDevTimingLut));
        hNandObj = hNandObj; /* To remove compilation warning */
        /* Initialize the local variables                                     */
        devTiming = (PSP_nandDeviceTiming*)nandDevTimingLut;
        flag = FALSE;

        vendorId = (Uint8)(nandId & 0xFFu);
        deviceId = (Uint8)((nandId >> 8u) & 0xFFu);

        /* Lookup the table until all the entries of the table are checked    */
        while (devTiming->vendorId != 0)
        {
            /* Check if the vendor ID and device ID match                     */
            if ((devTiming->vendorId == vendorId) &&
                 (devTiming->deviceId == deviceId))
            {
                /* Device timing values found                                 */
                break;
            }

            /* Device ID values did not match. Check next entry.              */
            devTiming++;
        }

        /* If no matching devices are found, set return address as NULL.      */
        if (0u == devTiming->vendorId)
        {
            devTiming = NULL;
        }

    }while (flag);

    /* Return the address of the device timing                                */
    return (devTiming);
}


/**
 * \brief  Reads the device ID of the device.
 *
 *         This function reads the NAND device ID and returns it. It issues the
 *         NAND read ID command and reads a 4-byte NAND device ID.
 *
 * \param  hNandObj     [IN]  Handle of the NAND LLC module instance
 *
 * \return   IOM_COMPLETED           - On success
 *         PSP_E_TIMEOUT         - If write of bad block value did not succeed.
 */
static Uint32 LLC_nandReadId(LLC_nandObj  *hNandObj)
{
    Uint32       deviceId;
    Bool         flag = TRUE;
    do
    {
        /* Initialize the local variables */
        deviceId = 0u;
        flag = FALSE;

        /* Validate the input parameters  */
        assert(NULL != hNandObj);

        /* Send the read ID command       */
        LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_READID);

        /* Write one cycle address with address as zero */
        LLC_nandWriteAddress(hNandObj, 0x00u);
        LLC_nandDelay(10u);

        /* Read the 4-byte device ID     */
        deviceId  = LLC_nandReadData8Bit(hNandObj);
        deviceId |= ((Uint32)(LLC_nandReadData8Bit(hNandObj)) << 8u);
        deviceId |= ((Uint32)(LLC_nandReadData8Bit(hNandObj)) << 16u);
        deviceId |= ((Uint32)(LLC_nandReadData8Bit(hNandObj)) << 24u);

    }while (flag);

    /* Return the 4-byte device ID       */
    return(deviceId);
}


/**
 * \brief  Waits until the NAND device is ready.
 *
 *         This function waits until the NAND device enters the ready state. It
 *         reads the status of the NAND's ready/busy status by reading the wait
 *         status from the NANDFSR register. It also implements a timeout
 *         functionality to avoid indefinte lockup.
 *
 * \param  hNandObj     [IN]  Handle of the NAND LLC module instance
 *
 * \return IOM_COMPLETED         - On success
 *         PSP_E_TIMEOUT         - If write of bad block value did not succeed.
 */
static Int32 LLC_nandWaitUntilReady(LLC_nandObj *hNandObj)
{
    Int32            result;        /*Result to be returned                   */
    volatile Uint32  timeout;       /* Timeout incase NAND never becomes ready*/
    volatile Uint32  dummyEMIFRead; /* Temporary variable                     */
    Bool             flag = TRUE;
    do
    {
        /* Validate the input parameters */
        assert(NULL != hNandObj);

        /* Initialize the local variables */
        timeout = LLC_NAND_RBWAIT_TIMEOUT;
        result  = IOM_COMPLETED;
        flag = FALSE;

        /* This function is called immediatly after issuing commands to the   *
         * NAND flash. Since the NAND flash takes sometime to pull the R/B    *
         * line low,it would be safe to introduce a delay before checking the *
         * ready/busy status                                                  */
        LLC_nandDelay(150u);

        /* Keep checking the status of ready/busy line.Also maintain a timeout*
         * count. If the NAND is not ready during the timeout period, stop    *
         * checking the ready/busy status                                     */
        while (timeout > 0u)
        {
            /* Check the Ready/Busy status */
            if ((hNandObj->regs->NANDFSR & CSL_EMIFA_NANDFSR_WAITST_HIGH) != 0)
            {
                /* NAND flash is ready. Break from the loop.                  */
                break;
            }
            timeout = timeout - 1u;
        }

        /* Determine if the wait for ready status ended due to timeout        */
        if (0 == timeout)
        {
            result = PSP_E_TIMEOUT;
        }

    }while (flag);

    /* Wait for few more cycles before procedding. Is this required? */
    LLC_nandDelay(5u);

    /* Return the result of the waiting for NAND ready status                 */
    return (result);
}



/**
 * \brief  Gets the status of NAND device by issuing the status read command.
 *
 *         This function gets the status of the NAND flash device by issuing a
 *         status read command. After getting the status, it checks if the NAND
 *         device is write protected. If it is not write protected, it checks
 *         the status of the previous command (erase/write). Note: This function
 *         should be called only when the NAND device is in ready state.
 *
 * \param  hNandObj     [IN]  Handle of the NAND LLC module instance
 *
 * \return   IOM_COMPLETED       - On success
 *           PSP_E_TIMEOUT       - If write of bad block value did not succeed.
 */
static Int32 LLC_nandGetStatus(LLC_nandObj *hNandObj)
{
    Uint8       nandStatus;  /* Holds the status of the NAND flash */
    Int32       result;      /* Result to be returned              */
    Bool        flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert(NULL != hNandObj);

        /* Initialize the local variables                                     */
        result = IOM_COMPLETED;
        flag = FALSE;

        /* Issue the READ_STATUS command and read the NAND status             */
        LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_READ_STATUS);
        LLC_nandDelay(5u);
        nandStatus = LLC_nandReadData8Bit(hNandObj);

        /* Determine the current status of the NAND flash                     */
        /* Is NAND write-protected                                            */
        if ((Uint8)0x00 == (0x80u & nandStatus))
        {
            /* NAND is write protected                                        */
            result = PSP_NAND_E_WRITE_PROTECTED;
        }
        else
        {
            /* Did last command pass                                          */
            if ((0x1u & nandStatus) != 0)
            {
                /* Program or Erase operation has failed                      */
                result = LLC_NAND_E_STATUS_FAIL;
            }
        }
    }while (flag);

    /* Return the status of the NAND flash                                    */
    return (result);
}


/**
 * \brief  Writes the page address onto the NAND data bus
 *
 *         This function writes the address onto the NAND data bus. The address
 *         specified has to be broken into the row address and the column
 *         address. The row address is the address of the page and the column
 *         address is the offset within the page. It is also possible to only
 *         write the row address only.
 *
 * \param  hNandObj      [IN]  Handle of the NAND LLC module instance
 * \param  rowAddress    [IN]  Row address portion of NAND page address.
 * \param  columnAddress [IN]  Column address portion of NAND page address.
 * \param  rowOnly       [IN]  Should only row address be
 *
 * \return   IOM_COMPLETED           - On success
 *           PSP_E_TIMEOUT   - If write of bad block value did not succeed.
 */
static Void LLC_nandSendPageAddress(LLC_nandObj  *hNandObj,
                                    Uint32       rowAddress,
                                    Uint32       columnAddress,
                                    Bool         rowOnly)
{
    Uint32  count;          /* Used as a counter                              */
    Uint32  addressCycles;  /* Number of write cycles needed to write address */
    Bool    flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert(NULL != hNandObj);
        flag = FALSE;

        /* Keep a copy of the number of address cycles needed                 */
        addressCycles = hNandObj->addressCycles;

        /* Check if it is required to write the column address also           */
        if (FALSE == rowOnly)
        {
            /* In case of 16-bit NAND devices, the column address have to
               be 16-bit word aligned. So divide the column address by 2.     */
            if (LLC_NAND_BW_16BIT == hNandObj->nandDevInfo->dataBusWidth)
            {
                /* This is 16-bit NAND device. So adjust the column address.  */
                columnAddress = columnAddress >> 1;
            }

            /* Write the column address */
            LLC_nandWriteAddress(hNandObj, (Uint8)(columnAddress & 0xFF));
            if (LLC_NAND_BIG_PAGE_SIZE == hNandObj->nandDevInfo->pageSize)
            {
                /* For 2048 byte page, column address is two-cycles           */
                LLC_nandWriteAddress(hNandObj, ((columnAddress >> 8) & 0xFF));
            }
        }

        /* Write the row address. Write 8-bits for each write.                */
        for (count = 0; count < addressCycles; count++)
        {
            LLC_nandWriteAddress(hNandObj, (Uint8)rowAddress);
            rowAddress = rowAddress >> 8u;
        }
    }while (flag);
}


/**
 * \brief  NAND device wait timing configuration function.
 *
 *         This function configures the wait timing for the NAND device. The
 *         device timing can be provided to the function. If the device timing
 *         is not provided, this function sets the wait timing to maximum. This
 *         is a hardware dependent function.
 *
 * \param  hNandObj     [IN]  Handle of the NAND LLC module instance
 * \param  devTiming    [IN]  NAND device timing information
 *
 * \return None.
 */
static Void LLC_nandSetWaitTime(LLC_nandObj           *hNandObj,
                                PSP_nandDeviceTiming  *devTiming)
{
    Uint32  wset;     /* Write setup time in ns    */
    Uint32  wstb;     /* Write strobe time in ns   */
    Uint32  whld;     /* Write hold time in ns     */
    Uint32  rset;     /* Read setup time in ns     */
    Uint32  rstb;     /* Read strobe time in ns    */
    Uint32  rhld;     /* Read hold time in ns      */
    Uint32  ta;       /* Turnaround time in ns     */
    Uint32  clkFreq;  /* Clock frequeny in Mhz     */
    Uint32  timing;   /* Concatenated timing value */
    Uint32  defaultCfg = 0x3ffffffc;

    Bool    flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert(NULL != hNandObj);

        /* Intitialize the local variables                                    */
        timing = 0u;
        flag = FALSE;
        /* If the NAND device timing is not supplied, then worst case timing  *
           is configured. */
        if (NULL ==  devTiming)
        {
            wset = CSL_EMIFA_CE3CFG_W_SETUP_RESETVAL;
            wstb = CSL_EMIFA_CE3CFG_W_STROBE_RESETVAL;
            whld = CSL_EMIFA_CE3CFG_W_HOLD_RESETVAL;
            rset = CSL_EMIFA_CE3CFG_R_SETUP_RESETVAL;
            rstb = CSL_EMIFA_CE3CFG_R_STROBE_RESETVAL;
            rhld = CSL_EMIFA_CE3CFG_R_HOLD_RESETVAL;
            ta   = CSL_EMIFA_CE3CFG_TA_RESETVAL;
        }
        else  /* Device timing has been provided                              */
        {
            /* Calculate the register bit field values                        */
            clkFreq = hNandObj->clkFreq / 1000000u;

            wset = ((clkFreq * devTiming->writeSetup) / 1000u) &
                    CSL_EMIFA_CE3CFG_W_SETUP_RESETVAL;
            wstb = ((clkFreq * devTiming->writeStrobe) / 1000u) &
                    CSL_EMIFA_CE3CFG_W_STROBE_RESETVAL;
            whld = ((clkFreq * devTiming->writeHold) / 1000u) &
                    CSL_EMIFA_CE3CFG_W_HOLD_RESETVAL;
            rset = ((clkFreq * devTiming->readSetup) / 1000u) &
                    CSL_EMIFA_CE3CFG_R_SETUP_RESETVAL;
            rstb = ((clkFreq * devTiming->readStrobe) / 1000u) &
                    CSL_EMIFA_CE3CFG_R_STROBE_RESETVAL;
            rhld = ((clkFreq * devTiming->readHold) / 1000u) &
                    CSL_EMIFA_CE3CFG_R_HOLD_RESETVAL;
            ta   = ((clkFreq * devTiming->turnAround) / 1000u) &
                    CSL_EMIFA_CE3CFG_TA_RESETVAL;
        }

        /* Concatinate the timing parameters */
        timing = (wset << CSL_EMIFA_CE3CFG_W_SETUP_SHIFT)  |
                 (wstb << CSL_EMIFA_CE3CFG_W_STROBE_SHIFT) |
                 (whld << CSL_EMIFA_CE3CFG_W_HOLD_SHIFT)   |
                 (rset << CSL_EMIFA_CE3CFG_R_SETUP_SHIFT)  |
                 (rstb << CSL_EMIFA_CE3CFG_R_STROBE_SHIFT) |
                 (rhld << CSL_EMIFA_CE3CFG_R_HOLD_SHIFT)   |
                 (ta   << CSL_EMIFA_CE3CFG_TA_SHIFT);

        /* Write the device wait timing based on the instance selected        */
        switch (hNandObj->instanceId)
        {
            /* Configure the wait timing for chip select 3                    */
            case 0:
                hNandObj->regs->CE2CFG = defaultCfg;
                hNandObj->regs->CE4CFG = defaultCfg;
                hNandObj->regs->CE5CFG = defaultCfg;
                hNandObj->regs->CE3CFG &= CSL_EMIFA_CE3CFG_ASIZE_MASK;
                hNandObj->regs->CE3CFG |= timing;
                break;

            default:
                break;
        }
    }while (flag);
}

/**
 * \brief  Sets the data width in the EMIF registers.
 *
 *         This function sets data bus width bit of the AxCR register. This is a
 *         platform dependent code.
 *
 * \param  hNandObj     [IN]  Handle of the NAND LLC module instance
 * \param  dataWidth    [IN]  Width of the NAND data bus
 *
 * \return None.
 */
static Void LLC_nandSetDataWidth(LLC_nandObj *hNandObj, Uint8 dataWidth)
{
    /* Validate the input parameters */
    assert(NULL != hNandObj);

    hNandObj->regs->CE3CFG &= ~CSL_EMIFA_CE3CFG_ASIZE_MASK;
    hNandObj->regs->CE3CFG |= dataWidth;

}


/**
 * \brief  Resets a NAND device.
 *
 *         This function resets the NAND device by issuing the RESET command
 *         (0xFF) to the NAND device. It then waits until the NAND device is
 *         ready.
 *
 * \param  hNandObj     [IN]  Handle of the NAND LLC module instance
 *
 * \return   IOM_COMPLETED       - On success
 *           PSP_E_TIMEOUT       - If timeout happens.
 */
static Int32 LLC_nandResetDevice(LLC_nandObj  *hNandObj)
{
    Int32  result;  /* Result of reseting the NAND device */
    Bool   flag = TRUE;
    do
    {
        /* Initialize the local variables                                     */
        result = IOM_COMPLETED;
        flag = FALSE;
        /* Validate the input parameters                                      */
        assert(NULL != hNandObj);

        /* Issue the NAND reset command (0xFF)                                */
        LLC_nandWriteCommand(hNandObj, LLC_NAND_CMD_RESET);

        /* Wait until the NAND device is ready                                */
        result = LLC_nandWaitUntilReady(hNandObj);
    }while (flag);
    /* Return the result of reseting the NAND device */
    return (result);
}


/**
 * \brief  EDMA transfer completion callback function
 *
 *         This function is invoked when EDMA channel completes the transfer of
 *         data to/from the Nand device. This function releases the sync sema.
 *
 * \param  lch     [IN]  EDMA channel number
 * \param  status  [IN]  Status of transfer
 * \param  data    [IN]  Pointer to NAND LLC module instance
 */
static Void LLC_nandDmaCallback(Uint32                lch,
                                EDMA3_RM_TccStatus    status,
                                Ptr                   data)
{
    LLC_nandObj  *hNandObj;
    Bool         flag = TRUE;

    lch = lch;      /* to remove compilation warning */
    status = status;      /* to remove compilation warning */
    do
    {
        /* Validate the input parameters                                      */
        if(NULL == data)
        {
            return;
        }

        /* Initialize the local variables as required                         */
        hNandObj = (LLC_nandObj*)data;
        flag = FALSE;

        /* Complete the EDMA transfer completion steps                        */
        EDMA3_DRV_disableTransfer((Ptr)hNandObj->hEdma,
            hNandObj->edmaChannel,
            EDMA3_DRV_TRIG_MODE_MANUAL);

        EDMA3_DRV_setOptField(hNandObj->hEdma,
            hNandObj->edmaChannel,
            EDMA3_DRV_OPT_FIELD_TCINTEN,
            0u);

        EDMA3_DRV_clearErrorBits(hNandObj->hEdma, hNandObj->edmaChannel);

        /* Release the semaphore                                              */
        SEM_post(hNandObj->syncSem);
    }while (flag);
}

/**
 * \brief  Reads the contents of data area using EDMA channel
 *
 *         This function reads the contents of the data area of the page using
 *         EDMA channel.
 *
 * \param  hNandObj [IN]  Handle of the NAND LLC module instance
 * \param  data     [IN]  Data buffer pointer to which data is read
 *
 * \return   IOM_COMPLETED           - On success
 *           PSP_E_TIMEOUT           - If read of data area fails
 */
static Int32 LLC_nandEdmaRead(LLC_nandObj *hNandObj, Uint8 *data)
{
    EDMA3_DRV_Result        edmaResult;     /* EDMA Result            */
    EDMA3_DRV_ChainOptions  chainingOption; /* EDMA chaining options  */
    Int32                   result;         /* Result of EDMA write   */
    Bool                    semResult;
    EDMA3_DRV_FifoWidth     fifoWidth;      /* EDMA FIFO Width        */
    EDMA3_DRV_PaRAMRegs     edmaParam;      /* EDMA Parameters        */
    Bool                    flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert((NULL != hNandObj) && (NULL != data));
#ifdef NAND_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
        "NAND",
        PSP_DVT_DESC(PSP_DVT_eRCV_START,
                     PSP_DVT_dCHANNEL_ID_INST,
                     PSP_DVT_dPACKET_ID,
                     PSP_DVT_dNONE),
        CHANNELID_INSTANCEID(
            0,
            0),
        data
        );
#endif /* NAND_INSTRUMENTATION_ENABLED */
        /* Initialize the local variables as required                         */
        result = PSP_NAND_E_ERROR;
        flag = FALSE;
        /* Invalidate the cache                                               */
        BCACHE_wbInv((Ptr)data, 512u, TRUE);
        /* Determine the EDMA FIFO Width configuration based on the NAND
           bus width. */
        if (LLC_NAND_BW_8BIT == hNandObj->nandDevInfo->dataBusWidth)
        {
            /* NAND has 8-bit bus width. So FIFO width is 8-bits.             */
            fifoWidth = EDMA3_DRV_W8BIT;
        }
        else
        {
            /* NAND has 16-bit bus width. So FIFO width is 16-bits.           */
            fifoWidth = EDMA3_DRV_W16BIT;
        }

        /* DMA Transfer procedure: The ACNT is set to 8-Bytes and BCNT is set
           to 64. The channel is then chained to itself. This method of
           transferring 8-Bytes prevents the EDMA transfer controller from
           being locked up for the entire transfer and preventing higher
           priority transfers from completing. */

        /* Fill up the EDMA channel parameters                                */
        edmaParam.aCnt       = 8u;
        edmaParam.bCnt       = 64u;
        edmaParam.bCntReload = 0u;
        edmaParam.cCnt       = 1u;
        edmaParam.destAddr   = (Uint32)(data);
        edmaParam.destBIdx   = 8;
        edmaParam.destCIdx   = 0u;
        edmaParam.linkAddr   = 0xFFFFu;
        edmaParam.srcAddr    = (Uint32)(hNandObj->CExDATA);
        edmaParam.srcBIdx    = 0u;
        edmaParam.srcCIdx    = 0u;
        edmaParam.opt        = (0x00100000u | (fifoWidth << 8u)) |
                               (hNandObj->edmaTcc << 12u);
        edmaResult = EDMA3_DRV_setPaRAM (hNandObj->hEdma,
                        hNandObj->edmaChannel, &edmaParam);
        if (edmaResult != EDMA3_DRV_SOK)
        {
            /* EDMA configuration is not complete. Cannot continue.           */
            break;
        }
        /* Enable chaning with the channel itself                             */
        chainingOption.tcchEn   = EDMA3_DRV_TCCHEN_DIS;
        chainingOption.itcchEn  = EDMA3_DRV_ITCCHEN_EN;
        chainingOption.tcintEn  = EDMA3_DRV_TCINTEN_EN;
        chainingOption.itcintEn = EDMA3_DRV_ITCINTEN_DIS;
        edmaResult = EDMA3_DRV_chainChannel(hNandObj->hEdma,
                          hNandObj->edmaChannel,
                          hNandObj->edmaChannel,
                          &chainingOption);

        if (edmaResult != EDMA3_DRV_SOK)
        {
            /* EDMA configuration is not complete. Cannot continue.           */
            break;
        }

        /* Enable the EDMA transfer                                           */
        edmaResult = EDMA3_DRV_enableTransfer(hNandObj->hEdma,
                          hNandObj->edmaChannel,
                          EDMA3_DRV_TRIG_MODE_MANUAL);

        if (edmaResult != EDMA3_DRV_SOK)
        {
            /* EDMA transfer was not started                                  */
            break;
        }

        /* Wait until the EDMA transfer is complete                           */
        semResult = SEM_pend(hNandObj->syncSem, SYS_FOREVER);
        result = (semResult == TRUE) ? IOM_COMPLETED : PSP_NAND_E_ERROR;
    }while (flag);
#ifdef NAND_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
        "NAND",
        PSP_DVT_DESC(PSP_DVT_eRCV_END,
                     PSP_DVT_dCHANNEL_ID_INST,
                     PSP_DVT_dPACKET_ID,
                     PSP_DVT_dNONE),
        CHANNELID_INSTANCEID(
            0,
            0),
        data
        );
#endif /* NAND_INSTRUMENTATION_ENABLED */
    /* Return the result of reading data using EDMA channel                   */
    return (result);
}

/**
 * \brief  Writes the contents of data area using EDMA channel
 *
 *         This function writes the contents of the data area of the page using
 *         EDMA channel.
 *
 * \param  hNandObj [IN]  Handle of the NAND LLC module instance
 * \param  data     [IN]  Data buffer pointer from which data is written
 *
 * \return   IOM_COMPLETED           - On success
 *           PSP_E_TIMEOUT           - If read of data area fails
 */
static Int32 LLC_nandEdmaWrite(LLC_nandObj *hNandObj, Uint8 *data)
{
    EDMA3_DRV_Result        edmaResult;     /* EDMA Result            */
    EDMA3_DRV_ChainOptions  chainingOption; /* EDMA chaining options  */
    Int32                   result;         /* Result of EDMA write   */
    Bool                    semResult;
    EDMA3_DRV_FifoWidth     fifoWidth;      /* EDMA FIFO Width        */
    EDMA3_DRV_PaRAMRegs     edmaParam;      /* EDMA Parameters        */
    Bool                    flag = TRUE;
    do
    {
        /* Validate the input parameters                                      */
        assert((NULL != hNandObj) && (NULL != data));

#ifdef NAND_INSTRUMENTATION_ENABLED
        PSP_LOG_EVENT(&DVTEvent_Log,
            "NAND",
            PSP_DVT_DESC(PSP_DVT_eDATA_SND_START,
                         PSP_DVT_dCHANNEL_ID_INST,
                         PSP_DVT_dPACKET_ID,
                         PSP_DVT_dNONE),
            CHANNELID_INSTANCEID(
                1,
                0),
            data);
#endif /* NAND_INSTRUMENTATION_ENABLED */

        /* Initialize the local variables as required                         */
        result = PSP_NAND_E_ERROR;
        flag = FALSE;
        /* Invalidate the cache */
        BCACHE_wb ((Ptr)data, 512u, TRUE);
        /* Determine the EDMA FIFO Width configuration based on the NAND
           bus width. */
        if (LLC_NAND_BW_8BIT == hNandObj->nandDevInfo->dataBusWidth)
        {
            /* NAND has 8-bit bus width. So FIFO width is 8-bits.             */
            fifoWidth = EDMA3_DRV_W8BIT;
        }
        else
        {
            /* NAND has 16-bit bus width. So FIFO width is 16-bits.           */
            fifoWidth = EDMA3_DRV_W16BIT;
        }

        /* DMA Transfer procedure: The ACNT is set to 8-Bytes and BCNT is set
           to 64. The channel is then chained to itself. This method of
           transferring 8-Bytes prevents the transfer controller from
           locking up. */

        /* Fill up the EDMA channel parameters                                */
        edmaParam.aCnt       = 8u;
        edmaParam.bCnt       = 64u;
        edmaParam.bCntReload = 0u;
        edmaParam.cCnt       = 1u;
        edmaParam.destAddr   = (Uint32)(hNandObj->CExDATA);
        edmaParam.destBIdx   = 0u;
        edmaParam.destCIdx   = 0u;
        edmaParam.linkAddr   = 0xFFFFu;
        edmaParam.srcAddr    = (Uint32)(data);
        edmaParam.srcBIdx    = 8;
        edmaParam.srcCIdx    = 0u;
        edmaParam.opt        = (0x00100000u | (fifoWidth << 8u)) |
                               (hNandObj->edmaTcc << 12u);
        edmaResult = EDMA3_DRV_setPaRAM (hNandObj->hEdma,
                        hNandObj->edmaChannel,
                        &edmaParam);
        if (edmaResult != EDMA3_DRV_SOK)
        {
            /* EDMA configuration is not complete. Cannot continue.           */
            break;
        }
        /* Enable chaning with the channel itself                             */
        chainingOption.tcchEn   = EDMA3_DRV_TCCHEN_DIS;
        chainingOption.itcchEn  = EDMA3_DRV_ITCCHEN_EN;
        chainingOption.tcintEn  = EDMA3_DRV_TCINTEN_EN;
        chainingOption.itcintEn = EDMA3_DRV_ITCINTEN_DIS;
        edmaResult = EDMA3_DRV_chainChannel(hNandObj->hEdma,
                          hNandObj->edmaChannel,
                          hNandObj->edmaChannel,
                          &chainingOption);
        if (edmaResult != EDMA3_DRV_SOK)
        {
            /* EDMA configuration is not complete. Cannot continue.           */
            break;
        }

        /* Enable the EDMA transfer                                           */
        edmaResult = EDMA3_DRV_enableTransfer(hNandObj->hEdma,
                          hNandObj->edmaChannel,
                          EDMA3_DRV_TRIG_MODE_MANUAL);
        if (edmaResult != EDMA3_DRV_SOK)
        {
            /* EDMA transfer was not started                                  */
            break;
        }

        /* Wait until the EDMA transfer is complete                           */
        semResult = SEM_pend(hNandObj->syncSem, SYS_FOREVER);
        result = (semResult == TRUE) ? IOM_COMPLETED : PSP_NAND_E_WRITE_FAIL;

    }while (flag);

#ifdef NAND_INSTRUMENTATION_ENABLED
    PSP_LOG_EVENT(&DVTEvent_Log,
        "NAND",
        PSP_DVT_DESC(PSP_DVT_eDATA_SND_END,
                     PSP_DVT_dCHANNEL_ID_INST,
                     PSP_DVT_dPACKET_ID,
                     PSP_DVT_dNONE),
        CHANNELID_INSTANCEID(
            1,
            0),
        data);
#endif /* NAND_INSTRUMENTATION_ENABLED */

    /* Return the result of writting data using EDMA channel                  */
    return (result);
}


/**
 * \brief  Delay function for NAND device interface.
 *
 *         This function provides a delay functionality for the NAND device
 *         interface module. The parameter specified as delay is not related to
 *         any time. It is just a count to execute a dummy loop.
 *
 * \param  delay  [IN]  Number of cycles in the delay loop.
 *
 * \return None.
 */
static Void LLC_nandDelay(volatile Uint32 delay)
{
    while (delay > 0u)
    {
#ifdef USE_TI_TOOLS
        __asm("\tNOP");
#else
        __asm("NOP");
#endif
        delay--;
    }
}

/**\brief Writes a 8-bit portion of the NAND device address */
static inline Void LLC_nandWriteAddress(LLC_nandObj  *handle, Uint8 address)
{
    /* Validate the input parameters                                          */
    assert(NULL != handle);
    (*(volatile Uint8*)((handle)->CExALE) = (address));
}

/**\brief Writes a 8-bit NAND device command */
static inline Void LLC_nandWriteCommand(LLC_nandObj  *handle, Uint8 command)
{
    /* Validate the input parameters                                          */
    assert(NULL != handle);
    (*(volatile Uint8*)((handle)->CExCLE) = (command));
}

/**\brief Read the data bus */
static inline Uint16 LLC_nandReadData(LLC_nandObj  *handle)
{
    /* Validate the input parameters                                          */
    assert(NULL != handle);
    return (*(volatile Uint16*)((handle)->CExDATA));
}

/**\brief Read the data bus (only 8 LSB bits of the bus) */
static inline Uint8 LLC_nandReadData8Bit(LLC_nandObj  *handle)
{
    /* Validate the input parameters                                          */
    assert(NULL != handle);
    return (*(volatile Uint8*)((handle)->CExDATA));
}

/**\brief Write the data bus */
static inline Void LLC_nandWriteData(LLC_nandObj  *handle, Uint16 data)
{
    /* Validate the input parameters                                          */
    assert(NULL != handle);
    ((*(volatile Uint16*)((handle)->CExDATA)) = ((Uint16)(data)));
}


/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
