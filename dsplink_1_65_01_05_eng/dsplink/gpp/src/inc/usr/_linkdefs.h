/** ============================================================================
 *  @file   _linkdefs.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Definitions of constants and structures for Link.
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
