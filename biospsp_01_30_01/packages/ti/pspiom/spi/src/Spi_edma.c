/*
 * Spi_edma.c
 *
 * This file contains local functions for the SPI driver which contain implemen-
 * tation for EDMA operation specifc calls like starting an EDMA transfer for 
 * for SPI peripheral, EDMA completion and/or error callbacks etc
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
 *  \file   Spi_edma.c
 *
 *  \brief  This files contains the edma functions required for the Spi driver
 *          to function in the EDMA mode.

 *   (C) Copyright 2008, Texas Instruments, Inc
 *
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include <std.h>
#include <assert.h>
#include <sys.h>
#include <iom.h>
#include <bcache.h>
#include <sem.h>

#ifdef Spi_EDMA_ENABLE

#include <ti/sdo/edma3/drv/edma3_drv.h>
#include <ti/pspiom/spi/Spi.h>
#include <ti/pspiom/gpio/Gpio.h>
#include "SpiLocal.h"

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */

static Void SpiUpdateError(Spi_ChanObj *chanHandle,Uint32 intStatus);

/* ========================================================================== */
/*                       GLOBAL MODULE STATE                                  */
/* ========================================================================== */

/* ========================================================================== */
/*                       FUNCTION DEFINITIONS                                 */
/* ========================================================================== */

/**
 *  \brief  This function initiates Edma mode of operation.SPI Edma Rx & Tx
 *          channels are configured and spi Edma interrupt in enable here
 *          to get Rx and Tx events.
 *
 *  \param  handle      [IN]   Handle of the SPI Driver Object
 *  \param  chipSelect  [IN]   chip seclect for the spi
 *  \param  dataformat  [IN]   data format to be used for the transaction
 *  \param  flags       [IN]   flags to be used for the transfer operation
 *
 *  \return          IOM_COMPLETED if successful<br>
 *                   Suitable error code if failure
 *
 */
