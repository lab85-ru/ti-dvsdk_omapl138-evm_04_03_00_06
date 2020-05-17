/*
 * Atadrv.c
 *
 * ATA driver PROS adaptation file
 * This file defines the Driver adaptation for PrOs of ATA drivers.
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
 *	\version    1.2
 */

#include <std.h>
#include <iom.h>
#include <hwi.h>
#include <tsk.h>
#include <sys.h>
#include <bcache.h>
#include <string.h>
#include <assert.h> 
#include "psp_ata_debug.h"
#include "ata.h"
#include "psp_ata_pwrm.h"

#include "tistdtypes.h"

#include <psp_ata_med.h>
#include <ti/pspiom/blkmedia/psp_blkdev.h>

#define True	1
#define False	0

#define	BLK_ERR_OK	0
#define	BLK_ERR_GEN 1

#define ATA_DRV_MAXDEV		(2u)
#define ATA_DRV_MHZ		(1000000u)

static Int32 ATA_DrvRegMedia0 (PSP_BlkDevIdent_t * const pIdent, PSP_blkDevCallback_t BlockDevCb,
                                   PSP_BlkDevOps_t **pDevOps, Ptr *pHandle);
//static Int32 ATA_DrvRegMedia1 (PSP_BlkDevIdent_t * const pIdent, PSP_blkDevCallback_t BlockDevCb,
//                                   PSP_BlkDevOps_t **pDevOps, Ptr *pHandle);
static Int32 ATA_DrvRegMedia (int DevNum, PSP_BlkDevIdent_t const *pIdent, PSP_blkDevCallback_t BlockDevCb,
                                   PSP_BlkDevOps_t **pDevOps, Ptr * const pHandle);
static Int32 ATA_DrvIoctl  (Ptr Handle, Ptr MHndl, PSP_BlkDevIoctlInfo_t const * pIoctlInfo, Bool *pIsComplete);
static Bool ATA_DevCallBack0 (Ptr Handle, ATA_DEV_Res_t *pRes);
//static Bool ATA_DevCallBack1 (Ptr Handle, ATA_DEV_Res_t *pRes);
static Bool ATA_DevCallBack (Int32 DevNum, Ptr Handle, ATA_DEV_Res_t *pRes);
static Int32 ATA_Read (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 SectNum, Uint32 NumSect);
static Int32 ATA_Write (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 SectNum,Uint32 NumSect);
void attach_AtaToBlkmedia(void);


typedef struct {
  PSP_BlkDevOps_t       DevOps;
  PSP_blkDevCallback_t  DevCb;
  Ptr                 Handle;
  ATA_INT_Ops_t      *pIntOps;
}ATA_DrvInfo_t;

static ATA_DrvInfo_t ATA_DrvInfo [ATA_DRV_MAXDEV];

static int mount_cb_cnt = 0;

static PSP_BlkDrvReg_t drv_reg_info = 
{
    ATA_DrvRegMedia0
};
#if 0
static PSP_BlkDrvReg_t drv_reg_info1 = 
{
    ATA_DrvRegMedia1
};
#endif

/**
 *	\brief 	blockmedia mount call back function
 *	
 *	\param	none
 */
static void blkmedia_mount_cb(void)
{
    mount_cb_cnt++;
}
/**
 *  \brief ATA_DrvInit () : Init the ATA driver.
 *
 *  \return void
 ******************************************************************************/
void ATA_DrvDeInit (void)
{
	Int32	DevNum = 0;

	ATA_MED_UnRegister (ATA_DrvInfo [DevNum] . Handle);
	ATA_DrvInfo [DevNum] . Handle = NULL;
	ATA_DrvInfo [DevNum] . pIntOps =  NULL;

}

/**
 *  \brief ATA_DrvInit () : Init the ATA driver.
 *
 *  \return void
 ******************************************************************************/
