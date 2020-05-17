/*
 * SpiLocal.h
 *
 * SPI Driver internal header file
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
 *  ======== ti/psp/spi/src/SpiLocal.h ========
 */
#ifndef _SPILOCAL_H_
#define _SPILOCAL_H_

#if !defined(CHIP_C6747) && !defined(CHIP_OMAPL137) && !defined(CHIP_C6748) && !defined(CHIP_OMAPL138)
#error "No chip type defined! (Must use -DCHIP_C6747 or -DCHIP_OMAPL137 or -DCHIP_C6748 or -DCHIP_OMAPL138)"
#endif

#include <std.h>
#include <iom.h>
#include <que.h>
#include <sem.h>
#include <ti/pspiom/spi/Spi.h>
#include <ti/pspiom/cslr/cslr_spi.h>

#if ((defined(CHIP_C6748) || defined(CHIP_OMAPL138)) && (!defined(BIOS_PWRM_ENABLE)))
#error "Use Bios PWRM module only for the SoC"
#elif defined(BIOS_PWRM_ENABLE)
    #include <pwrm.h>
#endif

#if ((defined(CHIP_C6747) || defined(CHIP_OMAPL137)) && defined(BIOS_PWRM_ENABLE))
#error "Do not use Bios PWRM module for this SoC"
#endif

#ifdef __cplusplus
extern "C"
{
#endif
/* -------- constants -------- */

#define Spi_ERROR_BASE                  (-11)
/**< Base error code                                                          */

#define Spi_RECEIVE_OVERRUN_ERR         ((-11) - 1)
/**< SPI receive overrun error code                                           */

#define Spi_BIT_ERR                     ((-11) - 2)
/**< SPI Bit transfer error code                                              */

#define Spi_DESYNC_ERR                  ((-11) - 3)
/**< SPI Slave desync error code                                              */

#define Spi_PARITY_ERR                  ((-11) - 4)
/**< SPI parity error code                                                    */

#define Spi_TIMEOUT_ERR                 ((-11) - 5)
/**< SPI time out error code                                                  */

#define Spi_DATALENGTH_ERR              ((-11) - 6)
/**< SPI Data length  error code                                              */

#define Spi_CANCEL_IO_ERROR             ((-11) - 7)
/**< SPI Cancel IO error code                                                 */

#define Spi_MAX_CHAN_PRIORITY           16
/**< This priority is defined so that any channel which is created by the ISR
 * can have a maximum priority of 16, the bios task will then only have a
 * maximum priority of 15.Hence the channels submitting requests from ISR 
 * will have more priority always during IO                                   */


#define Spi_MAX_CHIPSELECT              (3u)
/**< Max chip select numbers                                                  */

#define Spi_INTERRUPT_MASK              (CSL_SPI_SPIINT0_BITERRENA_MASK   \
                                        | CSL_SPI_SPIINT0_TIMEOUTENA_MASK \
                                        | CSL_SPI_SPIINT0_OVRNINTENA_MASK \
                                        | CSL_SPI_SPIINT0_DESYNCENA_MASK  \
                                        | CSL_SPI_SPIINT0_PARERRENA_MASK  \
                                        | CSL_SPI_SPIINT0_RXINTENA_MASK   \
                                        | CSL_SPI_SPIINT0_TXINTENA_MASK)

/**< SPI Interrupt register mask                                              */

#define Spi_DATAFORMAT_VALUE            (4u)
/**< it shows spi supports maximum value of data format register.             */

#define Spi_SPIDAT1_CSHOLD_8BIT_MASK    (0x10u)
/**< SPI CSHOLD MASK 8 bit                                                    */

#define Spi_RX_EDMA_CALLBACK_OCCURED    (0x2u)
/**< Flag to indicate that Reception EDMA callback has occured                */

#define Spi_TX_EDMA_CALLBACK_OCCURED    (0x1u)
/**< Flag to indicate that Transmission EDMA callback has occured             */

#define Spi_EDMA_8_BIT_CHAR_LEN         (8u)
/**< SPI 8 bit character length                                               */

#define Spi_ACNT_FLAG                   (2u)
/**< CNT flag to indicate in EDMA Transfer                                    */

#define Spi_DEST_INDEX_FLAG             (1u)
/**< Destination flag to indicate in EDMA Transfer                            */

