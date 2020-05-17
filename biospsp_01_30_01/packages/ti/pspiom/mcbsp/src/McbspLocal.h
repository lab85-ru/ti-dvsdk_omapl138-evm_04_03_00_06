/*
 * McbspLocal.h
 *
 * McBSP Driver internal header file
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
 *  \file     McbspLocal.h
 *
 *  \brief    Header file conataining the driver internal definitions and data
 *            structures.
 *
 *            (C) Copyright 2008, Texas Instruments, Inc
 *
 *  \author   Platform Support Group
 *
 *  \version
 *            0.1 created
 */

#ifndef _MCBSPLOCAL_H_
#define _MCBSPLOCAL_H_


#if !defined(CHIP_C6748) && !defined(CHIP_OMAPL138)
#error "No chip type defined! (Must use -DCHIP_C6748 or -DCHIP_OMAPL138)"
#endif

/*============================================================================*/
/*                        INCLUDE FILES                                       */
/*============================================================================*/

#include <std.h>
#include <iom.h>
#include <que.h>
#include <swi.h>
#include <ti/sdo/edma3/drv/edma3_drv.h>
#include "ti/pspiom/mcbsp/Mcbsp.h"
#include "ti/pspiom/cslr/cslr_mcbsp.h"

#ifdef CHIP_C6748
    #include "ti/pspiom/cslr/soc_C6748.h"
#endif

#ifdef CHIP_OMAPL138
    #include "ti/pspiom/cslr/soc_OMAPL138.h"
#endif

#if ((defined(CHIP_C6748) || defined(CHIP_OMAPL138)) && (!defined(BIOS_PWRM_ENABLE)))
#error "Use Bios PWRM module only for the SoC"
#else
    #include <pwrm.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*                            MACRO DEFINTIONS                                */
/*============================================================================*/

#define Mcbsp_MAXLINKCNT               (2u)
/**<  Maximum number of EDMA jobs linked at a time (Must be 2).               */

#define Mcbsp_TXEVENTQUE               (0u)
/**<  Transmit EDMA channel event queue number                                */

#define Mcbsp_RXEVENTQUE               (1u)
/**<  Receive EDMA channel event queue number                                 */

#define Mcbsp_CNT_MAX_VAL              (0xFFFFu)
/**<  Max possible value of aCnt, bCnt and cCnt                               */

#define Mcbsp_STATUS_INVALID           (0xFFFF)
/**<  Generic invalidate status                                               */

#define Mcbsp_STATUS_VALID             (0x1)
/**<  Generic validate status                                                 */

#define Mcbsp_MAX_PACKET_SIZE          (32768u)
/**< Maximum sixe of the packet programmable                                  */

#define Mcbsp_SPI_TRANSCEIVE_BUF_LEN   (256u)
/**< The length of the driver's Transceive buffer                             */

#define Mcbsp_FRAME_LENGTH             (127u)
/**< No of frames Max supported by the mcbsp                                  */

#define Mcbsp_MAX_PWRM_EVENTS          (PWRM_INVALIDEVENT)
/**< Max number of PWRM events for which the mcasp will register              */

#define Mcbsp_MAX_PWRM_CONSTRAINTS     (PWRM_DISALLOWEDSLEEPSTATE_MASK)
/**< Max number of constraints for which a moudule can register               */

/*============================================================================*/
/*                         ENUMERATED DATA TYPES                              */
/*============================================================================*/

/**
 *  \brief Mcbsp driver state
 *
 *   Mcbsp driver state enums used to track the driver and channel state.
 */
typedef enum Mcbsp_DriverState_t
{
    Mcbsp_DriverState_DELETED,
    Mcbsp_DriverState_CREATED,
    Mcbsp_DriverState_INITIALIZED,
    Mcbsp_DriverState_OPENED,
    Mcbsp_DriverState_CLOSED,
    Mcbsp_DriverState_DEINITIALIZED,
    Mcbsp_DriverState_POWERED_DOWN,
    Mcbsp_DriverState_PWRM_SUSPEND
}Mcbsp_DriverState;
/**< Mcbsp driver and channel states                              */

/**
 * \brief Mcbsp SPCR control enums
 *
 *  These enums are used to control the settings of the SPCR register.
 */
