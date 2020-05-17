/*
 * Uart.h
 *
 * This file contains Application programming interface for the UART driver and 
 * command/macro definitions used by the UART driver.
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
 *  \mainpage    Uart Driver on BIOS 5.x
 *
 *  \section    Introduction
 *
 *  \subsection    xxx    Purpose and Scope
 *  The purpose of this document is to explain the Uart driver written for
 *  DSP/BIOS 5.x
 *
 *
 *  \subsection    bbb    UART Controller
 *  This module is a Universal Asynchronous Receiver/Transmitter (UART) with a 
 *  VBUS slave interface. This module performs serial-to-parallel conversion on 
 *  data received from a peripheral device or modem, and parallel-to-serial 
 *  conversion on data received from the VBUS central resource. The UART module 
 *  includes complete modem control capability and an interrupt system that can 
 *  be tailored to minimize software management of the communication link. This 
 *  module also includes a programmable baud rate generator capable of dividing 
 *  a reference clock by divisors from 1 to 65,535 to produce a 16 x clock 
 *  driving the internal logic.
 *
 *  \subsection    ccc    DSP/BIOS System
 *  DSP/BIOS is a real time kernel provided by TI to use with its
 *  DSP processors.
 *
 *  \subsection    ddd   Compiler defines to add/remove features
 *  Define to enable edma support. Use -DUart_EDMA_ENABLE when building library.
 *
 *  Define to enable TX buffering. Use -DUart_TX_BUFFERING_ENABLE when building.
 *
 *  Define to turn OFF parameter checking. 
 *  Use -DPSP_DISABLE_INPUT_PARAMETER_CHECK when building library
 *
 *  Define to turn OFF asserts. Use -DNDEBUG when building library.
 *
 *  \subsection    References
 *    -# DSP/BIOS driver development <b>SPRU616.pdf</b>
 *
 *  \file   ti/psp/uart/Uart.h
 *
 *  \brief  UART interface definition
 *
 *  This file contains the interfaces, data types and symbolic definitions
 *  that are needed by the application to utilize the serivces of the UART
 *  device driver.
 */
 
#ifndef _UART_H_
#define _UART_H_

#include <std.h>

