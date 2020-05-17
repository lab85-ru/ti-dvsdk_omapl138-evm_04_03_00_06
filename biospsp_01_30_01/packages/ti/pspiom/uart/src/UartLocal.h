/*
 * UartLocal.h
 *
 * UART Driver internal header file
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
 *  ======== ti/psp/uart/src/UartLocal.h ========
 *  Internal Header file
 */
#ifndef _UARTLOCAL_H_
#define _UARTLOCAL_H_

#if !defined(CHIP_C6747) && !defined(CHIP_OMAPL137) && !defined(CHIP_C6748) && !defined(CHIP_OMAPL138)
#error "No chip type defined! (Must use -DCHIP_C6747 or -DCHIP_OMAPL137 or
       -DCHIP_C6748 or -DCHIP_OMAPL138 )"
#endif

#if ((defined(CHIP_C6748) || defined(CHIP_OMAPL138)) && (!defined(BIOS_PWRM_ENABLE)))
#error "Use Bios PWRM module only for the SoC"
#elif defined(BIOS_PWRM_ENABLE)
    #include <pwrm.h>
#endif

#if ((defined(CHIP_C6747) || defined(CHIP_OMAPL137)) && defined(BIOS_PWRM_ENABLE))
#error "Do not use Bios PWRM module for this SoC"
#endif

#include <std.h>
#include <sem.h>
#include "ti/pspiom/cslr/cslr_uart.h"
#include "ti/pspiom/uart/Uart.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define Uart_TPOLL_MSECS                (1u)
/**< Poll time in milliSeconds                                                */

#define Uart_TXRX_ENABLE                (0xE003u)
/**< Mask to enable Rx and Tx                                                 */

#define Uart_TXRX_DISABLE               (0x0000u)
/**< Mask to edisable Rx and Tx                                               */

#define Uart_OPT_TCINTEN_SHIFT          (0x00000014u)
/**< Interrupt enable bit in OPT register for edma                            */

#define Uart_MAX_PWRM_EVENTS            (PWRM_INVALIDEVENT)
/**< Max number of PWRM events for which the uart will register               */

#define Uart_MAX_PWRM_CONSTRAINTS       (PWRM_DISALLOWEDSLEEPSTATE_MASK)
/**< Max number of constraints for which a moudule can register               */

/**
 *  \brief   Uart driver state enums used to track the driver state
 */
typedef enum Uart_DriverState
{
    Uart_DriverState_DELETED,
    Uart_DriverState_CREATED,
    Uart_DriverState_INITIALIZED,
    Uart_DriverState_OPENED,
    Uart_DriverState_CLOSED,
    Uart_DriverState_DEINITIALIZED,
    Uart_DriverState_POWERED_DOWN,
    Uart_DriverState_PWRM_SUSPEND
}Uart_DriverState;

typedef enum Uart_Intr
{
    Uart_Intr_RHR = 1u,
    /**< Data ready in receive register                                     */
    Uart_Intr_THR = 2u,
    /**< Transmitter empty interrupt                                        */
    Uart_Intr_RLS = 4u,
    /**< A line status changed or an error condition was detected           */
    Uart_Intr_MS  = 8u,
    /**< Modem status interrupt                                             */
    Uart_Intr_ALL = 0xFu
    /**< All interrupts */
}Uart_Intr;

/**
 *  \brief  Mode is INPUT or OUTPUT
 */
typedef enum Uart_IoMode {
    Uart_INPUT = 1,
    Uart_OUTPUT
}Uart_IoMode;

/**
 *  \brief  Internal data structure maintaining Uart device instance options
 */
typedef struct Uart_DevParams {
    Bool                fifoEnable;
    Bool                loopbackEnabled;
    Uart_BaudRate       baudRate;
    Uart_NumStopBits    stopBits;
    Uart_CharLen        charLen;
    Uart_Parity         parity;
    Uart_RxTrigLvl      rxThreshold;
    Uart_FlowControl    fc;
    Uint16              softTxFifoThreshold;
    Bool                pscPwrmEnable;
}Uart_DevParams;

/**
 *  \brief  Uart structure to hold the instance specific information.
 */
typedef struct Uart_HwInfo {
    CSL_UartRegsOvly    baseAddress;
    Uint32              cpuEventNumber;
    Uint32              rxDmaEventNumber;
    Uint32              txDmaEventNumber;
    Uint32              inputFrequency;
    Uint32              pwrmLpscId;
    Uint32              pscInstance;
}Uart_HwInfo;

#ifdef BIOS_PWRM_ENABLE
/**
 *  \brief  Structure for holding the PWRM related info
 */
typedef struct Uart_pwrmInfo_t
{
    Uart_pllDomain         pllDomain;
    /**< Pll domain to be used for the device                                 */

    Bool                   ioSuspend;
    /**< whether the driver is in IO suspend mode (PWRM change is in progress)*/

    Uint32                 constraintMask;
    /**< Constraint mask (for information only)                               */

    Uint32                 dependencyCount;
    /**< Current dependency count when the driver is going for sleep or
     * deepsleep                                                              */

    PWRM_Event             pwrmEvent;
    /**< Current pwrm event being processed                                   */

    PWRM_NotifyHandle      notifyHandle[Uart_MAX_PWRM_EVENTS];
    /**< Handles required for unregistering of the events with PWRM           */

    Fxn                    delayedCompletionFxn[Uart_MAX_PWRM_EVENTS];
    /**< delayed completion callback function pointer                         */

    PWRM_ConstraintHandle  constraintHandle[Uart_MAX_PWRM_CONSTRAINTS];
    /**< Handles required for unregistering of the constraints with PWRM      */
}Uart_pwrmInfo;
#endif

