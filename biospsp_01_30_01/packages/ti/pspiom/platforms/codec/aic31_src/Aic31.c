/*
 * Aic31.c
 *
 * This file contains Driver Layer Interface implementation for Aic3106 codec 
 * Driver. Aic3106 Codec Driver provides Driver Layer Interface to do operations
 * on the Aic3106 peripheral like device initialization, channel creation, 
 * control commands for peripheral specific operations etc
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

/** \file      Aic31.c
 *
 *  \brief     Implementation of the IOM for the AIC31 audio codec.
 *
 *  This file contains the implementation of the AIC31 audio codec driver for
 *  DSP BIOS operating system.
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 */

/* ========================================================================== */
/*                            INCLUDE FILES                                   */
/* ========================================================================== */

#include <string.h>
#include <assert.h>
#include <std.h>
#include <iom.h>
#include <dev.h>
#include <sem.h>
#include <ti/pspiom/i2c/I2c.h>

#include <ti/pspiom/platforms/codec/Aic31.h>
#include <ti/pspiom/platforms/codec/ICodec.h>
#include "Aic31Local.h"

/* -------- constants -------- */
const Aic31_Params Aic31_PARAMS = {
    ICodec_CodecType_AIC31,         /* acType                 */
    ICodec_ControlBusType_I2C,      /* acControlBusType       */
    "NONE",                         /* acCtrlBusName          */
    ICodec_OpMode_MASTER,           /* acOpMode               */
    ICodec_DataType_DSP,            /* acSerialDataType       */
    (Uint32)ICodec_SlotWidth_16,    /* acSlotWidth            */
    ICodec_DataPath_TXRX,           /* acDataPath             */
    0,                              /* isRxTxClockIndependent */
};

/* ========================================================================== */
/*                       GLOBAL MODULE STATE                                  */
/* ========================================================================== */
/**
 *  \brief  Array which is part of Aic31 Module State
 */
static Bool inUse[AIC31_NUM_INSTANCES];
/**
 *  \brief  Aic31 Module State Object
 */
static Aic31_Module_State Aic31_module = {&inUse[0]};
/**
 *  \brief  Array of Aic31 instance State objects array
 */
static Aic31_Object Aic31_Instances[AIC31_NUM_INSTANCES];
/**
 *  \brief  Array for  per instance device information
 */
Aic31_HwInfo Aic31_deviceInstInfo[AIC31_NUM_INSTANCES];

/* ========================================================================== */
/*                       Aic31 Local Functions Definitions                    */
/* ========================================================================== */
/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Int aic31MdBindDev(Ptr *devp, Int devId, Ptr devParams);
static Int aic31MdUnBindDev(Ptr devp);
static Int aic31MdCreateChan(
                    Ptr                 *chanp,
                    Ptr                 devp,
                    String              name,
                    Int                 mode,
                    Ptr                 chanParams,
                    IOM_TiomCallback    cbFxn,
                    Ptr                 cbArg
                    );
static Int aic31MdDeleteChan(Ptr chanp);
static Int aic31MdSubmitChan(Ptr chanp, IOM_Packet *ioPacket);
static Int aic31MdControlChan(Ptr chanp, Uns cmd, Ptr cmdArgs);


static Bool aic31I2cWrite(Aic31_Object   *instHandle,
                          Uint32          deviceId,
                          ICodec_RegData *wrRegData);

static Bool aic31RegWrite(Aic31_Object *instHandle,
                          Uint8         regAddr,
                          Uint8         regData);

static Bool aic31I2cRead(Aic31_Object   *instHandle,
                         Uint32          deviceId,
                         ICodec_RegData  *rdRegData);

static Bool aic31RegRead(Aic31_Object *instHandle,
                         Uint8         regAddr,
                         Uint8        *regData);

static Bool aic31CtrlBusWrite(Aic31_Object  *instHandle,
                              ICodec_RegData *wrRegData);

static Bool aic31CtrlBusRead(Aic31_Object    *instHandle,
                             ICodec_RegData  *rdRegData);

static Int32 aic31MuteConfig(Aic31_Object  *instHandle,
                             ICodec_Channel  acChannel,
                             Bool           muteConfig);

static Int32 aic31SetSamplingRate(Aic31_Object *instHandle,
                                  ICodec_Channel acChannel,
                                  Uint32        sampleRate);

static Int32 aic31InitAdc  (Aic31_Object   *instHandle);

static Int32 aic31InitDac  (Aic31_Object   *instHandle);

static Int32 aic31DeinitAdc(Aic31_Object   *instHandle);

static Int32 aic31DeinitDac(Aic31_Object   *instHandle);

static Bool aic31RegWriteBf(Aic31_Object *instHandle,
                            Uint8         regAddr,
                            Uint8         bfPosition,
                            Uint8         bfSize,
                            Uint8         bfData);                             

static Void aic31CallBack (Ptr semHandle, IOM_Packet * ioPacket);

static Int32 aic31SelectInputSource(Aic31_Object    *instHandle,
                                    Ptr              srcOption);

static Int32 aic31SelectOutputDest(Aic31_Object    *instHandle,
                                   Ptr              destOption);


/**
 * \brief IOM function pointer table.
 *        
 *        This variables is the array of function pointers of all the IOM 
 *        functions implemented by the codec driver.
 * 
 */

IOM_Fxns Aic31_IOMFXNS =
{
    &aic31MdBindDev,
    &aic31MdUnBindDev,
    &aic31MdControlChan,
    &aic31MdCreateChan,
    &aic31MdDeleteChan,
    &aic31MdSubmitChan,
};



/* ========================================================================== */
/*                          FUNCTION DEFINTIONS                               */
/* ========================================================================== */
/**
 *  \brief  Function called by Bios during instance initialisation
 *
 */
Void Aic31_init(Void)
{
    Int i;

    for (i = 0; i < AIC31_NUM_INSTANCES; i++)
    {
        /* have to initialize statically */
        Aic31_module.inUse[i] = FALSE;
        memset((Void *)&Aic31_Instances[i], 0x0, sizeof(Aic31_Object));
    }

    Aic31_deviceInstInfo[0].deviceAddress = (Uint32)AIC31_INST0_ADDRESS;
#if defined(CHIP_C6747) || defined (CHIP_OMAPL137)
    Aic31_deviceInstInfo[1].deviceAddress = (Uint32)AIC31_INST1_ADDRESS;
    Aic31_deviceInstInfo[2].deviceAddress = (Uint32)AIC31_INST2_ADDRESS;
#endif
}

/**
 *  \brief  Function called by Bios during instance initialisation
 *
 *  \param  devp      [OUT]  Pointer to the codec driver object
 *  \param  devId     [IN]   instance number of the codec
 *  \param  devParams [IN]   Device parameters passed by the user
 *
 *  \return IOM_COMPLETED    if success
 *          Error ID                 in case of error
 */
static Int aic31MdBindDev(Ptr *devp, Int devId, Ptr devParams)
{
    Int32         retVal        = IOM_COMPLETED;
    Aic31_Params *params        = NULL;
    Uint32        count         = 0;
    Aic31_Object *instHandle    = NULL;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == devp)                  ||
        (AIC31_NUM_INSTANCES <= devId)  ||
        (TRUE == Aic31_module.inUse[devId]))
    {
        retVal = IOM_EBADARGS;
    }
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == retVal)
    {
        instHandle =  &Aic31_Instances[devId];

        if (devParams == NULL) 
        {
            params = (Aic31_Params*)&Aic31_PARAMS;
        }
        else 
        {
            params = (Aic31_Params*) devParams;
        }

        Aic31_module.inUse[devId] = TRUE;

        /* copy the initialization parameters                                 */
        instHandle->acType           = params->acType;
        instHandle->instNum          = (Uint8)devId;
        instHandle->acControlBusType = params->acControlBusType;
        instHandle->acCtrlBusName    = params->acCtrlBusName;
        instHandle->acOpMode         = params->acOpMode;
        instHandle->acSerialDataType = params->acSerialDataType;
        instHandle->acSlotWidth      = params->acSlotWidth;
        instHandle->acDataPath       = params->acDataPath;
        instHandle->isRxTxClockIndependent = params->isRxTxClockIndependent;
        instHandle->acCodecId = Aic31_deviceInstInfo[devId].deviceAddress;

        for (count = 0; count < ICodec_Channel_MAX; count++)
        {
            /* Set the state of both the channels as closed                   */
            instHandle->ChanObj[count].chanStatus = ICodec_DriverState_CLOSED;
            instHandle->ChanObj[count].channelMode = ICodec_Channel_MAX;
            instHandle->ChanObj[count].samplingRate = 0;
            instHandle->ChanObj[count].chanGain  = 0;
            instHandle->ChanObj[count].devHandle = NULL;
        }

        instHandle->hCtrlBus = NULL;
        instHandle->acCtrlBusHandle = NULL;

        /* set the state of the driver as created                             */
        instHandle->devState = ICodec_DriverState_CREATED;
        *devp = instHandle;
    }
    else
    {
        retVal = IOM_EBADIO;

        if (NULL != devp)
        {
            *devp = NULL;
        }
    }

    return (retVal);
}

