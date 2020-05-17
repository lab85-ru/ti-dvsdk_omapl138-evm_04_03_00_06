/** ============================================================================
 *  @file   pmgr_proc.h
 *
 *  @path   $(DSPLINK)/gpp/src/pmgr/
 *
 *  @desc   Defines the interfaces and data structures for the sub-component
 *          PMGR_PROC.
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


#if !defined (PMGR_PROC_H)
#define PMGR_PROC_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <_dsplink.h>
#include <linkcfgdefs.h>
#include <procdefs.h>
#include <prcs.h>

#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */



#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   PMGR_PROC_setup
 *
 *  @desc   Sets up the necessary data structures for the PMGR_PROC
 *          sub-component.
 *
 *  @arg    linkCfg
 *             Pointer to the user-side configuration information structure for
 *             DSP/BIOS LINK.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EACCESSDENIED
 *              Access denied. Another client has already called
 *              PMGR_PROC_setup ().
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    PMGR_PROC_destroy
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_setup (IN LINKCFG_Object * linkCfg) ;


/** ============================================================================
 *  @func   PMGR_PROC_destroy
 *
 *  @desc   Destroys the data structures for the PMGR_PROC component,
 *          allocated earlier by a call to PROC_setup ().
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EACCESSDENIED
 *              Access denied. Only the client who had successfully called
 *              PMGR_PROC_setup () can call this function.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    PMGR_PROC_setup
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_destroy (Void) ;


/** ============================================================================
 *  @func   PMGR_PROC_attach
 *
 *  @desc   Attaches the client to the specified DSP and also
 *          initializes the DSP (if required).
 *
 *  @arg    procId
 *             Processor Identifier.
 *  @arg    attr
 *             Attributes for the processor on which attach is to be done.
 *  @arg    optArgs
 *              Optional arguments
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SALREADYATTACHED
 *              Successful attach. Also, indicates that another client has
 *              already attached to DSP.
 *          DSP_EACCESSDENIED
 *              Not allowed to access the DSP.
 *          DSP_EFAIL
 *              General failure, unable to attach to processor.
 *          DSP_EWRONGSTATE
 *              Incorrect state to complete the requested operation.
 *
 *  @enter  procId must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_PROC_detach
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_attach (IN    ProcessorId    procId,
                         PROC_Attrs * attr,
                  IN     Void *       optArgs) ;


/** ============================================================================
 *  @func   PMGR_PROC_detach
 *
 *  @desc   Detaches the client from specified processor.
 *          If the caller is the owner of the processor, this function releases
 *          all the resources that this component uses and puts the DSP in an
 *          unusable state (from application perspective).
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    optArgs
 *              Optional arguments
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure, unable to detach.
 *          DSP_EACCESSDENIED
 *              Not allowed to access the DSP.
 *          DSP_EATTACHED
 *              Not attached to the target processor.
 *          DSP_EWRONGSTATE
 *              Incorrect state to complete the requested operation.
 *
 *  @enter  procId must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_PROC_attach
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_detach (IN  ProcessorId   procId, IN     Void * optArgs) ;


/** ============================================================================
 *  @func   PMGR_PROC_getState
 *
 *  @desc   Gets the current status of DSP by querying the Link Driver.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    procState
 *              Placeholder for processor state.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure, couldn't get state information.
 *
 *  @enter  procId must be valid.
 *          The pointer to PROC_State is valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_PROC_load, PMGR_PROC_start, PMGR_PROC_stop, PMGR_PROC_Idle
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_getState (IN   ProcessorId    procId,
                    OUT  PROC_State *   procState) ;


/** ============================================================================
 *  @func   PMGR_PROC_load
 *
 *  @desc   Loads the specified DSP executable on the target DSP.
 *          It ensures that the caller owns the DSP.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    imagePath
 *              Full path to the image file to load on DSP.
 *  @arg    argc
 *              Number of arguments to be passed to the DSP executable.
 *  @arg    argv
 *              Arguments to be passed to DSP executable.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EACCESSDENIED
 *              Access denied. Only the owner client is allowed to
 *              load the DSP executable.
 *          DSP_EFILE
 *              Invalid DSP executable.
 *          DSP_ESIZE
 *              Size of the .args section is not sufficient to hold the passed
 *              arguments.
 *          DSP_EFAIL
 *              General failure, unable to load DSP executable.
 *          DSP_EWRONGSTATE
 *              Incorrect state to complete the requested operation.
 *
 *  @enter  procId must be valid.
 *          Base image path must be valid.
 *          If argc is 0 then argv must be NULL pointer.
 *          If argc is non-zero then argv must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    PMGR_PROC_attach, PMGR_PROC_loadSection
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_load (IN ProcessorId  procId,
                IN Char8 *      imagePath,
                IN Uint32       argc,
                IN Char8 **     argv) ;


/** ============================================================================
 *  @func   PMGR_PROC_loadSection
 *
 *  @desc   Loads the specified section of DSP executable onto the target DSP.
 *          It ensures that the client owns the DSP.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    imagePath
 *              Full path to the image file.
 *  @arg    sectID
 *              Section ID of section to load.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFILE
 *              Invalid ImagePath parameter.
 *          DSP_EINVALIDSECTION
 *              Invalid section id.
 *          DSP_EACCESSDENIED
 *              Access denied. Only the owner client is allowed to
 *              load section on the DSP.
 *          DSP_EFAIL
 *              General failure, unable to load section on DSP.
 *          DSP_EWRONGSTATE
 *              Incorrect state to complete the requested operation.
 *
 *  @enter  procId must be valid.
 *          Base image path must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_PROC_attach, PMGR_PROC_load
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_loadSection (IN ProcessorId  procId,
                       IN Char8 *      imagePath,
                       IN Uint32       sectID) ;


/** ============================================================================
 *  @func   PMGR_PROC_read
 *
 *  @desc   This function allows GPP side applications to read from the DSP
 *          memory space.
 *
 *  @arg    procId
 *              Processor ID of the target DSP.
 *  @arg    dspAddr
 *              Address of the DSP memory region from where to read.
 *  @arg    numBytes
 *              Number of bytes to be read
 *  @arg    buffer
 *              User specified buffer to contain the data from DSP memory space
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EPENDING
 *              H/W specific error. The request can’t be serviced at this
 *              point of time.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure, unable to load section on DSP.
 *
 *  @enter  procId must be valid.
 *          buffer must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_PROC_write
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_PROC_read (IN     ProcessorId    procId,
                IN     Uint32         dspAddr,
                IN     Uint32         numBytes,
                IN OUT Pvoid          buffer) ;


/** ============================================================================
 *  @func   PMGR_PROC_write
 *
 *  @desc   This function allows GPP side applications to write to the DSP
 *          memory space
 *
 *  @arg    procId
 *              Processor ID of the target DSP.
 *  @arg    dspAddr
 *              Address of the DSP memory region from where the data is
 *              to be written to.
 *  @arg    numBytes
 *              Number of bytes to be written
 *  @arg    buffer
 *              User specified buffer with data to be written into DSP memory
 *              space
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EPENDING
 *              H/W specific error. The request can’t be serviced at this
 *              point of time.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure, unable to load section on DSP.
 *
 *  @enter  procId must be valid.
 *          buffer must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_PROC_read
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_PROC_write (IN ProcessorId    procId,
                 IN Uint32         dspAddr,
                 IN Uint32         numBytes,
                 IN Pvoid          buffer) ;


/** ============================================================================
 *  @func   PMGR_PROC_start
 *
 *  @desc   Starts execution of the loaded code on DSP from the starting
 *          point specified in the DSP executable loaded earlier by call to
 *          PROC_load ().
 *
 *  @arg    procId
 *              DSP Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EACCESSDENIED
 *              Access denied. Only the owner client is allowed
 *              to start the DSP.
 *          DSP_EFAIL
 *              General failure, unable to start DSP.
 *          DSP_EWRONGSTATE
 *              Incorrect state to complete the requested operation.
 *
 *  @enter  procId must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_PROC_attach, PMGR_PROC_load, PMGR_PROC_stop
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_start (IN  ProcessorId  procId) ;


/** ============================================================================
 *  @func   PMGR_PROC_stop
 *
 *  @desc   Stops the specified DSP.
 *
 *  @arg    procId
 *              DSP Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EACCESSDENIED
 *              Access denied. Only the owner client is allowed
 *              to stop the DSP.
 *          DSP_EFAIL
 *              General failure, unable to stop DSP.
 *          DSP_EWRONGSTATE
 *              Incorrect state to complete the requested operation.
 *
 *  @enter  procId must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_PROC_attach, PMGR_PROC_load, PMGR_PROC_start
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_stop (IN   ProcessorId  procId) ;


/** ============================================================================
 *  @func   PMGR_PROC_getSymbolAddress
 *
 *  @desc   Gets the DSP address corresponding to a symbol within a DSP
 *          executable currently loaded on the DSP.
 *
 *  @arg    procId
 *              DSP identifier.
 *  @arg    symbolName
 *              Name of the symbol to be looked up.
 *  @arg    dspAddr
 *              Location to receive the DSP address for the specified symbol.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EWRONGSTATE
 *              No executable is currently loaded on the DSP.
 *          DSP_ENOTSUPPORTED
 *              Symbol lookup feature is not supported by the loader.
 *          DSP_ENOTFOUND
 *              Symbol not found.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          symbolName must be valid pointer.
 *          dspAddr must be valid pointer.
 *
 *  @leave  On success, dspAddr contains a valid DSP address.
 *          On failure, dspAddr is NULL.
 *
 *  @see    PROC_load ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_PROC_getSymbolAddress (IN   ProcessorId   procId,
                            IN   Char8 *       symbolName,
                            OUT  Uint32 *      dspAddr) ;


/** ============================================================================
 *  @func   PMGR_PROC_control
 *
 *  @desc   Provides a hook to perform device dependent control operations.
 *
 *  @arg    procId
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
 *  @see    PMGR_PROC_attach
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_control (IN  ProcessorId procId,
                   IN  Int32       cmd,
                       Pvoid       arg) ;


/** ============================================================================
 *  @func   PMGR_PROC_isAttached
 *
 *  @desc   Function to check whether the client identified by the specified
 *          'client' object is attached to the specified processor.
 *
 *  @arg    procId
 *              Processor Id.
 *  @arg    client
 *              Client identifier.
 *  @arg    isAttached
 *              Place holder for flag indicating the client is attached.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *
 *  @enter  procId must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_PROC_attach
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_isAttached (IN  ProcessorId  procId,
                      IN  PrcsObject * client,
                      OUT Bool *       isAttached) ;


/** ============================================================================
 *  @func   PMGR_PROC_getSetupRefCount
 *
 *  @desc   Returns the current reference count for PROC_setup/PROC_destroy.
 *
 *  @arg    None
 *
 *  @ret    Reference count
 *              On success
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Uint32
PMGR_PROC_getSetupRefCount (Void) ;


/** ============================================================================
 *  @func   PMGR_PROC_getAttachRefCount
 *
 *  @desc   Returns the current reference count for PROC_attach/PROC_detach.
 *
 *  @arg    procId
 *              Processor Id.
 *
 *  @ret    Reference count
 *              On success
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Uint32
PMGR_PROC_getAttachRefCount (IN Uint32 procId) ;


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   PMGR_PROC_instrument
 *
 *  @desc   Gets the instrumentation data associated with PMGR_PROC
 *          sub-component.
 *
 *  @arg    procId
 *              Identifier for processor for which instrumentation
 *              information is to be obtained.
 *  @arg    retVal
 *              OUT argument to contain the instrumentation information.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              retVal is invalid.
 *
 *  @enter  procId must be valid.
 *          retVal must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_instrument (IN ProcessorId procId, OUT PROC_Instrument * retVal) ;
#endif /* #if defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   PMGR_PROC_debug
 *
 *  @desc   Prints the debug information summarizing the current status
 *          of the PMGR_PROC component.
 *
 *  @arg    procId
 *              Identifier for processor.
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
PMGR_PROC_debug (IN ProcessorId procId) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (PMGR_PROC_H) */
