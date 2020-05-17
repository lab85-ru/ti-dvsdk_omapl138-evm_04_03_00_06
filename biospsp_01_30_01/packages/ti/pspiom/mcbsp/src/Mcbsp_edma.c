/*
 * Mcbsp_edma.c
 *
 * This file contains local functions for MCBSP driver which contain implemen-
 * tation for EDMA operation specifc calls like starting an EDMA transfer for
 * for McBSP peripheral, EDMA completion and/or error callbacks etc
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
 *  \file   Mcbsp_edma.c
 *
 * \brief   This files contains the edma functions required for the Mcbsp driver
 *          to function in the EDMA mode.
 *
 *   (C) Copyright 2009, Texas Instruments, Inc
 *
 * \author  Platform Support Group
 *
 * \version 0.2     Added the support for the Non loop job mode
 *          0.1     Newly created.
 */

/*============================================================================*/
/*                              INCLUDE FILES                                 */
/*============================================================================*/

#include <std.h>
#include <assert.h>
#include <bcache.h>
#include "ti/pspiom/cslr/cslr_edma3cc.h"
#include "McbspLocal.h"

/*============================================================================*/
/*                            IMPORTED VARIABLES                              */
/*============================================================================*/

extern far Mcbsp_TempBuffer Mcbsp_muteBuf[CSL_MCBSP_PER_CNT];

/*============================================================================*/
/*                          LOCAL FUNCTION PROTOTYPES                         */
/*============================================================================*/

static Int32 mcbspIsValidPacket(Mcbsp_ChannelObj *chanHandle);

#ifdef Mcbsp_SPI_ENABLE
static Void mcbspSpiEdmaTxCallback(Uint32              tcc,
                                   EDMA3_RM_TccStatus  status,
                                   Ptr                 data);
static Void mcbspSpiEdmaRxCallback(Uint32              tcc,
                                   EDMA3_RM_TccStatus  status,
                                   Ptr                 data);
#endif
/*============================================================================*/
/*                             LOCAL VARIABLES                                */
/*============================================================================*/

#ifdef Mcbsp_SPI_ENABLE
/*
 * \brief  Buffer to be used when the mcbsp is in SPI mode for the transceive
 *         operation.
 */
/* aligns the buffer to the cache line length  */
#pragma DATA_ALIGN(Mcbsp_SpiTransceiveBuff, 32);
Uint8 Mcbsp_SpiTransceiveBuff[Mcbsp_SPI_TRANSCEIVE_BUF_LEN];
#endif /* Mcbsp_SPI_ENABLE */

/* ========================================================================== */
/*                          FUNCTION DEFINITIONS                              */
/* ========================================================================== */

/**
 * \brief   Function to intialize and set up the edma for transfer.
 *
 *          This function does the following
 *          1) Requests the EDMA channel from the edma driver.
 *          2) Requests the link channels for linking (exclusive for tx & rx)
 *          3) set up the param set of main xfer channel (for loopjob transfers)
 *          4) set up the param sets of all link channel (for loopjob transfers)
 *          5) link the last link channel to itself (this channel will be linked
 *             to main xfer channel later and to supply the loopjob xfer
 *             information infinely this is done so;
 *          other link channels are setup here just for completion sake
 *
 * \param   chanHandle   [IN] Handle to the channel for which the edma is setup
 *
 * \return  status       IOM_COMPLETED if is sucess
 *                       error id in case of failure
 *
 */