/**
 *  \brief  Function called by Bios during closing of the instance
 *
 *  \param  devp [IN] Pointer to the driver instance object.
 *
 *  \return None
 */

static Int aic31MdUnBindDev(Ptr devp)
{
    Aic31_Object       *instHandle  = NULL;
    Int32               retVal      = IOM_COMPLETED;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == devp)
    {
        retVal = IOM_EBADARGS;
    }
    else
    {
#endif
        instHandle = (Aic31_Object *)devp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((NULL == instHandle)                                             ||
            (AIC31_NUM_INSTANCES <= instHandle->instNum)                     ||
           (ICodec_DriverState_CREATED != instHandle->devState)              ||
            (instHandle->ChanObj[0].chanStatus != ICodec_DriverState_CLOSED) ||
            (instHandle->ChanObj[1].chanStatus != ICodec_DriverState_CLOSED))
        {
            retVal = IOM_EBADARGS;
        }
    }
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == retVal)
    {
        instHandle->devState = ICodec_DriverState_DELETED;
        Aic31_module.inUse[instHandle->instNum] = FALSE;
    }

    return (retVal);
}

/* ========================================================================== */
/*                           IOM FUNCTIONS                                    */
/* ========================================================================== */

/**
 * \brief     This function configures the RX or TX portion of the Aic31 codec
 *
 *  This function internally uses a control bus to configure the codec. The
 *  control bus could be either SPI or I2c etc.The open function configures the
 *  DAC and ADC sections of the codec accordingly
 *
 * \param     chanp        [OUT]    pointer to Aic31 driver channel handle
 * \param     devp         [IN]     pointer to Aic31 driver object
 * \param     name         [IN]     Aic31 Instance name
 * \param     mode         [IN]     channel  mode -> input or output
 * \param     chanParams   [IN]     channel parameters from user
 * \param     cbFxn        [IN]     callback function pointer
 * \param     cbArg        [IN]     callback function Arguments
 *
 * \return    Channel handle if successful
 *            NULL in case of error
 */
static Int aic31MdCreateChan(Ptr             *chanp,
                             Ptr              devp,
                             String           name,
                             Int              mode,
                             Ptr              chanParams,
                             IOM_TiomCallback cbFxn,
                             Ptr              cbArg)
{
    Aic31_Object           *instHandle   = NULL;
    ICodec_ChannelConfig   *acChanConfig = NULL;
    Aic31_Channel_Object   *chanHandle   = NULL;
    Int32                   result       = IOM_COMPLETED;
    Uint8                   tempData     = 0;
    String                  remName      = NULL;
    I2c_ChanParams          i2cChanPrms  = {0};
    Bool                    retVal       = Aic31_REG_WRITE_FAIL;


/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == devp)                                          ||
        (NULL == chanParams)                                    ||
        ((IOM_INPUT != mode) && (IOM_OUTPUT != mode)))
    {
        result = IOM_EBADARGS;
    }
    else
    {
#endif
        instHandle = (Aic31_Object *)devp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((NULL == instHandle) ||
            (ICodec_DriverState_CREATED != instHandle->devState)    ||
            (ICodec_DriverState_CLOSED != instHandle->ChanObj[mode-1].chanStatus))
        {
            result = IOM_EBADARGS;
        }
    }
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    /* mode is mode-1 because iom.h defines input=1; output=2                 */
    mode = mode -1;

    /* validate the input parameters and return in case of error              */
    if (IOM_COMPLETED == result)
    {
        acChanConfig = (ICodec_ChannelConfig *)chanParams;

        /* To remove the compiler warning                                     */
        if ((NULL != cbFxn) && (NULL != cbArg))
        {
            cbFxn = cbFxn;
            cbArg = cbArg;             
        }

        /* copy the initial parameters to the channel                         */
        instHandle->ChanObj[mode].samplingRate = acChanConfig->samplingRate;
        instHandle->ChanObj[mode].chanGain = acChanConfig->chanGain;

        /* If either of the Tx or Rx channel is already open, then in this*
         * function call, there is no need to do any configuration.           */
        if ((instHandle->ChanObj[0].chanStatus == ICodec_DriverState_CLOSED)
          &&(instHandle->ChanObj[1].chanStatus == ICodec_DriverState_CLOSED))
        {

            SEM_new(&(instHandle->semObj), 0);

            /* These variables are not used as no callback is required by     *
             *  the codec since it does not support the submit call.hence     *
             *  to remove the warning by compiler of unused variables we      *
             *  do a dummy assign                                             */
            if (NULL != name)
            {
                name  = name;
            }

            /* we have to open the control bus to the codec so that we can*
             * use the handle and configure the codec                         */
            remName = DEV_match(
                         instHandle->acCtrlBusName,
                         &instHandle->acCtrlBusHandle);

            if ((NULL == instHandle->acCtrlBusHandle) || 
                (NULL == remName))
            {
                result = IOM_EBADARGS;
            }
            else
            {
                if (ICodec_ControlBusType_I2C == instHandle->acControlBusType)
                {
                    i2cChanPrms.hEdma = NULL;
                    i2cChanPrms.masterOrSlave = I2c_CommMode_MASTER;

                    /* driver handle obtained from the name of the device     */
                    result = ((IOM_Fxns*)instHandle->acCtrlBusHandle->fxns)-> \
                             mdCreateChan(
                                &instHandle->hCtrlBus,
                                instHandle->acCtrlBusHandle->devp,
                                remName,
                                IOM_INOUT,
                                &i2cChanPrms,
                                &aic31CallBack,
                                (Ptr)(&instHandle->semObj));
                }
                else
                {
                    result = IOM_EBADMODE;
                }

                if (NULL == instHandle->hCtrlBus)
                {
                    result = IOM_EBADARGS;
                }

            }

            /* if the control bus was opened sucesfully then we will          *
             * start the configuration of the codec                           */
            if (IOM_COMPLETED == result)
            {
                /* Select Register Page 0 of Aic31                            */
                retVal = aic31RegWrite(
                             instHandle,
                             Aic31_PAGE_SELECT_ADDR,
                             Aic31_PAGE_0);

                /* Reset the Aic31                                            */
                retVal |= aic31RegWrite(
                             instHandle,
                             Aic31_P0_REG1,
                             (Uint8)0x80);
                
                /* If Aic31 is master mode then set the direction of          *
                 * the bit clock and word clock as output                     */
                if (ICodec_OpMode_MASTER == instHandle->acOpMode)
                {
                    retVal |= aic31RegWriteBf(
                                  instHandle,
                                  Aic31_SERIAL_INTERFACEA_ADDR,
                                  (Uint8)6,
                                  (Uint8)2,
                                  (Uint8)3);
                }

                /* Configure the audio serial data interface mode             */
                switch (instHandle->acSerialDataType)
                {
                    case ICodec_DataType_I2S:    tempData = (0u << 6u); break;
                    case ICodec_DataType_DSP:    tempData = (1u << 6u); break;
                    case ICodec_DataType_RIGHTJ: tempData = (2u << 6u); break;
                    case ICodec_DataType_LEFTJ:  tempData = (3u << 6u); break;
                    default:                     tempData = (0u << 6u); break;
                }

                /* configure the slot width                                   */
                switch (instHandle->acSlotWidth)
                {
                    case ICodec_SlotWidth_16:  tempData |= (0u << 4u); break;
                    case ICodec_SlotWidth_20:  tempData |= (1u << 4u); break;
                    case ICodec_SlotWidth_24:  tempData |= (2u << 4u); break;
                    case ICodec_SlotWidth_32:  tempData |= (3u << 4u); break;
                    default:  tempData |= (0x00 << 4); break;
                }

                retVal |= aic31RegWrite(
                              instHandle,
                              Aic31_SERIAL_INTERFACEB_ADDR,
                              tempData);

                if (Aic31_REG_WRITE_FAIL == retVal)
                {
                    result = IOM_EBADARGS;
                }
            }
        }

        if (IOM_COMPLETED == result)
        {
            /* Configure the sampling rate of the audio codec                 */
            result = aic31SetSamplingRate(
                         instHandle,
                         (ICodec_Channel)(mode),
                         acChanConfig->samplingRate);

            /* Complete the default initialization for Tx and RX channels     */
            if (IOM_COMPLETED == result)
            {
                if (ICodec_Channel_INPUT == mode)
                {
                    /* Do the default configuration for ADC section           */
                    result = aic31InitAdc(instHandle);
                }
                else
                {
                    /* Do the default initialization for DAC section          */
                    result = aic31InitDac(instHandle);
                }
            }
        }
    }

    /* Return the result of opening the audio codec                           */
    if (IOM_COMPLETED == result )
    {
        /* The channel creation is successful hence update the handle         */
        instHandle->ChanObj[mode].chanStatus   = ICodec_DriverState_OPENED;
        instHandle->ChanObj[mode].channelMode  = (ICodec_Channel)(mode);
        instHandle->ChanObj[mode].devHandle    = instHandle;

        chanHandle = &instHandle->ChanObj[mode];
        *chanp = chanHandle;
    }
    else
    {
        chanHandle = NULL;
        *chanp = NULL;
    }

    /* Return the channel handle                                              */
    return (result);
}


