/** ============================================================================
 *  @file   dpc.h
 *
 *  @path   $(DSPLINK)/gpp/src/osal/
 *
 *  @desc   Defines the interfaces and data structures for the
 *          sub-component DPC.
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


#if !defined (DPC_H)
#define DPC_H

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @name   DpcObject
 *
 *  @desc   Forward declaration for DpcObject. Actual definition is dependent
 *          on OS and is defined in the corresponding C file.
 *  ============================================================================
 */
typedef struct DpcObject_tag  DpcObject ;


/** ============================================================================
 *  @func   FnDpcProc
 *
 *  @desc   Function prototype for DPC function. The user defined functions
 *          that is to be invoked as a DPC should conform to this signature.
 *
 *  @arg    refData
 *             Argument to be passed to DPC call.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    DPC_Callback, DPC_Create
 *  ============================================================================
 */
typedef Void (*FnDpcProc) (Pvoid refData) ;


/** ============================================================================
 *  @func   DPC_Initialize
 *
 *  @desc   Initializes the DPC module. It initializes the global area for
 *          holding all the DPC objects.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *
 *  @enter  None
 *
 *  @leave  DPC must be initialized.
 *
 *  @see    DPC_Finalize
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DPC_Initialize (Void) ;


/** ============================================================================
 *  @func   DPC_Finalize
 *
 *  @desc   Releases all resources used by this sub-component.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *
 *  @enter  DPC must be initialized.
 *
 *  @leave  All in-use DPC objects are released.
 *
 *  @see    DPC_Initialize
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DPC_Finalize (Void) ;


/** ============================================================================
 *  @func   DPC_Create
 *
 *  @desc   Creates a DPC object and returns it after populating relevant
 *          fields.
 *
 *  @arg    userDPCFn
 *             User specified function to be invoked as DPC.
 *  @arg    dpcArgs
 *             Arguments to be passed to the DPC.
 *  @arg    dpcObj
 *             OUT argument to store the DPC object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ERESOURCE
 *              No more DPC objects are available for creation.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *
 *  @enter  DPC must be initialized.
 *          userDPCFn must be a valid function.
 *          dpcObj must be a valid pointer.
 *
 *  @leave  *dpcObj points to an initialized DPC Object on successful
 *          completion or *dpcObj is NULL on failure.
 *
 *  @see    DPC_Delete, DPC_Schedule
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DPC_Create (IN  FnDpcProc       userDPCFn,
            IN  Pvoid           dpcArgs,
            OUT DpcObject **    dpcObj) ;


/** ============================================================================
 *  @func   DPC_Delete
 *
 *  @desc   Deletes the DPC object.
 *
 *  @arg    dpcObj
 *             The DPC object to be deleted.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EPOINTER
 *              Invalid dpcObj object.
 *          DSP_EINVALIDARG
 *              Incorrect dpcObj specified.
 *
 *  @enter  DPC must be initialized.
 *          dpcObj must be a valid DPC object.
 *
 *  @leave  Upon successful completion the dpcObj is reset.
 *
 *  @see    DPC_Create
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DPC_Delete (IN DpcObject * dpcObj) ;


/** ============================================================================
 *  @func   DPC_Cancel
 *
 *  @desc   Cancels any pending DPCs associated to dpcObj.
 *
 *  @arg    dpcObj
 *             The DPC object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EPOINTER
 *              Invalid dpcObj object.
 *          DSP_EINVALIDARG
 *              Incorrect dpcObj specified.
 *
 *  @enter  DPC must be initialized.
 *          dpcObj must be a valid DPC object.
 *
 *  @leave  All pending calls to the DPC are cancelled.
 *
 *  @see    DPC_Schedule, DPC_Create
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DPC_Cancel (IN DpcObject * dpcObj) ;


/** ============================================================================
 *  @func   DPC_Schedule
 *
 *  @desc   Schedules the user defined function associated with dpcObj to be
 *          invoked at a later point of time.
 *
 *  @arg    dpcObj
 *             The DPC object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EPOINTER
 *              Invalid dpcObj object.
 *          DSP_EINVALIDARG
 *              Incorrect dpcObj specified.
 *
 *  @enter  DPC must be initialized.
 *          dpcObj must be a valid DPC object.
 *
 *  @leave  None
 *
 *  @see    DPC_Create, DPC_Cancel, DPC_Callback
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DPC_Schedule (IN DpcObject * dpcObj) ;


/** ============================================================================
 *  @func   DPC_Disable
 *
 *  @desc   Disables execution of DPCs.
 *
 *  @arg    None
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
EXPORT_API
Void
DPC_Disable (Void) ;


/** ============================================================================
 *  @func   DPC_Enable
 *
 *  @desc   Enables execution of DPCs.
 *
 *  @arg    None
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
EXPORT_API
Void
DPC_Enable (Void) ;


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   DPC_Debug
 *
 *  @desc   Prints the current status of DPC objects.
 *
 *  @arg    None
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *  ============================================================================
 */
EXPORT_API
Void
DPC_Debug (Void) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (DPC_H) */