/**
 *  \brief  Structure of the channel object
 */
typedef struct Uart_ChanObj {
    Uart_DriverState        status;
    /**< Flag to tell channel object resource is used or free                 */

    Uart_IoMode             mode;
    /**< mode of the channel i.e INPUT or OUTPUT mode                         */

    IOM_TiomCallback        cbFxn;
    /**< Callback to the application                                          */

    Ptr                     cbArg;
    /**< Argument to be passed in the callback                                */

    QUE_Obj                 queuePendingList;
    /**< pending Iop List head                                                */

    IOM_Packet              *activeIOP;
    /**< Current Active Packet under progress in the channel                  */

    Uint8                   *activeBuf;
    /**< Buffer address of the current packet                                 */

    Uint32                  bytesRemaining;
    /**< Bytes remaining for the completion of the current packet             */

    Uint32                  chunkSize;
    /**< No. of bytes to be transferred per operation (i.e FIFO Size or 1)    */

    Ptr                     devHandle;
    /**< Handle to the Device                                                 */

    Uint32                  errors;
    /**< Counts how many errors encountered per IOP request                   */

    Ptr                     hEdma;
    /**< Handle to DMA object                                                 */

    Uint32                  edmaTcc;
    /**< Transfer complete code number                                        */

    Uint32                  edmaChId;
    /**< DMA Channel                                                          */

    Uint32                  edmaTC;
    /**< EDMA TC to be used                                                   */

    Bool                    ioCompleted;
    /**< To workaround the EDMA error interrupt issue                         */

    Bool                    optFlag;
    /**< Flag used to get paramSet register value in EDMA mode                */

    Uint32                  optValue;
    /**< Used to store opt value in PaRAM register                            */
    
    Uart_GblErrCallback     gblErrCbkFxn;
    /**< Function registered to notify of error conditions                    */
    
    Ptr                     gblErrCbkArg;
    /**< Callback argument to be used by the driver for the callback          */
}Uart_ChanObj;

/**
 *  \brief module-wide state
 *
 *  inUse in module state points to an array whose size is soc specific
 *  Pointer used to match IDriver codebase.
 */
typedef struct Uart_Module_State
{
    Bool *inUse;
    /**< Maintain inUse state of each uart device                             */
} Uart_Module_State;

/**
 *  \brief per-instance state
 */
typedef struct Uart_Object
{
    Uint32              instNum;
    /**< Instance number or device ID of the UART                             */

    Uart_OpMode         opMode;
    /**< Mode of operation                                                    */

    Uart_DevParams      devParams;
    /**< Uart device Params                                                   */

    Uart_DriverState    devState;
    /**< Driver State (deleted or created)                                    */

    Uart_ChanObj        xmtChanObj;
    /**< transmiiter channel                                                  */

    Uart_ChanObj        rcvChanObj;
    /**< receiver channel                                                     */

    Uart_HwInfo         deviceInfo;
    /**< Instance specific information                                        */

    Uint16              hwiNumber;
    /**< Hardware interrupt Number                                            */

    Bool                enableCache;
    /**< Submitted buffers are in cacheable memory                            */

    Uart_Stats          stats;
    /**< UART specific stats                                                  */

    Ptr                 txTskletHandle;
    /**< Tasklet(Software int) handle                                         */

    Ptr                 rxTskletHandle;
    /**< Tasklet(Software int) handle                                         */

    Uint32              polledModeTimeout;
    /**< Timeout used in polled mode - could be changed by an IOCTL           */

    SEM_Obj             syncSem;
    /**< sync semaphore object(used in the polled mode transfer for sync      *
     * between multiple tasks IO submit requests)                             */
     
    Bool                uartConfigured;
    /**< This boolean ensures that Uart is configured only once               */
#ifdef BIOS_PWRM_ENABLE    
    Uart_pwrmInfo       pwrmInfo;
#endif    
    /**< structure to hold the PWMR related information                       */
} Uart_Object;

/* -------- shared functions -------- */

typedef Void (*Uart_pspUartIsr)(Arg arg);

#ifdef Uart_EDMA_ENABLE
extern Void Uart_localIsrEdma(Uint32 tcc, EDMA3_RM_TccStatus status,
                              Ptr appData);

extern Int32 Uart_localStartEdmaTransfer(Uart_ChanObj *chanHandle, Uint32 cmd);
#endif

extern Int32 Uart_localLpscOn(Uart_Object *instHandle);
extern Int32 Uart_localLpscOff(Uart_Object *instHandle);
extern Void Uart_localCompleteCurrentIO (Uart_ChanObj *chanHandle);
extern Void Uart_loadPendedIops(Uart_ChanObj  *chanHandle);

#ifdef __cplusplus
}
#endif

#endif  /*_UARTLOCAL_H_ */