/**
 *  This function executes a control command specified by the application. The
 *  channel handle obtained after the channel open call should be passed as the
 *  parameter.
 *
 * \param     instHandle   [IN]     Pointer to the Aic31 driver instance object
 * \param     chanp        [IN]     Handle to the channel
 * \param     cmd          [IN]     command to be executed
 * \param     cmdArgs      [IN]     Arguments for the command(if required)
 * \param     eb           [OUT]    error block
 *
 * \return    None
 */
static Int aic31MdControlChan(Ptr chanp, Uns cmd, Ptr cmdArgs)
{
    Aic31_Object          *instHandle = NULL;
    Aic31_Channel_Object  *chanHandle = NULL;
    ICodec_RegData        *wRegPtr    = NULL;
    Int                    status     = IOM_COMPLETED;
    Uint32                 tempVal    = 0;
    ICodec_CodecData      *codecData  = NULL;
    Bool                   retVal     = Aic31_REG_WRITE_FAIL;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == chanp)
    {
        status = IOM_EBADARGS;
    }
    else 
    {
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
        chanHandle = (Aic31_Channel_Object*)chanp;
        instHandle = (Aic31_Object *)chanHandle->devHandle;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if (NULL == instHandle)
        {
            status = IOM_EBADARGS;
        }
    }
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == status)
    {
        /* Interpret the control command and execute the comnand              */
        if (Aic31_AC_IOCTL_MUTE_ON == cmd)
        {
            status = aic31MuteConfig(instHandle,chanHandle->channelMode,TRUE);
        }

        else if (Aic31_AC_IOCTL_MUTE_OFF == cmd)
        {
            status = aic31MuteConfig(instHandle,chanHandle->channelMode,FALSE);
        }

        else if (Aic31_AC_IOCTL_SET_VOLUME == cmd)
        {
            if (NULL != cmdArgs)
            {
                if (ICodec_Channel_OUTPUT == chanHandle->channelMode)
                {
                    /* calculate the actual values to be set depending on the *
                     * percentage of the values requested                     */
                    tempVal =
                        ((*(Uint32*)cmdArgs) * Aic31_MAX_DAC_GAIN_VALUE)/100u;

                    tempVal = Aic31_MAX_DAC_GAIN_VALUE - tempVal;

                    retVal  = aic31RegWriteBf(
                                 instHandle,
                                 Aic31_P0_REG43,
                                 (Uint8)0,
                                 (Uint8)7,
                                 (Uint8)tempVal);

                    retVal |= aic31RegWriteBf(
                                  instHandle,
                                  Aic31_P0_REG44,
                                  (Uint8)0,
                                  (Uint8)7,
                                  (Uint8)tempVal);
                }
                else if (ICodec_Channel_INPUT == chanHandle->channelMode)
                {
                    /* calculate the actual values to be set depending on the *
                     * percentage of the values requested                     */
                    tempVal =
                        ((*(Uint32*)cmdArgs) * Aic31_MAX_AGC_GAIN_VALUE)/100u;

                    /* modify the ADC volume                                  */
                    retVal  = aic31RegWriteBf(
                                instHandle,
                                Aic31_P0_REG27,
                                (Uint8)1,
                                (Uint8)7,
                                (Uint8)tempVal);

                    retVal |= aic31RegWriteBf(
                                instHandle,
                                Aic31_P0_REG30,
                                (Uint8)1,
                                (Uint8)7,
                               (Uint8)tempVal);
                }
                else
                {
                    status = IOM_EBADARGS;
                }

                if (Aic31_REG_WRITE_PASS != retVal)
                {
                    status = IOM_EBADARGS;
                }                
            }
            else
            {
                status = IOM_EBADARGS;
            }
        }
        else if (Aic31_AC_IOCTL_SET_LOOPBACK == cmd)
        {
            /* Loopback not supported in Aic31                                */
            status = IOM_ENOTIMPL;
        }

        else if (Aic31_AC_IOCTL_SET_SAMPLERATE == cmd)
        {
            /* set the sample rate for the codec                              */
            if (NULL != cmdArgs)
            {
                status = aic31SetSamplingRate(
                            instHandle,chanHandle->channelMode,
                            *(Uint32 *)cmdArgs);
            }
            else
            {
                status = IOM_EBADARGS;
            }
        }

        else if (Aic31_AC_IOCTL_REG_WRITE == cmd)
        {
            /* cmdArgs should be pointer to type ICodec_RegData               */
            if (NULL != cmdArgs)
            {
                /* Write a single Aic31 registe                               */
                wRegPtr = (ICodec_RegData*)cmdArgs;
                retVal = aic31RegWrite(
                            instHandle,
                            (Uint8)wRegPtr->regIndex,
                            *(Uint8*)(wRegPtr->regData));
            }
            else
            {
                status = IOM_EBADARGS;
            }

            if (Aic31_REG_WRITE_PASS != retVal)
            {
                status = IOM_EBADARGS;
            }            
        }

        else if (Aic31_AC_IOCTL_REG_WRITE_MULTIPLE == cmd)
        {
            /* Write multiple Aic31 register.                                 */
            /* cmdArgs should be pointer to type ICodec_RegData               */
            if (NULL != cmdArgs)
            {
                /* Write multiple Aic31 registers                             */
                retVal = aic31CtrlBusWrite(
                            instHandle->hCtrlBus,
                            (ICodec_RegData *)cmdArgs);
            }
            else
            {
                status = IOM_EBADARGS;
            }

            if (Aic31_REG_WRITE_PASS != retVal)
            {
                status = IOM_EBADARGS;
            }
        }

        else if (Aic31_AC_IOCTL_REG_READ == cmd)
        {
            /* cmdArgs should be pointer to type ICodec_RegData               */
            if (NULL != cmdArgs)
            {
                /* Write a single Aic31 registe                               */
                wRegPtr = (ICodec_RegData*)cmdArgs;
                retVal = aic31RegRead(
                            instHandle,
                            (Uint8)wRegPtr->regIndex,
                            (Uint8*)&(wRegPtr->regData));
            }
            else
            {
                status = IOM_EBADARGS;
            }

            if (Aic31_REG_WRITE_PASS != retVal)
            {
                status = IOM_EBADARGS;
            }            
        }
        else if (Aic31_AC_IOCTL_REG_READ_MULTIPLE == cmd)
        {
            /* Read multiple Aic31 register.                                  */
            /* cmdArgs should be pointer to type ICodec_RegData               */
            if (NULL != cmdArgs)
            {
                /* Read multiple Aic31 registers                              */
                retVal = aic31CtrlBusRead(
                            instHandle,
                            (ICodec_RegData *)cmdArgs);
            }
            else
            {
                status = IOM_EBADARGS;
            }
            if (Aic31_REG_WRITE_PASS != retVal)
            {
                status = IOM_EBADARGS;
            }            
        }
        else if (Aic31_AC_IOCTL_SELECT_OUTPUT_SOURCE == cmd)
        {
            aic31SelectOutputDest(instHandle,cmdArgs);
        }
        else if (Aic31_AC_IOCTL_SELECT_INPUT_SOURCE == cmd)
        {
            aic31SelectInputSource(instHandle,cmdArgs);
        }
        else if (Aic31_AC_IOCTL_GET_CODEC_INFO == cmd)
        {
            /* command is the request for the codec information               */
            if (NULL != cmdArgs)
            {
                codecData = (ICodec_CodecData *)cmdArgs;

                /* update the required information to the provided pointer    */
                codecData->acOpMode = instHandle->acOpMode;
            }
            else
            {
                status = IOM_EBADARGS;
            }
        }
        else
        {
            /* The IOCTL is unrecognised or not implemented                   */
            status = IOM_ENOTIMPL;
        }
    }

    /* return the status of the IOCTL executed                                */
    return (status);
}


