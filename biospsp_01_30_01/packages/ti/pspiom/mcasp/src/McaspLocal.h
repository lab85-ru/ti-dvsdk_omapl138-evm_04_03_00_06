/*
 * McaspLocal.h
 *
 * McASP Driver internal header file
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
 *  ======== ti/psp/mcasp/McaspLocal.h ========
 */

#ifndef _MCASPLOCAL_H_
#define _MCASPLOCAL_H_

#if !defined(CHIP_C6747) && !defined(CHIP_OMAPL137) && !defined(CHIP_C6748) && !defined(CHIP_OMAPL138)
#error "No chip type defined! (Must use -DCHIP_C6747 or -DCHIP_OMAPL137)"
#endif

#include <std.h>
#include "ti/pspiom/mcasp/Mcasp.h"

#include "ti/pspiom/cslr/cslr_mcasp.h"

#if ((defined(CHIP_C6748) || defined(CHIP_OMAPL138)) && (!defined(BIOS_PWRM_ENABLE)))
#error "Use Bios PWRM module only for the SoC"
#else
    #include <pwrm.h>
#endif

#if ((defined(CHIP_C6747) || defined(CHIP_OMAPL137)) && defined(BIOS_PWRM_ENABLE))
#error "Do not use Bios PWRM module for this SoC"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CHIP_DM6437
    #include "ti/pspiom/cslr/DM6437.h"
#endif

#ifdef CHIP_6747
    #include "ti/pspiom/cslr/soc_C6747.h"
#endif

#ifdef CHIP_OMAPL137
    #include "ti/pspiom/cslr/soc_OMAPL137.h"
#endif

#ifdef CHIP_6748
    #include "ti/pspiom/cslr/soc_C6748.h"
#endif

#ifdef CHIP_OMAPL138
    #include "ti/pspiom/cslr/soc_OMAPL138.h"
#endif

#include <ti/sdo/edma3/drv/edma3_drv.h>

/* -------- constants -------- */

#define Mcasp_NUMSERIALIZERS      (16u)
/**<  This variable sets a limit on the maximum number of serializers that are*
 * possible in a given Mcasp instance.                                        */

#define Mcasp_MAX_NUM_CHANS       (0x02)
/**<  Maximum number of channels supported by the Mcasp instance              */

#define Mcasp_MAX_PACKET_SIZE     (32768u)
/**<  Maximum packet size in bytes                                            */

#define Mcasp_MAXLINKCNT          (2u)
/**<  Maximum number of EDMA jobs linked at a time (Must be 2).               */

#define Mcasp_TXEVENTQUE          (0u)
/**<  Transmit EDMA channel event queue number                                */

#define Mcasp_RXEVENTQUE          (1u)
/**<  Receive EDMA channel event queue number                                 */

#define Mcasp_SM_RESET            (0u)
/**<  Reset value of xmt/rcv state m/c and frame sync                         */

#define Mcasp_TWO_PKTS_QUEUED     (2u)
/**<  When xmt is taken out of reset, it needs two words to                   *
 *  service it. This is used to keep tally of this requirement.               */

#define Mcasp_STATUS_INVALID      (0xFFFF)
/**<  Generic invalidate status                                               */

#define Mcasp_STATUS_VALID        (0x1)
/**<  Generic validate status                                                 */

#define Mcasp_DATA_RDY            (0x20u)
/**<  Interrupt due to: Data Ready                                            */

#define Mcasp_DMAERR              (0x80u)
/**<  Interrupt due to: DMA channel error                                     */

#define Mcasp_XMT_UNDERRUN        (0x1u)
/**<  Interrupt due to: Transmit Buffer Underrun                              */

#define Mcasp_RCV_OVERRUN         (0x1u)
/**<  Interrupt due to: Receive Buffer Overrun                                */

#define Mcasp_RSTAT_ERROR         (0x100u)
/**< Bit field to indicate if any receive error has occured                   */

