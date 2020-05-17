/*
 * Sata_intf.c
 *
 * SATA Interface Driver file
 * This file implements the sata interface driver functions.
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
 * \author     . Ravi B (PSP-USB)
 * \version    . 1.0
 */

#include <std.h>
#include <iom.h>
#include <hwi.h>
#include <tsk.h>
#include <sys.h>
#include <bcache.h>
#include <string.h>
#include <assert.h> 
#include <ecm.h>
#include <stdio.h>
#include <c64.h>

#include "ata.h"
#include "ata_cmd.h"
#include "ahci.h"
#include "psp_sata.h"
#include "sata_intf.h"
#include "psp_ata_debug.h"

#include "pwrm.h"
#include "psp_ata_pwrm.h"

Int32 sataSubmitReq (Ptr, ATA_Dev_Req_t *);
Int32 sataCmdStatus (Ptr, ATA_Dev_Stat_t *, Bool, int );
Int32 sataAltStatus (Ptr, ATA_Dev_Stat_t *);
Int32 sataReadPio   (Ptr, Ptr, Uint32);
Int32 sataWritePio  (Ptr, Ptr, Uint32);
Int32 sataEndDma    (Ptr);
Int32 sataStartDmaVoid  (Ptr);
Int32 sataStartDma  (Ptr, ATA_Dev_Req_t *);
Int32 sataDevSetMode (Ptr, Uint32, Uint32, ATA_DEV_Mode_t, Uint32);
Int32 sataReset (Ptr Handle, int width);

#ifdef BIOS_PWRM_ENABLE
Int32 sataIsPwrmEnabled  (Ptr Handle);
Int32 IsSata_Supports_PM(Ptr Handle, Uint32 port_num);
Int32 GetSata_PM_state(Ptr Handle, Uint32 port_num);
Int32 SetSata_PM_state(Ptr Handle, Uint32 port_num, Uint32 pm_state);
#endif
extern void ahciIntHandler (int cmdTag, void *port);
Int32 ahciRegAtaHandler (Ptr pHandle, ATA_IDE_IntHandler_t pHandler);
extern int ahciInterrupt(void *handle);
void sataAttachIntHandler(Uint32 hbaIndx);
Int32 sataRegAtaHandler (Ptr pHandle, ATA_IDE_IntHandler_t pHandler);
void ataTaskFileToFis(ATA_Dev_Cmd_t *tf, int is_cmd, RegFis *fis);
void sataEnableIntr(Uint32 hbaIndx);
void sataDisableIntr(Uint32 hbaIndx);
Int32 isSataOnline(Uint32 drive_num);
extern Ptr getIdeCtrl(Uint32 drive_num);

/**
 *	\brief	Reset the IDE bus (Hardware Reset)
 *
 *	\param	Handle	[IN]  Handle of the IDE Controller interface
 *	\param	ResetWidth	[In] Reset High time in Milliseconds   
 *
 */
Int32 (*Reset) (Ptr Handle, int ResetWidth);

/**
 *	\brief IDE interface operations
 *	defines the sata IDE inteface operations
 */
IDE_INT_Ops_t           IDE_IntOps = {&sataReadPio,
                                      &sataWritePio,
                                      &sataSubmitReq,
                                      &sataCmdStatus,
                                      &sataRegAtaHandler,
                                      &sataDevSetMode,
                                      &sataStartDmaVoid,
				                      &sataStartDma,
									  &sataReset,
									  &isSataOnline, 

#ifdef BIOS_PWRM_ENABLE
									  &sataIsPwrmEnabled,
									  &IsSata_Supports_PM,
									  &GetSata_PM_state,
									  &SetSata_PM_state,
#endif
									  };
/**
* \brief interrupt handler
*/
static ATA_IDE_IntHandler_t    ATA_IntHandler=NULL;
Uint32 gSataTFDError = 0;

IDE_INTF_Confg_t sataIntfConfig = {
	1, 				/* IsSata = 1 */
	&IDE_IntOps,	/* IDE interface functions */
};

Ptr gpAhciHandle = NULL;