/**
 *  \brief This function is not implemented as codec does not perform any read
 *  or write operations
 *
 * \param     instHandle   [IN]     Aic31 driver object
 * \param     chanp        [IN]     handle to the channel
 * \param     packet       [IN]     driver packet
 * \param     eb           [OUT]    error block
 *
 * \return    IOM_ERROR  - error as not implemeted.
 */
static Int aic31MdSubmitChan(Ptr chanp, IOM_Packet *ioPacket)
{
    /* to remove compiler warning                                             */
    if (NULL != chanp)
    {
        chanp = chanp;
    }

    if (NULL != ioPacket)
    {
        ioPacket = ioPacket;
    }
    
    /* not implemented   */
    return IOM_EBADIO;
}


/**
 * \brief    aic31MdDeleteChan
 *
 *  This function closes an specified Audio Codec Channel. The audio codec
 *  is moved into programming mode and then reset when both the channels
 *  are closed. It deinitalizes the control bus instance.
 *
 * \param     instHandle   [IN]     Aic31 driver object
 * \param     chanp        [IN]     handle to the channel
 * \param     eb           [OUT]    error block
 *
 * \return    None
 */
static Int aic31MdDeleteChan(Ptr chanp)
{
    Aic31_Object               *instHandle = NULL;
    Aic31_Channel_Object       *chanHandle = NULL;
    Int                        retVal      = IOM_COMPLETED;
    Bool                       status      = Aic31_REG_WRITE_FAIL;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == chanp)
    {
        retVal = IOM_EBADARGS;
    }
    else
    {
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
        chanHandle = (Aic31_Channel_Object*)chanp;
        instHandle = chanHandle->devHandle;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((NULL == instHandle) ||
            (ICodec_DriverState_CLOSED == chanHandle->chanStatus))
        {
            retVal = IOM_EBADARGS;
        }
    }
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == retVal)
    {
        /* Write the reset value of Aic31 channel registers                   */
        if (chanHandle->channelMode == ICodec_Channel_INPUT )
        {
            /* Reset the ADC registers                                        */
            retVal = aic31DeinitAdc(instHandle);
        }
        else
        {
            /* Reset the DAC registers */
            retVal = aic31DeinitDac(instHandle);
        }
    }


    if (IOM_COMPLETED == retVal)
    {
        /* Set the state of the channel as closed                             */
        chanHandle->chanStatus = ICodec_DriverState_CLOSED;

        /* Reset Aic31 only if both the channels are closed.                  */
        if ((ICodec_DriverState_CLOSED == instHandle->ChanObj[0].chanStatus)
            && (ICodec_DriverState_CLOSED == instHandle->ChanObj[1].chanStatus))
        {
            /* Select Register Page 0 of Aic31                                */
            status = aic31RegWrite(
                         instHandle,
                         Aic31_PAGE_SELECT_ADDR,
                         Aic31_PAGE_0);

            /* Reset the Aic31                                                */
            status |= aic31RegWrite(
                         instHandle,
                         Aic31_RESET_ADDR,
                         (Uint8)0x80);

            if (Aic31_REG_WRITE_FAIL == status)
            {
                retVal = IOM_EBADARGS;
            }

            ((IOM_Fxns*)instHandle->acCtrlBusHandle->fxns)->mdDeleteChan(
                instHandle->hCtrlBus);

            /* The control bus handle is not valid anymore                    */
            instHandle->hCtrlBus = NULL;
        }
    }

    return (retVal);
}

/* ========================================================================== */
/*               Local Interface for Aic31 audio codec driver                 */
/* ========================================================================== */


/**
 *  \brief  This function sets the sampling rate for the ADC and DAC. The ADC
 *          sampling rate is configured when called for input channel. The DAC
 *          sampling rate is configured when called for ouput channel.
 *
 *  \param  instHandle  [IN]  pointer to the aci31 instance handle
 *  \param  acChannel   [IN]  operational mode of the channel
 *  \param  sampleRate  [IN]  sample rate to be configured
 *  \param  eb          [OUT] Error block
 */
