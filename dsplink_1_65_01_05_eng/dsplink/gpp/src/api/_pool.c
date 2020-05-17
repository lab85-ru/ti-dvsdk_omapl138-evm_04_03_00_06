/** ============================================================================
 *  @file   _pool.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Implementation of internal functions for POOL component.
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


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- API                         */
#include <pooldefs.h>
#include <_pooldefs.h>
#include <_pool.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   COMPONENT_ID
 *
 *  @desc   Component ID of this module. That will be used by TRC macros.
 *  ============================================================================
 */
#define COMPONENT_ID    ID_API_IPOOL

/** ============================================================================
 *  @name   SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_IPOOL, __LINE__)


/** ============================================================================
 *  @func   POOL_addrConfig
 *
 *  @desc   Pool driver specific configuration datas.
 *
 *  @modif  object.
 *  ============================================================================
 */
POOL_AddrInfo POOL_addrConfig [MAX_DSPS][MAX_POOLENTRIES] ;


/** ============================================================================
 *  @func   _POOL_init
 *
 *  @desc   This function initializes the DRV POOL component.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
Void
_POOL_init (Void)
{
    Uint32 i ;
    Uint32 j ;

    TRC_0ENTER ("_POOL_init") ;

    for (i = 0 ; i < MAX_DSPS ; i++) {
        for (j = 0 ; j < MAX_POOLENTRIES ; j++) {
            POOL_addrConfig [i][j].isInit = FALSE ;
            POOL_addrConfig [i][j].addr [AddrType_Usr] = (Uint32) NULL ;
        }
    }

    TRC_0LEAVE ("_POOL_init") ;
}


/** ============================================================================
 *  @func   _POOL_exit
 *
 *  @desc   This function finalizes the DRV POOL component.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
Void
_POOL_exit (Void)
{
    Uint32 i ;
    Uint32 j ;

    TRC_0ENTER ("_POOL_exit") ;

    for (i = 0 ; i < MAX_DSPS ; i++) {
        for (j = 0 ; j < MAX_POOLENTRIES ; j++) {
            POOL_addrConfig [i][j].isInit = FALSE ;
            POOL_addrConfig [i][j].addr [AddrType_Usr] = (Uint32) NULL ;
        }
    }

    TRC_0LEAVE ("_POOL_exit") ;
}


/** ============================================================================
 *  @func   _POOL_xltBuf
 *
 *  @desc   This function translates the buffer address between two address
 *          spaces.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_POOL_xltBuf (IN PoolId poolId, OUT Pvoid * bufPtr, IN POOL_AddrXltFlag xltFlag)
{
    DSP_STATUS  status     = DSP_SOK ;
    Uint32      fmAddrBase = (Uint32) NULL ;
    Uint32      toAddrBase = (Uint32) NULL ;
    Uint8       poolNo = POOL_getPoolNo (poolId) ;
    ProcessorId procId = POOL_getProcId (poolId) ;
    Uint32      size ;
    Uint32      addr ;
    AddrType    toType ;
    AddrType    fmType ;

    TRC_3ENTER ("_POOL_xltBuf", poolId, bufPtr, xltFlag) ;

    DBC_Require (IS_VALID_POOLID (poolId)) ;
    DBC_Require (bufPtr != NULL) ;

    if (POOL_addrConfig [procId][poolNo].isInit != FALSE) {
        addr = (Uint32) (*bufPtr) ;
        size = POOL_addrConfig [procId][poolNo].size ;
        fmType = (AddrType) ((Uint32) xltFlag & 0xFF) ;
        toType = (AddrType) ((Uint32) xltFlag >> 8) ;

        fmAddrBase = POOL_addrConfig [procId][poolNo].addr [fmType] ;
        toAddrBase = POOL_addrConfig [procId][poolNo].addr [toType] ;

        /* Return NULL if the given address was not in expected range */
        if ((addr < fmAddrBase) || (addr >= (fmAddrBase + size))) {
            *bufPtr = NULL ;
            status = DSP_ERANGE ;
            SET_FAILURE_REASON ;
        }
        else {
            *bufPtr = (Pvoid) ((addr - fmAddrBase) + toAddrBase) ;
        }
    }
    else {
        *bufPtr = NULL ;
        size    = 0 ;
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    DBC_Ensure (    ((*bufPtr) == NULL)
                ||  (   ((Uint32) *bufPtr >= toAddrBase)
                     && ((Uint32) *bufPtr <= (toAddrBase + size)))) ;

    TRC_1LEAVE ("_POOL_xltBuf", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
