/** ============================================================================
 *  @file   dsplinkdata.h
 *
 *  @path   $(DSPLINK)/dsp/src/data/
 *
 *  @desc   Defines the interface of the DSPLINK Data driver Manager component.
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


#if !defined (DSPLINKDATA_H)
#define DSPLINKDATA_H


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   DSPLINKDATA_Config
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
typedef struct DSPLINKDATA_Config_tag {
    Uint32           baseChnlId ;
    Uint32           numChannels ;
    Uint32           maxBufSize ;
    Uint32           poolId ;
    Uint32           queuePerChnl ;
    Uint32           ipsId ;
    Uint32           ipsEventNo ;
    Uint32           arg1 ;
    Uint32           arg2 ;
} DSPLINKDATA_Config ;

/** ============================================================================
 *  @name   DSPLINKDATA_Ctrl
 *
 *  @desc   This structure defines the control structure required by the
 *          DSPLINK data driver component for each data driver instance
 *
 *  @field  dspAddr
 *              DSP address of the shared control structure for the data driver
 *              instance.
 *  @field  config
 *              Configuration structure for the data driver instance.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct DSPLINKDATA_Ctrl_tag {
    Uint32             dspAddr ;
    DSPLINKDATA_Config config ;
    Uint16             padding [DSPLINKDATA_CTRL_PADDING] ;
} DSPLINKDATA_Ctrl ;


/** ============================================================================
 *  @func   DSPLINKDATA_init
 *
 *  @desc   This function initializes the DSPLINKDATA component.
 *
 *  @arg    procId
 *              Processor Identifier with which the Data driver instances are
 *              shared.
 *  @arg    numDataDrivers
 *              Number of Data driver instances configured.
 *  @arg    shAddr
 *              Address in shared memory for the Data driver control structure.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid argument.
 *          SYS_EALLOC
 *              Memory allocation failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
Int
DSPLINKDATA_init (IN  Uint16  procId,
                  IN  Uint32  numDataDrivers,
                  IN  Uint32  shAddr) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* if !defined (DSPLINKDATA_H) */