typedef enum Mcbsp_SpcrCtrl_t
{
    Mcbsp_SpcrCtrl_RX_ENABLE   = (1u),
    /**< To enable receiver in resetControl Function              */

    Mcbsp_SpcrCtrl_TX_ENABLE   = (2u),
    /**< To enable Transmitter in resetControl Function           */

    Mcbsp_SpcrCtrl_RX_DISABLE  = (4u),
    /**< To disable Receiver in resetControl Function             */

    Mcbsp_SpcrCtrl_TX_DISABLE  = (8u),
    /**< To disable Transmitter in resetControl Function          */

    Mcbsp_SpcrCtrl_FSYNC_ENABLE = (16u),
    /**< To enable Frame Sync Generation in resetControl Function */

    Mcbsp_SpcrCtrl_SRG_ENABLE   = (32u),
    /**< To enable Sample Rate Generator in resetControl Function */

    Mcbsp_SpcrCtrl_FSYNC_DISABLE = (64u),
    /**< To disable Frame Sync Generation in resetControl Function*/

    Mcbsp_SpcrCtrl_SRG_DISABLE = (128u)
    /**< To disable Sample Rate Generator in resetControl Function*/
}Mcbsp_SpcrCtrl;
/**< Mcbsp SPCR control enums                                     */

/*============================================================================*/
/*                              DATA STRUCTURES                               */
/*============================================================================*/

/**
 *  \brief  Loop job buffer structure.
 *          This structure defines the format of the Loop job buffer.
 *
 *  \note   Loop job buffer is a buffer used by the mcbsp in edma mode. The 
 *          Mcbsp uses this buffer when no iopackets are present and the mcbsp 
 *          is also not stopped.In this condition the Mcbsp utilises the 
 *          loop job buffer to transmit a known pattern of data from the loopjob
 *          buffer or receives the data in to a loop job buffer
 */
typedef struct Mcbsp_TempBuffer_t
{
    Uint8   scratchBuf[(4u) + Mcbsp_CACHE_LENGTH];
    /**< This buffer will be aligned and also the same buffer will be used
     * for all slots and also only 4 bytes are required because the
     * max wordwidth is 4 and the cache length is used for buffer alignment   */
    Uint32 *scratchBuffer;
    /**< Pointer to hold the aligned buffer                                   */
}Mcbsp_TempBuffer;
/**< loop job buffer format.                                                  */

/**
 * \brief Mcbsp Hardware specific information Object
 *
 *  This structure maintains the information specific to the hardware instance
 *  of the Mcbsp. information like the base address and the cpu event numbers
 *  and DMA events is specific to the instance. This strcuture is a collection
 *  of such information.
 */
typedef struct Mcbsp_HwInfo_t
{
    Uint32               instNum;
    /**< Instance of MCBSP being referred by this object                      */

    CSL_McbspRegsOvly    regs;
    /**< Pointer to the register overlay structure of the MCBSP               */

    CSL_BfifoRegsOvly    fifoRegs;
    /**< Fifo address of the mcbsp instance                                   */

    CSL_BdataRegsOvly    dataAddress;
    /**< Mcbsp data registers address                                         */

    Uint32               edmaTxEventNum;
    /**< edma Transmit event number                                           */

    Uint32               edmaRxEventNum;
    /**< edma Receive event number                                            */

    Uint32               cpuTxEventNum;
    /**< Transmit interrupt number                                            */

    Uint32               cpuRxEventNum;
    /**< Transmit interrupt number                                            */

    Uint32               moduleClkFreq;
    /**< Module Clock Frequency                                               */

    Uint32               pwrmLpscId;
    /**< LPSC number for this instance                                        */

    Uint32               pscInstance;
    /**< Instance of the PSC which controls this module's instance            */
    
}Mcbsp_HwInfo;
/**< Mcbsp Hardware specific information Object                               */

#ifdef BIOS_PWRM_ENABLE
/**
 * \brief   Strcuture to hold the information related to the PWRM.
 */
