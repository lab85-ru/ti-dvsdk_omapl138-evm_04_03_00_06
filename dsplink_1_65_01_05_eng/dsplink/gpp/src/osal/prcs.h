/** ============================================================================
 *  @file   prcs.h
 *
 *  @path   $(DSPLINK)/gpp/src/osal/
 *
 *  @desc   Defines the interfaces and data structures for the sub-component
 *          PRCS.
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


#if !defined (PRCS_H)
#define PRCS_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   PrcsObject
 *
 *  @desc   Forward declaration to a OS specific client identifier object.
 *  ============================================================================
 */
typedef struct PrcsObject_tag  PrcsObject ;


/** ============================================================================
 *  @func   PRCS_Initialize
 *
 *  @desc   Initializes the PRCS subcomponent.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General error from GPP-OS.
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
PRCS_Initialize (Void) ;


/** ============================================================================
 *  @func   PRCS_Finalize
 *
 *  @desc   Releases resources used by the PRCS subcomponent.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General error from GPP-OS.
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
PRCS_Finalize (Void) ;


/** ============================================================================
 *  @func   PRCS_Create
 *
 *  @desc   Creates a PrcsObject and populates it with information to identify
 *          the client.
 *
 *  @arg    prcsObj
 *              OUT argument to store the created object.
 *  @arg    optArgs
 *              Optional arguments
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *
 *  @enter  prcsObj must be a valid pointer.
 *
 *  @leave  Valid object is returned in case of success.
 *
 *  @see    PRCS_Delete
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PRCS_Create (OUT PrcsObject ** prcsObj, IN     Void * optArgs) ;


/** ============================================================================
 *  @func   PRCS_Delete
 *
 *  @desc   Frees up resources used by the specified object.
 *
 *  @arg    prcsObj
 *              Object to be deleted.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EPOINTER
 *              Invalid prcsObj
 *
 *  @enter  prcsObj must be a valid object.
 *
 *  @leave  None
 *
 *  @see    PRCS_Create
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PRCS_Delete (IN PrcsObject * prcsObj) ;


/** ============================================================================
 *  @func   PRCS_IsEqual
 *
 *  @desc   Compares two clients to check if they are "equal". Equality is
 *          defined by implementation on the specific OS port.
 *
 *  @arg    client1
 *              First client's information
 *  @arg    client2
 *              Second client's information
 *  @arg    isEqual
 *              Place holder for result of comparison
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *
 *  @enter  client1 must be a valid object.
 *          client2 must be a valid object.
 *          isEqual must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    PRCS_Create
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PRCS_IsEqual (IN  PrcsObject *  client1,
              IN  PrcsObject *  client2,
              OUT Bool *        isEqual) ;


/** ============================================================================
 *  @func   PRCS_IsSameContext
 *
 *  @desc   Checks if the two clients share same context (address space).
 *
 *  @arg    client1
 *              First client's information
 *  @arg    client2
 *              Second client's information
 *  @arg    isSame
 *              Place holder for result of comparison
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *
 *  @enter  client1 must be a valid object.
 *          client2 must be a valid object.
 *          isSame must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    PRCS_Create
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PRCS_IsSameContext (IN  PrcsObject *  client1,
                    IN  PrcsObject *  client2,
                    OUT Bool *        isSame) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (PRCS_H) */
