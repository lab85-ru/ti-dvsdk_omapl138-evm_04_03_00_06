/*
 * Mcbsp.h
 *
 * This file contains Application programming interface for the Mcbsp driver and
 * command/macro definitions used by the Mcbsp driver.
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
 * \file        Mcbsp.h
 *
 * \brief       McBSP driver interface definition file
 *
 *              This file contains the interfaces, data types and symbolic
 *              definitions that are needed by the application to utilize the
 *              services of the McBSP device driver.
 *
 *              (C) Copyright 2008, Texas Instruments, Inc
 *
 *  \authors    Platform Support Group
 *
 *  \note       Set tabstop to 4 (:se ts=4) while viewing this file in an
 *              editor
 *
 *  \version    
 *              0.2   Modified for the Non loop job mode.
 *              0.1   created newly
 *
 */

#ifndef _MCBSP_H_
#define _MCBSP_H_

/*============================================================================*/
/*                            INCLUDE FILES                                   */
/*============================================================================*/

#include <std.h>
#include "ti/pspiom/mcbsp/mcbsp.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/*                         ENUMERATED DATA TYPES                              */
/*============================================================================*/

/**
 * \def    Mcbsp_CACHE_LENGTH
 *         Defines the Maximum length of cache line that is possible.
 */
#define Mcbsp_CACHE_LENGTH        (128u)

/**
 * \def    Mcbsp_SWI_PRIORITY
 *         Defines the priority of the SWI that handles the TX FIFO emptying
 *         when the Mcbsp is Running in non loop job mode.
 */
#define Mcbsp_SWI_PRIORITY        (0x01)

/**
 * \def    Mcbsp_POLLED_RETRYCOUNT
 *         This macro specifies the default retry count to be used by the Mcbsp
 *         driver when polling for any hardware bit to be set or reset.
 *
 * \note   This default value used by the driver can be changed by an IOCTL
 *         "Mcbsp_Ioctl_SET_TIMEOUT".
 */
#define Mcbsp_POLLED_RETRYCOUNT   (0xFFFFu)

/*============================================================================*/
/*                         ENUMERATED DATA TYPES                              */
/*============================================================================*/

/**
 *  \brief  Mcbsp device operational mode.
 *
 *          This enum is used to define the operational mode of the mcbsp device
 *          like normal mcbsp device or spi device master or slave mode.
 */
typedef enum Mcbsp_DevMode_t
{
    Mcbsp_DevMode_McBSP      = (0u),
    /**< Option to operate in normal McBSP mode     */

    Mcbsp_DevMode_SPIMASTER  = (1u),
    /**< Option to operate McBSP in SPI master mode */

    Mcbsp_DevMode_SPISLAVE   = (2u)
    /**< Option to operate McBSP in SPI slave mode  */

}Mcbsp_DevMode;
/**< Mcbsp device operational mode.       */

/**
 *  \brief  Mcbsp driver operational mode
 *
 *          Enumeration of the different modes of operation available for the
 *          Mcbsp device driver.(Mcbsp driver supports only EDMA mode).
 */
typedef enum Mcbsp_OpMode_t
{
    Mcbsp_OpMode_POLLED = 0,
    /**< Polled Mode                     */

    Mcbsp_OpMode_INTERRUPT,
    /**< Interrupt Mode                  */

    Mcbsp_OpMode_DMAINTERRUPT
    /**< DMA Mode                        */

}Mcbsp_OpMode;
/**< Mcbsp driver operational mode       */

/**
 * \brief   Mcbsp Emulation mode settings
 *
 *          Use this symbol to set the Emulation Mode to Free mode or any other 
 *          mode.
 *
 * \note    The SOFT mode settings are applicable only is the FREE mode is
 *          disabled.Hence if the soft mode is selected (both enable or disable)
 *          the driver will internally disable the FREE mode.
 */
typedef enum Mcbsp_EmuMode_t
{
    Mcbsp_EmuMode_SOFT_ABORT  = (0u),
    /**< SOFT mode is disabled.          */

    Mcbsp_EmuMode_SOFT_STOP   = (1u),
    /**< SOFT mode is enabled            */

    Mcbsp_EmuMode_FREE        = (2u)
    /**< FREE mode is enabled            */

}Mcbsp_EmuMode;
/**< Mcbsp emulation modes               */

