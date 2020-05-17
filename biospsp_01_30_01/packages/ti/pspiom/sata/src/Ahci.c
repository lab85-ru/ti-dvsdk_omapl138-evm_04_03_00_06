/*
 * Ahci.c
 *
 * This file implements the ahci interface driver functions.
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

#include "ata.h"
#include "ata_cmd.h"
#include "ahci.h"
#include "sata_intf.h"
#include "sata_pm.h"
#include "psp_ata_debug.h"
#include "psp_ata_pwrm.h"

#pragma DATA_ALIGN(gAhciHost,1024u)
AhciHostCtrl gAhciHost[SATA_CONFIG_MAX_NUM_AHCI_HOST];
PlatformResource sataPlatRes;

/**
 *	\brief	static functions
 */
static Uint32	AhciGetNumOfCmdSlot(AhciHostCtrl *pAhciHost);
static Uint32	AhciGetNumOfPorts(AhciHostCtrl *pAhciHost);
static void	AhciEnable(AhciHostCtrl *pAhci_host);
static void AhciInitCap(AhciHostCtrl *pAhciHost);
static void AhciPrintInfo(AhciHostCtrl *pAhciHost);
static void AhciRestoreInitConfig(AhciHostCtrl *pAhciHost);
static void AhciPortIntr(AhciPortData *port);
static int AhciHbaReset(AhciHostCtrl *pAhciHost);
static int AhciSoftReset(AhciHostCtrl *ahci_host, int port_num);
static Int32 AhciResetHostController(AhciHostCtrl *pAhciHost);

/**
 *	\brief	global functions
 */
void ahciStartPort(AhciPortData *port);
void ahciStopPort(AhciPortData *port);
void AhciPortIssueCmd(AhciPortData *port, Uint32 tag);
void ahciFillCmdSlot(AhciPortData *port, Uint32 cmdTag,Uint32 opts);
void sataIntrHandler (int portNum, void *pReq);
void AhciPortIntrEnable(AhciHostCtrl *ahci_host, Uint32 port_num, Uint32 intr_mask);
void AhciPortIntrDisable(AhciHostCtrl *ahci_host,Uint32 port_num);
void ahciInitPort (AhciPortData *port);
Uint32 ahciInterrupt(void *handle);
Int32 ahciReset(AhciHostCtrl *pAhciHost, Uint32 portNum, Uint32 resetType);
Int32 ahciPortGetFreeCmdSlot(AhciPortData *port);
Int32 AhciExecPolledCmd(Ptr Handle, ATA_Dev_Req_t *pReq);


/**
 * \brief	extern function declaration
 */
extern void ahci_IntHandler (int port_num, void *);
extern void ataTaskFileToFis(ATA_Dev_Cmd_t *tf, int is_cmd, RegFis *fis);
extern void bios_cache_flush(void *buffer, Uint32 size);
extern void bios_cache_invalidate(void *buffer, Uint32 size);
extern void timer2_stop(void);
extern void sataPscClkEnable(void);
extern void sataPscClkDisable(void);
extern Ptr getIdeCtrl(Uint32 drive_num);
extern Int32 sataStartDma  (Ptr Handle,  ATA_Dev_Req_t *pReq);

/**
 * \brief get ahci handle
 *
 *	\param	none
 *	\return	handle to ahci host cntrl object
 */
AhciHostCtrl *getAhciHandle(Uint32 hbaIndex)
{
	return  &gAhciHost[hbaIndex];
}
/**
 *  \brief	AhciEnable : This function enables the AE(AHCI Enable) bit in ahci host controller
 *
 *  \param	pAhciHost	[IN]  handle to ahci host controller object
 *
 *  \return	none
 */
static void AhciEnable(AhciHostCtrl *pAhci_host)
{
	Uint32 tmp;

    /* turn on AHCI_EN */
    tmp = pAhci_host->reg->GHC;
    if ( (tmp & (Uint32)AHCI_HOST_AHCI_EN) == 0)
    {
		tmp |= (Uint32)AHCI_HOST_AHCI_EN;
		pAhci_host->reg->GHC = tmp;
	}
}

/**
 *  \brief	AhciInitCap : This function saves the initial capabilities 
 *      	of ahci controller and enable/disable the capabilities 
 *			based on configuration flags
 *
 *  \param	pAhciHost	 	[IN] handle to ahci host controller object
 *
 *  \return	none
*/
static void AhciInitCap(AhciHostCtrl *pAhciHost)
{
	Uint32 flags = pAhciHost->privFlags ;
	Uint32 cap;

	/* enable ahci before accessing CAP register */
	AhciEnable (pAhciHost);

	/* save the AHCI capabilities and ports implemented */
	pAhciHost->origCap = cap = pAhciHost->reg->CAP;
	pAhciHost->NumPort = AhciGetNumOfPorts(pAhciHost);

    if ((cap & AHCI_HOST_CAP_NCQ) && (flags & (Uint32)SATA_FLAG_DISABLE_NCQ))
    {
         cap &= ~AHCI_HOST_CAP_NCQ;
    }

    if (!(cap & AHCI_HOST_CAP_NCQ) && (flags &SATA_FLAG_ENABLE_NCQ))
    {
         cap |= AHCI_HOST_CAP_NCQ;
    }

    if ((cap & AHCI_HOST_CAP_PMP) && (flags & SATA_FLAG_DISABLE_PMP))
    {
         cap &= ~AHCI_HOST_CAP_PMP;
    }

	/* save the original CAP reg values */
	pAhciHost->origCap = cap;
	pAhciHost->NumPort = 1; /* forcing number of Ports to 1 */ 

}

