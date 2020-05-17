/*
 * LiddLocal.h
 *
 * Lidd Driver internal header file
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
 *  ======== ti/pspiom/lcdclidd/src/LiddLocal.h ========
 *
 *! Revision History
 *! ================
 *! 19-Dec-2008 connell created from PSP Team's BIOS 6 Lidd module.
 */

#ifndef _LIDDLOCAL_H_
#define _LIDDLOCAL_H_

#include <std.h>
#include <ti/pspiom/cslr/cslr_lcdc.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(CHIP_C6747) && !defined(CHIP_OMAPL137) && !defined(CHIP_C6748) && !defined(CHIP_OMAPL138)
#error "No chip type defined! (Must use -DCHIP_C6747 or -DCHIP_OMAPL137 or -DCHIP_C6748 or -DCHIP_OMAPL138 )"
#endif

#if (defined(CHIP_C6748) || defined(CHIP_OMAPL138)) && !defined(BIOS_PWRM_ENABLE)
#error "Use Bios PWRM module only for the SoC"
#elif defined(BIOS_PWRM_ENABLE)
    #include <pwrm.h>
#endif

#if (defined(CHIP_C6747) || defined(CHIP_OMAPL137)) && defined(BIOS_PWRM_ENABLE)
#error "Do not use Bios PWRM module for this SoC"
#endif

/* -------- constants -------- */

/** Maximum clock divisor value                                               */
#define Lidd_DEV_CLKDIV_MAX     0xff
/**  Number of channels supported per driver instance                         */
#define Lidd_NUM_CHAN_PER_INST  1
/** The maximum number of chracters per line                                  */
#define Lidd_NUM_CHAR_PER_LINE  24
/** The maximum number of chracters per line                                  */
#define Lidd_NUM_LINE_PER_PANEL 2
/** The maximum number of instances supported by the driver                   */
#define Lidd_numInstances       1
/** Maximum PWRM events that can be occuring for Raster                      */
#define Lidd_MAX_PWRM_EVENTS             (PWRM_INVALIDEVENT)
/** Maximum PWRM constraints that can be registered for Raster               */
#define Lidd_MAX_PWRM_CONSTRAINTS        (PWRM_DISALLOWEDSLEEPSTATE_MASK)



/* -------- enums -------- */

/**
* \brief DevType
*
* Enumerates the type of underlying device the driver is interacting with
*/
typedef enum Lidd_DevType {
    /* Undefined. To be used only by driver   */
    Lidd_DevType_UNDEFINED = 0x0,
    /* This is LCDC device                    */
    Lidd_DevType_LCDC = 0x1
}Lidd_DevType;

/**
 * \brief DriverState
 *
 *  Lidd driver state enums used to track the driver state
 */
typedef enum Lidd_DriverState {
    Lidd_DriverState_DELETED,
    Lidd_DriverState_CREATED,
    Lidd_DriverState_INITIALIZED,
    Lidd_DriverState_OPENED,
    Lidd_DriverState_CLOSED,
    Lidd_DriverState_DEINITIALIZED,
    Lidd_DriverState_POWERED_DOWN,
    Lidd_DriverState_PWRM_SUSPEND
}Lidd_DriverState;

/* -------- structs -------- */

/**
 * \brief ChanObject
 *
 * Common structure for an LCDC channel information. All the channel
 * instances will be in contiguous memory.
 */
typedef struct Lidd_ChanObject {
    /**< Instance of the device on which this channel is active             */
    Uint32 devInst;
    /**< Current state of this channel                                      */
    Lidd_DriverState state;
    /**< Current configuration of this channel                              */
    Lidd_DisplayConf chanConf;
    /**< Application callback registered with this channel                  */
    IOM_TiomCallback appCb;
    /**< Current Line the cursor is in of the cursor along the line          */
    Uint32 currPos;
    /**< Current settings for the display                                    */
    Uint32 currSetting;
    /**< Current entry mode for the display                                  */
    Uint32 entryMode;
    /**< Current display/cursor shift  settings                              */
    Uint32 currShift;
    /**< Current cursor/display/blink on/off setting                        */
    Uint32 onOffSetting;
    /**< Application callback registered with this channel                  */
    Ptr    cbArg;
    /**< Active queue associated with this channel                          */
    QUE_Obj activeQ;
    /**< The nummber for request currently submitted                        */
    Uint32 submitCount;
    /**< Running/Idle state of the channel                                  */
    Bool enabled;
    /**< Pointer to instance object with which this channel is associated   */
    Ptr instHandle;
    /**< Current active IO Request in the driver for this channel           */
    IOM_Packet *activeIOP;
}Lidd_ChanObject;

/**
 * \brief HW info structure
 *
 *  Lidd structure to hold the instance specific information.
 */
typedef struct Lidd_HwInfo
{
    CSL_LcdcRegsOvly    baseAddress;
    /**< The base address of the instance register overlay                    */

    Int32               cpuEventNumber;
    /**< CPU event number associated with this instance                       */

    Int32               inputFrequency;
    /**< The system input frequency to this instance                          */
    
    Int32               hwiNumber;
    /**< The HWI number assigned to the event associated with this instance   */
    
    Uint32              pwrmLpscId;
    /**< LPSC number for this instance                                        */

    Uint32              pscInstance;
    /**< Instance of the PSC which controls this module's instance            */

}Lidd_HwInfo;

#ifdef BIOS_PWRM_ENABLE
typedef struct Lidd_pwrmInfo_t
{
    Lidd_pllDomain         pllDomain;
    /**< Pll domain to be used for the device                                 */

    Bool                   ioSuspend;
    /**< whether the driver is in IO suspend mode (PWRM change is in progress)*/

    PWRM_Event             pwrmEvent;
    /**< Current pwrm event being processed                                   */

    PWRM_NotifyHandle      notifyHandle[Lidd_MAX_PWRM_EVENTS];
    /**< Handles required for unregistering of the events with PWRM           */

    Fxn                    delayedCompletionFxn[Lidd_MAX_PWRM_EVENTS];
    /**< delayed completion callback function pointer                         */

    PWRM_ConstraintHandle  constraintHandle[Lidd_MAX_PWRM_CONSTRAINTS];
    /**< Handles required for unregistering of the constraints with PWRM      */
}Lidd_pwrmInfo;
#endif

/* -------- module-wide state -------- */

typedef struct Lidd_Module_State {
    Bool *inUse;
}Lidd_Module_State;

/* -------- per-instance state -------- */

/**
 * \brief Lidd_Object
 *
 * Lidd driver instance object.
 */
typedef struct Lidd_Object {
    /**< Total number of channels opened                                    */
    Uint32 chCount;
    /**< Instance number of this device                                     */
    Uint32 instNum;
    /**< Type of the device                                                 */
    Lidd_DevType devType;
    /**< Current state of the device                                        */
    Lidd_DriverState state;
    /**< Current configuration of this device                               */
    Lidd_DeviceConf devConf;
    /**< The Channel opened on this device                                  */
    Lidd_ChanObject ChannelObj;
    /**< Hardware configuration                                             */
    Lidd_HwInfo instHwInfo;
#ifdef BIOS_PWRM_ENABLE    
    /**< Power management information                                       */
    Lidd_pwrmInfo pwrmInfo;
#endif    
}Lidd_Object;

Void Lidd_init(Void);

#ifdef __cplusplus
}
#endif /* extern "C"                                                          */

#endif /* _LiddLocal_H_                                                       */