static Int32 aic31SetSamplingRate(Aic31_Object       *instHandle,
                                  ICodec_Channel      acChannel,
                                  Uint32              sampleRate)
{
    Uint8 pllPValue     = 0;
    Uint8 pllRValue     = 0;
    Uint8 pllJValue     = 0;
    Uint8 pllDLowValue  = 0;
    Uint8 pllDHighValue = 0;
    Uint8 dualRate      = 0x00;
    Uint8 sampleRateDiv = 0;
    Uint8 fsRef         = 0;
    Int32 status        = IOM_COMPLETED;
    Bool  retVal        = Aic31_REG_WRITE_FAIL;    
    
    assert(NULL != instHandle);

    if ((ICodec_Channel_INPUT == acChannel) ||
        (ICodec_Channel_OUTPUT == acChannel))
    {

        switch (sampleRate)
        {
            case 8000:
                /* Set sampling rate to 8000Hz */
                pllPValue      = 4u;
                pllRValue      = 1u;
                pllJValue      = 16u;
                pllDLowValue   = 0u;
                pllDHighValue  = 0u;
                dualRate       = 0u;
                fsRef          = 0u;
                sampleRateDiv  = 10u;
                break;

            case 11025:
                /* Set sampling rate to 11025Hz */
                pllPValue      = 2u;
                pllRValue      = 1u;
                pllJValue      = 7u;
                pllDLowValue   = 44u;
                pllDHighValue  = 54u;
                dualRate       = 0u;
                fsRef          = 1u;
                sampleRateDiv  = 6u;
                break;

            case 16000:
                /* Set sampling rate to 16000Hz */
                pllPValue      = 4u;
                pllRValue      = 1u;
                pllJValue      = 16u;
                pllDLowValue   = 0u;
                pllDHighValue  = 0u;
                dualRate       = 0u;
                fsRef          = 0u;
                sampleRateDiv  = 4u;
                break;

            case 22050:
                /* Set sampling rate to 22050Hz */
                pllPValue      = 2u;
                pllRValue      = 1u;
                pllJValue      = 7u;
                pllDLowValue   = 44u;
                pllDHighValue  = 54u;
                dualRate       = 0u;
                fsRef          = 1u;
                sampleRateDiv  = 2u;
                break;

            case 24000:
                /* Set sampling rate to 24000Hz */
                pllPValue      = 4u;
                pllRValue      = 1u;
                pllJValue      = 16u;
                pllDLowValue   = 0u;
                pllDHighValue  = 0u;
                dualRate       = 0u;
                fsRef          = 0u;
                sampleRateDiv  = 2u;
                break;

            case 32000:
                /* Set sampling rate to 32000Hz */
                pllPValue      = 4u;
                pllRValue      = 1u;
                pllJValue      = 16u;
                pllDLowValue   = 0u;
                pllDHighValue  = 0u;
                dualRate       = 0u;
                fsRef          = 0u;
                sampleRateDiv  = 1u;
                break;

            case 44100:
                /* Set sampling rate to 44100Hz */
                pllPValue      = 2u;
                pllRValue      = 1u;
                pllJValue      = 7u;
                pllDLowValue   = 44u;
                pllDHighValue  = 54u;
                dualRate       = 0u;
                fsRef          = 1u;
                sampleRateDiv  = 0u;
                break;

            case 48000:
                /* Set sampling rate to 48000Hz */
                pllPValue      = 4u;
                pllRValue      = 1u;
                pllJValue      = 16u;
                pllDLowValue   = 0u;
                pllDHighValue  = 0u;
                dualRate       = 0u;
                fsRef          = 0u;
                sampleRateDiv  = 0u;
                break;

            case 96000:
                /* Set sampling rate to 96000Hz */
                pllPValue      = 4u;
                pllRValue      = 1u;
                pllJValue      = 16u;
                pllDLowValue   = 0u;
                pllDHighValue  = 0u;
                dualRate       = 1u;
                fsRef          = 0u;
                sampleRateDiv  = 0u;
                break;
            default:
                /* Sampling rate is not supported */
                status = IOM_EBADARGS;
                break;
        }

        if (IOM_COMPLETED == status)
        {
            if (ICodec_Channel_INPUT == acChannel)
            {
                /* Write the ADC sample rate divisor value in Reg2 */
                retVal= aic31RegWriteBf(
                            instHandle,
                            Aic31_P0_REG2,
                            (Uint8)4,
                            (Uint8)4,
                            sampleRateDiv);

                retVal |= aic31RegWriteBf(
                             instHandle,
                             Aic31_P0_REG7,
                             (Uint8)6,
                             (Uint8)1,
                             dualRate);
            }
            else
            {
                /* Write the DAC sample rate divisor value in Reg2            */
                retVal = aic31RegWriteBf(
                            instHandle,Aic31_P0_REG2,
                            (Uint8)0,
                            (Uint8)4,
                            sampleRateDiv);

                retVal |= aic31RegWriteBf(
                              instHandle,
                              Aic31_P0_REG7,
                              (Uint8)5,
                              (Uint8)1,
                              dualRate);
            }

            /* enable the PLL                                             */
            retVal |= aic31RegWriteBf(
                          instHandle,
                          Aic31_P0_REG3,
                          (Uint8)7,
                          (Uint8)1,
                          (Uint8)1);

            /* select the MCLK as the input for the PLL and CLKDIV(N = 16)*/
            retVal |= aic31RegWriteBf(
                          instHandle,
                          Aic31_P0_REG102,
                          (Uint8)4,
                          (Uint8)2,
                          (Uint8)0x00);
            /* select the PLL_IN as codec input                           */
            retVal |= aic31RegWriteBf(
                          instHandle, 
                          Aic31_P0_REG101,
                          (Uint8)0, 
                          (Uint8)1,
                          (Uint8)0x00);


            /* selec the GPIO to output the divided PLL_IN clock(test purpose)*/
            retVal |= aic31RegWrite(instHandle,
                            Aic31_P0_REG98,
                            (Uint8)0x20);

            /* Write to PLL programming register A                            */
            retVal |= aic31RegWriteBf(
                            instHandle,
                            Aic31_P0_REG3,
                            (Uint8)0,
                            (Uint8)3,
                            pllPValue);

            /* Write to PLL programming register B                            */
            retVal |= aic31RegWriteBf(
                            instHandle,
                            Aic31_P0_REG4,
                            (Uint8)2,
                            (Uint8)6,
                            pllJValue);

            /* write the high and low bits of the D value                     */
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG5,pllDHighValue);
            retVal |= aic31RegWrite(
                           instHandle,
                           Aic31_P0_REG6,
                           (Uint8)(pllDLowValue << 2));

            retVal |= aic31RegWriteBf(
                            instHandle,
                            Aic31_P0_REG11,
                            (Uint8)0,
                            (Uint8)4,
                            pllRValue);

            /* Write to codec datapath setup register                         */
            retVal |= aic31RegWriteBf(
                            instHandle, 
                            Aic31_P0_REG7,
                            (Uint8)7,
                            (Uint8)1,
                            fsRef);
        }
    }


    if (Aic31_REG_WRITE_PASS == retVal)
    {
        status = IOM_COMPLETED;
    }
    else
    {
        status = IOM_EBADARGS;
    }

    /* Return the result of setting the sampling rate */
    return (status);
}

/**
 *  This function perfroms a register write(s) to the audio codec. The control
 *  bus is selected based on the parameter value selected.
 *
 *  \param  instHandle  [IN]  pointer to the aci31 instance handle
 *  \param  wrRegData   [OUT] data to be written to codec
 *
 *  \return  Aic31_REG_WRITE_PASS in case of success
 *           Aic31_REG_WRITE_FAIL in case of error
 */
static Bool aic31CtrlBusWrite(Aic31_Object    *instHandle,
                              ICodec_RegData  *wrRegData)
{
    ICodec_ControlBusType     acCtrlBusType;
    Uint32                    deviceId;
    Bool                      retVal = Aic31_REG_WRITE_FAIL;

    assert((NULL != instHandle) && (NULL != wrRegData));

    /* Initialize the local variables */
    acCtrlBusType = instHandle->acControlBusType;
    deviceId = instHandle->acCodecId;

    if (ICodec_ControlBusType_I2C == acCtrlBusType)
    {
        retVal = aic31I2cWrite(instHandle,deviceId,wrRegData);
    }
    else if (ICodec_ControlBusType_SPI == acCtrlBusType)
    {
        /* Add SPI call here */
        retVal = Aic31_REG_WRITE_FAIL;
    }
    else
    {
        retVal = Aic31_REG_WRITE_FAIL;
    }
    return (retVal);
}

/**
 *  \brief  This function perfroms a register write(s) to the audio codec using
 *          the I2C control bus.
 *
 *  \param  instHandle [IN]  pointer to the device instance object
 *  \param  deviceId   [IN]  slave device address
 *  \param  wrRegData  [IN]  data to write to slave
 *
 *  \return  Aic31_REG_WRITE_PASS in case of success
 *           Aic31_REG_WRITE_FAIL in case of error
 */
static Bool aic31I2cWrite(Aic31_Object    *instHandle,
                          Uint32           deviceId,
                          ICodec_RegData  *wrRegData)
{
    Uint8              count;      /* Local count variable                    */
    Uint32             regIndex;   /* Register indec to be written            */
    Uint8             *regData;    /* Pointer to array of register write data */
    Uint8              data[2];    /* Temporary data                          */
    IOM_Packet         ioPacket;
    I2c_DataParam      dataBuffer;
    Bool               retVal = Aic31_REG_WRITE_FAIL;
    Int32              result = IOM_EBADARGS; /* Result */

    assert((NULL != instHandle) && (NULL != wrRegData));

    /* Initialize the local variables */
    regIndex = wrRegData->regIndex;
    regData  = (Uint8*)wrRegData->regData;

    assert(NULL != regData);

    /* Write all the registers */
    for (count = 0; count < wrRegData->regCount; count++)
    {
        /* Prepare the data to be written to the audio codec */
        data[0] = regIndex;
        data[1] = ((*regData) & 0xFF);

        dataBuffer.slaveAddr = deviceId;
        dataBuffer.buffer    = &data[0];
        dataBuffer.bufLen    = 2u;
        dataBuffer.flags     = I2c_DEFAULT_WRITE;

        memset(&ioPacket,0x00,sizeof(IOM_Packet));

        ioPacket.addr   = &dataBuffer;
        ioPacket.size   = 2u;
        ioPacket.cmd    = (Uns)IOM_WRITE;
        ioPacket.status = IOM_COMPLETED;

        /* Write the data to Aic31 register */

        result = ((IOM_Fxns*)instHandle->acCtrlBusHandle->fxns)-> \
                    mdSubmitChan(
                        instHandle->hCtrlBus,
                        &ioPacket);

        if (IOM_PENDING == result)
        {
            SEM_pend (&(instHandle->semObj), Aic31_I2CTIMEOUT);
        }

        /* Validate the number of words written to Aic31 codec.               */
        if ((IOM_COMPLETED == ioPacket.status) ||
            (IOM_PENDING == ioPacket.status))
        {
            retVal = Aic31_REG_WRITE_PASS;
        }
        else
        {
            break;
        }
        /* Maintain the register data pointer and register index */
        regIndex++;
        regData++;
    }

    /* Return the result of writting to Aic31 registers */
    return (retVal);
}


/**
 *  \brief  This function writes a value to a single Aic31 register. This
 *          function is a wrapper for faclitating single register writes in the
 *          Aic31 codec driver. It internally calls aic31CtrlBusWrite function.
 *
 *  \param  instHandle [IN]  pointer to the device driver instance object
 *  \param  regAddr    [IN]  register address
 *  \param  regData    [IN]  register data to be written
 *
 * \return  FALSE  in case of success
 *          Error code in case of error
 */