/**
 *	\brief	This function prints the capabilities of ahci controller
 *
 *	\param	pAhciHost	[IN] handle to ahci host controller object
 *
 *	\return	none
 */
static void AhciPrintInfo(AhciHostCtrl *pAhciHost)
{
#ifdef ENABLE_ATA_INFO_DEBUG
	Uint32 cap ,speed;
	Uint32 version;
	const char *spdBuf;
#endif

	if (pAhciHost == NULL)
		return ;

#ifdef ENABLE_ATA_INFO_DEBUG
    version = pAhciHost->reg->VR;
    cap  = pAhciHost->reg->CAP;

	speed = (cap >> 20) & 0xf;
    if (speed == 1)
            spdBuf = "1.5";
    else if (speed == 2)
            spdBuf = "3";
    else
            spdBuf = "?";

 	ATA_INFO4("Version : %02x%02x.%02x%02x\n", (version >> 24) & 0xff,
                (version >> 16) & 0xff,
                (version >> 8) & 0xff,
                version & 0xff);

 	ATA_INFO1("NumOfCmdSlots : %d\n", ((cap >> 8) & 0x1f) + 1);
 	ATA_INFO1("NumOfPorts : %d\n", (cap & 0x1f) + 1);
 	ATA_INFO1("Speed : %s Gbps\n",spdBuf);

    ATA_INFO4("SATA:AHCI-CAP:%s%s%s%s", 
                cap & (Uint32)(1 << 31) ? "64bit " : "",
                cap & (1 << 30) ? "SNCQ " : "",
                cap & (1 << 29) ? "SSNTF " : "",
                cap & (1 << 28) ? "SMPS " : "");
	ATA_INFO4("%s%s%s%s",
                cap & (1 << 27) ? "SSS " : "",
                cap & (1 << 26) ? "SLAP " : "",
                cap & (1 << 25) ? "SAL " : "",
                cap & (1 << 24) ? "SCLO " : "");
	ATA_INFO4("%s%s%s%s",
                cap & (1 << 19) ? "SNZO " : "",
                cap & (1 << 18) ? "SAM " : "",
                cap & (1 << 17) ? "SPM " : "",
                cap & (1 << 15) ? "PMD " : "");
	ATA_INFO3("%s%s%s\n",
                cap & (1 << 14) ? "SSC " : "",
                cap & (1 << 13) ? "PSC " : "",
                cap & (1 << 6) ? "EMS ": ""
                );
#endif
}

/**
 * \brief	m_delay provide mill sec delay
 *
 * \param	msecDelay	 	[IN] delay in milli second
 *
 * \return	none
 */
void m_delay( Uint32 msecDelay)
{
	volatile Uint32 i,j;

//	TSK_sleep(msecDelay);
	for(i=0; i<msecDelay; ++i)
		for(j=0; j<0x1fff; ++j); //TODO ???

}

/**
 *	\brief	This function restore the init config of ahci
 *
 *	\param	pAhciHost	[IN] handle to ahci host controller object
 *
 *	\return	none
 */
void AhciRestoreInitConfig(AhciHostCtrl *pAhciHost)
{
	pAhciHost->reg->CAP =  pAhciHost->origCap ;
	pAhciHost->reg->PI =   pAhciHost->NumPort;
}

/**
 *	\brief	This functions returns the number of ports implemented by ahci controller
 *
 *	\param	pAhciHost	[IN] pointer to ahci host controller
 *
 *	\return	number of ports
 */
static Uint32 AhciGetNumOfPorts(AhciHostCtrl *pAhciHost)
{
	Uint32 numPorts;

	numPorts = (pAhciHost->reg->CAP & 0x1F) + 1;

	if( numPorts > SATA_CONFIG_MAX_NUM_AHCI_PORTS )
		numPorts = SATA_CONFIG_MAX_NUM_AHCI_PORTS;

	return numPorts;
}

/**
 *	\brief	This functions returns the number of ports 
 *			implemented by ahci controller
 *
 *	\param	pAhciHost	[IN] pointer to ahci host controller
 *
 *	\return	number of ports
 */
static Uint32 AhciGetNumOfCmdSlot(AhciHostCtrl *pAhciHost)
{
	return (pAhciHost->reg->CAP >> 8) & 0x1f;
}

/**
 *	\brief	get free command slot/tag from ahci port
 *
 *	\param	AhciPortData	[IN] pointer to ahci host port
 *
 *	\return IOM_EBADIO if no free command slot is available
 *             else returns the free command slot number.
 */
Int32 ahciPortGetFreeCmdSlot(AhciPortData *port)
{
	Uint32 word;
	Int32 tag = IOM_EBADIO,i;

	/* check free command slot from currently
	   active cmds */
	do
	{
	word = port->regs->PxCI;
	} while(word);
//	word |= port->curActCmdIssued;

	/* if NCQ enabled must check SActive register */
	if( TRUE == port->ncqEnable )
	{
		word |= port->regs->PxSACT;
	}

	/* get free cmd slot */
	for(i=0; i<32; ++i)
	{
		if( (word & (1<<i)) == 0 )
		{
			tag = i;
			break;
		}
	}

	tag = 0; // always use slot-0 for ncq not enable
	return tag;
}

