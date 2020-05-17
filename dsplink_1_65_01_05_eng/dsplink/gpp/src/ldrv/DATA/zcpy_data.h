/** ============================================================================
 *  @file   zcpy_data.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/DATA/
 *
 *  @desc   Defines the ZCPY Data driver using Shared Memory.
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


#if !defined (ZCPY_DATA_H)
#define ZCPY_DATA_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <mpcs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @const  MAX_ZCPY_DATA
 *
 *  @desc   Maximum number of ZCPY Data driver objects supported for each DSP.
 *  ============================================================================
 */
#define MAX_ZCPY_DATA     16u

/*  ============================================================================
 *  @const  MAX_SUPPORTED_CHANNELS
 *
 *  @desc   Maximum number of Channels supported by each data layer.
 *  ============================================================================
 */
#define MAX_SUPPORTED_CHANNELS     16u

/** ============================================================================
 *  @const  ZCPYDATA_LIST_PADLENGTH
 *
 *  @desc   Padding required for DSP L2 cache line alignment within ZCPYDATA
 *          control objects for aligning a LIST element.
 *  ============================================================================
 */
#define ZCPYDATA_LIST_PADLENGTH   (((CACHE_L2_LINESIZE) - sizeof (List)) / 2)

/** ============================================================================
 *  @const  ZCPYDATA_IRP_PADLENGTH
 *
 *  @desc   Padding required for DSP L2 cache line alignment for channel IRP
 *          object.
 *  ============================================================================
 */
#define ZCPYDATA_IRP_PADLENGTH (((CACHE_L2_LINESIZE) - sizeof (LDRV_CHNL_Irp)) / 2)


/** ============================================================================
 *  @name   ZCPYDATA_ChnlMask
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
typedef struct ZCPYDATA_ChnlMask_tag {
    volatile Uint16   bitValue ;
    ADD_PADDING (padding, DSPLINK_16BIT_PADDING)
} ZCPYDATA_ChnlMask ;

/** ============================================================================
 *  @name   ZCPYDATA_Ctrl
 *
 *  @desc   Defines the control structure used by GPP and DSP for ZCPY data
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
typedef struct ZCPYDATA_Ctrl_tag {
    volatile ZCPYDATA_ChnlMask   gppFreeMask   [MAX_SUPPORTED_CHANNELS] ;
    volatile ZCPYDATA_ChnlMask   dspFreeMask   [MAX_SUPPORTED_CHANNELS] ;
    volatile ZCPYDATA_ChnlMask   gppOutputFull [MAX_SUPPORTED_CHANNELS] ;
    volatile ZCPYDATA_ChnlMask   dspOutputFull [MAX_SUPPORTED_CHANNELS] ;

    volatile List                toDspList ;
    ADD_PADDING (toDspPadding, ZCPYDATA_LIST_PADLENGTH)
    volatile List                fmDspList ;
    ADD_PADDING (fmDspPadding, ZCPYDATA_LIST_PADLENGTH)
    volatile MPCS_ShObj          csToDspList ;
    volatile MPCS_ShObj          csFmDspList ;

    volatile List                freeChirps ;
    ADD_PADDING (freeChirpsPadding, ZCPYDATA_LIST_PADLENGTH)
    volatile MPCS_ShObj          csFreeChirps ;
} ZCPYDATA_Ctrl ;

/** ============================================================================
 *  @name   ZCPYDATA_ChnlIrp
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
typedef struct ZCPYDATA_ChnlIrp_tag {
    volatile LDRV_CHNL_Irp  chnlIrp ;
    ADD_PADDING (padding, ZCPYDATA_IRP_PADLENGTH)
} ZCPYDATA_ChnlIrp ;


/** ============================================================================
 *  @func   ZCPYDATA_init
 *
 *  @desc   This function allocates and initializes resources used by the
 *          ZCPY Data Driver.
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
 *  @see    ZCPYDATA_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYDATA_init (IN ProcessorId dspId,
               IN  Uint32     dataDrvId,
               OUT Uint32 *   shDspAddr) ;


/** ============================================================================
 *  @func   ZCPYDATA_exit
 *
 *  @desc   This function releases the resources allocated earlier by this
 *          ZCPY data driver in call to ZCPYDATA_init ().
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
 *  @see    ZCPYDATA_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYDATA_exit (IN  ProcessorId dspId,
               IN  Uint32      dataDrvId) ;


/** ============================================================================
 *  @func   ZCPYDATA_openChannel
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
 *  @see    ZCPYDATA_closeChannel ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYDATA_openChannel (IN ProcessorId dspId, IN ChannelId chnlId) ;


/** ============================================================================
 *  @func   ZCPYDATA_closeChannel
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
 *  @see    ZCPYDATA_openChannel ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYDATA_closeChannel (IN ProcessorId dspId, IN ChannelId chnlId) ;


/** ============================================================================
 *  @func   ZCPYDATA_cancelIo
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
 *  @see    ZCPYDATA_openChannel (), ZCPYDATA_closeChannel ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYDATA_cancelIo (IN ProcessorId dspId, IN ChannelId chnlId) ;


/** ============================================================================
 *  @func   ZCPYDATA_request
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
 *  @see    ZCPYDATA_openChannel ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYDATA_request (IN ProcessorId dspId, IN ChannelId chnlId) ;


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   ZCPYDATA_debug
 *
 *  @desc   Prints information related to ZCPY Link Driver.
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
ZCPYDATA_debug (IN  ProcessorId dspId,
                IN  Uint32      dataDrvId) ;
#endif /* if defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (ZCPY_DATA_H) */
