/*
 * ddc_nand.h
 *
 * This file contains NAND core functionality macros and interface definitions. 
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

/** \file      ddc_nand.h
 *
 *  \brief     Implementation of DDC layer of NAND of NAND driver.
 *
 *  This file contains the implementation of the DDC layer of the NAND
 *  driver. It implements all the API's defined in the psp_nand.h file.
 *
 *  (C)      Copyright 2008,      Texas Instruments, Inc
 */

#ifndef _DDC_NAND_H
#define _DDC_NAND_H

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include "llc_nand.h"
#include "ddc_nandftl.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           NAND DATA TYPES                                  */
/* ========================================================================== */

/**
 * \brief Driver states
 *
 *  Enumerations indicating the state of the drivers
 */
typedef enum
{
    /** Indicates that the driver is in deleted state          */
    DDC_DRIVER_STATE_DELETED,
    /** Indicates that the driver is in created state          */
    DDC_DRIVER_STATE_CREATED,
    /** Indicates that the driver has been initialized         */
    DDC_DRIVER_STATE_INITIALIZED,
    /** Indicates that the driver has been opened              */
    DDC_DRIVER_STATE_OPENED,
    /** Indicates that the driver is closed                    */
    DDC_DRIVER_STATE_CLOSED,
    /** Indicates that the driver has veen de-initialized      */
    DDC_DRIVER_STATE_DEINITIALIZED,
    /** Indicates that the driver has powered down the device  */
    DDC_DRIVER_STATE_POWERED_DOWN
}DDC_DriverState;

/**
 *  \brief  DDC NAND module object representation
 *
 *          This structure defines the data object instances of the NAND DDC
 *          module object.
 */
typedef struct DDC_nandObj_t
{
    DDC_DriverState      state;      /**< State of the driver        */
    PSP_NandOpMode       opMode;     /**< NAND driver operating mode */
    Uint32               instanceId; /**< Instance ID of driver      */
    PSP_nandDeviceInfo  *devInfo;    /**< Device organizaito info    */
    Uint32               maxSectors; /**< Maximum number of sectors  */
    Ptr                  llcHandle;  /**< LLC module handle          */
    LLC_nandFxns        *llcFxns;    /**< LLC function table         */
    DDC_nandFtlObj      *ftlObj;     /**< FTL module handle          */
}DDC_nandObj;
/**<  DDC NAND module object representation */

#ifdef __cplusplus
}
#endif

#endif /* _DDC_NAND_H */
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */

