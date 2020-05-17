/*
 * Atapi.c
 *
 * ATAPI core Driver file
 * This file implements the ATAPI support for ATA standard.
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
/*****************************************************************************
 lint -e740		//unusual pointer cast (incompatible indirect types)
 lint -e611		//suspicious cast
 lint -e631        //tag 'xxx' defined differently in files
 *+--------------------------------------------------------------------------+**
*******************************************************************************/

#include <std.h>
#include <iom.h>
#include <hwi.h>
#include <tsk.h>
#include <sys.h>
#include <bcache.h>
#include <string.h>
#include <assert.h> 

#include "tistdtypes.h" 
#include "ata.h"
#include "atapi.h"
#include "ata_cmd.h"
#include "psp_ata_med.h"
#include "psp_atapi_med.h"
#include "psp_ata_debug.h"
#include "psp_ata_pwrm.h"

#define True  1
#define False 0

static Bool ATAPI_PktTimeout (void const * Handle, ATA_Dev_Stat_t const *pStatus);
static Int32 ATAPI_SubmitReq (Ptr Handle, Ptr MedHndl, ATA_Req_Mode_t Mode, Int8 Feature, Ptr pCmd);

extern Uint32 ataRemMedTimeoutData;
extern void timer0_start(Uint32 timeout);
extern void timer0_stop(void);
extern void timer1_start(Uint32 timeout);
extern void timer1_stop(void);

extern TIMER_OBJ timer0,timer1;
/**
 *  \brief ATAPI_MED_Dma ()         : Submit a RAW ATAPI command when in DMA mode.
 *
 *  \param Handle  [IN]             : Device Handle
 *  \param MedHndl [IN]             : Handle for Media Layer
 *  \param pCmd    [IN]             : Pointer to Command
 *  \return int                     :
 ******************************************************************************/
Int32 ATAPI_MED_Dma (Ptr Handle, Ptr MedHndl, Ptr pCmd)
{
    ATAPI_Dev_Req_t *pPktCmd = NULL;
    ATA_Req_Mode_t Mode = TASKFILE;
    ATA_RawCmdMode_t RawMode;
    ATA_Drive_t      *pDevice = NULL;
    Bool ExTo;

    if ((pCmd == NULL) || (NULL == Handle))
    {
		
        return ATA_ERR_GEN;
    }


	pPktCmd = (ATAPI_Dev_Req_t *)pCmd;
	pDevice = (ATA_Drive_t *)Handle;

    if (pDevice -> Cap . SectSize != 0)
    {
      /* Return error if one request spans more than IDE_MAX_DMA_DISC DMA
       * descriptors.
       */
      if (pPktCmd -> TransCnt > (IDE_MAX_DMA_DISC_TRANS_SZ * IDE_MAX_DMA_DISC))
      {
        return ATA_ERR_GEN;
      }
    }

    ExTo = ((pPktCmd -> Mode) & ATA_CMD_EXTTO)? TRUE : FALSE;
    RawMode = (ATA_RawCmdMode_t)((pPktCmd -> Mode) & (~ATA_CMD_EXTTO));
    if (RawMode == ATA_MED_RAW_READ)
    {
        Mode = READ_DMA;
    } else 
	{
	    if (RawMode == ATA_MED_RAW_WRITE)
	    {
	        Mode = WRITE_DMA;
	    }
	}

    if (FALSE != ExTo)
    {
        Mode |= ATA_CMD_EXTTO;
    }

    /* For DMA set the feature to 1 */
    return ATAPI_SubmitReq (Handle, MedHndl, Mode, 1, pCmd);
}


/**
 *  \brief ATAPI_MED_Pio ()      : Submit a RAW ATAPI command when in PIO mode.
 *
 *  \param Handle  [IN]             : Device Handle
 *  \param MedHndl [IN]             : Handle for Media Layer
 *  \param pCmd    [IN]             : Pointer to Command
 *  \return int                     :
 ******************************************************************************/
