/** ============================================================================
 *  @file   zcpy_data.c
 *
 *  @path   $(DSPLINK)/dsp/src/data/DspBios/
 *
 *  @desc   Data streaming driver implementing zero copy data streaming.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <swi.h>
#include <iom.h>
#include <gbl.h>
#if defined (DSP_TSK_MODE)
#include <tsk.h>
#include <sem.h>
#endif /* if defined (DSP_TSK_MODE) */

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <failure.h>
#include <dbc.h>
#include <hal_interrupt.h>
#include <hal_cache.h>
#include <_dsplink.h>
#include <mpcs.h>
#include <_mpcs.h>
#include <ips.h>
#include <dsplinkdata.h>
#include <zcpy_data.h>
#include <_zcpy_data.h>
#include <_bitops.h>

/*  ----------------------------------- LOG  Headers                 */
#if defined (LOG_COMPONENT)
#include <linklog.h>
#endif /* if defined (LOG_COMPONENT) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  ZCPYDATA_MEM_ALIGN
 *
 *  @desc   Default alignment to be used for local memory allocations.
 *  ============================================================================
 */
#define ZCPYDATA_MEM_ALIGN  0

/*  ============================================================================
 *  @const  IO Completion State flags.
 *
 *  @desc   Status of completion.
 *  ============================================================================
 */
#define IOCSTATE_COMPLETE     0x0000  /* IO Completed.             */
#define IOCSTATE_PENDING      0x0002  /* IO is pending.            */
#define IOCSTATE_CANCELED     0x0004  /* IO was cancelled          */
#define IOCSTATE_TIMEOUT      0x0008  /* Wait for IOC timed out.   */
#define IOCSTATE_INPROGRESS   0x0010  /* IO is in progress between
                                       * GPP and DSP
                                       */
#define IOCSTATE_EOS          0x8000  /* End Of Stream reached.    */

/*  ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define FILEID  FID_ZCPYDATA_C

/*  ============================================================================
 *  @const  CHNLID_INVALID
 *
 *  @desc   Indicates invalid channel id.
 *  ============================================================================
 */
#define CHNLID_INVALID  0xFFFF

/*  ============================================================================
 *  @macro  MAX_IOM_PACKET_SIZE
 *
 *  @desc   Data buffer size is limited to 0xFFFF MAUs due to restrictions on
 *          packet size
 *  ============================================================================
 */
#define MAX_IOM_PACKET_SIZE   0xFFFF

/*  ============================================================================
 *  @macro  MAX
 *
 *  @desc   Gets the maximum of two values.
 *  ============================================================================
 */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))


/*  ============================================================================
 *  @name   ZCPYDATA_DevObject_tag
 *
 *  @desc   Forward declaration of ZCPYDATA_DevObject_tag structure.
 *  ============================================================================
 */
struct ZCPYDATA_DevObject_tag ;

/** ============================================================================
 *  @name   ZCPYDATA_ChannelObject
 *
 *  @desc   Channel object of LINK device.
 *
 *  @field  inUse
 *              Non zero value means this channel is in use.
 *  @field  chnlId
 *              Channel identifier.
 *  @field  mode
 *              Mode of channel. Mode can be input or output.
 *  @field  dev
 *              Reference to LINK device structure.
 *  @field  pendingIOQue
 *              Queue for pending IO packets.
 *  @field  flushPacket
 *              In case flush is pending on this channel. This field contains
 *              the flush packet.
 *  @field  cbFxn
 *              IOM callback function.
 *  @field  cbArg
 *              Argument to callback function.
 *  @field  maxBufferSize
 *              Maximum size of buffer that this channel supports.
 *              Defined for debug build only.
 *  @field  maxPendingIOs
 *              Maximum pending IOs that this channel can have.
 *              Defined for debug build only.
 *  @field  currentPendingIOs
 *              Number of pending IOs on this channel.
 *              Defined for debug build only.
 *  ============================================================================
 */
typedef struct ZCPYDATA_ChannelObject_tag {
    Uns                             inUse ;
    Uns                             chnlId ;
    Uns                             mode ;
    struct ZCPYDATA_DevObject_tag * dev ;
    QUE_Obj                         pendingIOQue ;
    IOM_Packet *                    flushPacket ;
    IOM_TiomCallback                cbFxn ;
    Ptr                             cbArg ;
#if defined(_DEBUG)
    Uns                             maxBufferSize ;
    Int                             maxPendingIOs ;
    Uns                             currentPendingIOs ;
#endif /* if defined(_DEBUG) */
} ZCPYDATA_ChannelObject ;

/** ============================================================================
 *  @name   ZCPYDATA_DevObject
 *
 *  @desc   LINK device structure.
 *
 *  @field  inUse
 *              Non zero value means this LINK device is in use.
 *  @field  devId
 *              Data driver ID.
 *  @field  ipsId
 *              IPS ID associated with the data driver.
 *  @field  ipsEventNo
 *              IPS event number associated with the data driver.
 *  @field  numChannels
 *              Maximum channels supported by this device.
 *  @field  outputMask
 *              Indicates on which channels output buffer available.
 *  @field  ongoingOutputMask
 *              Indicates on which channels output data transfer is ongoing.
 *  @field  lastOutputChannel
 *              Variable indicating on which channel last output was done.
 *  @field  ctrlPtr
 *              Pointer to shared memory control structure.
 *  @field  chnlObj
 *              Arrary of channel objects that belong to this device.
 *  @field  zcpyDataSem
 *              Semaphore object for TSK mode.
 *  @field  tskPriority
 *              Task Priority
 *  ============================================================================
 */
typedef struct ZCPYDATA_DevObject_tag {
    Uns                      inUse ;
    Uns                      devId ;
    Uint32                   ipsId ;
    Uint32                   ipsEventNo ;
    Uint32                   tskPriority ;
    Uns                      numChannels ;
    Uns                      outputMask ;
    Uns                      ongoingOutputMask ;
    Uns                      lastOutputChannel ;
    ZCPYDATA_Ctrl *          ctrlPtr ;
    ZCPYDATA_ChannelObject * chnlObj ;
#if defined (DSP_TSK_MODE)
    SEM_Obj                  zcpyDataSem ;
#endif /* if defined (DSP_TSK_MODE) */
} ZCPYDATA_DevObject ;


