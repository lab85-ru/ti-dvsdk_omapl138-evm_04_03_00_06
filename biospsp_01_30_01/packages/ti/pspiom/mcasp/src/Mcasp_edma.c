/*
 * Mcasp_edma.c
 *
 * This file contains local functions for McASP driver which contain implemen-
 * tation for EDMA operation specifc calls like starting an EDMA transfer for
 * for McASP peripheral, EDMA completion and/or error callbacks etc
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
 *  \file   Mcasp_edma.c
 *
 *  \brief  This files contains the edma functions required for the Mcasp driver
 *          to function in the EDMA mode.
 *
 *   (C) Copyright 2008, Texas Instruments, Inc
 *
 *
 * \version   0.2     Added the support for the Non loop job mode
 *            0.1     Newly created.
 */

/* ========================================================================== */
/*                             INCLUDE FILES                                  */
/* ========================================================================== */

#include <std.h>
#include <assert.h>
#include <que.h>
#include <iom.h>

#ifdef CHIP_DM6437
    #include "ti/pspiom/cslr/soc_DM6437.h"
#endif

#ifdef CHIP_C6747
    #include "ti/pspiom/cslr/soc_C6747.h"
#endif

#ifdef CHIP_OMAPL137
    #include "ti/pspiom/cslr/soc_OMAPL137.h"
#endif

#ifdef CHIP_C6748
    #include "ti/pspiom/cslr/soc_C6748.h"
#endif

#ifdef CHIP_OMAPL138
    #include "ti/pspiom/cslr/soc_OMAPL138.h"
#endif

#include <ti/sdo/edma3/drv/src/edma3.h>
#include "ti/pspiom/mcasp/Mcasp.h"
#include "McaspLocal.h"



extern far Mcasp_TempBuffer Mcasp_muteBuf[CSL_MCASP_PER_CNT];

static Int32 Mcasp_localIsValidPacket(Mcasp_ChannelHandle chanHandle);

/* ========================================================================== */
/*                         LOCAL FUNCTION PROTOTYPES                          */
/* ========================================================================== */

/* ========================================================================== */
/*                          FUNCTION DEFINITIONS                              */
/* ========================================================================== */

/**
 *  \brief  This function is called during channel creation (during open)
 *          This function does the following
 *          1) Request the Xfer (in/out depends on mode) channel to EDMA
 *             driver for this peripheral;
 *          2) Requests the link channels for linking (exclusive for each
 *             direction (tx/rx)
 *
 *  \param  chanHandle  [IN]  pointer to the channel object
 *  \param  eb          [OUT] pointer to error block
 *
 *  \return  status     IOM_COMPLETED if is sucess
 *                      error id in case of failure
 *
 *  \entry   chanHandle  should be non NULL and valid pointer
 *
 *  \leave   Not implemented
 */
Int32 Mcasp_localSetupEdmaDuringOpen(Mcasp_ChannelHandle chanHandle)
{
    Uint32              reqTcc      = EDMA3_DRV_TCC_ANY;
    EDMA3_RM_EventQueue queueNum    = 0;
    Bool                falsewhile  = TRUE;
    Uint32              linkCnt     = 0;
    Uint32              edmaChanNum = 0;
    Uint32              count       = 0;
    Int32               status      = IOM_COMPLETED;

    do
    {
        assert(NULL != chanHandle);

        falsewhile  = FALSE;

        /* For receive use EDMA Queue 1 and for transmit use EDMA queue 0     */
        if (IOM_INPUT == chanHandle->mode)
        {
            queueNum = (EDMA3_RM_EventQueue)Mcasp_RXEVENTQUE;
        }
        else
        {
            queueNum = (EDMA3_RM_EventQueue)Mcasp_TXEVENTQUE;
        }

        status = EDMA3_DRV_requestChannel(
                     chanHandle->edmaHandle,
                     &chanHandle->xferChan,
                     &chanHandle->tcc,
                     queueNum,
                     (chanHandle->edmaCallback),
                     chanHandle);

        if (IOM_COMPLETED != status)
        {
            status = IOM_EBADARGS;
            break;
        }

        /* Acquire the  PaRAM entries used for EDMA transfers linking         */
        for (count = 0; count < Mcasp_MAXLINKCNT; count++)
        {
            /* For requesting for a PaRam set                                 */
            edmaChanNum = EDMA3_DRV_LINK_CHANNEL;
            reqTcc = EDMA3_DRV_TCC_ANY;

            status = EDMA3_DRV_requestChannel(
                        chanHandle->edmaHandle,
                        &edmaChanNum,
                        &reqTcc,
                        queueNum,
                        (chanHandle->edmaCallback),
                        NULL);

            if (IOM_COMPLETED == status)
            {
                chanHandle->pramTbl[count] = edmaChanNum;

                status = EDMA3_DRV_getPaRAMPhyAddr(
                            chanHandle->edmaHandle,
                            chanHandle->pramTbl[count],
                            &(chanHandle->pramTblAddr[count]));
            }

            if (IOM_COMPLETED != status )
            {
                /* free the Synchonized EDMA channel                          */
                EDMA3_DRV_freeChannel(
                    chanHandle->edmaHandle,
                    chanHandle->xferChan);

                /* free the already allocated PaRAM entries                   */
                for (linkCnt = 0; linkCnt <count; linkCnt++)
                {
                    EDMA3_DRV_freeChannel(
                        chanHandle->edmaHandle,
                        chanHandle->pramTbl[linkCnt]);
                }

                status = IOM_EBADARGS;
                break;
            }
        }

        /* Configure EDMA channels                                            */
        status = Mcasp_localEdmaChanPaRAMsetup(chanHandle);
    }while(falsewhile);

    return status;
}
#ifdef Mcasp_LOOPJOB_ENABLED
/**
 *  \brief   This calls the registered application callback for the current
 *           receive request and processes the next request in queue.
 *           This if a mcasp edma callback function called in edma context
 *
 *  \param   tcc     [IN]    tcc number of EDMA
 *  \param   status  [IN]    status of EDMA transaction
 *  \param   data    [IN]    paramaters sent to EDMA ISR (chan handler)
 *
 *  \return  Nothing
 *
 *  \entry   tcc         should be a valid value
 *           status      should be a valid value
 *           data        should be a non NULL and valid pointer
 *
 *  \leave   Not implemented
 */