void bios_cache_flush(void *buffer, Uint32 size);
void bios_cache_invalidate(void *buffer, Uint32 size);

void bios_cache_flush(void *buffer, Uint32 size)
{
	BCACHE_wb(buffer, size, 1);
}
void bios_cache_invalidate(void *buffer, Uint32 size)
{
	BCACHE_inv(buffer, size, 1);
}

/**
 *	\brief	 Do a ATA PIO Read
 *
 *	\param Handle  [IN]           : IDE Controller info
 *	\param Buffer  [IN]           : Buffer pointer
 *	\param Count   [IN]           : Number of Bytes
 *
 * @return int                   : 1
 */
Int32 sataReadPio (Ptr Handle, Ptr Buffer, Uint32 Count)
{
  /* compiler remark: variable never referenced */
  if ( (Handle != NULL) && (Buffer != NULL) )
  {
	if (Count)
	{
	}
  }
  /* not supported */
  return IOM_EBADIO;
}


/**
 * \brief ahciWritePio   : Do a ATA PIO write
 *
 * @param Handle  [IN]           : IDE Controller info
 * @param Buffer  [IN]           : Buffer pointer
 * @param Count   [IN]           : Number of Bytes
 *
 * \return int                   : 1
 */
Int32 sataWritePio (Ptr Handle, Ptr Buffer, Uint32 Count)
{
  /* compiler remark: variable never referenced */
  if ( (Handle != NULL) && (Buffer != NULL) )
  {
	if (Count)
	{
	}
  }
	/* not supported */
    return IOM_EBADIO;
}

/**
 *	\brief	Submit a ATA request to ahci
 *
 *	\param	Handle	[IN]  ahci/sata Controller info
 *	\param	pReq	[IN]  ATA Request
 *
 *	\return 0 on success
 */
Int32 sataSubmitReq (Ptr Handle, ATA_Dev_Req_t *pReq)
{
	Int32 retVal = IOM_COMPLETED;
	AhciHostCtrl *ahci_host;
	AhciCmdHeader *CmdHdr;
	AhciCmdTable *CmdTbl; /** pointer to command Table */
	Int32 cmdTag,i;
	AhciPortData *port;
	Uint32 opts = 0,cmd_fis_len = 5, done = 1;

	do 
	{

		if ((NULL == pReq) || (NULL == Handle))/*Handle can be NULL*/
		{
			retVal = IOM_EBADARGS;
			break;
		}
		ahci_host = (AhciHostCtrl *)Handle;
		port = &ahci_host->port[pReq->PortNum];
		
		if (pReq -> Mode == CONTROL)
		{
			if (pReq -> Command . Cmd == ATA_DEV_RESET)
			{
				/* perform controller/IDE reset */
				ahciReset(Handle, 0, AHCI_PORT_RESET);
				break; 

			}
			break;
		}

		if( !port->linkOnline ) 
		{
			retVal = IOM_EBADIO;
			return retVal;
		}		
			
		/* get the free cmd slot */
		cmdTag = ahciPortGetFreeCmdSlot(port);

		CmdHdr = &port->cmdHeader[cmdTag];
		CmdTbl = &port->cmdTableBase[cmdTag];

		/* fill command FIS */
		CmdTbl->CmdFis.type 		= AHCI_H2D_FIS;
		CmdTbl->CmdFis.flags		= REGFIS_FLAGS_CMD;
		if( port->pmpEnable )
		{
			CmdTbl->CmdFis.flags	|= pReq->PMP_port;
			opts |= (pReq->PMP_port << 12); //TODO
		}
		ataTaskFileToFis (&pReq->Command, 1, &CmdTbl->CmdFis);
		bios_cache_flush(&CmdTbl->CmdFis, sizeof(RegFis));

		/* fill the command header */
		if( CmdTbl->CmdFis.u1.command == ATA_WRITE_DMA )
			opts |= AHCI_CMD_WRITE;

		/* for ATAPI request, fill the atapi packet in cmdtable */
		if( pReq->IsPktCmd )
		{
			/* set flag for command is ATAPI cmd */
			opts |= AHCI_CMD_ATAPI;
			memcpy( CmdTbl->atapi_cmd, pReq->pPktCmd->Buffer,  pReq->pPktCmd->TransCnt);
		}

		opts |= cmd_fis_len ;
		/* update the data buffers for read/write commands */
		if( pReq->TransferCount > 0 )
		{
			CmdHdr->cmdTblAdr = (Uint32)CmdTbl;

			/* fill the PRD Table */
			for(i=0; i< 1; ++i)
			{
				CmdTbl->prdTable[i].dataBaseAdr = (Uint32)pReq->Buffer;
				CmdTbl->prdTable[i].flagsDataByteCnt = 0x80000000 | (pReq->TransferCount-1); //TODO
				bios_cache_flush(&CmdTbl->prdTable[i], sizeof(AhciPrdTable));
				if(	opts & AHCI_CMD_WRITE )
					bios_cache_flush((void *)CmdTbl->prdTable[i].dataBaseAdr, pReq->TransferCount);
				else
					bios_cache_invalidate((void *)CmdTbl->prdTable[i].dataBaseAdr, pReq->TransferCount);

			}
			opts |= (i << 16);
		}
	    ahciFillCmdSlot(port, cmdTag, opts);
 
		pReq->CmdTag = cmdTag;
		pReq->CmdReady = 1; //TODO
		port->curActDevReq[cmdTag] = pReq;
		port->rxFis.D2HRegFis.u1.status = 0x80;

		bios_cache_flush(CmdTbl, sizeof(AhciCmdTable));
		bios_cache_flush(CmdHdr, sizeof(AhciCmdHeader));

		/* issue the command */
		sataStartDma(Handle, pReq);

	} while (!done);
 
	return retVal;
}

