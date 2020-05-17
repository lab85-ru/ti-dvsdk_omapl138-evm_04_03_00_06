/*
 * Mcasp_ioctl.c
 *
 * This file contains local functions for McASP driver which contain implemen-
 * tation for control commands specific for the McASP peripheral.
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
 *  \file   Mcasp_ioctl.c
 *
 *  \brief  This files contains the ioctl functions required for the Mcasp
 *          driver to execute the control commands.
 *
 *   (C) Copyright 2008, Texas Instruments, Inc
 *
 *
 *  \version   0.2     Added the support for the non loop job mode
 *             0.1     Newly created.
 */

/* ========================================================================== */
/*                             INCLUDE FILES                                  */
/* ========================================================================== */

#include <std.h>
#include <assert.h>
#include <iom.h>
#include <que.h>

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

#include "ti/pspiom/cslr/cslr_mcasp.h"
#include "ti/pspiom/mcasp/Mcasp.h"
#include "McaspLocal.h"

#ifdef Mcasp_LOOPJOB_ENABLED
extern far Mcasp_TempBuffer Mcasp_loopDstBuf[CSL_MCASP_PER_CNT];
extern far Mcasp_TempBuffer Mcasp_loopSrcBuf[CSL_MCASP_PER_CNT];
#endif  /* Mcasp_LOOPJOB_ENABLED */
/* ========================================================================== */
/*                          LOCAL FUNCTION PROTOTYPES                         */
/* ========================================================================== */

static Void mcaspConfigLoopBack(Mcasp_HwHandle hMcasp,Bool loopBack);

#ifdef Mcasp_LOOPJOB_ENABLED
static Void mcaspActivateRcvClkSer(Mcasp_HwHandle hMcasp);

static Void mcaspActivateXmtClkSer(Mcasp_HwHandle hMcasp);

static Void mcaspActivateClkRcvXmt(Mcasp_HwHandle hMcasp);
#endif

static Uint8 mcaspGetSmFsRcv(Mcasp_HwHandle hMcasp);

static Uint8 mcaspGetSmFsXmt(Mcasp_HwHandle hMcasp);

static Int32 mcaspSetSamplingRate(Mcasp_ChannelHandle  chanHandle,
                                  Ptr                  ioctlArg);
static Void mcaspDeviceInfoGet(Mcasp_ChannelHandle  chanHandle,
                               Ptr                  arg);

static Int32 mcaspGetHwStatus(Mcasp_HwHandle        hMcasp,
                              Mcasp_HwStatusQuery   myQuery,
                              Void                 *const response);

static Void mcaspReadXmtConfig(Mcasp_HwHandle       hMcasp,
                               Mcasp_HwSetupData   *const xmtData);

static Void mcaspReadRcvConfig(Mcasp_HwHandle       hMcasp,
                               Mcasp_HwSetupData   *const rcvData);

static Void mcaspGetSerMode(Mcasp_HwHandle        hMcasp,
                            Mcasp_SerMode        *serMode,
                            Mcasp_SerializerNum   serNum);

static Void mcaspGetSerRcvReady(Mcasp_HwHandle       hMcasp,
                                Bool                *serRcvReady,
                                Mcasp_SerializerNum  serNum);

static Void mcaspGetSerXmtReady(Mcasp_HwHandle        hMcasp,
                                Bool                 *serXmtReady,
                                Mcasp_SerializerNum   serNum);

/* ========================================================================== */
/*                           FUNCTION DEFINTIONS                              */
/* ========================================================================== */


/**
 *  \brief  This function executes the control commands passed by the IDriver,
 *          the application passes the control command required to the IDriver
 *          uisng the stream control function. The IDriver function then calls
 *          this function which then actually executes the control command.
 *
 *  \param  chanHandle  [IN]  Handle to channel
 *  \param  cmd         [IN]  Control command number
 *  \param  arg         [IN]  Arguments for control command
 *  \param  param       [IN]  user defined data structure
 *  \param  eb          [OUT] pointer to the error block
 *
 *  \return Appropriate error code in case of failure else IOM_COMPLETED
 *          is returned
 *
 * \entry   chanHandle  should be non NULL and valid pointer
 *          cmd         should be a valid value
 *          arg         arg can be NULL or any valid value
 *          param       should be a valid value
 *
 * \leave   Not implemented
 */