Void Mcasp_localEdmaCallback(Uint32 tcc, EDMA3_RM_TccStatus status, Ptr data)
{
    Mcasp_Object           *instHandle = NULL;
    Mcasp_ChannelHandle     chanHandle = NULL;
    EDMA3_DRV_PaRAMRegs     pramTbl    = {0};
    IOM_Packet             *ioPacket   = NULL;
    Bool                    falsewhile = TRUE;
    Uint32                  dlbMode    = 0;
    Int32                   retVal     = IOM_COMPLETED;
    Mcasp_PktAddrPayload   *payLoad    = NULL;

    do
    {
        assert(NULL != data);

        falsewhile = FALSE;
        
        /* To remove the compiler warning                                     */
        tcc = tcc;

        chanHandle = (Mcasp_ChannelHandle)data;
        instHandle = (Mcasp_Object *)chanHandle->devHandle;

        assert(NULL != instHandle);

        if (TRUE == QUE_empty(&(chanHandle->queueFloatingList)))
        {
            /* This cannot happen, if it happens then it is a sprurios one    */
            break;
        }

        /* For DMA transactions all the floating packets are managed in       *
         * queueFloatingList  Now pop out the packet from the queue and       *
         * complete it                                                        */
        chanHandle->tempPacket = (IOM_Packet *)QUE_get(
                                      &(chanHandle->queueFloatingList));

        /*
                             -------
                            | xfer  |
                            | chan  |
                             -------
                                                        Floating queue
                                                        --------------
                     -------     -------     -------        |
                    |       |   |       |   |       |<------
                    |   B   |   |   A   |   |   A   |
                     -------     -------     -------
                                            |       |
                                            |   B   |
                                             -------
         * As shown in the figure above, There are two IO request present in  *
         * floating queue. Whenever EDMA completetion callback is called,     *
         * xfer channel will be transferring data of one of the request. As   *
         * this is a ping-pong implementation, link params are linked to each *
         * other. Meaning once data transfer of IO A is complete,data transfer*
         * of IO B will be initiated. Ideally, when control comes to this     *
         * function xfer channel should be servicing the 2nd request of the   *
         * floating queue. Here,in above illustration when callback is called,*
         * xfer channel should be servicing request B while request A is      *
         * completed.                                                         *
         *                                                                    *
         * When due to any reason, CPU is halted(it can be due to break-point,*
         * memory access view using CCS,accessing variables using watch window*
         * etc.) EDMA will continue trasferring data. So assuming when CPU is *
         * halted EDMA is servicing request A, data transfer of request A will*
         * continue but ISR of transfer complete will notserve asCPU is halted*
         * As request A is complete and param A is linked with param B which  *
         * is holding request B, EDMA will start servicing request B. Again   *
         * EDMA will complete the request B service but transfer complete     *
         * ISR will not come as CPU is halted. But now as this is a ping-pong *
         * implementation, EDMA will start servicing request A. This will     *
         * ping-pong will continue until CPU is taken out of halt. After that *
         * first transfer complete ISR is due to which request is not known to*
         * driver. It can be either request A or B.The problem will arise when*
         * xfer channel is servicing A when callback is called. In that case, *
         * request that is in the servicing, will be returned to the app      *
         * as serviced. This will lead to continuous noise.                   *
         *                                                                    *
         * To take care the above problem, when callback is called, check is  *
         *  done to verify if the xfer channel is servicing the request A or  *
         *  B. If EDMA is servicing request A, requests are queued in a proper*
         *  sequence and no callback is called. When a next callback is called*
         *  it will be of request A, so floating queue is dequeued and from   *
         *  there on normal operation will continue.                          */

        /* get the param table information of transfer channel                */
        EDMA3_DRV_getPaRAM(chanHandle->edmaHandle,chanHandle->xferChan,&pramTbl);

        if (chanHandle->mode == IOM_INPUT)
        {
            /* Check if destination address falls into the range of 1st req   *
             * in the floating queue.                                         */
            if (TRUE == instHandle->isDataBufferPayloadStructure)
            {
                payLoad = (Mcasp_PktAddrPayload *)chanHandle->tempPacket->addr;
                assert(NULL != payLoad);

                if ((pramTbl.destAddr >= (Uint32)payLoad->addr)
                    && (pramTbl.destAddr < (Uint32)payLoad->addr
                            + chanHandle->tempPacket->size))
                {
                    /* Since we have already dequeue the 1st request, dequeue *
                     * 2nd io request from floating queue                     */
                    ioPacket = (IOM_Packet *)QUE_get(
                                          &chanHandle->queueFloatingList);

                    assert(NULL != ioPacket);

                    /* Queue the tempPacket (i.e. 1st io request) as this pkt *
                     * should be first in a queue                             */
                    QUE_put(
                        &chanHandle->queueFloatingList,
                        (Ptr)chanHandle->tempPacket);

                    /* Queue the ioPacket i.e. 2nd request in floating queue  */
                    QUE_put(&chanHandle->queueFloatingList,(Ptr)ioPacket);
                    break;
                }
            }
            else
            {
                if ((pramTbl.destAddr >= (Uint32)chanHandle->tempPacket->addr)
                    && (pramTbl.destAddr < (Uint32)chanHandle->tempPacket->addr
                            + chanHandle->tempPacket->size))
                {
                    /* Since we have already dequeue the 1st request, dequeue *
                     * 2nd io request from floating queue                     */
                    ioPacket = (IOM_Packet *)QUE_get(
                                          &chanHandle->queueFloatingList);

                    /* Queue the tempPacket (i.e. 1st io request) as this pkt *
                     * should be first in a queue                             */
                    QUE_put(
                        &chanHandle->queueFloatingList,
                        (Ptr)chanHandle->tempPacket);

                    /* Queue the ioPacket i.e. 2nd request in floating queue  */
                    QUE_put(&chanHandle->queueFloatingList,(Ptr)ioPacket);
                    break;
                }
            }
        }
        else
        {
            /* Check if destination address falls into the range of1st request*
             * in the floating queue.                                         */
            if (TRUE == instHandle->isDataBufferPayloadStructure)
            {
                payLoad = (Mcasp_PktAddrPayload *)chanHandle->tempPacket->addr;

                if ((pramTbl.srcAddr >= (Uint32)payLoad->addr)
                    && (pramTbl.srcAddr < (Uint32)payLoad->addr
                            + chanHandle->tempPacket->size))
                {
                    /* Since we have already dequeue the 1st request, dequeue *
                     * 2nd io request from floating queue                     */
                    ioPacket = (IOM_Packet *)QUE_get(
                                    &chanHandle->queueFloatingList);

                    /* Queue the tempPacket (i.e. 1st io request) as this     *
                     * packet should be first in a queue                      */
                    QUE_put(
                        &chanHandle->queueFloatingList,
                        (Ptr)chanHandle->tempPacket);

                    /* Queue the ioPacket i.e. 2nd request in floating queue  */
                    QUE_put(&chanHandle->queueFloatingList,(Ptr)ioPacket);
                    break;
                }
            }
            else
            {
                if ((pramTbl.srcAddr >= (Uint32)chanHandle->tempPacket->addr)
                    && (pramTbl.srcAddr < (Uint32)chanHandle->tempPacket->addr
                            + chanHandle->tempPacket->size))
                {
                    /* Since we have already dequeue the 1st request, dequeue *
                     * io request from floating queue                         */
                    ioPacket = (IOM_Packet *)QUE_get(
                                   &chanHandle->queueFloatingList);

                    /* Queue the tempPacket (i.e. 1st io request) as this     *
                     * packet should be first in a queue                      */
                    QUE_put(
                        &chanHandle->queueFloatingList,
                        (Ptr)chanHandle->tempPacket);

                    /* Queue the ioPacket i.e. 2nd request in floating queue  */
                    QUE_put(&chanHandle->queueFloatingList,(Ptr)ioPacket);
                    break;
                }

            }
        }

        if (EDMA3_RM_XFER_COMPLETE != status)
        {
            /* Clear the EDMA Error bits                                      */
            chanHandle->tempPacket->status = IOM_EBADIO;

            /* There is a data transfer error in EDMA transfer. Return        *
             * actualSize equal to 0 represent that there is an error in the  *
             * packet transfer                                                */
            chanHandle->tempPacket->size = 0;

            EDMA3_DRV_clearErrorBits(
                chanHandle->edmaHandle,
                chanHandle->xferChan);
        }
        else
        {
            chanHandle->tempPacket->status = IOM_COMPLETED;
            chanHandle->tempPacket->size = chanHandle->tempPacket->size;

            if (IOM_COMPLETED != chanHandle->currentPacketErrorStatus)
            {
                chanHandle->tempPacket->status =
                    (Int)chanHandle->currentPacketErrorStatus;

                chanHandle->currentPacketErrorStatus = IOM_COMPLETED;
                chanHandle->tempPacket->size = 0;
            }
        }

        chanHandle->submitCount--;

        /* If stopSmFs flag for the channel is false and pause is not issued  *
         * control will go in the if part. In case when stopSmFS is issued or *
         * pause is issued, control will go in else part                      */

        if ((((IOM_INPUT == chanHandle->mode)
            && (FALSE == instHandle->stopSmFsRcv))
            || ((IOM_OUTPUT == chanHandle->mode)
                && (FALSE == instHandle->stopSmFsXmt)))
            && (FALSE == chanHandle->paused))
        {
            /* Now that we have done with the last data packet - we check if  *
             * next packet is available for transmission. Even if we are not  *
             * having a valid packet - previous programming of linkchannels   *
             * (designed in such a way) to send out null buffer Also note that*
             * Mcasp_isValidPacket function will set the                      *
             * chanHandle->dataPacket with next (current now) data packet from*
             * queue                                                          */
            if (Mcasp_STATUS_VALID == Mcasp_localIsValidPacket(chanHandle))
            {
                /* This is a valid data request. Call function to process it. *
                 * If DMA cannot be programmed properly with request contents,*
                 * abort all queued up requests and putstate-machine intoreset*/
                if (IOM_COMPLETED != (Mcasp_localEdmaProcessPkt(
                                           chanHandle,
                                           chanHandle->dataPacket)))
                {
                    Mcasp_localCancelAndAbortAllIo(chanHandle);
                }
            }
            else
            {
                /* the following function will assign loopjob buffer to free  *
                 * param set Also the control will come here two times for    *
                 * last packet and last but one packet (obviously for more    *
                 * than one packet xfer) For last but packet callback itself  *
                 * we will come to know that there is no more packet pending  *
                 * and hence we load loopjob info into next param set and for *
                 * the last packet callback will not do it again (for only one*
                 * packet scenario the following flag will take care not to   *
                 * update the loopjob second time)                            */
                if (FALSE  == chanHandle->loopjobUpdatedinParamset)
                {
                    chanHandle->loopjobUpdatedinParamset = TRUE;

                    Mcasp_localGetNextIndex(
                        &chanHandle->nextLinkParamSetToBeUpdated);

                    Mcasp_localUpdtDtPktToLnkPrms(chanHandle, NULL);
                }
            }
        }
        else
        {
            /* Control will come over here when either of receive or transmit *
             * state machine stop command is issued for receive or transmit   *
             * channel.If the nextFlag is already set indicates that its time *
             * to reset the state machines and disable the edma transfer      */
            if (TRUE == chanHandle->nextFlag)
            {
                chanHandle->nextFlag = FALSE;

                if (FALSE == chanHandle->paused)
                {
                    dlbMode = (((instHandle->hwInfo.regs)->DLBCTL
                                 & CSL_MCASP_DLBCTL_DLBEN_MASK)
                              >> CSL_MCASP_DLBCTL_DLBEN_SHIFT);

                    if( CSL_MCASP_DLBCTL_DLBEN_ENABLE == dlbMode)
                    {
                        /* Stop both RCV and XMT state m/cs                   */
                        /* before touching McASP registers, place receive     *
                         * section in reset                                   */
                        retVal = Mcasp_localResetRcv(&instHandle->hwInfo);

                        if (IOM_COMPLETED != retVal)
                        {
                            break;
                        }
                        /* before touching McASP registers, place transmit    *
                         * section in reset                                   */
                        retVal = Mcasp_localResetXmt(&instHandle->hwInfo);

                        if (IOM_COMPLETED != retVal)
                        {
                            break;
                        }
                    }
                    else
                    {
                        if (IOM_INPUT == chanHandle->mode)
                        {
                            /* stop Receive state m/c */
                            /* before touching McASP registers, place receive *
                             * section in reset                               */
                            retVal = Mcasp_localResetRcv(&instHandle->hwInfo);

                            if (IOM_COMPLETED != retVal)
                            {
                                break;
                            }
                        }
                        else
                        {
                            /* stop Transmit state m/c*/
                            /* before touching McASP registers, place transmit*
                             * section in reset                               */
                            retVal = Mcasp_localResetXmt(&instHandle->hwInfo);

                            if (IOM_COMPLETED != retVal)
                            {
                                break;
                            }
                        }
                    }
                    EDMA3_DRV_disableTransfer(
                        chanHandle->edmaHandle,
                        chanHandle->xferChan,
                        EDMA3_DRV_TRIG_MODE_EVENT);
                }
            }
            else
            {
                /* For the first time when the stop port command is issued we *
                 * will have one more packet linked with the transfer channel.*
                 * So we will not stop at this time. We link the packet with  *
                 * NULL loopjob buffer and set a nextFlag to TRUE. We will    *
                 * wait for another callback (indicating the io request is    *
                 * complete) to disable EDMA trasnfer and reset state machines*/
                chanHandle->nextFlag = TRUE;

                Mcasp_localGetNextIndex(
                    &chanHandle->nextLinkParamSetToBeUpdated);

                Mcasp_localUpdtDtPktToLnkPrms(chanHandle, NULL);
            }
        }

        chanHandle->isTempPacketValid = TRUE;

        Mcasp_localCompleteCurrentIo(chanHandle);

    }while (falsewhile);

    return;
}
#else
/**
 *  \brief   This calls the registered application callback for the current
 *           receive request and processes the next request in queue.
 *           This if a mcasp edma callback function called in edma context
 *
 *  \param   tcc     [IN]    tcc number of EDMA
 *  \param   status  [IN]    status of EDMA transaction
 *  \param   data    [IN]    paramaters sent to EDMA ISR (chan handler)
 *
 *  \return  Nothing
 *
 *  \entry   tcc         should be a valid value
 *           status      should be a valid value
 *           data        should be a non NULL and valid pointer
 *
 *  \leave   Not implemented
 */