/**
 *	\brief	Enable the operation of port, this will enable port dma
 *
 *	\param	port[IN] pointer to ahci controller port object
 *
 *	\return	none
 */
void AhciEnablePort(AhciPortData *port)
{
	volatile unsigned int val;

	if( 0 == (port->regs->PxCMD & AHCI_PORT_CMD_ENABLE) )
	{
		val = port->regs->PxCMD;
		val |= AHCI_PORT_CMD_ENABLE;
		port->regs->PxCMD = val;
	}
}

/**
 *	\brief	Disable the operation of port, this will disable port dma
 *
 *	\param	port[IN] pointer to ahci controller port object
 *
 *	\return IOM_COMPLETED on success else return IOM_EBADIO
 */
Int32 AhciDisablePort(AhciPortData *port)
{
	Uint32 tmp,delay=0x1FFFF;
	Int32 retVal = IOM_COMPLETED;

	tmp = port->regs->PxCMD;

	/* check if the HBA is idle */
	if( ( tmp & AHCI_PORT_CMD_ENABLE) != 0 )
	{
		/* setting HBA to idle */
		tmp &= ~AHCI_PORT_CMD_ENABLE;
		port->regs->PxCMD &= tmp;

		do{
			/* check if the HBA is idle */
			if( (tmp & AHCI_PORT_CMD_ENABLE ) == 0 )
				break;
			tmp = port->regs->PxCMD;
		}while( delay--);

		if( tmp & AHCI_PORT_CMD_LIST_ON )
			retVal = IOM_EBADIO;
	}
	return retVal;
}
/**
 *	\brief	Stop the receive FIS on the port
 *
 *	\param	port[IN] pointer to ahci controller port object
 *
 *	\return	IOM_COMPLETED on success else return IOM_EBADIO
 */
Int32 AhciStopRxFis(AhciPortData *port)
{

	Uint32 tmp,delay=0x1FFFF;
	Int32 retVal = IOM_EBADIO;

	port->regs->PxCMD &= ~AHCI_PORT_CMD_FIS_ON;

	do
	{
		tmp = port->regs->PxCMD;

		/* check if the HBA is idle */
		if( 0 == (tmp & AHCI_PORT_CMD_FIS_ON) )
		{
			retVal = IOM_COMPLETED;
			break;
		}

	}while( delay--);

	return retVal;
}
/**
 *	\brief	disable the agressive link power management of ahci port	
 *
 *	\param	port	[IN] 	pointer to ahci controller port object
 *
 *	\return	none
 */
void AhciPowerDown(AhciPortData *port)
{
	AhciHostCtrl *ahci_host;
    Uint32 cmd, scontrol;

	ahci_host = port->ahciHost;
    if (!(ahci_host->cap & AHCI_HOST_CAP_SSS))
            return;

    /* put device into listen mode, first set PxSCTL.DET to 0 */

    scontrol = port->regs->PxSCTL;
    scontrol &= ~0xf;
	port->regs->PxSCTL = scontrol;

    /* then set PxCMD.SUD to 0 */

    cmd = port->regs->PxCMD & ~AHCI_PORT_CMD_ICC_MASK;
    cmd &= ~AHCI_PORT_CMD_SPIN_UP;
	port->regs->PxCMD = cmd;
}


/**
 *	\brief this function set power management states of the interface
 *
 *	\param	port			[IN] pointer to ahci controller port object
 *	\param	port_link_state	[IN] power management state
 *
 *	\return	IOM_COMPLETED on success
 */
Int32 AhciPort_SetPowerState(AhciPortData *port, Uint32 port_link_state)
{
	Int32 retVal = IOM_COMPLETED;
	Int32 temp = port->regs->PxCMD;
	Uint32 scontrol;

	temp &= ~(AHCI_PORT_CMD_ICC_MASK);

    scontrol = port->regs->PxSCTL;
    scontrol &= ~0xf;
	scontrol &= ~CSL_SATA_P0SCTL_IPM_MASK;

	switch ( port_link_state )
	{
		case AHCI_PM_ACTIVE :
			temp |= AHCI_PORT_CMD_ICC_ACTIVE;
			break;
		case AHCI_PM_PARTIAL :
			scontrol |= AHCI_PORT_CTRL_IPM_DIS_SLUMBER;
			port->regs->PxSCTL = scontrol;
			temp |= AHCI_PORT_CMD_ICC_PARTIAL;
			break;
		case AHCI_PM_SLUMBER :
			scontrol |= AHCI_PORT_CTRL_IPM_DIS_PARTIAL;
			port->regs->PxSCTL = scontrol;
			temp |= AHCI_PORT_CMD_ICC_SLUMBER;
			break;
		default :
			retVal = IOM_EBADARGS;
			break;
	}
	
	if( retVal != IOM_EBADARGS )
		port->regs->PxCMD = temp;

	//TODO initialize port power status
	 port->pm_state = port_link_state;
	 port->pm_enabled = 1;

	return retVal;
}

/**
 *	\brief this function start the operation of the ahci port
 *
 *	\param	port	[IN] pointer to ahci controller port object
 *
 *	\return	none
 */
