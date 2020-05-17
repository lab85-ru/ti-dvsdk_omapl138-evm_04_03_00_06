/*
 * psp_sata.h
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
 *	\version    .1.0
 */
#ifndef _SATA_CONFIG_H
	#define _SATA_CONFIG_H

#include <std.h> //"tistdtypes.h"

#if defined(CHIP_C6748) 
	#include "ti/pspiom/cslr/soc_C6748.h"
#else
#if defined(CHIP_OMAPL138) 
	#include "ti/pspiom/cslr/soc_OMAPL138.h"
#endif
#endif
 

#define		SATA_CONFIG_MAX_NUM_AHCI_HOST		(CSL_SATA_PER_CNT)	/* maximum number  ahci host controller */
#define		SATA_CONFIG_MAX_NUM_AHCI_PORTS		1					/* maximum number of ports supported ahci host controller */
#define		SATA_CONFIG_MAX_NUM_PRD_ENTRIES		10					/* maximum number of physical region  descriptor table entries for for each command slot */
#define		SATA_CONFIG_MAX_NUM_AHCI_CMDSLOT	32					/* maximum number of command slot supported by each ahci host controller */

/* configuration flags to enb/dis ahci feature */
#define SATA_FLAG_DISABLE_NCQ                	(1 << 0)
#define SATA_FLAG_ENABLE_NCQ					(1 << 1)
#define SATA_FLAG_DISABLE_PMP					(1 << 2)
#define SATA_FLAG_DISABLE_HOTPLUG				(1 << 3)

/* config flags to enable the ahci capability */
#define SATA_USER_CTRL_FLAG				( SATA_FLAG_ENABLE_NCQ | SATA_FLAG_DISABLE_PMP)

#define SATA_PSC_PWRM_ENABLE					(1)
#define SATA_PSC_PWRM_DISABLE					(0)

/**
 * \brief PlatformRes_t
 *
 * platform resource
 */
typedef struct{
	Uint32	baseAdr;
	Uint32	irqEventNum;
	Uint32	irqEventGroup;
}PlatformRes_t;

/**
 * \brief PlatformResource
 *
 * platform resource
 */
typedef struct{
	Uint32	numRes;
	PlatformRes_t res[SATA_CONFIG_MAX_NUM_AHCI_HOST];
	Uint32	pscPwrmEnable;
#ifdef BIOS_PWRM_ENABLE
	PWRM_Domain	pll_domain;
#endif
}PlatformResource;

/**
 *	\brief	Initializes the sata subsystem , does platform init, 
 *			evm specific init and initializes the ahci controller 
 *			and all the sata ports
 *
 *	\param	inst_id			[IN]	instance id of sata controller
 *	\param	platform_res	[IN]	platform resource
 *	
 *	\return	none
 */
Int32 PSP_sataDrvInit(Uint32 inst_id, PlatformResource *platform_res);
/**
 *	\brief	DeInitializes the sata subsystem , 
 *
 *	\param	inst_id			[IN]	instance id of sata controller
 *	\return	none
 */
Int32 PSP_sataDrvDeInit(Uint32 inst_id);

#endif

