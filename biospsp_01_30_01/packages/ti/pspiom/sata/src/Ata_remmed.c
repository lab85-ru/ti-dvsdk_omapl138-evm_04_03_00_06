/*
 * Ata_remmed.c
 *
 * ATA Removable Media Driver file
 * This file implements the ATA based removable media support for ATA/ATAPI devices.
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
  *
  *	\author     Swaminathan S.(PSP-USB)
  *	\version    4.5
  */
/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|						PC-Lint	8.00q Options	     |**
**+--------------------------------------------------------------------------+*/
/******************************************************************************/
/* lint -e740		//unusual pointer cast (incompatible indirect types)      */
/* lint -e611		//suspicious cast                                         */
/* lint -e631            //tag 'xxx' defined differently in files             */
/* lint -e534            //ignoring return values                             */
/*+--------------------------------------------------------------------------+**
*******************************************************************************/

#include <std.h>
#include <iom.h>
#include <hwi.h>
#include <tsk.h>
#include <sys.h>
#include <prd.h>
#include <bcache.h>
#include <string.h>
#include <assert.h> 

#include "tistdtypes.h"

#include "ata.h"
#include "ata_cmd.h"
#include "psp_ata_med.h"
#include "psp_atapi_med.h"
#include "psp_ata_debug.h"
#include "psp_ata_pwrm.h"

#define True	1
#define False	0
#define PAL_SOK		0



static void ATA_RemMedStat (ATA_Drive_t *pDevice, Bool IsStatus);
void ATA_MED_RemIdtfy (ATA_Drive_t *pDevice, Bool IsRemMedStatus);
static Bool ATA_RemMedHndlr (void const * Handle, ATA_Dev_Stat_t const *pStatus);
static void ATA_DisRemMedStat (ATA_Drive_t *pDevice);
//static void ATA_RemMedTimeout (PAL_osTimerCallbackParams const *cbParams);
static void ATA_RemMedTimeout (void *cbParams);
static void ATA_EnbRemMedStat (ATA_Drive_t *pDevice);

extern void ATA_SynchStatus (ATA_IDE_Int_t *pIdeInt, ATA_Dev_Stat_t *pStatus, Uint32 Timeout);
extern ATA_Dev_Req_t* ATA_RelReq (ATA_Drive_t *pDrive, ATA_Dev_Req_t *pReq);
extern Int32 ATA_MED_SetPwrMode (Ptr Handle, Ptr MHndl, ATA_DEV_PwrMode_t Mode);

//PAL_osTimerHandle ATA_RemMedTo;
TIMER_OBJ *ATA_RemMedTo;
Uint32 ataRemMedTimeoutData;

TIMER_OBJ timer1;
extern PRD_Obj	PRD1; /* PRD timer1 */

/**
 *  \brief sataTimer1_task () : 
 *                               
 *  \param void 
 *  \return none                : None
 *
 */
void sataTimer1_task(void)
{
	if( timer1.fn )
		timer1.fn(timer1.param);
}

/**
 *  \brief timer1_start () : 
 *                               
 *  \param Uint32					: timeout value 
 *  \return none                : None
 */

void timer1_start(Uint32 timeout)
{
	PRD1.period = timeout;
	PRD_start( &PRD1);
}
/**
 *  \brief timer1_stop () : 
 *                               
 *  \param void 
 *  \return none                : None
 *
 */
void timer1_stop(void)
{
	PRD_stop( &PRD1);
}
/**< ATA Media Status Timeout Id */

/**
 *  \brief ATA_MED_RemIdtfy ()  : Populate the removable media characteristics.
 *  \param pDevice      [IN]    : Device Pointer.
 *  \param IsRemMedStatus [IN]  : Get Media Status ???
 *  \return void
 ******************************************************************************/