Int32 ATAPI_MED_Pio (Ptr Handle, Ptr MedHndl, Ptr pCmd)
{
    ATAPI_Dev_Req_t *pPktCmd = NULL;
    ATA_Req_Mode_t Mode = TASKFILE;
    ATA_RawCmdMode_t RawMode;
    Bool ExTo;

    if (pCmd == NULL)
    {
        return ATA_ERR_GEN;
    }

	pPktCmd = (ATAPI_Dev_Req_t *)pCmd;

    ExTo = ((pPktCmd -> Mode) & ATA_CMD_EXTTO)? TRUE : FALSE;
    RawMode = (ATA_RawCmdMode_t)((pPktCmd -> Mode) & (~ATA_CMD_EXTTO));
    if (RawMode == ATA_MED_RAW_READ)
    {
        Mode = READ_PIO;
    } else
    { 
	    if (RawMode == ATA_MED_RAW_WRITE)
	    {
	        Mode = WRITE_PIO;
	    }
	}

    if (FALSE != ExTo)
    {
        Mode |= ATA_CMD_EXTTO;
    }

    /* For PIO set the feature to 0 */
    return ATAPI_SubmitReq (Handle, MedHndl, Mode, 0, pCmd);
}


/**
 *  \brief ATAPI_SubmitReq ()      : Submit a RAW ATAPI command.
 *
 *  \param Handle  [IN]             : Device Handle
 *  \param MedHndl [IN]             : Handle for Media Layer
 *  \param Mode    [IN]             : Mode of ATAPI command execution
 *  \param Feature [IN]             : Feature Value
 *  \param pCmd    [IN]             : Pointer to Command
 *  \return int                     :
 ******************************************************************************/
static Int32 ATAPI_SubmitReq (Ptr Handle, Ptr MedHndl, ATA_Req_Mode_t Mode, Int8 Feature,
                     Ptr pCmd)
{
    ATA_Drive_t *pDevice = NULL;
    ATA_Dev_Req_t *pReq;
    ATAPI_Dev_Req_t *pPktCmd = NULL;
    Int32 Err = ATA_ERR_GEN;

	if((NULL != Handle) && (NULL != pCmd))
	{
		pDevice = (ATA_Drive_t *)Handle;
		pPktCmd = (ATAPI_Dev_Req_t *)pCmd;
	    if (pDevice -> pIdeInt -> BusState != ATA_IDE_TO)
	    {
	      pReq = ATA_AllocReq (pDevice, FALSE);
	    } else
	    {
	      return ATA_ERR_TO;
	    }

	    if (pReq != NULL)
	    {
	        pReq -> Command . Features = Feature;
	        pReq -> Command . Device = pDevice -> Dev;
	        pReq -> MediaHandle = MedHndl;
	        pReq -> Buffer = pPktCmd -> Buffer;
	        pReq -> Command . Cmd = ATAPI_PACKET;
	        pReq -> Command . SectorCnt = 0;
	        pReq -> Command . LbaLow = 0;
	        if (Feature == 0)
	        {
	          pReq -> Command . LbaMid = (Uint8)(pDevice -> Cap . SectSize);
	          pReq -> Command . LbaHigh = (Uint8)((pDevice -> Cap . SectSize & 0xFF00) >> 8);
	        } else
	        {
	          pReq -> Command . LbaMid = 0;
	          pReq -> Command . LbaHigh = 0;
	        }
	        pReq -> IsPktCmd = TRUE;
	        pReq -> TransferCount = pPktCmd -> TransCnt;
	        pReq -> BlockCount = pDevice -> Cap . SectSize;
	        pReq -> Res . IsEvent = FALSE;
	        pReq -> Handler = &ATAPI_PktHndlr;
	        pReq -> Mode = Mode;

	        /* set the buffer to the packet command */
	        pReq -> Res . ReqInfo . Status . Buffer = pPktCmd -> Cmd;

	        if (pDevice -> Cap . IsDrqInt == False)
	        {
	            /* Wait for  packet command status until 10 ms */
	            pReq -> Retry = 1u;
	            pReq -> Timeout = ATA_TMSEC_TO;
	            pReq -> ToHndlr = &ATAPI_PktTimeout;
	        }
	        else
	        {
	            pReq -> Retry = 0;
	            pReq -> Timeout = ATA_CMD_TO;
	            pReq -> ToHndlr = NULL;
	        }

	        Err = ATA_NO_ERR;

	        if ((ATA_SubmitReq (pDevice, pReq) == True) )
	        {
				Err = ATAPI_ProcessPacketCmd (pDevice);
				if(Err != ATA_NO_ERR)
				{
	            	pDevice -> pReq = ATA_RelReq (pDevice, pReq);
				}
	        }
	    }

	}
    return Err;
}

