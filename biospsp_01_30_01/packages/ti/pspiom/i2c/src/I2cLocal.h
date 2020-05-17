/*
 * I2cLocal.h
 *
 * I2c Driver internal header file
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
 *  ======== ti/pspiom/i2c/src/I2cLocal.h ========
 *
 *! Revision History
 *! ================
 *! 23-Sep-2008 connell created from PSP Team's BIOS 6 I2c module.
 */



#ifndef _I2CLOCAL_H_
#define _I2CLOCAL_H_

#include <std.h>
#include <sem.h>
#include <ti/pspiom/cslr/cslr_i2c.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(CHIP_C6747) && !defined(CHIP_OMAPL137) && !defined(CHIP_C6748) && !defined(CHIP_OMAPL138)
#error "No chip type defined! (Must use -DCHIP_C6747 or -DCHIP_OMAPL137 or -DCHIP_C6748 or -DCHIP_OMAPL138)"
#endif

#if ((defined(CHIP_C6748) || defined(CHIP_OMAPL138)) && (!defined(BIOS_PWRM_ENABLE)))
#error "Use Bios PWRM module only for the SoC"
#else
    #include <pwrm.h>
#endif

#if ((defined(CHIP_C6747) || defined(CHIP_OMAPL137)) && defined(BIOS_PWRM_ENABLE))
#error "Do not use Bios PWRM module for this SoC"
#endif
/* -------- internal defines --------                                         */

#define I2c_BUFFER_MAXLENGTH       0xFFFFu
#define I2c_BUS_BUSY_TIMEOUT_CNT   10000000u
#define I2c_BUS_BUSY_TIMEOUT_MSECS 10u
#define I2c_ICIVR_INTCODE_ARDY     0x00000004u
#define I2c_NUMBITS_MASK           0x7u
#define I2c_NUM_CHANS              5
#define I2c_OPT_TCINTEN_SHIFT      0x00000014u
#define I2c_POLLED_TIMEOUT         0x000FFFFFu
#define I2c_SLAVEADRR_10BIT        0x3FFu
#define I2c_VALUE_RESTORE          0x0000D1FFu

#define I2c_MAX_PWRM_EVENTS        (PWRM_INVALIDEVENT)
/**< Max number of PWRM events for which the i2c will register                */

#define I2c_MAX_PWRM_CONSTRAINTS   (PWRM_DISALLOWEDSLEEPSTATE_MASK)
/**< Max number of constraints for which a moudule can register               */

#define I2c_MAX_CHAN_PRIORITY       16
/**< This priority is defined so that any channel which is created by the ISR
 * can have a maximum priority of 16, the bios task will then only have a
 * maximum priority of 15.Hence the channels submitting requests from ISR 
 * will have more priority always during IO                                   */


/* -------- internal typedef enums --------                                   */

typedef enum I2c_DriverState {
    I2c_DriverState_DELETED,
    I2c_DriverState_CREATED,
    I2c_DriverState_INITIALIZED,
    I2c_DriverState_OPENED,
    I2c_DriverState_CLOSED,
    I2c_DriverState_DEINITIALIZED,
    I2c_DriverState_POWERED_DOWN,
    I2c_DriverState_PWRM_SUSPEND
}I2c_DriverState;

/* -------- internal typedefs --------                                        */

typedef Void (*I2c_isr)(Ptr);

/* -------- structs --------                                                  */

/**
 * \brief   Strcuture to hold the information related to the PWRM.
 */
typedef struct I2c_pwrmInfo_t
{
    I2c_pllDomain          pllDomain;
    /**< Pll domain to be used for the device                                 */

    Bool                   ioSuspend;
    /**< whether the driver is in IO suspend mode (PWRM change is in progress)*/

    PWRM_Event             pwrmEvent;
    /**< Current pwrm event being processed                                   */

    Uint32                 constraintMask;
    /**< Constraint mask (for information only)                               */

    Uint32                 dependencyCount;
    /**< Current dependency count when the driver is going for sleep or
     * deepsleep                                                              */

    PWRM_NotifyHandle      notifyHandle[I2c_MAX_PWRM_EVENTS];
    /**< Handles required for unregistering of the events with PWRM           */

    Fxn                    delayedCompletionFxn[I2c_MAX_PWRM_EVENTS];
    /**< delayed completion callback function pointer                         */

    PWRM_ConstraintHandle  constraintHandle[I2c_MAX_PWRM_CONSTRAINTS];
    /**< Handles required for unregistering of the constraints with PWRM      */

}I2c_pwrmInfo;

/**
 * \brief   Strcuture to save the i2c context during PWRM operations
 */
typedef struct I2c_Context_t
{
    volatile Uint32  icmdr;
    volatile Uint32  psc;
    volatile Uint32  clkh;
    volatile Uint32  clkl;
    volatile Uint32  oar;
    volatile Uint32  icemdr;
}I2c_Context;
    