void ahciStartPort(AhciPortData *port)
{
    /* enable FIS reception */
	port->regs->PxCMD |= AHCI_PORT_CMD_FIS_RX; 

    /* enable port dma  */
    AhciEnablePort (port);

}

/**
 *	\brief	This function start the operation of the ahci port
 *
 *	\param	port	[IN] pointer to ahci controller port object
 *
 *	\return	none
 */
void ahciStopPort(AhciPortData *port)
{
    /* enable port dma  */
    AhciDisablePort (port);

    /* stop FIS reception */
    AhciStopRxFis (port);
}

/**
 *	\brief	This function start the operation of the ahci port
 *
 *	\param	pAhciHost	[IN] pointer to ahci controller  object
 *
 *	\return	IOM_COMPLETED
 */
static Int32 AhciResetHostController(AhciHostCtrl *pAhciHost)
{
	Uint32 tmp,delay;

	/* we must be in AHCI mode, before using anything
	* AHCI-specific, such as HOST_RESET.
	*/
	AhciEnable (pAhciHost);

	/* global controller reset */
	tmp = pAhciHost->reg->GHC;
	if ((tmp & AHCI_HOST_RESET) == 0)
	{
		pAhciHost->reg->GHC = tmp | AHCI_HOST_RESET;
	}

	delay = 0x1FFFF;
	do
	{
	    tmp = pAhciHost->reg->GHC;
	}while( (tmp & AHCI_HOST_RESET) && delay--);


	if (tmp & AHCI_HOST_RESET)
	{
	    return 1;
	}

	/* turn on AHCI mode */
	AhciEnable (pAhciHost);

	/*  restore to initial values */
	AhciRestoreInitConfig(pAhciHost);

	return IOM_COMPLETED;
}

/**
 *	\brief	This function initialize the ahci port
 *
 *	\param	port	[IN] pointer to ahci controller port object
 *
 *	\return	none
 */
void ahciInitPort (AhciPortData *port)
{
	AhciHostCtrl *pAhciHost;
    Uint32 tmp;

	pAhciHost = port->ahciHost;

    /* make sure port is not active */
    ahciStopPort (port);

    /* clear SError */
    tmp = port->regs->PxSERR;
	port->regs->PxSERR = tmp;

    /* clear port IRQ */
    tmp = port->regs->PxIS;
    if ( tmp )
    {
		port->regs->PxIS = tmp;
	}
    pAhciHost->reg->IS =  (1 << port->portNum);

	port->curActCmdIssued = 0;
}

/**
 *	\brief	This function initiates the command execcution 
 *			corresponds to command tag
 *
 *	\param	port	[IN] pointer to ahci controller port object
 *
 *	\returns none
 */
void ahciPortIssueCmd(AhciPortData *port, Uint32 tag)
{

	if( port->ncqEnable )
	{
		port->regs->PxSACT = (1 << tag);
	}
	port->regs->PxCI = (1 << tag);
	port->curActCmdIssued |= (1 << tag);
}

/**
 *	\brief	This function disables the ahci interrupt 
 *
 *	\param	port	[IN] pointer to ahci controller port object
 *
 *	\return	None
 */
void AhciIntrDisable(void) //AhciPortData *port)
{
    volatile Uint32 tmp,status;
	AhciHostCtrl *hAhciHost = getAhciHandle(0);
	AhciPortData *port = &hAhciHost->port[0];

	/* disable the port */
    tmp = hAhciHost->reg->GHC;
	tmp &= ~AHCI_HOST_IRQ_EN;
	hAhciHost->reg->GHC = tmp;
    tmp = hAhciHost->reg->GHC;

    status = port->regs->PxIS;
	port->regs->PxIS = status;
}
/**
 *	\brief	This function enables the ahci interrupt
 *
 *	\param	port	[IN] pointer to ahci controller port object
 *
 *	\return	None
 */
void AhciIntrEnable(void) //AhciPortData *port)
{
    volatile Uint32 tmp,status;
	AhciHostCtrl *hAhciHost = getAhciHandle(0);
	AhciPortData *port = &hAhciHost->port[0];

	/* disable the port */
    tmp = hAhciHost->reg->GHC;
	tmp |= AHCI_HOST_IRQ_EN;
	hAhciHost->reg->GHC = tmp;
    tmp = hAhciHost->reg->GHC;

    status = port->regs->PxIS;
	port->regs->PxIS = status;

}

/**
 *	\brief	This function initiates port reset
 *
 *	\param	port	[IN] pointer to ahci controller port object
 *
 *	\return	IOM_COMPLETED on success else error IOM_EBADIO
 */
