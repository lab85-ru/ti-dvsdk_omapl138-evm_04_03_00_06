/*
 * sata_pm.c
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
/** 
 *	\author     Ravi B.(PSP-USB)
 *	\version    1.0
 */
#include <std.h>
#include <iom.h>
#include <hwi.h>
#include <tsk.h>
#include <sys.h>
#include <bcache.h>
#include <string.h>
#include <assert.h> 
#include <log.h>

#include "ata.h"
#include "ahci.h"
#include "psp_ata_debug.h"
#include "sata_pm.h"

#ifdef CHIP_C6748
#ifdef BIOSATA_PSC_ENABLE
#include <ti/pspiom/psc/Psc.h>
#include <ti/pspiom/cslr/cslr_psc_C6748.h>
#endif
#include <ti/pspiom/cslr/soc_C6748.h>
#else
#ifdef CHIP_OMAPL138
#ifdef BIOSATA_PSC_ENABLE
#include <ti/pspiom/psc/Psc.h>
#include <ti/pspiom/cslr/cslr_psc_OMAPL138.h>
#endif
#include <ti/pspiom/cslr/soc_OMAPL138.h>
#endif
#endif

#include <ti/pspiom/sata/psp_ata_pwrm.h>

Uint32 pwrm_sata_trace = 0;

#define SATACTRL_PWRDN					(0x01E2C018)
#define PWRM_SATA_TRACE(val) (pwrm_sata_trace |= val)

extern Ptr getIdeCtrl(Uint32 drive_num);

/**
 *	\brief	sataPscClkEnable
 *			switch on sata module's power
 *
 *	\param	None
 *
 *	\return	None
 */
void sataPscClkEnable(void)
{
#ifdef BIOS_PWRM_ENABLE
	PWRM_setDependency(PWRM_RSRC_SATA);
#else
#ifdef BIOSATA_PSC_ENABLE
	Psc_ModuleClkCtrl(Psc_DevId_1, CSL_PSC_SATA,1);
#endif
#endif
}


/**
 *	\brief	sataPscClkDisable
 *			switch off sata module's power
 *
 *	\param	None
 *
 *	\return	None
 */
void sataPscClkDisable(void)
{
#ifdef BIOS_PWRM_ENABLE
	Uint32 sataDepCnt = 0xFFFF;

	do 
	{
		PWRM_releaseDependency(PWRM_RSRC_SATA);
		PWRM_getDependencyCount(PWRM_RSRC_SATA, &sataDepCnt);
	} while (sataDepCnt);

#else

#ifdef BIOSATA_PSC_ENABLE
	Psc_ModuleClkCtrl(Psc_DevId_1, CSL_PSC_SATA,0);
#endif

#endif
}


/**
 *	\brief	is_sata_pm_enable
 *			return sata pm enabled/disabled
 *
 *	\param	None
 *
 *	\return	None
 */
Uint32 is_sata_pm_enable(Uint32 hbaIndex)
{
	AhciHostCtrl *pAhciHost = getAhciHandle(hbaIndex);

	if( pAhciHost->plat_res->pscPwrmEnable == SATA_PSC_PWRM_ENABLE )
		return TRUE;
	else
		return FALSE;
}
/**
 *	\brief	pm_sata_power_on
 *			switch on sata module's power
 *
 *	\param	pAhciHost	[IN] pointer to ahci host controller
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
Int32 pm_sata_power_on(void)
{
	Int32 retVal = IOM_COMPLETED;
	AhciHostCtrl *pAhciHost = getAhciHandle(0);

#ifdef BIOS_PWRM_ENABLE
	Uint32 sataDepCnt = 0xFFFF;
	PWRM_Status pwrm_status;
#endif

	if( pAhciHost->is_pwr_mgmt_enabled )
	{
		if( pAhciHost->pm_state == PM_SATA_POWER_OFF ||
			pAhciHost->pm_state == PM_SATA_INIT )
		{
			
			if( pAhciHost->plat_res->pscPwrmEnable == SATA_PSC_PWRM_ENABLE )
			{
#ifdef BIOS_PWRM_ENABLE
				pwrm_status = PWRM_getDependencyCount(pAhciHost->lpsc_id, 
												&sataDepCnt);
				if( pwrm_status )
				{
					/* error */
					pAhciHost->pm_error |= PM_SATA_PWR_ON_ERR; 
				}
				if( sataDepCnt )
					pAhciHost->pm_error |= PM_SATA_ERR_MODULE_OFF_DCNT_NZERO; 

				pwrm_status = PWRM_setDependency(pAhciHost->lpsc_id);
				
				if( pwrm_status )
				{
					/* error */
					pAhciHost->pm_error |= PM_SATA_PWR_ON_ERR; 
				}
#else
#ifdef BIOSATA_PSC_ENABLE
	 			Psc_ModuleClkCtrl(Psc_DevId_1, pAhciHost->lpsc_id,1);
#endif
#endif	

				pAhciHost->pm_state = PM_SATA_POWER_ON;
				pAhciHost->pm_on_cnt++;

			}
		}
	}
	return retVal;
}

/**
 *	\brief	pm_sata_power_off
 *			switch off sata module's power
 *
 *	\param	pAhciHost	[IN] pointer to ahci host controller
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
Int32 pm_sata_power_off(void)
{
	Int32 retVal = IOM_COMPLETED;
	AhciHostCtrl *pAhciHost = getAhciHandle(0);
#ifdef BIOS_PWRM_ENABLE
	Uint32 sataDepCnt = 0xFFFF;
	PWRM_Status pwrm_status;
#endif

	if( pAhciHost->is_pwr_mgmt_enabled )
	{
		if( pAhciHost->pm_state == PM_SATA_POWER_ON )
		{
			if( pAhciHost->plat_res->pscPwrmEnable == SATA_PSC_PWRM_ENABLE )
			{
#ifdef BIOS_PWRM_ENABLE
			do 
			{

				pwrm_status = PWRM_getDependencyCount(pAhciHost->lpsc_id, 
												&sataDepCnt);
				if( pwrm_status )
				{
					/* error */
				}
				if( sataDepCnt == 0) 
				{
					pAhciHost->pm_error |= PM_SATA_ERR_MODULE_ON_DCNT_ZERO; 
				}

				pwrm_status = PWRM_releaseDependency(pAhciHost->lpsc_id);
				if( pwrm_status )
				{
					/* error */
					pAhciHost->pm_error |= PM_SATA_PWR_OFF_ERR; 
				}

				pwrm_status = PWRM_getDependencyCount(pAhciHost->lpsc_id, 
												&sataDepCnt);
			} while (sataDepCnt);
#else
#ifdef BIOSATA_PSC_ENABLE
		 		Psc_ModuleClkCtrl(Psc_DevId_1, pAhciHost->lpsc_id,0);
#endif
#endif
				pAhciHost->pm_state = PM_SATA_POWER_OFF;
				pAhciHost->pm_off_cnt++;


			}
		}
	}
	
	return retVal;
}
/**
 *	\brief	pm_sata_prd_task
 *			switch off sata module's power
 *
 *	\param	None
 *
 *	\return	None
 */

void pm_sata_prd_task(void)
{
	AhciHostCtrl *pAhciHost = getAhciHandle(0);

	if ( pAhciHost->pm_mode & PWRM_DRIVER_SELF_PM_ENABLE )
	{
		/* check any i/o is pending */
		if( !Is_ATA_IO_Pending() )
			pm_sata_power_off();
	}
}

/**
 *	\brief	pm_sata_init
 *			initialize power management for sata
 *
 *	\param	pAhciHost	[IN] pointer to ahci host controller
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
Int32 pm_sata_init(AhciHostCtrl *pAhciHost)
{
	Int32 retVal = IOM_COMPLETED;

#ifdef BIOS_PWRM_ENABLE
	Uint32 sataDepCnt = 0xFFFF;
	PWRM_Status pwrm_status;
#endif

#ifdef BIOS_PWRM_ENABLE	
	pAhciHost->lpsc_id 		= PWRM_RSRC_SATA;	
#else
	pAhciHost->lpsc_id 		= CSL_PSC_SATA;	
#endif

	pAhciHost->pm_on_cnt 			= 0;
	pAhciHost->pm_off_cnt 			= 0;
	pAhciHost->pm_mode				= PWRM_DRIVER_SELF_PM_ENABLE;

	pAhciHost->is_pwr_mgmt_enabled 	= 1;
	pAhciHost->pm_state				= PM_SATA_INIT;

	if( pAhciHost->plat_res->pscPwrmEnable == SATA_PSC_PWRM_ENABLE )
	{
		/* power on the sata module */
	#ifdef BIOS_PWRM_ENABLE
		do
		{
			pwrm_status = PWRM_getDependencyCount(pAhciHost->lpsc_id, 
													&sataDepCnt);
			if( pwrm_status )
			{
				/* error */
				pAhciHost->pm_error |= PM_SATA_PWR_ON_ERR; 
			}

			if( sataDepCnt )
				PWRM_releaseDependency(pAhciHost->lpsc_id);

		}while( sataDepCnt );
	#endif
		pm_sata_power_on();
	}

	return retVal;
}

/**
 *	\brief	pm_sata_deinit
 *			deinitialize power management module
 *
 *	\param	pAhciHost	[IN] pointer to ahci host controller
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
Int32 pm_sata_deinit(AhciHostCtrl *pAhciHost)
{
	Int32 retVal = IOM_COMPLETED;

	pAhciHost->pm_mode				= PWRM_DRIVER_SELF_PM_ENABLE;

	/* power on the sata module */
	pm_sata_power_off();

	pAhciHost->is_pwr_mgmt_enabled 	= 0;
	pAhciHost->pm_state				= 0;

	return retVal;
}

#ifdef BIOS_PWRM_ENABLE

/**
 *	\brief	ataPwrm_suspend_Iops
 *			suspend the io operation on the device
 *
 *	\param	devHandle			[IN] pointer to device handle
 *			eventType			[IN] pwrm event type
 *
 *	\return	PWRM_NOTIFYDONE on sucess else PWRM_NOTIFYNOTDONE
 */
PWRM_NotifyResponse	ataPwrm_suspend_Iops(Ptr *devHandle, 
										PWRM_Event   eventType)
{
	PWRM_NotifyResponse retVal = PWRM_NOTIFYDONE;  
	ATA_PWRM_Info *pAtaPwrm;
	Uint32 done = 1;
	
	do
	{
		pAtaPwrm = (ATA_PWRM_Info *)ATA_getPwrmHandle(devHandle);
		pAtaPwrm->pwrmEvent = eventType;
		if(	0 != ATA_IDE_SuspendIO(devHandle) )
		{
			pAtaPwrm->ioSuspend = 1;
			retVal = PWRM_NOTIFYNOTDONE;
			break;
		}
	} while (!done);

	return retVal;
}

/**
 *	\brief	ataPwrm_resume_Iops
 *			resume the io operation on the device
 *
 *	\param	devHandle			[IN] pointer to device handle
 *			eventType			[IN] pwrm event type
 *
 *	\return	PWRM_NOTIFYDONE on sucess else PWRM_NOTIFYNOTDONE
 */
PWRM_NotifyResponse	ataPwrm_resume_Iops(Ptr *devHandle, 
										PWRM_Event   eventType)
{
	PWRM_NotifyResponse retVal = PWRM_NOTIFYDONE;  
	ATA_PWRM_Info *pAtaPwrm;
	Uint32 done = 1;

	do
	{
		pAtaPwrm = (ATA_PWRM_Info *)ATA_getPwrmHandle(devHandle);
		pAtaPwrm->pwrmEvent = eventType;
		if(	0 != ATA_IDE_ResumeIO(devHandle) )
		{
			pAtaPwrm->ioSuspend = 1;
			retVal = PWRM_NOTIFYNOTDONE;
			break;
		}
	} while (!done);

	return retVal;
}

void ataPwrm_CallbackFn(void)
{
	PWRM_Event event;
	Uint32 i;
	if( Ide_Ctlr_Info [0].Drive[0].DevState == ATA_DEV_IO_SUSPEND )
	{
		/* suspend the io */
		event = Ide_Ctlr_Info [0].pwrmInfo.pwrmEvent;
		i = (Uint32)event;

		if( Ide_Ctlr_Info [0].pwrmInfo.ioSuspend  )
		{
			Ide_Ctlr_Info [0].pwrmInfo.ioSuspend = 0;
			switch (event)
			{
				case PWRM_GOINGTOSTANDBY :
				case PWRM_GOINGTOSLEEP :
				case PWRM_GOINGTODEEPSLEEP :
					/* perform the controller power down operation */
					/* put the IDE controller in power down mode */
					if( /*(event == PWRM_GOINGTOSLEEP) ||*/
						(event == PWRM_GOINGTODEEPSLEEP) )
					{
						ATA_IDE_PowerDown((Ptr)getIdeCtrl(0),ATA_DEV_SLEEP);
					}
					else
					{
						ATA_IDE_PowerDown((Ptr)getIdeCtrl(0), ATA_DEV_IDLE); 
					}

					pm_sata_power_off();
					break;

				case PWRM_DONE_CPU_SETPOINTCHANGE :
				case PWRM_DONE_PER_SETPOINTCHANGE :
					ATA_IDE_ResumeIO(&Ide_Ctlr_Info [0]);	
					break;

				case PWRM_AWAKEFROMSTANDBY :
				case PWRM_AWAKEFROMSLEEP :
				case PWRM_AWAKEFROMDEEPSLEEP :
					break;

				default :
					break;
			}
			if ( i < ATA_MAX_PWRM_EVENTS )
			{
	 			if (Ide_Ctlr_Info [0].pwrmInfo.event[i].delayedCompletionFxn())
					Ide_Ctlr_Info [0].pwrmInfo.event[i].delayedCompletionFxn();
			}
		}
	}	
}
/**
 *	\brief	pwrm_ata_notifyFxn
 *			notification function invoked by pwrm module
 *			on power events
 *
 *	\param	PWRM_Event	[IN]	eventType 
 *								1.	PWRM_PENDING_CPU_SETPOINTCHANGE
 *								PWRM_DONE_CPU_SETPOINTCHANGE
 *								2.	PWRM_PENDING_PER_SETPOINTCHANGE
 *								PWRM_DONE_PER_SETPOINTCHANGE
 *								3.	PWRM_GOINGTOSTANDBY
 *								PWRM_AWAKEFROMSTANDBY
 *								4.	PWRM_GOINGTOSLEEP
 *								PWRM_AWAKEFROMSLEEP
 *								5.	PWRM_GOINGTODEEPSLEEP
 *								PWRM_AWAKEFROMDEEPSLEEP
 *			Arg			[IN]	eventArg1	cur setpoint
 *			Arg			[IN]	eventArg2	new setpoint to be changed
 *			Arg			[IN]	clientArg
 *
 *	\return	PWRM_NOTIFYDONE on sucess else PWRM_NOTIFYNOTDONE
 */