Int32 Mcbsp_localSetupEdmaDuringOpen(Mcbsp_ChannelObj *chanHandle)
{
    Uint32              reqTcc      = EDMA3_DRV_TCC_ANY;
    Bool                falsewhile  = TRUE;
    Uint32              linkCnt     = 0;
    Uint32              edmaChanNum = 0;
    Uint32              count       = 0;
    Int32               status      = IOM_COMPLETED;

    do
    {
        falsewhile  = FALSE;
        assert(NULL != chanHandle);

        /* request the transfer channel from the EDMA driver                  */
        status = EDMA3_DRV_requestChannel(
                     chanHandle->edmaHandle,
                     &chanHandle->xferChan,
                     &chanHandle->tcc,
                     chanHandle->edmaEventQue,
                     (chanHandle->edmaCallback),
                     chanHandle);

        if (IOM_COMPLETED != status)
        {
            status = IOM_EBADARGS;
            break;
        }

        /* if there are any errors set in the EDMA clear them here            */
        EDMA3_DRV_clearErrorBits(
            chanHandle->edmaHandle,
            chanHandle->xferChan);

        /* Acquire the  PaRAM entries used for EDMA transfers linking         */
        for (count = 0; count < Mcbsp_MAXLINKCNT; count++)
        {
            /* For requesting for a PaRam set                                 */
            edmaChanNum = EDMA3_DRV_LINK_CHANNEL;
            reqTcc = EDMA3_DRV_TCC_ANY;

            status = EDMA3_DRV_requestChannel(
                        chanHandle->edmaHandle,
                        &edmaChanNum,
                        &reqTcc,
                        chanHandle->edmaEventQue,
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
                for (linkCnt = 0; linkCnt < count; linkCnt++)
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
        if (IOM_COMPLETED == status)
        {
            status = Mcbsp_localEdmaChanPaRAMsetup(chanHandle);
        }
    }while(falsewhile);
    return status;
}
#ifdef Mcbsp_LOOPJOB_ENABLE
/**
 *  \brief   This calls the registered application callback for the current
 *           receive request and processes the next request in queue.
 *           This if a mcbsp edma callback function called in edma context
 *
 *  \param   tcc        [IN]  tcc number of EDMA
 *  \param   edmaStatus [IN]  status of EDMA transaction
 *  \param   data       [IN]  paramaters sent to EDMA ISR (chan handler)
 *
 *  \return  Nothing
 *
 *  \entry   tcc         should be a valid value
 *           status      should be a valid value
 *           data        should be a non NULL and valid pointer
 *
 *  \leave   Not implemented
 */
Void Mcbsp_localEdmaCallback(Uint32 tcc, EDMA3_RM_TccStatus edmaStatus, Ptr data)
{
    Mcbsp_ChannelObj      *chanHandle = NULL;
    EDMA3_DRV_PaRAMRegs    pramTbl    = {0};
    Int32                  status     = IOM_COMPLETED;
    IOM_Packet            *ioPacket   = NULL;

    assert(NULL != data);

    chanHandle = (Mcbsp_ChannelObj *)data;
    /* to remove the compiler warning                                         */
    tcc= tcc;

    if (TRUE != QUE_empty(&(chanHandle->queueFloatingList)))
    {
        /* should always have a packet present because this callback is       *
         * due to that packet                                                 */

        /* Get the packet from the top of the queue (atommic operation)       */
        chanHandle->tempPacket = QUE_get(&(chanHandle->queueFloatingList));

        /* get the param table information of transfer channel                */
        EDMA3_DRV_getPaRAM(chanHandle->edmaHandle,chanHandle->xferChan,&pramTbl);

        /* Handle the IOP packets appropriately in case of an breakpoint      *
         * in case of an breakpoint.either of the packets (2 link param Pkts) *
         * could have caused a callback as both of them as linkedto each other*
         * Hence we will handle that condition here                           */
        if (chanHandle->mode == IOM_INPUT)
        {
            /* Check if destination address falls into the range of 1st req   *
             * in the floating queue.                                         */
            if ((pramTbl.destAddr >= (Uint32)chanHandle->tempPacket->addr)
                && (pramTbl.destAddr < (Uint32)chanHandle->tempPacket->addr
                        + chanHandle->tempPacket->size))
            {
                /* Since we have already dequeue the 1st request, dequeue     *
                 * 2nd io request from floating queue                         */
                ioPacket = (IOM_Packet *)QUE_get(
                                      &chanHandle->queueFloatingList);

                /* Queue the tempPacket (i.e. 1st io request) as this pkt     *
                 * should be first in a queue                                 */
                QUE_put(
                    &chanHandle->queueFloatingList,
                    (Ptr)chanHandle->tempPacket);

                /* Queue the ioPacket i.e. 2nd request in floating queue      */
                QUE_put(&chanHandle->queueFloatingList,(Ptr)ioPacket);
            }
        }
        else
        {
            /* Check if destination address falls into the range of1st request*
             * in the floating queue.                                         */
            if ((pramTbl.srcAddr >= (Uint32)chanHandle->tempPacket->addr)
                && (pramTbl.srcAddr < (Uint32)chanHandle->tempPacket->addr
                        + chanHandle->tempPacket->size))
            {
                /* Since we have already dequeue the 1st request, dequeue     *
                 * io request from floating queue                             */
                ioPacket = (IOM_Packet *)QUE_get(
                               &chanHandle->queueFloatingList);

                /* Queue the tempPacket (i.e. 1st io request) as this         *
                 * packet should be first in a queue                          */
                QUE_put(
                    &chanHandle->queueFloatingList,
                    (Ptr)chanHandle->tempPacket);

                /* Queue the ioPacket i.e. 2nd request in floating queue      */
                QUE_put(&chanHandle->queueFloatingList,(Ptr)ioPacket);
            }
        }

        if (IOM_COMPLETED == status)
        {
            if (EDMA3_RM_XFER_COMPLETE != edmaStatus)
            {
                /* update the status of the IOP as error                      */
                chanHandle->tempPacket->status = IOM_EABORT;

                /* There is a data transfer error in EDMA transfer. clear the *
                 * Error bits and return                                      */
                EDMA3_DRV_clearErrorBits(
                    chanHandle->edmaHandle,
                    chanHandle->xferChan);
            }
            else
            {
                chanHandle->tempPacket->status  = IOM_COMPLETED;
            }

            chanHandle->submitCount--;
        }

        if (IOM_COMPLETED == status)
        {
            /* if the channel is not in a pause mode                          */
            if (FALSE == chanHandle->paused)
            {
                /* Now that we have done with the last data packet - we check *
                 * if next packet is available for transmission. Even if we   *
                 * are not having a valid packet - previous programming of    *
                 * linkchannels (designed in such a way) to send out null     *
                 * buffer Also note that Mcbsp_localIsValidPacket function    *
                 * will set the chanHandle->dataPacket with next (current now)*
                 * data packet from queue                                     */
                if ((Mcbsp_STATUS_VALID == (mcbspIsValidPacket(chanHandle))))
                {
                    /* This is a valid data request. Call function to process *
                     * it. If DMA cannot be programmed properly with request  *
                     * contents, abort all queued up requests and put state   *
                     * M/C into reset.                                        */
                    status = Mcbsp_localEdmaProcessPkt(
                                 chanHandle,
                                 chanHandle->dataPacket);

                    if (IOM_COMPLETED != status)
                    {
                        Mcbsp_localCancelAndAbortAllIo(chanHandle);
                    }
                }
                else
                {
                   /* The following function will assign loopjob buffer to    *
                    * free param set Also the control will come here two times*
                    * for last packet and last but one packet (obviously for  *
                    * more than one packet xfer) For last but packet callback *
                    * itself we will come to know that there is no more packet*
                    * pending and hence we load loopjob info into next param  *
                    * set and for the last packet callback will not do it     *
                    * again (for only one packet scenario the following flag  *
                    * will take care not to update the loopjob second time)   */
                    if (FALSE  == chanHandle->loopjobUpdatedinParamset)
                    {
                        chanHandle->loopjobUpdatedinParamset = TRUE;

                        Mcbsp_localGetNextIndex(
                            &chanHandle->nextLinkParamSetToBeUpdated);

                        Mcbsp_localUpdtDtPktToLnkPrms(chanHandle,NULL);
                    }
                }
            }
            else
            {
                /* Control will come over here when either of receive or      *
                 * transmit state  machine stop command is issued for receive *
                 * or transmit channel. If the nextFlag is already set        *
                 * indicates that its time to reset the state machines and    *
                 * disable the edma transfer                                  */
                if (TRUE == chanHandle->nextFlag)
                {
                    chanHandle->nextFlag = FALSE;
                }
                else
                {
                    /* For the first time when the stop port command is issued*
                     * we will have one more packet linked with the transfer  *
                     * channel. So we will not stop at this time. We link the *
                     * packet with NULL loopjob buffer and set a nextFlag to  *
                     * TRUE. We will wait for another callback(indicating the *
                     * io request is complete) to disable EDMA transfer and   *
                     * reset state  machines                                  */
                    chanHandle->nextFlag = TRUE;

                    Mcbsp_localGetNextIndex(&chanHandle->nextLinkParamSetToBeUpdated);

                    Mcbsp_localUpdtDtPktToLnkPrms(chanHandle, NULL);
                }
            }

            if (IOM_COMPLETED == status)
            {
                chanHandle->isTempPacketValid = TRUE;
                Mcbsp_localCompleteCurrentIo(chanHandle);
            }
        }
    }
}

/*!
 *  \brief  This function updates the link param set accordingly with data or
 *          loopjobbuffer. This uses properly maintained index to identify to
 *          what param set the info has to be updated.
 *
 *  \param  chanHandle  [IN]  Handle to the mcbsp channel
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
Int32 Mcbsp_localUpdtDtPktToLnkPrms(Mcbsp_ChannelObj *chanHandle,
                                    IOM_Packet       *const ioPacket)
{
    Mcbsp_Object         *instHandle  = NULL;
    EDMA3_DRV_PaRAMRegs   pramPtr     = {0};
    Bool                  falsewhile  = TRUE;
    Int32                 status      = IOM_COMPLETED;
    EDMA3_DRV_SyncType    syncType;

    do
    {
        falsewhile = FALSE;
        assert((NULL != chanHandle) && (NULL != chanHandle->devHandle));

        instHandle = (Mcbsp_Object *)chanHandle->devHandle;
        assert(NULL != instHandle);

        /* if at all this is the first packet we will update the main params  *
         * else we will update the link params                                */
        status = EDMA3_DRV_getPaRAM(
                    chanHandle->edmaHandle,
                    chanHandle->pramTbl[chanHandle->nextLinkParamSetToBeUpdated],
                    &pramPtr);
        if (IOM_COMPLETED != status )
        {
            status = IOM_EBADARGS;
            break;
        }

    /* Following the explanation behind the EDMA configuration                *
     * 1) We will have n arrays of data (now it is data buffer each of length *
     *    databuffersize bytes)                                               *
     * 2) for each sync event we have to fetch the data from each array and   *
     *    write to each slot                                                  *
     * 3) and the no. bytes transferred in this case from each array will be  *
     *    roundedwordwidth/4                                                  *
     * 4) as the loopjob buffer is contigous, we configure the counts as      *
     *    follows                                                             *
     *    a) consider there are arrays of length roundedwordwidth/4 (aCnt)    *
     *    b) these arrays are located apart databuffersize size  (bindex)     *
     *    c) there are 'no.of.slots' no. of such arrays   (bCnt)              *
     *    d) (a) and (b) constitute one frame of EDMA transfer and it happens *
     *       for one sync event in AB sync mode.                              *
     *    e) such frames have to be sent till we have buffer - ie             *
     *       databuffersize / (roundedwordwidth/8 ) and this constitutes  cCnt*
     *    f) address of buffer (rather offset from start of previous frame)   *
     *       is roundedwordwidth/8  and this forms cIndex                     *
     *                                                                        *
     *  Please take some time to visualise the setup mentioned above to check *
     *  with EDMA configuration                                               *
     *  Also we change only specific parameters because others will not have  *
     *  change from what we programed  intially                               */

    /* control will reach this function is two contexts                       *
     * 1) From submitreq function for first time packets                      *
     * 2) From EDMA callback for further handling of packets in Queue         *
     *    - we are from EDMA callback becuase we have completed some user     *
     *   data packet just now                                                 */

        if (NULL != ioPacket)
        {
            /* 1) when control comes to this function from submitreq control  *
             *    will surely comeinto the block                              *
             * 2) When control comes to this function from EDMA callback,     *
             *    control will some only if there is fuirther more packets to *
             *     handle                                                     */
            if (IOM_INPUT == chanHandle->mode)
            {
                pramPtr.destAddr = (Uint32)ioPacket->addr;
                Mcbsp_localGetIndicesSyncType(
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
                if (TRUE == chanHandle->bMuteON)
                {
                    /* Mute is set hence we need to switch to the mute buffer */
                    pramPtr.srcAddr =
                        (Uint32)&Mcbsp_muteBuf[instHandle->instNum].scratchBuffer;
                    Mcbsp_localGetIndicesSyncType(
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
                    pramPtr.srcAddr = (Uint32)ioPacket->addr;
                    Mcbsp_localGetIndicesSyncType(
                        chanHandle,
                        &(pramPtr.srcBIdx),
                        &(pramPtr.srcCIdx),
                        &(pramPtr.aCnt),
                        &(pramPtr.bCnt),
                        &(pramPtr.cCnt),
                        &syncType,
                        FALSE);
                }
            }

            /* Here we are enabling the interrupt for this param set info     *
             * we do it here in this way because using int enable API after   *
             * calling EDMA3_DRV_setPaRAM, may fail our intention. This is    *
             * because immediately after the set param the paramset might be  *
             * copied to linked param and will make the consecutive int enable*
             * (set opt field) api useless for our purpose                    */
            pramPtr.opt |= (0x01u<<20u);

            /* we are loading data request in the param set. As per our design*
             * when we load the data,  we should link this param set with next*
             * one most probably                                              *
             *  a) That may have loopjob loaded                               *
             *  b) it may be the currently loaded set, which will be updated  *
             *     in next edma callback.                                     *
             * hence in both cases to be run next to this data request        */
            pramPtr.linkAddr =
              (Uint16)(chanHandle->pramTblAddr[ \
              ((chanHandle->nextLinkParamSetToBeUpdated+1) & 0x01)] & 0x0000FFFF);
        }
        else
        {
            /* we have reached here becuase we dont have any more packett to  *
             * submit for Xfer assign loop job to params- no change in linking*
             * IMP: The following assignment will be skipped if already both  *
             * params are having  loopjob buffer                              */
            if (IOM_INPUT == chanHandle->mode)
            {
                pramPtr.destAddr = (Uint32)chanHandle->loopJobBuffer;
                Mcbsp_localGetIndicesSyncType(
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
                Mcbsp_localGetIndicesSyncType(
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
             * reached the end of packets inpending and floating queues. So   *
             * this loopjob had to be  linked to itself. Also we need to point*
             * the index to other param set in order to load data request     *
             * directly from submit call                                      */
            pramPtr.linkAddr =
                (Uint16)(chanHandle->pramTblAddr[    \
                chanHandle->nextLinkParamSetToBeUpdated] & 0x0000FFFF);

            pramPtr.opt &= (~(0x01<<20));
        }

        /* Update the changed param set info into the appropriate paramset    */
        status = EDMA3_DRV_setPaRAM(
                    chanHandle->edmaHandle,
                    chanHandle->pramTbl[chanHandle->nextLinkParamSetToBeUpdated],
                    &pramPtr);
        if (IOM_COMPLETED != status)
        {
            status = IOM_EBADARGS;
            break;
        }

        if (NULL == ioPacket)
        {
            /* we are loading loopjob into this param. we need to point the   *
             * index to other param set in order to load data request directly*
             * from submit call                                               */
            Mcbsp_localGetNextIndex(&chanHandle->nextLinkParamSetToBeUpdated);
        }
    }while(falsewhile);

    return status;
}

/**
 * \brief   setup the EDMA channel for the specific channel requested.
 *
 *          This function configures the McBSP synchonized EDMA channel PaRAM
 *          entries it also configures the various other parameters like
 *          source and destination address / modes
 *
 * \param   chanHandle   [IN]   Channel handle
 *
 * \return  IOM_COMPLETED  if setup successful
 *          Appropriate IOM error code if configuration has any error
 */
Int32 Mcbsp_localEdmaChanPaRAMsetup(Mcbsp_ChannelObj *chanHandle)
{
    Uint32               linkCnt    = 0;
    Mcbsp_Object        *instHandle = NULL;
    EDMA3_DRV_PaRAMRegs  paramSet   = {0,0,0,0,0,0,0,0,0,0,0,0};
    Int32                status     = IOM_COMPLETED;
    EDMA3_DRV_SyncType   tempSyncType;

    assert(NULL != chanHandle);
    assert(NULL != chanHandle->devHandle);

    instHandle = chanHandle->devHandle;
    assert(NULL != instHandle);

    /* Configure the McBSP synchonized EDMA channel PaRAM entries             *
     * Configure source and destination address / modes                       *
     * first - main xfer channel                                              */

    /* following is the explanation behind the EDMA configuration             *
     *
     *  1) We will have n arrays of data (now it is loopjob buffer each of    *
     *     length x bytes)                                                    *
     *  2) for each sync event we have to fetch the data from each array      *
     *     and write to each serilaiser                                       *
     *  3) and the no. bytes transferred in this case from each array will    *
     *     be roundedwordwidth/4                                              *
     *  4) as the loopjob buffer is continous, we configure the counts as     *
     *     follows                                                            *
     *     a) consider there are arrays of length roundedwordwidth/8(aCnt)    *
     *     b) these arrays are located apart lengthofloopjob size (bindex)    *
     *     c) there are 'no.of.slots' no. of such arrays  (bCnt)              *
     *     d) (a) and (b) constitute one frame of EDMA transfer and it        *
     *        happens for one sync event in AB sync mode                      *
     *     e) such frames have to be sent till we have buffer - ie            *
     *        lengthofloopjob / (roundedwordwidth/8 )  and this               *
     *        constitutes  cCnt                                               *
     *     f) address of buffer (rather offset from start of previous         *
     *         frame) is roundedwordwidth/8 and this forms cIndex             *
     *  Please take some time to visualise the setup mentioned above to       *
     *  check with EDMA configuration The same kind of setup will be used     *
     *  for data transfers also and the change will be buffer pointer         *
     *  and length of data buffer.                                            */

    /* Get the PaRAM set for default parameters                               */
    EDMA3_DRV_getPaRAM(chanHandle->edmaHandle,chanHandle->xferChan,&paramSet);

    if (IOM_INPUT == chanHandle->mode)
    {
        /* Configuring Source and Dest addresses                              */
        if (TRUE == chanHandle->enableHwFifo)
        {
            paramSet.srcAddr = (Uint32)(instHandle->hwInfo.dataAddress);
        }
        else
        {
            paramSet.srcAddr = (Uint32)&(instHandle->hwInfo.regs->DRR);
        }

        paramSet.destAddr = (unsigned int)(chanHandle->loopJobBuffer);

        /* Configuring Src and Dest B&C Indexes                               */
        paramSet.srcBIdx  = 0;
        paramSet.srcCIdx  = 0;

        if (IOM_COMPLETED != Mcbsp_localGetIndicesSyncType(
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
        /* Configuring Source and Dest addresses                              */
        paramSet.srcAddr  = (unsigned int)(chanHandle->loopJobBuffer);

        if (TRUE == chanHandle->enableHwFifo)
        {
            paramSet.destAddr = (Uint32)(instHandle->hwInfo.dataAddress);
        }
        else
        {
            paramSet.destAddr = (Uint32)&(instHandle->hwInfo.regs->DXR);
        }

        /* Configuring Src and Dest B&C Indexes                               */
        paramSet.destBIdx = 0;
        paramSet.destCIdx = 0;

        if (IOM_COMPLETED != Mcbsp_localGetIndicesSyncType(
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


    /* Configuring bCntReload                                                 */
    paramSet.bCntReload = paramSet.bCnt;

    /* Src & Dest are in INCR modes                                           */
    paramSet.opt &= 0xFFFFFFFCu;

    /* FIFO width is 8 bit                                                    */
    paramSet.opt &= 0xFFFFF8FFu;

    /* Set EDMA3_DRV_OPT_FIELD_TCINTEN to FALSE                               */
    paramSet.opt &= (~(CSL_EDMA3CC_OPT_ITCINTEN_MASK));

    paramSet.opt &= (~(CSL_EDMA3CC_OPT_TCINTEN_MASK));
    paramSet.opt |= (CSL_EDMA3CC_OPT_TCINTEN_DISABLE
                        << CSL_EDMA3CC_OPT_TCINTEN_SHIFT);

    /* Program the TCC                                                        */
    paramSet.opt |= ((chanHandle->tcc << CSL_EDMA3CC_OPT_TCC_SHIFT)
                      & CSL_EDMA3CC_OPT_TCC_MASK);

    /* EDMA3_DRV_SYNC_AB                                                      */
    paramSet.opt &= (~(CSL_EDMA3CC_OPT_SYNCDIM_MASK));
    paramSet.opt |= (tempSyncType << CSL_EDMA3CC_OPT_SYNCDIM_SHIFT);

    paramSet.opt &= (~(CSL_EDMA3CC_OPT_TCCMOD_MASK));
    paramSet.opt |= (1 << CSL_EDMA3CC_OPT_TCCMOD_SHIFT);

    /* Now, write the PaRAM Set.                                              */
    status = EDMA3_DRV_setPaRAM(
                  chanHandle->edmaHandle,
                  chanHandle->xferChan,
                  &paramSet);

    if (IOM_COMPLETED == status)
    {
        /* Configure the link PaRAMs with the appropriate parameters Though   *
         * we configure all the link paramsets, we will be using only one of  *
         * them to link with main xfer channel                                */
        for (linkCnt = 0; linkCnt < Mcbsp_MAXLINKCNT; linkCnt++)
        {
            /* Get the PaRAM set for default parameters                       */
            EDMA3_DRV_getPaRAM (chanHandle->edmaHandle,
                                chanHandle->pramTbl[linkCnt],
                                &paramSet);

            if (IOM_INPUT == chanHandle->mode)
            {
                /* Configuring Source and Dest addresses                      */
                if (TRUE == chanHandle->enableHwFifo)
                {
                    paramSet.srcAddr = (Uint32)(instHandle->hwInfo.dataAddress);
                }
                else
                {
                    paramSet.srcAddr = (Uint32)&(instHandle->hwInfo.regs->DRR);
                }

                paramSet.destAddr = (Uint32)(chanHandle->loopJobBuffer);

                /* Configuring Src and Dest B&C Indexes                       */
                paramSet.srcBIdx  = 0;
                paramSet.srcCIdx  = 0;

                if (IOM_COMPLETED != Mcbsp_localGetIndicesSyncType(
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
                paramSet.srcAddr  = (Uint32)(chanHandle->loopJobBuffer);

                if (TRUE == chanHandle->enableHwFifo)
                {
                    paramSet.destAddr = (Uint32)(instHandle->hwInfo.dataAddress);
                }
                else
                {
                    paramSet.destAddr = (Uint32)&(instHandle->hwInfo.regs->DXR);
                }

                paramSet.destBIdx  = 0;
                paramSet.destCIdx  = 0;

                if (IOM_COMPLETED != Mcbsp_localGetIndicesSyncType(
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
            paramSet.opt &= (~(CSL_EDMA3CC_OPT_SYNCDIM_MASK));
            paramSet.opt |= (tempSyncType << CSL_EDMA3CC_OPT_SYNCDIM_SHIFT);

            /* Set EDMA3_DRV_OPT_FIELD_TCINTEN to FALSE                       */
            paramSet.opt |= (CSL_EDMA3CC_OPT_TCINTEN_DISABLE
                                << CSL_EDMA3CC_OPT_TCINTEN_SHIFT);

            /* Program the TCC                                                */
            paramSet.opt |= ((chanHandle->tcc << CSL_EDMA3CC_OPT_TCC_SHIFT)
                              & CSL_EDMA3CC_OPT_TCC_MASK);

            /* early completion interrupt                                     */
            paramSet.opt &= (~(CSL_EDMA3CC_OPT_TCCMOD_MASK));
            paramSet.opt |= (1 << CSL_EDMA3CC_OPT_TCCMOD_SHIFT);

            if (IOM_COMPLETED == status)
            {
                /* Now, write the PaRAM Set.                                  */
                status = EDMA3_DRV_setPaRAM(chanHandle->edmaHandle,
                                            chanHandle->pramTbl[linkCnt],
                                            &paramSet);
            }

            if (Mcbsp_MAXLINKCNT - 1 > linkCnt )
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

                 chanHandle->loopjobUpdatedinParamset  = TRUE;
            }
        }/*for loop for linkcount*/
    }
    return (status);
}

#else
/**
 *  \brief   This calls the registered application callback for the current
 *           receive request and processes the next request in queue.
 *           This if a mcbsp edma callback function called in edma context
 *
 *  \param   tcc        [IN]  tcc number of EDMA
 *  \param   edmaStatus [IN]  status of EDMA transaction
 *  \param   data       [IN]  paramaters sent to EDMA ISR (chan handler)
 *
 *  \return  Nothing
 *
 *  \entry   tcc         should be a valid value
 *           status      should be a valid value
 *           data        should be a non NULL and valid pointer
 *
 *  \leave   Not implemented
 */
Void Mcbsp_localEdmaCallback(Uint32 tcc, EDMA3_RM_TccStatus edmaStatus, Ptr data)
{
    Mcbsp_Object          *instHandle = NULL;
    Mcbsp_ChannelObj      *chanHandle = NULL;
    Int32                  status     = IOM_COMPLETED;
    Bool                   falseWhile = TRUE;
    Uint32                 hwiKey     = 0x00;

    do
    {
        falseWhile = FALSE;

        assert(NULL != data);
        chanHandle = (Mcbsp_ChannelObj *)data;

        if (1u == chanHandle->submitCount)
        {
            /* This is the last packet available with the driver.Hence we will*
             * stop the EDMA and then proceed to process the packet           */
            EDMA3_DRV_disableTransfer(
                chanHandle->edmaHandle,
                chanHandle->xferChan,
                EDMA3_DRV_TRIG_MODE_EVENT);

            instHandle = (Mcbsp_Object *)chanHandle->devHandle;
            assert(NULL != instHandle);

            /* close the IOP now                                              */
            if (IOM_OUTPUT == chanHandle->mode)
            {
                /* update the status of the IOP                               */
                if (EDMA3_RM_XFER_COMPLETE != edmaStatus)
                {
                    /* update the status of the IOP as error                  */
                    chanHandle->currentError = IOM_EABORT;
                }
                else
                {
                    chanHandle->currentError = IOM_COMPLETED;
                }

                SWI_post(&instHandle->swiObject);
                break;
            }

            /* Stop the McBSP hardware.                                       *
             * 1. check if the RX has enabled the  FSG and also it is running *
             * 2. Also check that the TX state machine is not running off FSG */
            if (TRUE == instHandle->rxFsgEnable)
            {
                if ((TRUE != instHandle->txFsgEnable) ||
                    (TRUE == instHandle->stopSmFsXmt))
                {
                    Mcbsp_localResetCtrl(
                        (Mcbsp_Object *)chanHandle->devHandle,
                            Mcbsp_SpcrCtrl_FSYNC_DISABLE);

                    instHandle->fsgEnabled = FALSE;
                }
            }

            Mcbsp_localResetCtrl(
                (Mcbsp_Object *)chanHandle->devHandle,
                 Mcbsp_SpcrCtrl_RX_DISABLE);

            /* receive state machine is stopped                               */
            instHandle->stopSmFsRcv = TRUE;

            /* clear the error bits that have been set                        */
            EDMA3_DRV_clearErrorBits(
                chanHandle->edmaHandle,
                chanHandle->xferChan);            
        }

        /* To remove the compiler warning                                     */
        tcc = tcc;

        if (EDMA3_RM_XFER_COMPLETE != edmaStatus)
        {
            /* clear the error bits that have been set                        */
            EDMA3_DRV_clearErrorBits(
                chanHandle->edmaHandle,
                chanHandle->xferChan);

            /* update the status of the IOP as error                          */
            chanHandle->currentError = IOM_EABORT;
            break;
        }
        else
        {
            chanHandle->currentError = IOM_COMPLETED;

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

        instHandle = (Mcbsp_Object *)chanHandle->devHandle;
        assert(NULL != instHandle);

        /* check if the STOP command is issued for the channel. if the stop   *
         * command is issued we will not load the next packet so that the     *
         * state machine is stopped                                           */
        if (((IOM_INPUT == chanHandle->mode) && (FALSE == instHandle->stopSmFsRcv)) ||
            ((IOM_OUTPUT == chanHandle->mode) && (FALSE == instHandle->stopSmFsXmt)))
        {
#ifdef BIOS_PWRM_ENABLE
            if (TRUE != instHandle->pwrmInfo.ioSuspend)
            {
#endif /* #ifdef BIOS_PWRM_ENABLE  */
                /* now check if any other packet is available and load that   */
                if ((Mcbsp_STATUS_VALID == (mcbspIsValidPacket(chanHandle))))
                {
                    /* Valid packet is found,load the packet in to link params*/
                    status = Mcbsp_localEdmaProcessPkt(
                                 chanHandle,
                                 chanHandle->dataPacket);

                    if (IOM_COMPLETED != status)
                    {
                        Mcbsp_localCancelAndAbortAllIo(chanHandle);
                    }
                }
#ifdef BIOS_PWRM_ENABLE
            }
#endif /* #ifdef BIOS_PWRM_ENABLE  */
        }

        /* complete the IO pkt processing by calling the callback function    */
        chanHandle->isTempPacketValid = TRUE;
        Mcbsp_localCompleteCurrentIo(chanHandle);

#ifdef BIOS_PWRM_ENABLE
        /* check if the driver is waiting to go to sleep or process DVFS event*
         * (only if the application has requested PWRM support)               */
        if (TRUE == instHandle->pscPwrmEnable)
        {
            Mcbsp_localHandlePwrmEvent(instHandle,chanHandle);
        }
#endif /* #ifdef BIOS_PWRM_ENABLE  */
    }while (falseWhile);
}

/**
 *  \brief  This function updates the link param set accordingly with data or
 *          loopjobbuffer. This uses properly maintained index to identify to
 *          what param set the info has to be updated.
 *
 *  \param  chanHandle  [IN]  Handle to the mcbsp channel
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
Int32 Mcbsp_localUpdtDtPktToLnkPrms(Mcbsp_ChannelObj *chanHandle,
                                    IOM_Packet       *const ioPacket)
{
    Mcbsp_Object         *instHandle  = NULL;
    EDMA3_DRV_PaRAMRegs   pramPtr     = {0};
    Bool                  falsewhile  = TRUE;
    Int32                 status      = IOM_COMPLETED;
    EDMA3_DRV_SyncType    syncType;

    do
    {
        falsewhile = FALSE;
        assert((NULL != chanHandle) && (NULL != chanHandle->devHandle));

        instHandle = (Mcbsp_Object *)chanHandle->devHandle;
        assert(NULL != instHandle);

        /* if at all this is the first packet we will update the main params  *
         * else we will update the link params                                */
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

        if (IOM_COMPLETED != status )
        {
            status = IOM_EBADARGS;
            break;
        }

        if (NULL != ioPacket)
        {
            if (IOM_INPUT == chanHandle->mode)
            {
                if (TRUE == chanHandle->enableHwFifo)
                {
                    pramPtr.srcAddr  = (Uint32)(instHandle->hwInfo.dataAddress);
                }
                else
                {
                    pramPtr.srcAddr = (Uint32)&(instHandle->hwInfo.regs->DRR);
                }

                pramPtr.destAddr = (Uint32)ioPacket->addr;
                Mcbsp_localGetIndicesSyncType(
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
                if (TRUE == chanHandle->enableHwFifo)
                {
                    pramPtr.destAddr = (Uint32)(instHandle->hwInfo.dataAddress);
                }
                else
                {
                    pramPtr.destAddr = (Uint32)&(instHandle->hwInfo.regs->DXR);
                }

                if (TRUE == chanHandle->bMuteON)
                {
                    /* Mute is set hence we need to switch to the mute buffer */
                    pramPtr.srcAddr =
                        (Uint32)&Mcbsp_muteBuf[instHandle->instNum].scratchBuffer;
                    Mcbsp_localGetIndicesSyncType(
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
                    pramPtr.srcAddr = (Uint32)ioPacket->addr;
                    Mcbsp_localGetIndicesSyncType(
                        chanHandle,
                        &(pramPtr.srcBIdx),
                        &(pramPtr.srcCIdx),
                        &(pramPtr.aCnt),
                        &(pramPtr.bCnt),
                        &(pramPtr.cCnt),
                        &syncType,
                        FALSE);
                }
            }

            /* Here we are enabling the interrupt for this param set info     *
             * we do it here in this way because using int enable API after   *
             * calling EDMA3_DRV_setPaRAM, may fail our intention. This is    *
             * because immediately after the set param the paramset might be  *
             * copied to linked param and will make the consecutive int enable*
             * (set opt field) api useless for our purpose                    */
            pramPtr.opt |= (0x01u<<20u);

            /* early completion interrupt is enabled                          */
            pramPtr.opt &= (~(CSL_EDMA3CC_OPT_TCCMOD_MASK));
            pramPtr.opt |= (1 << CSL_EDMA3CC_OPT_TCCMOD_SHIFT);

            /* set the link address as 0xFFFF                                 */
            pramPtr.linkAddr = 0xFFFFu;

        }
        else
        {
            status = IOM_EBADARGS;
            break;
        }

        if (1u == chanHandle->submitCount)
        {
            /* Configuring bCntReload                                         */
            pramPtr.bCntReload = pramPtr.bCnt;

            /* Src & Dest are in INCR modes                                   */
            pramPtr.opt &= 0xFFFFFFFCu;

            /* FIFO width is 8 bit                                            */
            pramPtr.opt &= 0xFFFFF8FFu;

            /* Set EDMA3_DRV_OPT_FIELD_TCINTEN to TRUE                        */
            pramPtr.opt |= (CSL_EDMA3CC_OPT_TCINTEN_ENABLE
                            << CSL_EDMA3CC_OPT_TCINTEN_SHIFT);

            /* Program the TCC                                                */
            pramPtr.opt |= ((chanHandle->tcc << CSL_EDMA3CC_OPT_TCC_SHIFT)
                              & CSL_EDMA3CC_OPT_TCC_MASK);

            /* EDMA3_DRV_SYNC_AB                                              */
            pramPtr.opt &= (~(CSL_EDMA3CC_OPT_SYNCDIM_MASK));
            pramPtr.opt |= (syncType << CSL_EDMA3CC_OPT_SYNCDIM_SHIFT);

            /* set the link address as 0xFFFF                                 */
            pramPtr.linkAddr = 0xFFFFu;

            /* early completion interrupt is enabled                          */
            pramPtr.opt &= (~(CSL_EDMA3CC_OPT_TCCMOD_MASK));
            pramPtr.opt |= (1 << CSL_EDMA3CC_OPT_TCCMOD_SHIFT);

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

        if (IOM_COMPLETED != status)
        {
            status = IOM_EBADARGS;
            break;
        }
    }while(falsewhile);
    return status;
}

/**
 * \brief   setup the EDMA channel for the specific channel requested.
 *
 *          This function configures the McBSP synchonized EDMA channel PaRAM
 *          entries it also configures the various other parameters like
 *          source and destination address / modes
 *
 * \param   chanHandle   [IN]   Channel handle
 *
 * \return  IOM_COMPLETED  if setup successful
 *          Appropriate IOM error code if configuration has any error
 */
Int32 Mcbsp_localEdmaChanPaRAMsetup(Mcbsp_ChannelObj *chanHandle)
{
    Uint32               linkCnt    = 0;
    Mcbsp_Object        *instHandle = NULL;
    EDMA3_DRV_PaRAMRegs  paramSet   = {0,0,0,0,0,0,0,0,0,0,0,0};
    Int32                status     = IOM_COMPLETED;
    EDMA3_DRV_SyncType   tempSyncType;

    assert(NULL != chanHandle);
    assert(NULL != chanHandle->devHandle);

    instHandle = chanHandle->devHandle;
    assert(NULL != instHandle);

    /* Configure the McBSP synchonized EDMA channel PaRAM entries             *
     * Configure source and destination address / modes                       *
     * first - main xfer channel                                              */

    /* following is the explanation behind the EDMA configuration             *
     *
     *  1) We will have n arrays of data (now it is loopjob buffer each of    *
     *     length x bytes)                                                    *
     *  2) for each sync event we have to fetch the data from each array      *
     *     and write to each serilaiser                                       *
     *  3) and the no. bytes transferred in this case from each array will    *
     *     be roundedwordwidth/4                                              *
     *  4) as the loopjob buffer is continous, we configure the counts as     *
     *     follows                                                            *
     *     a) consider there are arrays of length roundedwordwidth/8(aCnt)    *
     *     b) these arrays are located apart lengthofloopjob size (bindex)    *
     *     c) there are 'no.of.slots' no. of such arrays  (bCnt)              *
     *     d) (a) and (b) constitute one frame of EDMA transfer and it        *
     *        happens for one sync event in AB sync mode                      *
     *     e) such frames have to be sent till we have buffer - ie            *
     *        lengthofloopjob / (roundedwordwidth/8 )  and this               *
     *        constitutes  cCnt                                               *
     *     f) address of buffer (rather offset from start of previous         *
     *         frame) is roundedwordwidth/8 and this forms cIndex             *
     *  Please take some time to visualise the setup mentioned above to       *
     *  check with EDMA configuration The same kind of setup will be used     *
     *  for data transfers also and the change will be buffer pointer         *
     *  and length of data buffer.                                            */

    /* Get the PaRAM set for default parameters                               */
    EDMA3_DRV_getPaRAM(chanHandle->edmaHandle,chanHandle->xferChan,&paramSet);

    if (IOM_INPUT == chanHandle->mode)
    {
        /* Configuring Source and Dest addresses                              */
        if (TRUE == chanHandle->enableHwFifo)
        {
            paramSet.srcAddr = (Uint32)(instHandle->hwInfo.dataAddress);
        }
        else
        {
            paramSet.srcAddr = (Uint32)&(instHandle->hwInfo.regs->DRR);
        }

        /* Configuring Src and Dest B&C Indexes                               */
        paramSet.srcBIdx  = 0;
        paramSet.srcCIdx  = 0;

        if (IOM_COMPLETED != Mcbsp_localGetIndicesSyncType(
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
        /* Configuring Source and Dest addresses                              */
        if (TRUE == chanHandle->enableHwFifo)
        {
            paramSet.destAddr = (Uint32)(instHandle->hwInfo.dataAddress);
        }
        else
        {
            paramSet.destAddr = (Uint32)&(instHandle->hwInfo.regs->DXR);
        }

        /* Configuring Src and Dest B&C Indexes                               */
        paramSet.destBIdx = 0;
        paramSet.destCIdx = 0;

        if (IOM_COMPLETED != Mcbsp_localGetIndicesSyncType(
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

    /* Configuring bCntReload                                                 */
    paramSet.bCntReload = paramSet.bCnt;

    /* Src & Dest are in INCR modes                                           */
    paramSet.opt &= 0xFFFFFFFCu;

    /* FIFO width is 8 bit                                                    */
    paramSet.opt &= 0xFFFFF8FFu;

    /* Set EDMA3_DRV_OPT_FIELD_TCINTEN to FALSE                               */
    paramSet.opt &= (~(CSL_EDMA3CC_OPT_ITCINTEN_MASK));

    paramSet.opt &= (~(CSL_EDMA3CC_OPT_TCINTEN_MASK));
    paramSet.opt |= (CSL_EDMA3CC_OPT_TCINTEN_DISABLE
                        << CSL_EDMA3CC_OPT_TCINTEN_SHIFT);

    /* Program the TCC                                                        */
    paramSet.opt |= ((chanHandle->tcc << CSL_EDMA3CC_OPT_TCC_SHIFT)
                      & CSL_EDMA3CC_OPT_TCC_MASK);

    /* EDMA3_DRV_SYNC_AB                                                      */
    paramSet.opt &= (~(CSL_EDMA3CC_OPT_SYNCDIM_MASK));
    paramSet.opt |= (tempSyncType << CSL_EDMA3CC_OPT_SYNCDIM_SHIFT);

    /* early completion interrupt is enabled                                  */
    paramSet.opt &= (~(CSL_EDMA3CC_OPT_TCCMOD_MASK));
    paramSet.opt |= (1 << CSL_EDMA3CC_OPT_TCCMOD_SHIFT);

    /* Now, write the PaRAM Set.                                              */
    status = EDMA3_DRV_setPaRAM(
                  chanHandle->edmaHandle,
                  chanHandle->xferChan,
                  &paramSet);

    if (IOM_COMPLETED == status)
    {
        /* Configure the link PaRAMs with the appropriate parameters Though   *
         * we configure all the link paramsets, we will be using only one of  *
         * them to link with main xfer channel                                */
        for (linkCnt = 0; linkCnt < Mcbsp_MAXLINKCNT; linkCnt++)
        {
            /* Get the PaRAM set for default parameters                       */
            EDMA3_DRV_getPaRAM (chanHandle->edmaHandle,
                                chanHandle->pramTbl[linkCnt],
                                &paramSet);

            if (IOM_INPUT == chanHandle->mode)
            {
                /* Configuring Source and Dest addresses                      */
                if (TRUE == chanHandle->enableHwFifo)
                {
                    paramSet.srcAddr = (Uint32)(instHandle->hwInfo.dataAddress);
                }
                else
                {
                    paramSet.srcAddr = (Uint32)&(instHandle->hwInfo.regs->DRR);
                }

                /* Configuring Src and Dest B&C Indexes                       */
                paramSet.srcBIdx  = 0;
                paramSet.srcCIdx  = 0;

                if (IOM_COMPLETED != Mcbsp_localGetIndicesSyncType(
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
                if (TRUE == chanHandle->enableHwFifo)
                {
                    paramSet.destAddr = (Uint32)(instHandle->hwInfo.dataAddress);
                }
                else
                {
                    paramSet.destAddr = (Uint32)&(instHandle->hwInfo.regs->DXR);
                }

                paramSet.destBIdx  = 0;
                paramSet.destCIdx  = 0;

                if (IOM_COMPLETED != Mcbsp_localGetIndicesSyncType(
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
            paramSet.opt &= (~(CSL_EDMA3CC_OPT_SYNCDIM_MASK));
            paramSet.opt |= (tempSyncType << CSL_EDMA3CC_OPT_SYNCDIM_SHIFT);

            /* Set EDMA3_DRV_OPT_FIELD_TCINTEN to FALSE                       */
            paramSet.opt |= (CSL_EDMA3CC_OPT_TCINTEN_DISABLE
                                << CSL_EDMA3CC_OPT_TCINTEN_SHIFT);

            /* Program the TCC                                                */
            paramSet.opt |= ((chanHandle->tcc << CSL_EDMA3CC_OPT_TCC_SHIFT)
                              & CSL_EDMA3CC_OPT_TCC_MASK);

            /* early completion interrupt                                     */
            paramSet.opt &= (~(CSL_EDMA3CC_OPT_TCCMOD_MASK));
            paramSet.opt |= (1 << CSL_EDMA3CC_OPT_TCCMOD_SHIFT);

            if (IOM_COMPLETED == status)
            {
                /* Now, write the PaRAM Set.                                  */
                status = EDMA3_DRV_setPaRAM(chanHandle->edmaHandle,
                                            chanHandle->pramTbl[linkCnt],
                                            &paramSet);
            }
        }/*for loop for linkcount*/
    }
    return (status);
}

#if defined(BIOS_PWRM_ENABLE) && !defined(Mcbsp_LOOPJOB_ENABLE)
/**
 * \brief   This function handles the power events related to the McBSP.it
 *          reduces the dependency count and stops the further IO from being
 *          programmed.
 *
 * \param   instHandle [IN] Handle to the device instance.
 * \param   chanHandle [IN] Handle to the channel
 *
 * \return  None
 */
Void Mcbsp_localHandlePwrmEvent(Mcbsp_Object     *instHandle,
                                Mcbsp_ChannelObj *chanHandle)
{
    Int32   retVal = IOM_COMPLETED;
    Uint32  count  = 0x00;

    assert(NULL != instHandle);
    assert(NULL != chanHandle);

    /* check if the driver is waiting to go to sleep                          */
    if ((TRUE == instHandle->pscPwrmEnable) &&
        (TRUE == instHandle->pwrmInfo.ioSuspend))
    {
        /* Set the driver state to PWRM suspend so that no more IO can be     *
         * programmed                                                         */
        instHandle->devState = Mcbsp_DriverState_PWRM_SUSPEND;

        if (TRUE == QUE_empty(&chanHandle->queueFloatingList))
        {
            /* stop the current channel IO processing                         */
            /* stop the EDMA and then proceed to process the packet           */
            EDMA3_DRV_disableTransfer(
                chanHandle->edmaHandle,
                chanHandle->xferChan,
                EDMA3_DRV_TRIG_MODE_EVENT);
        
            if (IOM_OUTPUT == chanHandle->mode)
            {
                /* Stop the McBSP hardware.                                   * 
                 * 1. check if the RX has enabled the  FSG ,also it is running*
                 * 2. Also check that the TX state M/C is not running off FSG */
                if (TRUE == instHandle->txFsgEnable)
                {
                    if ((TRUE != instHandle->rxFsgEnable) ||
                        (TRUE == instHandle->stopSmFsRcv))
                    {
                        Mcbsp_localResetCtrl(
                            (Mcbsp_Object *)chanHandle->devHandle,
                                Mcbsp_SpcrCtrl_FSYNC_DISABLE);
            
                        instHandle->fsgEnabled = FALSE;
                    }
                }
            
                Mcbsp_localResetCtrl(
                    (Mcbsp_Object *)chanHandle->devHandle,
                     Mcbsp_SpcrCtrl_TX_DISABLE);
                
                /* receive state machine is stopped                           */
                instHandle->stopSmFsXmt = TRUE;

            }
            else
            {
                if (TRUE == instHandle->rxFsgEnable)
                {
                    if ((TRUE != instHandle->txFsgEnable) ||
                        (TRUE == instHandle->stopSmFsXmt))
                    {
                        Mcbsp_localResetCtrl(
                            (Mcbsp_Object *)chanHandle->devHandle,
                                Mcbsp_SpcrCtrl_FSYNC_DISABLE);
            
                        instHandle->fsgEnabled = FALSE;
                    }
                }
            
                Mcbsp_localResetCtrl(
                    (Mcbsp_Object *)chanHandle->devHandle,
                     Mcbsp_SpcrCtrl_RX_DISABLE);
                
                /* receive state machine is stopped                           */
                instHandle->stopSmFsRcv = TRUE;
            }            

            /* clear the error bits that have been set                        */
            EDMA3_DRV_clearErrorBits(
                chanHandle->edmaHandle,
                chanHandle->xferChan);
        }

        if ((TRUE == QUE_empty(&instHandle->xmtObj.queueFloatingList)) &&
            (TRUE == QUE_empty(&instHandle->rcvObj.queueFloatingList)))
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
#endif /* #ifdef Mcbsp_LOOPJOB_ENABLE */

/**
 * \brief  This checks is the next request in queue is data request. If this is
 *         an abort request, it calls the appropriate function to deal
 *         with it.
 *
 * \param  chanHandle   [IN]  Handle to the Mcbsp Channel
 *
 * \return Mcbsp_STATUS_INVALID,if no outstanding requests
 *         Mcbsp_STATUS_VALID, if pending packet is there
 *
 */
static Int32 mcbspIsValidPacket(Mcbsp_ChannelObj *chanHandle)
{
    Int32          status     = Mcbsp_STATUS_INVALID;

    assert(NULL != chanHandle);
    assert(NULL != chanHandle->devHandle);

    /* check if the queue has any request to be processed                     */
    if (FALSE == QUE_empty(&(chanHandle->queuePendingList)))
    {
        chanHandle->dataPacket = QUE_get(&(chanHandle->queuePendingList));

        if (NULL != chanHandle->dataPacket)
        {
            /* we have a valid packet to process next                         */
            chanHandle->currentDataSize =
                (Uint16)(chanHandle->dataPacket->size);
            chanHandle->userDataBufferSize =
                (Uint32)(chanHandle->dataPacket->size);

            /* Additional to assigning the important parameters as above      *
             * for DMA mode , we will have max 2 packets floating and hence   *
             * we use a activequue to manage this                             */
            QUE_put(&(chanHandle->queueFloatingList),chanHandle->dataPacket);

            /* Set the status as valid                                        */
            status = Mcbsp_STATUS_VALID;
        }
    }
    else
    {
        /* we dont have any packet in queue stop xmt/rcv SM before giving     *
         * error                                                              */
        chanHandle->dataPacket = NULL;

        /* Set the status as invalid to program the loopjob buffer            */
        status = Mcbsp_STATUS_INVALID;
    }
    return status;
}

/**
 * \brief   Process the packet and update the EDMA paramset
 *
 *          For the transmit operation, the eDMA channel's destination
 *          port is tied to the MCBSP DAT port. In case of receive, the eDMA
 *          channel's source port is tied to the MCBSP DAT port. The source
 *          address for transmit eDMA channel and the destination address for
 *          the receive eDMA channel are set here.
 *
 * \param  chanHandle [IN]    Pointer to channel
 * \param  ioPacket   [IN]    Pointer to request to be processed
 *
 * \return  IOM_COMPLETED, if the address is set correctly
 *          IOM_EBADIO otherwise
 */
Int32 Mcbsp_localEdmaProcessPkt(Mcbsp_ChannelObj *chanHandle,
                                IOM_Packet       *ioPacket)
{
    Int32   status = IOM_COMPLETED;

    assert((NULL != chanHandle) && (NULL != ioPacket));

    /* we are going to update the paramset from EDMA callback context. That   *
     * means the one of the param sets are having audio data just completed   *
     * data request and one having currently loaded (active) data request     *
     * Current index is pointing only latest paramset (that is active)  so    *
     * move the index                                                         */
    Mcbsp_localGetNextIndex(&chanHandle->nextLinkParamSetToBeUpdated);

    /* update the edma paramset with the packet details                       */
    status = Mcbsp_localUpdtDtPktToLnkPrms(chanHandle, ioPacket);

    return (status);
}

/**
 *  \brief  Mcbsp_localGetIndicesSyncType
 *
 *  This function computes the parameters requied to configure EDMA 3 based
 *  on the buffer format seleted while creating the driver.
 *
 *  Entry Criteria : This function expects channel variables such as no of slots
 *                   length of buffers is updated.
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
 *  CAUTION:  This function could be called with pointer pointing to EDMA 3
 *            paramset pointer (avoiding couple of variables). Care should be
 *            take to ensure that data types used in EDMA 3 paramset is
 *            consistent with EDMA 3 defined data types.
 *
 *  \return   IOM_EBADARGS on invalid buffer format, else IOM_COMPLETED
 */
Int32 Mcbsp_localGetIndicesSyncType(Mcbsp_ChannelObj   *chanHandle,
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

    /* Get the indices                                                        */
    switch (chanHandle->dataFormat)
    {
        case Mcbsp_BufferFormat_1SLOT:
        case Mcbsp_BufferFormat_MULTISLOT_INTERLEAVED:
            *aCnt = chanHandle->roundedWordWidth;
            *bCnt = 1u;
            *cCnt = (Uint16)(tempSize/(*aCnt));
            *bIndex = (Int16)tempSize;
            *cIndex = (Int16)(*aCnt);
            *syncType = EDMA3_DRV_SYNC_AB;
            break;
        case Mcbsp_BufferFormat_MULTISLOT_NON_INTERLEAVED:
            *aCnt = chanHandle->roundedWordWidth;
            *bCnt = (Uint16)(chanHandle->chanConfig.frmLen1
                        + chanHandle->chanConfig.frmLen2);
            /* temp Size is always a multiple of the acnt hence the division  *
             * result will always be an integer                               */
            *cCnt = (Uint16)(tempSize /((*aCnt) * (*bCnt)));
            /* temp Size is always a multiple of the bcnt when using the      *
             * multiple slots hence the result will be always an integer      */
            *bIndex = (Int16)(tempSize/(*bCnt));
            *cIndex = (Int16)((*aCnt) - (((*bCnt)- 1u) * (tempSize/(*bCnt))));
            *syncType = EDMA3_DRV_SYNC_A;
            break;
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

#ifdef Mcbsp_SPI_ENABLE
/*
 * \brief   setup the EDMA channel for the Mcbsp in SPI mode
 *
 *          This function configures the McBSP synchonized EDMA channel PaRAM
 *          entries it also configures the various other parameters like
 *          source and destination address / modes. This function is used when
 *          Mcbsp is configured to work in SPI mode.
 *
 * \param   chanHandle   [IN]   Channel handle
 *
 * \return  IOM_COMPLETED  if setup successful
 *          Appropriate IOM error code if configuration has any error
 */
Int32 Mcbsp_localSetupSpiEdmaDuringOpen(Mcbsp_Object     *instHandle,
                                        Mcbsp_ChannelObj *chanHandle)
{
    Int32      status = IOM_COMPLETED;

    assert((NULL != chanHandle) && (NULL != instHandle));

    status = EDMA3_DRV_requestChannel(
                 chanHandle->edmaHandle,
                 &(instHandle->hwInfo.edmaTxEventNum),
                 &(instHandle->hwInfo.edmaTxEventNum),
                 (EDMA3_RM_EventQueue)0,
                 &mcbspSpiEdmaTxCallback,
                 chanHandle);

    if (IOM_COMPLETED != status)
    {
        status = IOM_EBADARGS;
    }
    else
    {
        status = EDMA3_DRV_requestChannel(
                     chanHandle->edmaHandle,
                     &(instHandle->hwInfo.edmaRxEventNum),
                     &(instHandle->hwInfo.edmaRxEventNum),
                     (EDMA3_RM_EventQueue)0,
                     &mcbspSpiEdmaRxCallback,
                     chanHandle);

        if (IOM_COMPLETED != status)
        {
            EDMA3_DRV_freeChannel(
                chanHandle->edmaHandle,
                instHandle->hwInfo.edmaTxEventNum);
            status = IOM_EBADARGS;
        }
    }
    return status;
}

/*
 * \brief   EDMA callback for the TX event.
 *
 *          This function is the EDMA callback for the SPI TX event handling.
 *
 * \param   tcc     [IN]   tcc number of EDMA
 * \param   status  [IN]   status of EDMA
 * \param   data    [IN]   paramaters send to EDMA ISR (chan handler)
 *
 * \return  None
 */
static Void mcbspSpiEdmaTxCallback(Uint32              tcc,
                                   EDMA3_RM_TccStatus  status,
                                   Ptr                 data)
{
    /*  To remove warning, kept for future usage                              */
    tcc = tcc;
    status = status;

    if (NULL != data)
    {
        data = data;
    }
}

/*
 * \brief   EDMA callback for the RX event.
 *
 *          This function is the EDMA callback for the SPI RX event handling.
 *
 * \param   tcc        [IN]   tcc number of EDMA
 * \param   edmaStatus [IN]   status of EDMA
 * \param   data       [IN]   paramaters send to EDMA ISR (chan handler)
 *
 * \return  None
 */
static Void mcbspSpiEdmaRxCallback(Uint32              tcc,
                                   EDMA3_RM_TccStatus  edmaStatus,
                                   Ptr                 data)
{
    Mcbsp_Object       *instHandle = NULL;
    Mcbsp_ChannelObj   *chanHandle = NULL;

    chanHandle = (Mcbsp_ChannelObj *)data;

    assert(NULL != chanHandle);
    assert(NULL != chanHandle->devHandle);

    instHandle = (Mcbsp_Object *)chanHandle->devHandle;

    /* Disable current EDMA transfer                                          */
    EDMA3_DRV_disableTransfer (
        chanHandle->edmaHandle,
        instHandle->hwInfo.edmaRxEventNum,
        EDMA3_DRV_TRIG_MODE_EVENT);
    /* Disable current EDMA transfer                                          */
    EDMA3_DRV_disableTransfer (
        chanHandle->edmaHandle,
        instHandle->hwInfo.edmaTxEventNum,
        EDMA3_DRV_TRIG_MODE_EVENT);

    if (TRUE != QUE_empty(&(chanHandle->queueFloatingList)))
    {
        /* should always have a packet present because this callback is       *
         * due to that packet                                                 */

        /* Get the packet from the top of the queue (atomic operation)        */
        chanHandle->dataPacket = QUE_get(&(chanHandle->queueFloatingList));

        if (EDMA3_RM_XFER_COMPLETE != edmaStatus)
        {
            /* set the status of the packet to error                          */
            chanHandle->dataPacket->status = IOM_EBADIO;

            /* There is a data transfer error in EDMA transfer.               */
            EDMA3_DRV_clearErrorBits(
                chanHandle->edmaHandle,
                instHandle->hwInfo.edmaRxEventNum);

            EDMA3_DRV_clearErrorBits(
                chanHandle->edmaHandle,
                instHandle->hwInfo.edmaTxEventNum);
        }
        else
        {
            chanHandle->dataPacket->status = IOM_COMPLETED;
        }

        chanHandle->submitCount--;
        Mcbsp_localCompleteCurrentIo(chanHandle);
    }

    if (FALSE == QUE_empty(&(chanHandle->queuePendingList)))
    {
        /* There are some packets still to be processed                       */
        chanHandle->dataPacket = QUE_get(&(chanHandle->queuePendingList));

        if (NULL != chanHandle->dataPacket)
        {
            chanHandle->rxBytesIndex = 0;
            chanHandle->txBytesIndex = 0;

            /* Set Edma params                                                */
            Mcbsp_localSpiEdmaChanPaRAMsetup(chanHandle);

            /* Start edma                                                     */
            EDMA3_DRV_enableTransfer(
                chanHandle->edmaHandle,
                instHandle->hwInfo.edmaTxEventNum,
                EDMA3_DRV_TRIG_MODE_EVENT);

            EDMA3_DRV_enableTransfer(
                chanHandle->edmaHandle,
                instHandle->hwInfo.edmaRxEventNum,
                EDMA3_DRV_TRIG_MODE_EVENT);
        }
    }
    else
    {
        chanHandle->dataPacket  = NULL;
        chanHandle->submitCount = 0;
        chanHandle->rxBytesIndex = 0;
        chanHandle->txBytesIndex = 0;

        /* disable the TX and RX state machines                               */
        Mcbsp_localResetCtrl(instHandle,
            (Mcbsp_SpcrCtrl_RX_DISABLE | Mcbsp_SpcrCtrl_TX_DISABLE));
    }
}

/*
 * \brief   setup the EDMA channel for the specific channel requested.
 *
 *          This function configures the McBSP synchonized EDMA channel PaRAM
 *          entries it also configures the various other parameters like
 *          source and destination address / modes. This is for the Mcbsp
 *          operated in SPI mode.
 *
 * \param   chanHandle   [IN]   Channel handle
 *
 * \return  IOM_COMPLETED  if setup successful
 *          Appropriate IOM error code if configuration has any error
 */
Int32 Mcbsp_localSpiEdmaChanPaRAMsetup(Mcbsp_ChannelObj *chanHandle)
{
    Mcbsp_Object         *instHandle  = NULL;
    EDMA3_DRV_PaRAMRegs   paramSet    = {0};
    Uint8                *txBufAddr   = NULL;
    Uint8                *rxBufAddr   = NULL;
    Bool                  txAddrMode  = FALSE;
    Bool                  rxAddrMode  = FALSE;
    Uint32                status      = IOM_COMPLETED;

    assert(NULL != chanHandle);
    assert(NULL != chanHandle->dataPacket);
    assert(NULL != chanHandle->devHandle);

    instHandle = (Mcbsp_Object *)chanHandle->devHandle;

    txBufAddr = (((Mcbsp_TransParam *)chanHandle->dataPacket->addr))->txBufAddr;
    rxBufAddr = (((Mcbsp_TransParam *)chanHandle->dataPacket->addr))->rxBufAddr;

    assert((NULL != rxBufAddr) && (NULL != txBufAddr));

    if (NULL == rxBufAddr)
    {
        rxBufAddr = &Mcbsp_SpiTransceiveBuff[0];
        rxAddrMode = TRUE;
    }

    if (NULL == txBufAddr)
    {
        txBufAddr = &Mcbsp_SpiTransceiveBuff[0];
        txAddrMode = TRUE;
    }

    assert((0 == (Uint32)txBufAddr % 32) && (0 == (Uint32)rxBufAddr % 32));


    /* Cleans and invalidate the D Cache in MVA way                           */
    BCACHE_wbInv(rxBufAddr,chanHandle->dataPacket->size,TRUE);

    /* clean the D Cache in MVA way                                           */
    BCACHE_wb(txBufAddr,chanHandle->dataPacket->size,TRUE);

    /* Get current info from the identified param we would like to change     *
     * only certain parameters.update both the paramsets                      */
    status = EDMA3_DRV_getPaRAM(
                chanHandle->edmaHandle,
                instHandle->hwInfo.edmaRxEventNum,
                &paramSet);

    if (IOM_COMPLETED == status)
    {
        /* Configuring Source and Dest addresses                              */
        if (TRUE == chanHandle->enableHwFifo)
        {
            paramSet.srcAddr = (Uint32)(instHandle->hwInfo.dataAddress);
        }
        else
        {
            paramSet.srcAddr = (Uint32)&(instHandle->hwInfo.regs->DRR);
        }

        paramSet.destAddr = (unsigned int)(rxBufAddr);

        paramSet.aCnt = chanHandle->roundedWordWidth;
        paramSet.bCnt = 1u;
        paramSet.cCnt = (Uint16)(chanHandle->dataPacket->size/(paramSet.aCnt));
        paramSet.destBIdx = (Int16)chanHandle->dataPacket->size;

        if (TRUE == rxAddrMode)
        {
            paramSet.destCIdx = 0;
        }
        else
        {
            paramSet.destCIdx = (Int16)(paramSet.aCnt);
        }

        /* Configuring Src and Dest B&C Indexes                               */
        paramSet.srcBIdx  = 0;
        paramSet.srcCIdx  = 0;

        /* Configuring bCntReload                                             */
        paramSet.bCntReload = paramSet.bCnt;

        /* Src & Dest are in INCR modes                                       */
        paramSet.opt &= 0xFFFFFFFCu;

        /* FIFO width is 8 bit                                                */
        paramSet.opt &= 0xFFFFF8FFu;

        /* Set EDMA3_DRV_OPT_FIELD_TCINTEN to FALSE                           */
        paramSet.opt |= (CSL_EDMA3CC_OPT_TCINTEN_ENABLE
                            << CSL_EDMA3CC_OPT_TCINTEN_SHIFT);

        /* Program the TCC                                                    */
        paramSet.opt |= ((instHandle->hwInfo.edmaRxEventNum
                         << CSL_EDMA3CC_OPT_TCC_SHIFT)
                            & CSL_EDMA3CC_OPT_TCC_MASK);

        /* EDMA3_DRV_SYNC_AB                                                  */
        paramSet.opt &= (~(CSL_EDMA3CC_OPT_SYNCDIM_MASK));
        paramSet.opt |= (EDMA3_DRV_SYNC_AB << CSL_EDMA3CC_OPT_SYNCDIM_SHIFT);

        /* Now, write the PaRAM Set.                                          */
        status = EDMA3_DRV_setPaRAM(
                     chanHandle->edmaHandle,
                     instHandle->hwInfo.edmaRxEventNum,
                     &paramSet);
    }

    if (IOM_COMPLETED == status)
    {
        /* Get current info from the identified param we would like to change *
         * only certain parameters.update both the paramsets                  */
        status = EDMA3_DRV_getPaRAM(
                     chanHandle->edmaHandle,
                     instHandle->hwInfo.edmaTxEventNum,
                     &paramSet);

        if (IOM_COMPLETED == status)
        {
            if (TRUE == chanHandle->enableHwFifo)
            {
                paramSet.destAddr = (Uint32)(instHandle->hwInfo.dataAddress);
            }
            else
            {
                paramSet.destAddr = (Uint32)&(instHandle->hwInfo.regs->DXR);
            }

            /* Configuring Source and Dest addresses                          */
            paramSet.srcAddr  = (unsigned int)(txBufAddr);

            paramSet.aCnt = chanHandle->roundedWordWidth;
            paramSet.bCnt = 1u;
            paramSet.cCnt = (Uint16)(chanHandle->dataPacket->size/(paramSet.aCnt));
            paramSet.srcBIdx = (Int16)chanHandle->dataPacket->size;

            /* Destination index params                                       */
            paramSet.destBIdx = 0;
            paramSet.destCIdx = 0;

            /* Configuring Src and Dest B&C Indexes                           */
            if (TRUE == txAddrMode)
            {
                paramSet.srcCIdx = 0;
            }
            else
            {
                paramSet.srcCIdx = chanHandle->roundedWordWidth;
            }

            /* Configuring bCntReload                                         */
            paramSet.bCntReload = paramSet.bCnt;

            /* Src & Dest are in INCR modes                                   */
            paramSet.opt &= 0xFFFFFFFCu;

            /* FIFO width is 8 bit                                            */
            paramSet.opt &= 0xFFFFF8FFu;

            /* Set EDMA3_DRV_OPT_FIELD_TCINTEN to FALSE                       */
            paramSet.opt |= (CSL_EDMA3CC_OPT_TCINTEN_ENABLE
                                << CSL_EDMA3CC_OPT_TCINTEN_SHIFT);

            /* Program the TCC                                                */
            paramSet.opt |= ((instHandle->hwInfo.edmaTxEventNum
                                << CSL_EDMA3CC_OPT_TCC_SHIFT)
                                    & CSL_EDMA3CC_OPT_TCC_MASK);

            /* EDMA3_DRV_SYNC_AB                                              */
            paramSet.opt &= (~(CSL_EDMA3CC_OPT_SYNCDIM_MASK));
            paramSet.opt |= (EDMA3_DRV_SYNC_AB << CSL_EDMA3CC_OPT_SYNCDIM_SHIFT);

            /* Now, write the PaRAM Set.                                      */
            status = EDMA3_DRV_setPaRAM(
                         chanHandle->edmaHandle,
                         instHandle->hwInfo.edmaTxEventNum,
                         &paramSet);
        }
    }

    if (IOM_COMPLETED == status)
    {
        Mcbsp_localResetCtrl(instHandle,
            (Mcbsp_SpcrCtrl_RX_ENABLE | Mcbsp_SpcrCtrl_TX_ENABLE));
    }

    return status;
}
#endif /* #ifdef Mcbsp_SPI_ENABLE */

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */

