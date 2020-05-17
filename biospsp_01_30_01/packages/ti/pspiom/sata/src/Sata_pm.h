/*
 * sata_pm.h
 *
 * This file implements the power management for SATA.
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
#ifdef BIOS_PWRM_ENABLE
#include "psp_ata_pwrm.h"
#endif

#define PM_SATA_DEBUG

#define	PWRM_DRIVER_SELF_PM_ENABLE		1
#define	PWRM_DVFS_PM_ENABLE				2
#define	PWRM_SLEEP_PM_ENABLE			4


/**
 *	\brief SATA PM states
 */
#define	PM_SATA_INIT			1
#define	PM_SATA_POWER_ON		2
#define	PM_SATA_POWER_OFF		3


#define	PM_SATA_ERR_MODULE_ON_DCNT_ZERO		(1 << 0)
#define	PM_SATA_ERR_MODULE_OFF_DCNT_NZERO	(1 << 1)
#define	PM_SATA_PWR_ON_ERR					(1 << 30)
#define	PM_SATA_PWR_OFF_ERR			(Uint32)(1 << 31)

/**
 *	\brief	pm_sata_init
 *			initialize power management for sata
 *
 *	\param	pAhciHost	[IN] pointer to ahci host controller
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
extern Int32 pm_sata_init(AhciHostCtrl *pAhciHost);

/**
 *	\brief	pm_sata_deinit
 *			deinitialize power management module
 *
 *	\param	pAhciHost	[IN] pointer to ahci host controller
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
Int32 pm_sata_deinit(AhciHostCtrl *pAhciHost);

/**
 *	\brief	is_sata_pm_enable
 *			return sata pm enabled/disabled
 *
 *	\param	None
 *
 *	\return	None
 */
Uint32 is_sata_pm_enable(Uint32 hbaIndex);
#ifdef BIOS_PWRM_ENABLE
/**
 *	\brief	pm_ata_pwrm_init
 *			perfrom registeration with pwrm module
 *
 *	\param	pAtaPwrm			PWRM Information structure
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
Int32 pm_ata_pwrm_init(Ptr DevHandle);

/**
 *	\brief	pm_sata_power_off
 *			switch off sata module's power
 *
 *	\param	pAhciHost	[IN] pointer to ahci host controller
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
Int32 pm_sata_power_off(void);

/**
 *	\brief	pm_sata_power_on
 *			switch on sata module's power
 *
 *	\param	pAhciHost	[IN] pointer to ahci host controller
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
Int32 pm_sata_power_on(void);
#endif


