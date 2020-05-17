/*
 * Spi.h
 *
 * This file contains Application programming interface for the SPI driver and 
 * command/macro definitions used by the SPI driver.
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
 *  \mainpage    PSP Spi Driver on BIOS 5.x
 *
 *  \section    Introduction
 *
 *  \subsection    xxx    Purpose and Scope
 *  The purpose of this document is to explain the Spi driver on the PSP
 *  Framework architecture written in DSP/BIOS
 *
 *
 *  \subsection    bbb    Spi Controller
 *  Spi is a high-speed synchronous serial input/output port that allows a
 *  serial bit stream of programmed length (one to 16 bits) to be shifted into
 *  and out of the device at a programmed bit-transfer rate. Spi is normally
 *  used for communication between the microcontroller and external peripherals
 *  or another microcontroller. Typical applications include interface to
 *  external I/O or peripheral expansion via devices such as shift
 *  registers, display drivers, and analog-to-digital converters.
 *
 *  \subsection    ccc    DSP/BIOS System
 *  DSP/BIOS is a real time kernel provided by TI to use with its
 *  DSP processors.
 *
 *  \subsection    ddd   Compiler defines to add/remove features
 *  Define to enable edma support. Use -DUart_EDMA_ENABLE when building library.
 *
 *  Define to turn OFF parameter checking. 
 *  Use -DPSP_DISABLE_INPUT_PARAMETER_CHECK when building library
 *
 *  Define to turn OFF asserts. Use -DNDEBUG when building library.
 *
 *
 *  \subsection    References
 *    -# DSP/BIOS driver development <b>SPRU616.pdf</b>
 *
 */
#ifndef _Spi_H_
#define _Spi_H_

#include <std.h>

