/** ============================================================================
 *  @file   dcpy_data.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/DATA/
 *
 *  @desc   Defines the DCPY Data driver using Shared Memory.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation version 2.
 *  
 *  This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 *  whether express or implied; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *  ============================================================================
 */


#if !defined (DCPY_DATA_H)
#define DCPY_DATA_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <mpcs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @const  MAX_DCPY_DATA
 *
 *  @desc   Maximum number of DCPY Data driver objects supported for each DSP.
 *  ============================================================================
 */
#define MAX_DCPY_DATA     16

/*  ============================================================================
 *  @const  MAX_SUPPORTED_CHANNELS
 *
 *  @desc   Maximum number of Channels supported by each data layer.
 *  ============================================================================
 */
#define MAX_SUPPORTED_CHANNELS     16

/** ============================================================================
 *  @const  DCPYDATA_LIST_PADLENGTH
 *
 *  @desc   Padding required for DSP L2 cache line alignment within DCPYDATA
 *          control objects for aligning a LIST element.
 *  ============================================================================
 */
#define DCPYDATA_LIST_PADLENGTH   ((CACHE_L2_LINESIZE - sizeof (List)) / 2)

/** ============================================================================
 *  @const  DCPYDATA_IRP_PADLENGTH
 *
 *  @desc   Padding required for DSP L2 cache line alignment for channel IRP
 *          object.
 *  ============================================================================
 */
#define DCPYDATA_IRP_PADLENGTH ((CACHE_L2_LINESIZE - sizeof (LDRV_CHNL_Irp)) /2)


/** ============================================================================
 *  @name   DCPYDATA_ChnlMask
 *
 *  @desc   Defines the mask value for a channel.
 *
 *  @field  bitValue
 *              Bit mask with a '1' bit indicating that a command is pending on
 *              this channel.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct DCPYDATA_ChnlMask_tag {
    volatile Uint16   bitValue ;
    ADD_PADDING (padding, DSPLINK_16BIT_PADDING)
} DCPYDATA_ChnlMask ;

/** ============================================================================
 *  @name   DCPYDATA_Ctrl
 *
 *  @desc   Defines the control structure used by GPP and DSP for DCPY data
 *          driver
 *
 *  @field  gppFreeMask
 *              Indicating that a command is pending for the DSP on the channel
 *              corresponding to the index.
 *              This field is modified by the GPP only.
 *  @field  dspFreeMask
 *              Indicating that a command is pending for the GPP on the channel
 *              corresponding to the index.
 *              This field is modified by the DSP only.
 *  @field  gppOutputFull
 *              Output full flag for a gpp output channel.
 *  @field  dspOutputFull
 *              Output full flag for a dsp output channel.
 *  @field  toDspList
 *              Holds the list of buffers to be sent to the DSP.
 *  @field  toDspPadding
 *              Padding for alignment.
 *  @field  fmDspList
 *              Holds the list of buffers to be received from the DSP.
 *  @field  fmDspPadding
 *              Padding for alignment.
 *  @field  csToDspList
 *              Shared critical section object for protection of operations by
 *              the two processors on the toDspList.
 *  @field  csFmDspList
 *              Shared critical section object for protection of operations by
 *              the two processors on the fmDspList.
 *  ============================================================================
 */
typedef struct DCPYDATA_Ctrl_tag {
    volatile DCPYDATA_ChnlMask   gppFreeMask   [MAX_SUPPORTED_CHANNELS] ;
    volatile DCPYDATA_ChnlMask   dspFreeMask   [MAX_SUPPORTED_CHANNELS] ;
    volatile DCPYDATA_ChnlMask   gppOutputFull [MAX_SUPPORTED_CHANNELS] ;
    volatile DCPYDATA_ChnlMask   dspOutputFull [MAX_SUPPORTED_CHANNELS] ;

    volatile List                toDspList ;
    ADD_PADDING (toDspPadding, DCPYDATA_LIST_PADLENGTH)
    volatile List                fmDspList ;
    ADD_PADDING (fmDspPadding, DCPYDATA_LIST_PADLENGTH)
    volatile MPCS_ShObj          csToDspList ;
    volatile MPCS_ShObj          csFmDspList ;

    volatile List                freeChirps ;
    ADD_PADDING (freeChirpsPadding, DCPYDATA_LIST_PADLENGTH)
    volatile MPCS_ShObj          csFreeChirps ;
} DCPYDATA_Ctrl ;

/** ============================================================================
 *  @name   DCPYDATA_ChnlIrp
 *
 *  @desc   Encapsulates the LDRV_CHNL_Irp object to align it to the DSP L2 cache
 *          line size.
 *
 *  @field  chnlIrp
 *              CHNL IRP object.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct DCPYDATA_ChnlIrp_tag {
    volatile LDRV_CHNL_Irp  chnlIrp ;
    ADD_PADDING (padding, DCPYDATA_IRP_PADLENGTH)
} DCPYDATA_ChnlIrp ;


/** ============================================================================
 *  @func   DCPYDATA_init
 *
 *  @desc   This function allocates and initializes resources used by the
 *          DCPY Data Driver.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    dataDrvId
 *              Identifier of data driver to be initialized.
 *  @arg    shDspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the LDRV DATA component.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EMEMORY
 *              Out of memory.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          shDspAddr must be a valid pointer.
 *
 *  @leave  None.
 *
 *  @see    DCPYDATA_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYDATA_init (IN ProcessorId dspId,
               IN  Uint32     dataDrvId,
               OUT Uint32 *   shDspAddr) ;


/** ============================================================================
 *  @func   DCPYDATA_exit
 *
 *  @desc   This function releases the resources allocated earlier by this
 *          DCPY data driver in call to DCPYDATA_init ().
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    dataDrvId
 *              Identifier of data driver to be initialized.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EMEMORY
 *              Out of memory.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None.
 *
 *  @see    DCPYDATA_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYDATA_exit (IN  ProcessorId dspId,
               IN  Uint32      dataDrvId) ;


/** ============================================================================
 *  @func   DCPYDATA_openChannel
 *
 *  @desc   This function opens a channel to be used for data transfer.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None
 *
 *  @see    DCPYDATA_closeChannel ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYDATA_openChannel (IN ProcessorId dspId, IN ChannelId chnlId) ;


/** ============================================================================
 *  @func   DCPYDATA_closeChannel
 *
 *  @desc   This function closes the physical channel corresponding to the
 *          specified logical channel.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Could not close the channel successfully.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None
 *
 *  @see    DCPYDATA_openChannel ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYDATA_closeChannel (IN ProcessorId dspId, IN ChannelId chnlId) ;


/** ============================================================================
 *  @func   DCPYDATA_cancelIo
 *
 *  @desc   This function cancels IO on a channel.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None
 *
 *  @see    DCPYDATA_openChannel (), DCPYDATA_closeChannel ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYDATA_cancelIo (IN ProcessorId dspId, IN ChannelId chnlId) ;


/** ============================================================================
 *  @func   DCPYDATA_request
 *
 *  @desc   This function submits an IO request to the DSP.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments passed.
 *          DSP_EPOINTER
 *              Subcomponent is not initialized.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None.
 *
 *  @see    DCPYDATA_openChannel ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DCPYDATA_request (IN ProcessorId dspId, IN ChannelId chnlId) ;


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   DCPYDATA_debug
 *
 *  @desc   Prints information related to DCPY Link Driver.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    dataDrvId
 *              Identifier of data driver.
 *
 *  @ret    None
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Void
DCPYDATA_debug (IN  ProcessorId dspId,
                IN  Uint32      dataDrvId) ;
#endif /* if defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (DCPY_DATA_H) */