Int Spi_localEdmaTransfer(Spi_Object      *instHandle,
                          Spi_DataParam   *dataParam)
{
    Uint32                spidat1      = 0;
    Spi_ChanObj          *chanHandle   = NULL;
    Uint32                acntFlag     = 1u;
    volatile Uint16      *spiDat1Cfg   = NULL;
    EDMA3_DRV_PaRAMRegs   paramSet     = {0,0,0,0,0,0,0,0,0,0,0,0};
    Int32                 status       = IOM_COMPLETED;

    assert(NULL != instHandle);
    assert(NULL != dataParam);
    chanHandle = instHandle->currentActiveChannel;
    assert(NULL != chanHandle);

    /* Activate the GPIO chip select here. However, if CSHOLD is not set, then*
     * raise an error. The reason is, for word-by-word transfer, we cannot    *
     * control the CS, as the master who is doing the data transfer is EDMA   *
     * Controller. The SPI driver can only activate the CS at start of        *
     * transfer (EDMA start) and at the end of the transfer (EDMA end - in the*
     * callback)                                                              */
    if ((Spi_CSHOLD != (dataParam->flags & Spi_CSHOLD))
       && (Spi_GPIO_CS == (dataParam->flags & Spi_GPIO_CS)))
    {
        status = IOM_EBADARGS;
    }
    
    if (IOM_COMPLETED == status)
    {
        /* Extract data control param values to upload to SPIDAT1                 *
         * before read/write transfer starts                                      */
        spidat1 = Spi_localControlDataConfig(
                      chanHandle,
                      dataParam->chipSelect,
                      dataParam->dataFormat,
                      dataParam->gpioPinNum,
                      dataParam->flags);
        
        spiDat1Cfg = (((volatile Uint16 *)
                           (&(instHandle->deviceInfo.baseAddress)->SPIDAT1))+1);
        assert(NULL != spiDat1Cfg);

        /* write the upper 16 bits which contain the configurations to the SPIDAT1*
         * upper 16bits                                                           */
        *spiDat1Cfg = (Uint16)(spidat1 >> 16);
    }

    /* clear if any pending events in the EDMA event register for this channel*/
    if (IOM_COMPLETED == status)
    {
        status = EDMA3_DRV_clearErrorBits(
                      instHandle->hEdma,
                      instHandle->deviceInfo.txDmaEventNumber);
    }

    if (IOM_COMPLETED == status)
    {
        /* read and remove the stale data                                     */
        while (CSL_SPI_SPIBUF_RXEMPTY_MASK
            != ((instHandle->deviceInfo.baseAddress)->SPIBUF
                & CSL_SPI_SPIBUF_RXEMPTY_MASK))
        {
            (*chanHandle->rxBuffer) = 
                (Uint8)(instHandle->deviceInfo.baseAddress)->SPIBUF;
        }

        /* check the number of bytes to be transferred per sync event         */
        if (TRUE == chanHandle->charLength16Bits)
        {
            acntFlag = 2u;
        }
        else
        {
            acntFlag = 1u;
        }

        if ((TRUE == chanHandle->rxBufFlag) && (TRUE == instHandle->enableCache))
        {
            BCACHE_wbInv((Ptr)chanHandle->rxBuffer,chanHandle->rxBufferLen,TRUE);        
        }

        /* Get the paramset for the receive channel and then update with the  *
         * transfer parameters                                                */
        /* Get the PaRAM set for default parameters                           */
        EDMA3_DRV_getPaRAM (
            instHandle->hEdma,
            instHandle->deviceInfo.rxDmaEventNumber,
            &paramSet);

        /* configure the source and destination address                       */
        paramSet.srcAddr = (Uint32)(&((instHandle->deviceInfo.baseAddress)->SPIBUF));
        paramSet.destAddr = (Uint32)(chanHandle->rxBuffer);

        /* configure the source and destination indexes                       */
        paramSet.srcBIdx  = 0;
        paramSet.srcCIdx  = 0;
        paramSet.destCIdx = 0;

        /* dont increment the address in case the buffer is not  user supplied*
         * buffer.                                                            */
        if (FALSE == chanHandle->rxBufFlag)
        {
            paramSet.destBIdx = 0;
        }
        else
        {
            paramSet.destBIdx = 1;
        }

        /* Configuring aCnt, bCnt, cCnt and bCntReload                        */
        if ((acntFlag > 0xFFFFu ) || (chanHandle->rxBufferLen > 0xFFFF ))
        {
            status = IOM_EBADIO;
        }
        else
        {
            paramSet.aCnt = (unsigned short)acntFlag;
            paramSet.bCnt = (unsigned short)chanHandle->rxBufferLen;
        }
       
        paramSet.cCnt       = (Uint16)1;
        paramSet.bCntReload = 0;

        /* configure the OPT field of the EDMA paramset                       */
        /* Linking is disabled                                                */
        paramSet.linkAddr   = 0xFFFFu;

        /* Src & Dest are in INCR modes                                       */
        paramSet.opt &= 0xFFFFFFFCu;

        /* FIFO width is 8 bit                                                */
        paramSet.opt &= 0xFFFFF8FFu;

        /* EDMA3_DRV_SYNC_A                                                   */
        paramSet.opt &= 0xFFFFFFFBu;

        /* EDMA3_DRV_OPT_FIELD_TCINTEN                                        */
        paramSet.opt |= (1 << Spi_OPT_TCINTEN_SHIFT);

        /* update the paramset with the new values                            */
        if (IOM_COMPLETED == status)
        {
            /* Now, write the PaRAM Set.                                      */
            status = EDMA3_DRV_setPaRAM(
                         instHandle->hEdma,
                         instHandle->deviceInfo.rxDmaEventNumber,
                         &paramSet);
        }

        /* enable the transfer                                                */
        if (IOM_COMPLETED != status)
        {
            SYS_printf("\nSPI Test: EDMA Param Setting failed" );
        }
        else
        {
            status = EDMA3_DRV_enableTransfer(
                        instHandle->hEdma,
                        instHandle->deviceInfo.rxDmaEventNumber,
                        EDMA3_DRV_TRIG_MODE_EVENT);
        }

        if (IOM_COMPLETED != status)
        {
            SYS_printf("\nSPI Test: EDMA Transfer Enable failed" );
        }


        /* now update the paramset for the TX transfer channel                */
        if ((TRUE == chanHandle->txBufFlag) && (TRUE == instHandle->enableCache))
        {
            /* Write intialization for DMA mode                               */
            BCACHE_wbInv((Ptr)chanHandle->txBuffer,chanHandle->txBufferLen,TRUE);
        }

        /* Get the PaRAM set for default parameters                           */
        EDMA3_DRV_getPaRAM (instHandle->hEdma,
             instHandle->deviceInfo.txDmaEventNumber,
             &paramSet);

        /* configure the source and destination address                       */
        paramSet.srcAddr = (Uint32)(chanHandle->txBuffer);
        paramSet.destAddr = (Uint32)(&((instHandle->deviceInfo.baseAddress)->SPIDAT1));

        /* configure the source and destination indexes                       */
        if (TRUE == chanHandle->txBufFlag)
        {
            paramSet.srcBIdx = 1;
        }
        else
        {
            paramSet.srcBIdx = 0;
        }
        paramSet.srcCIdx   = 0;
        paramSet.destBIdx  = 0;
        paramSet.destCIdx  = 0;

        /* Configuring aCnt, bCnt, cCnt and bCntReload                        */
        if ((acntFlag > 0xFFFFu ) || (chanHandle->txBufferLen> 0xFFFFu))
        {
            status = IOM_EBADIO;
        }
        else
        {
            paramSet.aCnt = (Uint16)acntFlag;
            paramSet.bCnt = (Uint16)chanHandle->txBufferLen;
        }

        paramSet.cCnt       = (Uint16)1;
        paramSet.bCntReload = 0;

        /* Linking is disable                                                 */
        paramSet.linkAddr   = 0xFFFFu;

        /* Src & Dest are in INCR modes                                       */
        paramSet.opt &= 0xFFFFFFFCu;

        /* FIFO width is 8 bit                                                */
        paramSet.opt &= 0xFFFFF8FFu;

        /* EDMA3_DRV_SYNC_A                                                   */
        paramSet.opt &= 0xFFFFFFFBu;

        /* EDMA3_DRV_OPT_FIELD_TCINTEN                                        */
        paramSet.opt |= (1 << Spi_OPT_TCINTEN_SHIFT);

        if (IOM_COMPLETED == status)
        {
            /* Now, write the PaRAM Set.                                      */
            status = EDMA3_DRV_setPaRAM(instHandle->hEdma,
                         instHandle->deviceInfo.txDmaEventNumber,
                         &paramSet);
        }

        /* Shall handle only overrun in the interrupt handler                 */
        (instHandle->deviceInfo.baseAddress)->SPIINT0 |=
                 CSL_SPI_SPIINT0_OVRNINTENA_MASK;

        if (IOM_COMPLETED != status)
        {
            SYS_printf("SPI Test: EDMA Param Setting failed\n");
        }
        else
        {
            Spi_localGpioPinToggle(chanHandle, Spi_LOW);
            
            /* Provide delay between CS active and actual IO 
               as desired by the used   */
            if(Spi_GPIO_CS == (dataParam->flags & Spi_GPIO_CS))
            {
                Spi_localgenericDelay(dataParam->csToTxDelay);                            
            }
        
            EDMA3_DRV_enableTransfer(
                (instHandle->hEdma),
                instHandle->deviceInfo.txDmaEventNumber,
                EDMA3_DRV_TRIG_MODE_EVENT);
        }

        if (IOM_COMPLETED != status)
        {
            SYS_printf("SPI Test: EDMA Transfer Enable failed\n");
        }

        /* Enable SPI for transaction                                         */
        (instHandle->deviceInfo.baseAddress)->SPIGCR1 |=
            (((Uint32)CSL_SPI_SPIGCR1_ENABLE_ENABLE)
                << ((Uint32)CSL_SPI_SPIGCR1_ENABLE_SHIFT));
        
        (instHandle->deviceInfo.baseAddress)->SPIINT0 |=
           (CSL_SPI_SPIINT0_DMAREQEN_ENABLE 
            << CSL_SPI_SPIINT0_DMAREQEN_SHIFT);

    }

    /* return the function return code                                        */
    return (status) ;
}