/**
 *	\brief	converts taskfiles to register fis
 *
 *	\param	Handle		[IN]    sata/ahci Controller info
 *	\param	pStatus		[IN]   Status pointer
 *	\param IsAlt		[IN]   Alternate Status requested ?
 *
 *	\return 0 on success
 */
Int32 sataCmdStatus (Ptr Handle, ATA_Dev_Stat_t * pStatus, Bool IsAlt, int portNum)
{

	AhciHostCtrl *ahci_host;
	AhciPortData *port;
	AhciRxFis *rxFis;

	ahci_host = (AhciHostCtrl *)Handle;
	port = &ahci_host->port[portNum];

	rxFis = &port->rxFis;
	if(NULL != pStatus)
	{
		bios_cache_invalidate(rxFis, sizeof(AhciRxFis));

		if( port->IntrStatus & AHCI_PORT_IRQ_D2H_REG_FIS )
		{
			pStatus -> Error 	= rxFis->D2HRegFis.u2.Error;
			pStatus -> IntReason= rxFis->D2HRegFis.flags;
			pStatus -> LbaLow 	= rxFis->D2HRegFis.lbaLow;
			pStatus -> LbaMid	= rxFis->D2HRegFis.lbaMid;
			pStatus -> LbaHigh 	= rxFis->D2HRegFis.lbaHigh;
			pStatus -> Device 	= rxFis->D2HRegFis.Device;
			pStatus -> Stat 	= rxFis->D2HRegFis.u1.status;
			/* number of bytes transferred */
			pStatus -> ErrByte 	= rxFis->D2HRegFis.sector_count * 512;
			port->IntrStatus &= ~(1);

			if( pStatus -> Error  )
			{
				ahciReset( ahci_host, port->portNum, AHCI_PORT_RESET);
				gSataTFDError++;
			}
		}
		else
		{
			pStatus -> Error 	= rxFis->pioSetupFis.Error; 
			pStatus -> IntReason= rxFis->pioSetupFis.flags;
			pStatus -> LbaLow 	= rxFis->pioSetupFis.lbaLow;
			pStatus -> LbaMid	= rxFis->pioSetupFis.lbaMid;
			pStatus -> LbaHigh 	= rxFis->pioSetupFis.lbaHigh;
			pStatus -> Device 	= rxFis->pioSetupFis.Device;
			pStatus -> Stat 	= rxFis->pioSetupFis.status;
			/* number of bytes transferred */
			pStatus -> ErrByte 	= rxFis->pioSetupFis.sector_count  * 512;
			port->IntrStatus &= ~(2);
		}

	}

	/* compiler remark; variable never referenced */
	if (IsAlt)
	{
	}

	return IOM_COMPLETED;
}

