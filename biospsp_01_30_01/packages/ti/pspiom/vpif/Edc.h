/*
 * Edc.h
 *
 * This file contains interface definition for a generic external devices (video
 * encoder/decoder) interface which can be used/implemented by various external
 * devices (video encode/decoder) drivers.
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
 *  \file   Edc.h
 *
 *  \brief  This file contains common defines and structures for external
 *          devices
 */

#ifndef _EDC_H_
#define _EDC_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

/* ========================================================================== */
/*                             EDC TYPEDEF                                    */
/* ========================================================================== */
/** \brief EDC handle type casting */
typedef Ptr  EDC_Handle;

/* ========================================================================== */
/*                             EDC ENUMS                                      */
/* ========================================================================== */
/**
 *  \brief  EDC Ioctl commands
 * Following IOCTL commands will be used to perform control operation on EDC
 * device. The following Ioctl commands are common for encoder and decoders. The
 * IOCTL command is passed as second argument to ctrl() function pointer of the
 * EDC device
 * NOTE: These IOCTL's will be available only if user adds Vpif_IOCTL_CMD_MAX to
 * the IOCTL calls from application.
 */
typedef enum Edc_IOCTL_t
{
    Edc_IOCTL_CONFIG = 0,
    /**< EDC configure command
     *   Application has to pass appropriate config pointer described
     *   in the encoder or decoder file as a third argument in ctrl() function.
     *   For example
     *   Tvp5147_ConfParams      config;
     *   cmdArg = (&config);                                                  */
    Edc_IOCTL_RESET,
    /**< EDC reset command
     *   This will reset the EDC device. Only this ioctl command needs to be
     *   passed as second argument in ctrl() function.                        */
    Edc_IOCTL_SET_REG,
    /**< Command to write/set the EDC registers
     *   Application has to pass pointer to Edc_RegData structure variable as
     *   a third argument in ctrl() function.
     *   For example
     *   Edc_RegData writeData;
     *   Uint8 val;
     *   writeData.startReg = 0x00u;
     *   writeData.noRegToRW = 1u;
     *   val = 0xCE;
     *   writeData.value = (&val);
     *   cmdArg = (&writeData);                                               */
    Edc_IOCTL_GET_REG,
    /**< Command to read/get the EDC registers
     *   Application has to pass pointer to Edc_RegData structure variable as
     *   a third argument in ctrl() function.
     *   For example
     *   Edc_RegData readData;
     *   Uint8 val;
     *   readData.startReg = 0x00u;
     *   readData.noRegToRW = 1u;
     *   readData.value = (&val);
     *   cmdArg = (&readData);                                                */
    Edc_IOCTL_WRITE_SLICE_VBI_DATA,
    /**< Writes Slice VBI data for encoder like ADV7343. This IOCTL if
     *   supported will be mentioned in the respective EDC header file.
     *   User has to pass pointer to FVID_SliceFrame structure
     *   variable as a third argument to ctrl() function.
     *   Generally this IOCTL will be used by VPIF layer to get VBI data and
     *   put it inside the vpif Frame packet
     *   For example
     *   FVID_SliceFrame vbiData;
     *   Uint8 data[48];
     *   vbiData.fvidSliceServiceId = Fvid_SLICE_VBI_SERVICES_CC_NTSC;
     *   vbiData.fvidField = 0;
     *   vbiData.fvidData = data;
     *   cmdArg = (&vbiData);                                                 */
    Edc_IOCTL_READ_SLICE_VBI_DATA,
    /**< Reads Slice VBI data for decoders like TVP5147. This IOCTL if
     *   supported will be mentioned in the respective EDC header file.
     *   User has to pass pointer to FVID_SliceFrame structure
     *   variable as a third argument to ctrl() function.
     *   Generally this IOCTL will be used by VPIF layer to get VBI data and
     *   put it inside the vpif Frame packet
     *   For example
     *   FVID_SliceFrame vbiData;
     *   Uint8 data[48];
     *   vbiData.fvidSliceServiceId = Fvid_SLICE_VBI_SERVICES_CC_NTSC;
     *   vbiData.fvidField = 0;
     *   vbiData.fvidData = data;
     *   cmdArg = (&vbiData);                                                 */
    Edc_IOCTL_CMD_MAX
    /**< Book-keep - Max ioctl's                                              */
}Edc_IOCTL;


/**
 *  \brief  Enum for type of video parameters
 */
typedef enum Edc_VideoType_t
{
    Edc_VideoType_SD = 0,
    /**< Indicates SD parameters                                              */
    Edc_VideoType_ED,
    /**< Indicates ED parameters - Not supported                              */
    Edc_VideoType_HD
    /**< Indicates HD parameters - Not supported                              */
}Edc_VideoType;

/**
 *  \brief Control bus type to be used for configure the Encoder/Decoder.
 *
 *  This enum defines the Encoder/Decoder control bus type.
 */
typedef enum Edc_ControlBusType_t
{
    Edc_ControlBusType_I2C,
    /**< Control Bus for Encoder/Decoder is I2C                               */
    Edc_ControlBusType_SPI,
    /**< Control Bus for Encoder/Decoder is SPI - Not implemented             */
    Edc_ControlBusType_UNKNOWN
    /**< Delimiter Enum                                                       */
}Edc_ControlBusType;

/* ========================================================================== */
/*                             EDC STRUCTURES                                 */
/* ========================================================================== */
/**
 * \brief Codec callback function to propagate device status to above layer
 *
 *        This is the codec (I2C/SPI) callback function for the VPIF driver.
 *        This function is called directly called from ISR context. The first
 *        argument passed to this function is the user supplied
 *        callback argument, the second argument is the status.
 * \note  Since this function is called from an ISR context,care should be taken
 *        that this function conforms to ISR coding guidelines.
 */
typedef Void (*Edc_Callback) (Ptr handle, Int32 *status);

/**
 *  \brief EDC Function pointer and argument structure
 *  This structure stores the callback function pointer and the data
 */
typedef struct Edc_Data_t
{
    Edc_Callback                        cbFxn;
    /**< Callback function                                                    */
    Ptr                                 cbData;
    /**< Callback data to be passed to the callback function                  */
}Edc_Data;


/**
 *  \brief Structure used during read or write to the encoder/decoder registers.
 *
 *  This structure specifies the encoder/decoder register write or read
 *  information. The startReg element specifies the starting index
 *  of the encoder/decoder register. The value element specifies the register
 *  data to be read/written. The noRegToRW element specifies the
 *  total number of registers to be read/written.
 */
typedef struct Edc_RegData_t
{
    Uint32                              startReg;
    /**< Starting register address                                            */
    Uint32                              noRegToRW;
    /**< Number of registers written/read. CAUTION: noRegToRW should be number
    of CONSECUTIVE registers to be read or written                            */
    Ptr                                 value;
    /**< Ptr from where values to read or written                             */
}Edc_RegData;

/**
 *  \brief EDC Function table structure
 *  This structure contains function pointers of external devices
 */
typedef struct EDC_Fxns_t
{
    EDC_Handle (*open)(String name, Ptr optArg);
    /**< edcOpen() - required, open the device                                */
    Int32 (*close)(Ptr devHandle);
    /**< edcClose() - required, close the device                              */
    Int32 (*ctrl)(Ptr devHandle, Uns cmd, Ptr arg);
    /**< edcCtrl() - required, control/query device                           */
}EDC_Fxns;


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _EDC_H_ */

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
