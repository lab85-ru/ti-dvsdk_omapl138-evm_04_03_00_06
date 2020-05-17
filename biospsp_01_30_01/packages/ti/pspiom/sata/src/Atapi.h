/*
 * atapi.h
 *
 * ATAPI Interface header file
 * This file defines the various data structures to be used for various ATAPI
 * devices.
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
 *  \author     Swaminathan S.
 *  \version    10.0
 */
 
#ifndef _ATAPI_H
#define _ATAPI_H

#include <tistdtypes.h>
#include <psp_ata_med.h>
#include <psp_atapi_med.h>

/**
 *  \brief ATAPI_MED_Void ()       : Return error.
 *
 *  \param Handle  [IN]             : Device Handle
 *  \param MedHndl [IN]             : Handle for Media Layer
 *  \param pCmd    [IN]             : Pointer to Command
 *  \return int                     :
 ******************************************************************************/
Int32  ATAPI_MED_Void  (Ptr Handle, Ptr MedHndl, Ptr pCmd);
/**
 *  \brief ATAPI_MED_Dma ()      : Submit a RAW ATAPI command when in DMA mode.
 *
 *  \param Handle  [IN]             : Device Handle
 *  \param MedHndl [IN]             : Handle for Media Layer
 *  \param pCmd    [IN]             : Pointer to Command
 *  \return int                     :
 ******************************************************************************/
Int32  ATAPI_MED_Dma  (Ptr Handle, Ptr MedHndl, Ptr pCmd);
/**
 *  \brief ATAPI_MED_Pio ()      : Submit a RAW ATAPI command when in PIO mode.
 *
 *  \param Handle  [IN]             : Device Handle
 *  \param MedHndl [IN]             : Handle for Media Layer
 *  \param pCmd    [IN]             : Pointer to Command
 *  \return int                     :
 ******************************************************************************/
Int32  ATAPI_MED_Pio  (Ptr Handle, Ptr MedHndl, Ptr pCmd);
/**
 *  \brief ATAPI_PktHndlr () : ATAPI Packet request handler.
 *                           Executes in Interrupt context.
 *  \param Handle [IN]     : Handle to the Device.
 *  \param Status [IN]     : Command status
 *  \return Bool           : False (Also Called from timeout context so False)
 ******************************************************************************/
Bool ATAPI_PktHndlr (void const * Handle, ATA_Dev_Stat_t const *pStatus);
/**
 *  \brief ATAPI_ProcessPacketCmd () : Process a PACKET command submition.
 *                                     Can be invoked in Interrupt Context.
 *
 *  \param pDevice [IN]             : Device Pointer
 *  \return int                     : Success/Failure/Timeout
 ******************************************************************************/
Int32 ATAPI_ProcessPacketCmd (ATA_Drive_t const *pDevice);

#endif /* _ATAPI_H */