Void Mcasp_localEdmaCallback(Uint32 tcc, EDMA3_RM_TccStatus status, Ptr data)
{
    Mcasp_Object           *instHandle = NULL;
    Mcasp_ChannelHandle     chanHandle = NULL;
    Bool                    falseWhile = TRUE;
    Uint32                  dlbMode    = 0;
    Int32                   retVal     = IOM_COMPLETED;
    Uint32                  hwiKey     = 0x00;

    do
    {
        falseWhile = FALSE;

        assert(NULL != data);
        chanHandle = (Mcasp_ChannelObj *)data;

        if (1u == chanHandle->submitCount)
        {
            /* This is the last packet available with the driver.Hence we will*
             * stop the EDMA and then proceed to process the packet           */
            EDMA3_DRV_disableTransfer(
                chanHandle->edmaHandle,
                chanHandle->xferChan,
                EDMA3_DRV_TRIG_MODE_EVENT);

            instHandle = (Mcasp_Object *)chanHandle->devHandle;
            assert(NULL != instHandle);

            /* close the IOP now                                              */
            if (IOM_OUTPUT == chanHandle->mode)
            {
                if (CSL_MCASP_XINTCTL_XUNDRN_MASK ==
                    (CSL_MCASP_XINTCTL_XUNDRN_MASK & chanHandle->userIntValue))
                {
                    instHandle->hwInfo.regs->XINTCTL &=
                        (~CSL_MCASP_XINTCTL_XUNDRN_MASK);
                }

                /* update the status of the IOP                               */
                if (EDMA3_RM_XFER_COMPLETE != status)
                {
                    /* update the status of the IOP as error                  */
                    chanHandle->currentPacketErrorStatus = IOM_EABORT;
                }
                else
                {
                    chanHandle->currentPacketErrorStatus = IOM_COMPLETED;
                }

                SWI_post(&instHandle->fifoSwiObject);
                break;
            }
            else
            {
                if (CSL_MCASP_RINTCTL_ROVRN_MASK ==
                    (CSL_MCASP_RINTCTL_ROVRN_MASK & chanHandle->userIntValue))
                {
                    instHandle->hwInfo.regs->RINTCTL &=
                        (~CSL_MCASP_RINTCTL_ROVRN_MASK);
                }
            }

            dlbMode = (((instHandle->hwInfo.regs)->DLBCTL
                         & CSL_MCASP_DLBCTL_DLBEN_MASK)
                          >> CSL_MCASP_DLBCTL_DLBEN_SHIFT);

            if( CSL_MCASP_DLBCTL_DLBEN_ENABLE == dlbMode)
            {
                Mcasp_localDeactivateSmFsForRcv(instHandle);
                Mcasp_localDeactivateSmFsForXmt(instHandle);
            }
            else
            {
                /* stop the clocks and the EDMA transfers                     */
                /* stop the xmt/rcv state machine.                            */
                if (Mcasp_ChanMode_RCV == chanHandle->channelOpMode)
                {
                    /* Reset state m/c & frame sync of receive section        */
                    Mcasp_localDeactivateSmFsForRcv(instHandle);

                    /* reenable the RCV overrun error                         */
                    if (CSL_MCASP_RINTCTL_ROVRN_MASK ==
                        (CSL_MCASP_RINTCTL_ROVRN_MASK & chanHandle->userIntValue))
                    {
                         instHandle->hwInfo.regs->RINTCTL |=
                             (CSL_MCASP_RINTCTL_ROVRN_MASK);
                    }
                }
            }

            /* clear the error bits that have been set                        */
            EDMA3_DRV_clearErrorBits(
                chanHandle->edmaHandle,
                chanHandle->xferChan);
        }

        /* To remove the compiler warning                                     */
        tcc = tcc;

        if (EDMA3_RM_XFER_COMPLETE != status)
        {
            /* clear the error bits that have been set                        */
            EDMA3_DRV_clearErrorBits(
                chanHandle->edmaHandle,
                chanHandle->xferChan);

            /* update the status of the IOP as error                          */
            chanHandle->currentPacketErrorStatus = IOM_EABORT;
            break;
        }
        else
        {
            chanHandle->currentPacketErrorStatus = IOM_COMPLETED;

            /* reduce the submit count as the packet callback is completed    */
            chanHandle->submitCount--;
        }


        /* complete the receive IOP now                                       */
        hwiKey = (Uint32)_disable_interrupts();

        if (TRUE == QUE_empty(&(chanHandle->queueFloatingList)))
        {
            /* This cannot happen, if it happens then it is a sprurios one    */
            _restore_interrupts(hwiKey);
            break;
        }

        /* Get the packet from the top of the queue (atomic operation)        */
        chanHandle->tempPacket = QUE_get(&(chanHandle->queueFloatingList));

        /* end the critical section                                           */
        _restore_interrupts(hwiKey);

        instHandle = (Mcasp_Object *)chanHandle->devHandle;
        assert(NULL != instHandle);

        /* If stopSmFs flag for the channel is false and pause is not issued  *
         * control will go in the if part. In case when stopSmFS is issued or *
         * pause is issued, control will go in else part                      */

        if ((((IOM_INPUT == chanHandle->mode)
            && (FALSE == instHandle->stopSmFsRcv))
            || ((IOM_OUTPUT == chanHandle->mode)
                && (FALSE == instHandle->stopSmFsXmt)))
            && (FALSE == chanHandle->paused))
        {
            /* Now that we have done with the last data packet - we check if  *
             * next packet is available for transmission. Even if we are not  *
             * having a valid packet - previous programming of linkchannels   *
             * (designed in such a way) to send out null buffer Also note that*
             * Mcasp_isValidPacket function will set the                      *
             * chanHandle->dataPacket with next (current now) data packet from*
             * queue                                                          */
            if (Mcasp_STATUS_VALID == Mcasp_localIsValidPacket(chanHandle))
            {
                /* This is a valid data request. Call function to process it. *
                 * If DMA cannot be programmed properly with request contents,*
                 * abort all queued up requests and putstate-machine intoreset*/
                if (IOM_COMPLETED != (Mcasp_localEdmaProcessPkt(
                                           chanHandle,
                                           chanHandle->dataPacket)))
                {
                    Mcasp_localCancelAndAbortAllIo(chanHandle);
                }
            }
        }
        else
        {
            /* Control will come over here when either of receive or transmit *
             * state machine stop command is issued for receive or transmit   *
             * channel.If the nextFlag is already set indicates that its time *
             * to reset the state machines and disable the edma transfer      */
            if (TRUE == chanHandle->nextFlag)
            {
                chanHandle->nextFlag = FALSE;

                if (FALSE == chanHandle->paused)
                {
                    dlbMode = (((instHandle->hwInfo.regs)->DLBCTL
                                 & CSL_MCASP_DLBCTL_DLBEN_MASK)
                              >> CSL_MCASP_DLBCTL_DLBEN_SHIFT);

                    if( CSL_MCASP_DLBCTL_DLBEN_ENABLE == dlbMode)
                    {
                        /* Stop both RCV and XMT state m/cs                   */
                        /* before touching McASP registers, place receive     *
                         * section in reset                                   */
                        retVal = Mcasp_localResetRcv(&instHandle->hwInfo);

                        if (IOM_COMPLETED != retVal)
                        {
                            break;
                        }
                        /* before touching McASP registers, place transmit    *
                         * section in reset                                   */
                        retVal = Mcasp_localResetXmt(&instHandle->hwInfo);

                        if (IOM_COMPLETED != retVal)
                        {
                            break;
                        }
                    }
                    else
                    {
                        if (IOM_INPUT == chanHandle->mode)
                        {
                            /* stop Receive state m/c */
                            /* before touching McASP registers, place receive *
                             * section in reset                               */
                            retVal = Mcasp_localResetRcv(&instHandle->hwInfo);

                            if (IOM_COMPLETED != retVal)
                            {
                                break;
                            }
                        }
                        else
                        {
                            /* stop Transmit state m/c*/
                            /* before touching McASP registers, place transmit*
                             * section in reset                               */
                            retVal = Mcasp_localResetXmt(&instHandle->hwInfo);

                            if (IOM_COMPLETED != retVal)
                            {
                                break;
                            }
                        }
                    }
                    EDMA3_DRV_disableTransfer(
                        chanHandle->edmaHandle,
                        chanHandle->xferChan,
                        EDMA3_DRV_TRIG_MODE_EVENT);
                }
            }
        }

        chanHandle->isTempPacketValid = TRUE;
        Mcasp_localCompleteCurrentIo(chanHandle);

#if defined(BIOS_PWRM_ENABLE) && !defined(Mcasp_LOOPJOB_ENABLED)
        /* check if the driver is waiting to go to sleep or process DVFS event*
         * (only if the application has requested PWRM support)               */
        if (TRUE == instHandle->pscPwrmEnable)
        {
            Mcasp_localHandlePwrmEvent(instHandle,chanHandle);
        }
#endif /* #ifdef BIOS_PWRM_ENABLE  */
    }while (falseWhile);
    return;
}