#ifdef __cplusplus
extern "C" 
{
#endif

/*  -------- enums -------- */

/**
 *  \brief Enumeration to select the different data transfer format.
 */
typedef enum Spi_DataFormat {
    Spi_DataFormat_0,       /**< select format 0                              */
    Spi_DataFormat_1,       /**< select format 1                              */
    Spi_DataFormat_2,       /**< select format 2                              */
    Spi_DataFormat_3        /**< select format 3                              */
}Spi_DataFormat;

/**  
 *  \brief Enumeration of the different modes of operation
 */ 
typedef enum Spi_OpMode {
    Spi_OpMode_POLLED = 0,      /**< Polled Mode                              */
    Spi_OpMode_INTERRUPT,       /**< Interrupt Mode                           */
    Spi_OpMode_DMAINTERRUPT     /**< DMA Mode                                 */
}Spi_OpMode;

/**
 *  \brief Enumeration to set Spi master to different opmode
 */
typedef enum Spi_PinOpMode {
    Spi_PinOpMode_3PIN,         /**< Spi master 3 pin mode                    */
    Spi_PinOpMode_SPISCS_4PIN,  /**< Spi master 4 pin mode uses SpiSCS        */
    Spi_PinOpMode_SPIENA_4PIN,  /**< Spi master 4 pin mode uses SpiENA        */
    Spi_PinOpMode_5PIN          /**< Spi master 5 pin mode                    */
}Spi_PinOpMode;

/**
 *  \brief Enumeration to set Spi as master or slave
 *
 */
typedef enum Spi_CommMode {
    Spi_CommMode_MASTER,        /**< Spi master mode of operatoin             */
    Spi_CommMode_SLAVE          /**< Spi slave mode of operatoin              */
}Spi_CommMode;


/**
 * \brief  PLL domain to be used by the device
 */
typedef enum Spi_pllDomain_t
{
    Spi_pllDomain_0 = 0,
    /**< PLL domain 0       */

    Spi_pllDomain_1 = 1
    /**< PLL domain 1       */
}Spi_pllDomain;


/**
 *  \brief enumeration of IOCTLS Supported
 */
typedef enum Spi_ioctlCmd {
    Spi_IOCTL_CANCEL_PENDING_IO = 128,  
    /**< To cancel pending IO, cmdArg = NULL                                  */
    Spi_IOCTL_SET_CS_POLARITY,          
    /**< to set or reset CS Polarity, cmdArg = Bool *                         */
    Spi_IOCTL_SET_POLLEDMODETIMEOUT     
    /**< set polled mode timeout, cmdArg = Uint32 *                           */

}Spi_ioctlCmd;

/* -------- typedefs -------- */

typedef Void (*Spi_isr)(Ptr);

/* -------- structs -------- */
/**
 *  \brief Spi statistics Collection Object
 *
 *  Statistics are collected on a per-controller basis for Spi. Hence, an
 *  object of this type is found embedded in the instance structure.
 */
typedef struct Stats
{
    Uint32  rxBytes;                 /**< Number of bytes received            */
    Uint32  txBytes;                 /**< Number of bytes transmitted         */
    Uint32  pendingPacket;           /**< Number of pending packets           */
    Uint32  rxOverrunError;          /**< Number of overrun errors            */
    Uint32  timeoutError;            /**< Number of timeouts                  */
    Uint32  bitError;                /**< Number of biterrors                 */
    Uint32  parityError;             /**< Number of parity Errors             */
    Uint32  desyncError;             /**< Number of desync Errors             */
}Stats;

/**
 *  \brief Spi Channel Params Object
 */    
typedef struct Spi_ChanParams {
    Ptr hEdma;                      /**< Edma handle                          */
    Ptr hGpio;                      /**< Gpio handle                          */
}Spi_ChanParams;
    
/**
 *  \brief Structure to initialize the data transfer format register.
 */
typedef struct Spi_ConfigDataFmt {
    Uint32          wDelay;          
    /**< Delay in between two transfers                                       */
    Uint32          charLength;      
    /**< defines the word length- b/w 2-16                                    */               
    Bool            lsbFirst;          
    /**< Sets the Shift of bits, TRUE - LSB First ; FALSE - MSB First         */
    Bool            phaseIn;           
    /**< True - Phase In, data & clock in phase ; False - Phase out, data 1/2 
      * cycle before clock                                                    */
    Bool            oddParity;         
    /**< True - Odd Parity ; False - Even Parity, valid only if ParityEnable 
      * is true                                                               */
    volatile Bool   parityEnable; 
    /**< Parity check in the data format, True - Checks for parity, 
      * False - No Check                                                      */
    volatile Bool   clkHigh;  
    /**< Sets the polarity of the clock,  True - POLARITY HIGH , 
      * False - POLARITY LOW                                                  */
    volatile Bool   waitEnable;   
    /**< True - if in master mode,  wait for ENA signal from slave 
      * False - do not wait                                                   */
}Spi_ConfigDataFmt;

/**
  * \brief      Delay values for configuring the Spi.  All DELAY value is used
  *             in master mode only.
  *
  * c2TDelay -  It defines a setup time for the slave device that delays
  *                the data transmission from the chip select active edge.
  *
  * t2CDelay -  It defines a hold time for the slave device that delays
  *                the chip select deactivation by a multiple of VBUSPCLK
  *                cycles after the last bit is transferred.
  *
  * t2EDelay -  It defines a time-out value as a multiple of Spi clock before
  *                the ENAble signal has to become inactive and after the CS
  *                becomes  inactive.
  *
  * c2EDelay -  C2EDELAY defines the maximum time between the Spi activates the
  *                chip select signal and the addressed slave has to respond by
  *                activating the ENA signal.
  *                It applies only if the addressed slave generates an ENA
  *                signal as a hardware handshake response.
  *
  */
typedef struct Spi_DelayParam {
    Uint32 c2TDelay;    
    /**< [IN] Chip-select-active-to-transmit-start-delay                      */
    Uint32 t2CDelay;    
    /**< [IN] Transmit-end-to-chip-select-inactive-delay                      */
    Uint32 t2EDelay;    
    /**< [IN] Transmit-data-finished-to-ENA-pin-inactive-time-out             */
    Uint32 c2EDelay;    
    /**< [IN] Chip-select-active-to-ENA-signal-active-time-out                */
}Spi_DelayParam;

/**
 *  \brief Spi Hardware Configuartion Structure
 */
typedef struct Spi_HWConfigData {
    Bool                intrLevel;
    /**< True - maps to Intr1 Line ; false - Intr0 line                       */
    Spi_CommMode        masterOrSlave;
    /**< True - Slave Mode , False - Master Mode                              */
    Bool                clkInternal;
    /**< True - Selects Internal clock source ; False - Selects external      *
      *   clock source                                                        */
    Bool                enableHighZ;
    /**< whether ENA signal should be tristated when inactive - True ;        *
      *   or it should bear a value - False                                   */
    Spi_PinOpMode       pinOpModes;
    /**< Spi Operation Modes                                                  */
    Spi_DelayParam      delay;
    /**< Spi delay registers value                                            */
    Uint32              waitDelay;
    /**< True - enable format delay between 2 consecutive transfers ;         *
      *  False - No delay b/w 2 transfers                                     */
    Uint32              csDefault;
    /**< Default chip select pattern                                          */
    Spi_ConfigDataFmt   configDatafmt[4];
    /**< Data Format Configuration values                                     */
}Spi_HWConfigData;

/**
  * \brief Spi Transaction structure
  *
  * This structure holds the information needed to carry out a transaction on
  * an Spi bus to a slave device.
  */
typedef struct Spi_DataParam {
    Uint8*          outBuffer;
    /**< Data buffer - During transceieve operation this buffer data is       *
      * transmitted                                                           */
    Uint8*          inBuffer;
    /**< Data buffer - During transceive operation read data is stored        *
      * in this buffer                                                        */
    Uint32          bufLen;
    /**< Length of the buffer                                                 */
    Uint32          chipSelect;
    /**< chipselect ot be selected                                            */
    Spi_DataFormat  dataFormat;
    /**< which data format register to select                                 */
    Uint32          flags;
    /**< Flags  - To indicate the Read / Write modes of operation             */
    Ptr             param;
     /**< Extra paramerter                                                    */
    Uint32          gpioPinNum;
     /**< GPIO pin number incase the operation is on GPIO CS                  */
    Uint32          csToTxDelay;
    /**< The delay between GPIO cs assertion and actual IO (clk) start        */    
}Spi_DataParam;


/**
  * \brief Spi Chip select polarity command structure
  *
  * This structure holds the information needed for the command to decide the
  * chipselect polarity.
  */
typedef struct Spi_CsPolarity {
    Bool          isCsActiveHigh;
    /**< isCsActiveHigh - Set to TRUE if active high behaviour is require else*
      * set to false if active low polarity is required                       */
    Uint32        csMask;
    /**< csMask - the mask of chipselects on which the operation is needed    */
}Spi_CsPolarity;


/* -------- per-instance configs -------- */

/**
 *  \brief Spi Per instance configuration struct
 */
typedef struct Spi_Params
{
    Bool                enableCache;
    /**< enable/disable cache operations in driver                            */

    Spi_OpMode          opMode;
    /**< Driver operation mode                                                */

    Uint32              outputClkFreq;
    /**< output clock frequency from Spi Module , transmits bitrate
      * in bits per second                                                    */

    Bool                loopbackEnabled;
    /**< To enable/disable loop back                                          */

    Int32               polledModeTimeout;
    /**< Polled mode timeout                                                  */

    Spi_HWConfigData    spiHWCfgData;
    /**< Hardware configuration                                               */

    Ptr                 edmaHandle;
    /**< Handle for Edma                                                      */

    Uint16              hwiNumber;
    /**< hwi number configured for spi event                                  */

    Bool                pscPwrmEnable;
    /**< Option to enable or disable the PSC control in the driver            */

    Spi_pllDomain       pllDomain;
    /**> Pll domain where the device is                                       */

}Spi_Params;

/**
 *  \brief Default Spi_Params struct
 *
 *  const Spi_Params Spi_PARAMS = {
 *
 *      Spi_OpMode_POLLED,              opMode
 *
 *      (Uint32)0x2dc6c0,               outputClkFreq
 *
 *      0,                               loopbackEnabled
 * 
 *      SYS_FOREVER,                     timeout
 * 
 *      {
 *          0,                           intrLevel
 * 
 *          Spi_CommMode_MASTER,         masterOrSlave
 *
 *          1,                           clkInternal
 * 
 *          0,                           enableHighZ
 * 
 *          Spi_PinOpMode_SpiSCS_4PIN,   pinOpModes
 * 
 *          {
 *              (Uint32)0x0,             c2TDelay
 * 
 *              (Uint32)0x0,             t2CDelay
 * 
 *              (Uint32)0x0,             t2EDelay
 * 
 *              (Uint32)0x0,             c2EDelay
 * 
 *          },                           delay 
 *
 *          (Uint32)0x0,                 waitDelay
 *
 *          (Uint32)0xff,                csDefault
 *
 *          {
 *              {
 *              (Uint32)0x0,             wDelay
 * 
 *              (Uint32)0x0,             charLength
 * 
 *              0,                       lsbFirst 
 *
 *              0,                       phaseIn 
 *
 *              0,                       oddParity 
 *
 *              0,                       parityEnable
 * 
 *              0,                       clkHigh
 *
 *              0,                       waitEnable
 * 
 *          },                           [0]
  
 *          {
 *              (Uint32)0x0,             wDelay
 * 
 *              (Uint32)0x0,             charLength
 * 
 *              0,                       lsbFirst
 * 
 *              0,                       phaseIn
 * 
 *              0,                       oddParity
 * 
 *              0,                       parityEnable
 * 
 *              0,                       clkHigh
 *
 *              0,                       waitEnable
 * 
 *          },                           [1]
  
 *          {
 *              (Uint32)0x0,             wDelay
 * 
 *              (Uint32)0x0,             charLength
 * 
 *              0,                       lsbFirst 
 *
 *              0,                       phaseIn
 * 
 *              0,                       oddParity
 * 
 *              0,                       parityEnable
 * 
 *              0,                       clkHigh 
 *
 *              0,                       waitEnable
 * 
 *          },                           [2]
  
 *          {
 *              (Uint32)0x0,             wDelay
 * 
 *              (Uint32)0x0,             charLength
 * 
 *              0,                       lsbFirst
 * 
 *              0,                       phaseIn
 * 
 *              0,                       oddParity
 * 
 *              0,                       parityEnable
 * 
 *              0,                       clkHigh
 * 
 *              0,                       waitEnable
 * 
 *          },                           [3]
  
 *      },                               configDatafmt
 * 
 *  },                                   spiHWCfgData
 * 
 *  ((Ptr)((void*)0x0)),                 edmaHandle
 * 
 *  (Uint16)0x0,                         HWINumber
 * 
 *};
 * 
 */
extern const Spi_Params Spi_PARAMS;

/**
 *  \brief Spi IOM_Fxns table
 *
 *  Driver function table to be used by applications. 
 */
extern IOM_Fxns Spi_IOMFXNS;

/* -------- defines -------- */

/**
 *  \brief   Local Buffer data allocation for transcieve operation. 
 */
#define Spi_BUFFER_DATA_SIZE (32u)
/**
 *  \brief   Max channels supported
 * 
 *          Max Spi driver opens - which means number of s/w channels which 
 *          can be opened for a single instance of Spi
 */
#define Spi_NUM_CHANS (1)
/**
 * \brief   Macro used to enable/Disable CSHOLD  
 */
#define Spi_CSHOLD ((0x1u))

/**
 * \brief  Macro used to enable/Disable use of GPIO pin as chip select.
 */
#define Spi_GPIO_CS  (0x2u)

/**
 * \brief   Macro for CSHOLD multi transceive
 *
 *  CSHOLD will be selected [pulled low]after the trasceive operation and
 *  this is valid if Spi_CSHOLD flag is enabled. Also if Chip select value is
 *  changed in the next transceive operation then also this flag becomes
 *  invalid for the previous rtansceive operation i.e CS lines comes high. 
 */
#define Spi_CSHOLD_FOR_MULTI_TRANSCEIVE (0x4u)

/** Macro used to set gpio pin as active low                              */
#define Spi_LOW   (0x0u)

/** Macro used to set gpio pin as active high                             */
#define Spi_HIGH  (0x1u)



/**
 *  \brief    Initializes Spi instances which are statically allocated
 *
 *  This function needs to be be called at part of BIOS initialization by
 *  setting initFxn for that particular UDEV instance or by calling this
 *  function as part of user specific initFxn.
 */
Void Spi_init(Void);

#ifdef __cplusplus
}
#endif

#endif /*_Spi_H_ */