/**
 *  \brief  McBSP digital loopback mode selection
 *
 *          Enum to select the enable/disable of Loopback mode of the mcbsp.
 */
typedef enum Mcbsp_Loopback_t
{
    Mcbsp_Loopback_DISABLE = 0,
    /**< loopback mode off                 */

    Mcbsp_Loopback_ENABLE = 1
    /**< loopback mode on                  */

} Mcbsp_Loopback;
/**< McBSP digital loopback mode selection */

/**
 * \brief   ENUM for receive data justification settings
 *
 *          Use this symbol for setting up RCV sign-extension and justification
 *          mode
 */
typedef enum Mcbsp_Rxjust_t
{
    Mcbsp_RxJust_RZF        = (0u),
    /**< RCV setting - right justify, fill MSBs with zeros                    */

    Mcbsp_RxJust_RSE        = (1u),
    /**< RCV setting - right justify, sign-extend the data into MSBs          */

    Mcbsp_RxJust_RxJUST_LZF = (2u)
    /**< RCV setting - left justify, fill LSBs with zeros                     */

}Mcbsp_Rxjust;
/**< Enumeration for the Receive data justification                           */

/**
 * \brief   Transmit pin mode configuration
 *
 *          DX pin settings used for setting the pin in high impedance mode.
 */
typedef enum Mcbsp_DxEna_t
{
    Mcbsp_DxEna_OFF  = (0),
    /**< DX enabler is OFF                       */

    Mcbsp_DxEna_ON   = (1u)
    /**< DX enabler is ON                        */
}Mcbsp_DxEna;
/**< DX pin High impedance enable/disable option */

/**
 * \brief   Mcbsp Clock stop mode options.
 *
 */
typedef enum Mcbsp_ClkStpMode_t
{
    Mcbsp_ClkStpMode_DISABLED = (0u),
    /**< Clock stop mode is disabled.            */
    
    Mcbsp_CLKStpMode_NO_DELAY = (2u),
    /**< clock starts immedialtely without delay */
    
    Mcbsp_CLKStpMode_DELAY    = (3u)
    /**< clock starts after delay                */

}Mcbsp_ClkStpMode;
/**< Mcbsp Clock stop mode options.                                           */

/**
 * \brief Polarity of the CLKS pin
 *
 *        Enum to Set the polarity of the CLKS pin used to generate Frame Sync
 *        and bit clock
 */
typedef enum Mcbsp_ClkSPol_t
{
    Mcbsp_ClkSPol_RISING_EDGE   = 0,
    /**< Rising edge of CLKS generates CLKG and FSG.                          */

    Mcbsp_ClkSPol_FALLING_EDGE  = 1
    /**< Falling edge of CLKS generates CLKG and FSG.                         */

}Mcbsp_ClkSPol;
/**< Polarity of the CLKS pin                                                 */

/**
 * \brief   SRG clock source
 *
 *          Use this symbol to select input clock source for Sample Rate
 *          Generator
 */
typedef enum Mcbsp_SrgClk_t
{
    Mcbsp_SrgClk_CLKS   = (0u),
    /**< input clock source for Sample Rate Generator is CLKS pin             */

    Mcbsp_SrgClk_CLKCPU = (1u),
    /**< input clock source for Sample Rate Generator is CPU                  */

    Mcbsp_SrgClk_CLKR   = (2u),
    /**< input clock source for Sample Rate Generator is BCLKR pin            */

    Mcbsp_SrgClk_CLKX   = (3u)
    /**< input clock source for Sample Rate Generator is BCLKX pin            */

}Mcbsp_SrgClk;
/**< SRG clock source selection                                               */

/**
 * \brief  PLL domain to be used by the device
 */
typedef enum Mcbsp_pllDomain_t
{
    Mcbsp_pllDomain_0 = 0,
    /**< PLL domain 0          */

    Mcbsp_pllDomain_1 = 1,
    /**< PLL domain 1          */

    Mcbsp_pllDomain_NONE = 2
    /**< Not in any pll domain */

}Mcbsp_pllDomain;

