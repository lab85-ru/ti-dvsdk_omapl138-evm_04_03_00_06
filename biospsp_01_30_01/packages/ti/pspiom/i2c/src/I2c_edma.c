/*
 * I2c_edma.c
 *
 * This file contains local functions for the I2C driver which contain implemen-
 * tation for EDMA operation specifc calls like starting an EDMA transfer for 
 * for I2C peripheral, EDMA completion and/or error callbacks etc
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
 *  \file   I2c_edma.c
 *
 *  \brief  This files contains the edma functions required for the I2c driver
 *          to function in the EDMA mode.
 *
 *
 *   (C) Copyright 2008, Texas Instruments, Inc
 *
 *
 *   \version 0.1     Newly created for OMAPL138 pre silicon platform.
 *
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include <std.h>
#include <assert.h>

#include <que.h>
#include <iom.h>
#include <bcache.h>

#include <ti/pspiom/i2c/I2c.h>
#include "I2cLocal.h"

#include <ti/pspiom/cslr/cslr_uart.h>
#include <ti/pspiom/cslr/cslr_edma3cc.h>

#ifdef CHIP_C6747
    #include <ti/pspiom/cslr/soc_C6747.h>
#endif

#ifdef CHIP_OMAPL137
    #include <ti/pspiom/cslr/soc_OMAPL137.h>
#endif

#ifdef CHIP_C6748
    #include <ti/pspiom/cslr/soc_C6748.h>
#endif

#ifdef CHIP_OMAPL138
    #include <ti/pspiom/cslr/soc_OMAPL138.h>
#endif

#ifdef I2c_EDMA_ENABLE

#include <ti/sdo/edma3/drv/edma3_drv.h>


/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Void I2c_rxCallback(Uint32 tcc, EDMA3_RM_TccStatus status, Ptr appData);
static Void I2c_txCallback(Uint32 tcc, EDMA3_RM_TccStatus status, Ptr appData);

/* ========================================================================== */
/*                          FUNCTION DEFINTIONS                               */
/* ========================================================================== */

/*
 *  \brief   This function initiates Edma mode of operation
 *
 *  \param   instHandle  [IN]   Pointer to the I2c driver instance
 *  \param   flags       [IN]   Flags passed to transfer function
 *
 *  \return If successful, returns IOM_COMPLETED else Error code
 *
 */
Int I2c_localEdmaTransfer(I2c_Object *instHandle,Uint32 flags)
{
    Int32               retCode    = IOM_COMPLETED;
    volatile Uint32    *srcPtr     = NULL;
    volatile Uint32    *dstPtr     = NULL;
    Uint32              chan       = 0;
    Uint32              enableEdma = CSL_I2C_ICDMAC_RXDMAEN_MASK;
    Uint32              srcIndex   = 0;
    Uint32              dstIndex   = 1u;
    I2c_ChanObj        *chanHandle = NULL;
    EDMA3_DRV_PaRAMRegs paramSet   = {0,0,0,0,0,0,0,0,0,0,0,0};

    assert(NULL != instHandle);

    chanHandle = instHandle->currentActiveChannel;
    
    assert(NULL != chanHandle);
    
    srcPtr = &((instHandle->deviceInfo.baseAddress)->ICDRR);
    dstPtr = (Uint32 *)chanHandle->currBuffer;
    chan   = instHandle->deviceInfo.rxDmaEventNumber;

    if ((flags & I2c_WRITE) == I2c_WRITE)
    {
        srcPtr      = (Uint32*)chanHandle->currBuffer;
        dstPtr      = &((instHandle->deviceInfo.baseAddress)->ICDXR);
        chan        = instHandle->deviceInfo.txDmaEventNumber;
        enableEdma  = CSL_I2C_ICDMAC_TXDMAEN_MASK;
        srcIndex    = 1u;
        dstIndex    = 0;
    }

    if (TRUE == instHandle->enableCache)
    {
        /* Read intialization for DMA mode                                    */
        BCACHE_wbInv (
            (Ptr)chanHandle->currBuffer,
            chanHandle->currBufferLen,
            TRUE);
    }

    /* clear if any pending events in the EDMA event register for this channel*/
    retCode = EDMA3_DRV_clearErrorBits(instHandle->hEdma,chan);


    /* Get the PaRAM set for default parameters                               */
    EDMA3_DRV_getPaRAM (instHandle->hEdma,chan, &paramSet);

    assert((NULL != srcPtr) && (NULL != srcPtr));

    /* Updating the source and destination address                            */
    paramSet.srcAddr   = (unsigned int)(srcPtr);
    paramSet.destAddr  = (unsigned int)(dstPtr);

    /* Updating the source and destination B & C Indexes                      */
    paramSet.srcBIdx   = srcIndex;
    paramSet.srcCIdx   = 0;
    paramSet.destBIdx  = dstIndex;
    paramSet.destCIdx  = 0;

    /* Updating the a,b,c count values                                        */
    paramSet.aCnt      = 1u;

    if (chanHandle->currBufferLen <= 0xFFFF)
    {
        paramSet.bCnt  = chanHandle->currBufferLen;
    }
    else
    {
        retCode = IOM_EBADIO;
    }

    paramSet.cCnt       = 1u;
    paramSet.bCntReload = 0;

    paramSet.linkAddr   = 0xFFFFu;

    /* Src & Dest are in INCR modes                                           */
    paramSet.opt &= 0xFFFFFFFCu;

    /* FIFO width is 8 bit                                                    */
    paramSet.opt &= 0xFFFFF8FFu;

    /* EDMA3_DRV_SYNC_A                                                       */
    paramSet.opt &= 0xFFFFFFFBu;

    /* EDMA3_DRV_OPT_FIELD_TCINTEN                                            */
    paramSet.opt |= (1 << I2c_OPT_TCINTEN_SHIFT);

    /* update the transfer completion code                                    */
    paramSet.opt &= (~CSL_EDMA3CC_OPT_TCC_MASK);
    paramSet.opt |= (chan << CSL_EDMA3CC_OPT_TCC_SHIFT);

    if (IOM_COMPLETED == retCode)
    {
        /* Now, write the PaRAM Set.                                          */
        retCode = EDMA3_DRV_setPaRAM(instHandle->hEdma,chan,&paramSet);
    }

    if (IOM_COMPLETED != retCode)
    {
        retCode = IOM_EBADARGS;
    }
    else
    {
        retCode = EDMA3_DRV_enableTransfer(
                      instHandle->hEdma,
                      chan,
                      EDMA3_DRV_TRIG_MODE_EVENT);
    }

    if (IOM_COMPLETED != retCode)
    {
        retCode = IOM_EBADARGS;
    }

    /* Enable EDMA                                                        */
    (instHandle->deviceInfo.baseAddress)->ICDMAC = enableEdma;

    return (retCode);
}

