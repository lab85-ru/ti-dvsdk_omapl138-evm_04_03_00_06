/** ============================================================================
 *  @file   dsp.h
 *
 *  @path   $(DSPLINK)/gpp/inc/sys/arch/
 *
 *  @desc   Defines interface exposed by DSP sub-component.
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


#if !defined (DSP_H)
#define DSP_H


/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>
#include <_dspdefs.h>

#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   DSP_moduleInit
 *
 *  @desc   Initializes the DSP module.
 *
 *  @ret    None.
 *
 *  @enter  dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    DSP_init
 *  ============================================================================
 */
NORMAL_API
Void
DSP_moduleInit (Void) ;


/** ============================================================================
 *  @func   DSP_moduleExit
 *
 *  @desc   Finalizes the DSP module.
 *
 *  @ret    None.
 *
 *  @enter  dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    DSP_init
 *  ============================================================================
 */
NORMAL_API
Void
DSP_moduleExit (Void) ;


/** ============================================================================
 *  @func   DSP_init
 *
 *  @desc   Resets the DSP and initializes the components required by DSP.
 *          Puts the DSP in RESET state.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    interface
 *              Interface to DSP/DEVICE APIs.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EVALUE
 *              Invalid DSP MMU endianism configuration.
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    DSP_exit
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_init (IN ProcessorId  dspId, IN DSP_Interface * interface) ;


/** ============================================================================
 *  @func   DSP_exit
 *
 *  @desc   Resets the DSP and puts it into IDLE Mode.
 *
 *  @arg    dspId
 *              Processor Id.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument..
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    DSP_init
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_exit (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   DSP_start
 *
 *  @desc   Causes DSP to start execution from the given DSP address.
 *          DSP is put to STARTED state after successful completion.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    dspAddr
 *              Address to start execution from.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument..
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    DSP_stop
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_start (IN ProcessorId dspId, IN Uint32 dspAddr) ;


/** ============================================================================
 *  @func   DSP_stop
 *
 *  @desc   Stops execution on DSP.
 *          DSP is put to STOPPED state after successful completion.
 *
 *  @arg    dspId
 *              Processor Id.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument..
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    DSP_start
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_stop (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   DSP_idle
 *
 *  @desc   Idles the DSP.
 *          DSP is put to IDLE state after successful completion.
 *
 *  @arg    dspId
 *              Processor Id.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument..
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    DSP_start
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_idle (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   DSP_intCtrl
 *
 *  @desc   Perform the specified DSP interrupt control activity.
 *
 *  @arg    dspId
 *              Processor ID.
 *  @arg    intId
 *              Interrupt ID.
 *  @arg    cmd
 *              Interrupt control command to be performed.
 *  @arg    arg
 *              Optional input/output argument specific to each control command.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *          isrObj must be a valid pointer.
 *
 *  @leave  None.
 *
 *  @see    DSP_start
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_intCtrl (IN         ProcessorId       dspId,
             IN         Uint32            intId,
             IN         DSP_IntCtrlCmd    cmd,
             IN OUT     Pvoid             arg) ;


/** ============================================================================
 *  @func   DSP_read
 *
 *  @desc   Reads data from DSP.
 *
 *  @arg    dspId
 *              Processor ID.
 *  @arg    dspAddr
 *              DSP address to read from.
 *  @arg    endianInfo
 *              endianness of data - indicates whether swap is required or not.
 *  @arg    numBytes
 *              Number of bytes to read.
 *  @arg    buffer
 *              Buffer to hold the read data.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument..
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *          numBytes must be a valid pointer.
 *          buffer must be a valid pointer.
 *
 *  @leave  On successful completion, holds the number of bytes read.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_read (IN  ProcessorId  dspId,
          IN  Uint32       dspAddr,
          IN  Endianism    endianInfo,
          IN  Uint32       numBytes,
          OUT Uint8 *      buffer) ;


/** ============================================================================
 *  @func   DSP_write
 *
 *  @desc   Writes data to DSP.
 *
 *  @arg    dspId
 *              Processor ID.
 *  @arg    dspAddr
 *              DSP address to write to.
 *  @arg    endianInfo
 *              endianness of data - indicates whether swap is required or not.
 *  @arg    numBytes
 *              Number of bytes to write.
 *  @arg    buffer
 *              Buffer containing data to be written.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument..
 *          DSP_EFAIL
 *              DSP_setup function wasn't called before calling this function.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *          buffer must be a valid pointer.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_write (IN ProcessorId dspId,
           IN Uint32      dspAddr,
           IN Endianism   endianInfo,
           IN Uint32      numBytes,
           IN Uint8 *     buffer) ;


/** ============================================================================
 *  @func   DSP_addrConvert
 *
 *  @desc   Convert address between GPP and DSP address space.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    addr
 *              Address to be converted. If DSP address, the addr parameter
 *              reflects the DSP MADU address.
 *  @arg    type
 *              Type of address conversion.
 *
 *  @ret    Converted address.
 *              Operation successfully completed.
 *          ADDRMAP_INVALID.
 *              Specified address is not in mapped range.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    DSP_AddrConvType
 *  ============================================================================
 */
NORMAL_API
Uint32
DSP_addrConvert (IN  ProcessorId        dspId,
                 IN  Uint32             addr,
                 IN  DSP_AddrConvType    type) ;


/** ============================================================================
 *  @func   DSP_control
 *
 *  @desc   Hook for performing device dependent control operation.
 *
 *  @arg    dspId
 *              Processor ID.
 *  @arg    cmd
 *              Command id.
 *  @arg    arg
 *              Optional argument for the specified command.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments specified.
 *
 *  @enter  DSP_setup must be called before calling this function.
 *          dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_control (IN  ProcessorId dspId,
             IN  Int32       cmd,
                 Pvoid       arg) ;


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   DSP_instrument
 *
 *  @desc   Gets the instrumentation information related to the specified
 *          DSP object.
 *
 *  @arg    dspId
 *              Processor ID.
 *  @arg    retVal
 *              Placeholder to return the instrumentation information.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument(s).
 *
 *  @enter  dspId must be a valid DSP identifier.
 *          retVal must be valid.
 *
 *  @leave  None.
 *
 *  @see    DspObject
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DSP_instrument (IN  ProcessorId dspId, OUT DSP_Stats * retVal) ;

#endif /* if defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   DSP_debug
 *
 *  @desc   Prints debug information of the specified DSP object.
 *
 *  @arg    dspId
 *              Processor ID.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *
 *  @enter  dspId must be a valid DSP identifier.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
Void
DSP_debug (IN  ProcessorId dspId) ;
#endif /* if defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (DSP_H) */