/**
 * \brief Enum to specify the supported buffer formats.
 *
 * Interleaved and non-interleaved is standard format, this enumeration
 * captures the standard and custom data formats.
 */
typedef enum Mcbsp_BufferFormat_t
{
    /* This mode is used for buffer containing the data in continous      *
     * memory locations where each sample is  "wordWidth" long. This      *
     * format is supported for following configurations                   *
     * Note : interleaved and non interleaved does not apply for this     *
     * buffer format                                                      */
    Mcbsp_BufferFormat_1SLOT,

    /* This is used for transfer of data with multiple slots.please note  *
     * that the slot data is not interleaved in this format.              */
    Mcbsp_BufferFormat_MULTISLOT_NON_INTERLEAVED,

    /* This is used for transfer of data for multiple slots.please note   *
     * that the slot data is interleaved in this format.                  */
    Mcbsp_BufferFormat_MULTISLOT_INTERLEAVED
}Mcbsp_BufferFormat;
/**< Mcbsp supported buffer formats                                       */

/**
 * \brief Mcbsp PHASE control enums
 *
 * \note These enums are used to control the Phase settings of the MCBSP.
 */
typedef enum Mcbsp_Phase_t
{
    Mcbsp_Phase_SINGLE = (0u),
    /**< Single phase for frame                */

    Mcbsp_Phase_DUAL   = (1u)
    /**< Dual phase for frame                  */
} Mcbsp_Phase;
/**< Mcbsp frame phase selection ENUMERATION   */

/**
 *  \brief  Mcbsp supported wordlength.
 *
 *          Enumerated constant for wordlength supported by the MCBSP device
 */
typedef enum Mcbsp_WordLength_t
{
    Mcbsp_WordLength_8  = 8u,
    /**< Word length of 8 bits            */

    Mcbsp_WordLength_12 = 12u,
    /**< Word length of 12 bits           */

    Mcbsp_WordLength_16 = 16u,
    /**< Word length of 16 bits           */

    Mcbsp_WordLength_20 = 20u,
    /**< Word length of 20 bits           */

    Mcbsp_WordLength_24 = 24u,
    /**< Word length of 24 bits           */

    Mcbsp_WordLength_32 = 32u
    /**< Word length of 32 bits           */

}Mcbsp_WordLength;
/**< Mcbsp supported wordlength.          */

/**
 * \brief   Frame sync ignore status enum
 *
 * \note    Use this symbol to detect or ignore frame synchronisation
 */
typedef enum Mcbsp_FrmSync_t
{
    Mcbsp_FrmSync_DETECT  = (0u),
    /**< detect frame synchronisation          */

    Mcbsp_FrmSync_IGNORE  = (1u)
    /**< ignore frame synchronisation          */

}Mcbsp_FrmSync;
/**< Frame sync detection options ENUMERATION  */

/**
 * \brief   Enum to select Data delay in bits
 *
 *          Use this Enum to set XMT/RCV Data Delay (in bits)
 */
typedef enum Mcbsp_DataDelay_t
{
    Mcbsp_DataDelay_0_BIT = (0u),
    /**< sets XMT/RCV Data Delay to 0 bits     */

    Mcbsp_DataDelay_1_BIT = (1u),
    /**< sets XMT/RCV Data Delay to 1 bits     */

    Mcbsp_DataDelay_2_BIT = (2u)
    /**< sets XMT/RCV Data Delay to 2 bits     */

} Mcbsp_DataDelay;
/**< Enum to select Data delay in bits         */

/**
 * \brief   Enum to select the companding law
 *
 *          Use this Enum to select the appropriate companding rule applicable.
 */
typedef enum Mcbsp_Compand_t
{
    Mcbsp_Compand_OFF_MSB_FIRST =  (0u),
    /**< No companding, data transfer starts with MSB first.                  */

    Mcbsp_Compand_OFF_LSB_FIRST =  (1u),
    /**< No companding, 8-bit data transfer starts with LSB first.            */

    Mcbsp_Compand_MULAW         =  (2u),
    /**< mu-law comapanding enable for channel                                */

    Mcbsp_Compand_ALAW          =  (3u)
    /**< A-law comapanding enable for channel                                 */

}Mcbsp_Compand;
/**< Enum to select the companding law         */