Int32 Mcasp_localSubmitIoctl(Mcasp_ChannelHandle      chanHandle,
                             Uns                      cmd,
                             Ptr                      arg,
                             Ptr                      param)
{
    Mcasp_Object           *instHandle    = NULL;
    Mcasp_ChannelHandle     iohandlep     = NULL;
    Int16                   serNum        = 0;
    Uint32                  tempVal       = 0;
    Uint32                  queryRes      = 0;
    Int32                   status        = IOM_COMPLETED;
    Bool                    dlbMode       = FALSE;
    Bool                    falsewhile    = TRUE;
    Mcasp_HwSetupData      *chanSetup     = NULL;
    Mcasp_HwSetup          *mcaspHwSetup  = NULL;
    Uint32                  timeout       = Mcasp_POLLED_RETRYCOUNT;
#ifdef Mcasp_LOOPJOB_ENABLED
    Mcasp_ChanParams       *params        = NULL;
    EDMA3_DRV_PaRAMRegs     pramPtr       = {0};
    EDMA3_DRV_SyncType      syncType;
    IOM_Packet             *ioPacket      = NULL;
    Int16                   linkCnt       = 0;
#endif
    /* Validate the input parameters                                          */
    assert((NULL != chanHandle) && (NULL != chanHandle->devHandle));

    instHandle = (Mcasp_Object *)chanHandle->devHandle;

    assert(NULL != instHandle);

    /* To remove the compiler warning                                         */
    param = param;

    /*--------------------MCASP IOCTL FOR HANDLING TIMEOUT--------------------*/

    if ((Mcasp_IOCTL_CHAN_TIMEDOUT == cmd) ||
         (Mcasp_IOCTL_CHAN_RESET == cmd) ||
         (IOM_CHAN_RESET == cmd) ||
         (IOM_CHAN_TIMEDOUT ==cmd))
    {
        /* To be called in case of channel timeoutand reset                   */
        status = Mcasp_localAbortReset(chanHandle);
    }
    /*---------------------------MCASP RESET CMD -----------------------------*/
    else if ((Mcasp_IOCTL_DEVICE_RESET == cmd) ||
              (IOM_DEVICE_RESET == cmd))
    {
        /* Abort all the request, give application callback and configure     *
         * EDMA in the LOOPJOB for both input and output channels             */
        Mcasp_localAbortRequests(&(instHandle->RcvObj),NULL);
        Mcasp_localAbortRequests(&(instHandle->XmtObj),NULL);
    }
    /*-------------------------MCASP DRIVER MODE------------------------------*/
    else if (Mcasp_IOCTL_QUERY_LOOPJOB_MODE == cmd)
    {
        /* Query the working mode of the driver                               */
        assert(NULL != arg);

        *(Bool *)arg = instHandle->loopJobMode;
    }
    /*---------------------MCASP SET CHANNEL SETTINGS CMD --------------------*/
    else if (Mcasp_IOCTL_CNTRL_SET_FORMAT_CHAN == cmd)
    {
        do
        {
            falsewhile = FALSE;

            if (NULL == arg)
            {
                status = IOM_EBADARGS;
                break;
            }

            /* Reprogram xmt/rcv format unit registers. State m/c         *
             * associated with channel should have been reset.            *
             * a pointer to h/w setup data would have been passed through *
             * the argument field.                                        */
            chanSetup = (Mcasp_HwSetupData *)arg;

            /* Stop the xmt/rcv state machine.                            */
            Mcasp_localSmReset(chanHandle);

            if (Mcasp_ChanMode_RCV == chanHandle->channelOpMode)
            {
                /* Check if receive state machine and frame sync are reset*/
                status = mcaspGetHwStatus(
                            &(instHandle->hwInfo),
                            Mcasp_HwStatusQuery_SM_FS_RCV,
                            &queryRes);
            }
            else
            {
                /* Check if transmit state machine & frame sync are reset */
                status = mcaspGetHwStatus(
                            &(instHandle->hwInfo),
                            Mcasp_HwStatusQuery_SM_FS_XMT,
                            &queryRes);
            }

            if (IOM_COMPLETED != status)
            {
               status = IOM_EBADMODE;
               break;
            }

            if (Mcasp_SM_RESET != queryRes)
            {
                /* The corresponding state m/c and frame sync have to be  *
                 * reset. And they are not. So return error.              */
                status = IOM_EBADARGS;
                break;
            }

            if ((Mcasp_ChanMode_XMT_DIT == chanHandle->channelOpMode)
                || (Mcasp_ChanMode_XMT_TDM == chanHandle->channelOpMode))
            {
               /* Configure transmit section                              */
                status =  Mcasp_localConfigXmtSection(
                            instHandle,
                            (Mcasp_HwSetupData *)chanSetup);
            }
            else
            {
                /* Configure receive section                              */
                status = Mcasp_localConfigRcvSection(
                            instHandle,
                            (Mcasp_HwSetupData *)chanSetup);
            }

            if (IOM_COMPLETED != status)
            {
                /* Error configuring section.Mark channel state as INVALID*/
                chanHandle->chanState = Mcasp_DriverState_CLOSED;
                break;
            }

            /* Stop the xmt/rcv state machine.                            */
            Mcasp_localSmSet(chanHandle);

        }while(falsewhile);

    }
    /*-----------------MCASP GET CHANNEL SETTINGS CMD --------------------*/
    else if (Mcasp_IOCTL_CNTRL_GET_FORMAT_CHAN == cmd)
    {
        do
        {
            falsewhile = FALSE;

            if (NULL == arg)
            {
                status = IOM_EBADARGS;
                break;
            }

            /* Read xmt/rcv format unit registers. A pointer to h/w setup *
             * data would have been passed through the argument field.    */
            chanSetup = (Mcasp_HwSetupData *)arg;

            if ((Mcasp_ChanMode_XMT_DIT == chanHandle->channelOpMode)
                || (Mcasp_ChanMode_XMT_TDM == chanHandle->channelOpMode))
            {
                /* Read transmit section infomration                      */
                mcaspReadXmtConfig(&(instHandle->hwInfo),chanSetup);
            }
            else
            {
                /* Read receive section information                       */
                mcaspReadRcvConfig(&(instHandle->hwInfo),chanSetup);
            }
        }while(falsewhile);

    }
    /*---------------------MCASP SET GBL REG CMD -------------------------*/
    else if (Mcasp_IOCTL_CNTRL_SET_GBL_REGS == cmd)
    {
        do
        {
            falsewhile = FALSE;

            if (NULL == arg)
            {
                status = IOM_EBADARGS;
                break;
            }

            /* This will program the global registers. Both channels will *
             * be reset and global registers programmed. A pointer to h/w *
             * setup would have been passed through the argument field.   */
            mcaspHwSetup = (Mcasp_HwSetup *)arg;

            if ((Mcasp_ChanMode_XMT_DIT == chanHandle->channelOpMode)
                || (Mcasp_ChanMode_XMT_TDM == chanHandle->channelOpMode))
            {
                (instHandle->hwInfo.regs)->XGBLCTL = 0;
            }
            else
            {
                (instHandle->hwInfo.regs)->RGBLCTL = 0;
            }

            Mcasp_localSetupHwInfo(&instHandle->hwInfo, mcaspHwSetup);

        }while(falsewhile);

    }
    /*-----------------------MCASP SET DIT CMD ---------------------------*/
    else if (Mcasp_IOCTL_SET_DIT_MODE == cmd)
    {
        do
        {
            falsewhile = FALSE;

            if (NULL == arg)
            {
                status = IOM_EBADARGS;
                break;
            }

            /* This will set the DIT mode                                 */
            tempVal = *(Uint32 *)arg;

            if ((Mcasp_ChanMode_XMT_DIT == instHandle->XmtObj.channelOpMode)
            || (Mcasp_ChanMode_XMT_TDM == instHandle->XmtObj.channelOpMode))
            {
                instHandle->hwInfo.regs->DITCTL |= (Uint32)tempVal;
                chanHandle->channelOpMode= Mcasp_ChanMode_XMT_DIT;
            }

        }while(falsewhile);

    }
    /*-----------------------MCASP LOOPBACK CMD  -------------------------*/
    else if (Mcasp_IOCTL_SET_DLB_MODE == cmd)
    {
        do
        {
            falsewhile = FALSE;

            /* This will program the digital loopback mode.               */
            if (NULL == arg)
            {
                status = IOM_EBADARGS;
                break;
            }

            dlbMode = *(Bool*)arg;
            iohandlep = &instHandle->RcvObj;

            if (NULL == iohandlep)
            {
                /* May lead to unexpected results.Revert with error status*/
                status = IOM_EBADMODE;
                break;
            }

            if (NULL != iohandlep->dataPacket)
            {
                /* May lead to unexpected results.Revert with error status*/
                status = IOM_EBADMODE;
                break;
            }

            queryRes = (Uint32)iohandlep->noOfSerAllocated;

            iohandlep = &instHandle->XmtObj;

            if (NULL != iohandlep->dataPacket)
            {
                /* May lead to unexpected results.Revert with error status*/
                status = IOM_EBADMODE;
                break;
            }

            if (queryRes != iohandlep->noOfSerAllocated)
            {
                /* Need equal number of serializers for DLB mode          */
                status = IOM_EBADMODE;
                break;
            }

            while (serNum < (instHandle->hwInfo.numOfSerializers - (1u)))
            {
                if (Mcasp_SerializerStatus_FREE !=
                    instHandle->serStatus[serNum])
                {
                    if (Mcasp_SerializerStatus_FREE !=
                        instHandle->serStatus[serNum+1])
                    {
                       /* Increment serializers counter                   */
                       serNum+=2;
                    }
                    else
                    {
                        /* Serializers not in sequence                    */
                        break;
                    }
                }
                else
                {
                    if (Mcasp_SerializerStatus_FREE ==
                        instHandle->serStatus[serNum+1])
                    {
                       /* Increment serializers counter                   */
                       serNum+=2;
                    }
                    else
                    {
                        /* Serializers not in sequence                    */
                        status = IOM_EBADMODE;
                        break;
                    }
                }
            }

            mcaspConfigLoopBack(&(instHandle->hwInfo), dlbMode);

            if (IOM_COMPLETED != status)
            {
                /* Error in setup!                                        */
                status = IOM_EBADIO;
                break;
            }

            Mcasp_localActivateSmFsForBoth(instHandle);

        }while (falsewhile);

    }
    /*------------------------MCASP MUTE CMD  ----------------------------*/
    else if (Mcasp_IOCTL_CNTRL_AMUTE == cmd)
    {
        do
        {
            falsewhile = FALSE;

            if (NULL == arg)
            {
                status =   IOM_EBADARGS;
                break;
            }

            tempVal = *(Uint32 *)arg;

            /* Enable or Disable AMUTE register                           */
            (instHandle->hwInfo.regs)->AMUTE = tempVal;

        }while(falsewhile);

    }
    /*----------------------MCASP IOCTL QUERY  CMD -----------------------*/
    else if (Mcasp_IOCTL_QUERY_MUTE == cmd)
    {
        do
        {
            falsewhile = FALSE;
            status = mcaspGetHwStatus(
                         &(instHandle->hwInfo),
                         Mcasp_HwStatusQuery_AMUTE,
                         &queryRes);

            /*Sending back the value of the Amute Register                */
            *(Uint32 *)arg = queryRes;

        }while (falsewhile);

    }
#ifdef Mcasp_LOOPJOB_ENABLED
    /*-------------------MCASP LOOP JOB MODIFY CMD -----------------------*/
    /* Enable / disable loopjob. If the buffer pointer is, NULL then      *
     * disable loopjob not NULL then enable the loop job and asign buffer *
     * pointer to prdbuf                                                  */
    else if (Mcasp_IOCTL_CTRL_MODIFY_LOOPJOB == cmd)
    {
        do
        {
            falsewhile = FALSE;
            params = (Mcasp_ChanParams *)arg;

            if (NULL == params)
            {
                status = IOM_EBADARGS;
                break;
            }

            if (TRUE == chanHandle->isDmaDriven)
            {
                status = EDMA3_DRV_getPaRAM(
                            chanHandle->edmaHandle,
                            chanHandle->pramTbl[ \
                        ((chanHandle->nextLinkParamSetToBeUpdated + 1)% 2)],
                            &pramPtr);

                if (IOM_COMPLETED != status)
                {
                    break;
                }

                /* Here we need to change only the buffer pointer and the *
                 * length of the array which will go into bcnt            */
                if (IOM_OUTPUT == (chanHandle->mode))
                {
                    chanHandle->loopJobBuffer  =
                         &Mcasp_loopSrcBuf[(instHandle->instNum)].scratchBuffer;

                    chanHandle->loopJobLength =
                        (chanHandle->roundedWordWidth
                            * chanHandle->noOfSlots
                            * chanHandle->noOfSerAllocated);

                    if (NULL != params->userLoopJobBuffer)
                    {
                        /* user specified loop job is loaded                  */
                        chanHandle->userLoopJob = TRUE;

                        chanHandle->loopJobBuffer =
                            params->userLoopJobBuffer;
                        chanHandle->userLoopJobLength =
                            params->userLoopJobLength;

                        status = Mcasp_localGetIndicesSyncType(
                                    chanHandle,
                                    &(pramPtr.srcBIdx),
                                    &(pramPtr.srcCIdx),
                                    &(pramPtr.aCnt),
                                    &(pramPtr.bCnt),
                                    &(pramPtr.cCnt),
                                    &syncType,
                                    TRUE);
                        pramPtr.srcAddr = (Uint32)params->userLoopJobBuffer;
                    }
                    else
                    {
                        status = IOM_EBADARGS;
                        break;
                    }
                }
                else
                {
                    chanHandle->loopJobBuffer  =
                         &Mcasp_loopSrcBuf[(instHandle->instNum)].scratchBuffer;

                    chanHandle->loopJobLength =
                        (chanHandle->roundedWordWidth
                            * chanHandle->noOfSlots
                            * chanHandle->noOfSerAllocated);

                    if (NULL != params->userLoopJobBuffer)
                    {
                        /* user specified loop job is loaded                  */
                        chanHandle->userLoopJob = TRUE;

                        chanHandle->loopJobBuffer =
                            params->userLoopJobBuffer;
                        chanHandle->userLoopJobLength =
                            params->userLoopJobLength;

                        status = Mcasp_localGetIndicesSyncType(
                                    chanHandle,
                                    &(pramPtr.destBIdx),
                                    &(pramPtr.destCIdx),
                                    &(pramPtr.aCnt),
                                    &(pramPtr.bCnt),
                                    &(pramPtr.cCnt),
                                    &syncType,
                                    TRUE);
                        pramPtr.destAddr = (Uint32)params->userLoopJobBuffer;
                    }
                    else
                    {
                        status = IOM_EBADARGS;
                        break;
                    }
                }

                /* set BUS to VBUSP i.e DMA mode                          */
                (instHandle->hwInfo.regs)->AFSRCTL =
                    ((instHandle->hwInfo.regs)->AFSRCTL
                        & ~(CSL_MCASP_RFMT_RBUSEL_MASK))
                    | (CSL_MCASP_RFMT_RBUSEL_VBUSP
                        << CSL_MCASP_RFMT_RBUSEL_SHIFT);

                status = EDMA3_DRV_setPaRAM(
                            chanHandle->edmaHandle,
                            chanHandle->pramTbl[ \
                       ((chanHandle->nextLinkParamSetToBeUpdated + 1) % 2)],
                            &pramPtr);

                if (IOM_COMPLETED != status)
                {
                    break;
                }
            }
            else
            {
                status = IOM_ENOTIMPL;
                break;
            }
        }while (falsewhile);

    }
    /*----------------------- MCASP MUTE ON CMD --------------------------*/
    /* MUTE-ON command implementation Typical implementation for Mute is  *
     * writing zero data in buffer instead of writing real data.          */
    else if (Mcasp_IOCTL_CTRL_MUTE_ON == cmd)
    {
        do
        {
            falsewhile = FALSE;

            if (IOM_OUTPUT == chanHandle->mode)
            {
                if(TRUE == chanHandle->bMuteON)
                {
                    status = IOM_EBADARGS;
                    break;
                }
                chanHandle->bMuteON = (Bool)TRUE;
            }
            else
            {
                status = IOM_ENOTIMPL;
            }
        }while (falsewhile);

    }
    /*----------------------- MCASP MUTE OFF CMD -------------------------*/
    /* Take out from the the Muted state                                  */
    else if (Mcasp_IOCTL_CTRL_MUTE_OFF == cmd)
    {
        do
        {
            falsewhile = FALSE;

            if (IOM_OUTPUT == chanHandle->mode)
            {
                /* Check if mute is not ON                                */
                if (FALSE == chanHandle->bMuteON )
                {
                    status = IOM_EBADARGS;
                    break;
                }
                chanHandle->bMuteON =(Bool)FALSE;
            } /* if only dma driven transaction */
            else
            {
                status = IOM_ENOTIMPL;
            }
        }while(falsewhile);
    }
#endif /* Mcasp_LOOPJOB_ENABLED */    
    /*--------------------------- MCASP ABORT CMD-------------------------*/
    else if (Mcasp_IOCTL_ABORT == cmd)
    {
        status = Mcasp_localAbortRequests(chanHandle, NULL);
    }
    else if (Mcasp_IOCTL_SET_SAMPLE_RATE == cmd)
    {
        status = mcaspSetSamplingRate(chanHandle,arg);
    }
    else if (Mcasp_IOCTL_GET_DEVINFO == cmd)
    {
        mcaspDeviceInfoGet(chanHandle, arg);
    }
#ifdef Mcasp_LOOPJOB_ENABLED
    /*------------------------MCASP START CMD ----------------------------*/
    else if (Mcasp_IOCTL_START_PORT == cmd)
    {
        if (IOM_INPUT == chanHandle->mode)
        {
            if (TRUE == instHandle->stopSmFsRcv)
            {
                instHandle->stopSmFsRcv = FALSE;
            }
            else
            {
                /* Start port is issued without issuing stop              */
                status = IOM_EBADARGS;
            }
        }
        else
        {
            if (TRUE == instHandle->stopSmFsXmt)
            {
                instHandle->stopSmFsXmt = FALSE;
            }
            else
            {
                /* Start port is issued without issuing stop              */
                status = IOM_EBADARGS;
            }
        }

        if ((TRUE == chanHandle->isDmaDriven)
            && (TRUE == QUE_empty(&(chanHandle->queueFloatingList))))
        {
            for (linkCnt = 0;linkCnt < Mcasp_MAXLINKCNT; linkCnt++)
            {
                if (FALSE == QUE_empty(&(chanHandle->queueReqList)))
                {
                    ioPacket = (IOM_Packet *)QUE_get(
                                    &(chanHandle->queueReqList));

                    if (NULL == ioPacket)
                    {
                        status =  IOM_EBADARGS;
                        break;
                    }

                    /* floating queue not full . Process this request.    */
                    QUE_put(&(chanHandle->queueFloatingList),(Ptr)ioPacket);

                    chanHandle->currentDataSize =(Uint16)ioPacket->size;
                    chanHandle->userDataBufferSize =
                        (Uint32)ioPacket->size;

                    /* For DMA mode we will not use chanHandle->dataPacket*
                     * to hold current packet - we use floatingqueue      */
                    chanHandle->dataPacket = NULL;

                    if ((Mcasp_MAXLINKCNT - 1u) == linkCnt)
                    {
                        /* Though we have to post to param set directly   *
                         * from here,there will be differene between first*
                         * such packet and second packet. As we have      *
                         * control here we are second packet and first    *
                         * packet has not yet returned (or corresponding  *
                         * edma callback has not been called.For second   *
                         * packet, we will be updating the second param   *
                         * set, which is currently hosting loopjob        *
                         * parameter. Hence increment the index to point  *
                         * second paramset and since we are moving out    *
                         * loopjob from both param sets,the loopjobUpdated*
                         * inParamset is reset                            */
                        chanHandle->loopjobUpdatedinParamset = FALSE;

                        Mcasp_localGetNextIndex(
                            &chanHandle->nextLinkParamSetToBeUpdated);
                    }

                    if (IOM_COMPLETED !=
                        Mcasp_localUpdtDtPktToLnkPrms(
                            chanHandle,
                            ioPacket))
                    {
                        status = IOM_EBADIO;
                        break;
                    }

                    if (0 == linkCnt)
                    {
                        /* if at all this is the very first packet, then  *
                         * one param set has loop job loaded,self linked  *
                         * and active with the main xfer channel param.   *
                         * other param set is ready loaded (just now and  *
                         * has link paramater set as the one having loop  *
                         * job (this is to ensure that if at all we are   *
                         * not getting any more packets loopjob be will   *
                         * take over). Now we have to link the floating   *
                         * newly loaded param set to xfer channel.        */
                        status = EDMA3_DRV_linkChannel(
                                        chanHandle->edmaHandle,
                                        chanHandle->xferChan,
                                        chanHandle->pramTbl[    \
                                  chanHandle->nextLinkParamSetToBeUpdated]);

                        if (IOM_COMPLETED != status)
                        {
                            status = IOM_EBADIO;
                            break;
                        }
                    }
                }
            }

            EDMA3_DRV_enableTransfer(
                chanHandle->edmaHandle,
                chanHandle->xferChan,
                EDMA3_DRV_TRIG_MODE_EVENT);

            dlbMode = (((instHandle->hwInfo.regs)->DLBCTL
                          & CSL_MCASP_DLBCTL_DLBEN_MASK)
                      >> CSL_MCASP_DLBCTL_DLBEN_SHIFT);

            if (CSL_MCASP_DLBCTL_DLBEN_ENABLE == dlbMode)
            {
                /* start both RCV and XMT state m/cs                      */
                /* Activate the transmit and receive clocks               */
                mcaspActivateClkRcvXmt(&(instHandle->hwInfo));

                Mcasp_localActivateSmFsForBoth(instHandle);
            }
            else
            {
                if (IOM_INPUT == chanHandle->mode)
                {
                    /* start Receive state m/c */
                    /* before touching McASP registers, place receive     *
                     * section in reset                                   */
                    mcaspActivateRcvClkSer(&(instHandle->hwInfo));

                    Mcasp_localActivateSmFsForRcv(instHandle);
                }
                else
                {
                    /* start Transmit state m/c*/
                    /* before touching McASP registers, place transmit    *
                     * section in reset                                   */
                    mcaspActivateXmtClkSer(&(instHandle->hwInfo));

                    Mcasp_localActivateSmFsForXmt(instHandle);
                }
            }
        }/* DMA driven is TRUE if loop */

    }
    /*------------------------MCASP STOP CMD  ----------------------------*/
    else if (Mcasp_IOCTL_STOP_PORT == cmd)
    {
        do
        {
            falsewhile = FALSE;
            if (IOM_INPUT == chanHandle->mode)
            {
                if (FALSE == instHandle->stopSmFsRcv)
                {
                    instHandle->stopSmFsRcv = TRUE;
                }
                else
                {
                    /* Already stop port command is issued. This is a     *
                     * redundant command.                                 */
                    status = IOM_EBADARGS;
                    break;
                }
            }
            else
            {
                if (FALSE == instHandle->stopSmFsXmt)
                {
                    instHandle->stopSmFsXmt = TRUE;
                }
                else
                {
                     /* Already stop port command is issued. This is a    *
                      * redundant command.                                */
                    status = IOM_EBADARGS;
                    break;
                }
            }

        }while (falsewhile);

    }
    /*------------------------MCASP PAUSE CMD ----------------------------*/
    else if (Mcasp_IOCTL_PAUSE == cmd)
    {
        if (FALSE == chanHandle->paused)
        {
            chanHandle->paused = TRUE;
        }
        else
        {
            status = IOM_EBADARGS;
        }
    }
    /*------------------------MCASP RESUME CMD ---------------------------*/
    else if (Mcasp_IOCTL_RESUME == cmd)
    {
        do
        {
            falsewhile = FALSE;

            if (FALSE == chanHandle->paused)
            {
                /* Pause is not issued so can not perform resume          */
                status = IOM_EBADARGS;
                break;
            }

            if ((TRUE == chanHandle->isDmaDriven)
                && (TRUE == QUE_empty(&(chanHandle->queueFloatingList))))
            {
                for (linkCnt = 0;linkCnt < Mcasp_MAXLINKCNT;linkCnt++)
                {
                    if (FALSE == QUE_empty(&(chanHandle->queueReqList)))
                    {
                        ioPacket = (IOM_Packet *)QUE_get(
                                        &(chanHandle->queueReqList));

                        if (NULL == ioPacket)
                        {
                            break;
                        }

                        /* floating queue not full.Process this request   */
                        QUE_put(
                            &(chanHandle->queueFloatingList),
                            (Ptr)ioPacket);

                        chanHandle->currentDataSize =(Uint16)ioPacket->size;
                        chanHandle->userDataBufferSize = ioPacket->size;

                        /* For DMA mode we will not use chanHandle->dataPacket
                         * to hold current packet - we use floatingqueue  */
                        chanHandle->dataPacket = NULL;

                        if ((Mcasp_MAXLINKCNT - 1u) == linkCnt)
                        {
                            /* Though we have to post to param set direct *
                             * from here,there will be differene between  *
                             * first such packet and second packet. As we *
                             * have control here we are second packet and *
                             * first packet has not yet returned (or      *
                             * corresponding edma callback has not been   *
                             * called.For second packet, we will be       *
                             * updating the second param set, which is    *
                             * currently hosting loopjob parameter. Hence *
                             * increment the index to point second param  *
                             * set and since we are moving out loopjob    *
                             * from both param sets, the loopjobUpdated in*
                             * Paramset is reset                          */
                            chanHandle->loopjobUpdatedinParamset =FALSE;

                            Mcasp_localGetNextIndex(
                                &chanHandle->nextLinkParamSetToBeUpdated);
                        }

                        if (IOM_COMPLETED !=
                            Mcasp_localUpdtDtPktToLnkPrms(
                                chanHandle,
                                ioPacket))
                        {
                            status = IOM_EBADIO;
                            break;
                        }

                        if (0 == linkCnt)
                        {
                            /* if at all this is the very first packet,   *
                             * then one param set has loop job loaded,    *
                             * self linked and active with the main xfer  *
                             * channel param. other param set is ready    *
                             * loaded (just now and has link paramater set*
                             * as the one having loopjob (this is to      *
                             * ensure that if at all we are not getting   *
                             * any more packets loopjob be will taken over*
                             * ). Now we have to link the floating/newly  *
                             * loaded param set to xfer channel.          */
                            status = EDMA3_DRV_linkChannel(
                                            chanHandle->edmaHandle,
                                            chanHandle->xferChan,
                                            chanHandle->pramTbl[ \
                                  chanHandle->nextLinkParamSetToBeUpdated]);

                            if (IOM_COMPLETED != status)
                            {
                                status = IOM_EBADIO;
                                break;
                            }
                        }
                    }
                }
            }

            chanHandle->paused = FALSE;

        }while (falsewhile);
    }
    /*****************command to modify the Timeout count value****************/
    else if (Mcasp_IOCTL_SET_TIMEOUT == cmd)
    {
        assert(NULL != arg);

        /* modify the retry count value to be used                            */
        instHandle->retryCount = *(Uint32 *)arg;
    }
#endif    
    else if (Mcasp_IOCTL_FLUSH_RCV_FIFO == cmd)
    {
        if ((IOM_INPUT == chanHandle->mode) &&
            (TRUE == chanHandle->enableHwFifo))
        {
            while ((0 != (instHandle->hwInfo.fifoRegs->RFIFOSTS &
                            CSL_AFIFO_RFIFOSTS_RLVL_MASK)) && (0 != timeout--))
            {
                /* empty the FIFO now                                         */
                tempVal = *((Uint32 *)instHandle->hwInfo.dataAddr);
            }
        }
        else
        {
            status = IOM_EBADMODE;
        }
    }

    else
    {
        /* command not executed hence not set status as not implemented   */
        status = IOM_ENOTIMPL;
    }
    return (status);
}