/**
 *  \brief  This function is called in response to the completion of transmit.
 *          This is the Callback for the EDMA transmit completion event.
 *          Tx channel transfer for Edma is disable done here.
 *
 *  \param  tcc         [IN]   Transmit interrupt event channel
 *  \param  edmaStatus  [IN]   Transfer status whether transfer completed or not
 *  \param  appData     [IN]   Data passed Edma during initiation
 *
 */
Void Spi_localCallbackTransmit(Uns                tcc,
                               EDMA3_RM_TccStatus edmaStatus,
                               Ptr                appData)
{
    Spi_Object            *instHandle    = NULL;
    Spi_ChanObj           *chanHandle    = NULL;
    Uns                    ch_Tx         = tcc;
    Uint32                 intStatus     = 0x00;

    assert(NULL != appData);
    instHandle = (Spi_Object *)appData;
    assert(NULL != instHandle);
    chanHandle = instHandle->currentActiveChannel;

    if(NULL != chanHandle)
    {

        /* Disable edma transfer                                              */
        EDMA3_DRV_disableLogicalChannel(
            instHandle->hEdma,
            ch_Tx,
            EDMA3_DRV_TRIG_MODE_EVENT);
        
        EDMA3_DRV_setOptField(
            instHandle->hEdma,
            ch_Tx,
            EDMA3_DRV_OPT_FIELD_TCC,
            ch_Tx);
        
        if (Spi_RX_EDMA_CALLBACK_OCCURED == instHandle->edmaCbCheck)
        {
            intStatus = (instHandle->deviceInfo.baseAddress)->SPIFLG;
            SpiUpdateError(chanHandle,intStatus);
        
            (instHandle->deviceInfo.baseAddress)->SPIINT0 &=
                (CSL_SPI_SPIINT0_DMAREQEN_DISABLE
                << CSL_SPI_SPIINT0_DMAREQEN_SHIFT);
        
            /* Disable the spi enable pin                                     */
            (instHandle->deviceInfo.baseAddress)->SPIGCR1 &=
                (~(CSL_SPI_SPIGCR1_ENABLE_MASK));
                    
            instHandle->edmaCbCheck = 0x0;
        
            if(NULL != chanHandle->activeIOP)
            {
                if(EDMA3_RM_XFER_COMPLETE != edmaStatus)
                {
                    /* Ensure to clear the error bits of EDMA channel         */
                    EDMA3_DRV_clearErrorBits(instHandle->hEdma, ch_Tx);
                    chanHandle->activeIOP->status = (Int32)edmaStatus;
                    instHandle->stats.txBytes += chanHandle->activeIOP->size;
                }
                /* The CS sould be deactivated here - this is where EDMA      *
                * notifies of the transfer completion                         */
                Spi_localGpioPinToggle(chanHandle, Spi_HIGH);                
                spi_localCompleteIOedmaCallback(instHandle);
            }
        }
        else
        {
            if((EDMA3_RM_XFER_COMPLETE == edmaStatus)&&
               (NULL != chanHandle) &&
               (NULL != chanHandle->activeIOP))
            {
                instHandle->edmaCbCheck = Spi_TX_EDMA_CALLBACK_OCCURED;
            }
        }
    }
}

