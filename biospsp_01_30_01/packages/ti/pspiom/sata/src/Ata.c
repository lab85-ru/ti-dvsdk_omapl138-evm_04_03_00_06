/*
 * Ata.c
 *
 * ATA Interface Driver file
 * This file implements the ATA interface driver functions.
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
 *	\author     Swaminathan S.(PSP-USB)
 *  \version    10.0
 */
/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|            PC-Lint  8.00q Options       |**
**+--------------------------------------------------------------------------+*/
/******************************************************************************/
/* lint -e522       //Expected void type, assignment, increment or decrement  */
/* lint -e611       //suspicious cast                                         */
/* lint -e631       //tag 'xxx' defined differently in files                  */
/* lint -e715       //symbol not referenced                                   */
/* lint -e717       //do ... while(0);                                        */
/* lint -e740       //unusual pointer cast (incompatible indirect types)      */
/* lint -e818       //Pointer parameter could be declared as pointing to const*/
/* lint -e534       //ignoring return values of functions                     */
/* lint -e826       //suspicious pointer-to-pointer conversion                */
/* lint -e734       //Loss of precision (assignment)                          */
/* lint -e506       //Constant value Boolean                                  */
/* lint -e788       //enum constant " " not used                              */
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
#include "psp_ata_debug.h"

#include "tistdtypes.h"
#include "ata.h"

#define CONFIG_SATA

#ifdef ATA_ATAPI_SUPPORT
#include "atapi.h"
#include "psp_atapi_med.h"
#endif
#include "ata_cmd.h"
#include "psp_ata_med.h"

#include "ahci.h"
#include "sata_pm.h"
#include "psp_ata_pwrm.h"
#include <ti/pspiom/blkmedia/psp_blkdev.h>

#define True	1
#define False	0
#define PAL_SOK		0

#ifdef ATA_REMOVABLE_MEDIA_SUPPORT
extern void ATA_MED_RemIdtfy (ATA_Drive_t *, Bool);
#endif
extern Int32 AhciExecPolledCmd(Ptr Handle, ATA_Dev_Req_t *pReq);

Bool ATA_ResumeIOReq (ATA_Drive_t * const pDevice);

Int32 ATA_IDE_Register (IDE_INTF_Confg_t const * const pIdeInt_config, void const * const pHandle);
static Int32 ATA_IDE_Reset (Ptr Handle); 
static Bool ATA_ResetProgress (void const * Handle, ATA_Dev_Stat_t const *pStatus);
static Bool ATA_DevResEnd (void const * Handle, ATA_Dev_Stat_t const *pStatus);
static Bool ATA_ResetSched (void const * Handle, ATA_Dev_Stat_t const *pStatus);
static Bool ATA_IdeResEnd (void const * Handle, ATA_Dev_Stat_t const *pStatus);

static Int32 ATA_MED_ReadMul (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector);
static Int32 ATA_MED_Void (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector);
static Int32 ATA_MED_ReadPwr (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector);
static Int32 ATA_MED_WritePwr (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector);
static Int32 ATA_MED_WriteMul (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector);
static Int32 ATA_MED_ReadDma (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector);
static Int32 ATA_MED_WriteDma (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector);
//static Int32 ATA_MED_Read (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector);
//static Int32 ATA_MED_Write (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector);
static Int32 ATA_MED_GetMaxSect (Ptr Handle);
static Int32 ATA_MED_GetMaxSectVoid (Ptr Handle);
static void ATA_MED_IntHandler (Uint32 IntNum, ATA_Dev_Req_t *pReq);
static Int32 ATA_MED_SetMode (Ptr Handle, ATA_DEV_Mode_t Mode, Uint32 Level);
static Int32 ATA_MED_SetModeVoid (Ptr Handle, ATA_DEV_Mode_t Mode, Uint32 Level);

static Int32 ATA_MED_ModeVoid (Ptr Handle, ATA_DEV_Mode_t Mode, Uint32 Level);
static Int32 ATA_MED_Udma (ATA_Drive_t *pDevice, Uint32 Level);
static Int32 ATA_MED_Dma (ATA_Drive_t *pDevice, Uint32 Level);
static Int32 ATA_MED_Pio (ATA_Drive_t *pDevice, Uint32 Level);
static Int32 ATA_MED_SetDma (ATA_Drive_t *pDevice, Uint32 Level);
static Int32 ATA_MED_SetUdma (ATA_Drive_t *pDevice, Uint32 Level);
static Int32 ATA_MED_SetPio (ATA_Drive_t *pDevice, Uint32 Level);
static void ATA_MED_Identify (ATA_DEV_Ident_t * const pDeviceIdent, ATA_MED_Callback_t MediaCallback,
                       ATA_INT_Ops_t **pAtaIntOps, Ptr * const pHandle, ATA_Drive_t *pDevice);
Int32 ATA_MED_SetPwrMode (Ptr Handle, Ptr MHndl, ATA_DEV_PwrMode_t Mode);
Int32 ATA_IDE_SetPwrMode (Ptr Handle, Ptr MHndl, ATA_DEV_PwrMode_t Mode);
static Int32 ATA_MED_PwrVoid (Ptr Handle, Ptr MHndl, ATA_DEV_PwrMode_t Mode);
static Int32 ATA_MED_SetDevActive (Ptr Handle, ATA_DEV_Mode_t Mode, Uint32 Level);
//static Int32 ATA_MED_Reset (Ptr Handle);
Int32 ATA_IDE_SuspendIO (Ptr Handle);
Int32 ATA_IDE_ResumeIO (Ptr Handle);
Int32 ATA_IDE_PowerDown (Ptr Handle, ATA_DEV_PwrMode_t Mode);
Int32 ATA_IDE_PowerUp (Ptr Handle);
static Int32 ATA_MED_ResetVoid (Ptr Handle);
static Int32 ATA_MED_Ioctl (Ptr Handle, ATA_DEV_IoctlCmd_t Cmd, Ptr * const pData);
static Int32 ATA_MED_IoctlVoid (Ptr AtaHandle, ATA_DEV_IoctlCmd_t Cmd, Ptr * const pData);
static Int32 ATA_MED_ReqAbort (Ptr Handle);

static void ATA_SubmitExtReq (ATA_Drive_t const *pDevice, ATA_Dev_Req_t * const pReq);
static void ATA_SubmitComit (ATA_IDE_Int_t * const pIdeInt, ATA_Dev_Req_t * const pReq);
static Bool ATA_RetryReq (ATA_IDE_Int_t const *pIdeInt, ATA_Dev_Req_t const *pReq);
Bool ATA_SetUdmaHndlr (void const * Handle, ATA_Dev_Stat_t const *pStatus);
Bool ATA_IdentHndlr (void const * Handle, ATA_Dev_Stat_t const *pStatus);
/*static Bool ATA_MaxSect         (Ptr, ATA_Dev_Stat_t *);*/
static Int32 ATA_MaxLba (ATA_Drive_t *pDevice);
static void ATA_VoidDevops (ATA_Drive_t * const pDevice);
static Bool ATA_ReadErr (ATA_Drive_t const *pDevice, ATA_Dev_Stat_t const *pStatus);
static Bool ATA_WriteErr (ATA_Drive_t const *pDevice, ATA_Dev_Stat_t const *pStatus);
static Int32 ATA_PioBeginWrite (ATA_Drive_t const *  pDevice, Uint32 Count);
static void ATA_SubmitIdentify (ATA_Drive_t *pDevice, ATA_Dev_Stat_t * const pStatus, Bool IsAta);
void ATA_SynchStatus (ATA_IDE_Int_t const *pIdeInt, ATA_Dev_Stat_t * const pStatus, Uint32 Timeout);
//static void ATA_PacketDevRes (ATA_Drive_t *pDevice);
static Int32 ATA_MED_GetMediaStatus (Ptr Handle, Ptr Buffer);
static void ATA_MED_ReRegister (ATA_DEV_Ident_t * const pDeviceIdent, ATA_MED_Callback_t MediaCallback,
                       ATA_INT_Ops_t **pAtaIntOps, Ptr * const pHandle, ATA_Drive_t *pDevice);
static Int32 ATA_MED_GetPwrStat (Ptr Handle, Ptr Buffer);
//static Int32 ATA_MED_HardReset (Ptr Handle, Int32 ResetWidth);
//static Bool ATA_ResetToHandler (void const * Handle, ATA_Dev_Stat_t const *pStatus);
static Bool ATA_IdentHndlr1 (void const * Handle, ATA_Dev_Stat_t const *pStatus);

#if 0
static Bool ATA_ResrvReqBlk (ATA_Drive_t *, Uint8);
#endif

  /* This is a void function used otherwise */
Int32  ATAPI_MED_Void  (Ptr Handle, Ptr MedHndl, Ptr pCmd);

/* Following definitions need to be removed to PAL OS Timer functionality */
//void ATA_DEV_CmdTimeout  (PAL_osTimerCallbackParams const *cbParams);
void ATA_DEV_CmdTimeout  (void *cbParams);

ATA_IDE_Int_t Ide_Ctlr_Info [ATA_MAX_IDE_INT];
/**< IDE Controller information */
static ATA_Dev_Req_t ATA_Req [ATA_MAX_DEV_QUEUE * ATA_MAX_IDE_INT * ATA_MAX_INT_DEV];
/**< Request Buffer for the configured number of devices */
static Uint16 DriveIdent [256];
/**< Buffer for device Identification  */
static Uint32 ATA_Pwr_Evt [ATA_DEV_PWRMAX] = {ATA_DEV_EVT_PWR_ACTIVE,
                                       ATA_DEV_EVT_PWR_IDLE,
                                       ATA_DEV_EVT_PWR_STDBY,
                                       ATA_DEV_EVT_PWR_SLEEP,
                                       ATA_DEV_EVT_PWR_SLEEP
                                     };

/**< Power state to Event table */
#ifndef TRUE
#define True TRUE
#define False FALSE
#endif

static Uint32 gBusNotFree =0;
static Uint32 timeoutData = (580u);

TIMER_OBJ timer0;

extern PRD_Obj	PRD0, PRD2, PRD3; /* PRD timer0 */

static void Queue_init (Queue_t *que_elm);
static Uint32  Queue_empty (Queue_t *head);
static void Queue_add_tail (Queue_t *new, Queue_t *head);
static void Queue_del (Queue_t *entry);

extern void pm_sata_prd_task(void);
void ataPwrm_CallbackFn(void);

/**
 *  \brief getIdeCtrl () : 
 *                               
 *  \param 	drive_num		[IN] drive number 
 *  \return none 
 */
Ptr getIdeCtrl(Uint32 drive_num)
{
	return &Ide_Ctlr_Info[drive_num];
}

/**
 *  \brief ataPwrmTask () : 
 *                               
 *  \param 	none
 *  \return none 
 */
void ataPwrmTask(void)
{
#ifdef BIOS_PWRM_ENABLE
	ataPwrm_CallbackFn();
#endif
}
/**
 *  \brief timer3_start () : 
 *                               
 *  \param Uint32					: timeout value 
 *  \return none                : None
 */
void timer3_start(void)
{
	PRD_start( &PRD3);
}
/**
 *  \brief timer0_stop () : 
 *                               
 *  \param void 
 *  \return none                : None
 *
 */
void timer3_stop(void)
{
	PRD_stop( &PRD3);
}


/**
 *  \brief sataTimer0_task () : 
 *                               
 *  \param void 
 *  \return none                : None
 *
 */
void sataTimer0_task(void)
{
	if( timer0.fn )
		timer0.fn(timer0.param);
}

/**
 *  \brief timer0_start () : 
 *                               
 *  \param Uint32					: timeout value 
 *  \return none                : None
 */

void timer0_start(Uint32 timeout)
{
	PRD0.period = timeout;
	PRD_start( &PRD0);
}
/**
 *  \brief timer0_stop () : 
 *                               
 *  \param void 
 *  \return none                : None
 *
 */
void timer0_stop(void)
{
	PRD_stop( &PRD0);
}

/**
 *  \brief sataTimer2_task () : 
 *                               
 *  \param void 
 *  \return none                : None
 *
 */
void sataTimer2_task(void)
{
	if( is_sata_pm_enable(0) )
	{
		/* perform pm activity */
		pm_sata_prd_task();
	}
}

/**
 *  \brief timer2_start () : 
 *                               
 *  \param Uint32					: timeout value 
 *  \return none                : None
 */

void timer2_start(Uint32 timeout)
{
	if (timeout)
	{
	}
	PRD_start( &PRD2);
}
/**
 *  \brief timer0_stop () : 
 *                               
 *  \param void 
 *  \return none                : None
 *
 */
void timer2_stop(void)
{
	PRD_stop( &PRD2);
}

/**
 *  \brief	Get next Request from pointing to head of Queue 
 *          does not delete the node
 *
 *  \param	 head		[IN]	pointer to queue Head
 *
 *  \return Request pointing to ATA_Dev_Req_t 
 */

ATA_Dev_Req_t *GetNextReq( Queue_t *qHead)
{
	if( Queue_empty(qHead) )
		return NULL;

	return (ATA_Dev_Req_t *)qHead->next;
}

/**
 *  \brief	Initialize the queue head
 *
 *  \param	 que_elm		[IN]	pointer to queue Head
 *
 *  \return None
 */
static void Queue_init (Queue_t *que_elm)
{
	que_elm->next = que_elm;
	que_elm->prev = que_elm;
}
/**
 *  \brief	check for Queue empty condition
 *
 *  \param	 head		[IN]	pointer to queue Head
 *
 *  \return True if queue empty else false
 */
static Uint32 Queue_empty (Queue_t *node)
{
	return node->next == node ;
}
/**
 *  \brief	Add new Queue element to tail of QueueHead
 *
 *  \param	 new		[IN]	new element to be added
 *  \param	 head		[IN]	pointer to queue Head
 *
 *  \return None
 */
static void Queue_add_tail (Queue_t *elem, Queue_t *qHead)
{
	elem->next = qHead;
	elem->prev = qHead->prev;
	qHead->prev->next = elem;
	qHead->prev = elem;
}
/**
 *  \brief	Delete the Queue element
 *
 *  \param	 entry		[IN]	Queue element to be deleted
 *
 *  \return None
 */
static void Queue_del (Queue_t *elem)
{
	Queue_t *next = elem->next,*prev = elem->prev;
	prev->next = next;
	next->prev = prev;
}
#if 0
/**
 *  \brief	count number of entries in Queue
 *
 *  \param	 head		[IN]	Pointer to Queue head
 *
 *  \return number of elements in queue
 */
static Uint32 Queue_count( Queue_t *qHead)
{
	Queue_t *queue = qHead;
	int count = 0;
	
	if( Queue_empty (qHead) )
		return 0;
	for(; queue->next != queue ; queue=queue->next,count++);
	
	return count;
}
#endif
/**
 *  \brief	check whether any ATA_IO is pending
 *
 *  \param	 None
 *
 *  \return 1 when IO is pending else 0.
 */
int Is_ATA_IO_Pending(void)
{
	Int32 i,io_pending = 0;

	for (i=0; i<ATA_MAX_IDE_INT; ++i)
	{
			if( !Queue_empty(&Ide_Ctlr_Info [i] . Drive[0].pReqQueHead) )
			{
				io_pending = 1;
				break;
			}
	}
	return io_pending;
}

//void timer0_init(char *name, Uint32 timeout, Char mode);

/**
 *  \brief ATA_IDE_Register () : Register a IDE Controller driver with the ATA
 *                               Interface Driver.
 *  \param pIdeOps [IN]        : IDE Controller operations
 *  \param pHandle [IN]        : Handle to be used for future transactions.
 *  \return int                : Success or Failure
 *
 ******************************************************************************/
Int32 ATA_IDE_Register (IDE_INTF_Confg_t const * const pIdeInt_config, void const * const pHandle)
{
    Int32 Index = 0;
    Int32 Status = ATA_ERR_GEN;

	if(NULL != pIdeInt_config) /*Handle can be NULL*/
	{

	    for (Index = 0; Index < ATA_MAX_IDE_INT; Index++) {
	        if (Ide_Ctlr_Info [Index] . pIdeOps == NULL) {
	            /* Need PAL OS interface here and move this alarm structure into the
	            * IDE controller structure.
	            *
	            * Create the Timeout timer for this ATA Bus.
	            */
	            Ide_Ctlr_Info[Index].TimeOutData = (Ptr) &timeoutData; 

				timer0.mode = TIMER_MODE_ONESHOT;
				timer0.timeout = *(Uint32 *)Ide_Ctlr_Info[Index].TimeOutData;
				timer0.fn = &ATA_DEV_CmdTimeout;
				timer0.param = (void *)&Ide_Ctlr_Info [Index] . pActiveDrive ;
				timer0.start = 0;
				timer0.init = 1;
				timer0.mode = 0;
				timer0.id = 1;
				Ide_Ctlr_Info[Index].IdeTimeout = &timer0;

	            Ide_Ctlr_Info [Index] . pIdeOps    = pIdeInt_config->pIdeOps;
	            Ide_Ctlr_Info [Index] . pIntHndl   = (Ptr)(Uint32)pHandle;
	            Ide_Ctlr_Info [Index] . Drive [0] . pIdeInt = &Ide_Ctlr_Info [Index];
	            Ide_Ctlr_Info [Index] . Drive [0] . Cap . IsAta = True;
				Ide_Ctlr_Info [Index] . Drive [0] . OpMode = DMA;
	            ATA_VoidDevops (&(Ide_Ctlr_Info [Index] . Drive [0]));

	            Ide_Ctlr_Info [Index] . Drive [1] . pIdeInt =  &Ide_Ctlr_Info [Index];
	            Ide_Ctlr_Info [Index] . Drive [1] . Cap . IsAta = True;
				Ide_Ctlr_Info [Index] . Drive [1] . OpMode = DMA;
	            ATA_VoidDevops (&(Ide_Ctlr_Info [Index] . Drive [1]));

	            Ide_Ctlr_Info [Index] . BusState   = ATA_IDE_RESET;
	            Ide_Ctlr_Info [Index] . pActiveDrive = &(Ide_Ctlr_Info [Index] . Drive [0]);

				Ide_Ctlr_Info [Index] . IsSata = pIdeInt_config->isSata;

 
	            /* Do a Bus Reset to get the ATA devices to a known state. */
	            ATA_IDE_Reset (&Ide_Ctlr_Info [Index]);
	            while (Ide_Ctlr_Info [Index] . BusState != ATA_IDE_FREE) {
					TSK_sleep(1u);
	            }

#ifdef BIOS_PWRM_ENABLE
				Status = Ide_Ctlr_Info [Index].pIdeOps->isPwrmEnabled((void *)Ide_Ctlr_Info [Index].pIntHndl);
				Ide_Ctlr_Info [Index] . ide_pwrm_enabled = Status;
				if ( Ide_Ctlr_Info [Index] . ide_pwrm_enabled )	
				{
					pm_ata_pwrm_init(&Ide_Ctlr_Info [Index]);	 
					Ide_Ctlr_Info [Index] . pwrm_registerd = 1;
				}
#endif
	            Ide_Ctlr_Info [Index] . IsInit = True;
	            Status = ATA_NO_ERR;


	            break;
	        }
	    }
	}

    return Status;
}
#ifdef BIOS_PWRM_ENABLE
/**
 *  \brief ATA_getPwrmHandle () : get pwrm handle
 *                               
 *  \param 						: Handle to IDE controller
 *  \return int                : return pwrm handle
 *
 ******************************************************************************/
Ptr ATA_getPwrmHandle(Ptr Handle)
{
	ATA_IDE_Int_t *pIdeInt = (ATA_IDE_Int_t *)Handle;
	return &pIdeInt->pwrmInfo;	
}
#endif

/**
 *  \brief ATAPI_MED_Void ()   : Void the ATAPI calls.
 *
 *  \param Handle  [IN]        : Device Handle
 *  \param MedHndl [IN]        : Media Handle
 *  \param pCmd    [IN]        : ATAPI Command
 *  \return int                : Error
 ******************************************************************************/
Int32  ATAPI_MED_Void  (Ptr Handle, Ptr MedHndl, Ptr pCmd)
{
  
  if ( (Handle != NULL) && (MedHndl != NULL) && (pCmd != NULL) )
  {
	/* no processing void function */
  }
  return ATA_ERR_GEN;
}

/**
 *  \brief ATA_MED_Ioctl ()    : ATA IOCTL calls.
 *
 *  \param Handle  [IN]        : Device Handle
 *  \param Cmd     [IN]        : IOCTL Command
 *  \param pData   [IN/OUT]    : Data Buffer
 *  \return int                : Error/Success
 ******************************************************************************/
static Int32 ATA_MED_Ioctl (Ptr Handle, ATA_DEV_IoctlCmd_t Cmd, Ptr * const pData)
{
  Int32 Err = ATA_NO_ERR;
  ATA_Drive_t         *pDevice = NULL;
  ATA_Dev_Cap_t       *pCap = NULL;
  ATA_DEV_Ident_t     *pDeviceIdent;
  Uint8 count;
  Uint32 Cookie;

  Cmd = (ATA_DEV_IoctlCmd_t)((Uint32)Cmd - (Uint32)PSP_BLK_IOCTL_MAX);

  if((NULL != Handle))
  {
		pDevice = (ATA_Drive_t *)Handle;
		pCap = &pDevice -> Cap;

	    switch (Cmd)
	    {
	    case ATA_IOCTL_RETRY:
	      /* Set Device IO request retry count */
			if(NULL != pData)
			{
				pDevice -> RetryCnt = *((Uint32 *)(Uint32)pData);
			}
			else
			{
				Err = ATA_ERR_GEN;
			}
	      break;   
	    case ATA_IOCTL_GETMODE:
	      /* Get device Op mode */
		  if(NULL != pData)
		  {
	      	*(ATA_DEV_Mode_t *)pData = pDevice -> OpMode;
		  }
		  else
		  {
			Err = ATA_ERR_GEN;
		  }
	      break;
	    case ATA_IOCTL_GETIDENT:
			if(NULL != pData)
			{
		      pDeviceIdent = (ATA_DEV_Ident_t *)(Uint32)pData;

		      pDeviceIdent -> Cyl = pCap -> Cyl;
		      pDeviceIdent -> Head = pCap -> Head;
		      pDeviceIdent -> SectperTrack = pCap -> SectperTrack;
		      count = 0;

		      while (count < (8u))
		      {
		        pDeviceIdent->FirmwareRev[count] = pCap -> FirmwareRev[count];
		        pDeviceIdent->SerialNo[count]    = pCap -> SerialNo[count];
		        pDeviceIdent->ModelNo[count]     = pCap -> ModelNo[count];
		        ++count;
		      }

		      while (count < (20u))
		      {
		        pDeviceIdent->SerialNo[count]    = pCap -> SerialNo[count];
		        pDeviceIdent->ModelNo[count]     = pCap -> ModelNo[count];
		        ++count;
		      }

		      while (count < (40u))
		      {
		        pDeviceIdent->ModelNo[count]     = pCap -> ModelNo[count];
		        ++count;
		      }
			}
			else
			{
				Err = ATA_ERR_GEN;
			}
	      break;
	    case ATA_IOCTL_SETBLKSZ:
			if(NULL != pData)
			{
		      /* Set the Block Size for the device.  Default is 512 */
		      pDevice -> Cap . SectSize = (Uint16)(*(Uint32 *)pData);
			}
			else
			{
				Err = ATA_ERR_GEN;
			}
	      break;
	    case ATA_IOCTL_REQABRT:
	      /* Request to abort all IO request on this device */
	      ATA_MED_ReqAbort (pDevice);
	      break;
	    case ATA_IOCTL_GETPWRMODE:
			if(NULL != pData)
			{
		      /* Get device Op mode */
		      *(ATA_DEV_PwrMode_t *)pData = pDevice -> PwrMode;
			}
			else
			{
				Err = ATA_ERR_GEN;
			}
	      break;
	    case ATA_IOCTL_GETMEDSTAT:
	      Err = ATA_MED_GetMediaStatus (pDevice, pData);
	      break;
	    case ATA_IOCTL_SETDEVOFF:
		  Cookie = HWI_disable ();
	      pDevice -> PwrMode = ATA_DEV_SLEEP;
	      ATA_SetDevops (pDevice);
		  HWI_restore (Cookie);
	      break;
	    case ATA_IOCTL_GETPWRSTAT:
	      /* Initiate the Check Power Status command needed for ATAPI power
	       * check mechanism.
	       */
	      Err = ATA_MED_GetPwrStat (pDevice, pData);
	      break;
	    case ATA_IOCTL_HARDRESET:
		  Err = ATA_IDE_Reset(pDevice->pIdeInt);
//	      ATA_MED_HardReset (Handle, (Int32)pData);
	      break;
	    default:
	      Err = ATA_ERR_GEN;
	      break;
	    }
	}
	else
	{
	    Err = ATA_ERR_GEN;
	}

  return Err;
}
/**
 *  \brief ATA_MED_GetPwrStat ()    : Get the device power status.
 *
 *  \param Handle [IN]                  : Device Handle
 *  \param Buffer [IN]                  : Media status Buffer.
 *  \return int                         :
 ******************************************************************************/