#define Spi_OPT_TCINTEN_SHIFT           (0x00000014u)
/**< Interrupt enable bit in OPT register for edma                            */

#define Spi_MAX_PWRM_EVENTS             (PWRM_INVALIDEVENT)


#define Spi_MAX_PWRM_CONSTRAINTS        (PWRM_DISALLOWEDSLEEPSTATE_MASK)
/* -------- enums -------- */

/**
 *  \brief Spi driver state
 *
 *  SPi driver state enums used to track the driver state.
 */

typedef enum Spi_DriverState {
    Spi_DriverState_DELETED,
    Spi_DriverState_CREATED,
    Spi_DriverState_INITIALIZED,
    Spi_DriverState_OPENED,
    Spi_DriverState_CLOSED,
    Spi_DriverState_DEINITIALIZED,
    Spi_DriverState_POWERED_DOWN,
    Spi_DriverState_PWRM_SUSPEND
}Spi_DriverState;

/* -------- structs -------- */

/**
 *  \brief channel structure
 *
 *  There is one ChanObj per direction.  This mini-driver must be
 *  opened for input and output separately.
 */
typedef struct Spi_ChanObj
{
    Int32                  mode;
    /**< channel's mode of operation(Input or output)                         */

    Spi_DriverState         channelState;
    /**< state of the SPI Either created or deleted                           */

    IOM_TiomCallback        cbFxn;
    /**< to notify client when I/O complete                                   */

    Ptr                     cbArg;
    /**<  argument for cbFxn()                                                */

    Ptr                     instHandle;
    /**< Spi Handle to access the spi params                                  */

    Uint32                  busFreq;
    /**< SPI Bus Frequency                                                    */

    Bool                    loopbackEnabled;
    /**< Enable/Disable loop back mode                                        */

    Bool                    charLength16Bits;
    /**< Flag to indicate if char length greater than 8 bits                  */

    IOM_Packet             *activeIOP;
    /**< Current IO packet                                                    */

    Spi_DataParam           dataParam;
    /**< Current IO packet                                                    */

    Bool                    pendingState;
    /**< Shows whether io is in pending state or not                          */

    Bool                    abortAllIo;
    /**< Shows whether IO has to cancel or not                                */

    Int32                   currError;
    /**< current error flag                                                   */

    Uint8                  *txBuffer;
    /**< User(or driver) buffer for TX operation of transceive operation      */

    Uint8                  *rxBuffer;
    /**< User(or driver) buffer for RX operation of transceive operation      */

    QUE_Obj                 queuePendingList;
    /**< pending Iop List head                                                */

    Int32                   taskPriority;
    /**<this will hold the priority of the task that created this channel     */

    Uint32                  txBufferLen;
    /**< Length of the TX buffer                                              */

    Uint32                  rxBufferLen;
    /**< Length of the RX buffer                                              */

    Bool                    txBufFlag;
    /**< Flag to indicate if the TX buffer is suppiled by user or is NULL     *
     * (TRUE if user has supplied else FALSE)                                 */

    Bool                    rxBufFlag;
    /**< Flag to indicate if the TX buffer is suppiled by user or is NULL     *
     * (TRUE if user has supplied else FALSE)                                 */

    Uint8                   txTransBuf[128];
    /* Buffer to be used when the user supplied buffer is NULL                */

    Uint8                   rxTransBuf[128];
    /* Buffer to be used when the user supplied buffer is NULL                */

    Ptr                     hGpio;
    /**< Handle to GPIO driver used in case of GPIO CS operations. The application
     * will pass this as part of chan params and will be stored here          */
}Spi_ChanObj;

/**
 *  \brief HW info stfructure
 *
 *  Spi structure to hold the instance specific information.
 */
typedef struct Spi_HwInfo
{
    CSL_SpiRegsOvly baseAddress;
    Uint32          cpuEventNumber;
    Uint32          rxDmaEventNumber;
    Uint32          txDmaEventNumber;
    Uint32          inputFrequency;
    Uint32          maxChipSelect;

    Uint32          pwrmLpscId;
    /**< LPSC number for this instance                                        */

    Uint32          pscInstance;
    /**< Instance of the PSC which controls this module's instance            */
}Spi_HwInfo;

