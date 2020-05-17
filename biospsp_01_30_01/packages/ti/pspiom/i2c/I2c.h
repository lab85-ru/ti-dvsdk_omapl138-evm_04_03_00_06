/*
 * I2c.h
 *
 * This file contains Application programming interface for the I2C driver and 
 * command/macro definitions used by the I2C driver.
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

/** \mainpage    PSP I2c Driver on BIOS 5.x
 *
 *  \section    Introduction
 *
 *  \subsection    xxx    Purpose and Scope
 *  The purpose of this document is to explain the I2c driver written for
 *  DSP/BIOS 5.x.
 *
 *  \subsection    aaa    I2c Controller
 *  The I2c bus is a multi master bus which allows more than one device capable
 *  of controlling the bus to be connected to it. Each I2c device is recognized
 *  by a unique address and can operate as either transmitter or receiver
 *  depending on the function of the device. In addition to being a transmitter
 *  or receiver, device connected to the I2C bus can also be considered as
 *  master or slave when performing data transfers. Note that a master device
 *  is the device which initiates a data transfer on the bus and generates
 *  the clock signals to permit that transfer. During this transfer, any device
 *  addressed by this master is considered a slave.
 *
 *  \subsection    bbb    DSP/BIOS System
 *  DSP/BIOS is a real time kernel provided by TI to use with its
 *  DSP processors.
 *
 *  \subsection    ccc   Compiler defines to add/remove features
 *  Define to enable edma support. Use -DI2c_EDMA_ENABLE when building library.
 *
 *  \subsection    References
 *    -# DSP/BIOS driver development <b>SPRU616.pdf</b>
 *
 *  \file   ti/pspiom/i2c/I2c.h
 *
 *  \brief  I2c interface definition
 *
 *  This file contains the interfaces, data types and symbolic definitions
 *  that are needed by the application to utilize the serivces of the I2c
 *  device driver.
 */

#ifndef _I2C_H_
#define _I2C_H_

#include <std.h>
#include <iom.h>