static Int32 ATA_MED_GetPwrStat (Ptr Handle, Ptr Buffer)
{

  ATA_Drive_t *pDevice = NULL;
  ATA_Dev_Req_t *pReq = NULL;
  Int32 Err = ATA_ERR_GEN;
  Uint32  Timeout = 0;

  if((NULL != Handle) && (NULL != Buffer))
  {
	  pDevice = (ATA_Drive_t *)Handle;
	  Timeout = (pDevice -> PwrMode == ATA_DEV_ACTIVE)? ATA_CMD_TO :
	                            ATA_RESET_TO;
	  pReq = ATA_AllocReq (pDevice, False);
	  if (pReq != NULL)
	  {
	    pReq -> Command . Cmd = ATA_PWR_CHECK;
	    pReq -> Command . Device = pDevice -> Dev;
	    pReq -> Mode = TASKFILE;
	    pReq -> Buffer = Buffer;
	    pReq -> Handler = &ATA_Taskfile;
	    pReq -> Timeout = Timeout;
	    pReq -> Res . IsEvent = False;
	    ATA_SubmitReq (pDevice, pReq);
	    Err = ATA_NO_ERR;
	  }
  }

  return Err;
}

/**
 *  \brief ATA_MED_GetMediaStatus ()    : Get the media status.
 *
 *  \param Handle [IN]                  : Device Handle
 *  \param Buffer [IN]                  : Media status Buffer.
 *  \return int                         :
 ******************************************************************************/
static Int32 ATA_MED_GetMediaStatus (Ptr Handle, Ptr Buffer)
{
  ATA_Drive_t *pDevice = NULL;
  ATA_Dev_Req_t *pReq = NULL;
  Int32 Err = ATA_ERR_GEN;
  Uint32  Timeout = 0;

  if((NULL != Handle) && (NULL != Buffer))
  {
	  pDevice = (ATA_Drive_t *)Handle;
	  Timeout = (pDevice -> PwrMode == ATA_DEV_ACTIVE)? ATA_CMD_TO :
	                            ATA_RESET_TO;
	  pReq = ATA_AllocReq (pDevice, False);
	  if (pReq != NULL)
	  {
	    pReq -> Command . Cmd = ATA_GET_MEDSTAT;
	    pReq -> Command . Device = pDevice -> Dev;
	    pReq -> Mode = TASKFILE;
	    pReq -> Buffer = Buffer;
	    pReq -> Handler = &ATA_Taskfile;
	    pReq -> Timeout = Timeout;
	    pReq -> Res . IsEvent = False;
	    ATA_SubmitReq (pDevice, pReq);
	    Err = ATA_NO_ERR;
	  }
  }

  return Err;
}

/**
 *  \brief ATA_MED_IoctlVoid ()     : Return Error on IOCTL quiries.
 *
 *  \param AtaHandle       [IN]     : Device Handle.
 *  \param Cmd             [IN]     : IOCTL command.
 *  \param pData           [IN]     : Data for the Command.
 *
 *  \return int                     : ATA_ERR_GEN
 ******************************************************************************/
static Int32 ATA_MED_IoctlVoid (Ptr AtaHandle, ATA_DEV_IoctlCmd_t Cmd, Ptr * const pData)
{
	if ( (AtaHandle != NULL) && (pData != NULL) && (Cmd < ATA_IOCTL_MAX) )
	{
		/* no processing void function */	
	}
  return ATA_ERR_GEN;
}


/**
 *  \brief ATA_MED_GetMaxSect ()    : Get the maximum user accessable sector.
 *
 *  \param Handle  [IN]             : Device Handle
 *  \return int                     : Max user accessible Sector
 ******************************************************************************/
static Int32 ATA_MED_GetMaxSect (Ptr Handle)
{
	Int32 retVal = 0;
  
	if(NULL != Handle)
	{
		retVal =(Int32)((ATA_Drive_t *)Handle) -> Cap . MaxLba;
	}
	return retVal;
}


/**
 *  \brief ATA_MED_GetMaxSectVoid () : Return Error.
 *
 *  \param Handle  [IN]             : Device Handle
 *  \return int                     : ATA_ERR_GEN
 ******************************************************************************/
static Int32 ATA_MED_GetMaxSectVoid (Ptr Handle)
{
	if ( Handle != NULL )
	{
		/* no processing void function */
	}
  	return ATA_ERR_GEN;
}


/**
 *  \brief ATA_MaxLba ()            : Get the maximum user accessable LBA.
 *                                    Called at Init time.
 *  \param pDevice  [IN]            : Device Pointer
 *  \return int                     : Success
 ******************************************************************************/
static Int32 ATA_MaxLba (ATA_Drive_t *pDevice)
{
  ATA_Dev_Req_t *pReq = NULL;
  ATA_Dev_Stat_t Status;
  Int32 retVal = ATA_ERR_GEN;

  if(NULL != pDevice)
  {
		retVal = ATA_NO_ERR;
	  pReq = ATA_AllocReq (pDevice, False);
	  if (pReq != NULL)
	  {
	    pReq -> MediaHandle = NULL;
	    pReq -> IsInternal  = True;
	    pReq -> Command . Device = pDevice -> Dev;
	    pReq -> Command . Cmd = ATA_READ_MAX_SECT;
	    pReq -> Buffer = NULL;
	    pReq -> Mode = TASKFILE;
	    pReq -> Res . IsEvent = False;
	    pReq -> Handler = &ATA_IdentHndlr;
	    pReq -> ToHndlr = &ATA_IdentHndlr;
	    pReq -> Timeout = ATA_ONESEC_TO;
	    pReq -> Retry   = 10u;

	    ATA_SubmitReq (pDevice, pReq);

	    while (pReq -> Retry != 0)
	    {
		  m_delay(1u);
	    }

		Status = pReq->Status;

	    if (pReq -> Res . ReqInfo . Status . Error == ATA_ERR_TO)
	    {
	      /* Inform that this request timeout out and the status is not valid */
	      Status . Error = 0xFFu;
	      /* Set the Error Status on the device status */
	      Status . Stat  |= ATA_DEV_ERR;
	    }

	    if (FALSE == (Status . Stat & ATA_DEV_ERR))
	    {
	      pDevice -> Cap . MaxLba = ((Status . Device & (0x0Fu)) << 24) |
	                                ((Status . LbaHigh & (0xFFu)) << 16 ) | 
	                                ((Status . LbaMid & (0xFFu)) << 8) | 
	                                Status . LbaLow;
	    } else
	    {
	      pDevice -> Cap . MaxLba = 0;
	    }

	    ATA_SchedNextReq (pDevice -> pIdeInt, pReq);
	  }
	}
  return retVal;
}


/**
 *  \brief ATA_MED_PwrVoid ()       : Return Error.
 *
 *  \param Handle [IN]              : Device Handle
 *  \param Mode   [IN]              : Power Mode
 *  \param MHndl  [IN]              : Handle for Media Layer
 *
 *  \return int                     : ATA_ERR_GEN
 ******************************************************************************/
static Int32 ATA_MED_PwrVoid (Ptr Handle, Ptr MHndl, ATA_DEV_PwrMode_t Mode)
{	
	ATA_Drive_t *pDevice;
	Int32 retVal = ATA_ERR_GEN;

	if ( (Handle != NULL) && (MHndl != NULL) && (Mode < ATA_DEV_PWRMAX) )
	{
		pDevice = (ATA_Drive_t *)Handle;

		switch ( Mode )
		{
			case ATA_DEV_ACTIVE :
			case ATA_DEV_IDLE :
			  /* Initialize the Devops for a DMA mode operation */
			  pDevice -> DevOps . Ata_SetDevMode = &ATA_MED_SetModeVoid;
			  pDevice -> DevOps . Ata_DevReset = &ATA_IDE_Reset;
			  pDevice -> DevOps . Ata_SetPwrMode = &ATA_MED_SetPwrMode;
			  pDevice -> DevOps . Ata_GetMaxSect = &ATA_MED_GetMaxSect;
			  pDevice -> DevOps . Ata_Read = &ATA_MED_ReadDma;
			  pDevice -> DevOps . Ata_Write = &ATA_MED_WriteDma; 
			  pDevice -> PwrMode =  Mode;
			  retVal = ATA_NO_ERR;
			  break;
		}
	}
	return retVal;
}


/**
 *  \brief ATA_MED_SetPwrMode ()    : Set the operating power mode for the device.
 *
 *  \param Handle [IN]              : Device Handle
 *  \param Mode   [IN]              : Power Mode
 *  \param MHndl  [IN]              : Handle for Media Layer
 *  \return int                     :
 ******************************************************************************/
Int32 ATA_MED_SetPwrMode (Ptr Handle, Ptr MHndl, ATA_DEV_PwrMode_t Mode)
{
  ATA_Drive_t *pDevice = NULL;
  ATA_Dev_Req_t *pReq = NULL;
  ATA_DEV_Res_t Res;
  Bool Submit = True;
  Int32 Err = ATA_ERR_GEN;

  if((NULL != Handle) && (NULL != MHndl))
  {
		Err= ATA_NO_ERR;
	  pDevice = (ATA_Drive_t *)Handle;
	  if ((Mode < ATA_DEV_ACTIVE) || (Mode >= ATA_DEV_PWRMAX))
	  {
	    Err =  ATA_ERR_GEN;
	  }
	  else
	  {
	  
		  if (Mode != pDevice -> PwrMode)
		  {
		    if (Mode != ATA_DEV_ACTIVE)
		    {
		      pReq = ATA_AllocReq (pDevice, False);
		      if (pReq != NULL)
		      {
		        pReq -> Command . Device = pDevice -> Dev;
		        pReq -> Mode = TASKFILE;
		        pReq -> MediaHandle = MHndl;
		        pReq -> Handler = &ATA_Taskfile;
		        pReq -> Timeout = ATA_CMD_TO;
		        pReq -> Res . IsEvent = True;
				if ( Mode < ATA_DEV_PWRMAX)
				{
		        	pReq -> Res . ReqInfo . Event . EventId = ATA_Pwr_Evt [Mode];
				}

		        switch (pDevice -> PwrMode)
		        {
		        case ATA_DEV_ACTIVE:
		          switch (Mode)
		          {
		          case ATA_DEV_SLEEP:
		            pReq -> Command . Cmd = ATA_PWR_SLEEP;
		            break;
		          case ATA_DEV_STDBY:
		            pReq -> Command . Cmd = ATA_PWR_STDBY;
		            break;
		          case ATA_DEV_IDLE:
		            pReq -> Command . Cmd = ATA_PWR_IDLE;
		            break;
		          default :
		            break;
		          }
		          break;
		        case ATA_DEV_IDLE:
		          switch (Mode)
		          {
		          case ATA_DEV_SLEEP:
		            pReq -> Command . Cmd = ATA_PWR_SLEEP;
		            break;
		          case ATA_DEV_STDBY:
		            pReq -> Command . Cmd = ATA_PWR_STDBY;
		            break;
		          default :
		            break;
		          }
		          break;
		        case ATA_DEV_STDBY:
		          switch (Mode)
		          {
		          case ATA_DEV_SLEEP:
		            pReq -> Command . Cmd = ATA_PWR_SLEEP;
		            break;
		          case ATA_DEV_IDLE:
		            pReq -> Command . Cmd = ATA_PWR_IDLE;
		            break;
		          default :
		            break;
		          }
		          break;
		        default :
		          break;
		        }
		      }
		      else
		      {
		        Err = ATA_ERR_GEN;
		      }
		    }
		  }
		  else
		  {
		    Submit = False;
		  }

		  if(ATA_NO_ERR == Err)
		  {
			  if(Submit == False)
			  {
			    Res . IsEvent = True;
			    Res . ReqInfo . Event . EventId = ATA_Pwr_Evt [Mode];
			    pDevice -> MedHandler (MHndl, &Res);
			  }
			  else
			  {
			    if ( pReq != NULL )
				{
				    pReq -> Retry   = 10u;

				    ATA_SubmitReq (pDevice, pReq);

				    while (pReq -> Retry != 0)
				    {
					  m_delay(1u);
				    }
				}
				else
				{
					Err = ATA_ERR_GEN;
				}
			  }
		  }
		}
	}

  return Err;
}

/**
 *  \brief ATA_MED_SetPwrMode ()    : Set the operating power mode for the device.
 *
 *  \param Handle [IN]              : Device Handle
 *  \param Mode   [IN]              : Power Mode
 *  \param MHndl  [IN]              : Handle for Media Layer
 *  \return int                     :
 ******************************************************************************/
Int32 ATA_IDE_SetPwrMode (Ptr Handle, Ptr MHndl, ATA_DEV_PwrMode_t Mode)
{
  ATA_Drive_t *pDevice = NULL;
  ATA_Dev_Req_t *pReq = NULL;
  Bool Submit = True;
  Int32 Err = ATA_ERR_GEN;
  Uint32  portNum = 0, Cookie;

  if((NULL != Handle) && (NULL != MHndl))
  {
		Err= ATA_NO_ERR;
	  pDevice = (ATA_Drive_t *)Handle;
	  if ((Mode < ATA_DEV_ACTIVE) || (Mode >= ATA_DEV_PWRMAX))
	  {
	    Err =  ATA_ERR_GEN;
	  }
	  else
	  {
	  
		  if (Mode != pDevice -> PwrMode)
		  {
		    if (Mode != ATA_DEV_ACTIVE)
		    {
		      pReq = ATA_AllocReq (pDevice, False);
		      if (pReq != NULL)
		      {
		        pReq -> Command . Device = pDevice -> Dev;
		        pReq -> Mode = TASKFILE;
		        pReq -> MediaHandle = MHndl;
		        pReq -> Handler = &ATA_Taskfile;
		        pReq -> Timeout = ATA_CMD_TO;

		        switch (pDevice -> PwrMode)
		        {
		        case ATA_DEV_ACTIVE:
		          switch (Mode)
		          {
		          case ATA_DEV_SLEEP:
		            pReq -> Command . Cmd = ATA_PWR_SLEEP;
		            break;
		          case ATA_DEV_STDBY:
		            pReq -> Command . Cmd = ATA_PWR_STDBY;
		            break;
		          case ATA_DEV_IDLE:
		            pReq -> Command . Cmd = ATA_PWR_IDLE;
		            break;
		          default :
		            break;
		          }
		          break;
		        case ATA_DEV_IDLE:
		          switch (Mode)
		          {
		          case ATA_DEV_SLEEP:
		            pReq -> Command . Cmd = ATA_PWR_SLEEP;
		            break;
		          case ATA_DEV_STDBY:
		            pReq -> Command . Cmd = ATA_PWR_STDBY;
		            break;
		          default :
		            break;
		          }
		          break;
		        case ATA_DEV_STDBY:
		          switch (Mode)
		          {
		          case ATA_DEV_SLEEP:
		            pReq -> Command . Cmd = ATA_PWR_SLEEP;
		            break;
		          case ATA_DEV_IDLE:
		            pReq -> Command . Cmd = ATA_PWR_IDLE;
		            break;
		          default :
		            break;
		          }
		          break;
		        default :
		          break;
		        }
		      }
		      else
		      {
		        Err = ATA_ERR_GEN;
		      }
		    }
		  }
		  else
		  {
		    Submit = False;
		  }

		  if(ATA_NO_ERR == Err)
		  {
			  if(Submit != False && (pReq != NULL))
			  {
			    pReq -> Retry   = 10u;
		 
				if ((pDevice -> pIdeInt -> BusState == ATA_IDE_FREE))
				{
					Cookie = HWI_disable ();
					Queue_add_tail(&pReq->link, &pDevice->pReqQueHead);
					pDevice -> ActiveReq++;
					pDevice->pReq = GetNextReq(&pDevice->pReqQueHead);

					pDevice -> pIdeInt -> BusState = ATA_IDE_BUSY;
					pDevice -> pIdeInt -> pActiveDrive = pDevice;
					HWI_restore (Cookie);

					if( is_sata_pm_enable(0) )
					{
						timer2_stop();
						pm_sata_power_on();
					}

					AhciExecPolledCmd(pDevice -> pIdeInt -> pIntHndl, pReq);

				    /* update the status in ATA req status field */
				    Ide_Ctlr_Info [portNum] . pIdeOps -> CmdStatus (Ide_Ctlr_Info [portNum] . 
				                                pIntHndl, &pReq->Status, False, portNum);

					ATA_RelReq (pDevice -> pIdeInt -> pActiveDrive, pReq);
					Cookie = HWI_disable ();
					pDevice -> pIdeInt -> BusState = ATA_IDE_FREE;
					HWI_restore (Cookie);
				}
				else
				{
					 Err = ATA_ERR_GEN;
				}
			  }
		  }
		}
	}

  return Err;
}


/**
 *  \brief ATA_MED_ReqAbort ()      : Abort Requests on this device.  If a
 *                                    active request is pending on this device
 *                                    (this device being the active device)then
 *                                    we return with ATA_ERR_GEN
 *
 *  \param Handle  [IN]             : Device Handle
 *  \return int                     : Success.
 ******************************************************************************/
static Int32 ATA_MED_ReqAbort (Ptr Handle)
{
  ATA_Drive_t *pDevice = NULL;
  Uint32 Cookie;
  Int32 Err = ATA_ERR_GEN;

  if(NULL != Handle)
  {
	  pDevice = (ATA_Drive_t *)Handle;
	  Err = ATA_NO_ERR;
	  Cookie = HWI_disable ();
	  while (pDevice -> pReq != NULL)
	  {
	    pDevice -> pReq -> Res . IsEvent = False;
	    pDevice -> pReq -> Res . ReqInfo . Status . Error = ATA_ERR_RES;
	    pDevice -> MedHandler (pDevice -> pReq -> MediaHandle,
	                           &(pDevice -> pReq -> Res));
	    pDevice -> pReq = ATA_RelReq (pDevice, pDevice -> pReq);
	  }
	  HWI_restore (Cookie);
	}
  return Err;
}


/**
 *  \brief ATA_MED_ResetVoid ()    : Return Error.
 *  \param Handle [IN]             : Device Handle
 *  \return int                    : ATA_ERR_GEN
 ******************************************************************************/
static Int32 ATA_MED_ResetVoid (Ptr Handle)
{
	if (Handle != NULL )
	{
	}
	return ATA_ERR_GEN;
}

#if 0
/**
 *  \brief ATA_MED_HardReset ()    : Initiate a Hardware Reset.
 *
 *  \param Handle [IN]         : Device Handle
 *  \param ResetWidth [IN]     : Reset pulse width
 *  \return int                :
 ******************************************************************************/
static Int32 ATA_MED_HardReset (Ptr Handle, Int32 ResetWidth)
{

  ATA_Drive_t *pDevice = NULL;
  ATA_Drive_t *pNxtDev = NULL;
  ATA_DEV_Res_t Res;
  Int32 Err= ATA_ERR_GEN;

 if(NULL != Handle)
 {
	Err= ATA_NO_ERR;
  pDevice = (ATA_Drive_t *)Handle;
  pNxtDev = (&(pDevice -> pIdeInt -> Drive [0]) == Handle) ?
                         &(pDevice -> pIdeInt -> Drive [1]) :
                         &(pDevice -> pIdeInt -> Drive [0]);

  /* For ATAPI Device reset reset that device alone */
  Res . IsEvent = True;
  Res . ReqInfo . Event . EventId = ATA_DEV_EVT_RES_BEGIN;
  pDevice -> MedHandler (NULL, &Res);
  if (pNxtDev -> MedHandler != NULL)
  {
    pNxtDev -> MedHandler (NULL, &Res);
  }
  ATA_VoidDevops (pDevice);
  ATA_VoidDevops (pNxtDev);

  /* Initiate a Hardware Reset */
  pDevice -> pIdeInt -> pIdeOps -> Reset (pDevice -> pIdeInt, ResetWidth);

  /* Complete the Hardware Reset by initing the IDE structures */
  ATA_IdeResEnd (pDevice, NULL);

  }
  return Err;
  
}

/**
 *  \brief ATA_MED_Reset ()    : Reset the Device.  Take's care of bus reset if
 *                               required (in case of ATA device).
 *
 *  \param Handle [IN]         : Device Handle
 *  \return int                :
 ******************************************************************************/
static Int32 ATA_MED_Reset (Ptr Handle)
{
  ATA_Drive_t *pDevice = NULL;
  ATA_Drive_t *pNxtDev = NULL;
  Int32 Err = ATA_ERR_GEN;
  ATA_DEV_Res_t Res;

  if(NULL != Handle)
  {

	pDevice = (ATA_Drive_t *)Handle;
	pNxtDev = (&(pDevice -> pIdeInt -> Drive [0]) == Handle) ?
	                     &(pDevice -> pIdeInt -> Drive [1]) :
	                     &(pDevice -> pIdeInt -> Drive [0]);
	  /* Initiate a reset only under a Bus Timeout or Device Sleep scenario */
	  if ((pDevice -> PwrMode == ATA_DEV_SLEEP) ||
	          (pDevice -> pIdeInt -> BusState == ATA_IDE_TO))
	  {
	    /* For a ATA Device reset reset the ATA Bus */
	    if (pDevice -> Cap . IsAta == True)
	    {
	      pDevice -> pIdeInt -> BusState = ATA_IDE_RESET;
	      if (pNxtDev -> Cap . IsAta==False)
	      {
	        ATA_PacketDevRes (pDevice);
	      }else
	      {
	        pDevice -> pIdeInt -> pActiveDrive = pDevice;
	        ATA_IDE_Reset (pDevice -> pIdeInt);
	      }
	    } else
	    {
	      /* For ATAPI Device reset reset that device alone */
	      Res . IsEvent = True;
	      Res . ReqInfo . Event . EventId = ATA_DEV_EVT_RES_BEGIN;
	      pDevice -> MedHandler (NULL, &Res);

	      ATA_PacketDevRes (pDevice);
	    }
	    Err = ATA_NO_ERR;
	  }
	}

  return Err;
}


/**
 *  \brief ATA_PacketDevRes ()  : Reset the ATAPI Device
 *  \param pDevice [IN]         : Pointer to the ATAPI device.
 *  \return void
 ******************************************************************************/