#ifdef BIOS_PWRM_ENABLE
/**
 * \brief   This function handles the power events related to the McASP.it
 *          reduces the dependency count and stops the further IO from being
 *          programmed.
 *
 * \param   instHandle [IN] Handle to the device instance.
 * \param   chanHandle [IN] Handle to the channel
 *
 * \return  None
 */
Void Mcasp_localHandlePwrmEvent(Mcasp_Object     *instHandle,
                                Mcasp_ChannelObj *chanHandle)
{
    Int32   retVal  = IOM_COMPLETED;
    Uint32  count   = 0x00;
    Uint32  dlbMode = 0x00;

    assert(NULL != instHandle);
    assert(NULL != chanHandle);

    /* check if the driver is waiting to go to sleep                          */
    if ((TRUE == instHandle->pscPwrmEnable) &&
        (TRUE == instHandle->pwrmInfo.ioSuspend))
    {
        /* Set the driver state to PWRM suspend so that no more IO can be     *
         * programmed                                                         */
        instHandle->devState = Mcasp_DriverState_PWRM_SUSPEND;

        if (TRUE == QUE_empty(&chanHandle->queueFloatingList))
        {
            /* stop the current channel IO processing                         */
            /* stop the EDMA and then proceed to process the packet           */
            EDMA3_DRV_disableTransfer(
                chanHandle->edmaHandle,
                chanHandle->xferChan,
                EDMA3_DRV_TRIG_MODE_EVENT);
        
            /* close the IOP now                                              */
            if (IOM_OUTPUT == chanHandle->mode)
            {
                if (CSL_MCASP_XINTCTL_XUNDRN_MASK ==
                    (CSL_MCASP_XINTCTL_XUNDRN_MASK & chanHandle->userIntValue))
                {
                    instHandle->hwInfo.regs->XINTCTL &=
                        (~CSL_MCASP_XINTCTL_XUNDRN_MASK);
                }
            }
            else
            {
                if (CSL_MCASP_RINTCTL_ROVRN_MASK ==
                    (CSL_MCASP_RINTCTL_ROVRN_MASK & chanHandle->userIntValue))
                {
                    instHandle->hwInfo.regs->RINTCTL &=
                        (~CSL_MCASP_RINTCTL_ROVRN_MASK);
                }
            }

            dlbMode = (((instHandle->hwInfo.regs)->DLBCTL
                         & CSL_MCASP_DLBCTL_DLBEN_MASK)
                          >> CSL_MCASP_DLBCTL_DLBEN_SHIFT);

            if( CSL_MCASP_DLBCTL_DLBEN_ENABLE == dlbMode)
            {
                Mcasp_localDeactivateSmFsForRcv(instHandle);
                Mcasp_localDeactivateSmFsForXmt(instHandle);
            }
            else
            {
                /* stop the clocks and the EDMA transfers                     */
                /* stop the xmt/rcv state machine.                            */
                if (Mcasp_ChanMode_RCV == chanHandle->channelOpMode)
                {
                    /* Reset state m/c & frame sync of receive section        */
                    Mcasp_localDeactivateSmFsForRcv(instHandle);

                    /* reenable the RCV overrun error                         */
                    if (CSL_MCASP_RINTCTL_ROVRN_MASK ==
                        (CSL_MCASP_RINTCTL_ROVRN_MASK & chanHandle->userIntValue))
                    {
                         instHandle->hwInfo.regs->RINTCTL |=
                             (CSL_MCASP_RINTCTL_ROVRN_MASK);
                    }
                }
            }

            /* clear the error bits that have been set                        */
            EDMA3_DRV_clearErrorBits(
                chanHandle->edmaHandle,
                chanHandle->xferChan);
        }

        if ((TRUE == QUE_empty(&instHandle->XmtObj.queueFloatingList)) &&
            (TRUE == QUE_empty(&instHandle->RcvObj.queueFloatingList)))
        {
            /* if both the channels are inactive then reset the io suspend    */
            instHandle->pwrmInfo.ioSuspend = FALSE;

            if ((PWRM_GOINGTOSLEEP == instHandle->pwrmInfo.pwrmEvent) ||
                 (PWRM_GOINGTODEEPSLEEP == instHandle->pwrmInfo.pwrmEvent))
            {
                 /* reduce the dependency count                               */
                 retVal = (Int32)PWRM_getDependencyCount(
                                   (PWRM_Resource)instHandle->hwInfo.pwrmLpscId,
                                   &count);
        
                 instHandle->pwrmInfo.dependencyCount = count;
        
                 if (PWRM_SOK == retVal)
                 {
                     while (count > 0)
                     {
                         retVal = (Int32)PWRM_releaseDependency(
                             (PWRM_Resource)instHandle->hwInfo.pwrmLpscId);
            
                         if (PWRM_SOK != retVal)
                         {
                             break;
                         }
                         count--;
                     }
                 }
             }

            /* call the delayed completion function                           */
            (instHandle->pwrmInfo.delayedCompletionFxn  \
                [instHandle->pwrmInfo.pwrmEvent])();
        }
    }        
}
#endif /* #ifdef BIOS_PWRM_ENABLE     */
#endif /* #ifdef Mcasp_LOOPJOB_ENABLE */

/**
 *  \brief  This function does the following
 *          1) set up the param set of main xfer channel (for loopjob transfers)
 *          2) set up the param sets of all link channel (for loopjob transfers)
 *          3) link the last link channel to itself (this channel will be linked
 *             to  main xfer channel later and to supply the loopjob xfer
 *            information infinely this is done so; other link channels are
 *            setup here just for completion sake
 *
 *  \param  chanHandle [IN]  Handle to the mcasp channel
 *
 *  \return IOM_COMPLETED if is success
 *          Error id if failed
 *
 *  \entry  chanHandle  should be non NULL and valid pointer
 *
 *  \leave  Not implemented
 */
Int32 Mcasp_localEdmaChanPaRAMsetup(Mcasp_ChannelHandle chanHandle)
{
    Mcasp_Object        *instHandle  = NULL;
    Uint32               linkCnt     = 0;
    Int32                status      = IOM_COMPLETED;
    Bool                 falsewhile  = TRUE;
    EDMA3_DRV_PaRAMRegs  paramSet    = {0,0,0,0,0,0,0,0,0,0,0,0};
    EDMA3_DRV_SyncType   tempSyncType;

    do
    {
        assert((NULL != chanHandle) && (NULL != chanHandle->devHandle));

        falsewhile = FALSE;

        instHandle = (Mcasp_Object *)chanHandle->devHandle;

        assert(NULL != instHandle);

        /* Configure the McASP synchonized EDMA channel PaRAM entries         *
         * Configure source and destination address / modes first - main xfer *
         * channel                                                            */

        /* following is the explanation behind the EDMA configuration         *
         *
         *  1) We will have n arrays of data (now it is loopjob buffer each of*
         *     length x bytes)                                                *
         *  2) for each sync event we have to fetch the data from each array  *
         *     and write to each serilaiser                                   *
         *  3) and the no. bytes transferred in this case from each array will*
         *     be roundedwordwidth/8                                          *
         *  4) as the loopjob buffer is continous, we configure the counts as *
         *     follows                                                        *
         *     a) consider there are arrays of length roundedwordwidth/8(aCnt)*
         *     b) these arrays are located apart lengthofloopjob size (bindex)*
         *     c) there are 'no.of.serialiser' no. of such arrays  (bCnt)     *
         *     d) (a) and (b) constitute one frame of EDMA transfer and it    *
         *        happens for one sync event in AB sync mode                  *
         *     e) such frames have to be sent till we have buffer - ie        *
         *        lengthofloopjob / (roundedwordwidth/8 )  and this           *
         *        constitutes  cCnt                                           *
         *     f) address of buffer (rather offset from start of previous     *
         *         frame) is roundedwordwidth/8 and this forms cIndex         *
         *  Please take some time to visualise the setup mentioned above to   *
         *  check with EDMA configuration The same kind of setup will be used *
         *  for data transfers also and the change will be buffer pointer     *
         *  and length of data buffer.                                        */

        /* Get the PaRAM set for default parameters                           */
        EDMA3_DRV_getPaRAM(chanHandle->edmaHandle,chanHandle->xferChan,
                           &paramSet);

        if (IOM_INPUT == chanHandle->mode )
        {
            /* Configuring Source and Dest addresses                          */
            paramSet.srcAddr  = (unsigned int)(instHandle->hwInfo.dataAddr);
#ifdef Mcasp_LOOPJOB_ENABLED
            paramSet.destAddr = (unsigned int)(chanHandle->loopJobBuffer);
#endif
            /* Configurign Src and Dest B&C Indexes                           */
            paramSet.srcBIdx   = 0;
            paramSet.srcCIdx   = 0;

            if (IOM_COMPLETED != Mcasp_localGetIndicesSyncType(
                                     chanHandle,
                                     &paramSet.destBIdx,
                                     &paramSet.destCIdx,
                                     &paramSet.aCnt,
                                     &paramSet.bCnt,
                                     &paramSet.cCnt,
                                     &tempSyncType,
                                     TRUE))
            {
                status = IOM_EBADARGS;
            }
        }
        else
        {
            /* Configuring Source and Dest addresses                          */
#ifdef Mcasp_LOOPJOB_ENABLED
            paramSet.srcAddr  = (unsigned int)(chanHandle->loopJobBuffer);
#endif
            paramSet.destAddr = (unsigned int)(instHandle->hwInfo.dataAddr);

            /* Configuring dest B&C Indexes                                   */
            paramSet.destBIdx  = 0;
            paramSet.destCIdx  = 0;

            if (IOM_COMPLETED != Mcasp_localGetIndicesSyncType(
                                     chanHandle,
                                     &paramSet.srcBIdx,
                                     &paramSet.srcCIdx,
                                     &paramSet.aCnt,
                                     &paramSet.bCnt,
                                     &paramSet.cCnt,
                                     &tempSyncType,
                                     TRUE))
            {
                status = IOM_EBADARGS;
            }
        }

        /* Configuring bCntReload                                             */
        paramSet.bCntReload = paramSet.bCnt;

        /* Src & Dest are in INCR modes                                       */
        paramSet.opt &= 0xFFFFFFFCu;

        /* FIFO width is 8 bit                                                */
        paramSet.opt &= 0xFFFFF8FFu;

        /* Set EDMA3_DRV_OPT_FIELD_TCINTEN to FALSE                           */
        paramSet.opt |= (0 << Mcasp_OPT_TCINTEN_SHIFT);

        /* Program the TCC                                                    */
        paramSet.opt |= ((chanHandle->tcc << Mcasp_OPT_TCC_SHIFT)
                          & Mcasp_OPT_TCC_MASK);

        /* EDMA3_DRV_SYNC_AB                                                  */
        paramSet.opt &= (~(0x00000004u));
        paramSet.opt |= (tempSyncType << Mcasp_OPT_SYNCDIM_SHIFT);


        paramSet.opt &= (~(0x00000800u));
        paramSet.opt |= (1 << Mcasp_OPT_TCCMOD_SHIFT);
        
        /* reset the interrupt generation bit                                 */
        paramSet.opt &= (~0x00100000u);

        if (IOM_COMPLETED == status)
        {
            /* Now, write the PaRAM Set.                                      */
            status = EDMA3_DRV_setPaRAM(chanHandle->edmaHandle,
                                        chanHandle->xferChan,
                                        &paramSet);
        }


        /* Configure the link PaRAMs with the appropriate parameters          *
         * Though we configure all the link paramsets, we will be using       *
         * only one of them to link with main xfer channel                    */
        for (linkCnt = 0; linkCnt < Mcasp_MAXLINKCNT; linkCnt++)
        {
            /* Get the PaRAM set for default parameters                       */
            EDMA3_DRV_getPaRAM (chanHandle->edmaHandle,
                                chanHandle->pramTbl[linkCnt],
                                &paramSet);

            if (IOM_INPUT == chanHandle->mode)
            {
                /* Configuring Source and Dest addresses                      */
#ifdef Mcasp_LOOPJOB_ENABLED
                paramSet.srcAddr  = (unsigned int)(instHandle->hwInfo.dataAddr);
#endif
                paramSet.destAddr = (unsigned int)(chanHandle->loopJobBuffer);

                /* Configurign Src and Dest B&C Indexes                       */
                paramSet.srcBIdx   = 0;
                paramSet.srcCIdx   = 0;

                if (IOM_COMPLETED != Mcasp_localGetIndicesSyncType(
                                         chanHandle,
                                         &paramSet.destBIdx,
                                         &paramSet.destCIdx,
                                         &paramSet.aCnt,
                                         &paramSet.bCnt,
                                         &paramSet.cCnt,
                                         &tempSyncType,
                                         TRUE))
                {
                    status = IOM_EBADARGS;
                }
            }
            else
            {
                /* Configuring Source and Dest addresses                      */
#ifdef Mcasp_LOOPJOB_ENABLED
                paramSet.srcAddr  = (unsigned int)(chanHandle->loopJobBuffer);
#endif
                paramSet.destAddr = (unsigned int)(instHandle->hwInfo.dataAddr);

                paramSet.destBIdx  = 0;
                paramSet.destCIdx  = 0;

                if (IOM_COMPLETED != Mcasp_localGetIndicesSyncType(
                                         chanHandle,
                                         &paramSet.srcBIdx,
                                         &paramSet.srcCIdx,
                                         &paramSet.aCnt,
                                         &paramSet.bCnt,
                                         &paramSet.cCnt,
                                         &tempSyncType,
                                         TRUE))
                {
                    status = IOM_EBADARGS;
                }
            }

            /* Configuring bCntReload                                         */
            paramSet.bCntReload = paramSet.bCnt;

            /* Src & Dest are in INCR modes                                   */
            paramSet.opt &= 0xFFFFFFFCu;

            /* FIFO width is 8 bit                                            */
            paramSet.opt &= 0xFFFFF8FFu;

            /* EDMA3_DRV_SYNC_AB                                              */
            paramSet.opt &= (~(0x00000004u));
            paramSet.opt |= (tempSyncType << Mcasp_OPT_SYNCDIM_SHIFT);

            /* Set EDMA3_DRV_OPT_FIELD_TCINTEN to FALSE                       */
            paramSet.opt |= (0 << Mcasp_OPT_TCINTEN_SHIFT);

            /* Program the TCC                                                */
            paramSet.opt |= ((chanHandle->tcc << Mcasp_OPT_TCC_SHIFT)
                              & Mcasp_OPT_TCC_MASK);

            /* early completion interrupt                                     */
            paramSet.opt &= (~(0x00000800u));
            paramSet.opt |= (1 << Mcasp_OPT_TCCMOD_SHIFT);

            /* reset the interrupt generation bit                                 */
            paramSet.opt &= (~0x00100000u);

            if (IOM_COMPLETED == status)
            {
                /* Now, write the PaRAM Set.                                  */
                status = EDMA3_DRV_setPaRAM(chanHandle->edmaHandle,
                                            chanHandle->pramTbl[linkCnt],
                                            &paramSet);
            }
#ifdef Mcasp_LOOPJOB_ENABLED
            if (Mcasp_MAXLINKCNT - 1u > linkCnt)
            {
                /* link with next paramset both param sets are  linked to each*
                 * other to enable continous xfer - either user audio data or *
                 * loopjob buffer MAXLINKCNT must be 2                        */
                status = EDMA3_DRV_linkChannel(
                            chanHandle->edmaHandle,
                            chanHandle->xferChan,
                            chanHandle->pramTbl[linkCnt+1]);

                if (IOM_COMPLETED != status)
                {
                    status = IOM_EBADARGS;
                    break;
                }

                status = EDMA3_DRV_linkChannel(
                            chanHandle->edmaHandle,
                            chanHandle->pramTbl[linkCnt] ,
                            chanHandle->pramTbl[linkCnt+1]);

                if (IOM_COMPLETED != status)
                {
                    status = IOM_EBADARGS;
                    break;
                }

                status = EDMA3_DRV_linkChannel(
                            chanHandle->edmaHandle,
                            chanHandle->pramTbl[linkCnt+1] ,
                            chanHandle->pramTbl[linkCnt+1]);

                if (IOM_COMPLETED != status)
                {
                    status = IOM_EBADARGS;
                    break;
                }

                chanHandle->nextLinkParamSetToBeUpdated = 0;
                chanHandle->loopjobUpdatedinParamset = TRUE;
            }
#endif
        }/* for loop for linkcount*/
    }while(falsewhile);

    return status;
}