#define Mcasp_XSTAT_ERROR         (0x100u)
/**< Bit field to indicate if any transmit error has occured                  */

#define Mcasp_TX_CLK_FAIL         (0x04u)
/**< Interrupt due to transmit clock failure                                  */

#define Mcasp_RX_CLK_FAIL         (0x04u)
/**< Interrupt due to receive clock failure                                   */

#define Mcasp_SYNCERR             (0x2u)
/**<  Interrupt due to: Transmit/Receive Sync Error                           */

#define Mcasp_OPT_TCINTEN_SHIFT   (0x00000014u)
/**<  Interrupt enable bit in OPT register for edma                           */

#define Mcasp_OPT_SYNCDIM_SHIFT   (0x00000002u)
/**<  Sync Type AB set bit of OPT register for edma                           */

#define Mcasp_OPT_TCC_SHIFT       (0x0000000Cu)
/**<  TCC set bit in OPT register for edma                                    */

#define Mcasp_OPT_TCC_MASK        (0x0003F000u)
/**<  Tcc mask in OPT register for edma                                       */

#define Mcasp_OPT_TCCMOD_SHIFT    (0x0000000Bu)
/**< Transfer completion selection bit                                        */

#define Mcasp_CNT_MAX_VAL         (0xFFFFu)
/**<  Max possible value of aCnt, bCnt and cCnt                               */

#define Mcasp_SWI_PRIORITY         0x01
/**< Priority of the swi thread which handles the interrupts                  */

#define Mcasp_MAX_PWRM_EVENTS      (PWRM_INVALIDEVENT)
/**< Max number of PWRM events for which the mcasp will register              */

#define Mcasp_MAX_PWRM_CONSTRAINTS (PWRM_DISALLOWEDSLEEPSTATE_MASK)
/**< Max number of constraints for which a moudule can register               */

/**
 * \def    Mcasp_POLLED_RETRYCOUNT
 *         This macro specifies the default retry count to be used by the Mcasp
 *         driver when polling for any hardware bit to be set or reset.
 *
 * \note   This default value used by the driver can be changed by an IOCTL
 *         "Mcasp_Ioctl_SET_TIMEOUT".
 */
#define Mcasp_POLLED_RETRYCOUNT   (0xFFFFu)


/* -------- enums -------- */

/**
 *  \brief Enumeration for channel mode
 */
typedef enum Mcasp_ChanMode
{
    Mcasp_ChanMode_FREE    = 0,       /**<  Channel not allocated             */
    Mcasp_ChanMode_XMT_DIT = 1u,      /**<  Transmit channel: DIT mode        */
    Mcasp_ChanMode_XMT_TDM = 2u,      /**<  Transmit channel: TDM mode        */
    Mcasp_ChanMode_RCV     = 3u       /**<  Receive channel                   */
}Mcasp_ChanMode;

/**
 *  \brief Enumeration for DIT channel index
 */
typedef enum Mcasp_DITRegIndex {
    Mcasp_DITRegIndex_0 = 0,
    /**<  1st DIT (channel/user data), (left/right) Register                  */
    Mcasp_DITRegIndex_1 = 1,
    /**<  2nd DIT (channel/user data), (left/right) Register                  */
    Mcasp_DITRegIndex_2 = 2,
    /**<  3rd DIT (channel/user data), (left/right) Register                  */
    Mcasp_DITRegIndex_3 = 3,
    /**<  4th DIT (channel/user data), (left/right) Register                  */
    Mcasp_DITRegIndex_4 = 4,
    /**<  5th DIT (channel/user data), (left/right) Register                  */
    Mcasp_DITRegIndex_5 = 5
    /**<  6th DIT (channel/user data), (left/right) Register                  */
}Mcasp_DITRegIndex;

/**
 *  \brief Mcasp driver state
 *
 *  Mcasp driver state enums used to track the driver state
 */