static void ATA_PacketDevRes (ATA_Drive_t *pDevice)
{
  ATA_Dev_Req_t *pReq = NULL;
	if(NULL != pDevice)
	{
	  pReq = ATA_AllocReq (pDevice, False);

	  ATA_VoidDevops (pDevice);

	  if (NULL != pReq)
	  {
		  pReq -> Command . Device = pDevice -> Dev;
		  pReq -> Command . Cmd = ATAPI_RESET;
		  pReq -> Command . Features = 0;
		  pReq -> Mode = TASKFILE;
		  pReq -> Timeout = ATA_ONESEC_TO;
		  pReq -> ToHndlr = &ATA_ResetSched;
		  pReq -> Retry = 30u;
		  pReq -> MediaHandle = NULL;
		  pReq -> IsInternal  = True;

		  /* The following line has to be done as this function can be called both in
		   * a device reset scenario and also in a IDE reset scenario.  In a Device
		   * reset scenario we are already the active drive so setting this up one again
		   * is ok.  In an IDE Reset scenario we are not the active drive the next drive
		   * is.  But for us to execute our ATAPI reset we have to be active drive.  So
		   * we need to set that up here.  ATA_SubmitComit expects that the device owns
		   * the bus (be the active Drive).  We shall resolve at the end of our device
		   * reset whether the actual operation was Device Reset or IDE reset using the
		   * BusState information.
		   */
		  pDevice -> pIdeInt -> pActiveDrive = pDevice;
		  ATA_SubmitComit (pDevice -> pIdeInt, pReq);
		}
	}
}
#endif

/**
 *  \brief ATA_IDE_Reset ()  : Reset the ATA Bus
 *  \param pIdeInt [IN]      : Pointer to the ATA Controller Bus.
 *  \return void
 ******************************************************************************/
static Int32 ATA_IDE_Reset (Ptr Handle)
{
  ATA_Dev_Req_t *pReq = NULL;
  Uint8 Device = 0, done = 1;
  ATA_DEV_Res_t Res;
  Int32 retVal = IOM_COMPLETED;
  ATA_IDE_Int_t *pIdeInt = Handle;

  if(NULL != pIdeInt)
  {

	do
	{
		  if( pIdeInt -> IsSata )
		  {
			if( is_sata_pm_enable(0) )
			{
				timer2_stop();
				pm_sata_power_on();
			}

			pIdeInt -> Drive[0].pIdeInt ->pIdeOps->Reset(pIdeInt->pIntHndl, 0);
			pIdeInt -> BusState = ATA_IDE_FREE;
			break;
		  }

		  pReq = ATA_AllocReq (pIdeInt -> pActiveDrive, False);
		  Res . IsEvent = True;
		  Res . ReqInfo . Event . EventId = ATA_DEV_EVT_RES_BEGIN;

		  /* Void the Device operations of the devices on this bus.  Send the Reset Begin
		   * event.
		   */
		  while (Device != ATA_MAX_INT_DEV)
		  {
		    ATA_VoidDevops (&(pIdeInt -> Drive [Device]));
		    if (pIdeInt -> Drive [Device] . MedHandler != NULL)
		    {
		      pIdeInt -> Drive [Device] . MedHandler (NULL, &Res);
		    }
		    Device++;
		  }

		  pIdeInt -> BusState = ATA_IDE_RESET;
		  pReq -> MediaHandle = NULL;
		  pReq -> IsInternal  = True;
		  pReq -> Command . Cmd = ATA_DEV_RESET;
		  pReq -> Command . Features = 0x4;
		  pReq -> Mode = CONTROL;
		  pReq -> Timeout = (5u) * ATA_TMSEC_TO;
		  pReq -> ToHndlr = &ATA_ResetProgress;  /* for sata set ToHndlr to ATA_ResetToHandler */

		  /* Un-register from the IDE controller for this bus.  Protect ourself
		   * from interrupt completion race conditions.
		   */
		  pIdeInt -> pIdeOps -> RegHndlr (pIdeInt -> pIntHndl, NULL);
		  ATA_SubmitComit (pIdeInt, pReq);

		} while (!done);
	}
	return retVal;
}
#if 0
/**
 *  \brief ATA_ResetToHandler ()     : Handler for ATA Bus Reset progress phase.
 *                                    Refer ATA/ATAPI-4 std.
 *  \param Handle  [IN]             : Device Handle
 *  \param pStatus [IN]             : Status pointer
 *  \return Bool                    : Timeout or not
 ******************************************************************************/
static Bool ATA_ResetToHandler (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
  ATA_Drive_t const  *pDevice = NULL;
  ATA_Dev_Req_t *pReq = NULL;
  Bool retVal = FALSE;
  if((NULL != Handle))
  {
		pDevice = (ATA_Drive_t const *)Handle;
		if (pDevice -> pIdeInt -> BusState == ATA_IDE_RESET)
		{
			pDevice -> pIdeInt -> BusState = ATA_IDE_FREE;
			pReq->Timeout = 0;
		}
		timer0_stop();
		ATA_DevResEnd (Handle, pStatus);
  }

  return retVal;
}
#endif

/**
 *  \brief ATA_ResetProgress ()     : Handler for ATA Bus Reset progress phase.
 *                                    Refer ATA/ATAPI-4 std.
 *  \param Handle  [IN]             : Device Handle
 *  \param pStatus [IN]             : Status pointer
 *  \return Bool                    : Timeout or not
 ******************************************************************************/
static Bool ATA_ResetProgress (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
  ATA_Drive_t const  *pDevice = NULL;
  ATA_Dev_Req_t *pReq = NULL;
  Bool retVal = FALSE;

	/* remark: pStatus was never referenced */
	if ( pStatus != NULL ) 
	{ 
	}
	  
  if((NULL != Handle))
  {
 	  pDevice = (ATA_Drive_t const *)Handle;
	  pReq = pDevice -> pReq;
	  pReq -> Command . Cmd = ATA_DEV_UNRESET;
	  pReq -> Command . Features = 0x0;
	  pReq -> Mode = CONTROL;
	  pReq -> Timeout = ATA_ONESEC_TO;
	  pReq -> ToHndlr = &ATA_ResetSched;
	  pReq -> Retry   = 30u; /* Schedule for 30 sec in 1 sec incrments */

	  ATA_SubmitComit (pDevice -> pIdeInt, pReq);
 }

  return retVal;
}


/**
 *  \brief ATA_ResetSched ()        : Handler for ATA Bus Reset Schedule phase.
 *
 *  \param Handle  [IN]             : Device Handle
 *  \param pStatus [IN]             : Status pointer
 *  \return Bool                    : Timeout or not
 ******************************************************************************/
static Bool ATA_ResetSched (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
  ATA_Drive_t const *pDevice = NULL;
  ATA_Dev_Req_t *pReq = NULL;
  Bool retVal = FALSE;


  if((NULL != Handle) && (NULL != pStatus))
  {
	pDevice = (ATA_Drive_t const *)Handle;
	pReq = pDevice -> pReq;
	  if (((pStatus -> Stat & ATA_DEV_BUSY)==ATA_DEV_BUSY))
	  {
		if(--(pReq -> Retry) > 0)
	  	{
		    pReq -> ToHndlr = &ATA_ResetSched;
		    pDevice -> pIdeInt -> TimeOutData = (Ptr)pDevice;

			timer0_start( pReq -> Timeout);
	  	} 
		else
		{
		    if (pDevice -> Cap . IsAta == True)
		    {
		      ATA_IdeResEnd (Handle, pStatus);
		    } else
		    {
		      ATA_DevResEnd (Handle, pStatus);
		    }
	   }
	  }
	  else
	  {
	    if (pDevice -> Cap . IsAta == True)
	    {
	      ATA_IdeResEnd (Handle, pStatus);
	    } else
	    {
	      ATA_DevResEnd (Handle, pStatus);
	    }
	  }
	}
  
  

  return retVal;
}


/**
 *  \brief ATA_DevResEnd ()         : Handler for ATAPI Device Reset end phase.
 *
 *  \param Handle  [IN]             : Device Handle
 *  \param pStatus [IN]             : Status pointer
 *  \return Bool                    : False
 ******************************************************************************/
static Bool ATA_DevResEnd (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
  ATA_Drive_t *pDevice;
  Uint8 Device = 0;
  ATA_DEV_Res_t Res;
  Bool retVal = FALSE;

  if((NULL != Handle) && (NULL != pStatus))
  {
 	  pDevice = (ATA_Drive_t *)(Uint32)Handle;
	  retVal = FALSE;
	  Res . IsEvent = True;
	  Res . ReqInfo . Event . EventId = ATA_DEV_EVT_RES_END;
	  pDevice -> pReq = ATA_RelReq (pDevice, pDevice -> pReq);
	  
	  /* Return Reset Error */
	  while (pDevice -> pReq != NULL)
	  {
	    pDevice -> pReq -> Res . IsEvent = False;
	    pDevice -> pReq -> Res . ReqInfo . Status . Error = ATA_ERR_RES;
	    pDevice -> MedHandler (pDevice -> pReq -> MediaHandle,
	                           &(pDevice -> pReq -> Res));
	    pDevice -> pReq = ATA_RelReq (pDevice, pDevice -> pReq);
	  }

	  pDevice -> PwrMode = ATA_DEV_ACTIVE;
	  pDevice -> Probe = ATA_DEV_INIT;

	  /* Look into the bus state to differentiate between a actual ATAPI reset
	   * request and a IDE reset scenario.  In case of ATAPI reset start firing
	   * any pending IO's in the next device as the bus is now in a stable condition.
	   *
	   * In a IDE reset scenario we need to start the IDE Bus reset now and make the
	   * actual device (that initiated the IDE reset) the active device.
	   */
	  if (pDevice -> pIdeInt -> BusState == ATA_IDE_RESET)
	  {
	    Device = (Device == 0) ? 1 : 0 ;
	    pDevice -> pIdeInt -> pActiveDrive = &(pDevice -> pIdeInt -> Drive [Device]);
	    ATA_IDE_Reset (pDevice -> pIdeInt);
	  } else
	  {
	    pDevice -> pIdeInt -> BusState = ATA_IDE_FREE;

	    Device = (Device == 0) ? 1 : 0 ;
	    if (pDevice -> pIdeInt -> Drive [Device] . pReq != NULL)
	    {
	      /* There were pending commands on the other device.  This did not execute
	       * as the bus timedout due to our device.  So now execute the first request
	       * in the next device.
	       */
	      pDevice -> pIdeInt -> pActiveDrive = &(pDevice -> pIdeInt -> Drive [Device]);
	      ATA_SubmitComit (pDevice -> pIdeInt, pDevice -> pReq);
	    }
		if( NULL != pDevice -> MedHandler )
		{
		    /* Send the Reset End Event */
		    pDevice -> MedHandler (NULL, &Res);
		}
	    pDevice -> pIdeInt -> pActiveDrive = NULL;
	  }
	}

  return retVal;
}


/**
 *  \brief ATA_IdeResEnd ()         : Handler for ATA Bus Reset end phase.
 *
 *  \param Handle  [IN]             : Device Handle
 *  \param pStatus [IN]             : Status pointer
 *  \return Bool                    : False
 ******************************************************************************/
static Bool ATA_IdeResEnd (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
  ATA_Drive_t *pDevice = NULL;
  ATA_IDE_Int_t *pIdeInt = NULL;
  Uint8 Device = 0;
  ATA_DEV_Res_t Res;
  Bool retVal = FALSE;

  if(NULL != Handle)
  {

 	 pDevice = (ATA_Drive_t *)(Uint32)Handle;
  	 pIdeInt = pDevice -> pIdeInt;
	  Res . IsEvent = True;
	  Res . ReqInfo . Event . EventId = ATA_DEV_EVT_RES_END;


	  if (pStatus != NULL)
	  {
	    pDevice -> pReq = ATA_RelReq (pDevice, pDevice -> pReq);
	  }

	  /* Return Reset Error */
	  while (Device != ATA_MAX_INT_DEV)
	  {
	    pDevice = &(pIdeInt -> Drive [Device]);
	    if (pDevice -> Probe == ATA_DEV_REG)
	    {
	      while (pDevice -> pReq != NULL)
	      {
	        pDevice -> pReq -> Res . IsEvent = False;
	        pDevice -> pReq -> Res . ReqInfo . Status . Error = ATA_ERR_RES;
	        pDevice -> MedHandler (pDevice -> pReq -> MediaHandle, &(pDevice -> pReq -> Res));
	        pDevice -> pReq = ATA_RelReq (pDevice, pDevice -> pReq);
	      }
	      /* Send the Reset End Event */
	      pDevice -> MedHandler (NULL, &Res);

	      pDevice -> PwrMode = ATA_DEV_ACTIVE;
	      pDevice -> Probe = ATA_DEV_INIT;
	    }
	    Device++;
	  }

	  if (FALSE != (pIdeInt -> IsInit))
	  {
	    /* Subscribe with the IDE Controller */
	    pIdeInt -> pIdeOps -> RegHndlr (pIdeInt -> pIntHndl, &ATA_MED_IntHandler);
	    pIdeInt -> IsInit = False;
	  }
	  pIdeInt -> BusState = ATA_IDE_FREE;
	  pIdeInt -> pActiveDrive = NULL;
	}

  return retVal;
}

/**
 *  \brief ATA_MED_IntHandler ()  : ATA Interrupt handler.  Executes in Interrupt context.
 *  \param IntNum [IN]            : Interface on which the interrupt occurred.
 *  \return void
 ******************************************************************************/
static void ATA_MED_IntHandler (Uint32 portNum, ATA_Dev_Req_t *pReq)
{

  ATA_Dev_Stat_t Status;

#ifdef BIOS_PWRM_ENABLE
  volatile PWRM_Event event_type;
#endif

  if ((Ide_Ctlr_Info [portNum]. pActiveDrive != NULL) && (pReq != NULL))
  {
    pReq -> Res . ReqInfo . Status . ErrByte = 0;
    pReq -> Res . ReqInfo . Status . ErrSect = 0;
    Ide_Ctlr_Info [portNum] . pIdeOps -> CmdStatus (Ide_Ctlr_Info [portNum] . 
                                                 pIntHndl, &Status, False, portNum);

    /* update the status in ATA req status field */
    pReq->Status = Status;

    if ( pReq -> Handler != NULL)
    {
	  pReq->CmdReady = 0;
	  pReq->CmdTag = 0;
	  timer0_stop();
      /* Call the respective request handler */
      pReq -> Handler ((Ptr)Ide_Ctlr_Info [portNum]. pActiveDrive, &Status);

    }

	/* check DevState whether IO has suspended or resume state*/
#ifdef BIOS_PWRM_ENABLE
	if( Ide_Ctlr_Info [0].Drive[0].DevState == ATA_DEV_IO_SUSPEND )
	{
		/* suspend the io */
			if( Ide_Ctlr_Info [0].pwrmInfo.ioSuspend  )
			{
				timer3_stop();
				ataPwrm_CallbackFn();
			}
	}
#endif
	}
}
#ifdef BIOS_PWRM_ENABLE
#endif
/**
 *  \brief ATA_ReadErr ()      : Read Error Classifier.
 *  \param pDevice [IN]        : Pointer to the Device.
 *  \param pStatus [IN]        : Command status
 *  \return Bool               : True = Retry, False = Do Not Retry
 ******************************************************************************/
static Bool ATA_ReadErr (ATA_Drive_t const *pDevice, ATA_Dev_Stat_t const *pStatus)
{
  ATA_Dev_Req_t *pReq = NULL;
  Bool IsRetry = False;
  Uint32          StartSect = 0;
  Uint32          TotSect;

  if((NULL != pDevice) && (NULL != pStatus))
  {

	pReq = pDevice -> pReq;
	StartSect = ((pReq -> Command . Device & (0x0Fu)) << 24) |
                               ((pReq -> Command . LbaHigh & (0xFFu)) << 16) |
                               ((pReq -> Command . LbaMid & (0xFFu)) << 8) |
                               (pReq -> Command . LbaLow);

	  if (FALSE != (pStatus -> Error & ATA_DEV_ERR_UADDR))
	  {
	    pReq -> Res . ReqInfo . Status . Error = ATA_ERR_UADDR;
	  } else if (FALSE != (pStatus -> Error & ATA_DEV_ERR_ABRT))
	  {
	    pReq -> Res . ReqInfo . Status . Error = ATA_ERR_DF;
	  } else
	  {
	    pReq -> Res . ReqInfo . Status . Error = ATA_ERR_DF;
	    IsRetry = True;
	  }
	  /* Sector number were the error occurred */
	  pReq -> Res . ReqInfo . Status . ErrSect = ((pStatus -> Device & (0x0Fu)) << 24) |
	          (pStatus -> LbaHigh & (0xFFu) << 16) |
	          (pStatus -> LbaMid & (0xFFu) << 8) |
	          (pStatus -> LbaLow);
	  TotSect = pReq -> Res . ReqInfo . Status . ErrSect - StartSect;

	  pReq -> Res . ReqInfo . Status . ErrByte = (TotSect * pDevice -> Cap . SectSize) +
	                                             (pStatus -> ErrByte % pDevice -> Cap . SectSize);
	}

  return IsRetry;
}


/**
 *  \brief ATA_WriteErr ()     : Write Error Classifier.
 *  \param pDevice [IN]        : Pointer to the Device.
 *  \param pStatus [IN]        : Command status
 *  \return Bool               : True = Retry, False = Do Not Retry
 ******************************************************************************/
static Bool ATA_WriteErr (ATA_Drive_t const *pDevice, ATA_Dev_Stat_t const *pStatus)
{
  ATA_Dev_Req_t *pReq = NULL;
  Bool IsRetry = False;
  Uint32          StartSect = 0;
  Uint32          TotSect;


  if( (NULL != pDevice) && (NULL != pStatus))
  {
	pReq = pDevice -> pReq;
	StartSect = ((pReq -> Command . Device & (0x0Fu)) << 24) |
                               ((pReq -> Command . LbaHigh & (0xFFu)) << 16) |
                               ((pReq -> Command . LbaMid & (0xFFu)) << 8) |
                               (pReq -> Command . LbaLow & (0xFFu));
	  if (FALSE != (pStatus -> Error & ATA_DEV_ERR_UADDR))
	  {
	    pReq -> Res . ReqInfo . Status . Error = ATA_ERR_UADDR;
	  } else if (FALSE != (pStatus -> Error & ATA_DEV_ERR_ABRT))
	  {
	    pReq -> Res . ReqInfo . Status . Error = ATA_ERR_DF;
	  } else if (FALSE != (pStatus -> Error & ATA_DEV_ERR_WP))
	  {
	    pReq -> Res . ReqInfo . Status . Error = ATA_ERR_WP;
	  } else
	  {
	    pReq -> Res . ReqInfo . Status . Error = ATA_ERR_DF;
	    IsRetry = True;
	  }
	  /* Sector number were the error occurred */
	  pReq -> Res . ReqInfo . Status . ErrSect = ((pStatus -> Device & (0x0Fu)) << 24) |
								          ((pStatus -> LbaHigh & (0xFFu))<< 16) |
								          ((pStatus -> LbaMid & (0xFFu)) << 8) |
								          (pStatus -> LbaLow);
	  TotSect = pReq -> Res . ReqInfo . Status . ErrSect - StartSect;

	  pReq -> Res . ReqInfo . Status . ErrByte = (TotSect * pDevice -> Cap . SectSize) +
	                                             (pStatus -> ErrByte % pDevice -> Cap . SectSize);
	}
  return IsRetry;
}


/**
 *  \brief ATA_ReadPio ()  : Read PIO handler.  Executes in Interrupt context.
 *  \param Handle [IN]     : Handle to the Device.
 *  \param pStatus [IN]    : Command status
 *  \return Bool           : True
 ******************************************************************************/
Bool ATA_ReadPio (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
  ATA_Drive_t const *pDevice = NULL;
  ATA_Dev_Req_t *pReq = NULL;
  Uint32 Count = 0;
  Bool IsSched = True;
#ifdef ATA_ATAPI_SUPPORT
  ATA_Dev_Cmd_t Command;
#endif


  if((NULL != Handle) && (NULL != pStatus))
  {

	pDevice = (ATA_Drive_t const *)Handle;
	pReq = pDevice -> pReq;
    if (FALSE != (pStatus -> Stat & ATA_DEV_ERR))
    {
      /* Can we retry this type of error */
      if (FALSE != ATA_ReadErr (pDevice, pStatus))
      {
        pReq -> TransferCount += (Uint32)((Uint8 *)pReq -> Buffer -
                                  (Uint8 *)pReq -> Res . ReqInfo . Status . Buffer);
        pReq -> Buffer = pReq -> Res . ReqInfo . Status . Buffer;

#ifdef ATA_ATAPI_SUPPORT
        /* This an ATAPI PIO command so submit a Packet command before retry */
        if (pReq -> IsPktCmd == True)
        {
          Command.Features = pReq->Command.Features;
          Command.SectorCnt = pReq->Command.SectorCnt;
          Command.LbaLow = pReq->Command.LbaLow;
          Command.LbaMid = pReq->Command.LbaMid;
          Command.LbaHigh = pReq->Command.LbaHigh;
          Command.Device = pReq->Command.Device;
          Command.Cmd = pReq->Command.Cmd;

          pReq -> Command . Cmd = ATAPI_PACKET;
          pReq -> Command . SectorCnt = 0;
          pReq -> Command . LbaLow = 0;
          pReq -> Command . LbaMid = pDevice -> Cap . SectSize;
          pReq -> Command . LbaHigh = pDevice -> Cap . SectSize >> 8;
          pReq -> Command . Features = 0;
          pReq -> Command . Device = pDevice -> Dev;
          pReq -> Handler = &ATAPI_PktHndlr;
        }
#endif

        /* Can this request be retried ?? */
        if (FALSE != ATA_RetryReq (pDevice -> pIdeInt, pReq))
        {

#ifdef ATA_ATAPI_SUPPORT
          /* If we retry a ATAPI requuest now copy the actual Pkt Command into command
           * buffer.
           */
          if (pReq -> IsPktCmd == True)
          {
            pReq->Command.Features = Command.Features;
            pReq->Command.SectorCnt = Command.SectorCnt;
            pReq->Command.LbaLow = Command.LbaLow;
            pReq->Command.LbaMid = Command.LbaMid;
            pReq->Command.LbaHigh = Command.LbaHigh;
            pReq->Command.Device = Command.Device;
            pReq->Command.Cmd = Command.Cmd;
          }
#endif
          IsSched = False;
        }
      }
    } else
    {
      if (pReq -> TransferCount > pReq -> BlockCount)
      {
        Count = pReq -> BlockCount;
      } else
      {
        Count = pReq -> TransferCount;
      }

      pDevice -> pIdeInt -> pIdeOps -> Read (pDevice -> pIdeInt -> pIntHndl,
                                             pReq -> Buffer, Count);
      /* Decrement the transferred count accordingly */
      pReq -> TransferCount -= Count;

      if (pReq -> TransferCount == 0)
      {
        if ((Uint32)pReq -> ExtReqInfo != 0)
        {
          pReq -> Buffer = ((Uint8 *)pReq -> Buffer) + Count;

          /* We have a request that spans more than ATA_MAX_SECT sectors.  So we
           * re-use this request token and re-program the Taskfile for the next
           * ATA_MAX_SECT transaction.
           */
          ATA_SubmitExtReq (pDevice, pReq);
          IsSched = False;
        } else
        {
#ifdef ATA_ATAPI_SUPPORT
          /* Wait for the last interrupt if it is ATAPI device
           */
          if ((pReq -> IsPktCmd == True) && ((pStatus -> Stat & ATA_DEV_REQ) != False))
          {
            IsSched = False;

			timer0_start( pReq -> Timeout );
          } else
          {
#endif
            pReq -> Res . ReqInfo . Status . Error = ATA_NO_ERR;
            IsSched = True;
#ifdef ATA_ATAPI_SUPPORT
          }
#endif
        }
      } else
      {
        IsSched = False;
        pReq -> Buffer = ((Uint8 *)pReq -> Buffer) + Count;

//		sta_alm (tmrObj -> id, pReq -> Timeout);
		timer0_start( pReq -> Timeout );
      }
    }

  if (FALSE != IsSched)
  {
    ATA_SchedNextReq (pDevice -> pIdeInt, pReq);
  }

  }
  return True;
}


/**
 *  \brief ATA_WritePio ()  : Write PIO handler.  Executes in Interrupt context.
 *  \param Handle [IN]      : Handle to the Device.
 *  \param pStatus [IN]     : Command status
 *  \return Bool            : True
 ******************************************************************************/