static Int32 AhciPortLinkReset(AhciPortData *port)
{
    volatile Uint32 scontrol,last,sstatus,tmp;
	volatile Uint32 cur,timeout=0xffff,serror,temp;
	Int32 retVal = IOM_EBADIO;
	AhciHostCtrl *hAhciHost = port->ahciHost;

	/* disable the port */
    tmp = hAhciHost->reg->GHC;
	tmp &= ~AHCI_HOST_IRQ_EN;
	hAhciHost->reg->GHC = tmp;
	AhciPortIntrDisable(hAhciHost, port->portNum);
	AhciDisablePort (port);

	/* set BSY flag */
	port->rxFis.D2HRegFis.u1.status = 0x80; 

	port->linkOnline = 0;

	temp = port->regs->PxSCTL;
	if( temp & 1 )
		port->regs->PxSCTL = 0;

	/* issue phy reset */
	scontrol = port->regs->PxSCTL;
	scontrol = (scontrol & 0x0f0) | 0x301;
	port->regs->PxSCTL = scontrol;
	temp = port->regs->PxSCTL ;

	m_delay(4);


	/* bring phy out of reset  */
	scontrol = port->regs->PxSCTL;
	scontrol = (scontrol & 0x0f0) | 0x300;
	port->regs->PxSCTL = scontrol;

	m_delay(2);

	cur = port->regs->PxSSTS;
	cur &= 0xf;
	last = cur;
	while(timeout--) 
	{
		cur = port->regs->PxSSTS;
		cur &= 0xf;

		if( cur == last ){
			continue;
		}
		last = cur;
	}
	
	/* clear error */
	serror = port->regs->PxSERR;
	port->regs->PxSERR = serror;

	/* check link is online */
	sstatus = port->regs->PxSSTS;
	if( 0x03 == (sstatus & 0xf) )
	{
		port->linkOnline = 1;
		//ATA_INFO("SATA PHY initialized : online\n");
		retVal = IOM_COMPLETED;
	}
	else
	{
		//ATA_INFO("SATA PHY Not initialized : offline\n");
		//ATA_INFO("Check for SATA cable connected properly\n");
	}

	ahciInitPort(port);

	/* enable interrupt */
	AhciPortIntrEnable(hAhciHost, port->portNum, port->intrMask );

	/* Enable the global interrupt from HBA */
    tmp = hAhciHost->reg->GHC;
	hAhciHost->reg->GHC = tmp | AHCI_HOST_IRQ_EN;
    tmp = hAhciHost->reg->GHC;

	/* enable the port */
	ahciStartPort (port);
//	sataEnableIntr(0);

	return retVal;
}

/**
 *	\brief	This function fills the command slot
 *
 *	\param	port	[IN] pointer to ahci controller port object
 *
 *	\returns none
 */
void ahciFillCmdSlot(AhciPortData *port, Uint32 cmdTag,
                               Uint32 opts)
{
	AhciCmdHeader *cmdHdr;

	cmdHdr = &port->cmdHeader[cmdTag];
	cmdHdr->options = opts;
	cmdHdr->prdByteCount = 0;
}


/**
 *	\brief	This function executes the port command in polled mode
 *
 *	\param pAhciHost[IN] pointer to ahci controller object
 *
 *	\returns 0 on success
 */
Int32 AhciExecPolledCmd(Ptr Handle, ATA_Dev_Req_t *pReq)
{

	Int32 retVal = IOM_COMPLETED;
	AhciHostCtrl *ahci_host;
	AhciCmdHeader *CmdHdr;
	AhciCmdTable *CmdTbl; /** pointer to command Table */
	Int32 cmdTag,i;
	AhciPortData *port;
	Uint32 opts = 0,cmd_fis_len = 5, done = 1;
	volatile Uint32 status;
	void *buf;
	Uint32 len;

	do 
	{

		if ((NULL == pReq) || (NULL == Handle))/*Handle can be NULL*/
		{
			retVal = IOM_EBADARGS;
			break;
		}

		AhciIntrDisable();

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

		do {
			status = port->regs->PxIS;
		}while ( !(status & port->regs->PxIE));

		port->regs->PxIS = status;
		port->IntrStatus = status;

		/* invalidate the data */
		bios_cache_invalidate(ahci_host->port[port->portNum].cmdTableBase, sizeof(AhciCmdTable));
		bios_cache_invalidate(ahci_host->port[port->portNum].cmdHeader, sizeof(AhciCmdHeader));
		bios_cache_invalidate((void *)&ahci_host->port[port->portNum].rxFis, sizeof(AhciRxFis));

		if( CmdTbl->CmdFis.u1.command != ATA_WRITE_DMA &&  pReq->TransferCount > 0 )
		{
			buf = (void *)ahci_host->port[port->portNum].cmdTableBase[i].prdTable[0].dataBaseAdr;
			len = ahci_host->port[port->portNum].cmdHeader[i].prdByteCount;
			bios_cache_invalidate(buf, len);
		}

	} while (!done);

	AhciIntrEnable();
 
	return retVal;
}

/**
 *	\brief	Perform soft reset or device reset on select ahci port
 *
 *	\param	pAhciHost	[IN] pointer to ahci controller object
 *	\param	portNum		[IN] port number
 *
 * 	\returns 0 on success
 */
Int32 AhciSoftReset(AhciHostCtrl *ahci_host, int port_num)
{
	AhciPortData *port = &ahci_host->port[port_num];
	Uint32 timeout_ms = 1000,status;
	ATA_Dev_Cmd_t tf;
	Int32 retVal = IOM_EBADIO, done = 1;

	do 
	{
		//PAL_osMemSet(&tf, 0, sizeof(ATA_Dev_Cmd_t));
		memset(&tf, 0, sizeof(ATA_Dev_Cmd_t));
		tf.Device |= ATA_SRST;

	    /* spec says at least 5us,wait for 1ms */
	    m_delay(1);

	    /* issue the second D2H Register FIS */
	    tf.Device &= ~ATA_SRST;

		  /* wait for link to become ready */
		do{
			status = port->regs->PxTFD && 0xFF;
			if( !(status & 0x80) )
				break;
			m_delay(1);
		}while( timeout_ms-- );

		status = port->regs->PxTFD & 0xFF;
		if( status )
			retVal = IOM_EBADIO;
		else
			retVal = IOM_COMPLETED;

	} while(!done);

	return retVal;
}
/**
 * \brief AhciHbaReset
 *
 * perform ahci Controller reset or HBA Reset
 *
 * @param pAhciHost[IN] pointer to ahci controller object
 *
 * @returns 0 on success
 */