/*
 *  \brief DMA READ complete callback
 *
 *  This function will be called from the DMA driver, on completion of
 *  programmed receive operation. This function will be registered at the time
 *  of starting the DMA.
 *
 *  \param  tcc       [IN]  Receive interrupt event channel
 *  \param  status    [IN]  Error code returned by the Edma
 *  \param  appData   [IN]  Data passed Edma during initiation
 *
 *  \return None
 *  \enter  tcc       must be a valid interrupt event channel.
 *          status    must be a valid error code status.
 *          appData   must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Void I2c_rxCallback(Uint32 tcc, EDMA3_RM_TccStatus status, Ptr appData)
{
    I2c_Object     *instHandle   = NULL;
    Uint32          chRx         = tcc;
    I2c_ChanObj    *chanHandle   = NULL;

    assert(NULL != appData);
    instHandle =  (I2c_Object *)appData;

    /* get the Handle to the current active channel                           */
    chanHandle = instHandle->currentActiveChannel;
    
    if(NULL != chanHandle)
    {
        if (I2c_CommMode_MASTER == chanHandle->masterOrSlave)
        {
            /* Disable the EDMA transfer channel                              */
            EDMA3_DRV_disableLogicalChannel (
                instHandle->hEdma,
                chRx,
                EDMA3_DRV_TRIG_MODE_EVENT);    
        
            assert(NULL != chanHandle->activeIOP);
        
            if (EDMA3_RM_XFER_COMPLETE == status)
            {
                chanHandle->currError = IOM_COMPLETED;
                instHandle->stats.rxBytes += chanHandle->activeIOP->size;
            }
            else
            {
                /* Ensure to clear the error bits of EDMA channel             */
                EDMA3_DRV_clearErrorBits(instHandle->hEdma, chRx);        
                chanHandle->activeIOP->status = IOM_EBADIO;
            }
        
            I2c_completeIOedmaCallback(instHandle);
        }
    }
}