void ATA_DrvInit (void)
{
	Int32            i, DevNum = 0;
	PSP_BlkDevEvent_t EventInfo;
	ATA_DEV_Ident_t DevIdent [2]={0,};
	Int32 Err = BLK_ERR_GEN;
	PSP_BlkDrvId_t driverId;
	PSP_BlkDrvIoctlInfo_t  drv_ioctl_info;
	

#ifdef _DEBUG
	ATA_INFO ("ATA Build: Debug.\n");
#else
	ATA_INFO ("ATA Build : Release.\n");
#endif

	  for (i = 0; i < ATA_DRV_MAXDEV; i++)
	  {
		ATA_DrvInfo [i] . DevOps . Blk_Ioctl = &ATA_DrvIoctl;
		ATA_DrvInfo [i] . DevOps . Blk_Read  = &ATA_Read;
	    ATA_DrvInfo [i] . DevOps . Blk_Write = &ATA_Write;
		ATA_DrvInfo [i] . DevCb = 0;
		ATA_DrvInfo [i] . Handle = 0;
	  }


	  DevIdent [DevNum] . IsMaster = 1;
	  ATA_DrvInfo [DevNum] . Handle = NULL;
	  Err = ATA_MED_Register (&DevIdent[DevNum], &ATA_DevCallBack0,
	                     &(ATA_DrvInfo [DevNum] . pIntOps),
	                     &(ATA_DrvInfo [DevNum] . Handle));
	  if (Err == ATA_NO_ERR)
	  {
	    if (FALSE == (DevIdent [DevNum] . IsAta))
	    {
	      ATA_MED_UnRegister (ATA_DrvInfo [DevNum] . Handle);
	      ATA_DrvInfo [DevNum] . Handle = NULL;
	      ATA_DrvInfo [DevNum] . pIntOps =  NULL;
	    } else
	    {
	      DevNum++;
	    }
	  }

	  if ( Is_ATA_IDE_Online (0) )
	  {
		ATA_INFO("SATA PHY initialized : online\n");
	  }
	  else
	  {
		ATA_INFO("SATA PHY Not initialized : offline\n");
		ATA_INFO("Check for SATA cable connected properly\n");
	  }

	  if (ATA_DrvInfo [0] . Handle != NULL)
	  {
		 drv_ioctl_info.Cmd = PSP_BLK_DRV_SET_INIT_COMP_CALLBACK;
		 drv_ioctl_info.pData = (void *)&blkmedia_mount_cb;

		 driverId = PSP_BLK_DRV_ATA0;
		 PSP_blkmediaDrvIoctl((void *)&driverId, &drv_ioctl_info);

	     drv_reg_info . DeviceReg = &ATA_DrvRegMedia0;

		 PSP_blkmediaDrvRegister(driverId, &drv_reg_info);
	     EventInfo . EventId = PSP_BLK_EVT_MEDINS;
	     PSP_blkmediaCallback (driverId, &EventInfo);
	 }


#if 0
	  DevIdent [DevNum] . IsMaster = 0;
	  ATA_DrvInfo [DevNum] . Handle= NULL;
	  Err = ATA_MED_Register (&DevIdent[DevNum], (DevNum == 0) ? &ATA_DevCallBack0 : &ATA_DevCallBack1,
	                     &(ATA_DrvInfo [DevNum] . pIntOps),
	                     &(ATA_DrvInfo [DevNum] . Handle));
	  if ((Err == ATA_NO_ERR) && (FALSE == (DevIdent [DevNum] . IsAta)))
	  {
	    ATA_MED_UnRegister (ATA_DrvInfo [DevNum] . Handle);
	    ATA_DrvInfo [DevNum] . Handle = NULL;
	    ATA_DrvInfo [DevNum] . pIntOps =  NULL;
	  }

	  if (ATA_DrvInfo [1] . Handle != NULL)
	  {
		 drv_ioctl_info.Cmd = PSP_BLK_DRV_SET_INIT_COMP_CALLBACK;
		 drv_ioctl_info.pData = (void *)&blkmedia_mount_cb;

		 driverId = PSP_BLK_DRV_ATA1;
		 psp_rc = PSP_blkmediaDrvIoctl((void *)&driverId, &drv_ioctl_info);

	     drv_reg_info1 . DeviceReg = &ATA_DrvRegMedia1;
		 psp_rc = PSP_blkmediaDrvRegister(driverId, &drv_reg_info1);
	     EventInfo . EventId = PSP_BLK_EVT_MEDINS;
	     PSP_blkmediaCallback (driverId, &EventInfo);
	  }
#endif
}