static int AhciHbaReset(AhciHostCtrl *pAhciHost)
{
	return AhciResetHostController(pAhciHost);
}
/**
 *	\brief	perform ahci Controller reset or HBA Reset
 *
 *	\param	pAhciHost	[IN] pointer to ahci controller object
 *	\param	portNum 	[IN] port number
 *
 *	\return IOM_COMPLETED on success
 */
Int32 ahciReset(AhciHostCtrl *pAhciHost, Uint32 portNum, Uint32 resetType)
{
	int retVal = IOM_EBADIO;

	switch( resetType )
	{
		case AHCI_HBA_RESET :
			retVal = AhciHbaReset (pAhciHost);
			break;

		case AHCI_PORT_RESET :
			retVal = AhciPortLinkReset (&pAhciHost->port[portNum]);
			break;

		case AHCI_SOFT_RESET :
			retVal = AhciSoftReset (pAhciHost, portNum);
			break;
		default :
			retVal = IOM_EBADARGS;
	}
	return retVal;
}

/**
 *	\brief	This is interrupt handler, process the port interrupts of ahci port.
 *
 *	\param	pAhciHost	[IN] pointer to ahci controller object
 *
 *	\return None
 */
static void AhciPortIntr(AhciPortData *port)
{
    Uint32 status, qc_active, done_mask,i,serror;
	AhciHostCtrl *ahci_host;
	ATA_Dev_Req_t *cur_req;
	void *buf;
	AhciCmdTable *CmdTbl; /** pointer to command Table */
	Uint32 len;

	ahci_host = (AhciHostCtrl *)port->ahciHost;

	/* acknownledge all the port interrupts */
    status = port->regs->PxIS;
	port->regs->PxIS = status;
	port->IntrStatus = status;

    if ((port->flags & SATA_FLAG_DISABLE_HOTPLUG) &&
            (status & AHCI_PORT_IRQ_PHYRDY))
    {
            status &= ~AHCI_PORT_IRQ_PHYRDY;
			port->regs->PxSERR = ((1 << 16) | (1 << 18));
    }


    if (status & AHCI_PORT_IRQ_ERROR)
    {

		if( status & SATA_PORT_IRQ_PHYRDY )
		{
			/* change in the PHY Ready Signal of the port
			   this reflects the P#SERR.DIAG_N bit, need to
			   clear by the software */
			serror = port->regs->PxSERR;
			port->regs->PxSERR = serror;

			/* TODO : Sata device may be disconnected 
			   need to inform to ATA stack about
			   device removal */
			timer0_stop();
			timer1_stop();
			timer2_stop();
			//ATA_INFO("sata device disconnected\n");
			//ATA_IDE_SuspendIO(getIdeCtrl(0));
		}

		if( status & SATA_PORT_IRQ_CONNECT )
		{
			/* Port connect change status, new sata 
			   device connected */
			/* perform the device reset */
			serror = port->regs->PxSERR;
			port->regs->PxSERR = serror;
			ahci_host->reg->IS |= ahci_host->reg->IS;
			timer0_stop();
			timer1_stop();
			timer2_stop();
			ahciReset( ahci_host, port->portNum, AHCI_PORT_RESET);
			ATA_IDE_ResumeIO(getIdeCtrl(0));
		}
		else 
		{

			/* fatal/non-fatal error occured,
			   perform recovery of the ahci port link */
			serror = port->regs->PxSERR;
			port->regs->PxSERR = serror;
		}
       // return;
    }

	if (status & AHCI_PORT_IRQ_SDB_FIS) {
	    if (ahci_host->cap & AHCI_HOST_CAP_SNTF){
					/* device has set Notification bit
					   send notification to stack */
					//TODO not implemented
		}
	    else {
//            Uint32 SDB_flags = port->rxFis.setDevBitFis.flags;
	    }
	}

    if (port->ncqEnable && port->sActive)
    {
        qc_active = port->regs->PxSACT;
		done_mask  = qc_active ^ port->sActive;
	}
	else
	{
        qc_active = port->regs->PxCI;
		done_mask = port->curActCmdIssued;
	}

    for (i = 0; i < port->maxNumCmdSlot; i++)
    {
		if (!(done_mask & (1 << i)))
				continue;

		port->curActCmdIssued &= ~(1<<i);
		cur_req = port->curActDevReq[i];
		port->curActDevReq[i] = 0;

		/* invalidate the data */
		bios_cache_invalidate(ahci_host->port[port->portNum].cmdTableBase, sizeof(AhciCmdTable));
		bios_cache_invalidate(ahci_host->port[port->portNum].cmdHeader, sizeof(AhciCmdHeader));
		bios_cache_invalidate((void *)&ahci_host->port[port->portNum].rxFis, sizeof(AhciRxFis));

		CmdTbl = &ahci_host->port->cmdTableBase[i];

		if( CmdTbl->CmdFis.u1.command != ATA_WRITE_DMA &&  cur_req->TransferCount > 0 )
		{
			buf = (void *)ahci_host->port[port->portNum].cmdTableBase[i].prdTable[0].dataBaseAdr;
			len = ahci_host->port[port->portNum].cmdHeader[i].prdByteCount;
			bios_cache_invalidate(buf, len);
		}


		/* comand completion */
		sataIntrHandler (port->portNum, cur_req); 
    }
}
/**
 *	\brief	This is interrupt handler, process the all port 
 *			interrupts of ahci port.
 *
 *	\param	handle	[IN] pointer to ahci controller object
 *
 *	\returns 1 on IRQ-Handled else 0
 */