typedef struct Mcbsp_pwrmInfo_t
{
    Mcbsp_pllDomain        pllDomain;
    /**< Pll domain to be used for the device                                 */

    Bool                   ioSuspend;
    /**< whether the driver is in IO suspend mode (PWRM change is in progress)*/

    PWRM_Event             pwrmEvent;
    /**< Current pwrm event being processed                                   */

    Uint32                 constraintMask;
    /**< Constraint mask (for information only)                               */

    Bool                   constraintRegistered;
    /**< whether the contraint is registered for this module                  */

    Uint32                 dependencyCount;
    /**< Current dependency count when the driver is going for sleep or
     * deepsleep                                                              */

    PWRM_NotifyHandle      notifyHandle[Mcbsp_MAX_PWRM_EVENTS];
    /**< Handles required for unregistering of the events with PWRM           */

    Fxn                    delayedCompletionFxn[Mcbsp_MAX_PWRM_EVENTS];
    /**< delayed completion callback function pointer                         */

    PWRM_ConstraintHandle  constraintHandle[Mcbsp_MAX_PWRM_CONSTRAINTS];
    /**< Handles required for unregistering of the constraints with PWRM      */
}Mcbsp_pwrmInfo;
#endif

/**
 * \brief Mcbsp channel Object
 *
 *  This structure maintains the current channel state. It also holds
 *  information on DMA channel being used and holds the application
 *  callback function to be called in case of an interrupt.
 *
 *  This structure is initialized by mdCreateChan and a pointer to this
 *  is passed down to all other channel related functions. Lifetime of
 *  the data structure is from its creation by mdCreateChan till it
 *  is invalidated by mdDeleteChan.each instance object will have two channel
 *  object one for TX and one for the RX channel.
 */
typedef struct Mcbsp_ChannelObj_t
{
    Uint16                     mode;
    /**< Mode of operation of the channel (IOM_INPUT or IOM_OUTPUT)           */

    Mcbsp_DriverState          chanState;
    /**< operational state of the channel (opened/closed)                     */

    Ptr                        devHandle;
    /**< Pointer to McBSP device driver instance object                       */

    IOM_TiomCallback           cbFxn;
    /**< IOM call back function to be called once a packet is processed       */

    Arg                        cbArg;
    /**< Callback Function argument                                           */

    Ptr                        edmaHandle;
    /**< Handle to the EDMA3 driver(given by application)                     */

    Uint32                     edmaEventQue;
    /**< EDMA event queue to be used by the channel                           */

    EDMA3_RM_TccCallback       edmaCallback;
    /**< Edma callback function pointer                                       */

    Uint32                     xferChan;
    /**< eDMA  ChannelID                                                      */

    Uint32                     tcc ;
    /**< EDMA transfer completion code                                        */

    Uint32                     pramTbl[Mcbsp_MAXLINKCNT];
    /**< Logical channel numbers of EDMA, which are used for linking          */

    Uint32                     pramTblAddr[Mcbsp_MAXLINKCNT];
    /**<  Physical address of logical channel numbers of EDMA, which          *
     * are used for linking                                                   */

    QUE_Obj                    queuePendingList;
    /**< Queue to hold the pending packets received from the application      */

    QUE_Obj                    queueFloatingList;
    /**< Queue to manage floating packets in DMA                              */

    IOM_Packet                *tempPacket;
    /**< Temporary IOP holder                                                 */

    IOM_Packet                *dataPacket;
    /**< Temporary IOP holder used to load the next packet in to EDMA         */

    Uint32                     submitCount;
    /**< Number of submit calls pending in the driver                         */

    Mcbsp_BufferFormat         dataFormat;
    /**< Application supplied buffer format                                   */

    Bool                       nextFlag;
    /**< Flag to indicate if the state machine can be stopped or not          */

    volatile Bool              bMuteON;
    /**< Flag to set the mute ON/OFF status                                   */

    volatile Bool              paused;
    /**< Flag to indicate if the audio is paused or not                       */

    volatile Bool              flush;
    /**< Flag to indicate if the Driver IO request flush is set               */

    volatile Bool              isTempPacketValid;
    /**< Flag to indicate whether a valid packet is available in tempPacket   */

    Bool                       enableHwFifo;
    /**< whether the FIFO has to be enabled for this channel                  */

    Mcbsp_GblErrCallback       gblErrCbk;
    /**< Callback to called in case an error occurs(supplied by application)  */

    Uint32                     userDataBufferSize;
    /**< Size of the data buffer to be transferred                            */

    Ptr                        loopJobBuffer;
    /**< Buffer to be transferred when the loop job is running                */

    Uint16                     loopJobLength;
    /**<Length of userloop job for each serialiser                            */

    Uint32                     userLoopJobLength;
    /**< user specified loop job length to be used if supplied                */

    Uint32                     nextLinkParamSetToBeUpdated;
    /**<  Used to store the next index of link param to be updated            */

    volatile Bool              loopjobUpdatedinParamset;
    /**< Used to prevent updating second paramset with loopjob for            *
     * last datapacket edma callback                                          */

    Uint16                     roundedWordWidth;
    /**< Word length bytes to be transferred for DMA transaction              */

    Uint16                     currentDataSize;
    /**< data packet size of the currently transferring packet                */

    Uint32                      rxBytesIndex;
    /**< Number of RX bytes to process in the SPI mode                        */

    Uint32                      txBytesIndex;
    /**< Number of bytes to tranceive                                         */

    Mcbsp_DataConfig            chanConfig;
    /**< settings to configure the TX or RX hardware sections                 */

    Mcbsp_ClkSetup              clkSetup;
    /**< clock setup for the RX or the TX section                             */

    Mcbsp_McrSetup              multiChanCtrl;
    /**< multiple channel control settings                                    */

    Uint32                      chanEnableMask[4];
    /**< Mask of the channels to be enabled or disabled                       */

    Bool                        userLoopJob;
    /**< Option to indicate if the user loop job is used or driver loop job   */

    Int32                       currentError;
    /**< Current packet error status                                          */
                     
}Mcbsp_ChannelObj;
/**< Mcbsp channel Object                                                     */

