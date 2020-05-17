/*
 * Lidd.h
 *
 * This file contains Application programming interface for the LIDD driver and 
 * command/macro definitions used by the LIDD driver.
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

/** \mainpage    PSP LCDC Lidd Driver on BIOS 5.x
 *
 *  \section    Introduction
 *
 *  \subsection    xxx    Purpose and Scope
 *  The purpose of this document is to explain the LCDC Lidd driver written
 *  for DSP/BIOS 5.x.
 *
 *  \subsection    aaa    LCDC Lidd
 *
 *  \subsection    ddd   Compiler defines to add/remove features
 *
 *  Define to turn OFF parameter checking.
 *  Use -DPSP_DISABLE_INPUT_PARAMETER_CHECK when building library
 *
 *  Define to turn OFF asserts. Use -DNDEBUG when building library.
 *
 *  \subsection    bbb    DSP/BIOS System
 *  DSP/BIOS is a real time kernel provided by TI to use with its
 *  DSP processors.
 *
 *  \subsection    References
 *    -# DSP/BIOS driver development <b>SPRU616.pdf</b>
 *
 *  \file  ti/pspiom/lcdclidd/Lidd.h
 *
 *  \brief  Lidd interface definition
 *
 *  This file contains the interfaces, data types and symbolic definitions
 *  that are needed by the application to utilize the serivces of the Lidd
 *  device driver.
 */

#ifndef Lidd_H_
#define Lidd_H_

#include <std.h>
#include <iom.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* -------- enums -------- */

/**
* \brief Controller
*
* Enumerates controller types supported by the device
*/
typedef enum Lidd_Controller {
    /* Undefined type of controller           */
    Lidd_Controller_UNDEFINED = 0x0,
    /* Synchronous controller in LCDC         */
    Lidd_Controller_RASTER = 0x1,
    /* Asynchronous controller in LCDC        */
    Lidd_Controller_LIDD = 0x2
}Lidd_Controller;

/**
* \brief Display Type
*
* This enumerates the type of the display connected to the controller
*/
typedef enum Lidd_DisplayType {
    Lidd_DisplayType_SyncMPU68 = 0x00,
    Lidd_DisplayType_AsyncMPU68 = 0x01,
    Lidd_DisplayType_SyncMPU80 = 0x02,
    Lidd_DisplayType_AsyncMPU80 = 0x03,
    Lidd_DisplayType_Hitachi = 0x04
}Lidd_DisplayType;

/* -------- structs -------- */

/**
 * \brief ChanParams
 *
 * Structure to hold the parameters for Lcd Lidd. This will act as paramters for
 * the channel itself.
 */
typedef struct Lidd_ChanParams {
    /* To indicate controller type*/
    Lidd_Controller controller;
    /* To contain the controller config*/
    Ptr chanConf;
}Lidd_ChanParams;

/**
 * \brief CursorPosition
 *
 * This structure is to pass the command arguments to the IOCTL.
 */
typedef struct Lidd_CursorPosition {
    /* The column  position of the character to be set                    */
    Uint32 col;
    /* The row    position of the character to be set                     */
    Uint32 line;
    /* The chip select on which the cursor position should be set         */
    Uint32 chipSel;
}Lidd_CursorPosition;

/**
 * \brief DataParam
 *
 * This struture contains the paramters that need to be passed for every
 * submission.
 */
typedef struct Lidd_DataParam {
    /* The buffer pointer containing the actual data                      */
    Uint8* buffer;
    /* Length of the data that is being written                           */
    Uint32 len;
    /* Primary or the secondary device to be written to                   */
    Uint8 chipSel;
}Lidd_DataParam;

/**
 * \brief  PLL domain to be used by the device
 */
typedef enum Lidd_pllDomain_t
{
    Lidd_pllDomain_0 = 0,
    /**< PLL domain 0       */

    Lidd_pllDomain_1 = 1
    /**< PLL domain 1       */
}Lidd_pllDomain;

/**
 *  \brief DeviceConf
 *
 *  This structure defines different configuration options available for the
 *  LCDC device.
 */
typedef struct Lidd_DeviceConf {
    /**< Clock divisor to specify PCLK                                      */
    Uint32           clkFreqHz;

    /**< HWI number associated with this device event                       */
    Int32            hwiNum;

    /**< Numbmer of lines in the display                                    */
    Uint32           numLines;

    /**< Number of characters per line in the display.  Essentially, the
       display shall be (numCharPerLine X numLines)       */
    Uint32           numCharPerLine;

    /**< This is array of addresses (hex) for starting character of each line.
     * This is required to move to the next line in continous write. This
     * address list is specific to the display type and hence the user
     * should populate this. By default the driver shall write starting at
     * line 'one' character 'one', until (numLines * numCharPerLine) is
     * is reached. For now we shall limit to 4 line display
     */
    Uint32           addressArray[4];

    Bool             pscPwrmEnable;
    /**< PSC module power managment enable & disable control                  */
    
    Lidd_pllDomain   pllDomain;
}Lidd_DeviceConf;