/*
 *  \brief ATA_DrvRegMedia0 ()  : Register for the block device 0.
 *  \param pIdent  [IN]         : Identification information.
 *  \param BlockDevCb [IN]      : Callback routine for the registered block Device.
 *  \param pDevOps [OUT]        : Block Device Interface Operations.
 *  \param pHandle [OUT]        : Handle to device (to be used for future
 *                                transactions).
 *  \return int                 : Success/Error
 ******************************************************************************/
static Int32 ATA_DrvRegMedia0 (PSP_BlkDevIdent_t * const pIdent, PSP_blkDevCallback_t BlockDevCb,
                                   PSP_BlkDevOps_t **pDevOps, Ptr *pHandle)
{
  return ATA_DrvRegMedia (0, pIdent, BlockDevCb, pDevOps, pHandle);
}
#if 0
/*
 *  \brief ATA_DrvRegMedia1 ()  : Register for the block device 1.
 *  \param pIdent  [IN]         : Identification information.
 *  \param BlockDevCb [IN]      : Callback routine for the registered block Device.
 *  \param pDevOps [OUT]        : Block Device Interface Operations.
 *  \param pHandle [OUT]        : Handle to device (to be used for future
 *                                transactions).
 *  \return int                 : Success/Error
 ******************************************************************************/
static Int32 ATA_DrvRegMedia1 (PSP_BlkDevIdent_t * const pIdent, PSP_blkDevCallback_t BlockDevCb,
                                   PSP_BlkDevOps_t **pDevOps, Ptr *pHandle)
{
  return ATA_DrvRegMedia (1, pIdent, BlockDevCb, pDevOps, pHandle);
}
#endif
/*
 *  \brief ATA_DrvRegMedia ()  : Register for the block device.
 *  \param DevNum  [IN]         : Device Number to be registered.
 *  \param pIdent  [IN]         : Identification information.
 *  \param BlockDevCb [IN]      : Callback routine for the registered block Device.
 *  \param pDevOps [OUT]        : Block Device Interface Operations.
 *  \param pHandle [OUT]        : Handle to device (to be used for future
 *                                transactions).
 *  \return int                 : Success/Error
 ******************************************************************************/
static Int32 ATA_DrvRegMedia (Int32 DevNum, PSP_BlkDevIdent_t const *pIdent, PSP_blkDevCallback_t BlockDevCb,
                                   PSP_BlkDevOps_t **pDevOps, Ptr * const pHandle)
{
  Int32 Err = BLK_ERR_GEN;

  /* compiler remark: variable never referenced */
  if (pIdent!= NULL )
  {
  }

  if((NULL != pDevOps) && ((NULL != pHandle) && (NULL != BlockDevCb)))
  {
	  if (ATA_DrvInfo [DevNum] . Handle != NULL)
	  {
	    *pDevOps = &ATA_DrvInfo [DevNum] . DevOps;
	    *pHandle = &ATA_DrvInfo [DevNum];
	    ATA_DrvInfo [DevNum] . DevCb = BlockDevCb;

	    Err =  BLK_ERR_OK;
	  }
	}

  return Err;
}

/**< Block Device IOCTL
 *   \param Handle     [IN]  Handle of the Block Device
 *   \param MHndl      [IN]  Job Handle from this module
 *   \param pIoctlInfo [IN]  IOCTL information
 *   \param pIsComplete     [OUT] Is the IOCTL complete
 ******************************************************************************/