/**
 *  \brief  This function is called in response to the completion of receive.
 *          This is the Callback for the EDMA receive completion event.
 *          Rx channel transfer for Edma is disable done here.
 *
 *  \param  tcc         [IN]    receive interrupt event channel
 *  \param  edmaStatus  [IN]    receive status whether transfer completed or not
 *  \param  appData     [IN]    Data passed Edma during initiation
 *
 */
 Void Spi_localCallbackReceive(Uns                tcc,
                               EDMA3_RM_TccStatus edmaStatus,
                               Ptr                appData)
{
    Spi_Object            *instHandle  = NULL;
    Spi_ChanObj           *chanHandle  = NULL;
    Uns                    chan_Rx     = tcc;
    Uint32                 intStatus   = 0x00;

    assert(NULL != appData);
    instHandle =  (Spi_Object *)appData;
    assert(NULL != instHandle);
    chanHandle = instHandle->currentActiveChannel;
    
    if(NULL != chanHandle)
    {

        EDMA3_DRV_disableLogicalChannel(
            instHandle->hEdma,
            chan_Rx,
            EDMA3_DRV_TRIG_MODE_EVENT);
        
        EDMA3_DRV_setOptField (
            (instHandle->hEdma),
            chan_Rx,
            EDMA3_DRV_OPT_FIELD_TCC,
            chan_Rx);
        
        if (EDMA3_RM_XFER_COMPLETE != edmaStatus)
        {
            /* Ensure to clear the error bits of EDMA channel                 */
            EDMA3_DRV_clearErrorBits(instHandle->hEdma, chan_Rx);
        }
        
        if (Spi_TX_EDMA_CALLBACK_OCCURED == instHandle->edmaCbCheck)
        {
            intStatus = (instHandle->deviceInfo.baseAddress)->SPIFLG;

            SpiUpdateError(chanHandle,intStatus);

            (instHandle->deviceInfo.baseAddress)->SPIINT0 &=
                (CSL_SPI_SPIINT0_DMAREQEN_DISABLE
                 << CSL_SPI_SPIINT0_DMAREQEN_SHIFT);
        
            /* Disable the spi enable pin                                     */
            (instHandle->deviceInfo.baseAddress)->SPIGCR1 &=
                (~(CSL_SPI_SPIGCR1_ENABLE_MASK)); 
                    
            instHandle->edmaCbCheck = 0x0;
        
            if(NULL != chanHandle->activeIOP)
            {
                if(EDMA3_RM_XFER_COMPLETE != edmaStatus)
                {
                    chanHandle->activeIOP->status = (Int32)edmaStatus;
                    instHandle->stats.rxBytes += chanHandle->activeIOP->size;
                }
                /* The CS sould be deactivated here - this is where EDMA      *
                * notifies of the transfer completion                         */
                Spi_localGpioPinToggle(chanHandle, Spi_HIGH);            
                spi_localCompleteIOedmaCallback(instHandle);
            }
        }
        else
        {
            if((EDMA3_RM_XFER_COMPLETE == edmaStatus)&&
               (NULL != chanHandle) &&
               (NULL != chanHandle->activeIOP))
            {        
                instHandle->edmaCbCheck = Spi_RX_EDMA_CALLBACK_OCCURED;
            }
        }
    }
}