typedef enum Mcasp_DriverState {
    Mcasp_DriverState_DELETED,
    Mcasp_DriverState_CREATED,
    Mcasp_DriverState_INITIALIZED,
    Mcasp_DriverState_OPENED,
    Mcasp_DriverState_CLOSED,
    Mcasp_DriverState_DEINITIALIZED,
    Mcasp_DriverState_POWERED_DOWN,
    Mcasp_DriverState_PWRM_SUSPEND
}Mcasp_DriverState;

/**
 *  \brief Enumeration for the serializer mode
 */
typedef enum Mcasp_SerMode {
    Mcasp_SerMode_INACTIVE = 0,                /**< Serializer is inactive    */
    Mcasp_SerMode_XMT      = 1,                /**< Serializer is transmitter */
    Mcasp_SerMode_RCV      = 2                 /**< Serializer is receiver    */
}Mcasp_SerMode;

/**
 *  \brief Enumeration for serializer status
 */
typedef enum Mcasp_SerializerStatus {
    Mcasp_SerializerStatus_FREE = (0),/**< Serializer not allocated          */
    Mcasp_SerializerStatus_XMT  = (1),/**< Serializer configured to transmit */
    Mcasp_SerializerStatus_RCV  = (2) /**< Serializer configured to receive  */
}Mcasp_SerializerStatus;

/* -------- structs -------- */

/**
 *  \brief Enumeration for Hardware status commands
 */
typedef enum Mcasp_HwStatusQuery {
    /**
     * @brief   Return current transmit slot being transmitted
     * @param   (Uint16 *)
     */
    Mcasp_HwStatusQuery_CURRENT_XSLOT  = 1,
    /**
     * @brief   Return current receive slot being received
     * @param   (Uint16 *)
     */
    Mcasp_HwStatusQuery_CURRENT_RSLOT  = 2,
    /**
     * @brief   Return transmit error status bit
     * @param   (Bool *)
     */
    Mcasp_HwStatusQuery_XSTAT_XERR     = 3,
    /**
     * @brief   Return transmit clock failure flag status
     * @param   (Bool *)
     */
    Mcasp_HwStatusQuery_XSTAT_XCLKFAIL = 4,
    /**
     * @brief   Return unexpected transmit frame sync flag status
     * @param   (Bool *)
     */
    Mcasp_HwStatusQuery_XSTAT_XSYNCERR = 5,
    /**
     * @brief   Return transmit underrun flag status
     * @param   (Bool *)
     */
    Mcasp_HwStatusQuery_XSTAT_XUNDRN   = 6,
    /**
     * @brief   Return transmit data ready flag status
     * @param   (Bool *)
     */
    Mcasp_HwStatusQuery_XSTAT_XDATA    = 7,
    /**
     * @brief   Return receive error status bit
     * @param   (Bool *)
     */
    Mcasp_HwStatusQuery_RSTAT_RERR     = 8,
    /**
     * @brief   Return receive clock failure flag status
     * @param   (Bool *)
     */
    Mcasp_HwStatusQuery_RSTAT_RCLKFAIL = 9,
    /**
     * @brief   Return unexpected receive frame sync flag status
     * @param   (Bool *)
     */
    Mcasp_HwStatusQuery_RSTAT_RSYNCERR = 10,
    /**
     * @brief   Return receive overrun flag status
     * @param   (Bool *)
     */
    Mcasp_HwStatusQuery_RSTAT_ROVRN    = 11,
    /**
     * @brief   Return receive data ready flag status
     * @param   (Bool *)
     */
    Mcasp_HwStatusQuery_RSTAT_RDATA    = 12,
    /**
     * @brief   Return whether the serializer is ready to receive or not
     * @param   (McaspSerQuery *)
     */
    Mcasp_HwStatusQuery_SRCTL_RRDY     = 13,
    /**
     * @brief   Return status whether the serializer is ready to transmit
     *          or not
     * @param   (SerQuery *)
     */
    Mcasp_HwStatusQuery_SRCTL_XRDY     = 14,
    /**
     * @brief   Return status whether serializer is configured to
     *          transmit/receive/inactive
     * @param   (SerQuery *)
     */
    Mcasp_HwStatusQuery_SRCTL_SRMOD    = 15,
    /**
     * @brief   Return the value of transmitter status register.
     * @param   (Uint16 *)
     */
    Mcasp_HwStatusQuery_XSTAT          = 16,
    /**
     * @brief   Return the value of receiver status register.
     * @param   (Uint16 *)
     */
    Mcasp_HwStatusQuery_RSTAT          = 17,
    /**
     * @brief   Return the transmit state machine and transmit frame sync
     *          generator values in transmitter global control register
     * @param   (Uint8 *)
     * @li          0x00 - Both transmit frame generator sync and transmit
     *                     state machine are reset.
     * @li          0x1  - Only transmit state machine is active.
     * @li          0x10 - Only transmit frame sync generator is active.
     * @li          0x11 - Both transmit frame generator sync and transmit
     *                     state machine are active.
     */
    Mcasp_HwStatusQuery_SM_FS_XMT      = 18,
    /**
     * @brief   Return the receive state machine and receive frame sync
     *          generator values in receiver global control register
     * @param   (Uint8 *)
     * @li          0x00 - Both receive frame generator sync and receive
     *                     state machine are reset.
     * @li          0x1  - Only receive state machine is active.
     * @li          0x10 - Only receive frame sync generator is active.
     * @li          0x11 - Both receive frame generator sync and receive
     *                     state machine are active.
     */
    Mcasp_HwStatusQuery_SM_FS_RCV      = 19,
    /**
     * @brief   Queries whether DIT mode is set or not
     * @param   (Bool *)
     */
    Mcasp_HwStatusQuery_DIT_MODE       = 20,
    /**
     * @brief   Queries and returns the value in AMUTE register
     * @param   (Bool *)
     */
    Mcasp_HwStatusQuery_AMUTE          = 21,
    /**
     * @brief max number of queries
     */
    Mcasp_HwStatusQuery_MAX            = 22
}Mcasp_HwStatusQuery;