/**
 * \brief   McBSP 32-bit reversal feature
 *
 *          This ENUM allows the user to select the option of BIT reversal.
 */
typedef enum Mcbsp_BitReversal_t
{
    Mcbsp_BitReversal_DISABLE   = (0u),
    /**< 32-bit reversal disabled                                             */

    Mcbsp_BitReversal_ENABLE    = (1u)
    /**< 32-bit reversal enabled. 32-bit data is received LSB first. Word     *
     * length should be set for 32-bit operation; else operation undefined    */

}Mcbsp_BitReversal;
/**< McBSP 32-bit reversal feature                                            */

/**
 * \brief   Frame sync clock source
 *
 *          Use this ENUM to set the frame sync clock source as internal or
 *          external
 */
typedef enum Mcbsp_FsClkMode_t
{
    Mcbsp_FsClkMode_EXTERNAL = (0u),
    /**< frame sync clock source as internal     */

    Mcbsp_FsClkMode_INTERNAL = (1u),
    /**< frame sync clock source as external     */

    Mcbsp_FsClkMode_DXR_XSR  = (2u)
    /**< frame sync is generated on DXR_XSR copy */
    
} Mcbsp_FsClkMode;
/**< Frame sync clock source                     */

/**
 * \brief   Clock source selection ENUM
 *
 *          Use this symbol to set the clock source as internal or external
 */
typedef enum Mcbsp_TxRxClkMode_t
{
    Mcbsp_TxRxClkMode_EXTERNAL = (0u),
    /**< clock source as internal */

    Mcbsp_TxRxClkMode_INTERNAL = (1u)
    /**< clock source as external */

} Mcbsp_TxRxClkMode;
/**< Clock source selection ENUM  */

/**
 * \brief   Clock polarity
 *
 *          Use this symbol to set XMT or RCV clock polarity as rising or
 *          falling edge
 */
typedef enum Mcbsp_ClkPol_t
{
    Mcbsp_ClkPol_RISING_EDGE  = (0u),
    /**< Data sampled on rising edge of the bit Clock                         */

    Mcbsp_ClkPol_FALLING_EDGE = (1u),
    /**< Data sampled on falling edge of the bit Clock                        */

    Mcbsp_ClkPol_SRG_RISING_EDGE = (0u),
    /**< SRG clock polarity is rising edge      */

    Mcbsp_ClkPol_SRG_FALLING_EDGE = (1u)
   /**< SRG clock polarity Is falling edge      */

} Mcbsp_ClkPol;
/**< Clock polarity                             */

/**
 * \brief   Frame sync polarity
 *
 *          Use this symbol to set frame sync polarity as active-high or
 *          active-low
 */
typedef enum Mcbsp_FsPol_t
{
    Mcbsp_FsPol_ACTIVE_HIGH   = (0u),
    /**< frame sync polarity is active-high */

    Mcbsp_FsPol_ACTIVE_LOW    = (1u)
    /**< frame sync polarity is active-low  */

}Mcbsp_FsPol;
/**< Frame sync polarity                                                      */

/**
 * \brief   MCBSP Interrupt mode
 *
 *          Use this symbol to set Interrupt mode (i.e. source of interrupt
 *          generation).This symbol is used on both RCV and XMT for RINT and
 *          XINT generation mode.
 */
typedef enum Mcbsp_IntMode_t
{
    Mcbsp_IntMode_ON_READY   = (0u),
    /**< Interrupt generated on RRDY of RCV or XRDY of XMT                    */

    Mcbsp_IntMode_ON_EOB     = (1u),
    /**< Interrupt generated on end of 16-channel block transfer in           *
     *   multichannel mode                                                    */

    Mcbsp_IntMode_ON_FSYNC   = (2u),
    /**< Interrupt generated on frame sync                                    */

    Mcbsp_IntMode_ON_SYNCERR = (3u)
    /**< Interrupt generated on synchronisation error                         */

}Mcbsp_IntMode;
/**< MCBSP Interrupt mode                                                     */