typedef struct I2c_ChanObj
{
    /*  INPUT or OUTPUT or INOUT                                              */
    Uint32 mode;

    /*  Callback Function for stream                                          */
    IOM_TiomCallback cbFxn;

    /*  argument for cbFxn()                                                  */
    Ptr cbArg;

    /*  I2c Handle                                                            */
    Ptr instHandle;

    /* Own address (7 or 10 bit)                                              */
    Uint32 ownAddr;

    /* I2C Bus Frequency                                                      */
    Uint32 busFreq;

    /* Number of bits/byte to be sent/received                                */
    Uint32 numBits;

    /* 7bit/10bit Addressing mode                                             */
    Bool addressing;

    /* Digital Loob Back (DLB) mode                                           */
    Bool loopbackEnabled;

    /* Shows whether io is in pending state or not                            */
    Bool pendingState;

    /* Shows whether IO has to cancel or not                                  */
    Bool abortAllIo;

    /*current IO packet                                                       */
    IOM_Packet* activeIOP;

    /*current IO packet                                                       */
    I2c_DataParam dataParam;

    /* Error code                                                             */
    Int32 currError;

    /* Current Flags for read/write                                           */
    Uint32 currFlags;

    /* User buffer for read/write                                             */
    Uint8* currBuffer;

    /* User buffer length                                                     */
    Uint32 currBufferLen;

    /* pending Iop List head                                                  */
    QUE_Obj queuePendingList;

    /* this stores temp ICMDR value from input flags                          */
    Uint32 tempICMDRValue;

    /*this will hold the priority of the task that created this channel       */
    Uint32 taskPriority;

    I2c_DriverState channelState;
    /**< Current state of the I2C channel (OPENED or CLOSED)                  */

    I2c_CommMode masterOrSlave;
    /**< Current operational mode of the driver(MASTER or SLAVE)              */ 

    Uint32       tempCount;
    /**< Variable to hold the actual count of the data that could not be
     * accomodated in the application supplied buffer                         */
}I2c_ChanObj;

typedef struct I2c_HwInfo
{
    CSL_I2cRegsOvly     baseAddress;
    Uint32              cpuEventNumber;
    Uint32              rxDmaEventNumber;
    Uint32              txDmaEventNumber;
    Uint32              inputFrequency;
    Uint32              pwrmLpscId;
    Uint32              pscInstance;
}I2c_HwInfo;

/* -------- module-wide state --------
 *
 * inUse in module state points to an array whose size is soc specific
 * Pointer used to match IDriver codebase.
 */

typedef struct I2c_Module_State {
    Bool *inUse;
}I2c_Module_State;

/* -------- per-instance state --------                                       */

typedef struct I2c_Object {
    Uint32          instNum;
    I2c_DriverState devStatus;
    I2c_OpMode      opMode;
    I2c_ChanObj     chans[I2c_NUM_CHANS];
    Ptr             hwEventCallback;
    Ptr             hEdma;
    Bool            dmaChaAllocated;
    Int32           edma3EventQueue;
    Uint16          hwiNumber;
    Bool            enableCache;
    I2c_Stats       stats;
    I2c_HwInfo      deviceInfo;
    Uint32          polledModeTimeout;
    Uint32          numOpens;
    I2c_ChanObj*    currentActiveChannel;
    SEM_Obj         syncSem;

    Bool            pscPwrmEnable;
    /**< option to enable disbale the power management support for the driver */

    I2c_pwrmInfo    pwrmInfo;
    /**< structure to hold the PWMR related information                       */
    
    I2c_Context saveContext;
    /**< structure to hold the saved context during PWRM operations           */
} I2c_Object;

/* -------- shared functions --------                                         */

extern Int32 I2c_localEnsureIsReadyforNext(I2c_Object *instHandle);
extern Void I2c_localGetNextChannel(I2c_Object *instHandle, I2c_ChanObj**pChanHandle);
extern Void I2c_localPrepareForTransfer(I2c_ChanObj *chanHandle);
extern Void I2c_localEdmaChanRequest(I2c_Object *instHandle);
extern Int  I2c_localEdmaTransfer(I2c_Object *instHandle,Uint32 flags);
extern Void I2c_localGetNextChannel(I2c_Object *instHandle, I2c_ChanObj**pChanHandle);
extern Void I2c_localPrepareForTransfer(I2c_ChanObj *chanHandle);
extern Void I2c_completeIOedmaCallback (I2c_Object *instHandle);
extern Int32 I2c_localLpscOn(I2c_Object *instHandle);
extern Int32 I2c_localLpscOff(I2c_Object *instHandle);
extern Void I2c_loadPendedIops(I2c_Object *instHandle);
#ifdef __cplusplus
}
#endif /* extern "C"                                                          */

#endif /* _I2CLOCAL_H_                                                        */