static Bool aic31RegWrite(Aic31_Object  *instHandle,
                          Uint8          regAddr,
                          Uint8          regData)
{
    ICodec_RegData   acRegData;                    /* Register data structure */
    Bool             retVal = Aic31_REG_WRITE_FAIL;/* Result of write         */

    assert(NULL != instHandle);

    /* Write the data from the Aic31 register */
    acRegData.regCount = 1u;
    acRegData.regIndex = regAddr;
    acRegData.regData  = (Ptr)&regData;

    retVal = aic31CtrlBusWrite(instHandle,&acRegData);

    /* Return the result of reading the register */
    return (retVal);
}


/**
 * \brief   This function perfroms a register read(s) to the audio codec.
 *          The control bus is selected based on the parameter value selected.
 *
 * \param   instHandle [IN]  pointer to the device driver instance object
 * \param   rdRegData  [OUT] pointer to the register data read structure
 *
 *  \return  Aic31_REG_READ_PASS in case of success
 *           Aic31_REG_READ_FAIL in case of error
 */
static Bool aic31CtrlBusRead(Aic31_Object    *instHandle,
                             ICodec_RegData  *rdRegData)
{
    Bool                    retVal = Aic31_REG_READ_FAIL;
    ICodec_ControlBusType   acCtrlBusType;
    Uint32                  deviceId;

    assert(NULL != instHandle);

    /* Initialize the local variables */
    acCtrlBusType = instHandle->acControlBusType;
    deviceId = instHandle->acCodecId;

    /* Determine the control bus and invoke the corresponding read function   */
    if (ICodec_ControlBusType_I2C == acCtrlBusType)
    {
        /* Control bus is I2C. Read from I2c bus. */
        retVal = aic31I2cRead(instHandle,deviceId,rdRegData);
    }
    else if (ICodec_ControlBusType_SPI == acCtrlBusType)
    {
       /*  Add SPI call here */
       retVal = Aic31_REG_READ_FAIL;
    }
    else
    {
        retVal = Aic31_REG_READ_FAIL;
    }
    /* Return the result of reading register data */
    return (retVal);
}

/**
 *  \brief  This function perfroms a register reads(s) from the audio codec
 *          using the  I2C control bus. This function first writes the address
 *          of the register address without issuing a I2C stop. It then issues
 *          a I2C restart and reads the value of the register.
 *
 *  \param   instHandle  [IN]  pointer to the driver instance object
 *  \param   deviceId    [IN]  I2c slave device address
 *  \param   rdRegData   [OUT] register data read
 *
 *  \return  Aic31_REG_READ_PASS in case of success
 *           Aic31_REG_READ_FAIL in case of error
 */
static Bool aic31I2cRead(Aic31_Object         *instHandle,
                         Uint32                deviceId,
                         ICodec_RegData       *rdRegData)
{
    Uint8                count;    /* Local count variable                    */
    Uint32               regIndex; /* Register index to be written            */
    Uint8               *regData;  /* Pointer to array of register write data */
    Uint8                data[10]; /* Temporary data                          */
    I2c_DataParam        dataBuffer;
    IOM_Packet           ioPacket;
    Int32                status = IOM_COMPLETED;
    Bool                 retVal = Aic31_REG_READ_FAIL;

    assert(NULL != rdRegData);
    
    /* Initialize the local variables                                         */
    regIndex = rdRegData->regIndex;
    regData  = (Uint8*)rdRegData->regData;

    assert(NULL != regData);

    assert(NULL != instHandle);

    /* Read all the registers                                                 */
    for (count = 0; count < rdRegData->regCount; count++)
    {
        /* The following the procedure to read data from Aic31 codec

           Step A: Write the address of the register to be read.
                   Note: The I2C stop should not be issued.
           Step B: Issue I2C restart.
           Step C: Read the value of the register.                            */
        data[0] = regIndex;

        /* Prepare the data to be written to the audio codec                  */
        dataBuffer.slaveAddr = deviceId;
        dataBuffer.buffer    = &data[0];
        dataBuffer.bufLen    = 1u;
        dataBuffer.flags     = I2c_WRITE | I2c_MASTER | I2c_START;

        memset(&ioPacket,0x00,sizeof(IOM_Packet));

        ioPacket.addr   = &dataBuffer;
        ioPacket.size   = 2u;
        ioPacket.cmd    = (Uns)IOM_WRITE;
        ioPacket.status = IOM_COMPLETED;


        status = ((IOM_Fxns*)instHandle->acCtrlBusHandle->fxns)->mdSubmitChan(
                        instHandle->hCtrlBus,
                        &ioPacket);

        if (IOM_PENDING == status)
        {
            SEM_pend (&(instHandle->semObj), Aic31_I2CTIMEOUT);
        }

        /* If the write command is complete, read the registers of Aic31      */
        if ((IOM_COMPLETED == ioPacket.status )||
            (IOM_PENDING == ioPacket.status))
        {
            ioPacket.cmd = IOM_READ;

            dataBuffer.flags = I2c_READ
                              | I2c_MASTER
                              | I2c_STOP
                              | I2c_RESTART;

            status = ((IOM_Fxns *)instHandle->acCtrlBusHandle->fxns)->  \
                              mdSubmitChan(instHandle->hCtrlBus,&ioPacket);
        }

        if (IOM_PENDING == status)
        {
            SEM_pend (&(instHandle->semObj), Aic31_I2CTIMEOUT);
        }

        /* If read is complete, then copy the required register values*/
        if ((IOM_COMPLETED == ioPacket.status )||
                (IOM_PENDING == ioPacket.status))
        {
            *regData = data[0];
            regData++;
            regIndex++;
            retVal= Aic31_REG_READ_PASS;
        }
        else
        {
            break;
        }
    }

    /* Return the result of reading the Aic31 registers                       */
    return (retVal);
}

/**
 *  This function reads the value of a single Aic31 register. This function
 *  is a wrapper for faclitating single register reads in the Aic31 codec
 *  driver. It internally calls aic31CtrlBusRead function.
 *
 *  \param   instHandle  [IN]  pointer to the driver instance object
 *  \param   regAddr     [IN]  register address
 *  \param   regData     [OUT] register data read
 *
 *  \return  Aic31_REG_READ_PASS in case of success
 *           Aic31_REG_READ_FAIL in case of error
 */
static Bool aic31RegRead(Aic31_Object  *instHandle,
                         Uint8          regAddr,
                         Uint8         *regData)
{
    ICodec_RegData   acRegData;                   /* Register data structure  */
    Bool             retVal = IOM_COMPLETED;      /* Result of read           */

    assert((NULL != instHandle) && (NULL != regData));

    /* Read the data from the Aic31 register                                  */
    acRegData.regCount = 1u;
    acRegData.regIndex = regAddr;
    acRegData.regData  = (Ptr)regData;

    retVal = aic31CtrlBusRead(instHandle, &acRegData);

    /* Return the result of reading the register                              */
    return (retVal);
}


/**
 *  \brief This function writes data into a bit field of Aic31 register. This
 *         function reads the value of the register, modifies the value of the
 *         specified register and writes the value back to the register.
 *
 *  \param  instHandle [IN] pointer to the driver instance object
 *  \param  regAddr    [IN] register address
 *  \param  bfPosition [IN] bit field position
 *  \param  bfSize     [IN] bit field size
 *  \param  bfData     [IN] data to be written to the bit field
 *
 *  \return Aic31_REG_WRITE_PASS in case of success
 *          Aic31_REG_WRITE_FAIL in case of error

 */
static Bool aic31RegWriteBf(Aic31_Object *instHandle,
                            Uint8         regAddr,
                            Uint8         bfPosition,
                            Uint8         bfSize,
                            Uint8         bfData)
{
    Uint8   regData     = 0;                  /* Temporary Register data      */
    Uint8   mask        = 0;                  /* Bit field mask               */
    Bool    bFalseWhile = TRUE;
    Bool    retVal      = Aic31_REG_WRITE_FAIL;

    assert(NULL != instHandle);

    do
    {
        bFalseWhile = FALSE;
    
        /* Read the current value of the register                             */
        retVal = aic31RegRead( instHandle , regAddr , &regData );

        if (Aic31_REG_WRITE_PASS != retVal)
        {
            break;
        }

        /* Update the value of the bit field                                  */
        mask = ((1 << bfSize) - 1) << bfPosition;
        regData = (regData & (Uint8)(~(mask)));
        regData = (regData | (Uint8)( bfData << bfPosition));

        /* Write the data back into the register                              */
        retVal= aic31RegWrite(instHandle,regAddr,regData);

    }while (bFalseWhile);

    /* Return the result of writing data into the bit field                   */
    return (retVal);
}


