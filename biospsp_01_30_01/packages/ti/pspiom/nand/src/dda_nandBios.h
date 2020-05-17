/*
 * dda_nandBios.h
 *
 * This file contains NAND media layer macros and interface definitions. 
 * NAND Driver provides Driver Layer Interface to do operations on the NAND  
 * peripheral like device initialization, channel creation, control commands for 
 * peripheral specific operations etc
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

/** \file   dda_nandBios.h
 *
 *  \brief  DDA wrapper for interfacing NAND driver with block-media driver.
 *
 *  This file lists the interfaces between the NAND driver and the
 *  block media driver.
 */

#ifndef _DDA_NANDBIOS_H_  /* Sentry */
#define _DDA_NANDBIOS_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include <ti/pspiom/blkmedia/psp_blkdev.h>
#include <ti/pspiom/nand/psp_nand.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */

/**< Maximum no of NAND devices */
#define DDC_NAND_MAXDEVS    (1u)

/* ========================================================================== */
/*                           NAND DATA TYPES                                  */
/* ========================================================================== */

/**
 *  \brief NAND Driver Information structure
 *
 *  This structure defines the instance of DDA Nand object. Multiple instances
 *  of this structure can be created as required.
 */
typedef struct DDA_nandObj_t
{
    PSP_BlkDevOps_t       devOps;    /**< Block media options   */
    PSP_blkDevCallback_t  devCb;     /**< Block Media Callback  */
    Ptr                   handle;    /**< Handle to the Driver  */
}
DDA_nandObj;


/* ========================================================================== */
/*                           NAND PUBLIC FUNCTIONS                            */
/* ========================================================================== */

/**
 * \brief  Initialize the NAND driver block-media interface module.
 *
 *         This API initializes the NAND driver. Called by the application,
 *         this API initializes the device through the corresponding PSP
 *         initialization call.On a successful initialization, the NAND driver
 *         is registered with the block media and proceeds to check the NAND
 *         device(s) on board.
 *
 * \param  config - NAND driver initialization parameters.
 *
 *  \return  IOM_COMPLETED or Error
 */
Int32 DDA_nandInit(PSP_nandConfig  *config);

/**
 * \brief  Deinitialize the interface between block-meida and NAND driver.
 *
 *         API that De-initialize the NAND media on the board/in the system,
 *         through the PSP's Deinit call. After the NAND device(s) is (are)
 *         de-initialized read,write and ioctl will not work .
 *
 *  \return  IOM_COMPLETED or Error
 */
Int32 DDA_nandDeinit(Void);

/**
 * \brief  Execute an IOCTL command.
 *
 *         This function executes a IOCTL command specified by the block-meida
 *         driver.
 *
 * \param  handle      [IN]  Handle of the Block Device
 * \param  mHandle       [IN]  Job Handle from this module
 * \param  pIoctlInfo  [IN]  IOCTL information
 * \param  pIsComplete [OUT] Is the IOCTL complete
 *
 *  \return IOM_COMPLETED or IOM_EBADARGS
 */
Int32 DDA_nandIoctl(Ptr                           handle,
                    Ptr                           mHandle,
                    const PSP_BlkDevIoctlInfo_t  *pIoctlInfo,
                    Bool                         *pIsComplete);

/**
 * \brief  Read data from the NAND device
 *
 *         This function reads data from the NAND device. The starting logical
 *         sector number and the number of sectors to be read have to be
 *         specified.
 *
 * \param  handle   [IN]  Device Handle for the device
 * \param  mHandle  [IN]  Media Driver Handle for this device
 * \param  buffer   [IN]  Buffer Pointer
 * \param  sectNum  [IN]  Sector Number to start
 * \param  numSect  [IN]  Number of sectors to transfer
 *
 * \return IOM_COMPLETED or Error
 */
Int32 DDA_nandRead(Ptr     handle,
                   Ptr     mHandle,
                   Ptr     buffer,
                   Uint32  sectNum,
                   Uint32  numSect);

/**
 * \brief  Write data to the NAND device.
 *
 *         This function writes data to the NAND device. The starting logical
 *         sector number and the number of sectors to be read have to be
 *         specified.
 *
 * \param  handle   [IN] Device Handle for the device
 * \param  mHandle  [IN] Media Driver Handle for this device
 * \param  buffer   [IN] Buffer Pointer
 * \param  sectNum  [IN] Sector Number to start
 * \param  numSect  [IN] Number of sectors to transfer
 *
 * \return  IOM_COMPLETED or error.
 */
Int32 DDA_nandWrite(Ptr      handle,
                    Ptr      mHandle,
                    Ptr      buffer,
                    Uint32  sectNum,
                    Uint32  numSect);


#ifdef __cplusplus
}
#endif


#endif /* _DDA_NANDBIOS_H_ */
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