/**
 * \brief   This function sets the digital loopback mode
 *
 * \param   hMcasp   [IN]  pointer to the Mcasp Hardware information structure
 * \param   loopBack [IN]     Value to be loaded into the bit filed
 *
 * \return
 *          IOM_COMPLETED   - Successful completion
 *          IOM_EBADARGS    - Invalid parameters or Invalid handle
 *
 * \entry   hMcasp      should be non NULL and valid pointer
 *          loopBack    should be a valid value
 *
 * \leave   Not implemented
 */

static Void mcaspConfigLoopBack(Mcasp_HwHandle hMcasp,Bool loopBack)
{
    Bool       loopBackEnable = 0;
    Bool       orderBit       = 0;
    Int16      serNum         = 0;
    volatile Uint32 *pSrcCtl  = NULL;
    Uint32     serMode        = 0;

    assert((NULL != hMcasp) && (NULL != (hMcasp->regs)));

    /* Reset the RSRCLR and XSRCLR registers in GBLCTL                    */
    (hMcasp->regs)->GBLCTL =
        ((hMcasp->regs)->GBLCTL & ~(CSL_MCASP_GBLCTL_RSRCLR_MASK))
        | (CSL_MCASP_GBLCTL_RSRCLR_CLEAR << CSL_MCASP_GBLCTL_RSRCLR_SHIFT);

    (hMcasp->regs)->GBLCTL =
        ((hMcasp->regs)->GBLCTL & ~(CSL_MCASP_GBLCTL_XSRCLR_MASK))
        | (CSL_MCASP_GBLCTL_XSRCLR_CLEAR << CSL_MCASP_GBLCTL_XSRCLR_SHIFT);

    /* Reset the RSMRST and XSMRST registers in GBLCTL                    */
    (hMcasp->regs)->GBLCTL =
        ((hMcasp->regs)->GBLCTL & ~(CSL_MCASP_GBLCTL_RSMRST_MASK))
        | (CSL_MCASP_GBLCTL_RSMRST_RESET << CSL_MCASP_GBLCTL_RSMRST_SHIFT);

    (hMcasp->regs)->GBLCTL =
        ((hMcasp->regs)->GBLCTL & ~(CSL_MCASP_GBLCTL_XSMRST_MASK))
        | (CSL_MCASP_GBLCTL_XSMRST_RESET << CSL_MCASP_GBLCTL_XSMRST_SHIFT);

    /* Reset the RFRST and XFRST registers in GBLCTL                      */
    (hMcasp->regs)->GBLCTL =
        ((hMcasp->regs)->GBLCTL & ~(CSL_MCASP_GBLCTL_RFRST_MASK))
        | (CSL_MCASP_GBLCTL_RFRST_RESET << CSL_MCASP_GBLCTL_RFRST_SHIFT);

    (hMcasp->regs)->GBLCTL =
        ((hMcasp->regs)->GBLCTL & ~(CSL_MCASP_GBLCTL_XFRST_MASK))
        | (CSL_MCASP_GBLCTL_XFRST_RESET << CSL_MCASP_GBLCTL_XFRST_SHIFT);


    /* configure loop back mode                                           */
    (hMcasp->regs)->DLBCTL =
        (Uint32)((hMcasp->regs)->DLBCTL & ~(CSL_MCASP_DLBCTL_DLBEN_MASK))
        | (loopBack << CSL_MCASP_DLBCTL_DLBEN_SHIFT);

    loopBackEnable = (((hMcasp->regs)->DLBCTL & CSL_MCASP_DLBCTL_DLBEN_MASK)
                     >> CSL_MCASP_DLBCTL_DLBEN_SHIFT);

    if (loopBackEnable == TRUE)
    {
        (hMcasp->regs)->DLBCTL =
            ((hMcasp->regs)->DLBCTL & ~(CSL_MCASP_DLBCTL_MODE_MASK))
            |(CSL_MCASP_DLBCTL_MODE_XMTCLK << CSL_MCASP_DLBCTL_MODE_SHIFT);
    }

    orderBit = ((hMcasp->regs)->DLBCTL & CSL_MCASP_DLBCTL_ORD_MASK)
               >> CSL_MCASP_DLBCTL_ORD_SHIFT;

    if (orderBit == TRUE)
    {
        while (serNum < hMcasp->numOfSerializers)
        {
            serMode = (((hMcasp->regs)->SRCTL0 + (serNum * sizeof(Uint32)))
                       & CSL_MCASP_SRCTL0_SRMOD_MASK)
                       >> CSL_MCASP_SRCTL0_SRMOD_SHIFT;

            if (CSL_MCASP_SRCTL0_SRMOD_INACTIVE != serMode)
            {
                pSrcCtl = (((volatile Uint32 *)&((hMcasp->regs)->SRCTL0)) + (serNum));

                if (NULL != pSrcCtl)
                {
                    *pSrcCtl = (((*pSrcCtl) & ~(CSL_MCASP_SRCTL0_SRMOD_MASK))
                                | (CSL_MCASP_SRCTL0_SRMOD_RCV
                                << CSL_MCASP_SRCTL0_SRMOD_SHIFT));
                }

                (hMcasp->regs)->PDIR &= (~(1 << serNum));
            }
            serNum++;
        }
    }
    else
    {
        while (serNum < hMcasp->numOfSerializers)
        {
            pSrcCtl = (((volatile Uint32*)&((hMcasp->regs)->SRCTL0)) \
                            + (serNum));

            serMode = ((((hMcasp->regs)->SRCTL0 + (serNum * sizeof(Uint32)))
                        & CSL_MCASP_SRCTL0_SRMOD_MASK)
                        >> CSL_MCASP_SRCTL0_SRMOD_SHIFT);

            if (CSL_MCASP_SRCTL9_SRMOD_INACTIVE != serMode)
            {
                *pSrcCtl = (((*pSrcCtl) & ~(CSL_MCASP_SRCTL0_SRMOD_MASK))
                            | (CSL_MCASP_SRCTL0_SRMOD_XMT
                            << CSL_MCASP_SRCTL0_SRMOD_SHIFT));

                (hMcasp->regs)->PDIR |= (1u << serNum);
            }
            serNum++;
        }
    }
}