PWRM_NotifyResponse pwrm_ata_notifyFxn(PWRM_Event eventType, Arg eventArg1, 
    Arg eventArg2, Arg clientArg)
{
	PWRM_NotifyResponse retVal = PWRM_NOTIFYDONE;
	ATA_PWRM_Info *pAtaPwrm;

	PWRM_SATA_TRACE(0x10000000);
	pAtaPwrm = (ATA_PWRM_Info *)ATA_getPwrmHandle((Ptr)clientArg);

	/* compiler remark : variable never referenced */
	if (eventArg1 || eventArg2)
	{
	}

	/* handle the pwrm events */
	switch ( eventType )
	{
		case PWRM_PENDING_CPU_SETPOINTCHANGE:
		case PWRM_PENDING_PER_SETPOINTCHANGE :

			if( ((eventType == PWRM_PENDING_CPU_SETPOINTCHANGE) &&
				(pAtaPwrm->pll_domain == PWRM_CPU)) ||
				((eventType == PWRM_PENDING_PER_SETPOINTCHANGE) &&
				(pAtaPwrm->pll_domain == PWRM_PER)) )
			{
				retVal = ataPwrm_suspend_Iops((Ptr)clientArg, eventType);
				if (retVal != PWRM_NOTIFYDONE)
				{
					timer3_start();
				}

			}
			PWRM_SATA_TRACE(0x1);
			break;
		case PWRM_DONE_CPU_SETPOINTCHANGE :
		case PWRM_DONE_PER_SETPOINTCHANGE :
			if( ((eventType == PWRM_DONE_CPU_SETPOINTCHANGE) &&
				(pAtaPwrm->pll_domain == PWRM_CPU)) ||
				((eventType == PWRM_DONE_PER_SETPOINTCHANGE) &&
				(pAtaPwrm->pll_domain == PWRM_PER)) )
			{
				retVal = ataPwrm_resume_Iops((Ptr)clientArg, eventType);
				if (retVal != PWRM_NOTIFYDONE)
				{
					timer3_start();
				}
			}
			PWRM_SATA_TRACE(0x2);
			break;
		case PWRM_GOINGTOSTANDBY :
		case PWRM_GOINGTOSLEEP :
		case PWRM_GOINGTODEEPSLEEP :

			PWRM_SATA_TRACE(0x4000000);

			/* Suspend the IO operation */
			retVal = ataPwrm_suspend_Iops((Ptr)clientArg, eventType);

			if (retVal != PWRM_NOTIFYDONE)
			{
				timer3_start();
				PWRM_SATA_TRACE(0x10000000);
			}
			else
			{
				/* perform the controller power down operation */
				/* put the IDE controller in power down mode */
				if( (eventType == PWRM_GOINGTODEEPSLEEP) )
				{
					ATA_IDE_PowerDown((Ptr)clientArg, ATA_DEV_SLEEP);
				}
				else
				{
					ATA_IDE_PowerDown((Ptr)clientArg, ATA_DEV_IDLE); 
				}
			}

			/* cut off the IP clock */
			if (retVal == PWRM_NOTIFYDONE)
				pm_sata_power_off();

			PWRM_SATA_TRACE(0x4);
			break;

		case PWRM_AWAKEFROMSTANDBY :
		case PWRM_AWAKEFROMSLEEP :
		case PWRM_AWAKEFROMDEEPSLEEP :

			PWRM_SATA_TRACE(0x8000000);
			/* resume the controller from power down state to powered state
			   and device to operational state */
			/* switch on the IP clocks */
			pm_sata_power_on();

			/* perfrom port reset */
			if( (eventType == PWRM_AWAKEFROMDEEPSLEEP) )
			{
				ahciReset(getAhciHandle(0), 0, AHCI_PORT_RESET);
				pwrm_sata_trace |= 0x1000;
			}

			/* put the IDE controller in power up mode */
			ATA_IDE_PowerUp((Ptr)clientArg);

			/* resume the operation */
			retVal = ataPwrm_resume_Iops((Ptr)clientArg, eventType);

			if (retVal != PWRM_NOTIFYDONE)
			{
				timer3_start();
				PWRM_SATA_TRACE(0x20000000);
			}

			PWRM_SATA_TRACE(0x8);
			break;
		default :
			break;
	}
    return(retVal);    /* signal notification complete */
}