#ifdef __cplusplus
extern "C" 
{
#endif

/* -------- enums -------- */

/**
 *  \brief  Uart Baud Rate
 */
typedef enum Uart_BaudRate {
    Uart_BaudRate_2_4K      = 2400u,     /**< Baudrate 2400    bps   */
    Uart_BaudRate_4_8K      = 4800u,     /**< Baudrate 4800    bps   */
    Uart_BaudRate_9_6K      = 9600u,     /**< Baudrate 9600    bps   */
    Uart_BaudRate_19_2K     = 19200u,    /**< Baudrate 19200   bps   */
    Uart_BaudRate_38_4K     = 38400u,    /**< Baudrate 38400   bps   */
    Uart_BaudRate_57_6K     = 57600u,    /**< Baudrate 57600   bps   */
    Uart_BaudRate_115_2K    = 115200u    /**< Baudrate 115200  bps   */      
}Uart_BaudRate;

/**
 *  \brief  Uart Character Length
 *
 *  \note   The enum values should not be changed since it represents the
 *          actual register configuration values used to configure the Uart in
 *          this SoC by the Uart driver
 */
typedef enum Uart_CharLen {
    Uart_CharLen_5 = 0x0u,        /**< Character Length 5  bits  */
    Uart_CharLen_6 = 0x1u,        /**< Character Length 6  bits  */
    Uart_CharLen_7 = 0x2u,        /**< Character Length 7  bits  */
    Uart_CharLen_8 = 0x3u         /**< Character Length 8  bits  */
}Uart_CharLen;

/**
 *  \brief  Uart Flow Control Parameters
 *
 *          User to select the mode of Uart Software and Hardware flow Control 
 *  \note   SW flow control is not supported in this driver implementation
 *
 */
typedef enum Uart_FcParam {
    Uart_FcParam_NONE           = 0,    /**< no flow control            */
    Uart_FcParam_XONXOFF_1      = 1,    /**< xon-xoff 1 flow control    */
    Uart_FcParam_XONXOFF_2      = 2,    /**< xon-xoff 2 flow control    */
    Uart_FcParam_XONXOFF_12     = 3,    /**< xon-xoff 12 flow control   */
    Uart_FcParam_AUTO_RTS_CTS   = 4,    /**< rts cts flow control       */
    Uart_FcParam_AUTO_CTS_ONLY  = 5     /**< cts only flow control      */
}Uart_FcParam;

/**
 *  \brief  Uart Flow Control
 *
 *  \note The enum values should not be changed since it represents the
 *   actual register configuration values used to configure the Uart in
 *   this SoC by the Uart driver
 */
typedef enum Uart_FcType {
    Uart_FcType_NONE    = 0,        /**< No Flow Control        */
    Uart_FcType_SW      = 1,        /**< Software Flow Control  */
    Uart_FcType_HW      = 2         /**< Hardware Flow Control  */
}Uart_FcType;

/**
 *  \brief  Uart Stop Bits
 *
 *  \note   The enum values should not be changed since it represents the
 *          actual register configuration values used to configure the Uart in
 *          this SoC by the Uart driver
 */
typedef enum Uart_NumStopBits {
    Uart_NumStopBits_1      = 0u,        /**< Stop Bits 0    bits    */
    Uart_NumStopBits_1_5    = 0x2u,      /**< Stop Bits 1.5  bits    */
    Uart_NumStopBits_2      = 0x4u       /**< Stop Bits 2    bits    */
}Uart_NumStopBits;

/**  
 *  \brief  Uart OpMode 
 *  
 *  \note   Enumeration of the different modes of operation available for the 
 *          Uart device.
 */ 
typedef enum Uart_OpMode {
    Uart_OpMode_POLLED = 0,         /**< Polled Mode            */
    Uart_OpMode_INTERRUPT,          /**< Interrupt Mode         */
    Uart_OpMode_DMAINTERRUPT        /**< DMA Mode               */
}Uart_OpMode;

/**
 *  \brief  Uart Parity
 *
 *  \note   The enum values should not be changed since it represents the
 *          actual register configuration values used to configure the Uart in
 *          this SoC by the Uart driver
 */ 
typedef enum Uart_Parity {
    Uart_Parity_NONE = 0x0u,         /**< Parity Bits NONE   bits    */
    Uart_Parity_ODD  = 0x8u,         /**< Parity Bits ODD    bits    */
    Uart_Parity_EVEN = 0x18u         /**< Parity Bits EVEN   bits    */
}Uart_Parity;

/**
 * \brief   Uart Rx Trigger Level Param
 *
 *  \note   The enum values should not be changed since it represents the
 *          actual register configuration values used to configure the Uart in
 *          this SoC by the Uart driver
 */
typedef enum Uart_RxTrigLvl {
    Uart_RxTrigLvl_1  = 1,           /**< Trigger Level 1    */
    Uart_RxTrigLvl_4  = 4,           /**< Trigger Level 4    */
    Uart_RxTrigLvl_8  = 8,           /**< Trigger Level 8    */
    Uart_RxTrigLvl_14 = 14           /**< Trigger Level 14   */
}Uart_RxTrigLvl;

/**
 *  \brief  Uart Ioctl commands
 */
typedef enum Uart_IOCTL {
    Uart_IOCTL_SET_BAUD = 128,
    /**< Set baud rate, cmdArg = Uart_BaudRate *                              */
    Uart_IOCTL_SET_STOPBITS,
    /**< Set number of stop bits, cmdArg = Uart_NumStopBits *                 */          
    Uart_IOCTL_SET_DATABITS,
    /**< Set number of Data bits, cmdArg = Uart_CharLen *                     */    
    Uart_IOCTL_SET_PARITY,
    /**< Set parity type, cmdArg = Uart_Parity *                              */
    Uart_IOCTL_SET_FLOWCONTROL,
    /**< Set flowcontrol, cmdArg = Uart_FlowControl *                         */
    Uart_IOCTL_SET_TRIGGER_LEVEL,
    /**< Changing Trigger level, cmdArg = Uart_RxTrigLvl *                    */
    Uart_IOCTL_RESET_RX_FIFO,
    /**< Resets the Uart HW RX FIFO, cmdArg = NONE                            */
    Uart_IOCTL_RESET_TX_FIFO,
    /**< Resets the Uart HW TX FIFO, cmdArg = NONE                            */
    Uart_IOCTL_CANCEL_CURRENT_IO,
    /**< Cancel the current IO in TX or RX channel, cmdArg = NONE             */
    Uart_IOCTL_GET_STATS,
    /**< Getting the Uart stats for DDC, cmdArg = Uart_Stats *                */
    Uart_IOCTL_CLEAR_STATS,
    /**< Clearing the Stats of DDC, cmdArg = NONE                             */
    Uart_IOCTL_FLUSH_ALL_REQUEST,
    /**< Flush all IO requests , cmdArg = NONE                                */
    Uart_IOCTL_SET_POLLEDMODETIMEOUT,
    /**< Set Polled Mode timeout, cmdArg = timeout value in ticks             */    
    Uart_IOCTL_SET_ERROR_CALLBACK
    /**< Set error callback function, cmdArg =  pointer to callback structure */    
}Uart_IOCTL;

/**
 * \brief  PLL domain to be used by the device
 */
typedef enum Uart_pllDomain_t
{
    Uart_pllDomain_0 = 0,
    /**< PLL domain 0          */

    Uart_pllDomain_1 = 1,
    /**< PLL domain 1          */

    Uart_pllDomain_NONE = 2
    /**< Not in any pll domain */

}Uart_pllDomain;

/* -------- structs -------- */

/**
 *  \brief  Uart channel config parameters
 *
 *  Uart Channel Params passed to Uart_open function
 */
typedef struct Uart_ChanParams {
    Ptr hEdma;
}Uart_ChanParams;

/**
 *  \brief  Uart Flow Control Configuration
 */
typedef struct Uart_FlowControl {
    Uart_FcType  fcType;                     /**< Flow Control type     */
    Uart_FcParam fcParam;                    /**< Flow Control param    */
}Uart_FlowControl;

/**
 *  \brief Uart Statistics Collection Object
 *
 *  Statistics are collected on a per-device basis for Uart.
 */
typedef struct Uart_Stats {
    Uint32 rxBytes;             /**< Number bytes received          */
    Uint32 txBytes;             /**< Number bytes transmitted       */
    Uint32 overrun;             /**< Number of overrun errors       */
    Uint32 rxTimeout;           /**< Number of Rx timeouts          */
    Uint32 rxFramingError;      /**< Number of Rx Framing errors    */
    Uint32 rxBreakError;        /**< Number of Rx Break Errors      */
    Uint32 rxParityError;       /**< Number of Rx Parity Errors     */
}Uart_Stats;

/**
 *  \brief Uart Instance configuration parameters
 *
 *  A pointer to such a structure is used when driver is instantiated
 */
typedef struct Uart_Params 
{
    Bool                enableCache;        /**< Driver will use cache APIs   */
    Bool                fifoEnable;         /**< Fifo mode of Operation       */
    Uart_OpMode         opMode;             /**< Driver operational mode      */
    Bool                loopbackEnabled;    /**< LoopBack Mode flag           */
    Uart_BaudRate       baudRate;           /**< Baudrate of Operation        */
    Uart_NumStopBits    stopBits;           /**< Stopbits of Operation        */
    Uart_CharLen        charLen;            /**< Character Length             */
    Uart_Parity         parity;             /**< Parity of Operation          */
    Uart_RxTrigLvl      rxThreshold;        /**< Rx FIFO trigger level        */
    Uart_FlowControl    fc;                 /**< Flow Control                 */
    Uint16              edmaRxTC;           /**< Edma Rx TC                   */
    Uint16              edmaTxTC;           /**< Edma Tx TC                   */
    Uint16              hwiNumber;          /**< Hwi number used for uart isr */
    Uint32              polledModeTimeout;  /**< Timeout used in polled mode  */
    Uint16              softTxFifoThreshold;/**< FIFO threshold level         */
    Bool                pscPwrmEnable;      /**< PSC control enable disable   */
    Uart_pllDomain      pllDomain;
    /**< Pll domain where the device is configured                            */ 
} Uart_Params;


/**
 * \brief Global error callback structure 
 *
 *        This is the global error callback function for the UART driver.
 *        This function is called directly called from ISR context in case of
 *        error. The first argument passed to this function is the user supplied
 *        callback argument, the second argument is the status of the line
 * \note  Since this function is called from an ISR context,care should be taken
 *        that this function conforms to ISR coding guidelines.
 */
typedef Void (*Uart_GblErrCallback)(Uint32 Arg1,Uint32 Arg2,Uint32 Arg3);
 
typedef struct Uart_GblErrCbk {
    Uart_GblErrCallback gblErrCbkFxn;
    Ptr                 gblErrCbkArg;
} Uart_GblErrCbk;
    
/* -------- defines -------- */

/** 
 *  \brief Number of channels per uart instance
 */
#define Uart_NUM_CHANS (2)

/** 
 *  \brief Maximum number of iterations allowed within ISR to check the 
 *         RX/TX status. 
 */        
#define Uart_MAX_ISR_LOOP (5)

/**
 *  \brief Tasklet priority for TX interrupt handling in SWI context of DSP
 */
#define Uart_TASKLET_PRIORITY (1)

/**
 *  \brief Uart FIFO Size
 */
#define Uart_FIFO_SIZE (16u)

/**
 *  \brief Baud Rate Multiplier 
 */
#define Uart_BAUD_MULTIPLIER (16u)

/**
 *  \brief No of iterations to be done before "timing out" during fifo write
 */
#define Uart_TRANSMITEMPTY_TRIALCOUNT (0xFFFFFF)

/**
 *  \brief Default Uart_Params struct
 *
 *  Following values are defaults and application can assign and change
 *  interested parameters.
 *
 *  const struct Uart_Params Uart_PARAMS = {
 *
 *      TRUE,                   enableCache;
 *
 *      TRUE,                   fifoEnable
 * 
 *      Uart_OpMode_INTERRUPT,  opMode
 *
 *      FALSE,                  loopbackEnabled
 *
 *      Uart_BaudRate_115_2K,   baudRate
 *
 *      Uart_NumStopBits_1,     stopBits
 *
 *      Uart_CharLen_8,         charLen
 *
 *      Uart_Parity_NONE,       parity
 * 
 *      Uart_RxTrigLvl_14,      rxThreshold
 *
 *      {                       fc
 *          Uart_FcType_NONE,
 *          Uart_FcParam_NONE
 *      },
 *
 *      0,                      edmaRxTC
 * 
 *      0,                      edmaTxTC
 *
 *      0                       hwiNumber
 *
 *      0xffffffff,             polledModeTimeout
 *
 *  }; 
 */
extern const Uart_Params Uart_PARAMS;

/**
 *  \brief Uart IOM_Fxns table
 *
 *  Driver function table to be used by applications. 
 */
extern IOM_Fxns Uart_IOMFXNS;

/**
 *  \brief    Initializes Uart instances which are statically allocated
 *
 *  This function needs to ve be called at part of BIOS initialization by
 *  setting initFxn for that particular UDEV instance or by calling this
 *  function as part of user specific initFxn.
 */
Void Uart_init(Void);

#ifdef __cplusplus
}
#endif

#endif /*_UART_H_ */
