/*
 * CodecInterfaceLocal.h
 *
 * Codec Interface internal header file
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
 *  \file   CodecInterfaceLocal.h
 *
 *  \brief  This file contains I2C/SPI interface function declaration for
 *          external devices. The SPI implemenation to configure external
 *          device is not implemented
 */

#ifndef _CODECINTERFACELOCAL_H_
#define _CODECINTERFACELOCAL_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

/* ========================================================================== */
/*                             ENUMS                                          */
/* ========================================================================== */

/* ========================================================================== */
/*                              STRUCTURES                                    */
/* ========================================================================== */
/**
 *  \brief Structure used during read and write to the codec registers.
 *
 *  This structure specifies the codec register write or read
 *  information. The regIndex element specifies the starting index
 *  of the codec register. The regData element specifies the register
 *  data to be read/written. The regCount element specifies the
 *  total number of registers to be read/written.
 */
typedef struct CodecRegData_t
{
    Edc_ControlBusType edcControlBusType;
    /**< Type of the control bus (SPI or I2C)                                 */
    Uint8 slaveAddr;
    /**< Encoder Control bus address                                          */
    Uint32  subAddr;
    /**< Starting index if the codec register                                 */
    Ptr     buffer;
    /**< Register data written/read                                           */
    Uint32  length;
    /**< Number of registers written/read                                     */
}CodecRegData;

/* ========================================================================== */
/*                          FUNCTION DECLARATIONS                             */
/* ========================================================================== */
/**
 *  \brief Codec Init
 *
 *  This function must be called before starting any codec operations. It
 *  initializes the device interfacing codec
 *
 *  \param   devName [IN] name of device interfaceing codec
 *
 *  \return  IOM_COMPLETED if successful else IOM_EBADIO error code.
 */
Int32 initCodecInterface(String devName);

/**
 *  \brief Codec Deinit
 *
 *  This function should be called to deinitialize codec interface module
 *
 *  \param   edcControlBusType [IN] type of controlling bus
 *
 *  \return  IOM_COMPLETED if successful else IOM_EBADIO error code.
 */
Int32 deInitCodecInterface(Edc_ControlBusType edcControlBusType);

/**
 *  \brief Codec write register
 *
 *  This function is called to write on codec register via interface device.
 *
 *  \param   edcControlBusType [IN] type of controlling bus
 *  \param   slaveAddr         [IN] slave address of Codec device
 *  \param   subAddr           [IN] sub address of slave on which which
 *                                  write operation to be performed
 *  \param   buffer            [IN] buffer address from which values to be read
 *  \param   length            [IN] length of transfer. number of bytes to write
 *
 *  \return  FALSE if successful else TRUE for error(failure of write operation)
 */
Bool codecWriteReg(Edc_ControlBusType edcControlBusType,
                   Uint8 slaveAddr,
                   Uint8 subAddr,
                   const Uint8 *buffer,
                   Uint32 length);

/**
 *  \brief Codec read register
 *
 *  This function is called to read on codec register via interface device
 *
 *  \param   edcControlBusType [IN]  type of controlling bus
 *  \param   slaveAddr         [IN]  slave address of Codec device
 *  \param   subAddr           [IN]  sub address of slave from which read
 *                                   operation to be performed
 *  \param   buffer            [OUT] buffer address at which read values to be
 *                                   stored
 *  \param   length            [IN]  length of transfer(number of bytes to read)
 *
 *  \return  FALSE if successful else TRUE for error(failure of read operation)
 */
Bool codecReadReg(Edc_ControlBusType edcControlBusType,
                  Uint8 slaveAddr,
                  Uint8 subAddr,
                  Uint8 *buffer,
                  Uint32 length);


/**
 *  \brief Asynce codec write register
 *
 *  This function is called to write on codec register via interface device. The
 *  call returns immediately. The callback function present in cbData is used
 *  to callback and tell the status.
 *
 *  \param   wrRegData [IN] data to be written to codec
 *  \param   cbData    [IN] callback function and handle structure
 *
 *  \return  return the submit to interface layer status
 */
Int32 codecWriteRegAsync(CodecRegData *wrRegData, Edc_Data *cbData);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* _CODECINTERFACELOCAL_H_ */

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/

