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

#ifdef CHIP_C6748
#ifdef BIOSATA_PSC_ENABLE
#include <ti/pspiom/psc/Psc.h>
#include <ti/pspiom/cslr/cslr_psc_C6748.h>
#endif

#include <ti/pspiom/cslr/soc_C6748.h>
#ifndef BIOS_PWRM_ENABLE
#error "ATA Driver: Enable BIOS_PWRM_ENABLE option for C6748 platform"
#endif
#include "pwrm.h"
#include "pwrm6748.h"
#else
#ifdef CHIP_OMAPL138
#ifdef BIOSATA_PSC_ENABLE
#include <ti/pspiom/psc/Psc.h>
#include <ti/pspiom/cslr/cslr_psc_OMAPL138.h>
#endif
#include <ti/pspiom/cslr/soc_OMAPL138.h>
#ifndef BIOS_PWRM_ENABLE
#error "ATA Driver: Enable BIOS_PWRM_ENABLE option for OMAPL138 platform"
#endif
#include "pwrm.h"
#include "pwrmL138.h"
#endif
#endif

#ifdef BIOS_PWRM_ENABLE

#ifndef _ATA_PWRM_H
#define _ATA_PWRM_H




#define	ATA_MAX_PWRM_EVENTS		(PWRM_INVALIDEVENT)	

#define MAX_SETPOINTS           4      			/* # setpoints to reserve space for */
#define TIMEOUT                 10      		/* timeout (ticks) for notifications */
#define ALLSETPOINTSALLOWED     0xFFFF  		/* client allows all setpoints */
#define SOMESETPOINTSALLOWED    0xFFF0  		/* client doesn't allow lowest 4 SP */


typedef struct {
	Fxn 					delayedCompletionFxn;	/* delay pwrm completion called by ata driver */
	PWRM_NotifyHandle 		notifyHandle; 			/* handle for unregister */
	PWRM_ConstraintHandle	constraintHandle;		/* constraint handle */
}ATA_PWRM_Event;

typedef struct {
	PWRM_Domain 		pll_domain;						/* Power Domain */
	Uint32 				pwrm_mode;
	Bool				ioSuspend;					/* ATA Driver is in suspend mode */
	PWRM_Event			pwrmEvent;					/* current pwrm event */
	Uint32				numSetPoints;				/* max setpoints for DVSF */
	Uint32  			curSetPoint;				/* current setpoint */
	PWRM_SetpointInfo 	setPoint[MAX_SETPOINTS]; 	/* setPoint Info */
	ATA_PWRM_Event 		event[ATA_MAX_PWRM_EVENTS];	/* Event Details !!!change it to PWRM_INVALIDEVENT */
	PWRM_Constraint 	constraint;					/* constraint */
	PWRM_ConstraintHandle constraint_handle;		/* constraint handle */
	Uint32				registered;
}ATA_PWRM_Info;

#endif
#endif