Bool ATA_WritePio (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
  ATA_Drive_t const *pDevice = NULL;
  ATA_Dev_Req_t *pReq = NULL;
  Uint32 Count = 0;
  Bool IsSched = True;
#ifdef ATA_ATAPI_SUPPORT
  ATA_Dev_Cmd_t Command;
#endif
  

  if((NULL != Handle) && (NULL != pStatus))
  {
	pDevice = (ATA_Drive_t const *)Handle;
	pReq = pDevice -> pReq;

  if (pReq -> TransferCount > pReq -> BlockCount)
  {
    Count = pReq -> BlockCount;
  } else
  {
    Count = pReq -> TransferCount;
  }

  if (FALSE != (pStatus -> Stat & ATA_DEV_ERR))
  {
    if (FALSE != ATA_WriteErr (pDevice, pStatus))
    {
      pReq -> TransferCount += ((Uint8 *)pReq -> Buffer -
                   (Uint8 *)pReq -> Res . ReqInfo . Status . Buffer);
      pReq -> Buffer = pReq -> Res . ReqInfo . Status . Buffer;

#ifdef ATA_ATAPI_SUPPORT
      /* This an ATAPI PIO command so submit a Packet command before retry */
      if (pReq -> IsPktCmd == True)
      {
        Command.Features = pReq->Command.Features;
        Command.SectorCnt = pReq->Command.SectorCnt;
        Command.LbaLow = pReq->Command.LbaLow;
        Command.LbaMid = pReq->Command.LbaMid;
        Command.LbaHigh = pReq->Command.LbaHigh;
        Command.Device = pReq->Command.Device;
        Command.Cmd = pReq->Command.Cmd;

        pReq -> Command . Cmd = ATAPI_PACKET;
        pReq -> Command . SectorCnt = 0;
        pReq -> Command . LbaLow = 0;
        pReq -> Command . LbaMid = pDevice -> Cap . SectSize;
        pReq -> Command . LbaHigh = pDevice -> Cap . SectSize >> 8;
        pReq -> Command . Features = 0;
        pReq -> Command . Device = pDevice -> Dev;
        pReq -> Handler = &ATAPI_PktHndlr;
      }
#endif

      /* Can this request be retried ?? */
      if (FALSE != ATA_RetryReq (pDevice -> pIdeInt, pReq))
      {
        IsSched = False;

#ifdef ATA_ATAPI_SUPPORT
        /* If we retry a ATAPI requuest now copy the actual Pkt Command into command
         * buffer.
         */
        if (pReq -> IsPktCmd == True)
        {
          pReq->Command.Features = Command.Features;
          pReq->Command.SectorCnt = Command.SectorCnt;
          pReq->Command.LbaLow = Command.LbaLow;
          pReq->Command.LbaMid = Command.LbaMid;
          pReq->Command.LbaHigh = Command.LbaHigh;
          pReq->Command.Device = Command.Device;
          pReq->Command.Cmd = Command.Cmd;
        } else
#endif
        {
          if (ATA_PioBeginWrite (pDevice, Count) == ATA_ERR_GEN)
          {
            IsSched = True;
          }
        }
      }
    }
  } else
  {
    if (pReq -> TransferCount != 0)
    {
      /* Decrement the transferred count accordingly */
      pReq -> TransferCount -= Count;

      pDevice -> pIdeInt -> pIdeOps -> Write (pDevice -> pIdeInt -> pIntHndl,
                                              pReq -> Buffer, Count);
      pReq -> Buffer = ((Uint8 *)pReq -> Buffer) + Count;
      IsSched = False;
    } else
    {
      if ((Uint32)pReq -> ExtReqInfo != 0)
      {
        pReq -> Buffer = ((Uint8 *)pReq -> Buffer) + Count;

        /* We have a request that spans more than ATA_MAX_SECT sectors.  So we
         * re-use this request token and re-program the Taskfile for the next
         * ATA_MAX_SECT transaction.
         */
        ATA_SubmitExtReq (pDevice, pReq);

        /* Try writing the first SectorSize worth of Data */
        if (ATA_PioBeginWrite (pDevice, pDevice -> Cap . SectSize) == ATA_ERR_GEN)
        {
          IsSched = True;
        } else 
		{
		  IsSched = False;
        }
			
      } else
      {
        pReq -> Res . ReqInfo . Status . Error = ATA_NO_ERR;
        IsSched = True;
      }
    }
  }
  if (IsSched==False)
  {

	timer0_start( pReq -> Timeout );
  }else 
  {
	  if (FALSE != IsSched)
	  {
	    ATA_SchedNextReq (pDevice -> pIdeInt, pReq);
	  }
  }
	}
  return True;
}

/**
 *  \brief ATA_ReadDma ()   : Read DMA handler.  Executes in Interrupt context.
 *  \param Handle [IN]      : Handle to the Device.
 *  \param pStatus [IN]     : Command status
 *  \return Bool            : True
 ******************************************************************************/
Bool ATA_ReadDma (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
  ATA_Drive_t const *pDevice = NULL;
  ATA_Dev_Req_t *pReq = NULL;
  Bool IsSched = True;
#ifdef ATA_ATAPI_SUPPORT
  ATA_Dev_Cmd_t Command;
#endif
	if((NULL != Handle) && (NULL != pStatus))
	{
		pDevice = (ATA_Drive_t const *)Handle;
		pReq = pDevice -> pReq;
	  /* Do something special for ending the DMA request */
	  pDevice -> pIdeInt -> pIdeOps -> EndReq (pDevice -> pIdeInt -> pIntHndl);
	  if (FALSE != (pStatus -> Stat & ATA_DEV_ERR))
	  {
	    if (FALSE != ATA_ReadErr (pDevice, pStatus))
	    {
	#ifdef ATA_ATAPI_SUPPORT
	      /* This an ATAPI PIO command so submit a Packet command before retry */
	      if (pReq -> IsPktCmd == True)
	      {
	        Command.Features = pReq->Command.Features;
	        Command.SectorCnt = pReq->Command.SectorCnt;
	        Command.LbaLow = pReq->Command.LbaLow;
	        Command.LbaMid = pReq->Command.LbaMid;
	        Command.LbaHigh = pReq->Command.LbaHigh;
	        Command.Device = pReq->Command.Device;
	        Command.Cmd = pReq->Command.Cmd;

	        pReq -> Command . Cmd = ATAPI_PACKET;
	        pReq -> Command . SectorCnt = 0;
	        pReq -> Command . LbaLow = 0;
	        pReq -> Command . LbaMid = pDevice -> Cap . SectSize;
	        pReq -> Command . LbaHigh = pDevice -> Cap . SectSize >> 8;
	        pReq -> Command . Features = 1;
	        pReq -> Command . Device = pDevice -> Dev;
	        pReq -> Handler = &ATAPI_PktHndlr;
	      }
	#endif
	      /* Can this request be retried ?? */
	      if (ATA_RetryReq (pDevice -> pIdeInt, pReq) == True)
	      {
	#ifdef ATA_ATAPI_SUPPORT
	        /* If we retry a ATAPI requuest now copy the actual Pkt Command into command
	         * buffer.
	         */
	        if (pReq -> IsPktCmd == True)
	        {
	          pReq->Command.Features = Command.Features;
	          pReq->Command.SectorCnt = Command.SectorCnt;
	          pReq->Command.LbaLow = Command.LbaLow;
	          pReq->Command.LbaMid = Command.LbaMid;
	          pReq->Command.LbaHigh = Command.LbaHigh;
	          pReq->Command.Device = Command.Device;
	          pReq->Command.Cmd = Command.Cmd;
	        }
	#endif
	        pDevice -> pIdeInt -> pIdeOps -> StartReq (pDevice -> pIdeInt -> pIntHndl, pReq);
	        IsSched = False;
	      }
	    }
	  } else
	  {
	    if ((Uint32)pReq -> ExtReqInfo != 0)
	    {
	      pReq -> Buffer = ((Uint8 *)pReq -> Buffer) + (ATA_MAX_SECT * pDevice -> Cap. SectSize);

	      /* We have a request that spans more than ATA_MAX_SECT sectors.  So we
	       * re-use this request token and re-program the Taskfile for the next
	       * ATA_MAX_SECT transaction.
	       */
	      ATA_SubmitExtReq (pDevice, pReq);
	      pDevice -> pIdeInt -> pIdeOps -> StartReq (pDevice -> pIdeInt -> pIntHndl, pReq);
	      IsSched = False;
	    } else
	    {
	      pReq -> Res . ReqInfo . Status . Error = ATA_NO_ERR;
	    }
	  }

	  if (FALSE != IsSched)
	  {
	    ATA_SchedNextReq (pDevice -> pIdeInt, pReq);
	  }
	}

  return True;
}


/**
 *  \brief ATA_WriteDma ()  : Write DMA handler.  Executes in Interrupt context.
 *  \param Handle [IN]      : Handle to the Device.
 *  \param pStatus [IN]     : Command status
 *  \return Bool            : True
 ******************************************************************************/
Bool ATA_WriteDma (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
  ATA_Drive_t const *pDevice = NULL;
  ATA_Dev_Req_t *pReq = NULL;
  Bool IsSched = True;
#ifdef ATA_ATAPI_SUPPORT
  ATA_Dev_Cmd_t Command;
#endif

	if((NULL != Handle) && (NULL != pStatus))
	{
		pDevice = (ATA_Drive_t const *)Handle;
		pReq = pDevice -> pReq;
	  /* Do something special for ending the DMA request */
	  pDevice -> pIdeInt -> pIdeOps -> EndReq (pDevice -> pIdeInt -> pIntHndl);
	  if (FALSE != (pStatus -> Stat & ATA_DEV_ERR))
	  {
	    if (FALSE != ATA_WriteErr (pDevice, pStatus))
	    {
	#ifdef ATA_ATAPI_SUPPORT
	      /* This an ATAPI PIO command so submit a Packet command before retry */
	      if (pReq -> IsPktCmd == True)
	      {
	        Command.Features = pReq->Command.Features;
	        Command.SectorCnt = pReq->Command.SectorCnt;
	        Command.LbaLow = pReq->Command.LbaLow;
	        Command.LbaMid = pReq->Command.LbaMid;
	        Command.LbaHigh = pReq->Command.LbaHigh;
	        Command.Device = pReq->Command.Device;
	        Command.Cmd = pReq->Command.Cmd;

	        pReq -> Command . Cmd = ATAPI_PACKET;
	        pReq -> Command . SectorCnt = 0;
	        pReq -> Command . LbaLow = 0;
	        pReq -> Command . LbaMid = pDevice -> Cap . SectSize;
	        pReq -> Command . LbaHigh = pDevice -> Cap . SectSize >> 8;
	        pReq -> Command . Features = 1;
	        pReq -> Command . Device = pDevice -> Dev;
	        pReq -> Handler = &ATAPI_PktHndlr;
	      }
	#endif

	      /* Can this request be retried ?? */
	      if (FALSE != ATA_RetryReq (pDevice -> pIdeInt, pReq))
	      {

	#ifdef ATA_ATAPI_SUPPORT
	        /* If we retry a ATAPI requuest now copy the actual Pkt Command into command
	         * buffer.
	         */
	        if (pReq -> IsPktCmd == True)
	        {
	          pReq->Command.Features = Command.Features;
	          pReq->Command.SectorCnt = Command.SectorCnt;
	          pReq->Command.LbaLow = Command.LbaLow;
	          pReq->Command.LbaMid = Command.LbaMid;
	          pReq->Command.LbaHigh = Command.LbaHigh;
	          pReq->Command.Device = Command.Device;
	          pReq->Command.Cmd = Command.Cmd;
	        }
	#endif

	        pDevice -> pIdeInt -> pIdeOps -> StartReq (pDevice -> pIdeInt -> pIntHndl, pReq);
	        IsSched = False;
	      }
	    }
	  } else
	  {
	      if ((Uint32)pReq -> ExtReqInfo != 0)
	      {
	        pReq -> Buffer = ((Uint8 *)pReq -> Buffer) + (ATA_MAX_SECT * pDevice -> Cap. SectSize);

	        /* We have a request that spans more than ATA_MAX_SECT sectors.  So we
	         * re-use this request token and re-program the Taskfile for the next
	         * ATA_MAX_SECT transaction.
	         */
	        ATA_SubmitExtReq (pDevice, pReq);
	        pDevice -> pIdeInt -> pIdeOps -> StartReq (pDevice -> pIdeInt -> pIntHndl, pReq);
	        IsSched = False;
	      } else
	      {
	        pReq -> Res . ReqInfo . Status . Error = ATA_NO_ERR;
	      }
	  }
	  if (IsSched == True)
	  {
	    ATA_SchedNextReq (pDevice -> pIdeInt, pReq);
	  }
	}

  return True;
}


/**
 *  \brief ATA_Taskfile ()  : Taskfile commmand  handler.  Executes in Interrupt context.
 *                            Misc. handler for Taskfile based request.
 *                            Ex. Power Req.
 *  \param Handle [IN]      : Handle to the Device.
 *  \param pStatus [IN]     : Command status
 *  \return Bool            : True
 ******************************************************************************/
Bool ATA_Taskfile (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
  ATA_Drive_t *pDevice = NULL;
  ATA_Dev_Req_t *pReq = NULL;

  if((NULL != Handle) && (NULL != pStatus))
  {
	pDevice = (ATA_Drive_t *)(Uint32)Handle;
  	pReq = pDevice -> pReq;
	  if (FALSE == (pStatus -> Stat & ATA_DEV_ERR))
	  {
	    switch (pReq -> Command . Cmd)
	    {
	      case ATA_PWR_SLEEP:
	        pDevice -> PwrMode = ATA_DEV_SLEEP;
	        ATA_SetDevops (pDevice);
	        break;
	      case ATA_PWR_STDBY:
	        pDevice -> PwrMode = ATA_DEV_STDBY;
	        ATA_SetDevops (pDevice);
	        break;
	      case ATA_PWR_IDLE:
	        pDevice -> PwrMode = ATA_DEV_IDLE;
	        ATA_SetDevops (pDevice);
	        break;
	      case ATA_PWR_CHECK:
	        switch (pStatus -> IntReason)
	        {
	        case 0:
	          *((ATA_DEV_PwrMode_t *)pReq -> Buffer) = ATA_DEV_STDBY;
	          break;
	        case 0x80:
	          *((ATA_DEV_PwrMode_t *)pReq -> Buffer) = ATA_DEV_IDLE;
	          break;
	        default:
	          *((ATA_DEV_PwrMode_t *)pReq -> Buffer) = ATA_DEV_ACTIVE;
	          break;
	        }
	        pReq -> Res . ReqInfo . Status . Error = ATA_NO_ERR;
	        break;
	      default :
	        pReq -> Res . ReqInfo . Status . Error = ATA_NO_ERR;
	        break;
	    }
	  } else
	  {
	    if (pReq -> Command . Cmd == ATA_GET_MEDSTAT)
	    {
	      *((Uint8 *)pReq -> Buffer) = pStatus -> Error;
	    }

	    pReq -> Res . ReqInfo . Status . Error = ATA_ERR_DF;
	    pReq -> Res . IsEvent = False;
	  }
      /* Set this flag to signal the caller that this request is done */
	  pDevice -> pReq -> Retry = 0;

	  ATA_SchedNextReq (pDevice -> pIdeInt, pReq);
	}

  return True;
}


/**
 *  \brief ATA_MED_UnRegister ()  : Free the requested device on the ATA Bus.
 * If the device has a active request we return ATA_ERR_GEN.  The caller
 * can either wait for this active request to complete or do a "DeInit"
 * (under critical power conditions) inspite of this error.  Care should be
 * taken by the caller not to free the outstanding request information as it
 * might complete at any point in time.  In this case if the caller is not
 * willing to wait then the caller can call "DeInit" of ATA and "Init of ATA"
 * subsystem.  Alternately if this command does not complete then the normal Timeout
 * mechanism kicks in and resets the bus to bring back the system to its
 * normal operating self.
 *
 *
 *  \param  Handle [IN]      : Handle to device.
 *
 *  \return int                   : Error/Success
 ******************************************************************************/
Int32 ATA_MED_UnRegister (Ptr Handle)
{
  ATA_Drive_t *pDevice = NULL;
  Int32 Err = ATA_ERR_GEN;

  if (Handle != NULL)
  {
	pDevice = (ATA_Drive_t *)Handle;
	if(pDevice -> pReq == NULL)
	{
	    /* Void the devops */
	    ATA_VoidDevops (pDevice);
	    pDevice -> Probe = ATA_DEV_AVAIL;
	    pDevice -> MedHandler = NULL;
	    Err = ATA_NO_ERR;
	}
  }

  return Err;
}


/**
 *  \brief ATA_MED_Register ()  : Identify the requested device on the ATA Bus and
 *                                register for that device (Media) with ATA Interface.
 *  \param pDeviceIdent [IN]    : Device Indentification Information.
 *  \param MediaCallback [IN]   : Callback routine for the registered Media Driver.
 *  \param pAtaIntOps [OUT]     : ATA Interface API's.
 *  \param pHandle    [OUT]     : Handle to device (to be used for future
 *                                transactions).
 *  \return int                 : Success/Error
 ******************************************************************************/
Int32 ATA_MED_Register (ATA_DEV_Ident_t *pDeviceIdent, ATA_MED_Callback_t MediaCallback,
                      ATA_INT_Ops_t **pAtaIntOps, Ptr *pHandle)
{
  Int32 Result = ATA_ERR_GEN;
  Int32 Index = 0, Device = 0;
  ATA_Dev_Stat_t Status;


  if((((NULL != pDeviceIdent) && (NULL != MediaCallback)) && (NULL != pAtaIntOps)) && (NULL != pHandle))
  {

	  *pAtaIntOps = NULL;
  	  *pHandle = NULL;
	
	  while (((Index < ATA_MAX_IDE_INT) && (Ide_Ctlr_Info [Index] . pIdeOps != NULL)) &&
	         (Ide_Ctlr_Info [Index] . BusState == ATA_IDE_FREE))
	  {
	    for (Device = 0; Device < ATA_MAX_INT_DEV; Device++)
	    {
	      if (pDeviceIdent != NULL)
	      {
	        if (FALSE != (pDeviceIdent -> IsMaster))
	        {
	          if (Device != 0)
	          {
	            break;
	          }
	        } else
	        {
	          if (Device == 0)
	          {
	            continue;
	          }
	        }
	      }

	      if ((Ide_Ctlr_Info [Index] . Drive [Device] . Probe == ATA_DEV_REG) ||
	          (Ide_Ctlr_Info [Index] . Drive [Device] . Probe == ATA_DEV_NAVAIL))
	      {
	        /*  The device is either already taken or not available */
	        continue;
	      }
	      if (Ide_Ctlr_Info [Index] . Drive [Device] . Probe == ATA_DEV_AVAIL)
	      {
	        ATA_MED_ReRegister (pDeviceIdent, MediaCallback, pAtaIntOps, pHandle,
	                          &Ide_Ctlr_Info [Index] . Drive [Device]);
	        if ((*pHandle) == NULL)
	        {
	          continue;
	        } else
	        {
	          Result = ATA_NO_ERR;
	          break;
	        }
	      }

	      Ide_Ctlr_Info [Index] . Drive [Device] . Dev = (Uint8)(Device << 4);
	      /* First submit a ATA_IDENTIFY command */
	      ATA_SubmitIdentify (&Ide_Ctlr_Info [Index] . Drive [Device], &Status, True);

	      if (FALSE != (Status . Stat & ATA_DEV_ERR))
	      {
	        /* Look for ATAPI device signature if so issue a ATAPI_IDENTIFY command */
	        if ((Status . LbaMid == (0x14u)) && (Status . LbaHigh == (0xEBu)))
	        {
	          ATA_SchedNextReq (&Ide_Ctlr_Info [Index], Ide_Ctlr_Info [Index] . Drive [Device] . pReq);
	          /* First submit a ATAPI_IDENTIFY command */
	          ATA_SubmitIdentify (&Ide_Ctlr_Info [Index] . Drive [Device], &Status, False);

	          if (((FALSE == (Status . Stat & ATA_DEV_ERR)) && ((Status . Stat & ATA_DEV_REQ)!=FALSE)) &&
	              (FALSE == (Status . Stat & ATA_DEV_BUSY)))
	          {
	            Ide_Ctlr_Info [Index] . Drive [Device] . Cap . IsAta = False;
	            ATA_MED_Identify (pDeviceIdent, MediaCallback, pAtaIntOps, pHandle,
	                              &Ide_Ctlr_Info [Index] . Drive [Device]);
	          }
	        }
	      } else
		  {
		       if (((Status . Stat & ATA_DEV_REQ)!=FALSE) && (FALSE == (Status . Stat & ATA_DEV_BUSY)))
		      {
		        Ide_Ctlr_Info [Index] . Drive [Device] . Cap . IsAta = True;
		        ATA_MED_Identify (pDeviceIdent, MediaCallback, pAtaIntOps, pHandle,
		                          &Ide_Ctlr_Info [Index] . Drive [Device]);
		      }
		  }

	      if ((*pHandle) != NULL)
	      {
	        Result = ATA_NO_ERR;
	        break;
	      } else
	      {
	        if (Ide_Ctlr_Info [Index] . Drive [Device] . Probe == ATA_DEV_INIT)
	        {
	          Ide_Ctlr_Info [Index] . Drive [Device] . Probe = ATA_DEV_NAVAIL;
	        }
	        ATA_SchedNextReq (&Ide_Ctlr_Info [Index], Ide_Ctlr_Info [Index] . Drive [Device] . pReq);
	      }	

	    }

	    if (Result == ATA_NO_ERR)
	    {
	      Ide_Ctlr_Info [Index] . pIdeOps -> RegHndlr (pHandle, &ATA_MED_IntHandler);
	      break;
	    }

	    Device = 0;
	    Result = ATA_ERR_GEN;
	    Index++;
	  }
	}

  return Result;
}

/**
 *  \brief ATA_SubmitIdentify ()  : Submit the identify command.
 *
 *  \param pDevice [IN]           : Device Indentification Information.
 *  \param pStatus [IN]           : Status Pointer.
 *  \param IsAta   [IN]           : Is ATA_IDENTIFY ??
 *  \return void                  :
 ******************************************************************************/
static void ATA_SubmitIdentify (ATA_Drive_t *pDevice, ATA_Dev_Stat_t * const pStatus, Bool IsAta)
{
  ATA_Dev_Req_t *pReq;

  if((NULL != pDevice) && (NULL != pStatus))
  {
	  pReq = ATA_AllocReq (pDevice, False);

	  pReq -> Buffer = (Ptr) DriveIdent;
	  if (IsAta == True)
	  {
	    pReq -> Command . Cmd = ATA_IDENTIFY;
	  } else
	  {
	    pReq -> Command . Cmd = ATAPI_IDENTIFY;
	  }
	  pReq -> Command . Device = pDevice -> Dev;
	  pReq -> TransferCount = sizeof (DriveIdent);
	  pReq -> BlockCount = sizeof (DriveIdent);
	  pReq -> Mode = TASKFILE;
	  pReq -> Handler = &ATA_IdentHndlr1;
	  pReq -> ToHndlr = &ATA_IdentHndlr1;
	  pReq -> Timeout = ATA_ONESEC_TO;
	  pReq -> IsInternal = True;
	  pReq -> Retry   = 10u;

	  
	  ATA_SubmitReq (pDevice, pReq);

	  while (pReq -> Retry != 0)
	  {
	    //PAL_osWaitMsecs (1u);
		m_delay(1u);
		//pReq -> Retry--;
	  }

	  pDevice -> pIdeInt -> pIdeOps -> CmdStatus (pDevice -> pIdeInt -> pIntHndl, pStatus, False, pReq->PortNum);

	  if (pReq -> Res . ReqInfo . Status . Error == ATA_ERR_TO)
	  {
	    /* Inform that this request timeout out and the status is not valid */
	    pStatus -> Error = 0xFFu;
	    /* Set the Error Status on the device status */
	    pStatus -> Stat  |= ATA_DEV_ERR;

	  }
	}
}
/**
 *  \brief ATA_IdentHndlr ()  : Handle the Media Identification completion.
 *  \param Handle [IN]      : Handle to the Device.
 *  \param pStatus [IN]     : Command status
 *  \return Bool            : False
 ******************************************************************************/