/**
 *  \brief  This checks is the next request in queue is data request. If this is
 *          an abort request, it calls the appropriate function to deal
 *          with it.
 *
 *  \param  chanHandle [IN]  Handle to the Mcasp Channel
 *  \param  eb         [OUT] pointer to the error information block
 *
 *  \return IOM_COMPLETED,if request is Recieved/Transmitted completed
 *          Mcasp_STATUS_VALID, if request is not completed
 *
 *  \entry  chanHandle  should be non NULL and valid pointer
 *
 *  \leave  Not implemented
 */
static Int32 Mcasp_localIsValidPacket(Mcasp_ChannelHandle chanHandle)
{
    Int32  status     = IOM_COMPLETED;
    Bool   falsewhile = TRUE;

    do
    {
        falsewhile = FALSE;

        assert((NULL != chanHandle) && (NULL != chanHandle->devHandle));

        /* Present request validation for int mode transfer                   */
        if (((FALSE == chanHandle->isDmaDriven)
            && (NULL != chanHandle->dataPacket))
            && (0u != chanHandle->currentDataSize))
        {
            /* in interrupt mode - some transaction is going on               */
            status = Mcasp_STATUS_VALID;
        }
        else
        {
            /* Look for new request to be processed                           */
            if (TRUE == QUE_empty(&(chanHandle->queueReqList)))
            {
                /* we dont have any packet in queue stop xmt/rct sm before    *
                 * giving error                                               */
                chanHandle->dataPacket = NULL;

                if (FALSE == chanHandle->isDmaDriven)
                {
                    /* As interrupt mode there is not loopjob implementation  *
                     * we have to stop the state machine here                 */
                    chanHandle->dataQueuedOnReset = 0;
                    chanHandle->dataPacket = NULL;
                    chanHandle->currentDataSize = 0;
                    chanHandle->userDataBufferSize = 0;
                    chanHandle->xferinProgressIntmode = FALSE;

                    /* Stopping the state machine is taken care outside this  *
                     * function                                               */
                }/* DMA driven is FALSE if loop */


                /* for DMA mode only status is enought to program the loopjob *
                 * buffer                                                     */
                status = Mcasp_STATUS_INVALID;
            }
            else
            {
                status = Mcasp_STATUS_VALID;

                chanHandle->dataPacket = QUE_get(&(chanHandle->queueReqList));

                if (NULL != chanHandle->dataPacket)
                {
                    /* we have a valid packet to process next                 */

                    /* in int mode - current packet will be only one hence we *
                     * can use chanHandle->dataPacket for floating / current  *
                     * packet                                                 */

                    chanHandle->currentDataSize =
                        (Uint16)(chanHandle->dataPacket->size);
                    chanHandle->userDataBufferSize =
                        (Uint32)(chanHandle->dataPacket->size);
                    status = Mcasp_STATUS_VALID;

                    if (TRUE == chanHandle->isDmaDriven)
                    {
                        /* Additional to assigning the important parameters as*
                         * above for DMA mode , we will have max 2 packets    *
                         * floating and hence we use a active queue to manage */
                        QUE_put(
                            &(chanHandle->queueFloatingList),
                            (Ptr)chanHandle->dataPacket);
                    }
                }
            } /* Request List queue Empty is TRUE if loop */
        }
    }while(falsewhile);

    return status;
}

/*!
 *  \brief  This function updates the link param set accordingly with data or
 *          loopjobbuffer. This uses properly maintained index to idendtify to
 *          what param set the info has to be updated.
 *
 *  \param  chanHandle  [IN]  Handle to the mcasp channel
 *  \param  ioPacket    [IN]  Pointer to I/O packet
 *
 *  \return IOM_COMPLETED in case of sucess
 *          else Error code in case of failure
 *
 *  \entry  chanHandle  should be non NULL and valid pointer
 *          ioPacket    shoudl be non NULL and valid pointer
 *
 *  \leave  Not implemented
 */