#ifdef Mcasp_LOOPJOB_ENABLED
/**
 * \brief   This function activates the receive and transmit clock.
 *
 * \param   hMcasp  [IN]  Handle to the Mcasp Hw info object
 * \param   eb      [OUT] Pointer to the error block
 *
 * \return  IOM_COMPLETED in case of sucess else An error code
 *
 * \entry   hMcasp      should be non NULL and valid pointer
 *
 * \leave   Not implemented
 */
static Void mcaspActivateClkRcvXmt(Mcasp_HwHandle hMcasp)
{
    Uint32   bitValue = 0;

    assert((NULL != hMcasp) && (NULL != (hMcasp->regs)));

    /* Sequence of start: starting hclk first                             */
    /* start AHCLKR */
    (hMcasp->regs)->GBLCTL =
        ((hMcasp->regs)->GBLCTL & ~(CSL_MCASP_GBLCTL_RHCLKRST_MASK))
        | (CSL_MCASP_GBLCTL_RHCLKRST_ACTIVE
        << CSL_MCASP_GBLCTL_RHCLKRST_SHIFT);

    bitValue = 0;

    while (CSL_MCASP_GBLCTL_RHCLKRST_ACTIVE != bitValue)
    {
        bitValue = (((hMcasp->regs)->GBLCTL
                   & CSL_MCASP_GBLCTL_RHCLKRST_MASK)
                   >> CSL_MCASP_GBLCTL_RHCLKRST_SHIFT);
    }

    /* start ACLKR only if internal clock is used                         */
    if (CSL_MCASP_ACLKRCTL_CLKRM_INTERNAL ==
        (((hMcasp->regs)->ACLKRCTL & CSL_MCASP_ACLKRCTL_CLKRM_MASK)
        >> CSL_MCASP_ACLKRCTL_CLKRM_SHIFT))
    {
        (hMcasp->regs)->GBLCTL =
            ((hMcasp->regs)->GBLCTL & ~(CSL_MCASP_GBLCTL_RCLKRST_MASK))
            | (CSL_MCASP_GBLCTL_RCLKRST_ACTIVE
            << CSL_MCASP_GBLCTL_RCLKRST_SHIFT);

        bitValue = 0;

        while (CSL_MCASP_GBLCTL_RCLKRST_ACTIVE != bitValue)
        {
            bitValue = (((hMcasp->regs)->GBLCTL
                       & CSL_MCASP_GBLCTL_RCLKRST_MASK)
                       >> CSL_MCASP_GBLCTL_RCLKRST_SHIFT);
        }
    }

    /* Sequence of start: starting hclk first                             */
    (hMcasp->regs)->XGBLCTL =
        ((hMcasp->regs)->XGBLCTL & ~(CSL_MCASP_XGBLCTL_XHCLKRST_MASK))
        | (CSL_MCASP_XGBLCTL_XHCLKRST_ACTIVE
        << CSL_MCASP_XGBLCTL_XHCLKRST_SHIFT);

    bitValue = 0;

    while (CSL_MCASP_GBLCTL_XHCLKRST_ACTIVE != bitValue)
    {
        bitValue =
            ((hMcasp->regs)->GBLCTL & CSL_MCASP_GBLCTL_XHCLKRST_MASK)
            >> CSL_MCASP_GBLCTL_XHCLKRST_SHIFT;
    }

    /* start ACLKX only if internal clock is used                         */
    if (CSL_MCASP_ACLKXCTL_CLKXM_INTERNAL ==
          (((hMcasp->regs)->ACLKXCTL & CSL_MCASP_ACLKXCTL_CLKXM_MASK)
          >> CSL_MCASP_ACLKXCTL_CLKXM_SHIFT))
    {
        (hMcasp->regs)->XGBLCTL =
            ((hMcasp->regs)->XGBLCTL & ~(CSL_MCASP_XGBLCTL_XCLKRST_MASK))
            | (CSL_MCASP_XGBLCTL_XCLKRST_ACTIVE
            << CSL_MCASP_XGBLCTL_XCLKRST_SHIFT);

        bitValue = 0;

        while (CSL_MCASP_GBLCTL_XCLKRST_ACTIVE != bitValue)
        {
            bitValue =
                ((hMcasp->regs)->GBLCTL & CSL_MCASP_GBLCTL_XCLKRST_MASK)
                >> CSL_MCASP_GBLCTL_XCLKRST_SHIFT;
        }
    }
}



/**
 * \brief   This function sets the bits related to receive in RGBLCTL.
 *
 * \param   hMcasp  [IN] Pointer to the Mcasp Hardware info structure
 *
 * \return
 *          IOM_COMPLETED   - Successful completion
 *          IOM_EBADARGS    - Invalid parameters or Invalid handle
 *
 * \entry   hMcasp      should be non NULL and valid pointer
 *
 * \leave   Not implemented
 */

static Void mcaspActivateRcvClkSer(Mcasp_HwHandle hMcasp)
{
    Uint32   bitValue = 0;

    assert((NULL != hMcasp) && (NULL != (hMcasp->regs)));

    /* Sequence of start: starting hclk first                             */
    /* start AHCLKR */
    (hMcasp->regs)->GBLCTL =
        ((hMcasp->regs)->GBLCTL & ~(CSL_MCASP_GBLCTL_RHCLKRST_MASK))
        | (CSL_MCASP_GBLCTL_RHCLKRST_ACTIVE
            << CSL_MCASP_GBLCTL_RHCLKRST_SHIFT);

    bitValue = 0;

    while (CSL_MCASP_GBLCTL_RHCLKRST_ACTIVE != bitValue)
    {
        bitValue = ((hMcasp->regs)->GBLCTL & CSL_MCASP_GBLCTL_RHCLKRST_MASK)
                   >> (CSL_MCASP_GBLCTL_RHCLKRST_SHIFT);
    }

    /* start ACLKR only if internal clock is used                         */
    if (CSL_MCASP_ACLKRCTL_CLKRM_INTERNAL ==
        (((hMcasp->regs)->ACLKRCTL & CSL_MCASP_ACLKRCTL_CLKRM_MASK)
        >> CSL_MCASP_ACLKRCTL_CLKRM_SHIFT))
    {
        (hMcasp->regs)->GBLCTL =
            ((hMcasp->regs)->GBLCTL & ~(CSL_MCASP_GBLCTL_RCLKRST_MASK))
            | (CSL_MCASP_GBLCTL_RCLKRST_ACTIVE
            << CSL_MCASP_GBLCTL_RCLKRST_SHIFT);

        bitValue = 0;

        while (CSL_MCASP_GBLCTL_RCLKRST_ACTIVE != bitValue)
        {
            bitValue = (((hMcasp->regs)->GBLCTL
                       & CSL_MCASP_GBLCTL_RCLKRST_MASK)
                       >> CSL_MCASP_GBLCTL_RCLKRST_SHIFT);
        }
    }

    (hMcasp->regs)->RGBLCTL =
        ((hMcasp->regs)->RGBLCTL & ~(CSL_MCASP_RGBLCTL_RSRCLR_MASK))
        | (CSL_MCASP_RGBLCTL_RSRCLR_ACTIVE
        << CSL_MCASP_RGBLCTL_RSRCLR_SHIFT);

    bitValue = 0;

    while (CSL_MCASP_GBLCTL_RSRCLR_ACTIVE != bitValue)
    {
        bitValue = (((hMcasp->regs)->GBLCTL & CSL_MCASP_GBLCTL_RSRCLR_MASK)
                   >> CSL_MCASP_GBLCTL_RSRCLR_SHIFT);
    }
}

/**
 * \brief   This function sets the bits related to transmit in RGBLCTL.
 *
 * \param   hMcasp  [IN] Pointer to the Mcasp Hardware info structure
 *
 * \return
 *          IOM_COMPLETED   - Successful completion
 *          IOM_EBADARGS    - Invalid parameters or Invalid handle
 *
 * \entry   hMcasp      should be non NULL and valid pointer
 *
 * \leave   Not implemented
 */
static Void mcaspActivateXmtClkSer(Mcasp_HwHandle hMcasp)
{
    Uint32    bitValue = 0;

    assert((NULL != hMcasp) && (NULL != (hMcasp->regs)));

    /* Sequence of start: starting hclk first*/
    (hMcasp->regs)->XGBLCTL =
        ((hMcasp->regs)->XGBLCTL & ~(CSL_MCASP_XGBLCTL_XHCLKRST_MASK))
        | (CSL_MCASP_XGBLCTL_XHCLKRST_ACTIVE
            << CSL_MCASP_XGBLCTL_XHCLKRST_SHIFT);

    bitValue = 0;

    while (CSL_MCASP_GBLCTL_XHCLKRST_ACTIVE != bitValue)
    {
        bitValue = (((hMcasp->regs)->GBLCTL
                   & CSL_MCASP_GBLCTL_XHCLKRST_MASK)
                   >> CSL_MCASP_GBLCTL_XHCLKRST_SHIFT);
    }


     /* start ACLKX only if internal clock is used*/
    if (CSL_MCASP_ACLKXCTL_CLKXM_INTERNAL ==
       (((hMcasp->regs)->ACLKXCTL & CSL_MCASP_ACLKXCTL_CLKXM_MASK)
        >> CSL_MCASP_ACLKXCTL_CLKXM_SHIFT))
    {
        (hMcasp->regs)->XGBLCTL =
            ((hMcasp->regs)->XGBLCTL & ~(CSL_MCASP_XGBLCTL_XCLKRST_MASK))
            | (CSL_MCASP_XGBLCTL_XCLKRST_ACTIVE
            << CSL_MCASP_XGBLCTL_XCLKRST_SHIFT);

        bitValue = 0;

        while (CSL_MCASP_GBLCTL_XCLKRST_ACTIVE != bitValue)
        {
            bitValue = (((hMcasp->regs)->GBLCTL
                       & CSL_MCASP_GBLCTL_XCLKRST_MASK)
                       >> CSL_MCASP_GBLCTL_XCLKRST_SHIFT);
        }
    }

    (hMcasp->regs)->XGBLCTL =
        ((hMcasp->regs)->XGBLCTL & ~(CSL_MCASP_XGBLCTL_XSRCLR_MASK))
        | (CSL_MCASP_XGBLCTL_XSRCLR_ACTIVE
        << CSL_MCASP_XGBLCTL_XSRCLR_SHIFT);

    bitValue = 0;

    while (CSL_MCASP_GBLCTL_XSRCLR_ACTIVE != bitValue)
    {
        bitValue = (((hMcasp->regs)->GBLCTL & CSL_MCASP_GBLCTL_XSRCLR_MASK)
                   >> CSL_MCASP_GBLCTL_XSRCLR_SHIFT);
    }
}
#endif

/**
 *  \brief  This function is used to get the value of various parameters of the
 *          McASP instance. The value returned depends on the query passed.
 *
 *  \param  hMcasp    [IN]    Handle to mcasp H/W information structure
 *  \param  myQuery   [IN]    Query requested by application
 *  \param  response  [IN]    Pointer to buffer to return the data requested by
 *                            the query passed
 *  \param  eb        [IN]    pointer to error block
 *
 *  \return
 *          IOM_COMPLETED  - Successful completion of the query
 *          IOM error code - in case of error.
 *
 * \entry   hMcasp      should be non NULL and valid pointer
 *          myQuery     should be a valid value
 *          response    shluld be non NULL and valid pointer
 *
 * \leave   Not implemented
 */
static Int32 mcaspGetHwStatus(Mcasp_HwHandle        hMcasp,
                              Mcasp_HwStatusQuery   myQuery,
                              Void                 *const response)
{
    Int32                status      = IOM_COMPLETED;
    Mcasp_SerQuery     *serQuery     = NULL;
    Mcasp_SerModeQuery *serModeQuery = NULL;


    assert((NULL != hMcasp) && (NULL != response));

    switch (myQuery)
    {
        /* Return current transmit slot being transmitted                 */
        case Mcasp_HwStatusQuery_CURRENT_XSLOT:
            *((Uint16 *)response) =
                (Uint16)(((hMcasp->regs)->XSLOT
                & CSL_MCASP_XSLOT_XSLOTCNT_MASK)
                >> CSL_MCASP_XSLOT_XSLOTCNT_SHIFT);
            break;

        /* Return current receive slot being received                     */
        case Mcasp_HwStatusQuery_CURRENT_RSLOT:
            *((Uint16 *)response) =
                (Uint16)(((hMcasp->regs)->RSLOT
                & CSL_MCASP_RSLOT_RSLOTCNT_MASK)
                >> CSL_MCASP_RSLOT_RSLOTCNT_SHIFT);
            break;

        /* Return transmit error status bit                               */
        case Mcasp_HwStatusQuery_XSTAT_XERR:
            *(Bool *)response =
                (Bool)(((hMcasp->regs)->XSTAT & CSL_MCASP_XSTAT_XERR_MASK)
                >> CSL_MCASP_XSTAT_XERR_SHIFT);
            break;

        /* Return transmit clock failure flag status                      */
        case Mcasp_HwStatusQuery_XSTAT_XCLKFAIL:
            *(Bool *)response =
                (Bool)(((hMcasp->regs)->XSTAT
                 & CSL_MCASP_XSTAT_XCKFAIL_MASK)
                 >> CSL_MCASP_XSTAT_XCKFAIL_SHIFT);
            break;

        /* Return unexpected transmit frame sync flag status              */
        case Mcasp_HwStatusQuery_XSTAT_XSYNCERR:
            *(Bool *)response =
                (Bool)(((hMcasp->regs)->XSTAT
                & CSL_MCASP_XSTAT_XSYNCERR_MASK)
                >> CSL_MCASP_XSTAT_XSYNCERR_SHIFT);
            break;

        /* Return transmit underrun flag status                           */
        case Mcasp_HwStatusQuery_XSTAT_XUNDRN:
            *((Bool *)response) =
                (Bool)(((hMcasp->regs)->XSTAT & CSL_MCASP_XSTAT_XUNDRN_MASK)
                >> CSL_MCASP_XSTAT_XUNDRN_SHIFT);
            break;

        /* Return transmit data ready flag status                         */
        case Mcasp_HwStatusQuery_XSTAT_XDATA:
            (*(Bool *)response) =
                (Bool)(((hMcasp->regs)->XSTAT & CSL_MCASP_XSTAT_XDATA_MASK)
                >> CSL_MCASP_XSTAT_XDATA_SHIFT);
            break;

        /* Return receive error status bit                                */
        case Mcasp_HwStatusQuery_RSTAT_RERR:
            *((Bool *)response) =
                (Bool)(((hMcasp->regs)->RSTAT & CSL_MCASP_RSTAT_RERR_MASK)
                >> CSL_MCASP_RSTAT_RERR_SHIFT);
            break;

        /* Return receive clk failure flag status                         */
        case Mcasp_HwStatusQuery_RSTAT_RCLKFAIL:
            *((Bool *)response) =
                (Bool)(((hMcasp->regs)->RSTAT
                & CSL_MCASP_RSTAT_RCKFAIL_MASK)
                >> CSL_MCASP_RSTAT_RCKFAIL_SHIFT);
            break;

        /* Return unexpected receive frame sync flag status               */
        case Mcasp_HwStatusQuery_RSTAT_RSYNCERR:
            *((Bool *)response) =
                (Bool)(((hMcasp->regs)->RSTAT
                & CSL_MCASP_RSTAT_RSYNCERR_MASK)
                >> CSL_MCASP_RSTAT_RSYNCERR_SHIFT);
            break;

        /* Return receive overrun flag status                             */
        case Mcasp_HwStatusQuery_RSTAT_ROVRN:
            *((Bool *)response) =
                (Bool)(((hMcasp->regs)->RSTAT & CSL_MCASP_RSTAT_ROVRN_MASK)
                >> CSL_MCASP_RSTAT_ROVRN_SHIFT);
            break;
        /* Return receive data ready flag status                          */
        case Mcasp_HwStatusQuery_RSTAT_RDATA:
            *((Bool *)response) =
            (Bool)(((hMcasp->regs)->RSTAT & CSL_MCASP_RSTAT_RDATA_MASK)
            >> CSL_MCASP_RSTAT_RDATA_SHIFT);
            break;

        /* Return status whether rrdy is set or not                       */
        case Mcasp_HwStatusQuery_SRCTL_RRDY:
            {
                serQuery = (Mcasp_SerQuery *)response;
                mcaspGetSerRcvReady(
                             hMcasp,
                             (Bool *)&(serQuery->serStatus),
                             serQuery->serNum);
            }
            break;

        /* Return status whether xrdy is set or not                       */
        case Mcasp_HwStatusQuery_SRCTL_XRDY:
            {
                serQuery = (Mcasp_SerQuery *)response;
                mcaspGetSerXmtReady(
                            hMcasp,
                            (Bool *)&(serQuery->serStatus),
                            serQuery->serNum);
            }
            break;
       /* Return status whether serializer is configured as TX/RX/FREE    */
       case Mcasp_HwStatusQuery_SRCTL_SRMOD:
            {
                serModeQuery = (Mcasp_SerModeQuery *)response;
                mcaspGetSerMode(hMcasp,
                                &(serModeQuery->serMode),
                                serModeQuery->serNum);
                break;
            }

        /* Return the value of XSTAT register                             */
        case Mcasp_HwStatusQuery_XSTAT:
            *((Uint16 *)response) = (Uint16) (hMcasp->regs)->XSTAT;
            break;

        /* Return the value of RSTAT register                             */
        case Mcasp_HwStatusQuery_RSTAT:
            *((Uint16 *)response) = (Uint16) (hMcasp->regs)->RSTAT;
            break;

        /* Return the XSMRST and XFRST field values GBLCTL register       */
        case Mcasp_HwStatusQuery_SM_FS_XMT:
            *((Uint8 *)response) = mcaspGetSmFsXmt(hMcasp);
            break;

        /* Return the RSMRST and RFRST field values GBLCTL register       */
        case Mcasp_HwStatusQuery_SM_FS_RCV:
            *((Uint8 *)response) = mcaspGetSmFsRcv(hMcasp);
            break;

        /* Return status of DITEN bit in DITCTL register                  */
        case Mcasp_HwStatusQuery_DIT_MODE:
            *((Bool *)response) =
                (Bool)(((hMcasp->regs)->DITCTL
                & CSL_MCASP_DITCTL_DITEN_MASK)
                >> CSL_MCASP_DITCTL_DITEN_SHIFT);
            break;

        case Mcasp_HwStatusQuery_AMUTE:
            *((Uint16 *)response) = (Uint16)(hMcasp->regs)->AMUTE;
            break;

        default:
            status = IOM_EBADARGS;
            break;
    }

    return status;
}

