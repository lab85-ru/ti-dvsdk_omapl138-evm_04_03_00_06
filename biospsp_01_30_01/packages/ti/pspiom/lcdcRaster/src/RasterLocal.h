/*
 * RasterLocal.h
 *
 * LCDC Raster Driver internal header file
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
 * \file       Rasterlocal.h
 * \brief      This file declares the data structures and constants used by the
 *             LCDC raster controller driver.
 *
 * \author     PSG, Steve Connell, Madhvapathi Sriram
 * \version    0.1  created from IDriver Raster.c
 */



#ifndef _RASTERLOCAL_H_
#define _RASTERLOCAL_H_

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

#define RASTER_DEV_CLKDIV_MAX           0xffu
/**< Maximum divisor value                                                    */

#define RASTER_DEV_CLKDIV_MIN           0x2u
/**< Minimum divisor value                                                    */

#define RASTER_DMA_FIFO_DELAY_MAX       0xffu
/**< Maximum fifo dma delay                                                   */

#define RASTER_DMA_FIFO_DELAY_MIN       0x0
/**< Minimum fifo dma delay                                                   */

#define RASTER_ACB_FREQUENCY_MAX        0xffu
/**< Highest ACB frequency                                                    */

#define RASTER_ACB_FREQUENCY_MIN        0x0
/**< Lowest ACB frequency                                                     */

#define RASTER_ACB_I_MAX                0xfu
/**< Highest ACB transitions for interrupt value                              */

#define RASTER_ACB_I_MIN                0x0
/**< Lowest ACB transitions for interrupt value                               */

#define RASTER_SUBPANEL_DPD_MAX         0xfffu
/**< Highest DPD value                                                        */

#define RASTER_SUBPANEL_DPD_MIN         0x0
/**< Lowest DPD value                                                         */

#define RASTER_SUBPANEL_LPPT_MAX        0x3ffu
/**< Highest LPPT value                                                       */

#define RASTER_TIMING_HBP_MAX           0xffu
/**< Highest hbp value                                                        */

#define RASTER_TIMING_HBP_MIN           0x0
/**< Lowest hbp value                                                         */

#define RASTER_TIMING_HFP_MAX           0xffu
/**< Highest hfp value                                                        */

#define RASTER_TIMING_HFP_MIN           0x0
/**< Lowest hfp value                                                         */

#define RASTER_TIMING_HSPW_MAX          0x3fu
/**< Highest hspw value                                                       */

#define RASTER_TIMING_HSPW_MIN          0x0
/**< Lowest hspw value                                                        */

#define RASTER_TIMING_LPP_MAX           0x3ffu
/**< Highest lpp value                                                        */

#define RASTER_TIMING_LPP_MIN           0x0
/**< Lowest lpp value                                                         */

#define RASTER_TIMING_PPL_MAX           0x3ffu
/**< Highest ppl value                                                        */

#define RASTER_TIMING_PPL_MIN           0x0
/**< Lowest ppl value                                                         */

#define RASTER_TIMING_VBP_MAX           0xffu
/**< Highest vbp value                                                        */

#define RASTER_TIMING_VBP_MIN           0x0
/**< Lowest vbp value                                                         */

#define RASTER_TIMING_VFP_MAX           0xffu
/**< Highest vfp value                                                        */

#define RASTER_TIMING_VFP_MIN           0x0
/**< Lowest vfp value                                                         */

#define RASTER_TIMING_VSPW_MAX          0x3fu
/**< Highest vspw value                                                       */

#define RASTER_TIMING_VSPW_MIN          0x0
/**< Lowest vspw value                                                        */

#define RASTER_NUM_CHAN_PER_INST        0x1u
/**< Maximum number of channels per instance                                  */

#define Raster_MAX_PWRM_EVENTS             (PWRM_INVALIDEVENT)
/**< Maximum PWRM events that can be occuring for Raster                      */

#define Raster_MAX_PWRM_CONSTRAINTS        (PWRM_DISALLOWEDSLEEPSTATE_MASK)
/**< Maximum PWRM constraints that can be registered for Raster               */

/**
 * \brief DevType
 *
 * Enumerates the type of underlying device the driver is interacting with
 */
typedef enum Raster_DevType {

    Raster_DevType_UNDEFINED = 0x0,
    /**< Undefined. To be used only by driver                                 */

    Raster_DevType_LCDC = 0x1u
    /**< This is LCDC device                                                  */

}Raster_DevType;

/**
 * \brief DmaConfCmd
 *
 * Enumerates the sub-commands for the DMA Configuration operation (command)
 */
typedef enum Raster_DmaConfCmd {
    Raster_DmaConfCmd_SET_UNDEFINED = 0x0,
    /**< Undefined. To be used only by driver                               */
    Raster_DmaConfCmd_SET_FB_MODE = 0x1u,
    /**< Set the Frame buffer mode Single/Double                            */
    Raster_DmaConfCmd_SET_BURST_SIZE = 0x2u,
    /**< Set the burst size                                                 */
    Raster_DmaConfCmd_SET_EOF_INT = 0x3u
    /**< Set/Clear the End-Of-Frame interrupt                               */
}Raster_DmaConfCmd;

/**
 *  \brief DriverState
 *
 *  Lcdc driver state enums used to track the driver state
 */