/**
 *  \brief Channel Object
 *
 *  This structure maintains the current channel state. It also holds
 *  information on DMA channel being used and holds the application
 *  callback function to be called in case of an interrupt.
 *
 *  This structure is initialized by mdCreateChan and a pointer to this
 *  is passed down to all other channel related functions. Lifetime of
 *  the data structure is from its creation by mdCreateChan till it
 *  is invalidated by mdDeleteChan.
 */
typedef struct Mcasp_ChannelObj
{
    Mcasp_DriverState      chanState;
    /**<  Current state of the channel (opened or closed)                     */

    Uint32                 mode;
    /**<  mode for channel (i.e IOM_INPUT or OUTPUT)                          */

    Ptr                    devHandle;
    /**<  Pointer to McASP device driver port structure                       */

    IOM_TiomCallback       cbFxn;
    /**<   IOM call back function to be called once a packet is processed     */

    Arg                    cbArg;
    /**<  Callback Function argument                                          */

    QUE_Obj                queueReqList;
    /**<  Queue containing the pending requests received from application.    */

    QUE_Obj                queueFloatingList;
    /**<  list to manage floating packets in DMA                              */

    Uint16                 noOfSerAllocated;
    /**<  no of Serializer(s) allocated                                       */

    Uint32                 indexOfSersRequested[Mcasp_NUMSERIALIZERS];
    /**<  Multi Serializer numbers requested by channel                       */

    Mcasp_ChanMode         channelOpMode;
    /**<  Mode of operation: Transmit(DIT/TDM) or Receive                     */

    Uint16                 dataQueuedOnReset;
    /**<  McASP needs to service each serializer with two
        words when it awakes from reset                                       */

    Uint32                 intStatus;
    /**<  Holds id of McASP interrupt cause, if any
        used in interrupt mode                                                */

    IOM_Packet             *dataPacket;
    /**<  Current data packet being serviced.                                 */

    IOM_Packet             *tempPacket;
    /**<  Temp IOP holder                                                     */

    volatile Bool          isTempPacketValid;
    /**<  Valid packet flag in EDMA callback                                  */

    Uint32                 userDataBufferSize;
    /**<  Size of the data buffer per serialiser, hence the buffer given by   *
     * the application should really be userDataBufferSize * no. of           *
     * serialisers allocated for that channel                                 */

    Int32                  submitCount;
    /**<  Number of submit calls pending                                      */

    Uint16                 noOfSlots;
    /**< number of slots used by the mcasp                                    */

    Ptr                    edmaHandle;
    /**<  Edma Handle                                                         */

    Uint32                 xferChan;
    /**<  EDMA  ChannelID                                                     */

    Uint32                 tcc;
    /**<  EDMA  tcc                                                           */

    Uint32                 pramTbl[2u];
    /**<  Logical channel numbers of EDMA, which are used for linking         */

    Uint32                 pramTblAddr[2u];
    /**<  Physical address of logical channel numbers of EDMA, which          *
         * are used for linking                                               */

    Uint32                 nextLinkParamSetToBeUpdated;
    /**<  Used to store the next index of link param to be updated            */

    Bool                   isDmaDriven;
    /**<  Determines whether data input is using DMA or not                   */

    volatile Bool          loopjobUpdatedinParamset;
    /**<  Used to prevent updating second paramset with loopjob for last      *
     * datapacket edma callback                                               */

    volatile Bool          xferinProgressIntmode;
    /**<  use to start int mode transfer from  0 packet status                */

    volatile Bool          bMuteON;
    /**<  True, if playback is muted                                          */

    volatile Bool          paused;
    /**<  TRUE, if pause Request is called                                    */

    volatile Bool          nextFlag;
    /**<  TRUE, if pause Request is called                                    */

    Bool                   enableHwFifo;
    /**< whether the FIFO has to be enabled for this channel                  */

    Bool                   isDataPacked;
    /**< flag to indicate if the buffer data needs to be packed               */

    Uint32                 cpuEventNum;
    /**<  This hold the cpu event no. of this channel (TX/Rx) for this        *
     * instance                                                               */

    Ptr                    loopJobBuffer;
    /**<  Buffer to be transferred when the loop job is running               */

    Uint16                 loopJobLength;
    /**<  Length of userloop job for each serialiser                          */

    Uint16                 userLoopJobLength;
    /**< Length of the user supplied loop job buffer                          */

    Uint16                 roundedWordWidth;
    /**<  Word length bytes to be transferred for DMA transaction             */

    Uint16                 currentDataSize;
    /**<  data packet size used for interrupt mode                            */

    Int                    currentPacketErrorStatus;
    /**<  This member will hold the error status -normally updated from       *
     * cpu interrupt thread and is used in EDMA completion thread for         *
     * updating the error code in IOP                                         */

    EDMA3_RM_TccCallback   edmaCallback;
    /**<  stores the edma call back function ptr - functions actual type is   *
     * EDMA3_RM_TccCallback                                                   */

    Mcasp_GblCallback      gblErrCbk;
    /**<  callback required when the loop job is running must be callable     *
     * directly from the IOM layer                                            */

    Mcasp_BufferFormat     dataFormat;
    /**< Application supplied buffer format                                   */

    Uint32                 userIntValue;
    /**< User supplied mask for the interrupts to be enabled                  */

    Bool                   userLoopJob;
    /**< Option to indicate if the user loop job is used or driver loop job   */
}Mcasp_ChannelObj;