void ATA_MED_RemIdtfy (ATA_Drive_t *pDevice, Bool IsRemMedStatus)
{
    ATA_Dev_Cap_t *pCap;
    ATA_Dev_Stat_t Status;

	if(NULL != pDevice)
	{
	    pCap = &pDevice -> Cap;

	    if (FALSE != IsRemMedStatus)
	    {
	        ATA_EnbRemMedStat (pDevice);
	    }
	    else
	    {
	        ATA_DisRemMedStat (pDevice);
	    }

	    /*
	     * Issue Get Media Status command to verify whether it is supported.
	     * If supported then arm the timer for media status detection.
	     */
	    ATA_RemMedStat (pDevice, IsRemMedStatus);

	    /* NU_Control_Timer(&(pDevice -> pIdeInt -> IdeTimer), NU_DISABLE_TIMER); */
	   // PAL_osTimerStop (pDevice -> pIdeInt ->IdeTimeout);
	    timer1_stop();

	    ATA_SynchStatus (pDevice -> pIdeInt, &Status, ATA_CMD_TO);

	    pDevice -> pIdeInt -> BusState = ATA_IDE_FREE;

	    ATA_RelReq (pDevice, pDevice -> pReq);

	    if (FALSE == (Status . Error & ATA_DEV_ERR_ABRT))
	    {
	        /* Need PAL OS interface here and move this alarm structure into the
	         * IDE controller structure
	         */
	        ataRemMedTimeoutData = 2u;
			timer1.mode = TIMER_MODE_ONESHOT;
			timer1.timeout = ataRemMedTimeoutData;
			timer1.fn = &ATA_RemMedTimeout;
			timer1.param = (void *)pDevice;	
			timer1.start = 0;
			timer1.init = 1;
			timer1.mode = 0;
			timer1.id = 2;
			ATA_RemMedTo = &timer1;		

	        if (FALSE != IsRemMedStatus)
	        {
	            /* Removable Media Status notification supported */
	            pCap -> RemMedStatus = TRUE;
	        }
	        else
	        {
	            /* Removable Media Lock supported */
	            pCap -> RemMedStatus = FALSE;
	        }

	        /* NU_Control_Timer(&ATA_RemMedTo, NU_ENABLE_TIMER); */
	        //result = PAL_osTimerStart (ATA_RemMedTo);
			timer1_start(timer1.timeout );
	    }
	    else
	    {
	        /* Removable Media Status notification not supported */
	        pCap -> RemMedStatus = FALSE;

	        if ((((pDevice -> Cap . IsAta==False)) && (IsRemMedStatus==True)) ||
	                ((pDevice -> Cap . IsAta==True) && (IsRemMedStatus==False)))
	        {
	            /* This device does not support the Get Media Status/ Media Lock.
	             * So ATA Driver cannot support this device w.r.t Media Status info.
	             * It has to be managed by the ATAPI later or in the Media Driver.
	             * or it has to be managed as error in IO path.
	             */
	            pCap -> IsMedRem = FALSE;
	        }
	    }
	}
}

/**
 *  \brief ATA_EnbRemMedStat ()  : Enable Media Status Feature set.
 *  \param pDevice      [IN]     : Device Pointer.
 *  \return void
 ******************************************************************************/
static void ATA_EnbRemMedStat (ATA_Drive_t *pDevice)
{
    ATA_Dev_Req_t *pReq;
    ATA_Dev_Stat_t Status;

	if(NULL != pDevice)
	{
	    pReq = ATA_AllocReq (pDevice, FALSE);

	    if (pReq == NULL)
	    {
	       return;
	    }

	    pReq -> Buffer = NULL;
	    pReq -> Command . Device = pDevice -> Dev;
	    pReq -> Command . Cmd = ATA_SET_FEATURES;
	    pReq -> Command . Features = ATA_SET_ENBREMSTATUS;
	    pReq -> TransferCount = 0;
	    pReq -> BlockCount = 0;
	    pReq -> Mode = TASKFILE;
	    pReq -> Handler = NULL;
	    pReq -> Timeout = 0;

	    ATA_SubmitReq (pDevice, pReq);
	    ATA_SynchStatus (pDevice -> pIdeInt, &Status, ATA_CMD_TO);

	    ATA_RelReq (pDevice, pReq);
	    /* Set the bus status to free */
	    pDevice -> pIdeInt -> BusState = ATA_IDE_FREE;
	}

}

/**
 *  \brief ATA_DisRemMedStat ()  : Disable Media Status Feature set.
 *  \param pDevice      [IN]     : Device Pointer.
 *  \return void
 ******************************************************************************/
static void ATA_DisRemMedStat (ATA_Drive_t *pDevice)
{
    ATA_Dev_Req_t *pReq;
    ATA_Dev_Stat_t Status;

	if(NULL != pDevice)
	{
	    pReq = ATA_AllocReq (pDevice, FALSE);

	    if (pReq == NULL)
	    {
	       return;
	    }

	    pReq -> Buffer = NULL;
	    pReq -> Command . Device = pDevice -> Dev;
	    pReq -> Command . Cmd = ATA_SET_FEATURES;
	    pReq -> Command . Features = ATA_SET_DISREMSTATUS;
	    pReq -> TransferCount = 0;
	    pReq -> BlockCount = 0;
	    pReq -> Mode = TASKFILE;
	    pReq -> Handler = NULL;
	    pReq -> Timeout = 0;

	    ATA_SubmitReq (pDevice, pReq);

	    ATA_SynchStatus (pDevice -> pIdeInt, &Status, ATA_CMD_TO);

	    ATA_RelReq (pDevice, pReq);
	    /* Set the bus status to free */
	    pDevice -> pIdeInt -> BusState = ATA_IDE_FREE;
	}

}

