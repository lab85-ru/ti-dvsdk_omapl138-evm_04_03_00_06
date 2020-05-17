/** ============================================================================
 *  @file   _linkdefs.h
 *
 *  @path   $(DSPLINK)/gpp/inc/usr/
 *
 *  @desc   Definitions of constants and structures for Link.
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


#if !defined (_LINKDEFS_H)
#define _LINKDEFS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <gpptypes.h>
#include <errbase.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  DRV_SHMBASESYMBOL
 *
 *  @desc   Defines the symbol for the DSP-side location where the shared
 *          memory base address is to be written.
 *  ============================================================================
 */
#define DRV_SHMBASESYMBOL "_DSPLINK_shmBaseAddress"


/** ============================================================================
 *  @name   DRV_Handshake
 *
 *  @desc   Defines the types of handshake control actions.
 *
 *  @field  DRV_HandshakeSetup
 *              Setup the handshaking between the processors.
 *  @field  DRV_HandshakeStart
 *              Start the handshake process with the remote processor.
 *  @field  DRV_HandshakeCompl
 *              Complete the handshaking with the remote processor.
 *  ============================================================================
 */
typedef enum {
    DRV_HandshakeSetup = 0u,
    DRV_HandshakeStart = 1u,
    DRV_HandshakeCompl = 2u
} DRV_Handshake ;


/** ============================================================================
 *  @func   FnIpsInit
 *
 *  @desc   Signature of function that initializes an Inter Processor Signaling
 *          component.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *  @arg    shDspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the IPS component.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnIpsInit) (IN  ProcessorId dspId,
                                 IN  Uint32      ipsId,
                                 OUT Uint32 *    shDspAddr) ;


/** ============================================================================
 *  @func   FnIpsExit
 *
 *  @desc   Signature of function that finalizes an Inter Processor Signaling
 *          component.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnIpsExit) (IN ProcessorId dspId, IN Uint32 ipsId) ;


/** ============================================================================
 *  @func   FnIpsDebug
 *
 *  @desc   Signature of function that prints debug information about an Inter
 *          Processor Signaling component.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    ipsId
 *              IPS Identifier.
 *
 *  @ret    None.
 *  ============================================================================
 */
typedef Void (*FnIpsDebug) (IN ProcessorId dspId, IN Uint32 ipsId) ;

/** ============================================================================
 *  @func   FnLinkDrvInit
 *
 *  @desc   Signature of function that allocates and initializes resources
 *          used by the link driver.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    linkDrvId
 *              Link driver Identifier.
 *  @arg    dspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the link driver.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnLinkDrvInit) (IN  ProcessorId dspId,
                                     IN  Uint32      linkDrvId,
                                     OUT Uint32 *    dspAddr) ;

/** ============================================================================
 *  @func   FnLinkDrvExit
 *
 *  @desc   Signature of function that de-allocates and finalizes resources
 *          used by the link driver.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    linkDrvId
 *              Link driver Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnLinkDrvExit) (IN  ProcessorId dspId,
                                     IN  Uint32      linkDrvId) ;

/** ============================================================================
 *  @func   FnLinkDrvHandshake
 *
 *  @desc   Signature of function that setup, start, complete the handshake
 *          process toward specified DSP on the Link Driver.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    linkDrvId
 *              Link driver Identifier.
 *  @arg    hshkCtrl
 *              Handshake control action to be executed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnLinkDrvHandshake) (IN  ProcessorId   dspId,
                                          IN  Uint32        linkDrvId,
                                          IN  DRV_Handshake hshkCtrl) ;

#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @name   FnLinkDrvDebug
 *
 *  @desc   Signature of the function that prints debug information for the Link
 *          Driver.
 *
 *  @arg    dspId
 *              DSP Identifier.
 *  @arg    linkDrvId
 *              Link driver Identifier.
 *
 *  @ret    None.
 *  ============================================================================
 */
typedef Void (*FnLinkDrvDebug) (IN ProcessorId dspId, IN  Uint32 linkDrvId) ;
#endif /* defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   DRV_Interface
 *
 *  @desc   Interface functions exported by the Link Driver.
 *
 *  @field  init
 *              Function pointer to the Link Driver init function.
 *  @field  exit
 *              Function pointer to the Link Driver exit function.
 *  @field  handshake
 *              Function pointer to the Link Driver function to setup,start and
 *              complete handshake.
 *  @field  debug
 *              Function pointer to the Link Driver function for printing debug
 *              information
 *  ============================================================================
 */
typedef struct DRV_Interface_tag {
    FnLinkDrvInit          init ;
    FnLinkDrvExit          exit ;
    FnLinkDrvHandshake     handshake ;
#if defined (DDSP_DEBUG)
    FnLinkDrvDebug         debug ;
#endif /* if defined (DDSP_DEBUG) */
} DRV_Interface ;


#if defined (__cplusplus)
}
#endif


#endif /* !defined (_LINKDEFS_H) */
