/*
 * sata_intf.h
 *
 * This file includes SATA AHCI specific user configuration
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
 *	\author     .Ravi B (PSP-USB)
 *  \version    .1.0
 */

#ifndef _SATA_INTF_H
	#define _SATA_INTF_H

/**
* \brief AhciStartPort
*
* this function start the operation of the ahci port
*
* @param port[IN] pointer to ahci controller port object
*
* @returns none
*/
extern void ahciStartPort(AhciPortData *port);
/**
* \brief AhciStopPort
*
* this function start the operation of the ahci port
*
* @param port[IN] pointer to ahci controller port object
*
* @returns none
*/
extern void ahciStopPort(AhciPortData *port);

/**
* \brief ahciPortIssueCmd
*
* this function initiates the command execcution corresponds to command tag
*
* @param port[IN] pointer to ahci controller port object
*
* @returns none
*/
void ahciPortIssueCmd(AhciPortData *port, Uint32 tag);

/**
* \brief ahciFillCmdSlot
*
* this function fills the command slot
*
* @param port[IN] pointer to ahci controller port object
*
* @returns 0 on success
*/
void ahciFillCmdSlot(AhciPortData *port, Uint32 cmdTag, Uint32 opts);
/**
* \brief ahciReset
*
* perform ahci Controller reset or HBA Reset
*
* @param pAhciHost[IN] pointer to ahci controller object
* @param portNum [IN] port number
*
* @returns 0 on success
*/
Int32 ahciReset(AhciHostCtrl *pAhciHost, Uint32 portNum, Uint32 resetType);

/**
* \brief ahciPortGetFreeCmdSlot
*
* get free command slot/tag from ahci port
*
* @param pAhciHost	[IN] pointer to ahci host controller
*
* @returns -ve if no free command slot is available
*             else returns the free command slot number.
*/
Int32 ahciPortGetFreeCmdSlot(AhciPortData *port);

/**
* \brief sataEnableIntr
*
* enabling sata interrupt
*
* @param pAhciHost	[IN] host controller index
*
* @returns None
*          
*/
void sataEnableIntr(Uint32 hbaIndx);
/**
* \brief sataDisableIntr
*
* disables sata interrupt
*
* @param pAhciHost	[IN] host controller index
*
* @returns None
*          
*/
void sataDisableIntr(Uint32 hbaIndx);

/**
* \brief sataAttachIntHandler
*
* attach interrupt sata handler
*
* @param pAhciHost	[IN] host controller index
*
* @returns None
*          
*/
void sataAttachIntHandler(Uint32 hbaIndx);

#endif