/**
 * \brief IoctlCmdArg
 *
 * This structure is to pass the command arguments to the IOCTL.
 */
typedef struct Lidd_IoctlCmdArg {
    /**< Data, if required for the command                                  */
    Uint32 val;
    /**< Primary or the secondary device to be acted upon                   */
    Uint8 chipSel;
}Lidd_IoctlCmdArg;

/**
 * This structure is used to provide the configuration for the read / write
 * strobe signals.
 */
typedef struct Lidd_StrobeTiming {
    /**< Write setup cycles                                                 */
    Uint32 wSu;
    /**< Write strobe duration                                              */
    Uint32 wStrobe;
    /**< Write strobe hold duration                                          */
    Uint32 wHold;
    /**< Write setup cycles                                                 */
    Uint32 rSu;
    /**< Write strobe duration                                              */
    Uint32 rStrobe;
    /**< Write strobe hold duration                                          */
    Uint32 rHold;
    /**< Turn around time between successive access                          */
    Uint32 ta;
}Lidd_StrobeTiming;

/**
 * \brief DisplayConf
 *
 * This structure contains the configuration for the LIDD controller.
 */
typedef struct Lidd_DisplayConf {
    /**< The type of display being configured                               */
    Lidd_DisplayType displayType;
    /**< Read/Write strobe timing configuration for Primary Device connected  *
       on the Chip Select 0                                                   */
    Lidd_StrobeTiming cs0Timing;
    /**< Read/Write strobe timing configuration for Primary Device connected  *
       on the Chip Select 1                                                   */
    Lidd_StrobeTiming cs1Timing;
    
    /**< The chip select the on which the display is connected                */
    Uint32   chipSel;
}Lidd_DisplayConf;

/**
 *  \brief  Lidd IOCTL commands
 *
 *  Lidd IOCTL commands
 */
typedef enum Lidd_IOCTL {

    /**< Undefined. To be used only by driver                                 */
    Lidd_IOCTL_UNDEFINED,
    /**< Clear the screen of the character display                            */
    Lidd_IOCTL_CLEAR_SCREEN,
    /**< Set the cursor to the home position                                  */
    Lidd_IOCTL_CURSOR_HOME,
    /**< Move cursor to a particular position                                 */
    Lidd_IOCTL_SET_CURSOR_POSITION,
    /**< Turn the display on                                                  */
    Lidd_IOCTL_SET_DISPLAY_ON,
    /**< Turn the display off                                                 */
    Lidd_IOCTL_SET_DISPLAY_OFF,
    /**< Turn the cursor blink on                                             */
    Lidd_IOCTL_SET_BLINK_ON,
    /**< Turn the cursor blink off                                            */
    Lidd_IOCTL_SET_BLINK_OFF,
    /**< Turn the cursor display on                                           */
    Lidd_IOCTL_SET_CURSOR_ON,
    /**< Turn the cursor display off                                          */
    Lidd_IOCTL_SET_CURSOR_OFF,
    /**< Turn the display shift on                                            */
    Lidd_IOCTL_SET_DISPLAY_SHIFT_ON,
    /**< Turn the display shift off                                           */
    Lidd_IOCTL_SET_DISPLAY_SHIFT_OFF,
    /**< Move the cursor to the left                                          */
    Lidd_IOCTL_CURSOR_MOVE_LEFT,
    /**< Move the cursor to the right                                         */
    Lidd_IOCTL_CURSOR_MOVE_RIGHT,
    /**< Move the display to the left*/
    Lidd_IOCTL_DISPLAY_MOVE_LEFT,
    /**< Move the display to the right                                        */
    Lidd_IOCTL_DISPLAY_MOVE_RIGHT,
    /**< Move the display to the right                                        */
    Lidd_IOCTL_COMMAND_REG_WRITE

}Lidd_IOCTL;

/* -------- per-instance configs -------- */

/**
 *  \brief Lidd parameters
 *
 *  Structure used to configure a Lidd driver instance.
 */
typedef struct Lidd_Params {
    Lidd_DeviceConf devConf;
}Lidd_Params;

/**
 *  \brief Default Lidd_Params struct
 *  \code
 *  const struct Lidd_Params Lidd_PARAMS = {
 *      {                                   // DeviceConf struct
 *          5000000,                        // clkFreqHz
 *          0,		// hwiNum
 *          0,		// numLines
 *          0,		// numCharPerLine
 *          {			
 *              0,		// addressArray[0]
 *              0,		// addressArray[1]
 *              0,		// addressArray[2]
 *              0,		// addressArray[3]
 *          }
 *      }
 * };
 *  \endcode
 */
extern const Lidd_Params Lidd_PARAMS;

/**
 *  \brief Lidd IOM_Fxns table
 *
 *  Driver function table to be used by applications.
 */
extern const IOM_Fxns Lidd_IOMFXNS;

/**
 * \brief    Initializes Lidd instances which are statically allocated
 *
 * This function needs to be called at part of BIOS initialization by
 * setting initFxn for that particular UDEV instance or by calling this
 * function as part of user specific initFxn.
 */
Void Lidd_init(Void);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* Lidd_H_ */