/**
 * \brief   This function checks whether transmit buffer ready bit of serializer
 *          control register is set or not
 *
 * \param   hMcasp      [IN] pointer to the Mcasp Hardware information structure
 * \param   serXmtReady [IN] Status of the serializer will be stored here
 * \param   serNum      [IN] serializer number to be checked
 *
 * \return
 *          IOM_COMPLETED   - Successful completion
 *          IOM_EBADARGS    - Invalid parameters or Invalid handle
 *
 * \entry   hMcasp      should be non NULL and valid pointer
 *          serCmtReady should be non NULL and valid pointer
 *          serNum      should be a valid value
 *
 * \leave   Not implemented
 */

static Void mcaspGetSerXmtReady(Mcasp_HwHandle        hMcasp,
                                Bool                 *serXmtReady,
                                Mcasp_SerializerNum   serNum)
{
    volatile Uint32  *pSrcCtl = NULL;

    assert((NULL != serXmtReady) && (NULL != hMcasp));

    assert (serNum < hMcasp->numOfSerializers);

    pSrcCtl = (((volatile Uint32*)&((hMcasp->regs)->SRCTL0))+(Uint32)(serNum));

    if (NULL != pSrcCtl)
    {
        *(Bool *)serXmtReady =
            ((Bool)(*pSrcCtl & CSL_MCASP_SRCTL0_XRDY_MASK)
            >> CSL_MCASP_SRCTL0_XRDY_SHIFT);
    }
}

/**
 * \brief   This function checks whether receive buffer ready bit of serializer
 *          control register is set or not
 *
 * \param   hMcasp      [IN] pointer to the Mcasp Hardware information structure
 * \param   serRcvReady [IN] Serailizer status will be stored here
 * \param   serNum      [IN] Serailizer register to be checked
 *
 * \return
 *          IOM_COMPLETED   - Successful completion of read
 *          IOM_EBADARGS    - Invalid parameters or Invalid handle
 *
 * \entry   hMcasp      should be non NULL and valid pointer
 *          serRcvReady should be non NULL and valid pointer
 *          serNum      should be a valid value
 *
 * \leave   Not implemented
 *
 */

static Void mcaspGetSerRcvReady(Mcasp_HwHandle       hMcasp,
                                 Bool                *serRcvReady,
                                 Mcasp_SerializerNum  serNum)
{
    volatile Uint32  *pSrcCtl = NULL;

    assert((NULL != serRcvReady) && (NULL != hMcasp));
    assert(serNum < hMcasp->numOfSerializers);

    pSrcCtl = (((volatile Uint32*)&((hMcasp->regs)->SRCTL0)) +
                    (Uint32)(serNum));

    if (NULL != pSrcCtl)
    {
        (*(Bool *)serRcvReady) = ((Bool)(((*pSrcCtl) & CSL_MCASP_SRCTL0_RRDY_MASK)
                                   >> CSL_MCASP_SRCTL0_RRDY_SHIFT));
    }
}

/**
 * \brief   This function gets the current mode for the serializer requested
 *
 * \param   hMcasp    [IN]  pointer to the Mcasp Hardware information structure
 * \param   serMode   [IN]  Serializer mode will be stored here
 * \param   serNum    [IN]  Serailizer number to be checked
 *
 * \return
 *          IOM_COMPLETED   - Successful completion
 *          IOM_EBADARGS    - Invalid parameters or Invalid handle
 *
 * \entry   hMcasp      should be non NULL and valid pointer
 *          serMode     should be non NULL and valid pointer
 *          serNum      should be a valid value
 *
 * \leave   Not implemented
 */
static Void mcaspGetSerMode(Mcasp_HwHandle        hMcasp,
                            Mcasp_SerMode        *serMode,
                            Mcasp_SerializerNum   serNum)
{
    volatile Uint32  *pSrcCtl = NULL;

    assert((NULL != serMode) && (NULL != hMcasp));
    assert(serNum < hMcasp->numOfSerializers);

    pSrcCtl = (((volatile Uint32*)&((hMcasp->regs)->SRCTL0)) +
                    (Uint32)(serNum));

    if (NULL != pSrcCtl)
    {
        *serMode = (Mcasp_SerMode)(((*pSrcCtl) & CSL_MCASP_SRCTL0_SRMOD_MASK)
                       >> CSL_MCASP_SRCTL0_SRMOD_SHIFT);
    }
}


/**
 *  \brief    mcaspReadRcvConfig
 *
 *   Reads the configuration data of the receive section and updates the info
 *   to the pointer provided by the application.
 *
 *  \param    hMcasp         [IN]    Handle to mcasp hw information structure
 *  \param    rcvData        [OUT]   pointer to copy the data to
 *  \param    eb             [OUT]   pointer to error block
 *
 *  \return   Nothing
 *
 * \entry   hMcasp      should be non NULL and valid pointer
 *          rcvData     should be non NULL and valid pointer
 *
 * \leave   Not implemented
 */
static Void mcaspReadRcvConfig(Mcasp_HwHandle       hMcasp,
                               Mcasp_HwSetupData   *const rcvData)
{
    assert((NULL != hMcasp) && (NULL != rcvData));

    /* Read RMASK register             */
    rcvData->mask = (Uint32)((hMcasp->regs)->RMASK);

    /* Read RFMT register              */
    rcvData->fmt  = (Uint32)((hMcasp->regs)->RFMT);

    /* Read AFSRCTL register           */
    rcvData->frSyncCtl = (Uint32)((hMcasp->regs)->AFSRCTL);

    /* Read AHCLKRCTL register         */
    rcvData->clk.clkSetupClk = (Uint32)((hMcasp->regs)->ACLKRCTL);

    /* Read ACLKRCTL register          */
    rcvData->clk.clkSetupHiClk = (Uint32)((hMcasp->regs)->AHCLKRCTL);

    /* Read RTDM register              */
    rcvData->tdm = (Uint32)((hMcasp->regs)->RTDM);

    /* Read RINTCTL register           */
    rcvData->intCtl = (Uint32)((hMcasp->regs)->RINTCTL);

    /* Read RCLKCHK register           */
    rcvData->clk.clkChk = (Uint32)((hMcasp->regs)->RCLKCHK);

    /* Read RSTAT register             */
    rcvData->stat = (Uint32)((hMcasp->regs)->RSTAT);

    /* Read REVTCTL register           */
    rcvData->evtCtl = (Uint32)((hMcasp->regs)->REVTCTL);
}

/**
 *  \brief    mcaspReadXmtConfig
 *
 *   Reads the configuration data of the transmit section and updates
 *   the info to the pointer provided by the application.
 *
 *  \param    hMcasp        [IN]    Handle to mcasp hw information structure
 *  \param    rcvData       [OUT]   pointer to copy the data to
 *  \param    eb            [OUT]   pointer to error block
 *
 *  \return   Nothing
 *
 * \entry   hMcasp      should be non NULL and valid pointer
 *          xmtData     should be non NULL and valid pointer
 *
 * \leave   Not implemented
 */
static Void mcaspReadXmtConfig(Mcasp_HwHandle       hMcasp,
                               Mcasp_HwSetupData   *const xmtData)
{
    assert((NULL != hMcasp) && (NULL != xmtData));

    /* Read XMASK register              */
    xmtData->mask = (Uint32)((hMcasp->regs)->XMASK);

    /* Read XFMT register               */
    xmtData->fmt = (Uint32)((hMcasp->regs)->XFMT);

    /* Read AFSXCTL register            */
    xmtData->frSyncCtl = (Uint32)((hMcasp->regs)->AFSXCTL);

    xmtData->clk.clkSetupClk = (Uint32)((hMcasp->regs)->ACLKXCTL);

    /* Read AHCLKXCTL register          */
    xmtData->clk.clkSetupHiClk = (Uint32)((hMcasp->regs)->AHCLKXCTL);

    /* Read XTDM register               */
    xmtData->tdm = (Uint32)((hMcasp->regs)->XTDM);

    /* Read XINTCTL register            */
    xmtData->intCtl = (Uint32)((hMcasp->regs)->XINTCTL);

    /* Read XCLKCHK register            */
    xmtData->clk.clkChk = (Uint32)((hMcasp->regs)->XCLKCHK);

    /* Read XSTAT register              */
    xmtData->stat = (Uint32)((hMcasp->regs)->XSTAT);

    /* Read XEVTCTL register            */
    xmtData->evtCtl = (Uint32)((hMcasp->regs)->XEVTCTL);
}

