/*
 * AudioLocal.h
 *
 * Audio Driver internal header file
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
 *  ======== ti/psp/platforms/evmOMAPL137/audio/Audio.h ========
 */
#ifndef _AUDIOLOCAL_H_
#define _AUDIOLOCAL_H_

#include <std.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------- typedef enums -------- */

/**
 *   \brief Audio driver state
 *
 *  Audio driver state enums used to track the driver state.
 */
typedef enum Audio_DriverState {
    Audio_DriverState_DELETED,
    Audio_DriverState_CREATED,
    Audio_DriverState_INITIALIZED,
    Audio_DriverState_OPENED,
    Audio_DriverState_CLOSED,
    Audio_DriverState_DEINITIALIZED,
    Audio_DriverState_POWERED_DOWN
}Audio_DriverState;

/* -------- typedef structs -------- */

/**
 *  \brief Audio inteface channel object structure
 */
typedef struct Audio_Channel_Object
{
    Audio_DriverState       chanState;
    /**< status of the channel (open /closed)                                 */

    Ptr                     aiAudioChanHandle;
    /**< The handle to the audio device (RX/TX mode)                          */

    Uint32                  aiAudioCodecHandle[Audio_NUMCODECINSTANCES];
    /**< The handles to all the audio codecs                                  */

    IOM_TiomCallback        cbFxn;
    /**< call back function                                                   */

    Arg                     cbArg;
    /**< Callback Function argument                                           */

    Audio_ChannelConfig     aiChannelConfig;
    /**< The audio interface channel configuration params                     */

    Ptr                     devHandle;
    /**< instance handle                                                      */

}Audio_Channel_Object;

/* -------- module-wide state -------- */

/**
 *  \brief Audio module object structure
 */
typedef struct {
    Bool *inUse;
} Audio_Module_State;

/* -------- per-instance state -------- */

/**
 *  \brief Audio Interface instance local data structure.
 *
 *  This structure contains the audio interface driver local data. it
 *  stores all the information related to the initialisation parameters.
 *  it also stores the channel parameters required during the opening of
 *  all the audio devices and the audio codecs. it stores within it the
 *  various handles required for communication with the audio devices
 *  and codecs(which are returned during the open call of respective
 *  drivers).
 */
typedef struct
{
    Uint8                   instNum;
    /**< instance number of the audio interface                               */

    Audio_DriverState       devState;
    /**<  The current status of the instance (created/deleted)                */

    Audio_DeviceType        adDevType;
    /**< Identifies the type of audio device (McASP/McBSP)                    */

    String                  adDevName;
    /**< Name of the audio device used during the creation and  addition of
    the driver to the driver table.                                           */

    DEV_Device             *adDevHandle;
    /**< Handle to the Audio device IDriver                                   */

    Mcasp_AudioDevData      adAudioDevData;
    /**< Information related to the audio device                              */

    Uint8                   acNumCodecs;
    /**< Number of audio codec instance                                       */

    String                  acDevName[Audio_NUMCODECINSTANCES];
    /**< handle to the audio codec returned during bind function              */

    DEV_Device             *acDevHandle[Audio_NUMCODECINSTANCES];
    /**< Handle to the Audio codec IDriver                                    */

    ICodec_CodecData        acCodecData[Audio_NUMCODECINSTANCES];
    /**< Information related to the codec                                     */

    Audio_Channel_Object    ChanObj[Audio_NUM_CHANS];
    /**<  AI channel configuration information object                         */

} Audio_Object;

#ifdef __cplusplus
}
#endif

#endif  /*_AUDIOLOCAL_H_ */