/** ============================================================================
 *  @name   ZCPYDATA_devObj
 *
 *  @desc   LINK device structure.
 *  ============================================================================
 */
ZCPYDATA_DevObject ZCPYDATA_devObj ;

/** ============================================================================
 *  @name   ZCPYDATA_SWI_OBJ
 *
 *  @desc   Static SWI object used by DSPLINK.
 *  ============================================================================
 */
extern SWI_Obj ZCPYDATA_SWI_OBJ ;

/** ============================================================================
 *  @name   DSPLINKDATA_ctrlPtr
 *
 *  @desc   Declaration of pointer to the shared memory control structure for
 *          the DSPLINKDATA component.
 *  ============================================================================
 */
extern DSPLINKDATA_Ctrl * DSPLINKDATA_ctrlPtr ;

/** ============================================================================
 *  @name   DSPLINK_shmBaseAddress
 *
 *  @desc   Declaration of base address for the DSPLINK shared memory.
 *  ============================================================================
 */
extern Uint32 DSPLINK_shmBaseAddress ;

/** ============================================================================
 *  @func   atoi
 *
 *  @desc   Converts character string to integer value.
 *
 *  @arg    str
 *              Pointer to character string.
 *
 *  @ret    <integer value>
 *              Converted integer value.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
extern
int
atoi (const char * str) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_mdBindDev
 *
 *  @desc   Allocates resources needed for intialization of this device.
 *
 *  @arg    devp
 *              Output device structure.
 *          devid
 *              Device number. Not being used for LINK.
 *          devParams
 *              Device parameters.
 *
 *  @ret    IOM_COMPLETED
 *              Successful initialization.
 *          IOM_EINUSE
 *              Device already in use.
 *          IOM_EBADIO
 *              General failure during initialization.
 *          IOM_EBADARGS
 *              Invalid argument passed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYDATA_mdBindDev (Ptr * devp, Int devid, Ptr devParams) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_mdCreateChan
 *
 *  @desc   Creates a new channel on given device.
 *
 *  @arg    chanp
 *              Channel pointer where to put newly created channel.
 *          devp
 *              Device on which to create channel.
 *          name
 *              Channel number as character string.
 *          mode
 *              Mode of channel.
 *          chanParams
 *              Channel parameters.
 *          cbFxn
 *              IOM callback function.
 *          cbArg
 *              Argument to IOM callback function.
 *
 *  @ret    IOM_COMPLETED
 *              Successful completion of function.
 *          IOM_EINUSE
 *              Device already in use.
 *          IOM_EBADIO
 *              General failure during operation.
 *          IOM_EBADARGS
 *              Invalid argument passed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYDATA_mdCreateChan (Ptr *            chanp,
                       Ptr              devp,
                       String           name,
                       Int              mode,
                       Ptr              chanParams,
                       IOM_TiomCallback cbFxn,
                       Ptr              cbArg) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_mdDeleteChan
 *
 *  @desc   Deletes specified channel.
 *
 *  @arg    chanp
 *              Channel to be deleted.
 *
 *  @ret    IOM_COMPLETED
 *              Successful completion of function.
 *          IOM_EINUSE
 *              Device already in use.
 *          IOM_EBADIO
 *              General failure during operation.
 *          IOM_EBADARGS
 *              Invalid argument passed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYDATA_mdDeleteChan (Ptr chanp) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_mdSubmitChan
 *
 *  @desc   Submits a command or IO request on a channel.
 *
 *  @arg    chanp
 *              Channel on which to perform action.
 *          packet
 *              IOM Request packet.
 *
 *  @ret    IOM_COMPLETED
 *              Successful completion of function.
 *          IOM_EINUSE
 *              Device already in use.
 *          IOM_EBADIO
 *              General failure during operation.
 *          IOM_EBADARGS
 *              Invalid argument passed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYDATA_mdSubmitChan (Ptr chanp, IOM_Packet * packet) ;


/** ----------------------------------------------------------------------------
 *  @func   flushOutput
 *
 *  @desc   Flushes all the pending IOs on this channel. Note that this function
 *          is only called for output channels.
 *
 *  @arg    chnlObj
 *              Channel object pointer.
 *          flushPacket
 *              Flush packet.
 *
 *  @ret    IOM_COMPLETED
 *              Flush request completed successfully.
 *          IOM_PENDING
 *              Flush request is pending for completion at later point of time.
 *
 *  @enter  flushPacket in the argument is not NULL.
 *          chnlObj pointer is not NULL.
 *          There is no flush packet already present for this channel.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static Int flushOutput (ZCPYDATA_ChannelObject * chnlObj,
                        IOM_Packet *             flushPacket);


/** ----------------------------------------------------------------------------
 *  @func   abortio
 *
 *  @desc   Aborts all the pending IOs on this channel.
 *
 *  @arg    chnlObj
 *              Channel object pointer.
 *  @arg    status
 *              Status with which the packet is to be aborted.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static
Void
abortio (ZCPYDATA_ChannelObject * chnlObj, int retStatus) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_callback
 *
 *  @desc   This function implements the callback invoked by the SHMIPS
 *          component on receiving a data transfer request from the remote
 *          processor.
 *
 *  @arg    eventNo
 *              Event number associated with the callback being invoked.
 *  @arg    arg
 *              Argument registered with SHMIPS component.
 *  @arg    info
 *              info from SHMIPS component.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 * ----------------------------------------------------------------------------
 */
static
Void
ZCPYDATA_callback (Uint32 eventNo, Ptr arg, Ptr info) ;


#if defined (DSP_TSK_MODE)
/** ============================================================================
 *  @func   ZCPYDATA_tskFxn
 *
 *  @desc   ZCPYDATA_tskFxn function for data transfer in DSPLINK.
 *
 *  @arg    arg0
 *              First argument passed to TSK.
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
Void
ZCPYDATA_tskFxn (Arg arg0) ;
#endif /* if defined (DSP_TSK_MODE) */


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_dataCtrl
 *
 *  @desc   Data control function for data transfer in DSPLINK.
 *
 *  @arg    dev
 *              Pointer to dev object structure.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 * ----------------------------------------------------------------------------
 */