#ifdef BIOS_PWRM_ENABLE
typedef struct Spi_pwrmInfo_t
{
    Spi_pllDomain          pllDomain;
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

    PWRM_NotifyHandle      notifyHandle[Spi_MAX_PWRM_EVENTS];
    /**< Handles required for unregistering of the events with PWRM           */

    Fxn                    delayedCompletionFxn[Spi_MAX_PWRM_EVENTS];
    /**< delayed completion callback function pointer                         */

    PWRM_ConstraintHandle  constraintHandle[Spi_MAX_PWRM_CONSTRAINTS];
    /**< Handles required for unregistering of the constraints with PWRM      */
}Spi_pwrmInfo;
#endif

/**
 *  \brief instance structure
 *
 *  There is one instance structure per SPI instance. maximum number of
 *  structures is given by {@link #numInstances}
 */
typedef struct {
    Bool *inUse;
} Spi_Module_State;


/**
 *  \brief instance structure
 *
 *  There is one instance structure per SPI instance. maximum number of
 *  structures is given by {@link #numInstances}
 */
typedef struct
{
    Uint32                 instNum;
    /**< Instance number of this instance                                     */

    Spi_DriverState        devState;
    /**< state of the SPI Either created or deleted                           */

    Spi_OpMode             opMode;
    /**< Mode of operation                                                    */

    Spi_HwInfo             deviceInfo;
    /**< instance specific information                                        */

    Spi_ChanObj            chanObj[Spi_NUM_CHANS];
    /**< channel objects for the SPI                                          */

    Uint32                 numOpens;
    /**< Number of channels opened                                            */

    Uint16                 hwiNumber;
    /**< Hardware interrupt number                                            */

    Bool                   enableCache;
    /**< Submitted buffers are in cacheable memory                            */

    Stats                  stats;
    /**< SPI specific stats                                                   */

    Spi_HWConfigData       spiHWconfig;
    /**< SPI Hardware configurations                                          */

    Bool                   csHighPolarity;
    /**< Chip Select Polarity. Default is set to Active Low = FALSE           */

    Ptr                    hEdma;
    /**< Handle used for Edma                                                 */

    Bool                   dmaChanAllocated;
    /**< Flag to inidicate EDMA channels allocation status                    */

    volatile Uint32        edmaCbCheck;
    /**< Use to check occurance of EDMA callback                              */

    Int32                  polledModeTimeout;
    /**< Timeout to for the io operation                                      */

    Spi_ChanObj           *currentActiveChannel;
    /**< This specifies the current active channel                            */

    Bool                   isSlaveChannelOpened;
    /**<This boolean track for having only one slave channel                  */

    SEM_Obj                syncSem;
    /**< Semaphore to sync multiple tasks during the polled mode operation    */

    Bool                   pscPwrmEnable;
    /**< Option to enable or disable the PSC control in the driver            */
#ifdef BIOS_PWRM_ENABLE
    Spi_pwrmInfo           pwrmInfo;
    /**< structure to hold the PWMR related information                       */
#endif    
} Spi_Object;

extern Uint32 Spi_localControlDataConfig(Spi_ChanObj       *chanHandle,
                                         Uint32            chipSelect,
                                         Spi_DataFormat    dataFormat,
                                         Uint32            gpioPin,
                                         Uint32            flags);

extern Void Spi_localGetNextChannel(Spi_Object *instHandle,
                                    Spi_ChanObj**pChanHandle);

extern Int Spi_localEdmaTransfer(Spi_Object      *instHandle,
                                 Spi_DataParam   *dataparam);

extern Int32 Spi_localEdmaChannel_Request(Spi_Object *instHandle);

extern Void Spi_localGpioPinToggle(Spi_ChanObj *chanHandle,Bool toggleState);

extern Int32 Spi_localLpscOff(Spi_Object *instHandle);

extern Int32 Spi_localLpscOn(Spi_Object  *instHandle);

extern Void Spi_localgenericDelay(Int32 count);

extern Void spi_localCompleteIOedmaCallback (Spi_Object *instHandle);

extern Void Spi_loadPendedIops(Spi_Object *instHandle);
#ifdef __cplusplus
}
#endif

#endif  /*_SPILOCAL_H_ */