#ifdef __cplusplus
extern "C"
{
#endif


/** The following flags offer the user maximum flexibility in terms of
 *  making the right I2C transaction. In case the user does not want
 *  to set the flags on his own, the default read/write flag can be
 *  specified
 *
 *  NOTE: If no flag is specified by default in the flags, read is assumed
 */

/** Read from I2C bus (device)                                                */
#define I2c_READ            0x1u

/** Write to I2C bus (device)                                                 */
#define I2c_WRITE           0x2u

/** Ignore Bus Busy condition                                                 */
#define I2c_IGNORE_BUS_BUSY 0x20000u

/** Ignore NACK from device                                                    */
#define I2c_IGNORE_NACK     0x10000u

/** If this flag is not set, default is slave mode                            */
#define I2c_MASTER          0x20u

/** Repeat mode as per TI I2C specs                                           */
#define I2c_REPEAT          0x2000u

/** Re-Start is generated by Master                                           */
#define I2c_RESTART         0x400u

/** Generate Start - valid in master mode only                                */
#define I2c_START           0x100u

/** Generate Stop - valid in master mode only                                 */
#define I2c_STOP            0x200u

/** Default read flag                                                         */
#define I2c_DEFAULT_READ    (I2c_READ  | I2c_MASTER | I2c_START | I2c_STOP)

/** Default write flag                                                        */
#define I2c_DEFAULT_WRITE   (I2c_WRITE | I2c_MASTER | I2c_START | I2c_STOP)

/** I2c peripheral clock frequency                                            */
#define I2c_peripheralClkFreq 12000000u

#define I2c_SLAVE_TX_DATA   (0xFF)
/**< Default data to be transmitted by the slave when enough buffer is not
 * provided by the application for servicing a slave transmitter operation    */

/* -------- enums -------- */

/**  Enumeration of Master mode and slave modes of operation.  Only one channel
 *   is allowed to be opened in slave mode; however more than one channel may
 *   be opened by the driver in master mode (up to the max number of channels
 *   supported, which is configurable).
 */
typedef enum I2c_CommMode {
    I2c_CommMode_MASTER = 0, /**< Sets communication mode to master.          */
    I2c_CommMode_SLAVE       /**< Sets communication mode to slave.           */
}I2c_CommMode;

/**  Enumeration of the different modes of operation available for the
*    I2c driver.
*/
typedef enum I2c_OpMode {
    I2c_OpMode_POLLED = 0,  /**< Sets operation mode to polling.              */
    I2c_OpMode_INTERRUPT,   /**< Sets operation mode to interrupt.            */
    I2c_OpMode_DMAINTERRUPT /**< Sets operation mode to EDMA interrupt.       */
}I2c_OpMode;


/**
 * \brief  PLL domain to be used by the device
 */
typedef enum I2c_pllDomain_t
{
    I2c_pllDomain_0 = 0,
    /**< PLL domain 0          */

    I2c_pllDomain_1 = 1,
    /**< PLL domain 1          */

    I2c_pllDomain_NONE = 2
    /**< Not in any pll domain */

}I2c_pllDomain;

/* -------- structs -------- */

/** Main structure used for transactions with the I2c device.                 */
typedef struct I2c_DataParam {
    Uint32  slaveAddr; /**< Address of the slave to talk to                   */
    Uint8*  buffer;    /**< Data Buffer                                       */
    Uint32  bufLen;    /**< Length of buffer                                  */
    Uint32  flags;     /**< Flags for modes of operation                      */
    Ptr     param;     /**< Extra parameter for future use                    */
}I2c_DataParam;

/** Structure of the channel parameters required while creating the
*   the channel.
*/
typedef struct I2c_ChanParams {
    Ptr             hEdma;         /**< EDMA handle. Used in DMA opmode       */
    I2c_CommMode    masterOrSlave; /**< Master or slave channel               */
}I2c_ChanParams;

/**
 * I2C statistics Collection Object
 *
 * Statistics are collected on a per-controller basis for I2C. Hence, an
 * object of this type is found embedded in the instance structure.
 */
typedef struct I2c_Stats {
    Uint32  rxBytes;               /**< Number of bytes received              */
    Uint32  txBytes;               /**< Number of bytes transmitted           */
    Uint32  pendingPacket;         /**< Number of pending packets             */
    Uint32  overFlowError;         /**< Number of overflow errors             */
    Uint32  timeoutError;          /**< Number of timeouts                    */
    Uint32  arbLossError;          /**< Number of arbitration loss errors     */
    Uint32  nackError;             /**< Number of nack Errors                 */
}I2c_Stats;

/**
 *  \brief  I2c IOCTL commands
 *
 *  I2c IOCTL commands
 */
typedef enum I2c_IOCTL {

    /**< IOCTL for setting the I2c bit rate, cmdArg = Uint32 *                */
    I2c_IOCTL_SET_BIT_RATE = 128,

    /**< IOCTL for getting the current bit rate of the I2c bus, cmdArg = Uint32 * */
    I2c_IOCTL_GET_BIT_RATE,

    /**< IOCTL for cancelling the current pending IO, cmdArg = NONE           */
    I2c_IOCTL_CANCEL_PENDING_IO,

    /**< IOCTL for setting the I2c bit count, cmdArg = Uint32 *               */
    I2c_IOCTL_BIT_COUNT,

    /**< IOCTL for setting the I2c NACK, cmdArg = Bool *                      */
    I2c_IOCTL_NACK,

    /**< IOCTL for setting the I2c device own address, cmdArg = Uint32 *      */
    I2c_IOCTL_SET_OWN_ADDR,

    /**< IOCTL for getting the I2c device own address, cmdArg = Uint32 *      */
    I2c_IOCTL_GET_OWN_ADDR,

    /**< IOCTL for setting timeout in polled mode, cmdArg = Uint32 *          */
    I2c_IOCTL_SET_POLLEDMODETIMEOUT,
    
    /**< IOCTL for soft reset of the I2C module, cmdArg = NONE                */
    I2c_IOCTL_DEVICE_SOFT_RESET    

}I2c_IOCTL;

/* -------- per-instance configs -------- */

typedef struct I2c_Params
{
    Bool                enableCache;
    /**< Submitted buffers are in cacheable memory                            */

    I2c_OpMode          opMode;
    /**< Driver operating mode: polled, interrupt, or edma interrupt modes    */

    Uint32              ownAddr;
    /**< Own address (7 or 10 bit)                                            */

    Uint32              numBits;
    /**< Number of bits/byte to be sent/received                              */

    Uint32              busFreq;
    /**< I2C Bus Frequency                                                    */

    Bool                addressing;
    /**< 7bit/10bit Addressing mode                                           */

    Bool                loopbackEnabled;
    /* Digital Loob Back (DLB) mode                                           */

    Int32               edma3EventQueue;
    /**< Edma event Q to be requested, while requesting an EDMA3 channel      */

    Uint16              hwiNumber;
    /**< Hardware interrupt Number                                            */

    Uint32              polledModeTimeout;
    /**< Timeout to be used in polled mode                                    */

    Bool                pscPwrmEnable;
    /**< PSC module power managment enable & disable control                  */

    I2c_pllDomain       pllDomain;
    /**< Pll domain where the device is                                       */
}I2c_Params;

/**
 *  \brief Default I2c_Params struct
 *
 *  const struct I2c_Params I2c_PARAMS = {
 *      TRUE,                 enableCache
 *      I2c_OpMode_POLLED,    mode
 *      0x10u,                ownAddr
 *      8,                    numBits
 *      200000,               busFreq
 *      0,                    addressing
 *      0,                    loopBackEnabled
 *      0,                    edma3EventQueue
 *      0,                    hwiNumber
 *      0xffffffff            timeout
 *  };
 */
extern const I2c_Params I2c_PARAMS;

/**
 *  \brief I2c IOM_Fxns table
 *
 *  Driver function table to be used by applications. 
 */
extern const IOM_Fxns I2c_IOMFXNS;

/**
 * \brief    Initializes I2c instances which are statically allocated
 *
 * This function needs to be called at part of BIOS initialization by
 * setting initFxn for that particular UDEV instance or by calling this
 * function as part of user specific initFxn.
 */
Void I2c_init(Void);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _I2C_H_ */