Bool ATA_IdentHndlr (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
  ATA_Drive_t const *pDevice = NULL;
  
  if((NULL != Handle) && (NULL != pStatus))
  {
	pDevice = (ATA_Drive_t const *)Handle;
	  pDevice -> pReq -> Retry--;
	  if (((pStatus -> Stat & ATA_DEV_BUSY)!=FALSE) && (pDevice -> pReq -> Retry > 0))
	  {
	    pDevice -> pIdeInt -> TimeOutData = (Ptr)pDevice;    

   	    timer0_start( pDevice -> pReq -> Timeout );
	  } else
	  {
	    if (pDevice -> pReq -> Retry == 0)
	    {
	      pDevice -> pReq -> Res . ReqInfo . Status . Error = ATA_ERR_TO;
	    }

	    /* Set this flag to signal the caller that this request is done */
	    pDevice -> pReq -> Retry = 0;
	  }
	}

  return False;
  //return True;
}
/**
 *  \brief ATA_IdentHndlr ()  : Handle the Media Identification completion.
 *  \param Handle [IN]      : Handle to the Device.
 *  \param pStatus [IN]     : Command status
 *  \return Bool            : False
 ******************************************************************************/
Bool ATA_IdentHndlr1 (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
  ATA_Drive_t const *pDevice = NULL;
  
  if((NULL != Handle) && (NULL != pStatus))
  {
	pDevice = (ATA_Drive_t const *)Handle;
	  pDevice -> pReq -> Retry--;
	  if (((pStatus -> Stat & ATA_DEV_BUSY)!=FALSE) && (pDevice -> pReq -> Retry > 0))
	  {
	    pDevice -> pIdeInt -> TimeOutData = (Ptr)pDevice;    

   	    timer0_start( pDevice -> pReq -> Timeout );
	  } else
	  {
	    if (pDevice -> pReq -> Retry == 0)
	    {
	      pDevice -> pReq -> Res . ReqInfo . Status . Error = ATA_ERR_TO;
	    }

	    /* Set this flag to signal the caller that this request is done */
	    pDevice -> pReq -> Retry = 0;
		pDevice -> pIdeInt -> BusState = ATA_IDE_FREE;
		ATA_RelReq(pDevice->pIdeInt->pActiveDrive, pDevice -> pReq);

	  }
	}

  return False;
}
/**
 *  \brief ATA_MED_Identify ()  : Populate the device capabilities and set the
 *                                default opmode of device to PIO.
 *  \param pDeviceIdent   [IN]  : Device Indentification Information.
 *  \param MediaCallback  [IN]  : Callback routine for the registered Media Driver.
 *  \param pAtaIntOps     [OUT] : ATA Interface API's.
 *  \param pHandle        [OUT] : Handle to device (to be used for future
 *                                transactions).
 *  \param pDevice        [IN]  : Device Handle
 *  \return void                :
 ******************************************************************************/
static void ATA_MED_Identify (ATA_DEV_Ident_t * const pDeviceIdent, ATA_MED_Callback_t MediaCallback,
                       ATA_INT_Ops_t **pAtaIntOps, Ptr * const pHandle, ATA_Drive_t *pDevice)
{
  ATA_Dev_Cap_t *pCap;
  Uint16 count;
  Uint16 drive_count;

  if((NULL != pDeviceIdent) && (((NULL != MediaCallback) && (NULL != pAtaIntOps)) && ((NULL != pHandle) && (NULL != pDevice))))
  {
	  pCap = &pDevice -> Cap;
	  /* Obtain the Device capabilites needed */
	/*  pDevice -> pIdeInt -> pIdeOps -> Read (pDevice -> pIdeInt -> pIntHndl,
	                                         (Ptr) DriveIdent, sizeof (DriveIdent));
	  ATA_SchedNextReq (pDevice -> pIdeInt, pDevice -> pReq);
    */
	  /* Device Found */
	  /* For now just set register to True */

	  /* Verify the Ident details and then set the info */

	  pDevice -> Probe = ATA_DEV_REG;
	  pDevice -> MedHandler = MediaCallback;
	  *pAtaIntOps = &(pDevice -> DevOps);
	  *pHandle = (Ptr *)(Uint32)pDevice;

	  pDevice -> OpMode = PIO;
	  pDevice -> Level = 0;
	  /* Set the default Sector Size to ATA_DEV_BPS */
	  pDevice -> Cap . SectSize = ATA_DEV_BPS;
	  /* By default enable LBA mode for this device transactions */
	  pDevice -> Dev = (Uint8)((pDevice -> Dev) | ATA_DEV_LBA);

	  /* Max Sector transferred per READ/WRITE Multiple request */
	  if (FALSE != (DriveIdent [59] & (0x80u)))
	  {
	    pCap -> SectCnt = DriveIdent [59] & (0x4Fu);
	  }

	  count = 0;
	  do
	  {
	    drive_count = count/(2u);
	    pCap->FirmwareRev[count] = (Uint16)(DriveIdent[drive_count + 23] >> 8);
	    pCap->SerialNo[count]    = (Uint16)(DriveIdent[drive_count + 10] >> 8u);
	    pCap->ModelNo[count]     = (Uint16)(DriveIdent[drive_count + 27] >> 8u);
	    ++count;

	    pCap->FirmwareRev[count] = (Uint16)(DriveIdent[drive_count + 23]);
	    pCap->SerialNo[count]    = (Uint16)(DriveIdent[drive_count + 10]);
	    pCap->ModelNo[count]     = (Uint16)(DriveIdent[drive_count + 27]);
	    ++count;
	  }while (count < (8u));

	  do
	  {
	    drive_count = count/(2u);
	    pCap->SerialNo[count]    = (Uint8)(DriveIdent[drive_count + 10] >> 8u);
	    pCap->ModelNo[count]     = (Uint8)(DriveIdent[drive_count + 27] >> 8u);
	    ++count;

	    pCap->SerialNo[count]    = (Uint8)(DriveIdent[drive_count + 10]);
	    pCap->ModelNo[count]     = (Uint8)(DriveIdent[drive_count + 27]);
	    ++count;
	  }while (count < (20u));

	  do
	  {
	    drive_count = count/(2u);
	    pCap->ModelNo[count]     = (Uint8)(DriveIdent[drive_count + 27] >> 8u);
	    ++count;

	    pCap->ModelNo[count]     = (Uint8)(DriveIdent[drive_count + 27]);
	    ++count;
	  }while (count < (40u));

	#ifdef ATA_REMOVABLE_MEDIA_SUPPORT
	  /* Removable Media Device */
	  if (FALSE != (DriveIdent [0] & (0x80u)))
	  {
	    pCap -> IsMedRem = True;
	  }
	#endif

	  if (FALSE != (DriveIdent [53] & ATA_DEV_CYCLE_TIME))
	  {
	    pCap -> PioCycle = DriveIdent [67];
	    pCap -> PioCycleIordy = DriveIdent [68];
	    pCap -> DmaCycle = DriveIdent [65];
	    /* We work with only PIO mode 3 and 4 */
	    pCap -> Pio = (DriveIdent [64] & (0x2u)) ? ATA_PIO_4 :
	                  ((DriveIdent [64] & (0x1u)) ? ATA_PIO_3 : 0);

	    pCap -> Dma = (DriveIdent [63] & (0x4u)) ? ATA_DMA_2 : 
						((DriveIdent [63] & (0x2u)) ? ATA_DMA_1 :
	                  				((DriveIdent [63] & (0x1u)) ? ATA_DMA_0 : 0xFFu));
	  }

	  if (FALSE != (DriveIdent [53] & ATA_DEV_UDMA))
	  {
	    pCap -> Udma = (DriveIdent [88] & (0x20u)) ? ATA_UDMA_5 : ((DriveIdent [88] & (0x10u)) ? ATA_UDMA_4 :
	               ((DriveIdent [88] & (0x8u)) ? ATA_UDMA_3 : ((DriveIdent [88] & (0x4u)) ? ATA_UDMA_2 :
	               ((DriveIdent [88]  & (0x2u)) ? ATA_UDMA_1: ((DriveIdent [88] & (0x1u)) ? ATA_UDMA_0 : 0xFFu)))));
	  } else
	  {
	    pCap -> Udma = 0xFFu;
	  }

	  if (FALSE != (DriveIdent [53] & ATA_DEV_CHS))
	  {
	    pCap -> Cyl = DriveIdent [54];
	    pCap -> Head = DriveIdent [55];
	    pCap -> SectperTrack = DriveIdent [56];
	  }

	  /* We currently only support basic power management for now */
	  if (((DriveIdent [82] & (0x8u))!=FALSE) && ((DriveIdent [85] & (0x8u))!=FALSE))
	  {
	    pCap -> IsPowerMan = True;
	  }

	  pCap -> QueueDepth = DriveIdent [75];
	  pCap -> MaxLba = (((Uint32)DriveIdent [61]) << 16) | DriveIdent [60];
	  if (pCap -> MaxLba == 0)
	  {
	    //PAL_osWaitMsecs (1u);
		m_delay(1u);
	    ATA_MaxLba (pDevice);
	  }

	  /* If ATAPI device identify the command size and DRQ behaviour */
	  if (pCap -> IsAta == False)
	  {
	    if (FALSE != (DriveIdent [0] & (0x20u)))
	    {
	      pCap -> IsDrqInt = True;
	    }
	    if (FALSE != (DriveIdent [0] & (1u)))
	    {
	      pCap -> AtapiCmdSize = 16u;
	    } else
	    {
	      pCap -> AtapiCmdSize = 12u;
	    }
	  }
	  /* Set Default retry count */
	  pDevice -> RetryCnt = 0;

	#ifdef ATA_REMOVABLE_MEDIA_SUPPORT
	  /* Removable media support */
	  if (FALSE != (pCap -> IsMedRem))
	  {
	    if (FALSE != (DriveIdent [83] & (0x10u)))
	    {
	      /* First identify whether Get Media Status command is supported */
	      ATA_MED_RemIdtfy (pDevice, True);
	    }
	    if ((pDevice -> Cap . IsAta == False) && (pCap -> RemMedStatus == False))
	    {
	      pCap -> IsMedRem = False;
	    }
	  }

	  if (((pDevice -> Cap . IsAta == True) && (pCap -> IsMedRem == True)) && (pCap -> RemMedStatus == False))
	  {
	    if (FALSE != (DriveIdent [82] & (0x4u)))
	    {
	      ATA_MED_RemIdtfy (pDevice, False);
	    } else
	    {
	      pCap -> IsMedRem = False;
	    }
	  }
	#endif /* ATA_REMOVABLE_MEDIA_SUPPORT */

	  if (pDeviceIdent != NULL)
	  {
	    pDeviceIdent -> Cyl = pCap -> Cyl;
	    pDeviceIdent -> Head = pCap -> Head;
	    pDeviceIdent -> SectperTrack = pCap -> SectperTrack;
	    count = 0;
	    while (count < (8u))
	    {
	      pDeviceIdent->FirmwareRev[count] = pCap -> FirmwareRev[count];
	      pDeviceIdent->SerialNo[count]    = pCap -> SerialNo[count];
	      pDeviceIdent->ModelNo[count]     = pCap -> ModelNo[count];
	      ++count;
	    }

	    while (count < (20u))
	    {
	      pDeviceIdent->SerialNo[count]    = pCap -> SerialNo[count];
	      pDeviceIdent->ModelNo[count]     = pCap -> ModelNo[count];
	      ++count;
	    }

	    while (count < (40u))
	    {
	      pDeviceIdent->ModelNo[count]     = pCap -> ModelNo[count];
	      ++count;
	    }
	  }
	  /* Initialize the Devops for a PIO mode operation */
	  pDevice -> DevOps . Ata_Read = &ATA_MED_ReadDma;//&ATA_MED_Read;
	  pDevice -> DevOps . Ata_Write = &ATA_MED_WriteDma; //&ATA_MED_Write;
	  pDevice -> DevOps . Ata_SetDevMode = &ATA_MED_SetModeVoid;
	  pDevice -> DevOps . Ata_DevReset = &ATA_IDE_Reset; 
	  pDevice -> DevOps . Ata_SetPwrMode = &ATA_MED_SetPwrMode;
	  pDevice -> DevOps . Ata_GetMaxSect = &ATA_MED_GetMaxSect;

  	  pDevice -> OpMode = UDMA;
	#ifdef ATA_ATAPI_SUPPORT
	  if (pCap -> IsAta == False)
	  {
	    pDevice -> DevOps . Ata_SubAtapiReq = &ATAPI_MED_Pio;
	  } else
	  {
	    pDevice -> DevOps . Ata_SubAtapiReq = &ATAPI_MED_Void;
	  }
	#else
	  pDevice -> DevOps . Ata_SubAtapiReq = &ATAPI_MED_Void;
	#endif
	  pDeviceIdent -> IsAta = pCap -> IsAta;
	  pDevice -> DevOps . Ata_Ioctl = &ATA_MED_Ioctl;
    } 
    else
    {
		if (NULL != pDevice) 
		{
	    	/* Device Identification Failed */
		    pDevice -> Probe = ATA_DEV_AVAIL;
		}
	}
}

/**
 *  \brief ATA_MED_ReRegister ()  : Re-Register the device. Set the Device opmode
 *                                  to default PIO mode
 *  \param pDeviceIdent   [IN]  : Device Indentification Information.
 *  \param MediaCallback  [IN]  : Callback routine for the registered Media Driver.
 *  \param pAtaIntOps  [IN/OUT] : ATA Interface API's.
 *  \param pHandle   [OUT]      : Handle to device (to be used for future
 *                                transactions).
 *  \param pDevice        [IN]  : Device Handle
 *  \return void                :
 ******************************************************************************/
static void ATA_MED_ReRegister (ATA_DEV_Ident_t * const pDeviceIdent, ATA_MED_Callback_t MediaCallback,
                       ATA_INT_Ops_t **pAtaIntOps, Ptr * const pHandle, ATA_Drive_t *pDevice)
{
  ATA_Dev_Cap_t *pCap = NULL;
  Uint16 count;

  if((((NULL != MediaCallback) && (NULL != pAtaIntOps)) && ((NULL != pHandle) && (NULL != pDevice))))
  {
	pCap = &pDevice -> Cap;
	  if (pDeviceIdent != NULL)
	  {
	    pDeviceIdent -> Cyl = pCap -> Cyl;
	    pDeviceIdent -> Head = pCap -> Head;
	    pDeviceIdent -> SectperTrack = pCap -> SectperTrack;
	    count = 0;

		for (count=0; count < 8; ++count)
			pDeviceIdent->FirmwareRev[count] = pCap -> FirmwareRev[count];

		for (count=0; count < 20; ++count)
	      pDeviceIdent->SerialNo[count]    = pCap -> SerialNo[count];

		for (count=0; count < 40; ++count)
	      pDeviceIdent->ModelNo[count]     = pCap -> ModelNo[count];

	  }

	  ATA_MED_SetPio (pDevice, 0);

	  /* Initialize the Devops for a PIO mode operation */
	  pDevice -> DevOps . Ata_SetDevMode = &ATA_MED_SetModeVoid;
	  pDevice -> DevOps . Ata_DevReset = &ATA_IDE_Reset;
	  pDevice -> DevOps . Ata_SetPwrMode = &ATA_MED_SetPwrMode;
	  pDevice -> DevOps . Ata_GetMaxSect = &ATA_MED_GetMaxSect;

	#ifdef ATA_ATAPI_SUPPORT
	  if (pCap -> IsAta == False)
	  {
	    pDevice -> DevOps . Ata_SubAtapiReq = &ATAPI_MED_Pio;
	  } else
	  {
	    pDevice -> DevOps . Ata_SubAtapiReq = &ATAPI_MED_Void;
	  }
	#else
	  pDevice -> DevOps . Ata_SubAtapiReq = &ATAPI_MED_Void;
	#endif
	  pDevice -> DevOps . Ata_Ioctl = &ATA_MED_Ioctl;

	  pDevice -> MedHandler = MediaCallback;
	  *pAtaIntOps = &(pDevice -> DevOps);
	  *pHandle = (Ptr *)(Uint32)pDevice;
	  pDevice -> Probe = ATA_DEV_REG;
	}

}

/**
 *  \brief ATA_SynchStatus ()   : Look for Command completion synchronously.
 *
 *  \param pIdeInt  [IN]        : Handle to the ATA Bus.
 *  \param pStatus  [IN]        : Status Pointer
 *  \param Timeout  [IN]        : In MS
 *  \return void
 ******************************************************************************/
void ATA_SynchStatus (ATA_IDE_Int_t const *pIdeInt, ATA_Dev_Stat_t * const pStatus, Uint32 Timeout)
{
	if((NULL != pIdeInt) && (NULL != pStatus))
	{
	  do
	  {
		m_delay(1u);
	    pIdeInt -> pIdeOps -> CmdStatus (pIdeInt -> pIntHndl, pStatus, False, 0);

	    /* Look for no device present condition */
	    if (((((pStatus -> Error == pStatus -> IntReason) &&
	       (pStatus -> IntReason == pStatus -> LbaLow)) &&
	       (pStatus -> LbaLow ==  pStatus -> LbaMid)) &&
	       (pStatus -> LbaMid == pStatus -> LbaHigh)) &&
	       (pStatus -> LbaHigh == pStatus -> Stat))
	    {
	      continue;
	    }

	    if ((FALSE == (pStatus -> Stat & ATA_DEV_BUSY)) && (((pStatus -> Stat & ATA_DEV_ERR)!=FALSE) || ((pStatus -> Stat & ATA_DEV_RDY)!=FALSE)))
	    {
	      break;
	    }
	  } while (--Timeout != 0);

	  if (Timeout == 0)
	  {
	    /* Inform that this request timeout out and the status is not valid */
	    pStatus -> Error = 0xFFu;
	    /* Set the Error Status on the device status */
	    pStatus -> Stat  |= ATA_DEV_ERR;
	  }
	}
}


/**
 *  \brief ATA_Init ()          : Init ATA Interface Driver.
 ******************************************************************************/
void ATA_Init (void)
{
  Int32 Device = 0;
  Int32 Index = 0;
  Int32 Cnt = 0;
  ATA_Dev_Req_t *pReq = ATA_Req;
  Int32 i;

  /* clear ide ctrl structure to zero */
  for (i=0; i<ATA_MAX_IDE_INT; ++i)	
	memset(	&Ide_Ctlr_Info[i], 0, sizeof(ATA_IDE_Int_t));


  memset(ATA_Req, 0, sizeof(ATA_Req));

  /* Initialize the Request token queue */
  while (Index < ATA_MAX_IDE_INT)
  {
    Ide_Ctlr_Info [Index] . IntNum = NULL;
    Ide_Ctlr_Info [Index] . IsInit = False;

    while (Device != ATA_MAX_INT_DEV)
    {
      Ide_Ctlr_Info [Index] . Drive [Device] . Probe         = ATA_DEV_INIT;
      Ide_Ctlr_Info [Index] . Drive [Device] . ReservReq     = NULL;
      Ide_Ctlr_Info [Index] . Drive [Device] . Dev           = NULL;
      Ide_Ctlr_Info [Index] . Drive [Device] . IsMedStatComp = NULL;


	  Queue_init (&Ide_Ctlr_Info [Index] . Drive [Device] . pReqCacheQueHead);
	  Queue_init (&Ide_Ctlr_Info [Index] . Drive [Device] . pReqQueHead);

      while (Cnt != ATA_MAX_DEV_QUEUE)
      {
		Queue_add_tail(&(pReq->link),&Ide_Ctlr_Info [Index] . Drive [Device] . pReqCacheQueHead);
        Cnt++;
        pReq++;
      }

      Ide_Ctlr_Info [Index] . Drive [Device] . AvailReq = (Uint8)Cnt;
      Ide_Ctlr_Info [Index] . Drive [Device] . ActiveReq = 0;
      Cnt = 0;
      Device++;
    }
    Device = 0;
    Index++;
  }

}

/**
 *  \brief ATA_MED_SetDevActive ()  : Set the Power mode to Active and configure
 *                                    operating mode of the device.  Done during
 *                                    IDLE and STDBY states only.
 *  \param Handle  [IN]             : Pointer to the drive.
 *  \param Mode    [IN]             : Device Operating mode
 *  \param Level   [IN]             : Level in the Mode
 *  \return int
 ******************************************************************************/
static Int32 ATA_MED_SetDevActive (Ptr Handle, ATA_DEV_Mode_t Mode, Uint32 Level)
{
	Int32 retVal = ATA_ERR_GEN;
	if(NULL != Handle)
	{
	  ((ATA_Drive_t *)Handle) -> DevOps . Ata_SetDevMode = &ATA_MED_SetMode;
	  ((ATA_Drive_t *)Handle) -> PwrMode = ATA_DEV_ACTIVE;
	  retVal =  ATA_MED_SetMode (Handle, Mode, Level);
	}
	return retVal;
}


/**
 *  \brief ATA_DeInit ()          : DeInit ATA Interface Driver.
 ******************************************************************************/
void ATA_DeInit (void)
{
  Int32 Index = 0;
  Uint32 Cookie;
  Int32 Device = 0;
  Int32 QueCnt = 0;
  Int32 Index_device_cnt = 0;


  while (Index < ATA_MAX_IDE_INT)
  {
    if (Ide_Ctlr_Info [Index] . pIdeOps != NULL)
    {
	  Cookie = HWI_disable ();
      /* Un-register from the IDE controller for this bus.  Protect ourself
       * from interrupt completion race conditions.
       */
      Ide_Ctlr_Info [Index] . pIdeOps -> RegHndlr
      (Ide_Ctlr_Info [Index] . pIntHndl, NULL);

  	  Queue_init (&Ide_Ctlr_Info [Index] . Drive [Device] . pReqCacheQueHead);
	  Queue_init (&Ide_Ctlr_Info [Index] . Drive [Device] . pReqQueHead);

	  HWI_restore (Cookie);

      /* Delete the Timeout Alarm */
      //PAL_osTimerDelete (Ide_Ctlr_Info [Index] . IdeTimeout);
	  timer0_stop ();

      Ide_Ctlr_Info [Index] . pIdeOps = NULL;
      Ide_Ctlr_Info [Index] . BusState = ATA_IDE_RESET;
    }

    for (Device = 0; Device < ATA_MAX_INT_DEV; ++Device)
    {
      Index_device_cnt = Index + Device;
      Ide_Ctlr_Info [Index] . Drive [Device] . Probe = ATA_DEV_INIT;
      for (QueCnt = 0; QueCnt < ATA_MAX_DEV_QUEUE; ++QueCnt)
      {
        ATA_Req[Index_device_cnt + QueCnt].TransferCount = NULL;
        ATA_Req[Index_device_cnt + QueCnt].BlockCount = NULL;
        ATA_Req[Index_device_cnt + QueCnt].IsPktCmd = NULL;
        ATA_Req[Index_device_cnt + QueCnt].Timeout = NULL;
        ATA_Req[Index_device_cnt + QueCnt].Retry = NULL;
      }
    }

    Index++;
  }
}


/**
 *  \brief ATA_MED_ModeVoid ()      : Return Error.
 *  \param Handle  [IN]             : Pointer to the drive.
 *  \param Mode    [IN]             : Device Operating mode
 *  \param Level   [IN]             : Level in the Mode
 *  \return int                     : ATA_ERR_GEN
 ******************************************************************************/