/*
 *  \brief DMA write complete callback
 *
 *  This function will be called from the DMA driver, on completion of
 *  programmed transmit operation. This function will be registered at the time
 *  of starting the DMA.
 *
 *  \param  tcc       [IN]  transmit interrupt event channel
 *  \param  status    [IN]  Error code returned by the Edma
 *  \param  appData   [IN]  Data passed Edma during initiation
 *
 *  \return None
 *
 *  \enter  tcc       must be a valid interrupt event channel.
 *          status    must be a valid error code status.
 *          appData   must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Void I2c_txCallback(Uint32 tcc, EDMA3_RM_TccStatus status, Ptr appData)
{
    I2c_Object       *instHandle  = NULL;
    Uint32            chTx        = tcc;
    I2c_ChanObj      *chanHandle  = NULL;

    assert(appData != NULL);
    instHandle =  (I2c_Object *) appData;

    /* get the Handle to the current active channel                           */
    chanHandle = instHandle->currentActiveChannel;

    if(NULL != chanHandle)
    {    
        if (I2c_CommMode_MASTER == chanHandle->masterOrSlave)
        {
            EDMA3_DRV_disableLogicalChannel(
                instHandle->hEdma,
                chTx,
                EDMA3_DRV_TRIG_MODE_EVENT);
    
            assert(NULL != chanHandle->activeIOP);
    
            if (EDMA3_RM_XFER_COMPLETE == status)
            {
                chanHandle->currError = IOM_COMPLETED;
                instHandle->stats.txBytes += chanHandle->activeIOP->size;
            }
            else
            {
                /* Ensure to clear the error bits of EDMA channel             */
                EDMA3_DRV_clearErrorBits(instHandle->hEdma, chTx);            
                chanHandle->activeIOP->status = IOM_EBADIO;
            }
            I2c_completeIOedmaCallback(instHandle);
        }
    }
}

/*
 *  \brief Function used after edma callback
 *
 *  This function will be called after the completion of edma callback. This
 *  function requests the next channel and also calls the app callback function.
 *
 *  \param   instHandle  [IN]   Pointer to the i2c driver instance object
 *
 *  \return  None
 *
 *  \enter  instHandle must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
void I2c_completeIOedmaCallback (I2c_Object *instHandle)
{
    I2c_ChanObj         *chanHandle = NULL;
    IOM_Packet          *ioPacket   = NULL;
    EDMA3_DRV_PaRAMRegs  paramSet   = {0};
#ifdef BIOS_PWRM_ENABLE
    Uint32               count      = 0x00;
#endif
    Int32                retVal     = IOM_COMPLETED;
    Int32                status     = IOM_COMPLETED;

    assert(NULL != instHandle);

    chanHandle = instHandle->currentActiveChannel;
    assert(NULL != chanHandle);
    
    /* disable DMA interrupts                                                 */
    (instHandle->deviceInfo.baseAddress)->ICDMAC = 0;

    /* Update the size of the IOP                                             */
    /* check how many bytes were transferred before the EDMA was stopped      */
    if (IOM_INPUT == chanHandle->mode)
    {
        EDMA3_DRV_getPaRAM (instHandle->hEdma,
            instHandle->deviceInfo.rxDmaEventNumber, 
            &paramSet);
    }
    else
    {
        EDMA3_DRV_getPaRAM (instHandle->hEdma,
            instHandle->deviceInfo.txDmaEventNumber, 
            &paramSet);        
    }
    
    chanHandle->currBufferLen -= (paramSet.aCnt * paramSet.bCnt);

    /* Check the Pending State                                                */
    if (TRUE == chanHandle->abortAllIo)
    {
        /* Error of Cancel IO                                                 */
        chanHandle->activeIOP->status = IOM_ABORTED;
        chanHandle->abortAllIo = FALSE;
        chanHandle->activeIOP->size = chanHandle->currBufferLen;

        /* call the module power off function                                 */
        I2c_localLpscOff(instHandle);

        if ((NULL != chanHandle->cbFxn) && (NULL != chanHandle->cbArg))
        {
            /* Invoke Application callback for this channel                   */
            (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg, chanHandle->activeIOP);
        }

        /* we will ensure that all the IOPs will be aborted                   */
        while (TRUE != QUE_empty(&(chanHandle->queuePendingList)))
        {
            ioPacket = (IOM_Packet *)QUE_get(&chanHandle->queuePendingList);
            
            assert(NULL != ioPacket);
            
            instHandle->stats.pendingPacket--;
            ioPacket->size = 0;
            ioPacket->status = IOM_ABORTED;

            /* call the module power off function                             */
            I2c_localLpscOff(instHandle);

            if ((NULL != chanHandle->cbFxn) && (NULL != chanHandle->cbArg))
            {
                /* Invoke Application callback for this channel               */
               (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg,ioPacket);
            }
        }
    }
    else
    {
        /* update the size of the processed buffer                            */
        chanHandle->activeIOP->size = chanHandle->currBufferLen;

        status = I2c_localEnsureIsReadyforNext(instHandle);
        
        if(0 != chanHandle->currError)
        {
            chanHandle->activeIOP->status = chanHandle->currError;
        }
        else
        {
            chanHandle->activeIOP->status = status;
        }

        /* call the module power off function                                 */
        retVal = I2c_localLpscOff(instHandle);

        /* if the status of the IOP is OK then we will try to update the      *
         * status of the module OFF so that we can inform application when    *
         * LPSC off fails                                                     */
        if (IOM_COMPLETED == chanHandle->activeIOP->status)
        {
            chanHandle->activeIOP->status = retVal;
        }

        /* call the application completion callback function registered       *
         * with us during opening of the channel                              */
        if (NULL != chanHandle->cbFxn)
        {
            /* Invoke Application callback for this channel                   */
            (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg, chanHandle->activeIOP);
        }
    }
    
    /* There is no transaction for now.It will be set again when there is     *
     * actual a transaction is to be started                                  */
    chanHandle->pendingState = FALSE;    
    chanHandle->activeIOP = NULL;