/**
 * \brief    Transmit multichannel selection mode bit.
 *            
 *           MCM determines whether all channels or only selected channels are 
 *           enabled and unmasked for transmission/reception.
 *
 */
typedef enum Mcbsp_McmMode_t
{
    Mcbsp_McmMode_ALL_CHAN_ENABLED_UNMASKED  = (0u),
    /**< All the 128 channels are enabled                                     */

    Mcbsp_McmMode_ALL_CHAN_DISABLED_UNMASKED = (1u),
    /**< All channels are disabled unless selected by enable mask             */

    Mcbsp_McmMode_ALL_CHAN_ENABLED_MASKED    = (2u),
    /**< All channels are enabled but masked unless selected by Enable mask   */

    Mcbsp_McmMode_ALL_CHAN_DISABLED_MASKED   = (3u)
    /**< Symmetric transmission and reception                                 */

}Mcbsp_McmMode;
/**< Transmit multichannel selection mode bit.                                */

/**
 * \brief      Partition A/B block channel selection bit.
 */
typedef enum Mcbsp_PartitionSel_t
{
    Mcbsp_PartitionMode_CHAN_0_15    = (0u),
    /**< Select the channel 0-15 in block A     */
    
    Mcbsp_PartitionMode_CHAN_16_31   = (0u),
    /**< Select the channel 16-31 in block B    */

    Mcbsp_PartitionMode_CHAN_32_47   = (1u),
    /**< Select the channel 32-47 in block A    */

    Mcbsp_PartitionMode_CHAN_48_63   = (1u),
    /**< Select the channel 48-63 in block B    */
    
    Mcbsp_PartitionMode_CHAN_64_79   = (2u),
    /**< Select the channel 64-79 in block A    */

    Mcbsp_PartitionMode_CHAN_80_95   = (2u),
    /**< Select the channel 80-95 in block B    */

    Mcbsp_PartitionMode_CHAN_96_111  = (3u),
    /**< Select the channel 96-111 in block A   */

    Mcbsp_PartitionMode_CHAN_112_127 = (3u)
    /**< Select the channel 112-127 in block B  */

}Mcbsp_PartitionSel;
/**< Partition A/B block selection bit. */

/**
 * \brief    Multichannel partition mode selection.
 *
 *           Multichannel mode allows selection of either an 2 partition mode or
 *           8 partition mode.This enum allows the user to select the required
 *           partition mode of choice.
 */
typedef enum Mcbsp_PartitionMode_t
{
    Mcbsp_PartitionMode_2 = (0u),
    /**< 2-partition mode. Only partitions A and B are used.      */
    
    Mcbsp_PartitionMode_8 = (1u)
    /**< 8-partition mode. All partitions (A through H) are used  */

}Mcbsp_PartitionMode;
/**< Multichannel partition mode selction                         */

/*============================================================================*/
/*                              DATA STRUCTURES                               */
/*============================================================================*/

/**
 *  \brief McBSP sample rate generator configuration structure
 *
 *         Configurations for the Sample rate  generator to generate the BCLK 
 *         and Frame Sync signals are specified using this structure.
 */
typedef struct Mcbsp_srgConfig_t
{
    Bool             gSync;
    /**< sample rate generator clock syncronization bit (only if CLKS is used)*/

    Mcbsp_ClkSPol    clksPolarity;
    /**< CLKS polarity used to drive the CLKG and FSG clocks                  */

    Mcbsp_SrgClk     srgInputClkMode;
    /**< Source for the Sample rate generator (CLKS,CPU,CLKX,CLKR)            */

    Uint32           srgrInputFreq;
    /**< input clock frequency for the SRGR (freq of CLKS or CLKX etc..)      */

    Uint32           srgFrmPulseWidth;
    /**< Set the Frame Sync Pulse width in terms of FSG clock                 */

} Mcbsp_srgConfig;
/**< McBSP sample rate generator configuration structure                      */