static
Void
ZCPYDATA_dataCtrl (ZCPYDATA_DevObject * dev) ;


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_Send
 *
 *  @desc   Used for sending events.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Uint32
ZCPYDATA_send (Ptr buffer) ;


/** ----------------------------------------------------------------------------
 *  @func   selectOutputChannel
 *
 *  @desc   Selects the channel which is ready to output and on which GPP side
 *          is also ready to receive.
 *
 *  @arg    dev
 *              Device for which output is to be performed.
 *  @arg    mask
 *              Mask in which to look for ready channel.
 *
 *  @ret    >= (dev->numChannels + NUM_MSGCHNL)
 *              No channel is ready for output.
 *          else
 *              ID of the selected channel that is ready for output.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static
Uns
selectOutputChannel (ZCPYDATA_DevObject * dev, Uint32 mask) ;


#if defined (DSP_SWI_MODE)
/** ============================================================================
 *  @func   ZCPYDATA_SWI
 *
 *  @desc   SWI function for data transfer in DSPLINK.
 *
 *  @arg    arg0
 *              First argument passed to SWI.
 *  @arg    arg1
 *              Second argument passed to SWI.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
Void
ZCPYDATA_SWI (Arg arg0, Arg arg1) ;
#endif /* if defined (DSP_SWI_MODE) */


/** ============================================================================
 *  @name   ZCPYDATA_FXNS
 *
 *  @desc   DSPLINK function pointer table. Unbind device and control channel
 *          functions are not implemented.
 *  ============================================================================
 */
IOM_Fxns ZCPYDATA_FXNS = {
    ZCPYDATA_mdBindDev,
    IOM_UNBINDDEVNOTIMPL,
    IOM_CONTROLCHANNOTIMPL,
    ZCPYDATA_mdCreateChan,
    ZCPYDATA_mdDeleteChan,
    ZCPYDATA_mdSubmitChan
} ;

/** ============================================================================
 *  @name   ZCPYDATA_defaultChanParams
 *
 *  @desc   Default channel parameters.
 *  ============================================================================
 */
const DSPLINK_ChnlParams ZCPYDATA_defaultChanParams = {
    -1, /* Maximum SHM size */
    -1  /* Infinite buffer queueing */
} ;


/*  ============================================================================
 *  Create named sections for the functions to allow specific memory placement.
 *  ============================================================================
 */