/**
 * \brief   Mcbsp instance Object
 *
 * \note    This data structure holds the information pertaining to an instance
 *          of the Mcbsp device.it holds information like the current device
 *          state, handle to the McBSP channels. The data structure is
 *          initialized during "mdBindDev", which is called during DSP-BIOS
 *          initialization, and is persistent till it is invalidated by
 *          "mdUnBindDev".
 */
typedef struct Mcbsp_Object_t
{
    Int32                       instNum;
    /**< Instance number of the current instance                              */

    Mcbsp_DriverState           devState;
    /**< operational state of the driver (created/deleted)                    */

    Mcbsp_DevMode               mode;
    /**< Operating mode of the Mcbsp driver(Mcbsp/SPI slave/SPI master)       */

    Mcbsp_OpMode                opMode;
    /**< Operational mode of the driver(INT/DMA/POLLING)                      */

    Bool                        enablecache;
    /**< Buffer operations to be performed by the driver or not               */

    Mcbsp_HwInfo                hwInfo;
    /**< McBSP handle for initial port configuration                          */

    Uint32                      mcbspSpiFreq;
    /**< SPI frequncy to be used when the Mcbsp is operating in SPI mode      */
    
    Bool                        stopSmFsXmt;
    /**< TX state machine status(Stopped/Running)                             */

    Bool                        stopSmFsRcv;
    /**< RX state machine status(Stopped/Running)                             */

    Mcbsp_ChannelObj            xmtObj;
    /**< Transmit channel object                                              */

    Mcbsp_ChannelObj            rcvObj;
    /**< Receive channel object                                               */

    Mcbsp_srgConfig             srgrConfig;
    /**< configuration settings for the Sample rate generator                 */

    SWI_Obj                     swiObject;
    /**< Handle to the SWI for the TX FIFO handling (non loop job mode)       */

    Bool                        txSrgEnable;
    /**< Flag to indicate if the TX section needs the sample rate generator to*
     * be enabled                                                             */

    Bool                        rxSrgEnable;
    /**< Flag to indicate if the RX section needs the sample rate generator to*
     * be enabled                                                             */
    
    Bool                        srgConfigured;
    /**< Flag to indicate if the SRGR settings are configured or not          */

    volatile Bool               srgEnabled;
    /**< Flag to indicate if the sample rate generator is enabled and running */

    Bool                        txFsgEnable;
    /**< Flag to indicate if the TX section needs the frame sync generator to *
     * be enabled                                                             */

    Bool                        rxFsgEnable;
    /**< Flag to indicate if the RX section needs the frame sync generator to *
     * be enabled                                                             */

    Bool                        fsgConfigured;
    /**< Flag to indicate if the Framesync generator is configured or not     */
    
    volatile Bool               fsgEnabled;
    /**< Flag to indicate if the frame sync generator is enabled and running  */

    Uint32                      retryCount;
    /**< The retry count value to be used when waiting for the TX empty to be *
     * set                                                                    */

    Mcbsp_ClkStpMode            clkStpMode;
    /**< clock stop mode selection                                            */

    Bool                        loopJobMode;
    /**< Variable to check if the loop job mode is enabled or not             */

    Bool                        pscPwrmEnable;
    /**< Option to enable or disable the PSC control in the driver            */

#ifdef BIOS_PWRM_ENABLE
    Mcbsp_pwrmInfo              pwrmInfo;
#endif
    /**< structure to hold the PWMR related information                       */    
}Mcbsp_Object;
/**< Mcbsp instance Object                                                    */