/**
 * \brief   Mcbsp TX/RX section configuration structure.
 *
 *          This strcuture specifies the configuration for the McBSP data stream
 *          including the whether it is single phase or dual phase, number of
 *          frames,the word length in each phase and data delay etc.
 */
typedef struct Mcbsp_DataConfig_t
{
    Mcbsp_Phase               phaseNum;
    /**< Phase of the McBSP data stream                                       */

    Mcbsp_WordLength          wrdLen1;
    /**< Length of the data word in first phase                               */

    Mcbsp_WordLength          wrdLen2;
    /**< Length of the data word in second when dual phase is selected        */

    Uint32                    frmLen1;
    /**< Length of the data frame in first phase                              */

    Uint32                    frmLen2;
    /**< Length of the data frame in second phase                             */

    Mcbsp_FrmSync             frmSyncIgn;
    /**< Frame sync ignore bit                                                */

    Mcbsp_DataDelay           dataDelay;
    /**< Data delay to be configured in number of Bits                        */

    Mcbsp_Compand             compandSel;
    /**< companding selection                                                 */

    Mcbsp_BitReversal         bitReversal;
    /**< Transmit 32-bit bit reversal feature enable bit.                     */

    Mcbsp_IntMode             intMode;
    /**< Event for which the interrupt is to be generated                     */

    Mcbsp_Rxjust              rjust;
    /**< Receive sign extension and justification settings (RX only setting)  */
    
    Mcbsp_DxEna               dxState;
    /**< High impedance enable/disbale bit for the DX pin (TX only setting)   */
    
}Mcbsp_DataConfig;
/**< Mcbsp TX/RX section configuration structure.                             */

/**
 * \brief   Mcbsp multi channel control settings
 *
 *          structure to configure the multi channel settings for the mcbsp.used
 *          when the multi channel configuration is to be enabled.
 */
typedef struct Mcbsp_McrSetup_t
{
    Mcbsp_McmMode          multiChanMode;
    /**< Multi channel mode to be selcted                                     */  

    Mcbsp_PartitionSel     partitionSelA;
    /**< Channel selection for partition A                                    */

    Mcbsp_PartitionSel     partitionSelB;
    /**< Channel selection for partition B                                    */

    Mcbsp_PartitionMode    partitionMode;
    /**< Channel partition mode selection                                     */
}Mcbsp_McrSetup;
/**< Mcbsp multi channel control settings                                     */

/**
 * \brief   Mcbsp clock settings setup structure.
 *
 *          This structure contains the information required to configure the 
 *          clocks for the Mcbsp.Both the frame sync settings and the bit clock
 *          settings can be configured in this structure.
 */
typedef struct Mcbsp_ClkSetup_t
{
    Mcbsp_FsClkMode        frmSyncMode;
    /**< Frame sync mode bit (FSXM/FSRM)(Internal/External)                   */

    Uint32                 samplingRate;
    /**< Frame sync frequency                                                 */

    Mcbsp_TxRxClkMode      clkMode;
    /**< Bit clock mode (internal or external)                                */

    Mcbsp_FsPol            frmSyncPolarity;
    /**< frmSyncTxPolarity                                                    */

    Mcbsp_ClkPol           clkPolarity;
    /**< clkTxPolarity                                                        */
}Mcbsp_ClkSetup;
/**< Mcbsp clock settings setup structure                                     */

/*============================================================================*/
/*                               DATA TYPES                                   */
/*============================================================================*/

/**
 * \brief Global error callback function protype
 *
 *        This is the global error callback function for the McBSP driver.
 *        This function is called directly called from ISR context in case of
 *        error.
 * \note  Since this function is called from an ISR context,care should be taken
 *        that this function conforms to ISR coding guidelines.
 */
typedef Void (*Mcbsp_GblErrCallback)(Uint32 Arg1,Uint32 Arg2,Uint32 Arg3);

/**
 *  \brief  McBSP SPI Buffer format
 *
 *          This structure is the format of the SPI buffer to be supplied by the
 *          application when the mcbsp is in Operating in the SPI mode.
 *
 *  \note   This buffer is used when the Mcbsp is used in the SPI mode of
 *          operation.please note that the SPI mode support should also be
 *          supported by the underlying hardware.
 */