static Int32 ATA_MED_ModeVoid (Ptr Handle, ATA_DEV_Mode_t Mode, Uint32 Level)
{
  /* compiler remark: not referenced */
  if ( (Handle != NULL) && (Mode <= AUTO) )
  {
	 if (Level)
	 {
	 }	
  }
  return ATA_ERR_GEN;
}
/**
 *  \brief ATA_MED_SetMode ()  : Set the best operating mode for the given Drive.
 *  \param Handle  [IN]        : Pointer to the drive.
 *  \param Mode    [IN]        : Device Operating mode
 *  \param Level   [IN]        : Level in the Mode
 *  \return int                : Selected Level or Error (ATA_ERR_GEN)
 ******************************************************************************/
static Int32 ATA_MED_SetModeVoid (Ptr Handle, ATA_DEV_Mode_t Mode, Uint32 Level)
{
  /* compiler remark: not referenced */
  if ( (Handle != NULL) && (Mode <= AUTO) )
  {
	 if (Level)
	 {
	 }	
  }
  return ATA_NO_ERR;
}

/**
 *  \brief ATA_MED_SetMode ()  : Set the best operating mode for the given Drive.
 *  \param Handle  [IN]        : Pointer to the drive.
 *  \param Mode    [IN]        : Device Operating mode
 *  \param Level   [IN]        : Level in the Mode
 *  \return int                : Selected Level or Error (ATA_ERR_GEN)
 ******************************************************************************/
static Int32 ATA_MED_SetMode (Ptr Handle, ATA_DEV_Mode_t Mode, Uint32 Level)
{
  Int32 RetStat = ATA_ERR_GEN;
  Uint32 Cookie;

  if(NULL != Handle)
  {
	  /* Protect Device from Runtime issues during Mode Change.  Change mode only
	   * when no requests are pending.
	   */
	  Cookie = HWI_disable ();
	  if (((ATA_Drive_t *)Handle) -> pReq != NULL)
	  {
	    HWI_restore (Cookie);
	    return RetStat;
	  }
	  

	  ((ATA_Drive_t *)Handle) -> DevOps . Ata_Read = &ATA_MED_Void;
	  ((ATA_Drive_t *)Handle) -> DevOps . Ata_Write = &ATA_MED_Void;

	  HWI_restore (Cookie);

	  switch (Mode)
	  {
	    case UDMA:
	      RetStat = ATA_MED_Udma ((ATA_Drive_t *)Handle, Level);
	      break;
	    case DMA:
	      RetStat = ATA_MED_Dma ((ATA_Drive_t *)Handle, Level);
	      break;
	    case PIO:
	      RetStat = ATA_MED_Pio ((ATA_Drive_t *)Handle, Level);
	      break;
	    case AUTO:
	      /* First try Max UDMA then Max DMA then Max PIO */
	      RetStat = ATA_MED_Udma ((ATA_Drive_t *)Handle, ATA_UDMA_6);
	      if (RetStat == ATA_ERR_GEN)
	      {
	        RetStat = ATA_MED_Dma ((ATA_Drive_t *)Handle, ATA_DMA_2);
	      }
	      if (RetStat == ATA_ERR_GEN)
	      {
	        RetStat = ATA_MED_Pio ((ATA_Drive_t *)Handle, ATA_PIO_4);
	      }
	      break;
	    default:
	      break;
	  }
	}

  return RetStat;
}


/**
 *  \brief ATA_MED_Udma ()     : Validate and set the correct UDMA mode.
 *  \param pDevice [IN]        : Pointer to the drive.
 *  \param Level   [IN]        : Level in the Mode
 *  \return int                : Selected Level or Error
 ******************************************************************************/
static Int32 ATA_MED_Udma (ATA_Drive_t *pDevice, Uint32 Level)
{
  Int32 RetStat = ATA_ERR_GEN;

  if(NULL != pDevice)
  {

	  if (pDevice -> Cap . Udma != (0xFFu))
	  {
	    if (pDevice -> Cap . Udma < Level)
	    {
	      Level = pDevice -> Cap . Udma;
	    }
		if( pDevice->pIdeInt->IsSata )
			RetStat = ATA_NO_ERR;
		else
		    RetStat = ATA_MED_SetUdma (pDevice, Level);
	  } else
	  {
	    RetStat = ATA_ERR_GEN;
	  }

	}
  return RetStat;
}


/**
 *  \brief ATA_MED_Dma ()      : Validate and set the correct DMA mode.
 *  \param pDevice [IN]        : Pointer to the drive.
 *  \param Level   [IN]        : Level in the Mode
 *  \return int                : Selected Level or Error
 ******************************************************************************/
static Int32 ATA_MED_Dma (ATA_Drive_t *pDevice, Uint32 Level)
{
  Int32 RetStat = ATA_ERR_GEN;

   if(NULL != pDevice)
  {

	  if (pDevice -> Cap . Dma != (0xFFu))
	  {
	    if (pDevice -> Cap . Dma < Level)
	    {
	      Level = pDevice -> Cap . Dma;
	    }
	    RetStat = ATA_MED_SetDma (pDevice, Level);
	  } else
	  {
	    RetStat = ATA_ERR_GEN;
	  }
	}

  return RetStat;
}


/**
 *  \brief ATA_MED_Pio ()      : Validate and set the correct PIO mode.
 *  \param pDevice [IN]        : Pointer to the drive.
 *  \param Level   [IN]        : Level in the Mode
 *  \return int                : Selected Level or Error
 ******************************************************************************/
static Int32 ATA_MED_Pio (ATA_Drive_t *pDevice, Uint32 Level)
{
  Int32 RetStat = ATA_ERR_GEN;

  if(NULL != pDevice)
  {
	  if (pDevice -> Cap . Pio != (0xFFu))
	  {
	    if (pDevice -> Cap . Pio < Level)
	    {
	      Level = pDevice -> Cap . Pio;
	    }
	    RetStat = ATA_MED_SetPio (pDevice, Level);
	  } else
	  {
	    RetStat = ATA_ERR_GEN;
	  }
	}

  return RetStat;
}


/**
 *  \brief ATA_MED_SetUdma ()  : Set UDMA operating mode for the given Drive.
 *  \param pDevice [IN]        : Pointer to the drive.
 *  \param Level   [IN]        : Level in the Mode
 *  \return int                : Selected Level or Error
 ******************************************************************************/
static Int32 ATA_MED_SetUdma (ATA_Drive_t *pDevice, Uint32 Level)
{

    ATA_Dev_Req_t *pReq=NULL;
    Int32 RetStat = ATA_ERR_GEN;
    Uint32 DevLevel = 0;

	if(NULL != pDevice)
	{
		RetStat = 1;
		DevLevel = pDevice -> Level;
	  pDevice -> Level = pDevice -> pIdeInt -> pIdeOps -> SetDevMode (
	                         pDevice -> pIdeInt -> pIntHndl,
	                         (pDevice -> Dev & ~ATA_DEV_LBA) >> 4,
	                         0,
	                         UDMA,
	                         Level);
	  switch (pDevice -> Level)
	  {
	    case ATA_UDMA_0:
	      DevLevel = ATA_DEV_UDMA0;
	      break;
	    case ATA_UDMA_1:
	      DevLevel = ATA_DEV_UDMA1;
	      break;
	    case ATA_UDMA_2:
	      DevLevel = ATA_DEV_UDMA2;
	      break;
	    case ATA_UDMA_3:
	      DevLevel = ATA_DEV_UDMA3;
	      break;
	    case ATA_UDMA_4:
	      DevLevel = ATA_DEV_UDMA4;
	      break;
	    case ATA_UDMA_5:
	      DevLevel = ATA_DEV_UDMA5;
	      break;
	    case ATA_UDMA_6:
	      DevLevel = ATA_DEV_UDMA6;
	      break;
	    default:
	      pDevice -> Level = DevLevel;
	      RetStat = ATA_ERR_GEN;
	      break;
	  }

	  if (RetStat == (1u))
	  {
	    /* Enable the UDMA feature */
	    pReq = ATA_AllocReq (pDevice, False);
	    if (pReq != NULL)
	    {
	      pReq -> Command . SectorCnt = (Uint8)DevLevel;
	      pReq -> Command . Cmd = ATA_SET_FEATURES;
	      pReq -> Command . Features = ATA_SET_SPEED;
	      pReq -> Command . Device = pDevice -> Dev;
	      pReq -> Mode = TASKFILE;
	      pReq -> Handler = NULL;
	      pReq -> ToHndlr = &ATA_SetUdmaHndlr;
	      pReq -> Timeout = ATA_TMSEC_TO;
	      pReq -> Retry   = 1000u;
	      pReq -> MediaHandle = NULL;
	      pReq -> IsInternal  = True;
	      /* This request is always the only request on this device at this point in time */
	      ATA_SubmitReq (pDevice, pReq);

	      /* Wait for the request to complete executing in synch */
	      while (pReq -> Retry != 0)
	      {
	        //PAL_osWaitMsecs (1u);
			m_delay(1u);
	      }

	      if (pReq -> Res . ReqInfo . Status . Error == ATA_ERR_OK)
	      {
	        /* Init the DevOps table to reflect the capabilities of the drive */
	        pDevice -> DevOps . Ata_Read = &ATA_MED_ReadDma;
	        pDevice -> DevOps . Ata_Write = &ATA_MED_WriteDma;
	#ifdef ATA_ATAPI_SUPPORT
	        if (pDevice -> Cap . IsAta==False)
	        {
	          pDevice -> DevOps . Ata_SubAtapiReq = &ATAPI_MED_Dma;
	        }
	#endif
	        pDevice -> OpMode = UDMA;

	        RetStat = pDevice -> Level;
	      } else
	      {
	        RetStat = pReq -> Res . ReqInfo . Status . Error;
	      }
	      if (pReq -> Res . ReqInfo . Status . Error != ATA_ERR_TO)
	      {

	        /* Protect Device from Runtime issues during Mode Change.  Change mode only
	         * when no requests are pending.
	         */
	        ATA_SchedNextReq (pDevice -> pIdeInt, pReq);

	      }
	    } else
	    {
	      RetStat = ATA_ERR_GEN;
	    }
	  }
	}

  return RetStat;
}
/**
 *  \brief ATA_SetUdmaHndlr ()  : Handle the Set UDMA mode completion.
 *  \param Handle [IN]      : Handle to the Device.
 *  \param pStatus [IN]     : Command status
 *  \return Bool            : False
 ******************************************************************************/
Bool ATA_SetUdmaHndlr (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
  Bool Status = False;
  ATA_Drive_t const *pDevice = NULL;

	if((NULL != Handle) && (NULL != pStatus))
	{
		pDevice = (ATA_Drive_t const *)Handle;
	  if (FALSE != (pStatus -> Stat & ATA_DEV_BUSY))
	  {
			if(--(pDevice -> pReq -> Retry) > 0)
			{
			    pDevice -> pIdeInt -> TimeOutData = (Ptr)pDevice;
				timer0_start( pDevice -> pReq -> Timeout );
			}
			else
			{
				if (pDevice -> pReq -> Retry == 0)
				{
				  Status = True;
				  pDevice -> pReq -> Res . ReqInfo . Status . Error = ATA_ERR_TO;
				} else
				{
				  if (FALSE != (pStatus -> Stat & ATA_DEV_ERR))
				  {
				    pDevice -> pReq -> Res . ReqInfo . Status . Error = ATA_ERR_GEN;
				  } else
				  {
				    pDevice -> pReq -> Res . ReqInfo . Status . Error = ATA_ERR_OK;
				  }
				}
				/* Set this flag to signal the caller that this request is done */
				pDevice -> pReq -> Retry = 0;
			}
	  } 
	  else
	  {
	    if (pDevice -> pReq -> Retry == 0)
	    {
	      Status = True;
	      pDevice -> pReq -> Res . ReqInfo . Status . Error = ATA_ERR_TO;
	    } else
	    {
	      if (FALSE != (pStatus -> Stat & ATA_DEV_ERR))
	      {
	        pDevice -> pReq -> Res . ReqInfo . Status . Error = ATA_ERR_GEN;
	      } else
	      {
	        pDevice -> pReq -> Res . ReqInfo . Status . Error = ATA_ERR_OK;
	      }
	    }
	    /* Set this flag to signal the caller that this request is done */
	    pDevice -> pReq -> Retry = 0;
	  }
	}

  return Status;
}
/**
 *  \brief ATA_MED_SetDma ()   : Set DMA operating mode for the given Drive.
 *  \param pDevice [IN]        : Pointer to the drive.
 *  \param Level   [IN]        : Level in the Mode
 *  \return int                : Selected Level or Error
 ******************************************************************************/
static Int32 ATA_MED_SetDma (ATA_Drive_t *pDevice, Uint32 Level)
{
  ATA_Dev_Req_t *pReq;
  Int32 RetStat = ATA_ERR_GEN;
  Uint32 DevLevel = 0;

  if(NULL != pDevice)
  {
	RetStat = 1;
	DevLevel = pDevice -> Level;
	  pDevice -> Level = pDevice -> pIdeInt -> pIdeOps -> SetDevMode (
	                         pDevice -> pIdeInt -> pIntHndl,
	                         (pDevice -> Dev & ~ATA_DEV_LBA) >> 4,
	                         pDevice -> Cap . DmaCycle,
	                         DMA,
	                         Level);
	  switch (pDevice -> Level)
	  {
	    case ATA_DMA_0:
	      DevLevel = ATA_DEV_DMA0;
	      break;
	    case ATA_DMA_1:
	      DevLevel = ATA_DEV_DMA1;
	      break;
	    case ATA_DMA_2:
	      DevLevel = ATA_DEV_DMA2;
	      break;
	    default:
	      pDevice -> Level = DevLevel;
	      RetStat = ATA_ERR_GEN;
	      break;
	  }

	  if (RetStat == (1u))
	  {
	    /* Enable the DMA feature */
	    pReq = ATA_AllocReq (pDevice, False);
	    if (pReq != NULL)
	    {
	      pReq -> Command . SectorCnt = (Uint8)DevLevel;
	      pReq -> Command . Cmd = ATA_SET_FEATURES;
	      pReq -> Command . Features = ATA_SET_SPEED;
	      pReq -> Command . Device = pDevice -> Dev;
	      pReq -> Handler = NULL;
	      pReq -> ToHndlr = &ATA_SetUdmaHndlr;
	      pReq -> Timeout = ATA_TMSEC_TO;
	      pReq -> Retry   = 1000u;
	      pReq -> Mode = TASKFILE;
	      pReq -> MediaHandle = NULL;
	      pReq -> IsInternal  = True;
	      pReq -> IsPktCmd    = False;

	      ATA_SubmitReq (pDevice, pReq);

	      /* Wait for the request to complete executing in synch */
	      while (pReq -> Retry != 0)
	      {
			m_delay(1u);
	      }

	      if (pReq -> Res . ReqInfo . Status . Error == ATA_ERR_OK)
	      {
	        /* Init the DevOps table to reflect the capabilities of the drive */
	        pDevice -> DevOps . Ata_Read = &ATA_MED_ReadDma;
	        pDevice -> DevOps . Ata_Write = &ATA_MED_WriteDma;
	#ifdef ATA_ATAPI_SUPPORT
	        if (pDevice -> Cap . IsAta == False)
	        {
	          pDevice -> DevOps . Ata_SubAtapiReq = &ATAPI_MED_Dma;
	        }
	#endif
	        pDevice -> OpMode = DMA;

	        RetStat = pDevice -> Level;
	      } else
	      {
	        RetStat = pReq -> Res . ReqInfo . Status . Error;
	      }
	      if (pReq -> Res . ReqInfo . Status . Error != ATA_ERR_TO)
	      {
	        /* Protect Device from Runtime issues during Mode Change.  Change mode only
	         * when no requests are pending.
	         */
	        ATA_SchedNextReq (pDevice -> pIdeInt, pReq);
	      }
	    } else
	    {
	      RetStat = ATA_ERR_GEN;
	    }
	  }
	}

  return RetStat;
}


/**
 *  \brief ATA_MED_SetPio ()   : Set PIO operating mode for the given Drive.
 *  \param pDevice [IN]        : Pointer to the drive.
 *  \param Level   [IN]        : Level in the Mode
 *  \return int                : Selected Level
 ******************************************************************************/
static Int32 ATA_MED_SetPio (ATA_Drive_t *pDevice, Uint32 Level)
{
  ATA_Dev_Stat_t Status;
  ATA_Dev_Req_t *pReq;
  Int32 retVal = -1;

  if(NULL != pDevice)
  {
	  if (pDevice -> Cap . SectCnt != 0)
	  {
	    /* Enable the PIO Multiple  feature */
	    pReq = ATA_AllocReq (pDevice, False);
	    if (pReq != NULL)
	    {
	      pReq -> Command . Device = pDevice -> Dev;
	      pReq -> Command . Cmd = ATA_SET_MULT;
	      pReq -> Command . SectorCnt = (Uint8)(pDevice -> Cap . SectCnt);
	      pReq -> Handler = NULL;
	      pReq -> Timeout = 0;
	      pReq -> MediaHandle = NULL;
	      pReq -> Mode = TASKFILE;
	      pReq -> IsInternal  = True;

	      ATA_SubmitReq (pDevice, pReq);

	      ATA_SynchStatus (pDevice -> pIdeInt, &Status, ATA_CMD_TO);

	      if (FALSE == (Status . Stat & ATA_DEV_ERR))
	      {
	        /* Init the DevOps table to reflect the capabilities of the drive */
	        pDevice -> DevOps . Ata_Read = &ATA_MED_ReadMul;
	        pDevice -> DevOps . Ata_Write = &ATA_MED_WriteMul;
	      } else
	      {
	        pDevice -> Cap . SectCnt = 0;
	      }
	      ATA_SchedNextReq (pDevice -> pIdeInt, pReq);
	    } else
	    {
	      pDevice -> Cap . SectCnt = 0;
	    }
	  }

	  pDevice -> Level = pDevice -> pIdeInt -> pIdeOps -> SetDevMode (
	                         pDevice -> pIdeInt -> pIntHndl,
	                         (pDevice -> Dev & ~ATA_DEV_LBA) >> 4,
	                         pDevice -> Cap . PioCycleIordy,
	                         PIO,
	                         Level);
	  pDevice -> OpMode = PIO;
	#ifdef ATA_ATAPI_SUPPORT
	  if (pDevice -> Cap . IsAta == False)
	  {
	    pDevice -> DevOps . Ata_SubAtapiReq = &ATAPI_MED_Pio;
	  }
	#endif

	  if (pDevice -> Cap . SectCnt == 0)
	  {
	    pDevice -> DevOps . Ata_Read = &ATA_MED_ReadDma;//&ATA_MED_Read;
	    pDevice -> DevOps . Ata_Write = &ATA_MED_WriteDma;//&ATA_MED_Write;
		pDevice -> OpMode = UDMA;
	  }
	  retVal = (Int32)(pDevice -> Level);
	}

  	return retVal;
}


/**
 *  \brief ATA_MED_Void ()     : Return Error.
 *  \param Buffer [IN]         : Pointer to the Buffer.
 *  \param Sector [IN]         : Starting Sector Number.
 *  \param NumSector [IN]      : Number of sector's requested.
 *  \param Handle [IN]         : Handle for this Device instance.
 *  \param MHndl  [IN]         : Handle for Media Layer
 *
 *  \return int                : Error (ATA_ERR_GEN)
 ******************************************************************************/
static Int32 ATA_MED_Void (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector)
{
   /* compiler remark: not referenced */
  if ((Handle != NULL) && (MHndl != NULL) && (Buffer != NULL))
  {
	 if ( Sector || NumSector ) 	
	 {
	 }
  }
  return ATA_ERR_GEN;
}


/**
 *  \brief ATA_MED_ReadPwr ()  : Set Device Power Mode to Active and Read Sectors.
 *  \param Buffer [IN]         : Pointer to the Buffer.  In STDBY, IDLE power Modes.
 *  \param Sector [IN]         : Starting Sector Number.
 *  \param NumSector [IN]      : Number of sector's requested.
 *  \param Handle [IN]         : Handle for this Device instance.
 *  \param MHndl  [IN]         : Handle for Media Layer
 *
 *  \return int                : Success or Error
 ******************************************************************************/
static Int32 ATA_MED_ReadPwr (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector)
{
	Int32 retVal = -1;
  if((NULL != Handle) && (NULL != Buffer))
  {
	  ((ATA_Drive_t *)Handle) -> PwrMode = ATA_DEV_ACTIVE;
	  ATA_SetDevops ((ATA_Drive_t *)Handle);
	  retVal = ((ATA_Drive_t *)Handle) -> DevOps . Ata_Read (Handle, MHndl, Buffer,
          							Sector, NumSector);
  }

  return (retVal);

}


/**
 *  \brief ATA_MED_WritePwr () : Set Device Power Mode to Active and Write Sectors.
 *  \param Buffer [IN]         : Pointer to the Buffer.
 *  \param Sector [IN]         : Starting Sector Number.
 *  \param NumSector [IN]      : Number of sector's requested.
 *  \param Handle [IN]         : Handle for this Device instance.
 *  \param MHndl  [IN]         : Handle for Media Layer
 *
 *  \return int                : Success or Error
 ******************************************************************************/
static Int32 ATA_MED_WritePwr (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector)
{
	Int32 retVal = -1;
  if((NULL != Handle) && (NULL != Buffer))
  {
	  ((ATA_Drive_t *)Handle) -> PwrMode = ATA_DEV_ACTIVE;
	  ATA_SetDevops ((ATA_Drive_t *)Handle);
	  retVal = ((ATA_Drive_t *)Handle) -> DevOps . Ata_Write (Handle, MHndl, Buffer,
          							Sector, NumSector);
  }

  return (retVal);
}


/**
 *  \brief ATA_MED_ReadMul ()  : Read Multiple Sectors Mode of PIO Read.
 *  \param Buffer [IN]         : Pointer to the Buffer.
 *  \param Sector [IN]         : Starting Sector Number.
 *  \param NumSector [IN]      : Number of sector's requested.
 *  \param Handle [IN]         : Handle for this Device instance.
 *  \param MHndl  [IN]         : Handle for Media Layer
 *
 *  \return int                : Success or Error
 ******************************************************************************/
static Int32 ATA_MED_ReadMul (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector)
{

  ATA_Drive_t *pDevice = NULL;
  Uint32       Timeout = NULL;
  ATA_Dev_Req_t *pReq=NULL;
  Int32 Err = ATA_ERR_GEN;

  if((NULL != Handle) && (NULL != Buffer))
  {

	pDevice = (ATA_Drive_t *)Handle;
	Timeout = (pDevice -> PwrMode == ATA_DEV_ACTIVE)? ATA_CMD_TO :
                        ATA_RESET_TO;

	  pReq = ATA_AllocReq (pDevice, False);
	  if (pReq != NULL)
	  {
	    NumSector = (NumSector == 0)? ATA_MAX_SECT : NumSector;
	    /* Update any extended Request Information (NumSector > ATA_MAX_SECT) if any */
	    if (NumSector >= ATA_MAX_SECT)
	    {
	      pReq -> ExtReqInfo = (Ptr)((NumSector - ATA_MAX_SECT) *
	                                 pDevice -> Cap . SectSize);
	      pReq -> TransferCount = ATA_MAX_SECT * pDevice -> Cap . SectSize;
	      pReq -> Command . SectorCnt = 0;
	    } else
	    {
	      pReq -> ExtReqInfo = (Ptr)0;
	      pReq -> TransferCount = NumSector * pDevice -> Cap . SectSize;
	      pReq -> Command . SectorCnt = (Uint8)NumSector;
	    }

	    pReq -> MediaHandle       = MHndl;
	    pReq -> Command . Device  = pDevice -> Dev;
	    pReq -> Command . Cmd     = ATA_READ_MUL;
	    pReq -> Command . LbaLow    = Sector;
	    pReq -> Command . LbaMid    = Sector >> 8;
	    pReq -> Command . LbaHigh   = Sector >> 16;
	    pReq -> Command . Device    = ((Sector >> 24) | 0xF0) & pDevice -> Dev;
	    pReq -> Buffer            = Buffer;
	    pReq -> Mode              = READ_PIO;
	    pReq -> BlockCount        = pDevice -> Cap . SectCnt * pDevice -> Cap . SectSize;
	    pReq -> Handler           = &ATA_ReadPio;
	    pReq -> ToHndlr           = NULL;
	    pReq -> Timeout           = Timeout;
	    pReq -> Res . IsEvent = False;
	    pReq -> Res . ReqInfo . Status . Buffer = pReq -> Buffer;
	    pReq -> Res . ReqInfo . Status . Error = ATA_NO_ERR;
	    pReq -> IsPktCmd = False;

	    ATA_SubmitReq (pDevice, pReq);
	    Err = ATA_NO_ERR;
	  }
	}

  return Err;
}


