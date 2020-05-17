/** ============================================================================
 *  @file   _datadefs.h
 *
 *  @path   $(DSPLINK)/gpp/inc/usr/
 *
 *  @desc   Definitions of constants and structures for the data driver.
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
