/** ============================================================================
 *  @file   proc.h
 *
 *  @path   $(DSPLINK)/gpp/inc/usr/
 *
 *  @desc   Defines the interfaces and data structures for the API sub-component
 *          PROC.
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


#if !defined (PROC_H)
#define PROC_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <procdefs.h>
#include <linkcfgdefs.h>

/*  ----------------------------------- Profiling                       */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   PROC_setup
 *
 *  @desc   Sets up the necessary data structures for the PROC sub-component.
 *
 *  @arg    linkCfg
 *             Pointer to the configuration information structure for DSP/BIOS
 *             LINK.
 *             If NULL, indicates that default configuration should be used.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SALREADYSETUP
 *              The DSPLink driver is already setup in this process.
 *          DSP_ECONFIG
 *              Error in specified dynamic configuration. Please check
 *              CFG_<PLATFORM>.c
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  The configuration information structure has been updated with
 *          application specific information. If no configuration is
 *          specified the default configuration will be used.
 *
 *  @leave  None
 *
 *  @see    PROC_destroy
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_setup (IN LINKCFG_Object * linkCfg) ;


/** ============================================================================
 *  @func   PROC_destroy
 *
 *  @desc   Destroys the data structures for the PROC component,
 *          allocated earlier by a call to PROC_setup ().
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SDESTROYED
 *              The final client has finalized the driver.
 *          DSP_EACCESSDENIED
 *              The DSPLink driver was not setup in this process.
 *          DSP_ESETUP
 *              The DSPLink driver was not setup.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  All DSP/BIOS Link operations are complete.
 *
 *  @leave  None
 *
 *  @see    PROC_setup
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_destroy (Void) ;


/** ============================================================================
 *  @func   PROC_attach
 *
 *  @desc   Attaches the client to the specified DSP and also
 *          initializes the DSP (if required).
 *
 *  @arg    procId
 *             DSP identifier.
 *  @arg    attr
 *             Attributes for the processor on which attach is to be done.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SALREADYATTACHED
 *              Successful attach. Also, indicates that another client has
 *              already attached to DSP.
 *          DSP_EINVALIDARG
 *              Parameter ProcId is invalid.
 *          DSP_EACCESSDENIED
 *              Not allowed to access the DSP.
 *          DSP_EALREADYCONNECTED
 *              Another thread of the same process has already attached to the
 *              processor.
 *          DSP_EWRONGSTATE
 *              Incorrect state for completing the requested operation.
 *          DSP_EFAIL
 *              General failure, unable to attach to processor.
 *
 *  @enter  procId must be valid.
 *          PROC_setup has been successful.
 *
 *  @leave  None
 *
 *  @see    PROC_detach
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_attach (IN  ProcessorId   procId,
                 PROC_Attrs *  attr) ;


/** ============================================================================
 *  @func   PROC_detach
 *
 *  @desc   Detaches the client from specified processor.
 *          If the caller is the owner of the processor, this function releases
 *          all the resources that this component uses and puts the DSP in an
 *          unusable state (from application perspective).
 *
 *  @arg    procId
 *              DSP identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SDETACHED
 *              The final process has detached from the specific processor.
 *          DSP_EINVALIDARG
 *              Invalid ProcId parameter.
 *          DSP_ESETUP
 *              The DSPLink driver was not setup.
 *          DSP_EACCESSDENIED
 *              Not allowed to access the DSP.
 *          DSP_EATTACHED
 *              Not attached to the target processor.
 *          DSP_EWRONGSTATE
 *              Incorrect state for completing the requested operation.
 *          DSP_EFAIL
 *              General failure, unable to detach.
 *
 *  @enter  procId must be valid.
 *          All DSP/BIOS Link operations are complete.
 *
 *  @leave  None
 *
 *  @see    PROC_attach
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_detach (IN  ProcessorId procId) ;


/** ============================================================================
 *  @func   PROC_getState
 *
 *  @desc   Gets the current status of DSP by querying the Link Driver.
 *
 *  @arg    procId
 *              DSP identifier.
 *  @arg    procState
 *              Placeholder for processor state.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid ProcId and/or procState argument.
 *
 *  @enter  procId must be valid.
 *          procState must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    PROC_load, PROC_start, PROC_stop, PROC_Idle
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_getState (IN   ProcessorId    procId,
               OUT  PROC_State *   procState) ;


/** ============================================================================
 *  @func   PROC_load
 *
 *  @desc   Loads the specified DSP executable on the target DSP.
 *          It ensures that the caller owns the DSP.
 *
 *  @arg    procId
 *              DSP identifier.
 *  @arg    imagePath
 *              Full path to the image file to load on DSP.
 *  @arg    argc
 *              Number of argument to be passed to the base image upon start.
 *  @arg    argv
 *              Arguments to be passed to DSP main application.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SALREADYLOADED
 *              The specified processor has already been loaded.
 *          DSP_EINVALIDARG
 *              Invalid ProcId argument.
 *          DSP_EACCESSDENIED
 *              Not allowed to access the DSP.
 *          DSP_ESETUP
 *              The DSPLink driver has not been setup.
 *          DSP_EATTACHED
 *              This process has not attached to the specified processor.
 *          DSP_EPENDING
 *              H/W specific error. The request can’t be serviced at this
 *              point of time.
 *          DSP_EFILE
 *              Invalid base image.
 *          DSP_ESIZE
 *              Size of the .args section is not sufficient to hold the passed
 *              arguments.
 *          DSP_EFAIL
 *              General failure, unable to load image on DSP.
 *          DSP_EWRONGSTATE
 *              Incorrect state for completing the requested operation.
 *
 *  @enter  procId must be valid.
 *          imagePath must be a valid pointer.
 *          If argc is 0 then argv must be NULL pointer.
 *          If argc is non-zero then argv must be a valid pointer.
 *          PROC_setup and PROC_attach have been successful.
 *
 *  @leave  None
 *
 *  @see    PROC_attach, PROC_loadSection
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_load (IN   ProcessorId  procId,
           IN   Char8 *      imagePath,
           IN   Uint32       argc,
           IN   Char8 **     argv) ;


/** ============================================================================
 *  @func   PROC_loadSection
 *
 *  @desc   Loads the specified section of DSP executable onto the target DSP.
 *          It ensures that the client owns the DSP.
 *
 *  @arg    procId
 *              DSP identifier.
 *  @arg    imagePath
 *              Full path to the image file.
 *  @arg    sectID
 *              Section ID of section to load.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid ProcId argument.
 *          DSP_EPENDING
 *              H/W specific error. The request can’t be serviced at this
 *              point of time.
 *          DSP_EFILE
 *              Invalid ImagePath parameter.
 *          DSP_EINVALIDSECTION
 *              Invalid section name.
 *          DSP_EACCESSDENIED
 *              Not allowed to access the DSP.
 *          DSP_EFAIL
 *              General failure, unable to load section on DSP.
 *          DSP_EWRONGSTATE
 *              Incorrect state for completing the requested operation.
 *
 *  @enter  procId must be valid.
 *          imagePath must be a valid pointer.
 *          PROC_attach has been successful.
 *
 *  @leave  None
 *
 *  @see    PROC_attach, PROC_load
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_loadSection (IN    ProcessorId  procId,
                  IN    Char8 *      imagePath,
                  IN    Uint32       sectID) ;


/** ============================================================================
 *  @func   PROC_read
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
 *          PROC_attach has been successful.
 *          For DM642, the memory region must be mapped to GPP address
 *          space.
 *
 *
 *  @leave  None
 *
 *  @see    PROC_write
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_read (IN     ProcessorId    procId,
           IN     Uint32         dspAddr,
           IN     Uint32         numBytes,
           IN OUT Pvoid          buffer) ;


/** ============================================================================
 *  @func   PROC_write
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
 *          PROC_attach has been successful.
 *          For DM642, the memory region must be mapped to GPP address
 *          space.
 *
 *  @leave  None
 *
 *  @see    PROC_read
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_write (IN ProcessorId    procId,
            IN Uint32         dspAddr,
            IN Uint32         numBytes,
            IN Pvoid          buffer) ;


/** ============================================================================
 *  @func   PROC_start
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
 *          DSP_SALREADYSTARTED
 *              The specified processor has already been started.
 *          DSP_EINVALIDARG
 *              Invalid ProcId argument.
 *          DSP_EPENDING
 *              H/W specific error. The request can’t be serviced at this
 *              point of time.
 *          DSP_EACCESSDENIED
 *              Not allowed to access the DSP.
 *          DSP_ESETUP
 *              The DSPLink driver has not been setup.
 *          DSP_EATTACHED
 *              This process has not attached to the specified processor.
 *          DSP_EALREADYSTARTED
 *              The specified processor has already been started in this process
 *          DSP_EWRONGSTATE
 *              Incorrect state for completing the requested operation.
 *          DSP_ECONFIG
 *              The specified processor could not be started. Driver handshake
 *              failed due to DSP driver initialization/configuration failure.
 *          DSP_EFAIL
 *              General failure, unable to start DSP.
 *
 *  @enter  procId must be valid.
 *          PROC_attach has been successful.
 *          If the application uses POOL, POOL_open i.e POOL configuration has
 *          been successful.
 *          All setup and initialization needed for DSP to start execution
 *          has been done.
 *
 *  @leave  None
 *
 *  @see    PROC_attach, PROC_load, PROC_stop
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_start (IN ProcessorId procId) ;


/** ============================================================================
 *  @func   PROC_stop
 *
 *  @desc   Stops the DSP.
 *
 *  @arg    procId
 *              DSP Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SSTOPPED
 *              The final process has stopped the DSP execution.
 *          DSP_EINVALIDARG
 *              Invalid ProcId argument.
 *          DSP_EACCESSDENIED
 *              Not allowed to access the DSP.
 *          DSP_ESETUP
 *              The DSPLink driver has not been setup.
 *          DSP_EATTACHED
 *              This process has not attached to the specified processor.
 *          DSP_ESTARTED
 *              The specified processor has not been started.
 *          DSP_EWRONGSTATE
 *              Incorrect state for completing the requested operation.
 *          DSP_EFAIL
 *              General failure, unable to stop DSP.
 *
 *  @enter  procId must be valid.
 *          All DSP/BIOS Link operations are complete.
 *
 *  @leave  None
 *
 *  @see    PROC_attach, PROC_load, PROC_start
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_stop (IN ProcessorId procId) ;


/** ============================================================================
 *  @func   PROC_control
 *
 *  @desc   Provides a hook to perform device dependent control operations on
 *          the DSP.
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
 *          PROC_attach has been successful.
 *
 *  @leave  None
 *
 *  @see    PROC_attach
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_control (IN  ProcessorId procId,
              IN  Int32       cmd,
                  Pvoid       arg) ;


/** ============================================================================
 *  @func   PROC_GetSymbolAddress
 *
 *  @desc   Gets the DSP address corresponding to a symbol within a DSP
 *          executable currently loaded on the DSP.
 *
 *  @arg    procId
 *              DSP Identifier.
 *  @arg    symbolName
 *              name of the symbol.
 *  @arg    dspAddr
 *              pointer to get the dsp address corresponding to symbolName.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_GetSymbolAddress (IN   ProcessorId   procId,
                       IN   Char8 *       symbolName,
                       OUT  Uint32 *      dspAddr) ;


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   PROC_instrument
 *
 *  @desc   Gets the instrumentation data associated with PROC sub-component.
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
 *          PROC_attach has been successful.
 *          Profiling has been selected in the configuration.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_instrument (IN ProcessorId procId, OUT PROC_Instrument * retVal) ;
#endif /* defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   PROC_debug
 *
 *  @desc   Prints the debug information summarizing the current status
 *          of the PROC component.
 *
 *  @arg    procId
 *              Identifier for processor.
 *
 *  @ret    None
 *
 *  @enter  procId must be valid.
 *          PROC_setup and PROC_attach have been successful.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
Void
PROC_debug (IN ProcessorId procId) ;
#endif /* defined (DDSP_DEBUG) */


