/** ============================================================================
 *  @file   ldrv_data.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/DATA/
 *
 *  @desc   Defines interfaces exposed by LDRV_DATA subcomponent. These services
 *          are used by LDRV_CHNL for performing IO operations. It uses
 *          services from (LDRV_)DSP subcomponent for carrying out the tasks.
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


#if !defined (LDRV_DATA_H)
#define LDRV_DATA_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   LDRV_DATA_Config
 *
 *  @desc   This structure defines the configuration information shared between
 *          the Data driver instances on the GPP and DSP-sides.
 *
 *  @field  baseChnlId
 *              Base channel ID for this data driver.
 *  @field  numChannels
 *              Number of logical channels for this data driver.
 *  @field  maxBufSize
 *              Maximum size of data buffer supported by this data driver. If
 *              no limit is imposed by the driver, a value of -1 can be
 *              specified.
 *  @field  poolId
 *              Identifier for the pool from where buffers are allocated.
 *  @field  queuePerChnl
 *              Buffer Queue length on each channel supported by the data
 *              driver.
 *  @field  ipsId
 *              ID of the IPS to be used (if any). A value of -1 indicates that
 *              no IPS is required by the data driver.
 *  @field  ipsEventNo
 *              IPS Event number associated with data driver (if any). A value
 *              of -1 indicates that no IPS is required by the data driver.
 *  @field  arg1
 *              First optional data driver specific argument.
 *              The significance of this argument is specific to a data driver.
 *  @field  arg2
 *              Second optional data driver specific argument 2.
 *              The significance of this argument is specific to a data driver.
 *  ============================================================================
 */
typedef struct LDRV_DATA_Config_tag {
    Uint32           baseChnlId ;
    Uint32           numChannels ;
    Uint32           maxBufSize ;
    Uint32           poolId ;
    Uint32           queuePerChnl ;
    Uint32           ipsId ;
    Uint32           ipsEventNo ;
    Uint32           arg1 ;
    Uint32           arg2 ;
} LDRV_DATA_Config ;