/**
 *  \brief ATA_PioBeginWrite ()  : Begin writing the first block of data (PIO)
 *
 *  \param  *pDevice             : Device Pointer
 *  \param   Count               : Block Count
 *  \return  int                 : ATA_NO_ERR, ATA_ERR_GEN, ATA_ERR_TO
 ******************************************************************************/
static Int32 ATA_PioBeginWrite (ATA_Drive_t const *  pDevice, Uint32 Count)
{
  Uint32 Wait = 80u;
  Ptr Buffer=NULL;
  Int32 Err = ATA_ERR_GEN;
  ATA_Dev_Stat_t Status;

  if(NULL != pDevice)
  {
	Err = ATA_NO_ERR;
	  do
	  {
	    /* Simulate a 400ns wait (ATA4 Standard)*/
	    do
	    {
	      pDevice -> pIdeInt -> pIdeOps -> CmdStatus (pDevice -> pIdeInt -> pIntHndl, &Status, False,0);
	      if ((FALSE != (Status . Stat & ATA_DEV_REQ)) || (FALSE != (Status . Stat & ATA_DEV_ERR)))
	      {
	        break;
	      }
	    } while (--Wait != 0);

	    if (FALSE != (Status . Stat & ATA_DEV_REQ))
	    {
	      pDevice -> pReq -> TransferCount -= Count;
	      Buffer = pDevice -> pReq -> Buffer;
	      pDevice -> pReq -> Buffer = ((Uint8 *)Buffer) + Count;
	      pDevice -> pIdeInt -> pIdeOps -> Write (pDevice -> pIdeInt -> pIntHndl, Buffer, Count);
	      break;
	    } else if (FALSE != (Status . Stat & ATA_DEV_ERR))
	    {
	      /* Retry the error if possible */
	      if (FALSE != ATA_WriteErr (pDevice, &Status))
	      {
	        Wait = 5000u;
	        ATA_RetryReq (pDevice -> pIdeInt, pDevice -> pReq);
	      } else
	      {
	        /* Cannot retry */
	        Err = ATA_ERR_GEN;
	        break;
	      }
	    } else
	    {
	      Err = ATA_ERR_TO;
	      break;
	    }
	  } while (pDevice -> pReq -> Retry != 0);
	}

  return Err;
}


/**
 *  \brief ATA_MED_WriteMul ()  : Write Multiple Sectors mode of PIO Write.
 *  \param Buffer [IN]         : Pointer to the Buffer.
 *  \param Sector [IN]         : Starting Sector Number.
 *  \param NumSector [IN]      : Number of sector's requested.
 *  \param Handle [IN]         : Handle for this Device instance.
 *  \param MHndl  [IN]         : Handle for Media Layer
 *
 *  \return int                : Success or Error
 ******************************************************************************/
static Int32 ATA_MED_WriteMul (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector)
{

  ATA_Drive_t *pDevice = NULL;
  Uint32       Timeout = NULL;
  ATA_Dev_Req_t *pReq=NULL;
  Int32 Err = ATA_ERR_GEN;
  Uint32 Count = 0;

  if((NULL != Handle) && (NULL != Buffer))
  {

	pDevice = (ATA_Drive_t *)Handle;
	Timeout = (pDevice -> PwrMode == ATA_DEV_ACTIVE)? ATA_CMD_TO :
                        ATA_RESET_TO;

	  pReq = ATA_AllocReq (pDevice, False);
	  if (pReq != NULL)
	  {
	    NumSector = (NumSector == 0)? ATA_MAX_SECT : NumSector;
	    /* Update any extended Request Information (NumSector > ATA_MAX_SECT) if any */
	    if (NumSector >= ATA_MAX_SECT)
	    {
	      pReq -> ExtReqInfo = (Ptr)((NumSector - ATA_MAX_SECT) *
	                                 pDevice -> Cap . SectSize);
	      pReq -> TransferCount = ATA_MAX_SECT * pDevice -> Cap . SectSize;
	      pReq -> Command . SectorCnt = 0;
	    } else
	    {
	      pReq -> ExtReqInfo = (Ptr)0;
	      pReq -> TransferCount = NumSector * pDevice -> Cap . SectSize;
	      pReq -> Command . SectorCnt = (Uint8)NumSector;
	    }

	    pReq -> MediaHandle       = MHndl;
	    pReq -> Command . Device  = pDevice -> Dev;
	    pReq -> Command . Cmd     = ATA_WRITE_MUL;
	    pReq -> Command . LbaLow    = Sector;
	    pReq -> Command . LbaMid    = Sector >> 8;
	    pReq -> Command . LbaHigh   = Sector >> 16;
	    pReq -> Command . Device    = ((Sector >> 24u) | 0xF0) & pDevice -> Dev;
	    pReq -> Buffer            = Buffer;
	    pReq -> Mode              = WRITE_PIO;
	    pReq -> BlockCount        = pDevice -> Cap . SectCnt * pDevice -> Cap . SectSize;
	    pReq -> Res . IsEvent     = False;
	    pReq -> Handler           = &ATA_WritePio;
	    pReq -> ToHndlr           = NULL;
	    pReq -> Timeout           = Timeout;
	    pReq -> Res . ReqInfo . Status . Buffer = pReq -> Buffer;
	    pReq -> Res . ReqInfo . Status . Error = ATA_NO_ERR;
	    pReq -> IsPktCmd          = False;

	    if (ATA_SubmitReq (pDevice, pReq) == True)
	    {
	      if (pReq -> TransferCount <= pReq -> BlockCount)
	      {
	        Count = pReq -> TransferCount;
	      } else
	      {
	        Count = pReq -> BlockCount;
	      }

	      /* Write the first "Count" bytes */
	      if (ATA_PioBeginWrite (pDevice, Count) == ATA_ERR_GEN)
	      {
	        /* At this point no request are active on this Bus so we can safely
	         * call this function.
	         */
	        ATA_SchedNextReq (pDevice -> pIdeInt, pReq);
	      } else
	      {
	        Err = ATA_NO_ERR;
	      }
	    } else
	    {
	      Err = ATA_NO_ERR;
	    }
	  }
	}

  return Err;
}

#if 0
/**
 *  \brief ATA_MED_Read ()     : Read Sectors.
 *  \param Buffer [IN]         : Pointer to the Buffer.
 *  \param Sector [IN]         : Starting Sector Number.
 *  \param NumSector [IN]      : Number of sector's requested.
 *  \param Handle [IN]         : Handle for this Device instance.
 *  \param MHndl  [IN]         : Handle for Media Layer
 *
 *  \return int                : Success or Error
 ******************************************************************************/
static Int32 ATA_MED_Read (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector)
{

  ATA_Drive_t *pDevice = NULL;
  Uint32       Timeout = NULL;
  ATA_Dev_Req_t *pReq=NULL;
  Int32 Err = ATA_ERR_GEN;

  if((NULL != Handle) && (NULL != Buffer))
  {

	pDevice = (ATA_Drive_t *)Handle;
	Timeout = (pDevice -> PwrMode == ATA_DEV_ACTIVE)? ATA_CMD_TO :
                        ATA_RESET_TO;

	  pReq = ATA_AllocReq (pDevice, False);
	  if (pReq != NULL)
	  {
	    NumSector = (NumSector == 0)? ATA_PIO_MAX_SECT : NumSector;
	    /* Update any extended Request Information (NumSector > ATA_MAX_SECT) if any */
	    if (NumSector >= ATA_PIO_MAX_SECT)
	    {
	      pReq -> ExtReqInfo = (Ptr)((NumSector - ATA_PIO_MAX_SECT) *
	                                 pDevice -> Cap . SectSize);
	      pReq -> TransferCount = ATA_PIO_MAX_SECT * pDevice -> Cap . SectSize;
	      pReq -> Command . SectorCnt = ATA_PIO_MAX_SECT;
	    } else
	    {
	      pReq -> ExtReqInfo = (Ptr)0;
	      pReq -> TransferCount = NumSector * pDevice -> Cap . SectSize;
	      pReq -> Command . SectorCnt = (Uint8)NumSector;
	    }

	    pReq -> MediaHandle       = MHndl;
	    pReq -> Command . Device  = pDevice -> Dev;
	    pReq -> Command . Cmd     = ATA_READ;
	    pReq -> Command . LbaLow    = Sector;
	    pReq -> Command . LbaMid    = Sector >> 8;
	    pReq -> Command . LbaHigh   = Sector >> 16;
	    pReq -> Command . Device    = ((Sector >> 24) | 0xF0) & pDevice -> Dev;
	    pReq -> Buffer            = Buffer;
	    pReq -> Mode              = READ_PIO;
	    pReq -> BlockCount        = pDevice -> Cap . SectSize;
	    pReq -> Res . IsEvent     = False;
	    pReq -> Handler           = &ATA_ReadPio;
	    pReq -> ToHndlr           = NULL;
	    pReq -> Timeout           = Timeout;
	    pReq -> Res . ReqInfo . Status . Buffer = pReq -> Buffer;
	    pReq -> Res . ReqInfo . Status . Error = ATA_NO_ERR;
	    pReq -> IsPktCmd          = False;

	    ATA_SubmitReq (pDevice, pReq);
	    Err = ATA_NO_ERR;
	  }
	}

  return Err;
}
#endif

#if 0
/**
 *  \brief ATA_MED_Write ()    : Write Sectors.
 *  \param Buffer [IN]         : Pointer to the Buffer.
 *  \param Sector [IN]         : Starting Sector Number.
 *  \param NumSector [IN]      : Number of sector's requested.
 *  \param Handle [IN]         : Handle for this Device instance.
 *  \param MHndl  [IN]         : Handle for Media Layer
 *
 *  \return int                : Success or Error
 ******************************************************************************/
static Int32 ATA_MED_Write (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector)
{
  ATA_Drive_t *pDevice = NULL;
  Uint32       Timeout = NULL;
  ATA_Dev_Req_t *pReq=NULL;
  Int32 Err = ATA_ERR_GEN;

  if((NULL != Handle) && (NULL != Buffer))
  {

	pDevice = (ATA_Drive_t *)Handle;
	Timeout = (pDevice -> PwrMode == ATA_DEV_ACTIVE)? ATA_CMD_TO :
                        ATA_RESET_TO;
	  pReq = ATA_AllocReq (pDevice, False);
	  if (pReq != NULL)
	  {
	    NumSector = (NumSector == 0)? ATA_PIO_MAX_SECT : NumSector;
	    /* Update any extended Request Information (NumSector > ATA_MAX_SECT) if any */
	    if (NumSector >= ATA_PIO_MAX_SECT)
	    {
	      pReq -> ExtReqInfo = (Ptr)((NumSector - ATA_PIO_MAX_SECT) *
	                                 pDevice -> Cap . SectSize);
	      pReq -> TransferCount = ATA_PIO_MAX_SECT * pDevice -> Cap . SectSize;
	      pReq -> Command . SectorCnt = ATA_PIO_MAX_SECT;
	    } else
	    {
	      pReq -> ExtReqInfo = NULL;
	      pReq -> TransferCount = NumSector * pDevice -> Cap . SectSize;
	      pReq -> Command . SectorCnt = (Uint8)NumSector;
	    }


	    pReq -> MediaHandle = MHndl;
	    pReq -> Command . Device = pDevice -> Dev;
	    pReq -> Command . Cmd = ATA_WRITE;
	    pReq -> Command . LbaLow    = Sector;
	    pReq -> Command . LbaMid    =  Sector >> 8;
	    pReq -> Command . LbaHigh   = Sector >> 16;
	    pReq -> Command . Device    = ((Sector >> 24) | 0xF0) & pDevice -> Dev;
	    pReq -> Mode = WRITE_PIO;
	    pReq -> BlockCount = pDevice -> Cap . SectSize;
	    pReq -> Buffer = Buffer;
	    pReq -> Res . IsEvent = False;
	    pReq -> Handler = &ATA_WritePio;
	    pReq -> ToHndlr = NULL;
	    pReq -> Timeout = Timeout;
	    pReq -> Res . ReqInfo . Status . Buffer = pReq -> Buffer;
	    pReq -> Res . ReqInfo . Status . Error = ATA_NO_ERR;
	    pReq -> IsPktCmd = False;

	    if (ATA_SubmitReq (pDevice, pReq) == True)
	    {
	      if (ATA_PioBeginWrite (pDevice, pDevice -> Cap . SectSize) == ATA_ERR_GEN)
	      {
	        /* At this point no request are active on this Bus so we can safely
	         * call this function.
	         */
	        ATA_SchedNextReq (pDevice -> pIdeInt, pReq);
	      } else
	      {
	        Err = ATA_NO_ERR;
	      }
	    } else
	    {
	      Err = ATA_NO_ERR;
	    }
	  }
	}

  return Err;
}
#endif

/**
 *  \brief ATA_MED_ReadDma ()     : Read Sectors in DMA Mode.
 *  \param Buffer [IN]            : Pointer to the Buffer.
 *  \param Sector [IN]            : Starting Sector Number.
 *  \param NumSector [IN]         : Number of sector's requested.
 *  \param Handle [IN]            : Handle for this Device instance.
 *  \param MHndl  [IN]            : Handle for Media Layer
 *
 *  \return int                   : Success or Error
 ******************************************************************************/
static Int32 ATA_MED_ReadDma (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector)
{

  ATA_Drive_t *pDevice = NULL;
  ATA_Dev_Req_t *pReq=NULL;
  Uint32         Timeout = 0;
  Int32 Err = ATA_ERR_GEN;

  if((NULL != Handle) && (NULL != Buffer))
  {
	pDevice = (ATA_Drive_t *)Handle;
  	Timeout = (pDevice -> PwrMode == ATA_DEV_ACTIVE)? ATA_CMD_TO : ATA_RESET_TO;
	  pReq = ATA_AllocReq (pDevice, False);
	  if (pReq != NULL)
	  {
	    NumSector = (NumSector == 0)? ATA_MAX_SECT : NumSector;
	    /* Update any extended Request Information (NumSector > ATA_MAX_SECT) if any */
	    if (NumSector >= ATA_MAX_SECT)
	    {
	      pReq -> ExtReqInfo = (Ptr)((NumSector - ATA_MAX_SECT) *
	                                 pDevice -> Cap . SectSize);
	      pReq -> TransferCount = ATA_MAX_SECT * pDevice -> Cap . SectSize;
	      pReq -> Command . SectorCnt = 0;
	    } else
	    {
	      pReq -> ExtReqInfo = (Ptr)0;
	      pReq -> TransferCount = NumSector * pDevice -> Cap . SectSize;
	      pReq -> Command . SectorCnt = (Uint8)NumSector;
	    }

	    pReq -> MediaHandle       = MHndl;
	    pReq -> Command . Device  = pDevice -> Dev;
	    pReq -> Command . Cmd     = ATA_READ_DMA;
	    pReq -> Command . LbaLow    = Sector;
	    pReq -> Command . LbaMid    = Sector >> 8;
	    pReq -> Command . LbaHigh   = Sector >> 16;
	    pReq -> Command . Device    = ((Sector >> 24) | 0xF0) & pDevice -> Dev;
	    pReq -> Buffer            = Buffer;
	    pReq -> Mode              = READ_DMA;
	    pReq -> BlockCount        = pDevice -> Cap . SectSize;
	    pReq -> Res . IsEvent     = False;
	    pReq -> Handler           = &ATA_ReadDma;
	    pReq -> ToHndlr           = NULL;
		Timeout = 0x1000000;
	    pReq -> Timeout           = Timeout; 
	    pReq -> Res . ReqInfo . Status . Buffer = pReq -> Buffer;
	    pReq -> Res . ReqInfo . Status . Error = ATA_NO_ERR;
	    pReq -> IsPktCmd          = False;

	    if (ATA_SubmitReq (pDevice, pReq) == True)
	    {
	      pDevice -> pIdeInt -> pIdeOps -> StartReq (pDevice -> pIdeInt -> pIntHndl, pReq);
	    }

	    Err = ATA_NO_ERR;
	  }
	}

  return Err;
}


/**
 *  \brief ATA_MED_WriteDma ()    : Write Sectors in DMA Mode.
 *  \param Buffer [IN]            : Pointer to the Buffer.
 *  \param Sector [IN]            : Starting Sector Number.
 *  \param NumSector [IN]         : Number of sector's requested.
 *  \param Handle [IN]            : Handle for this Device instance.
 *  \param MHndl  [IN]            : Handle for Media Layer
 *
 *  \return int                   : Success or Error
 ******************************************************************************/
static Int32 ATA_MED_WriteDma (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 Sector, Uint32 NumSector)
{
  ATA_Drive_t *pDevice = NULL;
  ATA_Dev_Req_t *pReq=NULL;
  Uint32         Timeout = 0;
  Int32 Err = ATA_ERR_GEN;

  if((NULL != Handle) && (NULL != Buffer))
  {
	pDevice = (ATA_Drive_t *)Handle;
  	Timeout = (pDevice -> PwrMode == ATA_DEV_ACTIVE)? ATA_CMD_TO : ATA_RESET_TO;
	  pReq = ATA_AllocReq (pDevice, False);
	  if (pReq != NULL)
	  {
	    NumSector = (NumSector == 0)? ATA_MAX_SECT : NumSector;
	    /* Update any extended Request Information (NumSector > ATA_MAX_SECT) if any */
	    if (NumSector >= ATA_MAX_SECT)
	    {
	      pReq -> ExtReqInfo = (Ptr)((NumSector - ATA_MAX_SECT) *
	                                 pDevice -> Cap . SectSize);
	      pReq -> TransferCount = ATA_MAX_SECT * pDevice -> Cap . SectSize;
	      pReq -> Command . SectorCnt = 0;
	    } else
	    {
	      pReq -> ExtReqInfo = (Ptr)0;
	      pReq -> TransferCount = NumSector * pDevice -> Cap . SectSize;
	      pReq -> Command . SectorCnt = (Uint8)NumSector;
	    }

	    pReq -> MediaHandle = MHndl;
	    pReq -> Command . Device = pDevice -> Dev;
	    pReq -> Command . Cmd = ATA_WRITE_DMA;
	    pReq -> Command . LbaLow    = Sector;
	    pReq -> Command . LbaMid    = Sector >> 8;
	    pReq -> Command . LbaHigh   = Sector >> 16;
	    pReq -> Command . Device    = ((Sector >> 24) | 0xF0) & pDevice -> Dev;
	    pReq -> Mode = WRITE_DMA;
	    pReq -> BlockCount = pDevice -> Cap . SectSize;
	    pReq -> Buffer = Buffer;
	    pReq -> Res . IsEvent = False;
	    pReq -> Handler = &ATA_WriteDma;
	    pReq -> ToHndlr = NULL;
		Timeout = 0x1000000;
	    pReq -> Timeout = Timeout;
	    pReq -> Res . ReqInfo . Status . Buffer = pReq -> Buffer;
	    pReq -> Res . ReqInfo . Status . Error = ATA_NO_ERR;
	    pReq -> IsPktCmd = False;

	    if (ATA_SubmitReq (pDevice, pReq) == True)
	    {
	      pDevice -> pIdeInt -> pIdeOps -> StartReq (pDevice -> pIdeInt -> pIntHndl,pReq);
	    }

	    Err = ATA_NO_ERR;
	  }
	}

  return Err;
}

/**
 *  \brief ATA_SubmitReq ()       : Submit an IO request on the device.
 *  \param pDevice [IN]           : Pointer to the Drive.
 *  \param pReq   [IN]            : Request Pointer
 *
 *  \return Bool                  : True -> Submitted, False -> Queued
 ******************************************************************************/
Bool ATA_SubmitReq (ATA_Drive_t * const pDevice, ATA_Dev_Req_t * const pReq)
{
  Bool Submit = False;
  Uint32 Cookie, retVal;

  if ((NULL != pDevice) && (NULL != pReq)) // && (pDevice->DevState == ATA_DEV_IO_RESUME))
  {

	  Cookie = HWI_disable ();

	  Queue_add_tail(&pReq->link, &pDevice->pReqQueHead);
	  pDevice -> ActiveReq++;
	  pDevice->pReq = GetNextReq(&pDevice->pReqQueHead);

	  if( pDevice -> pIdeInt -> BusState != ATA_IDE_FREE)
	  	gBusNotFree++;
	  	 
	  if ((pDevice -> pIdeInt -> BusState == ATA_IDE_FREE) && 
	  	  (pDevice->DevState == ATA_DEV_IO_RESUME))
	  {
	    pDevice -> pIdeInt -> BusState = ATA_IDE_BUSY;
	    pDevice -> pIdeInt -> pActiveDrive = pDevice;
	    Submit = True;
	    if (pReq -> Timeout != 0)
	    {
	      pDevice -> pIdeInt -> TimeOutData = (Ptr)pDevice;

		  timer0_start( pReq -> Timeout );
	    }

		if( is_sata_pm_enable(0) )
		{
			timer2_stop();
			pm_sata_power_on();
		}

	    retVal = pDevice -> pIdeInt -> pIdeOps -> SubmitReq (pDevice -> pIdeInt -> pIntHndl, pReq);
		if( retVal != 0 )
		{
			Submit = False;	
		}
	  }else
		gBusNotFree++;

	  HWI_restore (Cookie);
	}
  return Submit;
}

/**
 *  \brief ATA_SubmitReq ()       : Submit an IO request on the device.
 *  \param pDevice [IN]           : Pointer to the Drive.
 *  \param pReq   [IN]            : Request Pointer
 *
 *  \return Bool                  : True -> Submitted, False -> Queued
 ******************************************************************************/
Bool ATA_ResumeIOReq (ATA_Drive_t * const pDevice)
{
  Bool Submit = False;
  Uint32 Cookie, retVal;
  ATA_Dev_Req_t *pReq = NULL;

  if ((NULL != pDevice)) 
  {

	  Cookie = HWI_disable ();

	  pDevice->pReq = GetNextReq(&pDevice->pReqQueHead);
	  pReq = pDevice->pReq;

	  if ( (NULL != pReq)  && (pDevice -> pIdeInt -> BusState == ATA_IDE_FREE) && 
	  	(pDevice->DevState == ATA_DEV_IO_RESUME))
	  {
	    pDevice -> pIdeInt -> BusState = ATA_IDE_BUSY;
	    pDevice -> pIdeInt -> pActiveDrive = pDevice;
	    Submit = True;
	    if (pReq -> Timeout != 0)
	    {
	      pDevice -> pIdeInt -> TimeOutData = (Ptr)pDevice;

		  timer0_start( pReq -> Timeout );
	    }

		if( is_sata_pm_enable(0) )
		{
			timer2_stop();
			pm_sata_power_on();
		}

	    retVal = pDevice -> pIdeInt -> pIdeOps -> SubmitReq (pDevice -> pIdeInt -> pIntHndl, pReq);
		if( retVal != 0 )
		{
			Submit = False;	
		}
	  }else
		gBusNotFree++;

	  HWI_restore (Cookie);
	}
  return Submit;
}