/**
 *  \brief Structure used to supply the information for the Mcasp setup
 */
typedef struct Mcasp_DevParams
{
    Bool            enablecache;
    /**< Submitted buffers are in cacheable memory                            */

    Bool            isDataBufferPayloadStructure;
    /**< Submitted buffers have payload information present or absent         */

    Mcasp_HwSetup   *mcaspHwSetup;
    /**<  Initial setup for the McASP                                         */

}Mcasp_DevParams;

/**
 *  \brief HW info structure
 *
 *  Mcasp structure to hold the instance specific information.
 */
typedef struct Mcasp_HwInfo
{
    CSL_McaspRegs   *baseAddress;
    /**< base address of the mcasp instance        */

    CSL_AfifoRegs   *fifoAddress;
    /**< Fifo address of the mcasp instance        */

    CSL_AdataRegs   *dataAddress;
    /**< Mcasp data registers address              */

    Bool             ditSupport;
    /**< Dit support enabled in hardware or not    */

    Uint16           numSerializers;
    /**<number of serializers supported by instance*/

    Uint32           cpuEventNumber;
    /**< hardware interrupt of the mcasp instance  */

    Uint32           rxDmaEventNumber;
    /**< RX DMA event number                       */

    Uint32           txDmaEventNumber;
    /**< TX DMA event number                       */

    Uint32           pwrmLpscId;
    /**< LPSC number for this instance                                        */

    Uint32           pscInstance;
    /**< Instance of the PSC which controls this module's instance            */

}Mcasp_HwInfo;