/**
 * \brief register ATAHandler
 *
 *	\param 	Handle [IN]    sata/ahci Controller info
 *	\param 	pHandler [IN]   interrupt handler
 *
 *	\return IOM_COMPLETED on success
 */
Int32 sataRegAtaHandler (Ptr pHandle, ATA_IDE_IntHandler_t pHandler)
{
	Int32 retVal  = IOM_COMPLETED;

	/* compiler remark; variable never referenced */
	if (pHandle)
	{
	}

	if (NULL != pHandler )
	{
		/* attach the ATA handler */
		ATA_IntHandler = pHandler;

		/* Enable the ATA interrupt as the ATA Interface Layer is up */
		sataEnableIntr(0);
	} 
	else 
	{
		/* Disable the ATA interrupt as the ATA Interface Layer is resetting */
		sataDisableIntr(0);
	}
  return retVal;
}

/**
 *	\brief	attach the interrupt handler
 *
 *	\param	hbaIndex	[IN] hba index number
 *
 *	\returns none
 */
void sataAttachIntHandler (Uint32 hbaIndx)
{
	AhciHostCtrl *ahci_host = getAhciHandle(hbaIndx);

	ECM_Attrs ecmattrs = ECM_ATTRS;
	ecmattrs.unmask = 1;
	ecmattrs.arg = (Arg)ahci_host;
	ECM_dispatchPlug(ahci_host->irqEventNum, (ECM_Fxn)ahci_host[hbaIndx].ahciIntrHandler,
		&ecmattrs);
	ECM_enableEvent(ahci_host->irqEventNum);
	C64_enableIER(ahci_host->irqEventGroup);
}

/**
 * \brief sataDevSetMode
 *
 *   \param Handle    [IN]  Handle of the IDE Controller interface
 *   \param Device    [IN]  ATA/ATAPI Device Number
 *   \param CycleTime [IN]  PIO/DMA cycle time values
 *   \param Mode      [IN]  Device operation Mode (PIO/DMA/UDMA)
 *   \param Level     [IN]  Level in that opmode  */
Int32 sataDevSetMode (Ptr Handle, Uint32 dev_no, Uint32 cycle, ATA_DEV_Mode_t mode, Uint32 level)
{
	/* compiler remark; variable never referenced */
	if ( (Handle != NULL) )
	{
		if (dev_no)
		{ 
		}
		if (cycle) 
		{ 
		}
		if (mode) 
		{ 
		}
		if (level) 
		{ 
		}
	}

	return IOM_COMPLETED;
}

/**
 * \brief sataDevSetMode
 *
 *   \param Handle    [IN]  Handle of the IDE Controller interface
 *   \param width    [IN]  ATA/ATAPI Device Number
 */
Int32 sataReset (Ptr Handle, int width)
{
	/* compiler remark; variable never referenced */
	if (width)
	{
	}
	ahciReset(Handle, 0, AHCI_PORT_RESET);
	return IOM_COMPLETED;
}

/**
 * \brief isSataOnline
 *
 *   \param drive_num    [IN]  port number of ahci controller
 */
Int32 isSataOnline(Uint32 drive_num)
{
	AhciHostCtrl *ahci_host = getAhciHandle(0);
	
	return ahci_host->port[drive_num].linkOnline;
}
/**
 *	\brief	enable sata interrupts
 *
 *	\param	hbaIndex[IN] hba index number
 *
 *	\return	none
 */
void sataEnableIntr(Uint32 hbaIndx)
{
	AhciHostCtrl *ahci_host = getAhciHandle(hbaIndx);

	C64_enableIER(ahci_host->irqEventGroup);
    ECM_enableEvent(ahci_host->irqEventNum);
}