/**
 *  \brief  This function writes the initialization values for Aic31 ADC
 *          registers. This function should be called to initialize the registers
 *          of the ADC.
 *
 *  \param  instHandle [IN]  Pointer to the driver instance object
 *
 *  \return IOM_COMPLETED
 */
static Int32 aic31InitAdc(Aic31_Object *instHandle)
{
    Int32  status  = IOM_EBADARGS;
    Bool   retVal  = Aic31_REG_WRITE_FAIL;
    Uint8  gain   = 0;

    assert(NULL != instHandle);

    /* Write the initialization values for the ADC registers                  */
    retVal  = aic31RegWriteBf(
                  instHandle, 
                  Aic31_P0_REG7, 
                  (Uint8)1,
                  (Uint8)4, 
                  (Uint8)5);

    /* enable the programmable PGA for left and right ADC                     */
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG15, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG16, (Uint8)0x00);

    /* MIC3L/R is not connected to the left ADC PGA                           */
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG17,(Uint8)0xFF);

    /* MIC3L/R is not connected to the right ADC PGA                          */
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG18,(Uint8)0xFF);

    /* power on the Line L1R                                                  */
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG19,(Uint8)0x04);

    /* REG20 (not required)                                                   */
    /* REG21 (not required)                                                   */

    /* power on the Line LIL                                                  */
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG22,(Uint8)0x04);

   /* check if the user requested gain in less than 100 %                    */
    if (instHandle ->ChanObj[0].chanGain <= Aic31_MAX_GAIN_PERCENT)
    {
        gain = (Uint8)((instHandle ->ChanObj[0].chanGain * 
                    Aic31_MAX_PGA_GAIN_VALUE)/ Aic31_MAX_GAIN_PERCENT);

        /* Left AGC control                                                   */
        retVal |= aic31RegWrite(
                      instHandle,
                      Aic31_LEFT_AGC_CTRLA_ADDR,
                      (Uint8)0x80);
        retVal |= aic31RegWrite(
                      instHandle,
                      Aic31_LEFT_AGC_CTRLB_ADDR,
                      (Uint8)(gain << 1));
        retVal |= aic31RegWrite(
                      instHandle, 
                      Aic31_LEFT_AGC_CTRLC_ADDR, 
                      (Uint8)0x00);

        /* Right AGC control                                                  */
        retVal |= aic31RegWrite(
                      instHandle,
                      Aic31_RIGHT_AGC_CTRLA_ADDR,
                      (Uint8)0x80);
        retVal |= aic31RegWrite(
                      instHandle,
                      Aic31_RIGHT_AGC_CTRLB_ADDR,
                      (Uint8)(gain << 1));
        retVal |= aic31RegWrite(
                      instHandle,
                      Aic31_RIGHT_AGC_CTRLC_ADDR,
                      (Uint8)0x00);
    }
    else
    {
        status = IOM_EBADARGS;
    }

    if (Aic31_REG_WRITE_PASS == retVal)
    {
        status = IOM_COMPLETED;
    }

    return (status);
}

/**
 *
 *  \brief  This function writes the initialization values for Aic31 DAC
 *          registers. This function should be called to initialize the
 *          registers of the DAC.
 *
 *  \param  instHandle [IN]  Pointer to the driver instance object
 *
 *  \return IOM_COMPLETED
 */
static Int32 aic31InitDac(Aic31_Object *instHandle)
{
    Int32  status  = IOM_EBADARGS;
    Bool   retVal  = Aic31_REG_WRITE_FAIL;
    Uint8  gain    = 0;

    assert(NULL != instHandle);

    /* Write the initialization values for the DAC registers                  */
    retVal  = aic31RegWriteBf(
                   instHandle,
                   Aic31_P0_REG7,
                   (Uint8)1,
                   (Uint8)4,
                   (Uint8)5);

    /* power up the left and right DACs                                       */
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG37, (Uint8)0xE0);

    /* select the DAC L1 R1 Paths                                             */
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG41, (Uint8)0x02);

    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG42, (Uint8)0x6C);

    /* unmute the DAC                                                         */
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG43, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG44, (Uint8)0x00);

    /* DAC L to HPL OUT Is connected                                          */
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG47, (Uint8)0x80);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG51, (Uint8)0x09);

    /* DAC R to HPROUT is connected                                           */
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG64, (Uint8)0x80);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG65, (Uint8)0x09);

    /* DACL1 connected to LINE1 LOUT                                          */
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG82, (Uint8)0x80);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG86, (Uint8)0x09);

    /* DACR1 connected to LINE1 ROUT                                          */
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG92, (Uint8)0x80);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG93, (Uint8)0x09);

    /* check if the user requested gain in less than 100 %                    */
    if (instHandle ->ChanObj[1].chanGain <= Aic31_MAX_GAIN_PERCENT)
    {
        gain = (Uint8)((instHandle->ChanObj[1].chanGain * 
                    Aic31_MAX_PGA_GAIN_VALUE) / Aic31_MAX_GAIN_PERCENT);

        gain = Aic31_MAX_PGA_GAIN_VALUE - gain;

        /* Left/Right DAC digital volume gain                                 */
        retVal |= aic31RegWrite(instHandle,Aic31_LEFT_DAC_VOL_CTRL_ADDR, gain);
        retVal |= aic31RegWrite(instHandle,Aic31_RIGHT_DAC_VOL_CTRL_ADDR, gain);
    }
    else
    {
       status = IOM_EBADARGS;
    }

    if (Aic31_REG_WRITE_PASS == retVal)
    {
        status = IOM_COMPLETED;
    }

    return (status);
}

/**
 *  \brief This function writes the reset values for Aic31 ADC registers.
 *         This function should be called to reset the registers of the ADC.
 *         It is recommended to write reset values explicitly.
 *
 *  \param  instHandle [IN]  Pointer to the driver instance object
 *
 *  \return IOM_COMPLETED
 */
static Int32 aic31DeinitAdc(Aic31_Object *instHandle)
{
    Int32  status = IOM_EBADARGS;
    Bool   retVal  = Aic31_REG_WRITE_FAIL;

    assert(NULL != instHandle);

    /* Write the reset values to ADC registers                                */
    retVal  = aic31RegWrite(instHandle, Aic31_P0_REG15, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG16, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG17, (Uint8)0xFF);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG18, (Uint8)0xFF);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG19, (Uint8)0x78);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG22, (Uint8)0x78);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG26, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG27, (Uint8)0xFE);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG28, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG29, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG30, (Uint8)0xFE);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG31, (Uint8)0x00);

    if (Aic31_REG_WRITE_PASS == retVal)
    {
        status = IOM_COMPLETED;
    }

    return (status);
}

/**
 *  \brief   This function writes the reset values for Aic31 DAC registers.
 *           This function should be called to reset the registers of the DAC.
 *           It is recommended to write reset values explicitly.
 *
 *  \param   instHandle [IN]  Pointer to the driver instance object
 *
 *  \return  IOM_COMPLETED
 */
static Int32 aic31DeinitDac(Aic31_Object *instHandle)
{
    Int32  status  = IOM_EBADARGS;
    Bool   retVal  = Aic31_REG_WRITE_FAIL;

    assert(NULL != instHandle);

    /* Write the reset values to DAC registers                                */
    retVal  = aic31RegWriteBf(
                  instHandle, 
                  Aic31_P0_REG7, 
                  (Uint8)1, 
                  (Uint8)4,
                  (Uint8)0);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG37, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG38, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG41, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG42, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG43, (Uint8)0x80);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG44, (Uint8)0x80);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG51, (Uint8)0x04);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG64, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG65, (Uint8)0x04);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG82, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG86, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG92, (Uint8)0x00);
    retVal |= aic31RegWrite(instHandle, Aic31_P0_REG93, (Uint8)0x00);

    if (Aic31_REG_WRITE_PASS == retVal)
    {
        status = IOM_COMPLETED;    
    }

    return (status);
}

/**
 *
 *  \brief   This function configures the mute for ADC and DAC. The mute
 *           functionality can be enabled or disabled for input (ADC) or output
 *           (DAC) channels.
 *
 *  \param   instHandle [IN] pointer to the driver instance object
 *  \param   acChannel  [IN] channel type input or output
 *  \param   muteconfig [IN] mute configuration value TRUE or FALSE
 *
 *  \return  IOM_COMPLETED if success
 *           IOM_EBADARGS  in case of error
 */