static Int32 ATA_DrvIoctl  (Ptr Handle, Ptr MHndl, PSP_BlkDevIoctlInfo_t const * pIoctlInfo,
                      Bool *pIsComplete)
{
  ATA_DrvInfo_t *pDevice = NULL;
  Uint32            *pData   = NULL;
  Uint32            *pData1  = NULL;
  Int32              Err     = BLK_ERR_GEN;
  ATA_Drive_t *pAtaDevice = NULL;


  if (((Handle != NULL) && (pIoctlInfo != NULL))
   && ((pIsComplete != NULL) && (MHndl != NULL)))
  {
		Err     = BLK_ERR_OK;
		pDevice = (ATA_DrvInfo_t *)Handle;
	    pAtaDevice = (ATA_Drive_t *)pDevice -> Handle;
		pData   = (Uint32 *)(Uint32)(pIoctlInfo -> pData);
		pData1  = (Uint32 *)(Uint32)(pIoctlInfo -> pData1);
  
	  *pIsComplete = True;
	  switch (pIoctlInfo -> Cmd)
	  {
	  case PSP_BLK_GETSECTMAX :
	    *pData = pDevice -> pIntOps -> Ata_GetMaxSect (pDevice -> Handle);
	    break;
	  case PSP_BLK_GETBLKSIZE :
	    *pData = ATA_DEV_BPS;
	    break;
	  case PSP_BLK_SETOPMODE :
	    switch (*pData)
		{
	    case PSP_OPMODE_AUTO :
	    case PSP_OPMODE_PIO :
	    case PSP_OPMODE_DMA :
	    case PSP_OPMODE_UDMA :
	      Err = pDevice -> pIntOps -> Ata_SetDevMode (pDevice -> Handle, (ATA_DEV_Mode_t)(*pData),
	                                                  (Uint32)(*pData1));
		  if (Err != ATA_ERR_GEN)
		  {
		    Err = BLK_ERR_OK;
		  }
		  break;
		default :
		  Err = BLK_ERR_GEN;
		  break;
		}
	    break;
	  case PSP_BLK_GETOPMODE :
		Err = BLK_ERR_OK;
		break;

	  case PSP_BLK_GETREMSTAT:
	    /* Not a removable Media */
	    *(Bool*)pData = False;
	    break;
	  case PSP_BLK_DEVRESET :
	  /* Initiate a reset only under a Bus Timeout or Device Sleep scenario */
	    Err = pDevice -> pIntOps -> Ata_DevReset (pAtaDevice -> pIdeInt);
		break;
	  case PSP_BLK_SETPWRMODE :
	    Err = pDevice -> pIntOps -> Ata_SetPwrMode (ATA_DrvInfo [0] . Handle, MHndl,
	                                  (ATA_DEV_PwrMode_t)(*pData));
		break;
	  case PSP_BLK_GETWPSTAT:
	    *((Bool *)(pIoctlInfo -> pData)) = False;
		break;
	  default :
	    Err = pDevice -> pIntOps -> Ata_Ioctl (pDevice -> Handle, (ATA_DEV_IoctlCmd_t)pIoctlInfo -> Cmd, pIoctlInfo -> pData);
	    break;
	  }
	}

  return Err;
}

/**
 * \brief ATA_DevCallBack0 : ATA Driver interface Block Device Callback
 *
 * \param  Handle   :  Handle for the request
 * \param  pRes     :  Pointer to Error Info
 * \return Bool     : True
 *****************************************************************************/
static Bool ATA_DevCallBack0 (Ptr Handle, ATA_DEV_Res_t *pRes)
{
  return ATA_DevCallBack (0, Handle, pRes);
}

#if 0
/**
 * \brief ATA_DevCallBack1 : ATA Driver interface Block Device Callback
 *
 * \param  Handle   :  Handle for the request
 * \param  pRes     :  Pointer to Error Info
 * \return Bool     : True
 *****************************************************************************/