/**
 *	\brief	pm_ata_GetPwrmInfo
 *			get pwrm information 
 *
 *	\param	DevHandle	[IN]	Device handle
 *			pAtaPwrm 	[IN]	PWRM Information structure
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
Int32 pm_ata_PwrmRegisterEvents(Ptr DevHandle, ATA_PWRM_Info *pAtaPwrm)
{
	Uint32 i;
	Int32 retVal = IOM_COMPLETED;

	for (i = 0; i < ATA_MAX_PWRM_EVENTS; ++i)
	{
		pAtaPwrm->event[i].notifyHandle = NULL;
		pAtaPwrm->event[i].delayedCompletionFxn = NULL;
	    PWRM_registerNotify((PWRM_Event)i, ALLSETPOINTSALLOWED, 
	        (Fxn)(Fxn)pwrm_ata_notifyFxn, (Arg)DevHandle, 
	        &pAtaPwrm->event[i].notifyHandle, 
	        (Fxn *) &pAtaPwrm->event[i].delayedCompletionFxn);  
	}

	return retVal;
}

/**
 *	\brief	pm_ata_PwrmRegisterConstraints
 *		 	register ata pwrm constraints
 *
 *	\param	pAtaPwrm	[IN]	PWRM Information structure
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
Int32 pm_ata_PwrmRegisterConstraints(ATA_PWRM_Info *pAtaPwrm)
{
	Int32 retVal = IOM_COMPLETED;

	/* register constrainsts */
	if (pAtaPwrm->pll_domain == PWRM_PER)
	{
		pAtaPwrm->constraint = PWRM_DISALLOWED_PER_SETPOINT_MASK;
		PWRM_registerConstraint(pAtaPwrm->constraint, 0, 
					&pAtaPwrm->constraint_handle);
	}

	return retVal;
}
/**
 *	\brief	pm_ata_PwrmUnRegisterConstraints
 *		 	unregister ata pwrm constraints
 *
 *	\param	pAtaPwrm	[IN]	PWRM Information structure
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
Int32 pm_ata_PwrmUnRegisterConstraints(ATA_PWRM_Info *pAtaPwrm)
{
	Int32 retVal = IOM_COMPLETED;

	/* register constrainsts */
	if (pAtaPwrm->pll_domain == PWRM_PER)
	{
		pAtaPwrm->constraint = PWRM_DISALLOWED_PER_SETPOINT_MASK;
		if ( pAtaPwrm->constraint_handle )
		{
			PWRM_unregisterConstraint(&pAtaPwrm->constraint_handle);
		}
	}

	return retVal;
}

/**
 *	\brief	pm_ata_PwrmTest
 *		    pwrm dvfs test 
 *
 *	\param	pAtaPwrm	[IN]	PWRM Information structure
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
Int32 pm_ata_PwrmTest(ATA_PWRM_Info *pAtaPwrm)
{
	Int32 retVal = IOM_COMPLETED;
#ifdef TEST_PWRM_DVFS
	Uint32 i;
#endif

	if (pAtaPwrm != NULL)
	{
#ifdef TEST_PWRM_DVFS
	    /*  Step down through setpoints, from highest to lowest */
	    for (i = 0; i < pAtaPwrm->numSetPoints ; i++) 
	    {
	        status = PWRM_changeSetpoint(pAtaPwrm->pll_domain, i, TIMEOUT);
	        if (status == PWRM_SOK) 
	        {
			    status = PWRM_getCurrentSetpoint(pAtaPwrm->pll_domain,
	    					&pAtaPwrm->curSetPoint);
	            ATA_DEBUG1( "New setpoint=%d", i);
	        }
	        else 
			{
	            ATA_DEBUG1( "Error: PWRM_changeSetpoint status=%x", status);
				retVal = IOM_EBADIO;
				break;
	        }
	    }
#endif
	}
	return retVal;
}