/**
 *  \brief ATA_SubmitExtReq ()    : Submit an Extended PIO IO request on the device.
 *                                  If a IO request extends beyond ATA_MAX_SECT
 *                                  sectors we will re-use the existing request
 *                                  token to submit the next <= ATA_MAX_SECT sector
 *                                  request.
 *  \param pDevice [IN]           : Pointer to the Drive.
 *  \param pReq    [IN]           : Request Pointer
 *
 *  \return void                  :
 ******************************************************************************/
static void ATA_SubmitExtReq (ATA_Drive_t const *pDevice, ATA_Dev_Req_t * const pReq)
{
  Uint32 MaxSector = 0;
  Uint32 NxtSector = 0;
  Uint32 NumBytes = 0;
  //ATA_Dev_Stat_t Status;

  if((NULL != pDevice) && (NULL != pReq))
  {
	  MaxSector = ((pReq -> Mode == READ_PIO) || (pReq -> Mode == WRITE_PIO)) ? ATA_PIO_MAX_SECT : ATA_MAX_SECT;
  	  NxtSector = (((pReq -> Command . Device & (0x0Fu)) << 24) |
		                      ((pReq -> Command . LbaHigh  & (0xFFu))<< 16) |
		                      ((pReq -> Command . LbaMid  & (0xFFu))<< 8) |
		                      (pReq -> Command . LbaLow)) + MaxSector;
  	  NumBytes = (Uint32)(pReq -> ExtReqInfo);
	  /* Reset the Command Start Sector Number */
	  pReq -> Command . LbaLow    = NxtSector;
	  pReq -> Command . LbaMid    = NxtSector >> 8;
	  pReq -> Command . LbaHigh   = NxtSector >> 16;
	  pReq -> Command . Device    = ((NxtSector >> 24) | 0xF0) & pDevice -> Dev;

	  /* Reset the TransferCount for this request and the Request Info */
	  if (NumBytes >= (MaxSector * pDevice -> Cap . SectSize))
	  {
	    pReq -> TransferCount = MaxSector * pDevice -> Cap . SectSize;
	    pReq -> Command . SectorCnt = (MaxSector == ATA_MAX_SECT)? 0 : MaxSector;
	    NumBytes -= (MaxSector * pDevice -> Cap . SectSize);
	    pReq -> ExtReqInfo = (Ptr)NumBytes;
	  } else
	  {
	    pReq -> TransferCount = (Uint32)pReq -> ExtReqInfo;
	    pReq -> Command . SectorCnt = (Uint8)(pReq -> TransferCount / pDevice -> Cap . SectSize);
	    pReq -> ExtReqInfo = NULL;
	  }

/*	  do 
	  {
	    pDevice -> pIdeInt-> pIdeOps -> CmdStatus (pDevice -> pIdeInt -> pIntHndl, &Status, False, pReq->PortNum);
	  } while (Status.Stat & ATA_DEV_BUSY);*/

	    pDevice -> pIdeInt -> pIdeOps -> SubmitReq (pDevice -> pIdeInt -> pIntHndl, pReq);

		timer0_start( pReq -> Timeout );
	}
}


/**
 *  \brief ATA_SubmitComit ()     : Sumbmit a request irrespective or Bus status.
 *  \param pIdeInt [IN]           : ATA Bus Pointer
 *  \param pReq    [IN]           : Request Pointer
 *
 *  \return void
 ******************************************************************************/
static void ATA_SubmitComit (ATA_IDE_Int_t * const pIdeInt, ATA_Dev_Req_t * const pReq)
{
  Uint32 Cookie;

  if((NULL != pIdeInt) && (NULL != pReq))
  {
	  Cookie = HWI_disable ();

	  if (pReq != pIdeInt -> pActiveDrive -> pReq)
	  {
	    pIdeInt -> pActiveDrive -> ActiveReq++;

	 // if (pIdeInt->IsInit == True)
	    {
		  Queue_add_tail(&pReq->link, &pIdeInt -> pActiveDrive -> pReqQueHead);
	    }
	    pIdeInt -> pActiveDrive -> pReq = GetNextReq(&pIdeInt -> pActiveDrive -> pReqQueHead);
	  }

	  pIdeInt -> pIdeOps -> SubmitReq (pIdeInt -> pIntHndl, pReq);
	  if (pReq -> Timeout != 0)
	  {
	    pIdeInt -> TimeOutData = (Ptr)pIdeInt -> pActiveDrive;

		timer0_start( pReq -> Timeout );
	  }
	  HWI_restore (Cookie);

	}
}

 

/**
 *  \brief ATA_AllocReq ()  : Allocate a Requrest token from ReqCache of the device.
 *                            If IsResrv is True then allocate out of reserved requests.
 *  \param pDrive [IN]      : Pointer to the Drive.
 *  \param IsResrv [IN]     : Allocate out of reserved requests ??
 *
 *  \return ATA_Dev_Req_t * : Pointer to allocated request
 ******************************************************************************/
ATA_Dev_Req_t* ATA_AllocReq (ATA_Drive_t * const pDrive, Bool IsResrv)
{
  ATA_Dev_Req_t *pReq=NULL;
  Uint32 Cookie;

  /* compiler warning: variable was never referenced */
  if (IsResrv)
  {
  }

  if(NULL != pDrive)
  {
	  Cookie = HWI_disable ();
	
	  if( !Queue_empty (&pDrive->pReqCacheQueHead) )
	  {
		pReq = GetNextReq (&pDrive->pReqCacheQueHead);
		Queue_del( &pReq->link );
		pDrive->AvailReq--;
	  }

	  HWI_restore (Cookie);
  }

  return pReq;
}
/**
 *  \brief ATA_RelReq ()    : Release a Requrest token to ReqCache of the device.
 *  \param pDrive [IN]      : Pointer to the Drive.
 *  \param pReq   [IN]      : Request Pointer
 *
 *  \return ATA_Dev_Req_t * : Next request in Queue
 ******************************************************************************/
ATA_Dev_Req_t* ATA_RelReq (ATA_Drive_t * const pDrive, ATA_Dev_Req_t * const pReq)
{
  ATA_Dev_Req_t *pRetReq=NULL;
  Uint32 Cookie;

  if((NULL != pDrive) && (NULL != pReq))
  {

	  Cookie = HWI_disable ();

	  pReq -> TransferCount = 0;
	  pReq -> Mode = TASKFILE;
	  pReq -> Buffer = NULL;
	  pReq -> Handler = NULL;
	  pReq -> IsInternal = False;
	  pReq -> IsPktCmd = False;
	  pReq -> ToHndlr = NULL;

	  Queue_del(&pReq->link);

	  Queue_add_tail(&pReq->link, &pDrive->pReqCacheQueHead);

	  pRetReq = GetNextReq(&pDrive->pReqQueHead);

	  pDrive->AvailReq++;
	  pDrive->ActiveReq--;

	  if( is_sata_pm_enable(0) )
	  {

		if( !Is_ATA_IO_Pending() )
			timer2_start(0);
	  }

 	  HWI_restore (Cookie);

	}

  return pRetReq;
}

/**
 *  \brief ATA_DEV_CmdTimeout ()    : Handle command timeouts on ATA bus.
 *  \param Handle [IN]              : Pointer to Device.
 *
 *  \return void
 ******************************************************************************/
void ATA_DEV_CmdTimeout (void *cbParams)
{
  ATA_Drive_t **pDevice = NULL;
  ATA_Dev_Stat_t Status;
  Bool Timeout = True;

  if(NULL != cbParams)
  {
	  pDevice = (ATA_Drive_t **)(cbParams); 

	  if ((*pDevice) -> pReq != NULL)
	  {

		  (*pDevice) -> pIdeInt -> pIdeOps -> CmdStatus (
		      (*pDevice) -> pIdeInt -> pIntHndl,
		      &Status, False,(*pDevice) -> pReq->PortNum);

		  if ((*pDevice) -> pReq -> ToHndlr != NULL)
		  {
		    Timeout = (*pDevice) -> pReq -> ToHndlr ((Ptr) * pDevice, &Status);
		  }

		  if (FALSE != Timeout)
		  {
		    /* Return the current command with Timeout Error.
		     * No operation on this bus is possible until this bus is reset by the
		     * Media driver.
		     */
		    (*pDevice) -> pReq -> Res . IsEvent = False;
		    (*pDevice) -> pReq -> Res . ReqInfo . Status . Error = ATA_ERR_TO;

		    (*pDevice) -> pIdeInt -> BusState = ATA_IDE_TO;

		    if (((*pDevice) -> pReq -> IsInternal) == FALSE)
		    {
		      (*pDevice) -> MedHandler ((*pDevice) -> pReq -> MediaHandle,
		                                  &((*pDevice) -> pReq -> Res));
		    }
		    /* Release the request with ERROR TIMEOUT */
		    (*pDevice) -> pReq = ATA_RelReq ((*pDevice), (*pDevice) -> pReq);
		    (*pDevice) -> pIdeInt -> pActiveDrive = NULL;
		  }
		}
	}
}


/**
 *  \brief ATA_SchedNextReq ()     : Schedule the next request on the ATA Bus.
 *  \param pIdeInt [IN]            : Pointer to the ATA Bus.
 *  \param pReq    [IN]            : Request Pointer
 *
 *  \return void
 ******************************************************************************/
void ATA_SchedNextReq (ATA_IDE_Int_t * pIdeInt, ATA_Dev_Req_t *pReq)
{
  Bool Submit = True;
  ATA_Drive_t *pDevice = NULL;
  Uint32 Count = 0;
  Bool ReSched = False;
  Uint32 Cookie;

  if((NULL != pIdeInt) && (NULL != pReq) )
  {
	pDevice = pIdeInt -> pActiveDrive;
	  do
	  {
	    Submit = True;
	    ReSched = False;
	    if (pReq -> IsInternal == False && pDevice -> MedHandler)
	    {
	      pDevice -> MedHandler (pReq -> MediaHandle, &pReq -> Res);
	    }

	    pIdeInt -> pActiveDrive -> pReq = ATA_RelReq (pIdeInt -> pActiveDrive, pReq);

	    if (pIdeInt -> pActiveDrive -> pReq == NULL)
	    {
	      /* Look for request in the next device */
	      if (pIdeInt -> pActiveDrive == pIdeInt -> Drive)
	      {
	        if (pIdeInt -> Drive [1] . pReq != NULL)
	        {
	          pIdeInt -> pActiveDrive = &pIdeInt -> Drive [1];
	        } else
	        {
	          Submit = False;
	        }
	      } else
	      {
	        if (pIdeInt -> Drive [0] . pReq != NULL)
	        {
	          pIdeInt -> pActiveDrive = &pIdeInt -> Drive [0];
	        } else
	        {
	          Submit = False;
	        }
	      }
	    } else
	    {
	      ReSched = False;
	    }

	    if ((FALSE != Submit)  
			&& (pIdeInt -> Drive [0].DevState != ATA_DEV_IO_SUSPEND)  
		  	&& (pIdeInt -> Drive [1].DevState != ATA_DEV_IO_SUSPEND) )
	    {
		  Cookie = HWI_disable ();

	      pIdeInt -> pIdeOps -> SubmitReq (pIdeInt -> pIntHndl, pIdeInt -> pActiveDrive -> pReq);
	      pReq = pIdeInt -> pActiveDrive -> pReq;
	#ifdef ATA_ATAPI_SUPPORT
	      if (pReq -> IsPktCmd == True)
		  {
	          if(ATAPI_ProcessPacketCmd (pIdeInt -> pActiveDrive) != ATA_NO_ERR)
		      {
		        ReSched = True;
		        continue;
		      } 
		      else
		      {
		        ReSched = False;
		      }
		  }
	      else
	      {
	        ReSched = False;
	      }
	      if (pReq -> IsPktCmd == False)
	      {
	#endif
	      switch ((pReq -> Mode) & (~ATA_CMD_EXTTO))
	      {
	      case READ_DMA:
	      case WRITE_DMA:
	        pIdeInt -> pIdeOps -> StartReq (pIdeInt -> pIntHndl, pReq);
	        break;
	      case WRITE_PIO:
	        if (pIdeInt -> pActiveDrive -> pReq -> TransferCount >
	                pIdeInt -> pActiveDrive -> pReq -> BlockCount)
	        {
	          Count = pIdeInt -> pActiveDrive -> pReq -> BlockCount;
	        } else
	        {
	          Count = pIdeInt -> pActiveDrive -> pReq -> TransferCount;
	        }

	        if (ATA_PioBeginWrite (pIdeInt -> pActiveDrive, Count) == ATA_ERR_GEN)
	        {
	          /* Met with error on first sector write return the request with error */
	          ReSched = True;
	          pIdeInt -> pActiveDrive -> pReq -> Timeout = 0;
	        }
	        break;
	      default:
	        break;
	      }
	#ifdef ATA_ATAPI_SUPPORT
	      }
	#endif
	      if (pIdeInt -> pActiveDrive -> pReq -> Timeout != 0)
	      {
	        /* Start the timeout */
	        pIdeInt -> TimeOutData = (Ptr)pIdeInt -> pActiveDrive;
	        
			timer0_stop ();
			timer0_start( pReq -> Timeout );
	      }
		  HWI_restore (Cookie);

	    } else
	    {
	      ReSched = False;
	      pIdeInt -> pActiveDrive = NULL;
	      pIdeInt -> BusState = ATA_IDE_FREE;
	    }

	  } while (ReSched);
	}
}


/**
 *  \brief ATA_RetryReq ()     : Retry current request on the ATA Bus.
 *  \param pIdeInt [IN]        : Pointer to the ATA Bus.
 *  \param pReq                : Request to retry
 *
 *  \return Bool               : Retry success/Retry Fail
 ******************************************************************************/
static Bool ATA_RetryReq (ATA_IDE_Int_t const *pIdeInt, ATA_Dev_Req_t const *pReq)
{
  Bool IsSched = 1; // False;

  if ( (pIdeInt != NULL) && (pReq != NULL) )
  {
  }

  return IsSched;
}


/**
 *  \brief ATA_VoidDevops ()   : Set the Devops to Void
 *  \param pDevice [IN]        : Pointer to th ATA Device.
 *  \return void
 ******************************************************************************/
static void ATA_VoidDevops (ATA_Drive_t * const pDevice)
{
  Uint32 Cookie;

  if(NULL != pDevice)
  {

	  Cookie = HWI_disable ();

	  pDevice -> DevOps . Ata_Read = &ATA_MED_Void;
	  pDevice -> DevOps . Ata_Write = &ATA_MED_Void;
	  pDevice -> DevOps . Ata_SetDevMode = &ATA_MED_ModeVoid;
	  pDevice -> DevOps . Ata_SubAtapiReq = &ATAPI_MED_Void;
	  pDevice -> DevOps . Ata_DevReset = &ATA_MED_ResetVoid;
	  pDevice -> DevOps . Ata_SetPwrMode = &ATA_MED_PwrVoid;
	  pDevice -> DevOps . Ata_GetMaxSect = &ATA_MED_GetMaxSectVoid;
	  pDevice -> DevOps . Ata_Ioctl = &ATA_MED_IoctlVoid;

	  HWI_restore (Cookie);
  }
}


/**
 *  \brief ATA_SetDevops ()    : Set the Devops in relation to OpMode, PwMode,
 *                               Device Media status.  Runs in Interrupt Context.
 *  \param pDevice [IN]        : Pointer to Device.
 *  \return void
 ******************************************************************************/
void ATA_SetDevops (ATA_Drive_t * pDevice)
{
  if(NULL != pDevice)
  {
      //pDevice -> OpMode = DMA;
	  switch (pDevice -> PwrMode)
	  {
	  case ATA_DEV_ACTIVE:
	    switch (pDevice -> OpMode)
	    {
	    case PIO:
	      if (pDevice -> Cap . SectCnt != 0)
	      {
	        pDevice -> DevOps . Ata_Read = &ATA_MED_ReadMul;
	        pDevice -> DevOps . Ata_Write = &ATA_MED_WriteMul;
	      }else
	      {
	        pDevice -> DevOps . Ata_Read = &ATA_MED_ReadDma;//&ATA_MED_Read;
	        pDevice -> DevOps . Ata_Write =&ATA_MED_WriteDma;// &ATA_MED_Write;
			pDevice -> OpMode = UDMA;
	      }
	      break;
	    case DMA:
	    case UDMA:
	      pDevice -> DevOps . Ata_Read = &ATA_MED_ReadDma;
	      pDevice -> DevOps . Ata_Write = &ATA_MED_WriteDma;
	      break;
	    default :
	      break;
	    }
	    break;
	  case ATA_DEV_IDLE:
	  case ATA_DEV_STDBY:
	    pDevice -> DevOps . Ata_Read = &ATA_MED_ReadPwr;
	    pDevice -> DevOps . Ata_Write = &ATA_MED_WritePwr;
	    pDevice -> DevOps . Ata_SetDevMode = &ATA_MED_SetDevActive;
	    break;
	  case ATA_DEV_SLEEP:
	    pDevice -> DevOps . Ata_Read = &ATA_MED_Void;
	    pDevice -> DevOps . Ata_Write = &ATA_MED_Void;
	    pDevice -> DevOps . Ata_SetDevMode = &ATA_MED_ModeVoid;
	    pDevice -> DevOps . Ata_SubAtapiReq = &ATAPI_MED_Void;
	    pDevice -> DevOps . Ata_SetPwrMode = &ATA_MED_PwrVoid;
	    break;
	  default:
	    break;
	  }

	#ifdef ATA_REMOVABLE_MEDIA_SUPPORT
	  if (FALSE != (pDevice -> Cap . IsNoMed))
	  {
	    /* Reset the IO entry points as No Media is present */
	    pDevice -> DevOps . Ata_Read = &ATA_MED_Void;
	    pDevice -> DevOps . Ata_Write = &ATA_MED_Void;
	  }
	#endif

	  if (FALSE != (pDevice -> Cap . IsMedWp))
	  {
	    /* Reset the Write entry point */
	    pDevice -> DevOps . Ata_Write = &ATA_MED_Void;
	  }
	}

}
/**
 *  \brief ATA_MED_SuspendIO ()    : Suspend the IO operation on ATA media
 *
 *  \param Handle [IN]         : Device Handle
 *  \return int                :
 ******************************************************************************/
Int32 ATA_IDE_SuspendIO (Ptr Handle)
{

	ATA_IDE_Int_t *pIdeCtrl;
	Int32 		retVal = ATA_IO_COMPLETED;
	Uint32 i, Cookie;

	if(NULL != Handle)
	{
	    Cookie = HWI_disable ();

		pIdeCtrl = (ATA_IDE_Int_t *)Handle;

		for( i = 0; i < ATA_MAX_INT_DEV; ++i)
		{
			pIdeCtrl->Drive[i].DevState = ATA_DEV_IO_SUSPEND;
		}
		
		/* stop all PRD timers */
		timer0_stop();
		timer1_stop();
		timer2_stop();
		timer3_stop();

		for( i = 0; i < ATA_MAX_INT_DEV; ++i)
		{
			if( pIdeCtrl->Drive[i] . pIdeInt -> BusState != ATA_IDE_FREE )
			{
				retVal = ATA_IO_PROGRESS;
				break;
			}
		}

     	HWI_restore (Cookie);
	}
	return retVal;
}

/**
 *  \brief ATA_MED_SuspendIO ()    : Resume the IO operation on ATA media
 *
 *  \param Handle [IN]         : Device Handle
 *  \return int                :
 ******************************************************************************/
Int32 ATA_IDE_ResumeIO (Ptr Handle)
{
	ATA_IDE_Int_t *pIdeCtrl;
	Int32 		retVal = ATA_IO_COMPLETED;
	Uint32 i,Cookie;

	if(NULL != Handle)
	{
	    Cookie = HWI_disable ();

		pIdeCtrl = (ATA_IDE_Int_t *)Handle;

		for( i = 0; i < ATA_MAX_INT_DEV; ++i)
		{
			pIdeCtrl->Drive[i].DevState = ATA_DEV_IO_RESUME;

		}
     	HWI_restore (Cookie);

		for( i = 0; i < ATA_MAX_INT_DEV; ++i)
		{
			if( !Queue_empty(&pIdeCtrl->Drive[0].pReqQueHead) )
			{
				ATA_ResumeIOReq(&pIdeCtrl->Drive[i]);
				break;
			}
		}

	}
	return retVal;
}

/**
 *  \brief Is_ATA_IDE_Online ()    : check controller link status
 *
 *  \param Handle [IN]         : Device Handle
 *  \param drive_num [IN]      : drive num
 *  \return int                :
 ******************************************************************************/
Int32 Is_ATA_IDE_Online (Uint32 drive_num)
{
	ATA_IDE_Int_t *pIdeCtrl;
	Uint32 retVal = 0;

	pIdeCtrl = (ATA_IDE_Int_t *)getIdeCtrl(0);

	if (pIdeCtrl != NULL )
	{
		retVal = pIdeCtrl-> pIdeOps->isIdeOnline(drive_num); 
	}

	return retVal;
				
}

#ifdef BIOS_PWRM_ENABLE
/**
 *  \brief ATA_IDE_PowerDown ()    : power down the ATA IDE controller 
 *
 *  \param Handle [IN]         : Device Handle
 *  \return int                :
 ******************************************************************************/
Int32 ATA_IDE_PowerDown (Ptr Handle,ATA_DEV_PwrMode_t Mode)
{
	ATA_IDE_Int_t *pIdeCtrl;
	Int32 		retVal = ATA_IO_COMPLETED;
	Uint32 drive_num = 0;

	if(NULL != Handle)
	{
		pIdeCtrl = (ATA_IDE_Int_t *)Handle;

		if( pIdeCtrl-> pIdeOps->Is_IDE_Supports_PM(pIdeCtrl->pIntHndl, drive_num) )
		{
			/* put ATA Device in STANDBY/IDLE/SLEEP state */
			if ( Mode == ATA_DEV_SLEEP)
				ATA_IDE_SetPwrMode (&pIdeCtrl-> Drive [drive_num], 
							pIdeCtrl->pIntHndl, Mode); 

			m_delay(1);
			/* put the IDE controller to power down state */
			if ( Mode == ATA_DEV_SLEEP)
			pIdeCtrl-> Drive [drive_num].port_pm_state = 
				pIdeCtrl-> pIdeOps->SetIDE_PM_state(pIdeCtrl->pIntHndl, drive_num, ATA_IDE_PORT_PM_DOWN);

		}
		/* put IDE controller in power down state */

	}
	return retVal;
}
/**
 *  \brief ATA_IDE_PowerUp ()    : power up the ATA IDE controller 
 *
 *  \param Handle [IN]         : Device Handle
 *  \return int                :
 ******************************************************************************/
Int32 ATA_IDE_PowerUp (Ptr Handle)
{
	ATA_IDE_Int_t *pIdeCtrl;
	Int32 		retVal = ATA_IO_COMPLETED;
	Uint32 drive_num = 0;

	if(NULL != Handle)
	{
		pIdeCtrl = (ATA_IDE_Int_t *)Handle;

		if( pIdeCtrl-> pIdeOps->Is_IDE_Supports_PM(pIdeCtrl->pIntHndl, drive_num) )
		{

			/* put the IDE controller to power down state */
			if (pIdeCtrl-> Drive [drive_num].PwrMode == ATA_DEV_SLEEP)
			pIdeCtrl-> Drive [drive_num].port_pm_state = 
				pIdeCtrl-> pIdeOps->SetIDE_PM_state(pIdeCtrl->pIntHndl, drive_num, ATA_IDE_PORT_PM_UP);

			switch (pIdeCtrl-> Drive [drive_num].PwrMode)
			{
				case ATA_DEV_IDLE:
				case ATA_DEV_STDBY:
				case ATA_DEV_SLEEP:
					pIdeCtrl-> Drive [drive_num].PwrMode = ATA_DEV_ACTIVE;
					ATA_SetDevops(&pIdeCtrl-> Drive [drive_num]);
					break;
			}
		}
	}
	return retVal;
}
#endif