static Bool ATA_DevCallBack1 (Ptr Handle, ATA_DEV_Res_t *pRes)
{
  return ATA_DevCallBack (1, Handle, pRes);
}
#endif
/**
 * \brief ATA_DevCallBack : ATA Driver interface Block Device Callback
 *
 * \param  DevNum   :  Device Number
 * \param  Handle   :  Handle for the request
 * \param  pRes     :  Pointer to Error Info
 * \return Bool     : True
 *****************************************************************************/
static Bool ATA_DevCallBack (Int32 DevNum, Ptr Handle, ATA_DEV_Res_t *pRes)
{
  PSP_BlkDevEvent_t    EventInfo;

  if((NULL != Handle) && (NULL != pRes))
  {


	  if (FALSE != (pRes -> IsEvent))
	  {
	    switch (pRes -> ReqInfo . Event . EventId)
		{
		case ATA_DEV_EVT_RES_BEGIN:
		  ATA_DrvInfo [DevNum] . Handle = NULL;
	      EventInfo . EventId = PSP_BLK_EVT_MEDREM;
	      EventInfo . EvtData = &ATA_DrvInfo [DevNum];
	      PSP_blkmediaCallback ((DevNum == 0) ? PSP_BLK_DRV_ATA0 : PSP_BLK_DRV_ATA1, &EventInfo);
		  break;
		case ATA_DEV_EVT_RES_END:
	      EventInfo . EventId = PSP_BLK_EVT_MEDINS;
	      PSP_blkmediaCallback ((DevNum == 0) ? PSP_BLK_DRV_ATA0 : PSP_BLK_DRV_ATA1, &EventInfo);
		  break;
		case ATA_DEV_EVT_PWR_SLEEP:
		case ATA_DEV_EVT_PWR_STDBY:
		case ATA_DEV_EVT_PWR_IDLE:
		case ATA_DEV_EVT_PWR_ACTIVE:
		  pRes -> IsEvent = False;
	      ATA_DrvInfo [DevNum] . DevCb (Handle, (PSP_BlkDevRes_t *)pRes);
		  break;
	    default:
		  break;
		}
	  } else
	  {
	    if (pRes -> ReqInfo . Status . Error == ATA_ERR_TO)
		{
		  ATA_DrvInfo [DevNum] . pIntOps -> Ata_DevReset (ATA_DrvInfo [DevNum] . Handle);
		}
		if (ATA_DrvInfo [DevNum] . DevCb)
		    ATA_DrvInfo [DevNum] . DevCb (Handle, (PSP_BlkDevRes_t *)pRes);
	  }
	}

  return True;
}

/**
 * \brief ATA_Read :Block device Read function Definition
 *
 *  Block device IO function type definitions.
 *
 * \param Handle [IN]  Device Handle for the device
 * \param MHndl  [IN]  Media Driver Handle for this device
 * \param Buffer [IN]  Buffer Pointer
 * \param SectNum [IN] Sector Number to start
 * \param NumSect [IN] Number of sectors to transfer
 ******************************************************************************/
static Int32 ATA_Read (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 SectNum,
                            Uint32 NumSect)
{
  return ((ATA_DrvInfo_t *)Handle) -> pIntOps -> Ata_Read (
          ((ATA_DrvInfo_t *)Handle) -> Handle, MHndl, Buffer, SectNum, NumSect);
}

/**
 * \brief ATA_Write : Block device Write function Definition
 *
 *  Block device IO function type definitions.
 *
 * \param Handle [IN]  Device Handle for the device
 * \param MHndl  [IN]  Media Driver Handle for this device
 * \param Buffer [IN]  Buffer Pointer
 * \param SectNum [IN] Sector Number to start
 * \param NumSect [IN] Number of sectors to transfer
 ******************************************************************************/
static Int32 ATA_Write (Ptr Handle, Ptr MHndl, Ptr Buffer, Uint32 SectNum,
                            Uint32 NumSect)
{
  return ((ATA_DrvInfo_t *)Handle) -> pIntOps -> Ata_Write (
          ((ATA_DrvInfo_t *)Handle) -> Handle, MHndl, Buffer, SectNum, NumSect);
}