/**
 *	\brief	pm_ata_GetPwrmInfo
 *			get pwrm information 
 *
 *	\param	pAtaPwrm		[IN]	PWRM Information structure
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
Int32 pm_ata_GetPwrmInfo(ATA_PWRM_Info *pAtaPwrm)
{
	PWRM_Status status;
	Int32 i,k;
	Int32 retVal = IOM_COMPLETED;
    Uint32 numSetpoints = 0, frequency, voltage, done = 1;

	do
	{

	    /* get the number of setpoints supported on this platform */
	    status = PWRM_getNumSetpoints(pAtaPwrm->pll_domain, &numSetpoints);
	    ATA_DEBUG1("PWRM_getNumSetpoints returned num=%d\n", numSetpoints);
	    if (status != PWRM_SOK) 
	    {   
	        ATA_DEBUG1( "Error: PWRM_getNumSetpoints status=%x\n", status);
			retVal = IOM_EBADIO;
			break;
	    }

		for (k=0, i=0; i<numSetpoints; ++i)
		{
			/* get the setpoint information */
			status = PWRM_getSetpointInfo(pAtaPwrm->pll_domain,i,
						&frequency, 
						&voltage);
			if( status == PWRM_SOK && (k < MAX_SETPOINTS) )
			{
				pAtaPwrm->setPoint[k].frequency = frequency;
				pAtaPwrm->setPoint[k].voltage = voltage;
				k++;
			}
		}
		pAtaPwrm->numSetPoints = numSetpoints;

	     /* Get the current setpoint */
	    status = PWRM_getCurrentSetpoint(pAtaPwrm->pll_domain,
	    					&pAtaPwrm->curSetPoint);
	    ATA_DEBUG1( "PWRM_getCurrentSetpoint returned=%d\n", pAtaPwrm->curSetPoint);
	    if (status != PWRM_SOK) 
	    {   
	        ATA_DEBUG1( "Error: PWRM_getCurrentSetpoint status=%x\n", status);
			retVal = IOM_EBADIO;
			break;
	    }

	} while (!done);

	return retVal;
}

/**
 *	\brief	pm_ata_pwrm_init
 *			perfrom registeration with pwrm module
 *
 *	\param	DevHandle		[IN]	device handle
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
Int32 pm_ata_pwrm_init(Ptr DevHandle)
{
	Int32 retVal = IOM_COMPLETED;
	ATA_PWRM_Info *pAtaPwrm;
	AhciHostCtrl *pAhci_host;
	PWRM_Domain power_domain;
	Uint32 done = 1;

	pAtaPwrm = (ATA_PWRM_Info *)ATA_getPwrmHandle(DevHandle);
	pAhci_host = getAhciHandle(0);

	do
	{
		/* check pwrm module is already initialized */
		if( pAtaPwrm->registered )
			return retVal;

		/* save the power domain either from (PLL0: PWRM_CPU
		   or PLL1:PWRM_PER) */
		power_domain = pAhci_host->plat_res->pll_domain;

		pAtaPwrm->pll_domain = power_domain; 

		/* support all the PWRM events */
		pAtaPwrm->pwrm_mode = (PWRM_STANDBY | PWRM_SLEEP |
	 							 PWRM_DEEPSLEEP|PWRM_CVFSCALING);

		/* Get the pwrm information */
		pm_ata_GetPwrmInfo(pAtaPwrm);

		/* register the pwrm events */
		pm_ata_PwrmRegisterEvents(DevHandle, pAtaPwrm);

		/* Register ATA PWRM Constraints */
		pm_ata_PwrmRegisterConstraints(pAtaPwrm);

		/* ata pwrm standalone_test */
		pm_ata_PwrmTest(pAtaPwrm);

		/* ata pwrm init completed */
		pAtaPwrm->registered = 1;

	} while (!done);

	return (retVal);
}

/**
 *	\brief	pm_ata_pwrm_deinit
 *			perfrom de-init with pwrm module
 *
 *	\param	ATA_PWRM_Info	[IN] pointer to pwrm Info
 *
 *	\return	IOM_COMPLETED on sucess else error
 */
void pm_ata_pwrm_deinit(ATA_PWRM_Info *pAtaPwrm)
{
	Uint32 i;

	for (i=0; i<ATA_MAX_PWRM_EVENTS; ++i)
	{
		/* unregister all notification with pwrm */
    	PWRM_unregisterNotify(pAtaPwrm->event[i].notifyHandle);
	}
	/* unregister pwrm constraints */
	pm_ata_PwrmUnRegisterConstraints(pAtaPwrm);
}
#endif