/** ============================================================================
 *  @name   LDRV_DATA_Ctrl
 *
 *  @desc   This structure defines the control structure required by the
 *          LDRV_DATA component for each Data driver instance
 *
 *  @field  dspAddr
 *              DSP address of the shared control structure for the Data driver
 *              instance.
 *  @field  config
 *              Configuration structure for the Data driver instance.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct LDRV_DATA_Ctrl_tag {
    Uint32           dspAddr ;
    LDRV_DATA_Config config ;
    Uint16           padding [LDRV_DATA_CTRL_PADDING] ;
} LDRV_DATA_Ctrl ;


/** ============================================================================
 *  @func   LDRV_DATA_moduleInit
 *
 *  @desc   This function initializes the LDRV_DATA module.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    LDRV_DATA_moduleExit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_moduleInit (Void) ;


/** ============================================================================
 *  @func   LDRV_DATA_moduleExit
 *
 *  @desc   This function finalizes the LDRV_DATA module.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    LDRV_DATA_moduleInit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_moduleExit (Void) ;


/** ============================================================================
 *  @func   LDRV_DATA_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    shDspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the LDRV DATA component.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EMEMORY
 *              Out of memory.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          shDspAddr must be a valid pointer.
 *
 *  @leave  None.
 *
 *  @see    LDRV_DATA_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_init (IN ProcessorId dspId, OUT Uint32 * shDspAddr) ;


/** ============================================================================
 *  @func   LDRV_DATA_exit
 *
 *  @desc   This function finalizes the LDRV DATA module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None.
 *
 *  @see    LDRV_DATA_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_exit (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   LDRV_DATA_openChannel
 *
 *  @desc   Open a channel for input/output.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Could not open the channel successfully.
 *
 *  @enter  dspId must be valid.
 *          chnlId must be valid.
 *
 *  @leave  None.
 *
 *  @see    LDRV_DATA_closeChannel ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_openChannel (IN ProcessorId dspId, IN ChannelId chnlId) ;


/** ============================================================================
 *  @func   LDRV_DATA_closeChannel
 *
 *  @desc   Close a channel.
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
 *          chnlId must be valid.
 *
 *  @leave  None.
 *
 *  @see    LDRV_DATA_openChannel ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_closeChannel (IN ProcessorId dspId, IN ChannelId chnlId) ;


/** ============================================================================
 *  @func   LDRV_DATA_allocateBuffer
 *
 *  @desc   Allocates an array of buffers of specified size and returns them
 *          to the client.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    bufArray
 *              Pointer to receive array of allocated buffers.
 *  @arg    size
 *              Size of each buffer.
 *  @arg    numBufs
 *              Number of buffers to allocate.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *
 *  @enter  dspId must be valid.
 *          chnlId must be valid.
 *          bufArray must be valid.
 *          numBufs must be less than maximum limit.
 *
 *  @leave  None
 *
 *  @see    LDRV_DATA_freeBuffer ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_allocateBuffer (IN  ProcessorId procId,
                          IN  ChannelId   chnlId,
                          OUT Char8 **    bufArray,
                          IN  Uint32      size,
                          IN  Uint32      numBufs) ;


/** ============================================================================
 *  @func   LDRV_DATA_freeBuffer
 *
 *  @desc   Frees buffer(s) allocated by LDRV_DATA_AllocateBuffer.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    bufArray
 *              Pointer to the array of buffers to freed.
 *  @arg    size
 *              Size of each buffer.
 *  @arg    numBufs
 *              Number of buffers to be freed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *
 *  @enter  dspId must be valid.
 *          chnlId must be valid.
 *          bufArray must be valid.
 *          numBufs must be less than maximum limit.
 *
 *  @leave  None
 *
 *  @see    LDRV_DATA_allocateBuffer ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_freeBuffer (IN ProcessorId procId,
                      IN ChannelId   chnlId,
                      IN Char8 **    bufArray,
                      IN Uint32      size,
                      IN Uint32      numBufs) ;


/** ============================================================================
 *  @func   LDRV_DATA_cancel
 *
 *  @desc   Cancel a channel.
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
 *          chnlId must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.

 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_cancel (IN ProcessorId dspId, IN ChannelId chnlId) ;


/** ============================================================================
 *  @func   LDRV_DATA_request
 *
 *  @desc   This function sends an IO request on specified channel to the
 *          data driver.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              chnlId is invalid.
 *
 *  @enter  dspId must be valid.
 *          chnlId must be valid.
 *          This sub-component must have been initialized before calling
 *          this function.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_request (IN ProcessorId dspId, IN ChannelId chnlId) ;


/** ============================================================================
 *  @func   LDRV_DATA_getPoolId
 *
 *  @desc   Returns the pool id to be used for allocating buffer for the channel
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    poolId
 *              Placeholder for returning the poolid.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          chnlId must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_DATA_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_getPoolId (IN  ProcessorId    dspId,
                     IN  ChannelId      chnlId,
                     OUT Uint32 *       poolId) ;


/** ============================================================================
 *  @func   LDRV_DATA_getDataDrvId
 *
 *  @desc   Get the link identifier for a given channel
 *
 *  @arg    dspId
 *              Processor idenfitier.
 *  @arg    chnlId
 *              Channel idenfitier.
 *  @arg    dataDrvId
 *              Placeholder for the link identifier
 *
 *  @ret    DSP_SOK
 *              Channel id is valid & maps to a data driver link towards DSP.
 *          DSP_ENOTFOUND
 *              Specified channel ID was not in the expected range
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              A general error occurred while mapping channel ID to link ID.
 *
 *  @enter  dspId must be valid.
 *          chnlId must be valid.
 *          dataDrvId must be a valid pointer.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_DATA_getDataDrvId (IN  ProcessorId  dspId,
                        IN  ChannelId    chnlId,
                        OUT Uint32 *     dataDrvId) ;


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_DATA_debug
 *
 *  @desc   Prints the current status of this subcomponent.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *
 *  @ret    None.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_DATA_debug (IN  ProcessorId  dspId) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_DATA_H) */