/*
 *  \brief  Abort the operation of the requested Mcasp Channel. This function
 *          Aborts all current and pending Read and write requests
 *
 *  \param  handle   [IN]   Handle to the McASP channel
 *  \param  arg      [IN]   Unused argument(for future use)
 *  \param  eb       [OUT]  pointer to the error block
 *
 *  \return IOM_COMPLETED or Error code
 *
 * \entry   handle      should be non NULL and valid pointer
 *          arg         should be a valis value
 *
 * \leave   Not implemented
 */
Int32 Mcasp_localAbortRequests(Ptr handle,Ptr arg)
{
    Mcasp_ChannelHandle   chanHandle = NULL;
    Mcasp_Object         *instHandle = NULL;
    Int32                 status     = IOM_COMPLETED;

    assert(NULL != handle);

    chanHandle = (Mcasp_ChannelHandle)handle;

    assert(NULL != chanHandle->devHandle);

    instHandle = (Mcasp_Object *)chanHandle->devHandle;

    assert(NULL != instHandle);
    
    /* To remove compiler warning                                             */
    arg = arg;

    if (Mcasp_DriverState_OPENED == chanHandle->chanState)
    {
        /* Abort the requested channels                               */
        if (IOM_INPUT == chanHandle->mode)
        {
            status = Mcasp_localAbortReset(&instHandle->RcvObj);
        }
        else
        {
            status = Mcasp_localAbortReset(&instHandle->XmtObj);
        }
    }

    return (status);
}

/**
 * \brief   This function returns the RSMRST and RFRST field values of RGBLCTL
 *          register.
 *
 * \param   hMcasp [IN] Handle to the McASP hardware info structure
 *
 * \return
 *          0x00        - Both receive frame generator sync and receive state
 *                        machine are reset.
 *          0x1         - Only receive state machine is active.
 *          0x10        - Only receive frame sync generator is active.
 *          0x11        - Both receive frame generator sync and receive state
 *                        machine are active.
 *
 * \entry   hMcasp      should be non NULL and valid pointer
 *
 * \leave   Not implemented
 */
static Uint8 mcaspGetSmFsRcv(Mcasp_HwHandle hMcasp)
{
    Uint8  smFsRcv = 0;

    assert(NULL != hMcasp);

    smFsRcv = (Uint8)(((hMcasp->regs)->RGBLCTL
              & CSL_MCASP_RGBLCTL_RFRST_MASK)
              >> CSL_MCASP_RGBLCTL_RFRST_SHIFT);

    smFsRcv = (Uint8)(smFsRcv << 4);

    smFsRcv |= (Uint8)(((hMcasp->regs)->RGBLCTL
                & CSL_MCASP_RGBLCTL_RSMRST_MASK)
                >> CSL_MCASP_RGBLCTL_RSMRST_SHIFT);

    return (smFsRcv);
}

/**
 * \brief   This function returns the XSMRST and XFRST field values of XGBLCTL
 *          register.
 *
 * \param   hMcasp [IN] Handle to the McASP hardware info structure
 *
 * \return
 *          0x00        - Both transmit frame generator sync and transmit state
 *                        machine are reset.
 *          0x1         - Only transmit state machine is active.
 *          0x10        - Only transmit frame sync generator is active.
 *          0x11        - Both transmit frame generator sync and transmit state
 *                        machine are active.
 * \entry   hMcasp      should be non NULL and valid pointer
 *
 * \leave   Not implemented
 */
static Uint8 mcaspGetSmFsXmt(Mcasp_HwHandle hMcasp)
{
    Uint8   smFsXmt = 0;

    assert(NULL != hMcasp);

    smFsXmt = (Uint8)(((hMcasp->regs)->XGBLCTL
              & CSL_MCASP_XGBLCTL_XFRST_MASK)
              >> CSL_MCASP_XGBLCTL_XFRST_SHIFT);

    smFsXmt = (Uint8) (smFsXmt << 4);

    smFsXmt |= (Uint8)(((hMcasp->regs)->XGBLCTL
                & CSL_MCASP_XGBLCTL_XSMRST_MASK)
                >> CSL_MCASP_XGBLCTL_XSMRST_SHIFT);

    return (smFsXmt);
}

/**
 *
 * \brief   This function configures the sample rate for the Mcasp.
 *
 * \param   chanHandle  [IN]  handle to the channel
 * \param   sampleRate  [IN]  sample rate to be configured.
 * \param   eb          [OUT] pointer to the application supplied error block
 *
 * \return  IOM_COMPLETED if success or else IOM_EBADARGS
 *
 * \entry   chanHandle  is a valid non NULL pointer
 *          sampleRate  is a valid supported sample rate
 *          eb          can be any value,in case the value is non null then the
 *                      application will receive updated error information block
 *                      in case of an error, if eb is NULL the program will
 *                      terminate the execution at the place where Error has
 *                      occured and an appropriate error message is logged.
 *
 * \leave   Not implemented
 */
static Int32 mcaspSetSamplingRate(Mcasp_ChannelHandle  chanHandle,
                                  Ptr                  ioctlArg)
{
    Mcasp_Object  *instHandle = NULL;
    Mcasp_HwSetup *hwSetUp    = NULL;
    Int32          status     = IOM_COMPLETED;

    assert((NULL != chanHandle) && (NULL != ioctlArg));

    instHandle = (Mcasp_Object *)chanHandle->devHandle;

    assert(NULL != instHandle);

    hwSetUp = (Mcasp_HwSetup *)ioctlArg;

    /* These values will not be changed hence we will take a copy of these    *
     * and reset the same values in the configuring of the TX ans RX sections */
    hwSetUp->tx.mask      = instHandle->hwInfo.regs->XMASK;
    hwSetUp->tx.fmt       = instHandle->hwInfo.regs->XFMT;
    hwSetUp->tx.frSyncCtl = instHandle->hwInfo.regs->AFSXCTL;
    hwSetUp->tx.tdm       = instHandle->hwInfo.regs->XTDM;
    hwSetUp->tx.intCtl    = instHandle->hwInfo.regs->XINTCTL;
    hwSetUp->tx.evtCtl    = instHandle->hwInfo.regs->XEVTCTL;

    status = Mcasp_localConfigXmtSection(instHandle, &hwSetUp->tx);

    if (IOM_COMPLETED == status)
    {
        hwSetUp->rx.mask      = instHandle->hwInfo.regs->RMASK;
        hwSetUp->rx.fmt       = instHandle->hwInfo.regs->RFMT;
        hwSetUp->rx.frSyncCtl = instHandle->hwInfo.regs->AFSRCTL;
        hwSetUp->rx.tdm       = instHandle->hwInfo.regs->RTDM;
        hwSetUp->rx.intCtl    = instHandle->hwInfo.regs->RINTCTL;
        hwSetUp->rx.evtCtl    = instHandle->hwInfo.regs->REVTCTL;

        status = Mcasp_localConfigRcvSection(instHandle, &hwSetUp->rx);
    }
    return status;
}


/**
 *
 * \brief   This function gets the device related information of the mcasp.
 *
 * \param   chanHandle  [IN]  handle to the channel
 * \param   arg         [IN]  pointer to the Mcasp_AudioDevData structure
 *                            supplied by the application.
 *
 * \return  None
 *
 * \entry   chanHandle  is a valid non NULL pointer
 *          arg         is a valid non NULL pointer
 *
 * \leave   Not implemented
 */
static Void mcaspDeviceInfoGet(Mcasp_ChannelHandle  chanHandle,
                               Ptr                  arg)
{
    Mcasp_Object       *instHandle = NULL;
    Mcasp_AudioDevData *devData    = NULL;
    Bool                isMaster   = FALSE;

    assert((chanHandle != NULL) && (NULL != arg));

    devData = (Mcasp_AudioDevData *)arg;

    instHandle = (Mcasp_Object *)chanHandle->devHandle;

    assert(NULL != instHandle);

    /* check if the bit clock of the TX section is generated internally       */
    if (CSL_MCASP_ACLKXCTL_CLKXM_INTERNAL ==
        (((instHandle->hwInfo.regs)->ACLKXCTL & CSL_MCASP_ACLKXCTL_CLKXM_MASK)
            >> CSL_MCASP_ACLKXCTL_CLKXM_SHIFT))
    {
        /* check if both the sections are in sync                             */
        if (CSL_MCASP_ACLKXCTL_ASYNC_SYNC ==
        (((instHandle->hwInfo.regs)->ACLKXCTL & CSL_MCASP_ACLKXCTL_ASYNC_MASK)
            >> CSL_MCASP_ACLKXCTL_ASYNC_SHIFT))
        {
            /* check if the TX and RX sections are in sync                    */
            isMaster = TRUE;
        }
        else
        {
            /* check if the transmit bit clock is generated internally        */
            if (CSL_MCASP_ACLKRCTL_CLKRM_INTERNAL ==
                (((instHandle->hwInfo.regs)->ACLKRCTL
                    & CSL_MCASP_ACLKRCTL_CLKRM_MASK)
                        >> CSL_MCASP_ACLKRCTL_CLKRM_SHIFT))
            {
                isMaster = TRUE;
            }
        }
    }
    /* update the information to the data structure                           */
    devData->isMaster = isMaster;
}

/* ========================================================================== */
/*                             END OF FILE                                    */
/* ========================================================================== */