Int32 Mcasp_localUpdtDtPktToLnkPrms(Mcasp_ChannelHandle       chanHandle,
                                    IOM_Packet *const         ioPacket)
{
    Mcasp_Object         *instHandle  = NULL;
    EDMA3_DRV_PaRAMRegs   pramPtr     = {0};
    Bool                  falsewhile  = TRUE;
    Mcasp_PktAddrPayload *payLoad     = NULL;
    Int32                 status      = IOM_COMPLETED;
    EDMA3_DRV_SyncType    syncType;

    do
    {
        falsewhile = FALSE;

        assert((NULL != chanHandle) && (NULL != chanHandle->devHandle));

        instHandle = (Mcasp_Object *)chanHandle->devHandle;

        assert(NULL != instHandle);

        /* if at all this is the first packet we will update the main params  *
         * else we will update the link params                                */
#ifdef Mcasp_LOOPJOB_ENABLED
        status = EDMA3_DRV_getPaRAM(
                    chanHandle->edmaHandle,
                    chanHandle->pramTbl[chanHandle->nextLinkParamSetToBeUpdated],
                    &pramPtr);
#else
        if (1u == chanHandle->submitCount)
        {
            status = EDMA3_DRV_getPaRAM(
                         chanHandle->edmaHandle,
                         chanHandle->xferChan,
                         &pramPtr);
        }
        else
        {
            status = EDMA3_DRV_getPaRAM(
                         chanHandle->edmaHandle,
                         chanHandle->pramTbl[chanHandle->nextLinkParamSetToBeUpdated],
                         &pramPtr);
        }
#endif
        if (IOM_COMPLETED != status )
        {
            status = IOM_EBADARGS;
            break;
        }

        /* following is the explanation behind the EDMA configuration         *
         *
         *  1) We will have n arrays of data (now it is loopjob buffer each of*
         *     length x bytes)                                                *
         *  2) for each sync event we have to fetch the data from each array  *
         *     and write to each serilaiser                                   *
         *  3) and the no. bytes transferred in this case from each array will*
         *     be roundedwordwidth/8                                          *
         *  4) as the loopjob buffer is continous, we configure the counts as *
         *     follows                                                        *
         *     a) consider there are arrays of length roundedwordwidth/8(aCnt)*
         *     b) these arrays are located apart lengthofloopjob size (bindex)*
         *     c) there are 'no.of.serialiser' no. of such arrays  (bCnt)     *
         *     d) (a) and (b) constitute one frame of EDMA transfer and it    *
         *        happens for one sync event in AB sync mode                  *
         *     e) such frames have to be sent till we have buffer - ie        *
         *        lengthofloopjob / (roundedwordwidth/8 )  and this           *
         *        constitutes  cCnt                                           *
         *     f) address of buffer (rather offset from start of previous     *
         *         frame) is roundedwordwidth/8 and this forms cIndex         *
         *  Please take some time to visualise the setup mentioned above to   *
         *  check with EDMA configuration. Also we change only specific       *
         *  parameters because others will not have change from what we       *
         *  programed intially                                                */

        /* Load the buffer pointer into the EDMA                              */

        /* control will reach this function is two contexts                   *
         *  1) From submitreq function for first time packets                 *
         *  2) From EDMA callback for further handling of packets in Queue    *
         *  -we are from EDMA callback becuase we have completed some user    *
         *  data packet just now                                              */
        if (NULL != ioPacket)
        {
            /* 1) when control comes to this function from submitreq control  *
             *    will surely comeinto the block                              *
             * 2) When control comes to this function from EDMA callback,     *
             *    control will some only if there is fuirther more packets to *
             *     handle                                                     */
            if (IOM_INPUT == chanHandle->mode)
            {
#ifndef Mcasp_LOOPJOB_ENABLED
                pramPtr.srcAddr  = (Uint32)(instHandle->hwInfo.dataAddr);
#endif
                if (TRUE == instHandle->isDataBufferPayloadStructure)
                {
                    payLoad = (Mcasp_PktAddrPayload *)ioPacket->addr;
                    assert(NULL != payLoad);
                    pramPtr.destAddr = (Uint32)payLoad->addr;
                }
                else
                {
                    pramPtr.destAddr = (Uint32)ioPacket->addr;
                }

                Mcasp_localGetIndicesSyncType(
                    chanHandle,
                    &(pramPtr.destBIdx),
                    &(pramPtr.destCIdx),
                    &(pramPtr.aCnt),
                    &(pramPtr.bCnt),
                    &(pramPtr.cCnt),
                    &syncType,
                    FALSE);
            }
            else
            {
#ifndef Mcasp_LOOPJOB_ENABLED
                pramPtr.destAddr  = (Uint32)(instHandle->hwInfo.dataAddr);
#endif
                if (TRUE == chanHandle->bMuteON)
                {
                    /* data buffer is changed to local mutebuffer             */
                    pramPtr.srcAddr  =
                        (Uint32)&Mcasp_muteBuf[(instHandle->instNum)].scratchBuffer;

                    /* calculation is similar to the loop job buffer          */
                    Mcasp_localGetIndicesSyncType(
                        chanHandle,
                        &(pramPtr.srcBIdx),
                        &(pramPtr.srcCIdx),
                        &(pramPtr.aCnt),
                        &(pramPtr.bCnt),
                        &(pramPtr.cCnt),
                        &syncType,
                        TRUE);
                }
                else
                {
                    if (TRUE == instHandle->isDataBufferPayloadStructure)
                    {
                        payLoad = (Mcasp_PktAddrPayload *)ioPacket->addr;
                        pramPtr.srcAddr = (Uint32)payLoad->addr;
                    }
                    else
                    {
                        pramPtr.srcAddr = (Uint32)ioPacket->addr;
                    }

                    Mcasp_localGetIndicesSyncType(
                        chanHandle,
                        &(pramPtr.srcBIdx),
                        &(pramPtr.srcCIdx),
                        &(pramPtr.aCnt),
                        &(pramPtr.bCnt),
                        &(pramPtr.cCnt),
                        &syncType,
                        FALSE);
                }/*for mute on/off*/
            }

            /* Here we are enabling the interrupt for this param set info     *
             * we do it here in this way because using int enable API after   *
             * calling EDMA3_DRV_setPaRAM, may fail our intention. This is    *
             * because immediately after the set param the paramset might be  *
             * copied to linked param and will make the consecutive int enable*
             * (set opt field) api useless for our purpose                    */

            pramPtr.opt |= (0x01u<<20u);

#ifdef Mcasp_LOOPJOB_ENABLED
            /* we are loading data request in the param set. As per our design*
             * when we load the data,  we should link this param set with next*
             * one most probably                                              *
             *  a)That may have loopjob loaded                                *
             *  b)it may be the currently loaded set, which will be updated in*
             *    next edma callback                                          *
             * hence in both cases to be run next to this data request        */
            pramPtr.linkAddr =
              (Uint16)(chanHandle->pramTblAddr[ \
              ((chanHandle->nextLinkParamSetToBeUpdated+1) & 0x01)] & 0x0000FFFF);
#else
            /* set the link address as 0xFFFF                                 */
            pramPtr.linkAddr = 0xFFFFu;
#endif
        }
#ifdef Mcasp_LOOPJOB_ENABLED
        else
        {
            /* we have reached here becuase we dont have any more packett to  *
             * submit for Xfer assign loop job to params- no change in linking*
             * IMP: The following assignment will be skipped if already both  *
             * params are having loopjob buffer                               */
            if (IOM_INPUT == chanHandle->mode)
            {
                pramPtr.destAddr = (Uint32)chanHandle->loopJobBuffer;
                Mcasp_localGetIndicesSyncType(
                    chanHandle,
                    &(pramPtr.destBIdx),
                    &(pramPtr.destCIdx),
                    &(pramPtr.aCnt),
                    &(pramPtr.bCnt),
                    &(pramPtr.cCnt),
                    &syncType,
                    TRUE);
            }
            else
            {
                pramPtr.srcAddr = (Uint32)chanHandle->loopJobBuffer;
                Mcasp_localGetIndicesSyncType(
                    chanHandle,
                    &(pramPtr.srcBIdx),
                    &(pramPtr.srcCIdx),
                    &(pramPtr.aCnt),
                    &(pramPtr.bCnt),
                    &(pramPtr.cCnt),
                    &syncType,
                    TRUE);
            }

            /* we are loading loopjob into this param.  That means we have    *
             * reached the end of packets in pending and floating queues. So  *
             * this loopjob had to be  linked to itself.Also we need to point *
             * the index to other param set in order to load data request     *
             * directly from submit call                                      */
            pramPtr.linkAddr =
                (Uint16)(chanHandle->pramTblAddr[    \
                chanHandle->nextLinkParamSetToBeUpdated] & 0x0000FFFF);

            pramPtr.opt &= (~(0x01<<20));
        }
#else
        else
        {
            status = IOM_EBADARGS;
            break;
        }
#endif

#ifdef Mcasp_LOOPJOB_ENABLED
        /* Update the changed param set info into the appropriate paramset    */
        status = EDMA3_DRV_setPaRAM(
                    chanHandle->edmaHandle,
                    chanHandle->pramTbl[chanHandle->nextLinkParamSetToBeUpdated],
                    &pramPtr);
#else
        if (1u == chanHandle->submitCount)
        {
            /* Configuring bCntReload                                         */
            pramPtr.bCntReload = pramPtr.bCnt;

            /* Src & Dest are in INCR modes                                   */
            pramPtr.opt &= 0xFFFFFFFCu;

            /* FIFO width is 8 bit                                            */
            pramPtr.opt &= 0xFFFFF8FFu;

            /* Set EDMA3_DRV_OPT_FIELD_TCINTEN to TRUE                        */
            pramPtr.opt |= (1 << Mcasp_OPT_TCINTEN_SHIFT);

            /* Program the TCC                                                */
            pramPtr.opt |= ((chanHandle->tcc << Mcasp_OPT_TCC_SHIFT)
                              & Mcasp_OPT_TCC_MASK);

            /* EDMA3_DRV_SYNC_AB                                              */
            pramPtr.opt &= (~(0x00000004u));
            pramPtr.opt |= (syncType << Mcasp_OPT_SYNCDIM_SHIFT);

            /* set the link address as 0xFFFF                                 */
            pramPtr.linkAddr = 0xFFFFu;

            status = EDMA3_DRV_setPaRAM(
                        chanHandle->edmaHandle,
                        chanHandle->xferChan,
                        &pramPtr);
        }
        else
        {
            status = EDMA3_DRV_setPaRAM(
                         chanHandle->edmaHandle,
                         chanHandle->pramTbl[chanHandle->
                             nextLinkParamSetToBeUpdated],
                         &pramPtr);
        }

        if (2u == chanHandle->submitCount)
        {
            /* link this paramset with the main channel                       */
            status = EDMA3_DRV_linkChannel(
                        chanHandle->edmaHandle,
                        chanHandle->xferChan,
                        chanHandle->pramTbl \
                        [chanHandle->nextLinkParamSetToBeUpdated]);

            if (IOM_COMPLETED != status)
            {
                status = IOM_EBADIO;
                break;
            }
        }

        /* link this paramset with the other paramset                         */
        if (3u == chanHandle->submitCount)
        {
            status = EDMA3_DRV_linkChannel(
                        chanHandle->edmaHandle,
                        chanHandle->pramTbl[
                            (chanHandle->nextLinkParamSetToBeUpdated + 1u) & 0x01u],
                        chanHandle->pramTbl \
                        [chanHandle->nextLinkParamSetToBeUpdated]);

            if (IOM_COMPLETED != status)
            {
                status = IOM_EBADIO;
                break;
            }
        }
#endif
        if (IOM_COMPLETED != status)
        {
            status = IOM_EBADARGS;
            break;
        }
#ifdef Mcasp_LOOPJOB_ENABLED
        if (NULL == ioPacket)
        {
            /* we are loading loopjob into this param. we need to point the   *
             * index to other param set in order to load data request directly*
             * from submit call                                               */
            Mcasp_localGetNextIndex(
                &chanHandle->nextLinkParamSetToBeUpdated);
        }
#endif
    }while(falsewhile);

    return status;
}

