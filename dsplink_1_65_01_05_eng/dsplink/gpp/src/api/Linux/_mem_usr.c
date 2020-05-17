/** ============================================================================
 *  @file   _mem_usr.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/Linux/
 *
 *  @desc   Implementation of sub-component MEM for user-side.
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


/*  ----------------------------------- OS Specific Headers         */
#include <stdlib.h>

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- User API                    */
#include <_mem_usr.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Identifier for sub-component.
 *  ============================================================================
 */
#define COMPONENT_ID        ID_API_MEM

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_MEM, __LINE__)


/** ============================================================================
 *  @func   _MEM_USR_init
 *
 *  @desc   Initialize MEM USR sub-component by allocating all resources.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MEM_USR_init (Void)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_0ENTER ("_MEM_USR_init") ;

    /* Nothing to be done for initialization of MEM_USR component. */

    TRC_1LEAVE ("_MEM_USR_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _MEM_USR_exit
 *
 *  @desc   Releases resources used by the sub-component.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MEM_USR_exit ()
{
    DSP_STATUS status = DSP_SOK ;

    TRC_0ENTER ("_MEM_USR_exit") ;

    /* Nothing to be done for finalization of MEM_USR component. */

    TRC_1LEAVE ("_MEM_USR_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _MEM_USR_alloc
 *
 *  @desc   Allocates the specified number of bytes.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MEM_USR_alloc (OUT Void ** ptr, IN Uint32 bytes)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("_MEM_USR_alloc", ptr, bytes) ;

    DBC_Require (ptr   != NULL) ;
    DBC_Require (bytes != 0) ;

    if ((ptr == NULL) || (bytes == 0)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        *ptr = (Void *) malloc (bytes) ;
        if (*ptr == NULL) {
            status = DSP_EMEMORY ;
            SET_FAILURE_REASON ;
        }
        TRC_1PRINT (TRC_LEVEL4, "_MEM_USR_alloc *ptr [0x%x]\n", *ptr) ;
    }

    TRC_1LEAVE ("_MEM_USR_alloc", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _MEM_USR_calloc
 *
 *  @desc   Allocates the specified number of bytes and clears them by filling
 *          it with 0s.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MEM_USR_calloc (OUT Void ** ptr, IN Uint32 bytes)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("_MEM_USR_calloc", ptr, bytes) ;

    DBC_Require (ptr   != NULL) ;
    DBC_Require (bytes != 0) ;

    if ((ptr == NULL) || (bytes == 0)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        *ptr = (Void *) calloc (bytes, 1) ;
        if (*ptr == NULL) {
            status = DSP_EMEMORY ;
            SET_FAILURE_REASON ;
        }
        TRC_1PRINT (TRC_LEVEL4, "_MEM_USR_calloc *ptr [0x%x]\n", *ptr) ;
    }

    TRC_1LEAVE ("_MEM_USR_calloc", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _MEM_USR_free
 *
 *  @desc   Frees up the allocated chunk of memory.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MEM_USR_free (IN Void ** ptr)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("_MEM_USR_free", ptr) ;

    DBC_Require (ptr != 0) ;

    if (ptr == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        free (*ptr) ;
        *ptr = NULL ;
    }

    TRC_1LEAVE ("_MEM_USR_free", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