/** ============================================================================
 *  @deprecated The deprecated API POOL_TranslateAddr has been replaced
 *              with POOL_translateAddr.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_Setup (Void) ;

/** ============================================================================
 *  @deprecated The deprecated API PROC_Destroy has been replaced
 *              with PROC_destroy.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define PROC_Destroy               PROC_destroy

/** ============================================================================
 *  @deprecated The deprecated API PROC_Attach has been replaced
 *              with PROC_attach.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define PROC_Attach                PROC_attach

/** ============================================================================
 *  @deprecated The deprecated API PROC_Detach has been replaced
 *              with PROC_detach.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define PROC_Detach                PROC_detach

/** ============================================================================
 *  @deprecated The deprecated API PROC_GetState has been replaced
 *              with PROC_getState.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define PROC_GetState              PROC_getState

/** ============================================================================
 *  @deprecated The deprecated API PROC_Load has been replaced
 *              with PROC_load.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define PROC_Load                  PROC_load

/** ============================================================================
 *  @deprecated The deprecated API PROC_LoadSection has been replaced
 *              with PROC_loadSection.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define PROC_LoadSection           PROC_loadSection

/** ============================================================================
 *  @deprecated The deprecated API PROC_Read has been replaced
 *              with PROC_read.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define PROC_Read                  PROC_read

/** ============================================================================
 *  @deprecated The deprecated API PROC_Write has been replaced
 *              with PROC_write.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define PROC_Write                 PROC_write

/** ============================================================================
 *  @deprecated The deprecated API PROC_Start has been replaced
 *              with PROC_start.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define PROC_Start                 PROC_start

/** ============================================================================
 *  @deprecated The deprecated API PROC_Stop has been replaced
 *              with PROC_stop.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define PROC_Stop                  PROC_stop

/** ============================================================================
 *  @deprecated The deprecated API PROC_Control has been replaced
 *              with PROC_control.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define PROC_Control               PROC_control

#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @deprecated The deprecated API PROC_Debug has been replaced
 *              with PROC_debug.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define PROC_Debug                 PROC_debug
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (PROC_H) */