#pragma CODE_SECTION (ZCPYDATA_init,         ".text:init")
#pragma CODE_SECTION (ZCPYDATA_mdBindDev,    ".text:ZCPYDATA_init")
#pragma CODE_SECTION (ZCPYDATA_mdCreateChan, ".text:DSPLINK_create")
#pragma CODE_SECTION (ZCPYDATA_mdDeleteChan, ".text:DSPLINK_delete")


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_mdBindDev
 *
 *  @desc   Allocates resources needed for working of the device. This function
 *          returns device structure in output parameter devp.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYDATA_mdBindDev (Ptr * devp, Int devid, Ptr devParams)
{
    Int                 status           = IOM_COMPLETED ;
    MPCS_Handle         temp             = NULL ;
    DRV_Ctrl *          drvCtrl          = (DRV_Ctrl *) DSPLINK_shmBaseAddress ;

    (Void) devParams ; /* To avoid compiler warning. */

    DBC_require (devp != NULL) ;

    if (ZCPYDATA_devObj.inUse == 1) {
        /* Dev object already in use... */
        status = IOM_EINUSE ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        ZCPYDATA_devObj.inUse       = 1 ;
        ZCPYDATA_devObj.devId       = devid ;
        ZCPYDATA_devObj.numChannels =
                               DSPLINKDATA_ctrlPtr [devid].config.numChannels ;
        ZCPYDATA_devObj.ipsId       = DSPLINKDATA_ctrlPtr [devid].config.ipsId ;
        ZCPYDATA_devObj.ipsEventNo  =
                                DSPLINKDATA_ctrlPtr [devid].config.ipsEventNo ;
        ZCPYDATA_devObj.tskPriority =
                                DSPLINKDATA_ctrlPtr [devid].config.arg1;
        ZCPYDATA_devObj.chnlObj = MEM_calloc (DSPLINK_SEGID,
                                             (   sizeof (ZCPYDATA_ChannelObject)
                                              *  ZCPYDATA_devObj.numChannels),
                                             ZCPYDATA_MEM_ALIGN) ;
        if (ZCPYDATA_devObj.chnlObj == MEM_ILLEGAL) {
            status = SYS_EALLOC ;
            SET_FAILURE_REASON (status) ;
        }
        else {
            *devp = &ZCPYDATA_devObj ;

            ZCPYDATA_devObj.ctrlPtr = (ZCPYDATA_Ctrl *)
                                           DSPLINKDATA_ctrlPtr [devid].dspAddr ;

#if defined (DSP_TSK_MODE)
            SEM_new (&(ZCPYDATA_devObj.zcpyDataSem), 0) ;
#endif /* if defined (DSP_TSK_MODE) */
            /* -----------------------------------------------------------------
             * Register Callback function with IPS component
             * -----------------------------------------------------------------
             */
            status = IPS_register (ID_GPP,
                                   ZCPYDATA_devObj.ipsId,
                                   ZCPYDATA_devObj.ipsEventNo,
                                   ZCPYDATA_callback,
                                   &(ZCPYDATA_devObj)) ;
            DBC_assert (status == SYS_OK) ;

            if (status == SYS_OK) {
                status  = IPS_setReserved (ID_GPP,
                                           ZCPYDATA_devObj.ipsId,
                                           ZCPYDATA_devObj.ipsEventNo) ;
                DBC_assert (status == SYS_OK) ;

                /* The MPCS objects are created by the GPP-side. */
                status = _MPCS_open (ID_GPP,
                                     MPCS_RESV_LOCKNAME,
                                     &temp,
                                     (MPCS_ShObj *) &(ZCPYDATA_devObj.ctrlPtr->csToDspList)) ;
                DBC_assert (status == SYS_OK) ;

                status = _MPCS_open (ID_GPP,
                                     MPCS_RESV_LOCKNAME,
                                     &temp,
                                     (MPCS_ShObj *) &(ZCPYDATA_devObj.ctrlPtr->csFmDspList)) ;
                DBC_assert (status == SYS_OK) ;

                status = _MPCS_open (ID_GPP,
                        MPCS_RESV_LOCKNAME,
                        &temp,
                        (MPCS_ShObj *) &(ZCPYDATA_devObj.ctrlPtr->csFreeChirps)) ;
                DBC_assert (status == SYS_OK) ;
            }
        }
    }

    drvCtrl->dataDspInitDone = (Uint32) status ;
    HAL_cacheWbInv ((Void *) drvCtrl, sizeof (DRV_Ctrl)) ;

    /* Handshake with the GPP */
    DSPLINK_handshake (ID_GPP, DRV_HANDSHAKE_DATA) ;

    DBC_ensure ((status != IOM_COMPLETED) || (*devp != NULL)) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_mdCreateChan
 *
 *  @desc   Creates a new channel on given device.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYDATA_mdCreateChan (Ptr *               chanp,
                       Ptr                 devp,
                       String              name,
                       Int                 mode,
                       Ptr                 chanParams,
                       IOM_TiomCallback    cbFxn,
                       Ptr                 cbArg)
{
    Int                  status   = IOM_COMPLETED ;
    DSPLINK_ChnlParams * chanAttr = chanParams ;
    ZCPYDATA_DevObject * dev      = devp ;
    LgUns                chnlId ;
    Uns                  maxBufSize ;

    DBC_require (chanp != NULL) ;
    DBC_require (dev   != NULL) ;

    if (chanAttr == NULL) {
        chanAttr = (DSPLINK_ChnlParams *) &ZCPYDATA_defaultChanParams ;
    }

    maxBufSize = MAX (DSPLINKDATA_ctrlPtr [dev->devId].config.maxBufSize,
                      MAX_IOM_PACKET_SIZE) ;
    if (chanAttr->maxBufferSize == -1) {
        chanAttr->maxBufferSize = maxBufSize ;
    }

    *chanp = NULL ;
    chnlId = atoi (name) ;

    if (   (chnlId                  >= dev->numChannels)
        || (chanAttr->maxBufferSize > maxBufSize)) {
        status = IOM_EBADARGS ;
        SET_FAILURE_REASON (status) ;
    }
    else if (dev->chnlObj [chnlId].inUse) {
        status = IOM_EINUSE ;
        SET_FAILURE_REASON (status) ;
    }
    else if (mode == IOM_INOUT) {
        status = IOM_EBADMODE ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        dev->chnlObj [chnlId].inUse = 1 ;
        dev->chnlObj [chnlId].mode  = mode ;
#if defined (_DEBUG)
        dev->chnlObj [chnlId].maxPendingIOs     = chanAttr->maxPendingIOs ;
        dev->chnlObj [chnlId].currentPendingIOs = 0 ;
        dev->chnlObj [chnlId].maxBufferSize     = chanAttr->maxBufferSize ;
#endif /* if defined (_DEBUG) */
        dev->chnlObj [chnlId].cbFxn             = cbFxn ;
        dev->chnlObj [chnlId].cbArg             = cbArg ;
        dev->chnlObj [chnlId].dev               = (Ptr)dev ;
        dev->chnlObj [chnlId].chnlId            = chnlId ;
        dev->chnlObj [chnlId].flushPacket       = NULL ;

        QUE_new (&(dev->chnlObj [chnlId].pendingIOQue)) ;
        *chanp = &(dev->chnlObj [chnlId]) ;
    }

    DBC_ensure ((status != IOM_COMPLETED) || (*chanp != NULL)) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_mdDeleteChan
 *
 *  @desc   Deletes a channel.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYDATA_mdDeleteChan (Ptr chanp)
{
    Int                      status     = IOM_COMPLETED ;
    ZCPYDATA_ChannelObject * chnlObj    = chanp ;

    DBC_require (chanp != NULL) ;

    chnlObj->inUse = 0 ;

    DBC_ensure ((status != IOM_COMPLETED) || (chnlObj->inUse == 0)) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_mdSubmitChan
 *
 *  @desc   Submit a command or IO request on a channel.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Int
ZCPYDATA_mdSubmitChan (Ptr chanp, IOM_Packet * packet)
{
    Int                      status  = IOM_PENDING ;
    ZCPYDATA_ChannelObject * chnlObj = chanp ;
    ZCPYDATA_Ctrl *          ctrlPtr = NULL ;
#if defined (LOG_COMPONENT)
    Uint32                   temp ;
#endif

    DBC_require (packet != NULL) ;
    DBC_require (chanp != NULL) ;

    ctrlPtr = (chnlObj->dev->ctrlPtr) ;

    switch (packet->cmd) {
        case IOM_FLUSH:
        {
            DBC_assert (chnlObj->flushPacket == NULL) ; /* One flush request is
                                                         * already pending.
                                                         */
            if (chnlObj->mode == IOM_OUTPUT) {
                status = flushOutput (chnlObj, packet);
            }
            else {
                /* In case of input channel flush is same as abort */
                ctrlPtr->dspFreeMask [chnlObj->chnlId].bitValue = 0 ;
                HAL_cacheWbInv ((Ptr) &(ctrlPtr->dspFreeMask [chnlObj->chnlId]),
                                sizeof (ZCPYDATA_ChnlMask)) ;

                abortio (chnlObj, IOM_FLUSHED) ;
                packet->status = IOM_COMPLETED ;
                status = IOM_COMPLETED ;
            }
        }
        break ;

        case IOM_ABORT:
        {
            if (chnlObj->mode == IOM_OUTPUT) {
                CLEAR_BIT (chnlObj->dev->outputMask, chnlObj->chnlId) ;
            }
            else {
                ctrlPtr->dspFreeMask [chnlObj->chnlId].bitValue = 0 ;
                HAL_cacheWbInv ((Ptr) &(ctrlPtr->dspFreeMask [chnlObj->chnlId]),
                                sizeof (ZCPYDATA_ChnlMask)) ;

            }
            abortio (chnlObj, IOM_ABORTED) ;
            packet->status = IOM_COMPLETED ;
            status = IOM_COMPLETED ;
        }
        break ;

        case IOM_READ: /* Fall through */
        case IOM_WRITE:
        {
            DBC_assert (   (chnlObj->maxPendingIOs == -1)
                        || (    chnlObj->currentPendingIOs
                            <   chnlObj->maxPendingIOs)) ;
            DBC_assert (packet != NULL) ;
            DBC_assert (packet->size <= chnlObj->maxBufferSize) ;
            DBC_assert (    (  (packet->cmd    == IOM_READ)
                            && (chnlObj->mode  == IOM_INPUT))
                        ||  (  (packet->cmd    == IOM_WRITE)
                            && (chnlObj->mode  == IOM_OUTPUT))) ;
            DBC_assert (    (   (packet->cmd          == IOM_WRITE)
                             && (chnlObj->flushPacket == NULL))
                        ||  (packet->cmd == IOM_READ)) ;  /* Cannot accept write
                                                           * request when flush
                                                           * request is pending
                                                           * on this channel
                                                           */
#if defined (DSP_TSK_MODE)
            TSK_disable () ;
#else /* if defined (DSP_TSK_MODE) */
            SWI_disable () ;
#endif /* if defined (DSP_TSK_MODE) */
            QUE_put (&chnlObj->pendingIOQue, packet) ;

#if defined (LOG_DG_CHNL_I_QUE)
            temp = ((GBL_getProcId () << 16) & 0xFFFF0000) | chnlObj->chnlId ;
           /* Log the event */
            DSPLINKLOG_logEvent (DG_CHNL_I_QUE,
                                 0,
                                 temp,
                                 (Uint32) packet->addr,
                                 packet->size,
                                 0,
                                 0) ;
#endif /* if defined (LOG_DG_CHNL_I_QUE) */

#if defined(_DEBUG)
            chnlObj->currentPendingIOs++;
#endif /* if defined(_DEBUG) */
            if (packet->cmd == IOM_READ) {
                ctrlPtr->dspFreeMask [chnlObj->chnlId].bitValue = 1 ;
                HAL_cacheWbInv ((Ptr) &(ctrlPtr->dspFreeMask [chnlObj->chnlId]),
                                sizeof (ZCPYDATA_ChnlMask)) ;

                IPS_notify (ID_GPP,
                            chnlObj->dev->ipsId,
                            chnlObj->dev->ipsEventNo,
                            (Uint16) 0,
                            FALSE) ;
            }
            else {
                SET_BIT (chnlObj->dev->outputMask, chnlObj->chnlId) ;
#if defined (DSP_TSK_MODE)
                SEM_post (&(chnlObj->dev->zcpyDataSem)) ;
#else /* if defined (DSP_TSK_MODE) */
                SWI_inc (&ZCPYDATA_SWI_OBJ) ;
#endif /* if defined (DSP_TSK_MODE) */
            }

#if defined (DSP_TSK_MODE)
            TSK_enable () ;
#else /* if defined (DSP_TSK_MODE) */
            SWI_enable () ;
#endif /* if defined (DSP_TSK_MODE) */
        }
        break ;

        default:
        {
            status = IOM_EBADIO ;
            SET_FAILURE_REASON (status) ;
        }
        break ;
    }

    return status ;
}


#if defined (DSP_SWI_MODE)
/** ============================================================================
 *  @func   ZCPYDATA_SWI
 *
 *  @desc   SWI function for data transfer in DSPLINK.
 *
 *  @modif  None
 *  ============================================================================
 */
Void
ZCPYDATA_SWI (Arg arg0, Arg arg1)
{
    ZCPYDATA_DevObject *     dev     = (ZCPYDATA_DevObject *) arg0 ;
    Uns                      swiCount ;

    (Void) arg1 ;

    DBC_require (dev != NULL) ;
    swiCount = SWI_getmbox () ;
    while (swiCount != 0) {
        swiCount-- ;
        ZCPYDATA_dataCtrl (dev) ;
    }
}
#endif /* if defined (DSP_SWI_MODE) */


#if defined (DSP_TSK_MODE)
/** ============================================================================
 *  @func   ZCPYDATA_tskFxn
 *
 *  @desc   TSK function for data transfer in DSPLINK.
 *
 *  @modif  None
 *  ============================================================================
 */
Void
ZCPYDATA_tskFxn (Arg arg0)
{
    ZCPYDATA_DevObject *     dev = (ZCPYDATA_DevObject *) arg0 ;
    Bool                     i   = 1 ;
    Int                      status  = SYS_OK ;

    if (dev->tskPriority != 0) {

        if (   (dev->tskPriority > TSK_MINPRI)
            && (dev->tskPriority <= TSK_MAXPRI)) {
            TSK_setpri (TSK_self (), dev->tskPriority) ;
        }
        else {
            status = IOM_EBADARGS ;
            SET_FAILURE_REASON (status) ;
        }
    }

    DBC_require (dev != NULL) ;

    while (i) {
        SEM_pend(&(dev->zcpyDataSem), SYS_FOREVER) ;
        ZCPYDATA_dataCtrl (dev) ;
    }
}
#endif /* if defined (DSP_TSK_MODE) */


/** ============================================================================
 *  @func   ZCPYDATA_dataCtrl
 *
 *  @desc   Data control function for data transfer in DSPLINK.
 *
 *  @modif  None
 *  ============================================================================
 */
static
Void
ZCPYDATA_dataCtrl (ZCPYDATA_DevObject * dev)
{
    ZCPYDATA_Ctrl *          ctrlPtr = dev->ctrlPtr ;
    ZCPYDATA_ChannelObject * chnlObj ;
    IOM_Packet  *            packet ;
    LgUns                    chnlId ;
    CHNL_Irp *               chirp ;
    CHNL_Irp                 tmpChirp ;
    QUE_Elem *               tmpPtr ;
#if defined (LOG_COMPONENT)
    Uint32                   temp ;
#endif
        HAL_cacheInv ((Ptr) &(ctrlPtr->toDspList),sizeof (ctrlPtr->toDspList)) ;
        while (!QUE_empty (&(ctrlPtr->toDspList))) {
            MPCS_enter ((MPCS_Handle) &(ctrlPtr->csToDspList)) ;

            /* Ensure that all three queue elements involved in QUE_get () are
             * invalidated.
             */
            HAL_cacheInv ((Ptr) &(ctrlPtr->toDspList),
                          sizeof (ctrlPtr->toDspList)) ;
            tmpPtr = QUE_next (&(ctrlPtr->toDspList)) ;
            HAL_cacheInv ((Ptr) tmpPtr, sizeof (CHNL_Irp)) ;
            tmpPtr = QUE_next (tmpPtr) ;
            HAL_cacheInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;

            chirp = QUE_get ((QUE_Handle) &(ctrlPtr->toDspList)) ;

            /* Ensure that both queue elements involved in QUE_get () are
             * written back.
             */
            HAL_cacheWbInv ((Ptr) &(ctrlPtr->toDspList),
                            sizeof (ctrlPtr->toDspList)) ;
            tmpPtr = QUE_next (&(ctrlPtr->toDspList)) ;
            HAL_cacheWbInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;

            MPCS_leave ((MPCS_Handle) &(ctrlPtr->csToDspList)) ;

            if ((QUE_Handle) chirp != &(ctrlPtr->toDspList)) {
                chnlId  = chirp->chnlId ;
#if defined (LOG_DG_CHNL_R_POST)
                temp = ((GBL_getProcId () << 16) & 0xFFFF0000) | chnlId ;
                /* Log the event */
                DSPLINKLOG_logEvent (DG_CHNL_R_POST,
                                     0,
                                     temp,
                                     (Uint32) chirp->buffer,
                                     chirp->size,
                                     0,
                                     0) ;
#endif /* if defined (LOG_DG_CHNL_R_POST) */

#if defined (LOG_GD_CHNL_XFER_PROCESSING)
                temp = ((GBL_getProcId () << 16) & 0xFFFF0000) | chnlId ;
                /* Log the event */
                DSPLINKLOG_logEvent (GD_CHNL_XFER_PROCESSING,
                                     0,
                                     temp,
                                     (Uint32) chirp->buffer,
                                     chirp->size,
                                     0,
                                     0) ;
#endif /* if defined (LOG_GD_CHNL_XFER_PROCESSING) */

                chnlObj = &(dev->chnlObj [chnlId]) ;

                /* We must have a valid packet pending if GPP sent a chirp based
                 * on our dspFreeMask
                 */
                packet = QUE_get (&chnlObj->pendingIOQue) ;
                DBC_assert ((QUE_Handle) (packet) != &chnlObj->pendingIOQue) ;
#if defined (_DEBUG)
                chnlObj->currentPendingIOs-- ;
#endif /* if defined (_DEBUG) */

                if (QUE_empty (&chnlObj->pendingIOQue)) {
                    if (chnlObj->mode == IOM_INPUT) {
                        ctrlPtr->dspFreeMask [chnlId].bitValue = 0 ;
                        HAL_cacheWbInv ((Ptr) &(ctrlPtr->dspFreeMask [chnlId]),
                                        sizeof (ZCPYDATA_ChnlMask)) ;
                    }
                    else {
                        CLEAR_BIT (dev->outputMask, chnlObj->chnlId) ;
                    }
                }

                if (chnlObj->mode == IOM_INPUT) {
                    tmpChirp.buffer = packet->addr ;
                    tmpChirp.size   = packet->size ;
                    tmpChirp.chnlId = chnlId ;
                    ctrlPtr->gppOutputFull [chnlId].bitValue = 0 ;
                    HAL_cacheWbInv ((Ptr) &(ctrlPtr->gppOutputFull [chnlId]),
                                    sizeof (ZCPYDATA_ChnlMask)) ;

                    /* Write back contents of buffer, in case user is optimizing
                     * by exchanging valid buffers for input channel.
                     */
                    HAL_cacheWbInv ((Ptr) tmpChirp.buffer, tmpChirp.size) ;
                    ZCPYDATA_send (&tmpChirp) ;
                }
                else {
                    /* Indicate that this data transfer on this channel is
                     * complete
                     */
                    CLEAR_BIT (dev->ongoingOutputMask, chnlId) ;
                }

                packet->addr   = chirp->buffer ;
                packet->size   = chirp->size ;
                packet->status = (  (chirp->iocStatus == IOCSTATE_CANCELED)
                                  ? IOM_ABORTED
                                  : IOM_COMPLETED) ;

                HAL_cacheInv ((Ptr) packet->addr, packet->size) ;
                (*chnlObj->cbFxn) (chnlObj->cbArg, packet) ;

#if defined (LOG_GD_CHNL_XFER_COMPLETED)
                temp = ((GBL_getProcId () << 16) & 0xFFFF0000) | chnlId ;
                /* Log the event */
                DSPLINKLOG_logEvent (GD_CHNL_XFER_COMPLETED,
                              0,
                              temp,
                              (Uint32) chirp->buffer,
                              chirp->size,
                              0,
                              0) ;
#endif /* if defined (LOG_GD_CHNL_XFER_COMPLETED) */

                if (QUE_empty (&chnlObj->pendingIOQue)) {
                    if (chnlObj->mode == IOM_OUTPUT) {
                        if (chnlObj->flushPacket != NULL) {
                            chnlObj->flushPacket->status = IOM_COMPLETED;
                            (*chnlObj->cbFxn)(chnlObj->cbArg,
                                              chnlObj->flushPacket) ;
                            chnlObj->flushPacket = NULL;
                        }
                    }
                }

                /*  ------------------------------------------------------------
                 *  Send the used chirp back to the free list
                 *  ------------------------------------------------------------
                 */
                MPCS_enter ((MPCS_Handle) &(ctrlPtr->csFreeChirps)) ;
                HAL_cacheInv ((Ptr) &(ctrlPtr->freeChirps),
                              sizeof (ctrlPtr->freeChirps)) ;
                tmpPtr = QUE_prev (&(ctrlPtr->freeChirps)) ;
                HAL_cacheInv (tmpPtr, sizeof (QUE_Elem)) ;

                QUE_put ((QUE_Handle) &(ctrlPtr->freeChirps), chirp) ;
                HAL_cacheWbInv ((Ptr) chirp, sizeof (CHNL_Irp)) ;
                HAL_cacheWbInv ((Ptr) &(ctrlPtr->freeChirps),
                                sizeof (ctrlPtr->freeChirps)) ;
                HAL_cacheWbInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;
                MPCS_leave ((MPCS_Handle) &(ctrlPtr->csFreeChirps)) ;
            }
        }

        /*  --------------------------------------------------------------------
         *  An invalid chirp being received is a valid condition which occurs
         *  when DSP was ready on output channel before GPP becomes ready.
         *  The SWI in this case only handles the notification from GPP
         *  (there is no chirp to handle).
         *  --------------------------------------------------------------------
         */
        chnlId = selectOutputChannel (dev,
                                      (   dev->outputMask
                                       &  ~(dev->ongoingOutputMask))) ;
        if (chnlId != CHNLID_INVALID) {
            HAL_cacheInv ((Ptr) &(dev->ctrlPtr->dspOutputFull [chnlId]),
                          sizeof (ZCPYDATA_ChnlMask)) ;

            if (dev->ctrlPtr->dspOutputFull [chnlId].bitValue == 0) {
                /*  ------------------------------------------------------------
                 *  Get the information about the next chirp.
                 *  ------------------------------------------------------------
                 */
                chnlObj = &(dev->chnlObj [chnlId]);
                packet  = QUE_head (&chnlObj->pendingIOQue) ;

                DBC_assert ((QUE_Handle) (packet) != &chnlObj->pendingIOQue) ;
                if ((QUE_Handle) (packet) != &chnlObj->pendingIOQue) {
                    tmpChirp.buffer = packet->addr ;
                    tmpChirp.size   = packet->size ;
                    tmpChirp.chnlId = chnlId ;

                    dev->ctrlPtr->dspOutputFull [chnlId].bitValue = 1 ;
                    HAL_cacheWbInv (
                                  (Ptr) &(dev->ctrlPtr->dspOutputFull [chnlId]),
                                  sizeof (ZCPYDATA_ChnlMask)) ;

#if defined (LOG_DG_CHNL_XFER_START)
                    temp = ((GBL_getProcId () << 16) & 0xFFFF0000) | chnlId ;
                    /* Log the event */
                    DSPLINKLOG_logEvent (DG_CHNL_XFER_START,
                                         0,
                                         temp,
                                         (Uint32) tmpChirp.buffer,
                                         tmpChirp.size,
                                         0,
                                         0) ;
#endif /* if defined (LOG_DG_CHNL_XFER_START) */

                    HAL_cacheWbInv ((Ptr) tmpChirp.buffer, tmpChirp.size) ;

                    /* Indicate that the transfer on this channel is ongoing.
                     * This channel would not be considered for next output
                     * till the previous data transfer on it is complete.
                     */
                    SET_BIT (dev->ongoingOutputMask,   chnlId) ;
                    ZCPYDATA_send (&tmpChirp) ;
                }
            }
        }

}


/** ============================================================================
 *  @func   ZCPYDATA_init
 *
 *  @desc   Initializes DSPLINK data driver structures before bind function.
 *
 *  @modif  None
 *  ============================================================================
 */
Void
ZCPYDATA_init ()
{
    ZCPYDATA_devObj.inUse               = 0 ;
    ZCPYDATA_devObj.devId               = 0 ;
    ZCPYDATA_devObj.ipsId               = 0 ;
    ZCPYDATA_devObj.ipsEventNo          = 0 ;
    ZCPYDATA_devObj.tskPriority         = 15 ;
    ZCPYDATA_devObj.numChannels         = 0 ;
    ZCPYDATA_devObj.outputMask          = 0 ;
    ZCPYDATA_devObj.ongoingOutputMask   = 0 ;
    ZCPYDATA_devObj.lastOutputChannel   = 0 ;
    ZCPYDATA_devObj.ctrlPtr             = NULL ;
    ZCPYDATA_devObj.chnlObj             = NULL ;

    _DSPLINK_init () ;
}


/** ----------------------------------------------------------------------------
 *  @func   flushOutput
 *
 *  @desc   Flushes all the pending IOs on this channel.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static Int flushOutput (ZCPYDATA_ChannelObject * chnlObj,
                        IOM_Packet *             flushPacket)
{
    Int status = IOM_PENDING ;

    DBC_require(flushPacket != NULL);
    DBC_require(chnlObj != NULL);
    DBC_require((chnlObj != NULL) && (chnlObj->flushPacket == NULL));

#if defined (DSP_TSK_MODE)
    TSK_disable () ;
#else /* if defined (DSP_TSK_MODE) */
    SWI_disable ();
#endif /* if defined (DSP_TSK_MODE) */

    if (QUE_empty(&chnlObj->pendingIOQue)) {
        flushPacket->status = IOM_COMPLETED ;
        status = IOM_COMPLETED ;
    }
    else {
        chnlObj->flushPacket = flushPacket ;
    }

#if defined (DSP_TSK_MODE)
    TSK_enable () ;
#else /* if defined (DSP_TSK_MODE) */
    SWI_enable ();
#endif /* if defined (DSP_TSK_MODE) */

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   abortio
 *
 *  @desc   Aborts all the pending IOs on this channel.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Void
abortio (ZCPYDATA_ChannelObject * chnlObj, int retStatus)
{
    IOM_Packet *     tmpPacket ;
    ZCPYDATA_Ctrl *  ctrlPtr = chnlObj->dev->ctrlPtr ;

#if defined (DSP_TSK_MODE)
    TSK_disable () ;
#else /* if defined (DSP_TSK_MODE) */
    SWI_disable () ;
#endif /* if defined (DSP_TSK_MODE) */

    if (ctrlPtr->dspFreeMask [chnlObj->chnlId].bitValue == 1) {
        ctrlPtr->dspFreeMask [chnlObj->chnlId].bitValue = 0 ;
        HAL_cacheWbInv ((Ptr) &(ctrlPtr->dspFreeMask [chnlObj->chnlId]),
                        sizeof (ZCPYDATA_ChnlMask)) ;

        IPS_notify (ID_GPP,
                    chnlObj->dev->ipsId,
                    chnlObj->dev->ipsEventNo,
                    (Uint16) 0,
                    FALSE) ;
    }

    while ((QUE_Handle) (tmpPacket = QUE_get (&chnlObj->pendingIOQue)) !=
                                                       &chnlObj->pendingIOQue) {
#if defined (_DEBUG)
        chnlObj->currentPendingIOs-- ;
#endif /* defined(_DEBUG) */
        tmpPacket->status = retStatus ;
        (*chnlObj->cbFxn) (chnlObj->cbArg, tmpPacket) ;
    }

#if defined (DSP_TSK_MODE)
    TSK_enable () ;
#else /* if defined (DSP_TSK_MODE) */
    SWI_enable () ;
#endif /* if defined (DSP_TSK_MODE) */
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYDATA_callback
 *
 *  @desc   This function implements the callback invoked by the SHMIPS
 *          component on receiving a data transfer request from the remote
 *          processor.
 *
 *  @modif  None
 * ----------------------------------------------------------------------------
 */
static
Void
ZCPYDATA_callback (Uint32 eventNo, Ptr arg, Ptr  info)
{
#if defined (DSP_TSK_MODE)
    ZCPYDATA_DevObject * dev      = (ZCPYDATA_DevObject *)arg ;
#else /* if defined (DSP_TSK_MODE) */
    (void) arg ;
#endif /* if defined (DSP_TSK_MODE) */
    (void) eventNo ;
    (void) info ;

#if defined (DSP_TSK_MODE)
    SEM_post (&(dev->zcpyDataSem)) ;
#else /* if defined (DSP_TSK_MODE) */
    SWI_inc (&ZCPYDATA_SWI_OBJ) ;
#endif /* if defined (DSP_TSK_MODE) */

}


/** ----------------------------------------------------------------------------
 *  @func   selectOutputChannel
 *
 *  @desc   Selects the channel which is ready to output.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Uns
selectOutputChannel (ZCPYDATA_DevObject *dev, Uint32 mask)
{
    Bool    done = FALSE ;
    Uint32  selectId ;
    Uint32  startId ;

    if (mask != 0) {
        startId  = dev->lastOutputChannel ;
        selectId = startId ;
        do {
            selectId = selectId + 1 ;
            if (selectId == dev->numChannels) {
                selectId = 0 ;
            }

            HAL_cacheInv ((Ptr *) &dev->ctrlPtr->gppFreeMask [selectId],
                          sizeof (ZCPYDATA_ChnlMask)) ;
            if (   ((mask & (1 << selectId)) != 0)
                && (dev->ctrlPtr->gppFreeMask [selectId].bitValue == 1)) {
                dev->lastOutputChannel = selectId ;
                done = TRUE ;
            }
        } while ((selectId != startId) && (done == FALSE)) ;
    }

    if (done == FALSE) {
        selectId = CHNLID_INVALID ;
    }

    return selectId ;
}


/** ----------------------------------------------------------------------------
 *  @func   ZCPYMQT_Send
 *
 *  @desc   Used for sending events.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
static
Uint32
ZCPYDATA_send (Ptr buffer)
{
    ZCPYDATA_Ctrl * ctrlPtr = NULL ;
    CHNL_Irp *      chirp ;
    CHNL_Irp *      fmChirp ;
    QUE_Elem *      tmpPtr ;
    Uint32          status ;

    DBC_require (buffer != NULL) ;

    ctrlPtr = ZCPYDATA_devObj.ctrlPtr ;

    /*  Get a free CHIRP from the shared memory */
    MPCS_enter ((MPCS_Handle) &(ctrlPtr->csFreeChirps)) ;
    HAL_cacheInv ((Ptr) &(ctrlPtr->freeChirps), sizeof (ctrlPtr->freeChirps)) ;
    tmpPtr = QUE_next (&(ctrlPtr->freeChirps)) ;
    HAL_cacheInv ((Ptr) tmpPtr, sizeof (CHNL_Irp)) ;
    tmpPtr = QUE_next (tmpPtr) ;
    HAL_cacheInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;

    chirp = (CHNL_Irp *) QUE_get ((QUE_Handle) &(ctrlPtr->freeChirps)) ;
    HAL_cacheWbInv ((Ptr) &(ctrlPtr->freeChirps),sizeof (ctrlPtr->freeChirps)) ;
    tmpPtr = QUE_next (&(ctrlPtr->freeChirps)) ;
    HAL_cacheWbInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;

    MPCS_leave ((MPCS_Handle) &(ctrlPtr->csFreeChirps)) ;

    /* Initialize the CHIRP with buffer details */
    fmChirp = (CHNL_Irp *) buffer ;
    chirp->buffer       = fmChirp->buffer ;
    chirp->arg          = fmChirp->arg ;
    chirp->size         = fmChirp->size ;
    chirp->iocStatus    = fmChirp->iocStatus ;
    chirp->chnlId       = fmChirp->chnlId ;

    /*  Send the CHIRP to the GPP */
    MPCS_enter ((MPCS_Handle) &(ctrlPtr->csFmDspList)) ;
    HAL_cacheInv ((Ptr) &(ctrlPtr->fmDspList), sizeof (ctrlPtr->fmDspList)) ;
    tmpPtr = QUE_prev (&(ctrlPtr->fmDspList)) ;
    HAL_cacheInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;

    QUE_put ((QUE_Handle) &(ctrlPtr->fmDspList), (Ptr) chirp) ;
    HAL_cacheWbInv ((Ptr) chirp, sizeof (CHNL_Irp)) ;
    HAL_cacheWbInv ((Ptr) &(ctrlPtr->fmDspList), sizeof (ctrlPtr->fmDspList)) ;
    HAL_cacheWbInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;

    status = IPS_notify (ID_GPP,
                         ZCPYDATA_devObj.ipsId,
                         ZCPYDATA_devObj.ipsEventNo,
                         (Uint16) 0,
                         FALSE) ;

    if (status != SYS_OK) {
        /* Remove the queued data buffer on failure */
        HAL_cacheInv ((Ptr) chirp, sizeof (QUE_Elem)) ;
        tmpPtr = QUE_next (chirp) ;
        HAL_cacheInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;
        tmpPtr = QUE_prev (chirp) ;
        HAL_cacheInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;

        QUE_remove (chirp) ;

        HAL_cacheWbInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;
        tmpPtr = QUE_next (chirp) ;
        HAL_cacheWbInv ((Ptr) tmpPtr, sizeof (QUE_Elem)) ;
    }

    MPCS_leave ((MPCS_Handle) &(ctrlPtr->csFmDspList)) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