static Int32 aic31MuteConfig(Aic31_Object    *instHandle,
                             ICodec_Channel   acChannel,
                             Bool             muteConfig)
{
    /* Intialize the local variables                                          */
    Int32 status        = IOM_COMPLETED;
    Bool  notMuteConfig = FALSE;
    Bool  retVal        = Aic31_REG_WRITE_FAIL;

    assert(NULL != instHandle);

    if (ICodec_Channel_INPUT == acChannel)
    {
        if (TRUE == muteConfig)
        {
            notMuteConfig = FALSE;
        }
        else
        {
            notMuteConfig = TRUE;
        }
    
        /* Mute the left and right input channels                             */
        retVal = aic31RegWriteBf(
                       instHandle, 
                       Aic31_P0_REG26,
                       (Uint8)7, 
                       (Uint8)1,
                       (Uint8)(notMuteConfig));
        retVal |= aic31RegWriteBf(
                       instHandle, 
                       Aic31_P0_REG15, 
                       (Uint8)7, 
                       (Uint8)1,
                       (Uint8)muteConfig);
        retVal |= aic31RegWriteBf(
                       instHandle,
                       Aic31_P0_REG29,
                       (Uint8)7,
                       (Uint8)1,
                       (Uint8)(notMuteConfig));
        retVal |= aic31RegWriteBf(
                       instHandle,
                       Aic31_P0_REG16,
                       (Uint8)7,
                       (Uint8)1,
                       (Uint8)muteConfig);
    }
    else if (ICodec_Channel_OUTPUT == acChannel)
    {
        /* Mute the left and right output channels                            */
        retVal  = aic31RegWriteBf(
                       instHandle, 
                       Aic31_P0_REG43, 
                       (Uint8)7,
                       (Uint8)1,
                       (Uint8)muteConfig);
        retVal |= aic31RegWriteBf(
                      instHandle, 
                      Aic31_P0_REG44, 
                      (Uint8)7, 
                      (Uint8)1, 
                      (Uint8)muteConfig);
    }
    else
    {
        /* Invalid parameter                                                  */
        status = IOM_EBADARGS;
    }

    if (Aic31_REG_WRITE_PASS == retVal)
    {
        status = IOM_COMPLETED;
    }

    /* Return the result of the mute operation                                */
    return (status);
}

/**

 *
 * \brief   This function is the codec call back function invoked from the I2c
 *          driver on transfer completion.
 *
 * \param   semHandle [IN] pointer to the Semaphore Handle
 * \param   ioPacket  [IN] pointer to the completed ioPacket
 *
 * \return  none
 *
 */
static Void aic31CallBack (Ptr semHandle, IOM_Packet * ioPacket)
{
    assert((NULL != semHandle) && (NULL != ioPacket));

    if (NULL != ioPacket)
    {
        ioPacket = ioPacket;
    }

    SEM_post((SEM_Handle)semHandle);
    return;
}

/**
 *
 * \brief   This function selects the output source of the audio data among the
 *          line out,hp out or both options
 *
 * \param   instHandle  [IN] pointer to the driver object
 * \param   destOption  [IN] pointer to Enum specifying the output destination
 *
 * \return  IOM_COMPLETED in case of success.
 *          IOM_EBADARGS  in case of error.
 */
static Int32 aic31SelectOutputDest(Aic31_Object    *instHandle,
                                   Ptr              destOption)
{
    Int    *dest      = 0;
    Int32   status    = IOM_EBADARGS;
    Bool    retVal    = Aic31_REG_WRITE_FAIL;

    assert((NULL != instHandle) && (NULL != destOption));

    dest = (Int *)destOption;

    switch (*dest)
    {
        case ICodec_OutputDest_LINEOUT:
            /* disconnect the HPOUT and conect the lINE OUT                   */

            /* disconnect and power down the HPOUT L                          */
            retVal  = aic31RegWrite(instHandle, Aic31_P0_REG47, (Uint8)0x00);
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG51, (Uint8)0x06);

            /* disconnect and powerdown HPOUT R                               */
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG64, (Uint8)0x00);
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG65, (Uint8)0x06);

            /* DACL1 connected to LINE1 LOUT                                  */
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG82, (Uint8)0x80);
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG86, (Uint8)0x09);

            /* DACR1 connected to LINE1 ROUT                                  */
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG92, (Uint8)0x80);
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG93, (Uint8)0x09);
            break;

        case ICodec_OutputDest_HPOUT:
            /* DACL1 disconnected from LINE1 LOUT                             */
            retVal  = aic31RegWrite(instHandle, Aic31_P0_REG82, (Uint8)0x00);
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG86, (Uint8)0x06);

            /* DACR1 disconnected from LINE1 ROUT                             */
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG92, (Uint8)0x00);
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG93, (Uint8)0x06);

            /* DAC L to HPL OUT Is connected                                  */
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG47, (Uint8)0x80);
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG51, (Uint8)0x09);

            /* DAC R to HPROUT is connected                                   */
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG64, (Uint8)0x80);
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG65, (Uint8)0x09);
            break;
        case ICodec_OutputDest_BOTH:
            /* DAC L to HPL OUT Is connected                                  */
            retVal = aic31RegWrite(instHandle, Aic31_P0_REG47, (Uint8)0x80);
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG51, (Uint8)0x09);

            /* DAC R to HPROUT is connected                                   */
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG64, (Uint8)0x80);
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG65, (Uint8)0x09);

            /* DACL1 connected to LINE1 LOUT                                  */
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG82, (Uint8)0x80);
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG86, (Uint8)0x09);

            /* DACR1 connected to LINE1 ROUT                                  */
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG92, (Uint8)0x80);
            retVal |= aic31RegWrite(instHandle, Aic31_P0_REG93, (Uint8)0x09);
            break;
        default:
            status = IOM_EBADARGS;
            break;
    }

    if (Aic31_REG_WRITE_PASS == retVal)
    {
        status = IOM_COMPLETED;
    }

    return (status);
}

/**
 *
 * \brief   This function selects the input source of the audio data among the
 *          available sources of Mic in and line in
 *
 * \param   instHandle  [IN] pointer to the driver object
 * \param   srcOption   [IN] Enum specifying the output destination
 *
 * \return  IOM_COMPLETED in case of success.
 *          IOM_EBADARGS  in case of error.(write failed)
 *
 */
static Int32 aic31SelectInputSource(Aic31_Object    *instHandle,
                                    Ptr              srcOption)
{
    Int    *src     = 0;
    Bool    retVal  = Aic31_REG_WRITE_FAIL;
    Int32   status  = IOM_EBADARGS;

    assert((NULL != instHandle) && (NULL != srcOption));

    src = (Int *)srcOption;

    if (ICodec_InputDest_MICIN == (*src))
    {
        /* disable the LINE IN connected to the ADC                           */
        retVal  = aic31RegWrite(instHandle, Aic31_P0_REG19, (Uint8)0x7C);
        retVal |= aic31RegWrite(instHandle, Aic31_P0_REG22, (Uint8)0x7C);

        /* connect the MIC IN to the ADC                                      */
        retVal |= aic31RegWrite(instHandle, Aic31_P0_REG17, (Uint8)0x0F);
        retVal |= aic31RegWrite(instHandle, Aic31_P0_REG18, (Uint8)0xF0);
    }
    else if (ICodec_InputDest_LINEIN == (*src))
    {
        /* disable the MIC IN connected to the ADC                            */
        retVal = aic31RegWrite(instHandle, Aic31_P0_REG17, (Uint8)0xFF);
        retVal |= aic31RegWrite(instHandle, Aic31_P0_REG18, (Uint8)0xFF);
        
        /* connect the LINE IN to the ADC                                     */
        retVal |= aic31RegWrite(instHandle, Aic31_P0_REG19, (Uint8)0x04);
        retVal |= aic31RegWrite(instHandle, Aic31_P0_REG22, (Uint8)0x04);
    }
    else
    {
        /* Invalid parameter                                                  */
        status = IOM_EBADARGS;
    }

    /* if all the reg writes have passed we will send the status as completed */
    if (Aic31_REG_WRITE_PASS == retVal)
    {
        status = IOM_COMPLETED;
    }
    
    return (status);
}
/* ========================================================================== */
/*                             END OF FILE                                    */
/* ========================================================================== */
