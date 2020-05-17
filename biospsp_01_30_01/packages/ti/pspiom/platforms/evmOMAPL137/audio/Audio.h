/*
 * Audio.h
 *
 * This file contains Application programming interface for the Audio interface 
 * driver and command/macro definitions used by the Audio Interface driver.
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
 *  \mainpage    Audio Driver on BIOS 5.x
 *
 *  \section    Introduction
 *
 *  \subsection    xxx    Purpose and Scope
 *  The purpose of this document is to explain the Audio driver written for
 *  DSP/BIOS 5.x
 *
 *
 *  \subsection    bbb    Audio driver
 *
 *  Driver to stream data to/from a Application and audio transport streams.
 *
 *  The audio transport devices like mcasp,mcbsp are connected to the codecs
 *  which allow audio data to be received and transmitted.
 *
 *  The audio devices like Mcasp,Mcbsp have individual device drivers to control
 *  the data. Also the application needs to configure the proper audio codecs
 *  connected to the appropriate audio device.
 *
 *  The configuration of audio codecs is usually achieved through the use of
 *  a control bus like I2c or SPI.
 *
 *  Hence the application needs to configure the audio device driver,audio 
 *  codec driver andd a control bus.
 *
 *  Instead an audio interface driver acts as a class driver which allows all 
 *  the above mentioned devices to be controlled through a single interface.
 *  The audio driver is compliant with the IDriver interface specified by the
 *  DSP BIOS operating system.
 *  
 *  The audio driver allows all the audio device and codecs to be configured 
 *  by using a single interface thereby simplifying the application 
 *  significantly.
 *
 *  The audio interface driver is  completly compliant with theIOM
 *  interface.
 *
 *  Note : since the audio codec does not handle any data transactions. The
 *  submit call of the audio interface will only be routed directly to the
 *  underlying audio device
 *
 *  \subsection    ccc    DSP/BIOS System
 *  DSP/BIOS is a real time kernel provided by TI to use with its
 *  DSP processors.
 *
 *  \subsection    ddd   Compiler defines to add/remove features.
 *
 *  Define to turn OFF parameter checking. 
 *  Use -DPSP_DISABLE_INPUT_PARAMETER_CHECK when building library
 *
 *  Define to turn OFF asserts. Use -DNDEBUG when building library.
 *
 *  \subsection    References
 *    -# DSP/BIOS driver development <b>SPRU616.pdf</b>
 *
 *  \file   ti/pspiom/platforms/evmOMAPL137/audio/Audio.h
 *
 *  \brief  Audio interface definition
 *
 *  This file contains the interfaces, data types and symbolic definitions
 *  that are needed by the application to utilize the serivces of the Audio
 *  device driver.
 */
#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <std.h>
#include <iom.h>
#include <ti/pspiom/platforms/codec/ICodec.h>

#ifdef __cplusplus
extern "C" {
#endif


/* -------- constants -------- */
/**
 *  \brief Total number of Audio_NUMINSTANCES
 */
#define Audio_NUMINSTANCES      (1u)
/**
 *  \brief Total number of Audio_NUMCODECINSTANCES
 */
#define Audio_NUMCODECINSTANCES (5u)
/**
 *  \brief  Audio_NUM_CHANS
 *
 *  Number of channels per Audio instance
 */
#define Audio_NUM_CHANS         (0x02u)

/* -------- typedef enums -------- */

/**
 *  \brief Audio Interface Device Types.
 *
 *  This enum defines the audio device types supported in the Audio
 *  Framework.
 */
typedef enum Audio_DeviceType {
    Audio_DeviceType_McASP,         /**< Audio Device is McASP                */
    Audio_DeviceType_McBSP,         /**< Audio Device is McBSP                */
    Audio_DeviceType_VOICE_CODEC,   /**< Audio device is voice codec          */
    Audio_DeviceType_UNKNOWN        /**< Audio device is unknown              */
}Audio_DeviceType;

/**
 *  \brief Audio Interface channel mode.
 *
 *  This enum defines the audio interface channel mode.
 */
typedef enum Audio_IoMode {
    Audio_IoMode_RX,
    /**< Audio Interface channel mode is Rx                                   */
    Audio_IoMode_TX,
    /**< Audio Interface channel mode is Tx                                   */
    Audio_IoMode_MAX
    /**< Max channel count                                                    */
}Audio_IoMode;

/**
 *  \brief Audio Interface module selection.
 *
 *  This enum defines the audio interface module selection (used in IOCTL).
 */
typedef enum Audio_ModuleSel
{
    Audio_ModuleSel_AUDIO_DEVICE,
    /**< Audio Interface module is Audio Device                               */

    Audio_ModuleSel_AUDIO_CODEC
    /**< Audio Interface module is Audio Codec                                */
}Audio_ModuleSel;

/**
 *  \brief Audio Interface Ioctl commands
 *
 *  This enum defines the audio interface module supported IOCTLS.
 */

typedef enum Audio_Ioctl
{
    Audio_IOCTL_SAMPLE_RATE
    /**< Audio interface ioctl to change the sample rate                      */
}Audio_Ioctl;

/* -------- typedef structs -------- */

/**
 *  \brief Audio Interface channel Parameters.
 *
 *  This structure defines the audio interface channel parameters.
 *  This parameter is supplied with the open call of the Audio Interface.
 */
typedef struct Audio_ChannelConfig
{
    Ptr                     chanParam;
    /**< Pointer to Rx/Tx channel parameter structure                         */

    ICodec_ChannelConfig    acChannelConfig[0x05u];
    /**< Audio codec channel configuration data structures                    */
}Audio_ChannelConfig;

/**
 *  \brief Audio Interface IOCTL Parameters.
 *
 *  This structure defines the audio interface IOCTL parameters.
 *  This parameter is supplied with the IOCTL call of the Audio Interface.
 */
typedef struct Audio_IoctlParam {
    Audio_ModuleSel aiModule;
    /**< Device type specification(Audio device or codec)                     */
    Uint32          codecId;
    /**< I2C slave address or SPI CS number.Used only when
       command is for Audio Codec                                             */
    Ptr             ioctlArg;
    /**< Command Specific data                                                */
}Audio_IoctlParam;

/* -------- per-instance configs -------- */

typedef struct Audio_Params {
    Audio_DeviceType    adDevType;
    /**< Audio device Type                                                    */
    String              adDevName;
    /**< Name of sudioDevice in DEV table                                     */
    Uint8               acNumCodecs;
    /**< Number of codecs present                                             */
    String              acDevName[Audio_NUMCODECINSTANCES];
    /**< Name od codec in DEV table                                           */
}Audio_Params;

/**
 *  \brief Audio IOM_Fxns table
 *
 *  Driver function table to be used by applications.
 */
extern IOM_Fxns Audio_IOMFXNS;

/**
 *  \brief Default Audio_Params struct
 *
 *  const Audio_Params Audio_PARAMS__C = {
 *
 *      (Uint8)0x0,                    instNum
 *
 *       Audio_DeviceType_McASP,       adDevType
 *
 *       "NONE",                       adDevName
 *
 *       (Uint8)0x1,                   acNumCodecs
 *
 *       "",                           acDevName
 *
 *  };
 */
extern const Audio_Params Audio_PARAMS;

/**
 *  \brief    Initializes Audio instances which are statically allocated
 *
 *  This function needs to ve be called at part of BIOS initialization by
 *  setting initFxn for that particular UDEV instance or by calling this
 *  function as part of user specific initFxn.
 */
Void Audio_init(Void);

#ifdef __cplusplus
}
#endif

#endif /*_AUDIO_H_ */