/* -------- module-wide state -------- */
/**
 *  \brief module-wide state
 *
 *  inUse in module state points to an array whose size is soc specific
 *  Pointer used to match IDriver codebase.
 */
typedef struct Mcbsp_Module_State_t
{
    Bool             *inUse;
    /**< Variable to indicate if the instance is in use or not                */
} Mcbsp_Module_State;
/**< Module-wide state structure                                              */


Void Mcbsp_localResetCtrl(Mcbsp_Object *instHandle, Uint32 selectMask);

Void Mcbsp_localCompleteCurrentIo(Mcbsp_ChannelObj *chanHandle);

Void Mcbsp_localEdmaCallback(Uint32 tcc, EDMA3_RM_TccStatus status, Ptr data);

Void Mcbsp_localAbortReset(Mcbsp_ChannelObj *chanHandle);

Void Mcbsp_localGetNextIndex(Uint32 *index);

Int32 Mcbsp_localSetupEdmaDuringOpen(Mcbsp_ChannelObj *chanHandle);

#ifdef Mcbsp_SPI_ENABLE
Int32 Mcbsp_localConfigureInSpiMode(Mcbsp_Object     *instHandle,
                                    Mcbsp_ChanParams *params);

Int32 Mcbsp_localSpiEdmaChanPaRAMsetup(Mcbsp_ChannelObj *chanHandle);

Int32 Mcbsp_localSetupSpiEdmaDuringOpen(Mcbsp_Object     *instHandle,
                                        Mcbsp_ChannelObj *chanHandle);
#endif
Int32 Mcbsp_localSubmitIoctl(Mcbsp_ChannelObj *chanHandle,
                             Mcbsp_IOCTL       cmd,
                             Ptr               arg,
                             Ptr               param);

Int32 Mcbsp_localUpdtDtPktToLnkPrms(Mcbsp_ChannelObj *chanHandle,
                                    IOM_Packet       *const ioPacket);

Int32 Mcbsp_localEdmaChanPaRAMsetup(Mcbsp_ChannelObj *chanHandle);

Int32 Mcbsp_localEdmaProcessPkt(Mcbsp_ChannelObj *chanHandle,
                                IOM_Packet       *ioPacket);

Void Mcbsp_localCancelAndAbortAllIo(Mcbsp_ChannelObj *chanHandle);

Void Mcbsp_localAbortReset(Mcbsp_ChannelObj *chanHandle);

Int32 Mcbsp_localGetIndicesSyncType(Mcbsp_ChannelObj   *chanHandle,
                                    volatile Int16     *bIndex,
                                    volatile Int16     *cIndex,
                                    volatile Uint16    *aCnt,
                                    volatile Uint16    *bCnt,
                                    volatile Uint16    *cCnt,
                                    EDMA3_DRV_SyncType *syncType,
                                    Bool                forLoopJobBuf);

Void Mcbsp_localGetNextIndex(Uint32 *index);
Int32 Mcbsp_localConfigureSrgr(Mcbsp_Object     *instHandle,
                               Mcbsp_ChannelObj *chanHandle);
Int32 Mcbsp_localConfigureRcvChannel(Mcbsp_Object     *instHandle,
                                     Mcbsp_ChanParams *params);
Int32 Mcbsp_localConfigureXmtChannel(Mcbsp_Object     *instHandle,
                                     Mcbsp_ChanParams *params);
Int32 Mcbsp_localLpscOn(Mcbsp_Object *instHandle);
Int32 Mcbsp_localLpscOff(Mcbsp_Object *instHandle);
Void Mcbsp_localLoadPktToEdma(Mcbsp_ChannelObj *chanHandle,IOM_Packet *ioPacket); 
Void Mcbsp_localHandlePwrmEvent(Mcbsp_Object     *instHandle,
                                Mcbsp_ChannelObj *chanHandle);
Int32 Mcbsp_localModifySampleRate(Mcbsp_ChannelObj *chanHandle,Ptr arg);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _MCBSPLOCAL_H_ */
/*============================================================================*/
/*                         END OF FILE                                        */
/*============================================================================*/