#ifdef BIOS_PWRM_ENABLE
    if (FALSE == instHandle->pwrmInfo.ioSuspend)
    {
#endif
        I2c_loadPendedIops(instHandle);
#ifdef BIOS_PWRM_ENABLE
    }
    else
    {
        if (TRUE == instHandle->pscPwrmEnable)
        {
            if (((PWRM_GOINGTOSLEEP == instHandle->pwrmInfo.pwrmEvent) ||
                  (PWRM_GOINGTODEEPSLEEP == instHandle->pwrmInfo.pwrmEvent)) &&
                       (0 != instHandle->instNum))
            {
                /* reduce the dependency count                                */
                status = PWRM_getDependencyCount(
                            (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId,
                             &count);

                instHandle->pwrmInfo.dependencyCount = count;
                
                if (PWRM_SOK == status)
                {
                    while (count > 0)
                    {
                        status =  PWRM_releaseDependency(
                            (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);

                        if (PWRM_SOK != status)
                        {
                            break;
                        }
                        count--;
                    }
                }
            }
        
            /* set the current active channel as NULL                         */
            instHandle->currentActiveChannel = NULL;

            instHandle->pwrmInfo.ioSuspend = FALSE;
            instHandle->devStatus = I2c_DriverState_PWRM_SUSPEND;

            /* call the delayed completion function                           */
            (instHandle->pwrmInfo.delayedCompletionFxn  \
                [instHandle->pwrmInfo.pwrmEvent])();
        }
    }
#endif    
}

/**
 * \brief   Function to request edma channels
 *
 *          This function will be called from the IOM driver open function.
 *          This function requests the required edma channels and in case 
 *          of failure raises an error
 *
 * \param   instHandle  [IN]   Pointer to the I2c driver instance object
 *
 * \return  None
 *
 * \enter   instHandle must be a valid pointer and should not be null.
 *
 * \leave   Not Implemented.
 */
Void I2c_localEdmaChanRequest(I2c_Object *instHandle)
{
    Uint32 status = IOM_COMPLETED;

    assert(NULL != instHandle);

    /* set the status of DMA channels as allocated                            */
    instHandle->dmaChaAllocated = TRUE;

    /* request for RX DMA channels                                            */
    status = EDMA3_DRV_requestChannel(
                 (instHandle->hEdma),
                 &(instHandle->deviceInfo.rxDmaEventNumber),
                 &(instHandle->deviceInfo.rxDmaEventNumber),
                 (EDMA3_RM_EventQueue)instHandle->edma3EventQueue,
                 &I2c_rxCallback,
                 instHandle);


    if (IOM_COMPLETED != status)
    {
        instHandle->dmaChaAllocated = FALSE;
    }
    else
    {
        /* request for TX DMA channels                                        */
        status = EDMA3_DRV_requestChannel(
                     (instHandle->hEdma),
                     &(instHandle->deviceInfo.txDmaEventNumber),
                     &(instHandle->deviceInfo.txDmaEventNumber),
                     (EDMA3_RM_EventQueue)instHandle->edma3EventQueue,
                     &I2c_txCallback,
                     instHandle);
    }

    if (IOM_COMPLETED != status)
    {
        /* Failed to allocated the channel hence free the already allocated   *
         * channel                                                            */
        EDMA3_DRV_freeChannel(
            instHandle->hEdma,
            instHandle->deviceInfo.rxDmaEventNumber);

        instHandle->dmaChaAllocated = FALSE;
    }
}

#endif /* I2c_EDMA_ENABLE */
/* ========================================================================== */
/*                            END OF FILE                                     */
/* ========================================================================== */