typedef enum Raster_DriverState {
    Raster_DriverState_DELETED,
    Raster_DriverState_CREATED,
    Raster_DriverState_INITIALIZED,
    Raster_DriverState_OPENED,
    Raster_DriverState_CLOSED,
    Raster_DriverState_DEINITIALIZED
}Raster_DriverState;

/**
 * \brief RasterSubpanelCmd
 *
 * Enumerates the sub-commands for the RasterSubpanel Configuration command
 */
typedef enum Raster_SubpanelCmd {

    Raster_SubpanelCmd_SET_UNDEFINED = 0x0,
    /* Undefined. To be used only by driver                                */
    Raster_SubpanelCmd_SET_SPEN = 0x1u,
    /* Enable the subpanel feature                                         */
    Raster_SubpanelCmd_SET_POSITION = 0x2u,
    /* Set the position of the subpanel                                    */
    Raster_SubpanelCmd_SET_LPPT = 0x3u,
    /* Set the threshold values                                            */
    Raster_SubpanelCmd_SET_DATA = 0x4u
    /* Set the data value                                                  */
}Raster_SubpanelCmd;

/**
 * \brief ChanObject
 *
 * Common structure for an LCDC channel information. All the channel
 * instances will be in contiguous memory
 */
typedef struct Raster_ChanObject {

    Uint32 dmaThrsld;
    /**< Higher threshold value for dma transfer                              */

    Uint32 devInst;
    /**< Instance of the device on which this channel is active               */

    Raster_Controller type;
    /**< Type of the channel                                                  */

    Raster_DriverState state;
    /**< Current state of this channel                                        */

    Raster_RasterConf chanConf;
    /**< Current configuration of this channel                                */

    IOM_TiomCallback appCb;
    /**< Application callback registered with this channel                    */

    Ptr cbArg;
    /**< Application callback registered with this channel                    */

    QUE_Obj activeQ;
    /**< Active queue associated with this channel                            */

    Uint32 activePktCount;
    /**< Active packet count with the driver                                  */

    QUE_Obj pendingQ;
    /**< Pending queue associated with this channel                           */

    Uint32 pendingPktCount;
    /**< Pending packet count with the driver                                 */

    Uint32 submitCount;
    /**< The nummber for frame buffers currently submitted                    */

    Bool enabled;
    /**< Running/Idle state of the channel                                    */

    Uint32 fbSize;
    /**< Size of the frame buffer associated with this channel                */

    Ptr instHandle;
    /**< Pointer to instance object with which this channel is associated     */

    Int segId;
    /**< Segment ID to heap from where allocation has to be done on
    * application's behalf for ALLOC_FB calls
    */

    Raster_ChanParams chanParams;
    /**< This structure stores the create time information for later use      */

}Raster_ChanObject;

typedef struct Raster_HwInfo
{
    CSL_LcdcRegsOvly    baseAddress;
    /**< Base address for this module                                         */

    Uint32              cpuEventNumber;
    /* Module's CPU event number                                              */

    Uint32              inputFrequency;
    /**< input frequency                                                      */

    Uint32              hwiNumber;
    /**< Interrupt Number for the ECM where this module event is avaliable    */

    Uint32              pwrmLpscId;
    /**< LPSC number for this instance                                        */

    Uint32              pscInstance;
    /**< Instance of the PSC which controls this module's instance            */
}Raster_HwInfo;

#ifdef BIOS_PWRM_ENABLE
typedef struct Raster_pwrmInfo_t
{
    Raster_pllDomain       pllDomain;
    /**< Pll domain to be used for the device                                 */

    Bool                   ioSuspend;
    /**< whether the driver is in IO suspend mode (PWRM change is in progress)*/

    PWRM_Event             pwrmEvent;
    /**< Current pwrm event being processed                                   */

    PWRM_NotifyHandle      notifyHandle[Raster_MAX_PWRM_EVENTS];
    /**< Handles required for unregistering of the events with PWRM           */

    Fxn                    delayedCompletionFxn[Raster_MAX_PWRM_EVENTS];
    /**< delayed completion callback function pointer                         */

    PWRM_ConstraintHandle  constraintHandle[Raster_MAX_PWRM_CONSTRAINTS];
    /**< Handles required for unregistering of the constraints with PWRM      */
}Raster_pwrmInfo;
#endif

/* -------- module-wide state -------- */

typedef struct Raster_Module_State {
    Bool *inUse;
}Raster_Module_State;

/* -------- per-instance state -------- */


/**
 * \brief Raster_Object
 *
 * Lcdc Raster driver instance object.
 */
typedef struct Raster_Object {

    Uint32 chCount;
    /**< Total number of channels opened                                      */

    Uint32 instNum;
    /**< Instance number of this device                                       */

    Raster_DevType type;
    /**< Device type                                                          */

    Raster_DriverState state;
    /**< Current state of the device                                          */

    Raster_DeviceConf devConf;
    /**< Current configuration of this device                                 */

    Raster_EventStat eventStat;
    /**< Pointer to the event status ds                                       */

    Raster_ChanObject ChannelObj;
    /**< The Channel opened on this device                                    */

    Raster_HwInfo instHwInfo;
    /**< Hardware configuration                                               */
#ifdef BIOS_PWRM_ENABLE    
    Raster_pwrmInfo pwrmInfo;
    /**< PWRM related information                                             */
#endif    
}Raster_Object;

#ifdef __cplusplus
}
#endif /* extern "C"                                                          */

#endif /* _RASTERLOCAL_H_                                                     */