/**
 *  \brief ATA_RemMedStat ()    : Submit the removable media status command.
 *  \param pDevice      [IN]    : Device Pointer.
 *  \param IsStatus     [IN]    : Get Media Status command / Media Lock
 *  \return void
 ******************************************************************************/
static void ATA_RemMedStat (ATA_Drive_t *pDevice, Bool IsStatus)
{
    ATA_Dev_Req_t *pReq;

	if(NULL != pDevice)
	{

	    pReq = ATA_AllocReq (pDevice, FALSE);

	    if (pReq == NULL)
	    {
	       return;
	    }

	    pReq -> Buffer = NULL;
	    pReq -> Command . Device = pDevice -> Dev;

	    if (FALSE != IsStatus)
	    {
	        pReq -> Command . Cmd = ATA_MEDIA_STATUS;
	    }
	    else
	    {
	        pReq -> Command . Cmd = ATA_MEDIA_LOCK;
	    }

	    pReq -> TransferCount = 0;
	    pReq -> BlockCount = 0;
	    pReq -> Mode = TASKFILE;
	    pReq -> ToHndlr = &ATA_RemMedHndlr;
	    pReq -> Retry = 10000u;
	    pReq -> Timeout = 1u;

	    ATA_SubmitReq (pDevice, pReq);
	}
}

/**
 *  \brief ATA_RemMedTimeout ()  : Removable Media Enquiry.  Executes in Interrupt context.
 *  \return void            :
 ******************************************************************************/
//static void ATA_RemMedTimeout (PAL_osTimerCallbackParams const *cbParams)
static void ATA_RemMedTimeout (void *cbParams)
{
    ATA_Drive_t *pDevice;
    Uint8 Index = 0;
    Uint8 Device = 0;

	if (cbParams != NULL)
	{
	}

    while (Ide_Ctlr_Info [Index] . pIdeOps != NULL)
    {
        for (Device = 0; Device < ATA_MAX_INT_DEV; Device++)
        {
            pDevice = &Ide_Ctlr_Info [Index] . Drive [Device];

            if ((((((pDevice -> Probe == ATA_DEV_REG) &&
                    ((pDevice -> pIdeInt -> BusState != ATA_IDE_TO)) ||
                     (pDevice -> pIdeInt -> BusState != ATA_IDE_RESET))) &&
                    (pDevice -> PwrMode != ATA_DEV_SLEEP)) &&
                    ((pDevice -> Cap . IsMedRem != FALSE))) &&
                    (pDevice -> IsMedStatComp == TRUE))
            {
                pDevice -> IsMedStatComp = FALSE;
                ATA_RemMedStat (pDevice, pDevice -> Cap . RemMedStatus);
            }
        }

        Index++;
    }
}

/**
 *  \brief ATA_RemMedHndlr ()   : Removable Media status Handler.
 *                                Executes in Interrupt context.
 *  \param Handle [IN]          : Handle to the Device.
 *  \param Status [IN]          : Command status
 *  \return Bool                : FALSE/TRUE
 ******************************************************************************/
