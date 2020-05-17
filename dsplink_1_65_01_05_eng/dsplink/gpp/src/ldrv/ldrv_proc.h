/** ============================================================================
 *  @file   ldrv_proc.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Provides Interface Definitions for Link Driver PROC component.
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


#if !defined (LDRV_PROC_H)
#define LDRV_PROC_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <_dsplink.h>
#include <procdefs.h>
#include <loaderdefs.h>
#include <_loaderdefs.h>

#if defined (DDSP_PROFILE)
#include <profile.h>
#endif


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   LDRV_PROC_moduleInit
 *
 *  @desc   This function initializes the LDRV_PROC module.
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
 *  @see    LDRV_PROC_moduleExit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_moduleInit (Void) ;


/** ============================================================================
 *  @func   LDRV_PROC_moduleExit
 *
 *  @desc   This function finalizes the LDRV_PROC module.
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
 *  @see    LDRV_PROC_moduleInit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_moduleExit (Void) ;


/** ============================================================================
 *  @func   LDRV_PROC_init
 *
 *  @desc   Allocates resources at GPP side that are required for using DSP.
 *          It also sets up the connection to DSP from the GPP and other
 *          associated peripheral hardware.
 *
 *  @arg    dspId
 *              DSP ID of DSP which is to be initialized.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspID must be valid.
 *
 *  @leave  DSP is put in Reset state on success.
 *
 *  @see    LDRV_PROC_exit
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_init (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   LDRV_PROC_exit
 *
 *  @desc   Releases all the resources created on GPP side for
 *          the specified DSP.
 *
 *  @arg    dspId
 *              DSP ID of DSP which is to be finalized.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EWRONGTATE
 *              LDRV_PROC_init wasn't called before this function.
 *
 *  @enter  dspId must be valid.
 *          DSP must not be in Unknown state.
 *
 *  @leave  DSP is put in Idle state on success.
 *
 *  @see    LDRV_PROC_init
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_exit (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   LDRV_PROC_start
 *
 *  @desc   Starts execution of DSP from the specified location.
 *          This function calls LDRV_DSP_Start to actually start the processor.
 *
 *  @arg    dspId
 *              DSP ID of DSP which is to be started.
 *  @arg    dspAddr
 *              Location from where to start.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EWRONGSTATE
 *              DSP not in the right state to execute this function.
 *
 *  @enter  dspId must be valid.
 *          DSP must be in either Loaded or Stopped state.
 *
 *  @leave  DSP is put in Started state on success.
 *
 *  @see    LDRV_PROC_write, LDRV_PROC_stop
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_start (IN ProcessorId dspId, IN Uint32 dspAddr) ;


/** ============================================================================
 *  @func   LDRV_PROC_stop
 *
 *  @desc   Stops the execution of DSP. It does so by the call to LDRV_DSP_Stop.
 *
 *  @arg    dspId
 *              DSP ID of DSP which is to be stoped.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EWRONGSTATE
 *              DSP not in the right state to execute this function.
 *
 *  @enter  dspId must be valid.
 *          DSP must be in either Started or Stopped  state.
 *
 *  @leave  DSP is put in Stopped state on success.
 *
 *  @see    LDRV_PROC_start
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_stop (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   LDRV_PROC_idle
 *
 *  @desc   Puts the processor in idle mode, which means that read and write
 *          can be done to the processor.
 *
 *  @arg    dspId
 *              DSP ID of DSP which is to be stopped.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EWRONGSTATE
 *              LDRV_PROC_init wasn't called before this function.
 *
 *  @enter  dspId must be valid.
 *          DSP must not be in Unknown state.
 *
 *  @leave  DSP is put in Idle state on success.
 *
 *  @see    LDRV_PROC_init
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_idle (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   LDRV_PROC_read
 *
 *  @desc   Reads the DSP's memory space. This function calls LDRV_DSP_Read.
 *
 *  @arg    dspId
 *              DSP ID of DSP whoose memory is to be read.
 *  @arg    dspAddr
 *              Address from where to read.
 *  @arg    endianInfo
 *              This specifies endianness of the data.
 *  @arg    numBytes
 *              Number of bytes to read from the location.
 *  @arg    buffer
 *              Buffer in which store the read data.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EWRONGSTATE
 *              DSP not in the right state to execute this function.
 *
 *  @enter  dspId must be valid.
 *          buffer must be valid.
 *          numBytes must be valid.
 *          DSP must not be in Unknown or Reset state.
 *          This function must not be called from the DPC context.
 *
 *  @leave  None
 *
 *  @see    LDRV_PROC_idle, LDRV_PROC_write
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_read (IN  ProcessorId  dspId,
                IN  Uint32       dspAddr,
                IN  Endianism    endianInfo,
                IN  Uint32       numBytes,
                OUT Uint8 *      buffer) ;


/** ============================================================================
 *  @func   LDRV_PROC_write
 *
 *  @desc   Writes to DSP's memory space. This function calls LDRV_DSP_Write.
 *
 *  @arg    dspId
 *              DSP ID of DSP whoose memory is to be written.
 *  @arg    dspAddr
 *              Address to which we need to write.
 *  @arg    endianInfo
 *              This specifies endianness of the data.
 *  @arg    numBytes
 *              Number of bytes to read from the location.
 *  @arg    buffer
 *              Buffer in which to store the data to write.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EWRONGSTATE
 *              DSP not in the right state to execute this function.
 *
 *  @enter  dspId must be valid.
 *          buffer must be valid.
 *          DSP must not be in Unknown state.
 *          This function must not be called from the DPC context.
 *
 *  @leave  DSP is put in Loaded state on success.
 *
 *  @see    LDRV_PROC_idle, LDRV_PROC_read
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_write (IN  ProcessorId  dspId,
                 IN  Uint32       dspAddr,
                 IN  Endianism    endianInfo,
                 IN  Uint32       numBytes,
                 IN  Uint8 *      buffer) ;


/** ============================================================================
 *  @func   LDRV_PROC_getState
 *
 *  @desc   Returns the current state of the processor.
 *
 *  @arg    dspId
 *              DSP ID of DSP which is to be stopped.
 *  @arg    procState
 *              Placeholder for DSP Status.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *
 *  @enter  dspId must be valid.
 *          procStatus must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_getState (IN ProcessorId dspId, OUT PROC_State * procState) ;


/** ============================================================================
 *  @func   LDRV_PROC_setState
 *
 *  @desc   Sets the current state of processor to the specified state.
 *
 *  @arg    dspId
 *              DSP ID of DSP which is to be stopped.
 *  @arg    procState
 *              State to which the processor state is to be set.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_setState (IN ProcessorId dspId, IN PROC_State procState) ;


/** ============================================================================
 *  @func   LDRV_PROC_control
 *
 *  @desc   Provides a hook to perform device dependent control operations.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    cmd
 *              Command id.
 *  @arg    arg
 *              Optional argument for the specified command.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_control (IN  ProcessorId dspId,
                   IN  Int32       cmd,
                       Pvoid       arg) ;


/** ============================================================================
 *  @func   LDRV_PROC_getLoader
 *
 *  @desc   Get the loader interface table.
 *
 *  @arg    procId
 *              Identifier for processor.
 *  @arg    loaderIntf
 *              Pointer to the loader interface table configured.
 *  @arg    kfileIntf
 *              Pointer to the KFILE interface table configured.
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
NORMAL_API
Void
LDRV_PROC_getLoader (IN  ProcessorId         procId,
                     OUT LOADER_Interface ** loaderIntf,
                     OUT KFILE_Interface **  kfileIntf) ;


/** ============================================================================
 *  @func   LDRV_PROC_addrConvert
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
 *  ============================================================================
 */
NORMAL_API
Uint32
LDRV_PROC_addrConvert (IN  ProcessorId        dspId,
                       IN  Uint32             addr,
                       IN  DSP_AddrConvType   type) ;

/** ============================================================================
 *  @func   LDRV_PROC_isStarted
 *
 *  @desc   Check whether DSP is started or not.
 *
 *  @arg    procId
 *              Identifier for processor.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_PROC_isStarted (IN  ProcessorId procId) ;


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   LDRV_PROC_instrument
 *
 *  @desc   Gets the instrumentation information related to PROC's
 *
 *  @arg    procId
 *              Identifier for processor.
 *  @arg    retVal
 *              OUT argument to contain the instrumentation information.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              retVal is invalid.
 *
 *  @enter  retVal must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_PROC_instrument (IN ProcessorId procId, OUT PROC_Instrument * retVal) ;
#endif


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_PROC_debug
 *
 *  @desc   Prints out debug information of PROC module.
 *          It prints all the important information maintained by this module.
 *
 *  @arg    procId
 *              Identifier for processor.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_PROC_debug (IN ProcessorId dspId) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_PROC_H) */