/**
 *  \brief ATAPI_ProcessPacketCmd () : Process a PACKET command submition.
 *                                     Can be invoked in Interrupt Context.
 *
 *  \param pDevice [IN]             : Device Pointer
 *  \return int                     : Success/Failure
 ******************************************************************************/
Int32 ATAPI_ProcessPacketCmd (ATA_Drive_t const *pDevice)
{
    ATA_Dev_Req_t *pReq;
    ATA_Dev_Stat_t Status;
    /* Do a loop for say 5 times to see for status else
    * look for it in the timeout context.
    */
    Uint8          Count = 5; 
    Int32 Err = ATA_ERR_OK;
	
    //osTimerObject *tmrObj;
	TIMER_OBJ *tmrObj;

	if(NULL != pDevice)
	{

		pReq = pDevice -> pReq;
		tmrObj = (TIMER_OBJ *) pDevice -> pIdeInt -> IdeTimeout;

	    if (FALSE == (pDevice -> Cap . IsDrqInt)) {
	        /* Loop for status here */
	        do {
	            ((pDevice -> pIdeInt) -> pIdeOps) -> CmdStatus (((pDevice -> pIdeInt) -> pIntHndl),
	                                                        &Status, TRUE, pReq->PortNum);
	        } while ((--Count > 0) &&
	                 (FALSE == (Status . Stat & ATA_DEV_REQ)) && (FALSE == (Status . Stat & ATA_DEV_ERR)));

	        if (Count != 0) {
	        	(tmrObj -> id == 1) ? timer0_start(timer0.timeout) : timer1_start(timer1.timeout);
				
	            if (FALSE != (Status . Stat & ATA_DEV_REQ)) {
	                pDevice -> pReq -> Handler (pDevice, &Status);
	            } 
	            else {
	                pReq -> Res . ReqInfo . Status . Error = ATA_ERR_GEN;
	                Err = ATA_ERR_GEN;
	            }
	        }
	    }
	}
	else
	{
		Err = ATA_ERR_GEN;
	}
    return Err;
}
/**
 *  \brief ATAPI_PktHndlr () : ATAPI Packet request handler.
 *                           Executes in Interrupt context.
 *  \param Handle [IN]     : Handle to the Device.
 *  \param pStatus [IN]    : Command status
 *  \return Bool           : FALSE (Also Called from timeout context so FALSE)
 ******************************************************************************/