static Bool ATA_RemMedHndlr (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
    ATA_Drive_t *pDevice = NULL;
    ATA_DEV_Res_t Res;
    Bool Err = FALSE, StartTimer = TRUE, Event = FALSE;
    Bool SetDevops = FALSE;
//    Uint32 Timeout = 0;
    ATA_DEV_PwrMode_t Mode;

	if((NULL != Handle) && (NULL != pStatus))
	{
		pDevice = (ATA_Drive_t *)(Uint32)Handle;

	    if (FALSE != (pStatus -> Stat & ATA_DEV_ERR))
	    {
	        if (((pStatus -> Error & ATA_DEV_ERR_WP)!=FALSE) &&
	                (FALSE == (pDevice -> Cap . IsMedWp)))
	        {
	            pDevice -> Cap . IsMedWp = TRUE;
	            Res . ReqInfo . Event . EventId = ATA_DEV_EVT_MED_WP;
	            SetDevops = TRUE;
	            Event = TRUE;
	        }

	        if (FALSE != (pStatus -> Error & ATA_DEV_ERR_MCR))
	        {
	            Res . ReqInfo . Event . EventId = ATA_DEV_EVT_MED_CHREQ;
	            Event = TRUE;
	        }

	        if (FALSE != (pStatus -> Error & ATA_DEV_ERR_MC))
	        {
	            Res . ReqInfo . Event . EventId = ATA_DEV_EVT_MED_CHANGE;
	            Event = TRUE;
	        }

	        if (((pStatus -> Error & ATA_DEV_ERR_NM)!=FALSE) &&
	                (FALSE == (pDevice -> Cap . IsNoMed)))
	        {
	            pDevice -> Cap . IsNoMed = TRUE;
	            Res . ReqInfo . Event . EventId = ATA_DEV_EVT_MED_NOMED;
	            SetDevops = TRUE;
	            Event = TRUE;
	        }
	    }

	    if (FALSE == (pStatus -> Stat & ATA_DEV_BUSY))
	    {
	        if (FALSE != (pDevice -> Cap . IsMedWp))
	        {
	            pDevice -> Cap . IsMedWp = FALSE;
	            Res . ReqInfo . Event . EventId = ATA_DEV_EVT_MED_WRITE;
	            /* Set the Read and Write entry points to active IO paths as the Media
	             * is available or the Write Protect flag has been removed.
	             */
	            SetDevops = TRUE;
	            Event = TRUE;
	        }

	        if (FALSE != (pDevice -> Cap . IsNoMed))
	        {
	            pDevice -> Cap . IsNoMed = FALSE;
	            Res . ReqInfo . Event . EventId = ATA_DEV_EVT_MED_AVAIL;
	            /* Set the Read and Write entry points to active IO paths as the Media
	             * is available or the Write Protect flag has been removed.
	             */
	            SetDevops = TRUE;
	            Event = TRUE;
	        }
	    }
	    else
	    {
	        if ((--pDevice -> pReq -> Retry) != 0)
	        {
	            /* Start the command timeout */
				timer1_stop();
				timer1.mode = TIMER_MODE_ONESHOT;
				timer1.timeout = pDevice -> pReq -> Timeout;
				timer1.fn = &ATA_DEV_CmdTimeout;
				timer1.param = (void *)NULL;	
				timer1_start(timer1.timeout );
	        }
	        else
	        {
	            Err = TRUE;
	        }

	        StartTimer = FALSE;
	    }

	    if (FALSE != StartTimer)
	    {
	        if (FALSE != SetDevops)
	        {
	            ATA_SetDevops (pDevice);
	        }

	        if (FALSE != Event)
	        {
	            /* Send a event to the Media Driver on Media status */
	            Res . IsEvent = TRUE;
	            pDevice -> MedHandler (NULL, &Res);
	        }

	        /* Check for the device power state to be IDLE/STDBY.  If so the
	         * device has transitioned into ACTIVE now so set the power mode to
	         * original power state
	         */
	        Mode = pDevice -> PwrMode;

	        if (Mode != ATA_DEV_ACTIVE)
	        {
	            pDevice -> PwrMode = ATA_DEV_ACTIVE;
	            ATA_MED_SetPwrMode ((Ptr)pDevice, NULL, Mode);
	        }

	        /* Set the flag for the removable media status enquiry
	         * command complete.  This is used to fire the next Media status enquiry
	         * on this device.
	         */
	        pDevice -> IsMedStatComp = TRUE;

	        /* If a RemMedTo is running stop it and re start it so that the status
	         * can be fired on all devices
	         */
	        /* NU_Control_Timer(&ATA_RemMedTo, NU_DISABLE_TIMER); */
			timer1_stop();

	        ATA_SchedNextReq (pDevice -> pIdeInt, pDevice -> pReq);

	        /* Re-arm the timer for the ATA_REMMED_TO timeout */
#if 0
	        if (Mode == ATA_DEV_ACTIVE)
	        {
	            Timeout = ATA_REMMED_TO;
	        }
	        else
	        {
	            Timeout = ATA_REMMEDPWR_TO;
	        }
#endif
			timer1.mode = TIMER_MODE_ONESHOT;
			timer1.timeout = pDevice -> pReq -> Timeout;
			timer1.fn = &ATA_DEV_CmdTimeout;
			timer1.param = (void *)NULL;
			timer1_start(timer1.timeout );
	    }
	}

    return Err;
}