typedef struct Mcbsp_TransParam_t
{

    Uint8           *txBufAddr;
    /**< Transmit data buffer pointer   */

    Uint8           *rxBufAddr;
    /**< Receive data buffer pointer    */

}Mcbsp_TransParam;
/**< McBSP SPI Buffer format            */


/**
 *  \brief  McBSP channel setup params
 *
 *          This structure holds configuration to be used for
 *          creating a channel of the Mcbsp. These parameters need to be 
 *          specified during the creation of the channel.
 */
typedef struct Mcbsp_ChanParams_t
{
    Uint32                  wordWidth;
    /**< This parameter informs the driver what is the width word (not        *
     * slot) and this help driver indirectly to decided no. of bytes to       *
     * be transfered for each slot- This is very                              *
     * important parameter - in case of invalid value default value           *
     * driver will assume is 32                                               */

    Ptr                     userLoopJobBuffer;
    /**< Buffer to be transferred when the loop job is running.               */

    Uint16                  userLoopJobLength;
    /**< Number of bytes of the userloopjob buffer for each slot
     * Please note that this is no. of bytes and this should be
     * pre-calcuated properly for word width of slot - Please refer the
     * wordWidth of this structure                                            */

    Mcbsp_GblErrCallback    gblCbk;
    /**< callback required when global error occurs - must be callable        *
     * directly from the ISR context                                          */

    Ptr                     edmaHandle;
    /**< Handle to the EDMA Driver                                            */

    Uint32                  edmaEventQue;
    /**< EDMA event queue to be used by the channel                           */

    Uint32                  hwiNumber;
    /**< Variable to specify the Hwi number to be used by the driver          */

    Mcbsp_BufferFormat      dataFormat;
    /**< Format of the application supplied buffer                            */

    Bool                    enableHwFifo;
    /**< Option to enable/disable the Hardware FIFO                           */

    Mcbsp_DataConfig       *chanConfig;
    /**< settings to configure the TX or RX hardware sections                 */

    Mcbsp_ClkSetup         *clkSetup;
    /**< clock setup for the RX or the TX section                             */

    Mcbsp_McrSetup         *multiChanCtrl;
    /**< multiple channel control settings                                    */

    Uint32                  chanEnableMask[4];
    /**< Mask of the channels to be enabled or disabled                       */

}Mcbsp_ChanParams;
/**< Mcbsp Channel setup parameters                                           */

/**
 *  \brief  McBSP device create params
 *
 *          This structure holds the configuration to be used for the
 *          creation of the Mcbsp device instance.This configuration need to be
 *          supplied during the creation of the device instance.
 */
typedef struct Mcbsp_Params_t
{
    Mcbsp_DevMode           mode;
    /**< Mode in which mcbsp instance needs to be created                     */

    Mcbsp_OpMode            opMode;
    /**< Operation mode of driver i.e. interrupt or EDMA.Mcbsp supports only
     *   EDMA mode                                                            */

    Bool                    enablecache;
    /**< whether the cache operations are to be performed on the application
     *   buffers or not                                                       */

    Mcbsp_EmuMode           emulationMode;
    /**< Emulation mode settings for the Mcbsp                                */

    Mcbsp_Loopback          dlbMode;
    /**< digital loop back mode (ENABLE/DISABLE)                              */

    Mcbsp_ClkStpMode        clkStpMode;
    /**< Clock settings to be configured if the Mode used is SPI              */

    Uint32                  mcbspSpiFreq;
    /**< SPI frequency to be used when Mcbsp is operating in SPI mode         */

    Mcbsp_srgConfig        *srgSetup;
    /**< configuration for the sample rate generator configurations           */

    Bool                    pscPwrmEnable;
    /**< Option to enable or disable the PSC control in the driver            */

    Mcbsp_pllDomain         pllDomain;
    /**> Pll domain where the device is                                       */

} Mcbsp_Params;
/**< McBSP device setup params                                                */

/*============================================================================*/
/*                           IOCTL COMMANDS                                   */
/*============================================================================*/