/**
 *  \brief Hardware specific information structure
 */
typedef struct Mcasp_HwObj
{
    Bool                 ditStatus;
    /**< Support for DIT mode  in hardware                                    */

    CSL_McaspRegsOvly    regs;
    /**< Pointer to the register overlay structure for the peripheral         */

    CSL_AfifoRegsOvly    fifoRegs;
    /**< pointer to the register overlay structure for the FIFO               */

    Int16                perNo;
    /**< Specifies a particular instance of McASP                             */

    Uint16               numOfSerializers;
    /**< Number of serializers                                                */

    volatile Ptr         dataAddr;
    /**< Instance Data Address                                                */

    Uint32               edmaTxEventNum;
    /**< edma Transmit event number                                           */

    Uint32               edmaRxEventNum;
    /**< edma Receive event number                                            */

    Uint32               cpuTxEventNum;
    /**< This holds the CPU event no. of Rx                                   */

    Uint32               cpuRxEventNum;
    /**< This holds the CPU event no. of Tx                                   */

    Uint32               pwrmLpscId;
    /**< LPSC number for this instance                                        */

    Uint32               pscInstance;
    /**< Instance of the PSC which controls this module's instance            */
}Mcasp_HwObj;

/**
 * \brief structure holding the information required for the ISR to service the
 *        appropriate instance.
 */
typedef struct Mcasp_IsrObject
{
    Bool           isIsrRegistered;
    /**< Hardware instance number of the device registering the interrupt   */

    Bool           chanEnabled[Mcasp_MAX_NUM_CHANS];
    /**< variable to indicate whether the channel has registered interrupts */

    struct Mcasp_Object  *instHandle;
    /**< pointer to the driver instance object                              */

    SWI_Handle     isrSwiTaskHandle;
    /**< Handle to the SWI tasklet that will be handling all the interrupts */

}Mcasp_IsrObject;

/**
 *  \brief Module specific parameters. Present implementation doesn't have
 *  any module specific parameters.
 */
typedef struct Mcasp_HwParam
{
    Uint16 flags;
    /**<
     *  Bit mask to be used for module specific parameters.
     *  The below declaration is just a place-holder for future
     *  implementation.
     */
}Mcasp_HwParam;

/**
 *  \brief The following structure will be used in
 *  MCASP_HwStatusQuery_SRCTL_SRMOD
 */
typedef struct Mcasp_SerModeQuery {
    Mcasp_SerializerNum   serNum;
    /**< Serializer number                                                    */
    Mcasp_SerMode         serMode;
    /**< Serializer mode                                                      */
}Mcasp_SerModeQuery;

/**
 *  \brief The following structure will be used in
 *  MCASP_HwStatusQuery_SRCTL_RRDY, and MCASP_HwStatusQuery_SRCTL_XRDY
 */
typedef struct Mcasp_SerQuery {
    Mcasp_SerializerNum   serNum;
    /**< Serializer number                                                    */
    Bool                  serStatus;
    /**< Return value of the query                                            */
}Mcasp_SerQuery;