/**
 *  \brief Function used after edma callback
 *
 *  This function will be called after the completion of edma callback. This
 *  function requests the next channel and also calls the app callback function.
 *
 *  \param   instHandle  [IN]   Pointer to the spi driver instance object
 *
 *  \return  None
 *
 *  \enter  instHandle must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
void spi_localCompleteIOedmaCallback (Spi_Object *instHandle)
{
    volatile Uint8     *spiDat1    = NULL;
    Spi_ChanObj        *chanHandle = NULL;
    Spi_DataParam      *dataParam  = NULL;
    IOM_Packet         *ioPacket   = NULL;
    EDMA3_DRV_PaRAMRegs paramSet   = {0};
    Uint32              edmaNum    = 0x00;
#ifdef BIOS_PWRM_ENABLE
    PWRM_Status         status     = PWRM_SOK;
    Uint32              count      = 0x00;
#endif
    Int32               retVal     = IOM_COMPLETED;

    assert(NULL != instHandle);

    chanHandle = instHandle->currentActiveChannel;
    assert(NULL != chanHandle);
    ioPacket = chanHandle->activeIOP;
    dataParam  = (Spi_DataParam *)chanHandle->activeIOP->addr;
    assert(NULL != dataParam);

    /* check if CSHOLD needs to be enable after the transmission also.        *
     * If No or if there is any eror disable CSHOLD value, else do nothing    */
    if ((dataParam->flags & Spi_CSHOLD_FOR_MULTI_TRANSCEIVE) !=
        Spi_CSHOLD_FOR_MULTI_TRANSCEIVE)
    {
        /* Pointing to the MSByte of SPIDAT1 to reset CSHOLD bit              */
        spiDat1 =
            ((volatile Uint8*)&((instHandle->deviceInfo.baseAddress)->SPIDAT1))+3u;
            
        assert(NULL != spiDat1);

        *spiDat1 &= ~(Spi_SPIDAT1_CSHOLD_8BIT_MASK);
    }

    /* Mask off interrupts                                                    */
    (instHandle->deviceInfo.baseAddress)->SPIINT0 &=
         (~(Spi_INTERRUPT_MASK));

    /* Update size parameters of the IOP                                      */
    if (TRUE == chanHandle->txBufFlag)
    {
        edmaNum = instHandle->deviceInfo.txDmaEventNumber;
    }
    else
    {
        edmaNum = instHandle->deviceInfo.rxDmaEventNumber;
    }
    
    /* check how many bytes were transferred before the EDMA was stopped      */
    EDMA3_DRV_getPaRAM (instHandle->hEdma, edmaNum, &paramSet);
    
    ioPacket->size -= (paramSet.aCnt * paramSet.bCnt);
    
    /* Update the return status of the IOP                                    */
    if (IOM_COMPLETED == ioPacket->status)
    {
        ioPacket->status = chanHandle->currError;
    }        

    /* power OFF the module                                                   */
    retVal = Spi_localLpscOff(instHandle);

    if (IOM_COMPLETED == ioPacket->status)
    {
        ioPacket->status = retVal;
    }
    
    /* call the application completion callback function registered           *
     * with us during opening of the channel                                  */
    if ((NULL != chanHandle->cbFxn) && (NULL != chanHandle->cbArg))
    {
        /* Invoke Application callback for this channel                       */
        (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg, ioPacket);
    }

    /* There is no transaction for now.It will be set again when there is     *
     * actual transaction is to be started                                    */
    chanHandle->currError = 0;
    chanHandle->pendingState = FALSE;
    chanHandle->activeIOP = NULL;
    chanHandle->charLength16Bits = FALSE;

    /* Check if the abort flag is set for this channel. If set, we need to    *
     * remove all (empty) the pending packets from the list and send it back  *
     * to the upper layer. We do it here because, the packets should be dealt *
     * with in FIFO order                                                     */
    if (TRUE == chanHandle->abortAllIo)
    {
        while(FALSE == QUE_empty(&chanHandle->queuePendingList))
        {
            /* we have atleast one packet                                     */
            ioPacket = (IOM_Packet *)
                            QUE_get(&chanHandle->queuePendingList);
            if (NULL != ioPacket)
            {
                ioPacket->status = IOM_ABORTED;

                /* power OFF the module                                       */
                Spi_localLpscOff(instHandle);

                if ((NULL != chanHandle->cbFxn) && (NULL != chanHandle->cbArg))
                {
                    /* Invoke Application callback for this channel           */
                    (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg,ioPacket);
                }
              
                instHandle->stats.pendingPacket--;
            }
        }
        chanHandle->abortAllIo = FALSE;
    }