/**
 * \brief	disables sata interrupts
 *
 *	\param	hbaIndex[IN] hba index number
 *
 *	\return	none
 */
void sataDisableIntr(Uint32 hbaIndx)
{
	AhciHostCtrl *ahci_host = getAhciHandle(hbaIndx);

	C64_disableIER(ahci_host->irqEventGroup);
    ECM_disableEvent(ahci_host->irqEventNum);
}

/**
 * \brief detach the sata interrupt handler
 *
 *	\param	hbaIndex[IN] hba index number
 *
 *	\returns none
 */
void sataDetachIntHandler(Uint32 hbaIndx)
{
	AhciHostCtrl *ahci_host = getAhciHandle(hbaIndx);

	ECM_enableEvent(ahci_host->irqEventNum);
	C64_disableIER(ahci_host->irqEventGroup);
}

/**
 *	\brief	sata subsystem interrupt handler
 *
 *	\param	port	[IN] pointer to ahci controller port object
 *	\param	pReq	[IN] ATA request
 *
 *	\return	none
*/
void sataIntrHandler (int portNum, void *pReq)
{
	Uint32 hbaNum = 0;

	sataDisableIntr (hbaNum);
	/* check interrupt handler is attaached and invoke the handler */
	if (ATA_IntHandler != NULL)
	{
	  /* invoke media attached media intr handler, this will the
	   function ATA_MED_IntHandler (Uint32 portNum, ATA_Dev_Req_t *pReq) from ata.c */
		ATA_IntHandler (portNum, pReq);
	}
	sataEnableIntr (hbaNum);
}

/**
 *	\brief	execute device i/o request and start the ahci port
 *
 *	\param	Handle  [IN] sata/ahci Controller info
 *	\param	pReq    [IN] ATA_Dev_Req_t
 *
 * \return 0 on success
 */
Int32 sataStartDma  (Ptr Handle,  ATA_Dev_Req_t *pReq)
{
	AhciHostCtrl *pAhciHost;
	AhciPortData *port;

	pAhciHost = (AhciHostCtrl *)Handle;
	port = &pAhciHost->port[pReq->PortNum];

	/* issue the command */
	ahciPortIssueCmd(port, pReq->CmdTag);

	/* enable the port */
	AhciEnablePort(port);

    return IOM_COMPLETED;
}

/**
 *	\brief sataStartDmaVoid
 *
 *	\param	Handle  [IN] sata/ahci Controller info
 *
 *	\return 0 on success
 */
Int32 sataStartDmaVoid  (Ptr Handle)
{
	/* compiler remark; variable never referenced */
	if (Handle != NULL)
	{
	}
    return IOM_COMPLETED;
}

/**
 * \brief sataEndDma
 *
 * stop or disables the ahci port
 *
 * @param Handle  [IN] sata/ahci Controller info
 *
 * \return 0 on success
 */
Int32 sataEndDma (Ptr Handle)
{
	AhciHostCtrl *pAhciHost;
	AhciPortData *port;

	pAhciHost = (AhciHostCtrl *)Handle;
	port = &pAhciHost->port[0];

	/* disable the port */
	AhciDisablePort (port);

	return IOM_COMPLETED;
}

#ifdef BIOS_PWRM_ENABLE
/**
 *	\brief	check whether controller support PWRM support
 *
 *	\param	Handle  [IN] sata/ahci Controller info
 *
 * \return 1 when PWRM supported else 0 
 */
Int32 sataIsPwrmEnabled  (Ptr Handle)
{
	AhciHostCtrl *pAhciHost;
	Int32 retVal = 0;

	pAhciHost = (AhciHostCtrl *)Handle;
	if( pAhciHost->plat_res->pscPwrmEnable == SATA_PSC_PWRM_ENABLE )
		retVal = 1;

    return retVal;
}

/**
 *	\brief	check whether controller support power management
 *
 *	\param	Handle  [IN] sata/ahci Controller info
 *	\param	port_num    [IN] port number
 *
 * \return 1 when PM supported else 0 
 */