Uint32 ahciInterrupt(void *handle)
{
	AhciHostCtrl *pAhciHost = (AhciHostCtrl *)handle;
	AhciPortData *port;

    Uint32 i, irqHandled = 0;
    Uint32 irqStat, irqMasked;

    /* sigh.  0xffffffff is a valid return from h/w */
    irqStat = pAhciHost->reg->IS;
    if (!irqStat)
            return irqHandled;

	/* get interrupt raised for which portnum */
    irqMasked = irqStat &  pAhciHost->NumPort;

    for (i = 0; i < pAhciHost->numPorts; i++)
    {
		if (!(irqMasked & (1 << i)))
				continue;
		port = &pAhciHost->port[i];
		AhciPortIntr(port);
		irqHandled = 1;
    }

	/* acknowledge the interrupts */
	pAhciHost->reg->IS = irqStat;

    return irqHandled;
}


/**
 *	\brief	This function reset the ahci host controller and ports and
 *			does the hardinit of cap register and re-establish the link
 * 			with the attached devices and enable the ahci interrupt
 *
 *	\param	hAhciHost	[IN] handle to ahci host controller object
 *
 *	\return IOM_COMPLETED
 */
static Int32 AhciHwInit(AhciHostCtrl *pAhciHost)
{
	AhciPortData *port;
	Uint32 tmp,i,rc;
	Int32 retVal = IOM_COMPLETED;

    /* save initial config */
    AhciInitCap(pAhciHost);

	/* perform reset to  controller */
	rc = AhciResetHostController(pAhciHost);
	if( rc )
		return rc;

	/* do port init */
	for( i = 0; i < pAhciHost->numPorts; ++i) 
	{
		port = &pAhciHost->port[i];

		/* initialized the port */
		ahciInitPort( port );
	}

	/* Enable the global interrupt from HBA */
    tmp = pAhciHost->reg->GHC;
	pAhciHost->reg->GHC = tmp | AHCI_HOST_IRQ_EN;
    tmp = pAhciHost->reg->GHC;

	/* print ahci version and capabilities */
	AhciPrintInfo (pAhciHost);

	return retVal;
}
/**
 *	\brief	This function enables the port interrupts
 *
 *	\param	pAhciHost	[IN] pointer to ahci controller object
 *	\param	port_num	[IN] port number of ahci host ctrl
 *	\param	intr_mask	[IN] interrupt mask
 *
 *	\return	None
 */
void AhciPortIntrEnable(AhciHostCtrl *ahci_host,Uint32 port_num, Uint32 intr_mask)
{

	AhciPortData *port = &ahci_host->port[port_num];

	port->regs->PxIS |= port->regs->PxIS;

	port->intrMask = (port->regs->PxIE | intr_mask);
	port->regs->PxIE = port->intrMask;
	port->intrMask = port->intrMask;
}

/**
 *	\brief	This function disables the port interrupts
 *
 *	\param	pAhciHost	[IN] pointer to ahci controller object
 *	\param	port_num	[IN] port number of ahci host ctrl
 *	\param	intr_mask	[IN] interrupt mask
 *
 *	\returns None
 */
void AhciPortIntrDisable(AhciHostCtrl *ahci_host,Uint32 port_num)
{
	AhciPortData *port = &ahci_host->port[port_num];

	port->regs->PxIE = 0; 
	port->regs->PxIS |= port->regs->PxIS;
}

/**
 *	\brief This function initializes the AHCI compliance Serial 
 *		ATA subsystem's data structures, and reset the ahci 
 *		controller and initializes the host controller and 
 *		its ports to operational state.
 *
 *	\param	hAhciHost	 	[IN] handle to ahci host controller object
 *
 *	\return  none
 */