#ifdef BIOS_PWRM_ENABLE
    if (FALSE == instHandle->pwrmInfo.ioSuspend)
    {
#endif
        Spi_loadPendedIops(instHandle);
#ifdef BIOS_PWRM_ENABLE
    }
    else
    {
        if (TRUE == instHandle->pscPwrmEnable)
        {
            if ((PWRM_GOINGTOSLEEP == instHandle->pwrmInfo.pwrmEvent) ||
                (PWRM_GOINGTODEEPSLEEP == instHandle->pwrmInfo.pwrmEvent))
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
            instHandle->devState = Spi_DriverState_PWRM_SUSPEND;

            /* call the delayed completion function                           */
            (instHandle->pwrmInfo.delayedCompletionFxn[instHandle->pwrmInfo.pwrmEvent])();
        
            /* No more packets to load.Disable the interrupts                 */
            (instHandle->deviceInfo.baseAddress)->SPIINT0 &=
                     (~CSL_SPI_SPIINT0_OVRNINTENA_MASK);
        }
    }
#endif
}

/**
 *  \brief    Requests required edma channels from the EDMA controller
 *            it requests both the TX and RX edma channels required.
 *
 * \param     instHandle   [IN]    pointer to spi driver object
 *
 * \return    IOM_COMPLETED if success
 *            Error ID in case of failure
 *
 */