/**
 *  \brief  Mcasp_localGetIndicesSyncType
 *
 *  This function computes the parameters requied to configure EDMA 3 based
 *  on the buffer format seleted while creating the driver.
 *
 *  Entry Criteria : This function expects channel variables such as no of slots
 *                     serializers, length of buffers is updated.
 *
 *  \param    chanHandle       [IN]    Pointer to channel handle
 *  \param    bIndex           [IN]    Pointer to stores computed bIndex
 *  \param    cIndex           [IN]    Pointer to stores computed cIndex
 *  \param    aCnt             [IN]    Pointer to stores computed aCnt
 *  \param    bCnt             [IN]    Pointer to stores computed bCnt
 *  \param    cCnt             [IN]    Pointer to stores computed cCnt
 *  \param    syncType         [IN]    Pointer to stores computed mode of EDMA
 *  \param    forLoopJobBuf    [IN]    To calucalate above for loopJob or data
 *
 * CAUTION : This function could be called with pointer pointing to EDMA 3
 *           paramset pointer (avoiding couple of variables). Care should be
 *           take to ensure that data types used in EDMA 3 paramset is
 *           consistent with EDMA 3 defined data types.
 *
 *  \return  IOM_EBADARGS on invalid buffer format, else IOM_COMPLETED
 */
Int32 Mcasp_localGetIndicesSyncType(Mcasp_ChannelHandle chanHandle,
                                    volatile Int16     *bIndex,
                                    volatile Int16     *cIndex,
                                    volatile Uint16    *aCnt,
                                    volatile Uint16    *bCnt,
                                    volatile Uint16    *cCnt,
                                    EDMA3_DRV_SyncType *syncType,
                                    Bool                forLoopJobBuf)
{
    Uint32  tempSize  = 0x0;
    Int32   status    = IOM_COMPLETED;

    assert(((NULL != chanHandle) && (NULL != bIndex) && (NULL != cIndex)
                    && (NULL != aCnt) && (NULL != bCnt) && (NULL != cCnt)
                    && (NULL != syncType)));

#ifdef Mcasp_LOOPJOB_ENABLED
    /* Get the size of the buffer to be used. In case of mute command and     *
     * normal IOP we will use the actual size of the buffer. For the loop job *
     * case 1. if internal loop job then the minimum loop job length will be  *
     *         used.                                                          *
     * case 2. user supplied loop job buffer then the user loop job length    *
     *         will be used.                                                  */    
    if ((TRUE == forLoopJobBuf) && (FALSE == chanHandle->bMuteON))
    {
        if (TRUE == chanHandle->userLoopJob)
        {
            /* use the user supplied buffer length for the EDMA params        */
            tempSize = chanHandle->userLoopJobLength;
        }
        else
        {
            tempSize = chanHandle->loopJobLength;
        }
    }
    else
    {
        tempSize = chanHandle->userDataBufferSize;
    }
#else
    tempSize = chanHandle->userDataBufferSize;
#endif

    /* Get the indices                                                        */
    switch (chanHandle->dataFormat)
    {
        case Mcasp_BufferFormat_1SER_1SLOT:
        case Mcasp_BufferFormat_1SER_MULTISLOT_INTERLEAVED:
            *aCnt = chanHandle->roundedWordWidth;
            *bCnt = chanHandle->noOfSerAllocated;
            *cCnt = (Uint16)(tempSize/(*aCnt));
            *bIndex = (Int16)tempSize;
            *cIndex = (Int16)(*aCnt);
            *syncType = EDMA3_DRV_SYNC_AB;
            break;
        case Mcasp_BufferFormat_MULTISER_1SLOT_SER_NON_INTERLEAVED:
            *aCnt = chanHandle->roundedWordWidth;
            *bCnt = chanHandle->noOfSerAllocated;
            /* temp Size is always a multiple of the acnt hence the division  *
             * result will always be an integer                               */
            *cCnt = (Uint16)(tempSize/ ((*aCnt)*(*bCnt)));
            /* temp Size is always a multiple of the bcnt when using the      *
             * multiple serializers hence the reuslt will be always an integer*/
            *bIndex = (Int16)(tempSize /(*bCnt));
            *cIndex = (Int16)(*aCnt);
            *syncType = EDMA3_DRV_SYNC_AB;
            break;
        case Mcasp_BufferFormat_1SER_MULTISLOT_NON_INTERLEAVED:
            *aCnt = chanHandle->roundedWordWidth;
            *bCnt = chanHandle->noOfSlots;
            /* temp Size is always a multiple of the acnt hence the division  *
             * result will always be an integer                               */
            *cCnt = (Uint16)(tempSize /((*aCnt) * (*bCnt)));
            /* temp Size is always a multiple of the bcnt when using the      *
             * multiple slots hence the result will be always an integer      */
            *bIndex = (Int16)(tempSize/(*bCnt));
            *cIndex = (Int16)((*aCnt) - (((*bCnt)- 1u) * (tempSize/(*bCnt))));
            *syncType = EDMA3_DRV_SYNC_A;
            break;
        case Mcasp_BufferFormat_MULTISER_1SLOT_SER_INTERLEAVED:
            *aCnt = (chanHandle->roundedWordWidth);
            *bCnt = chanHandle->noOfSerAllocated;
            /* temp Size is always a multiple of the bcnt when using the      *
             * multiple serializers hence the result will be always an integer*/
            *cCnt = (Uint16)(tempSize / ((*aCnt) * (*bCnt)));
            *bIndex = (Int16)(*aCnt);
            *cIndex = (Int16)((*aCnt) * (*bCnt));
            *syncType = EDMA3_DRV_SYNC_AB;
            break;
        case Mcasp_BufferFormat_MULTISER_MULTISLOT_SEMI_INTERLEAVED_1:
            *aCnt =  chanHandle->roundedWordWidth;
            *bCnt =  chanHandle->noOfSerAllocated;
            /* temp Size is always a multiple of the bcnt when using the      *
             * multiple serializers hence the result will be always an integer*/
            *cCnt =  (Uint16)(tempSize/((*bCnt) * (*aCnt)));
            *bIndex = (Int16)(*aCnt);
            *cIndex = (Int16)((*aCnt) * (*bCnt));
            *syncType = EDMA3_DRV_SYNC_AB;
            break;
        case Mcasp_BufferFormat_MULTISER_MULTISLOT_SEMI_INTERLEAVED_2:
            *aCnt = chanHandle->roundedWordWidth;
            *bCnt = chanHandle->noOfSerAllocated;
            /* temp Size is always a multiple of the acnt hence the division  *
             * result will always be an integer                               */
            *cCnt  = (Uint16)(tempSize/((*aCnt)*(*bCnt)));
            /* temp Size is always a multiple of the bcnt when using the      *
             * multiple serializers hence the result will be always an integer*/
            *bIndex = (Int16)(tempSize/(*bCnt));
            *cIndex = (Int16)(*aCnt);
            *syncType = EDMA3_DRV_SYNC_AB;
            break;
#if 0
        /* we are unable to support this buffer format because the MCASP gives *
         * tx underrun error.
         * When 2 serializers are configured in 2 slot mode each. The MCASP   *
         * even though it receives two 16 bit samples it considers it as only *
         * one 32 bit sample because of which it will always give an underrun *
         * error. Hence this buffer format cannot be supported                */
        case Mcasp_BufferFormat_MULTISER_MULTISLOT_SEMI_INTERLEAVED_3:
            *aCnt = ((chanHandle->roundedWordWidth) * chanHandle->noOfSerAllocated);
            *bCnt = chanHandle->noOfSlots;
            *cCnt = (tempSize / ((*aCnt) *(*bCnt)));
            *bIndex = (tempSize/(*bCnt));
            *cIndex = ((*aCnt) - (((*bCnt) - 1) * (tempSize/(*bCnt))));
            *syncType = EDMA3_DRV_SYNC_A;
            break;
#endif
        default:
            status = IOM_EBADARGS;
               break;
    }

    /* if the loop job buffer being used is the driver internal loop job      *
     * buffer, dont increment the index for it.Same is the case if mute is ON */
    if (TRUE == forLoopJobBuf)
    {
        if ((TRUE == chanHandle->bMuteON) ||
            (FALSE == chanHandle->userLoopJob))
        {
            *bIndex = 0;
            *cIndex = 0;
        }
    }
   return status;
}

/* ========================================================================== */
/*                             END OF FILE                                    */
/* ========================================================================== */