/**
 *  \brief  McBSP Ioctl commands
 *
 *          List of all the ioctl commands supported by the Mcbsp driver.
 *
 */
typedef enum Mcbsp_IOCTL_t
{
    Mcbsp_IOCTL_START = 128,
    /**< Starts the data transfer                                             */

    Mcbsp_IOCTL_SAMPLE_RATE_CHANGE,
    /**< Command to chnage the sample rate                                    */

    Mcbsp_IOCTL_STOP,
    /**< Stops the data transfer                                              */

    Mcbsp_IOCTL_SRGR_START,
    /**< Start the McBSP SRG                                                  */

    Mcbsp_IOCTL_SRGR_STOP,
    /**< Stop the McBSP SRG                                                   */

    Mcbsp_IOCTL_FSGR_START,
    /**< Start the McBSP FSG                                                  */

    Mcbsp_IOCTL_FSGR_STOP,
    /**< Stop the McBSP FSG                                                   */

    Mcbsp_IOCTL_SET_TIMEOUT,
    /**< Modify the timeout value in the driver                               */

    Mcbsp_IOCTL_MUTE_ON,
    /**< Mute ON the transfer                                                 */

    Mcbsp_IOCTL_MUTE_OFF,
    /**< Take out of Muted state                                              */

    Mcbsp_IOCTL_PAUSE,
    /**< Pause the playback operation                                         */

    Mcbsp_IOCTL_RESUME,
    /**< Resume the playback operation                                        */

    Mcbsp_IOCTL_CHAN_RESET,
    /**< Reset an I/O channel                                                 */

    Mcbsp_IOCTL_DEVICE_RESET,
    /**< Reset both input and output channel                                  */

    Mcbsp_IOCTL_SET_CLKMODE,
    /**< Set Bit clock mode for the McBSP                                     */

    Mcbsp_IOCTL_SET_FRMSYNCMODE,
    /**< Set Frame Sync mode for the McBSP                                    */

    Mcbsp_IOCTL_CONFIG_SRGR,
    /**< configure Sample Rate Generator                                      */

    Mcbsp_IOCTL_SET_BCLK_POL,
    /**< Set the CLKR or CLKX clock polarity                                  */

    Mcbsp_IOCTL_SET_FRMSYNC_POL,
    /**< Set the FSR and FSX polarity                                         */

    Mcbsp_IOCTL_MODIFY_LOOPJOB,
    /**< Enable/disable the loopjob                                           */

    Mcbsp_IOCTL_SYNCERR_INT_ENABLE,
    /**< Enable/disable the sync error                                        */

    Mcbsp_IOCTL_LOOPBACK,
    /**< enable/disable the loop back mode                                    */

    Mcbsp_IOCTL_CANCEL_PENDING_IO,
    /**< Cancel all the current pending IO in the driver                      */

    Mcbsp_IOCTL_SPI_CHAN_RESET,
    /**< Reset the SPI device                                                 */

    Mcbsp_IOCTL_SPI_DEVICE_RESET,
    /**< Reset the SPI device                                                 */

    Mcbsp_IOCTL_SPI_SET_CS_POL,
    /**< Reset the SPI set the chip select polarity                           */

    Mcbsp_IOCTL_SPI_SET_CLKX_POL
    /**< Reset the SPI set the transmit clock polarity                        */
    
} Mcbsp_IOCTL;
/**< McBSP Ioctl commands list */

/**
 * \brief   Default parameters to be used by the mcbsp driver
 *
 */
extern const Mcbsp_Params Mcbsp_PARAMS;

/**
 * \brief   Initializes McBSP IOM driver's data structures
 *
 *          This function initializes the McBSP IOM driver's data structures
 *          including instance objects and channel objects.This function needs
 *          to be called at part of BIOS initialization by setting initFxn for
 *          that particular UDEV instance or by calling this function as part of
 *          user specific initFxn.
 *
 * \return  None
 */
Void Mcbsp_init(Void);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _MCBSP_H_ */
/*============================================================================*/
/*                         END OF FILE                                        */
/*============================================================================*/