Int32 Spi_localEdmaChannel_Request(Spi_Object *instHandle)
{
    Int32 status = IOM_COMPLETED;

    assert(NULL != instHandle);

    /* channel request for edma Rx and Tx                                     */
    if (Spi_OpMode_DMAINTERRUPT == instHandle->opMode)
    {
        /* request for RX DMA channels                                        */
        instHandle->dmaChanAllocated = TRUE;

        /* request the required channels                                      */
        status = EDMA3_DRV_requestChannel(
                    (instHandle->hEdma),
                    &(instHandle->deviceInfo.rxDmaEventNumber),
                    &(instHandle->deviceInfo.rxDmaEventNumber),
                    (EDMA3_RM_EventQueue)0,
                    &Spi_localCallbackReceive,
                    instHandle);

        if (IOM_COMPLETED != status)
        {
            instHandle->dmaChanAllocated = FALSE;
            SYS_printf("\nSPI Test: EDMA Request failed");
        }
        else
        {
            /* Ensure to clear the error bits of EDMA channel                 */
            EDMA3_DRV_clearErrorBits(instHandle->hEdma,
                instHandle->deviceInfo.txDmaEventNumber);


            /* request for TX DMA channels                                    */
            status = EDMA3_DRV_requestChannel(
                        (instHandle->hEdma),
                        &(instHandle->deviceInfo.txDmaEventNumber),
                        &(instHandle->deviceInfo.txDmaEventNumber),
                        (EDMA3_RM_EventQueue)0,
                        &Spi_localCallbackTransmit,
                        instHandle);
        }

        if (IOM_COMPLETED !=  status)
        {
            /* free the allocated channels                                    */
            EDMA3_DRV_freeChannel(
                instHandle->hEdma,
                instHandle->deviceInfo.rxDmaEventNumber);

            instHandle->dmaChanAllocated = FALSE;

            SYS_printf("\nSPI Test: EDMA Request failed");
        }
        /* Ensure to clear the error bits of EDMA channel                     */
        EDMA3_DRV_clearErrorBits(instHandle->hEdma,
            instHandle->deviceInfo.txDmaEventNumber);
    }/* DMA mode channel request */
    return status;
}

/**
 * \brief    Function to set the appropriate error status for the channel
 *
 * \param    chanHandle   [IN]    pointer to channel object
 *
 * \return   None
 */
static Void SpiUpdateError(Spi_ChanObj *chanHandle,Uint32 intStatus)
{
    Spi_Object *instHandle = NULL;

    assert(NULL != chanHandle);

    instHandle = chanHandle->instHandle;
    assert(NULL != instHandle);

    /* check Timeout interrupt                                                */
    if (CSL_SPI_SPIFLG_TIMEOUTFLG_MASK
        == (intStatus & CSL_SPI_SPIFLG_TIMEOUTFLG_MASK))
    {
        chanHandle->currError = Spi_TIMEOUT_ERR;
        (instHandle->deviceInfo.baseAddress)->SPIFLG |=
            CSL_SPI_SPIFLG_TIMEOUTFLG_MASK;
        instHandle->stats.timeoutError++;
    }

    if (CSL_SPI_SPIFLG_PARERRFLG_MASK
        == (intStatus &  CSL_SPI_SPIFLG_PARERRFLG_MASK))
    {
        /* Parity error interrupt                                             */
        chanHandle->currError = Spi_PARITY_ERR ;
        (instHandle->deviceInfo.baseAddress)->SPIFLG |=
            CSL_SPI_SPIFLG_PARERRFLG_MASK;
        instHandle->stats.parityError++;
    }

    if (CSL_SPI_SPIFLG_DESYNCFLG_MASK
        == (intStatus &  CSL_SPI_SPIFLG_DESYNCFLG_MASK ))
    {
        /* De-Synchronization interrupt, holds only in master mode            */
        chanHandle->currError = Spi_DESYNC_ERR ;
        (instHandle->deviceInfo.baseAddress)->SPIFLG |=
            CSL_SPI_SPIFLG_DESYNCFLG_MASK ;
        instHandle->stats.desyncError++;
    }

    if (CSL_SPI_SPIFLG_BITERRFLG_MASK
        == (intStatus &  CSL_SPI_SPIFLG_BITERRFLG_MASK))
    {
        /* Bit error Interrupt                                                */
        chanHandle->currError = Spi_BIT_ERR ;
        (instHandle->deviceInfo.baseAddress)->SPIFLG |=
            CSL_SPI_SPIFLG_BITERRFLG_MASK;
        instHandle->stats.bitError++;
    }

    /* Receive Over run interrupt                                             */
    if ((intStatus & CSL_SPI_SPIFLG_OVRNINTFLG_MASK)
        == CSL_SPI_SPIFLG_OVRNINTFLG_MASK)
    {
        chanHandle->currError = Spi_RECEIVE_OVERRUN_ERR;
        (instHandle->deviceInfo.baseAddress)->SPIFLG |=
            CSL_SPI_SPIFLG_OVRNINTFLG_MASK;
        instHandle->stats.rxOverrunError++;
    }
}
#endif
/* ========================================================================== */
/*                            END OF FILE                                     */
/* ========================================================================== */