/**
 *  \brief Loopjob buffers used in EDMA mode
 */
typedef struct Mcasp_TempBuffer
{
    Uint8   scratchBuf[(4u) + Mcasp_CACHE_LENGTH];
    /* This buffer will be aligned and also the same buffer will be used      *
     * for all serializers and also only 4 bytes are required because the     *
     * max wordwidth is 4 and the cache length is used for alignment          */
    Uint32 *scratchBuffer;
}Mcasp_TempBuffer;


#ifdef BIOS_PWRM_ENABLE
/**
 * \brief   Strcuture to hold the information related to the PWRM.
 */
typedef struct Mcasp_pwrmInfo_t
{
    Mcasp_pllDomain        pllDomain;
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

    PWRM_NotifyHandle      notifyHandle[Mcasp_MAX_PWRM_EVENTS];
    /**< Handles required for unregistering of the events with PWRM           */

    Fxn                    delayedCompletionFxn[Mcasp_MAX_PWRM_EVENTS];
    /**< delayed completion callback function pointer                         */

    PWRM_ConstraintHandle  constraintHandle[Mcasp_MAX_PWRM_CONSTRAINTS];
    /**< Handles required for unregistering of the constraints with PWRM      */
}Mcasp_pwrmInfo;
#endif


/* -------- module-wide state -------- */
/**
 *  \brief module-wide state
 *
 *  inUse in module state points to an array whose size is soc specific
 *  Pointer used to match IDriver codebase.
 */
typedef struct Mcasp_Module_State
{
    Bool             *inUse;

    Mcasp_IsrObject  *isrObject;
} Mcasp_Module_State;

/* -------- per-instance state -------- */
/**
 *  \brief per-instance state
 */
typedef struct Mcasp_Object
{
    Uint16                   instNum;
    /**< Preserve  instance number in port                                    */

    Mcasp_DriverState        devState;
    /**< stores the current state of the driver                               */

    Bool                     isDataBufferPayloadStructure;
    /**< Input request is a flat data buffer or a  payload                    *
     * structure                                                              */

    Mcasp_HwSetup            mcaspHwSetup;
    /**< Hw parmas to configure mcasp                                         */

    Uint32                   hwiNumber;
    /**< Hwi number if using interrupt mode                                   */

    Bool                     enablecache;
    /**< Submitted buffers are in cacheable memory                            */

    Bool                     stopSmFsXmt;
    /**< Status of stoping state m/c for XMT                                  */

    Bool                     stopSmFsRcv;
    /**< Status of stoping state m/c for RCV                                  */

    Mcasp_ChannelObj         XmtObj;
    /**< Holds transmit channel to the McASP.                                 */

    Mcasp_ChannelObj         RcvObj;
    /**< Holds receive channel to the McASP.                                  */

    Mcasp_HwObj              hwInfo;
    /**< McASP handle for initial port configuration                          */

    Mcasp_SerializerStatus   serStatus[Mcasp_NUMSERIALIZERS];
    /**< Holds status information for both the serializers.                   *
     *  - Valid values are FREE, XMT and RCV                                  */

    SWI_Obj                  isrSwiObject;
    /**< SWI to handle the interrupts                                         */

    SWI_Obj                  fifoSwiObject;
    /**< SWI object for the swi used to handle the FIFO not empty while the   *
     * last packet callback is in progress                                    */

    Uint32                   retryCount;
    /**< The retry count value to be used when waiting for the TX empty to be *
     * set                                                                    */

    Bool                     loopJobMode;
    /**< Variable to check if the loop job mode is enabled or not             */

    Bool                     pscPwrmEnable;
    /**< Option to enable or disable the PSC control in the driver            */

#ifdef BIOS_PWRM_ENABLE
    Mcasp_pwrmInfo           pwrmInfo;
#endif
    /**< structure to hold the PWMR related information                       */

} Mcasp_Object;

/* -------- typedefs -------- */