static void AhciInitHostCtrl(AhciHostCtrl *hAhciHost)
{
	AhciPortData *port;
	AhciPortRegs *portRegs;
	Int32 i,j;

	hAhciHost->numPorts = AhciGetNumOfPorts(hAhciHost);

	/* for each ports of host controller initialize the
	 * port data structures
	 */
	for( i = 0 ; i < hAhciHost->numPorts; ++i)
	{
		port = &hAhciHost->port[i];
		hAhciHost->portreg[i] = (AhciPortRegs *)((Uint32)hAhciHost->reg + (i+1) * AHCI_PORT_OFFS) ;
		portRegs = hAhciHost->portreg[i];

		/* initialize port data structure, i.e, portnum, pointer to hostctrl,
		 * portregs,number of cmdslots supported by each ahci ports,
		 * and PRDT length of PRD table
		 */
		port->portNum = i;
		port->ahciHost = hAhciHost;
		port->ahciRegs = hAhciHost->reg;
		port->regs = portRegs;
		port->ncqEnable = 0;
		port->maxNumCmdSlot = AhciGetNumOfCmdSlot(hAhciHost);
		port->maxNumPrdtLen = SATA_CONFIG_MAX_NUM_PRD_ENTRIES;

		/* init port cmd/FB base address */
		portRegs->PxCLB = (Uint32)&port->cmdHeader[0];
		portRegs->PxFB  = (Uint32)&port->rxFis;

		/* enable DPE interrupt */
		port->intrMask = (AHCI_PORT_IRQ_PIOS_FIS | AHCI_PORT_IRQ_D2H_REG_FIS 
					| AHCI_PORT_IRQ_ERROR | AHCI_PORT_IRQ_IF_NONFATAL);

		for( j = 0; j < port->maxNumCmdSlot; ++j)
		{
			port->cmdHeader[j].cmdTblAdr = (Uint32)&port->cmdTableBase[j];
		}
	}

	/* hardware init of ahci host controller */
	AhciHwInit(hAhciHost);

	/* put port to idle/active state */
	AhciPort_SetPowerState(port, AHCI_PM_ACTIVE);

	/* Define ATA interrupt handler */
	sataAttachIntHandler (hAhciHost->hbaNum); 

	for( i = 0 ; i < hAhciHost->numPorts; ++i)
	{
		port = &hAhciHost->port[i];
		ahciInitPort(port);
		AhciPortIntrEnable(hAhciHost, port->portNum, port->intrMask );
		/* enable port */
		ahciStartPort(port);
	}

	/* enable interrupts */
	sataEnableIntr(hAhciHost->hbaNum);

	hAhciHost->initDone = AHCI_HBA_INITIALIZED;
}

/**
 *	\brief	This function initializes the AHCI compliance Serial ATA subsystem.
 *
 *	\param	hbaIndex	[IN] AHCI compliance host bus adaptor index ( there
 *                          can be multiple ahci host controller)
 *	\param	plat_res	[IN] platform resource for sata module
 *
 *	\return	Ptr			[OUT] returns pointer to ahci handle.
 */
Ptr ahciInitSata (Uint32 hbaIndex, PlatformResource *plat_res)
{
	AhciHostCtrl *pAhciCtrl = NULL;

	if( (hbaIndex < SATA_CONFIG_MAX_NUM_AHCI_HOST) && (NULL != plat_res) ) 
	{
		/* clear memory of host controller object */
		memset (&gAhciHost[hbaIndex], 0, sizeof(AhciHostCtrl));

		/* initialize HBA base address and irq_number */
	    gAhciHost[hbaIndex].reg 			= (CSL_SataRegs *)plat_res->res->baseAdr;
		gAhciHost[hbaIndex].irqEventNum 	= plat_res->res->irqEventNum;
		gAhciHost[hbaIndex].irqEventGroup 	= plat_res->res->irqEventGroup;
		gAhciHost[hbaIndex].privFlags 		= SATA_USER_CTRL_FLAG;
		gAhciHost[hbaIndex].hbaNum 			= hbaIndex;
		gAhciHost[hbaIndex].ahciIntrHandler = &ahciInterrupt;
		sataPlatRes = *plat_res;
		gAhciHost[hbaIndex].plat_res 		= &sataPlatRes;

		if( gAhciHost[hbaIndex].plat_res->pscPwrmEnable == SATA_PSC_PWRM_ENABLE )
		{
			/* initialize sata pm */
			pm_sata_init(&gAhciHost[hbaIndex]);
		}
		else
		{
			sataPscClkEnable();
		}

		/* initialize the ahci host controller */
		AhciInitHostCtrl(&gAhciHost[hbaIndex]);
		pAhciCtrl = &gAhciHost[hbaIndex];

	}

	return (Ptr)pAhciCtrl;
}
/**
 *	\brief	This function de-initializes the AHCI compliance Serial ATA subsystem.
 *
 *	\param	hbaIndex	[IN] AHCI compliance host bus adaptor index ( there
 *                          can be multiple ahci host controller)
 *
 *	\return  none
 */
void ahciDeInitSata (Uint32 hbaIndex)
{
	AhciHostCtrl *pAhciHost = getAhciHandle(hbaIndex);
	Uint32	i,tmp,intState;

	intState = HWI_disable();
	/* diasable AHCI controller interrupt */
    tmp = pAhciHost->reg->GHC;
	pAhciHost->reg->GHC = (tmp & ~AHCI_HOST_IRQ_EN);
    tmp = pAhciHost->reg->GHC;

	/* disable sata interrupt event */
	sataDisableIntr(hbaIndex);

	for( i = 0 ; i < pAhciHost->numPorts; ++i)
	{
		/* Disable the RxFIS reception */
		AhciStopRxFis(&pAhciHost->port[i]);

		/* stop the port */
		ahciStopPort(&pAhciHost->port[i]);

		/* disable the port interrupt */
		AhciPortIntrDisable(pAhciHost, pAhciHost->port[i].portNum);
	}
	
	if( pAhciHost->plat_res->pscPwrmEnable == SATA_PSC_PWRM_ENABLE )
	{
		/* de-initialize sata pm */
		pm_sata_deinit(pAhciHost);
	}
	else
	{
		sataPscClkDisable();
	}
	timer0_stop();
	timer1_stop();
	timer2_stop();

	HWI_restore(intState);

}


