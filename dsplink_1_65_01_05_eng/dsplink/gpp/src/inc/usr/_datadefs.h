/** ============================================================================
 *  @file   _datadefs.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Definitions of constants and structures for the data driver.
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


#if !defined (_DATADEFS_H)
#define _DATADEFS_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <gpptypes.h>
#include <errbase.h>


#if defined (__cplusplus)
extern "C" {
#endif


/*  ============================================================================
 *  @name   DATA_Interface
 *
 *  @desc   Forward declaration of DATA_Interface structure.
 *  ============================================================================
 */
typedef struct DATA_Interface_tag DATA_Interface ;


/** ============================================================================
 *  @func   FnDataInit
 *
 *  @desc   Signature of function that allocates and initializes resources
 *          used by the Data Driver.
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
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDataInit) (IN ProcessorId dspId,
                                  IN  Uint32     dataDrvId,
                                  OUT Uint32 *   shDspAddr) ;

/** ============================================================================
 *  @func   FnDataExit
 *
 *  @desc   Signature of function that de-allocates and finalizes resources
 *          used by the data driver
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    dataDrvId
 *              Identifier of data driver to be finalized.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDataExit) (IN  ProcessorId dspId,
                                  IN  Uint32      dataDrvId) ;

/** ============================================================================
 *  @func   FnDataOpenChannel
 *
 *  @desc   Signature of function that opens a channel for input/output.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    chnlId
 *              Channel ID on which IO is being requested.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Could not open the channel successfully.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDataOpenChannel) (IN ProcessorId dspId,
                                         IN ChannelId   chnlId) ;

/** ============================================================================
 *  @func   FnDataCloseChannel
 *
 *  @desc   Signature of function that closes a channel.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    chnlId
 *              Channel ID on which IO is being requested.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Could not open the channel successfully.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDataCloseChannel) (IN ProcessorId    dspId,
                                          IN ChannelId      chnlId) ;

/** ============================================================================
 *  @func   FnDataCancelIO
 *
 *  @desc   Signature of function that cancels a channel.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Could not open the channel successfully.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDataCancelIO) (IN ProcessorId    dspId,
                                      IN ChannelId      chnlId) ;

/** ============================================================================
 *  @func   FnDataRequest
 *
 *  @desc   Signature of function that de-allocates and finalizes resources
 *          used by the data driver
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    chnlId
 *              Channel ID on which IO is being requested.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              chnlId is invalid.
 *          DSP_EPOINTER
 *              Subcomponent is not initialized.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnDataRequest) (IN ProcessorId dspId,
                                     IN ChannelId   chnlId) ;

#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   FnDataDebug
 *
 *  @desc   Signature of function that prints debug information for the data
 *          driver.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    dataDrvId
 *              Identifier of data driver.
 *
 *  @ret    None.
 *  ============================================================================
 */
typedef Void (*FnDataDebug) (IN  ProcessorId dspId,
                             IN  Uint32      dataDrvId) ;
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   DATA_Interface_tag
 *
 *  @field  init
 *              Function pointer to the function to initialize the data driver.
 *  @field  exit
 *              Function pointer to the function to finalize the data driver.
 *  @field  openChannel
 *              Function pointer to the Data Driver function to open channel.
 *  @field  closeChannel
 *              Function pointer to the Data Driver function to close channel.
 *  @field  cancelChannel
 *              Function pointer to the Data Driver function to cancel channel.
 *  @field  request
 *              Function pointer to the Data Driver function to request IO.
 *  @field  debug
 *              Function pointer to the Data Driver function to print debug
 *              information.
 *  ============================================================================
 */
struct DATA_Interface_tag {
    FnDataInit              init ;
    FnDataExit              exit ;
    FnDataOpenChannel       openChannel ;
    FnDataCloseChannel      closeChannel ;
    FnDataCancelIO          cancelChannel ;
    FnDataRequest           request ;
#if defined (DDSP_DEBUG)
    FnDataDebug             debug ;
#endif /* if defined (DDSP_DEBUG) */
} ;


#if defined (__cplusplus)
}
#endif


#endif /* !defined (_DATADEFS_H) */