typedef Mcasp_ChannelObj   *Mcasp_ChannelHandle;
typedef Mcasp_HwObj        *Mcasp_HwHandle;

extern Int32 Mcasp_localConfigRcvSection(Mcasp_Object       *instHandle,
                                         Mcasp_HwSetupData  *const rcvData);

extern Int32 Mcasp_localConfigXmtSection(Mcasp_Object       *instHandle,
                                         Mcasp_HwSetupData  *const xmtData);

extern Void Mcasp_localSmReset(Mcasp_ChannelHandle chanHandle);

extern Void Mcasp_localSmSet(Mcasp_ChannelHandle chanHandle);

extern Int32 Mcasp_localAbortReset(Mcasp_ChannelHandle chanHandle);

extern Void Mcasp_localCompleteCurrentIo(Mcasp_ChannelHandle chanHandle);

extern Int32 Mcasp_localResetXmt(Mcasp_HwHandle  hMcasp);

extern Int32 Mcasp_localResetRcv(Mcasp_HwHandle  hMcasp);

extern Int32 Mcasp_localEdmaProcessPkt(Ptr chanp, IOM_Packet *ioPacket);

extern Int32 Mcasp_localCancelAndAbortAllIo(Mcasp_ChannelHandle chanHandle);

extern Void Mcasp_localSetupHwInfo(Mcasp_HwHandle        hMcasp,
                                   const Mcasp_HwSetup  *myHwSetup);

extern Void Mcasp_localActivateSmFsForBoth(Mcasp_Object *instHandle);

extern Void Mcasp_localActivateSmFsForXmt(Mcasp_Object *instHandle);

extern Void Mcasp_localActivateSmFsForRcv(Mcasp_Object *instHandle);

extern Void Mcasp_localDeactivateSmFsForXmt(Mcasp_Object *instHandle);

extern Void Mcasp_localDeactivateSmFsForRcv(Mcasp_Object *instHandle);

extern Int32 Mcasp_localUpdtDtPktToLnkPrms(Mcasp_ChannelHandle chanHandle,
                                           IOM_Packet *const ioPacket);

extern Int32 Mcasp_localEdmaChanPaRAMsetup(Mcasp_ChannelHandle chanHandle);

extern Int32 Mcasp_localSubmitIoctl(Mcasp_ChannelHandle      chanHandle,
                                    Uns                      cmd,
                                    Ptr                      arg,
                                    Ptr                      param);

extern Int32 Mcasp_localSetupEdmaDuringOpen(Mcasp_ChannelHandle chanHandle);

extern Void Mcasp_localEdmaCallback(Uint32              tcc,
                                    EDMA3_RM_TccStatus  status,
                                    Ptr                 data);

extern Int32 Mcasp_localGetIndicesSyncType(Mcasp_ChannelHandle chanHandle,
                                           volatile Int16     *bIndex,
                                           volatile Int16     *cIndex,
                                           volatile Uint16    *aCnt,
                                           volatile Uint16    *bCnt,
                                           volatile Uint16    *cCnt,
                                           EDMA3_DRV_SyncType *syncType,
                                           Bool                forLoopJobBuf);

extern Int32 Mcasp_localAbortRequests(Ptr handle,Ptr arg);

extern Void Mcasp_localGetNextIndex(Uint32 *index);

extern Int32 Mcasp_localLpscOff(Mcasp_Object *instHandle);

extern Int32 Mcasp_localLpscOn(Mcasp_Object *instHandle);

extern Int32 Mcasp_loadPendedIops(Mcasp_ChannelObj *chanHandle);

extern Void Mcasp_localLoadPktToEdma(Mcasp_ChannelObj *chanHandle,IOM_Packet *ioPacket);

extern Void Mcasp_localHandlePwrmEvent(Mcasp_Object     *instHandle,
                                       Mcasp_ChannelObj *chanHandle);

#ifdef __cplusplus
}
#endif

#endif  /*_MCASPLOCAL_H_ */