Int32 IsSata_Supports_PM(Ptr Handle, Uint32 port_num)
{
	AhciHostCtrl *pAhciHost;
	pAhciHost = (AhciHostCtrl *)Handle;

	return pAhciHost->port[port_num].pm_enabled;
}
/**
 *	\brief	get controller power management mode
 *
 *	\param	Handle  [IN] sata/ahci Controller info
 *	\param	port_num    [IN] port number
 *
 * \return PM STATE on success
 */
Int32 GetSata_PM_state(Ptr Handle, Uint32 port_num)
{
	AhciHostCtrl *pAhciHost;
	Uint32 pm_state;

	pAhciHost = (AhciHostCtrl *)Handle;
	if( pAhciHost->port[port_num].pm_state != AHCI_PM_ACTIVE)
		pm_state = ATA_IDE_PORT_PM_DOWN;
	else
		pm_state = ATA_IDE_PORT_PM_UP;

	return pm_state;
}
/**
 *	\brief	set controller power management mode
 *
 *	\param	Handle  	[IN] sata/ahci Controller info
 *	\param	port_num    [IN] port number
 *	\param	pm_state    [IN] power management state
 *
 * \return IOM_COMPLETED on success else IOM error
 */
Int32 SetSata_PM_state(Ptr Handle, Uint32 port_num, Uint32 pm_state)
{
	Int32 retVal = IOM_COMPLETED;
	AhciHostCtrl *pAhciHost;
	pAhciHost = (AhciHostCtrl *)Handle;

	switch (pm_state )
	{
		case ATA_IDE_PORT_PM_DOWN :
			retVal = AhciPort_SetPowerState(&pAhciHost->port[port_num], AHCI_PM_PARTIAL);
			break;
		case ATA_IDE_PORT_PM_UP:
			retVal = AhciPort_SetPowerState(&pAhciHost->port[port_num], AHCI_PM_ACTIVE);
			break;
		default :
			retVal = IOM_EBADARGS;
			break;
	}

	return retVal;
}
#endif
/**
 *	\brief	converts taskfiles to register fis
 *
 *	\param	tf	     [IN]  taskfile of type (ATA_Dev_Cmd_t)
 *	\param	is_cmd	 [IN]  whether fis is for command/control operation
 *	\param	fis    	 [IN]  register fis
 *
 *	\return none
 */
void ataTaskFileToFis(ATA_Dev_Cmd_t *tf, int is_cmd, RegFis *fis)
{
	fis->flags		 = 0;
	
	if( is_cmd )
		fis->flags 	  |= 0x80;
	fis->u2.features  = tf->Features;
	fis->sector_count = tf->SectorCnt;
	fis->lbaLow   	  = tf->LbaLow ;
	fis->lbaMid   	  = tf->LbaMid;
	fis->lbaHigh  	  = tf->LbaHigh;
	fis->Device   	  = tf->Device;
	fis->u1.command   = tf->Cmd;
	fis->control 	  = 0;
	fis->Device   	  |= (ATA_DEVICE_OBS);
	fis->control   	  |= ATA_DEVCTL_OBS;
}

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
Int32 PSP_sataDrvInit(Uint32 inst_id, PlatformResource *platform_res)
{
	Int32 retVal = IOM_COMPLETED, done = 1;

	do 
	{
		/* peform SATA subsystem initialization */
		gpAhciHandle = (Ptr)ahciInitSata(inst_id, platform_res);

		if( NULL == gpAhciHandle )
		{
			ATA_INFO("sata Initialization failed\n");
			retVal = IOM_EBADIO;
			break;
		}

		/* resume IO */
		ATA_IDE_ResumeIO((Ptr)getIdeCtrl(0));

		/* register SATA controller with media driver */
		ATA_IDE_Register (&sataIntfConfig, gpAhciHandle);

	} while (!done);

	return retVal;
}
/**
 *	\brief	DeInitializes the sata subsystem , 
 *
 *	\param	none
 *	\return	none
 */
Int32 PSP_sataDrvDeInit(Uint32 inst_id)
{
	Int32 retVal = IOM_COMPLETED;

	ahciDeInitSata(inst_id);

	return retVal;
}