Bool ATAPI_PktHndlr (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
    ATA_Drive_t const *pDevice = NULL;
    ATA_Dev_Req_t *pReq = NULL;
    Uint32         Timeout = 0;
    ATA_Req_Mode_t RawMode;
    Bool ExTo;

	TIMER_OBJ *tmrObj;
	if((NULL != Handle) && (NULL != pStatus))
	{
		pDevice = (ATA_Drive_t const *)Handle;
		pReq = pDevice -> pReq;
		tmrObj = (TIMER_OBJ *) pDevice -> pIdeInt -> IdeTimeout;
		Timeout = (pDevice -> PwrMode == ATA_DEV_ACTIVE)? ATA_CMD_TO : ATA_RESET_TO;
	    ExTo = ((pReq -> Mode) & ATA_CMD_EXTTO) ? TRUE : FALSE;
	    RawMode = (ATA_Req_Mode_t)((pReq -> Mode) & (~ATA_CMD_EXTTO));

	    if (FALSE != (pStatus -> Stat & ATA_DEV_ERR))
	    {
	        pReq -> Res . ReqInfo . Status . Error = ATA_ERR_DF;
	        pReq -> IsPktCmd = FALSE;
	        ATA_SchedNextReq (pDevice -> pIdeInt, pReq);
	    }
	    else
	    {
	        /* Check whether this command has to be executed with extended Timeout */
	        if (FALSE != ExTo)
	        {
	          Timeout = ATA_RESET_TO;
	        }

        	(tmrObj -> id == 1) ? timer0_stop() : timer1_stop();
        	(tmrObj -> id == 1) ? timer0_start(timer0.timeout) : timer1_start(timer1.timeout);


	        pReq -> Timeout = Timeout;
	        pReq -> ToHndlr = NULL;
	        switch (RawMode)
	        {
	        case READ_PIO:
	            pReq -> Handler = &ATA_ReadPio;
	            break;

	        case WRITE_PIO:
	            pReq -> Handler = &ATA_WritePio;
	            break;

	        case READ_DMA:
	            pReq -> Handler = &ATA_ReadDma;
	            break;

	        case WRITE_DMA:
	            pReq -> Handler = &ATA_WriteDma;
	            break;

	        case TASKFILE:
	            pReq -> Handler = &ATA_Taskfile;
	            break;

	        case CONTROL:
	        default :
	            break;
	        }

	        pDevice -> pIdeInt -> pIdeOps -> Write (pDevice -> pIdeInt -> pIntHndl,
	        									pReq -> Res . ReqInfo . Status . Buffer, 
	        									pDevice -> Cap . AtapiCmdSize);

	        if ((RawMode == READ_DMA) || (RawMode == WRITE_DMA))
	        {
	          pDevice -> pIdeInt -> pIdeOps -> StartReq (pDevice -> pIdeInt -> pIntHndl,pReq);
	        }
	    }

	}

    return FALSE;
}


/**
 *  \brief ATAPI_PktTimeout () : ATAPI Packet Timeout handler.
 *                             Executes in Interrupt context.
 *  \param Handle [IN]       : Handle to the Device.
 *  \param pStatus [IN]      : Command status
 *  \return Bool             : To Timeout or not
 ******************************************************************************/
static Bool ATAPI_PktTimeout (void const * Handle, ATA_Dev_Stat_t const *pStatus)
{
    Bool Timeout = TRUE;
    ATA_Drive_t const *pDevice = NULL;
    ATA_Dev_Req_t *pReq = NULL;
	TIMER_OBJ *tmrObj;

	if((NULL != Handle) && (NULL != pStatus))
	{
		pDevice = (ATA_Drive_t const *)Handle;
		pReq = pDevice -> pReq;
		tmrObj = (TIMER_OBJ *) pDevice -> pIdeInt -> IdeTimeout;

	    if (FALSE != (pStatus -> Stat & ATA_DEV_REQ))
	    {
	      Timeout = pDevice -> pReq -> Handler (pDevice, pStatus);
	    } else
	    {
	        /* Look for DRQ set until 10 ms */

	        if (((pDevice -> pReq -> Retry)--) != 0)
	        {
	            Timeout = FALSE;

	        	(tmrObj -> id == 1) ? timer0_start(pReq->Timeout) : timer1_start(pReq->Timeout);

	        }
	    }

	}
	else
	{
		Timeout = FALSE;
	}

    return Timeout;
}

